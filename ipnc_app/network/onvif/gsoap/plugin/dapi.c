/*
	dapi.c

	WS-Discovery 1.1 and 1.0 (D) plugin API

	See gsoap/doc/d for the D plugin user guide.

gSOAP XML Web services tools
Copyright (C) 2000-2011, Robert van Engelen, Genivia Inc., All Rights Reserved.
This part of the software is released under one of the following licenses:
GPL, the gSOAP public license, or Genivia's license for commercial use.
--------------------------------------------------------------------------------
gSOAP public license.

The contents of this file are subject to the gSOAP Public License Version 1.3
(the "License"); you may not use this file except in compliance with the
License. You may obtain a copy of the License at
http://www.cs.fsu.edu/~engelen/soaplicense.html
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Initial Developer of the Original Code is Robert A. van Engelen.
Copyright (C) 2000-2011, Robert van Engelen, Genivia Inc., All Rights Reserved.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org

This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

/**

@mainpage

- @ref d_0 documents the d library for WS-Discovery support.

*/

/**

@page d_0 The d library for WS-Discovery 1.1 support

@section d_1 WS-Discovery Setup

The material in this section relates to the WS-Discovery specification.

To use the d library:
-# Define WS-Discovery event handlers in your code, see Section @ref d_2.
-# Use the d API functions as described below.
-# (Re-)compile and link stdsoap2.c/pp or libgsoap, (dom.c/.cpp when needed),
   dapi.c, wsaapi.c and the soapcpp2-generated source files. Compile and link
   with plugin/threads.c when needed.

The material in this document pertains to the WS-Discovery protocol and model
and assumes that the reader is familiar with the WS-Discovery protocol, its
terms and definitions, and the WS-Discovery model. This document describes the
WS-Discovery interface to invoke and handle WS-Discovery events, while the
higher-level logic remains application-specific. Especially the mode of
operation, ad-hoc or managed with a Discovery Proxy, depends on the application
deployment and WS-Discovery support requirements.

The following assumptions are made. A Client is an endpoint that searches for
Target Service(s). A Target Service (TS) is and endpoint that makes itself
available for discovery. A Discovery Proxy (DP) is an endpoint that facilitates
discovery of Target Services by Clients. The interfaces defined in the d
library can be used to implement Client, Target Service, and Discovery Proxy
operations.

WS-Discovery ad-hoc and managed modes are supported by the d library. In an
ad-hoc mode discovery messages are sent multicast and response messages are
sent unicast.  In a managed mode discovery messages are sent unicast to a
Discovery Proxy.

@section d_2 WS-Discovery Event Handlers

The following event handlers MUST be defined by the user to handle inbound
WS-Discovery messages. The event handlers receive notifications (Hello, Bye,
ProbeMatches, and ResolveMatches) or receive requests to provide data (Probe
and Resolve).

The event handlers to define are:
- @ref d_event_Hello
- @ref d_event_Bye
- @ref d_event_Probe
- @ref d_event_ProbeMatches
- @ref d_event_Resolve
- @ref d_event_ResolveMatches

See the documentation provided with each of these functions in dapi.h.

@section d_3 WS-Discovery Event Listener

Inbound WS-Discovery multicast messages are handled via a listener on a port.
The user-defined event handlers are invoked when WS-Discovery messages arrive
on the port.

The @ref soap_d_listen function listens on the current port opened with
soap_bind for WS-Discovery messages for a brief time period as specified by a
timeout value in seconds (negative for micro seconds). The function allows for
periodically polling the port as shown:

@code
#include "dapi.h"
int port = 8080;
struct soap *soap = soap_new();
soap->user = (void*)&my_state;
if (!soap_valid_socket(soap_bind(soap, port, 100)))
{ soap_print_fault(soap, stderr);
  exit(0);
}

soap_d_listen(soap, -1000); // listen for messages for 1 ms

soap_d_listen(soap, -1000); // listen for messages for 1 ms
...
@endcode

WS-Discovery messages are relayed to the event handlers. The soap->user pointer
can be used to point to a state object that is updated by the event handlers as
per desired behavior at the Client side, the Target Service, or the Discovery
Proxy implementation.

@section d_4 Invoking WS-Discovery Operations

A Client may invoke the following WS-Discovery operations:
- @ref soap_d_Probe
- @ref soap_d_Resolve

A Target Service may invoke the following WS-Discovery
operations:
- @ref soap_d_Hello
- @ref soap_d_Bye
- @ref soap_d_ProbeMatches (automatic via @ref soap_d_listen)
- @ref soap_d_ResolveMatches (automatic via @ref soap_d_listen)

A Discovery Proxy can perform all operations listed above, and should use
"d:DiscoveryProxy" as the Type with the Hello, Bye, and ProbeMatches.

To send a Hello message to join a network:

@code
soap_d_Hello(soap,
  SOAP_D_MANAGED,    // or SOAP_D_ADHOC for ad-hoc mode
  "to address",         // "http(s):" URL, or "soap.udp:" UDP, or TCP/IP address
  soap_wsa_rand_uuid(soap), // a unique message ID
  NULL,
  "my address",         // where they can find me for WS-Discovery
  "d:DiscoveryProxy",// Types: I'm a DP
  NULL,                 // Scope
  NULL,                 // MatchBy
  NULL,                 // XAddrs
  75965);               // MDVersion
@endcode

Note that Types is a string with namespace-qualified names (QNames). These
should be qualified as in "namespace":name or you can use a namespace prefix
that is part of your namespace table (in the .nsmap). So you can use
"d:DiscoveryPRoxy" as a QName in Types because d is a namespace prefix
with a defined binding in the namespace table.

For UDP multicast, use

@code
soap.connect_flags = SO_BROADCAST;
@endcode

and optionally set the interface and TTL settings:

@code
in_addr_t addr = inet_addr("1.2.3.4");
soap.ipv4_multicast_if = &addr; // see setsockopt IPPROTO_IP IP_MULTICAST_IF
soap.ipv6_multicast_if = addr; // multicast sin6_scope_id
soap.ipv4_multicast_ttl = 1; // see setsockopt IPPROTO_IP, IP_MULTICAST_TTL
@endcode

Please refer to the socket options for IPPROTO_IP IP_MULTICAST_IF to specify
the default interface for multicast datagrams to be sent from. Otherwise,
the default interface set by the system administrator will be used (if any).

Please refer to the socket options for IPPROTO_IP IP_MULTICAST_TTL to limit
the lifetime of the packet. Multicast datagrams are sent with a default value
of 1, to prevent them to be forwarded beyond the local network. This parameter
can be set between 1 to 255.

To send a Bye message by to leave a network:

@code
soap_d_Bye(soap,
  SOAP_D_MANAGED,    // or SOAP_D_ADHOC for ad-hoc mode
  "to address",         // "http(s):" URL, or "soap.udp:" UDP, or TCP/IP address
  soap_wsa_rand_uuid(soap), // a unique message ID
  NULL,
  "my address",         // where they can find me for WS-Discovery
  "d:DiscoveryProxy",// Types: I'm a DP
  NULL,                 // Scope
  NULL,                 // MatchBy
  NULL,                 // XAddrs
  75965);               // MDVersion
@endcode

To send a Probe message (see WS-Discovery 1.1 Section 1.7) and then listen to
ProbeMatches:

@code
struct soap soap = soap_new(); // to invoke messages
struct soap serv = soap_new(); // for the listener and event handlers

soap_bind(serv, port, 100);

const char *id = soap_wsa_rand_uuid(soap);
serv->user = (void*)&my_state;
my_state.probe_id = id;

soap_d_Probe(soap,
  SOAP_D_ADHOC,      // ad-hoc mode
  SOAP_D_TO_TS,      // to a TS
  "to address",         // address of TS
  id,                   // message ID
  NULL,                 // ReplyTo
  "\"http://printer.example.org/2003/imaging\":PrintBasic",
  "ldap:///ou=engineering,o=examplecom,c=us",
  "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/ldap");

soap_d_listen(serv, -1000);
@endcode

The id is the WS-Addressing message ID that will be included in the
ProbeMatches RelatesTo WS-Addressing header. As an example, my_state is set to
this id so that when the @ref d_event_ProbeMatches event handler is invoked
it can find the id in the current state that is pointed to by serv->user
(soap->user in the handler).

To send a Resolve message and then listen to ResolveMatches:

@code
struct soap soap = soap_new(); // to invoke messages
struct soap serv = soap_new(); // for the listener and event handlers

soap_bind(serv, port, 100);

const char *id = soap_wsa_rand_uuid(soap);
serv->user = (void*)&my_state;
my_state.resolve_id = id;

soap_d_Resolve(soap,
  SOAP_D_ADHOC,      // ad-hoc mode
  SOAP_D_TO_TS,      // to a TS
  "to address",         // address to send to
  id,                   // message ID
  NULL,                 // ReplyTo
  "endpoint");          // EndpointReference of TS
 
soap_d_listen(serv, -1000);
@endcode

Again, the id and state are used to associate the asynchronously received
ResolveMatches response that is handled by the @ref d_event_ResolveMatches
for the original request.

In managed mode with unicast messages (request-response messages), the @ref
soap_d_Probe and @ref soap_d_Resolve are sufficient to invoke without
setting up a listener. The event handlers are invoked when the unicast response
message arrives.

In managed mode, the ProbeMatches and ResolveMatches are automatically sent via
@ref soap_d_listen and the event @ref d_event_Probe and @ref
d_event_Resolve handlers. These event handlers should set the matches to be
returned.

In ad-hoc mode, ProbeMatches or ResolveMatches responses are NOT sent
automatically. In ad-hoc mode the responses can be returned by adding code to
the event handler or from anywhere in the main program, for example after @ref
soap_d_listen. When responses are to be returned from the event handler or
from the main program, you should invoke @ref soap_d_ProbeMatches and @ref
soap_d_ResolveMatches to explicitly send unicast messages with the match(es)
back to the clients. The WS-Addressing ReplyTo address can be used as the
return address (when not anonymous), or by using the peer's host information
that is accessible in the soap->peer and soap->peerlen members. For example:

@code
char host[1024], port[16];
getnameinfo((struct sockaddr*)&soap->peer, soap->peerlen, host, sizeof(host), port, 16, NI_DGRAM | NI_NAMEREQD | NI_NUMERICSERV);
@endcode

@section d_5 Generating C++ Server Objects

The D library is developed to support C and C++. To support C++ server
objects generated with soapcpp2 options -i and -j, you need to define in your
C++ code the following wrappers (use this->soap below for soapcpp2 option -j):

@code
int dService::Hello(struct d__HelloType *hello)
{ return __d__Hello(this, hello);
}
int dService::Bye(struct d__ByeType *bye
{ return __d__Bye(this, bye);
}
int dService::Probe(struct d__ProbeType *probe)
{ return __d__Probe(this, probe);
}
int dService::ProbeMatches(struct d__ProbeMatchesType *matches)
{ return __d__ProbeMatches(this, matches);
}
int dService::Resolve(struct d__ResolveType *resolve)
{ return __d__Resolve(this, resolve);
}
int dService::ResolveProbeMatches(struct d__ResolveMatchesType *matches)
{ return __d__ResolveMatches(this, matches);
}
@endcode

Another approach to generate the D service operations is to run soapcpp2
separately on d.h (or d10.h for WS-Discovery 1.0) by soapcpp2 -a -L
-pd d.h to generate dService.cpp. Then chain the service operations at
the server side:

@code
if (soap_begin_serve(service.soap) == SOAP_OK)
  if (service.dispatch() == SOAP_NO_METHOD)
    soap_serve_request(service.soap);
@endcode

where the 'service' object is an instance of the application services generated
by soapcpp2 -j.

@section d_6 Miscellaneous

You MUST generate client-side operations that the D library expects to be
linked with, by executing:

@code
> soapcpp2 -L -pd -Iimport import/d.h
@endcode

Then compile and link the generated dClient.cpp code with your project.

Because WS-Addressing may relay faults to a FaultTo service, you need to
define a SOAP Fault service operation to accept and handle these:

@code
int SOAP_ENV__Fault(struct soap *soap, char *faultcode, char *faultstring, char *faultactor, struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *SOAP_ENV__Code, struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node, char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail)
{ 
  ... = faultcode; // SOAP 1.1 fault code string (QName)
  ... = faultstring; // SOAP 1.1 fault string
  ... = faultactor; // SOAP 1.1 fault actor string
  ... = detail; // SOAP 1.1 fault detail struct
  ... = SOAP_ENV__Code; // SOAP 1.2 fault code struct
  ... = SOAP_ENV__Reason; // SOAP 1.2 reason struct
  ... = SOAP_ENV__Node; // SOAP 1.2 node string
  ... = SOAP_ENV__Role; // SOAP 1.2 role string
  ... = SOAP_ENV__Detail; // SOAP 1.2 detail struct
  return SOAP_OK;
}
@endcode

*/

#include "dapi.h"

#ifdef SOAP_WSA_2005
/* WS-Discovery 1.0 */
const char *to_ts_URL = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
#else
/* WS-Discovery 1.1 */
const char *to_ts_URL = "urn:docs-oasis-open-org:ws-dd:ns:discovery:2009:01";
#endif

/******************************************************************************\
 *
 *	WS-Discovery AppSequence State
 *
\******************************************************************************/

static MUTEX_TYPE soap_d_state = MUTEX_INITIALIZER;

static unsigned int soap_d_InstanceId = 0;
static const char *soap_d_SequenceId = NULL;
static unsigned int soap_d_MessageNumber = 1;

static int soap_d_serve___d__Hello(struct soap *soap);
static int soap_d_serve___d__Bye(struct soap *soap);
static int soap_d_serve___d__Probe(struct soap *soap);
static int soap_d_serve___d__ProbeMatches(struct soap *soap);
static int soap_d_serve___d__Resolve(struct soap *soap);
static int soap_d_serve___d__ResolveMatches(struct soap *soap);

static void soap_d_set_AppSequence(struct soap*);
static void soap_d_reset_AppSequence(struct soap*);
static void soap_d_delay(struct soap*);

/******************************************************************************\
 *
 *	WS-Discovery Outbound Messages
 *
\******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
@fn int soap_d_Hello(struct soap *soap, soap_d_mode mode, const char *endpoint, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
@brief TS or DP Hello message to join a network.
@param soap context
@param[in] mode SOAP_D_MANAGED or SOAP_D_ADHOC
@param[in] endpoint to send Hello message to (unicast or multicast)
@param[in] MessageID unique WS-Addressing message ID (@ref soap_wsa_rand_uuid)
@param[in] RelatesTo WS-Addressing message ID
@param[in] EndpointReference of this Target Server or Discovery Proxy
@param[in] Types an unordered string of QNames or NULL, a Discovery Proxy MUST include "d:DiscoveryProxy"
@param[in] Scopes an unordered set of scopes or NULL
@param[in] MatchBy NULL (unused, reserved)
@param[in] XAddrs contains the transport address(es) that MAY be used to communicate with the Target Service or Discovery Proxy
@param[in] MetadataVersion incremented by a positive value (>= 1) whenever there is a change in the metadata of the Target Service
@return SOAP_OK or error code

Hello is a one-way message sent by a Target Service to announce its
availability when it joins the network. It is also sent by a Discovery Proxy to
reduce multicast traffic on an ad hoc network.
*/
int
soap_d_Hello(struct soap *soap, soap_d_mode mode, const char *endpoint, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
  struct d__HelloType req;
  struct d__ScopesType scopes;
  const char *Action = SOAP_NAMESPACE_OF_d"/Hello";
  const char *To = endpoint;

  /* SOAP Header */
  if (mode == SOAP_D_ADHOC)
    To = to_ts_URL;
  soap_wsa_request(soap, MessageID, To, Action);
  soap_wsa_add_RelatesTo(soap, RelatesTo);
#ifdef SOAP_WSA_2005
  soap_d_set_AppSequence(soap);
#else
  if (mode == SOAP_D_ADHOC)
    soap_d_set_AppSequence(soap);
  else
    soap_d_reset_AppSequence(soap);
#endif

  /* Hello */
  soap_default_d__HelloType(soap, &req);
#ifdef SOAP_WSA_2005
  req.wsa5__EndpointReference.Address = (char*)EndpointReference;
#else
  req.wsa__EndpointReference.Address = (char*)EndpointReference;
#endif
  req.Types = (char*)Types;
  if (Scopes)
  {
    soap_default_d__ScopesType(soap, &scopes);
    scopes.__item = (char*)Scopes;
    scopes.MatchBy = (char*)MatchBy;
    req.Scopes = &scopes;
  }
  req.XAddrs = (char*)XAddrs;
  req.MetadataVersion = MetadataVersion;
  if (soap_send___d__Hello(soap, endpoint, Action, &req))
    return soap->error;

  /* HTTP(S) POST expects an HTTP OK response */
  if (endpoint && !strncmp(endpoint, "http", 4))
    return soap_recv_empty_response(soap);
  return SOAP_OK;
}

/**
@fn int soap_d_Bye(struct soap *soap, soap_d_mode mode, const char *endpoint, const char *MessageID, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
@brief TS or DP Bye message to leave a network.
@param soap context
@param[in] mode SOAP_D_MANAGED or SOAP_D_ADHOC
@param[in] endpoint to send Bye message to (unicast or multicast)
@param[in] MessageID unique WS-Addressing message ID (@ref soap_wsa_rand_uuid)
@param[in] EndpointReference of this Target Server or Discovery Proxy
@param[in] Types an unordered string of QNames or NULL, a Discovery Proxy MUST include "d:DiscoveryProxy"
@param[in] Scopes an unordered set of scopes or NULL
@param[in] MatchBy NULL (unused, reserved)
@param[in] XAddrs contains the transport address(es) that MAY be used to communicate with the Target Service or Discovery Proxy
@param[in] MetadataVersion incremented by a positive value (>= 1) whenever there is a change in the metadata of the Target Service
@return SOAP_OK or error code

Bye is a one-way message sent by a Target Service to announce its
unavailability as a best effort when it leaves the network.
*/
int
soap_d_Bye(struct soap *soap, soap_d_mode mode, const char *endpoint, const char *MessageID, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
  struct d__ByeType req;
  struct d__ScopesType scopes;
  const char *Action = SOAP_NAMESPACE_OF_d"/Bye";
  const char *To = endpoint;

  /* SOAP Header */
  if (mode == SOAP_D_ADHOC)
    To = to_ts_URL;
  soap_wsa_request(soap, MessageID, To, Action);
#ifdef SOAP_WSA_2005
  soap_d_set_AppSequence(soap);
#else
  if (mode == SOAP_D_ADHOC)
    soap_d_set_AppSequence(soap);
  else
    soap_d_reset_AppSequence(soap);
#endif

  /* Bye */
  soap_default_d__ByeType(soap, &req);
#ifdef SOAP_WSA_2005
  req.wsa5__EndpointReference.Address = (char*)EndpointReference;
#else
  req.wsa__EndpointReference.Address = (char*)EndpointReference;
#endif
  req.Types = (char*)Types;
  if (Scopes)
  {
    soap_default_d__ScopesType(soap, &scopes);
    scopes.__item = (char*)Scopes;
    scopes.MatchBy = (char*)MatchBy;
    req.Scopes = &scopes;
  }
  req.XAddrs = (char*)XAddrs;
  req.MetadataVersion = &MetadataVersion;
  if (soap_send___d__Bye(soap, endpoint, Action, &req))
    return soap->error;

  /* HTTP(S) POST expects an HTTP OK response */
  if (endpoint && !strncmp(endpoint, "http", 4))
    return soap_recv_empty_response(soap);
  return SOAP_OK;
}

/**
@fn int soap_d_Probe(struct soap *soap, soap_d_mode mode, soap_d_to to, const char *endpoint, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy)
@brief Client Probe message to a TS or DP.
@param soap context
@param[in] mode SOAP_D_MANAGED or SOAP_D_ADHOC
@param[in] to SOAP_D_TO_TS or SOAP_D_TO_DP
@param[in] endpoint to send Probe to (unicast or multicast)
@param[in] MessageID WS-Addressing message ID of the message
@param[in] ReplyTo WS-Addressing ReplyTo message ID of the message
@param[in] Types an unordered string of QNames to probe
@param[in] Scopes an unordered set of scopes to probe
@param[in] MatchBy matching rule to apply for this probe
@return SOAP_OK or error code

A Client sends a probe to find Target Services by the Type of the Target
Service, a Scope in which the Target Service resides, both, or simply all
Target Services. The matches will be delivered to @ref d_event_ProbeMatches
when @ref soap_d_listen receives a ProbeMatch response. The RelatesTo of the
ProbeMatches is the MessageID of the Probe.
*/
int
soap_d_Probe(struct soap *soap, soap_d_mode mode, soap_d_to to, const char *endpoint, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy)
{
  unsigned int InstanceId = 0;
  const char *SequenceId = NULL;
  unsigned int MessageNumber = 0;
  struct d__ProbeType req;
  struct __d__ProbeMatches res;
  struct d__ScopesType scopes;
  const char *Action = SOAP_NAMESPACE_OF_d"/Probe";
  const char *To = endpoint;

	fprintf(stderr, "in  [onvif] soap_d_Probe .......... \n");
  /* SOAP Header */
  if (to == SOAP_D_TO_TS)
    To = to_ts_URL;
  soap_wsa_request(soap, MessageID, To, Action);
  soap_wsa_add_ReplyTo(soap, ReplyTo);
  soap_d_reset_AppSequence(soap);

  /* Probe */
  soap_default_d__ProbeType(soap, &req);
  req.Types = (char*)Types;
  if (Scopes)
  {
    soap_default_d__ScopesType(soap, &scopes);
    scopes.__item = (char*)Scopes;
    scopes.MatchBy = (char*)MatchBy;
    req.Scopes = &scopes;
  }
  if (soap_send___d__Probe(soap, endpoint, Action, &req))
    return soap->error;

  /* ad-hoc mode: we're done */
  if (mode == SOAP_D_ADHOC)
  {
    /* HTTP(S) POST expects an HTTP OK response */
    if (endpoint && !strncmp(endpoint, "http", 4))
      return soap_recv_empty_response(soap);
  }
  else
  { 
    /* managed mode: receive the matches */
    if (soap_recv___d__ProbeMatches(soap, &res))
      return soap->error;

    /* check Header D */
    if (!soap->header)
      return soap_wsa_sender_fault(soap, "D ProbeMatches incomplete", NULL);

    if (soap->header->d__AppSequence)
    {
      d__AppSequenceType *seq = soap->header->d__AppSequence;
      InstanceId = seq->InstanceId;
      SequenceId = seq->SequenceId;
      MessageNumber = seq->MessageNumber;
      DBGLOG(TEST, SOAP_MESSAGE(fdebug, "AppSeq id=%u seq=%s msg=%u\n", InstanceId, SequenceId ? SequenceId : "(null)", MessageNumber));
    }

    /* pass probe matches on to user-defined event handler */
    d_event_ProbeMatches(soap,
      InstanceId,
      SequenceId,
      MessageNumber, 
#ifdef SOAP_WSA_2005
      soap->header->wsa5__MessageID,
      soap->header->wsa5__RelatesTo ? soap->header->wsa5__RelatesTo->__item : NULL,
#else
      soap->header->wsa__MessageID,
      soap->header->wsa__RelatesTo ? soap->header->wsa__RelatesTo->__item : NULL,
#endif
      res.d__ProbeMatches);
  }
  return SOAP_OK;
}

/**
@fn int soap_d_Resolve(struct soap *soap, soap_d_mode mode, soap_d_to to, const char *endpoint, const char *MessageID, const char *ReplyTo, const char *EndpointReference)
@brief Client Resolve message to a TS or DP.
@param soap context
@param[in] mode SOAP_D_MANAGED or SOAP_D_ADHOC
@param[in] to SOAP_D_TO_TS or SOAP_D_TO_DP
@param[in] endpoint to send Resolve to (unicast or multicast)
@param[in] MessageID WS-Addressing message ID of the message
@param[in] ReplyTo WS-Addressing ReplyTo message ID of the message
@param[in] EndpointReference of the Target Service or Discovery Proxy
@return SOAP_OK or error code

A Client sends a resolve to locate a Target Service, i.e., to retrieve its
transport address(es). This server-side event handler returns the match(es).
The matches will be delivered to @ref d_event_ResolveMatches when @ref
soap_d_listen receives a ResolveMatch response. The RelatesTo of the
ResolveMatches is the MessageID of the Resolve.
*/
int
soap_d_Resolve(struct soap *soap, soap_d_mode mode, soap_d_to to, const char *endpoint, const char *MessageID, const char *ReplyTo, const char *EndpointReference)
{
  unsigned int InstanceId = 0;
  const char *SequenceId = NULL;
  unsigned int MessageNumber = 0;
  struct d__ResolveType req;
  struct __d__ResolveMatches res;
  const char *Action = SOAP_NAMESPACE_OF_d"/Resolve";
  const char *To = endpoint;

  /* SOAP Header */
  if (to == SOAP_D_TO_TS)
    To = to_ts_URL;
  soap_wsa_request(soap, MessageID, To, Action);
  soap_wsa_add_ReplyTo(soap, ReplyTo);
  soap_d_reset_AppSequence(soap);

  /* Resolve */
  soap_default_d__ResolveType(soap, &req);
#ifdef SOAP_WSA_2005
  req.wsa5__EndpointReference.Address = (char*)EndpointReference;
#else
  req.wsa__EndpointReference.Address = (char*)EndpointReference;
#endif
  if (soap_send___d__Resolve(soap, endpoint, Action, &req))
    return soap->error;

  /* ad-hoc mode: we're done */
  if (mode == SOAP_D_ADHOC)
  {
    /* HTTP(S) POST expects an HTTP OK response */
    if (endpoint && !strncmp(endpoint, "http", 4))
      return soap_recv_empty_response(soap);
  }
  else
  { 
    /* managed mode: receive the matches */
    if (soap_recv___d__ResolveMatches(soap, &res))
      return soap->error;

    /* check Header D */
    if (!soap->header)
      return soap_wsa_sender_fault(soap, "D ProbeMatches incomplete", NULL);

    if (soap->header->d__AppSequence)
    {
      d__AppSequenceType *seq = soap->header->d__AppSequence;
      InstanceId = seq->InstanceId;
      SequenceId = seq->SequenceId;
      MessageNumber = seq->MessageNumber;
      DBGLOG(TEST, SOAP_MESSAGE(fdebug, "AppSeq id=%u seq=%s msg=%u\n", InstanceId, SequenceId ? SequenceId : "(null)", MessageNumber));
    }

    /* pass probe matches on to user-defined event handler */
    if (res.d__ResolveMatches && res.d__ResolveMatches->ResolveMatch)
      d_event_ResolveMatches(soap,
        InstanceId,
        SequenceId,
        MessageNumber, 
#ifdef SOAP_WSA_2005
        soap->header->wsa5__MessageID,
        soap->header->wsa5__RelatesTo ? soap->header->wsa5__RelatesTo->__item : NULL,
#else
        soap->header->wsa__MessageID,
        soap->header->wsa__RelatesTo ? soap->header->wsa__RelatesTo->__item : NULL,
#endif
        res.d__ResolveMatches->ResolveMatch);
  }
  return SOAP_OK;
}

/**
@fn void soap_d_init_ProbeMatches(struct soap *soap, struct d__ProbeMatchesType *matches)
@brief Initalize the probe matches container.
@param soap context
@param matches the container with matches to initialize
*/
void
soap_d_init_ProbeMatches(struct soap *soap, struct d__ProbeMatchesType *matches)
{ soap_default_d__ProbeMatchesType(soap, matches);
}

/**
@fn int soap_d_add_ProbeMatch(struct soap *soap, struct d__ProbeMatchesType *matches, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
@brief Add a match to the probe matches container.
@param soap context
@param matches the container to update with a new match
@param[in] EndpointReference of this Target Server or Discovery Proxy
@param[in] Types an unordered string of QNames or NULL, a Discovery Proxy MUST include "d:DiscoveryProxy"
@param[in] Scopes an unordered set of scopes or NULL
@param[in] MatchBy NULL (unused, reserved)
@param[in] XAddrs contains the transport address(es) that MAY be used to communicate with the Target Service or Discovery Proxy
@param[in] MetadataVersion incremented by a positive value (>= 1) whenever there is a change in the metadata of the Target Service
@return SOAP_OK or error code

To populate a Prove matches container, first initialize with @ref
soap_d_init_ProbeMatches, then use this function to add each match. The
container is deallocated with soap_end(soap) and can be initialized again
(without leaks).
*/
int
soap_d_add_ProbeMatch(struct soap *soap, struct d__ProbeMatchesType *matches, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
  int n = 0, k = matches->__sizeProbeMatch;
  struct d__ProbeMatchType *m;

  /* need to increase space? */
  if (k == 0)
    n = 4;
  else if (k >= 4 && (k & (k - 1)) == 0)
    n = 2 * k;
  /* yes we do */
  if (n)
  {
    struct d__ProbeMatchType *tmp = (struct d__ProbeMatchType*)soap_malloc(soap, n * sizeof(struct d__ProbeMatchType));
    int i;

    if (!tmp)
      return soap->error = SOAP_EOM;
    for (i = 0; i < k; i++)
      tmp[i] = matches->ProbeMatch[i];
    matches->ProbeMatch = tmp;
  }

  /* add entry */
  m = &matches->ProbeMatch[k];
  soap_default_d__ProbeMatchType(soap, m);
#ifdef SOAP_WSA_2005
  m->wsa5__EndpointReference.Address = (char*)EndpointReference;
#else
  m->wsa__EndpointReference.Address = (char*)EndpointReference;
#endif
  m->Types = (char*)Types;
  if (Scopes)
  {
    m->Scopes = (struct d__ScopesType*)soap_malloc(soap, sizeof(struct d__ScopesType));
    soap_default_d__ScopesType(soap, m->Scopes);
    m->Scopes->__item = (char*)Scopes;
    m->Scopes->MatchBy = (char*)MatchBy;
  }
  m->XAddrs = (char*)XAddrs;
  m->MetadataVersion = MetadataVersion;
  matches->__sizeProbeMatch++;
  return SOAP_OK;
}

/**
@fn int soap_d_ProbeMatches(struct soap *soap, const char *endpoint, const char *MessageID, const char *RelatesTo, const char *To, struct d__ProbeMatchesType *matches)
@brief TS or DP ProbeMatches message.
@param soap context
@param[in] endpoint to send Probe to (unicast or multicast)
@param[in] MessageID WS-Addressing message ID of the message
@param[in] RelatesTo WS-Addressing RelatesTo message ID of the message
@param[in] To WS-Addressing endpoint IP and port or NULL for anonymous
@param[in] matches contains the probe matches, use @ref soap_d_add_ProbeMatch to populate this container
@return SOAP_OK or error code

A Client sends a probe to find Target Services by the Type of the Target
Service, a Scope in which the Target Service resides, both, or simply all
Target Services. The Target Server(s) or Discovery Proxy responds by sending
ProbeMatches to the Client.
*/
int
soap_d_ProbeMatches(struct soap *soap, const char *endpoint, const char *MessageID, const char *RelatesTo, const char *To, struct d__ProbeMatchesType *matches)
{
  const char *Action = SOAP_NAMESPACE_OF_d"/ProbeMatches";

	 fprintf(stderr, "in  [onvif] soap_d_ProbeMatches .......... \n");

  /* 0..APP_MAX_DELAY ms delay */
  soap_d_delay(soap);

  /* SOAP Header */
  soap_wsa_request(soap, MessageID, To, Action);
  soap_wsa_add_RelatesTo(soap, RelatesTo);
  soap_d_set_AppSequence(soap);

  /* ProbeMatches */
  if (soap_send___d__ProbeMatches(soap, endpoint, Action, matches))
    return soap->error;

  /* HTTP(S) POST expects an HTTP OK response */
  if (endpoint && !strncmp(endpoint, "http", 4))
    return soap_recv_empty_response(soap);
  return SOAP_OK;
}

/**
@fn int soap_d_ResolveMatches(struct soap *soap, const char *endpoint, const char *MessageID, const char *RelatesTo, const char *To, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
@brief TS or DP ResolveMatches message.
@param soap context
@param[in] endpoint to send Probe to (unicast or multicast)
@param[in] MessageID WS-Addressing message ID of the message
@param[in] RelatesTo WS-Addressing RelatesTo message ID of the message
@param[in] To WS-Addressing endpoint IP and port or NULL for anonymous
@param[in] EndpointReference of the Target Service
@param[in] Types an unordered string of QNames or NULL, a Discovery Proxy MUST include "d:DiscoveryProxy"
@param[in] Scopes an unordered set of scopes or NULL
@param[in] MatchBy NULL (unused, reserved)
@param[in] XAddrs contains the transport address(es) that MAY be used to communicate with the Target Service or Discovery Proxy
@param[in] MetadataVersion incremented by a positive value (>= 1) whenever there is a change in the metadata of the Target Service
@return SOAP_OK or error code

A Client sends a resolve to locate a Target Service, i.e., to retrieve its
transport address(es). The Target Server(s) or Discovery Proxy responds by
sending ResolveMatches to the Client.
*/
int
soap_d_ResolveMatches(struct soap *soap, const char *endpoint, const char *MessageID, const char *RelatesTo, const char *To, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
  struct d__ResolveMatchesType res;
  struct d__ResolveMatchType match;
  struct d__ScopesType scopes;
  const char *Action = SOAP_NAMESPACE_OF_d"/ResolveMatches";

  /* 0..APP_MAX_DELAY ms delay */
  soap_d_delay(soap);

  /* SOAP Header */
  soap_wsa_request(soap, MessageID, To, Action);
  soap_wsa_add_RelatesTo(soap, RelatesTo);
  soap_d_set_AppSequence(soap);

  /* ResolveMatches */
  soap_default_d__ResolveMatchType(soap, &match);
#ifdef SOAP_WSA_2005
  match.wsa5__EndpointReference.Address = (char*)EndpointReference;
#else
  match.wsa__EndpointReference.Address = (char*)EndpointReference;
#endif
  match.Types = (char*)Types;
  if (Scopes)
  {
    soap_default_d__ScopesType(soap, &scopes);
    scopes.__item = (char*)Scopes;
    scopes.MatchBy = (char*)MatchBy;
    match.Scopes = &scopes;
  }
  match.XAddrs = (char*)XAddrs;
  match.MetadataVersion = MetadataVersion;
  soap_default_d__ResolveMatchesType(soap, &res);
  res.ResolveMatch = &match;
  if (soap_send___d__ResolveMatches(soap, endpoint, Action, &res))
    return soap->error;

  /* HTTP(S) POST expects an HTTP OK response */
  if (endpoint && !strncmp(endpoint, "http", 4))
    return soap_recv_empty_response(soap);
  return SOAP_OK;
}

/******************************************************************************\
 *
 *	WS-Discovery Serving Inbound Messages
 *
\******************************************************************************/

/**
@fn int soap_d_listen(struct soap *soap, int timeout)
@brief Listen on a port for inbound WS-Discovery messages.
@param soap context
@param[in] timeout seconds to listen (use negative value for micro seconds)
@return SOAP_OK or error code

First you need to open a port with soap_bind(soap). Then invoke this function
to listen to WS-Discovery messages on that port. Inbound messages are accepted
and the information is relayed to the event handlers, see Section @ref d_2.
Use soap->user to pass a pointer to a state object that the event hanlders can
use. The timeout allows a Client to periodically poll the port for messages. A
value of zero will loop the listener forever or until an error occurs.
*/
int
soap_d_listen(struct soap *soap, int timeout)
{
  soap->accept_timeout = soap->recv_timeout = soap->send_timeout = timeout;

  /* event-serve loop (exits upon timeout) */
  for (;;)
  {
    if (!soap_valid_socket(soap_accept(soap)))
    {
      /* timeout? */
      if (!soap->errnum)
        return soap->error = SOAP_OK;
      return soap->error;
    }

	fprintf(stderr, "in  [onvif] soap_d_listen .......... \n");
    if (soap_begin_serve(soap))
      return soap->error;

    /* always close HTTP afterwards */
    soap->keep_alive = 0;

    /* receive event and serve */
    if (soap_d_serve_request(soap))
    {
      /* timeout? */
      if (!soap->errnum)
        return soap->error = SOAP_OK;
#ifdef WITH_FASTCGI
      soap_send_fault(soap);
#else
      return soap_send_fault(soap);
#endif
    }
    /* clean up for next iteration */
    soap_destroy(soap);
    soap_end(soap);
  } 
}

int
soap_d_serve_request(struct soap *soap)
{
  soap_peek_element(soap);
  if (!soap_match_tag(soap, soap->tag, "d:Hello"))
    return soap_d_serve___d__Hello(soap);
  if (!soap_match_tag(soap, soap->tag, "d:Bye"))
    return soap_d_serve___d__Bye(soap);
  if (!soap_match_tag(soap, soap->tag, "d:Probe"))
    return soap_d_serve___d__Probe(soap);
  if (!soap_match_tag(soap, soap->tag, "d:ProbeMatches"))
    return soap_d_serve___d__ProbeMatches(soap);
  if (!soap_match_tag(soap, soap->tag, "d:Resolve"))
    return soap_d_serve___d__Resolve(soap);
  if (!soap_match_tag(soap, soap->tag, "d:ResolveMatches"))
    return soap_d_serve___d__ResolveMatches(soap);
  soap->error = SOAP_NO_METHOD;
  /* close socket unless soap->keep_alive == 1 */
  return soap_closesock(soap);
}

#ifdef __cplusplus
}
#endif

static int
soap_d_serve___d__Hello(struct soap *soap)
{	struct __d__Hello soap_tmp___d__Hello;
	soap_default___d__Hello(soap, &soap_tmp___d__Hello);
	soap->encodingStyle = NULL;
	if (!soap_get___d__Hello(soap, &soap_tmp___d__Hello, "-d:Hello", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __d__Hello(soap, soap_tmp___d__Hello.d__Hello);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int
soap_d_serve___d__Bye(struct soap *soap)
{	struct __d__Bye soap_tmp___d__Bye;
	soap_default___d__Bye(soap, &soap_tmp___d__Bye);
	soap->encodingStyle = NULL;
	if (!soap_get___d__Bye(soap, &soap_tmp___d__Bye, "-d:Bye", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __d__Bye(soap, soap_tmp___d__Bye.d__Bye);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int
soap_d_serve___d__Probe(struct soap *soap)
{	struct __d__Probe soap_tmp___d__Probe;
	soap_default___d__Probe(soap, &soap_tmp___d__Probe);
	soap->encodingStyle = NULL;
	if (!soap_get___d__Probe(soap, &soap_tmp___d__Probe, "-d:Probe", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __d__Probe(soap, soap_tmp___d__Probe.d__Probe);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int
soap_d_serve___d__ProbeMatches(struct soap *soap)
{	struct __d__ProbeMatches soap_tmp___d__ProbeMatches;
	soap_default___d__ProbeMatches(soap, &soap_tmp___d__ProbeMatches);
	soap->encodingStyle = NULL;
	if (!soap_get___d__ProbeMatches(soap, &soap_tmp___d__ProbeMatches, "-d:ProbeMatches", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __d__ProbeMatches(soap, soap_tmp___d__ProbeMatches.d__ProbeMatches);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int
soap_d_serve___d__Resolve(struct soap *soap)
{	struct __d__Resolve soap_tmp___d__Resolve;
	soap_default___d__Resolve(soap, &soap_tmp___d__Resolve);
	soap->encodingStyle = NULL;
	if (!soap_get___d__Resolve(soap, &soap_tmp___d__Resolve, "-d:Resolve", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __d__Resolve(soap, soap_tmp___d__Resolve.d__Resolve);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int
soap_d_serve___d__ResolveMatches(struct soap *soap)
{	struct __d__ResolveMatches soap_tmp___d__ResolveMatches;
	soap_default___d__ResolveMatches(soap, &soap_tmp___d__ResolveMatches);
	soap->encodingStyle = NULL;
	if (!soap_get___d__ResolveMatches(soap, &soap_tmp___d__ResolveMatches, "-d:ResolveMatches", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __d__ResolveMatches(soap, soap_tmp___d__ResolveMatches.d__ResolveMatches);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

/**
@fn int __d__Hello(struct soap *soap, struct d__HelloType *Hello)
@brief Internal WS-Discovery service operation.
*/
int
__d__Hello(struct soap *soap, struct d__HelloType *Hello)
{
  unsigned int InstanceId = 0;
  const char *SequenceId = NULL;
  unsigned int MessageNumber = 0;

  DBGFUN("__d__Hello");

  /* check for WSA */
  if (soap_wsa_check(soap))
    return soap->error;

  /* check D */
  if (!Hello
#ifdef SOAP_WSA_2005
   || !Hello->wsa5__EndpointReference.Address
#else
   || !Hello->wsa__EndpointReference.Address
#endif
    )
    return soap_wsa_sender_fault(soap, "D Hello incomplete", NULL);

#ifdef SOAP_WSA_2005
  DBGLOG(TEST, SOAP_MESSAGE(fdebug, "EndpointReference=%s\n", Hello->wsa5__EndpointReference.Address));
#else
  DBGLOG(TEST, SOAP_MESSAGE(fdebug, "EndpointReference=%s\n", Hello->wsa__EndpointReference.Address));
#endif

  if (soap->header->d__AppSequence)
  {
    d__AppSequenceType *seq = soap->header->d__AppSequence;
    InstanceId = seq->InstanceId;
    SequenceId = seq->SequenceId;
    MessageNumber = seq->MessageNumber;
    DBGLOG(TEST, SOAP_MESSAGE(fdebug, "AppSeq id=%u seq=%s msg=%u\n", InstanceId, SequenceId ? SequenceId : "(null)", MessageNumber));
  }

  /* pass on to user-defined event handler */
  d_event_Hello(soap,
    InstanceId,
    SequenceId,
    MessageNumber, 
#ifdef SOAP_WSA_2005
    soap->header->wsa5__MessageID,
    soap->header->wsa5__RelatesTo ? soap->header->wsa5__RelatesTo->__item : NULL,
    Hello->wsa5__EndpointReference.Address,
#else
    soap->header->wsa__MessageID,
    soap->header->wsa__RelatesTo ? soap->header->wsa__RelatesTo->__item : NULL,
    Hello->wsa__EndpointReference.Address,
#endif
    Hello->Types,
    Hello->Scopes ? Hello->Scopes->__item : NULL,
    Hello->Scopes ? Hello->Scopes->MatchBy : NULL,
    Hello->XAddrs,
    Hello->MetadataVersion);

  /* one-way HTTP(S) POST message requires an HTTP OK response message */
  if (!strncmp(soap->endpoint, "http", 4))
    return soap_send_empty_response(soap, SOAP_OK);
  return SOAP_OK;
}

/**
@fn int __d__Bye(struct soap *soap, struct d__ByeType *Bye)
@brief Internal WS-Discovery service operation.
*/
int
__d__Bye(struct soap *soap, struct d__ByeType *Bye)
{
  unsigned int InstanceId = 0;
  const char *SequenceId = NULL;
  unsigned int MessageNumber = 0;

  DBGFUN("__d__Bye");

  /* check for WSA */
  if (soap_wsa_check(soap))
    return soap->error;

  /* check D */
  if (!Bye
#ifdef SOAP_WSA_2005
   || !Bye->wsa5__EndpointReference.Address
#else
   || !Bye->wsa__EndpointReference.Address
#endif
    )
    return soap_wsa_sender_fault(soap, "D Bye incomplete", NULL);

#ifdef SOAP_WSA_2005
  DBGLOG(TEST, SOAP_MESSAGE(fdebug, "EndpointReference=%s\n", Bye->wsa5__EndpointReference.Address));
#else
  DBGLOG(TEST, SOAP_MESSAGE(fdebug, "EndpointReference=%s\n", Bye->wsa__EndpointReference.Address));
#endif

  if (soap->header->d__AppSequence)
  {
    d__AppSequenceType *seq = soap->header->d__AppSequence;
    InstanceId = seq->InstanceId;
    SequenceId = seq->SequenceId;
    MessageNumber = seq->MessageNumber;
    DBGLOG(TEST, SOAP_MESSAGE(fdebug, "AppSeq id=%u seq=%s msg=%u\n", InstanceId, SequenceId ? SequenceId : "(null)", MessageNumber));
  }

  /* pass on to user-defined event handler */
  d_event_Bye(soap,
    InstanceId,
    SequenceId,
    MessageNumber, 
#ifdef SOAP_WSA_2005
    soap->header->wsa5__MessageID,
    soap->header->wsa5__RelatesTo ? soap->header->wsa5__RelatesTo->__item : NULL,
    Bye->wsa5__EndpointReference.Address,
#else
    soap->header->wsa__MessageID,
    soap->header->wsa__RelatesTo ? soap->header->wsa__RelatesTo->__item : NULL,
    Bye->wsa__EndpointReference.Address,
#endif
    Bye->Types,
    Bye->Scopes ? Bye->Scopes->__item : NULL,
    Bye->Scopes ? Bye->Scopes->MatchBy : NULL,
    Bye->XAddrs,
    Bye->MetadataVersion);

  /* one-way HTTP(S) POST message requires an HTTP OK response message */
  if (!strncmp(soap->endpoint, "http", 4))
    return soap_send_empty_response(soap, SOAP_OK);
  return SOAP_OK;
}

static int
soap_d_http(struct soap *soap, const char *endpoint, const char *host, int port, const char *path, const char *action, size_t count)
{ return soap->fresponse(soap, SOAP_OK, count);
}

/**
@fn int __d__Probe(struct soap *soap, struct d__ProbeType *Probe)
@brief Internal WS-Discovery service operation.
*/
int
__d__Probe(struct soap *soap, struct d__ProbeType *Probe)
{
  struct d__ProbeMatchesType ProbeMatches;
  soap_d_mode mode;

  DBGFUN("__d__Probe");

  /* check for WSA */
  if (soap_wsa_check(soap))
    return soap->error;

  /* check D */
  if (!Probe)
    return soap_wsa_sender_fault(soap, "D Probe incomplete", NULL);

  soap_default_d__ProbeMatchesType(soap, &ProbeMatches);


  /* pass probe request on to user-defined event handler to get matches */

	//soap->header->wsa5__ReplyTo = NULL;

  mode = d_event_Probe(soap,
#ifdef SOAP_WSA_2005
    soap->header->wsa5__MessageID,
    soap->header->wsa5__ReplyTo ? soap->header->wsa5__ReplyTo->Address : NULL,
#else
    soap->header->wsa__MessageID,
    soap->header->wsa__ReplyTo ? soap->header->wsa__ReplyTo->Address : NULL,
#endif
    Probe->Types,
    Probe->Scopes ? Probe->Scopes->__item : NULL,
    Probe->Scopes ? Probe->Scopes->MatchBy : NULL,
    &ProbeMatches);

  if(mode == 2)
  {
	onvif_fault_probe(soap,"d:MatchingRuleNotSupported","The matching rule specified is not supported");
	return SOAP_FAULT;
  }	
  if (mode == SOAP_D_ADHOC)
  {
    /* event handler or main application MUST return matches independently */

    /* one-way HTTP(S) POST message requires an HTTP OK response message */
    if (!strncmp(soap->endpoint, "http", 4))
      return soap_send_empty_response(soap, SOAP_OK);
  }
  else
  {
    int err;
    int (*fpost)(struct soap*, const char*, const char*, int, const char*, const char*, size_t);
    const char *MessageID = soap_wsa_rand_uuid(soap);
    const char *Action = SOAP_NAMESPACE_OF_d"/ProbeMatches";

    /* SOAP Header */
    soap_d_set_AppSequence(soap);
    soap_wsa_reply(soap, MessageID, Action);

    /* send over current socket as HTTP OK response: */
    fpost = soap->fpost;
    soap->fpost = soap_d_http;
    err = soap_send___d__ProbeMatches(soap, "http://", Action, &ProbeMatches);
    soap->fpost = fpost;
    return err;
  }
  return SOAP_OK;
}

/**
@fn int __d__ProbeMatches(struct soap *soap, struct d__ProbeMatchesType *ProbeMatches)
@brief Internal WS-Discovery service operation.
*/
int
__d__ProbeMatches(struct soap *soap, struct d__ProbeMatchesType *ProbeMatches)
{
  unsigned int InstanceId = 0;
  const char *SequenceId = NULL;
  unsigned int MessageNumber = 0;

  DBGFUN("__d__ProbeMatches");

  /* check for WSA */
  if (soap_wsa_check(soap))
    return soap->error;

  /* check D */
  if (!ProbeMatches)
    return soap_wsa_sender_fault(soap, "D ProbeMatches incomplete", NULL);

  if (soap->header->d__AppSequence)
  {
    d__AppSequenceType *seq = soap->header->d__AppSequence;
    InstanceId = seq->InstanceId;
    SequenceId = seq->SequenceId;
    MessageNumber = seq->MessageNumber;
    DBGLOG(TEST, SOAP_MESSAGE(fdebug, "AppSeq id=%u seq=%s msg=%u\n", InstanceId, SequenceId ? SequenceId : "(null)", MessageNumber));
  }

  /* pass probe matches on to user-defined event handler */
  d_event_ProbeMatches(soap,
    InstanceId,
    SequenceId,
    MessageNumber, 
#ifdef SOAP_WSA_2005
    soap->header->wsa5__MessageID,
    soap->header->wsa5__RelatesTo ? soap->header->wsa5__RelatesTo->__item : NULL,
#else
    soap->header->wsa__MessageID,
    soap->header->wsa__RelatesTo ? soap->header->wsa__RelatesTo->__item : NULL,
#endif
    ProbeMatches);

  /* one-way HTTP(S) POST message requires an HTTP OK response message */
  if (!strncmp(soap->endpoint, "http", 4))
    return soap_send_empty_response(soap, SOAP_OK);
  return SOAP_OK;
}

/**
@fn int __d__Resolve(struct soap *soap, struct d__ResolveType *Resolve)
@brief Internal WS-Discovery service operation.
*/
int
__d__Resolve(struct soap *soap, struct d__ResolveType *Resolve)
{
  struct d__ResolveMatchesType ResolveMatches;
  struct d__ResolveMatchType match;
  soap_d_mode mode;

  DBGFUN("__d__Resolve");

  /* check for WSA */
  if (soap_wsa_check(soap))
    return soap->error;

  /* check D */
  if (!Resolve
#ifdef SOAP_WSA_2005
   || !Resolve->wsa5__EndpointReference.Address
#else
   || !Resolve->wsa__EndpointReference.Address
#endif
    )
    return soap_wsa_sender_fault(soap, "D Resolve incomplete", NULL);

  soap_default_d__ResolveMatchesType(soap, &ResolveMatches);
  soap_default_d__ResolveMatchType(soap, &match);
  ResolveMatches.ResolveMatch = &match;

  /* pass resolve request on to user-defined event handler */
  mode = d_event_Resolve(soap,
#ifdef SOAP_WSA_2005
    soap->header->wsa5__MessageID,
    soap->header->wsa5__ReplyTo ? soap->header->wsa5__ReplyTo->Address : NULL,
    Resolve->wsa5__EndpointReference.Address,
#else
    soap->header->wsa__MessageID,
    soap->header->wsa__ReplyTo ? soap->header->wsa__ReplyTo->Address : NULL,
    Resolve->wsa__EndpointReference.Address,
#endif
    &match);

  if (mode == SOAP_D_ADHOC)
  {
    /* event handler or main application MUST return match independently */

    /* one-way HTTP(S) POST message requires an HTTP OK response message */
    if (!strncmp(soap->endpoint, "http", 4))
      return soap_send_empty_response(soap, SOAP_OK);
  }
  else
  {
    int err;
    int (*fpost)(struct soap*, const char*, const char*, int, const char*, const char*, size_t);
    const char *MessageID = soap_wsa_rand_uuid(soap);
    const char *Action = SOAP_NAMESPACE_OF_d"/ResolveMatches";

    /* SOAP Header */
    soap_d_set_AppSequence(soap);
    soap_wsa_reply(soap, MessageID, Action);

    /* send over current socket as HTTP OK response: */
    fpost = soap->fpost;
    soap->fpost = soap_d_http;
    err = soap_send___d__ResolveMatches(soap, NULL, Action, &ResolveMatches);
    soap->fpost = fpost;
    return err;
  }
  return SOAP_OK;
}

/**
@fn int __d__ResolveMatches(struct soap *soap, struct d__ResolveMatchesType *ResolveMatches)
@brief Internal WS-Discovery service operation.
*/
int
__d__ResolveMatches(struct soap *soap, struct d__ResolveMatchesType *ResolveMatches)
{
  unsigned int InstanceId = 0;
  const char *SequenceId = NULL;
  unsigned int MessageNumber = 0;

  DBGFUN("__d__ResolveMatches");

  /* check for WSA */
  if (soap_wsa_check(soap))
    return soap->error;

  /* check D */
  if (!ResolveMatches
   || !ResolveMatches->ResolveMatch)
    return soap_wsa_sender_fault(soap, "D ResolveMatch incomplete", NULL);

  if (soap->header->d__AppSequence)
  {
    d__AppSequenceType *seq = soap->header->d__AppSequence;
    InstanceId = seq->InstanceId;
    SequenceId = seq->SequenceId;
    MessageNumber = seq->MessageNumber;
    DBGLOG(TEST, SOAP_MESSAGE(fdebug, "AppSeq id=%u seq=%s msg=%u\n", InstanceId, SequenceId ? SequenceId : "(null)", MessageNumber));
  }

  /* pass resolve matches on to user-defined event handler */
  d_event_ResolveMatches(soap,
    InstanceId,
    SequenceId,
    MessageNumber, 
#ifdef SOAP_WSA_2005
    soap->header->wsa5__MessageID,
    soap->header->wsa5__RelatesTo ? soap->header->wsa5__RelatesTo->__item : NULL,
#else
    soap->header->wsa__MessageID,
    soap->header->wsa__RelatesTo ? soap->header->wsa__RelatesTo->__item : NULL,
#endif
    ResolveMatches->ResolveMatch);

  /* one-way HTTP(S) POST message requires an HTTP OK response message */
  if (!strncmp(soap->endpoint, "http", 4))
    return soap_send_empty_response(soap, SOAP_OK);
  return SOAP_OK;
}

/******************************************************************************\
 *
 *	WS-Discovery AppSequence State
 *
\******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
@fn void soap_d_set_InstanceId(unsigned int InstanceId)
@brief Set the global AppSequence InstanceId that is used to populate messages
@param[in] InstanceId
*/
void
soap_d_set_InstanceId(unsigned int InstanceId)
{
  MUTEX_LOCK(soap_d_state);
  soap_d_InstanceId = InstanceId;
  soap_d_MessageNumber = 1;
  MUTEX_UNLOCK(soap_d_state);
}

/**
@fn void soap_d_set_SequenceId(const char *SequenceId)
@brief Set the global AppSequence SequenceId that is used to populate messages
@param[in] SequenceId
*/
void
soap_d_set_SequenceId(const char *SequenceId)
{
  MUTEX_LOCK(soap_d_state);
  if (soap_d_SequenceId)
    free((void*)soap_d_SequenceId);
  soap_d_SequenceId = strdup(SequenceId);
  soap_d_MessageNumber = 1;
  MUTEX_UNLOCK(soap_d_state);
}

#ifdef __cplusplus
}
#endif

static void
soap_d_set_AppSequence(struct soap *soap)
{
  soap_header(soap);
  if (!soap->header->d__AppSequence)
  {
    soap->header->d__AppSequence = (d__AppSequenceType*)soap_malloc(soap, sizeof(d__AppSequenceType));
    soap_default_d__AppSequenceType(soap, soap->header->d__AppSequence);
  }

  MUTEX_LOCK(soap_d_state);
  soap->header->d__AppSequence->InstanceId = soap_d_InstanceId;
  soap->header->d__AppSequence->SequenceId = (char*)soap_d_SequenceId;
  soap->header->d__AppSequence->MessageNumber = soap_d_MessageNumber++;
  MUTEX_UNLOCK(soap_d_state);
}

static void
soap_d_reset_AppSequence(struct soap *soap)
{
  if (soap->header)
    soap->header->d__AppSequence = NULL;
}

static void
soap_d_delay(struct soap *soap)
{
  useconds_t delay = 1000*(soap_random % SOAP_D_APP_MAX_DELAY);
  usleep(delay);
}
