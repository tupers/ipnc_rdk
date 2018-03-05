#ifndef OSA_SOCKET_H
#define OSA_SOCKET_H


int OSA_Set_keepalive(int sd,int keepalive,int keepinterval,int keepidle,int keepcount);
int OSA_create_server_socket(int server_port);
int OSA_create_server_socket_udp(int server_port);


#endif
