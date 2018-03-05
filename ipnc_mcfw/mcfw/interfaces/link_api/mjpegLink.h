/** ==================================================================
 *  @file   mjpegLink.h
 *
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/
 *
 *  @desc   This  File contains MJPEG SIMCOP Encoder Link API.
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
    \defgroup MJPEG_LINK_API MJPEG Encoder Link API

    MJPEG SIMCOP Encode Link can be used to take input from a link and after doing
    MJPEG Encode output the frames to output queue.

    @{
*/

/**
    \file mjpegLink.h
    \brief MJPEG Video Encoder Link API
*/

#ifndef _MJPEG_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MJPEG_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

#define IS_PRINT_NEEDED         (0)
#define IS_PRINT_NEEDED_MJPEG_LINK         (0)

/** \brief Number of encoder output queue */
#define MJPEG_LINK_MAX_OUT_QUE    (1)

/** \brief SIMCOP Scaler output ID */
#define MJPEG_LINK_OUT_QUE_ID     (0)

/** \brief Max SIMCOP channels per link */
#define MJPEG_LINK_MAX_CH         (2)

/** \brief Max number of buffer polls in ENC link */
#define MJPEG_LINK_MAX_BUF_ALLOC_POOLS        (4)
/**
    \brief Link CMD: Configure Video Encoder params
*/
#define MJPEG_LINK_CMD_GET_CODEC_PARAMS       (0x6002)
#define MJPEG_LINK_CMD_SET_CODEC_FPS          (0x6004)

#define MJPEG_LINK_CMD_SET_CODEC_QP	       	(0x6008)

/** \Enc command to enable channel */
#define MJPEG_LINK_CMD_ENABLE_CHANNEL          (0x600C)
/** \Enc command to disable channel */
#define MJPEG_LINK_CMD_DISABLE_CHANNEL         (0x600D)
/** \Enc command to print channel level statistics */
#define MJPEG_LINK_CMD_PRINT_STATISTICS       (0x600F)

/**
    \brief Simcop link channel dynamic get dynamic parameters

    Defines encoder dynamic parameters that can be probed dynamically
    on a per channel basis for the encode link
*/
typedef struct MjpegLink_GetDynParams {
    UInt32 chId;
    /**< Simcopoder channel number */
    UInt32 inputWidth;
    /**< Encoder input width */
    UInt32 inputHeight;
    /**< Encoder input height */
    UInt32 baseFps;
    UInt32 targetFps;
    /**< Modified encoder Frames per second (FPS) value in fps x 1000 format */
    UInt32 mjpegQp;

} MjpegLink_GetDynParams;

/**
    \brief Simcop link channel dynamic set config params

    Defines encoder FPS parameters that can be changed dynamically
    on a per channel basis for the encode link
*/
typedef struct MjpegLink_ChFpsParams {
    UInt32 chId;
    /**< Encoder channel number */
    UInt32 baseFps;
    UInt32 targetFps;
    /**< Modified encoder Frames per second (FPS) value in fps x 1000 format */
} MjpegLink_ChFpsParams;

typedef struct MjpegLink_ChQpParams {
    UInt32 chId;
    /**< Encoder channel number */
    UInt32 mjpegQp;
    /**< Modified encoder Frames per second (FPS) value in fps x 1000 format */
} MjpegLink_ChQpParams;

/**
    \brief Simcop link channel info
*/
typedef struct {
    UInt32 chId;
    /**< Encoder channel number */
} MjpegLink_ChannelInfo;

/**
    \brief Simcop link channel dynamic params

    Defines those parameters that can be changed dynamically on a per channel
    basis for the encode link
*/
typedef struct MjpegLink_ChDynamicParams {
    UInt32 mjpegQp;
    UInt32 skipFrameFactor;
} MjpegLink_ChDynamicParams;

typedef struct MjpegLink_ChCreateParams {
    UInt32 format;
    /**< Coding type @sa IVIDEO_Format */
    Int32 profile;
    /**< Codec profile */
    Int32 dataLayout;
    /**< Video buffer layout */
    MjpegLink_ChDynamicParams defaultDynamicParams;
} MjpegLink_ChCreateParams;

/**
    \brief Simcop link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams outQueParams;
    /**< Output queue information. */
    MjpegLink_ChCreateParams chCreateParams[MJPEG_LINK_MAX_CH];
    /**< Encoder link channel create params */
    UInt32 numBufPerCh[MJPEG_LINK_MAX_BUF_ALLOC_POOLS];
    /**< Number of buffer to allocate per channel per buffr pool */
} MjpegLink_CreateParams;

/**
 *  @func     MjpegLink_init
 *
 *  @desc     Function does the MJPEG link register and init                          
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
Int32 MjpegLink_init();

/**
 *  @func     MjpegLink_deInit
 *
 *  @desc     Function does the MJPEG link de-register and de-init                             
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
Int32 MjpegLink_deInit();

/**
 *  @func     MjpegLink_CreateParams_Init
 *
 *  @desc     Function does the initialization of create params of
 *            MJPEG Encoder Link
 *                                                                    
 *  @modif    
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            pPrm                                         
 *            pointer to the create params to be initialized                                    
 *                                                                    
 *  @outputs  pPrm                                         
 *            pointer of the create params                                    
 *                                                                    
 *  @return   None                  
 *
 */                                                                   
static inline void MjpegLink_CreateParams_Init(MjpegLink_CreateParams * pPrm)
{
    UInt32 i;

    // memset(pPrm, 0, sizeof(*pPrm));

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;

    /* when set 0, encoder will take default value based on system defined
     * default on BIOS side */
    for (i = 0; i < MJPEG_LINK_MAX_BUF_ALLOC_POOLS; i++)
    {
        pPrm->numBufPerCh[i] = 0;
    }
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
