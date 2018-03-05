/** ==================================================================
 *  @file   glbceLink_glbce.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/glbce/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _GLBCELINK_GLBCE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GLBCELINK_GLBCE_H_

#include "glbceLink_priv.h"

typedef enum glbcePresets
{
   GLBCE_HS_FL = 1,
   GLBCE_HS_FM,
   GLBCE_HS_FH,
   GLBCE_HQ_FL,
   GLBCE_HQ_FM,
   GLBCE_HQ_FH,
   GLBCE_MAX_PRESET
} glbcePresets;

/* ===================================================================
 *  @func     MSP_GLBCE_ProcessFrame                                               
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
Int32 MSP_GLBCE_ProcessFrame(GlbceLink_Obj * pObj,
                           
                           FVID2_Frame * currentImageBuffer,
                           FVID2_Frame * outputImageBuffer);

/* ===================================================================
 *  @func     MSP_GLBCE_Engine_Init                                               
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
Int32 MSP_GLBCE_Engine_Init(GlbceLink_Obj * pObj);

/* ===================================================================
 *  @func     MSP_GLBCE_Engine_DeInit                                               
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
Int32 MSP_GLBCE_Engine_DeInit(GlbceLink_Obj * pObj);

/* ===================================================================
 *  @func     MSP_GLBCE_Engine_Open                                               
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
Int32 MSP_GLBCE_Engine_Open(GlbceLink_Obj * pObj);

/* ===================================================================
 *  @func     MSP_GLBCE_Engine_Close                                               
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
Int32 MSP_GLBCE_Engine_Close(GlbceLink_Obj * pObj);

#define GOTO_EXIT(E) \																									\
		Vps_printf("Error %d :: %s : %s : %d\n", E, __FILE__, __FUNCTION__, __LINE__);						  \
		return E;																				\

#define GOTO_EXIT_IF(C,E)                                                                                   \
        if (C) {                                                                                            \
			Vps_printf("Error %d :: %s : %s : %d\n", E, __FILE__, __FUNCTION__, __LINE__);                        \
            return E;                                                                                  \
        }                                                                                                   \

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     // _GLBCELINK_GLBCE_H_
