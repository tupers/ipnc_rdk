/** ==================================================================
 *  @file   rvmLink_tsk.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_c6xdsp/rvm/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "imgLink_priv.h"
#include "detectRect.h"

#pragma DATA_ALIGN(gImgLink_tskStack, 32)
#pragma DATA_SECTION(gImgLink_tskStack, ".bss:taskStackSection")
UInt8 gImgLink_tskStack[IMG_LINK_OBJ_MAX][IMG_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gImgLink_processTskStack, 32)
#pragma DATA_SECTION(gImgLink_processTskStack, ".bss:taskStackSection")
UInt8 gImgLink_processTskStack[IMG_LINK_OBJ_MAX][IMG_LINK_PROCESSTSK_STACK_SIZE];

ImgLink_Obj gImgLink_obj[IMG_LINK_OBJ_MAX];

/* ===================================================================
 *  @func     ImgLink_algCopyFrames
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
Int32 ImgLink_algCopyFrames(ImgLink_Obj *pObj,FVID2_Frame *pSrc,FVID2_Frame *pDst)
{
    UInt32 edmaWidth, edmaHeight;
	System_FrameInfo *pSrcfrmInfo;
	System_FrameInfo *pDstfrmInfo;	 
	pSrcfrmInfo  = pSrc->appData;
	pDstfrmInfo  = pDst->appData;
	 
    pDst->fid        = pSrc->fid;
    pDst->timeStamp  = pSrc->timeStamp;
    pDst->blankData    = NULL;
    pDst->drvData      = NULL;
    pDst->perFrameCfg  = NULL;
    pDst->subFrameInfo = NULL;
	pDst->channelNum = 0;
	pDst->timeStamp = pSrc->timeStamp;

     pDstfrmInfo->rtChInfo.width = pSrcfrmInfo->rtChInfo.width;
     pDstfrmInfo->rtChInfo.height = pSrcfrmInfo->rtChInfo.height;
     pDstfrmInfo->rtChInfo.pitch[0] = pSrcfrmInfo->rtChInfo.pitch[0];
     pDstfrmInfo->rtChInfo.pitch[1] = pSrcfrmInfo->rtChInfo.pitch[1];
     	
    edmaWidth  = pDstfrmInfo->rtChInfo.pitch[0];
    edmaHeight = pDstfrmInfo->rtChInfo.height;

    DM81XX_EDMA3_setParams(pObj->copyEdmaHndl.dmaHndl.chId, 			// chId
			         IMG_LINK_EDMA3_QUEUE_ID,  					// dmaQueue
				 (UInt32)pSrc->addr[0][0], 		// srcAddr
				 (UInt32)pDst->addr[0][0],		// dstAddr
				 edmaWidth,          								// edmaWidth
				 edmaHeight,         								// edmaHeight
				 edmaWidth,          								// srcLineOffset
				 edmaWidth);         								// dstLineOffset
     /* Trigger the edma transfer */
    DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndl.dmaHndl.chId);
    
   return 0;
}

/* ===================================================================
 *  @func     ImgLink_create
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
Int32 ImgLink_create(ImgLink_Obj * pObj, ImgLink_CreateParams * pPrm)
{
    Int32 status;
    UInt32 frameId,chId;
    FVID2_Frame *pFrame;
    System_LinkQueInfo *pOutQueInfo;
    System_LinkChInfo *pInChInfo, *pOutChInfo;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    /* Get the link info from prev link */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);
	
    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));	
	
	UTILS_assert(pObj->inQueInfo.numCh == 1);
	
    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue,           // pHndl
                             FALSE,                        // blockOnGet
                             FALSE);                       // blockOnPut
    UTILS_assert(status == FVID2_SOK);

	/* Allocate output buffers */
    /* Allocate memory for the buffers */
    pObj->bufferFrameFormat.width      = pObj->inQueInfo.chInfo[0].width;
    pObj->bufferFrameFormat.height     = pObj->inQueInfo.chInfo[0].height;
    pObj->bufferFrameFormat.dataFormat = pObj->inQueInfo.chInfo[0].dataFormat;
    pObj->bufferFrameFormat.pitch[0]   = pObj->inQueInfo.chInfo[0].pitch[0];
    pObj->bufferFrameFormat.pitch[1]   = pObj->inQueInfo.chInfo[0].pitch[1];
    pObj->inWidth = pObj->inQueInfo.chInfo[0].width;
    pObj->inHeight = pObj->inQueInfo.chInfo[0].height;

    status = Utils_memFrameAlloc(&pObj->bufferFrameFormat, // pFormat
		    pObj->outFrames,          // pFrame
		    IMG_LINK_MAX_OUT_FRAMES);  // numFrames
    UTILS_assert(status == FVID2_SOK);
  
	pObj->pAlgInfo = Utils_memAlloc(sizeof(Alg_MsgBuf) * IMG_LINK_MAX_OUT_FRAMES, 4);
    UTILS_assert(pObj->pAlgInfo != NULL);
    memset(pObj->pAlgInfo,0,sizeof(Alg_MsgBuf) * IMG_LINK_MAX_OUT_FRAMES);

    /**alloc alg result memory ****/ 
	Ptr srHeap = NULL;
	srHeap = SharedRegion_getHeap(ALG_SHAREDREGION_ID);
    UTILS_assert(srHeap != NULL);    
	
	pObj->pAlgShBuf = Memory_calloc(srHeap, ALGRESULT_MAX_SIZE * IMG_LINK_MAX_OUT_FRAMES, 128, NULL);
	pObj->srPtr = SharedRegion_getSRPtr(pObj->pAlgShBuf, ALG_SHAREDREGION_ID);

	Alg_MsgBuf *pMsg;
    /* Queue all the buffers to empty queue */
    for (frameId = 0; frameId < IMG_LINK_MAX_OUT_FRAMES; frameId++)
    {
	    pFrame = &pObj->outFrames[frameId];
	    pFrame->pImgAlg = pObj->pAlgInfo+sizeof(Alg_MsgBuf)*frameId;

		pMsg = (Alg_MsgBuf *)(pFrame->pImgAlg);
		pMsg->valid = 0;
		pMsg->srPtr = (unsigned int)(pObj->srPtr);
		pMsg->offset = frameId * ALGRESULT_MAX_SIZE;
		pMsg->size = ALGRESULT_MAX_SIZE;
	    pFrame->appData = &pObj->freeFrameInfoMem[frameId];
	    status = Utils_bufPutEmptyFrame(&pObj->outFrameQue, pFrame);
	    UTILS_assert(status == FVID2_SOK);
    }	

	pObj->pAlgPrmShbuf = Memory_calloc(srHeap, ALG_SHMEM_MAXSIZE, 128, NULL);
	pObj->algPrmsrPtr = SharedRegion_getSRPtr(pObj->pAlgPrmShbuf, ALG_SHAREDREGION_ID);

   /* Populating the link info structure */
    pObj->info.numQue = 1;
    pOutQueInfo = &pObj->info.queInfo[0];
    pObj->info.queInfo[0].numCh = pObj->inQueInfo.numCh;

    for (chId = 0; chId < pObj->info.queInfo[0].numCh; chId++)
    {
	    pInChInfo = &pObj->inQueInfo.chInfo[chId];
	    pOutChInfo = &pOutQueInfo->chInfo[chId];

	    pOutChInfo->bufType      = pInChInfo->bufType;
	    pOutChInfo->codingformat = pInChInfo->codingformat;
	    pOutChInfo->dataFormat   = pInChInfo->dataFormat;
	    pOutChInfo->memType      = pInChInfo->memType;
	    pOutChInfo->startX       = 0;
	    pOutChInfo->startY       = 0;
	    pOutChInfo->width        = pObj->bufferFrameFormat.width;
	    pOutChInfo->height       = pObj->bufferFrameFormat.height;
	    pOutChInfo->pitch[0]     = pObj->bufferFrameFormat.pitch[0];
	    pOutChInfo->pitch[1]     = pObj->bufferFrameFormat.pitch[1];
	    pOutChInfo->scanFormat   = pInChInfo->scanFormat;
    }

	pObj->useStaticImg = TRUE;
	System_getEdmaCh(&pObj->copyEdmaHndl);

    Vps_printf(" %d: ImgLink   : Create Done !!!\n", Clock_getTicks());

    return FVID2_SOK;
}


/* Img link drv delete */

/* ===================================================================
 *  @func     ImgLink_delete
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
Int32 ImgLink_delete(ImgLink_Obj * pObj)
{
    Int32 status;	

	free(pObj->pAlgInfo);
    /* Free the frame memory */
    status = Utils_memFrameFree(&pObj->bufferFrameFormat,  	// pFormat
                                pObj->outFrames,           	// pFrame
                                IMG_LINK_MAX_OUT_FRAMES);   // numFrames
    UTILS_assert(status == FVID2_SOK);	
	
    /* Delete the buffers */
    status = Utils_bufDelete(&pObj->outFrameQue);

    System_putEdmaCh(&pObj->copyEdmaHndl);  	
#ifdef SYSTEM_DEBUG_IMG
	Vps_printf(" %d: IMG   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

Int32 ImgLink_processFrames(ImgLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;
    UInt32 frameId;
    FVID2_FrameList frameList;
    FVID2_Frame *pFullFrame,*pEmptyFrame;
    System_LinkInQueParams *pInQueParams;
    Alg_Result *pResult;
	Alg_MsgBuf *pMsgbuf;
	
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {       
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
        	pFullFrame = frameList.frames[frameId];
	    	status = Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame, BIOS_NO_WAIT);
	    	if((status != FVID2_SOK) || (pEmptyFrame == NULL))
	   		{
	        	 /* Empty buffer is not avialbale,return the full frames back */
		 		System_putLinksEmptyFrames(pInQueParams->prevLinkId,pInQueParams->prevLinkQueId, &frameList);				
											  
		  		return FVID2_SOK;							  
	   		}
	   		else
	   		{
				ImgLink_algCopyFrames(pObj,pFullFrame,pEmptyFrame);
				pMsgbuf = (Alg_MsgBuf *)pEmptyFrame->pImgAlg;
				pMsgbuf->valid = 0;
				pMsgbuf->size = sizeof(Alg_Result);
				pResult = (Alg_Result *)((char *)pObj->pAlgShBuf + pMsgbuf->offset);
				pResult->weightValid = 0;
				pResult->Alg_saveImg = 0;
				pResult->saveInfo.Alg_saveImgIndex = 0;					
				pResult->resultSize =  0;

            	/* Put the full buffer into full queue */
            	status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
            	UTILS_assert(status == FVID2_SOK);
            }	
		}			
        /* Put the empty buffers back to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,pInQueParams->prevLinkQueId, &frameList);		
    }

    return FVID2_SOK;
} 


Int32 ImgLink_SkipFrames(ImgLink_Obj * pObj)
{
    FVID2_FrameList frameList;
    System_LinkInQueParams *pInQueParams;
	
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {       		
        /* Put the empty buffers back to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,pInQueParams->prevLinkQueId, &frameList);		
    }

    return FVID2_SOK;
} 
/* Img link task main function */
/* ===================================================================
 *  @func     ImgLink_tskMain                                               
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
Void ImgLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done,doAlg,algDone,NormalMode;
    Int32 status;
    static unsigned int first = 1;
    ImgLink_Obj *pObj = (ImgLink_Obj *) pTsk->appData;
//	Alg_Params *pPrm;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the img driver */
    status = ImgLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);
    if (status != FVID2_SOK)
        return;

    done   = FALSE;
    ackMsg = FALSE;
    doAlg = FALSE;
    algDone = TRUE;
    NormalMode = FALSE;

    unsigned int toAlgLink = FALSE;
	
    Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_CREATE);
    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done   = TRUE;
                ackMsg = TRUE;
                break;

            case SYSTEM_CMD_NEW_DATA:
            {
                Utils_tskAckOrFreeMsg(pMsg, status);
					
                if(TRUE==NormalMode)
                {
					if(TRUE == algDone)
					{
						toAlgLink = TRUE;
						algDone = FALSE;
					}
					else
					toAlgLink = FALSE;	
                }
                else
                {
                	if((TRUE==doAlg)&&(TRUE==algDone))
                	{
						algDone = FALSE;
						doAlg = FALSE;
						toAlgLink = TRUE;
                	}
                	else
                		toAlgLink = FALSE;
                }
		

              if(TRUE == toAlgLink)
			  {
				ImgLink_processFrames(pObj);
                Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_NEW_DATA);
			 }		
			else
			{
				if(first<100)
				{
					ImgLink_processFrames(pObj);
					System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);	
					first++;
				}
				else if(FALSE==NormalMode)
				{
					ImgLink_processFrames(pObj);
					System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);		
				}
				else
					ImgLink_SkipFrames(pObj);
        	}                          
        	break;
        }
	    case IMGLINK_CMD_ALG_ENABLE:
	    {
			doAlg = TRUE;
			Utils_tskAckOrFreeMsg(pMsg, status);
			break;
	    }
	    case IMGLINK_CMD_ALG_DISABLE:
	    {
			doAlg = 	FALSE;
			Utils_tskAckOrFreeMsg(pMsg, status);
			break;		
	    }				
	   case IMGLINK_CMD_STATIC_IMG:
	   {
			pObj->useStaticImg = TRUE;
			Utils_tskAckOrFreeMsg(pMsg, status);
			break;
		}
		case IMGLINK_CMD_SENSOR_IMG:
		{
			pObj->useStaticImg = FALSE;	
			Utils_tskAckOrFreeMsg(pMsg, status);
			break;	
		}
		case IMGLINK_CMD_PROCESS_IMG:
		{		
			Utils_tskSendCmd(&pObj->processTsk,SYSTEM_IMG_OUTPUT_SOURCE);
			Utils_tskAckOrFreeMsg(pMsg, status);
			break;	
		}						
		case SYSTEM_IMG_ALG_DONE:
		{
			algDone = TRUE;
			Utils_tskAckOrFreeMsg(pMsg, status);
			System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);  
			break;	
		}					
		case IMGLINK_CMD_GET_PARAM_SPACE:
		{
			Utils_tskAckOrFreeMsg(pMsg, status);
			AlgPrm_MsgBuf *pPrm = (AlgPrm_MsgBuf *)Utils_msgGetPrm(pMsg);						
			pPrm->srPtr = (unsigned int)(pObj->algPrmsrPtr);
			pPrm->size  = ALG_SHMEM_MAXSIZE;					
			break;		
		}
		case IMGLINK_CMD_SET_ALG_PARAM:
		{	
			AlgPrm_MsgBuf *pPrm = (AlgPrm_MsgBuf *)Utils_msgGetPrm(pMsg);	
			ImgAlg_PrmUpdate(&(pObj->algObj),(char *)(pObj->pAlgPrmShbuf),pPrm->size);			
			Utils_tskSendCmd(&pObj->processTsk,SYSTEM_IMG_ALG_UPDATE_PARAM);
			Utils_tskAckOrFreeMsg(pMsg, status);
			break;	
		}
		case IMGLINK_CMD_GET_ALG_PARAM:
		{
			AlgPrm_MsgBuf *pPrm = (AlgPrm_MsgBuf *)Utils_msgGetPrm(pMsg);			
			Alg_Params * pAlgPrm = (Alg_Params *)(pObj->pAlgPrmShbuf);			
			Utils_tskSendCmd(&pObj->processTsk,SYSTEM_IMG_ALG_GET_PARAM);
			ImgAlg_PrmGet(&(pObj->algObj),(char *)pAlgPrm);	
			pPrm->size = pAlgPrm->prmSize + sizeof(Alg_Params);	
			pPrm->offset = 0;			
			Utils_tskAckOrFreeMsg(pMsg, status);			
			break;
		}		
		case IMGLINK_CMD_NORMAL_MODE:
				NormalMode = TRUE;
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
		case IMGLINK_CMD_DEBUG_MODE:
				NormalMode = FALSE;
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;		
		case IMGLINK_CMD_UPDATE_DBGIMG:
		{		
			Utils_tskSendCmd(&pObj->processTsk,SYSTEM_IMG_UPDATE_DBGIMG);
			Utils_tskAckOrFreeMsg(pMsg, status);				
			break;
		}																				
		default:
		{
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }        
     }
  }

    /* Delete the img driver */
    ImgLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* Img link get info */

/* ===================================================================
 *  @func     ImgLink_getInfo
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
Int32 ImgLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    ImgLink_Obj *pObj = (ImgLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* Img get full frames */

/* ===================================================================
 *  @func     ImgLink_getFullFrames
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
Int32 ImgLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;
    ImgLink_Obj *pObj = (ImgLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Img put empty frames */

/* ===================================================================
 *  @func     ImgLink_putEmptyFrames
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
Int32 ImgLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
	Int32 status;
    ImgLink_Obj *pObj = (ImgLink_Obj *) pTsk->appData;

    status = Utils_bufPutEmpty(&pObj->outFrameQue, pFrameList);
    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Img Link init */

/* ===================================================================
 *  @func     ImgLink_init
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
Int32 ImgLink_init()
{

    Int32 status;

    System_LinkObj linkObj;

    UInt32 imgId;

    ImgLink_Obj *pObj;

    char tskName[32];

    for (imgId = 0; imgId < IMG_LINK_OBJ_MAX; imgId++)
    {
        pObj = &gImgLink_obj[imgId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = ImgLink_getFullFrames;
        linkObj.linkPutEmptyFrames = ImgLink_putEmptyFrames;
        linkObj.getLinkInfo = ImgLink_getInfo;

        System_registerLink(SYSTEM_LINK_ID_IMG + imgId, &linkObj);

        sprintf(tskName, "IMG%d", imgId);

        status = Utils_tskCreate(&pObj->tsk,
                                 ImgLink_tskMain,
                                 IMG_LINK_TSK_PRI,
                                 gImgLink_tskStack[imgId],
                                 IMG_LINK_TSK_STACK_SIZE, pObj, tskName);
	 	UTILS_assert(status == FVID2_SOK);
	 	
		/* Create DMVAL process task */
		sprintf(tskName, "IMGPROCESS%d", imgId);
        status = Utils_tskCreate(&pObj->processTsk,
                                 ImgLink_processTskMain,
                                 IMG_LINK_PROCESSTSK_PRI,
                                 gImgLink_processTskStack[imgId],
                                 IMG_LINK_PROCESSTSK_STACK_SIZE,pObj,tskName);
        UTILS_assert(status == FVID2_SOK);		 	  
    }

    return status;
}

/* Img link de-init */

/* ===================================================================
 *  @func     ImgLink_deInit
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
Int32 ImgLink_deInit()
{
    UInt32 imgId;

    for (imgId = 0; imgId < IMG_LINK_OBJ_MAX; imgId++)
    {
        Utils_tskDelete(&gImgLink_obj[imgId].tsk);
        Utils_tskDelete(&gImgLink_obj[imgId].processTsk);
    }

    return FVID2_SOK;
}
