/** ==================================================================
 *  @file   vstabLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains GlbceSupport Link API.        
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
    \defgroup GLBCESUPPORT_LINK_API GlbceSupport Link API

    GLBCESUPPORT link applies Video Stabilization (VS) algorithm on the captured Video frame.
	VS algorithm works on the Boundary Signal Calculator (BSC) Data of current and previous frames.
	BSC H/W module is present in the IPIPE.

    @{
*/

/**
    \file glbceSupportLink.h
    \brief GlbceSupport Link API
*/

#ifndef _GLBCESUPPORT_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GLBCESUPPORT_H_

#include <mcfw/interfaces/link_api/system.h>

#define GLBCE_SUPPORT_LINK_CMD_TC			(0x6004)
#define GLBCE_SUPPORT_LINK_CMD_BC			(0x6005)


/**
    \brief GlbceSupport Link process parameters
*/
typedef struct {
   UInt8 * boxCarPtr;
   Uint32  boxCarSize;
   UInt8 * tonalCurvePtr;
   Uint32  tonalCurveSize;
   Int16 * rgbMatPtr;
   Uint32  rgbMatSize;
   UInt8  index;
   UInt32 GBEStrength;
   UInt32 LBEStrength;
   
   UInt32 GCEStrength;
   UInt32 LCEStrength;
   
   UInt32 WB_RGain ;
   UInt32 WB_GrGain;
   UInt32 WB_GbGain;
   UInt32 WB_BGain ;
   
   UInt32 sensorType;
   //Int16 *rgb2rgbMat;
   UInt32 toneCurveIndex;
   UInt32 imgVertSize;
   UInt32 imgHorzSize;
   UInt32 downsampleRatio;
   UInt32 temporalAverWeightLog;
} GlbceSupportLink_processPrm;

/**
    \brief GlbceSupport Link create parameters
*/
typedef struct {
    UInt32 totalFrameHeight;
    /**< VS input frame height */
	
	UInt32 totalFrameWidth;
	/**< VS input frame width */
	
	UInt32 glbceLinkID;
	
} GlbceSupportLink_CreateParams;


/**
 *  @func     GlbceSupportLink_init                                               
 *                                                                    
 *  @desc     Function does the GlbceSupport link register and init
 *               - Creates link task
 *               - Registers as a link with the system API                           
 *                                                                    
 *  @modif             
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *                                                     
 *  @outputs  
 *                                                                    
 *  @return   GLBCE_SUPPORT_LINK_S_SUCCESS on success                  
 *
 */                                                                   
Int32 GlbceSupportLink_init();

/**
 *  @func     GlbceSupportLink_deInit                                               
 *                                                                    
 *  @desc     Function does the GlbceSupport link de-register and de-init                           
 *             - Deletes link task
 *             - De-registers as a link with the system API
 *                                                                    
 *  @modif    
 *                                                                    
 *  @inputs   
 *                                                                    
 *  @outputs                                     
 *                                                                    
 *  @return  GLBCE_SUPPORT_LINK_S_SUCCESS on success                
 *
 */                                                                   
Int32 GlbceSupportLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
