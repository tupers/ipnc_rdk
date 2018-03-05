#ifndef REMOTE_DEBUG_MSG_DRV_H
#ifdef __cplusplus
extern "C" {
#endif /* cplusplus */
#define REMOTE_DEBUG_MSG_DRV_H


int InitRemoteDebugMsgDrv(int key);

void CleanupRemoteDebugMsgDrv();

void SendRemoteDebugNONECmd();

int SendRemoteDebugSocketCmd(unsigned int port,char *ip);

void SendRemoteDebugSerialCmd();

void SendRemoteDebugQuitCmd();

int Remote_printf(char *format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* REMOTE_DEBUG_MSG_DRV_H */
