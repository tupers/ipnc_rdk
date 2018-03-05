/* ===========================================================================
* @path $(IPNCPATH)\sys_server\inc\
*
* @desc 
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file net_config.h
* @brief Functions about network settings.
*/

#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

/**
* @brief Structure to save network status
*/
struct NET_CONFIG {
	unsigned char mac[6];
	in_addr_t	ifaddr;
	in_addr_t	netmask;
	in_addr_t	gateway;
	in_addr_t	dns;
};


int create_server_socket(int server_port,unsigned int timeout_s);
int create_server_socket_udp(int server_port,unsigned int timeout_s);
int set_tcp_socket_keepalive(int sd,int keepalive,int keepinterval,int keepidle,int keepcount);
void delete_server_socket(int sd);
void delete_server_udp(int sd);
int udp_recv_data(int sd,char *buf,unsigned int size);
int udp_send_data(int sd,char *ip,unsigned int port,char *buf,unsigned int size);
#endif
