/** ==================================================================
 *  @file   frameProcess_tsk.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/glbceSupport/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Msg_Def.h>
#include <osa_shmem.h>
#include <osa_dma.h>

#include "frameProcess_pri.h"

extern Int FrameSave_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg,Uint32 curState);
extern Int AlgApp_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg,Uint32 curState);

FrameProcessLink_Obj gFrameProcessLink_obj[FRAMEPROCESS_LINK_OBJ_MAX];

static
Void FrameProcessLink_setOutQueInfo(FrameProcessLink_Obj *pObj)
{
    UInt32 totalNumInputChannels = 0;
    Int i;
    UInt32 chPerQueue,chId;
    UInt32 outChId,queueId;
    UInt32 inChId,inQueueId;

    for (i = 0; i < pObj->inQueInfo.numQue;i++)
    {
        totalNumInputChannels += pObj->inQueInfo.queInfo[i].numCh;
    }
    chPerQueue = totalNumInputChannels;
    pObj->info.numQue = 1;

    inQueueId = 0;
    inChId    = 0;
    for (chId = 0; chId < totalNumInputChannels;chId++)
    {
        outChId = chId % chPerQueue;
        queueId = chId / chPerQueue;
        OSA_assert(queueId < pObj->info.numQue);

        pObj->info.queInfo[queueId].numCh = chPerQueue;
        OSA_assert((inQueueId < pObj->inQueInfo.numQue)
                      &&
                      (inChId   < pObj->inQueInfo.queInfo[inQueueId].numCh));
        pObj->info.queInfo[queueId].chInfo[outChId] =
            pObj->inQueInfo.queInfo[inQueueId].chInfo[inChId];
        inChId++;
        if (inChId >= pObj->inQueInfo.queInfo[inQueueId].numCh)
        {
            inChId = 0;
            inQueueId++;
        }
    }
}


/* FrameProcessLink create */

/* ===================================================================
 *  @func     FrameProcessLink_create                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                 
Int32 FrameProcessLink_create(FrameProcessLink_Obj * pObj,FrameProcessHostLink_CreateParams * pPrm)
{	
	Int32 status;
	Int32 idx;
    System_LinkInQueParams *pInQueParams;
	VIDFrame_Buf *pFrame;
	ErrFrame_Buf *pErrFrame;

#ifdef SYSTEM_DEBUG_FRAMEPROCESS
    OSA_printf(" %d: FrameProcessLink   : Create in progress !!!\n",
               OSA_getCurTimeInMsec());
#endif

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    status = OSA_queCreate(&pObj->outFrameBufQue,
                           FRAMEPROCESS_LINK_MAX_FRAMES);
    OSA_assert(status == OSA_SOK);

    status = OSA_queCreate(&pObj->freeFrameBufQue,
                           FRAMEPROCESS_LINK_MAX_FRAMES);
    OSA_assert(status == OSA_SOK);                          
    
	/** create error frame buf queue */
    status = OSA_queCreate(&pObj->freeErrFrameBufQue,
                           FRAMEPROCESS_LINK_MAX_FRAMES);
    OSA_assert(status == OSA_SOK);

    status = OSA_queCreate(&pObj->outErrFrameBufQue,
                           FRAMEPROCESS_LINK_MAX_FRAMES);
    OSA_assert(status == OSA_SOK);

    
    pInQueParams = &pObj->createArgs.inQueParams;
	
    status = System_linkGetInfo(pInQueParams->prevLinkId, &pObj->inQueInfo);
    OSA_assert(status == OSA_SOK);

	for(idx=0;idx<FRAMEPROCESS_LINK_MAX_FRAMES;idx++)
	{
		pFrame = &(pObj->FrameBuf[idx]);
		status = OSA_quePut(&pObj->freeFrameBufQue,(Int32)(pFrame), OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);
	}
	
	for(idx=0;idx<FRAMEPROCESS_LINK_MAX_FRAMES;idx++)
	{
		pErrFrame = &(pObj->errBuf[idx]);
		pErrFrame->pData = (unsigned char *)malloc(ERR_FRAME_MAX_WIDTH*ERR_FRAME_MAX_HEIGTH);
		OSA_assert(pErrFrame->pData != NULL);
		
		status = OSA_quePut(&pObj->freeErrFrameBufQue,(Int32)(pErrFrame), OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);
	}
		
	FrameProcessLink_setOutQueInfo(pObj);

	pObj->getFrameFlag = FALSE;

	pObj->encodeEnable = TRUE;

	pObj->saveErrImg = TRUE;

  	Ptr srHeap = NULL;
  	srHeap = SharedRegion_getHeap(ALG_SHAREDREGION_ID);
  	OSA_assert(srHeap != NULL);
  	pObj->Algbuf = NULL;

	/** msg for alg process **/
	pObj->AlgStartFlag = FALSE;
	pObj->mAlgshmid = OSA_ShareMemInit(ALGAPP_SHM_KEY,ALGAPP_SHM_SIZE);
  	if((pObj->qSerialId = Msg_Init(ALGAPP_MSG_KEY)) < 0){
		OSA_printf("%s in %d:Msg_Init Fail\n",__func__,__LINE__);
		return FRAMEPROCESS_LINK_S_FAIL;
	}
        
    return FRAMEPROCESS_LINK_S_SUCCESS;
}

/* FrameProcessLink delete */

/* ===================================================================
 *  @func     FrameProcessLink_delete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 FrameProcessLink_delete(FrameProcessLink_Obj * pObj)
{
	int idx;
	ErrFrame_Buf *pErrFrame;
	
	OSA_queDelete(&(pObj->outFrameBufQue));

	OSA_queDelete(&pObj->freeFrameBufQue);
	
	for(idx=0;idx<FRAMEPROCESS_LINK_MAX_FRAMES;idx++)
	{
		pErrFrame = &(pObj->errBuf[idx]);
		if(pErrFrame->pData != NULL)
			free(pErrFrame->pData);
	}		
    return FRAMEPROCESS_LINK_S_SUCCESS;
}

static int SaveImage(VIDFrame_Buf *pFullFrame,unsigned char *pDst)
{
	OSA_FramesMMapCtrl ctrl;
	UInt32 pMemVirtAddr = 0;

	unsigned pitch = pFullFrame->framePitch[0];
	unsigned height = pFullFrame->frameHeight;
	if(pDst == NULL)
		return -1;
		
	OSA_FrameMapMem(&ctrl,(UInt32)(pFullFrame->phyAddr[0][0]), pitch*height, &pMemVirtAddr);

	memcpy(pDst,(char *)pMemVirtAddr,pitch*height);

	OSA_FrameUnmapMem(&ctrl);

	return 0;
}


static int SaveImageFile(FrameProcessLink_Obj * pObj,VIDFrame_Buf *pFullFrame)
{
	ErrFrame_Buf *pEmptyFrame;
	Int32 status;

	status = OSA_queGet(&pObj->freeErrFrameBufQue, (Int32 *)&pEmptyFrame,OSA_TIMEOUT_NONE);                  
	if (status != OSA_SOK)
	{
		printf("error:OSA_queGet line %d\n",__LINE__);
		return -1;	
	}

	pEmptyFrame->pitch = pFullFrame->framePitch[0];		
	pEmptyFrame->width = pFullFrame->frameWidth;
	pEmptyFrame->height = pFullFrame->frameHeight;
	pEmptyFrame->type	= IMG_SAVE;

	memcpy(pEmptyFrame->fileName,pObj->fileName,strlen((const char *)pObj->fileName));
	
	SaveImage(pFullFrame,pEmptyFrame->pData);
	status = OSA_quePut(&pObj->outErrFrameBufQue,(Int32)pEmptyFrame, OSA_TIMEOUT_NONE);
	if(status != OSA_SOK)
	{
		printf("error:OSA_quePut line %d\n",__LINE__);
		OSA_quePut(&pObj->freeErrFrameBufQue,(Int32)pEmptyFrame, OSA_TIMEOUT_NONE);
		return -1;
	}

	OSA_tskSendMsg(&(pObj->frameSavetsk), &(pObj->tsk), SYSTEM_CMD_NEW_DATA, NULL, 0);

	return 0;
}


static int Output_AlgResult(int offset,FrameProcessLink_Obj * pObj,Alg_Result *pResult)
{
	unsigned int len = pResult->resultSize;
	ALGAPP_MSG_BUF	msgbuf;
	int ret;
	
	if(pObj->AlgStartFlag == FALSE)
		return -1;
			
	if((len > 0)&&(len < ALGRESULT_MAX_SIZE))
	{		
		OSA_ShareMemWrite(pObj->mAlgshmid,0,pResult->result, len);
		msgbuf.Des = MSG_TYPE_MSG1;
		msgbuf.cmd = ALG_CMD_RESULT;
		msgbuf.src = MSG_TYPE_MSG2;
		msgbuf.offset = 0;
		msgbuf.size = len;
		msgbuf.ret = 0;

		ret = Msg_Send_Rsv( pObj->qSerialId, MSG_TYPE_MSG2, &msgbuf ,sizeof(msgbuf));
		if(ret <= 0)
			return -1;
		else	
		{
			if(msgbuf.ret != 0)
				return -1;
		}	
	}
	else
		return -1;

	return 0;		
}
			
static unsigned int needSaveImg()
{
	unsigned int ret = 0;
	static unsigned int oldtime = 0;
	unsigned int newtime = OSA_getCurTimeInMsec();

	if((newtime - oldtime) > IMG_SAVE_INTER_MS)
		ret = 1;

	oldtime = newtime;

	return ret;	
}
static int Alg_Process(FrameProcessLink_Obj * pObj,Alg_Result *pResult,VIDFrame_Buf *pFullFrame)
{
	int status = OSA_SOK;
	ErrFrame_Buf *pEmptyFrame;		
	do
	{
		if((TRUE==pObj->saveErrImg)&&(pResult->Alg_saveImg))
		{
			if(needSaveImg())
			{
				status = OSA_queGet(&pObj->freeErrFrameBufQue, (Int32 *)&pEmptyFrame,OSA_TIMEOUT_NONE);                  
    			if (status != OSA_SOK)
    			{
    				printf("error:OSA_queGet line %d\n",__LINE__);
					break;	
				}
				pEmptyFrame->pitch = pFullFrame->framePitch[0];		
				pEmptyFrame->width = pFullFrame->frameWidth;
				pEmptyFrame->height = pFullFrame->frameHeight;
				pEmptyFrame->type = IMG_ERR;
				OSA_getlocaltime((char *)pEmptyFrame->fileName);
				memcpy(&(pEmptyFrame->info),&(pResult->saveInfo),sizeof(AlgSaveImg));
				
				SaveImage(pFullFrame,pEmptyFrame->pData);
			
				status = OSA_quePut(&pObj->outErrFrameBufQue,(Int32)pEmptyFrame, OSA_TIMEOUT_NONE);
				if(status != OSA_SOK)
				{
					printf("error:OSA_quePut line %d\n",__LINE__);
					break;
				}
				OSA_tskSendMsg(&(pObj->frameSavetsk), &(pObj->tsk), SYSTEM_CMD_NEW_DATA, NULL, 0);
			}		
		}
		
		Output_AlgResult(pFullFrame->algbuf.offset,pObj,pResult);
	}while(0);

    return status;	
}

void FrameProcessLink_Process(FrameProcessLink_Obj * pObj)
{
	VIDFrame_BufList frameList;
	System_LinkInQueParams *pInQueParams;	
	VIDFrame_Buf *pFullFrame;
	VIDFrame_Buf *pEmptyFrame;
	Int32 idx;
	Int32 status;
	Alg_Result *pResult;
	pInQueParams = &pObj->createArgs.inQueParams;	
							
    System_getLinksFullFrames(pInQueParams->prevLinkId,pInQueParams->prevLinkQueId, &frameList);

	if(frameList.numFrames)
	{
		for(idx=0;idx<frameList.numFrames;idx++)
		{
			pFullFrame = &(frameList.frames[idx]);
			if(pObj->Algbuf == NULL)
				pObj->Algbuf = SharedRegion_getPtr(pFullFrame->algbuf.srPtr);

			if(pObj->Algbuf != NULL)
			{
				pResult = (Alg_Result *)((char *)pObj->Algbuf + pFullFrame->algbuf.offset);
				Alg_Process(pObj,pResult,pFullFrame);
			}	
		
			if(TRUE==pObj->getFrameFlag)
			{
				pObj->getFrameFlag = FALSE;
				SaveImageFile(pObj,pFullFrame);
			}	
		
			if(TRUE==pObj->encodeEnable)
			{
				status = OSA_queGet(&pObj->freeFrameBufQue, (Int32 *) &pEmptyFrame,OSA_TIMEOUT_NONE);
				if(status != OSA_SOK)
					break;		
						
				memcpy(pEmptyFrame,pFullFrame,sizeof(VIDFrame_Buf));

				status = OSA_quePut(&pObj->outFrameBufQue,
                            (Int32)pEmptyFrame, OSA_TIMEOUT_NONE);
				if(status != OSA_SOK)
					break;
			}		
		}
	}

	if(frameList.numFrames)
	{
		System_putLinksEmptyFrames(pInQueParams->prevLinkId,pInQueParams->prevLinkQueId,&frameList);
	}

	if((TRUE==pObj->encodeEnable)&&(frameList.numFrames))	
	{
		/* send SYSTEM_CMD_NEW_DATA to next link */
    	System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);	
	}
}


Int32 FrameProcessLink_getErrFrame(FrameProcessLink_Obj *pObj,ErrFrame_Buf **pFrame)
{
    Int32 status;

    status = OSA_queGet(&pObj->outErrFrameBufQue, (Int32 *)pFrame,
                       OSA_TIMEOUT_FOREVER);                  
    if (status != OSA_SOK)
		return FRAMEPROCESS_LINK_S_FAIL;
	
	return FRAMEPROCESS_LINK_S_SUCCESS;	
}

Int32 FrameProcessLink_ReleaseErrFrame(FrameProcessLink_Obj *pObj,ErrFrame_Buf * pFrame)
{
    Int32 status;

	status = OSA_quePut(&pObj->freeErrFrameBufQue,(Int32)(pFrame), OSA_TIMEOUT_NONE);
	OSA_assert(status == OSA_SOK);
		
	return FRAMEPROCESS_LINK_S_SUCCESS;	
}

Int32 FrameProcessHostLink_getFullFrames(OSA_TskHndl * pTsk, UInt16 queId,
                                   VIDFrame_BufList * pFrameBufList)
{
	
	UInt32 idx;
    Int32 status;
    VIDFrame_Buf *pFrame;
    FrameProcessLink_Obj *pObj = (FrameProcessLink_Obj *) pTsk->appData;

    for (idx = 0; idx < VIDFRAME_MAX_FRAME_BUFS; idx++)
    {
        status =
            OSA_queGet(&pObj->outFrameBufQue, (Int32 *) & pFrame,
                       OSA_TIMEOUT_NONE);
        if (status != OSA_SOK)
            break;
        pFrameBufList->frames[idx] = *pFrame;
    }

    pFrameBufList->numFrames = idx;	
	return FRAMEPROCESS_LINK_S_SUCCESS;	
}

Int32 FrameProcessHostLink_putEmptyFrames(OSA_TskHndl * pTsk, UInt16 queId,
                                   VIDFrame_BufList * pFrameBufList)
{
    UInt32 idx;
    Int32 status;
    VIDFrame_Buf *pFrame;
    FrameProcessLink_Obj *pObj = (FrameProcessLink_Obj *) pTsk->appData;

    for (idx = 0; idx < pFrameBufList->numFrames; idx++)
    {
    	pFrame = &(pFrameBufList->frames[idx]);
		status = OSA_quePut(&pObj->freeFrameBufQue,
                            (Int32)pFrame, OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);
    }

	return FRAMEPROCESS_LINK_S_SUCCESS;
}

Int32 FrameProcessHostLink_getLinkInfo(OSA_TskHndl * pTsk, System_LinkInfo * info)
{
    FrameProcessLink_Obj *pObj = (FrameProcessLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FRAMEPROCESS_LINK_S_SUCCESS;
}

/* FrameProcessLink task main function */

/* ===================================================================
 *  @func     FrameProcessLink_tskMain                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int FrameProcessLink_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg,
                          Uint32 curState)
{
    UInt32 cmd = OSA_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status = FRAMEPROCESS_LINK_S_SUCCESS;
    FrameProcessLink_Obj *pObj = (FrameProcessLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        OSA_tskAckOrFreeMsg(pMsg, OSA_EFAIL);
        return status;
    }

    /* create the FrameProcessLink */
    status = FrameProcessLink_create(pObj, OSA_msgGetPrm(pMsg));

    OSA_tskAckOrFreeMsg(pMsg, status);
	
    if (status != OSA_SOK)
        return status;

	OSA_tskSendMsg(&(pObj->frameSavetsk), &(pObj->tsk), SYSTEM_CMD_CREATE, NULL, 0);
	OSA_tskSendMsg(&(pObj->AlgApptsk), &(pObj->tsk), SYSTEM_CMD_CREATE, NULL, 0);
    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = OSA_tskWaitMsg(pTsk, &pMsg);
        if (status != OSA_SOK)
            break;

        cmd = OSA_msgGetCmd(pMsg);
		
        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
            {
                done = TRUE;
                ackMsg = TRUE;
                OSA_tskAckOrFreeMsg(pMsg, status);	
                break;
            }   
            case SYSTEM_CMD_NEW_DATA:	
            {
                FrameProcessLink_Process(pObj);
                OSA_tskAckOrFreeMsg(pMsg, status);	
                break;
			}
            case SAVE_FRAME_IMG:
            {
            	char *fileName = OSA_msgGetPrm(pMsg);
            	memcpy(pObj->fileName,fileName,strlen(fileName)+1);
            	pObj->getFrameFlag = TRUE;
            	OSA_tskAckOrFreeMsg(pMsg, status);
            	break; 
            }	
            case ENABLE_ENCODE_IMG:
            {
            	pObj->encodeEnable = TRUE;
            	OSA_tskAckOrFreeMsg(pMsg, status);
            	break;
            }		
            case DISABLE_ENCODE_IMG:
            {
            	pObj->encodeEnable = FALSE;
            	OSA_tskAckOrFreeMsg(pMsg, status);
            	break;
            }	
            case ENABLE_SAVE_ERRIMG:
            {
            	pObj->saveErrImg = TRUE;
            	OSA_tskAckOrFreeMsg(pMsg, status);
            	break;	
			}
            case DISABLE_SAVE_ERRIMG:
            {
            	pObj->saveErrImg = FALSE;
            	OSA_tskAckOrFreeMsg(pMsg, status);
            	break;
            }	            		            	   
            default:
            {
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
            }    
        }
    }
	
    /* Delete the glbceSupport alg */
    FrameProcessLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        OSA_tskAckOrFreeMsg(pMsg, status);
    }

    return status;
}

/*  FrameProcessHostLink init */

/* ===================================================================
 *  @func      FrameProcessHostLink_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 FrameProcessHostLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 frameProcessId;

    FrameProcessLink_Obj *pObj;

    char tskName[32];

    for (frameProcessId = 0; frameProcessId < FRAMEPROCESS_LINK_OBJ_MAX; frameProcessId++)
    {
        pObj = &gFrameProcessLink_obj[frameProcessId];

        memset(pObj, 0, sizeof(*pObj));
        linkObj.pTsk = &pObj->tsk;
        linkObj.getLinkInfo = FrameProcessHostLink_getLinkInfo;
		linkObj.linkGetFullFrames = FrameProcessHostLink_getFullFrames;
        linkObj.linkPutEmptyFrames = FrameProcessHostLink_putEmptyFrames;

        System_registerLink(SYSTEM_HOST_LINK_ID_FRAMEPROCESS + frameProcessId, &linkObj);

        OSA_SNPRINTF(tskName, "FRAMEPROCESS%d", frameProcessId);

        status = OSA_tskCreate(&pObj->tsk,
                                 FrameProcessLink_tskMain,
                                 FRAMEPROSSHOST_PRI,
                                 FRAMEPROSSHOST_LINK_TSK_STACK_SIZE ,0,  pObj/*, tskName*/);
        OSA_assert(status == OSA_SOK);	

		status = OSA_tskCreate(&pObj->frameSavetsk,
                                 FrameSave_tskMain,
                                 FRAMESAVE_TSK_PRI,
                                 FRAMESAVE_TSK_STACK_SIZE ,0,  pObj);
        OSA_assert(status == OSA_SOK);

		status = OSA_tskCreate(&pObj->AlgApptsk,
                                 AlgApp_tskMain,
                                 FRAMESAVE_TSK_PRI,
                                 FRAMESAVE_TSK_STACK_SIZE ,0,  pObj);
        OSA_assert(status == OSA_SOK);          
    }

    return status;
}

/* FrameProcessHostLink de-init */

/* ===================================================================
 *  @func     FrameProcessHostLink_deInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 FrameProcessHostLink_deInit()
{
    UInt32 frameProcessId;

    for (frameProcessId = 0; frameProcessId < FRAMEPROCESS_LINK_OBJ_MAX; frameProcessId++)
    {
        OSA_tskDelete(&gFrameProcessLink_obj[frameProcessId].tsk);
    }

    return FRAMEPROCESS_LINK_S_SUCCESS;
}
