/** ==================================================================
 *  @file   glbceLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains GLBCE Link API.           
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
    \defgroup GLBCE_LINK_API SALDRE Link API

    SALDRE Link can be used to take BAYER frames from Capture to give improved BAYER based on tonecurves. 

    @{
*/

/**
    \file glbceLink.h
    \brief GLBCE Link API
*/

#ifndef _GLBCE_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GLBCE_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief GLBCE default output queue ID */
#define GLBCE_LINK_OUT_QUE_ID     (0)

/** \brief Max GLBCE channels per link */
#define GLBCE_LINK_MAX_CH         (1)

/**
    \brief Link CMD: get and set filtering params
*/
#define GLBCE_LINK_CMD_GET_STATIC_PARAMS       	(0x6001)
#define GLBCE_LINK_CMD_GET_DYNAMIC_PARAMS       	(0x6002)
#define GLBCE_LINK_CMD_SET_DYNAMIC_PARAMS       	(0x6004)

#define GLBCE_LINK_CMD_TC_READY				(0x7004)
#define GLBCE_LINK_CMD_UPDATE_DCC              	(0x7005) 


/**
    \brief GLBCE link channel dynamic parameters

    Defines filtering dynamic parameters that can be changed and probed dynamically
    on a per channel basis for the GLBCE link
*/
typedef struct GlbceLink_ChDynamicParams {
    UInt32 chId;
    UInt32 featureMask;
	UInt32 glbceEnable;
} GlbceLink_ChDynamicParams;

typedef struct GlbceLink_PresetDynamicParams {
	UInt32 glbceEnable;
} GlbceLink_PresetDynamicParams;
/**
    \brief GLBCE link channel static parameters

    Defines filtering static parameters that can be probed dynamically
    on a per channel basis for the GLBCE link
*/
typedef struct GlbceLink_ChStaticParams {
    UInt32 chId;
    UInt32 computeWidth;
    UInt32 computeHeight;
    UInt32 outputStrideLuma;
    UInt32 outputStrideChroma;
} GlbceLink_ChStaticParams;

typedef enum _GlbcePreset
{
     GLBCE_DISABLED = 0,
     GLBCE_HIGHSPEED = 1,
	 GLBCE_HIGHQUALITY = 2
} GlbcePreset;

/**
    \brief GLBCE link channel create parameters
*/
typedef struct GlbceLink_ChCreateParams {
    GlbceLink_ChStaticParams sParams;
    GlbceLink_ChDynamicParams dParams;
	//UInt32 frequencyTonalCurve;
	GlbcePreset preset;
} GlbceLink_ChCreateParams;

/**
    \brief GLBCE link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams outQueParams;
    /**< Output queue information. */
    GlbceLink_ChCreateParams chCreateParams[GLBCE_LINK_MAX_CH];
	UInt32 glbceEnable;
	UInt32 glbceSupportID;
	UInt32 swosdLinkID;
} GlbceLink_CreateParams;

/**
 *  @func     GlbceLink_init                                               
 *                                                                    
 *  @desc     Function does the GLBCE link register and init
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
Int32 GlbceLink_init();

/**
 *  @func     GlbceLink_deInit                                               
 *                                                                    
 *  @desc     Function does the GLBCE link de-register and de-init                           
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
Int32 GlbceLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
