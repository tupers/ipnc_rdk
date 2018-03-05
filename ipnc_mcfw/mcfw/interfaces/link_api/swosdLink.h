/** ==================================================================
 *  @file   swosdLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains Swosd Link API.                 
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
    \defgroup SWOSD_LINK_API Swosd Link API

	SWOSD Link imprints the TI logo,Date/Time,Codec details and User data onto Video streams.
	Any change in the data to be imprinted is checked at an interval of Frames Per Second.
	
    @{
*/

/**
    \file swosdLink.h
    \brief Swosd Link API
*/

#ifndef _SWOSD_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SWOSD_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

#define SWOSDLINK_MAX_STR_LENGTH		(32)

#define SWOSDLINK_CMD_GUIPRM			(0x8001)
#define SWOSDLINK_CMD_BR				(0x8002)
#define SWOSDLINK_CMD_RC				(0x8003)
#define SWOSDLINK_CMD_FR				(0x8004)
#define SWOSDLINK_CMD_DATETIMEPOSN		(0x8005)
#define SWOSDLINK_CMD_DATETIME			(0x8006)
#define SWOSDLINK_CMD_VASTRING			(0x8007)
#define SWOSDLINK_CMD_HISTENABLE		(0x8008)


/**
    \brief Swosd link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information */

    System_LinkOutQueParams outQueParams;
    /**< Output queue information */

} SwosdLink_CreateParams;

/**
    \brief Swosd GUI parameters
*/
typedef struct {
    UInt32 streamId;
    /**< Stream Id */

    UInt32 transparencyEnable;
    /**< Transparency Enable */

    UInt32 dateEnable;
    /**< Date Enable */

    UInt32 timeEnable;
    /**< Time Enable */

    UInt32 logoEnable;
    /**< Logo Enable */

    UInt32 logoPos;
    /**< Logo Position */

    UInt32 textEnable;
    /**< Text Enable */

    UInt32 textPos;
    /**< Text Position */

    UInt32 detailedInfo;
    /**< Detailed Info */

    UInt8 usrString[64];
    /**< User String */

    UInt32 bitRate;
    /**< Codec Bit Rate */

    UInt32 rateControl;
    /**< Codec Rate Control */

    UInt32 frameRate;
    /**< Codec Frame Rate */

    UInt32 dateFormat;
    /**< Date format */

    UInt32 datePos;
    /**< Date position */

    UInt32 timeFormat;
    /**< Time format */

    UInt32 timePos;
    /**< Time position */

    UInt32 encFormat;
    /**< Encoder format */

    UInt32 histEnable;
    /**< Histogram Enable */
	
} SwosdLink_GuiParams;

/**
    \brief Swosd Date Time params
*/
typedef struct {
    UInt32 year;
    /**< Year */

    UInt32 month;
    /**< Month */

    UInt32 day;
    /**< Day */

    UInt32 hour;
    /**< Hour */

    UInt32 min;
    /**< Minute */

    UInt32 sec;
    /**< Second */

} SwosdLink_dateTime;

/**
    \brief Swosd VA str params
*/
typedef struct {
    UInt32 disFlag;
	/**<display flag */
	
	UInt8 vaString[SWOSDLINK_MAX_STR_LENGTH];
	/**< VA string */
	
} SwosdLink_vaStrPrm;

/**
 *  @func     SwosdLink_init                                               
 *                                                                    
 *  @desc     Function does theSwosd link register and init                          
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
Int32 SwosdLink_init();

/**
 *  @func     SwosdLink_deInit                                               
 *                                                                    
 *  @desc     Function does the Swosd link de-register and de-init                             
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
Int32 SwosdLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
