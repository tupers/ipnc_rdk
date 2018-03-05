/** ==================================================================
 *  @file   vstabLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains Vstab Link API.        
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
    \defgroup VSTAB_LINK_API Vstab Link API

    VSTAB link applies Video Stabilization (VS) algorithm on the captured Video frame.
	VS algorithm works on the Boundary Signal Calculator (BSC) Data of current and previous frames.
	BSC H/W module is present in the IPIPE.

    @{
*/

/**
    \file vstabLink.h
    \brief Vstab Link API
*/

#ifndef _VSTAB_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _VSTAB_H_

#include <mcfw/interfaces/link_api/system.h>

/**
    \brief Vstab Link process parameters
*/
typedef struct {
    UInt32 bscBufferCurrV;
	/**< Memory Address where the vertical BSC data for the current frame is present */
	
    UInt32 bscBufferCurrH;
	/**< Memory Address where the horizontal BSC data for the current frame is present */

    UInt32 bscBufferPrevV;
	/**< Memory Address where the vertical BSC data for the previous frame is present */

    UInt32 bscBufferPrevH;
	/**< Memory Address where the horizontal BSC data for the previous frame is present */	
	
	UInt32 startX;
	/**< X co-ordinate of stabilized frame */	
	
	UInt32 startY;
	/**< Y co-ordinate of stabilized frame */		

} VstabLink_processPrm;

/**
    \brief Vstab Link create parameters
*/
typedef struct {
    UInt32 totalFrameHeight;
    /**< VS input frame height */
	
	UInt32 totalFrameWidth;
	/**< VS input frame width */
	
} VstabLink_CreateParams;


/**
 *  @func     VstabLink_init                                               
 *                                                                    
 *  @desc     Function does the Vstab link register and init
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
Int32 VstabLink_init();

/**
 *  @func     VstabLink_deInit                                               
 *                                                                    
 *  @desc     Function does the Vstab link de-register and de-init                           
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
Int32 VstabLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
