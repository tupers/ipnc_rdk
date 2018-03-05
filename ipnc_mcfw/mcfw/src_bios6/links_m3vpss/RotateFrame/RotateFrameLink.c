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

#include "RotateFrameLink_priv.h"

#pragma DATA_ALIGN(gRotateFrameLink_tskStack, 32)
#pragma DATA_SECTION(gRotateFrameLink_tskStack, ".bss:taskStackSection")
UInt8 gRotateFrameLink_tskStack[ROTATEFRAME_LINK_OBJ_MAX][IMG_LINK_TSK_STACK_SIZE];

RotateFrameLink_Obj gRotateFrameLink_obj[ROTATEFRAME_LINK_OBJ_MAX];

Int32 RotateFrame_Init(RotateFrameLink_Obj * pObj)
{
	System_getEdmaCh(&pObj->copyEdmaHndl);

	pObj->isNeedRotateFrame = TRUE;

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
Int32 RotateFrameLink_create(RotateFrameLink_Obj * pObj, RotateFrameLink_CreateParams * pPrm)
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

    status = Utils_memFrameAlloc(&pObj->bufferFrameFormat, // pFormat
		    pObj->outFrames,          // pFrame
		    ROTATEFRAME_LINK_MAX_OUT_FRAMES);  // numFrames
    UTILS_assert(status == FVID2_SOK);

    /* Queue all the buffers to empty queue */
    for (frameId = 0; frameId < ROTATEFRAME_LINK_MAX_OUT_FRAMES; frameId++)
    {
	    pFrame = &pObj->outFrames[frameId];
	    pFrame->appData = &pObj->freeFrameInfoMem[frameId];
	    status = Utils_bufPutEmptyFrame(&pObj->outFrameQue, pFrame);
	    UTILS_assert(status == FVID2_SOK);
    }	
	
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

	RotateFrame_Init(pObj);
	
    Vps_printf(" %d: RotateFrame Link   : Create Done !!!\n", Clock_getTicks());

    return FVID2_SOK;
}

Int32 RotateFrame(RotateFrameLink_Obj * pObj,FVID2_Frame  *dst,FVID2_Frame  *src)
{
	System_FrameInfo *pSrcfrmInfo;
	System_FrameInfo *pDstfrmInfo;	 
	pSrcfrmInfo  = src->appData;
	pDstfrmInfo  = dst->appData;

	unsigned srcAddr = (UInt32)(src->addr[0][0]);
	unsigned dstAddr = (UInt32)(dst->addr[0][0]);
	unsigned pitch = pSrcfrmInfo->rtChInfo.pitch[0];
	unsigned width = pSrcfrmInfo->rtChInfo.width;
	unsigned height = pSrcfrmInfo->rtChInfo.height;

	int i;
	for(i=0;i<height;i++)
	{
		DM81XX_EDMA3_setParams(pObj->copyEdmaHndl.dmaHndl.chId,0,srcAddr,dstAddr,1,width,pitch,1);
		DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndl.dmaHndl.chId);
       	srcAddr += pitch;
       	dstAddr += 1;                    		 
	}
	 	
	dst->fid = src->fid;
	dst->channelNum = 0;
	dst->timeStamp = src->timeStamp; 
	pDstfrmInfo->rtChInfo.width = pSrcfrmInfo->rtChInfo.height;
	pDstfrmInfo->rtChInfo.height = pSrcfrmInfo->rtChInfo.width;
	pDstfrmInfo->rtChInfo.pitch[0] = pSrcfrmInfo->rtChInfo.height;
	pDstfrmInfo->rtChInfo.pitch[1] = pSrcfrmInfo->rtChInfo.pitch[1];

	pDstfrmInfo->rtChInfoUpdate = TRUE;

	return 0;	 
}

Int32 RotateFrameLink_CopyFrame(RotateFrameLink_Obj * pObj,FVID2_Frame  *dst,FVID2_Frame  *src)
{
	System_FrameInfo *pSrcfrmInfo;
	System_FrameInfo *pDstfrmInfo;	 
	pSrcfrmInfo  = src->appData;
	pDstfrmInfo  = dst->appData;

	unsigned srcAddr = (UInt32)(src->addr[0][0]);
	unsigned dstAddr = (UInt32)(dst->addr[0][0]);
	unsigned pitch = pSrcfrmInfo->rtChInfo.pitch[0];
	unsigned width = pSrcfrmInfo->rtChInfo.width;
	unsigned height = pSrcfrmInfo->rtChInfo.height;

	DM81XX_EDMA3_setParams(pObj->copyEdmaHndl.dmaHndl.chId,0,srcAddr,dstAddr,width,width,pitch,pitch);
	DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndl.dmaHndl.chId);

	 	
	dst->fid = src->fid;
	dst->channelNum = 0;
	dst->timeStamp = src->timeStamp; 
	pDstfrmInfo->rtChInfo.width = width;
	pDstfrmInfo->rtChInfo.height = height;
	pDstfrmInfo->rtChInfo.pitch[0] = pitch;
	pDstfrmInfo->rtChInfo.pitch[1] = pSrcfrmInfo->rtChInfo.pitch[1];

	pDstfrmInfo->rtChInfoUpdate = TRUE;

	return 0;	 
}
/* ===================================================================
 *  @func     RotateFrameLink_ProcessFrames
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
Int32 RotateFrameLink_ProcessFrames(RotateFrameLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;
    UInt32 frameId;
    FVID2_FrameList frameList;
    FVID2_Frame *pFullFrame,*pEmptyFrame;
    System_LinkInQueParams *pInQueParams;
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
        	pFullFrame = frameList.frames[frameId];


			/* Get the empty frame from out queue */
			status = Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame, BIOS_NO_WAIT);
			if((status != FVID2_SOK) || (pEmptyFrame == NULL))
			{
				/* Empty buffer is not avialbale,return the full frames back */
				System_putLinksEmptyFrames(pInQueParams->prevLinkId,pInQueParams->prevLinkQueId, &frameList);				
											  
				return FVID2_SOK;							  
			}
	   		if(TRUE==pObj->isNeedRotateFrame)
			{
				RotateFrame(pObj,pEmptyFrame,pFullFrame);
			}
			else
				RotateFrameLink_CopyFrame(pObj,pEmptyFrame,pFullFrame);
				
            /* Put the full buffer into full queue */
            status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
            UTILS_assert(status == FVID2_SOK);
		}

        /* Put the empty buffers back to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,pInQueParams->prevLinkQueId, &frameList);		
    }

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
Int32 RotateFrameLink_delete(RotateFrameLink_Obj * pObj)
{
    Int32 status;	

    /* Free the frame memory */
    status = Utils_memFrameFree(&pObj->bufferFrameFormat,  	// pFormat
                                pObj->outFrames,           	// pFrame
                                ROTATEFRAME_LINK_MAX_OUT_FRAMES);   // numFrames
    UTILS_assert(status == FVID2_SOK);	
	
    /* Delete the buffers */
    status = Utils_bufDelete(&pObj->outFrameQue);
	
#ifdef SYSTEM_DEBUG_IMG
	Vps_printf(" %d: RotateFrameLink   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}
 
/* Img link task main function */
/* ===================================================================
 *  @func     RotateFrameLink_tskMain                                               
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
Void RotateFrameLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    RotateFrameLink_Obj *pObj = (RotateFrameLink_Obj *) pTsk->appData;
    UInt32 flushCmds[2];

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the img driver */
    status = RotateFrameLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done   = FALSE;
    ackMsg = FALSE;
 
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
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);
				RotateFrameLink_ProcessFrames(pObj);				
				/* send SYSTEM_CMD_NEW_DATA to next link */
    		    System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);     
                break;
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the img driver */
    RotateFrameLink_delete(pObj);

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
Int32 RotateFrameLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    RotateFrameLink_Obj *pObj = (RotateFrameLink_Obj *) pTsk->appData;

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
Int32 RotateFrameLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;
    RotateFrameLink_Obj *pObj = (RotateFrameLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Img put empty frames */

/* ===================================================================
 *  @func     RotateFrameLink_putEmptyFrames
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
Int32 RotateFrameLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
	Int32 status;
    RotateFrameLink_Obj *pObj = (RotateFrameLink_Obj *) pTsk->appData;

    status = Utils_bufPutEmpty(&pObj->outFrameQue, pFrameList);
    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Img Link init */

/* ===================================================================
 *  @func     RotateFrameLink_init
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
Int32 RotateFrameLink_init()
{

    Int32 status;

    System_LinkObj linkObj;

    UInt32 imgId;

    RotateFrameLink_Obj *pObj;

    char tskName[32];

    for (imgId = 0; imgId < ROTATEFRAME_LINK_OBJ_MAX; imgId++)
    {
        pObj = &gRotateFrameLink_obj[imgId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = RotateFrameLink_getFullFrames;
        linkObj.linkPutEmptyFrames = RotateFrameLink_putEmptyFrames;
        linkObj.getLinkInfo = RotateFrameLink_getInfo;

        System_registerLink(SYSTEM_VPSS_LINK_ROTATE_FRAME + imgId, &linkObj);
		
        sprintf(tskName, "IMG%d", imgId);

        status = Utils_tskCreate(&pObj->tsk,
                                 RotateFrameLink_tskMain,
                                 IMG_LINK_TSK_PRI,
                                 gRotateFrameLink_tskStack[imgId],
                                 IMG_LINK_TSK_STACK_SIZE, pObj, tskName);
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
Int32 RotateFrameLink_deInit()
{
    UInt32 imgId;

    for (imgId = 0; imgId < ROTATEFRAME_LINK_OBJ_MAX; imgId++)
    {
        Utils_tskDelete(&gRotateFrameLink_obj[imgId].tsk);
    }

    return FVID2_SOK;
}
