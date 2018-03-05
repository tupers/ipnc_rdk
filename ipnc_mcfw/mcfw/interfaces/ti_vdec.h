/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup MCFW_API
    \defgroup MCFW_VDEC_API McFW Video Decode (VDEC) API

    @{
*/

/**
    \file ti_vdec.h
    \brief McFW Video Decode (VDEC) API
*/

#ifndef __TI_VDEC_H__
#define __TI_VDEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ti_vsys.h"

/* =============================================================================
 * Defines
 * =============================================================================
 */

/** Maximum Video Decode channels */
#define VDEC_CHN_MAX			(64)

/* =============================================================================
 * Enums
 * =============================================================================
 */
/**
    \brief TrickPlay Rate
*/
typedef enum
{
    VDEC_TPLAY_1X = 1,
    /**< Normal Speed Playback */

    VDEC_TPLAY_2X = 2,
    /**< 2X Speed Playback */

    VDEC_TPLAY_4X = 4,
    /**< 4X Speed Playback */

    VDEC_TPLAY_8X = 8,
    /**< 8X Speed Playback */

    VDEC_TPLAY_16X = 16,
    /**< 16X Speed Playback */

    VDEC_TPLAY_32X = 32,
    /**< 32X Speed Playback */

    VDEC_TPLAY_HALFX = 100*(1/2),
    /**< 1/2X Speed Playback */

    VDEC_TPLAY_QUARTERX = 100*(1/4),
    /**< 1/4X Speed Playback */

    VDEC_TPLAY_IFRAMEONLY = 0,
    /**< 1/4X Speed Playback */


    VDEC_TPLAY_MAX   = 100
    /**< Maximum Playback Rate */

}VDEC_TPLAY;

/**
    \brief Codec Identifier for Decoder
*/
typedef enum
{
    VDEC_CHN_H264 = 66,                     
    /**< BaseLine Profile   */

    VDEC_CHN_MJPEG = 77,                     
    /**< Main Profile       */

    VDEC_CHN_MPEG4 = 88                     
    /**< Advanced Simple Profile */

} VDEC_CHN_CODEC_E;



/* =============================================================================
 * Structures
 * =============================================================================
 */

/**
    \brief Decode Channel Specific Dynamic Parameters
*/
typedef struct
{
    Int32 frameRate;
    /**< frame rate */

    Int32 targetBitRate;
    /**< target bit-rate in bits/second */

} VDEC_CHN_DYNAMIC_PARAM_S;

/**
    \brief Decode Channel Specific Static Parameters
*/
typedef struct
{
    Int32 maxVideoWidth;
    /**< Decoded frame width */

    Int32 maxVideoHeight;
    /**< Decoded frame height */

    Int32 numBufPerCh;
    /**< Number of output buffers per channel required based on input */

    Int32 displayDelay;
    /**< Number of output buffers per channel required based on input */

    VDEC_CHN_CODEC_E isCodec;
    /**< Codec type */

    VDEC_CHN_DYNAMIC_PARAM_S dynamicParam;
    /**< Initial values for dynamic parameters */

} VDEC_CHN_PARAMS_S;

/**
    \brief Decode empty buffer request structure
*/
typedef struct {

    UInt32 numBufs;
    /**< Number of empty buffers needed from decode framework */

    UInt32 minBufSize[VCODEC_BITSBUF_MAX];
    /**< Minnimum size of buffer's needed */

} VDEC_BUF_REQUEST_S;

/**
    \brief Decode Sub-system initialization parameters
*/
typedef struct {

    UInt32 numChn;
    /**< Number of decoder channels - USED only if VDEC_PARAMS_S.forceUseDecChannelParams = TRUE */

    Bool   forceUseDecChannelParams;
    /**< TRUE: Decoder width and height decided by VDEC_PARAMS_S.decChannelParams[] \n
         FALSE: Decoder width and height decided based on encoder width and height (Default)
    */

    VDEC_CHN_PARAMS_S decChannelParams[VDEC_CHN_MAX];
    /**< Channel Specific Parameters */

} VDEC_PARAMS_S;


/* =============================================================================
 * APIs
 * =============================================================================
 */

/**
    \brief Set defaults for VDEC_PARAMS_S

    By default decode parameters are setup for 16CH D1 decode mode

    \param  pContext    [OUT] VDEC_PARAMS_S filled with default parameters
*/
Void  Vdec_params_init(VDEC_PARAMS_S * pContext);

/**
    \brief Initialize decode sub-system

    MUST be called before calling any other VDEC API

    \param  pContext    [IN] Decode configuration

    \return ERROR_NONE on success
*/
Int32 Vdec_init(VDEC_PARAMS_S * pContext);

/**
    \brief De-Initialize decode sub-system

    MUST be called after Vdec_stop() and Vsys_delete()
*/
Void  Vdec_exit();

/**
    \brief Start decode sub-system

    MUST be called after calling Vdec_init() and Vsys_create()

    \return ERROR_NONE on success
*/
Int32 Vdec_start();

/**
    \brief Stop decode sub-system

    MUST be called before calling Vsys_delete()

    \return ERROR_NONE on success
*/
Int32 Vdec_stop();
/**
    \brief Enable a channel in the decode sub-system

    By default all channels and streams are enabled after Vdec_start()

    \param vcChnId  [IN] decode channel ID

    \return ERROR_NONE on success
*/
Int32 Vdec_enableChn(VDEC_CHN vdecChnId);

/**
    \brief Disable a channel in the decode sub-system

    Disabling a channel will make the channel unavailable for display.
    If display is enabled on that channel then display will hold on the last frame
    that was shown for that window on the display.

    \param vcChnId  [IN] decode channel ID

    \return ERROR_NONE on success
*/
Int32 Vdec_disableChn(VDEC_CHN vdecChnId);

/**
    \brief Set playback control configuration to trickplay logic

    decoder output frame rate is controlled based on setting
    trickplay skipcount field.
    ex: for 2x speed, skip count should be 2 & decoder output
        frame rate will be skipped half of original frame rate.

    \param vcChnId  [IN] decode channel ID

*/
void Vdec_setTplayConfig(VDEC_CHN vdecChnId, VDEC_TPLAY speed);


/**
    \brief Request empty buffers from framework

    <b> Coming soon .. NOT SUPPORTED in this release </b>

    User calls this API to get empty buffer from the framework.
    After getting the buffers, user will
    - fill the buffers with bitstreams
    - update the info in VCODEC_BITSBUF_LIST_S
    - and then call Vdec_putBitstreamBuffer() to decode the frames and send to display

    \param bufReq           [IN]   Number of buffers and size of each buffers needed by user
    \param pBitsBufList     [OUT]  List of buffers returned by the framework
    \param timeout          [IN]   TIMEOUT_WAIT_FOREVER or TIMEOUT_NO_WAIT or timeout in msecs

    \return ERROR_NONE on success
*/
Int32 Vdec_requestBitstreamBuffer(VDEC_BUF_REQUEST_S *bufReq, VCODEC_BITSBUF_LIST_S *pBitsBufList, UInt32 timeout);

/**
    \brief Give buffers for decode to framework

    Buffers that are needed to be decoded are given to the McFW.
    Buffers should have been given to user via the Vdec_requestBitstreamBuffer() API.

    \param pBitsBufList [IN]   List of Bistream Buffers

    \return ERROR_NONE on success
*/
Int32 Vdec_putBitstreamBuffer(VCODEC_BITSBUF_LIST_S *pBitsBufList);

/**
    \brief Get info on number of channels and each channels property from
           decode framework

    \param channelListInfo [OUT]   Structure pointer that will be populated
                                   with input channel info by this function

    \return ERROR_NONE on success
*/
Int32 Vdec_getChannelInfo(VIDEO_CHANNEL_LIST_INFO_S *channelListInfo);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


/* @} */

