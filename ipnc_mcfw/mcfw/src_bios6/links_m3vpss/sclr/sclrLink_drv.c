/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "sclrLink_priv.h"
#include <mcfw/src_bios6/links_m3vpss/avsync/avsync_m3vpss.h>

// #define SCLR_LINK_QUEUE_REQ

Int32 SclrLink_resetStatistics(SclrLink_Obj * pObj)
{
    UInt32 chId;
    SclrLink_ChObj *pChObj;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChObj->inFrameRecvCount = 0;
        pChObj->inFrameRejectCount = 0;
        pChObj->inFrameProcessCount = 0;

        pChObj->outFrameUserSkipCount = 0;
        pChObj->outFrameSkipCount = 0;
        pChObj->outFrameCount = 0;

        pChObj->minLatency = 0xFF;
        pChObj->maxLatency = 0;
    }

    pObj->statsStartTime = Utils_getCurTimeInMsec();

    return 0;
}

Int32 SclrLink_drvPrintStatistics(SclrLink_Obj * pObj, Bool resetAfterPrint)
{
    UInt32 chId;
    SclrLink_ChObj *pChObj;
    UInt32 elaspedTime;

    elaspedTime = Utils_getCurTimeInMsec() - pObj->statsStartTime; // in msecs
    elaspedTime /= 1000; // convert to secs

    Vps_printf( " \n"
            " *** [%s] SCLR Statistics *** \n"
            " \n"
            " Elasped Time           : %d secs\n"
            " Total Fields Processed : %d \n"
            " Total Fields FPS       : %d FPS\n"
            " \n"
            " \n"
            " CH  | In Recv In Reject In Process Out Skip Out User Out Latency   \n"
            " Num | FPS     FPS       FPS        FPS    FPS   Skip FPS Min / Max \n"
            " ---------------------------------------------------------------------\n",
            pObj->name,
            elaspedTime,
            pObj->getProcessFrameCount,
            pObj->getProcessFrameCount * 100 / (pObj->totalTime / 10)
                    );

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
       pChObj = &pObj->chObj[chId];

        Vps_printf( " %3d | %7d %9d %10d %3d %8d %8d %3d %3d\n",
            chId,
            pChObj->inFrameRecvCount/elaspedTime,
            pChObj->inFrameRejectCount/elaspedTime,
            pChObj->inFrameProcessCount/elaspedTime,
            pChObj->outFrameCount/elaspedTime,
            pChObj->outFrameSkipCount/elaspedTime,
            pChObj->outFrameUserSkipCount/elaspedTime,
            pChObj->minLatency,
            pChObj->maxLatency
            );
    }

    Vps_printf( " \n");

    if(resetAfterPrint)
    {
        SclrLink_resetStatistics(pObj);
    }
    return FVID2_SOK;
}

Int32 SclrLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    SclrLink_Obj *pObj = (SclrLink_Obj *) appData;

#ifdef SCLR_LINK_QUEUE_REQ
    Utils_tskSendCmd(&pObj->tsk, DEI_LINK_CMD_GET_PROCESSED_DATA);
#else
    Semaphore_post(pObj->complete);
#endif
    return FVID2_SOK;
}

Int32 SclrLink_drvFvidErrCb(FVID2_Handle handle,
                            Ptr appData, Ptr errList, Ptr reserved)
{
    return FVID2_SOK;
}

Int32  SclrLink_drvCreateReqObj(SclrLink_Obj * pObj)
{
    Int32 status;
    UInt32 reqId;

    memset(pObj->reqObj, 0, sizeof(pObj->reqObj));

    status = Utils_queCreate(&pObj->reqQue,
                             SCLR_LINK_MAX_REQ,
                             pObj->reqQueMem, UTILS_QUE_FLAG_NO_BLOCK_QUE);
    UTILS_assert(status == FVID2_SOK);

    pObj->reqQueCount = 0;
    pObj->isReqPend = FALSE;

    for (reqId = 0; reqId < SCLR_LINK_MAX_REQ; reqId++)
    {
        status =
            Utils_quePut(&pObj->reqQue, &pObj->reqObj[reqId], BIOS_NO_WAIT);
        UTILS_assert(status == FVID2_SOK);
    }

    pObj->reqNumOutLists = 1;

    return FVID2_SOK;
}

Int32 SclrLink_drvCreateOutObj(SclrLink_Obj * pObj)
{
    Int32 status;
    SclrLink_OutObj *pOutObj;
    System_LinkChInfo *pChInfo;
    UInt32 frameId,chId;
    System_LinkChInfo *pInChInfo;
    System_LinkChInfo *pOutChInfo;        
    FVID2_Format *pFormat;
    UInt32 maxOutWidth = 0u, maxOutHeight = 0u;
    UInt32 numFrmsPerCh;

    memset(&pObj->outFrameDrop, 0, sizeof(pObj->outFrameDrop));

    maxOutWidth = pObj->inQueInfo.chInfo[0].width;
    maxOutHeight = pObj->inQueInfo.chInfo[0].height;
    if (pObj->createArgs.scaleMode == SCLR_SCALE_MODE_RATIO)
    {
        maxOutHeight = (maxOutHeight * pObj->createArgs.outScaleFactor.ratio.heightRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.heightRatio.denominator;
        maxOutWidth = (maxOutWidth * pObj->createArgs.outScaleFactor.ratio.widthRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.widthRatio.denominator;
    }
    else
    {
        maxOutHeight = pObj->createArgs.outScaleFactor.absoluteResolution.outHeight;
        maxOutWidth = pObj->createArgs.outScaleFactor.absoluteResolution.outWidth;
    }
#if defined (TI814X_DVR) || defined (TI814X_EVM) ||defined (TI8107_EVM )
    maxOutWidth = VpsUtils_floor(maxOutWidth, 16);
#else
    maxOutWidth = VpsUtils_floor(maxOutWidth, VPS_BUFFER_ALIGNMENT);
#endif   

    pOutObj = &pObj->outObj;

    if (SCLR_LINK_NUM_BUFS_PER_CH_DEFAULT == pObj->createArgs.numBufsPerCh)
    {
        numFrmsPerCh = SCLR_LINK_MAX_OUT_FRAMES_PER_CH;
    }
    else
    {
        numFrmsPerCh = pObj->createArgs.numBufsPerCh ;
    }

    pOutObj->outNumFrames = pObj->inQueInfo.numCh * numFrmsPerCh;

    UTILS_assert(pObj->inQueInfo.numCh <= SCLR_LINK_MAX_CH);
    UTILS_assert(pOutObj->outNumFrames <= SCLR_LINK_MAX_OUT_FRAMES);

    status = Utils_bufCreate(&pOutObj->bufOutQue, TRUE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    pFormat = &pOutObj->outFormat;
    pChInfo = &pObj->info.queInfo[0].chInfo[0];       
    
    pFormat->dataFormat     = pObj->createArgs.outDataFormat; // FVID2_DF_YUV422I_YUYV;//FVID2_DF_YUV422SP_UV; FVID2_DF_YUV420SP_UV
#if defined (TI814X_DVR) || defined (TI814X_EVM) ||defined (TI8107_EVM )

    if (pFormat->dataFormat == FVID2_DF_YUV420SP_VU)
    {
        // scaler driver checks only for FVID2_DF_YUV420SP_UV format so
        // forcing the dataformat to FVID2_DF_YUV420SP_UV
        pFormat->dataFormat = FVID2_DF_YUV420SP_UV;
    }
#endif   
    if( pFormat->dataFormat == FVID2_DF_YUV420SP_UV)
    {
        if (pObj->createArgs.tilerEnable)
        {
            pChInfo->pitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
            pChInfo->pitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
        }
        else
        {
#if defined (TI814X_DVR) || defined (TI814X_EVM) ||defined (TI8107_EVM )
            pChInfo->pitch[0] =
                VpsUtils_align(maxOutWidth, 16 * 2);
            pChInfo->pitch[1] =
                VpsUtils_align(maxOutWidth, 16 * 2);
#else
            pChInfo->pitch[0] =
                VpsUtils_align(maxOutWidth, VPS_BUFFER_ALIGNMENT * 2);
            pChInfo->pitch[1] =
                VpsUtils_align(maxOutWidth, VPS_BUFFER_ALIGNMENT * 2);
#endif   
        }
        pChInfo->pitch[2] = 0;
    }
    else
    {
#if defined (TI814X_DVR) || defined (TI814X_EVM) ||defined (TI8107_EVM )
        pChInfo->pitch[0] = VpsUtils_align(maxOutWidth, 16)*2;
#else
        pChInfo->pitch[0] = VpsUtils_align(maxOutWidth, VPS_BUFFER_ALIGNMENT)*2;
#endif   
        pChInfo->pitch[1] = pChInfo->pitch[2] = 0;
    }
    
    pFormat->channelNum     = 0;
    pFormat->width          = maxOutWidth;
    pFormat->height         = maxOutHeight;
    pFormat->fieldMerged[0] = FALSE;
    pFormat->fieldMerged[1] = FALSE;
    pFormat->fieldMerged[2] = FALSE;
    
    pFormat->pitch[0]       = pChInfo->pitch[0];
    pFormat->pitch[1]       = pChInfo->pitch[1];
    pFormat->pitch[2]       = pChInfo->pitch[2];
    pFormat->scanFormat     = FVID2_SF_PROGRESSIVE;
    pFormat->bpp            = FVID2_BPP_BITS16;
    pFormat->reserved       = NULL;


    /* alloc buffer of max possible size but use only what is needed for a
     * given resolution */
    if (pObj->createArgs.tilerEnable)
    {
        status = Utils_tilerFrameAlloc(&pOutObj->outFormat,
                                     pOutObj->outFrames,
                                     pOutObj->outNumFrames);
    }
    else
    {
        status = Utils_memFrameAlloc(&pOutObj->outFormat,
                                     pOutObj->outFrames,
                                     pOutObj->outNumFrames);
    }
    UTILS_assert(status == FVID2_SOK);

    for (frameId = 0; frameId < pObj->outObj.outNumFrames; frameId++)
    {
        pObj->outObj.frameInfo[frameId].rtChInfoUpdate = FALSE;
        pObj->outObj.outFrames[frameId].appData = &pObj->outObj.frameInfo[frameId];

        status = Utils_bufPutEmptyFrame(&pObj->outObj.bufOutQue,
                                        &pObj->outObj.outFrames[frameId]);
        UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_VERBOSE_PRINTS
                {
                    Vps_rprintf(" %d: SCLR: %2d: 0x%08x, %d x %d, %08x B\n",
                                Utils_getCurTimeInMsec(),
                                frameId, pOutObj->outFrames[frameId].addr[0][0],
                                pOutObj->outFormat.pitch[0] / 2,
                            pOutObj->outFormat.height,
                            pOutObj->outFormat.height *
                            pOutObj->outFormat.pitch[0]);
                }
#endif
    }

    pObj->info.numQue = SCLR_LINK_MAX_OUT_QUE;
    pObj->info.queInfo[0].numCh = pObj->inQueInfo.numCh;

    for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];

        pOutChInfo = &pObj->info.queInfo[0].chInfo[chId];

        pFormat = &pObj->outObj.outFormat;
        if (pObj->createArgs.tilerEnable)
        {
            pOutChInfo->memType = VPS_VPDMA_MT_TILEDMEM;
        }
        else
        {
            pOutChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
        }

        pOutChInfo->dataFormat = (FVID2_DataFormat) pFormat->dataFormat;

        /* Out Width / Height can be derived from pFormat calculated in previous for loop;
         *  still this is per channel, hence use pInChInfo and recalculate
         */
        pOutChInfo->width = pInChInfo->width;
        pOutChInfo->height = pInChInfo->height;

        if (pObj->createArgs.scaleMode == SCLR_SCALE_MODE_RATIO)
        {
            pOutChInfo->height = (pOutChInfo->height * pObj->createArgs.outScaleFactor.ratio.heightRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.heightRatio.denominator;
            pOutChInfo->width = (pOutChInfo->width * pObj->createArgs.outScaleFactor.ratio.widthRatio.numerator) / pObj->createArgs.outScaleFactor.ratio.widthRatio.denominator;
        }
        else
        {
            pOutChInfo->height = pObj->createArgs.outScaleFactor.absoluteResolution.outHeight;
            pOutChInfo->width = pObj->createArgs.outScaleFactor.absoluteResolution.outWidth;
        }
#if defined (TI814X_DVR) || defined (TI814X_EVM) ||defined (TI8107_EVM )
        pOutChInfo->width = VpsUtils_floor(pOutChInfo->width, 16);
#else
        pOutChInfo->width = VpsUtils_floor(pOutChInfo->width, VPS_BUFFER_ALIGNMENT);
#endif   

        pOutChInfo->startX = 0;
        pOutChInfo->startY = 0;
        pOutChInfo->pitch[0] = pFormat->pitch[0];
        pOutChInfo->pitch[1] = pFormat->pitch[1];
        pOutChInfo->pitch[2] = pFormat->pitch[2];
        pOutChInfo->scanFormat = (FVID2_ScanFormat) pFormat->scanFormat;
    }

    return (status);
}

Int32 SclrLink_drvCreateChObj(SclrLink_Obj * pObj, UInt32 chId)
{
    SclrLink_ChObj *pChObj;
    System_LinkChInfo *pInChInfo;
    System_LinkChInfo *pOutChInfo;
    Vps_M2mScChParams *pDrvChParams;
    FVID2_Format *pFormat;
    Int32 status;
    pChObj = &pObj->chObj[chId];

    status = Utils_bufCreate(&pChObj->inQue, FALSE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    pChObj->nextFid = 0;

    pInChInfo = &pObj->inQueInfo.chInfo[chId];


    pDrvChParams = &pObj->drvChArgs[chId];

    pFormat = &pDrvChParams->inFmt;

    pFormat->channelNum = chId;
    pFormat->width = pInChInfo->startX + pInChInfo->width;
    pFormat->height = pInChInfo->startY + pInChInfo->height;

    pFormat->fieldMerged[0] = FALSE;
    pFormat->fieldMerged[1] = FALSE;
    pFormat->fieldMerged[2] = FALSE;
    pFormat->pitch[0] = pInChInfo->pitch[0];
    pFormat->pitch[1] = pInChInfo->pitch[1];
    pFormat->pitch[2] = pInChInfo->pitch[2];
#if defined (TI814X_DVR) || defined (TI814X_EVM) ||defined (TI8107_EVM )
    if (pInChInfo->dataFormat == FVID2_DF_YUV420SP_VU)
    {
        // scaler driver checks only for FVID2_DF_YUV420SP_UV format so
        // forcing the dataformat to FVID2_DF_YUV420SP_UV
        pInChInfo->dataFormat = FVID2_DF_YUV420SP_UV;
    }
#endif   
    pFormat->dataFormat = pInChInfo->dataFormat;
    pFormat->scanFormat = pInChInfo->scanFormat;//FVID2_SF_PROGRESSIVE;
    pFormat->bpp = FVID2_BPP_BITS16;
    pFormat->reserved = NULL;

    if(pObj->createArgs.enableLineSkipSc && pInChInfo->memType == VPS_VPDMA_MT_NONTILEDMEM)
    {
        /* half the height and double the pitch
            possible only when input is non-tiled
         */
        pFormat->pitch[0] *= 2;
        pFormat->pitch[1] *= 2;
        pFormat->height /= 2;
    }

    pDrvChParams->inMemType = pInChInfo->memType;

    pChObj->curFrameNum = 0;
    pChObj->frameSkipCount = 0;
    pChObj->frameSkipCtx.firstTime = TRUE;

    pChObj->frameSkipCtx.firstTime = TRUE;
    pChObj->frameSkipCtx.inputFrameRate = pObj->createArgs.inputFrameRate;
    pChObj->frameSkipCtx.outputFrameRate = pObj->createArgs.outputFrameRate;

    pChObj->doFrameDrop = FALSE;

    pChObj->enableOut = TRUE;
    
    pOutChInfo = &pObj->info.queInfo[0].chInfo[chId];

    pFormat = &pChObj->outFormat;

    if (pInChInfo->width < pOutChInfo->width
        || pInChInfo->height < pOutChInfo->height)
    {
        pObj->loadUpsampleCoeffs = TRUE;
    }

    pFormat->channelNum = chId;
    pFormat->width = pOutChInfo->width;
    pFormat->height = pOutChInfo->height;
    pFormat->fieldMerged[0] = FALSE;
    pFormat->fieldMerged[1] = FALSE;
    pFormat->fieldMerged[2] = FALSE;
    pFormat->pitch[0] = pOutChInfo->pitch[0];
    pFormat->pitch[1] = pOutChInfo->pitch[1];
    pFormat->pitch[2] = pOutChInfo->pitch[2];
    pFormat->dataFormat = pOutChInfo->dataFormat;
    pFormat->scanFormat = pOutChInfo->scanFormat;
    pFormat->bpp = FVID2_BPP_BITS16;
    pFormat->reserved = NULL;

    /* Initilaize the rtparm ouput resolution from outObj */
    pChObj->chRtOutInfoUpdate = FALSE;

    pChObj->isSkipOneFidType = FALSE;
    pChObj->skipFidType = FVID2_FID_BOTTOM;

    memset(&pChObj->sclrRtPrm, 0, sizeof(pChObj->sclrRtPrm));

    pChObj->sclrRtOutFrmPrm.width = 
            pObj->outObj.outFormat.width;
    pChObj->sclrRtOutFrmPrm.height = 
            pObj->outObj.outFormat.height;
    pChObj->sclrRtOutFrmPrm.pitch[0] = 
            pObj->outObj.outFormat.pitch[0];
    pChObj->sclrRtOutFrmPrm.pitch[1] = 
            pObj->outObj.outFormat.pitch[1];
    pChObj->sclrRtOutFrmPrm.pitch[2] = 
            pObj->outObj.outFormat.pitch[2];
    pChObj->sclrRtOutFrmPrm.memType = pOutChInfo->memType;

    pChObj->sclrRtOutFrmPrm.dataFormat = pOutChInfo->dataFormat;

    pChObj->scCfg.bypass = FALSE;
    pChObj->scCfg.nonLinear = FALSE;
    pChObj->scCfg.stripSize = 0;
    pChObj->scCfg.vsType = VPS_SC_VST_POLYPHASE;
    pChObj->scCfg.hsType = VPS_SC_HST_AUTO;

    pChObj->scCropConfig.cropStartX = pInChInfo->startX;
    pChObj->scCropConfig.cropWidth = pInChInfo->width;
    pChObj->scCropConfig.cropStartY = pInChInfo->startY;
    pChObj->scCropConfig.cropHeight = pInChInfo->height;


    if(pObj->createArgs.enableLineSkipSc && pInChInfo->memType == VPS_VPDMA_MT_NONTILEDMEM)
    {
        /* half the height and crop start Y
            possible only when input is non-tiled
         */
        pChObj->scCropConfig.cropStartY /= 2;
        pChObj->scCropConfig.cropHeight /= 2;
    }
    

    return FVID2_SOK;
}


Int32 SclrLink_drvSetScCoeffs(FVID2_Handle fvidHandle, Bool loadUpsampleCoeffs) 
{
    Int32 retVal = FVID2_SOK;
    Vps_ScCoeffParams coeffPrms;

    if (loadUpsampleCoeffs)
    {
        Vps_rprintf(" %d: SCLR    : Loading Up-scaling Co-effs ... \n",
                    Utils_getCurTimeInMsec());

        coeffPrms.hScalingSet = VPS_SC_US_SET;
        coeffPrms.vScalingSet = VPS_SC_US_SET;
    }
    else
    {
        Vps_rprintf(" %d: SCLR    : Loading Down-scaling Co-effs ... \n",
                    Utils_getCurTimeInMsec());

        coeffPrms.hScalingSet = VPS_SC_DS_SET_0;
        coeffPrms.vScalingSet = VPS_SC_DS_SET_0;
    }
    coeffPrms.coeffPtr = NULL;
    coeffPrms.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;

    /* Program scalar coefficient - Always used */
    retVal = FVID2_control(fvidHandle, IOCTL_VPS_SET_COEFFS, &coeffPrms, NULL);
    UTILS_assert(FVID2_SOK == retVal);

    Vps_rprintf(" %d: SCLR    : Co-effs Loading ... DONE !!!\n",
                Utils_getCurTimeInMsec());

    return (retVal);
}


Int32 SclrLink_drvCreateFvidObj(SclrLink_Obj * pObj)
{
    Vps_M2mScChParams *pDrvChPrm;
    UInt32 chId;
    System_LinkChInfo *pChInfo;
    FVID2_CbParams cbParams;
    UInt32 pathId = VPS_M2M_INST_SEC0_SC5_WB2;

    pObj->scCreateParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    pObj->scCreateParams.numChannels = pObj->inQueInfo.numCh;;
    pObj->scCreateParams.chParams = (Vps_M2mScChParams *) pObj->drvChArgs;

    for (chId = 0; chId < pObj->scCreateParams.numChannels; chId++)
    {
        pDrvChPrm = &pObj->drvChArgs[chId];

        /* assume all CHs are of same input size, format, pitch */
        pChInfo = &pObj->inQueInfo.chInfo[chId];
                
        memcpy(&pDrvChPrm->outFmt, &pObj->outObj.outFormat, sizeof(FVID2_Format));

        if (pObj->createArgs.tilerEnable)
            pDrvChPrm->outMemType = VPS_VPDMA_MT_TILEDMEM;
        else
            pDrvChPrm->outMemType = VPS_VPDMA_MT_NONTILEDMEM;

        pDrvChPrm->scCfg = &pObj->chObj[chId].scCfg;
        pDrvChPrm->srcCropCfg = &pObj->chObj[chId].scCropConfig;

        /* Initilaize the rtparm input resolution */
        pObj->chObj[chId].sclrRtInFrmPrm.width = pChInfo->startX + pDrvChPrm->inFmt.width;
        pObj->chObj[chId].sclrRtInFrmPrm.height = pChInfo->startY + pDrvChPrm->inFmt.height;
        pObj->chObj[chId].sclrRtInFrmPrm.pitch[0] = pDrvChPrm->inFmt.pitch[0];
        pObj->chObj[chId].sclrRtInFrmPrm.pitch[1] = pDrvChPrm->inFmt.pitch[1];
        pObj->chObj[chId].sclrRtInFrmPrm.pitch[2] = pDrvChPrm->inFmt.pitch[2];
        pObj->chObj[chId].sclrRtInFrmPrm.memType = pChInfo->memType;
        pObj->chObj[chId].sclrRtInFrmPrm.dataFormat = pChInfo->dataFormat;

    }

    memset(&cbParams, 0, sizeof(cbParams));
    cbParams.cbFxn = SclrLink_drvFvidCb;
    cbParams.errCbFxn = SclrLink_drvFvidErrCb;
    cbParams.errList = &pObj->errProcessList;
    cbParams.appData = pObj;

    switch(pObj->createArgs.pathId)
    {
        case SCLR_LINK_SC5:
            pathId = VPS_M2M_INST_SEC0_SC5_WB2;
            break;
        case SCLR_LINK_SEC0_SC3:
            pathId = VPS_M2M_INST_SEC0_SC3_VIP0;
            break;
		case SCLR_LINK_SEC1_SC4:
            pathId = VPS_M2M_INST_SEC1_SC4_VIP1;
            break;

        default:
            UTILS_assert(FALSE);
    }

    pObj->fvidHandle = FVID2_create(FVID2_VPS_M2M_SC_DRV,
                                       pathId,
                                       &pObj->scCreateParams,
                                       &pObj->scCreateStatus, &cbParams);
    UTILS_assert(pObj->fvidHandle != NULL);

    SclrLink_drvSetScCoeffs(pObj->fvidHandle, TRUE);   /* TODO:
                                                                 * change
                                                                 * based on
                                                                 * scaling
                                                                 * ratio */
    return FVID2_SOK;
}

Int32 SclrLink_drvQueueFramesToChQue(SclrLink_Obj * pObj)
{
    UInt32 frameId, freeFrameNum;
    FVID2_Frame *pFrame;
    System_LinkInQueParams *pInQueParams;
    FVID2_FrameList frameList;
    SclrLink_ChObj *pChObj;
    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
#ifdef SYSTEM_DEBUG_SCLR_RT
        Vps_printf(" %d: SCLR    : Received %d IN frames !!!\n",
                   Utils_getCurTimeInMsec(), frameList.numFrames);
#endif

        pObj->inFrameGetCount += frameList.numFrames;

        freeFrameNum = 0;

        // in interlace mode, we need top and bottom field to do filed merge mode,
        // so we should treate one top and one bottom as a frame and then do frame drop.
        // so we'll only do frame drop logic when nextFid = 0 for interlace stuff.
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            pChObj = &pObj->chObj[pFrame->channelNum];

            pChObj->inFrameRecvCount++;
            
            if ((pChObj->isSkipOneFidType) && (pFrame->fid == pChObj->skipFidType))
            {
                pChObj->inFrameRejectCount++;
                // frame is not of expected FID, so release frame
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                continue;
            }
               
            if ((pObj->inQueInfo.chInfo[pFrame->channelNum].scanFormat ==
                                               FVID2_SF_PROGRESSIVE) ||
                                               (pChObj->nextFid == 0) ||
                                               (pChObj->isSkipOneFidType))
                pChObj->doFrameDrop = Utils_doSkipFrame(&(pChObj->frameSkipCtx));
            
            /* frame skipped due to user setting */
            if(pChObj->doFrameDrop)
            {
                pChObj->frameSkipCount++;
                pChObj->outFrameUserSkipCount++;
            }

            //pChObj->nextFid = pFrame->fid;
            if ((pObj->inQueInfo.chInfo[pFrame->channelNum].scanFormat !=
                                                FVID2_SF_INTERLACED) ||
                (pChObj->isSkipOneFidType))
            {
                pChObj->nextFid = pFrame->fid;
            }

            if (((pChObj->nextFid == pFrame->fid) ||
                 (pFrame->fid == FVID2_FID_FRAME)) &&
                 (pChObj->doFrameDrop == FALSE))
            {
                // frame is of the expected FID use it, else drop it
                status = Utils_bufPutFullFrame(&pChObj->inQue, pFrame);
                UTILS_assert(status == FVID2_SOK);
                pChObj->nextFid ^= 1;  // toggle to next
            }
            else
            {
                pChObj->inFrameRejectCount++;

                // frame is not of expected FID, so release frame
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
                if (pChObj->nextFid == pFrame->fid)
                {
                    pChObj->nextFid ^= 1;  // toggle to next
                }
            }
        }

        if (freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;

#ifdef SYSTEM_DEBUG_SCLR_RT
            Vps_printf(" %d: SCLR    : Skipped %d IN frames !!!\n",
                       Utils_getCurTimeInMsec(), frameList.numFrames);
#endif

            pObj->inFramePutCount += freeFrameNum;

            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId, &frameList);
        }
    }

    return FVID2_SOK;
}

Int32 SclrLink_drvCreate(SclrLink_Obj * pObj, SclrLink_CreateParams * pPrm)
{
    Semaphore_Params semParams;
    Int32 status;
    UInt32 chId;
#ifdef TI_814X_BUILD
    Vps_PlatformCpuRev cpuRev;
#endif

#ifdef SYSTEM_DEBUG_SCLR
    Vps_printf(" %d: SCLR: Create in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    UTILS_MEMLOG_USED_START();
    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);
    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

#ifndef SYSTEM_USE_TILER
    if (pObj->createArgs.tilerEnable)
    {
        Vps_printf("SCLR:!!WARNING.FORCIBLY DISABLING TILER since tiler is disabled at build time");
        pObj->createArgs.tilerEnable = FALSE;
    }
#endif

#ifdef TI_814X_BUILD
    cpuRev = Vps_platformGetCpuRev();

	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        if (cpuRev < VPS_PLATFORM_CPU_REV_2_0)
        {
            if((pObj->inQueInfo.chInfo[chId].width > SCLR_DRV_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0) && 
                 ((pObj->inQueInfo.chInfo[chId].dataFormat != FVID2_DF_YUV420SP_UV) ||
                   (pObj->inQueInfo.chInfo[chId].dataFormat != FVID2_DF_YUV420SP_VU) ||
                    (pObj->inQueInfo.chInfo[chId].dataFormat != FVID2_DF_YUV420P)))
            {

                Vps_printf(" %u: Warning: This CPU Revision [%s] does not support current set width %d\n",\
                    Utils_getCurTimeInMsec(), gVpss_cpuVer[cpuRev],\
                                              pObj->inQueInfo.chInfo[chId].width);
                Vps_printf(" %u: Warning: Limiting Input width to 960\n",Utils_getCurTimeInMsec());
                {
                    pObj->inQueInfo.chInfo[chId].width = SCLR_DRV_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0;
                    pObj->inQueInfo.chInfo[chId].pitch[0] = 2*SCLR_DRV_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0;
                    pObj->inQueInfo.chInfo[chId].pitch[1] = 2*SCLR_DRV_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0;
                    pObj->inQueInfo.chInfo[chId].pitch[2] = 2*SCLR_DRV_MAX_WIDTH_LIMIT_BEFORE_CPU_REV_2_0;
                }
            }
        }
    }
#endif

    pObj->inFrameGetCount = 0;
    pObj->inFramePutCount = 0;
    pObj->outFrameGetCount = 0;
    pObj->outFramePutCount = 0;
    pObj->processFrameReqPendCount = 0;
    pObj->processFrameReqPendSubmitCount = 0;
    pObj->processFrameCount = 0;
    pObj->getProcessFrameCount = 0;
    pObj->processFrameReqCount = 0;
    pObj->getProcessFrameReqCount = 0;
    pObj->totalTime = 0;
    pObj->curTime = 0;
    pObj->givenInFrames = 0x0;
    pObj->returnedInFrames = 0x0;
    pObj->loadUpsampleCoeffs = FALSE;

    SclrLink_resetStatistics(pObj);


    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->complete = Semaphore_create(0u, &semParams, NULL);
    UTILS_assert(pObj->complete != NULL);

    SclrLink_drvCreateOutObj(pObj);

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        SclrLink_drvCreateChObj(pObj, chId);
    }
    SclrLink_drvCreateFvidObj(pObj);
//    SclrLink_drvCreateScDrv(pObj);
    SclrLink_drvCreateReqObj(pObj);
    UTILS_MEMLOG_USED_END(pObj->memUsed);
    UTILS_MEMLOG_PRINT("SCLR",
                       pObj->memUsed,
                       UTILS_ARRAYSIZE(pObj->memUsed));

#ifdef SYSTEM_DEBUG_SCLR
    Vps_printf(" %d: SCLR: Create Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}

Int32 SclrLink_drvMakeFrameLists(SclrLink_Obj * pObj,
                                FVID2_FrameList * inFrameList,
                                 FVID2_FrameList * outFrameList)
{
    SclrLink_ChObj *pChObj;
    UInt32 chId, frameId;
    FVID2_Frame *pInFrame, *pOutFrame;
    Int32 status;
    Bool rtParamUpdatePerFrame;
    Vps_FrameParams *rtParam;
    System_FrameInfo *pInFrameInfo;
    System_FrameInfo *pOutFrameInfo;
    FVID2_FrameList frameList;
    UInt32 freeFrameNum;
   
    frameId = 0;
    freeFrameNum = 0;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        Utils_bufGetFullFrame(&pChObj->inQue, &pInFrame, BIOS_NO_WAIT);

        if (pInFrame)
        {
            pOutFrame = NULL;
            if((pChObj->enableOut) && 
                Utils_queGetQueuedCount (&pObj->outObj.bufOutQue.emptyQue))
            {

                status = Utils_bufGetEmptyFrame(&pObj->outObj.bufOutQue, 
                                                &pOutFrame,
                                                BIOS_WAIT_FOREVER);
                UTILS_assert(status == FVID2_SOK);
                UTILS_assert(pOutFrame != NULL);

                inFrameList->frames[frameId] = pInFrame;
                pInFrameInfo = (System_FrameInfo *) pInFrame->appData;
                pChObj->inFrameProcessCount++;

                pOutFrameInfo = (System_FrameInfo *) pOutFrame->appData;
                UTILS_assert(pOutFrameInfo != NULL);

                pObj->outFrameGetCount++;
                pChObj->outFrameCount++;
                outFrameList->frames[frameId] = pOutFrame;
                frameId++;
                pChObj->curFrameNum++;

                pInFrame->perFrameCfg = NULL;
                rtParamUpdatePerFrame = FALSE;
                UTILS_assert (pInFrame->channelNum == chId);
                rtParam = &pChObj->sclrRtInFrmPrm;
                if ((pInFrameInfo != NULL) && 
                    (pInFrameInfo->rtChInfoUpdate == TRUE))
                {
                    if (pInFrameInfo->rtChInfo.height != rtParam->height)
                    {
                        rtParam->height = pInFrameInfo->rtChInfo.height;
                        pChObj->scCropConfig.cropHeight = rtParam->height;
                        rtParamUpdatePerFrame = TRUE;
                    }
                    if (pInFrameInfo->rtChInfo.width != rtParam->width)
                    {
                        rtParam->width = pInFrameInfo->rtChInfo.width;
                        pChObj->scCropConfig.cropWidth = rtParam->width;
                        rtParamUpdatePerFrame = TRUE;
                    }
                    if (pInFrameInfo->rtChInfo.pitch[0] != rtParam->pitch[0])
                    {
                        rtParam->pitch[0] = pInFrameInfo->rtChInfo.pitch[0];
                        rtParamUpdatePerFrame = TRUE;
                    }
                    if (pInFrameInfo->rtChInfo.pitch[0] != rtParam->pitch[1])
                    {
                        rtParam->pitch[1] = pInFrameInfo->rtChInfo.pitch[0];
                        rtParamUpdatePerFrame = TRUE;
                    }
                }

                if ((pChObj->chRtOutInfoUpdate == TRUE) ||
                    (rtParamUpdatePerFrame == TRUE))
                {
                    pInFrame->perFrameCfg = &pChObj->sclrRtPrm;
                    pChObj->sclrRtPrm.inFrmPrms = 
                                     &pChObj->sclrRtInFrmPrm;
                    pChObj->sclrRtPrm.outFrmPrms = 
                                     &pChObj->sclrRtOutFrmPrm;
                    pChObj->sclrRtPrm.srcCropCfg = 
                                     &pChObj->scCropConfig;
                    pChObj->chRtOutInfoUpdate = FALSE;
                }
                pOutFrameInfo->rtChInfo.width = 
                               pChObj->sclrRtOutFrmPrm.width;
                pOutFrameInfo->rtChInfo.height = 
                               pChObj->sclrRtOutFrmPrm.height;
                pOutFrameInfo->rtChInfo.pitch[0] = 
                               pChObj->sclrRtOutFrmPrm.pitch[0];
                pOutFrameInfo->rtChInfo.pitch[1] = 
                               pChObj->sclrRtOutFrmPrm.pitch[1];
                pOutFrameInfo->rtChInfoUpdate = TRUE;
                if (pInFrameInfo != NULL)
                    pOutFrameInfo->ts64  = pInFrameInfo->ts64;

                pOutFrame->channelNum = pInFrame->channelNum;
                pOutFrame->timeStamp  = pInFrame->timeStamp;
                pOutFrame->fid = pInFrame->fid;
            }
            else
            {
                pChObj->outFrameSkipCount++;
                frameList.frames[freeFrameNum] = pInFrame;
                freeFrameNum++;
            }
        }
    }
    inFrameList->numFrames  = frameId;
    outFrameList->numFrames = frameId;

    if (freeFrameNum)
    {
        frameList.numFrames = freeFrameNum;
        System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
                                   pObj->createArgs.inQueParams.prevLinkQueId,
                                   &frameList);
        pObj->inFramePutCount += freeFrameNum;
    }

    return FVID2_SOK;
}

Int32 SclrLink_drvReleaseFrames(SclrLink_Obj * pObj,
                               FVID2_FrameList * inFrameList,
                               FVID2_FrameList outFrameList)
{
    UInt32 frameId, latency;
    FVID2_Frame *pFrame;
    Int32 status, sendCmd;
    SclrLink_ChObj *pChObj;

    sendCmd = FALSE;
    for (frameId = 0; frameId < inFrameList->numFrames; frameId++)
    {
         pFrame = outFrameList.frames[frameId];
         if (pFrame && pFrame != &pObj->outFrameDrop)
         {
              pChObj = &pObj->chObj[pFrame->channelNum];

              latency = Utils_getCurTimeInMsec() - pFrame->timeStamp;

              if(latency>pChObj->maxLatency)
                  pChObj->maxLatency = latency;
              if(latency<pChObj->minLatency)
                  pChObj->minLatency = latency;

             status =
                 Utils_bufPutFullFrame(&pObj->outObj.bufOutQue,
                                   pFrame);
             UTILS_assert(status == FVID2_SOK);

             sendCmd = TRUE;

             pObj->outFramePutCount++;
         }
    }

#ifdef SYSTEM_DEBUG_SCLR_RT
    Vps_printf(" %d: SCLR    : Released %d IN frames !!!\n", Utils_getCurTimeInMsec(),
               inFrameList->numFrames);
#endif

    pObj->inFramePutCount += inFrameList->numFrames;

    System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
                   pObj->createArgs.inQueParams.prevLinkQueId, inFrameList);

    if(sendCmd == TRUE)
    {
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
    }

    return FVID2_SOK;
}

Int32 SclrLink_drvSubmitData(SclrLink_Obj * pObj)
{
    Int32 status;
    SclrLink_ReqObj *pReqObj;

    status = Utils_queGet(&pObj->reqQue, (Ptr *) & pReqObj, 1, BIOS_NO_WAIT);
    if (status != FVID2_SOK)
    {
#ifdef SYSTEM_DEBUG_SCLR_RT
        Vps_printf(" %d: SCLR    : Pending request !!!\n", Utils_getCurTimeInMsec());
#endif

        pObj->processFrameReqPendCount++;

        pObj->isReqPend = TRUE;

        // cannot process more frames to process
        return FVID2_EFAIL;
    }


    if (pObj->processFrameReqPendCount == pObj->processFrameReqPendSubmitCount)
        pObj->isReqPend = FALSE;

    pReqObj->processList.inFrameList[0] = &pReqObj->inFrameList;
    pReqObj->processList.outFrameList[0] = &pReqObj->outFrameList;
    pReqObj->processList.numInLists = 1;
    pReqObj->processList.numOutLists = pObj->reqNumOutLists;

    pReqObj->inFrameList.appData = pReqObj;

    SclrLink_drvMakeFrameLists(pObj, &pReqObj->inFrameList,
                              &pReqObj->outFrameList);

#ifdef SYSTEM_DEBUG_SCLR_RT
    Vps_printf(" %d: SCLR    : Submitting %d frames !!!\n", Utils_getCurTimeInMsec(),
               pReqObj->inFrameList.numFrames);
#endif

    if (pReqObj->inFrameList.numFrames)
    {

        pObj->reqQueCount++;

        pObj->processFrameCount += pReqObj->inFrameList.numFrames;
        pObj->processFrameReqCount++;

        pObj->givenInFrames += pReqObj->inFrameList.numFrames;

        pObj->curTime = Utils_getCurTimeInMsec();
        status = FVID2_processFrames(pObj->fvidHandle, &pReqObj->processList);
        UTILS_assert(status == FVID2_SOK);

#ifndef SCLR_LINK_QUEUE_REQ
        Semaphore_pend(pObj->complete, BIOS_WAIT_FOREVER);

        SclrLink_drvGetProcessedData(pObj);
#endif

    }
    else
    {

        status = Utils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
        UTILS_assert(status == FVID2_SOK);

        // no more frames to process
        status = FVID2_EFAIL;
    }

    return status;
}

Int32 SclrLink_drvProcessData(SclrLink_Obj * pObj)
{
    Int32 status;

    SclrLink_drvQueueFramesToChQue(pObj);

    do
    {
        status = SclrLink_drvSubmitData(pObj);
    } while (status == FVID2_SOK);

    return FVID2_SOK;
}

Int32 SclrLink_drvGetProcessedData(SclrLink_Obj * pObj)
{
    SclrLink_ReqObj *pReqObj;
    Int32 status;
    FVID2_ProcessList processList;

    status =
        FVID2_getProcessedFrames(pObj->fvidHandle, &processList, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);

    pObj->curTime = Utils_getCurTimeInMsec() - pObj->curTime;
    pObj->totalTime += pObj->curTime;

#ifdef SYSTEM_DEBUG_SCLR_RT
    Vps_printf(" %d: SCLR    : Completed %d frames !!!\n", Utils_getCurTimeInMsec(),
               processList.outFrameList[0]->numFrames);
#endif

    pObj->getProcessFrameCount += processList.outFrameList[0]->numFrames;
    pObj->getProcessFrameReqCount++;

    pReqObj = (SclrLink_ReqObj *) processList.inFrameList[0]->appData;

    SclrLink_drvReleaseFrames(pObj, &pReqObj->inFrameList,
                             pReqObj->outFrameList);

    status = Utils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);

    pObj->reqQueCount--;

    if (pObj->isReqPend)
    {
#ifdef SYSTEM_DEBUG_SCLR_RT
        Vps_printf(" %d: SCLR    : Submitting pending request !!!\n",
                   Utils_getCurTimeInMsec());
#endif

        pObj->processFrameReqPendSubmitCount++;

        SclrLink_drvSubmitData(pObj);
    }
    return FVID2_SOK;
}

Int32 SclrLink_drvStop(SclrLink_Obj * pObj)
{
    Int32 rtnValue = FVID2_SOK;

#ifdef SYSTEM_DEBUG_SCLR
    Vps_printf(" %d: SCLR    : Stop in progress, %d requests pending !!!\n",
               Utils_getCurTimeInMsec(), pObj->reqQueCount);
#endif

    while (pObj->reqQueCount)
    {
        Utils_tskWaitCmd(&pObj->tsk, NULL, SCLR_LINK_CMD_GET_PROCESSED_DATA);
        SclrLink_drvGetProcessedData(pObj);
    }

#ifdef SYSTEM_DEBUG_SCLR
    Vps_printf(" %d: SCLR    : Stop Done !!!\n", Utils_getCurTimeInMsec());
#endif
    return (rtnValue);
}



Int32 SclrLink_drvDelete(SclrLink_Obj * pObj)
{
    UInt32 chId;
    SclrLink_ChObj *pChObj;

#ifdef SYSTEM_DEBUG_SCLR
    Vps_printf(" %d: SCLR    : Fields = %d (fps = %d), FRAMES -> Total %d, skipped %d, fps = %d!!!\n",
                Utils_getCurTimeInMsec(),
                pObj->getProcessFrameCount,
                pObj->getProcessFrameCount * 100 / (pObj->totalTime / 10),
                pObj->chObj[0].curFrameNum,
                pObj->chObj[0].frameSkipCount,
                (pObj->chObj[0].curFrameNum - pObj->chObj[0].frameSkipCount) * 100 / (pObj->totalTime / 10)
                );
#endif

#ifdef SYSTEM_DEBUG_SCLR
    Vps_printf(" %d: SCLR: Delete in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    FVID2_delete(pObj->fvidHandle, NULL);

    Semaphore_delete(&pObj->complete);

    Utils_bufDelete(&pObj->outObj.bufOutQue);

    Utils_memFrameFree(&pObj->outObj.outFormat,
                       pObj->outObj.outFrames, pObj->outObj.outNumFrames);

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        Utils_bufDelete(&pChObj->inQue);
    }

    Utils_queDelete(&pObj->reqQue);
    
#ifdef SYSTEM_DEBUG_SCLR
    Vps_printf(" %d: SCLR: Delete Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}

Int32 SclrLink_SetFrameRate(SclrLink_Obj * pObj, SclrLink_ChFpsParams * params)
{
    Int32 status = FVID2_SOK;
    SclrLink_ChObj *pChObj;

    pChObj = &pObj->chObj[params->chId];

    pChObj->frameSkipCtx.firstTime = TRUE;
    pChObj->frameSkipCtx.inputFrameRate = params->inputFrameRate;
    pChObj->frameSkipCtx.outputFrameRate = params->outputFrameRate;

    return (status);
}

Int32 SclrLink_drvSetChannelInfo(SclrLink_Obj * pObj, SclrLink_ChannelInfo *channelInfo)
{
	Int32 status = FVID2_SOK;
    SclrLink_ChObj *pChObj;

    UTILS_assert(channelInfo->channelId < (SCLR_LINK_MAX_CH * 2));


    pChObj = &pObj->chObj[channelInfo->channelId];
    pChObj->enableOut = channelInfo->enable;

    return status;

}

Int32 SclrLink_drvGetChDynamicOutputRes(SclrLink_Obj * pObj, 
                                       SclrLink_chDynamicSetOutRes * params)
{
    Int32 status = FVID2_SOK;
    SclrLink_ChObj *pChObj;
    UInt32 chId;

    if (params->chId < SCLR_LINK_MAX_CH)
    {
        chId = params->chId;
        pChObj = &pObj->chObj[chId];

        params->width = pChObj->sclrRtOutFrmPrm.width;
        params->height = pChObj->sclrRtOutFrmPrm.height;
        params->pitch[0] = pChObj->sclrRtOutFrmPrm.pitch[0];
        params->pitch[1] = pChObj->sclrRtOutFrmPrm.pitch[1];
        params->pitch[2] = pChObj->sclrRtOutFrmPrm.pitch[2];
    }
    return (status);
}

Int32 SclrLink_drvSetChDynamicOutputRes(SclrLink_Obj * pObj, 
                                       SclrLink_chDynamicSetOutRes * params)
{
    Int32 status = FVID2_SOK;
    SclrLink_ChObj *pChObj;
    UInt32 chId;

    if (params->chId < SCLR_LINK_MAX_CH)
    {
        chId = params->chId;
        pChObj = &pObj->chObj[chId];

        pChObj->chRtOutInfoUpdate = TRUE;

        pChObj->sclrRtOutFrmPrm.width    = params->width;
        pChObj->sclrRtOutFrmPrm.height   = params->height;
        pChObj->sclrRtOutFrmPrm.pitch[0] = params->pitch[0];
        pChObj->sclrRtOutFrmPrm.pitch[1] = params->pitch[1];
    }
    return (status);
}

Int32 SclrLink_drvDynamicSkipFidType(SclrLink_Obj * pObj, 
                                     SclrLink_chDynamicSkipFidType * params)
{
    Int32 status = FVID2_SOK;
    SclrLink_ChObj *pChObj;
    UInt32 chId;

    if (params->chId < SCLR_LINK_MAX_CH)
    {
        chId = params->chId;
        pChObj = &pObj->chObj[chId];

        pChObj->isSkipOneFidType = TRUE;
        pChObj->skipFidType = params->fidType;
    }
    return (status);
}

