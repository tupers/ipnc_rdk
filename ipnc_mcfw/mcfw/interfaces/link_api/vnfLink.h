/** ==================================================================
 *  @file   vnfLink.h                                                  
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
    \defgroup VNF_LINK_API 3D Noise Filter Link API

    3D Noise Filter Link can be used to take YUV frames from intput and after processing
    send YUV frames to output. Supported functionalities include:
    1) Lens Distortion Correction
    2) Affine Transform - warping, scaling, rotation
    3) Spatial Noise Filter (with additional features like shading gain correction, desaturation, smoothing, edge enhancement)
    4) Temporal Noise Filter

    @{
*/

/**
    \file vnfLink.h
    \brief 3D Noise Filter Link API
*/

#ifndef _VNF_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _VNF_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief VNF default output queue ID */
#define VNF_LINK_OUT_QUE_ID     				(0)

/** \brief Max VNF channels per link */
#define VNF_LINK_MAX_CH         				(1)

/**
    \brief Link CMD: get and set filtering params
*/
#define VNF_LINK_CMD_GET_STATIC_PARAMS       	(0x6001)
#define VNF_LINK_CMD_GET_DYNAMIC_PARAMS       	(0x6002)
#define VNF_LINK_CMD_SET_DYNAMIC_PARAMS       	(0x6004)
#define VNF_LINK_CMD_UPDATE_DCC              	(0x6005) 

#define VNF_LINK_CMD_ENABLE_LDC	 				(0x6010)
#define VNF_LINK_CMD_ENABLE_TNF	 				(0x6020)
#define VNF_LINK_CMD_ENABLE_SNF	 				(0x6040)

#define VNF_LINK_CMD_VNF_STRENGTH	 			(0x6080)

#define VNF_LINK_CMD_ENABLE_CHANNEL				(0x6090)
#define VNF_LINK_CMD_DISABLE_CHANNEL			(0x6091)


/**
 * @enum VNF_LINK_TNF3_STRENGTH
 * TNF3 strength
*/
typedef enum {
	VNF_LINK_VNF_STRENGTH_AUTO = 0,
	VNF_LINK_VNF_STRENGTH_LOW,
	VNF_LINK_VNF_STRENGTH_MIDDLE,
	VNF_LINK_VNF_STRENGTH_HIGH,
} VNF_LINK_VNF_STRENGTH;

/**==== */
/**
 * @enum VNF_LINK_OPERATION_MODE
 * Mode of operation of the VNF module
 *
 * @param VNF_LINK_LDC_AFFINE_NSF_TNF      : Performs LDC, Affine Transform, Spatial and Temporal Filtering
 * @param VNF_LINK_AFFINE_NSF_TNF          	: Performs only Affine Transform, Spatial and Temporal Filtering
 */
/**==== */
typedef enum {
    VNF_LINK_LDC_AFFINE_NSF_TNF = 0,
    VNF_LINK_AFFINE_NSF_TNF = 1,
    VNF_LINK_TNF = 2,
    VNF_LINK_3DNF = 3,
    VNF_LINK_LDC_TNF = 4,
    VNF_LINK_LDC = 5,
    VNF_LINK_TNF3 = 6,
    VNF_LINK_NSF2 = 7
} VNF_LINK_OPERATION_MODE;

/* ======================================================================= */
/**
 * @enum VNF_LINK_NSF_SET
 * Enumeration to check whether NSF2 is disabled
 * or enabled for chroma only
 * or enabled for both luma and chroma
 *
 * @param VNF_LINK_NSF_DISABLE   : disable the feature
 * @param VNF_LINK_NSF_CHROMA_ONLY   : chroma only NSF2
 * @param VNF_LINK_NSF_LUMA_CHROMA   : luma + chroma only NSF2
 */
/* ======================================================================= */
typedef enum
{
   VNF_LINK_NSF_DISABLE = 0,
   VNF_LINK_NSF_CHROMA_ONLY = 1,
   VNF_LINK_NSF_LUMA_CHROMA = 2
} VNF_LINK_NSF_SET;

/**==== */
/**
 * @enum VNF_LINK_Y_INTERPOLATE_METHOD
 * Enumeration constant for interpolation method to be used for Y data in LDC
 *
 * @param VNF_LINK_YINTERPOLATION_BICUBIC    : Bicubic Interpolation
 * @param VNF_LINK_YINTERPOLATION_BILINEAR   : Bilinear Interpolation
 */
/**======= 
 */
typedef enum {
    VNF_LINK_YINTERPOLATION_BICUBIC = 0,
    VNF_LINK_YINTERPOLATION_BILINEAR = 1
} VNF_LINK_Y_INTERPOLATE_METHOD;

/**==== */
/**
 * @enum VNF_LINK_SMOOTH_PARAM
 * Smooth parameter for NSF
 *
 * @param VNF_LINK_SMOOTH_0        : Smooth parameter =0
 * @param VNF_LINK_SMOOTH_1        : Smooth parameter =1
 * @param VNF_LINK_SMOOTH_2        : Smooth parameter =2
 * @param VNF_LINK_SMOOTH_3        : Smooth parameter =3
 */
/**==== */
typedef enum {
    VNF_LINK_SMOOTH_0 = 0,
    VNF_LINK_SMOOTH_1 = 1,
    VNF_LINK_SMOOTH_2 = 2,
    VNF_LINK_SMOOTH_3 = 3
} VNF_LINK_SMOOTH_PARAM;

/**==== */
/**
 * @enum VNF_LINK_PARAM_SET
 * Enumeration to check whether given feature is disabled or has to use default 
 * parameters or has to use user-supplied parameters
 *
 * @param VNF_LINK_PARAM_DISABLE   : Disable the feature
 * @param VNF_LINK_PARAM_DEFAULT   : Use default parameters 
 * @param VNF_LINK_PARAM_USER      : Use user-specified parameters
 */
/**==== */
typedef enum {
    VNF_LINK_PARAM_DISABLE = 0,
    VNF_LINK_PARAM_DEFAULT = 1,
    VNF_LINK_PARAM_USER = 2
} VNF_LINK_PARAM_SET;

/**
    \brief Dynamic params feature mask - to be set properly when calling VNF_LINK_CMD_SET(GET)_DYNAMIC_PARAMS
*/
#define VNF_LINK_FEATURE_NSF_GLOBAL					0x0001
#define VNF_LINK_FEATURE_TNF						0x0002
#define VNF_LINK_FEATURE_LDC						0x0004
#define VNF_LINK_FEATURE_NSF_FILTER 				0x0010
#define VNF_LINK_FEATURE_NSF_SHADING				0x0020
#define VNF_LINK_FEATURE_NSF_EDGE 					0x0040
#define VNF_LINK_FEATURE_NSF_DESATURATION			0x0080

/**
    \brief VNF link NSF global parameters
*/
typedef struct VnfLink_NsfGlobalParams {
    UInt32 bLumaEn;
    UInt32 bChromaEn;
    UInt32 bSmoothLumaEn;
    UInt32 bSmoothChromaEn;
    UInt32 eSmoothVal;
} VnfLink_NsfGlobalParams;

/**
    \brief VNF link NSF filter parameters
*/
typedef struct VnfLink_NsfFilterParams {
    UInt32 eFilterParam;
    Int16 rgnFilterVal[18];
    Int16 rgnHalfFilterVal[18];
    Int16 rgnQuadFilterVal[18];
} VnfLink_NsfFilterParams;

/**
    \brief VNF link NSF shading gain parameters
*/
typedef struct VnfLink_NsfShadingParams {
    UInt32 eShdParam;
    Int16 rgnShdVal[16];
} VnfLink_NsfShadingParams;

/**
    \brief VNF link NSF edge enhancement parameters
*/
typedef struct VnfLink_NsfEdgeParams {
    UInt32 eEdgeParam;
    Int16 rgnEdgeVal[16];
} VnfLink_NsfEdgeParams;

/**
    \brief VNF link NSF desaturation parameters
*/
typedef struct VnfLink_NsfDesaturationParams {
    UInt32 eDesatParam;
    Int16 rgnDesatVal[16];
} VnfLink_NsfDesaturationParams;

/**==== */
/**
 * @struct VnfLink_TnfParams 
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
typedef struct VnfLink_TnfParams {
    UInt32 useDefault;
    UInt32 unYComponentWt;
    UInt32 unUComponentWt;
    UInt32 unVComponentWt;
    UInt32 unMaxBlendingFactor;
    UInt32 unMotionThreshold;
} VnfLink_TnfParams;

/**==== */
/**
 * @struct VnfLink_LdcParams 
 * Create time parameter structure for LDC
 *
 * @param eInterpolationLuma        : Y-interpolation method
 * @param unPixelPad                   : Pixel pad added 
 */
/**==== */
typedef struct {
    UInt32 eInterpolationLuma;
    UInt32 unPixelPad;
} VnfLink_LdcParams;

/**
    \brief VNF link channel dynamic parameters

    Defines filtering dynamic parameters that can be changed and probed dynamically
    on a per channel basis for the VNF link
*/
typedef struct VnfLink_ChDynamicParams {
    UInt32 chId;
    UInt32 featureMask;
    VnfLink_NsfGlobalParams nsfParams;
    VnfLink_NsfFilterParams nsfFilter;
    VnfLink_NsfShadingParams nsfShading;
    VnfLink_NsfEdgeParams nsfEdge;
    VnfLink_NsfDesaturationParams nsfDesaturation;
    VnfLink_TnfParams tnfParams;
    VnfLink_LdcParams ldcParams;
} VnfLink_ChDynamicParams;

/**
    \brief VNF link channel static parameters

    Defines filtering static parameters that can be probed dynamically
    on a per channel basis for the VNF link
*/
typedef struct VnfLink_ChStaticParams {
    UInt32 chId;
    UInt32 computeWidth;
    UInt32 computeHeight;
    UInt32 outputStrideLuma;
    UInt32 outputStrideChroma;
    UInt32 eOutputFormat;
    UInt32 eOperateMode;
	UInt32 eNsfSet;
} VnfLink_ChStaticParams;

/**
    \brief VNF link channel create parameters
*/
typedef struct VnfLink_ChCreateParams {
    VnfLink_ChStaticParams sParams;
    VnfLink_ChDynamicParams dParams;
    UInt32 bTnfEnable;
    UInt32 bSnfEnable;
	VNF_LINK_VNF_STRENGTH strength;
} VnfLink_ChCreateParams;

/**
    \brief VNF link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams outQueParams;
    /**< Output queue information. */
    VnfLink_ChCreateParams chCreateParams[VNF_LINK_MAX_CH];
	
	UInt32 enable[VNF_LINK_MAX_CH];
	
	UInt32 tilerEnable;
	/**< Output memory is TILED or NON_TILED */
	
	UInt32 prevLinkMcTNF;
	/**< Previous link is McTNF or not */
} VnfLink_CreateParams;

/**
 *  @func     VnfLink_init                                               
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
Int32 VnfLink_init();

/**
 *  @func     VnfLink_deInit                                               
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
Int32 VnfLink_deInit();

static inline void VnfLink_CreateParams_Init(VnfLink_CreateParams *pPrm)
{
	UInt32 i;

    memset(pPrm, 0, sizeof(*pPrm));
    	
	for(i = 0;i < VNF_LINK_MAX_CH;i ++)
	{
		pPrm->enable[i] = 1;
	}
	
	pPrm->tilerEnable   = FALSE;	
	pPrm->prevLinkMcTNF = 1;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
