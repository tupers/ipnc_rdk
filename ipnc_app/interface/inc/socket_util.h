#ifndef UTILS_SOCK_H
#define UTILS_SOCK_H


in_addr_t util_net_get_ifaddr(char *ifname);

in_addr_t util_net_get_ifaddr(char *ifname);

int util_net_set_ifaddr(char *ifname, in_addr_t addr);

in_addr_t util_net_get_netmask(char *ifname);

in_addr_t util_net_get_netmask(char *ifname);

int util_net_get_hwaddr(char *ifname, unsigned char *mac);

int util_Check_Link_speed( char *pInterface );

int util_set_tcp_socket_keepalive(int sd,int keepalive,int keepinterval,int keepidle,int keepcount);

int util_create_server_socket(int server_port,unsigned int timeout_s);

int util_create_server_socket_udp(int server_port,unsigned int timeout_s);

int util_udp_recv_data(int sd,char *buf,unsigned int size);

int util_udp_send_data(int sd,char *ip,unsigned int port,char *buf,unsigned int size);

#endif
