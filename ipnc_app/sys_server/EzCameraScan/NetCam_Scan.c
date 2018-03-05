#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "NetCam_Scan.h"

unsigned IsSameNet(char *srcIP,char *dstIP)
{
	char ips[4][6];
	char ipd[4][6];
	unsigned cnt = 0,scnt=0;
	int i;
	unsigned isSame = 1;
	
	for(i=0;i<4;i++)
	{
		memset(ips[i],0,6);
		memset(ipd[i],0,6);
	}
	
	while(*srcIP != 0)
	{
		if(*srcIP == '.')
		{
			cnt++;
			scnt=0;
		}	
		else
			ips[cnt][scnt++] = *srcIP;

		srcIP++; 		
	}

	cnt = 0;
	scnt=0;
	while(*dstIP != 0)
	{
		if(*dstIP == '.')
		{
			cnt++;
			scnt=0;
		}	
		else
			ipd[cnt][scnt++] = *dstIP;

		dstIP++; 		
	}
	
	for(i=0;i<3;i++)
	{
		if(strcmp(ips[i],ipd[i]) != 0)
		{
			isSame = 0;
			break;
		}
	}

	return isSame;
}  
void getLocalIP(char *ip)
{
	int inet_sock;  
	struct ifreq ifr;  

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);  
	strcpy(ifr.ifr_name, "eth0");  
	ioctl(inet_sock, SIOCGIFADDR, &ifr);  
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));  
}

int main(int argc,char *argv[])
{
	int brdcFd;
	int sock_opt = 1;
	NetCamScanSt msg;
	NetCamScanSt rcvbuf;
	struct sockaddr_in remoteAddr;
	struct sockaddr_in myAddr;
	struct sockaddr_in Recvaddr;
	int recvBytes;
	char localIP[32];
	char *remoteIP;
	socklen_t addrLen = sizeof(struct sockaddr_in);
					
	brdcFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (brdcFd == -1) 
	{
		return 0;
	}

	/* reuse socket addr */
	if ((setsockopt(brdcFd, SOL_SOCKET, SO_REUSEADDR|SO_BROADCAST, (void *) &sock_opt,sizeof (sock_opt))) == -1)
	{
		close(brdcFd);

		return 0;
	}
	
	memset(&myAddr,0,sizeof(struct sockaddr_in));
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	myAddr.sin_port = htons(NETCAMERA_SCAN_PORT);

	memset(&remoteAddr,0,sizeof(struct sockaddr_in));
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	
	if(bind(brdcFd,(struct sockaddr *)&myAddr,sizeof(struct sockaddr)) == -1)
	{
		close(brdcFd);

		return 0;
	}

	memset(localIP,0,32);
	memset(&msg,0,sizeof(msg));
	msg.magic[0] = SCAN_MAGIC1;	
	msg.magic[1] = SCAN_MAGIC2;
	msg.cmd = CMD_PUT_CAMERA;
	getLocalIP(localIP);
	strcpy(msg.ip,localIP);
	
	while(1)
	{
		recvBytes = recvfrom(brdcFd,&rcvbuf,sizeof(msg),0,(struct sockaddr *)&Recvaddr,&addrLen);
		
		if(recvBytes == sizeof(NetCamScanSt))
		{
			if((rcvbuf.magic[0] == SCAN_MAGIC1) && (rcvbuf.magic[1] == SCAN_MAGIC2)&&(rcvbuf.cmd == CMD_GET_CAMERA))
			{	
				printf("send data\n");
				
				msg.random = rcvbuf.random;

				memcpy(&remoteAddr,&Recvaddr,sizeof(remoteAddr));

				remoteIP = inet_ntoa(Recvaddr.sin_addr);
				if(!IsSameNet(remoteIP,localIP))
					remoteAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
					
				sendto(brdcFd, &msg, sizeof(msg), 0,(const struct sockaddr *)&remoteAddr, sizeof(remoteAddr));					
			}
		}
		else
			sleep(2);
		
	}
}
#if 0
int main(int argc,char *argv[])
{
	int brdcFd;
	int sock_opt = 1;
	NetCamScanData msg;
	NetCamScanData *recvbuf;
	struct sockaddr_in remoteAddr;
	struct sockaddr_in myAddr;
	int len;
	int recvBytes;
	unsigned int crc;
		
	socklen_t addrLen = sizeof(struct sockaddr_in);

					
	brdcFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (brdcFd == -1) 
	{
		return 0;
	}

	/* reuse socket addr */
	if ((setsockopt(brdcFd, SOL_SOCKET, SO_REUSEADDR|SO_BROADCAST, (void *) &sock_opt,sizeof (sock_opt))) == -1)
	{
		close(brdcFd);

		return 0;
	}
	
	memset(&myAddr,0,sizeof(struct sockaddr_in));
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	myAddr.sin_port = htons(NETCAMERA_SCAN_PORT);

	if(bind(brdcFd,(struct sockaddr *)&myAddr,sizeof(struct sockaddr)) == -1)
	{
		close(brdcFd);

		return 0;
	}

	len = sizeof(NetCamScanData) + 100;
	recvbuf = (NetCamScanData *)malloc(len);
	if(NULL == recvbuf)
	{
		close(brdcFd);

		return 0;
	}
	

	memset(&msg,0,sizeof(NetCamScanData));
	len = ((sizeof(msg.identical) - 1) > strlen(IDENTICAL_STRING))?strlen(IDENTICAL_STRING):(sizeof(msg.identical) - 1);
	msg.maigc[0] = SCAN_MAGIC1;
	msg.maigc[1] = SCAN_MAGIC2;
	memcpy(msg.identical,IDENTICAL_STRING,strlen(IDENTICAL_STRING));
	msg.CamId = 0;
	msg.crc = calCrc((char *)&msg,sizeof(NetCamScanData) - sizeof(msg.crc));

	len = sizeof(NetCamScanData) + 100;
#ifdef SCAN_DEBUG
	printf("start send camera information\n");
#endif		
		
	while(1)
	{
		recvBytes = recvfrom(brdcFd,recvbuf,len,0,(struct sockaddr *)&remoteAddr,&addrLen);
	
		if(recvBytes == sizeof(NetCamScanData))
		{
			crc = calCrc((char *)recvbuf,sizeof(NetCamScanData) - sizeof(recvbuf->crc));
	
			if((recvbuf->maigc[0] == SCAN_MAGIC1) && (recvbuf->maigc[1] == SCAN_MAGIC2) && (crc == recvbuf->crc))
			{
				msg.CamId = 1;	
				msg.crc = calCrc((char *)&msg,sizeof(NetCamScanData) - sizeof(msg.crc));							
				sendto(brdcFd, &msg, sizeof(NetCamScanData), 0,(const struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));	
			}
		}
		else
			sleep(5);
		
	}

}
#endif
