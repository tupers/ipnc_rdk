#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/sockios.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "remote_debug_socket.h"

int RemoteDebug_Set_keepalive(int sd,int keepalive,int keepinterval,int keepidle,int keepcount)
{
	if(keepalive)
	{
		if ((setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (void *) &keepalive,sizeof (keepalive))) == -1) 
		{
			printf("setsockopt keepalive");
			return -1;
		}
	}
		
	if(keepidle>0)
	{
		if ((setsockopt(sd,SOL_TCP ,TCP_KEEPIDLE, (void *) &keepidle,sizeof(keepidle))) == -1) 
		{
			printf("setsockopt keepidle\n");
			return -1;
		}	
	}


	if(keepinterval>0)
	{
		if ((setsockopt(sd, SOL_TCP,TCP_KEEPINTVL, (void *) &keepinterval,sizeof(keepinterval))) == -1) 
		{
			printf("setsockopt keepinterval\n");
			return -1;
		}
	}

	if(keepcount>0)
	{
		if ((setsockopt(sd, SOL_TCP,TCP_KEEPCNT, (void *) &keepcount,sizeof(keepcount))) == -1) 
		{
			printf("setsockopt keepcount\n");
			return -1;
		}
	}

	return 0;
	
}
int RemoteDebug_create_server_socket(int server_port)
{
	int server_s;
	int sock_opt = 1;

	server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_s == -1) 
	{
		printf("unable to create socket");

		return -1;
	}
#ifdef SOCKET_TCP_NONBLOCKING
	/* server socket is nonblocking */
	if (set_nonblock_fd(server_s) == -1) {
		printf("fcntl: unable to set server socket to nonblocking");
		close(server_s);
		return -1;
		}
#endif
	/* close server socket on exec so cgi's can't write to it */
	if (fcntl(server_s, F_SETFD, 1) == -1) {
		printf("can't set close-on-exec on server socket!");
		close(server_s);
		return -1;
		}

	/* reuse socket addr */
	if ((setsockopt(server_s, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,sizeof (sock_opt))) == -1) 
	{
		printf("setsockopt");
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


	/* listen: large number just in case your kernel is nicely tweaked */
	if (listen(server_s, 5) == -1) 
	{
		printf("unable to listen");
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

int RemoteDebug_create_server_socket_udp(int server_port)
{
	int server_udp;
	unsigned char one = 1;
	int sock_opt = 1;

	server_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_udp == -1) 
	{
		printf("unable to create socket");
		return -1;
	}

#ifdef SOCKET_UDP_NONBLOCK
	/* server socket is nonblocking */
	if (set_nonblock_fd(server_udp) == -1) 
	{
		printf("fcntl: unable to set server socket to nonblocking");
		
		close(server_udp);

		return -1;
	}
#endif
	/* close server socket on exec so cgi's can't write to it */
	if (fcntl(server_udp, F_SETFD, 1) == -1) 
	{
		printf("can't set close-on-exec on server socket!");

		close(server_udp);

		return -1;
	}

	/* reuse socket addr */
	if ((setsockopt(server_udp, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,sizeof (sock_opt))) == -1)
	{
		printf("setsockopt");

		close(server_udp);

		return -1;
	}
	if ((setsockopt(server_udp, IPPROTO_IP, IP_MULTICAST_LOOP,&one, sizeof (unsigned char))) == -1)
	{
		printf("setsockopt");

		close(server_udp);

		return -1;
	}

	if(server_port>0)
	{
		/* internet family-specific code encapsulated in bind_server()  */
		if (bind_server_udp(server_udp, server_port) == -1) 
		{
			printf("unable to bind");
		
			close(server_udp);

			return -1;		
		}		
	}	


	return server_udp;
}

int Remote_debug_recv_data(int sd,char *buf,unsigned int size)
{
	ssize_t ret;
	struct sockaddr remote_addr;
	socklen_t len = sizeof(struct sockaddr);
	
	ret = recvfrom(sd, buf, size, 0,&remote_addr, &len);

	return ret;
}

int Remote_debug_send_data(int sd,char *ip,unsigned int port,char *buf,unsigned int size)
{
	ssize_t ret;
	struct sockaddr_in dest_addr;
	socklen_t len = sizeof(struct sockaddr_in);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = inet_addr(ip);
	bzero(&(dest_addr.sin_zero), 8);
	
	ret = sendto(sd, buf, size, 0,(struct sockaddr *)&dest_addr, len);

	return ret;
}
