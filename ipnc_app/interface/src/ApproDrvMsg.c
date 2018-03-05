/* ===========================================================================
* @file ApproDrvMsg.c
*
* @path $(IPNCPATH)/util/
*
* @desc Message driver for Appro_interface
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ApproDrvMsg.h>
#include <cmem.h>
#include "sem_util.h"
#include "share_mem.h"

static int gProcId = MSG_TYPE_MSG1;
static int qid;
static int mid;
static SemHandl_t hndlApproDrvSem = NULL;

#define PROC_MSG_ID gProcId

#define ENABLE_CMEM (1)

int gSentCmd;

/**
 * @brief Message Send Wrapper function
 *
 *
 * @param   msqid    Message qid
 *
 *
 *
 */
int MSGSND(int msqid, const void *msgp, size_t msgsz, int msgflg)
{
	int ret;
	MSG_BUF *pMsgBuf = (MSG_BUF *)msgp;
	
	ret = msgsnd(msqid,msgp,msgsz,msgflg);
	
	/* On success msgsnd returns 0 */
	if(ret == 0)
	{
		if(pMsgBuf->Src != 0)
		{
			gSentCmd = pMsgBuf->cmd;
		}
	}
	
	return ret;
}
 
 /**
 * @brief Message Receive Wrapper function
 *
 *
 * @param   * @param   msqid    Message qid
 *
 *
 *
 */
ssize_t MSGRCV(int msqid, void *msgp, size_t msgsz, long msgtyp,
               int msgflg)
{
	int ret;
	MSG_BUF *pMsgBuf = msgp;
	
	ret = msgrcv(msqid,msgp,msgsz,msgtyp,msgflg);
	
	/* On success msgrcv return the no of bytes received */
	if(ret >= 0)
	{
		if(pMsgBuf->Src != 0)
		{
			if(pMsgBuf->cmd != gSentCmd)
			{
				fprintf(stderr,"WARNING: Ack received for cmd(%d) is not for the sent cmd(%d)\n",
				        pMsgBuf->cmd,gSentCmd);
			}
		}
	}
	
	return ret;
}			   
 
/**
 * @brief set the message type id of the process
 *
 *
 * @param   proc_id    message type id of the proceess, defined at ipnc_app/include/Stream_Msg_Def.h
 *
 *
 *
 */
void ApproDrv_SetProcId(int proc_id)
{
    if(proc_id < MSG_TYPE_MSG1 || proc_id >= MSG_TYPE_END){
        gProcId = MSG_TYPE_MSG1;
        return;
    } else
        gProcId = proc_id;
}

/**
 * @brief set the message queue id of the process
 *
 *
 * @param   iqid    message queue id of the proceess
 *
 *
 *
 */
void ApproDrv_SaveQid(int iqid)
{
    qid = iqid;
}

int Testflag = 0;
/**
 * @brief Initiliztion of the message driver for Appro interface
 *
 *
 * @param   proc_id    message type id of the proceess, defined at ipnc_app/include/Stream_Msg_Def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */

int ApproDrvInit(int proc_id)
{
    if(proc_id < MSG_TYPE_MSG1 || proc_id > 20){
        gProcId = MSG_TYPE_MSG1;
        return -1;
    }
    gProcId = proc_id;
    fprintf(stderr, "%s: %d\n", __func__, proc_id);
#if ENABLE_CMEM
    /* CMEM only one init is allowed in each process */
    if(Testflag==0)
    {
        if(CMEM_init() < 0){
            gProcId = MSG_TYPE_MSG1;
            return -1;
        }
    }
#endif
    Testflag = 1;
    if(hndlApproDrvSem == NULL)
        hndlApproDrvSem = MakeSem();
    if(hndlApproDrvSem == NULL){
#if ENABLE_CMEM
        CMEM_exit();
#endif
        gProcId = MSG_TYPE_MSG1;
        return -1;
    }
    if((qid=Msg_Init(MSG_KEY)) < 0){
        DestroySem(hndlApproDrvSem);
        hndlApproDrvSem = NULL;
#if ENABLE_CMEM
        CMEM_exit();
#endif
        gProcId = MSG_TYPE_MSG1;
        return -1;
    }

	if((mid = pShareMemInit(MSG_KEY,MSG_SHMEM_MAXSIZE)) < 0){
		DestroySem(hndlApproDrvSem);
		hndlApproDrvSem = NULL;
#if ENABLE_CMEM
        CMEM_exit();
#endif		
		Msg_Kill(qid);
	}
 
    return 0;
}
/**
 * @brief Resource releasing of the message driver for Appro interface
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int ApproDrvExit()
{
    fprintf(stderr, "%s: %d\n", __func__, gProcId);
    gProcId = MSG_TYPE_MSG1;
    DestroySem(hndlApproDrvSem);
    hndlApproDrvSem = NULL;
#if ENABLE_CMEM

    if( Testflag != 0 )
    {
        CMEM_exit();
    }
    Testflag = 0;;
    return 0;
#else
    return 0;
#endif
}


int ResetPayload()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_RESET_PAYLOAD;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return 0;	
}

int StartPayload()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_START_PAYLOAD;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return 0;	
}


int PrintPayload()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_PRINT_PAYLOAD;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return 0;	
}


int GetPayload(EzCpuDynamicParam *prf)
{
    MSG_BUF msgbuf;
	
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_GET_PAYLOAD;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    	
    if(msgbuf.ret != 0)
        return -1;     
	memcpy(prf,&(msgbuf.prf),sizeof(EzCpuDynamicParam));

    

    return 0;	
}

int GetAlgResult(Alg_Result *result)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_ALG_RESULT;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);

    if((msgbuf.ret != 0)||(msgbuf.Shmem.offset<0))
    	memset(result,0,sizeof(Alg_Result));
    else	
    	pShareMemRead(mid,msgbuf.Shmem.offset,result, msgbuf.Shmem.size);
    SemRelease(hndlApproDrvSem);      
    
    return msgbuf.ret;		
}

int SaveFrameImg(char *filename)
{
    MSG_BUF msgbuf;
	unsigned int len;

	if(NULL == filename)
		return -1;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_GET_FRAME_IMG;

    len = strlen(filename) + 1;
    if(len > 20)
    	return -1;
    	
    SemWait(hndlApproDrvSem);
    msgbuf.Shmem.offset = 0;
    msgbuf.Shmem.size = len;
    pShareMemWrite(mid,0,filename, len);

    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return msgbuf.ret;
}

int EnableEncodeImg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_ENABLE_ENCODE_IMG;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return msgbuf.ret;
}

int DisableEncodeImg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_DISABLE_ENCODE_IMG;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return msgbuf.ret;
}


int EnableSaveErrImg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_ENABLE_SAVE_ERRIMG;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return msgbuf.ret;
}

int DisableSaveErrImg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_DISABLE_SAVE_ERRIMG;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    return msgbuf.ret;
}
int SetAlgParam(char *pPrm,int prmSize)
{
    MSG_BUF msgbuf;
	if(prmSize > ALGPARAM_MAX_SIZE)
		return -1;
	
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_ALG_PARAM;
    msgbuf.Shmem.offset = 0;
    msgbuf.Shmem.size = prmSize;
    SemWait(hndlApproDrvSem);
    pShareMemWrite(mid,0,pPrm,prmSize);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1;
    
    return 0;		
}

int GetAlgParam(char *pPrm,int *prmSize)
{
    MSG_BUF msgbuf;
	*prmSize = 0;
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_ALG_PARAM;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    if((msgbuf.ret != 0)||(msgbuf.Shmem.size > ALGPARAM_MAX_SIZE))
		memset(pPrm,0,sizeof(Alg_Params));
    else
    {
    	*prmSize = msgbuf.Shmem.size; 
		pShareMemRead(mid,0,pPrm,msgbuf.Shmem.size);	
    }	

    SemRelease(hndlApproDrvSem);
  
    return msgbuf.ret;		
}

int Update_Alg_DbgImg(char *data,unsigned size)
{
    MSG_BUF msgbuf;
	if(size > MSG_SHMEM_MAXSIZE)
		return -1;
	
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_UPDATE_ALG_DBGIMG;
    msgbuf.Shmem.offset = 0;
    msgbuf.Shmem.size = size;
    SemWait(hndlApproDrvSem);
    pShareMemWrite(mid,0,data,size);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1;
    
    return 0;		
}
int EnableImgAlg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_ALG_ENABLE;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1;
    
    return 0;		
}

int DisableImgAlg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_ALG_DISABLE;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1;   
    
    return 0;		
}

int SetAlgMode(unsigned int debugMode)
{
	int cmd;
    MSG_BUF msgbuf;
	cmd = (debugMode>0)?MSG_CMD_ENABLE_ALG_DBGMODE:MSG_CMD_DISABLE_ALG_DBGMODE;
	
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = cmd;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1;   
    
    return 0;	
}

int SetOutPut_ImgSource()
{
    MSG_BUF msgbuf;
	
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_OUTPUT_IMG_SOURCE;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1;   
    
    return 0;	
}

int ImgAlg_UseStaticImg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_IMGALG_STATIC_IMG;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1;
    
    return 0;		
}

int ImgAlg_UseSensorImg()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_IMGALG_SENSOR_IMG;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        return -1; 
    
    return 0;		
}
/**
 * @brief  Message driver for getting current frame information
 *
 *
* @param   fmt    stucture of frame information, defined at Msg_def.h
 *
 *
 *
 */
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_CUR_FRAME;
    msgbuf.frame_info.format = fmt;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        msgbuf.frame_info.serial_no = -1;
    return msgbuf.frame_info;
}

/**
 * @brief  Message driver for waiting newframe (Do not use now)
 *
 *
* @param   fmt    stucture pointer of frame information, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
FrameInfo_t WaitNewFrame(FrameFormat_t fmt)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_NEW_FRAME;
    msgbuf.frame_info.format = fmt;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        msgbuf.frame_info.serial_no = -1;
    return msgbuf.frame_info;
}
/**
 * @brief  Message driver for getting memory information
 *
 *
 * @return MSG_MEM
 *
 *
 */
MSG_MEM GetPhyMem()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_MEM;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret != 0)
        msgbuf.mem_info.addr = 0;
    return msgbuf.mem_info;
}

/**
 * @brief  Message driver for sending leave message (Do not use now)
 *
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
void SendQuitCmd()
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = 0;
    msgbuf.cmd = MSG_CMD_QUIT;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    SemRelease(hndlApproDrvSem);

}
/**
 * @brief  Message driver for unLock frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
void UnlockFrame(FrameInfo_t *pFrame)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = 0;
    msgbuf.cmd = MSG_CMD_UNLOCK_FRAME;
    msgbuf.frame_info.format = pFrame->format;
    msgbuf.frame_info.serial_no = pFrame->serial_no;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    SemRelease(hndlApproDrvSem);
    return ;
}
/**
 * @brief  Message driver for Lock frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int LockFrame(FrameInfo_t *pFrame)
{
    int cnt = 0;
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_LOCK_FRAME;
    msgbuf.frame_info.format = pFrame->format;
    msgbuf.frame_info.serial_no = pFrame->serial_no;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    pFrame->size = msgbuf.frame_info.size;
    pFrame->width = msgbuf.frame_info.width;
    pFrame->height = msgbuf.frame_info.height;
    pFrame->offset = msgbuf.frame_info.offset;
    pFrame->quality = msgbuf.frame_info.quality;
    pFrame->flags = msgbuf.frame_info.flags;
    pFrame->frameType = msgbuf.frame_info.frameType;
    pFrame->timestamp = msgbuf.frame_info.timestamp;
    pFrame->temporalId = msgbuf.frame_info.temporalId;
    for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
    {
        pFrame->ref_serial[cnt] = msgbuf.frame_info.ref_serial[cnt];
    }

    return msgbuf.ret;
}
/**
 * @brief  Message driver for getting MPEG4 VOL data
 *
 *
 * @param   pVolBuf   ouput buffer for getting MPEG4 VOL data
 *
 * @param   fmt_type    frame type ID : FMT_MJPEG, FMT_MPEG4 , FMT_MPEG4_EXT, FMT_AUDIO, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int GetVolInfo(FrameInfo_t *pFrame, int fmt_type)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_VOL;
    msgbuf.frame_info.format = fmt_type;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    pFrame->offset  = msgbuf.frame_info.offset;
    pFrame->size    = msgbuf.frame_info.size;

    return msgbuf.frame_info.size;
}

/**
 * @brief  Message driver for getting Media info
 *
 *
 * @param   pVolBuf   ouput buffer for getting Media info
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int GetMediaInfo(FrameInfo_t *pFrame)
{
    MSG_BUF msgbuf;
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_MEDIA_INFO;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);

    pFrame->format  = msgbuf.frame_info.format;

    return msgbuf.frame_info.format;
}


/**
 * @brief  Message driver for Lock MPEG4 I-frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 * @param   fmt_type    frame type ID : FMT_MJPEG, FMT_MPEG4 , FMT_MPEG4_EXT, FMT_AUDIO, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int LockMpeg4IFrame(FrameInfo_t *pFrame,int fmt_type)
{
    int cnt = 0;
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_AND_LOCK_IFRAME;
    msgbuf.frame_info.format = fmt_type;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    if(msgbuf.ret == -1)
        msgbuf.frame_info.serial_no = -1;
    else if(msgbuf.ret != 0)
        msgbuf.frame_info.serial_no = -2;
    pFrame->serial_no = msgbuf.frame_info.serial_no;
    pFrame->size = msgbuf.frame_info.size;
    pFrame->width = msgbuf.frame_info.width;
    pFrame->height = msgbuf.frame_info.height;
    pFrame->offset = msgbuf.frame_info.offset;
    pFrame->quality = msgbuf.frame_info.quality;
    pFrame->flags = msgbuf.frame_info.flags;
    pFrame->frameType = msgbuf.frame_info.frameType;
    pFrame->timestamp = msgbuf.frame_info.timestamp;
    pFrame->temporalId = msgbuf.frame_info.temporalId;
    for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
    {
        pFrame->ref_serial[cnt] = msgbuf.frame_info.ref_serial[cnt];
    }
    return msgbuf.frame_info.serial_no;
}

/**
 * @brief  day / night mode setting for UI
 *
 *
 * @param   nDayNight    0 : night mode     1: day mode
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetDayNight(unsigned char nDayNight)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DAY_NIGHT;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nDayNight;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  Motion Enable mode setting for UI
 *
 *
 * @param   enableVal  0: Enable        1:Disable
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int ApproSetMotion(ApproMotionPrm* pMotionPrm)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_MOTION;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, pMotionPrm, sizeof(ApproMotionPrm));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SendFaceDetectMsg(FaceDetectParam* faceParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_FDETECT;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, faceParam, sizeof(FaceDetectParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

/* DMVA Specific Message */

int SendDmvaEnableMsg(DmvaParam* dmvaParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVAENABLE;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, dmvaParam, sizeof(DmvaParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

int SendDmvaExptAlgoFrameRateMsg(DmvaParam* dmvaParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVAALGOFRAMERATE;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, dmvaParam, sizeof(DmvaParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

int SendDmvaExptAlgoDetectFreqMsg(DmvaParam* dmvaParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVAALGODETECTFREQ;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, dmvaParam, sizeof(DmvaParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

int SendDmvaExptEvtRecordEnableMsg(DmvaParam* dmvaParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVAEVTRECORDENABLE;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, dmvaParam, sizeof(DmvaParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}
int SendDmvaExptSmetaTrackerEnableMsg(DmvaParam* dmvaParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVASMETATRACKERENABLE;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, dmvaParam, sizeof(DmvaParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

/* DMVA TZ Specific Message */


int SendDmvaTZSetupMsg(DmvaTZParam* dmvaTZParam)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVA_TZ;
    msgbuf.mem_info.addr = (gProcId - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
    SemWait(hndlApproDrvSem);
    ShareMemWrite(msgbuf.mem_info.addr, dmvaTZParam, sizeof(DmvaTZParam));
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

/* DMVA MAIN PAGE Specific Message */

int SendDmvaMainSetupMsg(DmvaMainParam* dmvaMainParam)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVA_MAIN;
    msgbuf.mem_info.addr = (gProcId - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
    SemWait(hndlApproDrvSem);
    ShareMemWrite(msgbuf.mem_info.addr, dmvaMainParam, sizeof(DmvaMainParam));
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}



/* DMVA CTD PAGE Specific Message */

int SendDmvaCTDSetupMsg(DmvaCTDParam* dmvaCTDParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVA_CTD;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, dmvaCTDParam, sizeof(DmvaCTDParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}



/* DMVA OC PAGE Specific Message */

int SendDmvaOCSetupMsg(DmvaOCParam* dmvaOCParam)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVA_OC;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, dmvaOCParam, sizeof(DmvaOCParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}


/* DMVA SMETA PAGE Specific Message */

int SendDmvaSMETASetupMsg(DmvaSMETAParam* dmvaSMETAParam)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVA_SMETA;
    msgbuf.mem_info.addr = (gProcId - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
    SemWait(hndlApproDrvSem);
    ShareMemWrite(msgbuf.mem_info.addr, dmvaSMETAParam, sizeof(DmvaSMETAParam));
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

/* DMVA IMD PAGE Specific Message */

int SendDmvaIMDSetupMsg(DmvaIMDParam* dmvaIMDParam)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVA_IMD;
    msgbuf.mem_info.addr = (gProcId - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
    SemWait(hndlApproDrvSem);
    ShareMemWrite(msgbuf.mem_info.addr, dmvaIMDParam, sizeof(DmvaIMDParam));
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;

}

/* DMVA Enc Roi Enable */
int SendDmvaEncRoiEnableMsg(int *pEnable)
{
    MSG_BUF msgbuf;
    void* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DMVA_ENCROI_ENABLE;
    ptr = (void*)&msgbuf.mem_info;
    memcpy(ptr, pEnable, sizeof(int));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}


int SetDateTimeFormat(DateTimePrm* datetimeParam)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DATETIMEPRM;
    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, datetimeParam, sizeof(DateTimePrm));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetOSDPrmMsg(int id, OSDPrm* osdprm)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;

    if(id==0)
        msgbuf.cmd = MSG_CMD_SET_OSDPRM1;
    else if(id==1)
        msgbuf.cmd = MSG_CMD_SET_OSDPRM2;
    else if(id==2)
        msgbuf.cmd = MSG_CMD_SET_OSDPRM3;

    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, osdprm, sizeof(OSDPrm));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetCodecROIMsg(int id, CodecROIPrm* codecROIPrm)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;

    if(id==0)
        msgbuf.cmd = MSG_CMD_SET_ROIPRM1;
    else if(id==1)
        msgbuf.cmd = MSG_CMD_SET_ROIPRM2;
    else if(id==2)
        msgbuf.cmd = MSG_CMD_SET_ROIPRM3;

    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, codecROIPrm, sizeof(CodecROIPrm));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetCodecAdvPrmMsg(int id, CodecAdvPrm* codecAdvPrm)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;

    if(id==0)
        msgbuf.cmd = MSG_CMD_SET_ADVCODECPRM1;
    else if(id==1)
        msgbuf.cmd = MSG_CMD_SET_ADVCODECPRM2;
    else if(id==2)
        msgbuf.cmd = MSG_CMD_SET_ADVCODECPRM3;

    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, codecAdvPrm, sizeof(CodecAdvPrm));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetMcfwCreatePrmMsg(McfwCreatePrm *mcfwCreatePrm)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_MCFW_CREATE_PARAM;
    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, mcfwCreatePrm, sizeof(McfwCreatePrm));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}


/**
 * @brief  indoor / outdoor mode setting for UI
 *
 *
 * @param   nValue    0 : Auto  1: Day D65 2: Day D55 3: Florescent 4: Incandescent
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetWhiteBalance(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_WHITE_BALANCE;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetExposure(unsigned int nValue)
{
    MSG_BUF msgbuf;
    unsigned int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_EXPOSURE;
    ptr = (unsigned int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int GetExposure(unsigned int *nValue)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_EXPOSURE;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    memcpy(nValue,&(msgbuf.mem_info),sizeof(unsigned int));
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int Get_MaxExposure(unsigned int *nValue)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_MAX_EXPOSURE;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    memcpy(nValue,&(msgbuf.mem_info),sizeof(unsigned int));
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int Get_MinExposure(unsigned int *nValue)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_MIN_EXPOSURE;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    memcpy(nValue,&(msgbuf.mem_info),sizeof(unsigned int));
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}


int SetGain(unsigned int sensorGain,unsigned int pipeGain)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_GAIN;
	msgbuf.camPrm.sensorGain = sensorGain;
	msgbuf.camPrm.pipeGain = pipeGain;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int GetGain(unsigned int *sensorGain,unsigned int *pipeGain)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_GAIN;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    *sensorGain = msgbuf.camPrm.sensorGain;
	*pipeGain = msgbuf.camPrm.pipeGain;
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int Get_MaxGain(unsigned int *sensorGain,unsigned int *pipeGain)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_MAX_GAIN;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    *sensorGain = msgbuf.camPrm.sensorGain;
	*pipeGain = msgbuf.camPrm.pipeGain;
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int Get_MinGain(unsigned int *sensorGain,unsigned int *pipeGain)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_MIN_GAIN;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    *sensorGain = msgbuf.camPrm.sensorGain;
	*pipeGain = msgbuf.camPrm.pipeGain;
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetH3AParam(EzCamH3AParam *pPrm)
{
    MSG_BUF msgbuf;
    
    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_2A_PARAM;
    memcpy(&(msgbuf.H3aParams),pPrm,sizeof(EzCamH3AParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int GetH3AParam(EzCamH3AParam *pPrm)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_2A_PARAM;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    memcpy(pPrm,&(msgbuf.H3aParams),sizeof(EzCamH3AParam));
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetH3AWeight(char *weight,int size)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_2A_WEIGHT;
    msgbuf.Shmem.offset = 0;
    msgbuf.Shmem.size = size;
    pShareMemWrite(mid,msgbuf.Shmem.offset,weight,size);
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int GetH3AWeight(char *weight,int size)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_2A_WEIGHT;
	msgbuf.Shmem.size = size;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);

    if((msgbuf.ret != 0)||(msgbuf.Shmem.offset<0))
    {
		SemRelease(hndlApproDrvSem);
		return msgbuf.ret;
    }
    pShareMemRead(mid,msgbuf.Shmem.offset,weight, size);
	SemRelease(hndlApproDrvSem);			
    return msgbuf.ret;
}
int SetDisplayValue(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_LOCALDISPLAY;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int SetStreamingStack(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_STREAMSTACK;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int SetOSDEnable(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_OSDENABLE;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  back light setting for UI
 *
 *
 * @param   nValue      =128: median \n
*               >128: hight \n
*               <128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBacklight(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_BACKLIGHT;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
/**
 * @brief  brightness setting for UI
 *
 *
 * @param   nValue      =128: median \n
*               >128: hight \n
*               <128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBrightness(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_BRIGHTNESS;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
/**
 * @brief  contrast setting for UI
 *
 *
 * @param   nValue      =128: median \n
*               >128: hight \n
*               <128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetContrast(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_CONTRAST;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  Saturation setting for UI
 *
 *
 * @param   nValue      =128: median \n
*               >128: hight \n
*               <128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetSaturation(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_SATURATION;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
/**
 * @brief  Sharpness setting for UI
 *
 *
 * @param   nValue      =128: median \n
*               >128: hight \n
*               <128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetSharpness(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_SHARPNESS;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  Bitrate setting for UI
 *
 *
 * @param   stream      0: stream1 720P \n
*               1: stream2 SIF \n
*
 * @param   nValue      64~8000
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetEncBitrate(unsigned char stream, int nValue)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    if (stream == 0)
        msgbuf.cmd = MSG_CMD_SET_BITRATE1;
    else
        msgbuf.cmd = MSG_CMD_SET_BITRATE2;
    ptr = (int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}


/**
 * @brief  RateControl setting for UI
 *
 *
 * @param   stream      0: stream1 1080P \n
*               1: stream2 D1 \n
*
 * @param   nValue      0~2
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetEncRateControl(unsigned char stream, int nValue)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    if (stream == 0)
        msgbuf.cmd = MSG_CMD_RATE_CONTROL1;
    else
        msgbuf.cmd = MSG_CMD_RATE_CONTROL2;
    ptr = (int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  frame rate setting for UI
 *
 *
 * @param   stream      0: stream1 MPEG4 720P \n
*               1: stream2 MPEG4 SIF \n
*               2: stream3 JPG VGA
*
 * @param   nValue      7500~30000 ( 7.5fps ~30fps )
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetStreamFramerate(unsigned char stream, unsigned int nValue)
{
    MSG_BUF msgbuf;
    unsigned int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    if (stream == 0)
        msgbuf.cmd = MSG_CMD_SET_FRAMERATE1;
    else if (stream == 1)
        msgbuf.cmd = MSG_CMD_SET_FRAMERATE2;
    else
        msgbuf.cmd = MSG_CMD_SET_FRAMERATE3;

    ptr = (unsigned int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  frame rate setting for UI
 *
 *
 * @param   nValue      7500~30000 ( 7.5fps ~30fps )
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetJpgQuality(int nValue)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_JPEG_QUALITY;
    ptr = (int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetMirr(int nValue)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_MIRROR;
    ptr = (int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetOSDWindow(int nValue)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_OSDWINDOW;
    ptr = (int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  2A enable/disable setting for UI
 *
 *
 * @param   nValue      0: disable \n
 *              1: enable
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetImage2AVendor(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_VENDOR_2A;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetImage2AType(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_TYPE_2A;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int Set2APriority(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_2A_PRIORITY;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int SetDCIRIS(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_IRIS;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int SetFramectrl(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_FRAMECTRL;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  TV setting for UI
 *
 *
 * @param   nValue      0: NTSC \n
 *              1: PAL
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetTvSystem(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_TV_SYSTEM;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  PTZ  setting  for UI
 *
 *
 * @param   nValue          0: PTZ_ZOOM_IN \n
*                   1: PTZ_ZOOM_OUT \n
*                   2: PTZ_PAN_UP \n
*                   3: PTZ_PAN_DOWN \n
*                   4: PTZ_PAN_LEFT \n
*                   5: PTZ_PAN_RIGHT \n
*                   6: PTZ_INIT_4X\n
*                   7: PTZ_ZOOM_EMPTY \n
*                   8: PTZ_ZOOM_RESET
 *
 *@note This function only work at sensor output is VGA
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetPtz(int nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_PTZ;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  BinningSkip  setting  for UI
 *
 *
 * @param   nValue          0: Binning \n
*                   1: Skipping \n
*
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBinningSkip(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_BINNING_SKIP;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

/**
 * @brief  time stamp  setting  for UI
 *
 *
 * @param   nValue          0: timestamp open \n
*                   1: timestamp close \n
*
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetTStamp(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_TSTAMP;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetLdcEnableMsg(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_LDC_ENABLE;
    ptr = (unsigned int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int SetVNFParamMsg(VnfParam* vnfParam)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_VNF_PARAM;
    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, vnfParam, sizeof(VnfParam));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int SetROIConfig(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_ROICFG;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetAvOsdTextEnable(int enable)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;

    msgbuf.cmd = MSG_CMD_SET_OSDTEXT_EN;

    ptr = (int*)&msgbuf.mem_info;
    *ptr = enable;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetHistEnable(int nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_HIST_EN;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetAACBitrate(int nValue)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_AAC_BITRATE;
    ptr = (int*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetAvOsdLogoEnable(int enable)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;

    msgbuf.cmd = MSG_CMD_SET_OSDLOGO_EN;

    ptr = (int*)&msgbuf.mem_info;
    *ptr = enable;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}
int SetClipSnapName(char* strText, int nLength)
{
    MSG_BUF msgbuf;
    int* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;

    msgbuf.cmd = MSG_CMD_SET_CLICKNAME;

    ptr = (int*)&msgbuf.mem_info;
    *ptr = nLength;
    memcpy(++ptr, strText, nLength);
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetClipSnapLocation(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_CLICKSTORAGE;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SendForceIFrameMsg(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_IFRAME;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetAudioAlarmValue(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_AUDIO_ALARM_LEVEL;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetAudioAlarmFlag(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_AUDIO_ALARM_FLAG;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SetDynRangePrmMsg(DynRangePrm* dynRangePrm)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_DYNRANGE;
    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, dynRangePrm, sizeof(DynRangePrm));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int ApproPollingMsg()
{
    MSG_BUF msgbuf;
    int ret,i;
	int msgCnt;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_POLLING;
    SemWait(hndlApproDrvSem);
	msgCnt = 0;
	do
	{
		MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
		msgCnt ++;		
		
		ret = MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, IPC_NOWAIT);	
		if(ret >= 0){
			if(msgCnt > 0)
			{
				for(i = 0;i < (msgCnt - 1);i++)
				{
					MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
				}	
			}	
				
			ret = msgbuf.ret;
		}
		else{
			usleep(240000);			
		}		
	}while(ret < 0);
	SemRelease(hndlApproDrvSem);
    return ret;
}

int StopMcFWUsecase(unsigned char nValue)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_STOPDEMO;
    ptr = (unsigned char*)&msgbuf.mem_info;
    *ptr = nValue;
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}


int SendVideoCodecRes(StreamResolution *strmRes)
{
    MSG_BUF msgbuf;
    unsigned char* ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_VIDEO_CODEC_RES;
    ptr = (unsigned char*)&msgbuf.mem_info;
    memcpy(ptr, strmRes, sizeof(StreamResolution));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int SendVideoCodecCfg(VideoCodecCfg *pVidCodecCfg)
{
    MSG_BUF msgbuf;
    VideoCodecCfg *ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_SET_VIDEO_CODEC_CFG;
    ptr = (VideoCodecCfg*)&msgbuf.mem_info;
    memcpy(ptr, pVidCodecCfg, sizeof(VideoCodecCfg));
    SemWait(hndlApproDrvSem);
    MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
    MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	pVidCodecCfg->restartFlag = ptr->restartFlag;
    SemRelease(hndlApproDrvSem);
    return msgbuf.ret;
}

int GetMcFWInitStatus(McFWInitStatus *pStatus)
{
    int ret;
	MSG_BUF msgbuf;
    McFWInitStatus *ptr;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = PROC_MSG_ID;
    msgbuf.cmd = MSG_CMD_GET_MCFW_INIT_STATUS;
    ptr = (McFWInitStatus*)&msgbuf.mem_info;
    memcpy(ptr, pStatus, sizeof(McFWInitStatus));
	
    SemWait(hndlApproDrvSem);
	pStatus->initDone = 0;
    ret = MSGSND( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	if(ret == 0)
	{
		ret = MSGRCV( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
		if(ret >= 0)
		{
			pStatus->initDone = ptr->initDone;
		}
		else		
		{
			msgbuf.ret = ret;
		}
	}
	else	
	{
		msgbuf.ret = ret;		
	}
    SemRelease(hndlApproDrvSem);
	
    return msgbuf.ret;
}
