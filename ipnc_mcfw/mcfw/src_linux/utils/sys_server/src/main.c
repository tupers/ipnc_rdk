/** ===========================================================================
* @file main.c
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <Msg_Def.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <system_control.h>
#include "share_mem.h"
#include <pthread.h>
#include <file_msg_drv.h>
#include <netserver_msg_drv.h>
#include <remote_debug_msg_drv.h>
#include <stream_env_ctrl.h>


#define FILE_THR_CREATED		0x1
#define NETSERVER_THR_CREATED	0x2

#ifdef DEBUG
#define DBG(fmt, args...)	printf("Debug " fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("Error " fmt, ##args)
static int IsSysQuit = 0;
extern void *FileMngThread(void *arg);
extern void *NetServerThread(void *arg);

/**
 * @brief	System message thread
 * @param	"int qid" : [IN]QID
 * @param	"int mid" : [IN]MID
 * @return	0
 */
int SysMsgThread(int qid,int mid)
{

	SYS_MSG_BUF msgbuf;

	

	while(IsSysQuit == 0){
		msgrcv( qid, &msgbuf, sizeof(msgbuf) - sizeof(long),
				MSG_TYPE_MSG1, 0);	
	}
	IsSysQuit = 1;

	return 0;
}


/**
 * @brief	main entry
 * @param	command line argument
 * @return	error code
 */
int main(int argc,char *argv[]) {
	int qid, mid,bThreadStatus = 0;
	struct shmid_ds buf;
	pthread_t tFileMngThread,tNetServerThread;
	
	/* System message queue initial */
	qid = Msg_Init(SYS_MSG_KEY);

	if(pthread_create(&tFileMngThread, NULL, FileMngThread, NULL)){
		IsSysQuit = 1;
		ERR("FileMngThread create fail\n");
	}
	bThreadStatus |= FILE_THR_CREATED;
	/* Create communication between file manager and system server */
	if(InitFileMsgDrv(FILE_MSG_KEY, FILE_SYS_MSG) != 0){
		if(bThreadStatus & FILE_THR_CREATED){
			SendFileQuitCmd();
			pthread_join (tFileMngThread, NULL);
		}
		ERR("File message driver init fail\n");
		return -1;
	}
	
	/****** create net server thread */
	if(pthread_create(&tNetServerThread, NULL, NetServerThread, NULL)){
		IsSysQuit = 1;
		ERR("NetServerThread create fail\n");
	}
	bThreadStatus |= NETSERVER_THR_CREATED;
	
	/* Create communication between file manager and system server */
	if(InitNetServerMsgDrv(NETSERVER_KEY, NETSERVER_SYS_MSG) != 0){
		if(bThreadStatus & NETSERVER_THR_CREATED){
			SendNetServerQuitCmd();
			pthread_join (tNetServerThread, NULL);
		}
		ERR("NetServer message driver init fail\n");
		return -1;
	}


	if(InitRemoteDebugMsgDrv(REMOTEDEBUG_KEY) != 0){
		ERR("InitRemoteDebugMsgDrv message driver init fail\n");
		return -1;
	}
	
	/* Share memory initial (for system server) */
	if((mid = ShareMemInit(SYS_MSG_KEY)) < 0){
		IsSysQuit = 1;
		ERR("Share memory init fail\n");
	}
	
	while(IsSysQuit == 0){
	
		SystemInit();
		
		IsSysQuit = 1;
	}
	
	DoBootProc(1);
	
	if(bThreadStatus & FILE_THR_CREATED){
//		SendFileQuitCmd();
		pthread_join (tFileMngThread, NULL);
	}

	if(bThreadStatus & NETSERVER_THR_CREATED){
//		SendNetServerQuitCmd();
		pthread_join(tNetServerThread, NULL);
	}

	SystemCleanup();
	if(mid >= 0)
		shmctl(mid, IPC_RMID, &buf);
	Msg_Kill(qid);

	return 0;
}


