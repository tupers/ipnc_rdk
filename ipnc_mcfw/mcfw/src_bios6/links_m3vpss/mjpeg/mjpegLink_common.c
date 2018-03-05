/** ==================================================================
 *  @file  mjpegLink_common.c
 *
 *  @path   $(IPNC_PATH)/ipnc_mcfw/mcfw/src_bios6/links_m3vpss/mjpeg
 *
 *  @desc   This  File contains the implementation of various functions
 *           such as codec creation, frame processing etc routines.
 *  =====================================================================
 *  Copyright (c) Texas Instruments Inc 2012, 2011
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <mcfw/src_bios6/utils/utils_mem.h>
#include <mcfw/interfaces/link_api/system_tiler.h>
#include "mjpegLink_priv.h"
#include <ti/psp/iss/alg/rm/inc/rm.h>
#include "mjpegLink_enc.h"
/*------------------------------------------*/
/* Some variables for statistics collection */
/*------------------------------------------*/
int cntInFrames = 0;

int cntOutFrames = 0;

int cntProcessedFrames = 0;

int cntSkippedFrames = 0;

/*---------------------------------------------------------------------*/
/* MACRO decides whether frame needs to be encoded in a seperate task */
/*---------------------------------------------------------------------*/
#define JPEG_ENC_DONE_IN_SEPERATE_TASK (1)

/*---------------------------------------------------------------------*/
/* Function prototypes of various static functions */
/*---------------------------------------------------------------------*/
/* ===================================================================
 *  @func     MjpegLink_codecQueueFramesToChQue
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
static Int32 MjpegLink_codecQueueFramesToChQue(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecCreateReqObj
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
static Int32 MjpegLink_codecCreateReqObj(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecCreateOutObj
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
static Int32 MjpegLink_codecCreateOutObj(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecCreateChObj
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
static Int32 MjpegLink_codecCreateChObj(MjpegLink_Obj * pObj, UInt32 chId);

/* ===================================================================
 *  @func     mjpeglink_populate_outbuf_pool_size_info
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
static Int mjpeglink_populate_outbuf_pool_size_info(System_LinkQueInfo *
                                                    inQueInfo,
                                                    MjpegLink_OutObj *
                                                    outQueInfo);
/* ===================================================================
 *  @func     MjpegLink_codecCreateEncObj
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
static Int32 MjpegLink_codecCreateEncObj(MjpegLink_Obj * pObj, UInt32 chId);

/* ===================================================================
 *  @func     MjpegLink_codecCreateProcessTsk
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
static Int32 MjpegLink_codecCreateProcessTsk(MjpegLink_Obj * pObj,
                                             UInt32 tskId);
/* ===================================================================
 *  @func     MjpegLink_codecDeleteProcessTsk
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
static Int32 MjpegLink_codecDeleteProcessTsk(MjpegLink_Obj * pObj,
                                             UInt32 tskId);
/* ===================================================================
 *  @func     MjpegLink_codecSubmitData
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
static Int32 MjpegLink_codecSubmitData(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecGetProcessedData
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
static Int32 MjpegLink_codecGetProcessedData(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecProcessTskFunctionality
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
static Void MjpegLink_codecProcessTskFunctionality(MjpegLink_Obj * pObj,
                                                   UInt32 tskId,
                                                   MjpegLink_ReqObj * pReqObj);
/* ===================================================================
 *  @func     MjpegLink_codecProcessTskFxn
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
Void MjpegLink_codecProcessTskFxn(UArg arg1, UArg arg2);

/* ===================================================================
 *  @func     mjpeglink_populate_outbuf_pool_size_info
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
static Int mjpeglink_populate_outbuf_pool_size_info(System_LinkQueInfo *
                                                    inQueInfo,
                                                    MjpegLink_OutObj *
                                                    outQueInfo)
{
    Int i;

    Int status = MJPEG_LINK_S_SUCCESS;

    UInt key;

    key = Hwi_disable();

    outQueInfo->numAllocPools = inQueInfo->numCh;
    for (i = 0; i < outQueInfo->numAllocPools; i++)
    {
        outQueInfo->outNumBufs[i] = MJPEG_LINK_MAX_OUT_FRAMES_PER_CH;
        outQueInfo->buf_size[i] = 2048 * 1080 * 2;         /* Worst Case Size
                                                            */
        outQueInfo->ch2poolMap[i] = i;
    }

    Hwi_restore(key);
    return status;
}

/* ===================================================================
 *  @func     MjpegLink_codecCreateReqObj
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
static Int32 MjpegLink_codecCreateReqObj(MjpegLink_Obj * pObj)
{
    Int32 status;

    UInt32 reqId;

    memset(pObj->reqObj, 0, sizeof(pObj->reqObj));

    status = Utils_queCreate(&pObj->reqQue,
                             MJPEG_LINK_MAX_REQ,
                             pObj->reqQueMem, UTILS_QUE_FLAG_BLOCK_QUE_GET);
    UTILS_assert(status == FVID2_SOK);

    pObj->reqQueCount = 0;
    pObj->isReqPend = FALSE;

    for (reqId = 0; reqId < MJPEG_LINK_MAX_REQ; reqId++)
    {
        status =
            Utils_quePut(&pObj->reqQue, &pObj->reqObj[reqId], BIOS_NO_WAIT);
        UTILS_assert(status == FVID2_SOK);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_codecPrdCalloutFcn
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
static Void MjpegLink_codecPrdCalloutFcn(UArg arg)
{
    MjpegLink_Obj *pObj = (MjpegLink_Obj *) arg;

    Int32 status;

    status =
        System_sendLinkCmd(pObj->linkId, MJPEG_LINK_CMD_GET_PROCESSED_DATA);

    if (UTILS_ISERROR(status))
    {
#ifdef SYSTEM_DEBUG_CMD_ERROR
        UTILS_warn("MJPEGLINK:[%s:%d]:"
                   "System_sendLinkCmd MJPEG_LINK_CMD_GET_PROCESSED_DATA failed"
                   "errCode = %d", __FILE__, __LINE__, status);
#endif
    }

}

/* ===================================================================
 *  @func     MjpegLink_codecCreatePrdObj
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
static Int32 MjpegLink_codecCreatePrdObj(MjpegLink_Obj * pObj)
{
    Clock_Params clockParams;

    Clock_Params_init(&clockParams);
    clockParams.arg = (UArg) pObj;
    UTILS_assert(pObj->periodicObj.clkHandle == NULL);

    Clock_construct(&(pObj->periodicObj.clkStruct),
                    MjpegLink_codecPrdCalloutFcn, 1, &clockParams);

    pObj->periodicObj.clkHandle = Clock_handle(&pObj->periodicObj.clkStruct);
    pObj->periodicObj.clkStarted = FALSE;

    return MJPEG_LINK_S_SUCCESS;

}

/* ===================================================================
 *  @func     MjpegLink_codecDeletePrdObj
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
static Int32 MjpegLink_codecDeletePrdObj(MjpegLink_Obj * pObj)
{
    /* Stop the clock */
    Clock_stop(pObj->periodicObj.clkHandle);
    Clock_destruct(&(pObj->periodicObj.clkStruct));
    pObj->periodicObj.clkHandle = NULL;
    pObj->periodicObj.clkStarted = FALSE;

    return MJPEG_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     MjpegLink_codecStartPrdObj
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
static Int32 MjpegLink_codecStartPrdObj(MjpegLink_Obj * pObj, UInt period)
{

    UTILS_assert(pObj->periodicObj.clkHandle != NULL);

    if (FALSE == pObj->periodicObj.clkStarted)
    {
        /* Start the clock */
        Clock_setPeriod(pObj->periodicObj.clkHandle, period);
        Clock_setTimeout(pObj->periodicObj.clkHandle, period);
        Clock_start(pObj->periodicObj.clkHandle);
        pObj->periodicObj.clkStarted = TRUE;
    }

    return MJPEG_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     MjpegLink_codecStopPrdObj
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
static Int32 MjpegLink_codecStopPrdObj(MjpegLink_Obj * pObj)
{

    UTILS_assert(pObj->periodicObj.clkHandle != NULL);

    if (TRUE == pObj->periodicObj.clkStarted)
    {
        Clock_stop(pObj->periodicObj.clkHandle);
        pObj->periodicObj.clkStarted = FALSE;
    }
    return MJPEG_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     MjpegLink_codecCreateOutObj
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
static Int32 MjpegLink_codecCreateOutObj(MjpegLink_Obj * pObj)
{
    MjpegLink_OutObj *pOutObj;

    Int32 status;

    UInt32 chId, bufIdx, outId;

    System_LinkChInfo *pOutChInfo;

    Int i;

    UInt32 totalBufCnt;

    mjpeglink_populate_outbuf_pool_size_info(&pObj->inQueInfo, &pObj->outObj);

    pOutObj = &pObj->outObj;
    status = Utils_bitbufCreate(&pOutObj->bufOutQue, TRUE, FALSE,
                                pObj->outObj.numAllocPools);
    UTILS_assert(status == FVID2_SOK);

    status = Utils_queCreate(&pObj->processDoneQue,
                             MJPEG_LINK_MAX_OUT_FRAMES,
                             pObj->processDoneQueMem,
                             (UTILS_QUE_FLAG_BLOCK_QUE_GET |
                              UTILS_QUE_FLAG_BLOCK_QUE_PUT));
    UTILS_assert(status == FVID2_SOK);

    totalBufCnt = 0;
    for (i = 0; i < pOutObj->numAllocPools; i++)
    {
        status = Utils_memBitBufAlloc(&(pOutObj->outBufs[totalBufCnt]),
                                      pOutObj->buf_size[i],
                                      pOutObj->outNumBufs[i]);
        UTILS_assert(status == FVID2_SOK);

        for (bufIdx = 0; bufIdx < pOutObj->outNumBufs[i]; bufIdx++)
        {
            UTILS_assert((bufIdx + totalBufCnt) < MJPEG_LINK_MAX_OUT_FRAMES);
            pOutObj->outBufs[bufIdx + totalBufCnt].allocPoolID = i;
            pOutObj->outBufs[bufIdx + totalBufCnt].appData =
                &(pOutObj->frameInfo[bufIdx + totalBufCnt]);
            status =
                Utils_bitbufPutEmptyBuf(&pOutObj->bufOutQue,
                                        &pOutObj->outBufs[bufIdx +
                                                          totalBufCnt]);
            UTILS_assert(status == FVID2_SOK);
        }
        totalBufCnt += pOutObj->outNumBufs[i];
    }
    pObj->info.numQue = MJPEG_LINK_MAX_OUT_QUE;
    for (outId = 0u; outId < MJPEG_LINK_MAX_OUT_QUE; outId++)
    {
        pObj->info.queInfo[outId].numCh = pObj->inQueInfo.numCh;
    }

    for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
    {
        for (outId = 0u; outId < MJPEG_LINK_MAX_OUT_QUE; outId++)
        {
            pOutChInfo = &pObj->info.queInfo[outId].chInfo[chId];
            pOutChInfo->bufType = SYSTEM_BUF_TYPE_VIDBITSTREAM;
            pOutChInfo->codingformat =
                pObj->createArgs.chCreateParams[chId].format;
            pOutChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
            pOutChInfo->scanFormat = pObj->inQueInfo.chInfo[chId].scanFormat;
            pOutChInfo->width = pObj->inQueInfo.chInfo[chId].width;
            pOutChInfo->height = pObj->inQueInfo.chInfo[chId].height;
        }
    }

    return (status);
}

/* ===================================================================
 *  @func     MjpegLink_codecCreateChObj
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
static Int32 MjpegLink_codecCreateChObj(MjpegLink_Obj * pObj, UInt32 chId)
{
    MjpegLink_ChObj *pChObj;

    Int32 status;

    pChObj = &pObj->chObj[chId];

    status = Utils_queCreate(&pChObj->inQue, MJPEG_LINK_MAX_REQ,
                             pChObj->inFrameMem, UTILS_QUE_FLAG_BLOCK_QUE_GET);
    UTILS_assert(status == FVID2_SOK);

    pChObj->nextFid = FVID2_FID_TOP;
    pChObj->totalInFrameCnt = 0;
    pChObj->width = pObj->inQueInfo.chInfo[chId].width;
    pChObj->height = pObj->inQueInfo.chInfo[chId].height;
    pChObj->pitch = pObj->inQueInfo.chInfo[chId].pitch[0];
    pChObj->startX = pObj->inQueInfo.chInfo[chId].startX;
    pChObj->startY = pObj->inQueInfo.chInfo[chId].startY;
	pChObj->inputChromaFormat = pObj->inQueInfo.chInfo[chId].dataFormat;
	pChObj->isChromaFormat420 = ((pChObj->inputChromaFormat ==  FVID2_DF_YUV420SP_UV) || (pChObj->inputChromaFormat ==  FVID2_DF_YUV420SP_VU))? 1 : 0;
    Vps_rprintf
        ("MJPEG Enc Channel#%3d ::: Width X Height (%3d X %3d) and Pitch is %3d \n",
         chId, pChObj->width, pChObj->height, pChObj->pitch);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     mjpeglink_codec_set_ch_alg_create_params
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
static Int32 mjpeglink_codec_set_ch_alg_create_params(MjpegLink_Obj * pObj,
                                                      UInt32 chId)
{

    return MJPEG_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     mjpeglink_codec_set_ch_alg_default_dynamic_params
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
static Int32 mjpeglink_codec_set_ch_alg_default_dynamic_params(MjpegLink_Obj *
                                                               pObj,
                                                               UInt32 chId)
{
    MjpegLink_ChObj *pChObj;

    pChObj = &pObj->chObj[chId];
    pChObj->mjpegQp =
        pObj->createArgs.chCreateParams[chId].defaultDynamicParams.mjpegQp;
    pChObj->frameSkipModulo =
        pObj->createArgs.chCreateParams[chId].
        defaultDynamicParams.skipFrameFactor;
    return MJPEG_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     MjpegLink_codecCreateEncObj
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
static Int32 MjpegLink_codecCreateEncObj(MjpegLink_Obj * pObj, UInt32 chId)
{
    Int retVal;

    MjpegLink_ChObj *pChObj;

    pChObj = &pObj->chObj[chId];

    {
        pChObj->numReqObjPerProcess = 1;
    }

    mjpeglink_codec_set_ch_alg_create_params(pObj, chId);
    mjpeglink_codec_set_ch_alg_default_dynamic_params(pObj, chId);

    retVal = MSP_JPEGE_INC_init(&pChObj->algObj.alg.jpegEncComponent);

    return retVal;
}

/* ===================================================================
 *  @func     MjpegLink_codecProcessTskFunctionality
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
static Void MjpegLink_codecProcessTskFunctionality(MjpegLink_Obj * pObj,
                                                   UInt32 tskId,
                                                   MjpegLink_ReqObj * pReqObj)
{
    Int32 status, chId, i;
	UInt32 paddingBytes;

    MjpegLink_ChObj *pChObj;

    MjpegLink_ReqList reqList;

    status = FVID2_SOK;
    reqList.numReqObj = 0;

    {
        reqList.reqObj[reqList.numReqObj++] = pReqObj;
        chId = pReqObj->OutBuf->channelNum;
        pChObj = &pObj->chObj[chId];
        pChObj->processReqestCount--;
    }
    /* Encode Call ----> */
    {
        UInt32 latency;

        latency = Utils_getCurTimeInMsec();
        pReqObj->OutBuf->fillLength =
            (McFW_JPEG_Enc
             (pChObj->algObj.alg.jpegEncComponent,
              pReqObj->InFrameList.frames[0], pReqObj->OutBuf->addr,
              pChObj->width, pChObj->height, pChObj->pitch, pChObj->startX,
              pChObj->startY, pChObj->mjpegQp, 2048 * 1080 * 2, &paddingBytes, pChObj->isChromaFormat420));
        latency = Utils_getCurTimeInMsec() - latency;
        if (latency > pChObj->maxLatency)
            pChObj->maxLatency = latency;
        if (latency < pChObj->minLatency)
            pChObj->minLatency = latency;

        pReqObj->OutBuf->frameWidth = pChObj->width /* 1920 */ ;
        pReqObj->OutBuf->frameHeight = pChObj->height /* 1080 */ ;
        pReqObj->OutBuf->timeStamp = pReqObj->InFrameList.frames[0]->timeStamp;
        pReqObj->OutBuf->startOffset = paddingBytes;

    }
    /* Encode Call <---- */

    for (i = 0; i < reqList.numReqObj; i++)
    {
        status = Utils_quePut(&pObj->processDoneQue, pReqObj, BIOS_NO_WAIT);
        UTILS_assert(status == FVID2_SOK);
    }
}

/* ===================================================================
 *  @func     MjpegLink_codecProcessTskFxn
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
Void MjpegLink_codecProcessTskFxn(UArg arg1, UArg arg2)
{
    MjpegLink_Obj *pObj;

    MjpegLink_ReqObj *pReqObj;

    UInt32 tskId;

    UInt32 iteratonCnt = 0;

#if JPEG_ENC_DONE_IN_SEPERATE_TASK
#else
    Semaphore_Params params;

    Semaphore_Handle semHdle;

    Semaphore_Params_init(&params);
    semHdle = Semaphore_create(0, &params, NULL);
#endif

    pObj = (MjpegLink_Obj *) arg1;
    tskId = (UInt32) arg2;

    Vps_rprintf("MJPEG Enc Task Func of Task#%d started.\n", tskId);
    while (pObj->state != SYSTEM_LINK_STATE_STOP)
    {
        iteratonCnt++;

#if JPEG_ENC_DONE_IN_SEPERATE_TASK
        Utils_queGet(&pObj->simcopProcessTsk[tskId].processQue,
                     (Ptr *) & pReqObj, 1, BIOS_WAIT_FOREVER);

        MjpegLink_codecProcessTskFunctionality(pObj, tskId, pReqObj);
#else
        Semaphore_pend(semHdle, 10);
#endif
    }

    return;
}

#pragma DATA_ALIGN(gMjpegProcessTskStack, 32)
#pragma DATA_SECTION(gMjpegProcessTskStack, ".bss:taskStackSection")
UInt8 gMjpegProcessTskStack[MJPEG_LINK_MAX_CH][MJPEG_LINK_PROCESS_TSK_SIZE];

/* ===================================================================
 *  @func     MjpegLink_codecCreateProcessTsk
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
static Int32 MjpegLink_codecCreateProcessTsk(MjpegLink_Obj * pObj, UInt32 tskId)
{
    Int32 status = MJPEG_LINK_S_SUCCESS;

    Task_Params tskParams;

    Error_Block ebObj;

    Error_Block *eb = &ebObj;

    Error_init(eb);

    Task_Params_init(&tskParams);

    snprintf(pObj->simcopProcessTsk[tskId].name,
             (sizeof(pObj->simcopProcessTsk[tskId].name) - 1),
             "MJPEG_PROCESS_TSK_%d ", tskId);
    pObj->simcopProcessTsk[tskId].
        name[(sizeof(pObj->simcopProcessTsk[tskId].name) - 1)] = 0;

    tskParams.priority = MJPEG_LINK_TSK_PRI + 1;
    tskParams.stack = &gMjpegProcessTskStack[tskId][0];
    tskParams.stackSize = MJPEG_LINK_PROCESS_TSK_SIZE;
    tskParams.arg0 = (UArg) pObj;
    tskParams.arg1 = (UArg) tskId;
    tskParams.instance->name = pObj->simcopProcessTsk[tskId].name;

    Task_construct(&pObj->simcopProcessTsk[tskId].tskStruct,
                   MjpegLink_codecProcessTskFxn, &tskParams, eb);
    UTILS_assertError((Error_check(eb) == FALSE), status,
                      MJPEG_LINK_E_TSKCREATEFAILED, pObj->linkId, tskId);
    if (MJPEG_LINK_S_SUCCESS == status)
    {
        pObj->simcopProcessTsk[tskId].tsk =
            Task_handle(&pObj->simcopProcessTsk[tskId].tskStruct);
    }
    Utils_prfLoadRegister(pObj->simcopProcessTsk[tskId].tsk,
                          pObj->simcopProcessTsk[tskId].name);
    return status;
}

/* ===================================================================
 *  @func     MjpegLink_codecDeleteProcessTsk
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
static Int32 MjpegLink_codecDeleteProcessTsk(MjpegLink_Obj * pObj, UInt32 tskId)
{
    Int32 status = MJPEG_LINK_S_SUCCESS;

    Utils_queUnBlock(&pObj->simcopProcessTsk[tskId].processQue);
    Utils_queUnBlock(&pObj->processDoneQue);

    pObj->state = SYSTEM_LINK_STATE_STOP;                  /* Setting this
                                                            * makes the task
                                                            * exit by itself */
    while (Task_getMode(pObj->simcopProcessTsk[tskId].tsk) !=
           Task_Mode_TERMINATED)
    {
        Task_sleep(MJPEG_LINK_TASK_POLL_DURATION_MS);
    }

    Utils_prfLoadUnRegister(pObj->simcopProcessTsk[tskId].tsk);
    Task_destruct(&pObj->simcopProcessTsk[tskId].tskStruct);
    pObj->simcopProcessTsk[tskId].tsk = NULL;
    return status;
}

/* ===================================================================
 *  @func     MjpegLink_codecMapCh2ProcessTskId
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
static Int32 MjpegLink_codecMapCh2ProcessTskId(MjpegLink_Obj * pObj)
{
    UInt32 chId;

    Int32 status = MJPEG_LINK_S_SUCCESS;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        /* Channel to task mapping is one to one */
        pObj->ch2ProcessTskId[chId] = chId;
    }

    return status;
}

/* ===================================================================
 *  @func     MjpegLink_codecCreateInitStats
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
static void MjpegLink_codecCreateInitStats(MjpegLink_Obj * pObj)
{
    Int32 chId;

    MjpegLink_ChObj *pChObj;

    pObj->inFrameGetCount = 0;
    pObj->inFramePutCount = 0;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChObj->prevFrmRecvTime = 0;
        pChObj->totalProcessTime = 0;
        pChObj->totalFrameIntervalTime = 0;
        pChObj->totalInFrameCnt = 0;
        pChObj->inFrameQueCount = 0;
        pChObj->processReqestCount = 0;
        pChObj->getProcessedFrameCount = 0;
        pChObj->inFrameSkipCount = 0;
        pChObj->disableChn = FALSE;
        pChObj->curFrameNum = 0;
        pChObj->algObj.setConfigBitMask = 0;
        pChObj->frameStatus.firstTime = TRUE;
        pChObj->frameStatus.inCnt = 0;
        pChObj->frameStatus.outCnt = 0;
        pChObj->frameStatus.multipleCnt = 0;
    }

    return;
}

/* ===================================================================
 *  @func     MjpegLink_codecCreate
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
Int32 MjpegLink_codecCreate(MjpegLink_Obj * pObj, MjpegLink_CreateParams * pPrm)
{
    Int32 status;

    Int32 chId, tskId;

    MjpegLink_ChObj *pChObj;

#ifdef SYSTEM_DEBUG_MJPEG
    Vps_printf(" %d: MJPEG ENC: Create in progress ... !!!\n",
               Clock_getTicks());
#endif

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));
    UTILS_assert(pObj->inQueInfo.numCh <= MJPEG_LINK_MAX_CH);

#if MJPEGLINK_UPDATE_CREATEPARAMS_LOCALLY
    /* ENABLE the define MJPEGLINK_UPDATE_CREATEPARAMS_LOCALLY if App is not
     * configuring the create time paramters */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pObj->createArgs.chCreateParams[chId] = MJPEGLINK_DEFAULTCHCREATEPARAMS;
    }
#endif

    MjpegLink_codecCreateInitStats(pObj);
    MjpegLink_codecMapCh2ProcessTskId(pObj);
    MjpegLink_codecCreateOutObj(pObj);
    MjpegLink_codecCreateReqObj(pObj);
    pObj->state = SYSTEM_LINK_STATE_START;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChObj->nextFid = FVID2_FID_TOP;
        pChObj->inFrameQueCount = 0;
        pChObj->processReqestCount = 0;
        pChObj->getProcessedFrameCount = 0;
        MjpegLink_codecCreateChObj(pObj, chId);
        MjpegLink_codecCreateEncObj(pObj, chId);
        pChObj->mjpegQp = 80;                              /* Default value
                                                            * of QP */
        pChObj->frameSkipModulo = 12;                      /* Default value
                                                            * of
                                                            * frameSkipCounter
                                                            */
#ifdef SYSTEM_DEBUG_MJPEG
        Vps_printf
            (" %d: MJPEG ENC: Creating CH%d of %d x %d, pitch = (%d, %d) [%s] [%s], QP = %d  ... \n",
             Clock_getTicks(), chId, pObj->inQueInfo.chInfo[chId].width,
             pObj->inQueInfo.chInfo[chId].height,
             pObj->inQueInfo.chInfo[chId].pitch[0],
             pObj->inQueInfo.chInfo[chId].pitch[1],
             gSystem_nameScanFormat[pObj->inQueInfo.chInfo[chId].scanFormat],
             gSystem_nameMemoryType[pObj->inQueInfo.chInfo[chId].memType],
             pChObj->mjpegQp = 80);
#endif
    }

    for (tskId = 0; tskId < pObj->inQueInfo.numCh; tskId++)
    {

        status = Utils_queCreate(&pObj->simcopProcessTsk[tskId].processQue,
                                 (MJPEG_LINK_MAX_OUT_FRAMES /
                                  MJPEG_LINK_MAX_CH),
                                 pObj->simcopProcessTsk[tskId].processQueMem,
                                 (UTILS_QUE_FLAG_BLOCK_QUE_GET |
                                  UTILS_QUE_FLAG_BLOCK_QUE_PUT));
        UTILS_assert(status == FVID2_SOK);
        MjpegLink_codecCreateProcessTsk(pObj, tskId);
    }

    MjpegLink_codecCreatePrdObj(pObj);
    MjpegLink_codecStartPrdObj(pObj, MJPEG_LINK_PROCESS_DONE_PERIOD_MS);

#ifdef SYSTEM_DEBUG_MJPEG
    Vps_printf(" %d: MJPEG ENC: Create ... DONE !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_codecGetProcessedData
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
static Int32 MjpegLink_codecGetProcessedData(MjpegLink_Obj * pObj)
{
    FVID2_FrameList inFrameList;

    UInt32 chId, i;

    Bitstream_BufList outBitBufList;

    Bitstream_BufList outFreeBitBufList;

    System_LinkInQueParams *pInQueParams;

    MjpegLink_ChObj *pChObj;

    Int32 status;

    MjpegLink_ReqObj *pReqObj;

    Bool sendNotify = FALSE;

    UInt32 curTime;

    outBitBufList.numBufs = 0;
    outBitBufList.appData = NULL;
    outFreeBitBufList.numBufs = 0;
    outFreeBitBufList.appData = NULL;
    inFrameList.numFrames = 0;
    curTime = Utils_getCurTimeInMsec();

    {

        while (!Utils_queIsEmpty(&pObj->processDoneQue)
               &&
               (inFrameList.numFrames < (FVID2_MAX_FVID_FRAME_PTR - 1))
               &&
               (outFreeBitBufList.numBufs <
                (VIDBITSTREAM_MAX_BITSTREAM_BUFS - 1)) &&
               (outBitBufList.numBufs < (VIDBITSTREAM_MAX_BITSTREAM_BUFS - 1)))
        {
            status = Utils_queGet(&pObj->processDoneQue, (Ptr *) & pReqObj, 1,
                                  BIOS_NO_WAIT);
            if (status != FVID2_SOK)
            {
                break;
            }
            pObj->isSomethingSubmitted--;

            chId = pReqObj->OutBuf->channelNum;
            pChObj = &pObj->chObj[chId];
            if (pChObj->totalInFrameCnt > MJPEG_LINK_STATS_START_THRESHOLD)
            {
                pChObj->totalProcessTime +=
                    (curTime -
                     (UInt32) pReqObj->InFrameList.frames[0]->reserved);
            }
            pChObj->getProcessedFrameCount++;
            for (i = 0; i < pReqObj->InFrameList.numFrames; i++)
            {
                UTILS_assert(chId ==
                             pReqObj->InFrameList.frames[i]->channelNum);

                inFrameList.frames[inFrameList.numFrames] =
                    pReqObj->InFrameList.frames[i];
                inFrameList.numFrames++;
            }

            if (pReqObj->OutBuf->fillLength > 0)
            {
                outBitBufList.bufs[outBitBufList.numBufs] = pReqObj->OutBuf;
                outBitBufList.numBufs++;
                pChObj->outFrameCount++;
            }
            else
            {
                outFreeBitBufList.bufs[outFreeBitBufList.numBufs] =
                    pReqObj->OutBuf;
                outFreeBitBufList.numBufs++;
                pChObj->inFrameRejectCount++;
            }
            status = Utils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
            UTILS_assert(status == FVID2_SOK);
            pObj->reqQueCount--;

        }

    }
    if (outBitBufList.numBufs)
    {
        status = Utils_bitbufPutFull(&pObj->outObj.bufOutQue, &outBitBufList);
        UTILS_assert(status == FVID2_SOK);
        sendNotify = TRUE;
    }

    if (outFreeBitBufList.numBufs)
    {
        status = Utils_bitbufPutEmpty(&pObj->outObj.bufOutQue,
                                      &outFreeBitBufList);
        UTILS_assert(status == FVID2_SOK);
    }

    if (inFrameList.numFrames)
    {
        /* Free input frames */
        pInQueParams = &pObj->createArgs.inQueParams;
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &inFrameList);
        pObj->inFramePutCount += inFrameList.numFrames;
    }

    if (sendNotify)
    {
        /* Send-out the output bitbuffer */
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_codecQueueFramesToChQue
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
static Int32 MjpegLink_codecQueueFramesToChQue(MjpegLink_Obj * pObj)
{
    UInt32 frameId, freeFrameNum;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    MjpegLink_ChObj *pChObj;

    Int32 status;

    UInt32 curTime;

    Bool skipFrame;

    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    cntOutFrames += frameList.numFrames;

    if (frameList.numFrames)
    {
        cntInFrames += frameList.numFrames;
        pObj->inFrameGetCount += frameList.numFrames;

        freeFrameNum = 0;
        curTime = Clock_getTicks();

        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            pFrame->fid = FVID2_FID_FRAME;

            pChObj = &pObj->chObj[pFrame->channelNum];

            pChObj->inFrameRecvCount++;

            if (pFrame->channelNum)
                Vps_rprintf("MJPEG Mjpeg Channel Number is coming Non-Zero\n");

            skipFrame = FALSE;

            pChObj->curFrameNum++;

            if ((pChObj->curFrameNum % pChObj->frameSkipModulo))
            {
                skipFrame = TRUE;
            }

            if (((pChObj->nextFid == pFrame->fid) ||
                 (pFrame->fid == FVID2_FID_FRAME)) &&
                (pChObj->disableChn != TRUE) && (skipFrame == FALSE))
            {
                /* frame is of the expected FID use it, else drop it */
                pChObj->totalInFrameCnt++;
                if (pChObj->totalInFrameCnt > MJPEG_LINK_STATS_START_THRESHOLD)
                {
                    pChObj->totalFrameIntervalTime +=
                        (curTime - pChObj->prevFrmRecvTime);
                    pFrame->timeStamp = curTime;
                }
                else
                {
                    pChObj->totalFrameIntervalTime = 0;
                    pChObj->totalProcessTime = 0;
                }
                pChObj->prevFrmRecvTime = curTime;

                status = Utils_quePut(&pChObj->inQue, pFrame, BIOS_NO_WAIT);
                UTILS_assert(status == FVID2_SOK);

                pChObj->inFrameQueCount++;
                pChObj->nextFid ^= 1;                      /* toggle to next
                                                            * required FID */
            }
            else
            {
                pChObj->inFrameUserSkipCount++;
                pChObj->inFrameSkipCount++;

                /* frame is not of expected FID, so release frame */
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }

            pObj->inFramePutCount += freeFrameNum;
        }

        if (freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;
            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId, &frameList);
        }
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_codecSubmitData
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
static Int32 MjpegLink_codecSubmitData(MjpegLink_Obj * pObj)
{
    MjpegLink_ReqObj *pReqObj;

    MjpegLink_ChObj *pChObj;

    UInt32 chId, numProcessCh;

    FVID2_Frame *pInFrame;

    Bitstream_Buf *pOutBuf;

    Int32 status = FVID2_SOK , numReqObjPerProcess;

    UInt32 freeFrameNum, tskId, i;

    FVID2_FrameList frameList;

    System_LinkInQueParams *pInQueParams;

    freeFrameNum = 0;
    numProcessCh = 0;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        numReqObjPerProcess = 0;
        pChObj = &pObj->chObj[chId];
        while (numReqObjPerProcess < pChObj->numReqObjPerProcess)
        {
            numReqObjPerProcess++;
            status =
                Utils_queGet(&pObj->reqQue, (Ptr *) & pReqObj, 1, BIOS_NO_WAIT);
            if (UTILS_ISERROR(status))
            {
                break;
            }
            pObj->reqQueCount++;
            UTILS_assert(MJPEG_LINK_MAX_REQ >= pObj->reqQueCount);

            tskId = pObj->ch2ProcessTskId[chId];
            pReqObj->InFrameList.numFrames = 1;
            if ((status == FVID2_SOK) &&
                (pChObj->inFrameQueCount >= pReqObj->InFrameList.numFrames) &&
                (!Utils_queIsFull(&pObj->simcopProcessTsk[tskId].processQue)))
            {
                pOutBuf = NULL;
                UTILS_assert(chId < ENC_LINK_MAX_CH);
                status = Utils_bitbufGetEmptyBuf(&pObj->outObj.bufOutQue,
                                                 &pOutBuf,
                                                 pObj->outObj.ch2poolMap[chId],
                                                 BIOS_NO_WAIT);

                if ((status == FVID2_SOK) && (pOutBuf))
                {

                    for (i = 0; i < pReqObj->InFrameList.numFrames; i++)
                    {
                        status =
                            Utils_queGet(&pChObj->inQue, (Ptr *) & pInFrame, 1,
                                         BIOS_NO_WAIT);
                        UTILS_assert(status == FVID2_SOK);
                        pReqObj->InFrameList.frames[i] = pInFrame;
                        pChObj->inFrameQueCount--;
                    }

                    pOutBuf->channelNum = pInFrame->channelNum;
                    pReqObj->OutBuf = pOutBuf;
                    numProcessCh++;

                    status =
                        Utils_quePut(&pObj->simcopProcessTsk[tskId].processQue,
                                     pReqObj, BIOS_NO_WAIT);
                    UTILS_assert(status == FVID2_SOK);

                    pChObj->processReqestCount++;
                    pObj->isSomethingSubmitted++;

#if JPEG_ENC_DONE_IN_SEPERATE_TASK
                    {
                    }
#else
                    {
                        if (!Utils_queIsEmpty
                            (&pObj->simcopProcessTsk[tskId].processQue))
                        {
                            Utils_queGet(&pObj->
                                         simcopProcessTsk[tskId].processQue,
                                         (Ptr *) & pReqObj, 1,
                                         BIOS_WAIT_FOREVER);

                            MjpegLink_codecProcessTskFunctionality(pObj, tskId,
                                                                   pReqObj);
                        }
                    }
#endif
                }
                else
                {
                    /* Free the input frame if output buffer is not available
                     */
                    for (i = 0; i < pReqObj->InFrameList.numFrames; i++)
                    {
                        status =
                            Utils_queGet(&pChObj->inQue, (Ptr *) & pInFrame, 1,
                                         BIOS_NO_WAIT);
                        UTILS_assert(status == FVID2_SOK);
                        UTILS_assert(freeFrameNum <
                                     UTILS_ARRAYSIZE(frameList.frames));
                        frameList.frames[freeFrameNum] = pInFrame;
                        pChObj->inFrameQueCount--;
                        pChObj->inFrameSkipCount++;
                        pChObj->inFrameRejectCount++;
                        freeFrameNum++;
                    }

                    status = Utils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
                    UTILS_assert(status == FVID2_SOK);
                    pObj->reqQueCount--;
                    status = FVID2_EFAIL;
                    continue;
                }
            }
            else
            {
                /* Free the input frame if processQue is full */
                if (pChObj->inFrameQueCount >= pReqObj->InFrameList.numFrames)
                {
                    for (i = 0; i < pReqObj->InFrameList.numFrames; i++)
                    {
                        status =
                            Utils_queGet(&pChObj->inQue, (Ptr *) & pInFrame, 1,
                                         BIOS_NO_WAIT);
                        UTILS_assert(status == FVID2_SOK);
                        UTILS_assert(freeFrameNum <
                                     UTILS_ARRAYSIZE(frameList.frames));
                        frameList.frames[freeFrameNum] = pInFrame;
                        pChObj->inFrameQueCount--;
                        pChObj->inFrameSkipCount++;
                        pChObj->inFrameRejectCount++;
                        freeFrameNum++;
                    }
                }

                status = Utils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
                UTILS_assert(status == FVID2_SOK);
                pObj->reqQueCount--;
                status = FVID2_EFAIL;
            }

        }                                                  /* while */
    }                                                      /* for ch */

    if (freeFrameNum)
    {
        /* Free input frames */
        pInQueParams = &pObj->createArgs.inQueParams;
        frameList.numFrames = freeFrameNum;
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
        pObj->inFramePutCount += freeFrameNum;
    }

    return status;
}

/* ===================================================================
 *  @func     MjpegLink_codecProcessData
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
Int32 MjpegLink_codecProcessData(MjpegLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

    MjpegLink_codecQueueFramesToChQue(pObj);
    do
    {
        status = MjpegLink_codecSubmitData(pObj);
    }
    while (status == FVID2_SOK);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_codecGetProcessedDataMsgHandler
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
Int32 MjpegLink_codecGetProcessedDataMsgHandler(MjpegLink_Obj * pObj)
{
    Int32 status;

    status = MjpegLink_codecGetProcessedData(pObj);
    UTILS_assert(status == FVID2_SOK);

    return MJPEG_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     MjpegLink_codecFreeInQueuedBufs
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
static Int32 MjpegLink_codecFreeInQueuedBufs(MjpegLink_Obj * pObj)
{
    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    pInQueParams = &pObj->createArgs.inQueParams;
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);
    if (frameList.numFrames)
    {
        pObj->inFrameGetCount += frameList.numFrames;
        /* Free input frames */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
        pObj->inFramePutCount += frameList.numFrames;
    }
    return MJPEG_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     MjpegLink_codecStop
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
Int32 MjpegLink_codecStop(MjpegLink_Obj * pObj)
{
    Int32 rtnValue = FVID2_SOK;

    MJPEGLINK_INFO_LOG(pObj->linkId, -1, "Stop in progress !!!\n");

    MjpegLink_codecFreeInQueuedBufs(pObj);

    MJPEGLINK_INFO_LOG(pObj->linkId, -1, "Stop done !!!\n");

    return (rtnValue);
}

/* ===================================================================
 *  @func     MjpegLink_codecSetqpParamP
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
Int32 MjpegLink_codecSetqpParamP(MjpegLink_Obj * pObj,
                                 MjpegLink_ChQpParams * params)
{
    Int32 status = MJPEG_LINK_S_SUCCESS;

    MjpegLink_ChObj *pChObj;

    UInt key;

    key = Hwi_disable();

    pChObj = &pObj->chObj[params->chId];
    pChObj->mjpegQp = params->mjpegQp;

    Hwi_restore(key);

    return (status);
}

Int32 MjpegLink_codecSetFrameRate(MjpegLink_Obj * pObj,
                                 MjpegLink_ChFpsParams * params)
{
    Int32 status = MJPEG_LINK_S_SUCCESS;
    UInt32 inputFrameRate = 0, targetFrameRate = 0;

    MjpegLink_ChObj *pChObj;

    UInt key;

    key = Hwi_disable();

    pChObj = &pObj->chObj[params->chId];

    inputFrameRate 	= params->baseFps/1000;
    targetFrameRate = params->targetFps/1000;

    pChObj->frameSkipModulo = inputFrameRate/targetFrameRate;

    Hwi_restore(key);

    return (status);
}

/* ===================================================================
 *  @func     MjpegLink_codecDisableChannel
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
Int32 MjpegLink_codecDisableChannel(MjpegLink_Obj * pObj,
                                    MjpegLink_ChannelInfo * params)
{
    Int32 status = MJPEG_LINK_S_SUCCESS;

    MjpegLink_ChObj *pChObj;

    UInt key;

    key = Hwi_disable();
    pChObj = &pObj->chObj[params->chId];
    pChObj->disableChn = TRUE;
    Hwi_restore(key);

    return (status);
}

/* ===================================================================
 *  @func     MjpegLink_codecEnableChannel
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
Int32 MjpegLink_codecEnableChannel(MjpegLink_Obj * pObj,
                                   MjpegLink_ChannelInfo * params)
{
    Int32 status = MJPEG_LINK_S_SUCCESS;

    MjpegLink_ChObj *pChObj;

    UInt key;

    key = Hwi_disable();
    pChObj = &pObj->chObj[params->chId];
    pChObj->disableChn = FALSE;
    Hwi_restore(key);

    return (status);
}

/* ===================================================================
 *  @func     MjpegLink_codecDelete
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
Int32 MjpegLink_codecDelete(MjpegLink_Obj * pObj,
                            MSP_COMPONENT_TYPE * jComponent)
{
    UInt32 outId, chId, tskId;

    MjpegLink_ChObj *pChObj;

    MjpegLink_OutObj *pOutObj;

    Bool tilerUsed = FALSE;

    Int i, bitbuf_index;

    MJPEGLINK_INFO_LOG(pObj->linkId, -1, "Delete in progress !!!\n");
    MjpegLink_codecStopPrdObj(pObj);
    MjpegLink_codecDeletePrdObj(pObj);

    for (tskId = 0; tskId < pObj->inQueInfo.numCh; tskId++)
    {
        MjpegLink_codecDeleteProcessTsk(pObj, tskId);
        Utils_queDelete(&pObj->simcopProcessTsk[tskId].processQue);
    }

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        Utils_queDelete(&pChObj->inQue);

        if (pChObj->totalInFrameCnt > MJPEG_LINK_STATS_START_THRESHOLD)
        {
            pChObj->totalInFrameCnt -= MJPEG_LINK_STATS_START_THRESHOLD;
        }

        pChObj->totalInFrameCnt = 0;
        MSP_JPEGE_INC_deInit(pChObj->algObj.alg.jpegEncComponent);
    }

    Utils_queDelete(&pObj->processDoneQue);

    for (outId = 0; outId < MJPEG_LINK_MAX_OUT_QUE; outId++)
    {
        {
            pOutObj = &pObj->outObj;

            Utils_bitbufDelete(&pOutObj->bufOutQue);
            bitbuf_index = 0;
            for (i = 0; i < pOutObj->numAllocPools; i++)
            {
                UTILS_assert((pOutObj->outBufs[bitbuf_index].bufSize ==
                              pOutObj->buf_size[i]));
                Utils_memBitBufFree(&pOutObj->outBufs[bitbuf_index],
                                    pOutObj->outNumBufs[i]);
                bitbuf_index += pOutObj->outNumBufs[i];
            }
        }
    }

    Utils_queDelete(&pObj->reqQue);

    if (tilerUsed)
    {
        SystemTiler_freeAll();
    }

    MJPEGLINK_INFO_LOG(pObj->linkId, -1, "Delete done !!!\n");
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_resetStatistics
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
Int32 MjpegLink_resetStatistics(MjpegLink_Obj * pObj)
{
    UInt32 chId;

    MjpegLink_ChObj *pChObj;

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChObj->inFrameRecvCount = 0;
        pChObj->inFrameRejectCount = 0;
        pChObj->inFrameUserSkipCount = 0;
        pChObj->outFrameCount = 0;
        pChObj->minLatency = 0xFF;
        pChObj->maxLatency = 0;
    }

    pObj->statsStartTime = Utils_getCurTimeInMsec();

    return 0;
}

/* ===================================================================
 *  @func     MjpegLink_printStatistics
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
Int32 MjpegLink_printStatistics(MjpegLink_Obj * pObj, Bool resetAfterPrint)
{
    UInt32 chId;

    MjpegLink_ChObj *pChObj;

    UInt32 elaspedTime;

    elaspedTime = Utils_getCurTimeInMsec() - pObj->statsStartTime;  /* in
                                                                     * msecs */
    elaspedTime /= 1000;                                   /* convert to secs
                                                            */

    Vps_rprintf(" \n"
                " *** MJPEG ENCODE Statistics *** \n\n"
                " Elasped Time           : %d secs\n\n"
                " --------------------------------------------\n"
                " CH  | In Recv In Skip In User  Out Latency  \n"
                " Num | FPS     FPS     Skip FPS FPS Min / Max\n"
                " --------------------------------------------\n", elaspedTime);

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        Vps_printf(" %3d | %7d %7d %8d %3d %3d / %3d\n",
                   chId,
                   pChObj->inFrameRecvCount / elaspedTime,
                   pChObj->inFrameRejectCount / elaspedTime,
                   pChObj->inFrameUserSkipCount / elaspedTime,
                   pChObj->outFrameCount / elaspedTime,
                   pChObj->minLatency, pChObj->maxLatency);
    }

    cntInFrames = 0;
    cntOutFrames = 0;
    cntProcessedFrames = 0;
    cntSkippedFrames = 0;

    if (resetAfterPrint)
    {
        MjpegLink_resetStatistics(pObj);
    }

    return FVID2_SOK;
}
