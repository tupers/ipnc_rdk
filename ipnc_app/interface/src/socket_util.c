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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <unistd.h>
#include <signal.h>
#include <asm/types.h>
#include <linux/sockios.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "socket_util.h"


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

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
static struct sockaddr_in sa = {
	sin_family:	PF_INET,
	sin_port:	0
};
/**
* @brief Davinci emac private control type.
*/
typedef struct {
	unsigned int cmd;
	void *data;
} emac_drv_priv_ioctl;
/**
* @brief Mii data.
*/
struct mii_data
{
	unsigned short phy_id;
	unsigned short reg_num;
	unsigned short val_in;
	unsigned short val_out;
};
/**
 * @brief	get address of an interface
 * @param	"char *ifname" : interface name
 * @retval	net address
 */
in_addr_t util_net_get_ifaddr(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		DBG_ERR("net_get_ifaddr: ioctl SIOCGIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);

	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}

/**
 * @brief	get ip of an interface
 * @param	"int skfd" :
 * @param	"char *ifname" : interface name
 * @retval	ip
 */
struct in_addr util_net_get_ip(int skfd, char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		DBG_ERR("net_get_ip: ioctl SIOCGIFADDR");
		return (struct in_addr){-1};
	}
	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
}

/**
 * @brief	set ip of an interface
 * @param	"char *ifname" : interface name
 * @param	"in_addr_t addr" : ip address
 * @retval	0 : success ; -1 : fail
 */
int util_net_set_ifaddr(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFADDR, &ifr) < 0) {
		DBG_ERR("net_set_ifaddr: ioctl SIOCSIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);
	return 0;
}

/**
 * @brief	get address of an interface
 * @param	"char *ifname" : interface name
 * @retval	address
 */
in_addr_t util_net_get_netmask(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		DBG_ERR("net_get_netmask: ioctl SIOCGIFNETMASK");
		close(skfd);
		return -1;
	}
	close(skfd);

	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}

/**
 * @brief	get netmask of an interface
 * @param	"char *ifname" : [IN]interface name
 * @param	"in_addr_t addr" : [OUT]netmask
 * @retval	0 : success ; -1 : fail
 */
int util_net_set_netmask(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0) {
		DBG_ERR("net_set_netmask: ioctl SIOCSIFNETMASK");
		close(skfd);
		return -1;
	}
	close(skfd);
	return 0;
}

/**
 * @brief	get mac address of an interface
 * @param	"char *ifname" : [IN]interface name
 * @param	"unsigned char *mac" : [OUT]mac address
 * @retval	0 : success ; -1 : fail
 */
int util_net_get_hwaddr(char *ifname, unsigned char *mac)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		DBG_ERR("net_get_hwaddr: ioctl SIOCGIFHWADDR");
		close(skfd);
		return -1;
	}
	close(skfd);

	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	return 0;
}


/**
* @brief check ethernet speed
*
* @param pInterface [I] ethernt device name. Ex: "eth0" "eth1"
* @return Ethernet speed
* @retval 0  10M
* @reval 1  100M
* @retval -1  Error
*/
int util_Check_Link_speed( char *pInterface )
{
	int skfd = 0;
	struct ifreq ifr;
	struct mii_data* mii = NULL;

	if( ( skfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
			perror( "socket" );
			return -1;
	}


	bzero( &ifr, sizeof( ifr ) );
	strncpy( ifr.ifr_name, pInterface, IFNAMSIZ - 1 );
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
	if( ioctl( skfd, SIOCGMIIPHY, &ifr ) < 0 )
	{
			perror( "ioctl" );
			return -1;
	}

	mii = (struct mii_data*)&ifr.ifr_data;
	mii->reg_num = 0; //sPHY_register 0
	if( ioctl( skfd, SIOCGMIIREG, &ifr ) < 0 )
	{
		  perror( "ioctl2" );
		  return -1;
	}
	close( skfd );
	if( mii->val_out & (1<<13) ) //scheck bit13 of PHY_REG0
	{
		//printf( "100M\n" );
		return 1;
	}
	else
	{
		//printf( "10M\n" );
		return 0;
	}

}

int util_set_tcp_socket_keepalive(int sd,int keepalive,int keepinterval,int keepidle,int keepcount)
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
int util_create_server_socket(int server_port,unsigned int timeout_s)
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
#ifdef SOCKET_TCP_NONBLOCKING
	/* server socket is nonblocking */
	if (set_nonblock_fd(server_s) == -1) {
		DBG_ERR("fcntl: unable to set server socket to nonblocking");
		close(server_s);
		return -1;
		}
#endif
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


	/* listen: large number just in case your kernel is nicely tweaked */
	if (listen(server_s, 1) == -1) 
	{
		DBG_ERR("unable to listen");
		close(server_s);
		return -1;
	}

	return server_s;
}

static int util_bind_server_udp(int server_s,int server_port_udp)
{
	struct sockaddr_in server_sockaddr;
	memset(&server_sockaddr, 0, sizeof server_sockaddr);
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_sockaddr.sin_port = htons(server_port_udp);

	return bind(server_s, (struct sockaddr *) &server_sockaddr,sizeof (server_sockaddr));
}

int util_create_server_socket_udp(int server_port,unsigned int timeout_s)
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

#ifdef SOCKET_UDP_NONBLOCK
	/* server socket is nonblocking */
	if (set_nonblock_fd(server_udp) == -1) 
	{
		DBG_ERR("fcntl: unable to set server socket to nonblocking");
		
		close(server_udp);

		return -1;
	}
#endif
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
		if (util_bind_server_udp(server_udp, server_port) == -1) 
		{
			DBG_ERR("unable to bind");
		
			close(server_udp);

			return -1;		
		}		
	}	


	return server_udp;
}

int util_udp_recv_data(int sd,char *buf,unsigned int size)
{
	ssize_t ret;
	struct sockaddr remote_addr;
	socklen_t len = sizeof(struct sockaddr);
	
	ret = recvfrom(sd, buf, size, 0,&remote_addr, &len);

	return ret;
}

int util_udp_send_data(int sd,char *ip,unsigned int port,char *buf,unsigned int size)
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
