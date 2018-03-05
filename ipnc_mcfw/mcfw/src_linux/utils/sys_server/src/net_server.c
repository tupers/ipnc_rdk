/*
 * net_mng_main.c
 *
 *  Created on: 11 May, 2016
 *      Author: zpmc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sysctrl.h>
#include <errno.h>
#include "Msg_Def.h"
#include <net_server.h>
#include <net_config.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stream_env_ctrl.h>
#include <system_control.h>
#include <file_msg_drv.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ez_log.h>

#define SYSTEM_SERVER_BUFFER_LENGTH	256	

//#define EZSERVER_CONFIG_DEBUG

#ifdef EZSERVER_CONFIG_DEBUG
#define DBG_ERR(x...)		Log_printf(x)
#define DBG(x...)			Log_printf(x)
#else
#define DBG_ERR(x...)
#define DBG(x...)
#endif



static unsigned int gFrameUdpSd;
static unsigned char gStreamStart = 0;
static unsigned int gServerstop = 0;
static unsigned char gRestart = 0;
static int NetShareMemInit(int key,unsigned int size);
static void NetShareMemRead(int offset,void *buf, int length);

static int SendFrame(EzFrameInfo *info,char *ip,unsigned port)
{
	struct sockaddr_in daddr;
	struct sockaddr saddr;
	socklen_t addrlen = sizeof(struct sockaddr);
	int recvlen;
	int ret;
	int len;
	unsigned int size = info->pitch * info->height;
	
	char *buf = (char *)malloc(size);
	char *img = buf;	
	
	NetShareMemRead(0,buf, size);

	daddr.sin_family = AF_INET; 
	daddr.sin_port = htons(port);
	daddr.sin_addr.s_addr = inet_addr(ip) ;
	bzero(&(daddr.sin_zero), 8);

	len = sizeof(EzFrameInfo);
	
	/** send header **/
	ret = sendto(gFrameUdpSd, info, len, 0,&daddr, sizeof(saddr));
	if(ret!=len)
	{
		DBG_ERR("%s in %d:Send Frame header fail\n",__func__,__LINE__);
		return -1;
	}
	
	ret = recvfrom(gFrameUdpSd, &recvlen, sizeof(int), 0,&saddr, &addrlen);      
	if(ret!=sizeof(int))
	{
		DBG_ERR("%s in %d:receive header Frame fail\n",__func__,__LINE__);
		return -1;
	}   

	if(recvlen!=len)
	{
		DBG_ERR("%s in %d:receive header Frame fail\n",__func__,__LINE__);
		return -1;		
	}	
	
	while(size>0)
	{
		if(size>FRAME_LEN)
			len = FRAME_LEN;
		else
			len = size;
				
		ret = sendto(gFrameUdpSd, img, len, 0,&daddr, sizeof(saddr));
		if(ret!=len)
		{
			DBG_ERR("%s in %d:Send Frame fail\n",__func__,__LINE__);
			return -1;
		}
		else
		{
			img += len;
			size -= len;
		}

		ret = recvfrom(gFrameUdpSd, &recvlen, sizeof(int), 0,&saddr, &addrlen);      
		if(ret!=sizeof(int))
		{
			DBG_ERR("%s in %d:receive ack Frame fail\n",__func__,__LINE__);
			return -1;
		}   

		if(recvlen!=len)
		{
			DBG_ERR("%s in %d:receive ack Frame fail\n",__func__,__LINE__);
			return -1;		
		}
	}

	return 0;
}

static unsigned char gSendDataFlag = false;
static EzFrameInfo gInfo;
static unsigned char MsgBuf[2048];
static unsigned int logFlag = 0;
static int ProcMsgThread(NetMsg *msg,NetMsg *ackmsg)
{
	int ret = true;
	char buffer[SYSTEM_SERVER_BUFFER_LENGTH];
	
	ackmsg->cmd = msg->cmd;
	ackmsg->cmdValueLen = 0;
	ackmsg->ackCmd = ACKCMD_SOK;	

	if(msg->cmd != NET_MSG_LOGIN)
	{
		if(!logFlag)
		{
			ackmsg->ackCmd = ACKCMD_SFAIL;	
			return true;
		}	
	}
	
	switch(msg->cmd)
	{
		case NET_MSG_LOGIN:
		{
			DBG("NET_MSG_LOGIN\n");
			Acount_t	prm;
			if(msg->cmdValueLen != sizeof(prm))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			memcpy(&prm,msg->cmdValue,msg->cmdValueLen);

			if(EzDoLogin(&prm)<0)
			{
				DBG_ERR("Fail at NET_MSG_LOGIN\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			logFlag = 1;
			ackmsg->cmdValueLen = sizeof(Acount_t);
			memcpy(ackmsg->cmdValue,&prm,sizeof(Acount_t));		
			break;

		}
		case NET_MSG_GETUSER:
		{
			get_userdata_t	prm;
			DBG("NET_MSG_GETUSER\n");
			ackmsg->cmdValueLen = 	sizeof(prm);
			if(GetUser(&prm))
			{
				DBG_ERR("Fail at NET_MSG_GETUSER\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
			}
			memcpy(ackmsg->cmdValue,&prm,sizeof(prm));	
			break;
		}		
		case NET_MSG_ADDUSER:
		{
			Acount_t value;
			DBG("NET_MSG_ADDUSER\n");
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			memcpy(&value,msg->cmdValue,msg->cmdValueLen);

			if(AddUser(&value) != 0)
			{
				DBG_ERR("Fail at NET_MSG_ADDUSER\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}			
		case NET_MSG_DELUSER:
		{
			DBG("NET_MSG_DELUSER\n");
			memcpy(buffer,msg->cmdValue,msg->cmdValueLen);
			buffer[msg->cmdValueLen] = '\0';
			if(strlen(buffer) > USER_LEN){
				DBG_ERR("Invalid user name\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			if(DelUser(buffer) != 0){
				DBG_ERR("Fail at NET_MSG_DELUSER\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GETSYSINFO:
		{
			DBG("NET_MSG_GETSYSINFO\n");
			EzSysInfo info;
			if(SysGetSysInfo(&info)<0)
			{
				DBG_ERR("Fail at NET_MSG_GETSYSINFO\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			ackmsg->cmdValueLen = sizeof(EzSysInfo);
			memcpy(ackmsg->cmdValue,&info,sizeof(EzSysInfo));	
			break;			
		}
		case NET_MSG_SETSYSINFO:
		{
			DBG("NET_MSG_SETSYSINFO\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			if(msg->cmdValueLen != sizeof(EzSysInfo))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;					
			}
			if(SysSetSysInfo((EzSysInfo *)(msg->cmdValue))<0)
			{
				DBG_ERR("Fail at NET_MSG_SETSYSINFO\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
		
			break;			
		}	
		case NET_MSG_UPDATE_DEFAULTPARAM:
		{
			DBG("NET_MSG_UPDATE_DEFAULTPARAM\n");

			if(msg->cmdValueLen != sizeof(EzSysInfo))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}	

			if(msg->cmdValueLen != sizeof(EzSysInfo))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;					
			}
			if(SysUpdateSysFile((EzSysInfo *)(msg->cmdValue))<0)
			{
				DBG_ERR("Fail at NET_MSG_UPDATE_DEFAULTPARAM\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			
			break;			
		}						
		case NET_MSG_SET_DHCP:
		{
			int value;
			DBG("NET_MSG_SET_DHCP\n");
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(int *)(msg->cmdValue);
			if(SetDhcpEnable(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_CONTRAST\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_DHCP:
		{
			int *pData;
			DBG("NET_MSG_GETSYSINFO\n");

			if((pData = GetDhcpEn()) == NULL)
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				DBG_ERR("fail at NET_MSG_GET_DHCP\n");
				break;
			}
			ackmsg->cmdValueLen = sizeof(int);
			memcpy(ackmsg->cmdValue,pData,sizeof(int));	
			break;
		}
		case NET_MSG_SET_NETIP:
		{
			DBG("NET_MSG_SET_NETIP\n");
			if((msg->cmdValueLen < 0)||(msg->cmdValueLen > 16))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}	

			if(SetIp(msg->cmdValue) != 0){
				DBG_ERR("Fail at NET_MSG_SET_NETMASK\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_NETIP:
		{
			char *ip;
			DBG("NET_MSG_GET_NETIP\n");
	
			if((ip = GetIp()) == NULL){
				DBG_ERR("Fail at NET_MSG_GET_NETIP\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			ackmsg->cmdValueLen = sizeof(in_addr_t);
			memcpy(ackmsg->cmdValue,ip,strlen(ip)+1);
			break;
		}
		case NET_MSG_SET_NETMASK:
		{
			DBG("NET_MSG_SET_NETMASK\n");
			if((msg->cmdValueLen < 0)||(msg->cmdValueLen > 16))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}	

			if(SetNetMask(msg->cmdValue) != 0){
				DBG_ERR("Fail at NET_MSG_SET_NETMASK\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_NETMASK:
		{
			char *mask;
			DBG("NET_MSG_GET_NETIP\n");

			if((mask = GetNetmask()) == NULL){
				ackmsg->ackCmd = ACKCMD_SFAIL;
				DBG_ERR("Fail at NET_MSG_GET_NETIP\n");
				break;
			}
			ackmsg->cmdValueLen = sizeof(in_addr_t);
			memcpy(ackmsg->cmdValue,mask,strlen(mask+1));	
			break;
		}
		case NET_MSG_SET_GTWAY:
		{
			DBG("NET_MSG_SET_GTWAY\n");
			if((msg->cmdValueLen < 0)||(msg->cmdValueLen > 16))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}	

			if(SetGateway(msg->cmdValue) != 0){
				DBG_ERR("Fail at NET_MSG_SET_GTWAY\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_GTWAY:
		{
			char *gateway;
			DBG("NET_MSG_GET_NETIP\n");

			if((gateway = GetIp()) == NULL){
				DBG_ERR("Fail at NET_MSG_GET_NETIP\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			
			ackmsg->cmdValueLen = sizeof(in_addr_t);
			memcpy(ackmsg->cmdValue,gateway,strlen(gateway)+1);	
			break;
		}
		case NET_MSG_NET_UPDATE:
		{
			DBG("NET_MSG_NET_UPDATE\n");
			if(msg->cmdValueLen != sizeof(Network_Config))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			if(SetNetwork((Network_Config*)(msg->cmdValue)) != 0){
				DBG_ERR("Fail at NET_MSG_NET_UPDATE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			else
			{
		//		gRestart = 1;
			}
			break;
		}
		case NET_MSG_SET_BRIGHTNESS:
		{
			unsigned char value;
			DBG("NET_MSG_SET_BRIGHTNESS\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}			

			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetCamBrightness(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_BRIGHTNESS\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}		
		case NET_MSG_GET_BRIGHTNESS:
		{
			unsigned char value;
			DBG("NET_MSG_GET_BRIGHTNESS\n");
			
			if(GetCamBrightness(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_BRIGHTNESS\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_CONTRAST:
		{
			unsigned char value;
			DBG("NET_MSG_SET_CONTRAST\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetCamContrast(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_CONTRAST\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_CONTRAST:
		{
			unsigned char value;
			DBG("NET_MSG_GET_CONTRAST\n");
			
			if(GetCamContrast(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_CONTRAST\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_SATURATION:
		{
			unsigned char value;
			DBG("NET_MSG_SET_SATURATION\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetCamSaturation(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_SATURATION\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_SATURATION:
		{
			unsigned char value;
			DBG("NET_MSG_GET_SATURATION\n");

			if(GetCamSaturation(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_SATURATION\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_SHARPNESS:
		{
			unsigned char value;
			DBG("NET_MSG_SET_SHARPNESS\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetCamSharpness(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_SHARPNESS\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_SHARPNESS:
		{
			unsigned char value;
			DBG("NET_MSG_GET_SHARPNESS\n");

			if(GetCamSharpness(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_SHARPNESS\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_EXPOSURE:
		{
			unsigned char value;
			DBG("NET_MSG_SET_EXPOSURE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetCamExposure(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_EXPOSURE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}	
		case NET_MSG_GET_EXPOSURE:
		{
			unsigned char value;
			DBG("NET_MSG_GET_EXPOSURE\n");
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			if(GetCamExposure(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_EXPOSURE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}	
		case NET_MSG_SET_GAIN:
		{
			unsigned char sensorGain,pipeGain;
			unsigned short gain;
			DBG("NET_MSG_SET_GAIN\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
						
			if(msg->cmdValueLen != sizeof(unsigned short))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			
			gain = *(unsigned short *)(msg->cmdValue);
			sensorGain = (gain>>8)&0xff;
			pipeGain = gain&0xff;

			if(EnvSetGain(sensorGain,pipeGain) != 0){
				DBG_ERR("Fail at NET_MSG_SET_GAIN\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}		
		case NET_MSG_GET_GAIN:
		{
			unsigned char sensorGain,pipeGain;
			unsigned short gain;
			DBG("NET_MSG_GET_GAIN\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvGetGain(&sensorGain,&pipeGain) != 0){
				DBG_ERR("Fail at NET_MSG_GET_GAIN\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			gain = (sensorGain<<8)|pipeGain;
			
			*(unsigned short *)(ackmsg->cmdValue) = gain;
			
			ackmsg->cmdValueLen = sizeof(gain);
			break;
		}						
		case NET_MSG_SET_BLC:
		{
			unsigned char value;
			DBG("NET_MSG_SET_BLC\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetBlc(value) != 0){
				DBG_ERR("Fail at NETgateway_MSG_SET_BLC\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}		
		case NET_MSG_GET_BLC:
		{
			unsigned char value;
			DBG("NET_MSG_GET_BLC\n");

			if(GetBlc(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_BLC\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_WB:
		{
			unsigned char value;
			DBG("NET_MSG_SET_WB\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetCamWhiteBalance(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_WB\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_WB:
		{
			unsigned char value;
			DBG("NET_MSG_GET_WB\n");

			if(GetCamWhiteBalance(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_WB\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_DAYNIGHTMODE:
		{
			unsigned char value;
			DBG("NET_MSG_SET_DAYNIGHTMODE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetCamDayNight(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_DAYNIGHTMODE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_DAYNIGHTMODE:
		{
			unsigned char value;
			DBG("NET_MSG_GET_WB\n");

			if(GetCamDayNight(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_WB\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_MIRROR:
		{
			unsigned char value;
			DBG("NET_MSG_SET_MIRROR\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetMirror(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_MIRROR\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_MIRROR:
		{
			unsigned char value;
			DBG("NET_MSG_GET_MIRROR\n");

			if(GetCamDayNight(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_MIRROR\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}	
		case NET_MSG_SET_BINNING:
		{
			unsigned char value;
			DBG("NET_MSG_SET_BINNING\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetBinning(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_BINNING\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_BINNING:
		{
			unsigned char value;
			DBG("NET_MSG_GET_BINNING\n");

			if(GetBinning(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_BINNING\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_IMG2A:
		{
			unsigned char value;
			DBG("NET_MSG_SET_IMG2A\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			DBG("NET_MSG_SET_IMG2A value is [%d]\n",value);
			if(SetImageAEW(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_IMG2A\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_IMG2A:
		{
			unsigned char value;
			DBG("NET_MSG_GET_IMG2A\n");

			if(GetImageAEW(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_IMG2A\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_IMG2ATYPE:
		{
			unsigned char value;
			DBG("NET_MSG_SET_IMG2ATYPE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);

			if(SetImageAEWType(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_IMG2ATYPE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}	
			break;
		}
		case NET_MSG_GET_IMG2ATYPE:
		{
			unsigned char value;
			DBG("NET_MSG_GET_IMG2ATYPE\n");

			if(GetImageAEWType(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_IMG2ATYPE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_EXPRIORITY:
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			break;
		case NET_MSG_SET_2A_PARAM:
		{
			DBG("NET_MSG_SET_2A_PARAM\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(EzCamH3AParam))
			{
				DBG("%s in %d\n",__func__,__LINE__);
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			if(EnvSetH2AParams((EzCamH3AParam *)(msg->cmdValue)) != 0){
				DBG_ERR("Fail at NET_MSG_SET_2A_PARAM\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}	
			break;
		}
		case NET_MSG_GET_2A_PARAM:
		{
			DBG("NET_MSG_GET_2A_PARAM\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}

			if(EnvGetH2AParams((EzCamH3AParam *)(msg->cmdValue)) != 0){
				DBG_ERR("Fail at NET_MSG_GET_2A_PARAM\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}	

			ackmsg->cmdValueLen = sizeof(EzCamH3AParam);
			break;
		}	
		case NET_MSG_SET_2A_WEIGHT:
		{
			DBG("NET_MSG_SET_2A_WEIGHT\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}

			if(msg->cmdValueLen == 0)
			{
				DBG("%s in %d\n",__func__,__LINE__);
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			if(EnvSetH2AWeight(msg->cmdValue,msg->cmdValueLen) != 0){
				DBG_ERR("Fail at NET_MSG_SET_2A_WEIGHT\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}	
			break;
		}
		case NET_MSG_GET_2A_WEIGHT:
		{
			DBG("NET_MSG_GET_2A_WEIGHT\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
		
			if(EnvGetH2AWeight(ackmsg->cmdValue,&(ackmsg->cmdValueLen)) != 0){
				DBG_ERR("Fail at NET_MSG_GET_2A_WEIGHT\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}	

			break;
		}	
						
		case NET_MSG_SET_VIDSTAB:
		{
			unsigned char value;
			DBG("NET_MSG_SET_VIDSTAB\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetVstabValue(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_VIDSTAB\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_VIDSTAB:
		{
			unsigned char value;
			DBG("NET_MSG_GET_VIDSTAB\n");

			if(GetVstabValue(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_VIDSTAB\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_LDC:
		{
			unsigned char value;
			DBG("NET_MSG_SET_VIDSTAB\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetLdcValue(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_VIDSTAB\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_LDC:
		{
			unsigned char value;
			DBG("NET_MSG_GET_LDC\n");

			if(GetLdcValue(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_LDC\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_VNF:
		{
			unsigned char value;
			DBG("NET_MSG_SET_VNF\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;printf("NET_MSG_IMGALG_NORMAL_MODE is [%d]\n",NET_MSG_IMGALG_NORMAL_MODE);
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetVnfValue(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_VNF\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_VNF:
		{
			unsigned char value;
			DBG("NET_MSG_GET_VNF\n");

			if(GetVnfValue(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_LDC\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_VNF_MODE:
		{
			unsigned char value;
			DBG("NET_MSG_SET_VIDSTAB\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetVnfMode(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_VIDSTAB\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_VNF_MODE:
		{
			unsigned char value;
			DBG("NET_MSG_GET_VNF_MODE\n");

			if(GetVnfMode(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_VNF_MODE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_VNFSTRENGTH:
		{
			unsigned char value;
			DBG("NET_MSG_SET_VNFSTRENGTH\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(value))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			value = *(unsigned char *)(msg->cmdValue);
			if(SetVnfStrength(value) != 0){
				DBG_ERR("Fail at NET_MSG_SET_VNFSTRENGTH\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_VNFSTRENGTH:
		{
			unsigned char value;
			DBG("NET_MSG_GET_VNFSTRENGTH\n");
			
			if(GetVnfStrength(&value) != 0){
				DBG_ERR("Fail at NET_MSG_GET_VNFSTRENGTH\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			*(unsigned char *)(ackmsg->cmdValue) = value;
			ackmsg->cmdValueLen = sizeof(value);
			break;
		}
		case NET_MSG_SET_FRAMECTR:
			break;
		case NET_MSG_GET_FRAMECTR:
			break;  
		case NET_MSG_RESET_PAYLOAD:
		{
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			EnvResetPayload();
			break;
		}
		case NET_MSG_START_PAYLOAD:
		{
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}		
			EnvStartPayload();
			break;
		}
		case NET_MSG_PRINT_PAYLOAD:
		{
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			EnvPrintPayload();
			break;
		}
		case NET_MSG_GET_PAYLOAD:
		{
			EzCpuDynamicParam prf;
			DBG("NET_MSG_GET_PAYLOAD\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvGetPayload(&prf) != 0){
				DBG_ERR("Fail at NET_MSG_GET_PAYLOAD\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			memcpy(ackmsg->cmdValue,&prf,sizeof(EzCpuDynamicParam));
			ackmsg->cmdValueLen = sizeof(EzCpuDynamicParam);

			break;
		}
		case NET_MSG_GET_FRAME_IMG:
		{
			DBG("NET_MSG_GET_IMG\n");
			EzFrameInfo *info;

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
						
			if(msg->cmdValueLen != sizeof(EzFrameInfo))
			{
				DBG_ERR("cmdValueLen is [%d],not equal with sizeof(EzFrameInfo) is [%d]\n",msg->cmdValueLen,sizeof(EzFrameInfo));
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			info = (EzFrameInfo *)(msg->cmdValue);

			memcpy(&gInfo,info,sizeof(EzFrameInfo));
			
			if(EnvGetFrameImg(&gInfo)<0)
			{
				DBG_ERR("Fail at EnvGetFrameImg\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;	
			}

			gSendDataFlag = true;

			break;
		}
		case NET_MSG_ENABLE_ENCODE:
		{
			DBG("NET_MSG_ENABLE_ENCODE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvEnableEncodeImg() != 0){
				DBG_ERR("Fail at NET_MSG_ENABLE_ENCODE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;			
		}
		case NET_MSG_DISABLE_ENCODE:
		{
			DBG("NET_MSG_DISABLE_ENCODE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvDisableEncodeImg() != 0){
				DBG_ERR("Fail at NET_MSG_DISABLE_ENCODE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;			
		}
		case NET_MSG_SET_LED:
		{
			DBG("NET_MSG_SET_LED\n");
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(msg->cmdValueLen != sizeof(CamLight_Config))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			if(EnvSetLedConfig((CamLight_Config *)(msg->cmdValue)) != 0){
				DBG_ERR("Fail at NET_MSG_SET_LED\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		}
		case NET_MSG_GET_LED:
		{
			DBG("NET_MSG_GET_LED\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvGetLedConfig((CamLight_Config *)(ackmsg->cmdValue)) != 0){
				DBG_ERR("Fail at NET_MSG_GET_LED\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}

			ackmsg->cmdValueLen = sizeof(CamLight_Config);

			break;
		}
		case NET_MSG_GET_ALG_RESULT:
		{
			Alg_Result *pResult = (Alg_Result *)MsgBuf;
			DBG("NET_MSG_GET_ALG_RESULT\n");
			
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			if(EnvGetAlgResult(pResult) != 0){
				DBG_ERR("Fail at NET_MSG_GET_ALG_RESULT\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			
			memcpy(ackmsg->cmdValue,pResult->result,pResult->resultSize);

			ackmsg->cmdValueLen = pResult->resultSize;
			break;
		}
		case NET_MSG_IMGALG_ENABLE:
		{
			DBG("NET_MSG_IMGALG_ENABLE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvEnableImgAlg() != 0){
				DBG_ERR("Fail at NET_MSG_IMGALG_ENABLE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		} 
		case NET_MSG_IMGALG_DISABLE:
		{
			DBG("NET_MSG_IMGALG_DISABLE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
						
			if(EnvDisableImgAlg() != 0){
				DBG_ERR("Fail at NET_MSG_IMGALG_DISABLE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		} 
		case NET_MSG_IMGALG_STATIC_IMG:
		{
			DBG("NET_MSG_IMGALG_STATIC_IMG\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
						
			if(EnvImgAlg_UseStaticImg() != 0){
				DBG_ERR("Fail at NET_MSG_SET_IMGALG_ENABLE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		} 
		case NET_MSG_IMGALG_SENSOR_IMG:
		{
			DBG("NET_MSG_SET_SENSOR_IMG\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
						
			if(EnvImgAlg_UseSensorImg() != 0){
				DBG_ERR("Fail at NET_MSG_SET_IMGALG_ENABLE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		} 				
		case NET_MSG_IMGALG_SET_PARAM:
		{
			DBG("NET_MSG_IMGALG_SET_PARAM\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvSetAlgParam(msg->cmdValue,msg->cmdValueLen)!=0)
			{
				DBG_ERR("Fail at NET_MSG_IMGALG_SET_PARAM\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			break;
		}	
		case NET_MSG_IMGALG_GET_PARAM:
		{	
			int prmSize;	
			DBG("NET_MSG_IMGALG_GET_PARAM\n");
			
			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}			
			
			if(EnvGetAlgParam(ackmsg->cmdValue,&prmSize)!=0)
			{
				DBG_ERR("Fail at NET_MSG_IMGALG_GET_PARAM\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
		
			ackmsg->cmdValueLen = prmSize;		
			break;
		}	
		case NET_MSG_IMGALG_DEF_PARAM:
		{
			DBG("NET_MSG_IMGALG_DEF_PARAM\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			
			if(EnvUpdate_AlgPrmfile(msg->cmdValue,msg->cmdValueLen)!=0)
			{
				DBG_ERR("Fail at NET_MSG_IMGALG_DEF_PARAM\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			break;
		}	
		case NET_MSG_IMGALG_NORMAL_MODE:
		{
			DBG("NET_MSG_IMGALG_NORMAL_MODE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
				
			if(EnvSetAlgMode(0) != 0){
				DBG_ERR("Fail at NET_MSG_IMGALG_NORMAL_MODE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		} 
		case NET_MSG_IMGALG_DEBUG_MODE:
		{
			DBG("NET_MSG_IMGALG_DEBUG_MODE\n");

			if(gStreamStart == 0)
			{
				DBG("stream not ready\n");
				
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;				
			}
			if(EnvSetAlgMode(1) != 0){
				DBG_ERR("Fail at NET_MSG_IMGALG_DEBUG_MODE\n");
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			break;
		} 
		case NET_MSG_DEBUG_NONE:
		{
			DBG("NET_MSG_DEBUG_NONE\n");

			EnvSendRemoteDebugNONECmd();

			break;			
		}
		case NET_MSG_DEBUG_SERIAL:
		{
			DBG("NET_MSG_DEBUG_SERIAL\n");

			EnvSendRemoteDebugSerialCmd();

			break;			
		}
		case NET_MSG_DEBUG_SOCKET:
		{
			DBG("NET_MSG_DEBUG_SOCKET\n");

			DebugParam *pPrm;
			
			if(msg->cmdValueLen != sizeof(DebugParam))
			{
				ackmsg->ackCmd = ACKCMD_SFAIL;
				break;
			}
			
			pPrm = (DebugParam *)(msg->cmdValue);

			EnvSendRemoteDebugSocketCmd(pPrm->port,pPrm->ip);

			break;			
		}
		case NET_MSG_DEBUG_QUIT:
		{
			DBG("NET_MSG_DEBUG_QUIT\n");

			EnvSendRemoteDebugQuitCmd();

			break;			
		}						
		case NET_MSG_RESET_SYSTEM:
		{
			DBG("NET_MSG_RESET_SYSTEM\n");

			gRestart = 1;
			
			break;	
		}									
		default:
			DBG_ERR("[%s] in line [%d]:unkown message\n",__func__,__LINE__);
			ackmsg->ackCmd = ACKCMD_SFAIL;
			ackmsg->cmdValueLen = 0;
			break;
	}
	
	return ret;
}
static void Server_Proc(int sd,struct sockaddr_in saddr)
{
	int ret;
	NetMsg *msg;
	NetMsg *ackmsg;
	bool isQuit = 0;
	logFlag = 0;
	
	ret = set_tcp_socket_keepalive(sd,1,2,3,2);
	if(ret<0)
	{
		DBG_ERR("set_tcp_socket_keepalive fail\n");
	}
	
	msg = (NetMsg *)malloc(sizeof(NetMsg) + NETMSG_MAXLEN);
	if(NULL == msg)
	{
		DBG_ERR("error@%s in %d : malloc error!\n",__func__,__LINE__);

		return ;
	}

	ackmsg = (NetMsg *)malloc(sizeof(NetMsg) + NETMSG_MAXLEN);
	if(NULL == ackmsg)
	{
		free(msg);
		DBG_ERR("error@%s in %d : malloc error!\n",__func__,__LINE__);

		return ;
	}

	ackmsg->cmdValueLen = NETMSG_MAXLEN;
	
	while(!isQuit)
	{
		ret = recv(sd, msg, NETMSG_MAXLEN-sizeof(NetMsg), 0);
		if(ret<=0)
		{			
			isQuit = 1;
			DBG("the peer has shutdown\n");
		}
		else
		{
			ackmsg->cmdValueLen = NETMSG_MAXLEN;

			ret = ProcMsgThread(msg,ackmsg);
			if(true == ret)
			{
				ret = send(sd, ackmsg, sizeof(NetMsg)+ackmsg->cmdValueLen, 0);
				if(ret<0)
				{
					DBG_ERR("error@%s in %d : send error!\n",__func__,__LINE__);	
				
					isQuit = 1;	
				}

				if(gRestart)
					isQuit = 1;
			}

			if(gSendDataFlag == true)
			{
				gSendDataFlag = false;
				
				if(SendFrame(&gInfo,(char *)(gInfo.udpIp),gInfo.udpPort)<0)
				{
					DBG_ERR("SendFrame\n");
				}
			
				EnvReleaseFrameImg(&gInfo);
			}
		}
	}

	free(msg);
	free(ackmsg);
	close(sd);
}

static int ServerMain()
{
	int server_sd,client_sd;
	struct sockaddr_in saddr;

	gRestart = 0;
	
	socklen_t saddr_len = sizeof(saddr);
	server_sd = create_server_socket(EZ_SERVERPORT,0);
	if(server_sd == -1)
	{
		DBG_ERR("error@%s in line %d:create_server_socket\n",__func__,__LINE__);
		return -1;
	}

	gFrameUdpSd = create_server_socket_udp(EZ_FRAMEPORT,5);
	if(gFrameUdpSd == -1)
	{
		DBG_ERR("error@%s in line %d:create_server_socket_udp\n",__func__,__LINE__);
		return -1;
	}
	
//	EzSysInfo prm;
	while(!gServerstop)
	{
		/** wait for connect */
		client_sd =  accept(server_sd,(struct sockaddr *)(&saddr),&saddr_len);
		if(client_sd<0)
		{
			DBG_ERR("error@%s in line %d:socket accept\n",__func__,__LINE__);
			gServerstop = 0;
		}
		
		Server_Proc(client_sd,saddr);

		if(!gRestart)
		{
			/** disconnect network */
//			fResetSysInfo();
//			SysGetSysInfo(&prm);
//			SysSetSysInfo(&prm);
		}
		else
		{
			gServerstop = 1;
			DBG_ERR("restart network ok\n");
		}
	}

	close(server_sd);	

	return 0;
}

static int smid;
static int NetShareMemInit(int key,unsigned int size)
{
	smid = shmget(key, size, IPC_CREAT | 0660);
	if(smid < 0)
		smid = shmget(key, 0, 0);

	if(smid < 0)
		return -1;
	return smid;
}

static void NetShareMemRead(int offset,void *buf, int length)
{
	char *pSrc = shmat(smid,0,0);
	memcpy(buf, pSrc + offset, length);
	shmdt(pSrc);
}

static pthread_t tNetMsgMainThread;

static void *NetMsgMainThread(void *arg)
{
	int qNetId;	
	int IsNetQuit = 0;
	int msg_size;
	NETSERVER_MSG_BUF msgbuf;
	
	gStreamStart = 0;
	
	/* File manager message queue initial */
	if((qNetId = Msg_Init(NETSERVER_KEY)) < 0){
		return NULL;;
	}
		
	while(IsNetQuit == 0)
	{
		msg_size = msgrcv( qNetId, &msgbuf, sizeof(msgbuf) - sizeof(long),MSG_TYPE_MSG1, 0);
		if( msg_size < 0 )
		{
			IsNetQuit = 1;
		}
		else
		{
			switch(msgbuf.cmd)
			{
				case NET_MSG_NETSERVER_START:
				{
					gStreamStart = 1;
					
					DBG("stream is ready\n");
					
					break;
				}
				default:
					break; 	
			}
			if(msgbuf.src != 0)
			{
				msgbuf.Des = msgbuf.src;
				msgbuf.src = MSG_TYPE_MSG1;
				msgsnd( qNetId,&msgbuf,sizeof(msgbuf)-sizeof(long),0);
			}
		}
	}

	return NULL;	
}
/**
 * @brief	setup environment
 * @param	none
 * @retval	-1: fail ; 0: success
 */
static int NetSetupEnv()
{
	NetShareMemInit(NETSTREAM_KEY,2048*1920);

	/* Create thread for file writeback */
	if(pthread_create(&tNetMsgMainThread, NULL, NetMsgMainThread, NULL)){
		return -1;
	}
	return 0;
}

/**
 * @brief	zcCamera server
 * @param	argc		argv[]
 * @retval	-1: fail ; 0: success
 */
void *NetServerThread(void *arg)
{	
	if(NetSetupEnv()!=0)
		pthread_exit((void *)1);
			
	ServerMain();
	
	RestartSystem();
	
	return (void *)0;
}
