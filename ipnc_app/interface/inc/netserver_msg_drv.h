#ifndef _NETSERVER_MSG_DRV_H_
#define _NETSERVER_MSG_DRV_H_

void SendNetServerStartCmd();

void SendNetServerQuitCmd();

int InitNetServerMsgDrv(int key,int iProcId);

void CleanupNetServerMsgDrv();




#endif /* _NETSERVER_MSG_DRV_H_ */
