/** ===========================================================================
* @file file_mng_main.c
*
* @path $(IPNCPATH)\sys_adm\system_server
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
#include <Msg_Def.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "share_mem.h"
#include "file_mng.h"
#include "semaphore_util.h"
#include <sys_env_type.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <system_control.h>

#define FILE_WRITE_LOG      0x1
#define FILE_WRITE_SYS      0x2
#define FILE_WRITE_NETWORK	0x4
#define FILE_WRITE_ALGPRM	0x8
#ifdef DEBUG
#define DBG(fmt, args...)	printf("Debug " fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("Error " fmt, ##args)
#define NO_ERROR		    0
#define MSG_FAIL		    1
#define FILE_MNG_FAIL	    2
#define FILE_SEM_FAIL		3
#define FILE_THREAD_FAIL    4


void *pFileShareMem = NULL;
static int mFileId,qFileId;
static int IsFileQuit = 0,bNeedWrite = 0;
static pthread_t tProcFileThread;

static int CleanupFileEnv(int condition);

/**
 * @brief	check if file thread quit
 * @param	none
 * @retval	ret = IsFileQuit
 */
int IsFileThreadQuit()
{
	int ret;
	Sem_lock();
	ret = IsFileQuit;
	Sem_unlock();
	return ret;
}

/**
 * @brief	set file thread quit
 * @param	none
 * @retval	none
 */
static void SetFileQuit()
{
	Sem_lock();
	IsFileQuit = 1;
	Sem_unlock();
}

/**
 * @brief	check if file need write
 * @param	none
 * @retval	ret = IsFileQuit
 */
static int FileNeedWrite()
{
	int ret;
	Sem_lock();
	ret = bNeedWrite;
	Sem_unlock();
	return ret;
}

/**
 * @brief	set file to be updated
 * @param	none
 * @retval	none
 */
static void SetFileUpdate(int flag)
{
	Sem_lock();
	bNeedWrite |= flag;
	Sem_unlock();
}

/**
 * @brief	file has been updated
 * @param	none
 * @retval	none
 */
static void FileUpdated(int flag)
{
	Sem_lock();
	bNeedWrite &= ~flag;
	Sem_unlock();
}

/**
 * @brief	receive and process file message
 * @param	"FILE_MSG_BUF* pMsg"	[IN]buffer of message
 * @retval	1 : success
 */
static int ProcFileMsg(FILE_MSG_BUF* pMsg)
{
	int ret = 0;
	switch(pMsg->cmd){
		case FILE_MSG_QUIT:
		{
			SetFileQuit();
			pMsg->src = 0;
			break;
		}
		case FILE_GET_MID:
			pMsg->shmid = mFileId;
			ret = 1;
			break;
		case FILE_MSG_READ:
			if(pMsg->shmidx == 0){
				if(pMsg->shmid != mFileId){
					ERR("Error Share memory id\n");
					break;
				}
				pMsg->length = sizeof(SysInfo);
				ret = 1;
			} else if(pMsg->shmidx == 1){
				SysInfo *pSysInfo = pFileShareMem;
				LogEntry_t* pLogEntry;
				if(pMsg->shmid != mFileId){
					ERR("Error Share memory id\n");
					break;
				}
				if((pLogEntry = GetLog(pMsg->nPageNum, pMsg->nItemNum))
						== NULL)
					break;
				memcpy(&pSysInfo->tCurLog, pLogEntry, sizeof(LogEntry_t));
				pMsg->length = sizeof(SysInfo);
				ret = 1;
			}
			break;
		case FILE_MSG_WRITE:
			DBG("pMsg->shmidx=%d\n", pMsg->shmidx);

			pMsg->length = sizeof(SysInfo);
#if 0			
			if(pMsg->shmidx == 0){
				if(pMsg->shmid != mFileId){
					ERR("Error Share memory id\n");
					break;
				}

				SetFileUpdate(FILE_WRITE_SYS);

				pMsg->length = sizeof(SysInfo);
				ret = 1;
			} else if(pMsg->shmidx == 1){
				SysInfo *pSysInfo = pFileShareMem;
				if(pMsg->shmid != mFileId){
					ERR("Error Share memory id\n");
					break;
				}
				if(AddLog(&pSysInfo->tCurLog)){
					break;
				}

				SetFileUpdate(FILE_WRITE_LOG);

				pMsg->length = sizeof(SysInfo);
				ret = 1;
			}
#endif			
			break;
		case FILE_MSG_WRITE_I:
			DBG("pMsg->shmidx=%d\n", pMsg->shmidx);		
			if(pMsg->shmidx == 0){
				if(pMsg->shmid != mFileId){
					ERR("Error Share memory id\n");
					break;
				}

				SetFileUpdate(FILE_WRITE_SYS);

				pMsg->length = sizeof(SysInfo);
				ret = 1;
			} else if(pMsg->shmidx == 1){
				SysInfo *pSysInfo = pFileShareMem;
				if(pMsg->shmid != mFileId){
					ERR("Error Share memory id\n");
					break;
				}
				if(AddLog(&pSysInfo->tCurLog)){
					break;
				}

				SetFileUpdate(FILE_WRITE_LOG);

				pMsg->length = sizeof(SysInfo);
				ret = 1;
			}
			break;
#if 0		
			if(pMsg->shmid != mFileId){
				ERR("Error Share memory id\n");
				break;
			}
			FileUpdated(FILE_WRITE_LOG | FILE_WRITE_DMVA_LOG | FILE_WRITE_SYS);
			WriteGlobal(pFileShareMem);
			pMsg->length = sizeof(SysInfo);
			ret = 1;
			break;			
#endif			
		case FILE_MSG_WRITE_NETWORK:
			DBG("FILE_MSG_WRITE_NETWORK\n");
			int statu;	
			statu = UpdateNetwork(pFileShareMem);
			if(statu < 0)
				pMsg->length = 0;
			else
				pMsg->length = sizeof(SysInfo);		
			ret = 1;	
			break;
		case FILE_MSG_UPDATE_ALGFILE:
		{
			DBG("FILE_MSG_UPDATE_ALGFILE\n");			
			int statu;	
			SysInfo *pSysInfo = pFileShareMem;
			statu = WriteAlgParam(&(pSysInfo->algPrm));
			if(statu < 0)
				pMsg->length = 0;
			else
				pMsg->length = sizeof(SysInfo);		
			ret = 1;	
			break;
		}				
		case FILE_MSG_RESET:
			if(pMsg->shmid != mFileId){
				ERR("Error Share memory id\n");
				break;
			}
			FileMngReset(pFileShareMem);

			pMsg->length = sizeof(SysInfo);
			ret = 1;

			break;
		case FILE_MSG_CLEAR_SYSLOG:
			if(pMsg->shmid != mFileId){
				ERR("Error Share memory id\n");
				break;
			}
            ClearSystemLog();
            SetFileUpdate(FILE_WRITE_LOG);
			pMsg->length = sizeof(SysInfo);
			ret = 1;
		    break;
		case FILE_MSG_CLEAR_ACCESSLOG:
			if(pMsg->shmid != mFileId){
				ERR("Error Share memory id\n");
				break;
			}
            ClearAccessLog();
            SetFileUpdate(FILE_WRITE_LOG);
			pMsg->length = sizeof(SysInfo);
			ret = 1;
		    break;
		default:
			printf("Unknown message %d. Quit\n", pMsg->cmd);
			SetFileQuit();
			pMsg->src = 0;
			break;
	}
	return ret;
}

/**
 * @brief	thread processing file update
 * @param	"void *arg"
 * @retval	NULL
 */
static void *ProcFileThread(void *arg)
{
	int needWrite;

	while(IsFileThreadQuit() == 0)
	{
		needWrite = FileNeedWrite();

		if(needWrite != 0)
		{
			DBG("File write\n");

			if(needWrite & FILE_WRITE_LOG)
			{
			    FileUpdated(FILE_WRITE_LOG);
			    WriteGlobalLog(pFileShareMem);
			}

			if(needWrite & FILE_WRITE_SYS)
			{
			    FileUpdated(FILE_WRITE_SYS);
			    WriteGlobalSys(pFileShareMem);
			}
		}

		usleep(1000);
	}

	DBG("%s leave\n", __func__);
	return NULL;
}

/**
 * @brief	thread processing message
 * @param	none
 * @retval	0
 */
static int FileMsgThread()
{
	FILE_MSG_BUF msgbuf;
	int msg_size, e_count = 0;
	while(IsFileThreadQuit() == 0){
		msg_size = msgrcv( qFileId, &msgbuf, sizeof(msgbuf) - sizeof(long),
				MSG_TYPE_MSG1, 0);
		if( msg_size < 0 ){
			ERR("File Receive msg fail \n");
			if(e_count++ >= 3){
				/* Kill message queue and init angain */
				perror("msgrcv ERROR");
				Msg_Kill(qFileId);
				qFileId = Msg_Init(FILE_MSG_KEY);
			}
		}else if(msgbuf.src == MSG_TYPE_MSG1 || msgbuf.src < 0){
			ERR("Got Error message\n");
			SetFileQuit();
			break;
		} else {
			e_count = 0;
			if(ProcFileMsg(&msgbuf) == 1)
				/* Message has been processed */
				DBG("Message process success\n");
			if(msgbuf.src != 0){
				msgbuf.Des = msgbuf.src;
				msgbuf.src = MSG_TYPE_MSG1;
				msgsnd( qFileId,&msgbuf,sizeof(msgbuf)-sizeof(long),0);
			}
		}
	}
	return 0;
}

/**
 * @brief	setup environment
 * @param	none
 * @retval	-1: fail ; 0: success
 */
int SetupEnv()
{
	int ret;
	/* Share memory initial (for file manager) */
	mFileId = pShareMemInit(FILE_MSG_KEY);
	if(mFileId < 0)
		return -1;
	DBG("%s:mFileId=%d\n", __func__, mFileId);
	pFileShareMem = shmat(mFileId,0,0);
	DBG("%s:Share Memory Addr=%p\n", __func__, pFileShareMem);
	/* File manager message queue initial */
	if((qFileId = Msg_Init(FILE_MSG_KEY)) < 0){
		CleanupFileEnv(MSG_FAIL);
		return -1;
	}
	if((ret = FileMngInit(pFileShareMem)) != 0){
		CleanupFileEnv(FILE_MNG_FAIL);
		return -1;
	}
	if((ret = Sem_Creat()) != 0){
		CleanupFileEnv(FILE_SEM_FAIL);
		return -1;
	}
	/* Create thread for file writeback */
	if(pthread_create(&tProcFileThread, NULL, ProcFileThread, NULL)){
		ERR("ProcFileThread create fail\n");
		CleanupFileEnv(FILE_THREAD_FAIL);
		return -1;
	}
	return ret;
}

/**
 * @brief	clean up file environment
 * @param	"int condition" : condition
 * @retval	-1: fail ; 0: success
 */
static int CleanupFileEnv(int condition)
{
	int ret = 0, err = 1;
	struct shmid_ds buf;
	switch(condition){
		case NO_ERROR:
			pthread_join (tProcFileThread, NULL);
			err = 0;
			/* No break */
		case FILE_THREAD_FAIL:
			Sem_kill();
			/* No break */
		case FILE_SEM_FAIL:
			FileMngExit();
			/* No break */
		case FILE_MNG_FAIL:
			Msg_Kill(qFileId);
			/* No break */
		case MSG_FAIL:
			/* Share Memory unmap */
			shmdt(pFileShareMem);
			/* Kill Share Memory */
			shmctl(mFileId, IPC_RMID, &buf);
			break;
		default:
			ret = -1;
			break;
	}
	if(err)
		ERR("%s is called,condition = %d\n", __func__,condition);
	return ret;
}
/**
 * @brief	File manager thread
 * @param	"void *arg"
 * @retval	-1: fail ; 0: success
 */
void *FileMngThread(void *arg)
{
	int ret;
	ret = SetupEnv();
	if(ret == 0)
	{
		/* Processing all messages that was sent to file manager */
		ret = FileMsgThread();
		CleanupFileEnv(NO_ERROR);
	}
	return (void *)ret;
}
