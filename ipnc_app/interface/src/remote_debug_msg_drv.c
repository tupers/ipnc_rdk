#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sem_util.h>
#include <system_default.h>
#include <remote_debug_msg_drv.h>
#include <Msg_Def.h>
#include <sys_env_type.h>
#include <share_mem.h>
#include "socket_util.h"

#ifdef DEBUG
#define DBG(fmt, args...)	printf("Debug\n" fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("Error\n" fmt, ##args)

static SemHandl_t hndlDrvSem = NULL;
REMOTEDEBUG_SOCKMSG_BUF *remote_buf = NULL;
char *arg_buf = NULL;

static void SendRemoteMsg(char *msgbuf,unsigned int size);
#if 0
static int qid = 0;
static int mid = 0;
static int gKey = 0;

/**
* @brief Send quit command to file manager.

* This command will make file manager stop running. After you called this, all
*  the other process can't get system information because file manager is down.
*/
void SendRemoteDebugQuitCmd()
{
	REMOTEDEBUG_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = MSG_TYPE_MSG2;
	msgbuf.cmd = REMOTEDEBUG_MSG_QUIT;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), MSG_TYPE_MSG2, 0);
	SemRelease(hndlDrvSem);	
}

void SendRemoteDebugSerialCmd()
{
	REMOTEDEBUG_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = MSG_TYPE_MSG2;
	msgbuf.cmd = REMOTEDEBUG_MSG_SERIAL;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), MSG_TYPE_MSG2, 0);
	SemRelease(hndlDrvSem);		
}

int SendRemoteDebugSocketCmd(unsigned int port,char *ip)
{
	REMOTEDEBUG_MSG_BUF msgbuf;

	int len = strlen(ip);
	if(len>=20)
		return -1; 
	
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = MSG_TYPE_MSG2;
	msgbuf.cmd = REMOTEDEBUG_MSG_SOCKET;
	msgbuf.port = port;
	memcpy(msgbuf.ip,ip,len);
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), MSG_TYPE_MSG2, 0);
	SemRelease(hndlDrvSem);

	return 0;		
}

void SendRemoteDebugNONECmd()
{
	REMOTEDEBUG_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = MSG_TYPE_MSG2;
	msgbuf.cmd = REMOTEDEBUG_MSG_NONE;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), MSG_TYPE_MSG2, 0);
	SemRelease(hndlDrvSem);		
}

void SendRemoteNewInfo(unsigned int size)
{
	REMOTEDEBUG_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = MSG_TYPE_MSG2;
	msgbuf.cmd = REMOTEDEBUG_MSG_DEBUGINFO;
	msgbuf.shmsize = size;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), MSG_TYPE_MSG2, 0);
	SemRelease(hndlDrvSem);		
}

int Remote_printf(char *format, ...)
{
    va_list vaArgPtr;
    unsigned int len;

	SemWait(hndlDrvSem);
    va_start(vaArgPtr, format);
    vsnprintf(remote_buf, 1024, format, vaArgPtr);
    va_end(vaArgPtr);

	len = strlen(remote_buf);

	pShareMemWrite(mid,0,remote_buf, len);
	SemRelease(hndlDrvSem);	
	
	SendRemoteNewInfo(len);
	
    return 0;
}
/**
* @brief Initialize file message driver.

* Initialize file message driver. Please see \ref FILE_MSG_DRV_HOW to learn more.
* @note This API must be used before use any other file message driver API.
* @param key [I ] Key number for message queue and share memory.
* @param iProcId [I ] Message ID(Which was define in \ref File_Msg_Def.h) to initialize file message driver.
* @retval 0 Success.
* @retval -1 Fail.
*/
int InitRemoteDebugMsgDrv(int key)
{
	if(hndlDrvSem == NULL)
		hndlDrvSem = MakeSem();
	if(hndlDrvSem == NULL){
		ERR("Semaphore create fail!!!\n");
		return -1;
	}
	DBG("Semaphore Addr: %p\n", hndlDrvSem);
	if((qid = Msg_Init(key)) < 0){
		ERR("Message init fail\n");
		DestroySem(hndlDrvSem);
		return -1;
	}

	if((mid = pShareMemInit(key)) < 0)
	{
		ERR("share memory init fail\n");
		DestroySem(hndlDrvSem);
		return -1;		
	}

	remote_buf = (char *)malloc(2048);
	if(remote_buf == NULL)
	{
		ERR("share memory init fail\n");
		DestroySem(hndlDrvSem);
		return -1;				
	}
	DBG("Qid: %d,Mid: %d\n",qid,mid);
	gKey = key;
	return 0;
}
/**
* @brief Cleanup file message driver.

* This API shoud be called at the end of process.
*/
void CleanupRemoteDebugMsgDrv()
{
	DestroySem(hndlDrvSem);
}
#else
static int sd = -1;
/**
* @brief Send quit command to file manager.

* This command will make file manager stop running. After you called this, all
*  the other process can't get system information because file manager is down.
*/
void SendRemoteDebugQuitCmd()
{
	REMOTEDEBUG_SOCKMSG_BUF *msgbuf;
	
	SemWait(hndlDrvSem);
	msgbuf = remote_buf;
	msgbuf->cmd = REMOTEDEBUG_MSG_QUIT;
	msgbuf->dataBytes = 0;
	SendRemoteMsg((char *)msgbuf,sizeof(REMOTEDEBUG_SOCKMSG_BUF));
	SemRelease(hndlDrvSem);	
}

void SendRemoteDebugSerialCmd()
{
	REMOTEDEBUG_SOCKMSG_BUF *msgbuf;
	
	SemWait(hndlDrvSem);
	msgbuf = remote_buf;
	msgbuf->cmd = REMOTEDEBUG_MSG_SERIAL;
	msgbuf->dataBytes = 0;
	SendRemoteMsg((char *)msgbuf,sizeof(REMOTEDEBUG_SOCKMSG_BUF));
	SemRelease(hndlDrvSem);
}

int SendRemoteDebugSocketCmd(unsigned int port,char *ip)
{
	REMOTEDEBUG_SOCKMSG_BUF *msgbuf;
	REMOTEDEBUG_SOCKINFO_BUF *sockinfo;
	int len = strlen(ip);
	if(len>=20)
		return -1; 
	SemWait(hndlDrvSem);
	msgbuf = remote_buf;
	sockinfo = (REMOTEDEBUG_SOCKINFO_BUF *)(msgbuf->data);
	msgbuf->cmd = REMOTEDEBUG_MSG_SOCKET;
	msgbuf->dataBytes = sizeof(REMOTEDEBUG_SOCKINFO_BUF);
	sockinfo->port = port;
	memset(sockinfo->ip,0,20);

	memcpy(sockinfo->ip,ip,len);
	SendRemoteMsg((char *)msgbuf,sizeof(REMOTEDEBUG_SOCKMSG_BUF));
	SemRelease(hndlDrvSem);

	return 0;		
}

void SendRemoteDebugNONECmd()
{
	REMOTEDEBUG_SOCKMSG_BUF *msgbuf;
	
	SemWait(hndlDrvSem);
	msgbuf = remote_buf;
	msgbuf->cmd = REMOTEDEBUG_MSG_NONE;
	msgbuf->dataBytes = 0;
	SendRemoteMsg((char *)msgbuf,sizeof(REMOTEDEBUG_SOCKMSG_BUF));
	SemRelease(hndlDrvSem);		
}

static void SendRemoteMsg(char *msgbuf,unsigned int size)
{
	int ret;

	while(size>0)
	{
		ret = util_udp_send_data(sd,"127.0.0.1",REMOTEDEBUG_PORT,msgbuf,size);
		if(ret < 0)
		{
			ERR("util_udp_send_data fail\n");
			break;
		}
		else
			size -= ret;
	}	
}

int Remote_printf(char *format, ...)
{
    va_list vaArgPtr;
    unsigned int len;

	SemWait(hndlDrvSem);
    va_start(vaArgPtr, format);
    vsnprintf(arg_buf, 1024, format, vaArgPtr);
    va_end(vaArgPtr);

	len = strlen(arg_buf);

	if(len < REMOTE_DEBUG_BUF_SIZE)
	{
		memcpy(remote_buf->data,arg_buf,len);
		remote_buf->data[len] = 0;
		remote_buf->cmd = REMOTEDEBUG_MSG_DEBUGINFO;
		remote_buf->dataBytes = len + 1;

		SendRemoteMsg((char *)remote_buf,sizeof(REMOTEDEBUG_SOCKMSG_BUF));
	}	

	SemRelease(hndlDrvSem);	
	
    return 0;
}
/**
* @brief Initialize file message driver.

* Initialize file message driver. Please see \ref FILE_MSG_DRV_HOW to learn more.
* @note This API must be used before use any other file message driver API.
* @param key [I ] Key number for message queue and share memory.
* @param iProcId [I ] Message ID(Which was define in \ref File_Msg_Def.h) to initialize file message driver.
* @retval 0 Success.
* @retval -1 Fail.
*/
int InitRemoteDebugMsgDrv(int key)
{
	if(hndlDrvSem == NULL)
		hndlDrvSem = MakeSem();
	if(hndlDrvSem == NULL){
		ERR("Semaphore create fail!!!\n");
		return -1;
	}
	DBG("Semaphore Addr: %p\n", hndlDrvSem);

	if((sd = util_create_server_socket_udp(0,0)) < 0){
		ERR("util_create_server_socket_udp fail\n");
		DestroySem(hndlDrvSem);
		return -1;
	}

	remote_buf = (REMOTEDEBUG_SOCKMSG_BUF *)malloc(sizeof(REMOTEDEBUG_SOCKMSG_BUF));
	if(remote_buf == NULL)
	{
		ERR("malloc fail\n");
		DestroySem(hndlDrvSem);
		return -1;				
	}

	arg_buf = (char *)malloc(2048);
	if(arg_buf == NULL)
	{
		ERR("malloc fail\n");
		DestroySem(hndlDrvSem);
		free(remote_buf);
		return -1;				
	}

	return 0;
}
/**
* @brief Cleanup file message driver.

* This API shoud be called at the end of process.
*/
void CleanupRemoteDebugMsgDrv()
{
	DestroySem(hndlDrvSem);
	free(arg_buf);
	free(remote_buf);
}
#endif
