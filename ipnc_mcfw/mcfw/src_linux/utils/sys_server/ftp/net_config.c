/** ===========================================================================
* @file net_config.c
*
* @path $(IPNCPATH)\sys_server\src\
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "net_config.h"

#define NET_CONFIG_DEBUG

#ifdef NET_CONFIG_DEBUG
#define DBG_ERR(x...)		perror(x)
#define DBG_NET(x...)		printf(x)
#define DBG(x...)			printf(x)
#else
#define DBG_ERR(x...)
#define DBG_NET(x...)
#define DBG(x...)
#endif

int set_tcp_socket_keepalive(int sd,int keepalive,int keepinterval,int keepidle,int keepcount)
{
	if(keepalive)
	{
		if ((setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (void *) &keepalive,sizeof (keepalive))) == -1) 
		{
			DBG_ERR("setsockopt keepalive");
			return -1;
		}
	}
		
	if(keepidle>0)
	{
		if ((setsockopt(sd,SOL_TCP ,TCP_KEEPIDLE, (void *) &keepidle,sizeof(keepidle))) == -1) 
		{
			DBG_ERR("setsockopt keepidle\n");
			return -1;
		}	
	}


	if(keepinterval>0)
	{
		if ((setsockopt(sd, SOL_TCP,TCP_KEEPINTVL, (void *) &keepinterval,sizeof(keepinterval))) == -1) 
		{
			DBG_ERR("setsockopt keepinterval\n");
			return -1;
		}
	}

	if(keepcount>0)
	{
		if ((setsockopt(sd, SOL_TCP,TCP_KEEPCNT, (void *) &keepcount,sizeof(keepcount))) == -1) 
		{
			DBG_ERR("setsockopt keepcount\n");
			return -1;
		}
	}

	return 0;
	
}
int create_server_socket(int server_port,unsigned int timeout_s)
{
	int server_s;
	int sock_opt = 1;

	server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_s == -1) 
	{
		DBG_ERR("unable to create socket");

		return -1;
	}

	struct timeval tm;
	tm.tv_sec = timeout_s;
	tm.tv_usec = 0;
	
	if(timeout_s > 0)
	{
		setsockopt(server_s,SOL_SOCKET,SO_SNDTIMEO,&tm,sizeof(tm));
		setsockopt(server_s,SOL_SOCKET,SO_RCVTIMEO,&tm,sizeof(tm));
	}

	/* close server socket on exec so cgi's can't write to it */
	if (fcntl(server_s, F_SETFD, 1) == -1) {
		DBG_ERR("can't set close-on-exec on server socket!");
		close(server_s);
		return -1;
		}

	/* reuse socket addr */
	if ((setsockopt(server_s, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,sizeof (sock_opt))) == -1) 
	{
		DBG_ERR("setsockopt");
		close(server_s);
		return -1;
	}
	
	/* internet family-specific code encapsulated in bind_server()  */
	struct sockaddr_in server_sockaddr;
	memset(&server_sockaddr, 0, sizeof server_sockaddr);
			
	server_sockaddr.sin_family = AF_INET;
			
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			
	server_sockaddr.sin_port = htons(server_port);

    if(bind(server_s, (const struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr_in)))
	{
		printf("error@function %s in %d :unable to bind port[%d]\n",__func__,__LINE__,server_port);
		close(server_s);
		return -1;
	}

	return server_s;
}



static int bind_server_udp(int server_s,int server_port_udp)
{
	struct sockaddr_in server_sockaddr;
	memset(&server_sockaddr, 0, sizeof server_sockaddr);
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_sockaddr.sin_port = htons(server_port_udp);

	return bind(server_s, (struct sockaddr *) &server_sockaddr,sizeof (server_sockaddr));
}

int create_server_socket_udp(int server_port,unsigned int timeout_s)
{
	int server_udp;
	unsigned char one = 1;
	int sock_opt = 1;

	server_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_udp == -1) 
	{
		DBG_ERR("unable to create socket");
		return -1;
	}

	/* close server socket on exec so cgi's can't write to it */
	if (fcntl(server_udp, F_SETFD, 1) == -1) 
	{
		DBG_ERR("can't set close-on-exec on server socket!");

		close(server_udp);

		return -1;
	}

	/* reuse socket addr */
	if ((setsockopt(server_udp, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,sizeof (sock_opt))) == -1)
	{
		DBG_ERR("setsockopt");

		close(server_udp);

		return -1;
	}
	if ((setsockopt(server_udp, IPPROTO_IP, IP_MULTICAST_LOOP,&one, sizeof (unsigned char))) == -1)
	{
		DBG_ERR("setsockopt");

		close(server_udp);

		return -1;
	}

	struct timeval tm;
	tm.tv_sec = timeout_s;
	tm.tv_usec = 0;
	
	if(timeout_s > 0)
	{
		setsockopt(server_udp,SOL_SOCKET,SO_SNDTIMEO,&tm,sizeof(tm));
		setsockopt(server_udp,SOL_SOCKET,SO_RCVTIMEO,&tm,sizeof(tm));
	}
	if(server_port>0)
	{
		/* internet family-specific code encapsulated in bind_server()  */
		if (bind_server_udp(server_udp, server_port) == -1) 
		{
			DBG_ERR("unable to bind");
		
			close(server_udp);

			return -1;		
		}		
	}	


	return server_udp;
}

void delete_server_socket(int sd)
{
	close(sd);

	return ;
}

void delete_server_udp(int sd)
{
	close(sd);

	return ;
}
