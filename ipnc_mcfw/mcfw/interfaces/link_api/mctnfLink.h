/** ==================================================================
 *  @file   mctnfLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains 3D Noise Filter Link API.           
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
    \ingroup LINK_API
    \defgroup MCTNF_LINK_API MCTNF Link API

    3D Noise Filter Link can be used to take YUV frames from intput and after processing
    send YUV frames to output. Supported functionalities include:
    1) Lens Distortion Correction
    2) Affine Transform - warping, scaling, rotation
    3) Spatial Noise Filter (with additional features like shading gain correction, desaturation, smoothing, edge enhancement)
    4) Temporal Noise Filter

    @{
*/

/**
    \file mctnfLink.h
    \brief 3D Noise Filter Link API
*/

#ifndef _MCTNF_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MCTNF_LINK_H_

#include <mcfw/interfaces/link_api/system.h>
//#include <mcfw/src_bios6/links_m3video/mctnf/mctnfLink_mctnf.h>
/** \brief VNF default output queue ID */
#define MCTNF_LINK_OUT_QUE_ID     (0)

/** \brief Max VNF channels per link */
#define MCTNF_LINK_MAX_CH         (1)

/**
    \brief Link CMD: get and set filtering params
*/
#define MCTNF_LINK_CMD_GET_STATIC_PARAMS       	(0x6001)
#define MCTNF_LINK_CMD_GET_DYNAMIC_PARAMS       (0x6002)
#define MCTNF_LINK_CMD_SET_DYNAMIC_PARAMS       (0x6004)
#define MCTNF_LINK_CMD_UPDATE_DCC              	(0x6005) 
#define MCTNF_LINK_CMD_UPDATE_2A              	(0x6006) 
#define MCTNF_LINK_CMD_UPDATE_ISS_CONFIG      	(0x6007) 
#define MCTNF_LINK_CMD_BYPASS_EN            	(0x6008)
#define MCTNF_LINK_CMD_ENABLE_CHANNEL			(0x6009)
#define MCTNF_LINK_CMD_DISABLE_CHANNEL			(0x600A)


#define MCTNF_LINK_CMD_VNF_STRENGTH	 (0x6080)


/**
 * @enum MCTNF_LINK_TNF3_STRENGTH
 * TNF3 strength
*/
typedef enum {
	MCTNF_LINK_VNF_STRENGTH_AUTO = 0,
	MCTNF_LINK_VNF_STRENGTH_LOW,
	MCTNF_LINK_VNF_STRENGTH_MIDDLE,
	MCTNF_LINK_VNF_STRENGTH_HIGH,
} MCTNF_LINK_VNF_STRENGTH;

/**==== */
/**
 * @enum MCTNF_LINK_OPERATION_MODE
 * Mode of operation of the VNF module
 *
 * @param MCTNF_LINK_LDC_AFFINE_NSF_TNF      : Performs LDC, Affine Transform, Spatial and Temporal Filtering
 * @param MCTNF_LINK_AFFINE_NSF_TNF          	: Performs only Affine Transform, Spatial and Temporal Filtering
 */
/**==== */
typedef enum {
    MCTNF_LINK_LDC_AFFINE_NSF_TNF = 0,
    MCTNF_LINK_AFFINE_NSF_TNF = 1,
    MCTNF_LINK_TNF = 2,
    MCTNF_LINK_3DNF = 3,
    MCTNF_LINK_LDC_TNF = 4,
    MCTNF_LINK_LDC = 5,
    MCTNF_LINK_TNF3 = 6,
} MCTNF_LINK_OPERATION_MODE;

/* ======================================================================= */
/**
 * @enum MCTNF_LINK_NSF_SET
 * Enumeration to check whether NSF2 is disabled
 * or enabled for chroma only
 * or enabled for both luma and chroma
 *
 * @param MCTNF_LINK_NSF_DISABLE   : disable the feature
 * @param MCTNF_LINK_NSF_CHROMA_ONLY   : chroma only NSF2
 * @param MCTNF_LINK_NSF_LUMA_CHROMA   : luma + chroma only NSF2
 */
/* ======================================================================= */
typedef enum
{
   MCTNF_LINK_NSF_DISABLE = 0,
   MCTNF_LINK_NSF_CHROMA_ONLY = 1,
   MCTNF_LINK_NSF_LUMA_CHROMA = 2
} MCTNF_LINK_NSF_SET;

/**==== */
/**
 * @enum MCTNF_LINK_Y_INTERPOLATE_METHOD
 * Enumeration constant for interpolation method to be used for Y data in LDC
 *
 * @param MCTNF_LINK_YINTERPOLATION_BICUBIC    : Bicubic Interpolation
 * @param MCTNF_LINK_YINTERPOLATION_BILINEAR   : Bilinear Interpolation
 */
/**======= 
 */
typedef enum {
    MCTNF_LINK_YINTERPOLATION_BICUBIC = 0,
    MCTNF_LINK_YINTERPOLATION_BILINEAR = 1
} MCTNF_LINK_Y_INTERPOLATE_METHOD;

/**==== */
/**
 * @enum MCTNF_LINK_SMOOTH_PARAM
 * Smooth parameter for NSF
 *
 * @param MCTNF_LINK_SMOOTH_0        : Smooth parameter =0
 * @param MCTNF_LINK_SMOOTH_1        : Smooth parameter =1
 * @param MCTNF_LINK_SMOOTH_2        : Smooth parameter =2
 * @param MCTNF_LINK_SMOOTH_3        : Smooth parameter =3
 */
/**==== */
typedef enum {
    MCTNF_LINK_SMOOTH_0 = 0,
    MCTNF_LINK_SMOOTH_1 = 1,
    MCTNF_LINK_SMOOTH_2 = 2,
    MCTNF_LINK_SMOOTH_3 = 3
} MCTNF_LINK_SMOOTH_PARAM;

/**==== */
/**
 * @enum MCTNF_LINK_PARAM_SET
 * Enumeration to check whether given feature is disabled or has to use default 
 * parameters or has to use user-supplied parameters
 *
 * @param MCTNF_LINK_PARAM_DISABLE   : Disable the feature
 * @param MCTNF_LINK_PARAM_DEFAULT   : Use default parameters 
 * @param MCTNF_LINK_PARAM_USER      : Use user-specified parameters
 */
/**==== */
typedef enum {
    MCTNF_LINK_PARAM_DISABLE = 0,
    MCTNF_LINK_PARAM_DEFAULT = 1,
    MCTNF_LINK_PARAM_USER = 2
} MCTNF_LINK_PARAM_SET;

/**
    \brief Dynamic params feature mask - to be set properly when calling MCTNF_LINK_CMD_SET(GET)_DYNAMIC_PARAMS
*/
#define MCTNF_LINK_FEATURE_NSF_GLOBAL					0x0001
#define MCTNF_LINK_FEATURE_TNF						0x0002
#define MCTNF_LINK_FEATURE_LDC						0x0004
#define MCTNF_LINK_FEATURE_NSF_FILTER 				0x0010
#define MCTNF_LINK_FEATURE_NSF_SHADING				0x0020
#define MCTNF_LINK_FEATURE_NSF_EDGE 					0x0040
#define MCTNF_LINK_FEATURE_NSF_DESATURATION			0x0080

/**
    \brief VNF link NSF global parameters
*/
typedef struct MctnfLink_NsfGlobalParams {
    UInt32 bLumaEn;
    UInt32 bChromaEn;
    UInt32 bSmoothLumaEn;
    UInt32 bSmoothChromaEn;
    UInt32 eSmoothVal;
} MctnfLink_NsfGlobalParams;

/**
    \brief VNF link NSF filter parameters
*/
typedef struct MctnfLink_NsfFilterParams {
    UInt32 eFilterParam;
    Int16 rgnFilterVal[18];
    Int16 rgnHalfFilterVal[18];
    Int16 rgnQuadFilterVal[18];
} MctnfLink_NsfFilterParams;

/**
    \brief VNF link NSF shading gain parameters
*/
typedef struct MctnfLink_NsfShadingParams {
    UInt32 eShdParam;
    Int16 rgnShdVal[16];
} MctnfLink_NsfShadingParams;

/**
    \brief VNF link NSF edge enhancement parameters
*/
typedef struct MctnfLink_NsfEdgeParams {
    UInt32 eEdgeParam;
    Int16 rgnEdgeVal[16];
} MctnfLink_NsfEdgeParams;

/**
    \brief VNF link NSF desaturation parameters
*/
typedef struct MctnfLink_NsfDesaturationParams {
    UInt32 eDesatParam;
    Int16 rgnDesatVal[16];
} MctnfLink_NsfDesaturationParams;

/**==== */
/**
 * @struct MctnfLink_TnfParams 
 * Create time parameter structure for TNF
 *
 * @param unYComponentWt	: Assigned weight of luma component for motion computation (Q4 format is used)
 * @param unUComponentWt	: Assigned weight of chroma, cb or u component for motion computation (Q4 format is used)
 * @param unVComponentWt	: Assigned weight of chroma, cr or v component for motion computation (Q4 format is used)
 * @param unMaxBlendingFactor	: Maximum value of blending factor used for generating the temporal noise filter look-up table
 * @param unMotionThreshold	: Threshold for maximum allowed motion component, if motion exceeds this threshold then the 
 *                                  		  temporal filtered frame would be same as current frame
 */
/**==== */
typedef struct MctnfLink_TnfParams {
    UInt32 useDefault;
    UInt32 unYComponentWt;
    UInt32 unUComponentWt;
    UInt32 unVComponentWt;
    UInt32 unMaxBlendingFactor;
    UInt32 unMotionThreshold;
} MctnfLink_TnfParams;

/**==== */
/**
 * @struct MctnfLink_LdcParams 
 * Create time parameter structure for LDC
 *
 * @param eInterpolationLuma        : Y-interpolation method
 * @param unPixelPad                   : Pixel pad added 
 */
/**==== */
typedef struct {
    UInt32 eInterpolationLuma;
    UInt32 unPixelPad;
} MctnfLink_LdcParams;

typedef struct {
  Int8   motionSearchPreset;
  Int16  searchRangeHor;
  Int16  searchRangeVer;
}MctnfLink_MotionSearchParams;

typedef struct {
  Int32 meEffectivenessTh;
  Int32 blendingFactorQ4;
  Int32 minBlendQ4;
  Int32 maxBlendQ4;
  Int32 meLambdaType;
  Int32 meLambdaFactorQ2;
  Int32 maxLambdaQ2;
  Int32 sadForMinLambda;
  Int32 fixWtForCurQ8;
  Int32 minWtForCurQ8;
  Int32 biasZeroMotion;
  Int32 staticMBThZeroMV;
  Int32 staticMBThNonZeroMV;
  Int32 blockinessRemFactor;
  Int32 sadForMaxStrength;
  Int32 mvXThForStaticLamda;
  Int32 mvYThForStaticLamda;
}MctnfLink_NoiseFilterParams;
/**
    \brief VNF link channel dynamic parameters

    Defines filtering dynamic parameters that can be changed and probed dynamically
    on a per channel basis for the VNF link
*/
typedef struct MctnfLink_ChDynamicParams {
    UInt32 chId;
    //UInt32 featureMask;
    //MctnfLink_NsfGlobalParams nsfParams;
    //MctnfLink_NsfFilterParams nsfFilter;
    //MctnfLink_NsfShadingParams nsfShading;
    //MctnfLink_NsfEdgeParams nsfEdge;
    //MctnfLink_NsfDesaturationParams nsfDesaturation;
    //MctnfLink_TnfParams tnfParams;
    //MctnfLink_LdcParams ldcParams;
	MctnfLink_MotionSearchParams motionParams;
	MctnfLink_NoiseFilterParams noiseFilterParams;
} MctnfLink_ChDynamicParams;

/**
    \brief VNF link channel static parameters

    Defines filtering static parameters that can be probed dynamically
    on a per channel basis for the VNF link
*/
typedef struct MctnfLink_ChStaticParams {
    UInt32 chId;
    UInt32 computeWidth;
    UInt32 computeHeight;
    UInt32 outputStrideLuma;
    UInt32 outputStrideChroma;
    UInt32 eOutputFormat;
    UInt32 eOperateMode;
	UInt32 eNsfSet;
} MctnfLink_ChStaticParams;

/**
    \brief VNF link channel create parameters
*/
typedef struct MctnfLink_ChCreateParams {
    MctnfLink_ChStaticParams sParams;
    MctnfLink_ChDynamicParams dParams;
    UInt32 bTnfEnable;
    UInt32 bSnfEnable;
	MCTNF_LINK_VNF_STRENGTH strength;
	
} MctnfLink_ChCreateParams;

/**
    \brief VNF link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams outQueParams;
    /**< Output queue information. */
    MctnfLink_ChCreateParams chCreateParams[MCTNF_LINK_MAX_CH];
	UInt32 enable[MCTNF_LINK_MAX_CH];
} MctnfLink_CreateParams;

/**
 *  @func     MctnfLink_init                                               
 *                                                                    
 *  @desc     Function does the VNF link register and init
 *               - Creates link task
 *               - Registers as a link with the system API                           
 *                                                                    
 *  @modif             
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *                                                     
 *  @outputs  
 *                                                                    
 *  @return   FVID2_SOK on success                  
 *
 */                                                                   
Int32 MctnfLink_init();

/**
 *  @func     MctnfLink_deInit                                               
 *                                                                    
 *  @desc     Function does the VNF link de-register and de-init                           
 *             - Deletes link task
 *             - De-registers as a link with the system API
 *                                                                    
 *  @modif    
 *                                                                    
 *  @inputs   
 *                                                                    
 *  @outputs                                     
 *                                                                    
 *  @return  FVID2_SOK on success                
 *
 */                                                                   
Int32 MctnfLink_deInit();

static inline void MctnfLink_CreateParams_Init(MctnfLink_CreateParams *pPrm)
{
	UInt32 i;

    memset(pPrm, 0, sizeof(*pPrm));
    	
	for(i = 0;i < MCTNF_LINK_MAX_CH;i ++)
	{
		pPrm->enable[i] = 1;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
