/** ==================================================================
 *  @file   vnfLink_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/vnf/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _VNF_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _VNF_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/vnfLink.h>
#include <mcfw/interfaces/link_api/system_debug.h>

#include "vnfLink_vnf.h"
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
#include <ti/psp/iss/iss_capture.h>
#include <ti/psp/iss/alg/dcc/inc/idcc.h>

#define VNF_LINK_OBJ_MAX                    (1)
#define VNF_LINK_MAX_OUT_FRAMES_PER_CH		(SYSTEM_LINK_FRAMES_PER_CH)	
#define VNF_LINK_MAX_OUT_FRAMES            	(VNF_LINK_MAX_CH * VNF_LINK_MAX_OUT_FRAMES_PER_CH)

/*
 * Per Channel Information */
typedef struct VnfLink_ChObj {
    /* Channel number */
    UInt32 channelId;

    /*
     * Input frame queue - per channel
     * Utils_BufHndl.fullQue will be used to fill frames coming from
     * capture link.
     * Utils_BufHndl.emptyQue will be used to fill the noise-filtered frames.
     * These frames will be allocated during initialization and used in the
     * MSP_VNF_ProcessFrame() API.
     */
    Utils_BufHndl bufInQue;

    /*
     * Maximum FVID2 Frames that may be used for noise filtering
     */
    FVID2_Frame frames[VNF_LINK_MAX_OUT_FRAMES_PER_CH];

    /*
     * Run-time parameter structure for every possible frame
     */
    System_FrameInfo frameInfo[VNF_LINK_MAX_OUT_FRAMES_PER_CH];

    /* previous output frame */
    FVID2_Frame *pPrevOutFrame;

    UInt32 inFrameSkipCount;

    UInt32 nextFid;

    VnfCreateParams chCreateArgs;

    Bool disable;

    FVID2_Format format;
} VnfLink_ChObj;

typedef struct VnfLink_Obj {
    UInt32 linkId;

    /* VNF task structure */
    Utils_TskHndl tsk;

    /* VNF Link create params - to be passed from the application */
    VnfLink_CreateParams createArgs;

    /* VNF link info */
    System_LinkInfo info;

    /* Previous link (Capture) info */
    System_LinkInfo inTskInfo;

    /* Previous link's (Capture) queue info - only 1 input queue is allowed */
    System_LinkQueInfo inQueInfo;

    /* Output frame queue */
    Utils_BufHndl bufOutQue;

    /* VNF channels related info */
    VnfLink_ChObj linkChInfo[VNF_LINK_MAX_CH];

    /* VNF MSP handle */
    MSP_COMPONENT_TYPE *mspHandleVnf;

    UInt32 inFrameReceiveCount;
    UInt32 inFrameGivenCount;
    UInt32 outFrameReceiveCount;
    UInt32 outFrameGivenCount;
    UInt32 processFrameReqCount;
    UInt32 getProcessFrameReqCount;

    UInt32 curTime;
    UInt32 totalTime;
} VnfLink_Obj;

/* ===================================================================
 *  @func     VnfLink_getInfo
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
Int32 VnfLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info);

/* ===================================================================
 *  @func     VnfLink_getFullFrames
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
Int32 VnfLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList);
/* ===================================================================
 *  @func     VnfLink_putEmptyFrames
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
Int32 VnfLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     VnfLink_drvCreate
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
Int32 VnfLink_drvCreate(VnfLink_Obj * pObj, VnfLink_CreateParams * pPrm);

/* ===================================================================
 *  @func     VnfLink_drvDelete
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
Int32 VnfLink_drvDelete(VnfLink_Obj * pObj);

/* ===================================================================
 *  @func     VnfLink_drvProcessData
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
Int32 VnfLink_drvProcessData(VnfLink_Obj * pObj);

/* ===================================================================
 *  @func     VnfLink_drvReleaseFrames
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
Int32 VnfLink_drvReleaseFrames(VnfLink_Obj * pObj,
                               FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     VnfLink_drvStart
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
Int32 VnfLink_drvStart(VnfLink_Obj * pObj);

/* ===================================================================
 *  @func     VnfLink_drvStop
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
Int32 VnfLink_drvStop(VnfLink_Obj * pObj);

/* ===================================================================
 *  @func     VnfLink_drvGetDynamicParams
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
Int32 VnfLink_drvGetDynamicParams(VnfLink_Obj * pObj,
                                  VnfLink_ChDynamicParams * params);
/* ===================================================================
 *  @func     VnfLink_drvSetDynamicParams
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
Int32 VnfLink_drvSetDynamicParams(VnfLink_Obj * pObj,
                                  VnfLink_ChDynamicParams * params);

void  VnfLink_drvUpdateDCC( VnfRunTimeParams *pVnfRunTimeParams);

/* ===================================================================
 *  @func     VnfLink_drvGetStaticParams
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
Int32 VnfLink_drvGetStaticParams(VnfLink_Obj * pObj,
                                 VnfLink_ChStaticParams * params);

/* ===================================================================
 *  @func     VnfLink_drvEnableLDC
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
Int32 VnfLink_drvEnableLDC(VnfLink_Obj * pObj, UInt32 enableLDC);

/* ===================================================================
 *  @func     VnfLink_drvEnableTNF
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
Int32 VnfLink_drvEnableTNF(VnfLink_Obj * pObj, UInt32 enableTNF);

/* ===================================================================
 *  @func     VnfLink_drvEnableSNF
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
Int32 VnfLink_drvEnableSNF(VnfLink_Obj * pObj, UInt32 enableSNF);

Int32 VnfLink_drvSetStrength(VnfLink_Obj * pObj, VNF_LINK_VNF_STRENGTH strength);

Int32 VnfLink_enableChannel(VnfLink_Obj *pObj,UInt32 channelNum);
Int32 VnfLink_disableChannel(VnfLink_Obj *pObj,UInt32 channelNum);

#ifdef SYSTEM_DEBUG_VNF
#define VNFLINK_INFO_LOG(...)       do {                           \
                                                     Vps_printf(               \
                                                      "\n %d: VNF: ", \
                                                      Clock_getTicks()); \
                                                      Vps_printf(__VA_ARGS__);\
                                                } while(0)
#else
#define VNFLINK_INFO_LOG(...)
#endif

// #define SYSTEM_DEBUG_VNF_RT
#ifdef SYSTEM_DEBUG_VNF_RT
#define VNFLINK_INFO_LOG_RT(...)                    Vps_rprintf(__VA_ARGS__);
#else
#define VNFLINK_INFO_LOG_RT(...)
#endif

#define VNFLINK_VARGS_NUMARGS(...)  (sizeof((int[]){(int)__VA_ARGS__})/sizeof(int))

#define VNFLINK_INTERNAL_ERROR_LOG(errCode,...)                                \
      do {                                                                     \
        Vps_printf("\n%d:!ERROR!:VNFLINK::%s:[%d]::INTERNAL ERROR:%d",         \
                   Clock_getTicks(),                                           \
                   __FILE__,                                                   \
                   __LINE__,                                                   \
                   errCode);                                                   \
        if(VNFLINK_VARGS_NUMARGS(__VA_ARGS__)) {                               \
            Vps_printf(__VA_ARGS__);                                           \
        }                                                                      \
      } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _VNF_LINK_PRIV_H_
                                                            */
