/** ==================================================================
 *  @file   alg_ti_swosd.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/sw_osd/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _IMX_SWOSD_OSD_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IMX_SWOSD_OSD_H_

#include <ti/psp/iss/alg/swosd/inc/alg_ti_swosd.h>
/* ===================================================================
 *  @func     SWOSD_winDrawHandle                                               
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
int SWOSD_imxDrawHandle(SWOSD_Hndl * Hndl, short *mainWinAddrY,
                        short *mainWinAddrUV, int streamID);
						
int SWOSD_imxDrawHandle1(SWOSD_Hndl * Hndl, short *mainWinAddrY,
                        short *mainWinAddrUV, int streamID);
						
int SWOSD_imxCreate(int streamID, int isTiled, int offset);

int SWOSD_imxUpdateLineOffset(int streamID, int offset);

int SWOSD_imxDelete(int streamID);

int SWOSD_imxPrepUpdateDynamicParamsAndTrigger(SWOSD_Hndl * Hndl, short *mainWinAddrY,
                        short *mainWinAddrUV, int streamId);
						
int SWOSD_collateWindows(int streamID, SWOSD_Hndl * Hndl);						
						

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _SWOSD_OSD_H_ */
