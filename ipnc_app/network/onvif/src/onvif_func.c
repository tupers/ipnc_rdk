#include <onvif.h>
#include <file_msg_drv.h>

#define KEY (1492)
#define NUM_BROADCAST (4)        //number of times hello is broadcasted

int server_udp;
int server_hello;
char udp_sync = 0;
pthread_t hello_thread;
pthread_t set_ip_thread;
int sub_cnt;
int sub_cnt_prev;
int eventstatus;
int IsDHCPchange;
int IsIPchange;
int set_DHCP_value;


/*
 *  @brief Used to initalize the dom attributes (names) and the correspoonding values (data)  
 */

int dom_att(struct soap_dom_attribute *dom, int num_of_attributes, char *name[], char *data[], char *nstr, wchar_t *wide, struct soap *soap)
{
    int i = 0;
    struct soap_dom_attribute *att;
    att = dom;
    while(dom)
    {
        dom->nstr = NULL;
        dom->wide = NULL;
        if(name == NULL)
            dom->name = NULL;
        else
            dom->name = name[i];

        if(data == NULL)
            dom->data = NULL;
        else
            dom->data = data[i];
        dom->soap = soap;
        if (num_of_attributes > 0)
        {
            dom->next = (struct soap_dom_attribute *)soap_malloc(soap, sizeof(struct soap_dom_attribute));
            dom = dom->next;
            num_of_attributes--;
            i++;
        }
        else
        {
            dom->next = NULL;
            return 0;
        }
    }
	return 0;
}

int soap_default_dom_element(struct soap * soap, struct soap_dom_element * dom_e)
{
	dom_e->next = NULL;
	dom_e->prnt = NULL;
	dom_e->elts = NULL;
	dom_e->atts = NULL;
	dom_e->nstr = NULL;
	dom_e->name = NULL;
	dom_e->data = NULL;
	dom_e->wide = NULL;
	dom_e->type = 0;
	dom_e->node = NULL;
	dom_e->head = NULL;
	dom_e->tail = NULL;
	dom_e->soap = soap;
 	return 0;
}

int soap_default_notify_msg(struct soap * soap,struct _tt__Message * msg)
{
	msg->Source = NULL;
	msg->Key = NULL;
	msg->Data = NULL;
	msg->Extension = NULL;
	msg->PropertyOperation = NULL;
	msg->UtcTime = (time_t)0;
	msg->PropertyOperation = NULL;
	dom_att(&msg->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	return 0;
}
const char * wtochar(struct soap * soap,const wchar_t * wchar)
{
	char *retchar = NULL;
	int w_len = 0,index = 0;
	char *tmp = NULL;
	w_len = (int)wcslen(wchar);
	tmp = retchar = soap_malloc(soap,sizeof(char)*w_len);
	if(w_len > 0)
	{
		for(index =0;index < w_len;index++)
		{
			*tmp++ = wchar[index];
		}
		*tmp = '\0';
		tmp = NULL;
		index = 0;
	}
	if(retchar != NULL)
	return (const char *)retchar;
	else return NULL;
}

int motion_conf(motion_param* mptr)
{
	int ret =0;
	if(mptr != NULL)
	{
		if((ret = ControlSystemData(SFIELD_SET_ALARM_ENABLE, &mptr->alarm_enable, sizeof(mptr->alarm_enable)))<0)
			ONVIF_DBG("Failed to enable ALARM");
		if((ret = ControlSystemData(SFIELD_SET_MOTIONENABLE,&mptr->motion_enable, sizeof(mptr->motion_enable)))<0)
			ONVIF_DBG("Failed to enable Motion");
		if((ret = ControlSystemData(SFIELD_SET_MOTIONCENABLE,&mptr->custmotion_enable, sizeof(mptr->custmotion_enable)))<0)
			ONVIF_DBG("Failed to Enable NORMAL MODE MOTION");
		if((ret = ControlSystemData(SFIELD_SET_MOTIONBLOCK,&mptr->motion_block, sizeof(mptr->motion_block)))<0)
			ONVIF_DBG("Failed to enable MOTIONCELLS");
		if((ret = ControlSystemData(SFIELD_SET_MOTIONLEVEL,&mptr->motion_level, sizeof(mptr->motion_level)))<0)
			ONVIF_DBG("Failed to enable MOTIONLEVEL");
		if((ret = ControlSystemData(SFIELD_SET_MOTIONBLOCK,&mptr->motion_block, sizeof(mptr->motion_block)))<0)
			ONVIF_DBG("Failed to enable iMOTIONCELLS");
		if((ret = ControlSystemData(SFIELD_SET_MOTIONLEVEL,&mptr->motion_level, sizeof(mptr->motion_level)))<0)
			ONVIF_DBG("Failed to enable MOTIONLEVEL");

	}
	else ret = -1;
	return ret;
}

/* @brief Parse the scope URI and return the scope parameter */
char *strparser(char *str)
{
	int i = 0;
	int j = 0;
	char *retstr = malloc(sizeof(char) * INFO_LENGTH);
	while(i < 3)
	{
		if(*str == '/')
		{
			i++;
		}
		str++;
	}
	while(*str != '/' && *str != '\0')
	{
		*(retstr + j) = *str;
		j++;
		str++;
	}
	*(retstr + j) = '\0';
	if(((strcmp(retstr, "type") == 0)) || (strcmp(retstr, "name") == 0) || (strcmp(retstr, "location") == 0) || (strcmp(retstr, "hardware") == 0))
	{
		return retstr;
	}
	else
	{
		return NULL;
	}
}

/*
 *  @brief Parsing the string for Motion and Facedetection parameters 
 *  @return Parameters of Motion and FD parameters
 */
char *anyparser(char *str)
{
	char *retstr = malloc(sizeof(char)* INFO_LENGTH);
	while(*str != '>')
	{
                str++;
	}
        if(strstr(str,"tns1:VideoAnalytics/tns1:MotionDetection/") != NULL ||  strstr(str,"tns1:VideoAnalytics/tns1:FaceDetection/") != NULL)
        {
                return str;
        }
	else if(strstr(str,"@Name") != NULL)
	{
		str = strstr(str,"@Name=");
		while(*str != '"')
		{
			str ++;
			
		}
		str++;
		int i=0;
		while(*str != ']')
		{
			*(retstr+i) = *str;
			str ++;
			i++;
		}
		*(retstr + (--i)) = '\0';
		return retstr;
	}
        else
	{
		return NULL;
	}

}

long epoch_convert_switch(int value, char convert, int time)
{
	long seconds = 0;
	switch(convert)
	{
		case 'Y': seconds = value * YEAR_IN_SEC ;
			  break;
		case 'M':
			  if(time == 1)
			  {
				  seconds = value * MIN_IN_SEC;
			  }
			  else
			  {
				  seconds = value * MONTH_IN_SEC;
			  }
			  break;
		case 'D': seconds = value * DAY_IN_SEC;
			  break;
		case 'H': seconds = value * HOURS_IN_SEC;
			  break;
		case 'S': seconds = value;
			  break;
	}
	return seconds;
}

/*
 *  @brief Converts XML period format (PnYnMnDTnHnMnS) to long data type
 */

long periodtol(char *ptr)
{
	char buff[10] = "\0";
	char convert;
	int i = 0; 
	int value = 0;
	int time = 0;
	long cumulative = 0;

	while(*ptr != '\0')
	{

		if(*ptr == 'P' || *ptr == 'T')
		{
			ptr++;
			if(*ptr == 'T')
			{
				time = 1;
				ptr++;
			}
		}
		else
		{
			if(*ptr >= '0' && *ptr <= '9')
			{
				buff[i] = *ptr;
				i++;
				ptr++;
			}
			else
			{
				buff[i] = '\0';
				i=0;
				value = atoi(buff);
				memset(buff, 0, sizeof(buff));
				convert = *ptr;
				ptr++;
				cumulative += epoch_convert_switch(value, convert, time);
			}
		}
	}

	return cumulative;
}

int xsperiodtol(const char * timestring)
{
	int t_sec = 0;
	int i = 0;
	int hour = 0, minute = 0, sec = 0, hour_diff = 0, minute_diff = 0;
	char t_ch[3];
	time_t time_n;
	struct tm * tm_t;
	time_n = time(NULL);
	tm_t = localtime(&time_n);
	char time_cur[150];
	sprintf(time_cur,"%4d-%02d-%02dT%02d:%02d:%02d",1900+tm_t->tm_year,1+tm_t->tm_mon,tm_t->tm_mday,tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec);
	char *tst = NULL;
	tst = (char *)strstr(timestring,"T");
	
	if(tst != NULL)
	{
		tst++;
		while((t_ch[i] = *tst++)!= ':')i++;
		t_ch[i] = '\0';
		i = 0;
		hour = (int)atoi(t_ch);
		hour_diff = hour - tm_t->tm_hour;
		while((t_ch[i] = *tst++)!= ':')i++;
		t_ch[i] = '\0';
		i = 0;
		minute = (int)atoi(t_ch);
		minute_diff = minute - tm_t->tm_min;
		while((t_ch[i] = *tst++)!= 'Z')i++;
		t_ch[i] = '\0';
		i = 0;
		sec = (int)atoi(t_ch);
	}
	
	t_sec = (hour_diff*60*60 + minute_diff*60 + sec);
	return (int)t_sec;

}

unsigned long iptol(char *ip)
{
	unsigned char o1,o2,o3,o4; /* The 4 ocets */
	char tmp[13] = "000000000000\0";
	short i = 11; /* Current Index in tmp */
	short j = (strlen(ip) - 1);
	do 
	{
		if ((ip[--j] == '.'))
		{
			i -= (i % 3);
		}
		else 
		{
			tmp[--i] = ip[j];
		}
	}while (i > -1);

	o1 = (tmp[0] * 100) + (tmp[1] * 10) + tmp[2];
	o2 = (tmp[3] * 100) + (tmp[4] * 10) + tmp[5];
	o3 = (tmp[6] * 100) + (tmp[7] * 10) + tmp[8];
	o4 = (tmp[9] * 100) + (tmp[10] * 10) + tmp[11];
	return (o1 * 16777216) + (o2 * 65536) + (o3 * 256) + o4;
}

/* @brief Check if IP is valid */
int isValidIp4 (char *str) 
{
	int segs = 0;   /* Segment count. */     
	int chcnt = 0;  /* Character count within segment. */     
	int accum = 0;  /* Accumulator for segment. */      
	/* Catch NULL pointer. */      
	if (str == NULL) return 0;      
	/* Process every character in string. */      
	while (*str != '\0') 
	{         
		/* Segment changeover. */          
		if (*str == '.') 
		{             
			/* Must have some digits in segment. */              
			if (chcnt == 0) return 0;              
			/* Limit number of segments. */              
			if (++segs == 4) return 0;              
			/* Reset segment values and restart loop. */              
			chcnt = accum = 0;             
			str++;             
			continue;         
		}  

		/* Check numeric. */          
		if ((*str < '0') || (*str > '9')) return 0;
		/* Accumulate and check segment. */          
		if ((accum = accum * 10 + *str - '0') > 255) return 0;
		/* Advance other segment specific stuff and continue loop. */          
		chcnt++;         
		str++;     
	}      
	/* Check enough segments and enough characters in last segment. */      
	if (segs != 3) return 0;      
	if (chcnt == 0) return 0;      
	/* Address okay. */      
	return 1; 
} 

/* @brief Check if a hostname is valid */
int isValidHostname (char *str) 
{
	/* Catch NULL pointer. */      
	if (str == NULL) 
	{
		return 0;      
	}
	/* Process every character in string. */      
	while (*str != '\0') 
	{         
		if ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || (*str >= '0' && *str <= '9') || (*str == '.') || (*str == '-') )
		{
			str++;
		}
		else
		{
			return 0;
		}
	}
	return 1; 
}

#define MAXBUFSIZE 65536 // Max UDP Packet size is 64 Kbyte
#define NIPQUAD(addr) ((unsigned char *)&addr)[0], ((unsigned char *)&addr)[1], ((unsigned char *)&addr)[2], ((unsigned char *)&addr)[3]
int udp_probe1(int *server_udp)
{
   int sock, status;
   unsigned int socklen;
   char buffer[MAXBUFSIZE];
   struct sockaddr_in saddr;
   struct ip_mreq imreq;

   // set content of struct saddr and imreq to zero
   memset(&saddr, 0, sizeof(struct sockaddr_in));
   memset(&imreq, 0, sizeof(struct ip_mreq));

   saddr.sin_family = PF_INET;
   saddr.sin_port = htons(3702); // listen on port 4096
   saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface
   sock = (int) server_udp;
   status = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

   if ( status < 0 )
     perror("Error binding socket to interface");

   imreq.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
   imreq.imr_interface.s_addr = INADDR_ANY; // use DEFAULT interface

   // JOIN multicast group on default interface
   status = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
              (const void *)&imreq, sizeof(struct ip_mreq));

   socklen = sizeof(struct sockaddr_in);

    // receive packet from socket
     while(1)
    {
        int len = 0;
        memset(buffer,0,MAXBUFSIZE);
        len = recvfrom(sock, buffer, MAXBUFSIZE, 0,
                     (struct sockaddr *)&saddr, &socklen);
        if(len==-1)
            printf("errno:%d ",errno);
        else
            printf("[src=%d.%d.%d.%d,sport=%d,len=%d,buf=%s] \n\n",
                NIPQUAD(saddr.sin_addr.s_addr),
                ntohs(saddr.sin_port),
                len,
                buffer
        );
    }


   // shutdown socket
   shutdown(sock, 2);
   // close socket
   close(sock);

   return 0;


}

/*
 * Name: udp_probe
 *
 * Description: This operation binds the socket and calls soap_server function.
 */
int udp_probe(int *server_udp)
{
    int retval;
    struct soap *soap_udp;
    int fault_flag = 0;
    SysInfo* oSysInfo = GetSysInfo();

    while(1)
    {
    	usleep(4000);
        if(oSysInfo->lan_config.net.discovery_mode == DISCOVERABLE)
        {
            struct ip_mreq mcast; 
            mcast.imr_multiaddr.s_addr = inet_addr("235.255.255.250"); 
            mcast.imr_interface.s_addr = htonl(INADDR_ANY);

            soap_udp=soap_new();
            soap_init1(soap_udp, SOAP_IO_UDP);
            soap_udp->master = (int) server_udp;
            soap_udp->socket = (int) server_udp;
            soap_udp->errmode = 0;
            soap_udp->bind_flags = 1;

            if (!soap_valid_socket(soap_bind(soap_udp, NULL, 3702, 100)))
            {	 
                soap_print_fault(soap_udp, stderr);
            }
            retval = soap_serve(soap_udp); 
            if(retval && !(fault_flag))
            {
                fault_flag = 1;
            }
            else if(!retval)
            {
                fault_flag = 0;
            }
            soap_destroy(soap_udp);
            soap_end(soap_udp);
            soap_done(soap_udp);
            free(soap_udp);
        }

    }
}	


/**
 * @brief explodeitem
 * @This function splits the given string based on a delimiter.
    'list' is an array which holds the split strings
  */

void explodeitem(char *in, char dl, char list[][100])
{
	int i = 0;
	int j = 0;
	int k = 0;
	int len = strlen(in);
	for(i = 0; i < len; i++)
	{
		if(in[i] == dl)
		{
			
			*( *(list + j) + k) = 0;
			j++;
			k = 0;
		}
		else
		{
			*( *(list + j) + k) = in[i];
			k++;
		}
	}
	*(*(list + j) + k) = 0;
	j++;
	**(list + j) = 0;
}

int __Notify(struct soap* soap, struct _wsnt__Notify *wsnt__Notify,notify_struct * ptr)
{
	struct _tt__Message * msg = (struct _tt__Message *)soap_malloc(soap,sizeof(struct _tt__Message));
	soap_default_notify_msg(soap,msg);
	time_t time_n;
	struct tm *tm_t;
	time_n = time(NULL);
	tm_t = localtime(&time_n);
	char time_cur[150] = "\0";
	char _IPAddr[INFO_LENGTH];
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	sprintf(time_cur,"%4d-%02d-%02dT%02d:%02d:%02d",1900+tm_t->tm_year,1+tm_t->tm_mon,tm_t->tm_mday,tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec);
	
	wsnt__Notify->__sizeNotificationMessage = 1;
	wsnt__Notify->NotificationMessage = (struct wsnt__NotificationMessageHolderType *)soap_malloc(soap,sizeof(struct wsnt__NotificationMessageHolderType)*wsnt__Notify->__sizeNotificationMessage);
	wsnt__Notify->NotificationMessage->SubscriptionReference = (struct wsa5__EndpointReferenceType *)soap_malloc(soap, sizeof(struct wsa5__EndpointReferenceType));
	wsnt__Notify->NotificationMessage->SubscriptionReference->Address = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services/SubscriptionIdx=%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3],ptr->t_cnt);
	strcpy(wsnt__Notify->NotificationMessage->SubscriptionReference->Address,_IPAddr);
	wsnt__Notify->NotificationMessage->SubscriptionReference->ReferenceParameters = NULL;
	wsnt__Notify->NotificationMessage->SubscriptionReference->Metadata = NULL;
	wsnt__Notify->NotificationMessage->SubscriptionReference->__size = 0;
	wsnt__Notify->NotificationMessage->SubscriptionReference->__any = NULL;
	wsnt__Notify->NotificationMessage->SubscriptionReference->__anyAttribute = NULL;
	
	wsnt__Notify->NotificationMessage->Topic = (struct wsnt__TopicExpressionType *)soap_malloc(soap, sizeof(struct wsnt__TopicExpressionType));	
	wsnt__Notify->NotificationMessage->Topic->__any = (struct soap_dom_element *)soap_malloc(soap,sizeof(struct soap_dom_element));
	soap_default_dom_element(soap, wsnt__Notify->NotificationMessage->Topic->__any);
	wsnt__Notify->NotificationMessage->Topic->__any->data = "tns1:RuleEngine/tns1:CellMotionDetector/tns1:Motion";
	wsnt__Notify->NotificationMessage->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";
	dom_att(&wsnt__Notify->NotificationMessage->Topic->__anyAttribute, 0, NULL, NULL, NULL, NULL, soap);
	soap_default_dom_element(soap, &wsnt__Notify->NotificationMessage->Topic->__mixed);
	
	wsnt__Notify->NotificationMessage->ProducerReference = NULL;
	
	wsnt__Notify->NotificationMessage->Message.__any = (struct soap_dom_element *)soap_malloc(soap,sizeof(struct soap_dom_element));
	soap_default_dom_element(soap,wsnt__Notify->NotificationMessage->Message.__any);
	{
		wsnt__Notify->NotificationMessage->Message.__any->nstr = "http://www.onvif.org/ver10/schema";
		wsnt__Notify->NotificationMessage->Message.__any->name = "Message";
		wsnt__Notify->NotificationMessage->Message.__any->type = SOAP_TYPE__tt__Message;
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
			msg->Data->SimpleItem[0].Value = "true";
			time(&msg->UtcTime);
		}
		wsnt__Notify->NotificationMessage->Message.__any->node = msg;
		wsnt__Notify->__size = 0;
		wsnt__Notify->__any = NULL;
	}
		
	return SOAP_OK;	
}

int function_event(notify_struct * ptr)
{
	unsigned char macaddr[MACH_ADDR_LENGTH];
	char _HwId[LARGE_INFO_LENGTH];
	time_t time_n;
	struct tm *tm_t;
	struct soap soap;
	struct _wsnt__Notify soap_tmp___wsnt__Notify;
	soap_init2(&soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE | SOAP_XML_INDENT);
	struct SOAP_ENV__Header header;
	soap_default_SOAP_ENV__Header(&soap, &header); 
	soap.header = &header;	
	net_get_hwaddr(ETH_NAME, macaddr);
	time_n = time(NULL);
	tm_t=localtime(&time_n);
	sprintf(_HwId,"uuid:1319d68a-date=%02d-%02d-%d-time=%02d:%02d.%02d MAC-ID -%02X%02X%02X%02X%02X%02X",1+tm_t->tm_mon,tm_t->tm_mday,1900+tm_t->tm_year,tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec,macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	header.wsa__MessageID =(char *)soap_malloc(&soap, sizeof(char) * 100);
	strcpy(header.wsa__MessageID, _HwId);
	header.wsa__To = (char *)soap_malloc(&soap, sizeof(char) * 100);
	strcpy(header.wsa__To,ptr->consumer_addr);
	header.wsa__Action = "http://docs.oasis-open.org/wsn/bw-2/Notification/Notify";
	__Notify(&soap, &soap_tmp___wsnt__Notify,ptr);
	soap_send___tev__Notify(&soap, ptr->consumer_addr, NULL, &soap_tmp___wsnt__Notify);
	//soap_print_fault(&soap, stderr);
//	soap_closesock(&soap); /* soap_send operations keep the socket open to possibly accept responses, so we need to explicitly close the socket now 
//	soap_end(&soap);  this will close the socket too (if keep alive is off), just in case */
	soap_destroy(&soap);
    soap_end(&soap);
	soap_done(&soap); /* detach environment (also closes sockets even with keep-alive) */

	return 0;
}

typedef struct 
{
	char *user_id;
	char *password;
	int authority; 
}add_user;
void *reboot_thread();

void *reboot_thread()
{
	sleep(2);
	kill(getpid(),SIGUSR1);
	pthread_exit(NULL);
}

void *hello_thr_scope(void *arg)
{
    usleep(1000);
    int *serv_udp;
    serv_udp = (int *)arg;    
    SysInfo *oSysInfo = GetSysInfo();
    if(oSysInfo->lan_config.net.discovery_mode == DISCOVERABLE)
    {
        udp_hello(serv_udp);
    }
    return NULL;
}

void *set_ip_thr(unsigned long s_addr)
{
        int semid;
        struct sembuf operations;
        SysInfo *oSysInfo = GetSysInfo();

        operations.sem_num = 0;
        operations.sem_op =  1;
        operations.sem_flg = 0;

        usleep(2000);
        if(IsIPchange)
                net_set_ifaddr(ETH_NAME,s_addr);
        if(IsDHCPchange)
                ControlSystemData(SFIELD_SET_DHCPC_ENABLE, (void *)&set_DHCP_value, sizeof(set_DHCP_value));
	
	usleep(200000);
        if(oSysInfo->lan_config.net.discovery_mode == DISCOVERABLE)
        {
                semid = semget(KEY, 1, 0666);
                if(semid > 0)
                        semop(semid, &operations, 1);
        }

        pthread_exit(NULL);
        return NULL;
}

void *notify_thr(int t_cnt_ptr)
{
	time_t current =0,now=0;
	int t_cnt = t_cnt_ptr;
	notify_args[t_cnt].status = 1;
	notify_args[t_cnt].thread_id = (pthread_t)pthread_self();
	notify_args[t_cnt].t_cnt = t_cnt;
	//fprintf(stderr,"\n\n\tBasic Notification Interface Thread-%d created with time out %d with status %d \n\n\tfor the consumer at %s \n\n",t_cnt, (long)notify_args[t_cnt].t_out_thr,notify_args[t_cnt].status,notify_args[t_cnt].consumer_addr);
	int sem_value = 0;
	union semun{
		int val;
		struct semid_ds *buf;
		short *array;
	}argument;
	int id;

    pthread_detach(notify_args[t_cnt].thread_id);
	
	/*Global variables for Configuring Motion Detection*/
	motion_param motion_p = {1,1,0,50,1,"060"};
	if(motion_conf(&motion_p)<0)
	ONVIF_DBG("Failed to enable and configure motion parameters");
	else ONVIF_DBG("Enabled and configured motion parameters successfully!");
	
	/* Get semaphore descriptor */
	id = semget(KEY_NOTIFY_SEM, 1, 0666);
	if(id < 0)
	{
		ONVIF_DBG( "Unable to obtain semaphore.");
	}


	/* Ignore any previous motion */
	argument.val = 0;
	semctl(id, 0, SETVAL, argument);
	int flag = 0;
	current = time(NULL);
	/* Loop till Termination Time */
	do
	{	
		/* semaphore get value */
		/* Semaphore value set in proc_alarm.c when motion is detected */
		sem_value = semctl(id, 0, GETVAL, 0);

		/* Check if any motion is detected */
		if(sem_value & MOTION_DETECTED_MASK)
		{
			if(function_event(&notify_args[t_cnt])) 	
			{			
				ONVIF_DBG("ERROR IN ASYNCHRONOUS ONE-WAY NOTIFICATION INTERFACE");
				pthread_exit(NULL);
			}
			argument.val = 0;
			if(semctl(id, 0, SETVAL, argument) < 0)
				ONVIF_DBG("failed to SETVAL upon MOTION DETECTION");
			if(!flag)ONVIF_DBG("MOTION DETECTED!!");flag = 1;
		}
		now = time(NULL);
	}while(((long)now-current <= (long)notify_args[t_cnt].t_out_thr));
	--sub_cnt;
	notify_thread[t_cnt] = 0;
	memset(&notify_args[t_cnt],0,sizeof(notify_args[t_cnt]));
	pthread_exit(NULL);
}

void *eventtracker_thr(int t_cnt_ptr)
{
	int t_cnt = t_cnt_ptr;
	notify_args[t_cnt].status = 1;
	notify_args[t_cnt].thread_id = (pthread_t)pthread_self();
	notify_args[t_cnt].t_cnt = t_cnt;
	int sem_value = 0;
	union semun{
		int val;
		struct semid_ds *buf;
		short *array;
	}argument;
	int id;
	time_t current = 0,now = 0;

    pthread_detach(notify_args[t_cnt].thread_id);

	/* Get semaphore descriptor */
	id = semget(KEY_NOTIFY_SEM, 1, 0666);
	if(id < 0)
	{
		ONVIF_DBG("Unable to obtain semaphore.");
	}

	motion_param motion_p = {1,1,0,50,1,"060"};
	if(motion_conf(&motion_p)<0)
	ONVIF_DBG("Failed to enable and configure motion parameters");
	else ONVIF_DBG("Enabled and configured motion parameters successfully!");

	/* Ignore any previous motion */
	argument.val = 0;
	semctl(id, 0, SETVAL, argument);
	/* Loop till Termination Time */

	current = time(NULL);
	do
	{	
		/* semaphore get value */
		/* Semaphore value set in proc_alarm.c when motion is detected */
		sem_value = semctl(id, 0, GETVAL, 0);
		/* Check if any motion is detected */
		if(sem_value & MOTION_DETECTED_MASK)
		{
			argument.val = 0;
			semctl(id, 0, SETVAL, argument);
			eventstatus = 1;
		}
		else if(sem_value & NO_MOTION_MASK)
		{
			argument.val = 0;
			semctl(id, 0, SETVAL, argument);
		}
		now = time(NULL);
	}while(((long)now-current <= (long)notify_args[t_cnt].t_out_thr));
	--sub_cnt;
	notify_thread[t_cnt] = 0;	
	memset(&notify_args[t_cnt],0,sizeof(notify_args[t_cnt]));
	ONVIF_DBG("PullPointThread - Reached a Time Out");
	pthread_exit(NULL);
}


