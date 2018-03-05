#include <onvif.h>
#include <dapi.h>
#include <file_msg_drv.h>
#include <net_config.h>
/*
 * Name: udp_hello
 *
 * Description: This operation creates soap object, set SO_BROADCAST flag to enable udp broadcast,
	        initializing soap packet and set default values for header. This operation sends 
		Hello Messages four times to client by calling function soap_send___ns1__sendHello.
 */
int udp_hello(int *server_udp)
{
	unsigned char macaddr[MACH_ADDR_LENGTH];
	char _HwId[LARGE_INFO_LENGTH];
	time_t time_n;
	struct tm *tm_t;
	int itr = 0;
	struct soap *soaph;
	struct sembuf operations[1];
	int id = 0;
	int retval = 0;
	char *endpoint; // broadcast address
	char *MessageID; // uuid+time+mac address
	char *RelatesTo; 
	char *EndpointReference; //uuid+mac address
	char *Types; 
	char *Scopes;
	char *MatchBy;
	char *XAddrs;
	unsigned int MetadataVersion;
	char _IPAddr[INFO_LENGTH];
	char ip[IP_LENGTH];
	NET_IPV4 _ip;
	int i = 0;
	SysInfo* oSysInfo = GetSysInfo();

	operations[0].sem_num = 0;
	operations[0].sem_op = -1;
	operations[0].sem_flg = 0;

	/* HELLO */
	while(1)
	{
		/* sending hello NUM_BROADCAST times */	
		while(itr < NUM_BROADCAST)
		{
			soaph=soap_new();                                                    /*creating  soap object*/   
			soaph->connect_flags= SO_BROADCAST;                                  /*setting SO_BROADCAST flag to enable udp broadcast*/
			soap_init1(soaph,SOAP_IO_UDP | SO_BROADCAST);                        /*initializing soap packet*/
			soaph->send_timeout = 1;
			soaph->recv_timeout = 1;
			soaph->master = (int) server_udp;                                         
			soaph->socket = (int) server_udp;                                          
			soaph->errmode = 0;
			soaph->bind_flags = 1;
			soap_set_omode(soaph, SOAP_ENC_ZLIB);

			endpoint = (char*)soap_malloc(soaph, sizeof(char)*LARGE_INFO_LENGTH);
			strcpy(endpoint,"soap.udp://239.255.255.250:3702");

			Types = (char*)soap_malloc(soaph, sizeof(char)*LARGE_INFO_LENGTH);
			strcpy(Types, "dn:NetworkVideoTransmitter");

			_ip.int32 = net_get_ifaddr(ETH_NAME);
			sprintf(ip, "%03d.%03d.%03d.%03d",_ip.str[0], _ip.str[1], _ip.str[2], _ip.str[3]);
			sprintf(_IPAddr, "http://%s/onvif/services", ip);
			XAddrs = (char*)soap_malloc(soaph, sizeof(char)*INFO_LENGTH);
			strcpy(XAddrs,_IPAddr);// http://169.254.166.174/onvif/services";

			net_get_hwaddr(ETH_NAME, macaddr);
			sprintf(_HwId,"urn:uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
			EndpointReference = (char*)soap_malloc(soaph, sizeof(char)*LARGE_INFO_LENGTH);
			strcpy(EndpointReference,_HwId);

			Scopes = (char*)soap_malloc(soaph, sizeof(char)*LARGE_INFO_LENGTH);
			memset(Scopes,0,sizeof(Scopes));

			for(i = 0; i < 3; i++)
			{
				strcat(Scopes,oSysInfo->onvif_scopes.type[i]);
				strcat(Scopes," ");
			}
			strcat(Scopes, oSysInfo->onvif_scopes.name);
			strcat(Scopes, " ");
			strcat(Scopes, oSysInfo->onvif_scopes.hardware);
			strcat(Scopes, " ");

			for(i = 0; i < 3; i++)
			{
				strcat(Scopes, oSysInfo->onvif_scopes.location[i]);
				strcat(Scopes, " ");
			}
			for(i = 0; i < 3; i++)
			{
				if(oSysInfo->onvif_scopes.improper_input[i]!= NULL)
				{
					strcat(Scopes, oSysInfo->onvif_scopes.improper_input[i]);
					strcat(Scopes, " ");
				}
			}
			i = 0;

			time_n = time(NULL);
			tm_t=localtime(&time_n);
			sprintf(_HwId,"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",tm_t->tm_wday,tm_t->tm_mday,tm_t->tm_mon,tm_t->tm_year,tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec,macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
			MessageID = (char*)soap_malloc(soaph, sizeof(char)*LARGE_INFO_LENGTH);
			strcpy(MessageID, _HwId);

			MatchBy = NULL;
			MetadataVersion = 1;
			RelatesTo = NULL;

			/* defined in gsaop/plugin dapi.c [wsddapi.c] - to send hello message to endpoint address */

			soap_d_Hello(soaph, SOAP_D_ADHOC, endpoint, MessageID, RelatesTo, EndpointReference, Types, Scopes, MatchBy, XAddrs, MetadataVersion);
			soap_destroy(soaph);
			soap_end(soaph);
			soap_done(soaph);
			free(soaph);
			itr++;
			usleep(4000);

		}   

		udp_sync = 1;
		if(id <= 0)
		{
			id = semget( KEY, 1, 0666 | IPC_CREAT );
		}
		if(id > 0)
		{
			/* block until linkstatus changes */
			retval = semop(id, operations, 1);
			udp_sync = 0;
			if(retval == 0)
			{
				itr = 0;	
			}
			else
			{
				perror("REASON");
			}
		}
		usleep(1000);
	}
	/* End-HELLO */
}


/*
 * Name: Bye Handler (Its a signal handler for bye).
 *
 * Description: This function is creating soap object, setting SO_BROADCAST flag to enable udp broadcast 
 and initializing soap packet.
 */

void ByeHandler(int sig)
{
	char cmd[COMMAND_LENGTH];
	unsigned char macaddr[MACH_ADDR_LENGTH];
	char _HwId[LARGE_INFO_LENGTH];
	time_t time_n;
	struct tm *tm_t;
	struct soap *soapb;

	char *endpoint;
	char *MessageID;
	char *EndpointReference;
	char *Types;
	char *Scopes;
	char *MatchBy;
	char *XAddrs;
	unsigned int MetadataVersion;

	char _IPAddr[INFO_LENGTH];
	char ip[IP_LENGTH];
	NET_IPV4 _ip;

	signal(sig, SIG_IGN);
	soapb=soap_new();                                                    /* creating  soap object */   
	soapb->connect_flags= SO_BROADCAST;                                  /* setting SO_BROADCAST flag to enable udp broadcast */
	soap_init1(soapb,SOAP_IO_UDP | SO_BROADCAST);                        /* initializing soap packet */
	soapb->send_timeout = 1;
	soapb->recv_timeout = 1;
	soapb->master = server_udp;                                         
	soapb->socket = server_udp;                                          
	soapb->errmode = 0;
	soapb->bind_flags = 1;
	soap_set_omode(soapb, SOAP_ENC_ZLIB);


	endpoint = (char*)soap_malloc(soapb, sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(endpoint,"soap.udp://239.255.255.250:3702");

	Types = (char*)soap_malloc(soapb, sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(Types, "dn:NetworkVideoTransmitter");

	_ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(ip, "%03d.%03d.%03d.%03d",_ip.str[0], _ip.str[1], _ip.str[2], _ip.str[3]);
	sprintf(_IPAddr, "http://%s/onvif/services", ip);
	XAddrs = (char*)soap_malloc(soapb, sizeof(char)*INFO_LENGTH);
	strcpy(XAddrs,_IPAddr);// http://169.254.166.174/onvif/services";

	net_get_hwaddr(ETH_NAME, macaddr);
	sprintf(_HwId,"urn:uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	EndpointReference = (char*)soap_malloc(soapb, sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(EndpointReference,_HwId);

	Scopes = (char*)soap_malloc(soapb, sizeof(char)*LARGE_INFO_LENGTH);
	memset(Scopes,0,sizeof(Scopes));
	strcpy(Scopes,"onvif://www.onvif.org/type/video_encoder onvif://www.onvif.org/type/audio_encoder onvif://www.onvif.org/Hardware/DM365_IPNC onvif://www.onvif.org/name/Dhruv%20DM365_IPNC onvif://www.onvif.org/location/");

	time_n = time(NULL);
	tm_t=localtime(&time_n);
	sprintf(_HwId,"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",tm_t->tm_wday,tm_t->tm_mday,tm_t->tm_mon,tm_t->tm_year,tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec,macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	MessageID = (char*)soap_malloc(soapb, sizeof(char)*LARGE_INFO_LENGTH);
	strcpy(MessageID, _HwId);

	MatchBy = NULL;
	MetadataVersion = 1;

	/* defined in gsaop/plugin dapi.c [wsddapi.c] - to send bye message to endpoint address */
	soap_d_Bye(soapb, SOAP_D_ADHOC, endpoint, MessageID, EndpointReference, Types, Scopes, MatchBy, XAddrs, MetadataVersion);



	soap_destroy(soapb);
	soap_end(soapb);
	soap_done(soapb);
	free(soapb);
	sleep(1);
	sprintf(cmd, "shutdown -r now\n");
	system(cmd);
}


/* defined in dapi.c[wsddapi.c] - to pass probe request on to user-defined event handler to get matches */
soap_d_mode d_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct d__ProbeMatchesType *ns1__ProbeResponse)
{
	unsigned char macaddr[MACH_ADDR_LENGTH];
	char _IPAddr[INFO_LENGTH];
	char _HwId[LARGE_INFO_LENGTH];
	SysInfo *oSysInfo = GetSysInfo();
	int i = 0;
	char list[20][100];
	int j = 0;
	static int MATCH = 1;
	const char invalid_match_rule[INFO_LENGTH] = "InvalidMatchRule";

	
	soap->header->wsa__To = "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";
	soap->header->wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";

	soap->header->wsa__RelatesTo = (struct wsa__Relationship*)soap_malloc(soap, sizeof(struct wsa__Relationship));
	soap->header->wsa__RelatesTo->__item = (char *)soap_malloc(soap, sizeof(char)*100);
	strcpy(soap->header->wsa__RelatesTo->__item,MessageID);
	soap->header->wsa__RelatesTo->RelationshipType = NULL;
	soap->header->wsa__RelatesTo->__anyAttribute = NULL;
	soap->header->wsa5__MessageID = NULL;
	soap->header->wsa5__To= NULL;
	soap->header->wsa5__Action = NULL;

	NET_IPV4 ip;
	net_get_hwaddr(ETH_NAME, macaddr);
	sprintf(_HwId,"uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/device_service", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

	strcpy((char *)MessageID,_HwId);
	ReplyTo = NULL;

	if(MatchBy != NULL)
		if(!strcmp(MatchBy,"InvalidMatchRule"))
			return 2;

	if(Scopes != NULL)
	{

		if(strcmp(Scopes, "") == 0)
		{
			MATCH = 1;
		}
		else
		{
			explodeitem((char *)Scopes, ' ', list);
			for(i = 0; **(list + i) != 0; i++)
			{
				if(strstr(list[i], "onvif://www.onvif.org/type"))
				{
					for(j = 0; j < 3; j++)
					{
						if(strlen(oSysInfo->onvif_scopes.type[j]) > 1)
						{
							if(strcmp(list[i], oSysInfo->onvif_scopes.type[j]) == 0)
								MATCH = 1;

						}
					}
				}

				else if(strstr(list[i], "onvif://www.onvif.org/location"))
				{
					for(j = 0; j < 3; j++)
					{
						if(strlen(oSysInfo->onvif_scopes.location[j]) > 1)
						{
							if(strcmp(list[i], oSysInfo->onvif_scopes.location[j]) == 0)
								MATCH = 1;
						}
					}
				}


				else if(strstr(list[i], "onvif://www.onvif.org/hardware"))
				{
					if(strcmp(list[i], oSysInfo->onvif_scopes.hardware) == 0)
						MATCH = 1;
				}

				else if(strstr(list[i], "onvif://www.onvif.org/name"))
				{
					if(strcmp(list[i], oSysInfo->onvif_scopes.name) == 0)
						MATCH = 1;
				}
				else if(strstr(list[i], "onvif://www.onvif.org/"))
				{
					for(j = 0; j < 3; j++)
					{
						if(strlen(oSysInfo->onvif_scopes.improper_input[j]) > 1)
						{
							if(strcmp(list[i], oSysInfo->onvif_scopes.improper_input[j]) == 0)
							{
								MATCH = 1;
								goto label_outoffor;
							}
							else
							{
								MATCH = 0;
							}
						}
					}
label_outoffor:
					if(MATCH == 0)
					{
						return 1;
					}

				}
				else
				{	
					return 1;
				}


			}
		}
	}



	else
	{
		MATCH =1;
	}

	if((MATCH == 1) && (oSysInfo->lan_config.net.discovery_mode == 0))
	{


		ns1__ProbeResponse->__sizeProbeMatch = 1;
		ns1__ProbeResponse->ProbeMatch = (struct d__ProbeMatchType *)soap_malloc(soap, sizeof(struct d__ProbeMatchType));

		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		sprintf(_HwId,"urn:uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
		strcpy(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.Address, _HwId);

		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ReferenceProperties = (struct wsa__ReferencePropertiesType*)soap_malloc(soap,sizeof(struct wsa__ReferencePropertiesType));
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ReferenceProperties->__size = 0;
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ReferenceProperties->__any = NULL;
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ReferenceParameters = (struct wsa__ReferenceParametersType*)soap_malloc(soap,sizeof(struct wsa__ReferenceParametersType));
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ReferenceParameters->__size = 0;
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ReferenceParameters->__any = NULL;

		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.PortType = (char **)soap_malloc(soap, sizeof(char *) * SMALL_INFO_LENGTH);
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.PortType[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
		strcpy(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.PortType[0], "ttl");

		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ServiceName = (struct wsa__ServiceNameType*)soap_malloc(soap,sizeof(struct wsa__ServiceNameType));
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ServiceName->__item = NULL;
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ServiceName->PortName = NULL;
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.ServiceName->__anyAttribute = NULL;

		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.__size = 0;

		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.__any = (char **)soap_malloc(soap, sizeof(char*) * SMALL_INFO_LENGTH);
		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.__any[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
		strcpy(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.__any[0], "Any");

		ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.__anyAttribute = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
		strcpy(ns1__ProbeResponse->ProbeMatch->wsa__EndpointReference.__anyAttribute, "Attribute");


		ns1__ProbeResponse->ProbeMatch->Types = (char *)soap_malloc(soap, sizeof(char ) * INFO_LENGTH);
		strcpy(ns1__ProbeResponse->ProbeMatch->Types, "dn:NetworkVideoTransmitter");

		ns1__ProbeResponse->ProbeMatch->Scopes = (struct d__ScopesType*)soap_malloc(soap,sizeof(struct d__ScopesType));
		ns1__ProbeResponse->ProbeMatch->Scopes->__item =(char *)soap_malloc(soap, LARGE_INFO_LENGTH);
		memset(ns1__ProbeResponse->ProbeMatch->Scopes->__item,0,sizeof(ns1__ProbeResponse->ProbeMatch->Scopes->__item));        
		for(i = 0; i < 3; i++)
		{
			if((oSysInfo->onvif_scopes.type[i]) != NULL)
			{
				strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, oSysInfo->onvif_scopes.type[i]);
				strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, " ");
				i++;
			}
		}


		strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, oSysInfo->onvif_scopes.name);
		strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, " ");
		for(i = 0; i < 3; i++)
		{
			if((oSysInfo->onvif_scopes.location[i]) != NULL)
			{
				strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, oSysInfo->onvif_scopes.location[i]);
				strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, " ");
				i++;
			}
		}

		strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, oSysInfo->onvif_scopes.hardware);
		strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, " ");
		for(i = 0; i < 3; i++)
		{
			if(oSysInfo->onvif_scopes.improper_input[i] != NULL)
			{
				strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, oSysInfo->onvif_scopes.improper_input[i]);
				strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, " ");
				i++;
			}
		}
		strcat(ns1__ProbeResponse->ProbeMatch->Scopes->__item, "\0");
		ns1__ProbeResponse->ProbeMatch->Scopes->MatchBy = NULL;

		ns1__ProbeResponse->ProbeMatch->XAddrs = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(ns1__ProbeResponse->ProbeMatch->XAddrs, _IPAddr);
		ns1__ProbeResponse->ProbeMatch->MetadataVersion = 1;
	}
	else
	{
		return 1;
	}
	return SOAP_OK;
}


void d_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct d__ResolveMatchType *match)
{
	return SOAP_OK;
}

void d_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct d__ProbeMatchesType *matches)
{
	return SOAP_OK;
}

void d_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
	return SOAP_OK;
}

void d_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
	return SOAP_OK;
}


soap_d_mode d_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct d__ResolveMatchType *match)
{
	return SOAP_OK;
}
