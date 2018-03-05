/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup SWMS_LINK_API Software Mosaic Link API

    @{
*/

/**
    \file swMsLink.h
    \brief Software Mosaic Link API
*/

#ifndef _SYSTEM_SW_MS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_SW_MS_H_

#include <mcfw/interfaces/link_api/system.h>



#define SYSTEM_SW_MS_MAX_INST                (4)

#define SYSTEM_SW_MS_SC_INST_DEIHQ_SC        (1)
#define SYSTEM_SW_MS_SC_INST_DEI_SC          (2)
#define SYSTEM_SW_MS_SC_INST_VIP0_SC         (3)
#define SYSTEM_SW_MS_SC_INST_VIP1_SC         (4)
#define SYSTEM_SW_MS_SC_INST_SC5             (5)
#define SYSTEM_SW_MS_SC_INST_DEIHQ_SC_NO_DEI (6)
#define SYSTEM_SW_MS_SC_INST_DEI_SC_NO_DEI   (7)



    #define SYSTEM_SW_MS_MAX_WIN           (36)
    #define SYSTEM_SW_MS_MAX_CH_ID         (64)
#define SYSTEM_SW_MS_ALL_CH_ID               (SYSTEM_SW_MS_MAX_CH_ID + 1)

#ifdef TI_816X_BUILD
    #define SYSTEM_SW_MS_MAX_WIN_PER_SC        (18)
#endif

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    #define SYSTEM_SW_MS_MAX_WIN_PER_SC        (16)
#endif

#define SYSTEM_SW_MS_INVALID_ID        (0xFF)
#define SYSTEM_SW_MS_MAX_LAYOUTS	   (20)

#define SYSTEM_SW_MS_INVALID_INPUT_QUE_LEN   (0xFF)
#define SYSTEM_SW_MS_DEFAULT_INPUT_QUE_LEN   (0)


/**
    \brief Link CMD: Switch channel that is being scaled by scaler

    \param SwMsLink_LayoutParams * [IN] Command parameters
*/
#define SYSTEM_SW_MS_LINK_CMD_SWITCH_LAYOUT        (0x8000)
#define SYSTEM_SW_MS_LINK_CMD_GET_LAYOUT_PARAMS    (0x8001)
#define SYSTEM_SW_MS_LINK_CMD_PRINT_STATISTICS     (0x8002)
#define SYSTEM_SW_MS_LINK_CMD_GET_INPUT_CHNL_INFO  (0x8003)
#define SYSTEM_SW_MS_LINK_CMD_SET_CROP_PARAM        (0x8004)
#define SYSTEM_SW_MS_LINK_CMD_PRINT_BUFFER_STATISTICS        (0x8005)

/**
    \brief SWMS cmd to Switch layout from current one to a new layout
    \param SwMsLink_LayoutParams * [IN] Command parameters
*/
#define SYSTEM_SW_MS_LINK_CMD_FLUSH_BUFFERS            (0x8006)
typedef struct
{
    UInt32 channelNum;
    /* channel associated with this window */

    UInt32 bufAddrOffset[2];

    UInt32 startX;
    UInt32 startY;
    UInt32 width;
    UInt32 height;

    UInt32 bypass; /* TRUE/FALSE - Low Cost Scaling/DEI enable disable */

} SwMsLink_LayoutWinInfo;


typedef struct
{
    Uint32  winId;
    Uint32  startX;
    Uint32  startY;
    Uint32  width;
    Uint32  height;
} SwMsLink_WinInfo;

/*
 Make sure windows are specified in the order of overlap from
 background to foreground
*/
typedef struct
{
    UInt32 onlyCh2WinMapChanged;
    /* FALSE: Layout is changed
         TRUE: Layout is not changed only Channels mapped to windows are changed
     */

    UInt32 numWin;
    /**< Display Layout Number of Windows */

    SwMsLink_LayoutWinInfo winInfo[SYSTEM_SW_MS_MAX_WIN];
    /**< Display Layout Individual window coordinates */

    UInt32  ch2WinMap[SYSTEM_SW_MS_MAX_CH_ID];
    /**< Display Layout Channel to Window  Mapping - NOT TO BE SET BY USER */

    UInt32 outputFPS;
    /**< Rate at which output frames should be generated,
         should typically be equal to the display rate

         Example, for 60fps display, outputFPS should be 60
    */

} SwMsLink_LayoutPrm;

/**
    \brief Link create parameters
*/
typedef struct
{
    UInt32      numSwMsInst;
    /**< number of scaler instance in one sw mosaic */

    UInt32      swMsInstId[SYSTEM_SW_MS_MAX_INST];
    /**< scaler ID of each scaler instance */

    UInt32      swMsInstStartWin[SYSTEM_SW_MS_MAX_INST];
    /**< start win ID for each scaler instance */

    System_LinkInQueParams  inQueParams;
    /**< input queue information */

    System_LinkOutQueParams outQueParams;
    /**< output queue information */

    SwMsLink_LayoutPrm    layoutPrm;

    UInt32 maxOutRes;
    /**< output resolution SYSTEM_RES_xxx */

    UInt32 lineSkipMode;
    /**< supported value: TRUE/FALSE */

    UInt32 enableLayoutGridDraw;
    /**< Enable/Disable Drawing of Layout specific Grids: TRUE/FALSE */

    UInt32 maxInputQueLen;
    /**< Maximum number of frames that can be queued at input of
     *   sw mosaic for each active window.
     *   If queue length exceeds this value, frames will be dropped.
    */

    UInt32 numOutBuf;
    /**<
        When set to 0 system default is used else
        user specified number of buffers is allocated */

    UInt32 enableOuputDup;
    /**<
        Flag to enable the SWMS ouput dup. This can be enabled only if the
        SWMS layout FPS is <= 30 and the displaylink FPS is 60
        This dup feature will be turned-off automatically in SWMS link if the
        layout FPS is > 30 */

    UInt32 enableProcessTieWithDisplay;
    /**<
        Flag to enable the SWMS doScaling process tie-up with dsiplay
        interrupt. Otherwise scaling is triggered by SWMS internal clock object
    */

    UInt32 outDataFormat;
    /**<
         Output Data format from the SWMS.
         It can be either SYSTEM_DF_YUV422I_YUYV or SYSTEM_DF_YUV420SP_UV
    */
} SwMsLink_CreateParams;

/**
 *  \brief SWMS link buffer flush parameters
 *
 *  Defines the structure to flush a channel of the SWMS link
 */
typedef struct SwMsLink_FlushParams
{
    UInt32  chNum;
    /**< Channel number */
    UInt32  holdLastFrame;
    /**< Flag to hold the last frame or not */
} SwMsLink_FlushParams;

/**
    \brief Dei link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 SwMsLink_init();

/**
    \brief Dei link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 SwMsLink_deInit();

static inline void SwMsLink_CreateParams_Init(SwMsLink_CreateParams *pPrm)
{
    memset(pPrm, 0, sizeof(*pPrm));
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
