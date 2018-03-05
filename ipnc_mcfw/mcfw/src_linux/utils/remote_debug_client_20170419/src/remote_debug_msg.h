#ifndef REMOTE_DEBUG_MSG_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define REMOTE_DEBUG_MSG_H


int RemoteDebug_Msg_Init( int msgKey );

int RemoteDebug_Msg_Send( int qid, void *pdata , int size );

int RemoteDebug_Msg_Rsv( int qid, void *pdata , int size ,int msg_type);

int RemoteDebug_Msg_Send_Rsv( int qid, int msg_type, void *pdata , int size );

int RemoteDebug_Msg_Kill( int qid );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* REMOTE_DEBUG_MSG_H */
