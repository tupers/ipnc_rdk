/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <xdc/std.h>
#include <mcfw/interfaces/link_api/system_tiler.h>
#include "nsfLink_priv.h"

extern ti2a_output ti2a_output_params;

Vps_NsfProcessingCfg gNsfProcessingCfg[] = {
    // Level 0
    {
     .staticFrameNoise[0] = 0,//REG8 B12-B0
     .spatialStrengthLow[0] = 0xC,//REG2 B7-B0
     .spatialStrengthHigh[0] = 0xC,//REG2 B15-B8

     .staticFrameNoise[1] = 0,//REG9 B28-B16
     .spatialStrengthLow[1] = 0x14,//REG3 B7-B0
     .spatialStrengthHigh[1] = 0x14,//REG3 B15-B8

     .staticFrameNoise[2] = 0,//REG9 B12-B0
     .spatialStrengthLow[2] = 0x14,//REG3 B23-B16
     .spatialStrengthHigh[2] = 0x14,//REG3 B31-B24

     .temporalStrength = 0xB,//REG4 B5-B0
     .temporalTriggerNoise = 0x6,//REG4 B11-B8
     .noiseIirCoeff = 0xD,//REG5 B3-B0
     .maxNoise = 0xA,//REG5 B8-B4
     .pureBlackThres = 0x16,//REG6 B5-B0
     .pureWhiteThres = 0x16,//REG6 B13-B8
     }
    ,
    // Level 1
    {
     .staticFrameNoise[0] = 0,
     .spatialStrengthLow[0] = 0x10,
     .spatialStrengthHigh[0] = 0x10,

     .staticFrameNoise[1] = 0,
     .spatialStrengthLow[1] = 0x18,
     .spatialStrengthHigh[1] = 0x18,

     .staticFrameNoise[2] = 0,
     .spatialStrengthLow[2] = 0x18,
     .spatialStrengthHigh[2] = 0x18,

     .temporalStrength = 0xF,
     .temporalTriggerNoise = 0x6,
     .noiseIirCoeff = 0xD,
     .maxNoise = 0xA,
     .pureBlackThres = 0x16,
     .pureWhiteThres = 0x16,
     }
    ,
    // Level 2
    {
     .staticFrameNoise[0] = 0,
     .spatialStrengthLow[0] = 0x12,
     .spatialStrengthHigh[0] = 0x12,

     .staticFrameNoise[1] = 0,
     .spatialStrengthLow[1] = 0x1C,
     .spatialStrengthHigh[1] = 0x1C,

     .staticFrameNoise[2] = 0,
     .spatialStrengthLow[2] = 0x1C,
     .spatialStrengthHigh[2] = 0x1C,

     .temporalStrength = 0x12,
     .temporalTriggerNoise = 0x6,
     .noiseIirCoeff = 0xD,
     .maxNoise = 0xC,
     .pureBlackThres = 0x16,
     .pureWhiteThres = 0x16,
     }
    ,
    // Level 3
    {
     .staticFrameNoise[0] = 0,
     .spatialStrengthLow[0] = 0x14,
     .spatialStrengthHigh[0] = 0x14,

     .staticFrameNoise[1] = 0,
     .spatialStrengthLow[1] = 0x1E,
     .spatialStrengthHigh[1] = 0x1E,

     .staticFrameNoise[2] = 0,
     .spatialStrengthLow[2] = 0x1E,
     .spatialStrengthHigh[2] = 0x1E,

     .temporalStrength = 0x14,
     .temporalTriggerNoise = 0x6,
     .noiseIirCoeff = 0xD,
     .maxNoise = 0xD,
     .pureBlackThres = 0x16,
     .pureWhiteThres = 0x16,
     }
    ,
    // Level 4
    {
     .staticFrameNoise[0] = 0,
     .spatialStrengthLow[0] = 0x16,
     .spatialStrengthHigh[0] = 0x16,

     .staticFrameNoise[1] = 0,
     .spatialStrengthLow[1] = 0x20,
     .spatialStrengthHigh[1] = 0x20,

     .staticFrameNoise[2] = 0,
     .spatialStrengthLow[2] = 0x20,
     .spatialStrengthHigh[2] = 0x20,

     .temporalStrength = 0x16,
     .temporalTriggerNoise = 0x6,
     .noiseIirCoeff = 0xD,
     .maxNoise = 0xF,
     .pureBlackThres = 0x16,
     .pureWhiteThres = 0x16,
     }
    ,
    // Level 5
    {
     .staticFrameNoise[0] = 0,
     .spatialStrengthLow[0] = 0x16,
     .spatialStrengthHigh[0] = 0x16,

     .staticFrameNoise[1] = 0,
     .spatialStrengthLow[1] = 0x20,
     .spatialStrengthHigh[1] = 0x20,

     .staticFrameNoise[2] = 0,
     .spatialStrengthLow[2] = 0x20,
     .spatialStrengthHigh[2] = 0x20,

     .temporalStrength = 0x30,
     .temporalTriggerNoise = 0x6,
     .noiseIirCoeff = 0xD,
     .maxNoise = 0xF,
     .pureBlackThres = 0x16,
     .pureWhiteThres = 0x16,
     }
    ,
    // Level 6
    {
     .staticFrameNoise[0] = 0,
     .spatialStrengthLow[0] = 0x30,
     .spatialStrengthHigh[0] = 0x30,

     .staticFrameNoise[1] = 0,
     .spatialStrengthLow[1] = 0x30,
     .spatialStrengthHigh[1] = 0x30,

     .staticFrameNoise[2] = 0,
     .spatialStrengthLow[2] = 0x30,
     .spatialStrengthHigh[2] = 0x30,

     .temporalStrength = 0x30,
     .temporalTriggerNoise = 0x6,
     .noiseIirCoeff = 0xD,
     .maxNoise = 0xF,
     .pureBlackThres = 0x16,
     .pureWhiteThres = 0x16,
     }
    ,
    // Level 7
    {
     .staticFrameNoise[0] = 0,
     .spatialStrengthLow[0] = 0x40,
     .spatialStrengthHigh[0] = 0x40,

     .staticFrameNoise[1] = 0,
     .spatialStrengthLow[1] = 0x40,
     .spatialStrengthHigh[1] = 0x40,

     .staticFrameNoise[2] = 0,
     .spatialStrengthLow[2] = 0x40,
     .spatialStrengthHigh[2] = 0x40,

     .temporalStrength = 0x30,
     .temporalTriggerNoise = 0x6,
     .noiseIirCoeff = 0xD,
     .maxNoise = 0xF,
     .pureBlackThres = 0x16,
     .pureWhiteThres = 0x16,
     }
};

Int32 NsfLink_resetStatistics(NsfLink_Obj * pObj)
{
    UInt32 chId;
    NsfLink_ChObj *pChObj;

    for (chId = 0; chId < pObj->nsfCreateParams.numCh; chId++)
    {
        pChObj = &pObj->linkChInfo[chId];

        pChObj->inFrameRecvCount = 0;
        pChObj->inFrameRejectCount = 0;
        pChObj->inFrameProcessCount = 0;
        pChObj->outFrameCount = 0;
        pChObj->outFrameUserSkipCount = 0;
		pChObj->maxLatency = 0;
		pChObj->minLatency = 0xFF;
    }

    pObj->statsStartTime = Utils_getCurTimeInMsec();

    return 0;
}

Int32 NsfLink_drvPrintStatistics(NsfLink_Obj * pObj, Bool resetAfterPrint)
{
    UInt32 chId;
    NsfLink_ChObj *pChObj;
    UInt32 elaspedTime;

    elaspedTime = Utils_getCurTimeInMsec() - pObj->statsStartTime; // in msecs
    elaspedTime /= 1000; // convert to secs

    Vps_printf( " \n"
            " *** [%s] NSF Statistics *** \n"
            " \n"
            " Elasped Time           : %d secs\n"
            " Total Fields Processed : %d \n"
            " Total Fields FPS       : %d FPS\n"
            " \n"
            " \n"
            " CH  | In Recv In Reject In Process Out User Out Latency  \n"
            " Num | FPS     FPS       FPS        FPS Skip FPS Min / Max\n"
            " ---------------------------------------------------------\n",
            pObj->name,
            elaspedTime,
            pObj->processFrameCount,
            pObj->processFrameCount * 100 / (pObj->totalTime / 10)
                    );

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->linkChInfo[chId];

        Vps_printf( " %3d | %7d %9d %10d %3d %8d %3d / %3d\n",
            chId,
            pChObj->inFrameRecvCount/elaspedTime,
            pChObj->inFrameRejectCount/elaspedTime,
            pChObj->inFrameProcessCount/elaspedTime,
            pChObj->outFrameCount/elaspedTime,
            pChObj->outFrameUserSkipCount/elaspedTime,
			pChObj->minLatency, pChObj->maxLatency
            );
    }

    Vps_printf( " \n");

    if(resetAfterPrint)
    {
        NsfLink_resetStatistics(pObj);
    }
    return FVID2_SOK;
}

Int32 NsfLink_printBufferStatus(NsfLink_Obj * pObj)
{
    Uint8 i, str[256];

    Vps_rprintf(
        " \n"
        " *** [%s] NSF Statistics *** \n"
        "%d: NSF: Rcvd from prev = %d, Returned to prev = %d\r\n",
        pObj->name,
        Utils_getCurTimeInMsec(), pObj->inFrameReceiveCount, pObj->inFrameGivenCount);

    for (i=0; i<pObj->createArgs.numOutQue; i++)
//        for (i=0; i<NSF_LINK_MAX_OUT_QUE; i++)
    {
        sprintf ((char *)str, "NSF Out [%d]", i);
        Utils_bufPrintStatus(str, &pObj->bufOutQue[i]);
    }
    return 0;
}


Int32 NsfLink_drvCallback(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    NsfLink_Obj *pObj = (NsfLink_Obj *) appData;

    Semaphore_post(pObj->complete);

    return FVID2_SOK;
}

Int32 NsfLink_drvFvidErrCb(FVID2_Handle handle, Ptr appData,
                           Ptr errList, Ptr reserved)
{
#ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Error Callback !!!\n", Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}

Int32 NsfLink_drvCreate(NsfLink_Obj * pObj, NsfLink_CreateParams * pPrm)
{
    Int32 status;
    UInt32 queId;
    Semaphore_Params semParams;
    FVID2_CbParams cbParams;

#ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Create in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    UTILS_MEMLOG_USED_START();
    /* Store the create params coming from the app */
    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->inFrameReceiveCount = 0;
    pObj->inFrameGivenCount = 0;
    pObj->outFrameReceiveCount = 0;
    pObj->outFrameGivenCount = 0;
    pObj->processFrameReqCount = 0;
    pObj->getProcessFrameReqCount = 0;
    pObj->processFrameCount = 0;
    pObj->getFrames = 0;
    pObj->totalTime = 0;
    pObj->curTime = 0;
    pObj->curEvenFieldFrame = 0;

    NsfLink_resetStatistics(pObj);

    /* Info about the NSF link */
    pObj->info.numQue = NSF_LINK_MAX_OUT_QUE;

    for (queId = 0; queId < NSF_LINK_MAX_OUT_QUE; queId++)
    {
        status = Utils_bufCreate(&pObj->bufOutQue[queId], FALSE, FALSE);
        UTILS_assert(status == FVID2_SOK);

        pObj->info.queInfo[queId].numCh = 0;
    }

    status = Utils_bufCreate(&pObj->bufEvenFieldOutQue, FALSE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    pObj->info.queInfo[2].numCh = 0;

    /* Copy previous link (capture) info */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);
    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    #ifndef SYSTEM_USE_TILER
    if (pObj->createArgs.tilerEnable)
    {
        Vps_printf("NSFLINK:!!WARNING.FORCIBLY DISABLING TILER since tiler is disabled at build time");
        pObj->createArgs.tilerEnable = FALSE;
    }
    #endif

    /* Copy previous link's queue info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* Create semaphores */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->complete = Semaphore_create(0u, &semParams, NULL);
    UTILS_assert(pObj->complete != NULL);

    /*
     * Configure the channels, allocate frames for each channel and
     * put those frames in channel specific buffer queue.
     */
    NsfLink_drvInitCh(pObj);

    /* Callback functions */
    memset(&cbParams, 0, sizeof(cbParams));

    cbParams.cbFxn = NsfLink_drvCallback;
    cbParams.appData = pObj;
    cbParams.errCbFxn = NsfLink_drvFvidErrCb;
    cbParams.errList = &pObj->errCbProcessList;

    /* Create NSF handle */
    pObj->fvidHandleNsf = FVID2_create(FVID2_VPS_M2M_NSF_DRV,
                                       VPS_M2M_INST_NF0,
                                       &pObj->nsfCreateParams,
                                       &pObj->nsfCreateStatus, &cbParams);
    UTILS_assert(pObj->fvidHandleNsf != NULL);

    UTILS_MEMLOG_USED_END(pObj->memUsed);
    UTILS_MEMLOG_PRINT("NSF:",
                       pObj->memUsed,
                       UTILS_ARRAYSIZE(pObj->memUsed));

#ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Create Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return status;
}

/* This function gets called when capture driver has captured frames. This
 * function takes those frames from the capture driver and stores them in the
 * channel specific buf-queue for noise filtering stage. */
Int32 NsfLink_drvProcessData(NsfLink_Obj * pObj)
{
    UInt32 frameId;
    FVID2_FrameList frameList;
    FVID2_Frame *pFrame;
    System_LinkInQueParams *pInQueParams;
    Int32 status;
    UInt32 freeFrameNum, latency;
    NsfLink_ChObj *pChObj;

    /* Pointer to the input link's queue */
    pInQueParams = &pObj->createArgs.inQueParams;

    /* Get the captured frames from the capture driver */
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        freeFrameNum = 0;
        pObj->inFrameReceiveCount += frameList.numFrames;

        /* For each captured frame, check the channel no and copy it
         * accordingly in the fullQueue. */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            UTILS_assert(pFrame->channelNum < pObj->nsfCreateParams.numCh);

            pChObj = &pObj->linkChInfo[pFrame->channelNum];

            pChObj->inFrameRecvCount++;

            if ((pObj->inQueInfo.chInfo[pFrame->channelNum].scanFormat ==
                                               FVID2_SF_PROGRESSIVE) ||
                                               (pChObj->nextFid == 0))
            {
                pChObj->doFrameDrop = Utils_doSkipFrame(&(pChObj->frameSkipCtx));
            }

			if(pObj->createArgs.enable[pFrame->channelNum] == 0)
			{
				pChObj->doFrameDrop = TRUE;
			}
			
            if (pObj->inQueInfo.chInfo[pFrame->channelNum].scanFormat !=
                                                 FVID2_SF_INTERLACED)
            {
                pChObj->nextFid = pFrame->fid;
            }
            if (((pChObj->nextFid == pFrame->fid) ||
                 (pFrame->fid == FVID2_FID_FRAME)) &&
                 (pChObj->doFrameDrop == FALSE))
            {
			    latency = Utils_getCurTimeInMsec() - pFrame->timeStamp;

                if(latency>pChObj->maxLatency)
                    pChObj->maxLatency = latency;
                if(latency<pChObj->minLatency)
                    pChObj->minLatency = latency;
                pChObj->outFrameCount++;
                // frame is of the expected FID use it, else drop it
                status = Utils_bufPutFullFrame(&pChObj->bufInQue, pFrame);
                UTILS_assert(status == FVID2_SOK);
            }
            else
            {
                pChObj->inFrameRejectCount++;
                // frame is not of expected FID, so release frame
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                if (pChObj->doFrameDrop == TRUE)
                    pChObj->outFrameUserSkipCount++;
            }
            if (pChObj->nextFid == pFrame->fid)
                    pChObj->nextFid ^= 1;                      // toggle to next
        }

        if (freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;

#ifdef SYSTEM_DEBUG_NSF_RT
            Vps_printf(" %d: NF    : Skipped %d IN frames !!!\n",
                       Utils_getCurTimeInMsec(), frameList.numFrames);
#endif
            pObj->inFrameGivenCount += frameList.numFrames;
            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId, &frameList);
        }
    }

    do
    {
        status = NsfLink_drvDoNsfFilter(pObj);
    } while (status == FVID2_SOK);

    return FVID2_SOK;
}

Int32 NsfLink_drvDoNsfFilter(NsfLink_Obj * pObj)
{
    FVID2_FrameList inFrameList[2], outFrameList;
    FVID2_ProcessList processList;

    #define SLC_MAX 96
	#define PRM_MAX 32
    FVID2_FrameList inFrameListSlc[2], outFrameListSlc;
    FVID2_ProcessList processListSlc;
    Bool doSlice = FALSE;
    FVID2_Frame slcFrame[SLC_MAX];
    UInt32 slcFrameIdx = 0;
    UInt32 slcIdx = 0;

    Vps_M2mNsfRtParams rtParamsSlc[PRM_MAX];
    Vps_NsfDataFormat rtParamDataFmtSlc[PRM_MAX];
    
    FVID2_Frame *pFullFrame;
    FVID2_Frame *pEmptyFrame;
    Int32 status;
    UInt32 chId, frameId, queueId = 0, chPerQueue, latency;
    FVID2_Frame *pFrame;
    UInt32 sendMsgToTsk = 0;
    UInt32 perChCount, numFrames;
    System_FrameInfo *pInFrameInfo;
    System_FrameInfo *pOutFrameInfo;
    NsfLink_ChObj *pChObj;
    Vps_M2mNsfRtParams *rtParams;
    Vps_NsfDataFormat *rtParamDataFmt;
    NsfLink_ChObj *nsfChObj;
    Bool rtParamUpdatePerFrame;

    /* Initialize the process list with different frame lists */
    processList.inFrameList[0] = &inFrameList[0];
    processList.inFrameList[1] = &inFrameList[1];
    processList.outFrameList[0] = &outFrameList;

    processList.numInLists = 2;
    processList.numOutLists = 1;

    processList.drvData = NULL;
    processList.reserved = NULL;

    numFrames = 0;

    pObj->getFrames++;

    /* init the process list which will do the slice operation */
    processListSlc.inFrameList[0] = &inFrameListSlc[0];
    processListSlc.inFrameList[1] = &inFrameListSlc[1];
    processListSlc.outFrameList[0] = &outFrameListSlc;

    processListSlc.numInLists = 2;
    processListSlc.numOutLists = 1;

    processListSlc.drvData = NULL;
    processListSlc.reserved = NULL;

    /** For all the available channels, look into the respective
     * bufInQue.fullQueue and take the frames out of that queue and add them
     * to the inFrameList.
     * Take the same number of frames from the respective bufInQue.emptyQueue
     * and add them to the outFrameList.
     * This process will make the desired processList, ready for noise
     * filtering.
     */
    for (chId = 0; chId < pObj->nsfCreateParams.numCh; chId++)
    {
        /* While there are captured frames... and less than what NSF could
         * consume per request per channel. */
        perChCount = 0u;
        while (1)
        {
            status = Utils_bufGetFullFrame(&pObj->linkChInfo[chId].bufInQue,
                                           &pFullFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
                break;

            pInFrameInfo = (System_FrameInfo *) pFullFrame->appData;

            /* First check whether it can be accomodated or not */
            status = Utils_bufGetEmptyFrame(&pObj->linkChInfo[chId].bufInQue,
                                            &pEmptyFrame, BIOS_WAIT_FOREVER);
            UTILS_assert(status == FVID2_SOK);

            pOutFrameInfo = (System_FrameInfo *) pEmptyFrame->appData;


            /* Add both the frames in the respective frame lists */
            inFrameList[0].frames[numFrames] = pFullFrame;
            inFrameList[1].frames[numFrames] =
                pObj->linkChInfo[chId].pPrevOutFrame;

            pFullFrame->perFrameCfg = NULL;
            rtParamUpdatePerFrame = FALSE;
            UTILS_assert (pFullFrame->channelNum == chId);
            nsfChObj = &pObj->linkChInfo[chId];
            rtParamDataFmt = &nsfChObj->nsfRtParamDataFormat;
            if ((pInFrameInfo != NULL) && (pInFrameInfo->rtChInfoUpdate == TRUE))
            {
				rtParamUpdatePerFrame = TRUE;
                if (pInFrameInfo->rtChInfo.height != rtParamDataFmt->inFrameHeight)
                {
                    rtParamDataFmt->inFrameHeight = pInFrameInfo->rtChInfo.height;
                    rtParamUpdatePerFrame = TRUE;
                }
                if (pInFrameInfo->rtChInfo.width != rtParamDataFmt->inFrameWidth)
                {
                    rtParamDataFmt->inFrameWidth = pInFrameInfo->rtChInfo.width;
                    rtParamUpdatePerFrame = TRUE;
                }
                if (pInFrameInfo->rtChInfo.pitch[0] != rtParamDataFmt->inPitch)
                {
                    rtParamDataFmt->inPitch = pInFrameInfo->rtChInfo.pitch[0];
                    rtParamUpdatePerFrame = TRUE;
					rtParamDataFmt->outPitch[0] = VpsUtils_align(rtParamDataFmt->inFrameWidth, VPS_BUFFER_ALIGNMENT * 2);
					rtParamDataFmt->outPitch[1] = VpsUtils_align(rtParamDataFmt->inFrameWidth, VPS_BUFFER_ALIGNMENT * 2);
                }
            }
            if (rtParamUpdatePerFrame == TRUE)
            {
                rtParams = &nsfChObj->nsfRtParams;
                rtParams->dataFormat = &nsfChObj->nsfRtParamDataFormat;
                rtParams->processingCfg = NULL;
                pFullFrame->perFrameCfg = rtParams;
            }

            pOutFrameInfo->rtChInfo.width    = rtParamDataFmt->inFrameWidth;
            pOutFrameInfo->rtChInfo.height   = rtParamDataFmt->inFrameHeight;
            pOutFrameInfo->rtChInfo.pitch[0] = rtParamDataFmt->outPitch[0];
            pOutFrameInfo->rtChInfo.pitch[1] = rtParamDataFmt->outPitch[1];
            pOutFrameInfo->rtChInfoUpdate    = TRUE;

            outFrameList.frames[numFrames] = pEmptyFrame;
			
            /** check if we need slice process
             * if needed, copy the frame and frame info and also rt parameters
             * then modify those stuff and build a new process list for slice proce
             * Don't need to change the inFrame information and outFrame information
             * in hdvpss driver, every thing is calculated by the inframe[0]
             * Don't need copy and modify back the rt information because it is no need for
             * next link
             * Not using the slice process in the driver, it is no efficient under
             * this case
             * PS: Split all the block to same width and height so don't need change the
             * rt information from time to time. 
             * In nsf hdvpss, only rt used to change the hw configuration, so don't need 
             * recover back any configuration.
             * pInFrameInfo->rtChInfo should always have something right
             */
            
            if ((pInFrameInfo->rtChInfo.height > NSF_MAX_SLICE_SIZE)||
                (pInFrameInfo->rtChInfo.width > NSF_MAX_SLICE_SIZE))
            {
                UInt32 numInVert,i,j;
                UInt32 numInHorz;
                UInt32 numPixelsPerSubFrame;
                UInt32 numLinesPerSubFrame;
                UInt32 numPixelsLastSubFrame;
                UInt32 numLinesLastSubFrame;
				Vps_M2mNsfRtParams *rtParams;
				
                numInVert = VpsUtils_align(pInFrameInfo->rtChInfo.height, NSF_MAX_SLICE_SIZE)/NSF_MAX_SLICE_SIZE;
                numInHorz = VpsUtils_align(pInFrameInfo->rtChInfo.width, NSF_MAX_SLICE_SIZE)/NSF_MAX_SLICE_SIZE;

                
                numPixelsPerSubFrame = 
                    VpsUtils_floor(pInFrameInfo->rtChInfo.width/numInHorz,
                        NSF_FRAME_SIZE_ALIGN);
				numPixelsLastSubFrame = pInFrameInfo->rtChInfo.width - numPixelsPerSubFrame * (numInHorz - 1);
                numLinesPerSubFrame = 
                    VpsUtils_floor(pInFrameInfo->rtChInfo.height/numInVert,
                        NSF_FRAME_SIZE_ALIGN);
				numLinesLastSubFrame = pInFrameInfo->rtChInfo.height - numLinesPerSubFrame * (numInVert - 1);

                //this will cause the RT update to true for evey frame
                //because we change it to something not equel the original value				
                for(i=0;i<numInHorz;i++)
                {
                    for(j=0;j<numInVert;j++)
                    {
                        /* copy the frame */
                        memcpy(&slcFrame[slcFrameIdx],
                            inFrameList[0].frames[numFrames],sizeof(FVID2_Frame));
                        inFrameListSlc[0].frames[slcIdx] = &(slcFrame[slcFrameIdx]);
                        slcFrame[slcFrameIdx].addr[FVID2_FRAME_ADDR_IDX]
                           [FVID2_YUV_INT_ADDR_IDX] =
                        (char *)inFrameList[0].frames[numFrames]->addr[FVID2_FRAME_ADDR_IDX]
                            [FVID2_YUV_INT_ADDR_IDX]
                            + (j*numLinesPerSubFrame*rtParamDataFmt->inPitch)
                            + (i*numPixelsPerSubFrame*2);

						rtParams = &rtParamsSlc[slcIdx];
						slcFrame[slcFrameIdx].perFrameCfg = rtParams;						
						rtParams->processingCfg = NULL;
						rtParams->dataFormat = &rtParamDataFmtSlc[slcIdx];
						memcpy(rtParams->dataFormat,&nsfChObj->nsfRtParamDataFormat,sizeof(Vps_NsfDataFormat));
						if (i == numInHorz - 1)
							rtParams->dataFormat->inFrameWidth = numPixelsLastSubFrame;
						else
							rtParams->dataFormat->inFrameWidth = numPixelsPerSubFrame;
						if (j == numInVert - 1)
							rtParams->dataFormat->inFrameHeight = numLinesLastSubFrame;
						else
							rtParams->dataFormat->inFrameHeight = numLinesPerSubFrame;
                       slcFrameIdx ++;

                        
                        memcpy(&slcFrame[slcFrameIdx],
                            inFrameList[1].frames[numFrames],sizeof(FVID2_Frame));
                        inFrameListSlc[1].frames[slcIdx] = &(slcFrame[slcFrameIdx]);
                        
                        slcFrame[slcFrameIdx].addr[FVID2_FRAME_ADDR_IDX]
                        [FVID2_YUV_SP_Y_ADDR_IDX] =
                            (char *)inFrameList[1].frames[numFrames]->addr[FVID2_FRAME_ADDR_IDX]
                                   [FVID2_YUV_SP_Y_ADDR_IDX]
                            + (j*numLinesPerSubFrame*pOutFrameInfo->rtChInfo.pitch[0])
                            + (i*numPixelsPerSubFrame);
                        
                        slcFrame[slcFrameIdx].addr[FVID2_FRAME_ADDR_IDX]
                        [FVID2_YUV_SP_CBCR_ADDR_IDX] =
                            (char *)inFrameList[1].frames[numFrames]->addr[FVID2_FRAME_ADDR_IDX]
                            [FVID2_YUV_SP_CBCR_ADDR_IDX]
                            + (j*numLinesPerSubFrame*pOutFrameInfo->rtChInfo.pitch[1]/2)
                            + (i*numPixelsPerSubFrame);
                        slcFrameIdx ++;


                        memcpy(&slcFrame[slcFrameIdx],
                            outFrameList.frames[numFrames],sizeof(FVID2_Frame));
                        outFrameListSlc.frames[slcIdx] = &(slcFrame[slcFrameIdx]);
                        slcFrame[slcFrameIdx].addr[FVID2_FRAME_ADDR_IDX]
                        [FVID2_YUV_SP_Y_ADDR_IDX] =
                            (char *)outFrameList.frames[numFrames]->addr[FVID2_FRAME_ADDR_IDX]
                                   [FVID2_YUV_SP_Y_ADDR_IDX]
                            + (j*numLinesPerSubFrame*pOutFrameInfo->rtChInfo.pitch[0])
                            + (i*numPixelsPerSubFrame);
                        
                        slcFrame[slcFrameIdx].addr[FVID2_FRAME_ADDR_IDX]
                        [FVID2_YUV_SP_CBCR_ADDR_IDX] =
                            (char *)outFrameList.frames[numFrames]->addr[FVID2_FRAME_ADDR_IDX]
                            [FVID2_YUV_SP_CBCR_ADDR_IDX]
                            + (j*numLinesPerSubFrame*pOutFrameInfo->rtChInfo.pitch[1]/2)
                            + (i*numPixelsPerSubFrame);
                        slcFrameIdx ++;

                        slcIdx++;
                        UTILS_assert(slcFrameIdx <SLC_MAX);
                    }
                }
                doSlice = TRUE;
            }


            /* next previous output frame is current output frame */
            pObj->linkChInfo[chId].pPrevOutFrame = pEmptyFrame;

            pEmptyFrame->timeStamp = pFullFrame->timeStamp;
            if (pInFrameInfo != NULL)
                pOutFrameInfo->ts64  = pInFrameInfo->ts64;

            nsfChObj->inFrameProcessCount++;
            numFrames++;
            perChCount++;
            if (perChCount >
                0 /* pObj->nsfCreateStatus.maxFramesPerChInQueue */ )
                break;
        }
    }

    pObj->getFrames--;

    inFrameList[0].numFrames = numFrames;
    inFrameList[1].numFrames = numFrames;
    outFrameList.numFrames = numFrames;

    inFrameListSlc[0].numFrames = 
        inFrameListSlc[1].numFrames =
        outFrameListSlc.numFrames = slcIdx;
        
    /* Start noise filtering */
    if (inFrameList[0].numFrames)
    {
		/* strength adjustment */
		if (nsfChObj->inFrameProcessCount % 60 == 0)
		{
			int gain;
			int level;
		
			if (IssAlg_capt2AGetAEWBVendor() == AEWB_ID_TI)
				gain = ti2a_output_params.sensorGain * ti2a_output_params.ipipe_awb_gain.dGain / 256;
			else
				gain = 36000;
		
			switch (pObj->createArgs.nsfStrength)
			{
				default:
				case NSF_LINK_NF_STRENGTH_AUTO:
					level = 4;
					if (gain < 24000)
						level = 1;
					else if (gain > 48000)
						level = 7;
					break;
		
				case NSF_LINK_NF_STRENGTH_MIDDLE:
					level = 4;
					break;
		
				case NSF_LINK_NF_STRENGTH_LOW:
					level = 1;
					break;
		
				case NSF_LINK_NF_STRENGTH_HIGH:
					level = 7;
					break;
			}
		
			NsfLink_drvSetNsfLevel(pObj, level);
		}
		
#ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: Noise Filtering %d frames !!!\n",
                   Utils_getCurTimeInMsec(), inFrameList[0].numFrames);
#endif

        pObj->processFrameReqCount++;

        pObj->curTime = Utils_getCurTimeInMsec();

		if (!doSlice)
		{
	        status = FVID2_processFrames(pObj->fvidHandleNsf, &processList);
	        UTILS_assert(status == FVID2_SOK);

	        Semaphore_pend(pObj->complete, BIOS_WAIT_FOREVER);
	            
	        status = FVID2_getProcessedFrames(pObj->fvidHandleNsf, &processList,
	                                              BIOS_NO_WAIT);
		}
        else
        {
            status = FVID2_processFrames(pObj->fvidHandleNsf, &processListSlc);
            UTILS_assert(status == FVID2_SOK);
            
            Semaphore_pend(pObj->complete, BIOS_WAIT_FOREVER);
                
            status = FVID2_getProcessedFrames(pObj->fvidHandleNsf, &processListSlc,
                                                  BIOS_NO_WAIT);
        }
		
        UTILS_assert(status == FVID2_SOK);

        pObj->curTime = Utils_getCurTimeInMsec() - pObj->curTime;

        pObj->totalTime += pObj->curTime;

        pObj->processFrameCount += processList.outFrameList[0]->numFrames;

        pObj->getProcessFrameReqCount++;

#ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: Noise Filtering of %d frames Done !!!\n",
                   Utils_getCurTimeInMsec(), inFrameList[0].numFrames);
#endif
        /* Put the processed frames in the appropriate output queue */
        for (frameId = 0; frameId < outFrameList.numFrames; frameId++)
        {
            pFrame = outFrameList.frames[frameId];

            if (pFrame)
            {
                pChObj = &pObj->linkChInfo[pFrame->channelNum];
                /** Split the frames into both the output queues,
                 * if they are enabled. Else use output queue 0 only.
                 * Also, if output queue 1 is used, frames sent to this queue
                 * should be modified before submitting so that the
                 * pFrame->channelNum should start with 0 and not with
                 * (pObj->nsfCreateParams.numCh / 2).
                 */
                chPerQueue =
                    (pObj->nsfCreateParams.numCh / pObj->createArgs.numOutQue);
                queueId = (pFrame->channelNum / chPerQueue);
                pFrame->channelNum = pFrame->channelNum % chPerQueue;
                latency = Utils_getCurTimeInMsec() - pFrame->timeStamp;

                if(latency>pChObj->maxLatency)
                    pChObj->maxLatency = latency;
                if(latency<pChObj->minLatency)
                    pChObj->minLatency = latency;

                /* Set the flag whether queue is filled or not */
                sendMsgToTsk |= (1 << queueId);

                status = Utils_bufPutFullFrame(&pObj->bufOutQue[queueId], pFrame);
                UTILS_assert(status == FVID2_SOK);
            } /* end of if (pFrame) */
        }

        /* Send new data available command to the next link */
        for (queueId = 0; queueId < NSF_LINK_MAX_OUT_QUE; queueId++)
        {
            if (sendMsgToTsk & 0x1)
            {
                pObj->outFrameGivenCount += outFrameList.numFrames;
                /* Send data available message to next task */
                System_sendLinkCmd(pObj->createArgs.outQueParams[queueId].
                                   nextLink, SYSTEM_CMD_NEW_DATA);
            }

            sendMsgToTsk >>= 1;
            if (sendMsgToTsk == 0)
                break;
        }

        pObj->inFrameGivenCount += inFrameList[0].numFrames;

        /* Return frames back to the capture link as well */
        System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
                                   pObj->createArgs.inQueParams.prevLinkQueId,
                                   &inFrameList[0]);
        status = FVID2_SOK;
    }
    else
    {
#ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: No frames available for filtering !!!\n",
                   Utils_getCurTimeInMsec());
#endif

        /* no more frame availble so exit the loop outside of this function */
        status = FVID2_EFAIL;
    }

    return status;
}

/* This function will be called from the next link in the chain when the
 * frames are no more required. This function restores channel number for
 * each frame from the appData and pushes the frame back to the channel
 * specific bufInQue.emptyQueue. */
Int32 NsfLink_drvPutEmptyFrames(NsfLink_Obj * pObj,
                                FVID2_FrameList * pFrameList)
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
            pFrame->channelNum = pFrameInfo->nsfChannelNum;

            UTILS_assert(pFrame->channelNum < pObj->nsfCreateParams.numCh);

            /* Return each frame to its original channel specific
             * NsfLink_ChObj.bufInQue.emptyQue. */
            status =
                Utils_bufPutEmptyFrame(&pObj->linkChInfo[pFrame->channelNum].
                                       bufInQue, pFrame);
            UTILS_assert(status == FVID2_SOK);
        }

#ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: Returned %d frames to NF channels !!!\n",
                   Utils_getCurTimeInMsec(), pFrameList->numFrames);
#endif
    }

    return FVID2_SOK;
}

Int32 NsfLink_drvDelete(NsfLink_Obj * pObj)
{
    UInt32 queId, chId;
    NsfLink_ChObj *nsfChObj;

#ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: NSF    : Fields = %d (fps = %d) !!!\n",
               Utils_getCurTimeInMsec(),
               pObj->outFrameGivenCount,
               pObj->outFrameGivenCount * 100 / (pObj->totalTime / 10));
#endif

#ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Delete in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    /* NSF handle */
    FVID2_delete(pObj->fvidHandleNsf, NULL);

    /* Free the allocated frames */
    NsfLink_drvFreeFrames(pObj);

    /* Free the NSF link output queues */
    for (queId = 0; queId < NSF_LINK_MAX_OUT_QUE; queId++)
    {
        Utils_bufDelete(&pObj->bufOutQue[queId]);
    }

    Utils_bufDelete(&pObj->bufEvenFieldOutQue);

    /* Free the channel specific buf-queue */
    for (chId = 0; chId < pObj->nsfCreateParams.numCh; chId++)
    {
        nsfChObj = &pObj->linkChInfo[chId];
        Utils_bufDelete(&nsfChObj->bufInQue);
    }

    /* Delete semaphores */
    Semaphore_delete(&pObj->complete);

#ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Delete Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}

Int32 NsfLink_drvInitCh(NsfLink_Obj * pObj)
{
    Int32 status;
    UInt32 chId, frameId, queueId, outChId, chPerQueue;
    System_LinkChInfo *pInChInfo;
    Vps_NsfDataFormat *nsfDataFmt;
    Vps_NsfProcessingCfg *nsfProcCfg;
    FVID2_Frame *frames;
    FVID2_Format format;
    NsfLink_ChObj *nsfChObj;
    UInt32 numFrmsPerCh;
	UInt32 level;

    UTILS_assert(pObj != NULL);

    /* Fill the nsfCreateParams structure */
    /* Find no of channels from input (capture) queue */
    pObj->nsfCreateParams.numCh = pObj->inQueInfo.numCh;

    /*
     * Point to memory for data format structure for each channel
     */
    pObj->nsfCreateParams.dataFormat = &pObj->nsfDataFormat[0];

    /*
     * Point to memory for processing params structure for each channel
     */
    pObj->nsfCreateParams.processingCfg = &pObj->nsfProcCfg[0];

    /*
     * For each channel, do the initialization and
     * FVID2 frame / buffer allocation.
     */
    for (chId = 0; chId < pObj->nsfCreateParams.numCh; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];
        nsfDataFmt = &pObj->nsfDataFormat[chId];
        nsfProcCfg = &pObj->nsfProcCfg[chId];
        nsfChObj = &pObj->linkChInfo[chId];

        nsfChObj->frameSkipCtx.firstTime = TRUE;
        nsfChObj->frameSkipCtx.inputFrameRate  = pObj->createArgs.inputFrameRate;
        nsfChObj->frameSkipCtx.outputFrameRate = pObj->createArgs.outputFrameRate;
        /* Set the channel number first */
        nsfChObj->channelId = chId;

        nsfChObj->doFrameDrop = FALSE;
        nsfChObj->nextFid = FVID2_FID_TOP;

        /* Initialize the createParams.dataFormat */
        nsfDataFmt->channelNum = chId;
        nsfDataFmt->inMemType = pInChInfo->memType;

        if (pObj->createArgs.tilerEnable)
            nsfDataFmt->outMemType = VPS_VPDMA_MT_TILEDMEM;
        else
            nsfDataFmt->outMemType = VPS_VPDMA_MT_NONTILEDMEM;
		
		nsfDataFmt->inDataFormat = pInChInfo->dataFormat;
		nsfDataFmt->inPitch = pInChInfo->pitch[0];		
		
		if(pObj->createArgs.enable[chId] == 0)
		{
			/*
			 * If the channel is disabled for NSF then NSF is not there in the data path.
			 * And input data format may be different than YUV422I.
			 * NSF create should happen successfully so that it may be enabled at later point of time.
			 * Therefore the data format is hardcoded to YUV422I format and pitch is doubled.	
			 */ 		
			if((nsfDataFmt->inDataFormat == FVID2_DF_YUV420SP_UV) ||
			   (nsfDataFmt->inDataFormat == FVID2_DF_YUV420SP_VU))
			{			
				nsfDataFmt->inDataFormat = FVID2_DF_YUV422I_UYVY;
				nsfDataFmt->inPitch      = pInChInfo->pitch[0] * 2;
			}
		}
		
        nsfDataFmt->inFrameWidth  = pInChInfo->width;
        nsfDataFmt->inFrameHeight = pInChInfo->height;
        
        /* Only one output format is supported */
        nsfDataFmt->outDataFormat = FVID2_DF_YUV420SP_UV;

        if (nsfDataFmt->outMemType == VPS_VPDMA_MT_TILEDMEM)
        {
            nsfDataFmt->outPitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
            nsfDataFmt->outPitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
        }
        else
        {
            nsfDataFmt->outPitch[0] =
                VpsUtils_align(pInChInfo->width, VPS_BUFFER_ALIGNMENT * 2);
            nsfDataFmt->outPitch[1] =
                VpsUtils_align(pInChInfo->width, VPS_BUFFER_ALIGNMENT * 2);
        }
        nsfDataFmt->outPitch[2] = 0;

        /* Initialize the createParams.processingCfg */
        nsfProcCfg->channelNum = chId;
        /* TBD: Check later on */
        nsfProcCfg->bypassMode = pObj->createArgs.bypassNsf;

        nsfProcCfg->frameNoiseAutoCalcEnable = TRUE;
        nsfProcCfg->frameNoiseCalcReset = FALSE;
        nsfProcCfg->subFrameModeEnable = FALSE;
        nsfProcCfg->numLinesPerSubFrame = 128;

		switch (pObj->createArgs.nsfStrength)
		{
			default:
			case NSF_LINK_NF_STRENGTH_AUTO:
			case NSF_LINK_NF_STRENGTH_MIDDLE:
				level = 4;
				break;

			case NSF_LINK_NF_STRENGTH_LOW:
				level = 1;
				break;

			case NSF_LINK_NF_STRENGTH_HIGH:
				level = 7;
				break;
		}

        nsfProcCfg->staticFrameNoise[0] =
            gNsfProcessingCfg[level].staticFrameNoise[0];
        nsfProcCfg->spatialStrengthLow[0] =
            gNsfProcessingCfg[level].spatialStrengthLow[0];
        nsfProcCfg->spatialStrengthHigh[0] =
            gNsfProcessingCfg[level].spatialStrengthHigh[0];

        nsfProcCfg->staticFrameNoise[1] =
            gNsfProcessingCfg[level].staticFrameNoise[1];
        nsfProcCfg->spatialStrengthLow[1] =
            gNsfProcessingCfg[level].spatialStrengthLow[1];
        nsfProcCfg->spatialStrengthHigh[1] =
            gNsfProcessingCfg[level].spatialStrengthHigh[1];

        nsfProcCfg->staticFrameNoise[2] =
            gNsfProcessingCfg[level].staticFrameNoise[2];
        nsfProcCfg->spatialStrengthLow[2] =
            gNsfProcessingCfg[level].spatialStrengthLow[2];
        nsfProcCfg->spatialStrengthHigh[2] =
            gNsfProcessingCfg[level].spatialStrengthHigh[2];

        nsfProcCfg->temporalStrength =
            gNsfProcessingCfg[level].temporalStrength;
        nsfProcCfg->temporalTriggerNoise =
            gNsfProcessingCfg[level].temporalTriggerNoise;
        nsfProcCfg->noiseIirCoeff =
            gNsfProcessingCfg[level].noiseIirCoeff;
        nsfProcCfg->maxNoise =
            gNsfProcessingCfg[level].maxNoise;
        nsfProcCfg->pureBlackThres =
            gNsfProcessingCfg[level].pureBlackThres;
        nsfProcCfg->pureWhiteThres =
            gNsfProcessingCfg[level].pureWhiteThres;

        /*
         * Per channel bufInQue structure needs to be created & initialized.
         * bufInQue.fullQue will be populated with captured frames and
         * bufInQue.emptyQuewill be allocated here so that they can be
         * used later on for noise filtering.
         * Frames need to be allocated for bufInQue.emptyQue here.
         */
        /* Create the per channel bufInQue */
        status = Utils_bufCreate(&nsfChObj->bufInQue, TRUE, FALSE);
        UTILS_assert(status == FVID2_SOK);

        /* Fill format with channel specific values to allocate frame */
        format.channelNum = chId;
        format.width = pInChInfo->width;
        format.height =
            VpsUtils_align(pInChInfo->height, VPS_BUFFER_ALIGNMENT * 2);

        if (NSF_LINK_NUM_BUFS_PER_CH_DEFAULT == pObj->createArgs.numBufsPerCh)
        {
            numFrmsPerCh = NSF_LINK_FRAMES_PER_CH;
        }
        else
        {
			if(pObj->createArgs.numBufsPerCh > NSF_LINK_FRAMES_PER_CH)			
				numFrmsPerCh = NSF_LINK_FRAMES_PER_CH ;
			else
				numFrmsPerCh = pObj->createArgs.numBufsPerCh ;
        }

        format.pitch[0] = nsfDataFmt->outPitch[0];
        format.pitch[1] = nsfDataFmt->outPitch[1];
        format.pitch[2] = nsfDataFmt->outPitch[2];
        format.fieldMerged[0] = FALSE;
        format.fieldMerged[1] = FALSE;
        format.fieldMerged[2] = FALSE;
        format.dataFormat = FVID2_DF_YUV420SP_UV;
        format.scanFormat = FVID2_SF_PROGRESSIVE;
        format.bpp = FVID2_BPP_BITS8;                      /* ignored */

        /*
         * Alloc memory based on 'format'
         * Allocated frame info is put in frames[]
         * numFrmsPerCh is the number of frames per channel to
         * allocate.
         */
        if (nsfDataFmt->outMemType == VPS_VPDMA_MT_TILEDMEM)
        {
            status = Utils_tilerFrameAlloc(&format, nsfChObj->frames,
                                  numFrmsPerCh);
        }
        else
        {
            status = Utils_memFrameAlloc(&format, nsfChObj->frames,
                                numFrmsPerCh);
        }
        UTILS_assert(status == FVID2_SOK);

        /* Set remaining parameters for the allocated frames */
        for (frameId = 0; frameId < numFrmsPerCh; frameId++)
        {
            /* Point to the frame's array */
            frames = &nsfChObj->frames[frameId];
            frames->perFrameCfg = NULL;
            frames->subFrameInfo = NULL;
            frames->appData = &nsfChObj->frameInfo[frameId];

            nsfChObj->frameInfo[frameId].nsfChannelNum = frames->channelNum;
            nsfChObj->frameInfo[frameId].rtChInfoUpdate = FALSE;
            /* Finally, add this frame to the NsfLink_ChObj.bufInQue.emptyQue
             */
            status = Utils_bufPutEmptyFrame(&nsfChObj->bufInQue, frames);
            UTILS_assert(status == FVID2_SOK);
        }

        /* make initial previous frame point to first frame for first frame
         * of NF this wont be used */
        nsfChObj->pPrevOutFrame = &nsfChObj->frames[0];

        /* Populate the remaining fileds of
         * NsfLink_Obj.System_LinkInfo.System_LinkQueInfo.System_LinkChInfo
         * This information will be used by the next link to configure itself
         * properly. Structure used: info.queInfo[].chInfo[] Channels will be
         * divided equally between output queues */
        chPerQueue = (pObj->nsfCreateParams.numCh / pObj->createArgs.numOutQue);
        outChId = chId % chPerQueue;
        queueId = chId / chPerQueue;
        UTILS_assert(queueId < pObj->info.numQue);

        pObj->info.queInfo[queueId].chInfo[outChId].dataFormat =
            FVID2_DF_YUV420SP_UV;
        pObj->info.queInfo[queueId].chInfo[outChId].memType =
            (Vps_VpdmaMemoryType) nsfDataFmt->outMemType;
        pObj->info.queInfo[queueId].chInfo[outChId].startX = 0;
        pObj->info.queInfo[queueId].chInfo[outChId].startY = 0;
        pObj->info.queInfo[queueId].chInfo[outChId].width =
            nsfDataFmt->inFrameWidth;
        pObj->info.queInfo[queueId].chInfo[outChId].height =
            nsfDataFmt->inFrameHeight;
        pObj->info.queInfo[queueId].chInfo[outChId].pitch[0] =
            nsfDataFmt->outPitch[0];
        pObj->info.queInfo[queueId].chInfo[outChId].pitch[1] =
            nsfDataFmt->outPitch[1];
        pObj->info.queInfo[queueId].chInfo[outChId].pitch[2] =
            nsfDataFmt->outPitch[2];
        pObj->info.queInfo[queueId].chInfo[outChId].scanFormat =
            pInChInfo->scanFormat;

        /* Increase the number of channels now */
        pObj->info.queInfo[queueId].numCh++;
        memcpy (&nsfChObj->nsfRtParamDataFormat, nsfDataFmt,
                sizeof (Vps_NsfDataFormat));
    }

    return FVID2_SOK;
}

/*
 * Free allocated frames
 *
 * pObj - NSF driver information */
Int32 NsfLink_drvFreeFrames(NsfLink_Obj * pObj)
{
    UInt16 chId;
    FVID2_Format format;
    UInt32 tilerUsed = FALSE;
    Vps_NsfDataFormat *nsfDataFmt;
    NsfLink_ChObj *nsfChObj;
    UInt32 numFrmsPerCh;

    for (chId = 0; chId < pObj->nsfCreateParams.numCh; chId++)
    {
        nsfDataFmt = &pObj->nsfDataFormat[chId];
        nsfChObj = &pObj->linkChInfo[chId];

        /* fill format with channel specific values */
        format.channelNum = chId;
        format.width = nsfDataFmt->inFrameWidth;
        format.height =
            VpsUtils_align(nsfDataFmt->inFrameHeight, VPS_BUFFER_ALIGNMENT * 2);
        format.pitch[0] = nsfDataFmt->outPitch[0];
        format.pitch[1] = nsfDataFmt->outPitch[1];
        format.pitch[2] = nsfDataFmt->outPitch[2];
        format.fieldMerged[0] = FALSE;
        format.fieldMerged[1] = FALSE;
        format.fieldMerged[2] = FALSE;
        format.dataFormat = FVID2_DF_YUV420SP_UV;
        format.scanFormat = FVID2_SF_PROGRESSIVE;
        format.bpp = FVID2_BPP_BITS8;                      /* ignored */

        if (NSF_LINK_NUM_BUFS_PER_CH_DEFAULT == pObj->createArgs.numBufsPerCh)
        {
            numFrmsPerCh = NSF_LINK_FRAMES_PER_CH;
        }
        else
        {
			if(pObj->createArgs.numBufsPerCh > NSF_LINK_FRAMES_PER_CH)			
				numFrmsPerCh = NSF_LINK_FRAMES_PER_CH ;
			else
				numFrmsPerCh = pObj->createArgs.numBufsPerCh ;		
        }

        if (nsfDataFmt->outMemType == VPS_VPDMA_MT_TILEDMEM)
        {
            /*
             * Cannot free tiled frames
             */
            tilerUsed = TRUE;
        }
        else
        {
            /*
             * Free frames for this channel, based on format
             */
            Utils_memFrameFree(&format, nsfChObj->frames,
                               numFrmsPerCh);
        }
    }

    if (tilerUsed)
    {
        SystemTiler_freeAll();
    }

    return FVID2_SOK;
}

Int32 NsfLink_SetFrameRate(NsfLink_Obj * pObj, NsfLink_ChFpsParams * params)
{
    Int32 status = FVID2_SOK;
    NsfLink_ChObj *pChObj;

    if (params->chId < NSF_LINK_MAX_CH_PER_QUE)
    {
        pChObj = &pObj->linkChInfo[params->chId];

        pChObj->frameSkipCtx.firstTime = TRUE;
        pChObj->frameSkipCtx.inputFrameRate = params->inputFrameRate;
        pChObj->frameSkipCtx.outputFrameRate = params->outputFrameRate;
    }
    return (status);
}

Int32 NsfLink_drvSetNsfLevel(NsfLink_Obj * pObj, UInt32 level)
{
    Vps_NsfProcessingCfg *nsfProcCfg = pObj->nsfCreateParams.processingCfg;

	UTILS_assert(level <= 7);

	nsfProcCfg->staticFrameNoise[0] =
		gNsfProcessingCfg[level].staticFrameNoise[0];
	nsfProcCfg->spatialStrengthLow[0] =
		gNsfProcessingCfg[level].spatialStrengthLow[0];
	nsfProcCfg->spatialStrengthHigh[0] =
		gNsfProcessingCfg[level].spatialStrengthHigh[0];

	nsfProcCfg->staticFrameNoise[1] =
		gNsfProcessingCfg[level].staticFrameNoise[1];
	nsfProcCfg->spatialStrengthLow[1] =
		gNsfProcessingCfg[level].spatialStrengthLow[1];
	nsfProcCfg->spatialStrengthHigh[1] =
		gNsfProcessingCfg[level].spatialStrengthHigh[1];

	nsfProcCfg->staticFrameNoise[2] =
		gNsfProcessingCfg[level].staticFrameNoise[2];
	nsfProcCfg->spatialStrengthLow[2] =
		gNsfProcessingCfg[level].spatialStrengthLow[2];
	nsfProcCfg->spatialStrengthHigh[2] =
		gNsfProcessingCfg[level].spatialStrengthHigh[2];

	nsfProcCfg->temporalStrength =
		gNsfProcessingCfg[level].temporalStrength;
	nsfProcCfg->temporalTriggerNoise =
		gNsfProcessingCfg[level].temporalTriggerNoise;
	nsfProcCfg->noiseIirCoeff =
		gNsfProcessingCfg[level].noiseIirCoeff;
	nsfProcCfg->maxNoise =
		gNsfProcessingCfg[level].maxNoise;
	nsfProcCfg->pureBlackThres =
		gNsfProcessingCfg[level].pureBlackThres;
	nsfProcCfg->pureWhiteThres =
		gNsfProcessingCfg[level].pureWhiteThres;

	return FVID2_control(pObj->fvidHandleNsf, IOCTL_VPS_NSF_SET_PROCESSING_CFG, nsfProcCfg, NULL);
}

Int32 NsfLink_drvEnableSNF(NsfLink_Obj * pObj, UInt32 enableSNF)
{
    Vps_NsfProcessingCfg *nsfProcCfg = pObj->nsfCreateParams.processingCfg;
#ifdef SYSTEM_DEBUG_NSF
	Vps_printf("NSF: EnableSNF %d current mode %d!!!\n", enableSNF, nsfProcCfg->bypassMode);
#endif

	if (enableSNF)
	{
		switch (nsfProcCfg->bypassMode)
		{
			case VPS_NSF_DISABLE_SNF:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_NONE;
				break;
			case VPS_NSF_DISABLE_SNF_TNF:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_TNF;
				break;
			default:
				return FVID2_SOK;
		}
	}
	else
	{
		switch (nsfProcCfg->bypassMode)
		{
			case VPS_NSF_DISABLE_TNF:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_SNF_TNF;
				break;
			case VPS_NSF_DISABLE_NONE:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_SNF;
				break;
			default:
				return FVID2_SOK;
		}
	}

	return FVID2_control(pObj->fvidHandleNsf, IOCTL_VPS_NSF_SET_PROCESSING_CFG, nsfProcCfg, NULL);
}

Int32 NsfLink_drvEnableTNF(NsfLink_Obj * pObj, UInt32 enableTNF)
{
    Vps_NsfProcessingCfg *nsfProcCfg = pObj->nsfCreateParams.processingCfg;
#ifdef SYSTEM_DEBUG_NSF
	Vps_printf("NSF: EnableTNF %d current mode %d!!!\n", enableTNF, nsfProcCfg->bypassMode);
#endif

	if (enableTNF)
	{
		switch (nsfProcCfg->bypassMode)
		{
			case VPS_NSF_DISABLE_TNF:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_NONE;
				break;
			case VPS_NSF_DISABLE_SNF_TNF:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_SNF;
				break;
			default:
				return FVID2_SOK;
		}
	}
	else
	{
		switch (nsfProcCfg->bypassMode)
		{
			case VPS_NSF_DISABLE_SNF:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_SNF_TNF;
				break;
			case VPS_NSF_DISABLE_NONE:
				nsfProcCfg->bypassMode = VPS_NSF_DISABLE_TNF;
				break;
			default:
				return FVID2_SOK;
		}
	}

	return FVID2_control(pObj->fvidHandleNsf, IOCTL_VPS_NSF_SET_PROCESSING_CFG, nsfProcCfg, NULL);
}

/* ===================================================================
 *  @func     NsfLink_enableChannel
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
Int32 NsfLink_enableChannel(NsfLink_Obj *pObj,UInt32 channelNum)
{
	if(channelNum < pObj->nsfCreateParams.numCh)
	{
		pObj->createArgs.enable[channelNum] = 1;
	}
	
	return FVID2_SOK;
} 
 
 /* ===================================================================
 *  @func     NsfLink_disableChannel
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
Int32 NsfLink_disableChannel(NsfLink_Obj *pObj,UInt32 channelNum)
{
	if(channelNum < pObj->nsfCreateParams.numCh)
	{
		pObj->createArgs.enable[channelNum] = 0;
	}
	
	return FVID2_SOK;
}
