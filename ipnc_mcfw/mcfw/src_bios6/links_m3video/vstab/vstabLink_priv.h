/** ==================================================================
 *  @file   vstabLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/vstab/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _VSTAB_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _VSTAB_LINK_PRIV_H_

#include <xdc/std.h>
#include <ti/psp/iss/drivers/alg/vstab/alg_vstab.h>
#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/vstabLink.h>

#define VSTAB_LINK_OBJ_MAX		(1)

typedef struct {
	IALG_Handle vsHandle;

} VstabLink_algObj;

typedef struct {
    Utils_TskHndl tsk;
    VstabLink_CreateParams createArgs;
    UInt32 receviedFrameCount;
    VstabLink_algObj algObj;

} VstabLink_Obj;

/* Function prototypes */

/* ===================================================================
 *  @func     VstabLink_algCreate                                               
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
Int32 VstabLink_algCreate(VstabLink_Obj *pObj);

/* ===================================================================
 *  @func     VstabLink_algDelete                                               
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
Int32 VstabLink_algDelete(VstabLink_Obj * pObj);

Int32 VstabLink_algProcess(VstabLink_Obj * pObj,
                           VstabLink_processPrm *pProcessPrm);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
