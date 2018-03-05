/** ==================================================================
 *  @file   osdLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains OSD Link API.              
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
    \defgroup OSD_LINK_API OSD Link API

    OSD Link can be used to take input from a link and after doing OSD output
    the frames to output queue.

    OSD can have upto two outputs
    - Output from OSD scaler
    - Output from VIP scaler

    Each can be individually enabled/disabled during link create.

    Each frame output is put in its corresponding output queue.

    @{
*/

/**
    \file OSDLink.h
    \brief OSD Link API
*/

#ifndef _OSD_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _OSD_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

#define OSD_LINK_MAX_WINDOWS (10)
#define OSD_LINK_MAX_CH		 (16)
/**
    \brief Link CMD: Configure Osd params
*/
#define OSD_LINK_CMD_SET_CHANNEL_POSITION     (0x6001)
#define OSD_LINK_CMD_SET_CHANNEL_SIZE         (0x6002)
#define OSD_LINK_CMD_SET_CHANNEL_TRANSPARENCY (0x6003)
#define OSD_LINK_CMD_SET_CHANNEL_GLOBALALPHA  (0x6004)
#define OSD_LINK_CMD_SET_CHANNEL_ENABLE       (0x6005)

/**
  \brief Graphics window parameter's
*/
typedef struct {
    UInt8 *addr[2][2];                                     /* Y/C buf
                                                            * Ptr/yuv422
                                                            * interleaved ptr 
                                                            */

    UInt32 format;                                         // /<
                                                           // SWOSD_FORMAT_YUV422i 
                                                           // or
                                                           // SWOSD_FORMAT_RGB565
    UInt32 startX;                                         // /< in pixels,
                                                           // relative to
                                                           // start of video
                                                           // window, must be 
                                                           // multiple of 2
    UInt32 startY;                                         // /< in lines,
                                                           // relative to
                                                           // start of video
                                                           // window
    UInt32 width;                                          // /< in pixels,
                                                           // must be
                                                           // multiple of 4
    UInt32 height;                                         // /< in lines
    UInt32 lineOffset;                                     // /< in pixels,
                                                           // must be >=
                                                           // width, must be
                                                           // multiple of 4,
                                                           // recommended to
                                                           // be multiple of
                                                           // 32 for
                                                           // efficiency

    UInt32 globalAlpha;                                    // /< 8-bit global 
                                                           // Alpha Value,
                                                           // used only if
                                                           // Alpha window is 
                                                           // not enabled,
                                                           // set to 0 to
                                                           // disable alpha
                                                           // blend \n 0: Min 
                                                           // Alpha, show
                                                           // only video,
                                                           // 128: Max Alpha, 
                                                           // show only
                                                           // Graphics
    UInt32 transperencyEnable;                             // /< TRUE: enable 
                                                           // transperency,
                                                           // FALSE: disable
                                                           // transperency
    UInt32 enableWin;

} OsdLink_WindowPrm;

/**
    \brief Enc link channel dynamic set config params

    Defines encoder bitRate parameters that can be changed dynamically
    on a per channel basis for the encode link
*/
typedef struct OsdLink_ChWinParams {
    UInt32 chId;
    /**< OSD channel number */

    UInt32 numWindows;
    /**< Number of OSD windows per channel */

    OsdLink_WindowPrm winPrm[OSD_LINK_MAX_WINDOWS];
    /**< Modified encoder bit rate value in bits/sec format */
} OsdLink_ChWinParams;

/**
	\brief OSD channel create time configuration parameters
*/
typedef struct {
    UInt32 maxWidth;
    UInt32 maxHeight;
    /**< Set the max width and height of osd graphics window */
    OsdLink_ChWinParams chDeafultParams;
    /**< run time params for all channels. */
} OsdLink_ChCreateParams;

/**
    \brief OSD link create parameters
*/
typedef struct {
    /**< System configuration ID. */
    System_LinkInQueParams inQueParams;
    /**< Input queue information. */
    /**< No output queue as it is inplace overlay */
    OsdLink_ChCreateParams chCreateParams[OSD_LINK_MAX_CH];
    /**< create time params for all channels. */
} OsdLink_CreateParams;

/**
 *  @func     OsdLink_init                                               
 *                                                                    
 *  @desc     Function does the OSD link register and init                          
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
Int32 OsdLink_init();

/**
 *  @func     OsdLink_deInit                                               
 *                                                                    
 *  @desc     Function does the OSD link de-register and de-init                             
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
Int32 OsdLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
