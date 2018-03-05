/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2012 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup PROCESSOR_LINK_API
    \defgroup SYSTEM_M3VPSS_LINK_API Processor Link API: VPSS M3

    This link is used to perform initialization and deIntialization of
    Display controller

    The Link uses hdvpss API to control external video devices related
    to Display. The link holds system information and give VENC output control.

    @{
*/

/**
    \file systemLink_m3vpss.h
    \brief Processor Link API: VPSS M3
*/

#ifndef _SYSTEM_M3VPSS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_M3VPSS_H_

#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/systemLink_common.h>


/**
    \ingroup LINK_API_CMD
    \addtogroup SYSTEM_M3VPSS_API_CMD Processor Link API: VPSS M3 Control Commands

    @{
*/

/**
    \brief Link CMD: Reset video devices

    SUPPORTED in TI810X_DVR platforms where i2c is controlled from M3

    \param NONE
*/
#define SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES              (0xA000)

/**
    \brief Link CMD: Get platform information

    - cpuRev
    - boardId
    - baseBoardRev
    - dcBoardRev

    SUPPORTED in all platforms

    \param SystemVpss_PlatformInfo * [OUT] platform information
*/
#define SYSTEM_M3VPSS_CMD_GET_PLATFORM_INFO                (0xA001)

/**
    \brief Link CMD: Initialize display controller

    SUPPORTED in all platforms

    \param VDIS_PARAMS_S * [IN] Vdis module params
*/
#define SYSTEM_M3VPSS_CMD_GET_DISPLAYCTRL_INIT             (0xA002)

/**
    \brief Link CMD: DeInitialize display controller

    SUPPORTED in all platforms

    \param VDIS_PARAMS_S * [IN] Vdis module params
*/
#define SYSTEM_M3VPSS_CMD_GET_DISPLAYCTRL_DEINIT           (0xA003)

/**
    \brief Link CMD: Set venc output

    SUPPORTED in all platforms

    \param VDIS_PARAMS_S * [IN] Vdis module params
*/
#define SYSTEM_M3VPSS_CMD_GET_AVSYNC_SHAREDOBJ_PTR          (0xA005)
/**
    \brief Link CMD: Set venc output

    SUPPORTED in all platforms

    \param VDIS_PARAMS_S * [IN] Vdis module params
*/
#define SYSTEM_M3VPSS_CMD_SET_DISPLAYCTRL_VENC_OUTPUT       (0xA006)



typedef struct
{
    UInt32 cpuRev;
    UInt32 boardId;
    UInt32 baseBoardRev;
    UInt32 dcBoardRev;

} SystemVpss_PlatformInfo;



typedef struct {

    UInt32 grpxId;
    Bool   enable;

} SystemVpss_GrpxEnable;

typedef struct {

    Void *pAvSyncSharedObjPtr;

} SystemVpss_AvSyncSharedObjPtr;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
