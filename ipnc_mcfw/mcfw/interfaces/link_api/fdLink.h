/** ==================================================================
 *  @file   fdLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains FD (Face Detect) Link API.        
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
    \defgroup FD_LINK_API Fd Link API

    @{
*/

/**
    \file fdLink.h
    \brief FD Link API
*/

#ifndef _FD_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _FD_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

#define FD_CMD_PRM				(0x8001)
//#define CODEC_PRIV_MASK

/**
    \brief FD link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information */

} FdLink_CreateParams;

/**
    \brief FD link GUI parameters
*/

typedef struct {
    UInt32 fdetect;
    /**< FD enable */

    UInt32 startX;
    /**< FD ROI start X */

    UInt32 startY;
    /**< FD ROI start Y */

    UInt32 width;
    /**< FD ROI width */

    UInt32 height;
    /**< FD ROI height */

    UInt32 fdconflevel;
    /**< FD confidence level */

    UInt32 fddirection;
    /**< FD direction */

    UInt32 pmask;
    /**< FD privacy mask */

} FdLink_GuiParams;

/**
    \brief FD link result parameters
*/
typedef struct {
    UInt32 startX;
    /* X co-ordinate for the face */

    UInt32 startY;
    /* Y co-ordinate for the face */

    UInt32 width;
    /* width of the face */

    UInt32 height;
    /* Height of the face */

} FdLink_faceInfo;

/**
    \brief FD link result parameters
*/
typedef struct {
    UInt32 numFaces;
    /**< Number of faces detected */

    FdLink_faceInfo faceInfo[35];
    /* Detected face info */

} FdLink_result;

/**
 *  @func     FdLink_init                                               
 *                                                                    
 *  @desc     Function does the FD link register and init                          
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
Int32 FdLink_init();

/**
 *  @func     FdLink_deInit                                               
 *                                                                    
 *  @desc     Function does the FD link de-register and de-init                             
 *                                                                    
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
Int32 FdLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
