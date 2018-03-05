/** ==================================================================
 *  @file   wdrLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains WDR Link API.           
 * 
 * ===================================================================
 *
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 *
 * ===================================================================*/

#ifndef _WDR_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _WDR_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief WDR default output queue ID */
#define WDR_LINK_OUT_QUE_ID     (0)

/** \brief Max WDR channels per link */
#define WDR_LINK_MAX_CH         (1)

#define WDR_LINK_CMD_CHANGE_MODE 0x8001

typedef enum
{
	WDR_DECOMPRESS_ONLY = 0,
	WDR_DECOMPRESS_ENHANCE_LOW = 1,
	WDR_DECOMPRESS_ENHANCE_MID = 2,
	WDR_DECOMPRESS_ENHANCE_HIGH = 3,
} WdrLink_Mode;

/**
    \brief WDR link channel dynamic parameters

    Defines filtering dynamic parameters that can be changed and probed dynamically
    on a per channel basis for the WDR link
*/
typedef struct WdrLink_ChDynamicParams {
	WdrLink_Mode wdrMode;
} WdrLink_ChDynamicParams;

/**
    \brief WDR link channel static parameters

    Defines filtering static parameters that can be probed dynamically
    on a per channel basis for the WDR link
*/
typedef struct WdrLink_ChStaticParams {
    UInt32 computeWidth;
    UInt32 computeHeight;
    UInt32 outputStride;
} WdrLink_ChStaticParams;

/**
    \brief WDR link channel create parameters
*/
typedef struct WdrLink_ChCreateParams {
    WdrLink_ChStaticParams sParams;
    WdrLink_ChDynamicParams dParams;
} WdrLink_ChCreateParams;

/**
    \brief WDR link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams outQueParams;
    /**< Output queue information. */
    WdrLink_ChCreateParams chCreateParams[WDR_LINK_MAX_CH];
} WdrLink_CreateParams;

/**
 *  @func     WdrLink_init                                               
 *                                                                    
 *  @desc     Function does the WDR link register and init
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
Int32 WdrLink_init();

/**
 *  @func     WdrLink_deInit                                               
 *                                                                    
 *  @desc     Function does the WDR link de-register and de-init                           
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
Int32 WdrLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
