/** ==================================================================
 *  @file   ti_vcam.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/                                                 
 *                                                                    
 *  @desc   This  File contains McFW Video Camera (VCAM) API.    
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
    \defgroup MCFW_VCAM_API McFW Video Camera (VCAM) API

    @{
*/

/**
    \file ti_vcam.h
    \brief McFW Video Camera (VCAM) API
*/

#ifndef __TI_VCAM_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_VCAM_H__

#include "ti_vsys.h"

/**==========
 * Defines
 * ============================================================================= */

/** Maximum Video Camera channels */
#define VCAM_CHN_MAX		(1)

/** Maximum Video Camera streams */
#define VCAM_STRM_MAX       (2)

/**==========
 * Enums
 * ============================================================================= */

/**
    \brief Camera Device ID
*/
typedef enum {
    VCAM_DEV_VP = 0,
    /**< Parallel video port */

    VCAM_DEV_CSI2 = 1,
    /**< CSI2 Port */

    VCAM_DEV_MAX = 2
    /**< Maximum Camera Devices */
} VCAM_DEV;

/**
    \brief Camera bus interface mode
*/
typedef enum {
    VCAM_MODE_YUV422 = 0,
    /**< 16-bit Embedded sync YUV422 mode */

    VCAM_MODE_YUV420,
    /**< 16-bit Embedded sync YUV420 mode */

} VCAM_MODE_E;

/**
    \brief Camera Multi-Channel mode
*/
typedef enum {

    VCAM_MULTICHN_OPTION_SINGLE_CHN = 0
    /**< 1CH Non-Multiplexed mode */
} VCAM_MULTICHN_OPTION_E;

/**
    \brief Camera Param Set/Get ID
*/
typedef enum {
    VCAM_CONTRAST,
    /**< Get/Set Contrast */

    VCAM_SATURATION,
    /**< Get/Set Saturation */

    VCAM_BRIGHTNESS,
    /**< Get/Set Brightness */

    VCAM_SHARPNESS,
    /**< Get/Set Sharpness */
	VCAM_INCREASE_EXPOSURE,
	/**<increace exposure */
	VCAM_DECREASE_EXPOSURE,
	/**<decreace exposure */
	VCAM_SET_EXPOSURE,
	VCAM_GET_EXPOSURE,
	VCAM_GET_MAX_EXPOSURE,
	VCAM_GET_MIN_EXPOSURE,
	VCAM_SET_GAIN,
	VCAM_GET_GAIN,
	VCAM_GET_MAX_GAIN,
	VCAM_GET_MIN_GAIN,	
    VCAM_MIRROR_MODE,
    /**< Mirror/Flip mode */

    VCAM_BLC,
    /**< Get/Set BLC */

    VCAM_AWBMODE,
    /**< Get/Set AWB Mode */

    VCAM_AEMODE,
    /**< Get/Set AE Mode */

    VCAM_HUE,
    /**< Get/Set HUE */

    VCAM_RESOLUTION,
    /**< Get/Set Resolution */

    VCAM_FORMAT,
    /**< Get/Set Data Format */

    VCAM_PORTMODE,
    /**< Get/Set Port Mode */

    VCAM_SIGNALTYPE,
    /**< Get/Set Signal Type */

    VCAM_DCCBUFADDR,
    /**< Set DCC PARAM Buffer */

    VCAM_AEWB_VENDOR,
    /**< Set AEWB Vendor Type */

    VCAM_AEWB_MODE,
    /**< Set AEWB Vendor Type */

    VCAM_AEWB_PRIORITY,
    /**< Set AEWB Priority */

	VCAM_GET_2A_PARAM,
	/**< Get 2A param */
	
	VCAM_SET_2A_PARAM,
	/**< Set 2A param */

	VCAM_GET_2A_WEIGHT,
	/**< Get 2A weight */
	
	VCAM_SET_2A_WEIGHT,
	/**< Set 2A weight */
	
    VCAM_IRIS,
    /**< Set IRIS */

    VCAM_FRAMECTRL,
    /**< Set Frame rate control */

    VCAM_ENV_50_60HZ,
    /**< Set ENV 50 60 hz */

    VCAM_BINNING_SKIP,
    /**< Set Binning Skip mode */

    VCAM_FRAMERATE,
    /**< Set Frame Rate */

    VCAM_LDC,
    /**< Lens Distorsion Correction */

    VCAM_SNF,
    /**< Spatial Noise Filter */

    VCAM_TNF,
    /**< Temporal Noise Filter */

	VCAM_SD_FORMAT,
	/**< NTSC/PAL */

    VCAM_GLBCE,
    /** < GLBCE PRESET */

    VCAM_NF_STRENGTH,
    /** < Noise Filter Strength */

    VCAM_ALL
    /**< Get/Set All Params */
} VCAM_PARAMS_E;

/**==========
 * Structures
 * ============================================================================= */

/**
    \brief Camera Device Parameters
*/
typedef struct {
    Bool portEnable;
    /**< Enable/Disable Camera Port */

    VCAM_MODE_E portMode;
    /**< Camera bus interface mode */

    VCAM_MULTICHN_OPTION_E dataFormat;
    /**< Camera Multi-channel mode */

    VIDEO_STANDARD_E signalType;
    /**< Camera video standard or signal type */

} VCAM_DEV_PARAM_S;

typedef struct {
	int size;
	char *pData;
}CameraH3AParam;
/**
    \brief Camera Channel Specific Dynamic Parameters
*/
typedef struct {
    Int32 contrast;
    /**< Constrast, 0..255 */

    Int32 saturation;
    /**< Saturation, 0..255 */

    Int32 brightness;
    /**< Brightness, 0..255 */

    Int32 sharpness;
    /**< Sharpness, 0..255 */

    Int32 frameRate;
    /**< Framerate, 0..60 */

    Int32 resolution;
    /**< Resolution, 0..255 */

    Int32 blc;
    /**< blc, 0..5 */
	Int32 exposure;
	/**<exposure >*/
	Int32 maxexposure;
	Int32 minexposure;

/** sensor gain **/
	Int32 sensorGain;
	Int32 maxsensorGain;
	Int32 minsensorGain;
/** pipe gain **/
	Int32 pipeGain;
	Int32 maxpipeGain;
	Int32 minpipeGain;
		
    Int32 awbMode;
    /**< AWB mode, 0..2 */

    Int32 AEMode;
    /**< AE mode, 0..1 */

    Int32 hue;
    /**< HUE, 0..255 */

    Int32 dccBufAddr;
    /**< DCC Buffer Address */
	
	UInt32 dccSize;
    /**< DCC Buffer Size */

    Int32 aewbVendor;
    /**< AEWB Vendor Type */

    Int32 aewbMode;
    /**< AEWB Mode*/

    Int32 mirrorMode;
    /**< Mirror Mode*/

    Int32 aewbPriority;
    /**< AEWB Priority*/

    Int32 env50_60hz;
    /**< ENV Mode*/

    Int32 binningMode;
    /**< Binning Skip mode*/

    Int32 IRIS;

    Int32 frameCtrl;

    Int32 enableLDC;

    Int32 enableTNF;

    Int32 enableSNF;

	Int32 sdFormat;

    Int32 enableGlbce;

	Int32 InputWidth;

	Int32 InputHeight;

	Int32 nfStrength;

	CameraH3AParam H3aParams;

	char *pAppData;
} VCAM_CHN_DYNAMIC_PARAM_S;

/**
    \brief Camera Channel Specific Static Parameters
*/
typedef struct {
    UInt32 inputWidth;
    /**< Camera Input Width */

    UInt32 inputHeight;
    /**< Camera Input height */

    VCAM_DEV deviceId;
    /**< Device ID to which this channel is mapped */

    Bool strmEnable[VCAM_STRM_MAX];
    /**< Enable/Disable Streams in camera, stream 0 should be enabled always */

    VIDEO_FORMATE_E strmFormat[VCAM_STRM_MAX];
    /**< Data Format, YUV422 or YUV420SP of the camera stream. MUST be VF_YUV422I_YUYV for multi-channel camera */

    WINDOW_S strmResolution[VCAM_STRM_MAX];
    /**< Resolution of the stream, stream 0 is always 1:1 resolution, stream 1 can downsized resolution of stream 0 */

    VCAM_CHN_DYNAMIC_PARAM_S dynamicParams;
    /**< Initial values for dynamic parameters of camera channel */

} VCAM_CHN_PARAM_S;

/**
    \brief Camera Sub-system initialization parameters
*/
typedef struct {
    VCAM_DEV_PARAM_S deviceParams[VCAM_DEV_MAX];
    /**< Device Parameters */

    VCAM_CHN_PARAM_S channelParams[VCAM_CHN_MAX];
    /**< Channel Specific Parameters */

} VCAM_PARAMS_S;

/**
    \brief Callback for new captured video frames available

    \return NONE
*/
typedef Void (*VCAM_CALLBACK_NEW_DATA)(Ptr cbCtx);

/**
    \brief Capture User Callbacks
*/
typedef struct {

    VCAM_CALLBACK_NEW_DATA  newDataAvailableCb;
    /**< New encoded bitstream available callback */

} VCAM_CALLBACK_S;

/**==========
 * APIs
 * ============================================================================= */

/**
 *  @func     Vcam_params_init                                               
 *                                                                    
 *  @desc     Function does the Set defaults for VCAM_PARAMS_S         
 *            By default camera parameters are setup for 16CH D1 camera mode
 *                                                                    
 *  @modif             
 *                                                                    
 *  @inputs   None                                    
 *                                                                    
 *  @outputs  pContext    [OUT] VCAM_PARAMS_S filled with default parameters
 *                                                                    
 *  @return   None
 *
 */                                                                   
Void Vcam_params_init(VCAM_PARAMS_S * pContext);

/**
 *  @func     Vcam_init                                               
 *                                                                    
 *  @desc     Function does the Initialize camera sub-system                             
 *                                                                    
 *  @modif          
 *                                                                    
 *  @inputs   pContext [IN] params for creation / Camera configuration                                   
 *                                                                    
 *  @outputs  None                                   
 *                                                                    
 *  @return   ERROR_NONE on success                  
 *
 */                                                                   
Int32 Vcam_init(VCAM_PARAMS_S * pContext);

/**
 *  @func     Vcam_exit                                               
 *                                                                    
 *  @desc     Function does the De-Initialize camera sub-system      
 *            MUST be called after Vcam_stop() and Vsys_delete()
 *                                                                    
 *  @modif           
 *                                                                    
 *  @inputs   None                                    
 *                                                                    
 *  @outputs  None                                    
 *                                                                    
 *  @return  None                  
 *
 */                                                                   
Void Vcam_exit();

/**
 *  @func     Vcam_start                                               
 *                                                                    
 *  @desc     Function does the Start camera sub-system       
 *            MUST be called after calling Vcam_init() and Vsys_create()
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
Int32 Vcam_start();

/**
 *  @func     Vcam_stop                                               
 *                                                                    
 *  @desc     Function does Stop camera sub-system  
 *            MUST be called before calling Vsys_delete()
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
Int32 Vcam_stop();

/**
 *  @func     Vcam_enableChn                                               
 *                                                                    
 *  @desc     Function does the Enable a channel in the camera sub-system    
 *             By default all channels and streams are enabled after Vcam_start()
 *                                                                    
 *  @modif           
 *                                                                    
 *  @inputs   vcChnId  [IN] Camera channel ID                                        
 *            vcStrmId [IN] Camera stream ID                                   
 *                                                                    
 *  @outputs  None                                   
 *                                                                    
 *  @return   ERROR_NONE on success                  
 *
 */                                                                   
Int32 Vcam_enableChn(VCAM_CHN vcChnId, VCAM_STRM vcStrmId);

/**
 *  @func     Vcam_disableChn                                               
 *                                                                    
 *  @desc     Function does the  Disable a channel in the camera sub-system                            
 *            Disabling a channel will make the channel unavailable for encode and display.
 *            If encode is enabled on that channel then user will stop getting bitstream for that channel.
 *            If display is enabled on that channel then display will hold on the last frame
 *            that was shown for that window on the display.
 *            When a camera channel is disabled, it is recommended to disable the channel on the display as well.
 *                                                                    
 *  @modif    
 *                                                                    
 *  @inputs   vcChnId  [IN] Camera channel ID                                         
 *            vcStrmId [IN] Camera stream ID                                    
 *                                                                    
 *  @outputs None                                   
 *                                                                    
 *  @return   ERROR_NONE on success                  
 *
 */                                                                   
Int32 Vcam_disableChn(VCAM_CHN vcChnId, VCAM_STRM vcStrmId);

/**
 *  @func     Vcam_setParamChn                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_setParamChn(VCAM_CHN vcChnId, VCAM_CHN_PARAM_S * psCamChnParam,
                       VCAM_PARAMS_E paramId);

/**
 *  @func     Vcam_getParamChn                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_getParamChn(VCAM_CHN vcChnId, VCAM_CHN_PARAM_S * psCamChnParam,
                       VCAM_PARAMS_E paramId);

/**
 *  @func     Vcam_setDynamicParamChn                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_setDynamicParamChn(VCAM_CHN vcChnId,
                              VCAM_CHN_DYNAMIC_PARAM_S * psCamChnDynaParam,
                              VCAM_PARAMS_E paramId);

/**
 *  @func     Vcam_setFrameRate                                               
 *                                                                    
 *  @desc     Function does  Set Camera frame rate. 
 *            This is an additional control in camera side itself; 
 *            is really useful for secondary stream <CIF>. Stream 0 is D1 & Stream 1 is CIF.                             
 *                                                                    
 *  @modif    
 *                                                                    
 *  @inputs   vcChnId [IN] video capture Channel ID
 *            vStrmID [IN] stream ID
 *            frameRate [IN] Frame rate to be set
 *                                                                    
 *  @outputs  None                                    
 *                                                                    
 *  @return   ERROR_NONE on success                    
 *
 */                                                                   
Int32 Vcam_setFrameRate(VCAM_CHN vcChnId, VCAM_STRM vStrmId, Int32 frameRate);

/**
 *  @func     Vcam_getFrameRate                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_getFrameRate(VCAM_CHN vcChnId, VCAM_STRM vStrmId);

/**
 *  @func     Vcam_getDynamicParamChn                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_getDynamicParamChn(VCAM_CHN vcChnId,
                              VCAM_CHN_DYNAMIC_PARAM_S * psCamChnParam,
                              VCAM_PARAMS_E paramId);

/**
 *  @func     Vcam_enableDevice                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_enableDevice(VCAM_DEV vcDevId);

/**
 *  @func     Vcam_disableDevice                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_disableDevice(VCAM_DEV vcDevId);

/**
 *  @func     Vcam_setParamDevice                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_setParamDevice(VCAM_DEV vcDevId, VCAM_DEV_PARAM_S * psvcDevParam,
                          VCAM_PARAMS_E paramId);

/**
 *  @func     Vcam_getParamDevice                                               
 *                                                                    
 *  @desc     Coming soon .. NOT SUPPORED in this release                            
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
Int32 Vcam_getParamDevice(VCAM_DEV vcDevId, VCAM_DEV_PARAM_S * psvcDevParam,
                          VCAM_PARAMS_E paramId);

/**
    \brief Request filled video buffers from framework

    User calls this API to get full video frames from the framework.
    After getting the video frames, user will
    - consume the video frames
    - and then call Vcam_putEmptyVideoFrames() to free the video frames back to the framework

    \param pFrameBufList    [OUT]  List of video frames returned by the framework
    \param timeout          [IN]   TIMEOUT_WAIT_FOREVER or TIMEOUT_NO_WAIT or timeout in msecs

    \return ERROR_NONE on success
*/
Int32 Vcam_getFullVideoFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList, UInt32 timeout);

/**
    \brief Give consumed video frames back to the application to be freed

    Buffers that are were previously got from Vcam_getFullVideoFrames can be
    freed back to the framework by invoking this API.

    \param pFrameBufList [IN]   List of video frames

    \return ERROR_NONE on success
*/
Int32 Vcam_putEmptyVideoFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);

Int32 Vcam_EnableEncodeVideo();

Int32 Vcam_DisableEncodeVideo();

Int32 Vcam_EnableSaveErrImg();

Int32 Vcam_DisableSaveErrImg();

Int32 Vcam_SaveFrame(char *filename,unsigned size);

/**
    \brief Register user specified callbacks

    \param callback            [IN] User specified callbacks

    \param arg                 [IN] Callback context

    \return ERROR_NONE on success
*/
Int32 Vcam_registerCallback(VCAM_CALLBACK_S * callback, Ptr arg);

/**
    \brief Change the capture mode/resolution

    \param vcChnId             [IN] channel Id, 0 always

    \param chPrms              [IN] Channel Parameters containing
                                    Resolution information

    \return ERROR_NONE on success
*/
Int32 Vcam_changeCaptMode(VCAM_CHN vcChnId, VCAM_CHN_PARAM_S *chPrms);

/**
    \brief Enable/Disable channels for Scaling

    \param linkId [IN]   Link Id of the Scaler Link
	
	\param chId [IN]   Channel Id
	
	\param enable [IN]   Enable/Dsiable flag

    \return ERROR_NONE on success
*/
Int32 Vcam_enableDisableScalerCh(UInt32 linkId,UInt32 chId,Bool enable);

/**
    \brief Enable/Disable channels for MP Scaling

    \param linkId [IN]   Link Id of the MP Scaler Link
	
	\param chId [IN]   Channel Id
	
	\param enable [IN]   Enable/Dsiable flag

    \return ERROR_NONE on success
*/
Int32 Vcam_enableDisableMpScalerCh(UInt32 linkId,UInt32 chId,Bool enable);

/**
    \brief Enable/Disable channels for Deinterlacing

    \param linkId [IN]   Link Id of the DEI Link
	
	\param chId [IN]   Channel Id
	
	\param streamId [IN]   Stream Id
	
	\param enable [IN]   Enable/Dsiable flag

    \return ERROR_NONE on success
*/
Int32 Vcam_enableDisableDeiCh(UInt32 linkId,UInt32 chId,UInt32 streamId,Bool enable);

/**
    \brief Set Scalar Output Resolution

    \param linkId [IN]   Link Id of the Scaler Link
	
	\param chId [IN]   Channel Id
	
	\param width [IN]   Output Width
	
	\param height [IN]   Output Height

	\param pitch0 [IN]   Output Pitch0

	\param pitch1 [IN]   Output Pitch1	

    \return ERROR_NONE on success
*/
Int32 Vcam_setScalarOutResolution(UInt32 linkId,UInt32 chId,UInt32 width,
							      UInt32 height,UInt32 pitch0,UInt32 pitch1);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
