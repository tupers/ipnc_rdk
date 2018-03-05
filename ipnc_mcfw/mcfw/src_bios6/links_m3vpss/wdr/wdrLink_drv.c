/** ==================================================================
 *  @file   wdrLink_drv.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/wdr/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <mcfw/src_bios6/utils/utils_mem.h>
#include <ti/psp/iss/alg/wdr/TI_WDR_Engine.h>

#include "wdrLink_priv.h"

/* ===================================================================
 *  @func     WdrLink_drvInitCh
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
static Int32 WdrLink_drvInitCh(WdrLink_Obj * pObj)
{
    Int32 status;

    UInt32 chId, frameId, queueId = WDR_LINK_OUT_QUE_ID;

    FVID2_Frame *frame;

    WdrLink_ChObj *WdrChObj;

    WdrCreateParams *chWdrCreateParams;

    WdrLink_ChCreateParams *chCreateParams;

    System_LinkChInfo *inChInfo, *outChInfo;

    /*
     * For each channel, do the initialization and
     * FVID2 frame / buffer allocation.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        WdrChObj = &pObj->linkChInfo[chId];
        chWdrCreateParams = &WdrChObj->chCreateArgs;
        chCreateParams = &pObj->createArgs.chCreateParams[chId];
        inChInfo = &pObj->inQueInfo.chInfo[chId];
        outChInfo = &pObj->info.queInfo[queueId].chInfo[chId];

        /* Set the channel number first */
        WdrChObj->channelId = chId;
        WdrChObj->inFrameSkipCount = 0;

        WDRLINK_INFO_LOG
            ("Input Channel CH%d of %d x %d, pitch = %d [%d,%d][%s] [%d] ... \n",
             chId, inChInfo->width, inChInfo->height, inChInfo->pitch[0],
             inChInfo->startX, inChInfo->startY,
             gSystem_nameScanFormat[inChInfo->scanFormat],
             inChInfo->dataFormat);

		UTILS_assert(inChInfo->dataFormat == FVID2_DF_BAYER_RAW || inChInfo->dataFormat == SYSTEM_DF_BAYER_RAW); //TO BE BAYER

		chCreateParams->sParams.computeWidth = inChInfo->width;
		chCreateParams->sParams.computeHeight = inChInfo->height;
 		chCreateParams->sParams.outputStride = inChInfo->pitch[0];

        chWdrCreateParams->ulInputStride = inChInfo->pitch[0];
		chWdrCreateParams->wdrMode = chCreateParams->dParams.wdrMode;
		chWdrCreateParams->update = 0;
        chWdrCreateParams->ulComputeWidth = chCreateParams->sParams.computeWidth;
        chWdrCreateParams->ulComputeHeight = chCreateParams->sParams.computeHeight;
        chWdrCreateParams->ulOutputStride = chCreateParams->sParams.outputStride;

        /*
         * Per channel bufInQue structure needs to be created & initialized.
         * bufInQue.fullQue will be populated with captured frames and
         * bufInQue.emptyQuewill be allocated here so that they can be
         * used later on for noise filtering.
         * Frames need to be allocated for bufInQue.emptyQue here.
         */
        /* Create the per channel bufInQue */
        status = Utils_bufCreate(&WdrChObj->bufInQue, TRUE, FALSE);
        UTILS_assert(status == FVID2_SOK);

        /* Fill format with channel specific values to allocate frame */
        WdrChObj->format.channelNum = chId;
        WdrChObj->format.width = chWdrCreateParams->ulComputeWidth;
        WdrChObj->format.height = chWdrCreateParams->ulComputeHeight;

        WdrChObj->format.pitch[0] = chWdrCreateParams->ulOutputStride;
        WdrChObj->format.pitch[1] = 0;
        WdrChObj->format.pitch[2] = 0;
        WdrChObj->format.fieldMerged[0] = FALSE;
        WdrChObj->format.fieldMerged[1] = FALSE;
        WdrChObj->format.fieldMerged[2] = FALSE;
        WdrChObj->format.dataFormat = FVID2_DF_BAYER_RAW;//SYSTEM_DF_BAYER_RAW;
        WdrChObj->format.scanFormat = FVID2_SF_PROGRESSIVE;
        WdrChObj->format.bpp = FVID2_BPP_BITS16;
		
        /*
         * Alloc memory based on 'format'
         * Allocated frame info is put in frames[]
         */
        Utils_memFrameAlloc(&WdrChObj->format, WdrChObj->frames,
                            WDR_LINK_MAX_OUT_FRAMES_PER_CH);

        /* Set remaining parameters for the allocated frames */
        for (frameId = 0; frameId < WDR_LINK_MAX_OUT_FRAMES_PER_CH; frameId++)
        {
            /* Point to the frame's array */
            frame = &WdrChObj->frames[frameId];
            frame->perFrameCfg = NULL;
            frame->subFrameInfo = NULL;
            frame->appData = &WdrChObj->frameInfo[frameId];

            WdrChObj->frameInfo[frameId].wdrChannelNum = frame->channelNum;

            /* Finally, add this frame to the WdrLink_ChObj.bufInQue.emptyQue
             */
            status = Utils_bufPutEmptyFrame(&WdrChObj->bufInQue, frame);
            UTILS_assert(status == FVID2_SOK);
        }

        /* Populate the remaining fileds of
         * WdrLink_Obj.System_LinkInfo.System_LinkQueInfo.System_LinkChInfo
         * This information will be used by the next link to configure itself
         * properly. */
        UTILS_assert(queueId < pObj->info.numQue);

        outChInfo->bufType = SYSTEM_BUF_TYPE_VIDFRAME;
        outChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
        outChInfo->dataFormat = WdrChObj->format.dataFormat;
        outChInfo->scanFormat = WdrChObj->format.scanFormat;

        outChInfo->startX = 0;
        outChInfo->startY = 0;

        outChInfo->width = WdrChObj->format.width;
        outChInfo->height = WdrChObj->format.height;
        outChInfo->pitch[0] = WdrChObj->format.pitch[0];
        outChInfo->pitch[1] = WdrChObj->format.pitch[1];
        outChInfo->pitch[2] = WdrChObj->format.pitch[2];

        /* Increase the number of channels now */
        pObj->info.queInfo[queueId].numCh++;
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     WdrLink_drvCreate
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
Int32 WdrLink_drvCreate(WdrLink_Obj * pObj, WdrLink_CreateParams * pPrm)
{
    Int32 status;
	WDR_ENGINE_STATIC_PARAMS sWdrParams;
	WdrLink_ChObj *WdrChObj;
	WdrCreateParams *chWdrCreateParams;

    WDRLINK_INFO_LOG("Create in progress ... !!!\n");

    /* Store the create params coming from the app */
    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->inFrameReceiveCount = 0;
    pObj->inFrameGivenCount = 0;
    pObj->outFrameReceiveCount = 0;
    pObj->outFrameGivenCount = 0;
    pObj->processFrameReqCount = 0;
    pObj->getProcessFrameReqCount = 0;
    pObj->totalTime = 0;
    pObj->curTime = 0;

    status = Utils_bufCreate(&pObj->bufOutQue, FALSE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    /* Copy previous link (capture) info */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* Copy previous link's queue info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));
    UTILS_assert(pObj->inQueInfo.numCh <= WDR_LINK_MAX_CH);

    pObj->info.numQue = 1;

    /*
     * Configure the channels, allocate frames for each channel and
     * put those frames in channel specific buffer queue.
     */
    WdrLink_drvInitCh(pObj);

	WdrChObj = &pObj->linkChInfo[0];
	chWdrCreateParams = &WdrChObj->chCreateArgs;
	
	sWdrParams.width = chWdrCreateParams->ulComputeWidth;
	sWdrParams.height = chWdrCreateParams->ulComputeHeight;
	sWdrParams.inputStride = chWdrCreateParams->ulInputStride;
	sWdrParams.outputStride = chWdrCreateParams->ulOutputStride;
	
#if defined(IMGS_SONY_IMX136)
	sWdrParams.wdrSensor.type = WDR_BAYER_RGGB;
#elif defined(IMGS_SONY_IMX104)
	sWdrParams.wdrSensor.type = WDR_BAYER_GBRG;
#elif defined IMGS_PANASONIC_MN34041
	sWdrParams.wdrSensor.type = WDR_BAYER_BGGR;
#else
	sWdrParams.wdrSensor.type = WDR_BAYER_GRBG;
#endif

#if defined(IMGS_MICRON_MT9M034) || defined(IMGS_SONY_IMX104) || defined (IMGS_SONY_IMX136) || defined (IMGS_MICRON_AR0331)
	sWdrParams.wdrSensor.isDataCompressed = 1;
#else
	sWdrParams.wdrSensor.isDataCompressed = 0;
#endif

	sWdrParams.wdrSensor.numKneePts = 4;
	sWdrParams.wdrSensor.blackLevel = 0;

#ifdef IMGS_MICRON_MT9M034
	sWdrParams.wdrSensor.slope[0] = 1;
	sWdrParams.wdrSensor.slope[1] = 4*16;    /* 4*R1 */
	sWdrParams.wdrSensor.slope[2] = 4*16*16;	/* 4*R1*R2 */
	sWdrParams.wdrSensor.thr[0] = 0;
	sWdrParams.wdrSensor.thr[1] = 2048;
	sWdrParams.wdrSensor.thr[2] = 3040;
	sWdrParams.wdrSensor.thr[3] = 4000;	
#elif defined(IMGS_SONY_IMX104) || defined (IMGS_SONY_IMX136)
	sWdrParams.wdrSensor.slope[0] = 1;
	sWdrParams.wdrSensor.slope[1] = 16;	   /* R1 */
	sWdrParams.wdrSensor.slope[2] = 16*4;  /* R1*R2 */
	sWdrParams.wdrSensor.thr[0] = 0;
	sWdrParams.wdrSensor.thr[1] = 2048;
	sWdrParams.wdrSensor.thr[2] = 2944;
	sWdrParams.wdrSensor.thr[3] = 3712;
#elif defined(IMGS_MICRON_AR0331)
	sWdrParams.wdrSensor.blackLevel = -168;
	sWdrParams.wdrSensor.numKneePts = 5;
	sWdrParams.wdrSensor.slope[0] = 1;
	sWdrParams.wdrSensor.slope[1] = 2;    /* R1 */
	sWdrParams.wdrSensor.slope[2] = 32;  /* R1*R2 */
	sWdrParams.wdrSensor.slope[3] = 64;  /* R1*R2*R3 */
	sWdrParams.wdrSensor.thr[0] = 0;
	sWdrParams.wdrSensor.thr[1] = 1024;
	sWdrParams.wdrSensor.thr[2] = 2560;
	sWdrParams.wdrSensor.thr[3] = 3456;
	sWdrParams.wdrSensor.thr[4] = 3968;
#else
	/* for test only */
	sWdrParams.wdrSensor.slope[0] = 1;
	sWdrParams.wdrSensor.slope[1] = 1;    /* R1 */
	sWdrParams.wdrSensor.slope[2] = 1;  /* R1*R2 */
	sWdrParams.wdrSensor.thr[0] = 0;
	sWdrParams.wdrSensor.thr[1] = 2048;
	sWdrParams.wdrSensor.thr[2] = 3072;	
	sWdrParams.wdrSensor.thr[3] = 4096;
#endif

    status = WDR_Engine_Init(&sWdrParams);
    UTILS_assert(status == MSP_ERROR_NONE);

    WDRLINK_INFO_LOG("Create ... DONE !!!\n");

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     WdrLink_drvDoWdrFilter
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
static Int32 WdrLink_drvDoWdr(WdrLink_Obj * pObj)
{
    FVID2_Frame *pFullFrame;

    FVID2_Frame *pEmptyFrame;

    Int32 status;

    UInt32 chId;

    UInt32 numFrames;

    WdrLink_ChObj *pChObj;
	
	WdrCreateParams *chWdrCreateParams;

	WDR_ENGINE_DYNAMIC_PARAMS dWdrParams;

    numFrames = 0;

    WDRLINK_INFO_LOG_RT("WDR: WdrLink_drvDoWdrFilter !!!\n");

    /** For all the available channels, look into the respective
     * bufInQue.fullQueue and take the frames out of that queue.
     * Take the same number of frames from the respective bufInQue.emptyQueue.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->linkChInfo[chId];
		chWdrCreateParams = &pChObj->chCreateArgs;

        while (1)
        {
            status = Utils_bufGetFullFrame(&pChObj->bufInQue,
                                           &pFullFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
                break;

            /* First check whether it can be accomodated or not */
            status = Utils_bufGetEmptyFrame(&pChObj->bufInQue,
                                            &pEmptyFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
            {
				pChObj->inFrameSkipCount++;
                WDRLINK_INFO_LOG_RT("WDR: Skip frames !!!\n");
                break;
            }

            pObj->processFrameReqCount++;

			dWdrParams.wdrMode = chWdrCreateParams->wdrMode;
			if (chWdrCreateParams->update)
			{
				dWdrParams.update = 1;
				chWdrCreateParams->update = 0;
			}
			else
			{
				dWdrParams.update = 0;
			}

            pObj->curTime = Utils_getCurTimeInUsec();

            WDRLINK_INFO_LOG_RT("Start %d WDR: processing frame %d !!!\n",
                                pObj->curTime, numFrames);

    pEmptyFrame->timeStamp = pFullFrame->timeStamp;

    //Vps_rprintf("Calling MSP_WDR_ProcessFrame \n");
            status =
                WDR_ProcessFrame(&dWdrParams,
                                     pFullFrame->addr[0][0], pEmptyFrame->addr[0][0]);
            UTILS_assert(status == FVID2_SOK);
    //Vps_rprintf("Came out of MSP_WDR_ProcessFrame \n");

            pObj->curTime = Utils_getCurTimeInUsec() - pObj->curTime;

            pObj->totalTime += pObj->curTime/100;

            pObj->getProcessFrameReqCount++;

			if (pObj->getProcessFrameReqCount % 100 == 0)
				WDRLINK_INFO_LOG("WDR processing is taking %.2f ms !!!\n",
                 (float)pObj->totalTime/pObj->getProcessFrameReqCount/10);

            /* Put the processed frames in the appropriate output queue */
                status =
                    Utils_bufPutFullFrame(&pObj->bufOutQue, pEmptyFrame);
                UTILS_assert(status == FVID2_SOK);
                WDRLINK_INFO_LOG_RT("WDR: Put frame %d to Out Que !!!\n",
                                    numFrames);
                numFrames++;
        }
    }

    if (numFrames)
    {
        pObj->outFrameGivenCount += numFrames;

        /* Send data available message to next task */
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
        WDRLINK_INFO_LOG_RT("WDR: Send %d frames to the next link !!!\n",
                            numFrames);
    }
    else
    {
        WDRLINK_INFO_LOG_RT("WDR: No frames available for processing !!!\n");

        /* no more frame availble so exit the loop outside of this function */
        status = FVID2_EFAIL;
    }

    return status;
}

/* This function gets called when capture driver has captured frames. This
 * function takes those frames from the capture driver and stores them in the
 * channel specific buf-queue for noise filtering stage. */
/* ===================================================================
 *  @func     WdrLink_drvProcessData
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
Int32 WdrLink_drvProcessData(WdrLink_Obj * pObj)
{
    Int32 frameId;

    FVID2_FrameList frameList;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    Int32 status;

    WdrLink_ChObj *pChObj;

    WDRLINK_INFO_LOG_RT("WDR: WdrLink_drvProcessData !!!\n");
    /* Pointer to the input link's queue */
    pInQueParams = &pObj->createArgs.inQueParams;

    /* Get the captured frames from the capture driver */
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    WDRLINK_INFO_LOG_RT("WDR: %d frames in !!!\n", frameList.numFrames);
    if (frameList.numFrames)
    {
        pObj->inFrameReceiveCount += frameList.numFrames;

        /* For each captured frame, check the channel no and copy it
         * accordingly in the fullQueue. */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            UTILS_assert(pFrame->channelNum < pObj->inQueInfo.numCh);
        	pChObj = &pObj->linkChInfo[pFrame->channelNum];

                status =
                    Utils_bufPutFullFrame(&pChObj->bufInQue, pFrame);
                UTILS_assert(status == FVID2_SOK);

                WDRLINK_INFO_LOG_RT("WDR: Put frame %d to In Que !!!\n",
                                    frameId);
        }

        //Vps_rprintf("Calling WdrLink_drvDoWdrFilter\n")           ;
        /* with the previous frame skipping algo, we are sure that there is
         * only one frame to process per channel */
        // do
        {
            status = WdrLink_drvDoWdr(pObj);
        }                                                  // while (status
        // == FVID2_SOK);
        //Vps_rprintf("Done Calling WdrLink_drvDoWdrFilter\n")           ;

        /* Return frames back to the capture link as well */
        WDRLINK_INFO_LOG_RT("WDR	: Release %d IN frames !!!\n",
                            frameList.numFrames);
        pObj->inFrameGivenCount += frameList.numFrames;
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     WdrLink_drvStart
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
Int32 WdrLink_drvStart(WdrLink_Obj * pObj)
{
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     WdrLink_drvStop
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
Int32 WdrLink_drvStop(WdrLink_Obj * pObj)
{
    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    Int32 status = FVID2_SOK;

    WDRLINK_INFO_LOG("WDRLINK: Stop in progress !!!\n");

    pInQueParams = &pObj->createArgs.inQueParams;
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);
    if (frameList.numFrames)
    {
        /* Free input frames */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }

    WDRLINK_INFO_LOG("WDRLINK: Stop done !!!\n");

    return (status);
}

/* ===================================================================
 *  @func     WdrLink_drvDelete
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
Int32 WdrLink_drvDelete(WdrLink_Obj * pObj)
{
    UInt32 chId;

    WdrLink_ChObj *wdrChObj;

    Int32 status;

    status = WDR_Engine_DeInit(pObj);
    UTILS_assert(status == MSP_ERROR_NONE);

    WDRLINK_INFO_LOG("WDR: Delete in progress !!!\n");

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        wdrChObj = &pObj->linkChInfo[chId];

        /* Free the allocated frames */
        Utils_memFrameFree(&wdrChObj->format, wdrChObj->frames,
                           WDR_LINK_MAX_OUT_FRAMES_PER_CH);

        /* Free the channel specific buf-queue */
        Utils_bufDelete(&wdrChObj->bufInQue);
    }

    /* Free the WDR link output queues */
    Utils_bufDelete(&pObj->bufOutQue);

    WDRLINK_INFO_LOG("WDR: Delete done !!!\n");

    return FVID2_SOK;
}

/* This function will be called from the next link in the chain when the
 * frames are no more required. This function restores channel number for
 * each frame from the appData and pushes the frame back to the channel
 * specific bufInQue.emptyQueue. */
/* ===================================================================
 *  @func     WdrLink_drvReleaseFrames
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
Int32 WdrLink_drvReleaseFrames(WdrLink_Obj * pObj, FVID2_FrameList * pFrameList)
{
    Int32 status;

    UInt32 frameId;

    FVID2_Frame *pFrame;

    System_FrameInfo *pFrameInfo;

    pObj->outFrameReceiveCount += pFrameList->numFrames;
    if (pFrameList->numFrames)
    {
        for (frameId = 0; frameId < pFrameList->numFrames; frameId++)
        {
            pFrame = pFrameList->frames[frameId];

            /* Channel number might have changed, restore it with the
             * orignial stored value. */
            pFrameInfo = (System_FrameInfo *) pFrame->appData;
            UTILS_assert(pFrameInfo != NULL);
            pFrame->channelNum = pFrameInfo->wdrChannelNum;

            UTILS_assert(pFrame->channelNum < pObj->inQueInfo.numCh);

            /* Return each frame to its original channel specific
             * WdrLink_ChObj.bufInQue.emptyQue. */
            status =
                Utils_bufPutEmptyFrame(&pObj->
                                       linkChInfo[pFrame->channelNum].bufInQue,
                                       pFrame);
            UTILS_assert(status == FVID2_SOK);

            WDRLINK_INFO_LOG_RT
                ("WDR	: Release frame %d to WDR channel !!!\n", frameId);
        }

        WDRLINK_INFO_LOG_RT("WDR: Returned %d frames to WDR channel %d !!!\n",
                            pFrameList->numFrames, pFrame->channelNum);
    }

    return FVID2_SOK;
}

Int32 WdrLink_drvChangeMode(WdrLink_Obj * pObj, WdrLink_Mode mode)
{
	UInt32 chId;
	
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
    	if (pObj->linkChInfo[chId].chCreateArgs.wdrMode != mode)
    	{
			pObj->linkChInfo[chId].chCreateArgs.wdrMode = mode;
			pObj->linkChInfo[chId].chCreateArgs.update = 1;
    	}
    }

	return FVID2_SOK;
}

