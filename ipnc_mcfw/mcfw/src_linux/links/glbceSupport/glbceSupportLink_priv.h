/** ==================================================================
 *  @file   glbceSupportLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/glbceSupport/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _GLBCE_SUPPORT_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GLBCE_SUPPORT_LINK_PRIV_H_

#include <stddef.h>
#include <stdio.h>
#include <osa.h>
#include <osa_que.h>
#include <osa_tsk.h>
#include <osa_debug.h>
#include <ti/ipc/SharedRegion.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/system_debug.h>

//#include <ti/psp/iss/drivers/alg/glbceSupport/alg_glbceSupport.h>
#include <mcfw/src_linux/links/system/system_priv_common.h>
#include <mcfw/src_linux/links/system/systemLink_priv.h>
#include <mcfw/interfaces/link_api/glbceSupportLink.h>
#include <mcfw/interfaces/link_api/glbceLink.h>
#include <glbce/inc/dataTypeDefs.h>
#include <glbce/inc/TI_glbce.h>
#include <glbce/inc/glbceStruct.h>


#define GLBCE_SUPPORT_LINK_OBJ_MAX		(1)

/**
 *  @def   IPC_BITS_IN_LINK_S_SUCCESS
 *  @brief Define used to indicate successful execution
 */
#define GLBCE_SUPPORT_LINK_S_SUCCESS   (0)



typedef struct {
	int vsHandle;

} GlbceSupportLink_algObj;

typedef struct {
    OSA_TskHndl tsk;
    GlbceSupportLink_CreateParams createArgs;
    UInt32 receviedFrameCount;
    GlbceSupportLink_algObj algObj;

	GLBCE_CONTEXT glbceHandle; //This Handle handles the tonc curve etc
	GLBCE_CreationInputParamsStruct *glbceCIP;
	GLBCE_ImageInputParamsStruct    *glbceIIP;
	GLBCEstruct *glbce;
	UInt32 WIDTH;
	UInt32 HEIGHT;
    //External scratch memory
    Word16 *externalScratchMemory;

    //External scratch memory
    Word16 *PersistentMemory;
	
	
	
} GlbceSupportLink_Obj;

/* Function prototypes */

/* ===================================================================
 *  @func     GlbceSupportLink_algCreate                                               
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
Int32 GlbceSupportLink_algCreate(GlbceSupportLink_Obj *pObj);

/* ===================================================================
 *  @func     GlbceSupportLink_algDelete                                               
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
Int32 GlbceSupportLink_algDelete(GlbceSupportLink_Obj * pObj);

Int32 GlbceSupportLink_algProcess(GlbceSupportLink_Obj * pObj,
                           GlbceSupportLink_processPrm *pProcessPrm);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
