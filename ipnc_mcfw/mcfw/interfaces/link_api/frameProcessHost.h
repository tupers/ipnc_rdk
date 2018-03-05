
/** ==================================================================
 *  @file   FrameProcessLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains Img Link API.                 
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
    \defgroup IMG_LINK_API Img Link API

	IMG Link imprints the TI logo,Date/Time,Codec details and User data onto Video streams.
	Any change in the data to be imprinted is checked at an interval of Frames Per Second.
	
    @{
*/

/**
    \file frameProcessHost.h
    \brief Link API
*/

#ifndef _FRAMEPROCESSHOST_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _FRAMEPROCESSHOST_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief Commands handled by the IPCFRAMEPROCESS link */

#define SAVE_FRAME_IMG			(0xA000)
#define ENABLE_ENCODE_IMG		(0xA001)
#define DISABLE_ENCODE_IMG		(0xA002)
#define ENABLE_SAVE_ERRIMG		(0xA003)
#define DISABLE_SAVE_ERRIMG		(0xA004)

/**
    \brief FrameProcessHostLink link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information */

    System_LinkOutQueParams outQueParams;
    /**< Output queue information */

} FrameProcessHostLink_CreateParams;

/**
 *  @func     ImgLink_init                                               
 *                                                                    
 *  @desc     Function does the Img link register and init                          
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
Int32 FrameProcessHostLink_init();

/**
 *  @func     FrameProcessHostLink_deInit                                               
 *                                                                    
 *  @desc     Function does the Rvm link de-register and de-init                             
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
Int32 FrameProcessHostLink_deInit();

Int32 FrameProcessLink_getNetFrame(FrameNet_Buf * pFrame);
Int32 FrameProcessLink_ReleaseNetFrame(FrameNet_Buf * pFrame);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
