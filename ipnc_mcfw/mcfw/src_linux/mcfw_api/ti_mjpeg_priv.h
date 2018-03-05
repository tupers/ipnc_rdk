/** ==================================================================
 *  @file   ti_mjpeg_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_linux/mcfw_api/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef __TI_MJPEG_PRIV_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_MJPEG_PRIV_H__

#include "ti_mjpeg.h"

/* =============================================================================
 * Structure
 * ============================================================================= */

typedef struct {
    UInt32 mjpepEncId;
    UInt32 ipcBitsInHLOSId;
    UInt32 ipcBitsOutRTOSId;
    MJPEG_CALLBACK_S callbackFxn;
    Ptr callbackArg;
    MJPEG_PARAMS_S mjpegEncConfig;
} MJPEG_MODULE_CONTEXT_S;

extern MJPEG_MODULE_CONTEXT_S gMjpegModuleContext;

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
Int32 Mjpeg_delete();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
