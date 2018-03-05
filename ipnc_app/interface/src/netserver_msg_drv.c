#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sem_util.h>
#include <netserver_msg_drv.h>
#include <system_default.h>
#include <Msg_Def.h>
#include <sys_env_type.h>

#ifdef DEBUG
#define DBG(fmt, args...)	printf("FileMsgDrv: Debug\n" fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("FileMsgDrv: Error\n" fmt, ##args)

static int qid,gProcId, eCount = 0, gKey;
static SemHandl_t hndlDrvSem = NULL;

int SetNetServerInfo()
{
	NETSERVER_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = 0;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);

	return 0;
}
/**
* @brief Send quit command to file manager.

* This command will make file manager stop running. After you called this, all
*  the other process can't get system information because file manager is down.
*/
void SendNetServerQuitCmd()
{
	NETSERVER_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = 0;
	msgbuf.cmd = NET_MSG_QUIT;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	SemRelease(hndlDrvSem);
}

void SendNetServerStartCmd()
{
	NETSERVER_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = 0;
	msgbuf.cmd = NET_MSG_NETSERVER_START;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	SemRelease(hndlDrvSem);
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
int InitNetServerMsgDrv(int key,int iProcId)
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
	DBG("Qid: %d\n",qid);
	gKey = key;
	gProcId = iProcId;
	return 0;
}
/**
* @brief Cleanup file message driver.

* This API shoud be called at the end of process.
*/
void CleanupNetServerMsgDrv()
{
	DestroySem(hndlDrvSem);
}
