/** ==================================================================
 *  @file   wdrLink_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/wdr/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _WDR_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _WDR_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/wdrLink.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
#include <ti/psp/iss/iss_capture.h>

#define WDR_LINK_OBJ_MAX                   	(1)
#define WDR_LINK_MAX_OUT_FRAMES_PER_CH     	(SYSTEM_LINK_FRAMES_PER_CH)
#define WDR_LINK_MAX_OUT_FRAMES            	(WDR_LINK_MAX_CH * WDR_LINK_MAX_OUT_FRAMES_PER_CH)
#define MAX_DEPTH_OF_BUFFERS  				(8)

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/iss/core/msp_types.h>

typedef struct {
    MSP_U32 ulComputeWidth;
    MSP_U32 ulComputeHeight;
    MSP_U32 ulInputStride;
    MSP_U32 ulOutputStride;
	MSP_U32 wdrMode;
	MSP_U32 update;
} WdrCreateParams;

/*
 * Per Channel Information */
typedef struct WdrLink_ChObj {
    /* Channel number */
    UInt32 channelId;

    /*
     * Input frame queue - per channel
     * Utils_BufHndl.fullQue will be used to fill frames coming from
     * capture link.
     * Utils_BufHndl.emptyQue will be used to fill the noise-filtered frames.
     * These frames will be allocated during initialization and used in the
     * MSP_WDR_ProcessFrame() API.
     */
    Utils_BufHndl bufInQue;

    /*
     * Maximum FVID2 Frames that may be used for noise filtering
     */
    FVID2_Frame frames[WDR_LINK_MAX_OUT_FRAMES_PER_CH];

    /*
     * Run-time parameter structure for every possible frame
     */
    System_FrameInfo frameInfo[WDR_LINK_MAX_OUT_FRAMES_PER_CH];

    UInt32 inFrameSkipCount;

    WdrCreateParams chCreateArgs;

    FVID2_Format format;
} WdrLink_ChObj;

typedef struct WdrLink_Obj {
    UInt32 linkId;

    /* WDR task structure */
    Utils_TskHndl tsk;

    /* WDR Link create params - to be passed from the application */
    WdrLink_CreateParams createArgs;

    /* WDR link info */
    System_LinkInfo info;

    /* Previous link (Capture) info */
    System_LinkInfo inTskInfo;

    /* Previous link's (Capture) queue info - only 1 input queue is allowed */
    System_LinkQueInfo inQueInfo;

    /* Output frame queue */
    Utils_BufHndl bufOutQue;

    /* WDR channels related info */
    WdrLink_ChObj linkChInfo[WDR_LINK_MAX_CH];

    FVID2_FrameList inFrameList;
    FVID2_FrameList outFrameList;
    FVID2_ProcessList processList;

    UInt32 inFrameReceiveCount;
    UInt32 inFrameGivenCount;
    UInt32 outFrameReceiveCount;
    UInt32 outFrameGivenCount;
    UInt32 processFrameReqCount;
    UInt32 getProcessFrameReqCount;

    UInt32 curTime;
    UInt32 totalTime;
} WdrLink_Obj;

/* ===================================================================
 *  @func     WdrLink_getInfo
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
Int32 WdrLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info);

/* ===================================================================
 *  @func     WdrLink_getFullFrames
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
Int32 WdrLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList);
/* ===================================================================
 *  @func     WdrLink_putEmptyFrames
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
Int32 WdrLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     WdrLink_drvCreate
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
Int32 WdrLink_drvCreate(WdrLink_Obj * pObj, WdrLink_CreateParams * pPrm);

/* ===================================================================
 *  @func     WdrLink_drvDelete
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
Int32 WdrLink_drvDelete(WdrLink_Obj * pObj);

/* ===================================================================
 *  @func     WdrLink_drvProcessData
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
Int32 WdrLink_drvProcessData(WdrLink_Obj * pObj);

/* ===================================================================
 *  @func     WdrLink_drvReleaseFrames
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
Int32 WdrLink_drvReleaseFrames(WdrLink_Obj * pObj,
                               FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     WdrLink_drvStart
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
Int32 WdrLink_drvStart(WdrLink_Obj * pObj);

/* ===================================================================
 *  @func     WdrLink_drvStop
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
Int32 WdrLink_drvStop(WdrLink_Obj * pObj);

Int32 WdrLink_drvChangeMode(WdrLink_Obj * pObj, WdrLink_Mode mode);

#define SYSTEM_DEBUG_WDR
#ifdef SYSTEM_DEBUG_WDR
#define WDRLINK_INFO_LOG(...)       do {                           \
                                                     Vps_printf(               \
                                                      "\n %d: WDR: ", \
                                                      Clock_getTicks()); \
                                                      Vps_printf(__VA_ARGS__);\
                                                } while(0)
#else
#define WDRLINK_INFO_LOG(...)
#endif

// #define SYSTEM_DEBUG_WDR_RT
#ifdef SYSTEM_DEBUG_WDR_RT
#define WDRLINK_INFO_LOG_RT(...)                    Vps_rprintf(__VA_ARGS__);
#else
#define WDRLINK_INFO_LOG_RT(...)
#endif

#define WDRLINK_VARGS_NUMARGS(...)  (sizeof((int[]){(int)__VA_ARGS__})/sizeof(int))

#define WDRLINK_INTERNAL_ERROR_LOG(errCode,...)                                \
      do {                                                                     \
        Vps_printf("\n%d:!ERROR!:WDRLINK::%s:[%d]::INTERNAL ERROR:%d",         \
                   Clock_getTicks(),                                           \
                   __FILE__,                                                   \
                   __LINE__,                                                   \
                   errCode);                                                   \
        if(WDRLINK_VARGS_NUMARGS(__VA_ARGS__)) {                               \
            Vps_printf(__VA_ARGS__);                                           \
        }                                                                      \
      } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _WDR_LINK_PRIV_H_
                                                            */
