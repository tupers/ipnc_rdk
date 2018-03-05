#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <onvif.h>
#include <sys_env_type.h>
#include <sysctrl.h>
#include <para_list.h>
#include <file_msg_drv.h>
#include <system_default.h>
#include <DeviceBinding.nsmap>
#include <file_list.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <onvifface.h>
#include <soapStub.h>
#include <pthread.h>
#include <stdsoap2.h>
#include <net_config.h>
#include <appro_api.h>
#define security_token_profile (1)
#define MULTICAST_ENABLE (0)

#define IPCKEY 881103
#define IPCSZ 100
#include <assert.h>

#include <web_translate.h>
#include <signal.h>
int _true = 1;
int _false = 0;
int tempdhcp_flag;
int tempdnssearchdomainsize;
int temprtspportsize = 5;
char domainname[MID_INFO_LENGTH] = "ipnc";
extern char *TZname[];
long t_out; 
long time_out; 
int canSubscribe_pullmessages = 1;
extern int midhnewsock;
extern struct SOCKADDR onvif_remote_addr;
int isValidIp4 (char *str);
int startonvif(struct soap*);
unsigned long iptol(char *ip);
int canSubscribe = 1;
int _DateTimeType = DATE_MANUAL;
extern int server_udp;
notify_struct notify_args[MAX_SUBSCRIPTION];

int SOAP_ENV__Fault(struct soap* soap, char *faultcode, char *faultstring, char *faultactor, struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *SOAP_ENV__Code, struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node, char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));    
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __dn__Hello(struct soap* soap, struct d__HelloType dn__Hello, struct d__ResolveType *dn__HelloResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __dn__Bye(struct soap* soap, struct d__ByeType dn__Bye, struct d__ResolveType *dn__ByeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __dn__Probe(struct soap* soap, struct d__ProbeType dn__Probe, struct d__ProbeMatchesType *dn__ProbeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/**
 * @brief Get Services 
 * @This message contains a request for GetServices.
 *   
 * @param Request	: _tds__GetServices structure contains a request for GetServices. 
 Returns a collection of device services and their available capabilities 			 
 * @param Response	: _tds__GetServicesResponse contains the requested device capabilities.
 * @return               : On success - SOAP_OK.
 */
int __tds__GetServices(struct soap* soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *tds__GetServicesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
	}
#endif
	char _IPAddr[INFO_LENGTH];
	int i = 0;
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	char namespace[5][INFO_LENGTH] = {"http://www.onvif.org/ver10/device/wsdl","http://www.onvif.org/ver10/media/wsdl","http://www.onvif.org/ver20/imaging/wsdl","http://www.onvif.org/ver10/events/wsdl","http://www.onvif.org/ver10/deviceIO/wsdl"};
	tds__GetServicesResponse->__sizeService = 5;
	tds__GetServicesResponse->Service = (struct tds__Service *)soap_malloc(soap, tds__GetServicesResponse->__sizeService * sizeof(struct tds__Service));
	/* Attributes for network in device*/ 
	char *name_nw[] = {"IPFilter", "ZeroConfiguration", "IPVersion6", "DynDNS", "Dot11Configuration", "HostnameFromDHCP", "NTP"}; 
	/* If Attributes for network in device if implemented then -true else -false (based on datatype else 0 or 1)*/
	char *data_nw[] = {"false","false", "false", "false", "false ", "false","0"};
	/* Attributes for Security in device*/
	char *name_sec[] = {"TLS1.0", "TLS1.1", "TLS1.2", "OnboardKeyGeneration", "AccessPolicyConfig", "DefaultAccessPolicy", 
		"Dot1X", "RemoteUserHandling", "X.509Token", "SAMLToken", "KerberosToken","UsernameToken", "HttpDigest", "RELToken"};
	/* If Attributes for Security in device if implemented then -true else -false (based on datatype else 0 or 1)*/
	char *data_sec[] = {"false","false","false","false","false","false","false","false","false","false","false","true","false","false"};
	/* Attributes for System in device*/
	char *name_sys[] = {"DiscoveryResolve", "DiscoveryBye","RemoteDiscovery","SystemBackup", "SystemLogging",
		"FirmwareUpgrade", " HttpFirmwareUpgrade","HttpSystemBackup"," HttpSystemLogging", "HttpSupportInformation"};
	/* If Attributes for System in device if implemented then -true else -false (based on datatype else 0 or 1)*/
	char *data_sys[] = {"false", "false","false","false","false","false","false", "false","false", "false"};
	/* Attributes for Miscellaneous */
	char *name_misc[] = {"AuxiliaryCommands"};
	/* If Attributes for Miscellaneous is implemented then -true else -false (based on datatype else 0 or 1)*/
	char *data_misc[] = {"false"};
	/* Attributes for Events */
	//char *name_event[] = {"WSSubscriptionPolicySupport","WSPullPointSupport", "WSPausableSubscriptionManagerInterfaceSupport","RateLimitPolicySupport"};
	char *name_event[] = {"WSSubscriptionPolicySupport","WSPullPointSupport", "WSPausableSubscriptionManagerInterfaceSupport"};
	/* If Attributes for Events is implemented then -true else -false (based on datatype else 0 or 1)*/
	//char *data_event[] = {"true", "true", "true", "false"};
	char *data_event[] = {"true", "true", "true"};

	/* Attributes for Media */
	char *name_media[] = {"SnapshotUri","Rotation"};
	char *data_media[] = {"true","false"};
	/* Attributes for Media - profile */
	char *name_profile[] = {"MaximumNumberOfProfiles"};
	char *data_profile[] = {"4"};
	/* Attributes for Media - Streaming */
	char *name_stream[] = {"RTPMulticast", "RTP_TCP", "RTP_RTSP_TCP", "NonAggregateControl"};
	char *data_stream[] = {"true","false", "true", "true"};
	/* Attributes for IO*/
	char *name_IO[] = {"VideoSources","VideoOutputs","AudioSources","AudioOutputs","RelayOutputs","SerialPorts","DigitalInputs"};
	char *data_IO[] = {"1","0","1","1","1","0","0"};
	/* Attributes for Imaging*/
	char *name_imaging[] = {"ImageStabilization"};
	char *data_imaging[] = {"true"};
	for(i=0;i<tds__GetServicesResponse->__sizeService; i++)
	{
		if(tds__GetServices->IncludeCapability)
		{   
			tds__GetServicesResponse->Service[i].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(tds__GetServicesResponse->Service[i].Namespace, namespace[i]); 
			tds__GetServicesResponse->Service[i].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(tds__GetServicesResponse->Service[i].XAddr, _IPAddr);
			tds__GetServicesResponse->Service[i].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc(soap, sizeof(struct _tds__Service_Capabilities));

			if(strcmp( namespace[i], "http://www.onvif.org/ver10/deviceIO/wsdl" ) == 0)
			{
				/* Device IO */
				/* Parent- Device IO Capabilities */
				tds__GetServicesResponse->Service[i].Capabilities->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->next =  NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->prnt = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts  = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(tds__GetServicesResponse->Service[i].Capabilities->__any->atts, 6, name_IO, data_IO, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
				strcpy((char *)tds__GetServicesResponse->Service[i].Capabilities->__any->nstr, "http://www.onvif.org/ver10/DeviceIO/wsdl");
				tds__GetServicesResponse->Service[i].Capabilities->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->name, "tmd:Capabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->soap = soap;
				/* Device IO ends here */
			}

			if(strcmp(namespace[i],"http://www.onvif.org/ver10/media/wsdl") == 0)
			{
				/* Media */
				tds__GetServicesResponse->Service[i].Capabilities->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->next = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->prnt = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element)); ;
				tds__GetServicesResponse->Service[i].Capabilities->__any->atts = (struct soap_dom_attribute*)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(tds__GetServicesResponse->Service[i].Capabilities->__any->atts, 1, name_media, data_media, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->name, "trt:Capabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->soap = soap;

				/* Child profile Starts */
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next = (struct soap_dom_element*)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->prnt = tds__GetServicesResponse->Service[i].Capabilities->__any;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->elts = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->atts = (struct soap_dom_attribute*)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->atts, 0, name_profile, data_profile, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->name, "trt:ProfileCapabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->soap = soap;
				/* Child profile Ends */


				/* Child Streaming Starts */
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->prnt = tds__GetServicesResponse->Service[i].Capabilities->__any; 
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->elts = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->atts = (struct soap_dom_attribute*)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->atts, 3, name_stream, data_stream, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->name, "trt:StreamingCapabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->soap = soap;
				/* Child Streaming End */
			}

			if(strcmp(namespace[i], "http://www.onvif.org/ver10/device/wsdl") == 0)
			{
				/* Device Starts here */
				/* Parent- Device Capabilities */

				tds__GetServicesResponse->Service[i].Capabilities->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->next = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->prnt = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element)); ;
				tds__GetServicesResponse->Service[i].Capabilities->__any->atts = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->name, "tds:Capabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->soap = soap;

				/*Child Network Starts*/
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next = (struct soap_dom_element*)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->prnt = tds__GetServicesResponse->Service[i].Capabilities->__any;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->elts = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->atts = (struct soap_dom_attribute*)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(&tds__GetServicesResponse->Service[i].Capabilities->__any->elts->atts, 6, name_nw, data_nw, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->name, "tds:Network");
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->soap = soap;
				/*Child Network Ends*/

				/* Child Security Starts */
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next = (struct soap_dom_element*)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->prnt = tds__GetServicesResponse->Service[i].Capabilities->__any; 
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->elts = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->atts = (struct soap_dom_attribute*)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(&tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->atts, 13, name_sec, data_sec, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->name, "tds:Security");
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->soap = soap;
				/* Child Security Ends */

				/* Child System Starts */
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next = (struct soap_dom_element*)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->prnt = tds__GetServicesResponse->Service[i].Capabilities->__any; 
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->elts = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->atts = (struct soap_dom_attribute*)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(&tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->atts, 9, name_sys, data_sys, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->name, "tds:System");
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->soap = soap;
				/* Child System Starts */

				/* Child Misc Starts */
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->next = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->prnt = tds__GetServicesResponse->Service[i].Capabilities->__any; 
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->elts = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->atts = (struct soap_dom_attribute*)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(&tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->atts, 0, name_misc, data_misc, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->nstr = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->name, "tds:MiscCapabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts->next->next->next->soap = soap;
				/* Child Misc Starts */
				/* Device Ends here */
			}
			if(strcmp(namespace[i], "http://www.onvif.org/ver20/imaging/wsdl") == 0)
			{
				tds__GetServicesResponse->Service[i].Capabilities->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->next =  NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->prnt = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts  = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(tds__GetServicesResponse->Service[i].Capabilities->__any->atts, 0, name_imaging, data_imaging, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
				strcpy((char *)tds__GetServicesResponse->Service[i].Capabilities->__any->nstr, "http://www.onvif.org/ver20/imaging/wsdl");
				tds__GetServicesResponse->Service[i].Capabilities->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->name, "timg:Capabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->soap = soap;

			}

			if(strcmp(namespace[i], "http://www.onvif.org/ver10/events/wsdl") == 0)
			{
				/* Event Starts here */
				/* Parent- Event Capabilities */
				tds__GetServicesResponse->Service[i].Capabilities->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
				tds__GetServicesResponse->Service[i].Capabilities->__any->next =  NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->prnt = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->elts  = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
				dom_att(&tds__GetServicesResponse->Service[i].Capabilities->__any->atts, 2, name_event, data_event, NULL, NULL, soap);
				tds__GetServicesResponse->Service[i].Capabilities->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
				strcpy((char *)tds__GetServicesResponse->Service[i].Capabilities->__any->nstr, "http://www.onvif.org/ver10/events/wsdl");
				tds__GetServicesResponse->Service[i].Capabilities->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
				strcpy(tds__GetServicesResponse->Service[i].Capabilities->__any->name, "tev:Capabilities");
				tds__GetServicesResponse->Service[i].Capabilities->__any->data = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->wide = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->type = 0;
				tds__GetServicesResponse->Service[i].Capabilities->__any->node = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->head = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->tail = NULL;
				tds__GetServicesResponse->Service[i].Capabilities->__any->soap = soap;
				/* Event ends here */
			}

			tds__GetServicesResponse->Service[i].Version = (struct tt__OnvifVersion *)soap_malloc(soap,sizeof(struct tt__OnvifVersion));
			tds__GetServicesResponse->Service[i].Version->Major = MAJOR_VER;
			tds__GetServicesResponse->Service[i].Version->Minor = MINOR_VER;
			tds__GetServicesResponse->Service[i].__size = 0;
			tds__GetServicesResponse->Service[i].__any = NULL;
			dom_att(&tds__GetServicesResponse->Service[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		}
		else
		{
			tds__GetServicesResponse->Service[i].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(tds__GetServicesResponse->Service[i].Namespace, namespace[i]);
			tds__GetServicesResponse->Service[i].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH); 
			strcpy(tds__GetServicesResponse->Service[i].XAddr, _IPAddr);
			tds__GetServicesResponse->Service[i].Capabilities = NULL;
			tds__GetServicesResponse->Service[i].Version = (struct tt__OnvifVersion *)soap_malloc(soap,sizeof(struct tt__OnvifVersion));

			tds__GetServicesResponse->Service[i].Version->Major = MAJOR_VER;
			tds__GetServicesResponse->Service[i].Version->Minor = MINOR_VER;

			tds__GetServicesResponse->Service[i].__size = 0;
			tds__GetServicesResponse->Service[i].__any = NULL;
			dom_att(&tds__GetServicesResponse->Service[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		}
	}
	return SOAP_OK;
}

/**
 * @brief get service capabilities 
 * @this message contains a request for getservicecapabilities.
 *   
 * @param request	: _tds__getservicecapabilities structure contains a request for getservicecapabilities. 
 returns a capailities of device services 			 
 * @param response	: _tds__getservicesresponse contains the requested device capabilities.
 * @return               : on success - soap_ok.
 */
int __tds__GetServiceCapabilities(struct soap* soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
	}
#endif
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	char _IPv4Address[LARGE_INFO_LENGTH]; 
	sprintf(_IPv4Address, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	tds__GetServiceCapabilitiesResponse->Capabilities = (struct tds__DeviceServiceCapabilities *)soap_malloc(soap, sizeof(struct tds__DeviceServiceCapabilities));

	/* NETWORK */
	tds__GetServiceCapabilitiesResponse->Capabilities->Network = (struct tds__NetworkCapabilities *)soap_malloc(soap, sizeof(struct tds__NetworkCapabilities));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPFilter = soap_malloc(soap, sizeof(enum xsd__boolean ));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPFilter = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->ZeroConfiguration = soap_malloc(soap, sizeof(enum xsd__boolean ));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->ZeroConfiguration = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPVersion6 = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPVersion6 =  xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->DynDNS = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->DynDNS =  xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot11Configuration = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot11Configuration =  xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot1XConfigurations = (int *)soap_malloc(soap, sizeof(int));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot1XConfigurations = 0;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->HostnameFromDHCP = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->HostnameFromDHCP =  xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->NTP = (int *)soap_malloc(soap, sizeof(int));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->NTP = 0;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->DHCPv6 = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->DHCPv6 = xsd__boolean__false_;
	memset(&tds__GetServiceCapabilitiesResponse->Capabilities->Network->__anyAttribute, 0, sizeof(struct soap_dom_attribute));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->__anyAttribute.soap = soap; 

	/* SYSTEM */
	tds__GetServiceCapabilitiesResponse->Capabilities->System = (struct tds__SystemCapabilities*)soap_malloc(soap, sizeof(struct  tds__SystemCapabilities ));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryResolve =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryResolve = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryBye  =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryBye = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->RemoteDiscovery =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->RemoteDiscovery = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemBackup  =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemBackup = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->FirmwareUpgrade  =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->FirmwareUpgrade = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemLogging =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemLogging = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemBackup =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemBackup = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpFirmwareUpgrade =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpFirmwareUpgrade = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemLogging =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemLogging = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSupportInformation =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSupportInformation = xsd__boolean__false_;
	memset(&tds__GetServiceCapabilitiesResponse->Capabilities->System->__anyAttribute, 0, sizeof(struct soap_dom_attribute));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->__anyAttribute.soap = soap;


	/* SECURITY */
	tds__GetServiceCapabilitiesResponse->Capabilities->Security = (struct tds__SecurityCapabilities *)soap_malloc(soap, sizeof(struct tds__SecurityCapabilities));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e0 =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e0 = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e1 = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e1 = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e2 = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e2 = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->OnboardKeyGeneration = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->OnboardKeyGeneration = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->AccessPolicyConfig = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->AccessPolicyConfig = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->DefaultAccessPolicy = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->DefaultAccessPolicy = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->Dot1X = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->Dot1X = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->RemoteUserHandling = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->RemoteUserHandling = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->X_x002e509Token = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->X_x002e509Token = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->SAMLToken = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->SAMLToken = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->KerberosToken = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->KerberosToken = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->UsernameToken = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->UsernameToken = xsd__boolean__true_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->HttpDigest = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->HttpDigest = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->RELToken = soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->RELToken = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->SupportedEAPMethods = NULL;
	memset(&tds__GetServiceCapabilitiesResponse->Capabilities->Security->__anyAttribute, 0, sizeof(struct soap_dom_attribute));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->__anyAttribute.soap = soap;

	/* MISC */
	tds__GetServiceCapabilitiesResponse->Capabilities->Misc = (struct tds__MiscCapabilities*)soap_malloc(soap, sizeof(struct  tds__MiscCapabilities));
	tds__GetServiceCapabilitiesResponse->Capabilities->Misc->AuxiliaryCommands = NULL;

	memset(&tds__GetServiceCapabilitiesResponse->Capabilities->Misc->__anyAttribute, 0, sizeof(struct soap_dom_attribute ));
	tds__GetServiceCapabilitiesResponse->Capabilities->Misc->__anyAttribute.soap = soap;

	return SOAP_OK;
}

/**
 * @brief Get Device Information (Onvif Core Spec.Ver 2.1)
 * @This operation gets the following device information:
 -Manufacturer,
 -Model,
 -Firmware version, 
 -Serial Number,
 -Hardware Id
 *
 * @param Request       : _tds__GetDeviceInformation structure contains the request. In this function, the request is an empty message.
 * @param Response      : _tds__GetDeviceInformationResponse structure contains the response. 
 * @return              : On success - SOAP_OK
 On failure - There is no command specific faults returned.
 */
int __tds__GetDeviceInformation(struct soap* soap, struct _tds__GetDeviceInformation *tds__GetDeviceInformation, struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse) {

	SysInfo* oSysInfo = GetSysInfo();
	unsigned char mac[MACH_ADDR_LENGTH];

	char *_Manufacturer = (char *) malloc(sizeof(char) * LARGE_INFO_LENGTH);
	char *_Model = (char *) malloc(sizeof(char) * LARGE_INFO_LENGTH);
	char *_FirmwareVersion =  (char *) malloc(sizeof(char) * LARGE_INFO_LENGTH);
	char *_SerialNumber = (char *) malloc(sizeof(char) * LARGE_INFO_LENGTH);
	char *_HardwareId = (char *) malloc(sizeof(char) * LARGE_INFO_LENGTH);

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
	}
#endif

	strcpy(_Manufacturer,oSysInfo->lan_config.title);
	strcpy(_Model,oSysInfo->lan_config.modelname);

	strcpy(_FirmwareVersion,IPNC_APP_VERSION);
	net_get_hwaddr(ETH_NAME, mac);

	sprintf(_SerialNumber,"%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	sprintf(_HardwareId,"1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	tds__GetDeviceInformationResponse->Manufacturer = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	tds__GetDeviceInformationResponse->Model = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	tds__GetDeviceInformationResponse->FirmwareVersion = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	tds__GetDeviceInformationResponse->SerialNumber = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	tds__GetDeviceInformationResponse->HardwareId = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);

	strcpy(tds__GetDeviceInformationResponse->Manufacturer, _Manufacturer);
	strcpy(tds__GetDeviceInformationResponse->Model, _Model);
	strcpy(tds__GetDeviceInformationResponse->FirmwareVersion, _FirmwareVersion);
	strcpy(tds__GetDeviceInformationResponse->SerialNumber, _SerialNumber);
	strcpy(tds__GetDeviceInformationResponse->HardwareId, _HardwareId);

	free(_Manufacturer);
	free(_Model);
	free(_FirmwareVersion);
	free(_SerialNumber);
	free(_HardwareId);
	return SOAP_OK;
}


/**
 * @brief Setting Date and Time of IPNC.(Onvif Core Spec.Ver 2.0- Section 8.3.7)
 * @This operation sets the following device information.
 -System date,
 -Time
 *
 * @param Request       : _tds__SetSystemDateAndTime structure contains the date and time information of the device.
 * @param Response      : _tds___tds__SetSystemDateAndTimeResponse structure contains dummy response.
 * @return returns       : On success - SOAP_OK.
 On failure - SOAP_FAULT.
 */
int __tds__SetSystemDateAndTime(struct soap* soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	char value[INFO_LENGTH];
	__u8  ret;
	int _Year;
	int _Month;
	int _Day;
	int _Hour;
	int _Minute;
	int _Second;
	__u8 _DaylightSavings;
	char *_TZ;

	_DateTimeType = tds__SetSystemDateAndTime->DateTimeType;//Manual = 0, NTP = 1

	// Time Zone
	if(tds__SetSystemDateAndTime->TimeZone)
	{
		_TZ  = tds__SetSystemDateAndTime->TimeZone->TZ;
		ret = (__u8 )oset_timezone(_TZ);
		if(ret == 100)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:InvalidTimeZone","Invalid data");
			return SOAP_FAULT;
		}
		ControlSystemData(SFIELD_SET_TIMEZONE, &ret, sizeof(ret));
	}


	_DaylightSavings = (__u8)tds__SetSystemDateAndTime->DaylightSavings;

	ControlSystemData(SFIELD_SET_DAYLIGHT, &_DaylightSavings, sizeof(_DaylightSavings));

	if(tds__SetSystemDateAndTime->UTCDateTime)
	{
		_Year = tds__SetSystemDateAndTime->UTCDateTime->Date->Year;
		_Month = tds__SetSystemDateAndTime->UTCDateTime->Date->Month;
		_Day = tds__SetSystemDateAndTime->UTCDateTime->Date->Day;
		_Hour = tds__SetSystemDateAndTime->UTCDateTime->Time->Hour;
		_Minute = tds__SetSystemDateAndTime->UTCDateTime->Time->Minute;
		_Second = tds__SetSystemDateAndTime->UTCDateTime->Time->Second;

		if((_Hour>24) || (_Minute>60) || (_Second>60) || (_Month>12) || (_Day>31))
		{		
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidDateTime","Invalid date and time.");
			return SOAP_FAULT;
		}

		sys_set_date(_Year, _Month, _Day);
		sys_set_time(_Hour, _Minute, _Second);
	}

	if(_DateTimeType == 1)//only for NTP
	{
		ControlSystemData(SFIELD_GET_SNTP_FQDN, (void *)value, sizeof(value)); //get sntp ip
		run_ntpclient(value);
		return SOAP_OK;
	}     

	return SOAP_OK;

}

/**
 * @brief Getting Date and Time of IPNC.
 * @This Operation gets the following information from the device:
 -System Date
 -Day light saving
 -Time Zone
 -Time
 * @param Request       :_tds__GetSystemDateAndTime structure contains requests.In this function, the request is an empty message.
 * @param Response      :_tds___tds__GetSystemDateAndTimeResponse sturcture variable members contains the values for Date and Time of IPNC.
 * @return               :On success -  SOAP_OK.
 :On failure -  No command specific faults
 */
int __tds__GetSystemDateAndTime(struct soap* soap, struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
	}
#endif
	/* UTC - Coordinated Universal Time */
	time(&current_time);
	sleep(2);
	SysInfo* oSysInfo = GetSysInfo();
	time_t tnow;
	struct tm *tmnow;

	time(&tnow);
	tmnow = localtime(&tnow);

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime = (struct tt__SystemDateTime*)soap_malloc(soap, sizeof(struct tt__SystemDateTime));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DateTimeType = _DateTimeType;

	if((oSysInfo->lan_config.net.daylight_time))
		tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DaylightSavings = 1;//(int)oSysInfo->lan_config.net.daylight_time;//1
	else tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DaylightSavings = 0;

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->Extension = NULL;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone = (struct tt__TimeZone*)soap_malloc(soap, sizeof(struct tt__TimeZone));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone->TZ = (char *)soap_malloc(soap,sizeof(char)*100);
	strcpy(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone->TZ,oget_timezone(oSysInfo->lan_config.net.ntp_timezone));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime = (struct tt__DateTime*)soap_malloc(soap, sizeof(struct tt__DateTime));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time = (struct tt__Time*)soap_malloc(soap, sizeof(struct tt__Time));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Hour = tmnow->tm_hour;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Minute = tmnow->tm_min;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Second = tmnow->tm_sec;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date = (struct tt__Date*)soap_malloc(soap, sizeof(struct tt__Date));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Year = tmnow->tm_year + 1900;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Month = tmnow->tm_mon + 1;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Day = tmnow->tm_mday;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime = (struct tt__DateTime*)soap_malloc(soap, sizeof(struct tt__DateTime));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time = (struct tt__Time*)soap_malloc(soap, sizeof(struct tt__Time));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Hour = tmnow->tm_hour;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Minute = tmnow->tm_min;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Second = tmnow->tm_sec;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date = (struct tt__Date*)soap_malloc(soap, sizeof(struct tt__Date));
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Year = tmnow->tm_year + 1900;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Month = tmnow->tm_mon + 1;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Day = tmnow->tm_mday;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->Extension = NULL;
	dom_att(&tds__GetSystemDateAndTimeResponse->SystemDateAndTime->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK; 

}


/**
 * @brief Setting System Factory Default of  IPNC.
 * @This operation reloads parameters of a device to their factory default values.
 * @param Request       : _tds__GetSystemFactoryDefault structure contains request for the following types of factory default to perform:
 "Hard": All parameters are set to their factory default value
 "Soft": All parameters except device vendor specific
 parameters are set to their factory default values
 * @param Response      : _tds__GetSystemDateAndTimeResponse structure variable members contains dummy response  .
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults
 */
int __tds__SetSystemFactoryDefault(struct soap* soap, struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault, struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	__u8 value;
	value = DEFAULT_VALUE;
	ControlSystemData(SFIELD_SET_DEFAULT, &value, sizeof(value));
	//"Hard": All parameters are set to their factory default value
	//"Soft": All parameters except device vendor specific parameters are set to their factory default values
	return SOAP_OK;
}


/**
 * @brief Updating firmware for IPNC.
 * @This operation upgrades a device firmware version
 * @param Request       : _tds__UpgradeSystemFirmware structure contains the firmware used for the upgrade.
 * @param Response      : _tds__UpgradeSystemFirmwareResponse structure variable members contains dummy response.
 * @return               : On success - SOAP_OK.
 On failure - The firmware was invalid, i.e., not supported by this device.
 */
int __tds__UpgradeSystemFirmware(struct soap* soap, struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware, struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse) {
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/**
 * @brief Reboot the IPNC.
 * @This function reboot the system.
 * @param Request       : _tds__SystemReboot structure contains dummy request.
 * @param Response      : _tds__SystemReboot structure contains the following message:
 "Rebooting".
 * @return               : On success - SOAP_OK.
 : On failure - No command specific faults.
 */
int __tds__SystemReboot(struct soap* soap, struct _tds__SystemReboot *tds__SystemReboot, struct _tds__SystemRebootResponse *tds__SystemRebootResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	tds__SystemRebootResponse->Message = "Rebooting";
	pthread_t reboot_th = 0;
	pthread_create(&reboot_th,NULL,reboot_thread,NULL);
	return SOAP_OK;
}    


/**
 * @brief Restore System.
 * @This operation restores the system backup configuration files(s) previously retrieved from a device.

 * @param Request       : _tds__RestoreSystem structure contains the system backup file(s).
 * @param Response      : _tds__RestoreSystem structure dummy response.in this function response message is empity.
 * @return               : On success - SOAP_OK.
 : On failure - The backup file(s) are invalid.
 */
int __tds__RestoreSystem(struct soap* soap, struct _tds__RestoreSystem *tds__RestoreSystem, struct _tds__RestoreSystemResponse *tds__RestoreSystemResponse) {
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}



/**
 * @brief Get System backup
 * @This operation gets the  system backup configuration files(s) previously retrieved from adevice.
 * @param Request       :_tds__GetSystemBackup structure contains request for system backup.in this function message is empity.
 * @param Response      :_tds__GetSystemBackupResponse structure contains the system backup configuration files(s).
 * @return              : On success - SOAP_OK.
 : On failure - No command specific faults.

 */
int __tds__GetSystemBackup(struct soap* soap, struct _tds__GetSystemBackup *tds__GetSystemBackup, struct _tds__GetSystemBackupResponse *tds__GetSystemBackupResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


/** 
 * @brief  Get System log from a device. 
 * @This operation gets a system log from a device.

 * @param Request       : _tds__GetSystemLog structure contains the type of system log to retrieve. The types of supported log information is defined in two different types:
 "System": The system log
 "Access": The client access log

 * @param Response      : _tds__GetSystemLogResponse structure variable members contains the requested system log information.
 * @return               : On success - SOAP_OK on success.
 : On failure - There is no access log information available or 
 There is no system log information available

 */
int __tds__GetSystemLog(struct soap* soap, struct _tds__GetSystemLog *tds__GetSystemLog, struct _tds__GetSystemLogResponse *tds__GetSystemLogResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	assert(tds__GetSystemLogResponse != NULL);
	tds__GetSystemLogResponse->SystemLog = (struct tt__SystemLog*)soap_malloc(soap, sizeof(struct tt__SystemLog));
	tds__GetSystemLogResponse->SystemLog->Binary = (struct tt__AttachmentData*)soap_malloc(soap, sizeof(struct tt__AttachmentData));
	//System = 0, Access = 1

	if(tds__GetSystemLog->LogType == 0)
	{
		onvif_fault(soap,1,"ter:InvalidArgs","ter:SystemlogUnavailable","No system log available");
		return SOAP_FAULT;
	}

	else if(tds__GetSystemLog->LogType == 1)
	{
		onvif_fault(soap,1,"ter:InvalidArgs","ter:AccesslogUnavailable","No system log available");
		return SOAP_FAULT;
	}
	return SOAP_OK;
}


/**
  @brief Get System Support Information
  @This operation gets arbitrary device diagnostics information from a device.
  @param request             :_tds__GetSystemSupportInformation structure contains the request.in this function request is an empity message.
  @param response            :_tds__GetSystemSupportInformationResponse strucure contains the support information. The device can choose if it wants to return the support 
  information as binary data or as a common string.
  @return                    :On success - SOAP_OK
  :On failure - There is no support information available.

 */
int __tds__GetSystemSupportInformation(struct soap* soap, struct _tds__GetSystemSupportInformation *tds__GetSystemSupportInformation, struct _tds__GetSystemSupportInformationResponse *tds__GetSystemSupportInformationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


/** 
 * @brief Get Scopes of the device
 * @This operation requests the scope parameters of a device. 

 *   
 * @param Request   : _tds__GetScopes structure varialbles members contains an empty message.
 * @param Response  : _tds__GetScopesResponse structure variable members ontains a list of URIs 
 defining the device scopes.
 * @return               : On success - SOAP_OK.
 : On failure - Scope list is empty.
 */
int __tds__GetScopes(struct soap* soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int i = 0;
	int num_type = 0;
	int num_loc = 0;
	int num_improper = 0;
	int num_scope = 0;
	int _size = 0;

	for(i = 0; i < 3; i++)
	{
		if(oSysInfo->onvif_scopes.type[i]!= NULL)
			num_type++;
		if(oSysInfo->onvif_scopes.location[i]!= NULL)
			num_loc++;
		if(oSysInfo->onvif_scopes.improper_input[i]!= NULL)
			num_improper++;
	}
	_size = (num_type + num_loc + 2 + num_improper);

	tds__GetScopesResponse->__sizeScopes = _size;
	tds__GetScopesResponse->Scopes = (struct tt__Scope*)soap_malloc(soap, (_size *sizeof(struct tt__Scope)));
	for(i = 0; i < _size; i++)
		tds__GetScopesResponse->Scopes[i].ScopeItem = (char *)soap_malloc(soap, (sizeof(char) * INFO_LENGTH));

	for(i = 0; i < 3; i++)
	{
		if(oSysInfo->onvif_scopes.type[i] != NULL)
		{
			tds__GetScopesResponse->Scopes[num_scope].ScopeDef = 1; // Fixed = 0, Configurable = 1
			strcpy(tds__GetScopesResponse->Scopes[num_scope].ScopeItem, oSysInfo->onvif_scopes.type[i]);
			num_scope++;
		}
	}

	tds__GetScopesResponse->Scopes[num_scope].ScopeDef = 0;
	strcpy(tds__GetScopesResponse->Scopes[num_scope].ScopeItem, oSysInfo->onvif_scopes.name);
	num_scope ++;

	for(i = 0; i < 3; i++)
	{
		if(oSysInfo->onvif_scopes.location[i] != NULL)
		{
			tds__GetScopesResponse->Scopes[num_scope].ScopeDef = 1; // Fixed = 0, Configurable = 1
			strcpy(tds__GetScopesResponse->Scopes[num_scope].ScopeItem, oSysInfo->onvif_scopes.location[i]);
			num_scope++;
		}
	}

	tds__GetScopesResponse->Scopes[num_scope].ScopeDef = 0; // Fixed = 0, Configurable = 1
	strcpy(tds__GetScopesResponse->Scopes[num_scope].ScopeItem, oSysInfo->onvif_scopes.hardware);
	num_scope++;

	for(i = 0; i < 3; i++)
	{
		if(oSysInfo->onvif_scopes.improper_input[i] != NULL)
		{
			tds__GetScopesResponse->Scopes[num_scope].ScopeDef = 1; // Fixed = 0, Configurable = 1
			strcpy(tds__GetScopesResponse->Scopes[num_scope].ScopeItem, oSysInfo->onvif_scopes.improper_input[i]);
			num_scope++;
		}
	}
	return SOAP_OK;
}


/**
 * @brief Set scopes of device 
 * @This operation sets the scope parameters of a device. The scope parameters are used in the
 device discovery to match probe messages.
 *   
 * @param Request   : _tds__SetScopes structure contains a list of URIs defining the device scope. 
 * @param Response  : _tds__SetScopesResponse structure contains an empty message.
 * @return               : On success SOAP_OK.
 : On failure - Scope parameter overwrites fixed device scope setting, command rejected. or
 The requested scope list exceeds the supported number of scopes.

 */
int __tds__SetScopes(struct soap* soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int TYPE_MAX = 0;
	int LOC_MAX = 0;
	int i = 0;
	int num_tp = 0;
	int num_loc = 0;
	int MATCH_FOUND = 0;
	int NO_MATCH = 0;
	int num_type = 0;
	int num_location = 0;

	int num_improper = 0;
	int IMPROPER_MAX = 0;
	int num_junk = 0;

	int size = tds__SetScopes->__sizeScopes;
	Onvif_Scopes_t onvif_scopes_t;
	memset(&onvif_scopes_t,0,sizeof(onvif_scopes_t));

	for(i = 0; i < 3; i++)
	{
		if(strstr(oSysInfo->onvif_scopes.type[i], "/type") != NULL)
			TYPE_MAX++;

		if(strstr(oSysInfo->onvif_scopes.location[i], "/location") != NULL)
			LOC_MAX++;
		if(strstr(oSysInfo->onvif_scopes.improper_input[i],"onvif://www.onvif.org/")!= NULL)
		{
			IMPROPER_MAX++;
		}
	}   
	num_junk = 0;int len =0;
	for(i = 0; i < size; i++)
	{   
		len =0;
		if((len = strlen(tds__SetScopes->Scopes[i]))< 2)
			continue;
		NO_MATCH = 0;
		MATCH_FOUND = 0; 

		if(strstr(tds__SetScopes->Scopes[i], "/type")!= NULL)
		{
			while(num_tp < TYPE_MAX)
			{
				if(strcmp(oSysInfo->onvif_scopes.type[num_tp],tds__SetScopes->Scopes[i])== 0)
				{
					MATCH_FOUND++;
					num_tp++;
				}
				else
				{
					NO_MATCH++;
					num_tp++;
				}
			}
			num_tp = 0;
			if(MATCH_FOUND == 0)
				strcpy(onvif_scopes_t.type_t[num_type],tds__SetScopes->Scopes[i]);
			num_type++;
		}

		else if(strstr(tds__SetScopes->Scopes[i],"/location")!= NULL)
		{
			while(num_loc < LOC_MAX)
			{
				if(strcmp(oSysInfo->onvif_scopes.location[num_loc], tds__SetScopes->Scopes[i])== 0)
				{
					MATCH_FOUND++;
					num_loc++;
				}
				else
				{
					NO_MATCH++;
					num_loc++;
				}
			}
			num_loc = 0;
			if(MATCH_FOUND == 0)
				strcpy(onvif_scopes_t.location_t[num_location],tds__SetScopes->Scopes[i]);
			num_location++;
		}

		else if((strstr(tds__SetScopes->Scopes[i],"/name")!= NULL)||(strstr(tds__SetScopes->Scopes[i],"/hardware")!= NULL))
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:ScopeOverwrite","Trying overwriting permanent device scope setting");
			return SOAP_FAULT;
		}
		/* FOR IMPROPER INPUT */
		else if((strstr(tds__SetScopes->Scopes[i], "/type") == NULL) && (strstr(tds__SetScopes->Scopes[i],"/location")== NULL) && (strstr(tds__SetScopes->Scopes[i],"/name") == NULL) &&(strstr(tds__SetScopes->Scopes[i],"/hardware") == NULL)&&(strstr(tds__SetScopes->Scopes[i],"onvif") != NULL))
		{
			while(num_improper < IMPROPER_MAX)
			{
				if(strcmp(oSysInfo->onvif_scopes.improper_input[num_improper],tds__SetScopes->Scopes[i])== 0)
				{
					MATCH_FOUND++;
					num_improper++;
				}
				else
				{
					NO_MATCH++;
					num_improper++;
				}
			}
			num_improper = 0;
			if(MATCH_FOUND == 0 && num_junk < 3)
			{
				strcpy(onvif_scopes_t.improper_input_t[num_junk],tds__SetScopes->Scopes[i]);
			}
			num_junk++;
		}

		else
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:ScopeOverwrite","Trying overwriting permanent device scope setting ");
			return SOAP_FAULT;
		}

	}
	ControlSystemData(SFIELD_SET_SCOPE, &onvif_scopes_t, sizeof(onvif_scopes_t));
	return SOAP_OK;
}


/**     
 * @brief Add Scopes of the device
 * @This operation adds new configurable scope parameters to a device. 

 *   
 * @param Request        : _tds__AddScopes structure  contains a list of URIs to be added to the existing configurable scope list.

 * @param Response       : _tds__AddScopesResponse structure contains an empty message. 

 * @return               : On success - SOAP_OK.
 : On failure - Appropriate fault code.

 */
int __tds__AddScopes(struct soap* soap, struct _tds__AddScopes *tds__AddScopes, struct _tds__AddScopesResponse *tds__AddScopesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	Onvif_Scopes_t onvif_scopes_t;
	int size =  tds__AddScopes->__sizeScopeItem;
	int i;
	int num_loc=0;
	int loc_index=0;
	int num_tp=0;
	int type_index =0;
	int name_index = 0;
	int hardware_index = 0;
	int TYPE_EXIST =0;
	int LOC_EXIST =0;
	int IMPROPER_EXIST = 0;
	int num_improper = 0;
	int junk_index = 0;
	memset(&onvif_scopes_t,0,sizeof(onvif_scopes_t));
	for(i = 0; i < 3; i++)
	{
		if(oSysInfo->onvif_scopes.type[i]!=NULL)
		{
			if(strstr(oSysInfo->onvif_scopes.type[i],"/type")!=NULL)
				TYPE_EXIST++;
		}
		if(oSysInfo->onvif_scopes.location[i]!=NULL)
		{	
			if(strstr(oSysInfo->onvif_scopes.location[i],"/location")!=NULL)
				LOC_EXIST++;
		}
		if(strstr(oSysInfo->onvif_scopes.improper_input[i], "onvif://www.onvif.org")!= NULL)
		{	
			IMPROPER_EXIST++;
		}

	}
	for(i = 0; i < size; i++)
	{
		num_tp =0;
		num_loc =0; 
		num_improper = 0;	
		if(strstr(tds__AddScopes->ScopeItem[i],"/type")!=NULL)
		{
			if(TYPE_EXIST == 3)
			{
				onvif_fault(soap,0,"ter:OperationProhibited", "ter:TooManyScopes","Too large scope list");
				return SOAP_FAULT;
			}

			while(num_tp < 3)
			{
				if(strcmp(oSysInfo->onvif_scopes.type[num_tp],tds__AddScopes->ScopeItem[i])==0)
				{
					onvif_fault(soap,1,"ter:OperationProhibited", "ter:ScopeOverwrite","Trying overwriting permanent device scope setting");
					return SOAP_FAULT;
				}	
				else
				{
					num_tp++;
				}
			}
			if(type_index < 3)
			{
				strcpy(onvif_scopes_t.type_t[type_index],tds__AddScopes->ScopeItem[i]);
				type_index++;
			}
			else
			{
				onvif_fault(soap,0,"ter:OperationProhibited", "ter:TooManyScopes","Too large scope list");
				return SOAP_FAULT;	
			}
		}
		if(strstr(tds__AddScopes->ScopeItem[i],"/location")!=NULL)
		{
			if(LOC_EXIST == 3)
			{
				onvif_fault(soap,0,"ter:OperationProhibited", "ter:TooManyScopes","Too large scope list");
				return SOAP_FAULT;
			}

			while(num_loc < 3)
			{
				if(strcmp(oSysInfo->onvif_scopes.location[num_loc],tds__AddScopes->ScopeItem[i])==0)
				{
					onvif_fault(soap,1,"ter:OperationProhibited", "ter:ScopeOverwrite","Trying overwriting permanent device scope setting");
					return SOAP_FAULT;
				}	
				else
				{
					num_loc++;
				}
			}
			if(loc_index < 3)
			{
				strcpy(onvif_scopes_t.location_t[loc_index],tds__AddScopes->ScopeItem[i]);
				loc_index++;
			}
			else
			{
				onvif_fault(soap,0,"ter:OperationProhibited", "ter:TooManyScopes","Too large scope list");
				return SOAP_FAULT;	
			}

		} 
		if((strstr(tds__AddScopes->ScopeItem[i],"/name")!=NULL)||(strstr(tds__AddScopes->ScopeItem[i],"/hardware")!=NULL))
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:ScopeOverwrite","Trying overwriting permanent device scope setting");
			return SOAP_FAULT;
		}

		/*TODO: Name and Hardware index are hardcoded with '0'. Since the onvif spec doesnt support multiple name and hardware values, we stick to one value here. When there is a change in the spec, this needs to be modified*/
		if((strstr(tds__AddScopes->ScopeItem[i], "/type") == NULL) && (strstr(tds__AddScopes->ScopeItem[i],"/location")== NULL) && (strstr(tds__AddScopes->ScopeItem[name_index],"/name") == NULL) &&(strstr(tds__AddScopes->ScopeItem[hardware_index],"/hardware") == NULL))
		{
			if(IMPROPER_EXIST == 3)
			{
				onvif_fault(soap,0,"ter:OperationProhibited", "ter:TooManyScopes","Too large scope list");
				return SOAP_FAULT;
			}

			while(num_improper < 3)
			{
				if(strcmp(oSysInfo->onvif_scopes.improper_input[num_improper],tds__AddScopes->ScopeItem[i])==0)
				{
					onvif_fault(soap,1,"ter:OperationProhibited", "ter:ScopeOverwrite","Trying overwriting permanent device scope setting");
					return SOAP_FAULT;
				}	
				else
				{
					num_improper++;
				}
			}
			if(junk_index < 3)
			{
				strcpy(onvif_scopes_t.improper_input_t[junk_index],tds__AddScopes->ScopeItem[i]);
				junk_index++;
			}
			else
			{
				onvif_fault(soap,0,"ter:OperationProhibited", "ter:TooManyScopes","Too large scope list");
				return SOAP_FAULT;	
			}
		}
	}

	ControlSystemData(SFIELD_ADD_SCOPE, &onvif_scopes_t, sizeof(onvif_scopes_t));

	if(pthread_create(&hello_thread, NULL, hello_thr_scope, (void *)server_udp) != 0)
		ONVIF_DBG("Error:thread creation failed");

	return SOAP_OK;
}

/**         
 * @brief Remove Scopes of the device
 * @ This operation deletes scope-configurable scope parameters from a device.
 *   
 * @param Request        : _tds__RemoveScopes structure  contains a list of URIs to be removed to the existing configurable scope list.

 * @param Response       : _tds__RemoveScopesResponse structure The scope response message contains a list of URIs that has been removed from the     device scope.
 * @return               : On success - SOAP_OK.
 : On failure - Appropriate fault code.

 */
int __tds__RemoveScopes(struct soap* soap, struct _tds__RemoveScopes *tds__RemoveScopes, struct _tds__RemoveScopesResponse *tds__RemoveScopesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i = 0;
	int num_tp = 0;
	int num_loc = 0;	
	int num_type = 0;
	int num_location = 0;
	int name_index = 0;
	int hardware_index = 0;
	int LOC_EXIST = 0;
	int TYPE_EXIST = 0;
	Onvif_Scopes_t onvif_scopes_t;
	memset(&onvif_scopes_t,0,sizeof(onvif_scopes_t));
	int num_improper = 0;
	int num_junk = 0;
	int size = tds__RemoveScopes->__sizeScopeItem;
	for(i=0; i<3; i++)
	{
		if(strstr(oSysInfo->onvif_scopes.type[i],"/type")!=NULL)
			TYPE_EXIST++;
		if(strstr(oSysInfo->onvif_scopes.location[i],"/location")!=NULL)
			LOC_EXIST++;
	}
	for(i = 0; i < size; i++)
	{
		num_tp = 0;
		num_loc= 0;
		num_junk = 0;

		if(strstr(tds__RemoveScopes->ScopeItem[i],"/type")!=NULL)
		{
			while(num_tp < 3)
			{
				if(strcmp(oSysInfo->onvif_scopes.type[num_tp],tds__RemoveScopes->ScopeItem[i])==0)
				{
					if(num_type < 3)
					{
						strcpy(onvif_scopes_t.type_t[num_type],tds__RemoveScopes->ScopeItem[i]);
						num_type++;
					}
					else
					{
						onvif_fault(soap,0,"ter:InvalidArgs", "ter:TooManyScopes","Too large scope list");
						return SOAP_FAULT;
					}
				}
				num_tp++;
			}
		}		
		else if(strstr(tds__RemoveScopes->ScopeItem[i],"/location")!=NULL)
		{
			while(num_loc < 3)
			{
				if(strcmp(oSysInfo->onvif_scopes.location[num_loc],tds__RemoveScopes->ScopeItem[i])==0)
				{
					if(num_location < 3)
					{
						strcpy(onvif_scopes_t.location_t[num_location],tds__RemoveScopes->ScopeItem[i]);
						num_location++;
					}
					else
					{
						onvif_fault(soap,0,"ter:InvalidArgs", "ter:TooManyScopes","Too large scope list");
						return SOAP_FAULT;
					}
				}
				num_loc++;
			}
		}

		else if((strstr(tds__RemoveScopes->ScopeItem[i],"/name")!=NULL)||(strstr(tds__RemoveScopes->ScopeItem[i],"/hardware")!=NULL))
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:ScopeOverwrite","Trying overwriting permanent device scope setting");
			return SOAP_FAULT;
		}

		else if((strstr(tds__RemoveScopes->ScopeItem[i], "/type") == NULL) && (strstr(tds__RemoveScopes->ScopeItem[i],"/location")== NULL) && (strstr(tds__RemoveScopes->ScopeItem[name_index],"/name") == NULL) &&(strstr(tds__RemoveScopes->ScopeItem[hardware_index],"/hardware") == NULL))
		{
			while(num_junk < 3)
			{
				if(strcmp(oSysInfo->onvif_scopes.improper_input[num_junk],tds__RemoveScopes->ScopeItem[i])==0)
				{
					if(num_improper < 3)
					{
						strcpy(onvif_scopes_t.improper_input_t[num_improper],tds__RemoveScopes->ScopeItem[i]);
						num_improper++;
					}
					else
					{
						onvif_fault(soap,0,"ter:InvalidArgs", "ter:TooManyScopes","Too large scope list");
						return SOAP_FAULT;
					}
				}
				num_junk++;
			}
		}

	}

	ControlSystemData(SFIELD_DEL_SCOPE, &onvif_scopes_t, sizeof(onvif_scopes_t));
	tds__RemoveScopesResponse->__sizeScopeItem = size;
	tds__RemoveScopesResponse->ScopeItem = (char **)soap_malloc(soap,sizeof(char *) * INFO_LENGTH);
	for(i = 0; i < size; i++)
	{
		tds__RemoveScopesResponse->ScopeItem[i] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(tds__RemoveScopesResponse->ScopeItem[i], tds__RemoveScopes->ScopeItem[i]);
	}
	if(pthread_create(&hello_thread, NULL, hello_thr_scope, (void *)server_udp) != 0)
		fprintf(stderr,"Error:thread creation failed");
	return SOAP_OK;
}

/** 
 * @brief Get Discovery Mode of device
 * @This operation gets the discovery mode of a device.
 *   
 * @param Request   :_tds__GetDiscoveryMode structures contains an empty message.
 * @param Response  :_tds__iGetDiscoveryModeResponse members contains the current discovery mode setting, i.e. discoverable or non-discoverable. Discovery    mode is always On.
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults
 */
int __tds__GetDiscoveryMode(struct soap* soap, struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode, struct _tds__GetDiscoveryModeResponse *tds__GetDiscoveryModeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	tds__GetDiscoveryModeResponse->DiscoveryMode =  oSysInfo->lan_config.net.discovery_mode;
	return SOAP_OK;
}


/** 
 * @brief Set Discovery Mode of device
 * @This operation sets the discovery mode operation of a device.
 *   
 * @param Request        :_tds__SetDiscoveryMode contains an empty message.
 * @param Response       :_tds__SetDiscoveryModeResponse contains the current discovery mode setting, i.e. discoverable or non-discoverable.

 * @return               : On success - SOAP_OK.
 On failure - No command specific faults.
 */
int __tds__SetDiscoveryMode(struct soap* soap, struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode, struct _tds__SetDiscoveryModeResponse *tds__SetDiscoveryModeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	__u8  _mode = tds__SetDiscoveryMode->DiscoveryMode;
	if(_mode == 0 || _mode == 1)
	{
		ControlSystemData(SFIELD_DIS_MODE,&_mode, sizeof(_mode));
	}
	else
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidDiscoveryMode","DiscoveryMode should be 1 or 0");
		return SOAP_FAULT;
	}   
	return SOAP_OK;
}


int __tds__GetRemoteDiscoveryMode(struct soap* soap, struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode, struct _tds__GetRemoteDiscoveryModeResponse *tds__GetRemoteDiscoveryModeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetRemoteDiscoveryMode(struct soap* soap, struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode, struct _tds__SetRemoteDiscoveryModeResponse *tds__SetRemoteDiscoveryModeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetDPAddresses(struct soap* soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetEndpointReference(struct soap* soap, struct _tds__GetEndpointReference *tds__GetEndpointReference, struct _tds__GetEndpointReferenceResponse *tds__GetEndpointReferenceResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetRemoteUser(struct soap* soap, struct _tds__GetRemoteUser *tds__GetRemoteUser, struct _tds__GetRemoteUserResponse *tds__GetRemoteUserResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetRemoteUser(struct soap* soap, struct _tds__SetRemoteUser *tds__SetRemoteUser, struct _tds__SetRemoteUserResponse *tds__SetRemoteUserResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/** 
 * @brief Get Users Information
 * @This operation lists the registered users and the following corresponding credentials on a device:
 -User name
 -User level
 *   
 * @param Request	:_tds__GetUsers structures contains an empty message.
 * @param Response	:_tds__GetUsersResponse contains list of users and corresponding credentials.
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults

 */
int __tds__GetUsers(struct soap* soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif


	SysInfo* oSysInfo = GetSysInfo();
	int userIndex = 0;
	int i;
	int NEW_ACOUNT=0;
	for(i = 0; i < ACOUNT_NUM; i++) 
	{
		if (strlen(oSysInfo->acounts[NEW_ACOUNT].user) == 0)
		{
			break;
		}
		NEW_ACOUNT++;
	}
	tds__GetUsersResponse->__sizeUser = NEW_ACOUNT;
	tds__GetUsersResponse->User = (struct tt__User*)soap_malloc(soap, (NEW_ACOUNT * sizeof(struct tt__User)));
	for(i = 0; i < NEW_ACOUNT; i++) 
	{
		tds__GetUsersResponse->User[userIndex].Username = oSysInfo->acounts[userIndex].user;
		tds__GetUsersResponse->User[userIndex].Password = NULL;
		tds__GetUsersResponse->User[userIndex].UserLevel = oSysInfo->acounts[userIndex].authority;
		tds__GetUsersResponse->User[userIndex].Extension = NULL;
		dom_att(&tds__GetUsersResponse->User[userIndex].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		if (strlen(tds__GetUsersResponse->User[userIndex].Username) == 0)
		{
			break;
		}
		userIndex++;
	}
	return SOAP_OK; 
}

/**
 * @brief Create Users on device 
 * @This operation creates new device users and corresponding credentials on a device for the user token profile.
 *   
 * @param Request	:_tds__CreateUsers structures contains the following  user parameters element for a new user:
 -Username
 -Password
 -UserLevel
 * @param Response	:_tds__CreateUsersResponse structure contains an empty message.
 * @return               : On success - SOAP_OK.
 On failure - Appropriate fault code(ONVIF Core Spec. â€?Ver. 2.00 8.4.4)
 */
int __tds__CreateUsers(struct soap* soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse) {

	SysInfo* oSysInfo = GetSysInfo();
	add_user_t account;
	int size = tds__CreateUsers->__sizeUser;
	int i;
	int j;
	int NEW_ACOUNT = 0;

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
	for(i = 0; i < ACOUNT_NUM; i++)
	{
		if(!strcmp(wsse_username, oSysInfo->acounts[i].user))
		{
			if(!(oSysInfo->acounts[i].authority))
				break;
		}
		else
		{
			onvif_fault(soap,1,"ter:NotAdminstrator","ter:FailedAunthentication","Permission denied");
			return SOAP_FAULT;
		}

	}
#endif

	for(i = 0; i < ACOUNT_NUM; i++)
	{
		if (strlen(oSysInfo->acounts[NEW_ACOUNT].user) == 0)
		{
			break;
		}
		NEW_ACOUNT++;
	}

	if(size > (ACOUNT_NUM - NEW_ACOUNT))
	{
		onvif_fault(soap,1,"ter:OperationProhibited","ter:TooManyUsers","Too many users");
		return SOAP_FAULT;
	}

	for(i = 0; i < size; i++)
	{
		if(strlen(tds__CreateUsers->User[i].Username) > USER_LEN)
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:UsernameTooLong","Too long username");
			return SOAP_FAULT;
		}
		if(strlen(tds__CreateUsers->User[i].Password) > PASSWORD_LEN)
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:PasswordTooLong","Too long password");
			return SOAP_FAULT;
		}
		if(tds__CreateUsers->User[i].UserLevel == 0 || tds__CreateUsers->User[i].UserLevel > 4 || tds__CreateUsers->User[i].UserLevel < 0)
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:AnonymousNotAllowed","User level anonymous is not allowed.");
			return SOAP_FAULT;
		}
	}
	/* check username already exist */    
	if (strcmp(tds__CreateUsers->User[0].Username, "admin") != 0)
	{
		for(i = 0; i < size; i++)
		{
			for(j = 0; j < ACOUNT_NUM; j++)
			{

				if (strcmp(tds__CreateUsers->User[i].Username, oSysInfo->acounts[j].user) == 0)
				{
					onvif_fault(soap,1,"ter:OperationProhibited", "ter:UsernameClash","Username already exists");
					return SOAP_FAULT;
				}
			}
		}
	}

	/* Save user */
	for(i = 0; i < size; i++)
	{
		strcpy(account.user_id, tds__CreateUsers->User[i].Username);
		strcpy(account.password, tds__CreateUsers->User[i].Password);
		account.authority = tds__CreateUsers->User[i].UserLevel;
		ControlSystemData(SFIELD_ADD_USER, (void *)&account, sizeof(account));
	}
	return SOAP_OK;
}


/** 
 * @brief Delete User from device
 * @This operation creates new device users and corresponding credentials on a device for the user token profile.
 *   
 * @param Request	: _tds__DeleteUsers structures contains the name of the user or users to be deleted.
 * @param Response	: _tds__DeleteUsersResponse structure contains an empty message.
 * @return               : On success - SOAP_OK.
 On failure - Username NOT recognized. or
 Username may not be deleted
 */
int __tds__DeleteUsers(struct soap* soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse) {

	SysInfo* oSysInfo = GetSysInfo();
	char newuser[SMALL_INFO_LENGTH];
	int size = tds__DeleteUsers->__sizeUsername;
	int i = 0;
	int j = 0;
	int NUM_ACOUNT = 0;

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
	for(i = 0; i < ACOUNT_NUM; i++)
	{
		if(!strcmp(wsse_username, oSysInfo->acounts[i].user))
		{
			if(!(oSysInfo->acounts[i].authority))
				break;
		}
		else
		{
			onvif_fault(soap,1,"ter:NotAdminstrator","ter:FailedAunthentication", "Permission denied");
			return SOAP_FAULT;
		}

	}
#endif


	for(i = 0; i < ACOUNT_NUM; i++)
	{
		if (strlen(oSysInfo->acounts[NUM_ACOUNT].user) == 0)
		{
			break;
		}
		NUM_ACOUNT++;
	}

	/* check username already exist */	
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < NUM_ACOUNT; j++) 
		{
			if (strcmp(tds__DeleteUsers->Username[i],oSysInfo->acounts[j].user) == 0)
			{
				if(oSysInfo->acounts[j].authority == 0)
				{
					onvif_fault(soap,1,"ter:InvalidArgVal", "ter:FixedUser","Removing fixed user");
					return SOAP_FAULT;
				}
				break;
			}
		}

		if(j == NUM_ACOUNT) 
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:UsernameMissing","Username not recognized");
			return SOAP_FAULT;				
		}
	}

	/* Delete user */
	for(i = 0; i < size; i++)
	{
		strcpy(newuser, tds__DeleteUsers->Username[i]);
		if(strlen(newuser) == 0)
		{
			onvif_fault(soap,1, "ter:OperationProhibited", "ter:AnonymousNotAllowed","User level anonymous is not allowed.");
			return SOAP_FAULT;
		}
		ControlSystemData(SFIELD_DEL_USER, (void *)newuser, (strlen(newuser)+1));
	}
	return SOAP_OK; 
}


/** 
 * @brief Set User On device
 * @This operation updates the following settings for one or several users on a device for the user token profile.
 -Username
 -Password
 -Userlevel
 *   
 * @param Request	: _tds__SetUsers structures contains a list of users and corresponding parameters to be updated.
 * @param Response	: _tds__SetUsersResponse structure contains an empty message.
 * @return               : On success - SOAP_OK.
 On failure - Approprite fault code(ONVIF Core Spec. â€?Ver. 2.00 8.4.6).
 */
int __tds__SetUser(struct soap* soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	Acount_t   account;
	int size = tds__SetUser->__sizeUser;
	int i;
	int j;
	int NEW_ACOUNT = 0;
	//Response
	for(i = 0; i < ACOUNT_NUM; i++)
	{
		if (strlen(oSysInfo->acounts[NEW_ACOUNT].user) == 0)
		{
			break;
		}
		NEW_ACOUNT++;
	}

	/* check username already exist */    
	int flag = 0;

	if (strcmp(tds__SetUser->User[0].Username, "admin") != 0)
	{
		for(i = 0; i < size; i++)
		{ 
			flag = 0;
			for(j = 0; j < ACOUNT_NUM; j++)
			{
				if(strcmp(tds__SetUser->User[i].Username,oSysInfo->acounts[j].user) == 0)
				{
					if(tds__SetUser->User[i].UserLevel == 0)
					{
						onvif_fault(soap,1,"ter:InvalidArgVal", "ter:FixedUser","Removing fixed user"); 
						return SOAP_FAULT;
					}
					flag = 1;
				}
			}
			if(flag == 0)
			{
				onvif_fault(soap,1,"ter:InvalidArgVal", "ter:UsernameMissing","Username not recognized");
				return SOAP_FAULT;
			}
		}
	}

	/* Save user */
	if(flag == 0)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:UsernameMissing","Username not recognized");
		return SOAP_FAULT;
	}

	if(size > (ACOUNT_NUM - NEW_ACOUNT))
	{
		onvif_fault(soap,1,"ter:OperationProhibited", "ter:TooManyUsers","Too many users");
		return SOAP_FAULT;
	}

	for(i=0;i<size;i++)
	{
		if(strlen(tds__SetUser->User[i].Username) > USER_LEN)
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:UsernameTooLong","Too long username");
			return SOAP_FAULT;
		}

		if(strlen(tds__SetUser->User[i].Password) > PASSWORD_LEN)
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:PasswordTooLong","Too long password");
			return SOAP_FAULT;
		}
		if(strlen(tds__SetUser->User[i].Password) ==  PASSWORD_ZERO)
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:PasswordTooWeak","Too weak password");
			return SOAP_FAULT;
		}


		if(tds__SetUser->User[i].UserLevel == 0 || tds__SetUser->User[i].UserLevel > 4 || tds__SetUser->User[i].UserLevel < 0)
		{
			onvif_fault(soap,1,"ter:OperationProhibited", "ter:AnonymousNotAllowed","User level anonymous is not allowed.");
			return SOAP_FAULT;
		}
	}

	for(i = 0; i < size; i++)
	{
		strcpy(account.user, tds__SetUser->User[i].Username);
		strcpy(account.password, tds__SetUser->User[i].Password);
		account.authority = tds__SetUser->User[i].UserLevel;
		ControlSystemData(SFIELD_ADD_USER, (void *)&account, sizeof(account));
	}
	return SOAP_OK;
}

/**
 * @brief Get wsl Url 
 * @This operation request a URL that can be used to retrieve the complete schema and WSDL definitions of a device.
 *   
 * @param Request       : _tds__GetWsdlUrl structures contains an empty message.
 * @param Response      : _tds__GetWsdlUrlResponse structure contains the requested URL.
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults
 */
int __tds__GetWsdlUrl(struct soap* soap, struct _tds__GetWsdlUrl *tds__GetWsdlUrl, struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	tds__GetWsdlUrlResponse->WsdlUrl = "http://www.onvif.org/Documents/Specifications.aspx";
	return SOAP_OK;
}

/**
 * @brief Get Capabilities of device 
 * @This message contains a request for device capabilities.
 *   
 * @param Request       : _tds__GetCapabilities structures contains a request for device capabilities. 
 The client can either ask for all capabilities or just the capabilities for a particular service category.
 All = 0, Analytics = 1, Device = 2, Events = 3, Imaging = 4, Media = 5, PTZ = 6
 * @param Response      : _tds__GetCapabilitiesResponse contains the requested device capabilities using a hierarchical XML capability structure.
 * @return               : On success - SOAP_OK.
 On failure - The requested WSDL service category is not supported by the device.
 */
int __tds__GetCapabilities(struct soap* soap, struct _tds__GetCapabilities *tds__GetCapabilities, struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
	}
#endif
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	int _Category;
	char _IPv4Address[LARGE_INFO_LENGTH];

	if(tds__GetCapabilities->Category == NULL)
	{
		_Category = ALL;
	}
	else
	{
		_Category = *tds__GetCapabilities->Category;
	}
	sprintf(_IPv4Address, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	tds__GetCapabilitiesResponse->Capabilities = (struct tt__Capabilities*)soap_malloc(soap, sizeof(struct tt__Capabilities));
	tds__GetCapabilitiesResponse->Capabilities->Device = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Analytics= NULL;
	tds__GetCapabilitiesResponse->Capabilities->Imaging= NULL;
	tds__GetCapabilitiesResponse->Capabilities->Events = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Media= NULL;
	tds__GetCapabilitiesResponse->Capabilities->PTZ= NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension = (struct tt__CapabilitiesExtension*)soap_malloc(soap, sizeof(struct tt__CapabilitiesExtension));
	tds__GetCapabilitiesResponse->Capabilities->Extension->__size = 0;
	tds__GetCapabilitiesResponse->Capabilities->Extension->__any = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO = (struct tt__DeviceIOCapabilities*)soap_malloc(soap, sizeof(struct tt__DeviceIOCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Extension->Display = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->Recording = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->Search = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->Replay = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->Receiver = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->AnalyticsDevice = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->Extensions = NULL;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->XAddr = (char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	strcpy(tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->XAddr, _IPv4Address);
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->VideoSources = 0;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->VideoOutputs = 0;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->AudioSources = 0;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->AudioOutputs = 0;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->RelayOutputs = 1;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->__size = 0;
	tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->__any = NULL;
	dom_att(&tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->__anyAttribute, 0, NULL,NULL,NULL,NULL,soap);
	dom_att(&tds__GetCapabilitiesResponse->Capabilities->__anyAttribute, 0, NULL,NULL,NULL,NULL,soap);
	if((_Category == ALL) || (_Category == EVENTS))
	{
		tds__GetCapabilitiesResponse->Capabilities->Events = (struct tt__EventCapabilities*)soap_malloc(soap, sizeof(struct tt__EventCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Events->XAddr = (char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
		strcpy(tds__GetCapabilitiesResponse->Capabilities->Events->XAddr, _IPv4Address);
		tds__GetCapabilitiesResponse->Capabilities->Events->WSSubscriptionPolicySupport = xsd__boolean__true_; 
		tds__GetCapabilitiesResponse->Capabilities->Events->WSPullPointSupport = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Events->WSPausableSubscriptionManagerInterfaceSupport = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Events->__size = 0;
		tds__GetCapabilitiesResponse->Capabilities->Events->__any = NULL;	
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Events->__anyAttribute, 0, NULL,NULL,NULL,NULL,soap);
	}
	if((_Category == ALL) || (_Category == DEVICE))
	{
		tds__GetCapabilitiesResponse->Capabilities->Device = (struct tt__DeviceCapabilities*)soap_malloc(soap, sizeof(struct tt__DeviceCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Device->XAddr = (char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
		strcpy(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr, _IPv4Address);	
		tds__GetCapabilitiesResponse->Capabilities->Device->Extension = NULL;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network = (struct tt__NetworkCapabilities*)soap_malloc(soap, sizeof(struct tt__NetworkCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration = soap_malloc(soap, sizeof(enum xsd__boolean)); 
		*tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6 = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6 = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS = xsd__boolean__false_;	
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension = (struct tt__NetworkCapabilitiesExtension*)soap_malloc(soap, sizeof(struct tt__NetworkCapabilitiesExtension));
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->__size = 0;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->__any = NULL;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->Dot11Configuration = soap_malloc(soap, sizeof(enum xsd__boolean)); 
		*tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->Dot11Configuration = xsd__boolean__false_ ; 
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->Extension = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Device->Network->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		tds__GetCapabilitiesResponse->Capabilities->Device->System = (struct tt__SystemCapabilities*)soap_malloc(soap, sizeof(struct tt__SystemCapabilities));		
		tds__GetCapabilitiesResponse->Capabilities->Device->System->DiscoveryResolve = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->DiscoveryBye = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->RemoteDiscovery = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SystemBackup = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SystemLogging = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->FirmwareUpgrade = xsd__boolean__false_;	
		tds__GetCapabilitiesResponse->Capabilities->Device->System->__sizeSupportedVersions = 2;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions = (struct tt__OnvifVersion*)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions->Major = MAJOR;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions->Minor = MINOR;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension = (struct tt__SystemCapabilitiesExtension*)soap_malloc(soap, sizeof(struct tt__SystemCapabilitiesExtension));
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->__any = NULL;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpFirmwareUpgrade = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpFirmwareUpgrade = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemBackup = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemBackup = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemLogging = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemLogging = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSupportInformation = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSupportInformation = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->Extension = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Device->System->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		tds__GetCapabilitiesResponse->Capabilities->Device->IO = (struct tt__IOCapabilities*)soap_malloc(soap, sizeof(struct tt__IOCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Device->IO->InputConnectors = (int *)soap_malloc(soap, sizeof(int));
		*tds__GetCapabilitiesResponse->Capabilities->Device->IO->InputConnectors = 0;
		tds__GetCapabilitiesResponse->Capabilities->Device->IO->RelayOutputs = (int *)soap_malloc(soap, sizeof(int));
		*tds__GetCapabilitiesResponse->Capabilities->Device->IO->RelayOutputs = 1;
		tds__GetCapabilitiesResponse->Capabilities->Device->IO->Extension = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Device->IO->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		tds__GetCapabilitiesResponse->Capabilities->Device->Security = (struct tt__SecurityCapabilities*)soap_malloc(soap, sizeof(struct tt__SecurityCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->TLS1_x002e1 = xsd__boolean__false_;	
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->TLS1_x002e2 = xsd__boolean__false_;	
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->OnboardKeyGeneration = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->AccessPolicyConfig = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->X_x002e509Token = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->SAMLToken = xsd__boolean__false_;	
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->KerberosToken = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->RELToken = xsd__boolean__false_;	
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->__any = NULL;
		tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Device->Security->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		tds__GetCapabilitiesResponse->Capabilities->Device->Extension = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Device->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	}

	if ((_Category == ALL) || (_Category == IMAGING)) 
	{
		tds__GetCapabilitiesResponse->Capabilities->Imaging = (struct tt__ImagingCapabilities*)soap_malloc(soap, sizeof(struct tt__ImagingCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr =(char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH); 
		strcpy(tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr,_IPv4Address);
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Imaging->__anyAttribute,0, NULL, NULL, NULL, NULL, soap);

	}


	if((_Category == ALL) || (_Category == MEDIA))
	{
		tds__GetCapabilitiesResponse->Capabilities->Media = (struct tt__MediaCapabilities*)soap_malloc(soap, sizeof(struct tt__MediaCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Media->XAddr = (char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
		strcpy(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, _IPv4Address);
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities = (struct tt__RealTimeStreamingCapabilities*)soap_malloc(soap, sizeof(struct tt__RealTimeStreamingCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast = xsd__boolean__true_ ;
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->Extension = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		tds__GetCapabilitiesResponse->Capabilities->Media->__size = 0;
		tds__GetCapabilitiesResponse->Capabilities->Media->__any = NULL;
		tds__GetCapabilitiesResponse->Capabilities->Media->Extension = (struct tt__MediaCapabilitiesExtension*)soap_malloc(soap, sizeof(struct tt__MediaCapabilitiesExtension));
		tds__GetCapabilitiesResponse->Capabilities->Media->Extension->ProfileCapabilities = (struct tt__ProfileCapabilities*)soap_malloc(soap, sizeof(struct tt__ProfileCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Media->Extension->ProfileCapabilities->MaximumNumberOfProfiles = MAX_PROF;//4;      //<<11MAR2013>>
		tds__GetCapabilitiesResponse->Capabilities->Media->Extension->ProfileCapabilities->__size = 0;
		tds__GetCapabilitiesResponse->Capabilities->Media->Extension->ProfileCapabilities->__any = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Media->Extension->ProfileCapabilities->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		tds__GetCapabilitiesResponse->Capabilities->Media->Extension->__size = 0;
		tds__GetCapabilitiesResponse->Capabilities->Media->Extension->__any = NULL;
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Media->Extension->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		dom_att(&tds__GetCapabilitiesResponse->Capabilities->Media->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	}

	if(_Category == PANTILTZOOM) 
	{
		tds__GetCapabilitiesResponse->Capabilities->PTZ = NULL;
		onvif_fault( soap,0,"ter:ActionNotSupported","ter:NoSuchService","The service is not supported" );
		return SOAP_FAULT;
	}

	if (_Category == ANALYTICS) 
	{	
		tds__GetCapabilitiesResponse->Capabilities->Analytics = NULL;
		onvif_fault( soap,0,"ter:ActionNotSupported","ter:NoSuchService","The service is not supported" );
		return SOAP_FAULT;
	}

	return SOAP_OK;
}

int __tds__SetDPAddresses(struct soap* soap, struct _tds__SetDPAddresses *tds__SetDPAddresses, struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetHostname(struct soap* soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	tds__GetHostnameResponse->HostnameInformation = (struct tt__HostnameInformation *)soap_malloc(soap, sizeof(struct tt__HostnameInformation));
	tds__GetHostnameResponse->HostnameInformation->FromDHCP = 0;
	tds__GetHostnameResponse->HostnameInformation->Name = (char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	tds__GetHostnameResponse->HostnameInformation->Extension = NULL;
	strcpy( tds__GetHostnameResponse->HostnameInformation->Name, oSysInfo->lan_config.hostname );

	dom_att( &tds__GetHostnameResponse->HostnameInformation->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	return SOAP_OK;


}


int __tds__SetHostname(struct soap* soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	int ret;
	hostname_tmp hostname_tm;

	strcpy(hostname_tm.hostname_t, tds__SetHostname->Name);
	ret = checkhostname(hostname_tm.hostname_t);
	if(ret != 0)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidHostname", "Invalid name");
		return SOAP_FAULT;
	}
	ControlSystemData(SFIELD_SET_HOSTNAME, (void *)&hostname_tm, sizeof(hostname_tm));

	return SOAP_OK;
}


int __tds__SetHostnameFromDHCP(struct soap* soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	return SOAP_OK;
}


int __tds__GetDNS(struct soap* soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	struct sockaddr_in dns_ip;
	char _dns[LARGE_INFO_LENGTH];
	SysInfo* oSysInfo = GetSysInfo();
	dns_ip.sin_addr.s_addr = oSysInfo->lan_config.net.dns.s_addr;
	strcpy(_dns, inet_ntoa(dns_ip.sin_addr));

	tds__GetDNSResponse->DNSInformation = (struct tt__DNSInformation *)soap_malloc(soap, sizeof(struct tt__DNSInformation));
	tds__GetDNSResponse->DNSInformation->FromDHCP = oSysInfo->lan_config.net.dhcp_enable;
	tds__GetDNSResponse->DNSInformation->__sizeSearchDomain = 1;
	tds__GetDNSResponse->DNSInformation->SearchDomain = (char **)soap_malloc(soap, sizeof(char *));
	*tds__GetDNSResponse->DNSInformation->SearchDomain = domainname;
	tds__GetDNSResponse->DNSInformation->__sizeDNSFromDHCP = 0;
	tds__GetDNSResponse->DNSInformation->__sizeDNSManual = 0;
	tds__GetDNSResponse->DNSInformation->Extension = NULL;

	if((tds__GetDNSResponse->DNSInformation->FromDHCP) == 1)
	{
		tds__GetDNSResponse->DNSInformation->__sizeDNSFromDHCP = 1;
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP->Type = 0;
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
		strcpy(tds__GetDNSResponse->DNSInformation->DNSFromDHCP->IPv4Address, _dns);
		tds__GetDNSResponse->DNSInformation->DNSFromDHCP->IPv6Address = NULL;
	}
	else
	{
		tds__GetDNSResponse->DNSInformation->__sizeDNSManual = 1;
		tds__GetDNSResponse->DNSInformation->DNSManual = ((struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress)));
		tds__GetDNSResponse->DNSInformation->DNSManual->Type = 0;
		tds__GetDNSResponse->DNSInformation->DNSManual->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
		strcpy(tds__GetDNSResponse->DNSInformation->DNSManual->IPv4Address, _dns);
		tds__GetDNSResponse->DNSInformation->DNSManual->IPv6Address = NULL;
	}

	dom_att( &tds__GetDNSResponse->DNSInformation->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	return SOAP_OK;

}


int __tds__SetDNS(struct soap* soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	struct sockaddr_in dns_ip;
	SysInfo* oSysInfo = GetSysInfo();
	int sys_dhcp = oSysInfo->lan_config.net.dhcp_enable;
	IsIPchange=0;
	IsDHCPchange=0;
	int value = tds__SetDNS->FromDHCP;
	char _IPv4Address[SMALL_INFO_LENGTH];

	if(tds__SetDNS->SearchDomain)
	{
		strcpy(domainname, *(tds__SetDNS->SearchDomain));
	}
	if (sys_dhcp != value)
	{
		IsDHCPchange=1;
		set_DHCP_value=value;
		if(value == 1)
		{
			return SOAP_OK;
		}
	}
	if(tds__SetDNS->__sizeSearchDomain)
	{
		tempdnssearchdomainsize = tds__SetDNS->__sizeSearchDomain;
	}
	else
	{
		tempdnssearchdomainsize = 0;
	}

	if(tds__SetDNS->DNSManual == NULL)
	{
		return SOAP_OK;
	}

	if(tds__SetDNS->DNSManual->Type != 0)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal","ter:IPv6AddressNotAllowed","The service is not supported");
		return SOAP_FAULT;
	}

	strcpy(_IPv4Address, tds__SetDNS->DNSManual->IPv4Address);
	if(isValidIp4(_IPv4Address) == 0)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidIPv4Address","Invalid address");
		return SOAP_FAULT;
	}
	inet_aton(_IPv4Address, &dns_ip.sin_addr);

	ControlSystemData(SFIELD_SET_DNS, (void *)&dns_ip.sin_addr.s_addr, sizeof(dns_ip.sin_addr.s_addr));

	return SOAP_OK;

}


int __tds__GetNTP(struct soap* soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	char _NTPhostname[MID_INFO_LENGTH];
	struct hostent *host;
	struct in_addr ip;
	char *_IPv4Address;

	int dhcp_status = oSysInfo->lan_config.net.dhcp_enable;
	ControlSystemData(SFIELD_GET_SNTP_FQDN, (void *)_NTPhostname, sizeof(_NTPhostname));

	host = gethostbyname(_NTPhostname);
	if(host!= NULL)
	{
		memcpy((char *)&ip, host->h_addr_list[0], sizeof(ip));
		_IPv4Address = inet_ntoa(ip);
	} else {
		_IPv4Address = "127.0.0.1";
	}

	tds__GetNTPResponse->NTPInformation = (struct tt__NTPInformation*)soap_malloc(soap, sizeof(struct tt__NTPInformation));
	tds__GetNTPResponse->NTPInformation->FromDHCP = dhcp_status;

	if(dhcp_status == 1) //FromDHCP
	{
		tds__GetNTPResponse->NTPInformation->__sizeNTPFromDHCP = 1;
		tds__GetNTPResponse->NTPInformation->NTPFromDHCP = (struct tt__NetworkHost*)soap_malloc(soap, sizeof(struct tt__NetworkHost));
		tds__GetNTPResponse->NTPInformation->NTPFromDHCP->Type = IPV4_NTP;

		tds__GetNTPResponse->NTPInformation->NTPFromDHCP->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(tds__GetNTPResponse->NTPInformation->NTPFromDHCP->IPv4Address, _IPv4Address);

		tds__GetNTPResponse->NTPInformation->NTPFromDHCP->IPv6Address = NULL;
		tds__GetNTPResponse->NTPInformation->NTPFromDHCP->DNSname = NULL;
		tds__GetNTPResponse->NTPInformation->NTPFromDHCP->Extension = NULL;
		dom_att(&tds__GetNTPResponse->NTPInformation->NTPFromDHCP->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		tds__GetNTPResponse->NTPInformation->__sizeNTPManual = 0;
		tds__GetNTPResponse->NTPInformation->NTPManual = NULL;

	}
	else // NTPManual || Static
	{

		tds__GetNTPResponse->NTPInformation->__sizeNTPManual = 1;
		tds__GetNTPResponse->NTPInformation->NTPManual = ((struct tt__NetworkHost *)soap_malloc(soap, sizeof(struct tt__NetworkHost)));
		tds__GetNTPResponse->NTPInformation->NTPManual->Type = IPV4_NTP;

		tds__GetNTPResponse->NTPInformation->NTPManual->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(tds__GetNTPResponse->NTPInformation->NTPManual->IPv4Address, _IPv4Address);

		tds__GetNTPResponse->NTPInformation->NTPManual->IPv6Address = NULL;
		tds__GetNTPResponse->NTPInformation->NTPManual->DNSname = NULL;
		tds__GetNTPResponse->NTPInformation->NTPManual->Extension = NULL;
		dom_att( &tds__GetNTPResponse->NTPInformation->NTPManual->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		tds__GetNTPResponse->NTPInformation->__sizeNTPFromDHCP = 0;
		tds__GetNTPResponse->NTPInformation->NTPFromDHCP = NULL;


	}

	tds__GetNTPResponse->NTPInformation->Extension = NULL;
	dom_att( &tds__GetNTPResponse->NTPInformation->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK; 

}


int __tds__SetNTP(struct soap* soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	struct sockaddr_in sa;
	char host[LARGE_INFO_LENGTH];
	char service[SMALL_INFO_LENGTH];
	int s;
	int value = tds__SetNTP->FromDHCP;

	if (value == TRUE)
	{
		onvif_fault(soap,1,"ter:NotSupported", "ter:SetDHCPNotAllowed","The service is not supported");
		return SOAP_FAULT;
	}
	if (tds__SetNTP->NTPManual->IPv6Address != NULL)
	{
		onvif_fault(soap,1,"ter:NotSupported", "ter:IPv6AddressNotAllowed","The service is not supported");
		return SOAP_FAULT;
	}
	if(tds__SetNTP->NTPManual->IPv4Address != NULL)
	{
		if(isValidIp4(tds__SetNTP->NTPManual->IPv4Address) == 0)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidIPv4Address","Invalid address");
			return SOAP_FAULT;
		}

		sa.sin_family = AF_INET;
		inet_aton(tds__SetNTP->NTPManual->IPv4Address, &sa.sin_addr.s_addr);
		s = getnameinfo((struct sockaddr *)&sa, sizeof(sa), host, sizeof(host), service, sizeof(service), NI_NAMEREQD);
		if(s) {

			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidDnsName","Invalid name");
			return SOAP_FAULT;
		}

		if(isValidHostname(host) == 0)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidDnsName","Invalid name");
			return SOAP_FAULT;
		}
		if(tds__SetNTP->NTPManual)
		{
			ControlSystemData(SFIELD_SET_SNTP_SERVER, (void *)host, strlen(host));
		}
	}
	return SOAP_OK;
}

int __tds__GetDynamicDNS(struct soap* soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetDynamicDNS(struct soap* soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetNetworkInterfaces(struct soap* soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	unsigned char mac[MACH_ADDR_LENGTH];
	char _mac[LARGE_INFO_LENGTH];
	char _IPAddr[LARGE_INFO_LENGTH];
	char _IPAddr_local[LARGE_INFO_LENGTH];
	NET_IPV4 ip;
	NET_IPV4 ip_local;
	int MTU = 1500;
	SysInfo* oSysInfo = GetSysInfo();
	int local_dhcp_status = oSysInfo->lan_config.net.dhcp_enable;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	ip_local.int32 = net_get_ifaddr(ETH_NAME_LOCAL);
	net_get_hwaddr(ETH_NAME, mac);
	sprintf(_mac, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	sprintf(_IPAddr_local, "%d.%d.%d.%d", ip_local.str[0], ip_local.str[1], ip_local.str[2], ip_local.str[3]);
	tds__GetNetworkInterfacesResponse->__sizeNetworkInterfaces = 1;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces = (struct tt__NetworkInterface *)soap_malloc(soap, sizeof(struct tt__NetworkInterface));
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->token =  (char *)soap_malloc(soap, SMALL_INFO_LENGTH);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->token,ETH_NAME);
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Enabled = TRUE;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info = (struct tt__NetworkInterfaceInfo *)soap_malloc(soap, sizeof(struct tt__NetworkInterfaceInfo));
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->Name = (char *)soap_malloc(soap, SMALL_INFO_LENGTH);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->Name,ETH_NAME);
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress = (char *)soap_malloc(soap, LARGE_INFO_LENGTH);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress,_mac);
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->MTU = &MTU;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link = NULL;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4 = (struct tt__IPv4NetworkInterface *)soap_malloc(soap, sizeof(struct tt__IPv4NetworkInterface));
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Enabled = TRUE;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config = (struct tt__IPv4Configuration *)soap_malloc(soap, sizeof(struct tt__IPv4Configuration));
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__sizeManual = 1;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual = (struct tt__PrefixedIPv4Address *)soap_malloc(soap, sizeof(struct tt__PrefixedIPv4Address));
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address,_IPAddr);
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->PrefixLength=24;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->LinkLocal = (struct tt__PrefixedIPv4Address *)soap_malloc(soap, sizeof(struct tt__PrefixedIPv4Address));
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->LinkLocal->Address = (char *)soap_malloc(soap, LARGE_INFO_LENGTH);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->LinkLocal->Address,_IPAddr_local);
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->LinkLocal->PrefixLength = 24;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP = (struct tt__PrefixedIPv4Address *)soap_malloc(soap, sizeof(struct tt__PrefixedIPv4Address));
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP->Address = (char *)soap_malloc(soap, LARGE_INFO_LENGTH);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP->Address, _IPAddr);
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP->PrefixLength = 24;

	if(local_dhcp_status)
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->DHCP = TRUE;
	else
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->DHCP = FALSE;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__size = 0;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__any = NULL;
	dom_att(&tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__anyAttribute,  0, NULL, NULL, NULL, NULL, soap);


	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv6 = NULL;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Extension = NULL;

	dom_att(&tds__GetNetworkInterfacesResponse->NetworkInterfaces->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	return SOAP_OK;
}


int __tds__SetNetworkInterfaces(struct soap* soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	struct in_addr ipaddr, sys_ip;
	int ret;
	SysInfo* oSysInfo = GetSysInfo(); 
	int local_dhcp_status = oSysInfo->lan_config.net.dhcp_enable;

	if(tds__SetNetworkInterfaces->InterfaceToken != NULL)
	{
		if(strcmp(tds__SetNetworkInterfaces->InterfaceToken, ETH_NAME))
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidNetworkInterface","Invalid token");
			return SOAP_FAULT;
		}
	}

	if(tds__SetNetworkInterfaces->NetworkInterface->MTU != NULL)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingMTUNotSupported","The service is not supported");
		return SOAP_FAULT;
	}
	if(tds__SetNetworkInterfaces->NetworkInterface->Link != NULL)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingLinkValuesNotSupported","The service is not supported");
		return SOAP_FAULT;
	}
	if(tds__SetNetworkInterfaces->NetworkInterface->IPv6 != NULL)
	{
		if(tds__SetNetworkInterfaces->NetworkInterface->IPv6->Enabled == status_true)
		{	
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:IPv6NotSupported","The service is not supported");
			return SOAP_FAULT;
		}
	}	
	if(tds__SetNetworkInterfaces->NetworkInterface->IPv4 == NULL)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:IPv4ValuesMissing","invalid address");
		return SOAP_FAULT;
	}
	if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Enabled != NULL)
	{

		tds__SetNetworkInterfacesResponse->RebootNeeded = FALSE;
		IsDHCPchange = 0;
		IsIPchange = 0;
		if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->DHCP != NULL)
		{
			if(*(tds__SetNetworkInterfaces->NetworkInterface->IPv4->DHCP)) 
				set_DHCP_value = 1;
			else
				set_DHCP_value = 0;

			if(local_dhcp_status != set_DHCP_value) {
				IsDHCPchange = 0;
				onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported","dhcp status dismatch");
				return SOAP_FAULT;
			}
		}

		if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual != NULL)
		{
			if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual->Address != NULL)
			{
				if(isValidIp4(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual->Address) == 0) // Check IP address
				{
					onvif_fault(soap,1, "ter:InvalidArgVal", "ter:InvalidIPv4Address","invalid address");
					return SOAP_FAULT;
				}
				if (ipv4_str_to_num(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual->Address, &ipaddr) == 0)
				{
					return SOAP_FAULT;
				}
			}
			if(tds__SetNetworkInterfaces->InterfaceToken != NULL)
			{
				if ((sys_ip.s_addr = net_get_ifaddr(tds__SetNetworkInterfaces->InterfaceToken)) == -1)
				{
					return SOAP_FAULT;
				}
				if (sys_ip.s_addr != ipaddr.s_addr)
				{
					IsIPchange = 1;
					tds__SetNetworkInterfacesResponse->RebootNeeded = TRUE;
					ret = ControlSystemData(SFIELD_SET_IP, (void *)&ipaddr.s_addr, sizeof(ipaddr.s_addr));
				}

			}
		}       

		if(IsDHCPchange)
		{
			if(pthread_create(&set_ip_thread, NULL, set_ip_thr,ipaddr.s_addr) != 0)
				ONVIF_DBG("Error: set ip thread creation failed");
		}  
	}

	return SOAP_OK;
}


int __tds__GetNetworkProtocols(struct soap* soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	int _http_port = 80;
	int Response, port_no = 0;
	SysInfo* oSysInfo = GetSysInfo();
	tds__GetNetworkProtocolsResponse->__sizeNetworkProtocols = 2;

	tds__GetNetworkProtocolsResponse->NetworkProtocols = (struct tt__NetworkProtocol *)soap_malloc(soap,tds__GetNetworkProtocolsResponse->__sizeNetworkProtocols * sizeof(struct tt__NetworkProtocol));

	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Name = HTTP;      // HTTP = 0, HTTPS = 1, RTSP = 2
	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Enabled = 1;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].__sizePort = 1;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Port = (int *)soap_malloc(soap, sizeof(int));
	*tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Port = _http_port;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Extension = NULL;

	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Enabled = oSysInfo->RtspServerPort.RTSP_Enable;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Name = RTSP;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].__sizePort = oSysInfo->RtspServerPort.size_Port;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Extension = NULL;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Port = (int *) soap_malloc(soap, (sizeof(int)*oSysInfo->RtspServerPort.size_Port));
	oSysInfo->RtspServerPort.RTSP_Enable = 1;

	for(Response = 0; Response < oSysInfo->RtspServerPort.size_Port  ; Response++)
	{
		if(oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[Response] == 0)
			continue;
		else {

			tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Port[port_no] = oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[Response];
			port_no++;
		}
	}


	dom_att( &tds__GetNetworkProtocolsResponse->NetworkProtocols[0].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	dom_att( &tds__GetNetworkProtocolsResponse->NetworkProtocols[1].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	return SOAP_OK;
}


int __tds__SetNetworkProtocols(struct soap* soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	int i,j;
	SysInfo* oSysInfo = GetSysInfo();
	int RTSP_enable = -1;
	int port_no, RTSP_enable_port;
	unsigned int RTSP_default_port[5] = {8557,8556,8554,8553,8555};
	int do_not_change = 0;
	int Next_Port_No = -1;
	int default_RTSP;
	int IsRestart_AV_Server = 0;

	for(i = 0;i < tds__SetNetworkProtocols->__sizeNetworkProtocols; i++)
	{ 
		if((tds__SetNetworkProtocols->NetworkProtocols[i].Name)== HTTP)
		{ 
			if(tds__SetNetworkProtocols->NetworkProtocols[i].__sizePort!= 1 && tds__SetNetworkProtocols->NetworkProtocols[i].Port[0] != 80)
			{
				onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported","The service is not supported");
				return SOAP_FAULT;
			}
		}

		if((tds__SetNetworkProtocols->NetworkProtocols[i].Name)== RTSP)
		{       
			RTSP_enable = i;


			if(tds__SetNetworkProtocols->NetworkProtocols[i].Enabled == 1 )
			{     

				oSysInfo->RtspServerPort.RTSP_Enable = 1;	

				for(j = 0; j < tds__SetNetworkProtocols->NetworkProtocols[i].__sizePort; j++)
				{       


					for(port_no = 0; port_no < oSysInfo->RtspServerPort.size_Port; port_no++)
					{
						if(tds__SetNetworkProtocols->NetworkProtocols[i].Port[j] == oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[port_no]) {
							do_not_change = 1;
							break;
						}
					}


					if(!do_not_change)
					{       
						IsRestart_AV_Server = 1;

						for(port_no = 0; port_no < 5; port_no++)
						{
							if( (oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[port_no]) == 0 || ( oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[port_no] == RTSP_default_port[port_no]) )
							{
								Next_Port_No = port_no;
								break;
							}
						}



						if(Next_Port_No!= -1)	
							oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[Next_Port_No] = tds__SetNetworkProtocols->NetworkProtocols[i].Port[j];

						else
							oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[j] = tds__SetNetworkProtocols->NetworkProtocols[i].Port[j];

						if(oSysInfo->RtspServerPort.size_Port < 5)
							oSysInfo->RtspServerPort.size_Port++;					

					}

					Next_Port_No = -1;
					do_not_change = 0;
				}        

			} 

			if(tds__SetNetworkProtocols->NetworkProtocols[i].Enabled == 0)
			{
				RTSP_enable_port = oSysInfo->RtspServerPort.size_Port;

				for(j = 0; j < tds__SetNetworkProtocols->NetworkProtocols[i].__sizePort; j++)
				{   

					for( port_no = 0; port_no < RTSP_enable_port; port_no++)
					{

						if(tds__SetNetworkProtocols->NetworkProtocols[i].Port[j] == RTSP_default_port[port_no] )
						{
							for(default_RTSP = 0 ; default_RTSP < RTSP_enable_port ; default_RTSP++)
							{
								if(RTSP_default_port[default_RTSP] == oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[port_no])
								{
									oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[port_no] = 0;
									oSysInfo->RtspServerPort.size_Port--;
									oSysInfo->RtspServerPort.RTSP_Enable = 0;
									IsRestart_AV_Server = 1;
									break;
								}
							}

						}

						else if( tds__SetNetworkProtocols->NetworkProtocols[i].Port[j] == oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[port_no] )
						{
							oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[port_no] = RTSP_default_port[port_no];
							oSysInfo->RtspServerPort.RTSP_Enable = 0;
							IsRestart_AV_Server = 1;

						}      

					}	
				}	

			}

		}


		if((tds__SetNetworkProtocols->NetworkProtocols[i].Name) == HTTPS )
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported","The service is not supported");
			return SOAP_FAULT;
		}

	}

	if(IsRestart_AV_Server)
		oSysInfo->RtspServerPort.change = 1;

	return SOAP_OK;  
}


int __tds__GetNetworkDefaultGateway(struct soap* soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	struct sockaddr_in gateway_ip;
	SysInfo* oSysInfo = GetSysInfo();
	char _GatewayAddress[LARGE_INFO_LENGTH];
	gateway_ip.sin_addr.s_addr = oSysInfo->lan_config.net.gateway.s_addr;
	strcpy(_GatewayAddress, inet_ntoa(gateway_ip.sin_addr));
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway = ((struct tt__NetworkGateway *)soap_malloc(soap,sizeof(struct tt__NetworkGateway*)));
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->__sizeIPv4Address = 1;
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->__sizeIPv6Address = 0;
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address[0] = (char *)soap_malloc(soap, INFO_LENGTH);
	strcpy(*tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address, _GatewayAddress);
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv6Address = NULL;
	return SOAP_OK;
}


int __tds__SetNetworkDefaultGateway(struct soap* soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	struct in_addr ipaddr;
	char _IPv4Address[LARGE_INFO_LENGTH];

	if(tds__SetNetworkDefaultGateway->__sizeIPv6Address)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:IPv6NotSupported","The service is not supported");
		return SOAP_FAULT;
	}
	if(!tds__SetNetworkDefaultGateway->__sizeIPv4Address)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidGatewayAddress","Invalid address");
		return SOAP_FAULT;
	}
	strcpy(_IPv4Address, *tds__SetNetworkDefaultGateway->IPv4Address);
	if(isValidIp4(_IPv4Address) == 0) // Check IP address
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:InvalidGatewayAddress","Invalid address");
		return SOAP_FAULT;
	}
	ipv4_str_to_num(_IPv4Address, &ipaddr);
	ControlSystemData(SFIELD_SET_GATEWAY, (void *)&ipaddr.s_addr, sizeof(ipaddr.s_addr));

	return SOAP_OK;
}


int __tds__GetZeroConfiguration(struct soap* soap, struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	char _IPAddr[LARGE_INFO_LENGTH];
	NET_IPV4 ip;

	ip.int32 = net_get_ifaddr(ETH_NAME_LOCAL);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

	tds__GetZeroConfigurationResponse->ZeroConfiguration = (struct tt__NetworkZeroConfiguration *)soap_malloc(soap, sizeof(struct tt__NetworkZeroConfiguration));

	tds__GetZeroConfigurationResponse->ZeroConfiguration->InterfaceToken = ETH_NAME;
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Enabled = 1;
	tds__GetZeroConfigurationResponse->ZeroConfiguration->__sizeAddresses = 1;
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses = (char **)soap_malloc(soap, sizeof(char *));
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[1] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[0], _IPAddr);
	strcpy(tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[1], LINK_LOCAL_ADDR);
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Extension = NULL;

	dom_att( &tds__GetZeroConfigurationResponse->ZeroConfiguration->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;

}


int __tds__SetZeroConfiguration(struct soap* soap, struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(tds__SetZeroConfiguration->InterfaceToken);
	if(ip.int32 == -1)
	{
		onvif_fault( soap,1,"ter:InvalidArgsVal", "ter:InvalidNetworkInterface","Invalid token" );
		return SOAP_FAULT;
	}
	return SOAP_OK;
}


int __tds__GetIPAddressFilter(struct soap* soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetIPAddressFilter(struct soap* soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__AddIPAddressFilter(struct soap* soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__RemoveIPAddressFilter(struct soap* soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetAccessPolicy(struct soap* soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetAccessPolicy(struct soap* soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__CreateCertificate(struct soap* soap, struct _tds__CreateCertificate *tds__CreateCertificate, struct _tds__CreateCertificateResponse *tds__CreateCertificateResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetCertificates(struct soap* soap, struct _tds__GetCertificates *tds__GetCertificates, struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetCertificatesStatus(struct soap* soap, struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus, struct _tds__GetCertificatesStatusResponse *tds__GetCertificatesStatusResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetCertificatesStatus(struct soap* soap, struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus, struct _tds__SetCertificatesStatusResponse *tds__SetCertificatesStatusResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__DeleteCertificates(struct soap* soap, struct _tds__DeleteCertificates *tds__DeleteCertificates, struct _tds__DeleteCertificatesResponse *tds__DeleteCertificatesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetPkcs10Request(struct soap* soap, struct _tds__GetPkcs10Request *tds__GetPkcs10Request, struct _tds__GetPkcs10RequestResponse *tds__GetPkcs10RequestResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__LoadCertificates(struct soap* soap, struct _tds__LoadCertificates *tds__LoadCertificates, struct _tds__LoadCertificatesResponse *tds__LoadCertificatesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetClientCertificateMode(struct soap* soap, struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode, struct _tds__GetClientCertificateModeResponse *tds__GetClientCertificateModeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetClientCertificateMode(struct soap* soap, struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode, struct _tds__SetClientCertificateModeResponse *tds__SetClientCertificateModeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i, duration;

	tds__GetRelayOutputsResponse->__sizeRelayOutputs = oSysInfo->nrelays;
	tds__GetRelayOutputsResponse->RelayOutputs = (struct tt__RelayOutput *)soap_malloc(soap, sizeof(struct tt__RelayOutput) * oSysInfo->nrelays);
	for(i = 0; i < oSysInfo->nrelays; i++)
	{
		tds__GetRelayOutputsResponse->RelayOutputs[i].token = oSysInfo->onvif_relay[i].onvif_relay_in.token;
		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties = (struct tt__RelayOutputSettings *)soap_malloc(soap, sizeof(struct tt__RelayOutputSettings));    
		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->Mode = oSysInfo->onvif_relay[i].onvif_relay_in.relaymode;
		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->DelayTime = oSysInfo->onvif_relay[i].onvif_relay_in.delaytime;
		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->IdleState = oSysInfo->onvif_relay[i].onvif_relay_in.idlestate;
		tds__GetRelayOutputsResponse->RelayOutputs[i].__size = 0;
		tds__GetRelayOutputsResponse->RelayOutputs[i].__any = NULL;
		dom_att(&tds__GetRelayOutputsResponse->RelayOutputs[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	}
	return SOAP_OK;
}


int __tds__SetRelayOutputSettings(struct soap* soap, struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings, struct _tds__SetRelayOutputSettingsResponse *tds__SetRelayOutputSettingsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo  = GetSysInfo();
	int i;
	long duration;
	relay_conf  relay;
	int _idlestate;
	long _delaytime;
	for(i = 0 ;i < oSysInfo->nrelays; i++)
	{
		if(strcmp(tds__SetRelayOutputSettings->RelayOutputToken, oSysInfo->onvif_relay[i].onvif_relay_in.token) == 0)
		{
			relay.position = i;
			break; 
		}
	}   
	if(i >= oSysInfo->nrelays)
	{       
		onvif_fault(soap,1,"ter:InvalidArgVal ", "ter:RelayToken","");
		return SOAP_FAULT;
	}   

	relay.logicalstate_t = oSysInfo->onvif_relay[i].relaylogicalstate;
	strcpy(relay.relay_conf_in_t.token_t, oSysInfo->onvif_relay[i].onvif_relay_in.token);
	relay.relay_conf_in_t.relaymode_t = tds__SetRelayOutputSettings->Properties->Mode;
	relay.relay_conf_in_t.idlestate_t = tds__SetRelayOutputSettings->Properties->IdleState;
	relay.relay_conf_in_t.delaytime_t = (long) tds__SetRelayOutputSettings->Properties->DelayTime;
	ControlSystemData(SFIELD_SET_RELAYS_CONF, (void*) &relay, sizeof(relay));

	_delaytime = relay.relay_conf_in_t.delaytime_t;
	_idlestate = relay.relay_conf_in_t.idlestate_t;
	if(tds__SetRelayOutputSettings->Properties->Mode == 1)
		_delaytime = 5;
	else
	{
		duration = (tds__SetRelayOutputSettings->Properties->DelayTime)/1000;
		if(duration!=10 && duration!=30 && duration!=60 && duration!=300 && duration!=600)
		{
			if(duration > 450)
				duration = 600;
			else if(duration > 180)
				duration = 300;
			else if(duration > 45)
				duration = 60;
			else if(duration > 20)
				duration = 30;
			else
				duration = 10;
		}
		switch(duration)
		{
			case 600:	_delaytime = 4;
					break;
			case 300:	_delaytime = 3;
					break;
			case 60:	_delaytime = 2;
					break;
			case 30:	_delaytime = 1;
					break;
			default:	_delaytime = 0;
					break;
		}
	}
	ControlSystemData(SFIELD_SET_GIOOUTTYPE,  (unsigned char *) &_idlestate, sizeof(unsigned char));
	ControlSystemData(SFIELD_SET_ALARMDURATION, (unsigned char *) &_delaytime, sizeof(unsigned char));
	return SOAP_OK;
}


int __tds__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo  = GetSysInfo();
	int i = 0;
	relay_conf  relay;

	for(i = 0 ;i < oSysInfo->nrelays; i++)
	{
		if(strcmp(tds__SetRelayOutputState->RelayOutputToken, oSysInfo->onvif_relay[i].onvif_relay_in.token) == 0)
		{
			relay.position = i;
			break; 
		}
	}   
	if(i >= oSysInfo->nrelays)
	{       
		onvif_fault(soap,1,"ter:InvalidArgVal ", "ter:RelayToken","");
		return SOAP_FAULT;
	}   

	relay.logicalstate_t = tds__SetRelayOutputState->LogicalState ;
	strcpy(relay.relay_conf_in_t.token_t, oSysInfo->onvif_relay[i].onvif_relay_in.token);
	relay.relay_conf_in_t.relaymode_t = oSysInfo->onvif_relay[i].onvif_relay_in.relaymode;
	relay.relay_conf_in_t.delaytime_t = oSysInfo->onvif_relay[i].onvif_relay_in.delaytime;
	relay.relay_conf_in_t.idlestate_t = oSysInfo->onvif_relay[i].onvif_relay_in.idlestate;
	ControlSystemData(SFIELD_SET_RELAY_LOGICALSTATE, (void*) &relay, sizeof(relay));
	ControlSystemData(SFIELD_SET_GIOOUTENABLE, (unsigned char *) &relay.logicalstate_t, sizeof(unsigned char));
	return SOAP_OK;
}


int __tds__SendAuxiliaryCommand(struct soap* soap, struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand, struct _tds__SendAuxiliaryCommandResponse *tds__SendAuxiliaryCommandResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetCACertificates(struct soap* soap, struct _tds__GetCACertificates *tds__GetCACertificates, struct _tds__GetCACertificatesResponse *tds__GetCACertificatesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__LoadCertificateWithPrivateKey(struct soap* soap, struct _tds__LoadCertificateWithPrivateKey *tds__LoadCertificateWithPrivateKey, struct _tds__LoadCertificateWithPrivateKeyResponse *tds__LoadCertificateWithPrivateKeyResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetCertificateInformation(struct soap* soap, struct _tds__GetCertificateInformation *tds__GetCertificateInformation, struct _tds__GetCertificateInformationResponse *tds__GetCertificateInformationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__LoadCACertificates(struct soap* soap, struct _tds__LoadCACertificates *tds__LoadCACertificates, struct _tds__LoadCACertificatesResponse *tds__LoadCACertificatesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__CreateDot1XConfiguration(struct soap* soap, struct _tds__CreateDot1XConfiguration *tds__CreateDot1XConfiguration, struct _tds__CreateDot1XConfigurationResponse *tds__CreateDot1XConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__SetDot1XConfiguration(struct soap* soap, struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration, struct _tds__SetDot1XConfigurationResponse *tds__SetDot1XConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetDot1XConfiguration(struct soap* soap, struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration, struct _tds__GetDot1XConfigurationResponse *tds__GetDot1XConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetDot1XConfigurations(struct soap* soap, struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations, struct _tds__GetDot1XConfigurationsResponse *tds__GetDot1XConfigurationsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__DeleteDot1XConfiguration(struct soap* soap, struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration, struct _tds__DeleteDot1XConfigurationResponse *tds__DeleteDot1XConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetDot11Capabilities(struct soap* soap, struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities, struct _tds__GetDot11CapabilitiesResponse *tds__GetDot11CapabilitiesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetDot11Status(struct soap* soap, struct _tds__GetDot11Status *tds__GetDot11Status, struct _tds__GetDot11StatusResponse *tds__GetDot11StatusResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__ScanAvailableDot11Networks(struct soap* soap, struct _tds__ScanAvailableDot11Networks *tds__ScanAvailableDot11Networks, struct _tds__ScanAvailableDot11NetworksResponse *tds__ScanAvailableDot11NetworksResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__GetSystemUris(struct soap* soap, struct _tds__GetSystemUris *tds__GetSystemUris, struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__StartFirmwareUpgrade(struct soap* soap, struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade, struct _tds__StartFirmwareUpgradeResponse *tds__StartFirmwareUpgradeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tds__StartSystemRestore(struct soap* soap, struct _tds__StartSystemRestore *tds__StartSystemRestore, struct _tds__StartSystemRestoreResponse *tds__StartSystemRestoreResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__PullMessages(struct soap* soap, struct _tev__PullMessages *tev__PullMessages, struct _tev__PullMessagesResponse *tev__PullMessagesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	soap->header->wsa5__Action = (char*)soap_malloc(soap,sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(soap->header->wsa5__Action,"http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/PullMessagesResponse");
	struct _tt__Message * msg = (struct _tt__Message *)soap_malloc(soap,sizeof(struct _tt__Message));
	soap_default_notify_msg(soap,msg);
	time_t time_n;
	struct tm *tm_t;
	time_n = time(NULL);
	tm_t = localtime(&time_n);
	char time_cur[150] = "\0";
	sprintf(time_cur,"%4d-%02d-%02dT%02d:%02d:%02d",1900+tm_t->tm_year,1+tm_t->tm_mon,tm_t->tm_mday,tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec);
	unsigned char macaddr[MACH_ADDR_LENGTH];
	char _HwId[LARGE_INFO_LENGTH];
	NET_IPV4 ip;
	time_t tnow = time(NULL);
	net_get_hwaddr(ETH_NAME, macaddr);
	sprintf(_HwId,"urn:uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X", macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	tev__PullMessagesResponse->__sizeNotificationMessage = 1;
	tev__PullMessagesResponse->CurrentTime = tnow;
	tev__PullMessagesResponse->TerminationTime = tnow + tev__PullMessages->Timeout;
	tev__PullMessagesResponse->wsnt__NotificationMessage = (struct wsnt__NotificationMessageHolderType *) soap_malloc(soap, sizeof(struct wsnt__NotificationMessageHolderType)*tev__PullMessagesResponse->__sizeNotificationMessage);
	tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference = (struct wsa5__EndpointReferenceType *)soap_malloc(soap, sizeof(struct wsa5__EndpointReferenceType));
	char _IPAddr[INFO_LENGTH];
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference->Address = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference->Address, _IPAddr);
	tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference->ReferenceParameters = NULL;
	tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference->Metadata = NULL;
	tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference->__size = 0;
	tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference->__any = NULL;
	tev__PullMessagesResponse->wsnt__NotificationMessage->SubscriptionReference->__anyAttribute = NULL;
	tev__PullMessagesResponse->wsnt__NotificationMessage->ProducerReference = NULL; 
	tev__PullMessagesResponse->wsnt__NotificationMessage->Topic = (struct wsnt__TopicExpressionType *)soap_malloc(soap, sizeof(struct wsnt__TopicExpressionType));
	dom_att(&tev__PullMessagesResponse->wsnt__NotificationMessage->Topic->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	soap_default_dom_element(soap, &tev__PullMessagesResponse->wsnt__NotificationMessage->Topic->__mixed);
	tev__PullMessagesResponse->wsnt__NotificationMessage->Topic->__any = (struct soap_dom_element *)soap_malloc(soap,sizeof(struct soap_dom_element));
	soap_default_dom_element(soap, tev__PullMessagesResponse->wsnt__NotificationMessage->Topic->__any);
	tev__PullMessagesResponse->wsnt__NotificationMessage->Topic->__any->data = "tns1:RuleEngine/tns1:CellMotionDetector/tns1:Motion";
	tev__PullMessagesResponse->wsnt__NotificationMessage->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet"; 
	tev__PullMessagesResponse->wsnt__NotificationMessage->Message.__any = (struct soap_dom_element *)soap_malloc(soap,sizeof(struct soap_dom_element));
	soap_default_dom_element(soap,tev__PullMessagesResponse->wsnt__NotificationMessage->Message.__any);
	{	tev__PullMessagesResponse->wsnt__NotificationMessage->Message.__any->nstr = "http://www.onvif.org/ver10/schema";
		tev__PullMessagesResponse->wsnt__NotificationMessage->Message.__any->name = "Message";
		tev__PullMessagesResponse->wsnt__NotificationMessage->Message.__any->type = SOAP_TYPE__tt__Message;
		{
			msg->Source = (struct tt__ItemList *)soap_malloc(soap,sizeof(struct tt__ItemList));
			msg->Source->__sizeSimpleItem = 3;
			msg->Source->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc(soap,sizeof(struct _tt__ItemList_SimpleItem)*msg->Source->__sizeSimpleItem);
			msg->Source->__sizeElementItem = 0;
			msg->Source->ElementItem =  NULL;
			msg->Source->Extension = NULL;
			dom_att(&msg->Source->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
			msg->Source->SimpleItem[0].Name = "VideoSourceConfigurationToken";
			msg->Source->SimpleItem[0].Value = "1";
			msg->Source->SimpleItem[1].Name = "VideoAnalyticsConfigurationToken";
			msg->Source->SimpleItem[1].Value = "1";
			msg->Source->SimpleItem[2].Name = "Rule";
			msg->Source->SimpleItem[2].Value = "MotionInDefinedCells";
			msg->Data = (struct tt__ItemList *)soap_malloc(soap,sizeof(struct tt__ItemList));
			msg->Data->__sizeSimpleItem = 1;
			msg->Data->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc(soap,sizeof(struct _tt__ItemList_SimpleItem)*msg->Data->__sizeSimpleItem);
			msg->Data->__sizeElementItem = 0;
			msg->Data->ElementItem =  NULL;
			msg->Data->Extension = NULL;
			dom_att(&msg->Data->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
			msg->Data->SimpleItem[0].Name = "IsMotion";
			if(eventstatus)msg->Data->SimpleItem[0].Value = "true";
			else msg->Data->SimpleItem[0].Value = "false";
			time(&msg->UtcTime);
		}
		tev__PullMessagesResponse->wsnt__NotificationMessage->Message.__any->node = msg;
	}
	return SOAP_OK;
}

int __tev__SetSynchronizationPoint(struct soap* soap, struct _tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, struct _tev__SetSynchronizationPointResponse *tev__SetSynchronizationPointResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	soap->header->wsa5__Action = (char*)soap_malloc(soap,sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(soap->header->wsa5__Action,"http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/SetSynchronizationPointResponse");
	return SOAP_OK;
}


/*
 * @brief Get Event Service Capabilities
 * @This message contains a request for GetServiceCapabilities.
 *   
 * @param Request       : _tev__GetServiceCapabilities structure contains a request for GetServiceCapabilities. 
 Returns a capailities of device services                       
 * @param Response      : _tev__GetServicesCapabilitiesResponse contains the requested event capabilities.
 * @return              : On success - SOAP_OK.
 */
int __tev__GetServiceCapabilities(struct soap* soap, struct _tev__GetServiceCapabilities *tev__GetServiceCapabilities, struct _tev__GetServiceCapabilitiesResponse *tev__GetServiceCapabilitiesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
	}
#endif

	tev__GetServiceCapabilitiesResponse->Capabilities = (struct tev__Capabilities  *)soap_malloc(soap, sizeof(struct tev__Capabilities));
	tev__GetServiceCapabilitiesResponse->Capabilities->__size = 0;
	tev__GetServiceCapabilitiesResponse->Capabilities->__any = NULL;
	tev__GetServiceCapabilitiesResponse->Capabilities->WSSubscriptionPolicySupport = NULL;
	tev__GetServiceCapabilitiesResponse->Capabilities->WSPullPointSupport = (int*)soap_malloc(soap,sizeof(int));
	tev__GetServiceCapabilitiesResponse->Capabilities->WSPullPointSupport = &_true;
	tev__GetServiceCapabilitiesResponse->Capabilities->WSPausableSubscriptionManagerInterfaceSupport = (int*)soap_malloc(soap,sizeof(int));
	tev__GetServiceCapabilitiesResponse->Capabilities->WSPausableSubscriptionManagerInterfaceSupport = &_true;
	tev__GetServiceCapabilitiesResponse->Capabilities->RateLimitPolicySupport = (int*)soap_malloc(soap,sizeof(int));
	tev__GetServiceCapabilitiesResponse->Capabilities->RateLimitPolicySupport = &_false;
	tev__GetServiceCapabilitiesResponse->Capabilities->MaxNotificationProducers = (int*)soap_malloc(soap,sizeof(int));
	*tev__GetServiceCapabilitiesResponse->Capabilities->MaxNotificationProducers = 3;
	tev__GetServiceCapabilitiesResponse->Capabilities->MaxPullPoints = (int*)soap_malloc(soap,sizeof(int));
	*tev__GetServiceCapabilitiesResponse->Capabilities->MaxPullPoints = 3;
	tev__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute.soap = soap;
	tev__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute.next = NULL;
	tev__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute.name = NULL;
	tev__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute.data = NULL;
	tev__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute.wide = NULL;
	tev__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute.nstr = NULL;
	return SOAP_OK;
}


int __tev__CreatePullPointSubscription(struct soap* soap, struct _tev__CreatePullPointSubscription *tev__CreatePullPointSubscription, struct _tev__CreatePullPointSubscriptionResponse *tev__CreatePullPointSubscriptionResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	soap->header->wsa5__Action = (char*)soap_malloc(soap,sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(soap->header->wsa5__Action,"http://www.onvif.org/ver10/events/wsdl/EventPortType/CreatePullPointSubscriptionResponse");
	char _IPAddr[INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	time_t tnow = time(NULL);
	char initial_time[] = "PT10S";	
	time_t t_time = 0;
	int i, s = 0,t = 0, thread_running = 0;
	char **topic_des = NULL;
	char *topic = "tns1:RuleEngine/tns1:CellMotionDetector/tns1:Motion";
	if(tev__CreatePullPointSubscription->InitialTerminationTime != NULL)
	{
		t_time = periodtol(tev__CreatePullPointSubscription->InitialTerminationTime);
	}
	else 
	{
		t_time = periodtol(initial_time);
	}

	if(tev__CreatePullPointSubscription->Filter != NULL)
	{
		s = tev__CreatePullPointSubscription->Filter->__size;
		if(s > 0)
		{
			topic_des = (char**)soap_malloc(soap,sizeof(char**)*s);
			for(t=0;t<s;t++)
			{
				topic_des[t] = wtochar(soap,tev__CreatePullPointSubscription->Filter->__any[t].wide);
				if(topic_des[t] != NULL)
				{
					if(strstr(tev__CreatePullPointSubscription->Filter->__any[t].name,"TopicExpression")!=NULL)
					{
						if(strstr(topic_des[t],topic) == NULL)
						{
							onvif_fault(soap,1,"ter:InvalidArgVal","ter:InvalidTopicExpression","Invalid TopicExpression");
							return SOAP_FAULT;							
						}
						else ONVIF_DBG("Valid Topic Expression Found!!");
					}
					else if(strstr(tev__CreatePullPointSubscription->Filter->__any[t].name,"MessageContent")!=NULL)
					{
						onvif_fault( soap,1,"ter:InvalidArgVal", "ter:InvalidMesssageContent" ,"Invalid MesssageContent" );
						return SOAP_FAULT;
					}

				}	
			}
		}
	}

	/* change in sub_cnt, if it gets corrupted accidently due to threads or something */
	if(sub_cnt >= MAX_SUBSCRIPTION)
	{
		for(i = 0; i < MAX_SUBSCRIPTION; i++) {
			if(notify_thread[i] == 1) {
				thread_running++;               /* how many subscription and pullpoint threads are running */
			}
		}
		sub_cnt = thread_running;
	}

	/* Check if previous Subscription is terminated */	
	if(sub_cnt < MAX_SUBSCRIPTION )
	{
		for(i = 0; i < MAX_SUBSCRIPTION; i++)
		{
			if(notify_thread[i] == 0) {
				notify_thread[i] = 1;
				break;
			}
		}

		notify_args[i].t_out_thr = t_time;
		strcpy(notify_args[i].topic_str,topic);

		/* Notification thread Initialization */
		if(t_time == 0 || t_time < 0)
		{
			onvif_fault(soap,1,"ter:InValidArgs","ter:InvalidTimeFormat", "Invalid time format");
			return SOAP_FAULT;
		}

		if(pthread_create(&notify_thread_sub[i], NULL, eventtracker_thr, i) != 0)
		{
			tev__CreatePullPointSubscriptionResponse->SubscriptionReference.Address = NULL;
			notify_args[i].t_out_thr = 0;
			notify_thread[i] = 0;
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:CannotCreateSubscriptionManager","Reached Max Subcription[10]");
			return SOAP_FAULT;
		}
		else
		{
			/*the correct subscription reference acc to wsnt specifications*/
			sleep(1);
			sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services/SubscriptionIdx=%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3],i);
			tev__CreatePullPointSubscriptionResponse->SubscriptionReference.Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
			strcpy(tev__CreatePullPointSubscriptionResponse->SubscriptionReference.Address,_IPAddr);
			sub_cnt++;
		}

	}
	else 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal","ter:CannotCreateSubscriptionManager","Reached Max Subcription[10]");
		return SOAP_FAULT;

	}
	tev__CreatePullPointSubscriptionResponse->SubscriptionReference.ReferenceParameters = NULL;
	tev__CreatePullPointSubscriptionResponse->SubscriptionReference.Metadata = NULL;
	tev__CreatePullPointSubscriptionResponse->SubscriptionReference.__size = 0;
	tev__CreatePullPointSubscriptionResponse->SubscriptionReference.__any = NULL;
	tev__CreatePullPointSubscriptionResponse->SubscriptionReference.__anyAttribute = NULL;
	tev__CreatePullPointSubscriptionResponse->wsnt__CurrentTime = tnow;
	tev__CreatePullPointSubscriptionResponse->wsnt__TerminationTime = tnow + t_time;
	tev__CreatePullPointSubscriptionResponse->__size = 0;
	tev__CreatePullPointSubscriptionResponse->__any = NULL;
	return SOAP_OK;


}

int __tev__GetEventProperties(struct soap* soap, struct _tev__GetEventProperties *tev__GetEventProperties, struct _tev__GetEventPropertiesResponse *tev__GetEventPropertiesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
	}
#endif

	soap->header->wsa5__Action = (char*)soap_malloc(soap,sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(soap->header->wsa5__Action,"http://www.onvif.org/ver10/events/wsdl/EventPortType/GetEventPropertiesResponse");
	char Name_lookup[3][INFO_LENGTH] = {"tt:ReferenceToken","tt:ReferenceToken","tt:String"}; 
	char Type_lookup[3][INFO_LENGTH] = {"VideoSourceConfigurationToken","VideoAnalyticsConfigurationToken","Rule"};
	int i = 0;
	struct tt__MessageDescription * md = (struct tt__MessageDescription *)soap_malloc(soap,sizeof(struct tt__MessageDescription));
	memset(md,0,sizeof(struct tt__MessageDescription));
	md->__anyAttribute.soap = soap;
	dom_att(&md->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	md->Source = (struct tt__ItemListDescription *)soap_malloc(soap,sizeof(struct tt__ItemListDescription));
	memset(md->Source,0,sizeof(struct tt__ItemListDescription));
	md->Source->__sizeSimpleItemDescription = 3;
	md->Source->SimpleItemDescription= (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc(soap,sizeof(struct _tt__ItemListDescription_SimpleItemDescription)*md->Source->__sizeSimpleItemDescription);
	md->IsProperty = (int *)soap_malloc(soap,sizeof(int));
	md->IsProperty = &_false; 	
	for(i=0;i< md->Source->__sizeSimpleItemDescription;i++)
	{
		md->Source->SimpleItemDescription[i].Name = (char*)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		md->Source->SimpleItemDescription[i].Type = (char*)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		strcpy(md->Source->SimpleItemDescription[i].Name,Name_lookup[i]);
		strcpy(md->Source->SimpleItemDescription[i].Type,Type_lookup[i]);
	}
	i = 0;
	md->Source->__anyAttribute.soap = soap;
	dom_att(&md->Source->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	md->Data = (struct tt__ItemListDescription *)soap_malloc(soap,sizeof(struct tt__ItemListDescription));
	memset(md->Data,0,sizeof(struct tt__ItemListDescription));
	md->Data->__sizeSimpleItemDescription = 1;
	md->Data->SimpleItemDescription = (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc(soap,sizeof(struct _tt__ItemListDescription_SimpleItemDescription)*md->Data->__sizeSimpleItemDescription);
	md->Data->SimpleItemDescription->Name = (char*)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
	md->Data->SimpleItemDescription->Type =  (char*)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
	strcpy(md->Data->SimpleItemDescription->Name,"IsMotion");
	strcpy(md->Data->SimpleItemDescription->Type,"tt:Boolean");
	md->Data->__anyAttribute.soap = soap;
	dom_att(&md->Data->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	tev__GetEventPropertiesResponse->__sizeTopicNamespaceLocation = 1;
	tev__GetEventPropertiesResponse->TopicNamespaceLocation = (char **)soap_malloc(soap, sizeof(char *));
	tev__GetEventPropertiesResponse->TopicNamespaceLocation[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	strcpy(tev__GetEventPropertiesResponse->TopicNamespaceLocation[0],"http://www.onvif.org/onvif/ver10/topics/topicns.xml");
	tev__GetEventPropertiesResponse->wsnt__FixedTopicSet = 0;
	tev__GetEventPropertiesResponse->wstop__TopicSet = (struct wstop__TopicSetType *)soap_malloc(soap, sizeof(struct wstop__TopicSetType));
	memset( tev__GetEventPropertiesResponse -> wstop__TopicSet, 0, sizeof( struct wstop__TopicSetType));
	tev__GetEventPropertiesResponse->wstop__TopicSet->__size = 1;
	tev__GetEventPropertiesResponse->wstop__TopicSet->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element) * tev__GetEventPropertiesResponse->wstop__TopicSet->__size);
	memset(tev__GetEventPropertiesResponse->wstop__TopicSet->__any,0,sizeof(struct soap_dom_element));
	tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
	memset(tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts,0,sizeof(struct soap_dom_element));
	tev__GetEventPropertiesResponse->wstop__TopicSet->__any->nstr = "http://www.onvif.org/ver10/topics";    //for tns1
	tev__GetEventPropertiesResponse->wstop__TopicSet->__any->name = "RuleEngine";   //for root topic expression tns1:RuleEngine
	tev__GetEventPropertiesResponse->wstop__TopicSet->__any->soap = soap;
	{
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->prnt = tev__GetEventPropertiesResponse->wstop__TopicSet->__any;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
		memset(tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts,0,sizeof(struct soap_dom_element));
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->nstr = "http://www.onvif.org/ver10/topics";
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->name = "CellMotionDetector";
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->soap = soap;
	}
	{
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->prnt = tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
		memset(tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts,0,sizeof(struct soap_dom_element));
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->atts = (struct soap_dom_attribute*)soap_malloc(soap,sizeof(struct soap_dom_attribute));
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->atts->nstr = "http://docs.oasis-open.org/wsn/t-1";//wstop
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->atts->name = "topic";
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->atts->data = "true";
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->atts->soap = soap;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->atts->wide = NULL;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->atts->next = NULL;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->nstr = "http://www.onvif.org/ver10/topics";//tns1
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->name = "Motion";
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->soap = soap;
	}
	{
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts->prnt = tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts->nstr = "http://www.onvif.org/ver10/schema";//tt
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts->name = "MessageDescription";
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts->type = SOAP_TYPE_tt__MessageDescription;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts->node = md;
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any->elts->elts->elts->soap = soap;
	}

	tev__GetEventPropertiesResponse->__sizeTopicExpressionDialect = 2;
	tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect = (char **)soap_malloc(soap,sizeof(char*)*2);
	tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect[0] = (char *)soap_malloc(soap,INFO_LENGTH);
	tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect[1] = (char *)soap_malloc(soap,INFO_LENGTH);
	strcpy(tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect[0], "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
	strcpy(tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect[1], "http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete");
	tev__GetEventPropertiesResponse->__sizeMessageContentFilterDialect = 1;
	tev__GetEventPropertiesResponse->MessageContentFilterDialect = (char **)soap_malloc(soap, sizeof(char *));
	tev__GetEventPropertiesResponse->MessageContentFilterDialect[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	strcpy(tev__GetEventPropertiesResponse->MessageContentFilterDialect[0], "http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter");
	tev__GetEventPropertiesResponse->__sizeProducerPropertiesFilterDialect = 0;
	tev__GetEventPropertiesResponse->ProducerPropertiesFilterDialect = NULL;
	tev__GetEventPropertiesResponse->__sizeMessageContentSchemaLocation = 1;
	tev__GetEventPropertiesResponse->MessageContentSchemaLocation = (char **)soap_malloc(soap, sizeof(char *));
	tev__GetEventPropertiesResponse->MessageContentSchemaLocation[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(tev__GetEventPropertiesResponse->MessageContentSchemaLocation[0], "http://www.onvif.org/ver10/schema/onvif.xsd");
	tev__GetEventPropertiesResponse->__size = 0;
	tev__GetEventPropertiesResponse->__any = NULL;
	return SOAP_OK;
}

int __tev__Renew(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	char * tst = NULL;
	int i = 0;
	time_t term = 0;
	time_t time_tm;
	time_t termtime_tm;

	sleep(1);//some delay is required for the global settings on sub_cnt variable to takeplace
	if(soap->header != NULL)
	{
		soap->header->wsa5__Action = soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		soap->header->wsa5__Action = "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/RenewResponse";
	}
	if(wsnt__Renew->TerminationTime != NULL)
	{
		term = (long)periodtol(wsnt__Renew->TerminationTime);
		if(term == 0)
		{
			term = (long)xsperiodtol(wsnt__Renew->TerminationTime);		
		}
	}
	if(soap->header->wsa5__To != NULL)
	{
		tst = strstr(soap->header->wsa5__To,"=");
		if(tst != NULL)
		{
			tst++;
			i = atoi(tst);
			if(notify_args[i].status == 1)
			{
				if(i >= 0 || i<= MAX_SUBSCRIPTION) {notify_args[i].t_out_thr += term;}

			}
			else
			{
				onvif_fault(soap,1,"ter:InvalidArdVal","ter:ResourceUnknownFaultType","Invalid Subscription Manager - DETECTED Negative Renew!");
				return SOAP_FAULT;
			}
		}
	}
	time_tm = time(NULL);
	termtime_tm = time_tm + term;
	wsnt__RenewResponse->TerminationTime = termtime_tm;
	wsnt__RenewResponse->CurrentTime = (time_t *) soap_malloc(soap, sizeof(time_t)); 
	wsnt__RenewResponse->CurrentTime[0] = time_tm; 
	wsnt__RenewResponse->__size = 0;
	wsnt__RenewResponse->__any = NULL;
	return SOAP_OK;

}

int __tev__Unsubscribe(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	char * tst = NULL;
	int i = 0;
	sleep(1);//some delay is required for the global settings on sub_cnt variable to takeplace
	if(soap->header != NULL)
	{
		soap->header->wsa5__Action = soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		soap->header->wsa5__Action = "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/UnsubscribeResponse";
	}
	if(soap->header->wsa5__To != NULL)
	{
		tst = strstr(soap->header->wsa5__To,"=");
		if(tst != NULL)
		{
			tst++;
			i = atoi(tst);
			if(notify_args[i].status == 1)
			{
				if(i >= 0 && i<= MAX_SUBSCRIPTION) notify_args[i].t_out_thr = 0;
			}
			else
			{
				onvif_fault( soap,1,"ter:InvalidArgVal", "ter:InvalidSubscriptionManager" ,"Negative Unsubscribe test" );
				return SOAP_FAULT;
			}
			wsnt__UnsubscribeResponse->__size = 0;
			wsnt__UnsubscribeResponse->__any = NULL;
		}
		else 
		{
			onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
			return SOAP_FAULT;		
		}

		return SOAP_OK;

	}
}

int __tev__Subscribe(struct soap* soap, struct _wsnt__Subscribe *wsnt__Subscribe, struct _wsnt__SubscribeResponse *wsnt__SubscribeResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	char *initial_time = "PT10S";
	time_t time_tm = 0;
	time_t t_time = 0 ;
	char _IPAddr[LARGE_INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	int s = 0,t = 0, i, thread_running = 0;

	if(soap->header)
	{
		soap->header->wsa5__Action = soap_malloc(soap,sizeof(char)*LARGE_INFO_LENGTH);
		strcpy(soap->header->wsa5__Action,"http://docs.oasis-open.org/wsn/bw-2/NotificationProducer/SubscribeResponse");
	}
	/*get current time*/
	time_tm = time(NULL);

	//TopicExpression Extraction and checking
	char **topic_des = NULL;
	char *topic = "tns1:RuleEngine/tns1:CellMotionDetector/tns1:Motion";
	if(wsnt__Subscribe->Filter != NULL)
	{
		s = wsnt__Subscribe->Filter->__size;
		if(s > 0)
		{	
			topic_des = (char**)soap_malloc(soap,sizeof(char**)*s);
			for(t = 0;t < s;t++)
			{
				topic_des[t] = wtochar(soap,wsnt__Subscribe->Filter->__any[t].wide);
				if(topic_des[t] != NULL)
				{
					if(strstr(wsnt__Subscribe->Filter->__any[t].name,"TopicExpression")!=NULL)
					{
						if(strstr(topic_des[t],topic) == NULL)
						{
							onvif_fault(soap,1,"ter:InvalidArgVal","ter:InvalidTopicExpression","Inavlaid topic expression ");
							return SOAP_FAULT;							
						}
						else ONVIF_DBG("Valid Topic Expression Found!!");
					}
					else if(strstr(wsnt__Subscribe->Filter->__any[t].name,"MessageContent")!=NULL)
					{
						onvif_fault(soap,1,"ter:InvalidArgVal","ter:InvalidMesssageContent", "Invalid message content ");
						return SOAP_FAULT;
					}
				}
			}
		}
	}
	if(wsnt__Subscribe->InitialTerminationTime != NULL)
	{
		t_time = periodtol(wsnt__Subscribe->InitialTerminationTime);
	}
	else
	{
		t_time = periodtol(initial_time);//the specified TIME (PT___) has to be extracted from the soap request prior to spawning the notify thread
	}

	/* change in sub_cnt, if it gets corrupted accidently due to threads or something */
	if(sub_cnt >=MAX_SUBSCRIPTION)
	{
		for(i = 0; i < MAX_SUBSCRIPTION; i++) {
			if(notify_thread[i] == 1) {
				thread_running++;               /* how many subscription and pullpoint threads are running */
			}
		}
		sub_cnt = thread_running;
	}

	/* Check if previous Subscription is terminated */	
	if(sub_cnt < MAX_SUBSCRIPTION )
	{
		for(i = 0; i < MAX_SUBSCRIPTION; i++)
		{
			if(notify_thread[i] == 0) {
				notify_thread[i] = 1;
				break;
			}
		}
		notify_args[i].t_out_thr = t_time;
		if(wsnt__Subscribe->ConsumerReference.Address != NULL)
		{
			strcpy(notify_args[i].consumer_addr,wsnt__Subscribe->ConsumerReference.Address);

		}
		strcpy(notify_args[i].topic_str,topic);
		/* Notification thread Initialization */
		if(t_time == 0 || t_time < 0)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:InvalidTimeFormat","Inavlaid time format ");
			return SOAP_FAULT;
		}

		if(pthread_create(&notify_thread_sub[i], NULL, notify_thr, i) != 0)
		{
			ONVIF_DBG("Error: Notify thread creation failed");
			wsnt__SubscribeResponse->SubscriptionReference.Address = NULL;
			notify_args[i].t_out_thr = 0;
			notify_thread[i] = 0;
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:CannotCreateSubscriptionManager","Reached Max Subcription[10]");
			return SOAP_FAULT;
		}
		else
		{
			sleep(1);
			/*the correct subscription reference acc to wsnt specifications*/
			sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services/SubscriptionIdx=%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3],i);
			wsnt__SubscribeResponse->SubscriptionReference.Address = (char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
			strcpy(wsnt__SubscribeResponse->SubscriptionReference.Address, _IPAddr);
			sub_cnt++;
		}

	}
	else 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal","ter:CannotCreateSubscriptionManager","Reached Max Subcription[10]");
		return SOAP_FAULT;

	}
	wsnt__SubscribeResponse->SubscriptionReference.ReferenceParameters = NULL;
	wsnt__SubscribeResponse->SubscriptionReference.Metadata = NULL;
	wsnt__SubscribeResponse->SubscriptionReference.__size = 0;
	wsnt__SubscribeResponse->SubscriptionReference.__any = NULL;
	wsnt__SubscribeResponse->SubscriptionReference.__anyAttribute = NULL;
	wsnt__SubscribeResponse->CurrentTime = (time_t *) soap_malloc(soap, sizeof(time_t));
	wsnt__SubscribeResponse->CurrentTime[0] = time_tm;
	t_time += time_tm;
	wsnt__SubscribeResponse->TerminationTime = (time_t *) soap_malloc(soap, sizeof(time_t));
	wsnt__SubscribeResponse->TerminationTime[0] = t_time;
	wsnt__SubscribeResponse->__size = 0;
	wsnt__SubscribeResponse->__any = NULL;
	return SOAP_OK;
}


int __tev__GetCurrentMessage(struct soap* soap, struct _wsnt__GetCurrentMessage *wsnt__GetCurrentMessage, struct _wsnt__GetCurrentMessageResponse *wsnt__GetCurrentMessageResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__Notify(struct soap* soap, struct _wsnt__Notify *wsnt__Notify) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__GetMessages(struct soap* soap, struct _wsnt__GetMessages *wsnt__GetMessages, struct _wsnt__GetMessagesResponse *wsnt__GetMessagesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__DestroyPullPoint(struct soap* soap, struct _wsnt__DestroyPullPoint *wsnt__DestroyPullPoint, struct _wsnt__DestroyPullPointResponse *wsnt__DestroyPullPointResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__Notify_(struct soap* soap, struct _wsnt__Notify *wsnt__Notify) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__CreatePullPoint(struct soap* soap, struct _wsnt__CreatePullPoint *wsnt__CreatePullPoint, struct _wsnt__CreatePullPointResponse *wsnt__CreatePullPointResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__Renew_(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__Unsubscribe_(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__PauseSubscription(struct soap* soap, struct _wsnt__PauseSubscription *wsnt__PauseSubscription, struct _wsnt__PauseSubscriptionResponse *wsnt__PauseSubscriptionResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tev__ResumeSubscription(struct soap* soap, struct _wsnt__ResumeSubscription *wsnt__ResumeSubscription, struct _wsnt__ResumeSubscriptionResponse *wsnt__ResumeSubscriptionResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __tmd__GetServiceCapabilities(struct soap* soap, struct _tmd__GetServiceCapabilities *tmd__GetServiceCapabilities, struct _tmd__GetServiceCapabilitiesResponse *tmd__GetServiceCapabilitiesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	tmd__GetServiceCapabilitiesResponse->Capabilities = (struct tmd__Capabilities*)soap_malloc(soap, sizeof(struct tmd__Capabilities));
	tmd__GetServiceCapabilitiesResponse->Capabilities->__size = 0;
	tmd__GetServiceCapabilitiesResponse->Capabilities-> __any = NULL;
	tmd__GetServiceCapabilitiesResponse->Capabilities->VideoSources = 1;
	tmd__GetServiceCapabilitiesResponse->Capabilities->VideoOutputs = 0;
	tmd__GetServiceCapabilitiesResponse->Capabilities->AudioSources = 1;
	tmd__GetServiceCapabilitiesResponse->Capabilities->AudioOutputs = 1;
	tmd__GetServiceCapabilitiesResponse->Capabilities->RelayOutputs = 1;
	tmd__GetServiceCapabilitiesResponse->Capabilities->SerialPorts = 0;
	tmd__GetServiceCapabilitiesResponse->Capabilities-> DigitalInputs = 0;
	dom_att(&tmd__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}

int __tmd__GetRelayOutputOptions(struct soap* soap, struct _tmd__GetRelayOutputOptions *tmd__GetRelayOutputOptions, struct _tmd__GetRelayOutputOptionsResponse *tmd__GetRelayOutputOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo  = GetSysInfo();
	int i;
	relay_conf relay;
	for(i = 0 ;i < oSysInfo->nrelays; i++)
	{
		if(strcmp(tmd__GetRelayOutputOptions->RelayOutputToken, oSysInfo->onvif_relay[i].onvif_relay_in.token) == 0)
		{
			break;
		}
	}
	if(i >= oSysInfo->nrelays)
	{
		i = 0;
		/*onvif_fault(soap,1,"ter:InvalidArgVal ", "ter:RelayToken","");
		  return SOAP_FAULT;*/
	}

	tmd__GetRelayOutputOptionsResponse->__sizeRelayOutputOptions = 1;
	tmd__GetRelayOutputOptionsResponse->RelayOutputOptions = (struct tmd__RelayOutputOptions *)soap_malloc(soap, tmd__GetRelayOutputOptionsResponse->__sizeRelayOutputOptions * sizeof(struct tmd__RelayOutputOptions));
	for(i = 0; i < tmd__GetRelayOutputOptionsResponse->__sizeRelayOutputOptions; i++)
	{
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].__sizeMode = 2;
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].Mode = (int *)soap_malloc(soap, 2 * sizeof(int));
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].Mode[0] = 0;
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].Mode[1] = 1;
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].DelayTimes = NULL;
		/*tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].DelayTimes = (char *)soap_malloc(soap, SMALL_INFO_LENGTH * sizeof(char));
		  strcpy(tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].DelayTimes, "012345");*/
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].Discrete = NULL;
		/*tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].Discrete = (int *)soap_malloc(soap, sizeof(int));
		 *tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].Discrete = 0;*/
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].Extension = NULL;
		tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].token = (char *)soap_malloc(soap, SMALL_INFO_LENGTH * sizeof(char));
		strcpy(tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].token, oSysInfo->onvif_relay[i].onvif_relay_in.token);
		dom_att(&tmd__GetRelayOutputOptionsResponse->RelayOutputOptions[i].__anyAttribute, 0 , NULL, NULL, NULL, NULL, soap);
	}

	return SOAP_OK;
}

/**
 * @brief Get Audio Source 
 * @This operation lists all available audio sources of the device.
 *   
 * @param Request	: _tmd__GetAudioSource structure contains empty response.  
 * @param Response	: _tmd__GetAudioSourceResponse structure contains list of structures describing all available audio sources of the device.
 * @return               : On success - SOAP_OK.
 : On failure - NVT does not support audio.

 */
int __tmd__GetAudioSources(struct soap* soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse)
{
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int i;

	trt__GetAudioSourcesResponse->__sizeAudioSources = oSysInfo->nprofile;

	trt__GetAudioSourcesResponse->AudioSources = (struct tt__AudioSource *)soap_malloc(soap, sizeof(struct tt__AudioSource) * (oSysInfo->nprofile));
	for(i=0; i < oSysInfo->nprofile; i++)
	{
		trt__GetAudioSourcesResponse->AudioSources[i].token = oSysInfo->Oprofile[i].AASC.Asourcetoken;
		trt__GetAudioSourcesResponse->AudioSources[i].Channels = 1;
		trt__GetAudioSourcesResponse->AudioSources[i].__size = 0;
		trt__GetAudioSourcesResponse->AudioSources[i].__any = NULL;
		dom_att(&trt__GetAudioSourcesResponse->AudioSources[i].__anyAttribute, 0 , NULL, NULL, NULL, NULL, soap);
	}

	return SOAP_OK;
}


int __tmd__GetAudioOutputs(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


/**
 * @brief Get Video Source 
 * @This operation lists all available video sources for the device.
 *   
 * @param Request	: _trt__GetVideoSources structure contains an empty message.  
 * @param Response	: _trt__GetVideoSourcesResponse structure contains a list of structures describing all available video sources of the device.
 * @return               : On success SOAP_OK.
 : On failure Audio or Audio Outputs are not supported by the NVT.

 */
int __tmd__GetVideoSources(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	int flag = 0, j =0;
	float _Framerate; 
	float _Brightness;
	float _Saturation; 
	float _Contrast; 
	float _Sharpness; 
	float _WideDynamicRange;
	int _Backlight; 
	float _BacklightCompensationLevel; 
	int _WhiteBalance;
	int height;
	int size = 0;
	int i = 0;
	int n = 0;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		flag = NOT_EXIST;
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AVSC.Vsourcetoken, oSysInfo->Oprofile[i].AVSC.Vsourcetoken)==0)
			{
				flag = EXIST;
				break;
			}
		}
		if(!flag)
		{
			size++;
		}
	}

	trt__GetVideoSourcesResponse->__sizeVideoSources = size;
	trt__GetVideoSourcesResponse->VideoSources = (struct tt__VideoSource *)soap_malloc(soap, sizeof(struct tt__VideoSource) * size);

	for(i = 0; i < oSysInfo->nprofile ; i++)
	{
		flag = NOT_EXIST;
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AVSC.Vsourcetoken, oSysInfo->Oprofile[i].AVSC.Vsourcetoken)==0)
			{
				flag = EXIST;
				break;
			}
		}

		if(!flag)  
		{
			_Framerate = oget_framerate(0);
			_Brightness = (float)(oSysInfo->imaging_conf[i].Brightness * 100) / 255; 
			_Saturation = (float)(oSysInfo->imaging_conf[i].Saturation * 100) / 255; 
			_Contrast = (float)(oSysInfo->imaging_conf[i].Contrast * 100) / 255; 
			_Sharpness = (float)(oSysInfo->imaging_conf[i].Sharpness * 100) / 255;
			_WideDynamicRange = oSysInfo->imaging_conf[i].WideDynamicMode;
			_Backlight = oSysInfo->imaging_conf[i].BacklightCompensationMode; 
			_BacklightCompensationLevel = oSysInfo->imaging_conf[i].BackLightLevel;
			_WhiteBalance = oSysInfo->imaging_conf[i].WhiteBalanceMode;
			height = oget_resolutionheight(0);

			trt__GetVideoSourcesResponse->VideoSources[n].token = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;//"IPNC_VideoSource";
			trt__GetVideoSourcesResponse->VideoSources[n].Framerate = _Framerate;
			trt__GetVideoSourcesResponse->VideoSources[n].Resolution = (struct tt__VideoResolution*)soap_malloc(soap, sizeof(struct tt__VideoResolution));
			trt__GetVideoSourcesResponse->VideoSources[n].Resolution->Width = oSysInfo->Oprofile[i].AVSC.windowwidth;//width;
			trt__GetVideoSourcesResponse->VideoSources[n].Resolution->Height = oSysInfo->Oprofile[i].AVSC.windowheight;
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging =(struct tt__ImagingSettings*)soap_malloc(soap, sizeof(struct tt__ImagingSettings));
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Brightness = (float*)soap_malloc(soap,sizeof(float));
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Brightness[0] = _Brightness;
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging->ColorSaturation = (float*)soap_malloc(soap,sizeof(float));
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging->ColorSaturation[0] = _Saturation;
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Contrast = (float*)soap_malloc(soap,sizeof(float));
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Contrast[0] = _Contrast;
			trt__GetVideoSourcesResponse->VideoSources[n].Imaging->IrCutFilter = (int *)soap_malloc(soap,sizeof(int));
			*trt__GetVideoSourcesResponse->VideoSources[n].Imaging->IrCutFilter = oSysInfo->imaging_conf[i].IrCutFilterMode; // dummy //{onv__IrCutFilterMode__ON = 0, onv__IrCutFilterMode__OFF = 1, onv__IrCutFilterMode__AUTO = 2}
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Sharpness = (float*)soap_malloc(soap,sizeof(float));
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Sharpness[0] = _Sharpness;
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->BacklightCompensation = (struct tt__BacklightCompensation*)soap_malloc(soap, sizeof(struct tt__BacklightCompensation));
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->BacklightCompensation->Mode = _Backlight;//Streaming_onvif->BacklightCompensationMode;  //{onv__BacklightCompensationMode__OFF = 0, onv__BacklightCompensationMode__ON = 1}
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->BacklightCompensation->Level = _BacklightCompensationLevel;//float
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Exposure = NULL;
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Focus = NULL;
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WideDynamicRange = (struct tt__WideDynamicRange*)soap_malloc(soap, sizeof(struct tt__WideDynamicRange));
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WideDynamicRange->Mode = _WideDynamicRange;   //{onv__WideDynamicMode__OFF = 0, onv__WideDynamicMode__ON = 1}
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WideDynamicRange->Level = oSysInfo->imaging_conf[i].WideDynamicMode;// dummy float
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WhiteBalance = (struct tt__WhiteBalance*)soap_malloc(soap, sizeof(struct tt__WhiteBalance));
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WhiteBalance->Mode = _WhiteBalance;	//{onv__WhiteBalanceMode__AUTO = 0, onv__WhiteBalanceMode__MANUAL = 1}
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WhiteBalance->CrGain = oSysInfo->imaging_conf[i].CrGain; // dummy
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WhiteBalance->CbGain = oSysInfo->imaging_conf[i].CbGain; // dummy
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WhiteBalance->__size = 0;
		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WhiteBalance->__any = NULL;
		dom_att(&trt__GetVideoSourcesResponse->VideoSources[n].Imaging->WhiteBalance->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		trt__GetVideoSourcesResponse->VideoSources[n].Imaging->Extension = NULL;
		dom_att(&trt__GetVideoSourcesResponse->VideoSources[n].Imaging->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		trt__GetVideoSourcesResponse->VideoSources[n].Extension = NULL;
		dom_att(&trt__GetVideoSourcesResponse->VideoSources[n].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		n++;
		}
	}

	return SOAP_OK; 

}


int __tmd__GetVideoOutputs(struct soap* soap, struct _tmd__GetVideoOutputs *tmd__GetVideoOutputs, struct _tmd__GetVideoOutputsResponse *tmd__GetVideoOutputsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__GetVideoSourceConfiguration(struct soap* soap, struct _tmd__GetVideoSourceConfiguration *tmd__GetVideoSourceConfiguration, struct _tmd__GetVideoSourceConfigurationResponse *tmd__GetVideoSourceConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	return SOAP_OK;
}

/**
 * @brief Get Video Output Configuration 
 * @This operation gets the video output configuration.
 *   
 * @param Request	: _tmd__GetVideoOutputConfiguration structure contains an empty message.  
 * @param Response	: _tmd__GetVideoOutputConfigurationResponse structure contains a list of structures describing all available video output configurations.
 * @return               : On success SOAP_OK.
 */
int __tmd__GetVideoOutputConfiguration(struct soap* soap, struct _tmd__GetVideoOutputConfiguration *tmd__GetVideoOutputConfiguration, struct _tmd__GetVideoOutputConfigurationResponse *tmd__GetVideoOutputConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int i;
	int Ptoken_exist = NOT_EXIST;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(tmd__GetVideoOutputConfiguration->VideoOutputToken, oSysInfo->Oprofile[i].AVOC.name) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}
	tmd__GetVideoOutputConfigurationResponse->__any = NULL;
	tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration = (struct tt__VideoOutputConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoOutputConfiguration));

	tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration->Name = oSysInfo->Oprofile[0].AVOC.name;
	tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration->UseCount = oSysInfo->Oprofile[0].AVOC.usecount;
	tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration->token = oSysInfo->Oprofile[0].AVOC.token;
	tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration->OutputToken = NULL;
	tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration->__size = 0;
	tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration->__any = NULL;
	dom_att(&tmd__GetVideoOutputConfigurationResponse->VideoOutputConfiguration->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}


/**
 * @brief Get Audio Source Configuration 
 * @This operation lists the configuration of an Audio Input.
 *   
 * @param Request	: _tmd__GetAudioSourceConfiguration structure contains the token of an AudioSource.    
 * @param Response	: _tmd__GetAudioSourceConfigurationResponse structure variables members contains the requested AudioSourceConfiguration with the matching token.
 * @return               : On success SOAP_OK.
 : On failure AudioSourceToken does not exist.

 */
int __tmd__GetAudioSourceConfiguration(struct soap* soap, struct _tmd__GetAudioSourceConfiguration *tmd__GetAudioSourceConfiguration, struct _tmd__GetAudioSourceConfigurationResponse *tmd__GetAudioSourceConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	int i, use, k;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(tmd__GetAudioSourceConfiguration->AudioSourceToken, oSysInfo->Oprofile[i].AASC.Asourcetoken) == 0)
		{
			break;
		}
	}
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoAudioSource","");
		return SOAP_FAULT;		
	}

	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].AASC.Atoken, oSysInfo->Oprofile[k].AASC.Atoken) == 0)
			use++;

	tmd__GetAudioSourceConfigurationResponse = (struct _tmd__GetAudioSourceConfigurationResponse*)soap_malloc(soap, sizeof(struct _tmd__GetAudioSourceConfigurationResponse));

	tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration = (struct tt__AudioSourceConfiguration*)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
	tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration->Name = oSysInfo->Oprofile[i].AASC.Aname;
	tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration->UseCount = use;
	tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration->token = oSysInfo->Oprofile[i].AASC.Atoken;
	tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration->SourceToken = oSysInfo->Oprofile[i].AASC.Asourcetoken;
	tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration->__size = 0;
	tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration->__any = NULL;
	dom_att(&tmd__GetAudioSourceConfigurationResponse->AudioSourceConfiguration->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	tmd__GetAudioSourceConfigurationResponse->__size = 0;
	tmd__GetAudioSourceConfigurationResponse->__any = NULL;

	return SOAP_OK;

}


int __tmd__GetAudioOutputConfiguration(struct soap* soap, struct _tmd__GetAudioOutputConfiguration *tmd__GetAudioOutputConfiguration, struct _tmd__GetAudioOutputConfigurationResponse *tmd__GetAudioOutputConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__SetVideoSourceConfiguration(struct soap* soap, struct _tmd__SetVideoSourceConfiguration *tmd__SetVideoSourceConfiguration, struct _tmd__SetVideoSourceConfigurationResponse *tmd__SetVideoSourceConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/**
 * @brief Set Video Output Configuration
 * @This operation modifies an video output configuration.
 *   
 * @param Request	: _tmd__SetVideoOutputConfiguration structure contains the credentials to be set for VideoOutput.    
 * @param Response	: _tmd__SetVideoOutputConfigurationResponse structure contains an empty message. 
 * @return               :  On success - SOAP_OK.
 On failure - The requested VideoOutput does not exist. or
 */
int __tmd__SetVideoOutputConfiguration(struct soap* soap, struct _tmd__SetVideoOutputConfiguration *tmd__SetVideoOutputConfiguration, struct _tmd__SetVideoOutputConfigurationResponse *tmd__SetVideoOutputConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	video_output_conf set_video_output;
	int i = 0;
	int Ptoken_exist = NOT_EXIST;
	int ret;


	if(tmd__SetVideoOutputConfiguration->Configuration->token != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(tmd__SetVideoOutputConfiguration->Configuration->token, oSysInfo->Oprofile[i].AVOC.token) == 0)
			{
				Ptoken_exist = EXIST;
				break;
			}
		}
	}
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}

	if(tmd__SetVideoOutputConfiguration->Configuration->token != NULL)
	{
		set_video_output.position = i;
		strcpy(set_video_output.video_output_conf_t.name, tmd__SetVideoOutputConfiguration->Configuration->Name);
		set_video_output.video_output_conf_t.usecount = tmd__SetVideoOutputConfiguration->Configuration->UseCount;
		strcpy(set_video_output.video_output_conf_t.token, tmd__SetVideoOutputConfiguration->Configuration->token);
	}
	ret = ControlSystemData(SFIELD_SET_VIDEOOUTPUT_CONF, &set_video_output, sizeof(set_video_output));
	tmd__SetVideoOutputConfigurationResponse->__any = NULL;
	if(ret != SUCCESS)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}

/**
 * @brief Get Video Source ConfigurationOptions 
 * @This operation returns the available options when the video source parameters are reconfigured If a video source configuration is specified, the options shall concern that
 particular configuration. 
 *   
 * @param Request	:_tmd__GetVideoSourceConfigurationOptions structure contains optional tokens of a video source configuration and a media profile.    
 * @param Response	:_tmd__GetVideoSourceConfigurationOptionsResponse structure contains the video configuration options.  
 * @return               : On success - SOAP_OK.
 : On failure - The requested Video Input does not exist

 */
int __tmd__GetVideoSourceConfigurationOptions(struct soap* soap, struct _tmd__GetVideoSourceConfigurationOptions *tmd__GetVideoSourceConfigurationOptions, struct _tmd__GetVideoSourceConfigurationOptionsResponse *tmd__GetVideoSourceConfigurationOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions = (struct tt__VideoSourceConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfigurationOptions));
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange = (struct tt__IntRectangleRange *)soap_malloc(soap, sizeof(struct tt__IntRectangleRange));
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->XRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->XRange->Min = 100;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->XRange->Max = 1000;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->YRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->YRange->Min = 100;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->YRange->Max = 1000;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->WidthRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->WidthRange->Min = 720;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->WidthRange->Max = 2592;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->HeightRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->HeightRange->Min = 480;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->BoundsRange->HeightRange->Max = 1920;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->__sizeVideoSourceTokensAvailable = 3;
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->VideoSourceTokensAvailable = (char **)soap_malloc(soap, sizeof(char) * 3);
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->VideoSourceTokensAvailable[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
	strcpy(tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->VideoSourceTokensAvailable[0], "JPEG");
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->VideoSourceTokensAvailable[1] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
	strcpy(tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->VideoSourceTokensAvailable[1], "MPEG4");
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->VideoSourceTokensAvailable[2] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
	strcpy(tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->VideoSourceTokensAvailable[2], "H264");
	tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->Extension = NULL;
	dom_att(&tmd__GetVideoSourceConfigurationOptionsResponse->VideoSourceConfigurationOptions->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	tmd__GetVideoSourceConfigurationOptionsResponse->__any = NULL;

	return SOAP_OK; 
}


int __tmd__GetVideoOutputConfigurationOptions(struct soap* soap, struct _tmd__GetVideoOutputConfigurationOptions *tmd__GetVideoOutputConfigurationOptions, struct _tmd__GetVideoOutputConfigurationOptionsResponse *tmd__GetVideoOutputConfigurationOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__GetAudioSourceConfigurationOptions(struct soap* soap, struct _tmd__GetAudioSourceConfigurationOptions *tmd__GetAudioSourceConfigurationOptions, struct _tmd__GetAudioSourceConfigurationOptionsResponse *tmd__GetAudioSourceConfigurationOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__GetAudioOutputConfigurationOptions(struct soap* soap, struct _tmd__GetAudioOutputConfigurationOptions *tmd__GetAudioOutputConfigurationOptions, struct _tmd__GetAudioOutputConfigurationOptionsResponse *tmd__GetAudioOutputConfigurationOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


/** 
 * @brief Get Relay outputs
 * @This operation returns all the available relay outputs.
 *   
 * @param Response  :_tmd__GetRelayOptionsResponse structure contains the relay tokens created in the camera along with their properties.  
 * @return               : On success - SOAP_OK.
 */

int __tmd__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i, duration;

	tds__GetRelayOutputsResponse->__sizeRelayOutputs = oSysInfo->nrelays;
	tds__GetRelayOutputsResponse->RelayOutputs = (struct tt__RelayOutput *)soap_malloc(soap, sizeof(struct tt__RelayOutput) * oSysInfo->nrelays);
	for(i = 0; i < oSysInfo->nrelays; i++)
	{   
		tds__GetRelayOutputsResponse->RelayOutputs[i].token = oSysInfo->onvif_relay[i].onvif_relay_in.token;

		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties = (struct tt__RelayOutputSettings *)soap_malloc(soap, sizeof(struct tt__RelayOutputSettings));    
		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->Mode = oSysInfo->onvif_relay[i].onvif_relay_in.relaymode;
		duration = oSysInfo->onvif_relay[i].onvif_relay_in.delaytime;
		switch(duration)
		{
			case 4:		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->DelayTime = 600*1000;
					break;
			case 3:		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->DelayTime = 300*1000;
					break;
			case 2:		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->DelayTime = 60*1000;
					break;
			case 1:		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->DelayTime = 30*1000;
					break;
			default:	tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->DelayTime = 10*1000;
					break;
		}
		tds__GetRelayOutputsResponse->RelayOutputs[i].Properties->IdleState = oSysInfo->onvif_relay[i].onvif_relay_in.idlestate;
		tds__GetRelayOutputsResponse->RelayOutputs[i].__size = 0;
		tds__GetRelayOutputsResponse->RelayOutputs[i].__any = NULL;
		dom_att(&tds__GetRelayOutputsResponse->RelayOutputs[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	}
	return SOAP_OK;
}

int __tmd__SetRelayOutputSettings(struct soap* soap, struct _tmd__SetRelayOutputSettings *tmd__SetRelayOutputSettings, struct _tmd__SetRelayOutputSettingsResponse *tmd__SetRelayOutputSettingsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo  = GetSysInfo();
	int i;
	long duration;
	relay_conf relay;

	for(i = 0 ;i < oSysInfo->nrelays; i++)
	{
		if(strcmp(tmd__SetRelayOutputSettings->RelayOutput->token, oSysInfo->onvif_relay[i].onvif_relay_in.token) == 0)
		{
			relay.position = i;
			break; 
		}
	}   
	if(i >= oSysInfo->nrelays)
	{       
		onvif_fault(soap,1,"ter:InvalidArgVal ", "ter:RelayToken","");
		return SOAP_FAULT;
	}   

	relay.relay_conf_in_t.relaymode_t = tmd__SetRelayOutputSettings->RelayOutput->Properties->Mode;
	relay.relay_conf_in_t.idlestate_t = tmd__SetRelayOutputSettings->RelayOutput->Properties->IdleState;

	if(tmd__SetRelayOutputSettings->RelayOutput->Properties->Mode == 1)
		relay.relay_conf_in_t.delaytime_t = 5;
	else
	{
		duration = (tmd__SetRelayOutputSettings->RelayOutput->Properties->DelayTime)/1000;
		if(duration!=10 && duration!=30 && duration!=60 && duration!=300 && duration!=600)
		{
			if(duration > 450)
				duration = 600;
			else if(duration > 180)
				duration = 300;
			else if(duration > 45)
				duration = 60;
			else if(duration > 20)
				duration = 30;
			else
				duration = 10;
		}
		switch(duration)
		{
			case 600:	relay.relay_conf_in_t.delaytime_t = 4;
					break;
			case 300:	relay.relay_conf_in_t.delaytime_t = 3;
					break;
			case 60:	relay.relay_conf_in_t.delaytime_t = 2;
					break;
			case 30:	relay.relay_conf_in_t.delaytime_t = 1;
					break;
			default:	relay.relay_conf_in_t.delaytime_t = 0;
					break;
		}
	}

	ControlSystemData(SFIELD_SET_RELAYS_CONF, (void*) &relay, sizeof(relay));
	ControlSystemData(SFIELD_SET_GIOOUTTYPE, (void*) &relay.relay_conf_in_t.idlestate_t, sizeof(int));
	ControlSystemData(SFIELD_SET_ALARMDURATION, (void*) &relay.relay_conf_in_t.delaytime_t, sizeof(long));

	return SOAP_OK;
}

/** 
 * @brief Set Relay Output state
 * @This operation sets the particualar relay referenced by relay token. Resetting the paritular GIO has to be completed in this function based on the relay  configs set by the client. Accordingly setting the GIO pins has to be implemented in this function.
 *   
 * @param Request   :_tmd__SetRelayOutputSettings structure contains the relay token and the state to which relay has to be set for.  
 * @return               : On success - SOAP_OK.

 */
int __tmd__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo  = GetSysInfo();
	int i = 0;
	relay_conf  relay;
	int token_exist = NOT_EXIST;
	for(i = 0 ;i < oSysInfo->nrelays; i++)
	{
		if(strcmp(tds__SetRelayOutputState->RelayOutputToken, oSysInfo->onvif_relay[i].onvif_relay_in.token) == 0)
		{
			relay.position = i;
			break; 
		}
	}   
	if(i >= oSysInfo->nrelays)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal ", "ter:RelayToken","");
		return SOAP_FAULT;
	}

	relay.logicalstate_t = tds__SetRelayOutputState->LogicalState;
	strcpy(relay.relay_conf_in_t.token_t, oSysInfo->onvif_relay[i].onvif_relay_in.token);
	relay.relay_conf_in_t.relaymode_t = oSysInfo->onvif_relay[i].onvif_relay_in.relaymode;
	relay.relay_conf_in_t.delaytime_t = oSysInfo->onvif_relay[i].onvif_relay_in.delaytime;
	relay.relay_conf_in_t.idlestate_t = oSysInfo->onvif_relay[i].onvif_relay_in.idlestate;
	ControlSystemData(SFIELD_SET_RELAY_LOGICALSTATE, (void*) &relay, sizeof(relay));
	ControlSystemData(SFIELD_SET_GIOOUTENABLE, (void*) &relay.logicalstate_t, sizeof(int));

	return SOAP_OK;
}

int __tmd__GetDigitalInputs(struct soap* soap, struct _tmd__GetDigitalInputs *tmd__GetDigitalInputs, struct _tmd__GetDigitalInputsResponse *tmd__GetDigitalInputsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__GetSerialPorts(struct soap* soap, struct _tmd__GetSerialPorts *tmd__GetSerialPorts, struct _tmd__GetSerialPortsResponse *tmd__GetSerialPortsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__GetSerialPortConfiguration(struct soap* soap, struct _tmd__GetSerialPortConfiguration *tmd__GetSerialPortConfiguration, struct _tmd__GetSerialPortConfigurationResponse *tmd__GetSerialPortConfigurationResponse) { 
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__SetSerialPortConfiguration(struct soap* soap, struct _tmd__SetSerialPortConfiguration *tmd__SetSerialPortConfiguration, struct _tmd__SetSerialPortConfigurationResponse *tmd__SetSerialPortConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__GetSerialPortConfigurationOptions(struct soap* soap, struct _tmd__GetSerialPortConfigurationOptions *tmd__GetSerialPortConfigurationOptions, struct _tmd__GetSerialPortConfigurationOptionsResponse *tmd__GetSerialPortConfigurationOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __tmd__SendReceiveSerialCommand(struct soap* soap, struct _tmd__SendReceiveSerialCommand *tmd__SendReceiveSerialCommand, struct _tmd__SendReceiveSerialCommandResponse *tmd__SendReceiveSerialCommandResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/**
 * @brief Get Medid Service Capabilities
 * @This message contains a request for GetServiceCapabilities.
 *   
 * @param Request	: _trt__GetServiceCapabilities structure contains a request for GetServiceCapabilities. 
 Returns a capailities of device services 			 
 * @param Response	: _trt__GetServicesCapabilitiesResponse contains the requested meida capabilities.
 * @return               : On success - SOAP_OK.
 */
int __trt__GetServiceCapabilities(struct soap* soap, struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities, struct _trt__GetServiceCapabilitiesResponse *trt__GetServiceCapabilitiesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	trt__GetServiceCapabilitiesResponse->Capabilities = (struct trt__Capabilities  *)soap_malloc(soap, sizeof(struct trt__Capabilities));

	trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities = (struct trt__ProfileCapabilities *)soap_malloc(soap, sizeof(struct trt__ProfileCapabilities));
	trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities->__size = 1;
	trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities->__any = NULL;
	trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities->MaximumNumberOfProfiles = (int*)soap_malloc(soap, sizeof(int));
	*trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities->MaximumNumberOfProfiles = 4;
	dom_att(&trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities = (struct trt__StreamingCapabilities *)soap_malloc(soap, sizeof(struct trt__StreamingCapabilities));
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->__size = 0;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->__any = NULL;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTPMulticast = soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTPMulticast  = xsd__boolean__true_ ;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORETCP = soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORETCP = xsd__boolean__false_;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = xsd__boolean__true_;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->NonAggregateControl = soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->NonAggregateControl = xsd__boolean__true_;
	dom_att(&trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetServiceCapabilitiesResponse->Capabilities->__size = 0;
	trt__GetServiceCapabilitiesResponse->Capabilities->__any = NULL;
	trt__GetServiceCapabilitiesResponse->Capabilities->SnapshotUri = soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->SnapshotUri	= xsd__boolean__true_;
	trt__GetServiceCapabilitiesResponse->Capabilities->Rotation =  soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->Rotation = xsd__boolean__false_;
	dom_att(&trt__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}



/**
 * @brief Get Video Source 
 * @This operation lists all available video sources for the device.
 *   
 * @param Request	: _trt__GetVideoSources structure contains an empty message.  
 * @param Response	: _trt__GetVideoSourcesResponse structure contains a list of structures describing all available video sources of the device.
 * @return               : On success SOAP_OK.
 : On failure Audio or Audio Outputs are not supported by the NVT.

 */
int __trt__GetVideoSources(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	trt__GetVideoSourcesResponse->__sizeVideoSources = 1;
	trt__GetVideoSourcesResponse->VideoSources = (struct tt__VideoSource *)soap_malloc(soap, sizeof(struct tt__VideoSource));
	trt__GetVideoSourcesResponse->VideoSources->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);	/* required attribute of type onv:ReferenceToken */
	strcpy(trt__GetVideoSourcesResponse->VideoSources->token, "IPNC_Video_Source0");	/* required attribute of type onv:ReferenceToken */
	trt__GetVideoSourcesResponse->VideoSources->Framerate = 1;  /* required element of type xsd:float */
	trt__GetVideoSourcesResponse->VideoSources->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution)); // required element of type onv:VideoResolution 
	trt__GetVideoSourcesResponse->VideoSources->Resolution->Width = 640; 
	trt__GetVideoSourcesResponse->VideoSources->Resolution->Height = 480; 
	trt__GetVideoSourcesResponse->VideoSources->Imaging = NULL; 
	trt__GetVideoSourcesResponse->VideoSources->Extension = NULL;
	dom_att(&trt__GetVideoSourcesResponse->VideoSources->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK; 

}


int __trt__GetAudioSources(struct soap* soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int i;


	trt__GetAudioSourcesResponse->__sizeAudioSources = oSysInfo->nprofile;
	trt__GetAudioSourcesResponse->AudioSources = (struct tt__AudioSource *)soap_malloc(soap, sizeof(struct tt__AudioSource) * (oSysInfo->nprofile));
	for(i=0; i < oSysInfo->nprofile; i++)
	{
		trt__GetAudioSourcesResponse->AudioSources[i].token = oSysInfo->Oprofile[i].AASC.Atoken;
		trt__GetAudioSourcesResponse->AudioSources[i].Channels = 1;
		trt__GetAudioSourcesResponse->AudioSources[i].__size = 0;
		trt__GetAudioSourcesResponse->AudioSources[i].__any = NULL;
		dom_att(&trt__GetAudioSourcesResponse->AudioSources[i].__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);
	}

	return SOAP_OK;
}

int __trt__GetAudioOutputs(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i;

	trt__GetAudioOutputsResponse = (struct _trt__GetAudioOutputsResponse*)soap_malloc(soap, oSysInfo->nprofile * sizeof(struct _trt__GetAudioOutputsResponse));

	trt__GetAudioOutputsResponse->__sizeAudioOutputs = oSysInfo->nprofile;
	trt__GetAudioOutputsResponse->AudioOutputs = (struct tt__AudioOutput*)soap_malloc(soap, trt__GetAudioOutputsResponse->__sizeAudioOutputs * sizeof(struct tt__AudioOutput));
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		trt__GetAudioOutputsResponse->AudioOutputs[i].token = oSysInfo->Oprofile[i].AAOC.AOtoken;
		trt__GetAudioOutputsResponse->AudioOutputs[i].__size = 0;
		trt__GetAudioOutputsResponse->AudioOutputs[i].__any = NULL;
		dom_att(&trt__GetAudioOutputsResponse->AudioOutputs[i].__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);
	}

	return SOAP_OK;
}


/**
 * @brief Set Audio Source Configuration
 * @This operation modifies an audio source configuration.
 *   
 * @param Request	: _tmd__SetAudioSourceConfiguration structure contains the credentials to be set for AudioSource.    
 * @param Response	: _tmd__SetAudioSourceConfigurationResponse structure contains an empty message. 
 * @return               :  On success - SOAP_OK.
 On failure - The requested AudioSource does not exist. or
 The configuration parameters are not possible to set.

 */
int __tmd__SetAudioSourceConfiguration(struct soap* soap, struct _tmd__SetAudioSourceConfiguration *tmd__SetAudioSourceConfiguration, struct _tmd__SetAudioSourceConfigurationResponse *tmd__SetAudioSourceConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	return SOAP_FAULT;		
}


int __tmd__SetAudioOutputConfiguration(struct soap* soap, struct _tmd__SetAudioOutputConfiguration *tmd__SetAudioOutputConfiguration, struct _tmd__SetAudioOutputConfigurationResponse *tmd__SetAudioOutputConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}



/** 
 * @brief Create Profile
 * @This operation creates a new empty media profile.
 *   
 * @param Request	:_trt__CreateProfile structure contains  the friendly Name of the Profile to create as well as an optional Token 
 parameter, specifying the unique identifier of the new media profile.
 @param Response	:_trt__CreateProfileResponse structure contains an empty Profile structure with no configuration entities.

 * @return               : On success - SOAP_OK.
 : On failure - A profile with the token ProfileToken already exists. or
 The maximum number of supported profiles has been reached.

 */
int __trt__CreateProfile(struct soap* soap, struct _trt__CreateProfile *trt__CreateProfile, struct _trt__CreateProfileResponse *trt__CreateProfileResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* pSysInfo = GetSysInfo();
	char tmp[10] = "";
	/* check the limit on number of profiles */
	if(pSysInfo->nprofile >= MAX_PROF)                                            	
	{
		onvif_fault(soap,0,"ter:Action ", "ter:MaxNVTProfiles","Maximum number reached"); 
		return SOAP_FAULT;
	}

	int i,ret;
	media_profile profile_acount;
	char Token[SMALL_INFO_LENGTH];                                                                     //<<27FEB2013>>

	if( (trt__CreateProfile->Token == NULL) || (trt__CreateProfile->Token == tmp) )                            //<<27FEB2013>>
		strcpy(Token, trt__CreateProfile->Name);
	else
		strcpy(Token, trt__CreateProfile->Token);

	/* check whether profile already exist or not */
	for(i = 0; i < pSysInfo->nprofile; i++)
	{
		if(strcmp(Token, pSysInfo->Oprofile[i].profiletoken) == 0)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ProfileExists","Profile token already exists");
			return SOAP_FAULT;
		}
	}


	/* save profile */
	strcpy(profile_acount.profile_name, trt__CreateProfile->Name);
	strcpy(profile_acount.profile_token, Token);

	ret = ControlSystemData(SFIELD_ADD_PROFILE, &profile_acount, sizeof(media_profile));

	if(ret == -1)
	{
		onvif_fault(soap,1, "ter:InvalidArgVal", "ter:AddProfileFailed","");
		return SOAP_FAULT;
	}

	pSysInfo = GetSysInfo();

	trt__CreateProfileResponse->Profile = (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));  

	trt__CreateProfileResponse->Profile->Name = pSysInfo->Oprofile[pSysInfo->nprofile - 1].profilename;                    
	trt__CreateProfileResponse->Profile->token = pSysInfo->Oprofile[pSysInfo->nprofile - 1].profiletoken;                 
	trt__CreateProfileResponse->Profile->fixed = xsd__boolean__false_;//0;
	trt__CreateProfileResponse->Profile->VideoSourceConfiguration = NULL;
	trt__CreateProfileResponse->Profile->AudioSourceConfiguration = NULL;
	trt__CreateProfileResponse->Profile->VideoEncoderConfiguration = NULL;
	trt__CreateProfileResponse->Profile->AudioEncoderConfiguration = NULL;
	trt__CreateProfileResponse->Profile->VideoAnalyticsConfiguration = NULL;
	trt__CreateProfileResponse->Profile->PTZConfiguration = NULL;
	trt__CreateProfileResponse->Profile->MetadataConfiguration = NULL;
	trt__CreateProfileResponse->Profile->Extension = NULL;
	dom_att(&trt__CreateProfileResponse->Profile->__anyAttribute, 0,NULL, NULL, NULL,NULL, soap);


	return SOAP_OK; 
}

/**
 * @brief Get Profile 
 * @This operation fetches the whole profile configuration. 
 *   
 * @param Request	:_trt__GetProfile structure contains the token to the requested profile.       
 * @param Response	:_trt__GetProfileResponse structure contains the Profile indicated by the Token parameter. 
 * @return               : On success -  SOAP_OK.
 : On failure - The requested profile token ProfileToken does not exist.
 */
int __trt__GetProfile(struct soap* soap, struct _trt__GetProfile *trt__GetProfile, struct _trt__GetProfileResponse *trt__GetProfileResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	char _IPAddr[LARGE_INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	int i, use, k;
	int _Bitrate = 0, _SampleRate = 0 ;
	int Ptoken_status = NOT_EXIST;

	/* Check if ProfileToken Exist or Not */
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetProfile->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_status = EXIST;
			break;
		}
	}
	if(!Ptoken_status)
	{
		onvif_fault(soap,1, "ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;
	}

	trt__GetProfileResponse->Profile = (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
	trt__GetProfileResponse->Profile->Name = oSysInfo->Oprofile[i].profilename;
	trt__GetProfileResponse->Profile->token = oSysInfo->Oprofile[i].profiletoken;
	trt__GetProfileResponse->Profile->fixed = (int *)soap_malloc(soap, sizeof(int)); //xsd__boolean__false_ = 0, xsd__boolean__true_ = 1
	*trt__GetProfileResponse->Profile->fixed = _false; //xsd__boolean__false_ = 0, xsd__boolean__true_ = 1
	dom_att(&trt__GetProfileResponse->Profile->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);



	/* VideoSourceConfiguration */
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].AVSC.Vtoken, oSysInfo->Oprofile[k].AVSC.Vtoken) == 0)
			use++;

	trt__GetProfileResponse->Profile->VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name = oSysInfo->Oprofile[i].AVSC.Vname;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount = use;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->token = oSysInfo->Oprofile[i].AVSC.Vtoken;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->y = oSysInfo->Oprofile[i].AVSC.windowy;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->x = oSysInfo->Oprofile[i].AVSC.windowx;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->width  = oSysInfo->Oprofile[i].AVSC.windowwidth;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->height = oSysInfo->Oprofile[i].AVSC.windowheight;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->__size = 0;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->__any = NULL;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Extension = NULL;
	dom_att(&trt__GetProfileResponse->Profile->VideoSourceConfiguration->__anyAttribute, 0,NULL, NULL, NULL,NULL, soap);

	/* AudioSourceConfiguration */
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].AASC.Atoken, oSysInfo->Oprofile[k].AASC.Atoken) == 0)
			use++;

	trt__GetProfileResponse->Profile->AudioSourceConfiguration = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->Name = oSysInfo->Oprofile[i].AASC.Aname;
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->UseCount = use;
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->token = oSysInfo->Oprofile[i].AASC.Atoken;
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->SourceToken = oSysInfo->Oprofile[i].AASC.Asourcetoken;
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->__size = 0;
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->AudioSourceConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	/*VideoEncoderConfiguration */
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].AESC.VEtoken, oSysInfo->Oprofile[k].AESC.VEtoken) == 0)
			use++;

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name = oSysInfo->Oprofile[i].AESC.VEname;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount = use;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token = oSysInfo->Oprofile[i].AESC.VEtoken;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding = oSysInfo->Oprofile[i].AESC.Vencoder;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Width = oSysInfo->Oprofile[i].AESC.Rwidth;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Height = oSysInfo->Oprofile[i].AESC.Rheight;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Quality = oSysInfo->Oprofile[i].AESC.VEquality; 
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit = oSysInfo->Oprofile[i].AESC.frameratelimit;   
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval = oSysInfo->Oprofile[i].AESC.encodinginterval;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit = oSysInfo->Oprofile[i].AESC.bitratelimit;

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->MPEG4->GovLength = oSysInfo->Oprofile[i].AESC.Mpeggovlength;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->MPEG4->Mpeg4Profile = 0;//SP = 0, ASP = 1;

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->GovLength = oSysInfo->Oprofile[i].AESC.H264govlength;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = 3;//Baseline = 0, Main = 1, High = 3

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Port = oSysInfo->Oprofile[i].AESC.Multicast_Conf.port;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->TTL = oSysInfo->Oprofile[i].AESC.Multicast_Conf.ttl;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->AutoStart = oSysInfo->Oprofile[i].AESC.Multicast_Conf.autostart;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->Type = oSysInfo->Oprofile[i].AESC.Multicast_Conf.nIPType;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	strcpy(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address , oSysInfo->Oprofile[i].AESC.Multicast_Conf.IPv4Addr);
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->__size = 0;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->__anyAttribute, 0,NULL, NULL, NULL,NULL, soap);
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->SessionTimeout = oSysInfo->Oprofile[i].AESC.sessiontimeout;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__size = 0;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__anyAttribute, 0,NULL, NULL, NULL,NULL, soap);

	/* AudioEncoderConfiguration */
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].AAEC.AEtoken, oSysInfo->Oprofile[k].AAEC.AEtoken) == 0)
			use++;

	//Fill Audio Encoder Configuration structure.    
	if(oSysInfo->Oprofile[i].AAEC.bitrate == 0)
		_Bitrate = 64;	   
	else if(oSysInfo->Oprofile[i].AAEC.bitrate == 1)
		_Bitrate = 128;   

	if( oSysInfo->Oprofile[i].AAEC.samplerate == 0)
		_SampleRate = 8;   
	else if(oSysInfo->Oprofile[i].AAEC.samplerate == 1)
		_SampleRate = 16;  
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Name = oSysInfo->Oprofile[i].AAEC.AEname;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->token = oSysInfo->Oprofile[i].AAEC.AEtoken;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->UseCount = oSysInfo->Oprofile[i].AAEC.AEusercount;//use;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Bitrate = _Bitrate;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->SampleRate = _SampleRate;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Encoding = oSysInfo->Oprofile[i].AAEC.AEencoding;

	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Port = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.port;//80;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->TTL = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.ttl;//1;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->AutoStart = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.autostart;//0;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->Type = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.nIPType;//0;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	strcpy(trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].AAEC.Multicast_Conf.IPv4Addr);
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->__size = 0;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->__anyAttribute, 0,NULL, NULL, NULL,NULL, soap);

	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->SessionTimeout = 60;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->__size= 0;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->AudioEncoderConfiguration->__anyAttribute, 0,NULL, NULL, NULL,NULL, soap);

	/* VideoAnalyticsConfiguration */
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].AVAC.VAtoken, oSysInfo->Oprofile[k].AVAC.VAtoken) == 0)
			use++;

	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->Name = oSysInfo->Oprofile[i].AVAC.VAname;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->UseCount = use;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->token = oSysInfo->Oprofile[i].AVAC.VAtoken;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 0;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = NULL;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->Extension = NULL;
	dom_att(&trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__RuleEngineConfiguration));
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 0;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = NULL;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Extension = NULL;
	dom_att(&trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->__size = 0;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	/* PTZConfiguration */    
	trt__GetProfileResponse->Profile->PTZConfiguration = NULL;

	/* MetadataConfiguration */
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].MDC.MDtoken, oSysInfo->Oprofile[k].MDC.MDtoken) == 0)
			use++;

	trt__GetProfileResponse->Profile->MetadataConfiguration = (struct tt__MetadataConfiguration *)soap_malloc(soap, sizeof(struct tt__MetadataConfiguration));
	trt__GetProfileResponse->Profile->MetadataConfiguration->Name = oSysInfo->Oprofile[i].MDC.MDname;// "metadata_config";
	trt__GetProfileResponse->Profile->MetadataConfiguration->UseCount = use;
	trt__GetProfileResponse->Profile->MetadataConfiguration->token = oSysInfo->Oprofile[i].MDC.MDtoken;//"metadata_config";
	trt__GetProfileResponse->Profile->MetadataConfiguration->Analytics = NULL;//oSysInfo->Oprofile[i].MDC.MDanalytics;//0;
	trt__GetProfileResponse->Profile->MetadataConfiguration->SessionTimeout =  oSysInfo->Oprofile[i].AESC.sessiontimeout;
	trt__GetProfileResponse->Profile->MetadataConfiguration->PTZStatus = NULL;

	trt__GetProfileResponse->Profile->MetadataConfiguration->Events = (struct tt__EventSubscription *)soap_malloc(soap, sizeof(struct tt__EventSubscription));
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter = (struct wsnt__FilterType *)soap_malloc(soap, sizeof(struct wsnt__FilterType));
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__size =  1;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->next =  NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->prnt = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->elts  = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
	//dom_att(&trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts, 0, "Dialect", "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet", NULL, NULL, soap);
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->next = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->nstr = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->name = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->name, "Dialect");
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->data = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->data, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->wide = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->atts->soap = soap;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy((char *)trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->nstr, "http://docs.oasis-open.org/wsn/b-2");
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
	strcpy(trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->name, "wsnt:TopicExpression");
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->data = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->wide = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->type = 0;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->node = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->head = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->tail = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->Filter->__any->soap = soap;

	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->SubscriptionPolicy = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->__size = 0;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Events->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->MetadataConfiguration->Events->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->Port = oSysInfo->Oprofile[i].MDC.VMC.port;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->TTL = oSysInfo->Oprofile[i].MDC.VMC.ttl;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->AutoStart = oSysInfo->Oprofile[i].MDC.VMC.autostart;//false
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->Address->Type = oSysInfo->Oprofile[i].MDC.VMC.nIPType;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].MDC.VMC.IPv4Addr);
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->Address->IPv6Address = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->__size = 0;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->__any = NULL;
	dom_att(&trt__GetProfileResponse->Profile->MetadataConfiguration->Multicast->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);
	trt__GetProfileResponse->Profile->MetadataConfiguration->__size = 0;
	trt__GetProfileResponse->Profile->MetadataConfiguration->__any = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->AnalyticsEngineConfiguration = NULL;
	trt__GetProfileResponse->Profile->MetadataConfiguration->Extension = NULL;
	dom_att(&trt__GetProfileResponse->Profile->MetadataConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	trt__GetProfileResponse->Profile->Extension = NULL;

	return SOAP_OK;
}

/** 
 * @brief Get Profiles
 * @Any endpoint can ask for the existing media profiles of an NVT using the GetProfiles command.
 *   
 * @param Request	:_trt__GetProfile structure contains an empty message.      
 * @param Response	:_trt__GetProfileResponse structure contains contains a configuration of all available profiles.
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults!

 */
int __trt__GetProfiles(struct soap* soap, struct _trt__GetProfiles *trt__GetProfiles, struct _trt__GetProfilesResponse *trt__GetProfilesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int i, k;
	int _Bitrate = 0, _SampleRate = 0;
	int use = 0;
	char _IPAddr[LARGE_INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);


	trt__GetProfilesResponse->Profiles =(struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile) * (oSysInfo->nprofile) ); 
	trt__GetProfilesResponse->__sizeProfiles = oSysInfo->nprofile;                                      

	for(i = 0; i < oSysInfo->nprofile; i++)                                
	{
		trt__GetProfilesResponse->Profiles[i].Name = oSysInfo->Oprofile[i].profilename;
		trt__GetProfilesResponse->Profiles[i].token = oSysInfo->Oprofile[i].profiletoken;
		trt__GetProfilesResponse->Profiles[i].fixed = (int *)soap_malloc(soap, sizeof(int)); //false_ = 0, true_ = 1
		*trt__GetProfilesResponse->Profiles[i].fixed = xsd__boolean__false_;                    
		dom_att(&trt__GetProfilesResponse->Profiles[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);


		/* VideoSourceConfiguration */
		use =0;
		for(k = 0; k < oSysInfo->nprofile; k++)
			if(strcmp(oSysInfo->Oprofile[i].AVSC.Vtoken, oSysInfo->Oprofile[k].AVSC.Vtoken) == 0)
				use++;

		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));

		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name = oSysInfo->Oprofile[i].AVSC.Vname;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->UseCount = use;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token = oSysInfo->Oprofile[i].AVSC.Vtoken;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->y = oSysInfo->Oprofile[i].AVSC.windowy;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->x = oSysInfo->Oprofile[i].AVSC.windowx;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->width  = oSysInfo->Oprofile[i].AVSC.windowwidth;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->height = oSysInfo->Oprofile[i].AVSC.windowheight;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->__size = 0;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->__any = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Extension = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

		/* AudioSourceConfiguration */
		use = 0;
		for(k = 0; k < oSysInfo->nprofile; k++)
			if(strcmp(oSysInfo->Oprofile[i].AASC.Atoken, oSysInfo->Oprofile[k].AASC.Atoken) == 0)
				use++;

		trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
		trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->Name = oSysInfo->Oprofile[i].AASC.Aname;
		trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->UseCount = use;
		trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->token = oSysInfo->Oprofile[i].AASC.Atoken;
		trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->SourceToken = oSysInfo->Oprofile[i].AASC.Asourcetoken;
		trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->__size = 0;
		trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

		/* VideoEncoderConfiguration */
		use = 0;
		for(k = 0; k < oSysInfo->nprofile; k++)
			if(strcmp(oSysInfo->Oprofile[i].AESC.VEtoken, oSysInfo->Oprofile[k].AESC.VEtoken) == 0)
				use++;

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name = oSysInfo->Oprofile[i].AESC.VEname;	
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->UseCount = use;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token = oSysInfo->Oprofile[i].AESC.VEtoken;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Encoding = oSysInfo->Oprofile[i].AESC.Vencoder;//JPEG = 0, MPEG4 = 1, H264 = 2;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Width = oSysInfo->Oprofile[i].AESC.Rwidth;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Height = oSysInfo->Oprofile[i].AESC.Rheight;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Quality = oSysInfo->Oprofile[i].AESC.VEquality; 

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit = oSysInfo->Oprofile[i].AESC.frameratelimit;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->EncodingInterval = oSysInfo->Oprofile[i].AESC.encodinginterval;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->BitrateLimit = oSysInfo->Oprofile[i].AESC.bitratelimit;

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->MPEG4->GovLength = oSysInfo->Oprofile[i].AESC.Mpeggovlength;
		if(i == 0)	
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->MPEG4->Mpeg4Profile = 0; //SP = 0, ASP = 1
		else
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->MPEG4->Mpeg4Profile = oSysInfo->Oprofile[i].AESC.Mpegprofile; //SP = 0, ASP = 1
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->GovLength = oSysInfo->Oprofile[i].AESC.H264govlength;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->H264Profile = 3;//Baseline = 0, Main = 1, High = 3

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Port = oSysInfo->Oprofile[i].AESC.Multicast_Conf.port;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->TTL = oSysInfo->Oprofile[i].AESC.Multicast_Conf.ttl;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->AutoStart = oSysInfo->Oprofile[i].AESC.Multicast_Conf.autostart;//false
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->Type = oSysInfo->Oprofile[i].AESC.Multicast_Conf.nIPType;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].AESC.Multicast_Conf.IPv4Addr);
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->SessionTimeout = oSysInfo->Oprofile[i].AESC.sessiontimeout;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->__size = 0;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->__anyAttribute, 0, NULL,NULL,NULL,NULL, soap);
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->__size = 0;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->__anyAttribute, 0, NULL,NULL,NULL,NULL, soap);

		/* AudioEncoderConfiguration */
		use = 0;
		for(k = 0; k < oSysInfo->nprofile; k++)
			if(strcmp(oSysInfo->Oprofile[i].AAEC.AEtoken, oSysInfo->Oprofile[k].AAEC.AEtoken) == 0)
				use++;

		//Fill Audio Encoder Configuration structure.  
		if(oSysInfo->Oprofile[i].AAEC.bitrate == 0)
			_Bitrate = 64;	   
		else if(oSysInfo->Oprofile[i].AAEC.bitrate == 1)
			_Bitrate = 128;   

		if( oSysInfo->Oprofile[i].AAEC.samplerate == 0)
			_SampleRate = 8;   
		else if(oSysInfo->Oprofile[i].AAEC.samplerate == 1)
			_SampleRate = 16;  

		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Name = oSysInfo->Oprofile[i].AAEC.AEname;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->token = oSysInfo->Oprofile[i].AAEC.AEtoken;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->UseCount = oSysInfo->Oprofile[i].AAEC.AEusercount;//use;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Bitrate = _Bitrate;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->SampleRate = _SampleRate;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Encoding = oSysInfo->Oprofile[i].AAEC.AEencoding;

		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Port = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.port;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->TTL = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.ttl;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->AutoStart = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.autostart;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address->Type = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.nIPType;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].AAEC.Multicast_Conf.IPv4Addr);
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->__size = 0;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->SessionTimeout = 60;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->__size= 0;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->__anyAttribute, 0,NULL, NULL, NULL,NULL, soap);

		/* VideoAnalyticsConfiguration */
		use = 0;
		for(k = 0; k < oSysInfo->nprofile; k++)
			if(strcmp(oSysInfo->Oprofile[i].AVAC.VAtoken, oSysInfo->Oprofile[k].AVAC.VAtoken) == 0)
				use++;

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *) soap_malloc(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->Name = oSysInfo->Oprofile[i].AVAC.VAname; 
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->UseCount = use;                  
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->token = oSysInfo->Oprofile[i].AVAC.VAtoken;   
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 0;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->Extension = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__anyAttribute, 0,NULL,NULL,NULL,NULL, soap);

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__RuleEngineConfiguration));
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 0;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Extension = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->__anyAttribute, 0,NULL,NULL,NULL,NULL, soap);
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->__size = 0;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->__anyAttribute, 0,NULL,NULL,NULL,NULL, soap);


		/* PTZConfiguration */	
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration = NULL;
		/* MetadataConfiguration */
		use = 0;
		for(k = 0; k < oSysInfo->nprofile; k++)
			if(strcmp(oSysInfo->Oprofile[i].MDC.MDtoken, oSysInfo->Oprofile[k].MDC.MDtoken) == 0)
				use++;

#if METADATA_ENABLE
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration = (struct tt__MetadataConfiguration *)soap_malloc(soap,sizeof(struct tt__MetadataConfiguration));
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Name = oSysInfo->Oprofile[i].MDC.MDname;// "metadata_config";
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->UseCount = use;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->token = oSysInfo->Oprofile[i].MDC.MDtoken;//"metadata_config";
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Analytics = NULL;//oSysInfo->Oprofile[i].MDC.MDanalytics;//0;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->SessionTimeout = oSysInfo->Oprofile[i].AESC.sessiontimeout;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->PTZStatus = NULL;

		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events = (struct tt__EventSubscription *)soap_malloc(soap, sizeof(struct tt__EventSubscription));
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter = (struct wsnt__FilterType *)soap_malloc(soap, sizeof(struct wsnt__FilterType));
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__size =  1;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->next =  NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->prnt = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->elts  = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
		//dom_att(&trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts, 0, "Dialect", "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet", NULL, NULL, soap);
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->next = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->nstr = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->name = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->name, "Dialect");
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->data = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->data, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->wide = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->atts->soap = soap;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy((char *)trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->nstr, "http://docs.oasis-open.org/wsn/b-2");
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
		strcpy(trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->name, "wsnt:TopicExpression");
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->data = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->wide = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->type = 0;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->node = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->head = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->tail = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->Filter->__any->soap = soap;

		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->SubscriptionPolicy = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->__size = 0;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Events->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->Port = oSysInfo->Oprofile[i].MDC.VMC.port;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->TTL = oSysInfo->Oprofile[i].MDC.VMC.ttl;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->AutoStart = oSysInfo->Oprofile[i].MDC.VMC.autostart;//false
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->Address->Type = oSysInfo->Oprofile[i].MDC.VMC.nIPType;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->Address->IPv4Address,oSysInfo->Oprofile[i].MDC.VMC.IPv4Addr );
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->Address->IPv6Address = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->__size = 0;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->__any = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Multicast->__anyAttribute,0,NULL,NULL,NULL,NULL, soap);
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->__size = 0;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->__any = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->AnalyticsEngineConfiguration = NULL;
		trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->Extension = NULL;
		dom_att(&trt__GetProfilesResponse->Profiles[i].MetadataConfiguration->__anyAttribute, 0,NULL,NULL,NULL,NULL, soap);


#endif
		trt__GetProfilesResponse->Profiles[i].Extension = NULL;	
	}

	return SOAP_OK;
}

/**
 * @brief Add Video Encoder Configuration 
 * @This operation adds a VideoEncoderConfiguration to an existing media profile. If a configuration exists in the media profile, it will be replaced. 
 *   
 * @param Request	:_trt__AddVideoEncoderConfiguration structure contains a reference to the VideoEncoderConfiguration to add and the Profile where it shall be added.
 * @param Response	:_trt__AddVideoEncoderConfigurationResponse structure contains an empty message.
 * @return               : On success - SOAP_OK.
 : On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 11.2.5).
 */
int __trt__AddVideoEncoderConfiguration(struct soap* soap, struct _trt__AddVideoEncoderConfiguration *trt__AddVideoEncoderConfiguration, struct _trt__AddVideoEncoderConfigurationResponse *trt__AddVideoEncoderConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	int Ptoken_exist = NOT_EXIST;
	int i;
	video_encoder_conf video_encoder_t;

	memset(&video_encoder_t, 0, sizeof(video_encoder_t));
	SysInfo *oSysInfo = GetSysInfo();

	for(i = 0; i < oSysInfo->nprofile; i++)                                                            
	{
		if(strcmp(trt__AddVideoEncoderConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			video_encoder_t.position = i;
			break;
		}
	}

	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;		
	}
	else if((Ptoken_exist))
	{
		video_encoder_t.video_encoder_conf_in_t.VEusercount_t = 1;
		strcpy(video_encoder_t.video_encoder_conf_in_t.VEtoken_t, trt__AddVideoEncoderConfiguration->ConfigurationToken);
		video_encoder_t.position = i;


		char _IPAddr[INFO_LENGTH];
		NET_IPV4 ip;
		ip.int32 = net_get_ifaddr(ETH_NAME);
		sprintf(_IPAddr, "http://%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

		int numberofstream = oget_numberofstream();
		struct tt__VideoEncoderConfiguration * Configurations = 
			(struct tt__VideoEncoderConfiguration *)soap_malloc(soap, numberofstream * sizeof(struct tt__VideoEncoderConfiguration));
		int i = 0;
		for(i = 0; i < numberofstream; i++)
		{
			int bitrate = oget_bitrate(i);
			int ratecontrol = oget_ratecontrol(i);
			int framerate = oget_framerate(i);
			int width = oget_resolutionwidth(i);
			int height = oget_resolutionheight(i);
			int govlength = oget_ipratio(i);

			if(i == 0) {
				Configurations[i].Name = "DE_Name1"; 
				Configurations[i].token = "DE_Token1";
			} else if(i == 1) {
				Configurations[i].Name = "DE_Name2";
				Configurations[i].token = "DE_Token2";
			} else if(i == 2) {
				Configurations[i].Name = "DE_Name3"; 
				Configurations[i].token = "DE_Token3";
			} else {
				Configurations[i].Name = "DE_Name"; 
				Configurations[i].token = "DE_Token"; 
			}
			Configurations[i].UseCount = 1;
			Configurations[i].Encoding = oget_encoding(i);
			Configurations[i].Quality = oSysInfo->Oprofile[0].AESC.VEquality; 
			Configurations[i].SessionTimeout = 60;
			Configurations[i].Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
			Configurations[i].Resolution->Width = width;
			Configurations[i].Resolution->Height = height;
			Configurations[i].RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
			Configurations[i].RateControl->FrameRateLimit = framerate;
			Configurations[i].RateControl->EncodingInterval = ratecontrol; //0:vbr 1:cbr
			Configurations[i].RateControl->BitrateLimit = bitrate;
			Configurations[i].MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
			Configurations[i].MPEG4->GovLength = govlength;
			Configurations[i].MPEG4->Mpeg4Profile = 0;
			Configurations[i].H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
			Configurations[i].H264->GovLength = govlength;
			Configurations[i].H264->H264Profile = 0;//Baseline = 0, Main = 1, High = 3
			Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
			Configurations[i].Multicast->Port = oSysInfo->Oprofile[i].AESC.Multicast_Conf.port;
			Configurations[i].Multicast->TTL = oSysInfo->Oprofile[i].AESC.Multicast_Conf.ttl;
			Configurations[i].Multicast->AutoStart = oSysInfo->Oprofile[i].AESC.Multicast_Conf.autostart;
			Configurations[i].Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
			Configurations[i].Multicast->Address->Type = oSysInfo->Oprofile[i].AESC.Multicast_Conf.nIPType;;
			Configurations[i].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(Configurations[i].Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].AESC.Multicast_Conf.IPv4Addr);
			Configurations[i].Multicast->Address->IPv6Address = NULL;
			Configurations[i].Multicast->__size = 0;
			Configurations[i].Multicast->__any = NULL;
			dom_att(&Configurations[i].Multicast->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
			Configurations[i].__size = 0;
			Configurations[i].__any = NULL;
			dom_att(&Configurations[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

			if(strcmp(video_encoder_t.video_encoder_conf_in_t.VEtoken_t, Configurations[i].token ) == 0)
			{
				strcpy(video_encoder_t.video_encoder_conf_in_t.VEname_t, Configurations[i].Name);
				video_encoder_t.video_encoder_conf_in_t.VEquality_t = Configurations[i].Quality;
				video_encoder_t.video_encoder_conf_in_t.Vencoder_t = Configurations[i].Encoding;             //video encoder for jpeg, mpeg, h.264
				video_encoder_t.video_encoder_conf_in_t.Rwidth_t = Configurations[i].Resolution->Width;               //resolution width
				video_encoder_t.video_encoder_conf_in_t.Rheight_t = Configurations[i].Resolution->Height;                //resolution height
				video_encoder_t.video_encoder_conf_in_t.frameratelimit_t = Configurations[i].RateControl->FrameRateLimit;        //frameratelimit
				video_encoder_t.video_encoder_conf_in_t.encodinginterval_t = Configurations[i].RateControl->EncodingInterval;       //encodinginterval
				video_encoder_t.video_encoder_conf_in_t.bitratelimit_t = Configurations[i].RateControl->BitrateLimit;            //bitratelimit
				video_encoder_t.video_encoder_conf_in_t.Mpeggovlength_t = Configurations[i].MPEG4->GovLength ;              //mpeg GoVLength
				video_encoder_t.video_encoder_conf_in_t.Mpegprofile_t =  Configurations[i].MPEG4->Mpeg4Profile;                 //mpegprofile SP/ASP
				video_encoder_t.video_encoder_conf_in_t.H264govlength_t = Configurations[i].H264->GovLength;               //H264 govlength
				video_encoder_t.video_encoder_conf_in_t.H264profile_t = Configurations[i].H264->H264Profile;                 //H264 profile  baseline/main/extended/high
				video_encoder_t.video_encoder_conf_in_t.sessiontimeout_t = Configurations[i].SessionTimeout;
				video_encoder_t.video_encoder_conf_in_t.Multicast_Conf_t.nIPType_t =  Configurations[i].Multicast->Address->Type;
				strcpy(video_encoder_t.video_encoder_conf_in_t.Multicast_Conf_t.IPv4Addr_t,  Configurations[i].Multicast->Address->IPv4Address);
				video_encoder_t.video_encoder_conf_in_t.Multicast_Conf_t.port_t  =  Configurations[i].Multicast->Port;
				video_encoder_t.video_encoder_conf_in_t.Multicast_Conf_t.ttl_t  =  Configurations[i].Multicast->TTL;
				video_encoder_t.video_encoder_conf_in_t.Multicast_Conf_t.autostart_t =  Configurations[i].Multicast->AutoStart;
			}
		}
		ControlSystemData(SFIELD_ADD_VIDEOENCODER_CONF, &video_encoder_t, sizeof(video_encoder_t));
	}

	return SOAP_OK;
}


/**
 * @brief Add VIdeo Source Configuration 
 * @This operation adds a VideoEncoderConfiguration to an existing media profile.
 *   
 * @param Request	:_trt__AddVideoSourceConfiguration structure contains a reference to the VideoEncoderConfiguration to add and the Profile where it shall be added.
 * @param Response	:_trt__AddVideoSourceConfigurationResponse structure contains an empty message.
 * @return               : On success - SOAP_OK.
 : On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 11.2.4).
 */
int __trt__AddVideoSourceConfiguration(struct soap* soap, struct _trt__AddVideoSourceConfiguration *trt__AddVideoSourceConfiguration, struct _trt__AddVideoSourceConfigurationResponse *trt__AddVideoSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	int Ptoken_exist=NOT_EXIST;
	int i = 0;
	int num_tokens = 0;
	video_source_conf video_source_t;
	SysInfo *oSysInfo = GetSysInfo();

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__AddVideoSourceConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			video_source_t.position = i;
			break;
		}
	}

	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;		
	}
	else if((Ptoken_exist))
	{
		int k = 0, found = 0;
		for(k = 0; k < oSysInfo->nprofile; k++)
		{
			if(strcmp( oSysInfo->Oprofile[k].AVSC.Vtoken, trt__AddVideoSourceConfiguration->ConfigurationToken) == 0)
			{
				strcpy(video_source_t.video_source_conf_in_t.Vname_t, oSysInfo->Oprofile[k].AVSC.Vname);
				video_source_t.video_source_conf_in_t.Vcount_t = oSysInfo->Oprofile[k].AVSC.Vcount + 1;
				oSysInfo->Oprofile[k].AVSC.Vcount ++;
				strcpy(video_source_t.video_source_conf_in_t.Vtoken_t, oSysInfo->Oprofile[k].AVSC.Vtoken);
				strcpy(video_source_t.video_source_conf_in_t.Vsourcetoken_t, oSysInfo->Oprofile[k].AVSC.Vsourcetoken);
				video_source_t.video_source_conf_in_t.windowx_t = oSysInfo->Oprofile[k].AVSC.windowx;
				video_source_t.video_source_conf_in_t.windowy_t = oSysInfo->Oprofile[k].AVSC.windowy;
				video_source_t.video_source_conf_in_t.windowwidth_t = oSysInfo->Oprofile[k].AVSC.windowwidth;
				video_source_t.video_source_conf_in_t.windowheight_t = oSysInfo->Oprofile[k].AVSC.windowheight;
				found = 1;
				break;                   
			}
		}
		if(found == 0)
		{
			video_source_t.video_source_conf_in_t.Vcount_t = num_tokens;
			strcpy(video_source_t.video_source_conf_in_t.Vtoken_t, trt__AddVideoSourceConfiguration->ConfigurationToken);
		}
	}

	ControlSystemData(SFIELD_ADD_VIDEOSOURCE_CONF, &video_source_t, sizeof(video_source_t));

	return SOAP_OK;
}


int __trt__AddAudioEncoderConfiguration(struct soap* soap, struct _trt__AddAudioEncoderConfiguration *trt__AddAudioEncoderConfiguration, struct _trt__AddAudioEncoderConfigurationResponse *trt__AddAudioEncoderConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int Ptoken_exist = NOT_EXIST;
	int i;
	Add_AudioEncoder_Configuration_tmp add_audio_encoder;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__AddAudioEncoderConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			add_audio_encoder.position = i;
			break;
		}
	}

	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;		
	}

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__AddAudioEncoderConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AAEC.AEtoken) == 0)
		{
			break;
		}
	}
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist"); 
		return SOAP_FAULT;		
	}

	strcpy(add_audio_encoder.Add_AudioEncoder_in.AEname_t, trt__AddAudioEncoderConfiguration->ConfigurationToken);
	strcpy(add_audio_encoder.Add_AudioEncoder_in.AEtoken_t, trt__AddAudioEncoderConfiguration->ConfigurationToken);
	add_audio_encoder.Add_AudioEncoder_in.AEusercount_t = oSysInfo->Oprofile[i].AAEC.AEusercount;//1;
	add_audio_encoder.Add_AudioEncoder_in.bitrate_t = oSysInfo->Oprofile[i].AAEC.bitrate;
	add_audio_encoder.Add_AudioEncoder_in.samplerate_t = oSysInfo->Oprofile[i].AAEC.samplerate;
	add_audio_encoder.Add_AudioEncoder_in.AEencoding_t = oSysInfo->Oprofile[i].AAEC.AEencoding;

	ControlSystemData(SFIELD_ADD_AUDIOENCODER_CONF, &add_audio_encoder, sizeof(add_audio_encoder));


	return SOAP_OK;
}

int __trt__AddAudioSourceConfiguration(struct soap *soap, struct _trt__AddAudioSourceConfiguration *trt__AddAudioSourceConfiguration, struct _trt__AddAudioSourceConfigurationResponse *trt__AddAudioSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int Ptoken_exist = NOT_EXIST;
	int i ;
	int flag = 0;
	Add_AudioSource_Configuration_tmp add_audio_source;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__AddAudioSourceConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			add_audio_source.position = i;
			break;
		}
	}
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;
	}

	Ptoken_exist = NOT_EXIST;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__AddAudioSourceConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AASC.Atoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist"); 
		return SOAP_FAULT;
	}

	strcpy(add_audio_source.Add_AudioSource_in.Atoken_t, trt__AddAudioSourceConfiguration->ConfigurationToken);

	ControlSystemData(SFIELD_ADD_AUDIOSOURCE_CONF, &add_audio_source, sizeof(add_audio_source));

	return SOAP_OK;
}


int __trt__AddPTZConfiguration(struct soap* soap, struct _trt__AddPTZConfiguration *trt__AddPTZConfiguration, struct _trt__AddPTZConfigurationResponse *trt__AddPTZConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __trt__AddVideoAnalyticsConfiguration(struct soap* soap, struct _trt__AddVideoAnalyticsConfiguration *trt__AddVideoAnalyticsConfiguration, struct _trt__AddVideoAnalyticsConfigurationResponse *trt__AddVideoAnalyticsConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	return SOAP_OK;
}


int __trt__AddMetadataConfiguration(struct soap* soap, struct _trt__AddMetadataConfiguration *trt__AddMetadataConfiguration, struct _trt__AddMetadataConfigurationResponse *trt__AddMetadataConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int Ptoken_exist = NOT_EXIST;
	int i;
	Metadata_configuration_tmp metadata;
	memset(&metadata, 0, sizeof(metadata));

	for(i=0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__AddMetadataConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			metadata.position = i;
			break;
		}
	}

	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;		
	}

	Ptoken_exist = NOT_EXIST;
	for(i=0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__AddMetadataConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].MDC.MDtoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;		
	}

	strcpy(metadata.MDC_in.MDtoken_t, trt__AddMetadataConfiguration->ConfigurationToken);
	metadata.MDC_in.MDusecount_t = oSysInfo->Oprofile[i].MDC.MDusecount;
	strcpy(metadata.MDC_in.MDtoken_t, trt__AddMetadataConfiguration->ConfigurationToken);
	metadata.MDC_in.MDanalytics_t = oSysInfo->Oprofile[i].MDC.MDanalytics;
	metadata.MDC_in.sessiontimeout_t = oSysInfo->Oprofile[i].MDC.sessiontimeout;

	ControlSystemData(SFIELD_ADD_METADATA, &metadata, sizeof(metadata));

	return SOAP_OK;
}


int __trt__AddAudioOutputConfiguration(struct soap* soap, struct _trt__AddAudioOutputConfiguration *trt__AddAudioOutputConfiguration, struct _trt__AddAudioOutputConfigurationResponse *trt__AddAudioOutputConfigurationResponse) {	
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" ); 
	return SOAP_FAULT;	
}


int __trt__AddAudioDecoderConfiguration(struct soap* soap, struct _trt__AddAudioDecoderConfiguration *trt__AddAudioDecoderConfiguration, struct _trt__AddAudioDecoderConfigurationResponse *trt__AddAudioDecoderConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" ); 
	return SOAP_FAULT;	
}

/**
 * @brief Remove Video Encoder Configuration
 * @This operation removes a VideoEncoderConfiguration from an existing media profile. If the media profile does not contain a VideoEncoderConfiguration, the operation has no
 effect.
 * @params request            : _trt__RemoveVideoEncoderConfiguration structure Contains a reference to the media profile from which the VideoEncoderConfiguration shall be 
 removed.
 * @params response           : _trt__RemoveVideoEncoderConfigurationResponse contains an empty message.
 * @return                    : On success - SOAP_OK.
 : On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 11.2.14)

 */
int __trt__RemoveVideoEncoderConfiguration(struct soap* soap, struct _trt__RemoveVideoEncoderConfiguration *trt__RemoveVideoEncoderConfiguration, struct _trt__RemoveVideoEncoderConfigurationResponse *trt__RemoveVideoEncoderConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	video_encoder_conf set_video_encoder_conf;
	memset(&set_video_encoder_conf, 0, sizeof(video_encoder_conf));
	int i;
	int ret;
	int Ptoken_exist = NOT_EXIST;
	for(i = 0; i < oSysInfo->nprofile; i++)   
	{
		if(strcmp(trt__RemoveVideoEncoderConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken)==0)
		{
			Ptoken_exist = EXIST;
			set_video_encoder_conf.position = i;	
			ret = ControlSystemData(SFIELD_DEL_VIDEOENCODER_CONF, &set_video_encoder_conf, sizeof(set_video_encoder_conf));
			if(ret != SUCCESS)
			{		
				onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","");
				return SOAP_FAULT;
			}
		}
	}
	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}


/**
 * @brief Remove Video Source Configuration
 * @This operation removes a VideoSourceConfiguration from an existing media profile.
 * @params   request       :_trt__RemoveVideoSourceConfiguration structure Contains a reference to the media profile from which the VideoSourceConfiguration shall be removed.
 @params   response      :_trt__RemoveVideoSourceConfigurationResponse structure contains an empity message.
 @return                 : On success - SOAP_OK.
 On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 11.2.13)

 */
int __trt__RemoveVideoSourceConfiguration(struct soap* soap, struct _trt__RemoveVideoSourceConfiguration *trt__RemoveVideoSourceConfiguration, struct _trt__RemoveVideoSourceConfigurationResponse *trt__RemoveVideoSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i;
	int ret;
	int Ptoken_exist = NOT_EXIST;
	video_source_conf set_video_source_conf;
	memset(&set_video_source_conf, 0, sizeof(video_source_conf));
	for(i = 0; i < oSysInfo->nprofile; i++)                                        
	{
		if(strcmp(trt__RemoveVideoSourceConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			set_video_source_conf.position = i;	
			set_video_source_conf.video_source_conf_in_t.Vcount_t = 0; 
			ret = ControlSystemData(SFIELD_SET_VIDEOSOURCE_CONF, &set_video_source_conf, sizeof(set_video_source_conf));
			if(ret != SUCCESS)
			{		
				onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","ConfigurationConflict");
				return SOAP_FAULT;
			}
		}
	}

	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","The requested profile token ProfileToken does not exist");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}


int __trt__RemoveAudioEncoderConfiguration(struct soap* soap, struct _trt__RemoveAudioEncoderConfiguration *trt__RemoveAudioEncoderConfiguration, struct _trt__RemoveAudioEncoderConfigurationResponse *trt__RemoveAudioEncoderConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i, k; 
	int profile_exist = 0;
	int flag = 0;
	Add_AudioEncoder_Configuration_tmp remove_audio_encoder;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__RemoveAudioEncoderConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			profile_exist = EXIST;
			remove_audio_encoder.position = i;
			break;
		}
	}
	if(!profile_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}
	/*
	   use = 0;
	   for(k = 0; k < oSysInfo->nprofile; k++)
	   if(strcmp(oSysInfo->Oprofile[i].AAEC.AEtoken, oSysInfo->Oprofile[k].AAEC.AEtoken) == 0)
	   use++;

	   strcpy(remove_audio_encoder.Add_AudioEncoder_in.AEname_t, oSysInfo->Oprofile[i].AAEC.AEname);
	   strcpy(remove_audio_encoder.Add_AudioEncoder_in.AEtoken_t, trt__RemoveAudioEncoderConfiguration->ProfileToken);
	   remove_audio_encoder.Add_AudioEncoder_in.AEusercount_t = oSysInfo->Oprofile[i].AAEC.AEusercount;//use;
	   remove_audio_encoder.Add_AudioEncoder_in.bitrate_t = oSysInfo->Oprofile[i].AAEC.bitrate;
	   remove_audio_encoder.Add_AudioEncoder_in.samplerate_t = oSysInfo->Oprofile[i].AAEC.samplerate;
	   remove_audio_encoder.Add_AudioEncoder_in.AEencoding_t = oSysInfo->Oprofile[i].AAEC.AEencoding;
	 */
	ControlSystemData(SFIELD_DEL_AUDIO_ENCODER, &remove_audio_encoder, sizeof(remove_audio_encoder));

	return SOAP_OK;
}


int __trt__RemoveAudioSourceConfiguration(struct soap* soap, struct _trt__RemoveAudioSourceConfiguration *trt__RemoveAudioSourceConfiguration, struct _trt__RemoveAudioSourceConfigurationResponse *trt__RemoveAudioSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i;
	int profile_exist = 0;
	Add_AudioSource_Configuration_tmp remove_audio_source;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__RemoveAudioSourceConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			profile_exist = EXIST;
			remove_audio_source.position = i;
			break;
		}
	}
	if(!profile_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}
	strcpy(remove_audio_source.Add_AudioSource_in.Aname_t, oSysInfo->Oprofile[i].AASC.Aname);
	remove_audio_source.Add_AudioSource_in.Ausecount_t = oSysInfo->Oprofile[i].AASC.Ausecount;
	strcpy(remove_audio_source.Add_AudioSource_in.Atoken_t, oSysInfo->Oprofile[i].AASC.Atoken);
	strcpy(remove_audio_source.Add_AudioSource_in.Asourcetoken_t, oSysInfo->Oprofile[i].AASC.Asourcetoken);

	ControlSystemData(SFIELD_DEL_AUDIO_SOURCE, &remove_audio_source,sizeof(remove_audio_source));

	return SOAP_OK;
}


int __trt__RemovePTZConfiguration(struct soap* soap, struct _trt__RemovePTZConfiguration *trt__RemovePTZConfiguration, struct _trt__RemovePTZConfigurationResponse *trt__RemovePTZConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __trt__RemoveVideoAnalyticsConfiguration(struct soap* soap, struct _trt__RemoveVideoAnalyticsConfiguration *trt__RemoveVideoAnalyticsConfiguration, struct _trt__RemoveVideoAnalyticsConfigurationResponse *trt__RemoveVideoAnalyticsConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __trt__RemoveMetadataConfiguration(struct soap* soap, struct _trt__RemoveMetadataConfiguration *trt__RemoveMetadataConfiguration, struct _trt__RemoveMetadataConfigurationResponse *trt__RemoveMetadataConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int Ptoken_exist = NOT_EXIST;
	int i, ret = 0;
	Metadata_configuration_tmp metadata;

	for(i=0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__RemoveMetadataConfiguration->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			metadata.position = i;
			break;
		}
	}
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;		
	}

	ret = ControlSystemData(SFIELD_DEL_METADATA, &metadata, sizeof(metadata));
	if(ret != SUCCESS)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:RemoveMetadata","Metadata cannot be removed");
		return SOAP_FAULT;		
	}

	return SOAP_OK;
}


int __trt__RemoveAudioOutputConfiguration(struct soap* soap, struct _trt__RemoveAudioOutputConfiguration *trt__RemoveAudioOutputConfiguration, struct _trt__RemoveAudioOutputConfigurationResponse *trt__RemoveAudioOutputConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __trt__RemoveAudioDecoderConfiguration(struct soap* soap, struct _trt__RemoveAudioDecoderConfiguration *trt__RemoveAudioDecoderConfiguration, struct _trt__RemoveAudioDecoderConfigurationResponse *trt__RemoveAudioDecoderConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __trt__DeleteProfile(struct soap* soap, struct _trt__DeleteProfile *trt__DeleteProfile, struct _trt__DeleteProfileResponse *trt__DeleteProfileResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	int index;
	int Ptoken_exist = NOT_EXIST;
	SysInfo *pSysInfo = GetSysInfo();

	/* check whether profile already exist or not */
	for(index = 0; index < pSysInfo->nprofile; index++) {
		if(strcmp(trt__DeleteProfile->ProfileToken, pSysInfo->Oprofile[index].profiletoken) == 0) {
			Ptoken_exist = EXIST;
			break;		   	
		}
	}
	if(!Ptoken_exist) {
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist"); 
		return SOAP_FAULT;
	}

	ControlSystemData(SFIELD_DEL_PROFILE, &index, sizeof(index));

	return SOAP_OK;
}


/**
 * @brief Get Source Source Configurations
 * @This operation lists the video source configurations of a VideoSource.  
 *   
 * @param Request	: _trt__GetVideoSourceConfigurations structure contains the token of the video input.
 * @param Response	: _trt__GetVideoSourceConfigurationsResponse structure contains the requested VideoSourceConfiguration with the matching token.
 * @return               : On success - SOAP_OK.
 : On failure No command specific faults!

 */
int __trt__GetVideoSourceConfigurations(struct soap* soap, struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations, struct _trt__GetVideoSourceConfigurationsResponse *trt__GetVideoSourceConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i;
	int num_tokens = 0;
	int num = 0;
	int j;
	int flag = NOT_EXIST;
	int k = 0;
	int use = 0;
	for(i = 0; i < oSysInfo->nprofile; i++) 
	{
		flag = 0;
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AVSC.Vtoken, oSysInfo->Oprofile[i].AVSC.Vtoken)==0)
			{
				flag = 1;
				break;
			}
		}
		if(!flag)
		{
			num_tokens++;
		}
	}

	trt__GetVideoSourceConfigurationsResponse->Configurations = (struct tt__VideoSourceConfiguration*)soap_malloc(soap,(num_tokens * sizeof(struct tt__VideoSourceConfiguration)));
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		flag = NOT_EXIST;
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AVSC.Vtoken, oSysInfo->Oprofile[i].AVSC.Vtoken)==0)
			{
				flag = EXIST;
				break;
			}
		}

		if(!flag)  
		{
			use = 0;
			for(k = 0; k < oSysInfo->nprofile; k++) 
			{
				if(strcmp(oSysInfo->Oprofile[i].AVSC.Vtoken, oSysInfo->Oprofile[k].AVSC.Vtoken) == 0)
				{
					use++;
				}
			}
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].Name = oSysInfo->Oprofile[i].AVSC.Vname;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].UseCount = use;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].token = oSysInfo->Oprofile[i].AVSC.Vtoken;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].SourceToken = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].Bounds->x = oSysInfo->Oprofile[i].AVSC.windowx;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].Bounds->y = oSysInfo->Oprofile[i].AVSC.windowy;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].Bounds->width = oSysInfo->Oprofile[i].AVSC.windowwidth;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].Bounds->height = oSysInfo->Oprofile[i].AVSC.windowheight;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].__size = 0;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].__any = NULL;
			trt__GetVideoSourceConfigurationsResponse->Configurations[num].Extension = NULL;
			dom_att(&trt__GetVideoSourceConfigurationsResponse->Configurations[num].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

			num++;
		}
	}
	trt__GetVideoSourceConfigurationsResponse->__sizeConfigurations = num;

	return SOAP_OK;
}

/** 
 * @brief Get Video Encoder Configuration
 * @This operation lists all existing video encoder configurations of an NVT.
 *   
 * @param Request	: _trt__GetVideoEncoderConfigurations structure contains an empty message.
 * @param Response	:_trt__GetVideoEncoderConfigurationsResponse structure contains a list of all existing video encoder configurations in the NVT.
 * @return               : On success - SOAP_OK.
 : On failure - The requested configuration indicated with ConfigurationToken does not exist.

 */ 
int __trt__GetVideoEncoderConfigurations(struct soap* soap, struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations, struct _trt__GetVideoEncoderConfigurationsResponse *trt__GetVideoEncoderConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *pSysInfo = GetSysInfo();
	int i;
	int j;
	int flag = 0;
	int num_tokens = 0;
	int num = 0;
	int use = 0;
	int k = 0;
	char _IPAddr[LARGE_INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

	for(i = 0; i < pSysInfo->nprofile; i++)                                        
	{
		flag  = NOT_EXIST;
		for(j = 0; j < i; j++)
		{
			if(strcmp(pSysInfo->Oprofile[i].AESC.VEtoken, pSysInfo->Oprofile[j].AESC.VEtoken) == 0)
			{
				flag = EXIST;
				break;
			}
		}
		if(!flag)
		{
			num_tokens++;
		}
	}
	trt__GetVideoEncoderConfigurationsResponse->Configurations = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, num_tokens*sizeof(struct tt__VideoEncoderConfiguration));

	flag = 0;
	for(i = 0; i < pSysInfo->nprofile; i++)                                       
	{
		for(j = 0; j<i; j++)
		{
			if(strcmp(pSysInfo->Oprofile[i].AESC.VEtoken, pSysInfo->Oprofile[j].AESC.VEtoken) == 0)
			{
				flag = EXIST;
				break;
			}
		}
		if(!flag)
		{
			use = 0;
			for(k = 0; k < pSysInfo->nprofile; k++)                                       			
				if(strcmp(pSysInfo->Oprofile[i].AESC.VEtoken, pSysInfo->Oprofile[k].AESC.VEtoken) == 0)
					use++;

			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Name = pSysInfo->Oprofile[i].AESC.VEname;               
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].UseCount = use; 
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].token = pSysInfo->Oprofile[i].AESC.VEtoken;            
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Encoding = pSysInfo->Oprofile[i].AESC.Vencoder;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Quality = pSysInfo->Oprofile[i].AESC.VEquality; 
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].SessionTimeout = pSysInfo->Oprofile[i].AESC.sessiontimeout;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Resolution->Width = pSysInfo->Oprofile[i].AESC.Rwidth;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Resolution->Height = pSysInfo->Oprofile[i].AESC.Rheight;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].RateControl->FrameRateLimit = pSysInfo->Oprofile[i].AESC.frameratelimit;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].RateControl->EncodingInterval = pSysInfo->Oprofile[i].AESC.encodinginterval; 
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].RateControl->BitrateLimit = pSysInfo->Oprofile[i].AESC.bitratelimit;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].MPEG4->GovLength = pSysInfo->Oprofile[i].AESC.Mpeggovlength;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].MPEG4->Mpeg4Profile = pSysInfo->Oprofile[i].AESC.Mpegprofile;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].H264->GovLength = pSysInfo->Oprofile[i].AESC.H264govlength;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].H264->H264Profile = pSysInfo->Oprofile[i].AESC.H264profile;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->Port = pSysInfo->Oprofile[i].AESC.Multicast_Conf.port;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->TTL = pSysInfo->Oprofile[i].AESC.Multicast_Conf.ttl;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->AutoStart = pSysInfo->Oprofile[i].AESC.Multicast_Conf.autostart; 
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->Address->Type = pSysInfo->Oprofile[i].AESC.Multicast_Conf.nIPType;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->Address->IPv4Address, pSysInfo->Oprofile[i].AESC.Multicast_Conf.IPv4Addr);
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->Address->IPv6Address = NULL;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->__size = 0;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->__any = NULL;
			dom_att(&trt__GetVideoEncoderConfigurationsResponse->Configurations[num].Multicast->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].__size = 0;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[num].__any= NULL;
			dom_att(&trt__GetVideoEncoderConfigurationsResponse->Configurations[num].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

			num++;
			flag = 0;
		}
	}
	trt__GetVideoEncoderConfigurationsResponse->__sizeConfigurations = num_tokens; // JPEG | MPEG4 | H264 

	return SOAP_OK; 
}

/** 
 * @brief Get Audio Source Configuration
 * @This operation lists all existing audio source configurations of an NVT.
 *   
 * @param Request	:_trt__GetAudioSourceConfigurations structure contains an empty message.
 * @param Response	:_trt__GetAudioSourceConfigurationsResponse structure contains a list of all existing audio source configurations in the NVT.
 * @return               : On success -  SOAP_OK.
 : On failure - NVT does not support audio.

 */
int __trt__GetAudioSourceConfigurations(struct soap* soap, struct _trt__GetAudioSourceConfigurations *trt__GetAudioSourceConfigurations, struct _trt__GetAudioSourceConfigurationsResponse *trt__GetAudioSourceConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i, use, k;
	int flag = 0;
	int flg = 0;
	int j;
	int num_token = 0;
	int index = 0;

	for(i = 0; i < oSysInfo->nprofile ; i++)                                       
	{
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AASC.Atoken, oSysInfo->Oprofile[i].AASC.Atoken)==0)
			{
				flg = 1;
				break;
			}
		}
		if(flg == 0)
		{
			num_token++;
		}
	}
	trt__GetAudioSourceConfigurationsResponse->__sizeConfigurations = num_token;


	/* AudioSourceConfiguration */
	trt__GetAudioSourceConfigurationsResponse->Configurations = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration)* num_token);
	for(i = 0; i < oSysInfo->nprofile ; i++)                                           
	{
		flag = NOT_EXIST;
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AASC.Atoken, oSysInfo->Oprofile[i].AASC.Atoken)==0)
			{
				flag = EXIST;
				break;
			}
		}
		if(!flag)
		{
			use = 0;
			for(k = 0; k < oSysInfo->nprofile; k++)
				if(strcmp(oSysInfo->Oprofile[i].AASC.Atoken, oSysInfo->Oprofile[k].AASC.Atoken) == 0)
					use++;

			trt__GetAudioSourceConfigurationsResponse->Configurations[index].Name = oSysInfo->Oprofile[i].AASC.Aname;
			trt__GetAudioSourceConfigurationsResponse->Configurations[index].UseCount = use;
			trt__GetAudioSourceConfigurationsResponse->Configurations[index].token = oSysInfo->Oprofile[i].AASC.Atoken;
			trt__GetAudioSourceConfigurationsResponse->Configurations[index].SourceToken = oSysInfo->Oprofile[i].AASC.Asourcetoken;
			trt__GetAudioSourceConfigurationsResponse->Configurations[index].__size = 0;
			trt__GetAudioSourceConfigurationsResponse->Configurations[index].__any = NULL;
			dom_att(&trt__GetAudioSourceConfigurationsResponse->Configurations[index].__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

			index++;
		}
	}

	return SOAP_OK;
}



/** 
 * @brief Get Audio Encoder Configuration
 * @This operation lists all existing device audio encoder configurations.
 *   
 * @param Request	: _trt__GetAudioEncoderConfigurations structure contains an empty message.
 * @param Response	: _trt__GetAudioEncoderConfigurationsResponse structure contains contains a list of all existing audio encoder configurations in the NVT.
 * @return               : On success - SOAP_OK.
 On failure - NVT does not support audio.

 */
int __trt__GetAudioEncoderConfigurations(struct soap* soap, struct _trt__GetAudioEncoderConfigurations *trt__GetAudioEncoderConfigurations, struct _trt__GetAudioEncoderConfigurationsResponse *trt__GetAudioEncoderConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	char _IPAddr[INFO_LENGTH];
	int _Bitrate = 0;
	int _SampleRate = 0;
	int i;
	int flag = 0,flg = 0;
	int j,num_token = 0;
	int index = 0;
	int use, k;
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

	for(i = 0; i < oSysInfo->nprofile ; i++)                                           
	{
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AAEC.AEtoken, oSysInfo->Oprofile[i].AAEC.AEtoken)==0)
			{
				flg = 1;
				break;
			}
		}
		if(flg == 0)
		{
			num_token++;
		}
	}



	trt__GetAudioEncoderConfigurationsResponse->__sizeConfigurations = num_token;//num_profile;
	trt__GetAudioEncoderConfigurationsResponse->Configurations = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap,num_token*sizeof(struct tt__AudioEncoderConfiguration));


	/* AudioSourceConfiguration */
	for(i = 0; i < oSysInfo->nprofile ; i++)                                                    
	{
		flag = NOT_EXIST;
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AAEC.AEtoken, oSysInfo->Oprofile[i].AAEC.AEtoken)==0)
			{
				flag = EXIST;
				break;
			}
		}

		if(!flag)
		{

			if(oSysInfo->Oprofile[i].AAEC.bitrate == 0)
				_Bitrate = 64;	   
			else if(oSysInfo->Oprofile[i].AAEC.bitrate == 1)
				_Bitrate = 128;   

			if( oSysInfo->Oprofile[i].AAEC.samplerate == 0)
				_SampleRate = 8;   
			else if(oSysInfo->Oprofile[i].AAEC.samplerate == 1)
				_SampleRate = 16;  


			use = 0;
			for(k = 0; k < oSysInfo->nprofile; k++)
				if(strcmp(oSysInfo->Oprofile[i].AAEC.AEtoken, oSysInfo->Oprofile[k].AAEC.AEtoken) == 0)
					use++;

			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Name = oSysInfo->Oprofile[i].AAEC.AEname;

			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].UseCount = oSysInfo->Oprofile[i].AAEC.AEusercount;//use;                               
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].token = oSysInfo->Oprofile[i].AAEC.AEtoken;//"G711";
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Encoding = oSysInfo->Oprofile[i].AAEC.AEencoding;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Bitrate = _Bitrate;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].SampleRate = _SampleRate;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].SessionTimeout = 60;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->Port = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.port; //80
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->TTL = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.ttl;//1;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->AutoStart = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.autostart;//0;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->Address->Type = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.nIPType;//0;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].AAEC.Multicast_Conf.IPv4Addr);
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->Address->IPv6Address = NULL;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->__size = 0;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->__any = NULL;
			dom_att(&trt__GetAudioEncoderConfigurationsResponse->Configurations[index].Multicast->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].__size = 0;
			trt__GetAudioEncoderConfigurationsResponse->Configurations[index].__any = NULL;
			dom_att(&trt__GetAudioEncoderConfigurationsResponse->Configurations[index].__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

			index++;
		}
	}

	return SOAP_OK;
}


/** 
 * @brief Get Video Analytics Configurations
 * @This operation lists all video analytics configurations of a device.
 *   
 * @param Request	:_trt__GetVideoAnalyticsConfigurations structure contains message is empty.
 * @param Response	:_trt__GetVideoAnalyticsConfigurationsResponse structure contains a list of all existing video analytics configurations in the device.
 * @return               : On success - SOAP_OK.
 : On failure - Device does not support video analytics.
 */
int __trt__GetVideoAnalyticsConfigurations(struct soap* soap, struct _trt__GetVideoAnalyticsConfigurations *trt__GetVideoAnalyticsConfigurations, struct _trt__GetVideoAnalyticsConfigurationsResponse *trt__GetVideoAnalyticsConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations=(struct tt__VideoAnalyticsConfiguration*)soap_malloc(soap, 2 * sizeof(struct tt__VideoAnalyticsConfiguration));
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].Name = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH); 
	strcpy(trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].Name, "Face Detection"); 
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].UseCount = 1; 
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].token = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].token, "Face Detection");
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].AnalyticsEngineConfiguration =  NULL;
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].RuleEngineConfiguration =  NULL;

	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].__size =0;
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].__any = NULL;
	dom_att(&trt__GetVideoAnalyticsConfigurationsResponse->Configurations[0].__anyAttribute,0,NULL, NULL, NULL,NULL, soap);

	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].Name = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH); 
	strcpy(trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].Name, "Motion Detecion"); 
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].UseCount = 1;
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].token = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].token, "Motion Detection");
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].AnalyticsEngineConfiguration = NULL;
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].RuleEngineConfiguration =  NULL;

	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].__size =0;
	trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].__any = NULL;
	dom_att(&trt__GetVideoAnalyticsConfigurationsResponse->Configurations[1].__anyAttribute,0,NULL, NULL, NULL,NULL, soap);

	return SOAP_OK;
}


/**
 * @brief Get MetaData Configurations 
 * @This operation lists all existing metadata configurations.
 *   
 * @param Request	:_trt__GetVideoAnalyticsConfigurations structure contains message is empty.
 * @param Response	:_trt__GetVideoAnalyticsConfigurationsResponse structure contains a list of all existing metadata configurations in the device.
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults!
 */
int __trt__GetMetadataConfigurations(struct soap* soap, struct _trt__GetMetadataConfigurations *trt__GetMetadataConfigurations, struct _trt__GetMetadataConfigurationsResponse *trt__GetMetadataConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i, j;
	int token_exist = 0;
	int num_token = 0;
	int num = 0;
	char _IPAddr[INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		for(j = 0; j< i ; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].MDC.MDtoken, oSysInfo->Oprofile[i].MDC.MDtoken) == 0)
			{
				token_exist = EXIST;
				break;
			}

		}
		if(!token_exist)
		{
			num_token++;
		}
	}
	trt__GetMetadataConfigurationsResponse->__sizeConfigurations = num_token;

	trt__GetMetadataConfigurationsResponse->Configurations=(struct tt__MetadataConfiguration *)soap_malloc(soap, trt__GetMetadataConfigurationsResponse->__sizeConfigurations *sizeof(struct tt__MetadataConfiguration));
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		token_exist = NOT_EXIST;
		for(j = 0; j< i ; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].MDC.MDtoken, oSysInfo->Oprofile[i].MDC.MDtoken) == 0)
			{
				token_exist = EXIST;
			}

		}
		if(!token_exist)
		{
			trt__GetMetadataConfigurationsResponse->Configurations[num].Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
			trt__GetMetadataConfigurationsResponse->Configurations[num].Name = oSysInfo->Oprofile[i].MDC.MDname;//"Face Detection";
			trt__GetMetadataConfigurationsResponse->Configurations[num].UseCount = oSysInfo->Oprofile[i].MDC.MDusecount;//1;
			trt__GetMetadataConfigurationsResponse->Configurations[num].token = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
			trt__GetMetadataConfigurationsResponse->Configurations[num].token = oSysInfo->Oprofile[i].MDC.MDtoken;//"Face Detection";
			trt__GetMetadataConfigurationsResponse->Configurations[num].SessionTimeout = oSysInfo->Oprofile[i].MDC.sessiontimeout;
			trt__GetMetadataConfigurationsResponse->Configurations[num].PTZStatus = NULL;

			trt__GetMetadataConfigurationsResponse->Configurations[num].Events = (struct tt__EventSubscription *)soap_malloc(soap, sizeof(struct tt__EventSubscription));
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter = (struct wsnt__FilterType *)soap_malloc(soap, sizeof(struct wsnt__FilterType));
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__size =  1;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->next =  NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->prnt = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->elts  = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->next = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->nstr = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->name = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->name, "Dialect");
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->data = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->data, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->wide = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->atts->soap = soap;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy((char *)trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->nstr, "http://docs.oasis-open.org/wsn/b-2");
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
			strcpy(trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->name, "wsnt:TopicExpression");
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->data = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->wide = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->type = 0;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->node = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->head = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->tail = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->Filter->__any->soap = soap;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->SubscriptionPolicy = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->__size = 0;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Events->__any = NULL;
			dom_att(&trt__GetMetadataConfigurationsResponse->Configurations[num].Events->__anyAttribute,0, NULL, NULL, NULL,NULL, soap);

			trt__GetMetadataConfigurationsResponse->Configurations[num].Analytics = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->Address->Type = oSysInfo->Oprofile[i].MDC.VMC.nIPType;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].MDC.VMC.IPv4Addr);
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->Address->IPv6Address = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->Port = oSysInfo->Oprofile[i].MDC.VMC.port;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->TTL = oSysInfo->Oprofile[i].MDC.VMC.ttl;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->AutoStart = oSysInfo->Oprofile[i].MDC.VMC.autostart;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->__size = 0;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->__any = NULL;
			dom_att(&trt__GetMetadataConfigurationsResponse->Configurations[num].Multicast->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

			trt__GetMetadataConfigurationsResponse->Configurations[num].__size = 0;
			trt__GetMetadataConfigurationsResponse->Configurations[num].__any = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].AnalyticsEngineConfiguration = NULL;
			trt__GetMetadataConfigurationsResponse->Configurations[num].Extension = NULL;
			dom_att(&trt__GetMetadataConfigurationsResponse->Configurations[num].__anyAttribute,0, NULL, NULL, NULL,NULL, soap);

			num ++;
		}
	}

	return SOAP_OK;
}


int __trt__GetAudioOutputConfigurations(struct soap* soap, struct _trt__GetAudioOutputConfigurations *trt__GetAudioOutputConfigurations, struct _trt__GetAudioOutputConfigurationsResponse *trt__GetAudioOutputConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i, k, use;

	trt__GetAudioOutputConfigurationsResponse->Configurations = (struct tt__AudioOutputConfiguration*)soap_malloc(soap, sizeof(struct tt__AudioOutputConfiguration)* oSysInfo->nprofile);
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		use = 0;
		for(k = 0; k < oSysInfo->nprofile; k++)
			if(strcmp(oSysInfo->Oprofile[i].AAOC.AOtoken, oSysInfo->Oprofile[k].AAOC.AOtoken) == 0)
				use++;

		trt__GetAudioOutputConfigurationsResponse->Configurations[i].Name = oSysInfo->Oprofile[i].AAOC.AOname;
		trt__GetAudioOutputConfigurationsResponse->Configurations[i].UseCount = use;
		trt__GetAudioOutputConfigurationsResponse->Configurations[i].token = oSysInfo->Oprofile[i].AAOC.AOtoken;
		trt__GetAudioOutputConfigurationsResponse->Configurations[i].OutputToken = oSysInfo->Oprofile[i].AAOC.AOtoken;
		trt__GetAudioOutputConfigurationsResponse->Configurations[i].OutputLevel = oSysInfo->Oprofile[i].AAOC.AOvolume;
		trt__GetAudioOutputConfigurationsResponse->Configurations[i].__size = 0;
		trt__GetAudioOutputConfigurationsResponse->Configurations[i].__any = NULL;
		dom_att(&trt__GetAudioOutputConfigurationsResponse->Configurations[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	}
	return SOAP_OK;
}


int __trt__GetAudioDecoderConfigurations(struct soap* soap, struct _trt__GetAudioDecoderConfigurations *trt__GetAudioDecoderConfigurations, struct _trt__GetAudioDecoderConfigurationsResponse *trt__GetAudioDecoderConfigurationsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" ); 
	return SOAP_FAULT;
}


/** 
 * @brief Get Video Source Configuration
 * @This operation lists the video source configurations of a VideoSource.
 *   
 * @param Request	:_trt__GetVideoSourceConfiguration structure contains token of the video input.
 * @param Response	:_trt__GetVideoSourceConfigurationResponse structure contains requested VideoSourceConfiguration with the matching token. 
 * @return               : On success -  SOAP_OK.
 : On failure - No specific fault codes.

 */
int __trt__GetVideoSourceConfiguration(struct soap* soap, struct _trt__GetVideoSourceConfiguration *trt__GetVideoSourceConfiguration, struct _trt__GetVideoSourceConfigurationResponse *trt__GetVideoSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int i;
	int num_tokens = 0;
	int Ptoken_exist = NOT_EXIST;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetVideoSourceConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AVSC.Vtoken)==0)
		{
			num_tokens++;
		}
	}

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetVideoSourceConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AVSC.Vtoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}

	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	trt__GetVideoSourceConfigurationResponse->Configuration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
	trt__GetVideoSourceConfigurationResponse->Configuration->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoSourceConfigurationResponse->Configuration->Name = oSysInfo->Oprofile[i].AVSC.Vname;
	trt__GetVideoSourceConfigurationResponse->Configuration->UseCount = num_tokens;
	trt__GetVideoSourceConfigurationResponse->Configuration->token = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoSourceConfigurationResponse->Configuration->token = oSysInfo->Oprofile[i].AVSC.Vtoken;
	trt__GetVideoSourceConfigurationResponse->Configuration->SourceToken = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoSourceConfigurationResponse->Configuration->SourceToken = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->x = oSysInfo->Oprofile[i].AVSC.windowx;
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->y = oSysInfo->Oprofile[i].AVSC.windowy;
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->width = oSysInfo->Oprofile[i].AVSC.windowwidth;
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->height = oSysInfo->Oprofile[i].AVSC.windowheight;
	trt__GetVideoSourceConfigurationResponse->Configuration->__size = 0;
	trt__GetVideoSourceConfigurationResponse->Configuration->__any = NULL;
	trt__GetVideoSourceConfigurationResponse->Configuration->Extension = NULL;
	dom_att(&trt__GetVideoSourceConfigurationResponse->Configuration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	return SOAP_OK; 
}


/** 
 * @brief Get Video Encoder Configuration
 * @This operation lists all existing video encoder configurations of an NVT.
 *   
 * @param Request	:_trt__GetVideoEncoderConfiguration structure contains an empty message.
 * @param Response	:_trt__GetVideoEncoderConfigurationResponse structure contains a list of all existing video encoder configurations in the NVT. 
 * @return               : On success - SOAP_OK.
 On failure - The requested configuration indicated with ConfigurationToken does not exist.
 */
int __trt__GetVideoEncoderConfiguration(struct soap* soap, struct _trt__GetVideoEncoderConfiguration *trt__GetVideoEncoderConfiguration, struct _trt__GetVideoEncoderConfigurationResponse *trt__GetVideoEncoderConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int i;
	int num_tokens = 0;
	NET_IPV4 ip;
	char _IPAddr[INFO_LENGTH];
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	int Ptoken_exist = NOT_EXIST;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetVideoEncoderConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AESC.VEtoken) == 0)
		{
			num_tokens++;
		}
	}

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetVideoEncoderConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AESC.VEtoken) == 0)
		{

			Ptoken_exist = EXIST;
			break;
		}
	}
	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	trt__GetVideoEncoderConfigurationResponse->Configuration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));

	trt__GetVideoEncoderConfigurationResponse->Configuration->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoEncoderConfigurationResponse->Configuration->Name =oSysInfo->Oprofile[i].AESC.VEname;
	trt__GetVideoEncoderConfigurationResponse->Configuration->UseCount = num_tokens;
	trt__GetVideoEncoderConfigurationResponse->Configuration->token = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoEncoderConfigurationResponse->Configuration->token = oSysInfo->Oprofile[i].AESC.VEtoken;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Encoding = oSysInfo->Oprofile[i].AESC.Vencoder;

	trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
	trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Width = oSysInfo->Oprofile[i].AESC.Rwidth;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Height = oSysInfo->Oprofile[i].AESC.Rheight;

	trt__GetVideoEncoderConfigurationResponse->Configuration->Quality = oSysInfo->Oprofile[i].AESC.VEquality; 

	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->FrameRateLimit = oSysInfo->Oprofile[i].AESC.frameratelimit;   
	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->EncodingInterval = oSysInfo->Oprofile[i].AESC.encodinginterval;
	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->BitrateLimit = oSysInfo->Oprofile[i].AESC.bitratelimit;

	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4->GovLength = oSysInfo->Oprofile[i].AESC.Mpeggovlength;
	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4->Mpeg4Profile = oSysInfo->Oprofile[i].AESC.Mpegprofile;//SP = 0, ASP = 1;

	trt__GetVideoEncoderConfigurationResponse->Configuration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
	trt__GetVideoEncoderConfigurationResponse->Configuration->H264->GovLength = oSysInfo->Oprofile[i].AESC.H264govlength;
	trt__GetVideoEncoderConfigurationResponse->Configuration->H264->H264Profile = oSysInfo->Oprofile[i].AESC.H264profile;//Baseline = 0, Main = 1, High = 3

	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Port = oSysInfo->Oprofile[i].AESC.Multicast_Conf.port;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->TTL = oSysInfo->Oprofile[i].AESC.Multicast_Conf.ttl;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->AutoStart = oSysInfo->Oprofile[i].AESC.Multicast_Conf.autostart;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->Type = oSysInfo->Oprofile[i].AESC.Multicast_Conf.nIPType;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].AESC.Multicast_Conf.IPv4Addr);
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv6Address = NULL;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__size = 0;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__any = NULL;
	dom_att(&trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__anyAttribute,0, NULL, NULL, NULL,NULL, soap);
	trt__GetVideoEncoderConfigurationResponse->Configuration->SessionTimeout = oSysInfo->Oprofile[i].AESC.sessiontimeout;
	trt__GetVideoEncoderConfigurationResponse->Configuration->__size = 0;
	trt__GetVideoEncoderConfigurationResponse->Configuration->__any = NULL;
	dom_att(&trt__GetVideoEncoderConfigurationResponse->Configuration->__anyAttribute,0, NULL, NULL, NULL,NULL, soap);

	return SOAP_OK;
}

/** 
 * @brief Get Audio Source Configuration
 * @An AudioSourceConfiguration contains a reference to an AudioSource that is to be used for input in a media profile. This operation lists all existing audio source configurations of an NVT
 *   
 * @param Request	:_trt__GetAudioSourceConfiguration structure variable members contains empty message.
 * @param Response	:_trt__GetAudioSourceConfigurationResponse structure contains a list of structures describing all available audio sources of the device.
 * @return               : On success - SOAP_OK.
 On failure - The requested configuration indicated with ConfigurationToken does not exist. or
 NVT does not support audio.
 */
int __trt__GetAudioSourceConfiguration(struct soap* soap, struct _trt__GetAudioSourceConfiguration *trt__GetAudioSourceConfiguration, struct _trt__GetAudioSourceConfigurationResponse *trt__GetAudioSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i;
	int use = 0;
	int k = 0;

	for(i = 0;i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetAudioSourceConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AASC.Atoken) == 0)
		{
			break;
		}
	}

	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}
	for(k = 0; k < oSysInfo->nprofile; k++)
	{
		if(strcmp(trt__GetAudioSourceConfiguration->ConfigurationToken, oSysInfo->Oprofile[k].AASC.Atoken) == 0)
		{
			use++;
		}
	}
	trt__GetAudioSourceConfigurationResponse->Configuration = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
	trt__GetAudioSourceConfigurationResponse->Configuration->Name = oSysInfo->Oprofile[i].AASC.Aname;
	trt__GetAudioSourceConfigurationResponse->Configuration->UseCount = use;
	trt__GetAudioSourceConfigurationResponse->Configuration->token = oSysInfo->Oprofile[i].AASC.Atoken;
	trt__GetAudioSourceConfigurationResponse->Configuration->SourceToken = oSysInfo->Oprofile[i].AASC.Asourcetoken;
	trt__GetAudioSourceConfigurationResponse->Configuration->__size = 0;
	trt__GetAudioSourceConfigurationResponse->Configuration->__any = NULL;
	dom_att(&trt__GetAudioSourceConfigurationResponse->Configuration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	return SOAP_OK; 
}

/** 
 * @brief Get Audio Encoder Configuration
 * @brief This operation lists all existing device audio encoder configurations.
 *   
 * @param Request	:_trt__GetAudioEncoderConfiguration structure variable members contains an empty message.
 * @param Response	:_trt__GetAudioEncoderConfigurationResponse structure contains a list of all existing audio encoder configurations in the NVT.
 * @return               : On success - SOAP_OK.
 On failure - The configuration does not exist.
 NVT does not support audio.
 */ 
int __trt__GetAudioEncoderConfiguration(struct soap* soap, struct _trt__GetAudioEncoderConfiguration *trt__GetAudioEncoderConfiguration, struct _trt__GetAudioEncoderConfigurationResponse *trt__GetAudioEncoderConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = (SysInfo *)GetSysInfo();
	int _Bitrate = 0;
	int _SampleRate = 0;
	int i = 0,token_exist = 0;
	char _IPAddr[INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	char *name = (char *)malloc(SMALL_INFO_LENGTH);
	int k = 0;
	int use = 0;


	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetAudioEncoderConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AAEC.AEtoken) == 0)
		{
			token_exist = EXIST;
			break;

		}
	}
	if(!token_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
	{
		if(strcmp(trt__GetAudioEncoderConfiguration->ConfigurationToken, oSysInfo->Oprofile[k].AAEC.AEtoken) == 0)
		{
			use++;
		}
	}
	if(oSysInfo->Oprofile[i].AAEC.bitrate== 0)
		_Bitrate = 64;	   
	else if(oSysInfo->Oprofile[i].AAEC.bitrate == 1)
		_Bitrate = 128;   

	if(oSysInfo->Oprofile[i].AAEC.samplerate == 0)
		_SampleRate = 8;   
	else if(oSysInfo->Oprofile[i].AAEC.samplerate == 1)
		_SampleRate = 16;  

	trt__GetAudioEncoderConfigurationResponse->Configuration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
	trt__GetAudioEncoderConfigurationResponse->Configuration->Name = oSysInfo->Oprofile[i].AAEC.AEname;
	trt__GetAudioEncoderConfigurationResponse->Configuration->UseCount = oSysInfo->Oprofile[i].AAEC.AEusercount;//use;
	trt__GetAudioEncoderConfigurationResponse->Configuration->token = oSysInfo->Oprofile[i].AAEC.AEtoken;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Encoding = oSysInfo->Oprofile[i].AAEC.AEencoding;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Bitrate = _Bitrate;
	trt__GetAudioEncoderConfigurationResponse->Configuration->SampleRate = _SampleRate;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address = (struct tt__IPAddress*)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->Type = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.nIPType; 0;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].AAEC.Multicast_Conf.IPv4Addr);
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->IPv6Address = NULL;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Port = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.port;//80;	
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->TTL = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.ttl;//1;	
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->AutoStart = oSysInfo->Oprofile[i].AAEC.Multicast_Conf.autostart; 0;
	trt__GetAudioEncoderConfigurationResponse->Configuration->SessionTimeout = 60;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->__size = 0;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->__any = NULL;
	dom_att(&trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	trt__GetAudioEncoderConfigurationResponse->Configuration->__size = 0;
	trt__GetAudioEncoderConfigurationResponse->Configuration->__any = NULL;
	dom_att(&trt__GetAudioEncoderConfigurationResponse->Configuration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	free(name);

	return SOAP_OK;		
}


int  __trt__GetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__GetVideoAnalyticsConfiguration *trt__GetVideoAnalyticsConfiguration, struct _trt__GetVideoAnalyticsConfigurationResponse *trt__GetVideoAnalyticsConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i;
	int Ptoken_exist = NOT_EXIST;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetVideoAnalyticsConfiguration->ConfigurationToken,oSysInfo->Oprofile[i].AVAC.VAtoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->Name = oSysInfo->Oprofile[0].AVAC.VAname;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->UseCount =oSysInfo->Oprofile[0].AVAC.VAusercount;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->token = oSysInfo->Oprofile[0].AVAC.VAtoken;

	/* tt:AnalyticsEngineConfiguration */
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfiguration));

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = oSysInfo->Oprofile[0].AVAC.VAE.VAMsize;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule = (struct tt__Config *)soap_malloc(soap, sizeof(struct tt__Config));
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters = (struct tt__ItemList *)soap_malloc(soap, sizeof(struct tt__ItemList));
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeSimpleItem = oSysInfo->Oprofile[0].AVAC.VAE.VAsizeitem;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc(soap, sizeof(struct _tt__ItemList_SimpleItem));

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Name = oSysInfo->Oprofile[0].AVAC.VAE.VASname;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value = oSysInfo->Oprofile[0].AVAC.VAE.VASvalue;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeElementItem = oSysInfo->Oprofile[0].AVAC.VAE.VAEsize;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem = (struct _tt__ItemList_ElementItem *)soap_malloc(soap, sizeof(struct _tt__ItemList_ElementItem));

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->Name = oSysInfo->Oprofile[0].AVAC.VAE.VAEname;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->__any = NULL;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->Extension = NULL;
	dom_att(&trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Name = oSysInfo->Oprofile[0].AVAC.VAE.Aconfigname;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Type = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Type = oSysInfo->Oprofile[0].AVAC.VAE.Aconfigtype;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->Extension = NULL;
	dom_att(&trt__GetVideoAnalyticsConfigurationResponse->Configuration->AnalyticsEngineConfiguration->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	/* tt:RuleEngineConfiguration */
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__RuleEngineConfiguration));
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->__sizeRule = oSysInfo->Oprofile[0].AVAC.VRE.VARsize;

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule = (struct tt__Config *)soap_malloc(soap, sizeof(struct tt__Config));
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters = (struct tt__ItemList *)soap_malloc(soap, sizeof(struct tt__ItemList));
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->__sizeSimpleItem = oSysInfo->Oprofile[0].AVAC.VRE.VARSsizeitem;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc(soap, sizeof(struct _tt__ItemList_SimpleItem));

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem->Name = oSysInfo->Oprofile[0].AVAC.VRE.VARSname;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem->Value = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem->Value = oSysInfo->Oprofile[0].AVAC.VRE.VARSvalue;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->__sizeElementItem = oSysInfo->Oprofile[0].AVAC.VRE.VAREsize;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->ElementItem = (struct _tt__ItemList_ElementItem *)soap_malloc(soap, sizeof(struct _tt__ItemList_ElementItem));

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->ElementItem->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->ElementItem->Name = oSysInfo->Oprofile[0].AVAC.VRE.VAREname;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->ElementItem->__any = NULL;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->Extension = NULL;
	dom_att(&trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Parameters->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Name = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Name = oSysInfo->Oprofile[0].AVAC.VRE.Rconfigname;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Type = (char*)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Rule->Type = oSysInfo->Oprofile[0].AVAC.VRE.Rconfigtype;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->Extension = NULL;
	dom_att(&trt__GetVideoAnalyticsConfigurationResponse->Configuration->RuleEngineConfiguration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->__size = 0;
	trt__GetVideoAnalyticsConfigurationResponse->Configuration->__any = NULL;
	dom_att(&trt__GetVideoAnalyticsConfigurationResponse->Configuration->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	return SOAP_OK;
}


int  __trt__GetMetadataConfiguration(struct soap* soap, struct _trt__GetMetadataConfiguration *trt__GetMetadataConfiguration, struct _trt__GetMetadataConfigurationResponse *trt__GetMetadataConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int i = 0,token_exist = 0;
	char _IPAddr[INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);

	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetMetadataConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].MDC.MDtoken) == 0)
		{
			token_exist = EXIST;
			break;
		}
	}
	if(!token_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}


	trt__GetMetadataConfigurationResponse->Configuration = (struct tt__MetadataConfiguration *)soap_malloc(soap, sizeof(struct tt__MetadataConfiguration));
	trt__GetMetadataConfigurationResponse->Configuration[0].Name = (char*)soap_malloc(soap, sizeof(char)*MID_INFO_LENGTH);
	trt__GetMetadataConfigurationResponse->Configuration[0].Name = oSysInfo->Oprofile[i].MDC.MDname;//"Face Detection";
	trt__GetMetadataConfigurationResponse->Configuration[0].UseCount = oSysInfo->Oprofile[i].MDC.MDusecount;//1;
	trt__GetMetadataConfigurationResponse->Configuration[0].token = (char*)soap_malloc(soap, sizeof(char)*MID_INFO_LENGTH);
	trt__GetMetadataConfigurationResponse->Configuration[0].token = oSysInfo->Oprofile[i].MDC.MDtoken;//"Face Detection";
	trt__GetMetadataConfigurationResponse->Configuration[0].PTZStatus = NULL;

	//Events Start Here.
	trt__GetMetadataConfigurationResponse->Configuration[0].Events = (struct tt__EventSubscription *)soap_malloc(soap, sizeof(struct tt__EventSubscription));
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter = (struct wsnt__FilterType *)soap_malloc(soap, sizeof(struct wsnt__FilterType));
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__size =  1;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->next =  NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->prnt = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->elts  = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));

	//dom_att(&trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts, 0, "Dialect", "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet", NULL, NULL, soap);
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->next = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->nstr = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->name = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->name, "Dialect");
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->data = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->data, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->wide = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->atts->soap = soap;

	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy((char *)trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->nstr, "http://docs.oasis-open.org/wsn/b-2");
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
	strcpy(trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->name, "wsnt:TopicExpression");
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->data = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->wide = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->type = 0;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->node = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->head = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->tail = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->Filter->__any->soap = soap;

	trt__GetMetadataConfigurationResponse->Configuration[0].Events->SubscriptionPolicy = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->__size = 0;
	trt__GetMetadataConfigurationResponse->Configuration[0].Events->__any = NULL;
	dom_att(&trt__GetMetadataConfigurationResponse->Configuration[0].Events->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetMetadataConfigurationResponse->Configuration[0].Analytics = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->Type = oSysInfo->Oprofile[i].MDC.VMC.nIPType;
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->IPv4Address, oSysInfo->Oprofile[i].MDC.VMC.IPv4Addr);
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->IPv6Address = NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Port = oSysInfo->Oprofile[i].MDC.VMC.port;
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->TTL = oSysInfo->Oprofile[i].MDC.VMC.ttl;
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->AutoStart = oSysInfo->Oprofile[i].MDC.VMC.autostart;
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->__size = 0;
	trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->__any = NULL;
	dom_att(&trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetMetadataConfigurationResponse->Configuration[0].SessionTimeout = oSysInfo->Oprofile[i].MDC.sessiontimeout;
	trt__GetMetadataConfigurationResponse->Configuration[0].__size = 0;
	trt__GetMetadataConfigurationResponse->Configuration[0].__any= NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].AnalyticsEngineConfiguration= NULL;
	trt__GetMetadataConfigurationResponse->Configuration[0].Extension	= NULL;
	dom_att(&trt__GetMetadataConfigurationResponse->Configuration[0].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);


	return SOAP_OK;
}



int  __trt__GetAudioOutputConfiguration(struct soap* soap, struct _trt__GetAudioOutputConfiguration *trt__GetAudioOutputConfiguration, struct _trt__GetAudioOutputConfigurationResponse *trt__GetAudioOutputConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i, k, use;

	for(i = 0; i < oSysInfo->nprofile; i++)
		if(strcmp(trt__GetAudioOutputConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AAOC.AOtoken) == 0)
			break;
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal","ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(trt__GetAudioOutputConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].AAOC.AOtoken) == 0)
			use++;

	trt__GetAudioOutputConfigurationResponse = (struct _trt__GetAudioOutputConfigurationResponse*)soap_malloc(soap, sizeof(struct _trt__GetAudioOutputConfigurationResponse));
	trt__GetAudioOutputConfigurationResponse->Configuration = (struct tt__AudioOutputConfiguration*)soap_malloc(soap, sizeof(struct tt__AudioOutputConfiguration));
	trt__GetAudioOutputConfigurationResponse->Configuration->Name = oSysInfo->Oprofile[i].AAOC.AOname;
	trt__GetAudioOutputConfigurationResponse->Configuration->UseCount = use;
	trt__GetAudioOutputConfigurationResponse->Configuration->token = oSysInfo->Oprofile[i].AAOC.AOtoken;
	trt__GetAudioOutputConfigurationResponse->Configuration->OutputToken = oSysInfo->Oprofile[i].AAOC.AOtoken;
	trt__GetAudioOutputConfigurationResponse->Configuration->OutputLevel = oSysInfo->Oprofile[i].AAOC.AOvolume;
	trt__GetAudioOutputConfigurationResponse->Configuration->__size = 0;
	trt__GetAudioOutputConfigurationResponse->Configuration->__any = NULL;
	dom_att(&trt__GetAudioOutputConfigurationResponse->Configuration->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}



int  __trt__GetAudioDecoderConfiguration(struct soap* soap, struct _trt__GetAudioDecoderConfiguration *trt__GetAudioDecoderConfiguration, struct _trt__GetAudioDecoderConfigurationResponse *trt__GetAudioDecoderConfigurationResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/** 
 * @brief Get Compatible Video Encoder Configurations
 * @brief This operation lists all the video encoder configurations of the NVT that are compatible with a certain media profile.
 *   
 * @param Request	:_trt__GetCompatibleVideoEncoderConfigurations structure contains the token of an existing media profile.
 * @param Response	:_trt__GetCompatibleVideoEncoderConfigurationsResponse structure contains a list of video encoder configurations that are compatible with the given medi                            a profile.
 * @return               : On success -  SOAP_OK.
 : On failure - The requested profile token ProfileToken does not exist.
 */
int __trt__GetCompatibleVideoEncoderConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoEncoderConfigurations *trt__GetCompatibleVideoEncoderConfigurations, struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *trt__GetCompatibleVideoEncoderConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	char _IPAddr[INFO_LENGTH];
	char _IPAddr_MUL[] = "239.0.0.0";
	int i = 0;
	int Ptoken_exist = 0;
	SysInfo *oSysInfo = GetSysInfo();
	/* Read Settings */	
	NET_IPV4 ip;

	int numberofstream = oget_numberofstream();

	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

	trt__GetCompatibleVideoEncoderConfigurationsResponse->__sizeConfigurations = 1;
	/* VideoEncoderConfiguration */
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetCompatibleVideoEncoderConfigurations->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations = 
		(struct tt__VideoEncoderConfiguration *)soap_malloc(soap, numberofstream * sizeof(struct tt__VideoEncoderConfiguration));

	int bitrate = oget_bitrate(i);
	int ratecontrol = oget_ratecontrol(i);
	int framerate = oget_framerate(i);
	int width = oget_resolutionwidth(i);
	int height = oget_resolutionheight(i);
	int govlength = oget_ipratio(i);

	for(i = 0; i < numberofstream; i++)
	{
		bitrate = oget_bitrate(i);
		ratecontrol = oget_ratecontrol(i);
		framerate = oget_framerate(i);
		width = oget_resolutionwidth(i);
		height = oget_resolutionheight(i);
		govlength = oget_ipratio(i);

		if(i == 0) {
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Name = "DE_Name1"; 
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].token = "DE_Token1";
		} else if(i == 1) {
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Name = "DE_Name2"; 
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].token = "DE_Token2";
		} else if(i == 2) {
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Name = "DE_Name3"; 
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].token = "DE_Token3";
		} else {
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Name = "DE_Name"; 
			trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].token = "DE_Token";
		}
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].UseCount = 1;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Encoding = oget_encoding(i);
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Quality = oSysInfo->Oprofile[0].AESC.VEquality;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].SessionTimeout = 60;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Width = width;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Height = height;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl->FrameRateLimit = framerate;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl->EncodingInterval = ratecontrol; //0:vbr 1:cbr
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl->BitrateLimit = bitrate;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].MPEG4->GovLength = govlength;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].MPEG4->Mpeg4Profile = 0;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].H264->GovLength = govlength;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].H264->H264Profile = 0;//Baseline = 0, Main = 1, High = 3
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Port = 1234;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->TTL = 0;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->AutoStart = 0; 
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->Type = 0;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address, _IPAddr_MUL);      
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv6Address = NULL;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->__size = 0;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->__any = NULL;
		dom_att(&trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].__size = 0;
		trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].__any = NULL;
		dom_att(&trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	}

	return SOAP_OK; 
}

/** 
 * @brief Get Compatible Video Source Configurations
 * @This operation requests all the video source configurations of the NVT that are compatible with a certain media profile.
 *   
 * @param Request	:_trt__GetCompatibleVideoSourceConfigurations structure contains the token of an existing media profile.
 * @param Response	:_trt__GetCompatibleVideoSourceConfigurationsResponse structure contains a list of video source configurations that are compatible with the media profile.
 * @return               : On success - SOAP_OK.
 On failure - The requested profile token ProfileToken does not exist.
 */
int __trt__GetCompatibleVideoSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoSourceConfigurations *trt__GetCompatibleVideoSourceConfigurations, struct _trt__GetCompatibleVideoSourceConfigurationsResponse *trt__GetCompatibleVideoSourceConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	int i=0, use, k;
	int Ptoken_exist = NOT_EXIST;
	SysInfo *oSysInfo = GetSysInfo(); 
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetCompatibleVideoSourceConfigurations->ProfileToken,oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(trt__GetCompatibleVideoSourceConfigurations->ProfileToken,oSysInfo->Oprofile[k].profiletoken) == 0)
			use++;

	trt__GetCompatibleVideoSourceConfigurationsResponse->__sizeConfigurations = 3;//1; //MPEG4 | H264 | JPEG      
	/* VideoSourceConfiguration */
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, 3 * sizeof(struct tt__VideoSourceConfiguration));
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].Name = oSysInfo->Oprofile[i].AVSC.Vname;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].UseCount = use;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].token = oSysInfo->Oprofile[i].AVSC.Vtoken;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].SourceToken = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].Bounds->x = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].Bounds->y = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].Bounds->width = 1600;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].Bounds->height = 1200;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].__size  = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].__any = NULL;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].Extension = NULL;
	dom_att(&trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[0].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].Name = oSysInfo->Oprofile[i].AVSC.Vname;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].UseCount = use;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].token = oSysInfo->Oprofile[i].AVSC.Vtoken;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].SourceToken = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].Bounds->x = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].Bounds->y = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].Bounds->width = 1280;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].Bounds->height = 720;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].__size  = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].__any = NULL;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].Extension = NULL;
	dom_att(&trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[1].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].Name = oSysInfo->Oprofile[i].AVSC.Vname;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].UseCount = use;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].token = oSysInfo->Oprofile[i].AVSC.Vtoken;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].SourceToken = oSysInfo->Oprofile[i].AVSC.Vsourcetoken;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].Bounds->x = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].Bounds->y = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].Bounds->width = 1280;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].Bounds->height = 720;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].__size  = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].__any = NULL;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].Extension = NULL;
	dom_att(&trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations[2].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}

/**
 * @brief Get Compatible Audio Encoder Cofigurations 
 * @This operation requests all audio encoder configurations of the NVT that are compatible with a certain media profile.
 *   
 * @param Request	:_trt__GetCompatibleAudioEncoderConfigurations structure contains the token of an existing media profile.
 * @param Response	:_trt__GetCompatibleAudioEncoderConfigurationsResponse structure contains a list of audio encoder configurations that are compatible with the given 
 media profile.
 * @return               : On success - SOAP_OK.
 On failure - The requested profile token ProfileToken does not exist. or
 NVT does not support audio.
 */
int __trt__GetCompatibleAudioEncoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioEncoderConfigurations *trt__GetCompatibleAudioEncoderConfigurations, struct _trt__GetCompatibleAudioEncoderConfigurationsResponse *trt__GetCompatibleAudioEncoderConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	char _IPAddr[INFO_LENGTH];
	char _IPAddr_MUL[] = "239.0.0.0";
	NET_IPV4 ip;
	int _Bitrate = 0;
	int _SampleRate = 0;
	int _Encoding = oSysInfo->audio_config.codectype;
	int i,profile_exist = 0,index = 0;
	int use, k; 
	for(i = 0 ;i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetCompatibleAudioEncoderConfigurations->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			profile_exist = EXIST;
			index = i;
			break;
		}
	}
	if(!profile_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}
	/* Read Settings */	
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

	if(oSysInfo->audio_config.bitrate == 0)
	{
		_Bitrate = 64;	 
	}
	else if(oSysInfo->audio_config.bitrate == 1)
	{
		_Bitrate = 128; 
	}

	if(oSysInfo->audio_config.samplerate == 0)
	{
		_SampleRate = 8; 
	}
	else if(oSysInfo->audio_config.samplerate == 1)
	{
		_SampleRate = 16;
	}
	trt__GetCompatibleAudioEncoderConfigurationsResponse->__sizeConfigurations = 1;

	/* AudioEncoderConfiguration */
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[index].AAEC.AEtoken, oSysInfo->Oprofile[k].AAEC.AEtoken) == 0)
			use++;

	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Name = oSysInfo->Oprofile[index].AAEC.AEname;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->UseCount = use;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->token = oSysInfo->Oprofile[index].AAEC.AEtoken;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Encoding = _Encoding;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Bitrate = _Bitrate;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->SampleRate = _SampleRate;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->SessionTimeout = 60;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Port = 1234;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->TTL = 0;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->AutoStart = 0;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->Type = 0;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->IPv4Address, _IPAddr_MUL);
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->IPv6Address = NULL;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->__size = 0;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->__any = NULL;
	dom_att(&trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->__size = 0;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->__any = NULL;
	dom_att(&trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK; 
}


/** 
 * @brief Get Compatible Audio Source Configurations
 * @This operation requests all audio encoder configurations of the NVT that are compatible with a certain media profile. 
 *   
 * @param Request	:_trt__GetCompatibleAudioSourceConfigurations structure contains the token of an existing media profile.
 * @param Response	:_trt__GetCompatibleAudioSourceConfigurationsResponse structure contains list of audio encoder configurations that are compatible with the given media
 profile.
 * @return               : On success - SOAP_OK.
 On failure - The requested profile token ProfileToken does not exist. or
 NVT does not support audio.
 */
int __trt__GetCompatibleAudioSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioSourceConfigurations *trt__GetCompatibleAudioSourceConfigurations, struct _trt__GetCompatibleAudioSourceConfigurationsResponse *trt__GetCompatibleAudioSourceConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();                                                                                       
	int i, use, k;
	for(i = 0; i < oSysInfo->nprofile; i++)
		if(strcmp(trt__GetCompatibleAudioSourceConfigurations->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
			break;
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}                                                                                                                      
	trt__GetCompatibleAudioSourceConfigurationsResponse->__sizeConfigurations = 1;
	/* AudioSourceConfiguration */
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations = (struct tt__AudioSourceConfiguration*)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration) * trt__GetCompatibleAudioSourceConfigurationsResponse->__sizeConfigurations);
	use = 0;
	for(k = 0; k < oSysInfo->nprofile; k++)
		if(strcmp(oSysInfo->Oprofile[i].AASC.Atoken, oSysInfo->Oprofile[k].AASC.Atoken) == 0)
			use++;

	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->Name = oSysInfo->Oprofile[i].AASC.Aname;
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->UseCount = use;
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->token = oSysInfo->Oprofile[i].AASC.Atoken;
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->SourceToken = oSysInfo->Oprofile[i].AASC.Asourcetoken;
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->__size = 0;
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->__any = NULL;
	dom_att(&trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}


/**
 * @brief Get Compatible Video Analytics Configuratons 
 * @This operation requests all video analytic configurations of the device that are compatible with a certain media profile.
 *   
 * @param Request	:_trt__GetCompatibleVideoAnalyticsConfigurations structure contains the token of an existing media profile.
 * @param Response	:_trt__GetCompatibleVideoAnalyticsConfigurationsResponse structure contains a list of video analytics configurations that are compatible  with the
 given media profile.
 * @return               : On success - SOAP_OK.
 On failure - The requested profile token ProfileToken does not exist. or
 The device does not support video analytics.
 */
int __trt__GetCompatibleVideoAnalyticsConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoAnalyticsConfigurations *trt__GetCompatibleVideoAnalyticsConfigurations, struct _trt__GetCompatibleVideoAnalyticsConfigurationsResponse *trt__GetCompatibleVideoAnalyticsConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	char _name[LARGE_INFO_LENGTH];
	strcpy(_name, trt__GetCompatibleVideoAnalyticsConfigurations->ProfileToken);

	if(!strcmp(_name, "Face Detection"))
	{
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse-> __sizeConfigurations = 1;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations = (struct tt__VideoAnalyticsConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->Name = _name;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->UseCount = 1;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->UseCount = _name;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 1;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule = (struct tt__Config *)soap_malloc(soap, sizeof(struct tt__Config));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters = (struct tt__ItemList *)soap_malloc(soap, sizeof(struct tt__ItemList));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeSimpleItem = 0;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc(soap, sizeof(struct _tt__ItemList_SimpleItem));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Name = NULL;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value = NULL;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeElementItem = 0;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem = (struct _tt__ItemList_ElementItem *)soap_malloc(soap, sizeof(struct _tt__ItemList_ElementItem));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem = NULL;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->Extension = (struct tt__ItemListExtension *)soap_malloc(soap, sizeof(struct tt__ItemListExtension));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->Extension = NULL;
		dom_att(&trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->Extension = (struct tt__AnalyticsEngineConfigurationExtension *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfigurationExtension));
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->Extension = NULL;             
		dom_att(&trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->AnalyticsEngineConfiguration->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->RuleEngineConfiguration= NULL;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->__size = 0;
		trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->__any= NULL;
		dom_att(&trt__GetCompatibleVideoAnalyticsConfigurationsResponse->Configurations->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	}
	else
	{
		/* if ProfileToken does not exist */	
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile", "The requested profile token ProfileToken does not exist");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}


/**
 * @brief Get Compatible Metadata Configurations.
 * @This operation requests all the metadata configurations of the device that are compatible with a certain media profile.
 *   
 * @param Request        :_trt__GetCompatibleMetadataConfigurations structure contains the token of an existing media profile.
 * @param Response       :_trt__GetCompatibleMetadataConfigurationsResponse structure contains a list of metadata configurations that are compatible with the
 given media profile.

 * @return               : On success - SOAP_OK.
 On failure - The requested profile token ProfileToken does not exist.
 */
int __trt__GetCompatibleMetadataConfigurations(struct soap* soap, struct _trt__GetCompatibleMetadataConfigurations *trt__GetCompatibleMetadataConfigurations, struct _trt__GetCompatibleMetadataConfigurationsResponse *trt__GetCompatibleMetadataConfigurationsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int i, Ptoken_exist = NOT_EXIST;
	char _IPAddr_MUL[] = "239.0.0.0";

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetCompatibleMetadataConfigurations->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	if(!Ptoken_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	trt__GetCompatibleMetadataConfigurationsResponse->__sizeConfigurations = 1;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations = (struct tt__MetadataConfiguration *)soap_malloc(soap, trt__GetCompatibleMetadataConfigurationsResponse->__sizeConfigurations * sizeof(struct tt__MetadataConfiguration));
	/*	if((oSysInfo->Oprofile[1].profiletoken != NULL) && (oSysInfo->Oprofile[1].profilename != NULL))
		{
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Name = oSysInfo->Oprofile[1].profilename;
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].token = oSysInfo->Oprofile[1].profiletoken;
		}
		else
		{
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Name = oSysInfo->Oprofile[0].profilename;
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].token = oSysInfo->Oprofile[0].profiletoken;
		}*/

	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Name = oSysInfo->Oprofile[0].MDC.MDname;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].UseCount = 1;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].token = oSysInfo->Oprofile[0].MDC.MDtoken;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].PTZStatus = NULL;

	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events = (struct tt__EventSubscription *)soap_malloc(soap, sizeof(struct tt__EventSubscription));
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter = (struct wsnt__FilterType *)soap_malloc(soap, sizeof(struct wsnt__FilterType));
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__size =  1;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any = (struct soap_dom_element *)soap_malloc(soap, sizeof(struct soap_dom_element));
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->next =  NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->prnt = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->elts  = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->next = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->nstr = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->name = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->name, "Dialect");
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->data = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->data, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->wide = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->atts->soap = soap;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->nstr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	strcpy((char *)trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->nstr, "http://docs.oasis-open.org/wsn/b-2");
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->name = (char *)soap_malloc(soap, sizeof(char) * 30);
	strcpy(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->name, "wsnt:TopicExpression");
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->data = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->wide = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->type = 0;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->node = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->head = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->tail = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->Filter->__any->soap = soap;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->SubscriptionPolicy = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->__size = 0;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->__any = NULL;
	dom_att(&trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Events->__anyAttribute,0, NULL, NULL, NULL,NULL, soap);

	//trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Analytics = (int *)soap_malloc(soap, sizeof(int));
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Analytics = NULL;//1;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->Address->Type = 0;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH); 
	strcpy(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->Address->IPv4Address ,_IPAddr_MUL);
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->Address->IPv6Address = NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->Port = 1234;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->TTL = 0;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->AutoStart = 0;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->__size = 0;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->__any = NULL;
	dom_att(&trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Multicast->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].SessionTimeout = oSysInfo->Oprofile[0].MDC.sessiontimeout;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].__size = 0;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].__any = NULL;
	dom_att(&trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].__anyAttribute, 0,NULL, NULL, NULL, NULL, soap);
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].AnalyticsEngineConfiguration= NULL;
	trt__GetCompatibleMetadataConfigurationsResponse->Configurations[0].Extension= NULL;

	return SOAP_OK;
}


int __trt__GetCompatibleAudioOutputConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioOutputConfigurations *trt__GetCompatibleAudioOutputConfigurations, struct _trt__GetCompatibleAudioOutputConfigurationsResponse *trt__GetCompatibleAudioOutputConfigurationsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

int __trt__GetCompatibleAudioDecoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioDecoderConfigurations *trt__GetCompatibleAudioDecoderConfigurations, struct _trt__GetCompatibleAudioDecoderConfigurationsResponse *trt__GetCompatibleAudioDecoderConfigurationsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/**
 * @brief Set Video Source Configuration 
 * @This operation modifies a video input configuration.
 *   
 * @param Request	:_trt__SetVideoSourceConfiguration structure contains the configuration element contains the modified VideoSource configuration. 
 * @param Response	:_trt__SetVideoSourceConfigurationResponse structure members contains message is empty.
 * @return               : On success - SOAP_OK.
 On failure - The requested VideoSource does not exist or
 The configuration parameters are not possible to set.
 */
int __trt__SetVideoSourceConfiguration(struct soap* soap, struct _trt__SetVideoSourceConfiguration *trt__SetVideoSourceConfiguration, struct _trt__SetVideoSourceConfigurationResponse *trt__SetVideoSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	video_source_conf set_video_source_t; 
	int i;
	int j = 0;
	int flg = 0;
	int Ptoken_exist = NOT_EXIST;
	int ret = 0;
	int num_token = 0;
	int _width = 0;
	int _height = 0;
	if(trt__SetVideoSourceConfiguration->Configuration->Bounds != NULL)
	{
		_width = trt__SetVideoSourceConfiguration->Configuration->Bounds->width;
		_height = trt__SetVideoSourceConfiguration->Configuration->Bounds->height;
	}
	for(i = 0; i < oSysInfo->nprofile ; i++)
	{
		for(j = 0; j <= i; j++)
			if(strcmp(oSysInfo->Oprofile[j].AVSC.Vtoken, oSysInfo->Oprofile[i].AVSC.Vtoken)==0);
		{
			flg = 1;		
			break;
		}
		if(flg == 0)
			num_token++;
	}

	if(trt__SetVideoSourceConfiguration->Configuration->token != NULL)
		for(i = 0; i < oSysInfo->nprofile; i++)
			if(strcmp(trt__SetVideoSourceConfiguration->Configuration->token, oSysInfo->Oprofile[i].AVSC.Vtoken) == 0)
			{
				Ptoken_exist = EXIST;
				break;
			}

	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	/* check for width and height */
	if(!((_width >= 320 && _width <= 1920) && (_height >= 192 && _height <= 1080)))
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}
	if(trt__SetVideoSourceConfiguration->Configuration->token != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			Ptoken_exist = NOT_EXIST;
			if(strcmp(trt__SetVideoSourceConfiguration->Configuration->token, oSysInfo->Oprofile[i].AVSC.Vtoken) == 0)
			{
				Ptoken_exist = EXIST;
			}

			if(Ptoken_exist)
			{

				set_video_source_t.position = i; 
				strcpy(set_video_source_t.video_source_conf_in_t.Vtoken_t, trt__SetVideoSourceConfiguration->Configuration->token);
				strcpy(set_video_source_t.video_source_conf_in_t.Vname_t, trt__SetVideoSourceConfiguration->Configuration->Name);
				strcpy(set_video_source_t.video_source_conf_in_t.Vsourcetoken_t, trt__SetVideoSourceConfiguration->Configuration->SourceToken);
				set_video_source_t.video_source_conf_in_t.Vcount_t = trt__SetVideoSourceConfiguration->Configuration->UseCount;	
				if(trt__SetVideoSourceConfiguration->Configuration->Bounds != NULL)
				{
					set_video_source_t.video_source_conf_in_t.windowx_t = trt__SetVideoSourceConfiguration->Configuration->Bounds->x;
					set_video_source_t.video_source_conf_in_t.windowy_t = trt__SetVideoSourceConfiguration->Configuration->Bounds->y;
					set_video_source_t.video_source_conf_in_t.windowwidth_t = _width;
					set_video_source_t.video_source_conf_in_t.windowheight_t = _height;
				}

				ret = ControlSystemData(SFIELD_SET_VIDEOSOURCE_CONF, &set_video_source_t, sizeof(set_video_source_t));
			}
		}
	}
	if(ret != SUCCESS)
	{		
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}
	return SOAP_OK; 
}


/**
 * @brief Set Video Encoder Configuration 
 * @This operation modifies a video encoder configuration.
 *   
 * @param Request	:_trt__SetVideoEncoderConfiguration structure contains the configuration element contains the modified video encoder configuration. 
 * @param Response	:_trt__SetVideoEncoderConfigurationResponse structure contains message is empty.
 * @return               : On success - SOAP_OK.
 On failure - The configuration does not exist.or
 The configuration parameters are not possible to set.
 */
int __trt__SetVideoEncoderConfiguration(struct soap* soap, struct _trt__SetVideoEncoderConfiguration *trt__SetVideoEncoderConfiguration, struct _trt__SetVideoEncoderConfigurationResponse *trt__SetVideoEncoderConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int i=0;
	int Ptoken_exist = NOT_EXIST;
	int ret = 0;
	__u8 codec_combo = 0;
	__u8 codec_res = 0;
	__u8 mode = 0;
	__u8 encodinginterval = 0;
	__u8 frameratelimit = 0;
	int _width = 0;
	int _height = 0;
	int bitrate = 0;
	int govlength = 0;
	int multicast = 0;

	video_encoder_conf set_video_encoder_conf;
	if(trt__SetVideoEncoderConfiguration->Configuration != NULL)
	{
		if(trt__SetVideoEncoderConfiguration->Configuration->Resolution != NULL)
		{
			_width = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Width;
			_height = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Height;
		}
	}
	if(trt__SetVideoEncoderConfiguration->Configuration->RateControl != NULL)
	{
		encodinginterval = trt__SetVideoEncoderConfiguration->Configuration->RateControl->EncodingInterval;
		bitrate = trt__SetVideoEncoderConfiguration->Configuration->RateControl->BitrateLimit;
		frameratelimit = trt__SetVideoEncoderConfiguration->Configuration->RateControl->FrameRateLimit;
	}
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__SetVideoEncoderConfiguration->Configuration->token, oSysInfo->Oprofile[i].AESC.VEtoken) == 0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	if( ( (_width % 2) != 0 || (_height % 2) != 0 ) || ((_width * _height) % 16 != 0))	
	{
		onvif_fault(soap,1,"ter:InvalidArgVal","ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}

	if(trt__SetVideoEncoderConfiguration->Configuration != NULL)
	{
		strcpy(set_video_encoder_conf.video_encoder_conf_in_t.VEname_t, trt__SetVideoEncoderConfiguration->Configuration->Name);
		strcpy(set_video_encoder_conf.video_encoder_conf_in_t.VEtoken_t, trt__SetVideoEncoderConfiguration->Configuration->token); 
		set_video_encoder_conf.video_encoder_conf_in_t.VEusercount_t = trt__SetVideoEncoderConfiguration->Configuration->UseCount;
		set_video_encoder_conf.video_encoder_conf_in_t.VEquality_t = trt__SetVideoEncoderConfiguration->Configuration->Quality;


		set_video_encoder_conf.video_encoder_conf_in_t.Vencoder_t = trt__SetVideoEncoderConfiguration->Configuration->Encoding;
		if(trt__SetVideoEncoderConfiguration->Configuration->Resolution != NULL)
		{
			set_video_encoder_conf.video_encoder_conf_in_t.Rwidth_t = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Width;
			set_video_encoder_conf.video_encoder_conf_in_t.Rheight_t = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Height;
		}
		if(trt__SetVideoEncoderConfiguration->Configuration->RateControl != NULL)
		{
			set_video_encoder_conf.video_encoder_conf_in_t.frameratelimit_t = trt__SetVideoEncoderConfiguration->Configuration->RateControl->FrameRateLimit;
			set_video_encoder_conf.video_encoder_conf_in_t.encodinginterval_t = trt__SetVideoEncoderConfiguration->Configuration->RateControl->EncodingInterval;
			set_video_encoder_conf.video_encoder_conf_in_t.bitratelimit_t = bitrate;
		}
		else
		{
			set_video_encoder_conf.video_encoder_conf_in_t.frameratelimit_t = 0;
			set_video_encoder_conf.video_encoder_conf_in_t.encodinginterval_t = 0;
			set_video_encoder_conf.video_encoder_conf_in_t.bitratelimit_t = 0;
		}
		if(trt__SetVideoEncoderConfiguration->Configuration->MPEG4 != NULL)
		{
			set_video_encoder_conf.video_encoder_conf_in_t.Mpeggovlength_t = trt__SetVideoEncoderConfiguration->Configuration->MPEG4->GovLength;
			set_video_encoder_conf.video_encoder_conf_in_t.Mpegprofile_t  = trt__SetVideoEncoderConfiguration->Configuration->MPEG4->Mpeg4Profile;
		}
		else
		{
			set_video_encoder_conf.video_encoder_conf_in_t.Mpeggovlength_t =  oSysInfo->Oprofile[i].AESC.Mpeggovlength;
			set_video_encoder_conf.video_encoder_conf_in_t.Mpegprofile_t = oSysInfo->Oprofile[i].AESC.Mpegprofile;
		}
		if(trt__SetVideoEncoderConfiguration->Configuration->H264 != NULL)
		{
			set_video_encoder_conf.video_encoder_conf_in_t.H264govlength_t = trt__SetVideoEncoderConfiguration->Configuration->H264->GovLength;
			set_video_encoder_conf.video_encoder_conf_in_t.H264profile_t = trt__SetVideoEncoderConfiguration->Configuration->H264->H264Profile;
		}
		else
		{
			set_video_encoder_conf.video_encoder_conf_in_t.H264govlength_t =  oSysInfo->Oprofile[i].AESC.H264govlength;
			set_video_encoder_conf.video_encoder_conf_in_t.H264profile_t = oSysInfo->Oprofile[i].AESC.H264profile;
		}

		set_video_encoder_conf.video_encoder_conf_in_t.sessiontimeout_t = trt__SetVideoEncoderConfiguration->Configuration->SessionTimeout;
		if(trt__SetVideoEncoderConfiguration->Configuration->Multicast != NULL)
		{
			if(trt__SetVideoEncoderConfiguration->Configuration->Multicast->Address->Type == 1)
			{
				onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","IPV6 not supported and address can not be set");
				return SOAP_FAULT;

			}
			multicast = 1;
			set_video_encoder_conf.video_encoder_conf_in_t.Multicast_Conf_t.nIPType_t = trt__SetVideoEncoderConfiguration->Configuration->Multicast->Address->Type;
			if(trt__SetVideoEncoderConfiguration->Configuration->Multicast->Address->IPv4Address != NULL)
				strcpy(set_video_encoder_conf.video_encoder_conf_in_t.Multicast_Conf_t.IPv4Addr_t, trt__SetVideoEncoderConfiguration->Configuration->Multicast->Address->IPv4Address);
			set_video_encoder_conf.video_encoder_conf_in_t.Multicast_Conf_t.port_t = trt__SetVideoEncoderConfiguration->Configuration->Multicast->Port;
			set_video_encoder_conf.video_encoder_conf_in_t.Multicast_Conf_t.ttl_t = trt__SetVideoEncoderConfiguration->Configuration->Multicast->TTL;
			set_video_encoder_conf.video_encoder_conf_in_t.Multicast_Conf_t.autostart_t = trt__SetVideoEncoderConfiguration->Configuration->Multicast->AutoStart;
		}
	}
	for(i = 0 ;i < oSysInfo->nprofile; i++)
	{
		Ptoken_exist = NOT_EXIST;
		if(strcmp(trt__SetVideoEncoderConfiguration->Configuration->token, oSysInfo->Oprofile[i].AESC.VEtoken)==0)
		{
			Ptoken_exist = EXIST;
		}
		if(Ptoken_exist)
		{
			set_video_encoder_conf.position = i;
			break;
		}
	}
	if(Ptoken_exist)
	{
		ret = ControlSystemData(SFIELD_SET_VIDEOENCODER_CONF, &set_video_encoder_conf, sizeof(set_video_encoder_conf));
	}

	if(ret != SUCCESS)
	{	
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;

	}

	//StreamType 0:single 1:dual 2:tri
	//VideoCodecCombo , value modified in function SetVideoCodecMode() based on Streamtype
	//check  StartStream() function for videocodecres 
	if (trt__SetVideoEncoderConfiguration->Configuration->Encoding == 0)  //jpeg
	{
		if(((_width == 720) && (_height == 480)))
		{
			mode = 1; 
			codec_combo = 0; 
			codec_res = 0; 
		}
		else if(((_width == 1920) && (_height == 1080)))
		{
			mode = 0;
			codec_combo = 2;
			codec_res = 0;
		}
		else if(((_width == 1280) && (_height == 720)))
		{
			mode = 0;
			codec_combo = 2;
			codec_res = 0;
		}
		else 
		{
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:ConfigModify","Parameters can not be set");
			return SOAP_FAULT;	
		}
	}
	else if(trt__SetVideoEncoderConfiguration->Configuration->Encoding == 1) //mpeg4
	{
		if((_width == 720) && (_height == 480))
		{
			mode = 1;
			codec_combo = 3;
			codec_res = 0;
		}
		else if((_width == 1920) && (_height == 1080))
		{
			mode = 1;
			codec_combo = 3;
			codec_res = 0;
		}
		else if(((_width == 1280) && (_height == 720)))
		{
			mode = 1;
			codec_combo = 3;
			codec_res = 0;
		}
		else 
		{
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:ConfigModify","Parameters can not be set");
			return SOAP_FAULT;	
		}
		if(trt__SetVideoEncoderConfiguration->Configuration->MPEG4 != NULL)
		{
			govlength = trt__SetVideoEncoderConfiguration->Configuration->MPEG4->GovLength;
		}
		if((govlength < 0) || (govlength > 60))
		{
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:ConfigModify","Parameters can not be set");
			return SOAP_FAULT;
		}

	}	
	else if(trt__SetVideoEncoderConfiguration->Configuration->Encoding == 2)  //H.264
	{
		if((_width == 720) && (_height == 480))
		{
			mode = 1;
			codec_combo = 2;
			codec_res = 0;
		}
		else if((_width == 1920) && (_height == 1080))
		{
			mode = 1;
			codec_combo = 2;
			codec_res = 0;
		}
		else if((_width == 1280) && (_height == 720))
		{
			mode = 1;
			codec_combo = 2;
			codec_res = 0;
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
			return SOAP_FAULT;
		}
		if(trt__SetVideoEncoderConfiguration->Configuration->H264 != NULL)
		{
			govlength = trt__SetVideoEncoderConfiguration->Configuration->H264->GovLength;
		}
		if((govlength < 0) || (govlength > 60))
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
			return SOAP_FAULT;
		}

	}

	#if MULTICAST_ENABLE
	if(multicast)
		ControlSystemData(SFIELD_SET_MULTICAST, (unsigned char *)&multicast, sizeof(unsigned char));
	#endif
	ControlSystemData(SFIELD_SET_VIDEO_MODE, &mode, sizeof(mode));
	ControlSystemData(SFIELD_SET_MPEG41_BITRATE, &bitrate, sizeof(bitrate));
	ControlSystemData(SFIELD_SET_VIDEOCODECCOMBO, &codec_combo, sizeof(codec_combo));
	ControlSystemData(SFIELD_SET_VIDEOCODECRES, &codec_res, sizeof(codec_res));
	oset_framerate(codec_combo,codec_res,frameratelimit);


	return SOAP_OK;
}


/** 
 * @brief Set Audio Source Configuration
 * @This operation modifies an audio source configuration.

 * @param Request	:_trt__SetAudioSourceConfiguration structure configuration element contains the modified audio source configuration.
 * @param Response	:_trt__SetAudioSourceConfigurationResponse structure contains message is empty.
 * @return               : On success - SOAP_OK.
 On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 11.7.5)
 */
int __trt__SetAudioSourceConfiguration(struct soap* soap, struct _trt__SetAudioSourceConfiguration *trt__SetAudioSourceConfiguration, struct _trt__SetAudioSourceConfigurationResponse *trt__SetAudioSourceConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	char _Token[SMALL_INFO_LENGTH]; 
	char _Name[SMALL_INFO_LENGTH]; 
	char _SourceToken[SMALL_INFO_LENGTH]; 
	int i, use;
	int flag = 0;
	Add_AudioSource_Configuration_tmp Add_Audio_Conf;

	strcpy(_Name, trt__SetAudioSourceConfiguration->Configuration->Name);
	strcpy(_Token, trt__SetAudioSourceConfiguration->Configuration->token);
	strcpy(_SourceToken, trt__SetAudioSourceConfiguration->Configuration->SourceToken);

	for(i = 0; i < oSysInfo->nprofile; i++)
		if(strcmp(_Token, oSysInfo->Oprofile[i].AASC.Atoken) == 0)
		{
			Add_Audio_Conf.position = i;
			break;
		}
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1, "ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(_SourceToken, oSysInfo->Oprofile[i].AASC.Asourcetoken) == 0)
		{
			flag = EXIST;
			break;
		}
	}
	if(!flag)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}

	use = 0;
	for(i = 0; i < oSysInfo->nprofile; i++)
		if(strcmp(trt__SetAudioSourceConfiguration->Configuration->token, oSysInfo->Oprofile[i].AASC.Atoken) == 0)
			use++;

	strcpy(Add_Audio_Conf.Add_AudioSource_in.Aname_t, _Name);
	Add_Audio_Conf.Add_AudioSource_in.Ausecount_t = trt__SetAudioSourceConfiguration->Configuration->UseCount;
	strcpy(Add_Audio_Conf.Add_AudioSource_in.Atoken_t, _Token);
	strcpy(Add_Audio_Conf.Add_AudioSource_in.Asourcetoken_t, _SourceToken);
	ControlSystemData(SFIELD_SET_AUDIO_SOURCE, &Add_Audio_Conf, sizeof(Add_Audio_Conf));

	return SOAP_OK;
}

/**
 * @brief Set Audio Encoder Configuration 
 * @This operation modifies an audio encoder configuration.
 *   
 * @param Request	:_trt__SetAudioEncoderConfiguration structure contains the configuration element contains the modified audio encoderconfiguration. 
 * @param Response	:_trt__SetAudioEncoderConfigurationResponse structure contains message is empty.
 * @return               : On success - SOAP_OK.
 On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 11.8.5)
 */
int __trt__SetAudioEncoderConfiguration(struct soap* soap, struct _trt__SetAudioEncoderConfiguration *trt__SetAudioEncoderConfiguration, struct _trt__SetAudioEncoderConfigurationResponse *trt__SetAudioEncoderConfigurationResponse)	{

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	__u8 _Bitrate = trt__SetAudioEncoderConfiguration->Configuration->Bitrate;
	__u8 _SampleRate = trt__SetAudioEncoderConfiguration->Configuration->SampleRate;
	__u8 _Encoding = trt__SetAudioEncoderConfiguration->Configuration->Encoding;

	__u8 _Enable = 1;
	__u8 _Mode = 2;//0 - Mic, 1-Speaker ,2-Both
	Add_AudioEncoder_Configuration_tmp add_audio_encoder;
	int i;
	int flag = 0;
	int multicast = 0;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__SetAudioEncoderConfiguration->Configuration->token, oSysInfo->Oprofile[i].AAEC.AEtoken) == 0)
		{
			flag = EXIST;
			add_audio_encoder.position = i;
			break;
		}
	}
	if(!flag)
	{
		onvif_fault(soap,1, "ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}
	if(_Encoding != 0)
	{
		onvif_fault(soap,1, "ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}

	if(_Encoding == 0 || _Encoding == 1) //G711 || G726
	{
		_Encoding = 0;
		if(_Bitrate == 64)
		{
			_Bitrate = 0;
		}
		else if(_Bitrate == 128)
		{
			_Bitrate = 1;
		}
		else
		{
			onvif_fault(soap,1, "ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");        
			return SOAP_FAULT;
		}
	}
	else if(_Encoding == 2) //AAC LC
	{
		_Encoding = 1;
		if(_Bitrate == 64)                                     
		{
			_Bitrate = 0;
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
			return SOAP_FAULT;
		}
	}

	if(_SampleRate == 8)                                      
	{
		_SampleRate = 0;
	}
	else if(_SampleRate == 16)
	{
		_SampleRate = 1;
	}
	else
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}


	strcpy(add_audio_encoder.Add_AudioEncoder_in.AEname_t, trt__SetAudioEncoderConfiguration->Configuration->Name);
	strcpy(add_audio_encoder.Add_AudioEncoder_in.AEtoken_t, trt__SetAudioEncoderConfiguration->Configuration->token);
	//add_audio_encoder.Add_AudioEncoder_in.AEusercount_t = trt__SetAudioEncoderConfiguration->Configuration->UseCount;
	add_audio_encoder.Add_AudioEncoder_in.AEusercount_t = oSysInfo->Oprofile[i].AAEC.AEusercount;
	add_audio_encoder.Add_AudioEncoder_in.bitrate_t = _Bitrate;
	add_audio_encoder.Add_AudioEncoder_in.samplerate_t = _SampleRate;
	add_audio_encoder.Add_AudioEncoder_in.AEencoding_t = _Encoding;
	if(trt__SetAudioEncoderConfiguration->Configuration->Multicast != NULL)
	{
		if(trt__SetAudioEncoderConfiguration->Configuration->Multicast->Address->Type == 1)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","IPV6 not supported and address can not be set");
			return SOAP_FAULT;
		}
		multicast = 1;
		add_audio_encoder.multicast_in.nIPType_t = trt__SetAudioEncoderConfiguration->Configuration->Multicast->Address->Type;
		if(trt__SetAudioEncoderConfiguration->Configuration->Multicast->Address->IPv4Address != NULL)
			strcpy(add_audio_encoder.multicast_in.IPv4Addr_t, trt__SetAudioEncoderConfiguration->Configuration->Multicast->Address->IPv4Address);
		add_audio_encoder.multicast_in.port_t = trt__SetAudioEncoderConfiguration->Configuration->Multicast->Port;
		add_audio_encoder.multicast_in.ttl_t = trt__SetAudioEncoderConfiguration->Configuration->Multicast->TTL;
		add_audio_encoder.multicast_in.autostart_t = trt__SetAudioEncoderConfiguration->Configuration->Multicast->AutoStart;
	}

	ControlSystemData(SFIELD_SET_AUDIOENCODER_CONF, &add_audio_encoder, sizeof(add_audio_encoder));


	if(oSysInfo->audio_config.audioenable != _Enable)
	{
		ControlSystemData(SFIELD_SET_AUDIOENABLE, &_Enable, sizeof(_Enable));
		ControlSystemData(SFIELD_SET_AUDIOMODE, &_Mode, sizeof(_Mode));
	}

	#if MULTICAST_ENABLE
	if(multicast)
		ControlSystemData(SFIELD_SET_MULTICAST, (unsigned char *)&multicast, sizeof(unsigned char));
	#endif
		
	if(oSysInfo->audio_config.codectype != _Encoding)
	{
		ControlSystemData(SFIELD_SET_AUDIO_ENCODE, &_Encoding, sizeof(_Encoding));
	}

	if(oSysInfo->audio_config.bitrate != _Bitrate)
	{
		ControlSystemData(SFIELD_SET_AUDIO_BITRATE, &_Bitrate, sizeof(_Bitrate));
	}

	if(oSysInfo->audio_config.samplerate != _SampleRate)
	{
		ControlSystemData(SFIELD_SET_AUDIO_SAMPLERATE, &_SampleRate, sizeof(_SampleRate));
	}

	return SOAP_OK; 
}


int __trt__SetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__SetVideoAnalyticsConfiguration *trt__SetVideoAnalyticsConfiguration, struct _trt__SetVideoAnalyticsConfigurationResponse *trt__SetVideoAnalyticsConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int Ptoken_exist;
	video_analytics_conf video_analytics;
	int i = 0;
	for(i = 1; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__SetVideoAnalyticsConfiguration->Configuration->token, oSysInfo->Oprofile[1].AVAC.VAtoken)==0)
		{
			Ptoken_exist = EXIST;
			break;
		}
	}
	/* if ConfigurationToken does not exist */
	if(!Ptoken_exist) 
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	video_analytics.position = i;
	if(trt__SetVideoAnalyticsConfiguration->Configuration != NULL)
	{ 
		strcpy(video_analytics.video_analytics_conf_t.VAname_t, trt__SetVideoAnalyticsConfiguration->Configuration->Name);
		strcpy(video_analytics.video_analytics_conf_t.VAtoken_t, trt__SetVideoAnalyticsConfiguration->Configuration->token);
		video_analytics.video_analytics_conf_t.VAusercount_t = trt__SetVideoAnalyticsConfiguration->Configuration->UseCount;
		if(trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration != NULL)
		{
			video_analytics.video_analytics_conf_t.VAMsize_t = trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->__sizeAnalyticsModule;
			if(trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule != NULL)
			{
				if(trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters != NULL)
				{
					video_analytics.video_analytics_conf_t.VAsizeitem_t = trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeSimpleItem;
					if(trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem != NULL)
					{
						strcpy(video_analytics.video_analytics_conf_t.VASname_t, trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Name);
						strcpy(video_analytics.video_analytics_conf_t.VASvalue_t, trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value);
					}
					video_analytics.video_analytics_conf_t.VAEsize_t =trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeElementItem;
					if(trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem != NULL);
					{
						strcpy(video_analytics.video_analytics_conf_t.VAEname_t, trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->Name);
					}

					strcpy(video_analytics.video_analytics_conf_t.Aconfigname_t, trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Name);
					strcpy(video_analytics.video_analytics_conf_t.Aconfigtype_t, trt__SetVideoAnalyticsConfiguration->Configuration->AnalyticsEngineConfiguration->AnalyticsModule->Type);
				}
			}
		}

		if(trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration != NULL)
		{  
			video_analytics.video_analytics_conf_t.VARsize_t = trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->__sizeRule;
			if(trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule != NULL)
			{
				if(trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters != NULL)
				{
					video_analytics.video_analytics_conf_t.VARsize_t = trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters->__sizeSimpleItem;
					if(trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem != NULL)
					{
						strcpy(video_analytics.video_analytics_conf_t.VARSname_t, trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem->Name);
						strcpy(video_analytics.video_analytics_conf_t.VARSvalue_t, trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters->SimpleItem->Value);
					}
					video_analytics.video_analytics_conf_t.VAREsize_t =trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters->__sizeElementItem;
					if(trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters->ElementItem != NULL);
					{
						strcpy(video_analytics.video_analytics_conf_t.VAREname_t, trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Parameters->ElementItem->Name);
					}

					strcpy(video_analytics.video_analytics_conf_t.Rconfigname_t, trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Name);
					strcpy(video_analytics.video_analytics_conf_t.Rconfigtype_t, trt__SetVideoAnalyticsConfiguration->Configuration->RuleEngineConfiguration->Rule->Type);
				}
			}
		}
	} 

	ControlSystemData(SFIELD_SET_VIDEOANALYTICS_CONF, &video_analytics, sizeof(video_analytics));

	return SOAP_OK;
}




int __trt__SetMetadataConfiguration(struct soap* soap, struct _trt__SetMetadataConfiguration *trt__SetMetadataConfiguration, struct _trt__SetMetadataConfigurationResponse *trt__SetMetadataConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	Metadata_configuration_tmp metadata; 
	int i;
	int ptoken_exist = NOT_EXIST;
	int ret = 0;
	int multicast = 0;		
	if(trt__SetMetadataConfiguration->Configuration->token != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__SetMetadataConfiguration->Configuration->token, oSysInfo->Oprofile[i].MDC.MDtoken) == 0)
			{
				ptoken_exist = EXIST;
				break;
			}
		}
		if(!ptoken_exist) 
		{
			onvif_fault(soap,1, "ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
			return SOAP_FAULT;
		}
	}

	metadata.position = i;
	strcpy(metadata.MDC_in.MDname_t,trt__SetMetadataConfiguration->Configuration->Name);
	metadata.MDC_in.MDusecount_t = trt__SetMetadataConfiguration->Configuration->UseCount;
	strcpy(metadata.MDC_in.MDtoken_t, trt__SetMetadataConfiguration->Configuration->token);
	metadata.MDC_in.MDanalytics_t = trt__SetMetadataConfiguration->Configuration->Analytics;
	ret = periodtol(trt__SetMetadataConfiguration->Configuration->SessionTimeout);
	if(ret == -1)
	{
		onvif_fault(soap,1, "ter:InvalidArgVal", "ter:ConfigModify","Parameters can not be set");
		return SOAP_FAULT;
	}

	if(trt__SetMetadataConfiguration->Configuration->Multicast->Address->Type == 1)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","IPV6 not supported and address can not be set");
		return SOAP_FAULT;
	}

	metadata.MDC_in.sessiontimeout_t = trt__SetMetadataConfiguration->Configuration->SessionTimeout;
	if(trt__SetMetadataConfiguration->Configuration->Multicast != NULL)
	{
		if(trt__SetMetadataConfiguration->Configuration->Multicast->Address->Type == 1)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:ConfigModify","IPV6 not supported and address can not be set");
			return SOAP_FAULT;

		}
		multicast = 1;		
		metadata.MDC_in.VMC_t.nIPType_t = trt__SetMetadataConfiguration->Configuration->Multicast->Address->Type;

		if(trt__SetMetadataConfiguration->Configuration->Multicast->Address->IPv4Address != NULL)
			strcpy(metadata.MDC_in.VMC_t.IPv4Addr_t, (const char *)trt__SetMetadataConfiguration->Configuration->Multicast->Address->IPv4Address);

		metadata.MDC_in.VMC_t.port_t = trt__SetMetadataConfiguration->Configuration->Multicast->Port;
		metadata.MDC_in.VMC_t.ttl_t  = trt__SetMetadataConfiguration->Configuration->Multicast->TTL;
		metadata.MDC_in.VMC_t.autostart_t = trt__SetMetadataConfiguration->Configuration->Multicast->AutoStart;
	}

	ControlSystemData(SFIELD_SET_METADATA, &metadata, sizeof(metadata));

	#if MULTICAST_ENABLE
	if(multicast)
		ControlSystemData(SFIELD_SET_MULTICAST, (unsigned char *)&multicast, sizeof(unsigned char));
	#endif

	return SOAP_OK;	
}


int __trt__SetAudioOutputConfiguration(struct soap* soap, struct _trt__SetAudioOutputConfiguration *trt__SetAudioOutputConfiguration, struct _trt__SetAudioOutputConfigurationResponse *trt__SetAudioOutputConfigurationResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int i;

	for(i = 0; i < oSysInfo->nprofile; i++)
		if(strcmp(trt__SetAudioOutputConfiguration->Configuration->token, oSysInfo->Oprofile[i].AAOC.AOtoken) == 0)
			break;
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal","ter:NoConfig","Configuration token does not exist");
		return SOAP_FAULT;
	}

	int _Volume = trt__SetAudioOutputConfiguration->Configuration->OutputLevel;
	ControlSystemData(SFIELD_SET_AUDIOOUTVOLUME, &_Volume, sizeof(_Volume));

	return SOAP_FAULT;	
}


int __trt__SetAudioDecoderConfiguration(struct soap* soap, struct _trt__SetAudioDecoderConfiguration *trt__SetAudioDecoderConfiguration, struct _trt__SetAudioDecoderConfigurationResponse *trt__SetAudioDecoderConfigurationResponse) {
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}

/** 
 * @brief Get Video Source Configuration
 * @This operation returns the available options when the video source parameters are reconfigured If a video source configuration is specified,the options shall concern that
 particular configuration.
 *   
 * @param Request	:_trt__GetVideoSourceConfigurationOptions structure contains optional tokens of a video source configuration and a media profile.
 * @param Response	:_trt__GetVideoSourceConfigurationOptionsResponse structure contains the video configuration options.
 * @return               : On success - SOAP_OK.
 On failure - The requested Video Input does not exist
 */  
int __trt__GetVideoSourceConfigurationOptions(struct soap* soap, struct _trt__GetVideoSourceConfigurationOptions *trt__GetVideoSourceConfigurationOptions, struct _trt__GetVideoSourceConfigurationOptionsResponse *trt__GetVideoSourceConfigurationOptionsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i;
	int num_token = 0;
	int j;
	int flag = NOT_EXIST;
	int flg = 0;
	int index = 0;
	int num = 0;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		for(j = 0; j < i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].AVSC.Vtoken,oSysInfo->Oprofile[i].AVSC.Vtoken) == 0);
			{
				flg = 1;
				break;
			}
		}
		if(flg == 0)
		{
			num_token++;
		}
	}

	if(trt__GetVideoSourceConfigurationOptions->ConfigurationToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetVideoSourceConfigurationOptions->ConfigurationToken,oSysInfo->Oprofile[i].AVSC.Vtoken) == 0)
			{
				flag = EXIST;
				index = i;
				break;
			}
		}
	}

	if(trt__GetVideoSourceConfigurationOptions->ProfileToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetVideoSourceConfigurationOptions->ProfileToken, oSysInfo->Oprofile[i].profiletoken)==0)
			{
				flag = EXIST;
				index = i;
				break;
			}
		}
	}

	if(!flag)
	{
		onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
		return SOAP_FAULT;
	}
	else
	{
		trt__GetVideoSourceConfigurationOptionsResponse->Options = (struct tt__VideoSourceConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfigurationOptions));
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange = (struct tt__IntRectangleRange *)soap_malloc(soap, sizeof(struct tt__IntRectangleRange));
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->XRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->XRange->Min = 0;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->XRange->Max = 0;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->YRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->YRange->Min = 0;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->YRange->Max = 0;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->WidthRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->WidthRange->Min = 320;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->WidthRange->Max = 1920;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->HeightRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->HeightRange->Min = 192;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->HeightRange->Max = 1080;
		trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable = (char **)soap_malloc(soap, sizeof(char *) * 1);
		if(trt__GetVideoSourceConfigurationOptions->ProfileToken == NULL && trt__GetVideoSourceConfigurationOptions->ConfigurationToken == NULL)
		{
			trt__GetVideoSourceConfigurationOptionsResponse->Options->__sizeVideoSourceTokensAvailable = num_token;
			for(i = 0;i < oSysInfo->nprofile; i++)
			{
				for(j = 0; j < i; j++)
				{
					if(strcmp(oSysInfo->Oprofile[j].AVSC.Vtoken, oSysInfo->Oprofile[i].AVSC.Vtoken) == 0)
					{
						flg = 1;
						break;
					}
				}
				if(flg == 0)
				{

					trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable[num] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
					strcpy(trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable[num], oSysInfo->Oprofile[j].AVSC.Vsourcetoken);
					num++;
				}
				flg = 0;
			}
		}
		else
		{
			trt__GetVideoSourceConfigurationOptionsResponse->Options->__sizeVideoSourceTokensAvailable = 1;
			trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
			strcpy(trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable[0], oSysInfo->Oprofile[index].AVSC.Vsourcetoken);
		}
		trt__GetVideoSourceConfigurationOptionsResponse->Options->Extension = NULL;
		dom_att(&trt__GetVideoSourceConfigurationOptionsResponse->Options->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap); 
	}

	return SOAP_OK; 
}


/**
 * @brief Get Video Encoder COnfiguration Options. 
 * @This operation returns the available options when the video encoder parameters are reconfigured.
 *   
 * @param Request	:_trt__GetVideoEncoderConfigurationOptions structure contains  optional tokens of a video encoder configuration and a media profile.
 * @param Response	:_trt__GetVideoEncoderConfigurationOptionsResponse structure contains the video configuration options.
 * @return               : On success - SOAP_OK.
 On failure - No command specific faults!
 */
int __trt__GetVideoEncoderConfigurationOptions(struct soap* soap, struct _trt__GetVideoEncoderConfigurationOptions *trt__GetVideoEncoderConfigurationOptions, struct _trt__GetVideoEncoderConfigurationOptionsResponse *trt__GetVideoEncoderConfigurationOptionsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	int i;
	char JPEG_profile;
	char MPEG4_profile;
	char H264_profile;
	char PToken[SMALL_INFO_LENGTH];
	char CToken[SMALL_INFO_LENGTH];

	SysInfo *oSysInfo = GetSysInfo();
	trt__GetVideoEncoderConfigurationOptionsResponse->Options  = (struct tt__VideoEncoderConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfigurationOptions)); 
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG  = NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4 = NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264  = NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension  = NULL;

	trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension = (struct tt__VideoEncoderOptionsExtension *)soap_malloc(soap, sizeof(struct tt__VideoEncoderOptionsExtension));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG  = NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264  = NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4  = NULL;

	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange  = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange->Min = 10; //dummy
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange->Max = 100; //dummy

	if(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken != NULL && strcmp(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken, "") != 0)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken, oSysInfo->Oprofile[i].AESC.VEtoken) == 0)
			{
				if(oSysInfo->Oprofile[i].AESC.Vencoder == 0)
				{
					JPEG_profile = EXIST;
				}
				else if(oSysInfo->Oprofile[i].AESC.Vencoder == 1)
				{
					MPEG4_profile = EXIST;
				}
				else if(oSysInfo->Oprofile[i].AESC.Vencoder == 2)
				{
					H264_profile = EXIST;
				}
				break;
			}
		}
		if(i >= oSysInfo->nprofile)
		{
			onvif_fault(soap,1, "ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
			return SOAP_FAULT;
		}
	}
	else if(trt__GetVideoEncoderConfigurationOptions->ProfileToken != NULL && strcmp(trt__GetVideoEncoderConfigurationOptions->ProfileToken, "") != 0)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetVideoEncoderConfigurationOptions->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
			{
				if(oSysInfo->Oprofile[i].AESC.Vencoder == 0)
				{
					JPEG_profile = EXIST;
				}
				else if(oSysInfo->Oprofile[i].AESC.Vencoder == 1)
				{
					MPEG4_profile = EXIST;
				}
				else if(oSysInfo->Oprofile[i].AESC.Vencoder == 2)
				{
					H264_profile = EXIST;
				}
				break;
			}
		}
		if(i >= oSysInfo->nprofile)
		{
			onvif_fault(soap,1, "ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
			return SOAP_FAULT;
		}
	}
	else
	{
		JPEG_profile = EXIST;
		MPEG4_profile = EXIST;
		H264_profile = EXIST;
	}

	if(JPEG_profile)
	{
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG  = (struct tt__JpegOptions *)soap_malloc(soap, sizeof(struct tt__JpegOptions)); 
		// 10MP(3648X2736) |  8MP(3264X2448) | 5MP(2592x1920 or 2584X1936) 1080p(1920X1080) D1(720X480)
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->__sizeResolutionsAvailable = 2; 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable  = (struct tt__VideoResolution *)soap_malloc(soap, (trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->__sizeResolutionsAvailable) * sizeof(struct tt__VideoResolution)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[0].Width  = 720;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[0].Height = 480;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[1].Width  = 1920;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[1].Height = 1080;

#if 0
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[2].Width  = 2592;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[2].Height = 1920;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[3].Width  = 3264;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[3].Height = 2448;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[4].Width  = 3648;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[4].Height = 2736;
#endif        
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange  = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange->Min = 5; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange->Max = 60; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange  = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange->Min = 0; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange->Max = 3; //dummy

	}

	if(MPEG4_profile)
	{
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4 = (struct tt__Mpeg4Options *)soap_malloc(soap, sizeof(struct tt__Mpeg4Options));

		//720(1280x720) : D1(720x480) : SXVGA(1280x960) : 1080(1920x1080) : 720MAX60(1280x720) -- supported in product datasheet 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->__sizeResolutionsAvailable = 2;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->ResolutionsAvailable  = (struct tt__VideoResolution *)soap_malloc(soap, 2*sizeof(struct tt__VideoResolution)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->ResolutionsAvailable[0].Width  = 720; 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->ResolutionsAvailable[0].Height = 480; 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->ResolutionsAvailable[1].Width  = 1920; 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->ResolutionsAvailable[1].Height = 1080; 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->GovLengthRange  = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->GovLengthRange->Min = 0;  //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->GovLengthRange->Max = 60; //dummy         
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->FrameRateRange  = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->FrameRateRange->Min = 5; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->FrameRateRange->Max = 60; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->EncodingIntervalRange  = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->EncodingIntervalRange->Min = 0; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->EncodingIntervalRange->Max = 3; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->__sizeMpeg4ProfilesSupported = 1;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->Mpeg4ProfilesSupported = (int *)soap_malloc(soap, sizeof(int));
		*trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4->Mpeg4ProfilesSupported = 0; //SP = 0, ASP = 1
	}

	if(H264_profile)
	{
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264  = (struct tt__H264Options *)soap_malloc(soap, sizeof(struct tt__H264Options)); 
		// 10MP(3648X2736) | 8MP(3264X2448) | 5MP(2592x1920 or 2584X1936) 1080p(1920X1080) D1(720X480) - supoorted in product datasheet
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeResolutionsAvailable = 2; 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable  = (struct tt__VideoResolution *)soap_malloc(soap, (trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeResolutionsAvailable) * sizeof(struct tt__VideoResolution)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[0].Width  = 720;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[0].Height = 480;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[1].Width  = 1920;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[1].Height = 1080;
#if 0 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[2].Width  = 2592;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[2].Height = 1920;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[3].Width  = 3264;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[3].Height = 2448;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[4].Width  = 3648;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[4].Height = 2736;
#endif      
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange  = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange->Min = 0; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange->Max = 60; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange  = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange->Min = 5; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange->Max = 60; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange  = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange->Min = 1; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange->Max = 3; //dummy
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeH264ProfilesSupported = 3;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported = (int *)soap_malloc(soap, 3 * sizeof(int));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported[0] = 0; //Baseline = 0, Main = 1, High = 3
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported[1] = 1;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported[2] = 3;
		//ProfileIDC = 100 # Profile IDC (66=baseline, 77=main,
		//Z:\DM_365\IPNetCam\ipnc\av_capture\framework\alg\src\alg_vidEnc.c=pObj->params_h264.profileIdc = 100;

	}
	if(JPEG_profile)
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension = NULL;
	else
	{
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension = (struct tt__VideoEncoderOptionsExtension *)soap_malloc(soap, sizeof(struct tt__VideoEncoderOptionsExtension));
		if(MPEG4_profile)
		{
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->__size = 0;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->__any = NULL; 
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG = NULL;                                         
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4  = (struct tt__Mpeg4Options2 *)soap_malloc(soap, sizeof(struct tt__Mpeg4Options2)); 
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->ResolutionsAvailable = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->GovLengthRange = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->FrameRateRange = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->EncodingIntervalRange = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->Mpeg4ProfilesSupported = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->BitrateRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->BitrateRange->Min = 0;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->BitrateRange->Max = 64000;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->__size = 0;		
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->__any = NULL;			
			dom_att(&trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->MPEG4->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

			trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264 = NULL;                                         
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->Extension = NULL;
		}
		if(H264_profile)
		{
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->__size = 0;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->__any = NULL; 
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG = NULL;                                        
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4 = NULL;                                         
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264  = (struct tt__H264Options2 *)soap_malloc(soap, sizeof(struct tt__H264Options2)); 
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->ResolutionsAvailable = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->GovLengthRange = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->FrameRateRange = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->EncodingIntervalRange = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->H264ProfilesSupported = NULL;			
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->BitrateRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange)); 
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->BitrateRange->Min = 0;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->BitrateRange->Max = 64000;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->__size = 0;		
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->__any = NULL;			
			dom_att(&trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

			trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->Extension = NULL;
		}

	}
	dom_att(&trt__GetVideoEncoderConfigurationOptionsResponse->Options->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK; 
}


/** 
 * @brief Get Audio Source Configuration Options
 * @This operation returns the available options when the audio source parameters are reconfigured. If an audio source configuration is specified, the options shall concern that
 particular configuration.
 *   
 * @param Request	:_trt__GetAudioSourceConfigurationOptions structure contains optional tokens of an audio source configuration and a media profile.
 * @param Response	:_trt__GetAudioSourceConfigurationOptionsResponse structure contains the audio configuration options.
 * @return               : On success - SOAP_OK.
 On failure - NVT does not support audio.
 */
int __trt__GetAudioSourceConfigurationOptions(struct soap* soap, struct _trt__GetAudioSourceConfigurationOptions *trt__GetAudioSourceConfigurationOptions, struct _trt__GetAudioSourceConfigurationOptionsResponse *trt__GetAudioSourceConfigurationOptionsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int profile_exist = 0;
	int i = 0;
	int index = 0;

	if(trt__GetAudioSourceConfigurationOptions->ProfileToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetAudioSourceConfigurationOptions->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
			{
				profile_exist = EXIST;
				index = i;
				break;
			}
		}
		if(!profile_exist)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
			return SOAP_FAULT;
		}
	}

	if(trt__GetAudioSourceConfigurationOptions->ConfigurationToken != NULL)
	{
		profile_exist = 0;
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetAudioSourceConfigurationOptions->ConfigurationToken, oSysInfo->Oprofile[i].AASC.Atoken) == 0)
			{
				profile_exist = EXIST;
				index = i;
				break;
			}
		}
		if(!profile_exist)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
			return SOAP_FAULT;
		}
	}

	trt__GetAudioSourceConfigurationOptionsResponse->Options = (struct tt__AudioSourceConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfigurationOptions)); 
	trt__GetAudioSourceConfigurationOptionsResponse->Options->__sizeInputTokensAvailable = oSysInfo->nprofile;
	trt__GetAudioSourceConfigurationOptionsResponse->Options->InputTokensAvailable = (char **)soap_malloc(soap, sizeof(char *) * trt__GetAudioSourceConfigurationOptionsResponse->Options->__sizeInputTokensAvailable);

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		trt__GetAudioSourceConfigurationOptionsResponse->Options->InputTokensAvailable[i] = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
		trt__GetAudioSourceConfigurationOptionsResponse->Options->InputTokensAvailable[i] = oSysInfo->Oprofile[i].AASC.Asourcetoken;
	}
	trt__GetAudioSourceConfigurationOptionsResponse->Options->Extension = NULL;
	dom_att(&trt__GetAudioSourceConfigurationOptionsResponse->Options->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK; 
}

/** 
 * @Get Audio Encoder COnfiguration Options.
 * @This operation returns the available options when the audio encoder parameters are reconfigured.
 *   
 * @param Request	:_trt__GetAudioEncoderConfigurationOptions structure contains optional tokens of an audio encoder configuration and a media profile.
 * @param Response	:_trt__GetAudioEncoderConfigurationOptionsResponse structure contains This message contains the audio configuration options.
 * @return               : On success - SOAP_OK.
 On failure - NVT does not support audio encoder.

 */
int __trt__GetAudioEncoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioEncoderConfigurationOptions *trt__GetAudioEncoderConfigurationOptions, struct _trt__GetAudioEncoderConfigurationOptionsResponse *trt__GetAudioEncoderConfigurationOptionsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int token_exist = 0, profile_exist = 0;
	int i = 0;
	int index = 0;
	char *_Token = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	if(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken != NULL)
	{
		strcpy(_Token, trt__GetAudioEncoderConfigurationOptions->ConfigurationToken);
	}

	if(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken, oSysInfo->Oprofile[i].AAEC.AEtoken) == 0)
			{
				token_exist = EXIST;
				index = i;
				break;
			}
		}
		if(!token_exist)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoConfig","Configuration token does not exist");
			return SOAP_FAULT;
		}

	}
	else if(trt__GetAudioEncoderConfigurationOptions->ProfileToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
		{
			if(strcmp(trt__GetAudioEncoderConfigurationOptions->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
			{
				profile_exist = EXIST;
				index = i;
				break;
			}
		}
		if(!profile_exist)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
			return SOAP_FAULT;
		}
	}

	trt__GetAudioEncoderConfigurationOptionsResponse->Options = (struct tt__AudioEncoderConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfigurationOptions));

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->__sizeOptions = 1;
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options = (struct tt__AudioEncoderConfigurationOption *)soap_malloc(soap, trt__GetAudioEncoderConfigurationOptionsResponse->Options->__sizeOptions * sizeof(struct tt__AudioEncoderConfigurationOption));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].Encoding = oSysInfo->Oprofile[index].AAEC.AEencoding; //G711
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList = (struct tt__IntList *)soap_malloc(soap, sizeof(struct tt__IntList));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->__sizeItems = 2;
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->Items = (int *)soap_malloc(soap, (trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->__sizeItems) * sizeof(int));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->Items[0] = 64;//  (64kbps)
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->Items[1] = 128;//  (128kbps)
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList = (struct tt__IntList *)soap_malloc(soap, sizeof(struct tt__IntList));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->__sizeItems = 2;
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->Items = (int *)soap_malloc(soap, (trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->__sizeItems) * sizeof(int));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->Items[0] = 8;// (8KHz)
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->Items[1] = 16;//  (16KHz)
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].__size = 0;
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].__any = NULL;
	dom_att(&trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	dom_att(&trt__GetAudioEncoderConfigurationOptionsResponse->Options->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK; 
}


/** 
 * @Get Metadata Configuration Options.
 * @This operation returns the available options for changing the metadata configuration.
 *   
 * @param Request        :_trt__GetMetadataConfigurationOptions structure contains optional tokens of a metadata configuration and a media profile.
 * @param Response       :_trt__GetMetadataConfigurationOptionsResponse structure contains the metadata configuration options..
 * @return               : On success - SOAP_OK.
 On failure - The requested profile token does not exist.
 The requested configuration does not exist.
 */
int __trt__GetMetadataConfigurationOptions(struct soap* soap, struct _trt__GetMetadataConfigurationOptions *trt__GetMetadataConfigurationOptions, struct _trt__GetMetadataConfigurationOptionsResponse *trt__GetMetadataConfigurationOptionsResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int i, j;
	int num_token = 0;
	int flag = NOT_EXIST;
	int flg = 0;
	int index = 0;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		flg = 0;
		for(j = 0; j <= i; j++)
		{
			if(strcmp(oSysInfo->Oprofile[j].MDC.MDtoken, oSysInfo->Oprofile[i].MDC.MDtoken)==0);
			{
				flg = 1;		
				break;
			}
		}
		if(flg == 0)
		{
			num_token++;
		}
	}

	if(trt__GetMetadataConfigurationOptions->ProfileToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile ; i++)
		{
			if(strcmp(trt__GetMetadataConfigurationOptions->ProfileToken, oSysInfo->Oprofile[i].profiletoken)==0);
			{
				flag = EXIST;
				//index = j;
				break;
			}
		}
		if(!flag)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
			return SOAP_FAULT;
		}
	}

	flag = NOT_EXIST;
	if(trt__GetMetadataConfigurationOptions->ConfigurationToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile ; i++)
		{
			if(strcmp(trt__GetMetadataConfigurationOptions->ConfigurationToken, oSysInfo->Oprofile[i].MDC.MDtoken)==0);
			{
				flag = EXIST;
				//index = j;
				break;
			}
		}
		if(!flag)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
			return SOAP_FAULT;
		}
	}

	trt__GetMetadataConfigurationOptionsResponse->Options = (struct tt__MetadataConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__MetadataConfigurationOptions));

	//trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions = NULL;

	trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions = (struct tt__PTZStatusFilterOptions *)soap_malloc(soap, sizeof(struct tt__PTZStatusFilterOptions));
	trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->PanTiltStatusSupported = 0;
	trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->ZoomStatusSupported = 0;

	trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->PanTiltPositionSupported = (int *)soap_malloc(soap, sizeof(int));
	*trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->PanTiltPositionSupported = 0;
	trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->ZoomPositionSupported = (int *)soap_malloc(soap, sizeof(int));
	*trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->ZoomPositionSupported = 0;
	//trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->Extension = (struct tt__PTZStatusFilterOptionsExtension *)soap_malloc(soap,sizeof(struct tt__PTZStatusFilterOptionsExtension));
	trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->Extension = NULL;
	trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->__any = NULL;
	dom_att(&trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	trt__GetMetadataConfigurationOptionsResponse->Options->__size = 0;
	trt__GetMetadataConfigurationOptionsResponse->Options->__any = NULL;
	dom_att(&trt__GetMetadataConfigurationOptionsResponse->Options->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}



int __trt__GetAudioOutputConfigurationOptions(struct soap* soap, struct _trt__GetAudioOutputConfigurationOptions *trt__GetAudioOutputConfigurationOptions, struct _trt__GetAudioOutputConfigurationOptionsResponse *trt__GetAudioOutputConfigurationOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo *oSysInfo = GetSysInfo();
	int i = 0;

	if(trt__GetAudioOutputConfigurationOptions->ProfileToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
			if(strcmp(trt__GetAudioOutputConfigurationOptions->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
				break;
		if(i >= oSysInfo->nprofile)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:NoProfile","Profile token does not exist");
			return SOAP_FAULT;
		}
	}
	if(trt__GetAudioOutputConfigurationOptions->ConfigurationToken != NULL)
	{
		for(i = 0; i < oSysInfo->nprofile; i++)
			if(strcmp(trt__GetAudioOutputConfigurationOptions->ConfigurationToken, oSysInfo->Oprofile[i].AAOC.AOtoken) == 0)
				break;
		if(i >= oSysInfo->nprofile)
		{
			onvif_fault(soap,1,"ter:InvalidArgVal","ter:NoConfig","Configuration token does not exist");
			return SOAP_FAULT;
		}
	}

	trt__GetAudioOutputConfigurationOptionsResponse = (struct _trt__GetAudioOutputConfigurationOptionsResponse*)soap_malloc(soap, sizeof(struct _trt__GetAudioOutputConfigurationOptionsResponse));
	trt__GetAudioOutputConfigurationOptionsResponse->Options = (struct tt__AudioOutputConfigurationOptions*)soap_malloc(soap, sizeof(struct tt__AudioOutputConfigurationOptions));
	trt__GetAudioOutputConfigurationOptionsResponse->Options->__sizeOutputTokensAvailable = 1;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputTokensAvailable = (char **)soap_malloc(soap, trt__GetAudioOutputConfigurationOptionsResponse->Options->__sizeOutputTokensAvailable * SMALL_INFO_LENGTH);
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputTokensAvailable[0] = oSysInfo->Oprofile[i].AAOC.AOtoken;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->__sizeSendPrimacyOptions = 0;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->SendPrimacyOptions = NULL;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputLevelRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputLevelRange->Min = 0;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputLevelRange->Max = 100;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->__size = 0;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->__any = NULL;
	dom_att(&trt__GetAudioOutputConfigurationOptionsResponse->Options->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;
}


int __trt__GetAudioDecoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioDecoderConfigurationOptions *trt__GetAudioDecoderConfigurationOptions, struct _trt__GetAudioDecoderConfigurationOptionsResponse *trt__GetAudioDecoderConfigurationOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );

	return SOAP_FAULT;
}


int __trt__GetGuaranteedNumberOfVideoEncoderInstances(struct soap* soap, struct _trt__GetGuaranteedNumberOfVideoEncoderInstances *trt__GetGuaranteedNumberOfVideoEncoderInstances, struct _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse *trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse->TotalNumber = 1;
	return SOAP_OK; 
}

/** 
 * @brief Get Stream Uri
 * @This operation requests a URI that can be used to initiate a live media stream using RTSP as the control protocol.
 *   
 * @param Request	:_trt__GetStreamUri structure variable members contains two parts. StreamType defines if a unicast or multicast media stream is requested. 
 * @param Response	:_trt__GetStreamUriResponse structure variables members contains the stable Uri to be used for requesting the media stream as well as parameters defining                           the lifetime of the Uri.
 * @return               : On success - SOAP_OK.
 On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.1)
 */
int __trt__GetStreamUri(struct soap* soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	/* Read Settings */	
	SysInfo* oSysInfo = GetSysInfo();
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	int i = 0;
	char _IPAddr[INFO_LENGTH];
	int multicast = 0;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetStreamUri->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			break;
		}
	}
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}

	if(trt__GetStreamUri->StreamSetup != NULL)
	{
	  /* Currently when multicast is enabled, it is streaiming over same URI as unicast, 
			   if changes are made in multicast streaming and if streaming happen on differnt URI with multicast address & port, 
			   changes have to made here to send proper URI */

		
		#if MULTICAST_ENABLE
		/* Check if RTP_Multicast is enabled */
		if(trt__GetStreamUri->StreamSetup->Stream == 1)       
		{
			/* Enable Multicast */
			if(oSysInfo->lan_config.net.multicast_enable != 1)
			{
				multicast = 1;
				ControlSystemData(SFIELD_SET_MULTICAST, (unsigned char *)&multicast, sizeof(unsigned char));
			}
		
		}
		else
		{ 
			if(oSysInfo->lan_config.net.multicast_enable != 0)
			{
				multicast = 0;
				ControlSystemData(SFIELD_SET_MULTICAST, (unsigned char *)&multicast, sizeof(unsigned char));
			}
		}
		#endif
	}

	if(oSysInfo->Oprofile[i].AESC.Vencoder == 0)
	{
		sprintf(_IPAddr, "rtsp://%d.%d.%d.%d:%d/PSIA/Streaming/channels/0?videoCodecType=MJPEG", ip.str[0], ip.str[1], ip.str[2], ip.str[3], oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[4]);
	}
	else if(oSysInfo->Oprofile[i].AESC.Vencoder == 1)
	{
		if(oSysInfo->Oprofile[i].AESC.Rwidth == 720 && oSysInfo->Oprofile[i].AESC.Rheight == 480)
		{
			sprintf(_IPAddr, "rtsp://%d.%d.%d.%d:%d/PSIA/Streaming/channels/1?videoCodecType=MPEG4", ip.str[0], ip.str[1], ip.str[2], ip.str[3],oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[2]);
		}
		else
		{
			sprintf(_IPAddr, "rtsp://%d.%d.%d.%d:%d/PSIA/Streaming/channels/1?videoCodecType=MPEG4", ip.str[0], ip.str[1], ip.str[2], ip.str[3],oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[3]);
		}

	}
	else if(oSysInfo->Oprofile[i].AESC.Vencoder == 2)
	{
		if(oSysInfo->Oprofile[i].AESC.Rwidth == 720 && oSysInfo->Oprofile[i].AESC.Rheight == 480)
		{
			sprintf(_IPAddr, "rtsp://%d.%d.%d.%d:%d/PSIA/Streaming/channels/2?videoCodecType=H.264", ip.str[0], ip.str[1], ip.str[2], ip.str[3],oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[1]);
		}
		else
		{
			sprintf(_IPAddr, "rtsp://%d.%d.%d.%d:%d/PSIA/Streaming/channels/2?videoCodecType=H.264", ip.str[0], ip.str[1], ip.str[2], ip.str[3],oSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[0]);
		}
	}

	/* Response */
	trt__GetStreamUriResponse->MediaUri = (struct tt__MediaUri *)soap_malloc(soap, sizeof(struct tt__MediaUri));
	trt__GetStreamUriResponse->MediaUri->Uri = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	strcpy(trt__GetStreamUriResponse->MediaUri->Uri, _IPAddr);
	trt__GetStreamUriResponse->MediaUri->InvalidAfterConnect = 0;
	trt__GetStreamUriResponse->MediaUri->InvalidAfterReboot = 0;
	trt__GetStreamUriResponse->MediaUri->Timeout = 200;
	trt__GetStreamUriResponse->MediaUri->__size = 0;
	trt__GetStreamUriResponse->MediaUri->__any = NULL;
	dom_att(&trt__GetStreamUriResponse->MediaUri->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	return SOAP_OK; 
}

int __trt__StartMulticastStreaming(struct soap* soap, struct _trt__StartMulticastStreaming *trt__StartMulticastStreaming, struct _trt__StartMulticastStreamingResponse *trt__StartMulticastStreamingResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	int i = 0;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__StartMulticastStreaming->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			break;
		}
	}
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}

	return SOAP_OK;

}


int __trt__StopMulticastStreaming(struct soap* soap, struct _trt__StopMulticastStreaming *trt__StopMulticastStreaming, struct _trt__StopMulticastStreamingResponse *trt__StopMulticastStreamingResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	int i = 0;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__StopMulticastStreaming->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			break;
		}
	}
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}

	return SOAP_OK;

}

int __trt__SetSynchronizationPoint(struct soap* soap, struct _trt__SetSynchronizationPoint *trt__SetSynchronizationPoint, struct _trt__SetSynchronizationPointResponse *trt__SetSynchronizationPointResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	return SOAP_OK; 
}


/** 
 * @brief Get Snapshot Uri.
 * @A Network client uses the GetSnapshotUri command to obtain a JPEG snhapshot from the NVT.
 *   
 * @param Request       :_trt__GetSnapshotUri structure contains ProfileToken element indicates the media profile to use and will define the source and dimensions of the snapshot.
 * @param Response      :_trt__GetSnapshotUriResponse structure contains a stable Uri to be used for  acquiring a snapshot in JPEG format as well as parameters defining the 
 lifetime of the Uri.
 * @return              : On success - SOAP_OK.
 : On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 11.16.1)

 */
int __trt__GetSnapshotUri(struct soap* soap, struct _trt__GetSnapshotUri *trt__GetSnapshotUri, struct _trt__GetSnapshotUriResponse *trt__GetSnapshotUriResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif
	SysInfo* oSysInfo = GetSysInfo();
	int i;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(trt__GetSnapshotUri->ProfileToken, oSysInfo->Oprofile[i].profiletoken) == 0)
		{
			break;
		}
	}
	if(i >= oSysInfo->nprofile)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoProfile","Profile token does not exist");
		return SOAP_FAULT;
	}
	char _IPAddr[INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);

	if(ControlSystemData(SFIELD_SET_CLICK_SNAP_FILENAME, oSysInfo->lan_config.nClickSnapFilename,
				strlen(oSysInfo->lan_config.nClickSnapFilename)) < 0)
	{
	}

	sprintf(_IPAddr, "http://%d.%d.%d.%d/sdget.htm?FILE=%s.jpg", ip.str[0], ip.str[1], ip.str[2], ip.str[3], oSysInfo->lan_config.nClickSnapFilename);

	trt__GetSnapshotUriResponse->MediaUri = (struct tt__MediaUri *)soap_malloc(soap, sizeof(struct tt__MediaUri)); 
	trt__GetSnapshotUriResponse->MediaUri->Uri = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);	
	strcpy(trt__GetSnapshotUriResponse->MediaUri->Uri, _IPAddr);
	trt__GetSnapshotUriResponse->MediaUri->InvalidAfterConnect = 0;
	trt__GetSnapshotUriResponse->MediaUri->InvalidAfterReboot = 0;
	trt__GetSnapshotUriResponse->MediaUri->Timeout = 720000;
	trt__GetSnapshotUriResponse->MediaUri->__size = 0;
	trt__GetSnapshotUriResponse->MediaUri->__any = NULL;
	dom_att(&trt__GetSnapshotUriResponse->MediaUri->__anyAttribute, 0, NULL, NULL, NULL,NULL, soap);

	return SOAP_OK;
}


int __timg__GetServiceCapabilities(struct soap* soap, struct _timg__GetServiceCapabilities *timg__GetServiceCapabilities, struct _timg__GetServiceCapabilitiesResponse *timg__GetServiceCapabilitiesResponse) {

#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	timg__GetServiceCapabilitiesResponse->Capabilities = (struct timg__Capabilities  *)soap_malloc(soap, sizeof(struct timg__Capabilities));
	timg__GetServiceCapabilitiesResponse->Capabilities->__size = 0;
	timg__GetServiceCapabilitiesResponse->Capabilities->__any = NULL;
	timg__GetServiceCapabilitiesResponse->Capabilities->ImageStabilization =soap_malloc(soap, sizeof(enum xsd__boolean));
	*timg__GetServiceCapabilitiesResponse->Capabilities->ImageStabilization = xsd__boolean__true_;
	dom_att(&timg__GetServiceCapabilitiesResponse->Capabilities->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	return SOAP_OK;


}

/** 
 * @brief This operation requests the imaging setting for a video source on the device.
 *   
 * @param Request	: _timg__GetImagingSettings structure variable members contains a 
 reference to the VideoSource for which the ImagingSettings should be requested.
 * @param Response	: _timg__GetImagingSettingsResponse structure variables members contains the 
 ImagingSettings for the VideoSource thatwas requested.
 * @return returns SOAP_OK on success.
 */

int __timg__GetImagingSettings(struct soap* soap, struct _timg__GetImagingSettings *timg__GetImagingSettings, struct _timg__GetImagingSettingsResponse *timg__GetImagingSettingsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo* oSysInfo = GetSysInfo();
	int i = 0,flag = 0;
	int token_exist = 0;
	int imaging_exist = 0;
	float _Brightness;
	float _Saturation; 
	float _Contrast; 
	float _Sharpness;
	float _WideDynamicRange; 
	int _Backlight; 
	float _BacklightCompensationLevel; 
	int _WhiteBalance;
	int index = 0;

	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(timg__GetImagingSettings->VideoSourceToken, oSysInfo->Oprofile[i].AVSC.Vsourcetoken) == 0)
		{
			token_exist = EXIST;
			index = i;
			break;
		}
	}
	if(!token_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoSource","The requested VideoSource does not exist");
		return SOAP_FAULT;
	}
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(timg__GetImagingSettings->VideoSourceToken, oSysInfo->imaging_conf[i].Isourcetoken) == 0)
		{
			imaging_exist = EXIST;
			break;
		}
	}
	i = index;
	if(!imaging_exist)
	{
		onvif_fault(soap,1, "ter:InvalidArgVal", "ter:NoSource","The requested VideoSource does not exist");
		return SOAP_FAULT;
	}
	_Brightness = (float)(oSysInfo->imaging_conf[i].Brightness ); 
	_Saturation = (float)(oSysInfo->imaging_conf[i].Saturation ); 
	_Contrast = (float)(oSysInfo->imaging_conf[i].Contrast ); 
	_Sharpness = (float)(oSysInfo->imaging_conf[i].Sharpness );
	_WideDynamicRange = oSysInfo->imaging_conf[i].WideDynamicMode;
	_Backlight = oSysInfo->imaging_conf[i].BacklightCompensationMode; 
	_BacklightCompensationLevel = oSysInfo->imaging_conf[i].BackLightLevel;
	_WhiteBalance = oSysInfo->imaging_conf[i].WhiteBalanceMode;

	timg__GetImagingSettingsResponse->ImagingSettings = (struct tt__ImagingSettings20 *)soap_malloc(soap, sizeof(struct tt__ImagingSettings20));
	timg__GetImagingSettingsResponse->ImagingSettings->Brightness = (float *)soap_malloc(soap, sizeof(float));
	*timg__GetImagingSettingsResponse->ImagingSettings->Brightness = _Brightness;
	timg__GetImagingSettingsResponse->ImagingSettings->ColorSaturation = (float *)soap_malloc(soap, sizeof(float));
	*timg__GetImagingSettingsResponse->ImagingSettings->ColorSaturation = _Saturation;
	timg__GetImagingSettingsResponse->ImagingSettings->Contrast = (float *)soap_malloc(soap, sizeof(float));
	*timg__GetImagingSettingsResponse->ImagingSettings->Contrast = _Contrast;
	timg__GetImagingSettingsResponse->ImagingSettings->IrCutFilter = NULL;
	timg__GetImagingSettingsResponse->ImagingSettings->Sharpness = (float *)soap_malloc(soap, sizeof(float));
	*timg__GetImagingSettingsResponse->ImagingSettings->Sharpness = _Sharpness;
	timg__GetImagingSettingsResponse->ImagingSettings->BacklightCompensation = (struct tt__BacklightCompensation20 *)soap_malloc(soap, sizeof(struct tt__BacklightCompensation20));
	timg__GetImagingSettingsResponse->ImagingSettings->BacklightCompensation->Mode = _Backlight;
	//Streaming_onvif->BacklightCompensationMode;  //{onv__BacklightCompensationMode__OFF = 0, onv__BacklightCompensationMode__ON = 1}
	timg__GetImagingSettingsResponse->ImagingSettings->BacklightCompensation->Level =(float *) soap_malloc(soap, sizeof(float));//float
	*timg__GetImagingSettingsResponse->ImagingSettings->BacklightCompensation->Level =_BacklightCompensationLevel;//float
	timg__GetImagingSettingsResponse->ImagingSettings->Exposure = NULL;
	timg__GetImagingSettingsResponse->ImagingSettings->Focus = NULL;
	timg__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange = (struct tt__WideDynamicRange20 *)soap_malloc(soap, sizeof(struct tt__WideDynamicRange20));
	timg__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange->Mode = _WideDynamicRange;   
	//{onv__WideDynamicMode__OFF = 0, onv__WideDynamicMode__ON = 1}
	timg__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange->Level = NULL;
	timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance = (struct tt__WhiteBalance20 *)soap_malloc(soap, sizeof(struct tt__WhiteBalance20));
	timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->Mode = _WhiteBalance;
	//{onv__WhiteBalanceMode__AUTO = 0, onv__WhiteBalanceMode__MANUAL = 1}
	timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->Extension = NULL;
	timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->CrGain = (float *) soap_malloc(soap, sizeof(float));
	*timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->CrGain = oSysInfo->imaging_conf[i].CrGain;
	timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->CbGain = (float *) soap_malloc(soap, sizeof(float));
	*timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->CbGain = oSysInfo->imaging_conf[i].CbGain;	
	dom_att(&timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	timg__GetImagingSettingsResponse->ImagingSettings->Extension = NULL;
	dom_att(&timg__GetImagingSettingsResponse->ImagingSettings->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);

	return SOAP_OK;

}

/**
 * @brief Set Imaging Settings 
 * @This operation set the following imaging setting for a video source on the device:
 -Brightness
 -ColorSaturation
 -Contrast
 -Sharpness
 *   
 * @param Request	: _timg__SetImagingSettings structure contains a reference to the VideoSource and ImagingSettings that should be set. 
 * @param Response	: _timg__SetImagingSettingsResponse structure contains an empity message

 * @return               : On success - SOAP_OK.
 On failure - an appropriate fault code(ONVIF Core Spec. - Ver. 2.00 10.1.2)
 */
int __timg__SetImagingSettings(struct soap* soap, struct _timg__SetImagingSettings *timg__SetImagingSettings, struct _timg__SetImagingSettingsResponse *timg__SetImagingSettingsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	Imaging_Conf_tmp  imaging_conf_tmp;
	memset(&imaging_conf_tmp,0,sizeof(imaging_conf_tmp));
	int i,token_exist = 0;
	int imaging_exist;
	__u8 *_brightness;
	__u8 *_saturation;
	__u8 *_contrast;
	__u8 *_sharpness;
	__u8 *_pBacklightCompensationMode;
	float *_pBacklightCompensationModeLevel;
	float _br;
	int _sa;
	int _co;
	int _sh;
	int	_blc_mode;
	int	_blc_level;
	for(i = 0; i< oSysInfo->nprofile ;i++)
	{
		if(strcmp(timg__SetImagingSettings->VideoSourceToken, oSysInfo->Oprofile[i].AVSC.Vsourcetoken) == 0)
		{
			token_exist = EXIST;
			imaging_conf_tmp.position = i;
			break;
		}
	}
	if(!token_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoSource","The requested VideoSource does not exist");
		return SOAP_FAULT;
	}
	for(i = 0; i< oSysInfo->nprofile; i++)
	{
		if(strcmp(timg__SetImagingSettings->VideoSourceToken, oSysInfo->imaging_conf[i].Isourcetoken) == 0)
		{
			imaging_exist = EXIST;
			break;
		}
	}
	if(!imaging_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoSource","The requested VideoSource does not exist");
		return SOAP_FAULT;
	}
	strcpy(imaging_conf_tmp.imaging_conf_in.Isourcetoken_t, timg__SetImagingSettings->VideoSourceToken);
	_brightness = (unsigned char *)malloc(sizeof(unsigned char));
	_saturation = (unsigned char *)malloc(sizeof(unsigned char));
	_contrast = (unsigned char *)malloc(sizeof(unsigned char));
	_sharpness = (unsigned char *)malloc(sizeof(unsigned char));
	_pBacklightCompensationMode = (unsigned char *)malloc(sizeof(unsigned char));
	_pBacklightCompensationModeLevel = (float *)malloc(sizeof(float));
	if(timg__SetImagingSettings->ImagingSettings->Brightness != NULL)
	{
		*_brightness = (unsigned char)*timg__SetImagingSettings->ImagingSettings->Brightness;	
		_br = *timg__SetImagingSettings->ImagingSettings->Brightness;
		imaging_conf_tmp.imaging_conf_in.Brightness_t = _br;
	}
	if(timg__SetImagingSettings->ImagingSettings->ColorSaturation != NULL)
	{
		*_saturation = (unsigned char)*timg__SetImagingSettings->ImagingSettings->ColorSaturation;
		_sa = *timg__SetImagingSettings->ImagingSettings->ColorSaturation;
		imaging_conf_tmp.imaging_conf_in.Saturation_t = *_saturation;
	}
	if(timg__SetImagingSettings->ImagingSettings->Contrast != NULL)
	{
		*_contrast = (unsigned char)*timg__SetImagingSettings->ImagingSettings->Contrast;
		_co = *timg__SetImagingSettings->ImagingSettings->Contrast;
		imaging_conf_tmp.imaging_conf_in.Contrast_t = _co;

	}
	if(timg__SetImagingSettings->ImagingSettings->Sharpness != NULL)
	{
		*_sharpness = (unsigned char)*timg__SetImagingSettings->ImagingSettings->Sharpness;
		_sh = *timg__SetImagingSettings->ImagingSettings->Sharpness;
		imaging_conf_tmp.imaging_conf_in.Sharpness_t = _sh;
	}
	if(timg__SetImagingSettings->ImagingSettings->BacklightCompensation != NULL)
	{
		*_pBacklightCompensationMode =  timg__SetImagingSettings->ImagingSettings->BacklightCompensation->Mode;
		_pBacklightCompensationModeLevel = timg__SetImagingSettings->ImagingSettings->BacklightCompensation->Level;


		_blc_mode = imaging_conf_tmp.imaging_conf_in.BacklightCompensationMode_t 
			= timg__SetImagingSettings->ImagingSettings->BacklightCompensation->Mode;

		if(timg__SetImagingSettings->ImagingSettings->BacklightCompensation->Level != NULL)
		{
			_blc_level = imaging_conf_tmp.imaging_conf_in.BackLightLevel_t 
				= *timg__SetImagingSettings->ImagingSettings->BacklightCompensation->Level;
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->Exposure != NULL)
	{
		imaging_conf_tmp.imaging_conf_in.ExposureMode_t = timg__SetImagingSettings->ImagingSettings->Exposure->Mode;
		if(timg__SetImagingSettings->ImagingSettings->Exposure->Priority != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.ExposurePriority_t = *timg__SetImagingSettings->ImagingSettings->Exposure->Priority;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->Window != NULL)
		{
			if(timg__SetImagingSettings->ImagingSettings->Exposure->Window->bottom != NULL)
			{
				imaging_conf_tmp.imaging_conf_in.bottom_t = *timg__SetImagingSettings->ImagingSettings->Exposure->Window->bottom;
			}
			if(timg__SetImagingSettings->ImagingSettings->Exposure->Window->top != NULL)
			{
				imaging_conf_tmp.imaging_conf_in.top_t    = *timg__SetImagingSettings->ImagingSettings->Exposure->Window->top;
			}
			if(timg__SetImagingSettings->ImagingSettings->Exposure->Window->right != NULL)
			{
				imaging_conf_tmp.imaging_conf_in.right_t  = *timg__SetImagingSettings->ImagingSettings->Exposure->Window->right;
			}
			if(timg__SetImagingSettings->ImagingSettings->Exposure->Window->left != NULL)
			{
				imaging_conf_tmp.imaging_conf_in.left_t  = *timg__SetImagingSettings->ImagingSettings->Exposure->Window->left;
			}
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->MinExposureTime != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.MinExposureTime_t = *timg__SetImagingSettings->ImagingSettings->Exposure->MinExposureTime;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->MaxExposureTime != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.MaxExposureTime_t = *timg__SetImagingSettings->ImagingSettings->Exposure->MaxExposureTime;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->MinGain != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.MinGain_t         = *timg__SetImagingSettings->ImagingSettings->Exposure->MinGain;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->MaxGain != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.MaxGain_t         = *timg__SetImagingSettings->ImagingSettings->Exposure->MaxGain;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->MinIris != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.MinIris_t         = *timg__SetImagingSettings->ImagingSettings->Exposure->MinIris;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->MaxIris != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.MaxIris_t         = *timg__SetImagingSettings->ImagingSettings->Exposure->MaxIris;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->ExposureTime != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.ExposureTime_t    = *timg__SetImagingSettings->ImagingSettings->Exposure->ExposureTime;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->Gain != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.Gain_t            = *timg__SetImagingSettings->ImagingSettings->Exposure->Gain;
		}
		if(timg__SetImagingSettings->ImagingSettings->Exposure->Iris != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.Iris_t            = *timg__SetImagingSettings->ImagingSettings->Exposure->Iris;
		}

	}
	if(timg__SetImagingSettings->ImagingSettings->Focus != NULL)
	{
		imaging_conf_tmp.imaging_conf_in.AutoFocusMode_t = timg__SetImagingSettings->ImagingSettings->Focus->AutoFocusMode;
		if(timg__SetImagingSettings->ImagingSettings->Focus->DefaultSpeed != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.DefaultSpeed_t = *timg__SetImagingSettings->ImagingSettings->Focus->DefaultSpeed;
		}
		if(timg__SetImagingSettings->ImagingSettings->Focus->NearLimit != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.NearLimit_t    = *timg__SetImagingSettings->ImagingSettings->Focus->NearLimit;
		}
		if(timg__SetImagingSettings->ImagingSettings->Focus->FarLimit != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.FarLimit_t     = *timg__SetImagingSettings->ImagingSettings->Focus->FarLimit;
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->IrCutFilter != NULL)
	{
		imaging_conf_tmp.imaging_conf_in.IrCutFilterMode_t = *timg__SetImagingSettings->ImagingSettings->IrCutFilter;
	}
	if(timg__SetImagingSettings->ImagingSettings->WideDynamicRange != 0)
	{
		imaging_conf_tmp.imaging_conf_in.WideDynamicMode_t  = timg__SetImagingSettings->ImagingSettings->WideDynamicRange->Mode;
		if(timg__SetImagingSettings->ImagingSettings->WideDynamicRange->Level != NULL)
		{
			if(timg__SetImagingSettings->ImagingSettings->WideDynamicRange->Level != NULL)
			{
				imaging_conf_tmp.imaging_conf_in.WideDMLevel_t = *timg__SetImagingSettings->ImagingSettings->WideDynamicRange->Level;
			}
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->WhiteBalance != 0)
	{
		imaging_conf_tmp.imaging_conf_in.WhiteBalanceMode_t = timg__SetImagingSettings->ImagingSettings->WhiteBalance->Mode;
		if(timg__SetImagingSettings->ImagingSettings->WhiteBalance->CrGain != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.CrGain_t = *timg__SetImagingSettings->ImagingSettings->WhiteBalance->CrGain;
		}
		if(timg__SetImagingSettings->ImagingSettings->WhiteBalance->CbGain != NULL)
		{
			imaging_conf_tmp.imaging_conf_in.CbGain_t = *timg__SetImagingSettings->ImagingSettings->WhiteBalance->CbGain;
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->Brightness != NULL)
	{
		if(_br < 256 && _br > 0)
		{
			ControlSystemData(SFIELD_SET_BRIGHTNESS, (void *)_brightness, sizeof(*_brightness));
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingsInvalid","The requested settings are incorrect");
			free(_brightness);
			free(_saturation);
			free(_contrast);
			free(_sharpness);
			free(_pBacklightCompensationMode);
			free(_pBacklightCompensationModeLevel);
			return SOAP_FAULT;
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->ColorSaturation != NULL)
	{
		if(_sa < 256 && _sa > 0)
		{
			ControlSystemData(SFIELD_SET_SATURATION, (void *)_saturation, sizeof(*_saturation));
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingsInvalid","The requested settings are incorrect");
			free(_brightness);
			free(_saturation);
			free(_contrast);
			free(_sharpness);
			free(_pBacklightCompensationMode);
			free(_pBacklightCompensationModeLevel);
			return SOAP_FAULT;
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->Contrast != NULL)
	{
		if(_co < 256 && _co > 0)
		{
			ControlSystemData(SFIELD_SET_CONTRAST, (void *)_contrast, sizeof(*_contrast));
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingsInvalid","The requested settings are incorrect");
			free(_brightness);
			free(_saturation);
			free(_contrast);
			free(_sharpness);
			free(_pBacklightCompensationMode);
			free(_pBacklightCompensationModeLevel);
			return SOAP_FAULT;
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->Sharpness != NULL)
	{
		if(_sh < 256 && _sh > 0)
		{
			ControlSystemData(SFIELD_SET_SHARPNESS, (void *)_sharpness, sizeof(*_sharpness));
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingsInvalid","The requested settings are incorrect");
			free(_brightness);
			free(_saturation);
			free(_contrast);
			free(_sharpness);
			free(_pBacklightCompensationMode);
			free(_pBacklightCompensationModeLevel);
			return SOAP_FAULT;
		}
	}
	{
		if(_blc_mode < 2 && _blc_mode >= 0)
		{
			ControlSystemData(SFIELD_SET_BLC, (void *)_pBacklightCompensationMode, sizeof(*_pBacklightCompensationMode));
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingsInvalid","The requested settings are incorrect");
			free(_brightness);
			free(_saturation);
			free(_contrast);
			free(_sharpness);
			free(_pBacklightCompensationMode);
			free(_pBacklightCompensationModeLevel);
			return SOAP_FAULT;
		}
	}
	if(timg__SetImagingSettings->ImagingSettings->BacklightCompensation->Level != NULL)
	{
		if(_blc_level < 3 && _blc_level >= 0)
		{
			ControlSystemData(SFIELD_SET_BACKLIGHT, (void *)_pBacklightCompensationModeLevel, sizeof(*_pBacklightCompensationModeLevel));
		}
		else
		{
			onvif_fault(soap,1,"ter:InvalidArgVal", "ter:SettingsInvalid","The requested settings are incorrect");
			free(_brightness);
			free(_saturation);
			free(_contrast);
			free(_sharpness);
			free(_pBacklightCompensationMode);
			free(_pBacklightCompensationModeLevel);
			return SOAP_FAULT;
		}
	}
	for(i = 0; i< oSysInfo->nprofile ;i++)
	{
		if(strcmp(timg__SetImagingSettings->VideoSourceToken, oSysInfo->Oprofile[i].AVSC.Vsourcetoken) == 0)
		{
			token_exist = EXIST;
			imaging_conf_tmp.position = i;
		}

		if(token_exist)
		{
			ControlSystemData(SFIELD_SET_IMAGING, (void *) &imaging_conf_tmp, sizeof(imaging_conf_tmp));
			break;
		}
	}
	free(_brightness);
	free(_saturation);
	free(_contrast);
	free(_sharpness);
	free(_pBacklightCompensationMode);
	free(_pBacklightCompensationModeLevel);
	return SOAP_OK;

}


int __timg__GetOptions(struct soap* soap, struct _timg__GetOptions *timg__GetOptions, struct _timg__GetOptionsResponse *timg__GetOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	SysInfo *oSysInfo = GetSysInfo();
	int i = 0, token_exist;
	int imaging_exist = 0;
	int index = 0;
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(timg__GetOptions->VideoSourceToken, oSysInfo->Oprofile[i].AVSC.Vsourcetoken) == 0)
		{
			token_exist = EXIST;
			index = i;
			break;
		}
	}
	if(!token_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoSource","The requested VideoSource does not exist");
		return SOAP_FAULT;
	}
	for(i = 0; i < oSysInfo->nprofile; i++)
	{
		if(strcmp(timg__GetOptions->VideoSourceToken, oSysInfo->imaging_conf[i].Isourcetoken) == 0)
		{
			imaging_exist = EXIST;
			break;
		}
	}
	if(!imaging_exist)
	{
		onvif_fault(soap,1,"ter:InvalidArgVal", "ter:NoSource","The requested VideoSource does not exist");
		return SOAP_FAULT;
	}
	timg__GetOptionsResponse->ImagingOptions =  (struct tt__ImagingOptions20 *)soap_malloc(soap, sizeof(struct tt__ImagingOptions20));
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation = (struct tt__BacklightCompensationOptions20 *)soap_malloc(soap, sizeof(struct tt__BacklightCompensationOptions20));
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->__sizeMode = 1;

	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Mode = (int *) soap_malloc(soap, sizeof(int));
	*timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Mode = _false;
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Level = NULL;
	timg__GetOptionsResponse->ImagingOptions->Brightness = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Contrast = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Brightness->Min = 1;
	timg__GetOptionsResponse->ImagingOptions->Brightness->Max = 255;
	timg__GetOptionsResponse->ImagingOptions->Contrast->Min = 1;
	timg__GetOptionsResponse->ImagingOptions->Contrast->Max = 255;
	timg__GetOptionsResponse->ImagingOptions->Sharpness = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Min = 1;
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Max = 225;
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation->Min = 1;
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation->Max = 255;
	timg__GetOptionsResponse->ImagingOptions->__sizeIrCutFilterModes = 0;
	timg__GetOptionsResponse->ImagingOptions->IrCutFilterModes = (int *)soap_malloc(soap, sizeof(int));
	*timg__GetOptionsResponse->ImagingOptions->IrCutFilterModes = 2;
	timg__GetOptionsResponse->ImagingOptions->Exposure = NULL;
	timg__GetOptionsResponse->ImagingOptions->Focus = NULL;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange = NULL;
	timg__GetOptionsResponse->ImagingOptions->WhiteBalance = NULL;
	timg__GetOptionsResponse->ImagingOptions->Extension = NULL;
	dom_att(&timg__GetOptionsResponse->ImagingOptions->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	return SOAP_OK;	
}


int __timg__Move(struct soap* soap, struct _timg__Move *timg__Move, struct _timg__MoveResponse *timg__MoveResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __timg__Stop(struct soap* soap, struct _timg__Stop *timg__Stop, struct _timg__StopResponse *timg__StopResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __timg__GetStatus(struct soap* soap, struct _timg__GetStatus *timg__GetStatus, struct _timg__GetStatusResponse *timg__GetStatusResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


int __timg__GetMoveOptions(struct soap* soap, struct _timg__GetMoveOptions *timg__GetMoveOptions, struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse) {
#if security_token_profile
	wsse_ret = (silvan_authentication(soap));
	switch(wsse_ret)
	{
		case 1: onvif_fault(soap,1,"ter:NoSecuritytoken","ter:Failed_wsse_Aunthentication","Security token not found");
			return SOAP_FAULT;
		case 2: onvif_fault_probe(soap,"ter:NotAuthorized","Username not found");
			return SOAP_FAULT;
		case 3: onvif_fault_probe(soap,"ter:NotAuthorized","Password Mismatch");
			return SOAP_FAULT;
		case 401: 
			return wsse_ret;
	}
#endif

	onvif_fault( soap,1,"ter:InvalidArgVal", "ter:ServiceNotSupported" ,"The service is not supported" );
	return SOAP_FAULT;
}


