#ifndef REMOTE_DEBUG_SOCKET_H
#define REMOTE_DEBUG_SOCKET_H




int RemoteDebug_Set_keepalive(int sd,int keepalive,int keepinterval,int keepidle,int keepcount);
int RemoteDebug_create_server_socket(int server_port);
int RemoteDebug_create_server_socket_udp(int server_port);




#endif
