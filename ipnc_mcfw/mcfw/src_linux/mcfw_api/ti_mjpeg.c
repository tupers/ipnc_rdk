/*==============================================================================
 * @file:       ti_mjpeg.c
 *
 * @brief:      Video capture mcfw function definition.
 *
 * @vers:       0.5.0.0 2011-06
 *
 *==============================================================================
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <osa.h>
#include <osa_debug.h>
#include "ti_mjpeg_priv.h"
#include "ti_vsys_priv.h"
#include "ti_vsys.h"
#include "ti_mjpeg.h"

#include <mcfw/interfaces/link_api/mjpegLink.h>
#include <mcfw/interfaces/link_api/ipcLink.h>

#define MJPEG_TRACE_ENABLE_FXN_ENTRY_EXIT           (0)
#define MJPEG_TRACE_INFO_PRINT_INTERVAL             (8192)

#if MJPEG_TRACE_ENABLE_FXN_ENTRY_EXIT
#define MJPEG_TRACE_FXN(str,...)                    do {                           \
                                                     static Int printInterval = 0;\
                                                     if ((printInterval % MJPEG_TRACE_INFO_PRINT_INTERVAL) == 0) \
                                                     {                                                          \
                                                         OSA_printf("TI_MJPEG:%s function:%s",str,__func__);     \
                                                         OSA_printf(__VA_ARGS__);                               \
                                                     }                                                          \
                                                     printInterval++;                                           \
                                                   } while (0)
#define MJPEG_TRACE_FXN_ENTRY(...)                  MJPEG_TRACE_FXN("Entered",__VA_ARGS__)
#define MJPEG_TRACE_FXN_EXIT(...)                   MJPEG_TRACE_FXN("Leaving",__VA_ARGS__)
#else
#define MJPEG_TRACE_FXN_ENTRY(...)
#define MJPEG_TRACE_FXN_EXIT(...)
#endif

/* =============================================================================
 * Internal APIs declaration
 * ============================================================================= */
/* =============================================================================
 * Globals
 * ============================================================================= */

MJPEG_MODULE_CONTEXT_S gMjpegModuleContext;

/* =============================================================================
 * Externs
 * ============================================================================= */

/* =============================================================================
 * Simcop module APIs
 * ============================================================================= */
/**
 * \brief:
 *      Initialize parameters to be passed to init
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_params_init
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
Void Mjpeg_params_init(MJPEG_PARAMS_S * pContext)
{

    UInt16 i;

    for (i = 0; i < MJPEG_PRIMARY_CHANNELS; i++)
    {
        pContext->mjpegProfile[i] = MJPEG_CHN_HIGH_PROFILE;

    }

    for (i = MJPEG_PRIMARY_CHANNELS; i < MJPEG_CHN_MAX; i++)
    {
        pContext->mjpegProfile[i] = MJPEG_CHN_HIGH_PROFILE;

    }
    return;
}

/**
 * \brief:
 *      Initialize Simcop instance
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_init
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
Int32 Mjpeg_init(MJPEG_PARAMS_S * pContext)
{
    gMjpegModuleContext.mjpepEncId = SYSTEM_LINK_ID_INVALID;
    gMjpegModuleContext.ipcBitsOutRTOSId = SYSTEM_LINK_ID_INVALID;
    gMjpegModuleContext.ipcBitsInHLOSId = SYSTEM_LINK_ID_INVALID;

    gMjpegModuleContext.callbackFxn.newDataAvailableCb = NULL;
    gMjpegModuleContext.callbackArg = NULL;

    if (pContext == NULL)
    {
        Mjpeg_params_init(&gMjpegModuleContext.mjpegEncConfig);
    }
    else
    {
        memcpy(&gMjpegModuleContext.mjpegEncConfig, pContext,
               sizeof(MJPEG_PARAMS_S));
    }
    return 0;
}

/**
 * \brief:
 *      Finalize Simcop instance
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_exit
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
Void Mjpeg_exit()
{
}

/**
 * \brief:
 *      Start Simcop instance for capturing and proccessing
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_start
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
Int32 Mjpeg_start()
{
    System_linkStart(gMjpegModuleContext.mjpepEncId);
    return 0;
}

/**
 * \brief:
 *      Stop Simcop instance
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_stop
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
Int32 Mjpeg_stop()
{
    System_linkStop(gMjpegModuleContext.mjpepEncId);
    return 0;
}

/* =============================================================================
 * Simcop device related APIs
 * ============================================================================= */

/* =============================================================================
 * Simcop channel related APIs
 * ============================================================================= */

/**
 * \brief:
 *		Enable the specific encoder channel
 * \input:
 *		mjpegChnId 		-- encoder channel id
 * \output:
 *		NA
 * \return
*		TI_MEDIA_SUCCESS 	-- 	while success
*		ERROR_CODE 			-- 	refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_enableChn
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
Int32 Mjpeg_enableChn(MJPEG_CHN mjpegChnId)
{
    MjpegLink_ChannelInfo params = { 0 };

    OSA_printf("\r\nEnable Channel: %d", mjpegChnId);

    params.chId = mjpegChnId;

    System_linkControl(gMjpegModuleContext.mjpepEncId,
                       MJPEG_LINK_CMD_ENABLE_CHANNEL, &params, sizeof(params),
                       TRUE);

    return 0;

}

/**
 * \brief:
 *		Disable the specific encoder channel
 * \input:
 *		mjpegChnId 		-- encoder channel id
 * \output:
 *		NA
 * \return
*		TI_MEDIA_SUCCESS 	-- 	while success
*		ERROR_CODE 			-- 	refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_disableChn
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
Int32 Mjpeg_disableChn(MJPEG_CHN mjpegChnId)
{
    MjpegLink_ChannelInfo params = { 0 };

    OSA_printf("\r\nDisable Channel: %d", mjpegChnId);

    params.chId = mjpegChnId;

    System_linkControl(gMjpegModuleContext.mjpepEncId,
                       MJPEG_LINK_CMD_DISABLE_CHANNEL, &params, sizeof(params),
                       TRUE);

    return 0;
}

/* =============================================================================
 * Helper APIs
 * ============================================================================= */

/* =============================================================================
 * Internal APIs
 * ============================================================================= */

/* =============================================================================
 * Stream Related APIs
 * ============================================================================= */
 /**
 * \brief:
 *		Get the stream dynamic parameter
 * \input:
 *		mjpegChnId 		-- encoder channel id
 *		mjpegStrmID		-- the stream id
 *		ptMjpegDynamicParam -- encoder dynamic parameter to get
 * \output:
 *		NA
 * \return
*		TI_MEDIA_SUCCESS 	-- 	while success
*		ERROR_CODE 			-- 	refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_getDynamicParam
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
Int32 Mjpeg_getDynamicParam(MJPEG_CHN mjpegChnId, MJPEG_STRM mjpegStrmID,
                            MJPEG_CHN_DYNAMIC_PARAM_S * ptMjpegDynamicParam,
                            MJPEG_PARAM_E veParamId)
{
    MjpegLink_GetDynParams params = { 0 };

    OSA_printf("\r\nGet Dynamic params of Channel: %d", mjpegChnId);
    params.chId = mjpegChnId;
    System_linkControl(gMjpegModuleContext.mjpepEncId,
                       MJPEG_LINK_CMD_GET_CODEC_PARAMS, &params, sizeof(params),
                       TRUE);
    switch (veParamId)
    {
        case MJPEG_ALL:
            ptMjpegDynamicParam->inputFrameRate = params.baseFps;
			ptMjpegDynamicParam->targetFrameRate = params.targetFps;
            ptMjpegDynamicParam->mjpegQp = params.mjpegQp;
            break;

        case MJPEG_MJPEG_FRAMERATE:
            ptMjpegDynamicParam->inputFrameRate = params.baseFps;
			ptMjpegDynamicParam->targetFrameRate = params.targetFps;

            break;

        case MJPEG_MJPEG_QP:
            ptMjpegDynamicParam->mjpegQp = params.mjpegQp;
            break;
        default:
            break;

    }

    return 0;
}

 /**
 * \brief:
 *		Set the stream dynamic parameters
 * \input:
 *		mjpegChnId 		-- encoder channel id
 *		mjpegStrmID		-- the stream id
 *		ptMjpegDynamicParam -- encoder dynamic parameter to be set
 * \output:
 *		NA
 * \return
*		TI_MEDIA_SUCCESS 	-- 	while success
*		ERROR_CODE 			-- 	refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_setDynamicParam
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
Int32 Mjpeg_setDynamicParam(MJPEG_CHN mjpegChnId, MJPEG_STRM mjpegStrmID,
                            MJPEG_CHN_DYNAMIC_PARAM_S * ptMjpegDynamicParam,
                            MJPEG_PARAM_E veParamId)
{
    switch (veParamId)
    {
        case MJPEG_ALL:
        {
            OSA_printf("\r\n Not Supported");
            break;
        }

        case MJPEG_MJPEG_FRAMERATE:
        {
            MjpegLink_ChFpsParams params;

            params.chId = mjpegChnId;
            OSA_printf("\r\n Channel Selected: %d", params.chId);

            params.baseFps 	 = ptMjpegDynamicParam->inputFrameRate;
			params.targetFps = ptMjpegDynamicParam->targetFrameRate;

            System_linkControl(gMjpegModuleContext.mjpepEncId,
                               MJPEG_LINK_CMD_SET_CODEC_FPS, &params,
                               sizeof(params), TRUE);
            break;
        }

        case MJPEG_MJPEG_QP:
        {
            MjpegLink_ChQpParams params;

            params.chId = mjpegChnId;

            params.mjpegQp = ptMjpegDynamicParam->mjpegQp;
            System_linkControl(gMjpegModuleContext.mjpepEncId,
                               MJPEG_LINK_CMD_SET_CODEC_QP, &params,
                               sizeof(params), TRUE);
            break;
        }

        default:
            break;

    }
    return 0;
}

/**
 * \brief:
 *		register call back which will post the message
 * \input:
 *		callback 		-- callback function
 * \output:
 *		NA
 * \return
*		TI_MEDIA_SUCCESS 	-- 	while success
*		ERROR_CODE 			-- 	refer for err defination
*/
/* ===================================================================
 *  @func     Mjpeg_registerCallback
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
Int32 Mjpeg_registerCallback(MJPEG_CALLBACK_S * callback, Ptr arg)
{
    gMjpegModuleContext.callbackFxn = *callback;
    gMjpegModuleContext.callbackArg = arg;

    return 0;
}

/**
    \brief Returns Callback info registered by the application

    \param callback            [OUT] Pointer to User specified callbacks

    \param arg                 [OUT] Pointer to Callback context

    \return ERROR_NONE on success
*/
/* ===================================================================
 *  @func     Mjpeg_getCallbackInfo
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
Int32 Mjpeg_getCallbackInfo(MJPEG_CALLBACK_S ** callback, Ptr * arg)
{
    *callback = &gMjpegModuleContext.callbackFxn;
    *arg = gMjpegModuleContext.callbackArg;

    return 0;
}

/* ===================================================================
 *  @func     Mjpeg_copyBitBufInfoLink2McFw
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
static Void Mjpeg_copyBitBufInfoLink2McFw(VCODEC_BITSBUF_S * dstBuf,
                                          Bitstream_Buf * srcBuf)
{
    dstBuf->reserved = (UInt32) srcBuf;
    dstBuf->bufVirtAddr = srcBuf->addr;
    dstBuf->bufSize = srcBuf->bufSize;
    dstBuf->chnId = srcBuf->channelNum;
    dstBuf->codecType = srcBuf->codingType;
    dstBuf->mvDataFilledSize = srcBuf->mvDataFilledSize;
    dstBuf->mvDataOffset = srcBuf->mvDataOffset;	
    dstBuf->startOffset = srcBuf->startOffset;
    dstBuf->filledBufSize = srcBuf->fillLength;
    dstBuf->timestamp = srcBuf->timeStamp;
    if (srcBuf->isKeyFrame)
        dstBuf->frameType = VCODEC_FRAME_TYPE_I_FRAME;
    else
        dstBuf->frameType = VCODEC_FRAME_TYPE_P_FRAME;
    dstBuf->bufPhysAddr = (Void *) srcBuf->phyAddr;
    dstBuf->frameWidth = srcBuf->frameWidth;
    dstBuf->frameHeight = srcBuf->frameHeight;
    /* TODO the following members are to be added to bitStream bf structure */
    dstBuf->fieldId = 0;
    dstBuf->strmId = 0;
    MJPEG_TRACE_FXN_EXIT("BitBufInfo:"
                         "virt:%p,"
                         "bufSize:%d,"
                         "chnId:%d,"
                         "codecType:%d,"
                         "filledBufSize:%d,"
                         "mvDataFilledSize:%d,"
                         "timeStamp:%d,"
                         "isKeyFrame:%d,"
                         "phy:%p,"
                         "width:%d"
                         "height:%d",
                         dstBuf->bufVirtAddr,
                         dstBuf->bufSize,
                         dstBuf->chnId,
                         dstBuf->codecType,
                         dstBuf->filledBufSize,
                         dstBuf->mvDataFilledSize,
                         dstBuf->timestamp,
                         srcBuf->isKeyFrame,
                         dstBuf->bufPhysAddr,
                         dstBuf->frameWidth, dstBuf->frameHeight);
}

/**
    \brief Get encoded buffers from McFW

    \param pBitsBufList [OUT]   List of Bistream Buffers returned by the function
    \param timeout      [IN]    VSYS_WAIT_FOREVER or VSYS_NO_WAIT or timeout in units of msec

    \return SUCCESS or FAIL
 */
/* ===================================================================
 *  @func     Mjpeg_getBitstreamBuffer
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
Int32 Mjpeg_getBitstreamBuffer(VCODEC_BITSBUF_LIST_S * pBitsBufList,
                               UInt32 timeout)
{
    Bitstream_BufList ipcBufList;

    Bitstream_Buf *pInBuf;

    VCODEC_BITSBUF_S *pOutBuf;

    UInt32 i;

    MJPEG_TRACE_FXN_ENTRY();
    pBitsBufList->numBufs = 0;
    ipcBufList.numBufs = 0;
    IpcBitsInLink_getFullVideoBitStreamBufs(gMjpegModuleContext.ipcBitsInHLOSId,
                                            &ipcBufList);

    pBitsBufList->numBufs = ipcBufList.numBufs;
    for (i = 0; i < ipcBufList.numBufs; i++)
    {
        pOutBuf = &pBitsBufList->bitsBuf[i];
        pInBuf = ipcBufList.bufs[i];

        Mjpeg_copyBitBufInfoLink2McFw(pOutBuf, pInBuf);
    }

    MJPEG_TRACE_FXN_EXIT("NumBufs Received:%d", pBitsBufList->numBufs);
    return 0;
}

/**
    \brief Release encoded buffers to McFW

    Buffers returned by Mjpeg_getBitstreamBuffer() are returned to the framework
    for resue after user is done using the encoded bitstreams

    \param pBitsBufList [IN]   List of Bistream Buffers

    \return SUCCESS or FAIL
 */
/* ===================================================================
 *  @func     Mjpeg_releaseBitstreamBuffer
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
Int32 Mjpeg_releaseBitstreamBuffer(VCODEC_BITSBUF_LIST_S * pBitsBufList)
{
    VCODEC_BITSBUF_S *pOutBuf;

    Bitstream_BufList ipcBufList;

    UInt32 i;

    Int status = 0;

    MJPEG_TRACE_FXN_ENTRY("Num bufs released:%d", pBitsBufList->numBufs);
    ipcBufList.numBufs = pBitsBufList->numBufs;
    for (i = 0; i < ipcBufList.numBufs; i++)
    {
        pOutBuf = &pBitsBufList->bitsBuf[i];
        ipcBufList.bufs[i] =
            (Bitstream_Buf *) pBitsBufList->bitsBuf[i].reserved;
    }
    if (ipcBufList.numBufs)
    {
        status =
            IpcBitsInLink_putEmptyVideoBitStreamBufs(gMjpegModuleContext.
                                                     ipcBitsInHLOSId,
                                                     &ipcBufList);
    }
    MJPEG_TRACE_FXN_ENTRY("Buf release status:%d", status);
    return 0;
}

/* ===================================================================
 *  @func     Mjpeg_delete
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
Int32 Mjpeg_delete()
{
    if (gMjpegModuleContext.mjpepEncId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gMjpegModuleContext.mjpepEncId);
    }
    if (gMjpegModuleContext.ipcBitsOutRTOSId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gMjpegModuleContext.ipcBitsOutRTOSId);
    }
    if (gMjpegModuleContext.ipcBitsInHLOSId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gMjpegModuleContext.ipcBitsInHLOSId);
    }

    return 0;
}
