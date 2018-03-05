/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup MCFW_API
    \defgroup MCFW_VDIS_API McFW Video Display (VDIS) API

    @{
*/

/**
    \file ti_vdis.h
    \brief McFW Video Display (VDIS) API
*/

#ifndef __TI_VDIS_H__
#define __TI_VDIS_H__


#ifdef __cplusplus
extern "C" {
#endif
#include "ti_vdis_common_def.h"
#include "ti_vsys.h"

/* =============================================================================
 * Defines
 * =============================================================================
 */

/** Maximum Video Display channels */
#define VDIS_CHN_MAX            (32)

/** Invalid channel ID for a mosaic. Used to disable a window on the display */
#define VDIS_CHN_INVALID        (0xFF)

/** \brief Bitmask for HDMI VENC */
#define VDIS_VENC_HDMI          (0x1u)
/** \brief Bitmask for HDCOMP VENC */
#define VDIS_VENC_HDCOMP        (0x2u)
/** \brief Bitmask for DVO2 VENC */
#define VDIS_VENC_DVO2          (0x4u)
/** \brief Bitmask for SD VENC. */
#define VDIS_VENC_SD            (0x8u)


/* =============================================================================
 * Enums
 * =============================================================================
 */

/**
    \brief Display Device ID

    Association of display device ID to HDMI or HDDAC is done via VDIS_PARAMS_S
*/
typedef enum
{
    VDIS_DEV_HDMI   = 0,
    /**< Display 0 */

    VDIS_DEV_HDCOMP = 1,
    /**< Display 1 */

    VDIS_DEV_DVO2   = 2,
    /**< Display 2 */

    VDIS_DEV_SD     = 3,
    /**< Display 3 */

} VDIS_DEV;

/**
    \brief Display Type
*/
typedef enum
{
    VDIS_TYPE_SDDAC_COMPOSITE_CVBS  = 0,
    /**< Analog Composite - SDTV */

    VDIS_TYPE_DVO2_BT1120_YUV422 = 1,
    /**< Digital 16-bit embedded sync mode via DVO2 - HDTV */

    VDIS_TYPE_DVO1_BT1120_YUV422 = 2,
    /**< Digital 16-bit embedded sync mode via DVO1 - HDTV */

    VDIS_TYPE_HDDAC_VGA_RGB = 3,
    /**< Analog Component YPbPr - HDTV */

    VDIS_TYPE_HDDAC_COMPONENT_YPBPR = 4,
    /**< Analog Component RGB VESA Standard - HDTV */

    VDIS_TYPE_HDMI_RGB    = 5,
    /**< Digital HDMI Standard (On-Chip HDMI) - HDTV */

} VDIS_TYPE_E;

/**
    \brief TrickPlay Rate
*/
typedef enum
{
    VDIS_AVSYNC_1X = 1,
    /**< Normal Speed Playback */

    VDIS_AVSYNC_2X = 2,
    /**< 2X Speed Playback */

    VDIS_AVSYNC_4X = 4,
    /**< 4X Speed Playback */

    VDIS_AVSYNC_8X = 8,
    /**< 8X Speed Playback */

    VDIS_AVSYNC_16X = 16,
    /**< 16X Speed Playback */

    VDIS_AVSYNC_32X = 32,
    /**< 32X Speed Playback */

    VDIS_AVSYNC_HALFX = 100*(1/2),
    /**< 1/2X Speed Playback */

    VDIS_AVSYNC_QUARTERX = 100*(1/4),
    /**< 1/4X Speed Playback */

    VDIS_AVSYNC_IFRAMEONLY = 0,
    /**< 1/4X Speed Playback */


    VDIS_AVSYNC_MAX   = 100
    /**< Maximum Playback Rate */

}VDIS_AVSYNC;

/**
    \brief Color space conversion mode
*/
typedef enum
{
    VDIS_CSC_MODE_SDTV_VIDEO_R2Y = 0,
    /**< Select coefficient for SDTV Video */

    VDIS_CSC_MODE_SDTV_VIDEO_Y2R,
    /**< Select coefficient for SDTV Video */

    VDIS_CSC_MODE_SDTV_GRAPHICS_R2Y,
    /**< Select coefficient for SDTV Graphics */

    VDIS_CSC_MODE_SDTV_GRAPHICS_Y2R,
    /**< Select coefficient for SDTV Graphics */

    VDIS_CSC_MODE_HDTV_VIDEO_R2Y,
    /**< Select coefficient for HDTV Video */

    VDIS_CSC_MODE_HDTV_VIDEO_Y2R,
    /**< Select coefficient for HDTV Video */

    VDIS_CSC_MODE_HDTV_GRAPHICS_R2Y,
    /**< Select coefficient for HDTV Graphics */

    VDIS_CSC_MODE_HDTV_GRAPHICS_Y2R,
    /**< Select coefficient for HDTV Graphics */

    VDIS_CSC_MODE_MAX,
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */

    VDIS_CSC_MODE_NONE = 0xFFFFu
    /**< Used when coefficients are provided */

}VDIS_COLOR_SPACE_MODE_E;

/**
 * \brief DVO Format
 */
typedef enum
{
    VDIS_DVOFMT_SINGLECHAN = 0,
    /**< Ouput data format is single channel with embedded sync */
    VDIS_DVOFMT_DOUBLECHAN,
    /**< Output data format is dual channel with embedded sync */
    VDIS_DVOFMT_TRIPLECHAN_EMBSYNC,
    /**< Output data format is tripple channel with embedded sync */
    VDIS_DVOFMT_TRIPLECHAN_DISCSYNC,
    /**< Ouptut data format is triple channel with discrete sync */
    VDIS_DVOFMT_DOUBLECHAN_DISCSYNC,
    /**< Output data format is dual channel with discrete sync */
    VDIS_DVOFMT_MAX
    /**< This should be the last Enum */
} VDIS_DIGITAL_FMT_E;

/**
 * \brief Analog Format
 */
typedef enum
{
    VDIS_A_OUTPUT_COMPOSITE = 0,
    /**< Analog output format composite */
    VDIS_A_OUTPUT_SVIDEO,
    /**< Analog output format svideo */
    VDIS_A_OUTPUT_COMPONENT,
    /**< Analog output format component */
    VDIS_A_OUTPUT_MAX
} VDIS_ANALOG_FMT_E;

/**
 * \brief Signal polarity
 */
typedef enum
{
    VDIS_POLARITY_ACT_HIGH = 0,
    /**< Signal polarity Active high */
    VDIS_POLARITY_ACT_LOW = 1,
    /**< Signal polarity Active low */
    VDIS_POLARITY_MAX = 2,
    /**< Signal polarity Active low */
}VDIS_SIGNAL_POLARITY_E;

/**
 * \brief VENC Controls
 */
typedef enum
{
    VDIS_COLOR_CTRL_BRIGHTNESS = 0,
    /**< Brightness control */
    VDIS_COLOR_CTRL_CONTRAST,
    /**< Contrast control */
    VDIS_COLOR_CTRL_SATURATION,
    /**< Saturation control */
    VDIS_COLOR_CTRL_HUE,
    /**< Saturation control */
    VDIS_COLOR_CTRL_MAX
    /**< Control Max */
} VDIS_CONTROLS_E;

/* =============================================================================
 *  Structures
 * =============================================================================
 */

/**
    \brief Screen Buffer Information
*/
typedef struct VDIS_SCREEN_BUF_INFO_S
{
    Void           *bufVirtAddr;
    /**< Buffer user virtual address */

    Void           *bufPhysAddr;
    /**< Buffer system physical address */

    UInt32          pitch[2];
    /**< Buffer pitch, in bytes, for Y and C planes (YUV420SP) or YC plane (YUV422I) */

    UInt32          width;
    /**< Width of data in pixels */

    UInt32          height;
    /**< Height of data in lines */

    VIDEO_FORMATE_E dataFormat;
    /**< YUV Data Format */

} VDIS_SCREEN_BUF_INFO_S;


/**
    \brief VDIS set HMP forwading & crop parameters - MPSCLR & SWMS link level
*/
typedef struct VDIS_MPSCLR_SET_CROP_INFO_S
{
    /**< output channel Id */
    UInt32 cropStartX;
    /**< ouput crop startX offset, This should be alinged to 16 */
    UInt32 cropStartY;
    /**< ouput crop startY offset, This should be alinged to 16 */
    UInt32 cropWidth;
    /**< ouput width, max supported crop width is 1920 */
    UInt32 cropHeight;
    /**< ouput height, max supported crop height is 1080 */
} VDIS_MPSCLR_SET_CROP_INFO_S;


/* =============================================================================
 * APIs
 * =============================================================================
 */

/**
    \brief Set defaults for VDIS_PARAMS_S

    By default display parameters are setup for 2 HD (Mosaic) + 1 SD (Non-Moisac) Display mode

    \param  pContext    [OUT] VDIS_PARAMS_S filled with default parameters
*/
Void  Vdis_params_init(VDIS_PARAMS_S * pContext);

/**
    \brief Setup Tied device configuration in display sub-system

    MUST be called before calling Vdis_init

    \param   [IN] Device1, Device2, pContext Display configuration

    \return ERROR_NONE on success
*/

Int32 Vdis_tiedVencInit(VDIS_DEV VdDevId1, VDIS_DEV VdDevId2, VDIS_PARAMS_S * pContext);

/**
    \brief Initialize display sub-system

    MUST be called before calling any other VDIS API

    \param  pContext    [IN] Display configuration

    \return ERROR_NONE on success
*/
Int32 Vdis_init(VDIS_PARAMS_S * pContext);

/**
    \brief De-Initialize display sub-system

    MUST be called after Vdis_stop() and Vsys_delete()
*/
Void  Vdis_exit();

/**
    \brief Start display sub-system

    MUST be called after calling Vdis_init() and Vsys_create()

    \return ERROR_NONE on success
*/
Int32 Vdis_start();

/**
    \brief Stop display sub-system

    MUST be called before calling Vsys_delete()

    \return ERROR_NONE on success
*/
Int32 Vdis_stop();

/**
    \brief Set or change mosaic information on a given display

    Use this API to change or switch mosaic on a given display

    \param vdDevId              [IN] Device on which to apply this API
    \param psVdMosaicParam      [IN] Mosaic Parameters

    \return ERROR_NONE on success
*/
Int32 Vdis_setMosaicParams(VDIS_DEV vdDevId, VDIS_MOSAIC_S *psVdMosaicParam);

/**
    \brief Get current mosaic information on a given display

    \param vdDevId              [IN] Device on which to apply this API
    \param psVdMosaicParam      [OUT] Returned current Mosaic Parameters

    \return ERROR_NONE on success
*/
Int32 Vdis_getMosaicParams(VDIS_DEV vdDevId, VDIS_MOSAIC_S *psVdMosaicParam);

/**
    \brief Channel window to channel mapping for current selected mosaic

    \param vdDevId       [IN] Device on which to apply this API
    \param usChnMap      [IN] Window to channel mapping for each window. Set to VDIS_CHN_INVALID is no channel is mapped to a window

    \return ERROR_NONE on success
*/
Int32 Vdis_setMosaicChn(VDIS_DEV vdDevId, VDIS_CHN * usChnMap);

/**
    \brief Disable the window to which the specified channel is mapped

    If the channel is not being shown on the display then this API will have no effect.

    \param vdDevId     [IN] Device on which to apply this API
    \param vdChId      [IN] Channel to disable on the display

    \return ERROR_NONE on success
*/
Int32 Vdis_disableChn(VDIS_DEV vdDevId, VDIS_CHN vdChId);

/**
    \brief Enable the window to which the specified channel is mapped

    If the channel is not being shown on the display then this API will have no effect.

    \param vdDevId     [IN] Device on which to apply this API
    \param vdChId      [IN] Channel to enable on the display

    \return ERROR_NONE on success
*/
Int32 Vdis_enableChn(VDIS_DEV vdDevId, VDIS_CHN vdChId);

/**
 * \brief:
 *      Enable all the display channel
 * \input:
 *      vdDevId         -- Device Id
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS        --  while success
*       ERROR_CODE          --  refer for err defination
*/

Int32 Vdis_enableAllChn(VDIS_DEV vdDevId);
/**
 * \brief:
 *      Diable all the display channel with show blank frame
 * \input:
 *      vdDevId         -- Device Id
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS        --  while success
*       ERROR_CODE          --  refer for err defination
*/

Int32 Vdis_disableAllChn(VDIS_DEV vdDevId);


/**
    \brief Check if there window to which the specified channel is mapped and is enabled

    If the channel is not being shown on the display then this API will return FALSE.

    \param vdDevId     [IN] Device on which to apply this API
    \param vdChId      [IN] Channel to check for enable

    \return TRUE: channel is enabled on the display, FALSE: channel is disable for display
*/
Bool  Vdis_isEnableChn(VDIS_DEV vdDevId, VDIS_CHN vdChId);

/**
    \brief Set playback control configuration to Avsync logic

    decoder output frame rate is controlled based on trickplay setting.
    this call is used to make avsync logic to sync with new frame rate.

    \param vcChnId  [IN] decode channel ID

*/
void Vdis_setAvsyncConfig(VDIS_CHN vdispChnId, VDIS_AVSYNC speed);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_setParamDevice(VDIS_DEV VdDevId, VDIS_DEV_PARAM_S *psVdDevParam);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_getParamDevice(VDIS_DEV VdDevId, VDIS_DEV_PARAM_S *psVdDevParam);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_enableDevice(VDIS_DEV vdDevId);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_disableDevice(VDIS_DEV vdDevId);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_pauseMosaic(VDIS_DEV vdDevId);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_resumeMosaic(VDIS_DEV vdDevId);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_stepMosaic(VDIS_DEV vdDevId);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_pauseChn(VDIS_DEV vdDevId, VDIS_CHN VdChnId);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_resumeChn(VDIS_DEV vdDevId, VDIS_CHN VdChnId);

/**
    \brief Coming soon .. NOT SUPPORED in this release
*/
Int32 Vdis_stepChn(VDIS_DEV vdDevId, VDIS_CHN VdChnId);

/**
    \brief Send filled video buffers to framework for display

    User calls this API to put full video frames for display

    \param pFrameBufList    [OUT]  List of video frames to be displayed

    \return ERROR_NONE on success
*/
Int32 Vdis_putFullVideoFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);

/**
    \brief Give displayed video frames back to the application

    Buffers that are were previously put by Vdis_putFullVideoFrames can be
    freed back to the framework by invoking this API.

    \param pFrameBufList [IN]   List of video frames

    \return ERROR_NONE on success
*/
Int32 Vdis_getEmptyVideoFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList, UInt32 timeout);

/**
    \brief Get info on number of channels and each channels property from
           decode framework

    \param channelListInfo [OUT]   Structure pointer that will be populated
                                   with input channel info by this function

    \return ERROR_NONE on success
*/
Int32 Vdis_getChannelInfo(VIDEO_CHANNEL_LIST_INFO_S *channelListInfo);

/**
    \brief Gets display resolution for Venc

    \param devId   deviceId

    \return ERROR_NONE on success
*/
VSYS_VIDEO_STANDARD_E Vdis_getResolution(VDIS_DEV devId);

/**
 * \brief:
 *      Switch the queue from which frame are displayed
 * \input:
 *      vdDevId                -- Device id
 *      queueId                -- QueueID to switch to
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS          --    while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vdis_switchActiveQueue(VDIS_DEV vdDevId, UInt32 queueId);

/**
 * \brief:
 *      Switch the channel Id for SDTV live bypass path
 * \input:
 *      vdDevId                -- Device id
 *      chId                -- chID to switch to
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS          --    while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vdis_switchSDTVChId(VDIS_DEV vdDevId, UInt32 chId);


/**
 * \brief:
 *      Switch the queue from which frame are displayed
 * \input:
 *      vdDevId                -- Device id
 *      channelId              -- channelId to switch to
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS          --    while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vdis_switchActiveChannel(VDIS_DEV vdDevId, UInt32 channelId);

/**

    \brief Sets display resolution at run time

    \param devId   deviceId

    \return ERROR_NONE on success
*/


Int32 Vdis_setResolution(VDIS_DEV devId, UInt32 resolution);

/**
  * \brief:
  *     get channel input information from win ID
  * \input:
  *     vdDevId     -- Mosaic id
  *     winId       -- win ID which we want to get the channel information
  * \output:
  *     psChnlInfo  -- channel input infomation 
  * \return:
  *     Always return success
  */

Int32 Vdis_getChnlInfoFromWinId(VDIS_DEV vdDevId, UInt32 winId,WINDOW_S * psChnlInfo);

/**
  * \brief:
  *     set channel crop settings from window ID
  * \input:
  *     vdDevId     -- Mosaic id
  *     winId       -- win ID which we want to get the channel information
  *     cropParam   -- crop settings to apply 
  * \return:
  *     Always return success
  */

Int32 Vdis_SetCropParam(VDIS_DEV vdDevId, UInt32 winId,WINDOW_S cropParam);

/**
    \brief Stops display driver

    \param devId   deviceId

    \return ERROR_NONE on success
*/

Int32 Vdis_stopDrv(VDIS_DEV devId);
Int32 Vdis_stopDevAll();

/**
    \brief Starts display driver

    \param devId   deviceId

    \return ERROR_NONE on success
*/

Int32 Vdis_startDrv(VDIS_DEV devId);
Int32 Vdis_startDevAll();


/**
    \brief Executes sysfs command

    This API will create and execute a sysfs command based on arguments passed
    This API assumes first argument is a string and other arguments followed
    by the string are integers

    \param numAgrs  Number of arguments in sysfs command
    \param ...      Variable arguments

*/
Int32 Vdis_sysfsCmd(UInt32 numArgs, ...);

/**
 * \brief:
 *      Function to enable output crop for a given channel at MPSCLR level
 * \input:
 *      vdDevId             -- Mosaic id
 *      chId                -- channel Id
 *      cropParams          -- VDIS set crop parameters - MPSCLR level
 *      enable              -- Set to TRUE to enable
 *                             HMP forward(MPSCLR) & Crop(SWMS)
 *                          -- Set to FALSE to disable HMP forward & Crop
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS    --  while success
*       ERROR_CODE          --  refer for err defination
*/

Int32 Vdis_MpsclrSetFwdCrop(VDIS_DEV vdDevId, UInt32 chId,
                            VDIS_MPSCLR_SET_CROP_INFO_S *cropParams, 
                            Bool enable);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __TI_VDIS_H__ */

/* @} */
