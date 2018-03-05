/** ==================================================================
 *  @file   ispLink_tsk.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/isp/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "ispLink_priv.h"

#pragma DATA_ALIGN(gIspLink_tskStack, 32)
#pragma DATA_SECTION(gIspLink_tskStack, ".bss:taskStackSection")
UInt8 gIspLink_tskStack[ISP_LINK_OBJ_MAX][ISP_LINK_TSK_STACK_SIZE];

IspLink_Obj gIspLink_obj[ISP_LINK_OBJ_MAX];

/* Isp link set frame rate */
Void IspLink_setFrameRateMask(IspLink_Obj *pObj,UInt32 sensorFps,
                              UInt32 queId,UInt32 targetFps)
{
	if ((queId == 1) && (pObj->createArgs.outQueuePrm[1].standard == SYSTEM_STD_PAL) && (targetFps == 30))
	{
        targetFps = 25;
	}

    pObj->frameSkipContext[queId].firstTime       = TRUE;
	pObj->frameSkipContext[queId].inputFrameRate  = sensorFps;
	pObj->frameSkipContext[queId].outputFrameRate = targetFps;

#ifdef SYSTEM_DEBUG_ISP
	Vps_printf(" %d: ISP   : Set Frame rate input = %d,output = %d for %d queue!!!\n", Clock_getTicks(),
	           sensorFps,targetFps,queId);
#endif
}

/* Isp link drv create */

/* ===================================================================
 *  @func     IspLink_drvCreate
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
Int32 IspLink_drvCreate(IspLink_Obj * pObj,
                        IspLink_CreateParams * pPrm)
{
    Int32 frameId;
    Int32 status;
    UInt32 queId, chId;
	UInt32 horzLineOffset,vertLineOffset;
    FVID2_Frame *pFrame;
    System_LinkQueInfo *pOutQueInfo;
    System_LinkChInfo *pInChInfo, *pOutChInfo;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    pObj->receviedFrameCount = 0;

    /* Get the link info from prev link */
    status = System_linkGetInfo(pObj->createArgs.inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pObj->createArgs.inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pObj->createArgs.inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* No of channels assumed is 1 */
    UTILS_assert(pObj->inQueInfo.numCh == 1);

    /* check for num out queues */
    UTILS_assert(pObj->createArgs.numOutQueue <= ISPLINK_MAX_OUT_QUE);

    for(queId = 0;queId < pObj->createArgs.numOutQueue;queId++)
	{
		/* Create the output buffers */
		status = Utils_bufCreate(&pObj->outFrameQue[queId],    // pHndl
								 FALSE,                        // blockOnGet
								 FALSE);                       // blockOnPut

		UTILS_assert(status == FVID2_SOK);

		if(pObj->createArgs.vsEnable == TRUE)
		{
		    horzLineOffset = (pObj->createArgs.outQueuePrm[queId].width * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
			vertLineOffset = (pObj->createArgs.outQueuePrm[queId].height * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
		}
		else
		{
		    horzLineOffset = pObj->createArgs.outQueuePrm[queId].width ;
			vertLineOffset = pObj->createArgs.outQueuePrm[queId].height;
		}

		if((pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_UYVY) ||
		   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YUYV) ||
		   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YVYU) ||
		   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_VYUY))
		{
		    pObj->pitch[queId][0] = horzLineOffset * 2;
			pObj->pitch[queId][1] = horzLineOffset * 2;
		}
		else
		{
		    pObj->pitch[queId][0] = horzLineOffset;
			pObj->pitch[queId][1] = horzLineOffset;
		}

		if (TRUE == pObj->createArgs.outQueuePrm[queId].tilerEnable)
		{
			if((pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_UYVY) ||
			   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YUYV) ||
			   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YVYU) ||
			   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_VYUY))
			{
				pObj->createArgs.outQueuePrm[queId].tilerEnable = FALSE;
			}
			else
			{
				pObj->pitch[queId][0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
				pObj->pitch[queId][1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
			}
 		}

		/*
		 *	When VNF is ON then padding of 32 pixels and 32 lines is required.
		 *  So the pitch is increased by 64.
         */		 
		if (queId == 0) 
		{
			pObj->pitch[queId][0] += 64;
			pObj->pitch[queId][1] += 64;
		}

		/* Allocate memory for the buffers */
		pObj->bufferFrameFormat[queId].width          = horzLineOffset;
		pObj->bufferFrameFormat[queId].height         = vertLineOffset;
		
		/*
		 *	When NF mode is changed between NSF and VNF dynamically 
		 *  then capture format is changed to YUV422ILE and YUV420SP respectively. 	 
		 *  We need to allocate memory big enough for both formats. 
		 */	
		pObj->bufferFrameFormat[queId].dataFormat     = (FVID2_DF_YUV422I_UYVY | FVID2_DF_YUV420SP_UV);	
		pObj->bufferFrameFormat[queId].pitch[0]       = pObj->pitch[queId][0];
		pObj->bufferFrameFormat[queId].pitch[1]       = pObj->pitch[queId][1];
		pObj->bufferFrameFormat[queId].pitch[2]	      = 0;
		pObj->bufferFrameFormat[queId].scanFormat     = FVID2_SF_PROGRESSIVE;
		pObj->bufferFrameFormat[queId].channelNum     = 0;
		pObj->bufferFrameFormat[queId].fieldMerged[0] = FALSE;
        pObj->bufferFrameFormat[queId].fieldMerged[1] = FALSE;
        pObj->bufferFrameFormat[queId].fieldMerged[2] = FALSE;
		
		if((pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_UYVY) || 
		   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YUYV) ||		
		   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YVYU) ||		
		   (pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_VYUY))
		{
			pObj->bufferFrameFormat[queId].pitch[0] /= 2;
			pObj->bufferFrameFormat[queId].pitch[1] /= 2;
		}		

		if (queId == 0) 
		{
			pObj->bufferFrameFormat[queId].width += 32;
			pObj->bufferFrameFormat[queId].height += 32;
		}

		status = Utils_memFrameAlloc_capture(&pObj->bufferFrameFormat[queId],	// pFormat
									         pObj->outFrames[queId],       		// pFrame
									         ISP_LINK_MAX_OUT_FRAMES,			// numFrames
											 0,									// Padding
											 0);								// BoxCarDataSize

		UTILS_assert(status == FVID2_SOK);

		/* Queue all the buffers to empty queue */
		for (frameId = 0; frameId < ISP_LINK_MAX_OUT_FRAMES; frameId++)
		{
			pFrame = &pObj->outFrames[queId][frameId];
			pFrame->appData = (System_FrameInfo*)&pObj->frameInfo[queId][frameId];

			/* Per Frame Config */
			pObj->perFrameCfg[queId][frameId].queueId     = queId;
			pObj->perFrameCfg[queId][frameId].frameWidth  = pObj->createArgs.outQueuePrm[queId].width;
			pObj->perFrameCfg[queId][frameId].frameHeight = pObj->createArgs.outQueuePrm[queId].height;
			pObj->perFrameCfg[queId][frameId].dataFormat  = (FVID2_DataFormat)pObj->createArgs.outQueuePrm[queId].dataFormat;

			if ((queId == 0) && (pObj->createArgs.vnfFullResolution))
			{
				pObj->perFrameCfg[queId][frameId].frameWidth += 32;
				pObj->perFrameCfg[queId][frameId].frameHeight += 32;
			}

			pFrame->perFrameCfg = (Iss_IspPerFrameCfg*)&pObj->perFrameCfg[queId][frameId];

			status = Utils_bufPutEmptyFrame(&pObj->outFrameQue[queId], pFrame);
			UTILS_assert(status == FVID2_SOK);
		}

		/* Frame skip context init */
		pObj->frameSkipContext[queId].firstTime       = TRUE;
		pObj->frameSkipContext[queId].inputFrameRate  = ISP_LINK_DEFAULT_FRAMERATE;
		pObj->frameSkipContext[queId].outputFrameRate = ISP_LINK_DEFAULT_FRAMERATE;
	}

    /* Create the isp driver instance */
    status = IspLink_drvCreateFvidObj(pObj);
    UTILS_assert(status == FVID2_SOK);

    /* Populating the link info structure */
    pObj->info.numQue = pObj->createArgs.numOutQueue;

    for(queId = 0; queId < pObj->info.numQue; queId++)
    {
        pOutQueInfo = &pObj->info.queInfo[queId];
        pObj->info.queInfo[queId].numCh = pObj->inQueInfo.numCh;

        for (chId = 0; chId < pObj->info.queInfo[queId].numCh; chId++)
        {
            pInChInfo = &pObj->inQueInfo.chInfo[chId];
            pOutChInfo = &pOutQueInfo->chInfo[chId];

            pOutChInfo->bufType      = pInChInfo->bufType;
            pOutChInfo->codingformat = pInChInfo->codingformat;
            pOutChInfo->dataFormat   = pObj->createArgs.outQueuePrm[queId].dataFormat;
            pOutChInfo->memType      = pInChInfo->memType;
            pOutChInfo->startX       = 0;
            pOutChInfo->startY       = 0;
            pOutChInfo->width        = pObj->createArgs.outQueuePrm[queId].width;
            pOutChInfo->height       = pObj->createArgs.outQueuePrm[queId].height;
            pOutChInfo->pitch[0]     = pObj->pitch[queId][0];
			pOutChInfo->pitch[1]     = pObj->pitch[queId][1];
            pOutChInfo->scanFormat   = pInChInfo->scanFormat;

			if ((queId == 0) && (pObj->createArgs.vnfFullResolution))
			{
				pOutChInfo->width += 32;
				pOutChInfo->height += 32;
			}
        }
    }

#ifdef SYSTEM_DEBUG_ISP
    Vps_printf(" %d: ISP   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Isp link drv delete */

/* ===================================================================
 *  @func     IspLink_drvDelete
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
Int32 IspLink_drvDelete(IspLink_Obj * pObj)
{
    Int32 status;
	UInt32 queId;

    for(queId = 0;queId < pObj->createArgs.numOutQueue;queId++)
	{
		/* Free the frame memory */
		status = Utils_memFrameFree_Capture(&pObj->bufferFrameFormat[queId],// pFormat
									        pObj->outFrames[queId],        	// pFrame
									        ISP_LINK_MAX_OUT_FRAMES,    	// numFrames
											0,								// Padding
											0);								// BoxCarDataSize

		UTILS_assert(status == FVID2_SOK);

		/* Delete the buffers */
		status = Utils_bufDelete(&pObj->outFrameQue[queId]);

		UTILS_assert(status == FVID2_SOK);
	}

    /* Delete the isp driver instance */
    status = IspLink_drvDeleteFvidObj(pObj);

    UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_ISP
    Vps_printf(" %d: ISP   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Isp link task main function */

/* ===================================================================
 *  @func     IspLink_tskMain
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
Void IspLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
	IspLink_FpsParams *pFpsParams;
	IspLink_outputDataFormat *pOutDataFmt;				

    IspLink_Obj *pObj = (IspLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the isp driver */
    status = IspLink_drvCreate(pObj, Utils_msgGetPrm(pMsg));

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
		   case SYSTEM_CMD_STOP:
				done = TRUE;
				ackMsg = TRUE;
				break;
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);

                IspLink_drvProcessFrames(pObj);
                break;
			case ISPLINK_CMD_CHANGE_FRAMERATE:
				pFpsParams = (IspLink_FpsParams *)Utils_msgGetPrm(pMsg);
			    Utils_tskAckOrFreeMsg(pMsg, status);

				IspLink_setFrameRateMask(pObj,pFpsParams->sensorFps,
				                         pFpsParams->streamId,pFpsParams->targetFps);
			    break;
			case ISPLINK_CMD_MIRROR_MODE:
				pObj->mirrorMode = *(UInt32 *)Utils_msgGetPrm(pMsg);
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
			case ISPLINK_CMD_SET_OUTDATAFMT:
				pOutDataFmt = (IspLink_outputDataFormat*)Utils_msgGetPrm(pMsg);				
				IspLink_setOutputDataFormat(pObj,pOutDataFmt);				
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;					
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the isp driver */
    IspLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* Isp link get info */

/* ===================================================================
 *  @func     IspLink_getInfo
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
Int32 IspLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    IspLink_Obj *pObj = (IspLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* Isp get full frames */

/* ===================================================================
 *  @func     IspLink_getFullFrames
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
Int32 IspLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;

    IspLink_Obj *pObj = (IspLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->outFrameQue[queId], pFrameList, BIOS_NO_WAIT);

    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Isp put empty frames */

/* ===================================================================
 *  @func     IspLink_putEmptyFrames
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
Int32 IspLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
    Int32 status;

    IspLink_Obj *pObj = (IspLink_Obj *) pTsk->appData;

    status = Utils_bufPutEmpty(&pObj->outFrameQue[queId], pFrameList);

    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Isp Link init */

/* ===================================================================
 *  @func     IspLink_init
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
Int32 IspLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 ispId;

    IspLink_Obj *pObj;

    char tskName[32];

    for (ispId = 0; ispId < ISP_LINK_OBJ_MAX; ispId++)
    {
        pObj = &gIspLink_obj[ispId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = IspLink_getFullFrames;
        linkObj.linkPutEmptyFrames = IspLink_putEmptyFrames;
        linkObj.getLinkInfo = IspLink_getInfo;

        System_registerLink(SYSTEM_LINK_ID_ISP_0 + ispId, &linkObj);

        sprintf(tskName, "ISP%d", ispId);

        status = Utils_tskCreate(&pObj->tsk,
                                 IspLink_tskMain,
                                 ISP_LINK_TSK_PRI,
                                 gIspLink_tskStack[ispId],
                                 ISP_LINK_TSK_STACK_SIZE, pObj, tskName);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* Isp link de-init */

/* ===================================================================
 *  @func     IspLink_deInit
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
Int32 IspLink_deInit()
{
    UInt32 ispId;

    for (ispId = 0; ispId < ISP_LINK_OBJ_MAX; ispId++)
    {
        Utils_tskDelete(&gIspLink_obj[ispId].tsk);
    }

    return FVID2_SOK;
}
