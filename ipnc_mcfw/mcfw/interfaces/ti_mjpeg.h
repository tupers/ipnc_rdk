/** ==================================================================
 *  @file   ti_mjpeg.h
 *
 *  @path    ipnc_mcfw/mcfw/interfaces/
 *
 *  @desc   This  File contains McFW MJPEG Encode (SIMCOP) API.
 *
 * ===================================================================
 *
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 *
 * ===================================================================*/

/**
    \ingroup MCFW_API
    \defgroup MCFW_MJPEG_API McFW Video Encode (SIMCOP) API

    @{
*/

/**
    \file ti_mjpeg.h
    \brief McFW MJPEG Encode (SIMCOP) API
*/

#ifndef __TI_MJPEG_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_MJPEG_H__

#include "ti_vsys.h"

/**==========
 * Defines
 * ============================================================================= */

/** Maximum Video Encode channels */
#define MJPEG_PRIMARY_CHANNELS   	(2)

#define MJPEG_CHN_MAX				(1*MJPEG_PRIMARY_CHANNELS)

/** Maximum Video Encode streams */
#define MJPEG_STRM_MAX           	(2)

/**==========
 * Enums
 * ============================================================================= */

/**
    \brief Encode Param Set/Get ID
*/
typedef enum {
    MJPEG_MJPEG_FRAMERATE,
    /**< Get/Set frame-rate */

    MJPEG_MJPEG_QP,

    MJPEG_ALL
    /**< Get/Set All parameters */
} MJPEG_PARAM_E;

/**
    \brief Profile Identifier for H.264 Encoder
*/
typedef enum {
    MJPEG_CHN_BASELINE_PROFILE = 60,
    /**< BaseLine Profile   */

    MJPEG_CHN_MAIN_PROFILE = 70,
    /**< Main Profile       */

    MJPEG_CHN_EXTENDED_PROFILE = 80,
    /**< Extended Profile   */

    MJPEG_CHN_HIGH_PROFILE = 90,
    /**< High Profile       */

    MJPEG_CHN_DEFAULT_PROFILE = MJPEG_CHN_HIGH_PROFILE,
    /**< Default Profile    */

} MJPEG_CHN_MJEPG_Profile_E;

/**==========
 * Structures
 * ============================================================================= */

/**
    \brief Callback for new encoded bitstream buffers available

    \return ERROR_NONE on success
*/
typedef Void(*MJPEG_CALLBACK_NEW_DATA) (Ptr cbCtx);

/**
    \brief Encode Channel Specific Dynamic Parameters
*/
typedef struct {
    Int32 inputFrameRate;
    /**< Frame-rate */

    Int32 targetFrameRate;
    /**< Frame-rate */

    Int32 mjpegQp;
    /**< Quality profile */

} MJPEG_CHN_DYNAMIC_PARAM_S;

/**
    \brief Encode Channel Specific Static Parameters
*/
typedef struct {
    Int32 videoWidth;
    /**< Read only: Encoded Video Width */

    Int32 videoHeight;
    /**< Read only: Encoded Video Height */

    MJPEG_CHN_DYNAMIC_PARAM_S dynamicParam;
    /**< Initial values for dynamic parameters of encode channel */

} MJPEG_CHN_PARAMS_S;

/**
    \brief Encode User Callbacks
*/
typedef struct {

    MJPEG_CALLBACK_NEW_DATA newDataAvailableCb;
    /**< New encoded bitstream available callback */

} MJPEG_CALLBACK_S;

/**
    \brief Encode Sub-system initialization parameters
*/
typedef struct {

    MJPEG_CHN_PARAMS_S simcopChannelParams[MJPEG_CHN_MAX];
    /**< Channel Specific Parameters */

    MJPEG_CHN_MJEPG_Profile_E mjpegProfile[MJPEG_CHN_MAX];
    /**< Codec profile */

} MJPEG_PARAMS_S;

/**==========
 * APIs
 * ============================================================================= */

/**
 *  @func     Mjpeg_params_init
 *
 *  @desc     Function Set defaults for MJPEG_PARAMS_S
 *
 *  @modif    
 *
 *  @inputs   pContext    [IN] MJPEG_PARAMS_S filled with default parameters
 *
 *  @outputs  pContext    [OUT] MJPEG_PARAMS_S filled with default parameters
 *
 *  @return   None
 *
 */
Void Mjpeg_params_init(MJPEG_PARAMS_S * pContext);

/**
 *  @func     Mjpeg_init
 *
 *  @desc     Function initialises the Mjpeg 
 *
 *  @modif    
 *
 *  @inputs   pContext    [IN] Encode configuration
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_init(MJPEG_PARAMS_S * pContext);

/**
 *  @func     Mjpeg_exit
 *
 *  @desc     Function De-Initialize encode sub-system
 *            MUST be called after Mjpeg_stop() and Vsys_delete()
 *
 *  @modif    
 *
 *  @inputs   None
 *
 *  @outputs  None
 *
 *  @return   None
 *
 */
Void Mjpeg_exit();

/**
 *  @func     Mjpeg_start
 *
 *  @desc     Function starts the Mjpeg 
 *            MUST be called after calling Mjpeg_init() and Vsys_create()
 *
 *  @modif    
 *
 *  @inputs   None
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_start();

/**
 *  @func     Mjpeg_stop
 *
 *  @desc     Function does Stop of MJPEG encode
 *
 *  @modif    
 *
 *  @inputs   None
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_stop();

/**
 *  @func     Mjpeg_enableChn
 *
 *  @desc     Function does the Enable a channel in the MJPEG Link
 *
 *  @modif    
 *
 *  @inputs   mjpegChnId  [IN] channel ID to be enabled
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_enableChn(MJPEG_CHN mjpegChnId);

/**
 *  @func     Mjpeg_disableChn
 *
 *  @desc     Function does the Disable a channel in the MJPEG Link
 *
 *  @modif    
 *
 *  @inputs   mjpegChnId  [IN] channel ID to be enabled
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_disableChn(MJPEG_CHN mjpegChnId);

/**
 *  @func     Mjpeg_setDynamicParam
 *
 *  @desc     Function does Set MJPEG Channel specific dynamic parameters
 *
 *  @modif    
 *
 *  @inputs   mjpegChnId            [IN] Channel ID to which this applies
 *            mjpegStrmID           [IN] Channel ID to which this applies
 *            ptEncDynamicParam    [IN] Values for the various dynamic paramet<argument name>
 *            veParamId            [IN] Parameter on which to apply the changeDescription of usage
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_setDynamicParam(MJPEG_CHN mjpegChnId, MJPEG_STRM mjpegStrmID,
                            MJPEG_CHN_DYNAMIC_PARAM_S * ptEncDynamicParam,
                            MJPEG_PARAM_E veParamId);

/**
 *  @func     Mjpeg_getDynamicParam
 *
 *  @desc     Function Gets MJPEG Channel specific dynamic parameters
 *
 *  @modif    
 *
 *  @inputs   mjpegChnId            [IN] Channel ID to which this applies
 *            mjpegStrmID           [IN] Channel ID to which this applies
 *            ptEncDynamicParam    [IN] Values for the various dynamic paramet<argument name>
 *            veParamId            [IN] Parameter on which to apply the changeDescription of usage
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_getDynamicParam(MJPEG_CHN mjpegChnId, MJPEG_STRM mjpegStrmID,
                            MJPEG_CHN_DYNAMIC_PARAM_S * ptEncDynamicParam,
                            MJPEG_PARAM_E veParamId);

/**
 *  @func     Mjpeg_forceIDR
 *
 *  @desc     Function Forces I-frame on a given encoder channel and stream

 *
 *  @modif    
 *
 *  @inputs   mjpegChnId            [IN] Channel ID to which this applies
 *            mjpegStrmID           [IN] Channel ID to which this applies
 *
 *  @outputs  None
 *
 *  @return    ERROR_NONE on success
 *
 */
Int32 Mjpeg_forceIDR(MJPEG_CHN mjpegChnId, MJPEG_STRM mjpegStrmID);

/**
 *  @func     Mjpeg_registerCallback
 *
 *  @desc     Function does Register user specified callbacks
 *
 *  @modif    
 *
 *  @inputs   callback            [IN] User specified callbacks
 *            arg                 [IN] Callback context
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_registerCallback(MJPEG_CALLBACK_S * callback, Ptr arg);

/**
 *  @func     Mjpeg_getCallbackInfo
 *
 *  @desc     Function Returns Callback info registered by the application
 *
 *  @modif    
 *
 *  @inputs   callback            [IN] User specified callbacks
 *            arg                 [IN] Callback context
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_getCallbackInfo(MJPEG_CALLBACK_S ** callback, Ptr * arg);

/**
 *  @func     Mjpeg_getBitstreamBuffer
 *
 *  @desc     Function Get encoded buffers from encode framework
 *
 *  @modif    
 *
 *  @inputs   timeout      [IN]    TIMEOUT_WAIT_FOREVER or TIMEOUT_NO_WAIT or timeout in units of msec
 *
 *  @outputs  pBitsBufList [OUT]   List of Bistream Buffers returned by the encoder
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_getBitstreamBuffer(VCODEC_BITSBUF_LIST_S * pBitsBufList,
                               UInt32 timeout);

/**
 *  @func     Mjpeg_releaseBitstreamBuffer
 *
 *  @desc     Function Release encoded buffers to encode framework
 *   Buffers returned by Mjpeg_getBitstreamBuffer() are returned to the framework
 *   for resue after user is done using the encoded bitstreams
 *
 *  @modif    
 *
 *  @inputs   pBitsBufList [IN]   List of Bistream Buffers to return to the framework
 *
 *  @outputs  None
 *
 *  @return   ERROR_NONE on success
 *
 */
Int32 Mjpeg_releaseBitstreamBuffer(VCODEC_BITSBUF_LIST_S * pBitsBufList);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
