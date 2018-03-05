/** ==================================================================
 *  @file   fdLink_tsk.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/fd/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "fdLink_priv.h"

#pragma DATA_ALIGN(gFdLink_tskStack, 32)
#pragma DATA_SECTION(gFdLink_tskStack,".bss:taskStackSection")
UInt8 gFdLink_tskStack[FD_LINK_TSK_STACK_SIZE];

extern Int32 SwosdLink_postFrameSyncSem(UInt32 linkId);

FdLink_Obj gFdLink_obj = {.algHndl = {.fdetect = 0} };

/* FD Set GUI params */

/* ===================================================================
 *  @func     FdLink_setGuiPrm                                               
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
Void FdLink_setGuiPrm(UInt32 streamId, UInt32 * pPrm)
{
    DM812X_FD_setPrm(&gFdLink_obj.algHndl, pPrm);
}

Int32 FdLink_drvPrintRtStatus(FdLink_Obj * pObj)
{

    Vps_rprintf(" %d: FD:  Latency (Min / Max) = ( %d / %d ) !!! \r\n",
         Utils_getCurTimeInMsec(),
        pObj->minLatency,
        pObj->maxLatency
        );

    /* reset max time */

    pObj->maxLatency = 0;
    pObj->minLatency = 0xFF;

    return 0;
}

/* FD link create */

/* ===================================================================
 *  @func     FdLink_create                                               
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
Int32 FdLink_create(FdLink_Obj * pObj, FdLink_CreateParams * pPrm)
{
    Int32 status;

    Int32 frameId;

    FVID2_Frame *pFrame;

    /* copy the create params */
    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    /* Get the prev link info */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue,           // pHndl
                             FALSE,                        // blockOnGet
                             FALSE);                       // blockOnPut

    UTILS_assert(status == FVID2_SOK);

    /* Allocate memory for the buffers */
    pObj->bufferFrameFormat.width = pObj->inQueInfo.chInfo[0].width;
    pObj->bufferFrameFormat.height = pObj->inQueInfo.chInfo[0].height;
    pObj->bufferFrameFormat.dataFormat = pObj->inQueInfo.chInfo[0].dataFormat;
    pObj->bufferFrameFormat.pitch[0] = pObj->inQueInfo.chInfo[0].pitch[0];
    pObj->bufferFrameFormat.pitch[1] = pObj->inQueInfo.chInfo[0].pitch[1];

    status = Utils_memFrameAlloc(&pObj->bufferFrameFormat, // pFormat
                                 pObj->outFrames,          // pFrame
                                 FD_LINK_MAX_OUT_FRAMES);  // numFrames

    UTILS_assert(status == FVID2_SOK);

    /* Queue all the buffers to empty queue */
    for (frameId = 0; frameId < FD_LINK_MAX_OUT_FRAMES; frameId++)
    {
        pFrame = &pObj->outFrames[frameId];
        status = Utils_bufPutEmptyFrame(&pObj->outFrameQue, pFrame);

        UTILS_assert(status == FVID2_SOK);
    }

	System_getEdmaCh(&pObj->edmaHndl);
	
    /* Create FD algorithm instance */
    pObj->algHndl.fdWidth = pObj->inQueInfo.chInfo[0].width;
    pObj->algHndl.fdHeight = pObj->inQueInfo.chInfo[0].height;

    UTILS_assert(pObj->algHndl.fdWidth <= FD_LINK_MAX_WIDTH);
    UTILS_assert(pObj->algHndl.fdHeight <= FD_LINK_MAX_HEIGHT);
	pObj->minLatency = 0xFF;
	pObj->maxLatency = 0;
    status = DM812X_FD_AlgCreate(&pObj->algHndl);

    UTILS_assert(status == FVID2_SOK);
		
#ifdef SYSTEM_DEBUG_FD
    Vps_printf(" %d: FD   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* FD link delete */

/* ===================================================================
 *  @func     FdLink_delete                                               
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
Int32 FdLink_delete(FdLink_Obj * pObj)
{
    Int32 status;

    /* Free the frame memory */
    status = Utils_memFrameFree(&pObj->bufferFrameFormat,  // pFormat
                                pObj->outFrames,           // pFrame
                                FD_LINK_MAX_OUT_FRAMES);   // numFrames

    UTILS_assert(status == FVID2_SOK);

    /* Delete the buffers */
    status = Utils_bufDelete(&pObj->outFrameQue);

    UTILS_assert(status == FVID2_SOK);

    /* Delete the FD algorithm instance */
    status = DM812X_FD_AlgDelete(&pObj->algHndl);

    UTILS_assert(status == FVID2_SOK);
	
	System_putEdmaCh(&pObj->edmaHndl);

#ifdef SYSTEM_DEBUG_FD
    Vps_printf(" %d: FD   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* FD link copy frames */

/* ===================================================================
 *  @func     FdLink_copyFrames                                               
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
Int32 FdLink_copyFrames(FdLink_Obj * pObj)
{
    Int32 status;

    Int32 frameId;

    UInt32 edmaWidth, edmaHeight;

    FVID2_Frame *pEmptyFrame;

    FVID2_Frame *pFullFrame;

    FVID2_FrameList frameList;

    System_LinkInQueParams *pInQueParams;

    /* Get the full frames from the previous link */
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            /* Get the FD empty bufs if any */
            status =
                Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame,
                                       BIOS_NO_WAIT);

            if ((status == FVID2_SOK) && (pEmptyFrame != NULL))
            {
                pFullFrame = frameList.frames[frameId];

                /* copy the frame */
                edmaWidth = pObj->algHndl.fdWidth;
                edmaHeight = pObj->algHndl.fdHeight;
                pEmptyFrame->timeStamp = pFullFrame->timeStamp;

                /* copy only Y plane as FD works only on Y plane */
                DM81XX_EDMA3_setParams(pObj->edmaHndl.dmaHndl.chId, 		// chId
                                       FD_LINK_EDMA3_QUEUE_ID,  			// dmaQueue
                                       (UInt32) pFullFrame->addr[0][0], 	// srcAddr
                                       (UInt32) pEmptyFrame->addr[0][0],    // dstAddr
                                       edmaWidth,          					// edmaWidth
                                       edmaHeight,         					// edmaHeight
                                       edmaWidth,          					// srcLineOffset
                                       edmaWidth);         					// dstLineOffset

                /* Trigger the edma transfer */
                DM81XX_EDMA3_triggerTransfer(pObj->edmaHndl.dmaHndl.chId);

                /* put the buffer into full queue */
                status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);

                UTILS_assert(status == FVID2_SOK);
            }
        }

        /* Put the full buffers bacl to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }

    return FVID2_SOK;
}

/* FD link process frames */

/* ===================================================================
 *  @func     FdLink_processFrames                                               
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
Int32 FdLink_processFrames(FdLink_Obj * pObj)
{
    Int32 status;

	UInt32 latency;
    FVID2_Frame *pFullFrame;

    do
    {
        /* Get the frame from the full queue */
        status =
            Utils_bufGetFullFrame(&pObj->outFrameQue, &pFullFrame,
                                  BIOS_NO_WAIT);

        if ((status == FVID2_SOK) && (pFullFrame != NULL))
        {
            /* apply FD algorithm */
            status =
                DM812X_FD_AlgProcess(&pObj->algHndl, pFullFrame->addr[0][0]);

            UTILS_assert(status == FVID2_SOK);

            /* put the buffer into empty queue */
            status = Utils_bufPutEmptyFrame(&pObj->outFrameQue, pFullFrame);

            UTILS_assert(status == FVID2_SOK);
			
			latency = Utils_getCurTimeInMsec() - pFullFrame->timeStamp;

			if(latency>pObj->maxLatency)
				pObj->maxLatency = latency;
			if(latency<pObj->minLatency)
				pObj->minLatency = latency;
        }
    } while ((status == FVID2_SOK) && (pFullFrame != NULL));

    return FVID2_SOK;
}

/* FD link task main function */

/* ===================================================================
 *  @func     FdLink_tskMain                                               
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
Void FdLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status;

    FdLink_Obj *pObj = (FdLink_Obj *) pTsk->appData;

    FdLink_GuiParams *pFdGuiPrm;

    UInt32 fdPrm[8];

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* Allocate memory for FD buffers */
    status = FdLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done = FALSE;
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
                done = TRUE;
                ackMsg = TRUE;
                break;
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);

                FdLink_copyFrames(pObj);
                FdLink_processFrames(pObj);
                break;
            case FD_CMD_PRM:
                pFdGuiPrm = (FdLink_GuiParams *) Utils_msgGetPrm(pMsg);

                fdPrm[0] = pFdGuiPrm->fdetect;
                fdPrm[1] = pFdGuiPrm->startX;
                fdPrm[2] = pFdGuiPrm->startY;
                fdPrm[3] = pFdGuiPrm->width;
                fdPrm[4] = pFdGuiPrm->height;
                fdPrm[5] = pFdGuiPrm->fdconflevel;
                fdPrm[6] = pFdGuiPrm->fddirection;
                fdPrm[7] = pFdGuiPrm->pmask;

                FdLink_setGuiPrm(0, fdPrm);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
			case FD_LINK_CMD_PRINT_STATISTICS:

                FdLink_drvPrintRtStatus(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);

				break;
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the FD link instance */
    FdLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* FD link init */

/* ===================================================================
 *  @func     FdLink_init                                               
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
Int32 FdLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    FdLink_Obj *pObj;

    char tskName[32];

    pObj = &gFdLink_obj;

    memset(pObj, 0, sizeof(*pObj));

    linkObj.pTsk = &pObj->tsk;

    /* linkGetFullFrames,linkPutEmptyFrames and getLinkInfo will be NULL
     * since FD will be a terminating link */
    linkObj.linkGetFullFrames = NULL;
    linkObj.linkPutEmptyFrames = NULL;
    linkObj.getLinkInfo = NULL;

    System_registerLink(SYSTEM_LINK_ID_FD, &linkObj);

    sprintf(tskName, "FD");

    status = Utils_tskCreate(&pObj->tsk,
                             FdLink_tskMain,
                             FD_LINK_TSK_PRI,
                             gFdLink_tskStack,
                             FD_LINK_TSK_STACK_SIZE, pObj, tskName);

    UTILS_assert(status == FVID2_SOK);

    return status;
}

/* FD link de-init */

/* ===================================================================
 *  @func     FdLink_deInit                                               
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
Int32 FdLink_deInit()
{
    /* Delete FD task */
    Utils_tskDelete(&gFdLink_obj.tsk);

    return FVID2_SOK;
}

