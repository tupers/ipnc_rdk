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
#include "NetCam_Scan.h"

#define SCAN_DEBUG			1
unsigned int calCrc(char *data,int len)
{
	int i;
	int crc = 0;
	for(i=0;i<len;i++)
		crc += data[i];

	return crc;	
}

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
