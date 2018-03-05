/** ==================================================================
 *  @file   mctnfLink_priv.h
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

#ifndef _MCTNF_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MCTNF_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3video/system/system_priv_m3video.h>
#include <mcfw/interfaces/link_api/mctnfLink.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include <ti/psp/iss/alg/dcc/inc/idcc.h>
#include <mcfw/src_bios6/links_m3video/codec_utils/utils_encdec_prf.h>

#include "mctnfLink_mctnf.h"

#define MCTNF_LINK_OBJ_MAX                   	(1)
#define MCTNF_LINK_MAX_OUT_FRAMES_PER_CH     	(SYSTEM_LINK_FRAMES_PER_CH)
#define MCTNF_LINK_MAX_OUT_FRAMES            	(MCTNF_LINK_MAX_CH * MCTNF_LINK_MAX_OUT_FRAMES_PER_CH)

/*
 * Per Channel Information */
typedef struct MctnfLink_ChObj {
    /* Channel number */
    UInt32 channelId;

    /*
     * Input frame queue - per channel
     * Utils_BufHndl.fullQue will be used to fill frames coming from
     * capture link.
     * Utils_BufHndl.emptyQue will be used to fill the noise-filtered frames.
     * These frames will be allocated during initialization and used in the
     * MCTNF_ProcessFrame() API.
     */
    Utils_BufHndl bufInQue;

    /*
     * Maximum FVID2 Frames that may be used for noise filtering
     */
    FVID2_Frame frames[MCTNF_LINK_MAX_OUT_FRAMES_PER_CH];

    /*
     * Run-time parameter structure for every possible frame
     */
    System_FrameInfo frameInfo[MCTNF_LINK_MAX_OUT_FRAMES_PER_CH];

    /* previous output frame */
    FVID2_Frame *pPrevOutFrame;

    UInt32 inFrameSkipCount;

    UInt32 nextFid;

    MctnfCreateParams chCreateArgs;

	IMCTNF_Params        params;
	IMCTNF_DynamicParams dynParams;
	IMCTNF_Status        status;
	IMCTNF_InArgs        inArgs;
    IMCTNF_OutArgs       outArgs;
    IVIDEO2_BufDesc      gInputBufDesc;
    IVIDEO2_BufDesc      gOutputBufDesc;

    Bool disable;

    FVID2_Format format;
} MctnfLink_ChObj;

typedef struct MctnfLink_Obj {
    UInt32 linkId;

    /* VNF task structure */
    Utils_TskHndl tsk;

    /* VNF Link create params - to be passed from the application */
    MctnfLink_CreateParams createArgs;

    /* VNF link info */
    System_LinkInfo info;

    /* Previous link (Capture) info */
    System_LinkInfo inTskInfo;

    /* Previous link's (Capture) queue info - only 1 input queue is allowed */
    System_LinkQueInfo inQueInfo;

    /* Output frame queue */
    Utils_BufHndl bufOutQue;

    /* VNF channels related info */
    MctnfLink_ChObj linkChInfo[MCTNF_LINK_MAX_CH];

    /* VNF MSP handle */
    //MSP_COMPONENT_TYPE *mspHandleMctnf;

	MCTNF_Handle algHandle;

	UInt32 isControlToBeCalled;

    UInt32 inFrameReceiveCount;
    UInt32 inFrameGivenCount;
    UInt32 outFrameReceiveCount;
    UInt32 outFrameGivenCount;
    UInt32 processFrameReqCount;
    UInt32 getProcessFrameReqCount;

    UInt32 curTime;
    UInt32 totalTime;

    HDVICP_tskEnv tskEnv;

} MctnfLink_Obj;

Int32 MctnfLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info);

Int32 MctnfLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList);
Int32 MctnfLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList);

Int32 MctnfLink_drvCreate(MctnfLink_Obj * pObj, MctnfLink_CreateParams * pPrm);

Int32 MctnfLink_drvDelete(MctnfLink_Obj * pObj);

Int32 MctnfLink_drvProcessData(MctnfLink_Obj * pObj);

Int32 MctnfLink_drvReleaseFrames(MctnfLink_Obj * pObj,
                               FVID2_FrameList * pFrameList);

Int32 MctnfLink_drvStart(MctnfLink_Obj * pObj);

Int32 MctnfLink_drvStop(MctnfLink_Obj * pObj);

Int32 MctnfLink_drvGetDynamicParams(MctnfLink_Obj * pObj,
                                  MctnfLink_ChDynamicParams * params);
Int32 MctnfLink_drvSetDynamicParams(MctnfLink_Obj * pObj,
                                  MctnfLink_ChDynamicParams * params);

void  MctnfLink_drvUpdateDCC( MctnfRunTimeParams *pMctnfRunTimeParams);

Int32 MctnfLink_drvGetStaticParams(MctnfLink_Obj * pObj,
                                 MctnfLink_ChStaticParams * params);


Int32 MctnfLink_drvSetStrength(MctnfLink_Obj * pObj, MCTNF_LINK_VNF_STRENGTH strength);

Int32 MCTNF_Engine_Init(MctnfLink_Obj * pObj);

Int32 MCTNF_Engine_DeInit(MctnfLink_Obj * pObj);

Int32 MCTNF_ProcessFrame(MctnfLink_Obj * pObj,
                           FVID2_Frame * currentImageBuffer,
                           FVID2_Frame * outputImageBufferTobeFilled,
						   FVID2_Frame **outputImageBufferTobeFreed);

Int32 MCTNF_Engine_Reset(MctnfLink_Obj * pObj);

Int32 MctnfLink_enableChannel(MctnfLink_Obj *pObj,UInt32 channelNum);
Int32 MctnfLink_disableChannel(MctnfLink_Obj *pObj,UInt32 channelNum);

void MCTNF_TI_initOutputBufDesc
(
    MctnfLink_Obj * pObj,
    MCTNF_Status *status
);
void MCTNF_TI_initInputBufDesc
(
    MctnfLink_Obj * pObj,
    MCTNF_Status       *status,
    XDAS_Int32            uiCaptureWidth
);


#ifdef SYSTEM_DEBUG_MCTNF
#define MCTNFLINK_INFO_LOG(...)       do {                           \
                                                     Vps_printf(               \
                                                      "\n %d: MCTNF: ", \
                                                      Clock_getTicks()); \
                                                      Vps_printf(__VA_ARGS__);\
                                                } while(0)
#else
#define MCTNFLINK_INFO_LOG(...)
#endif

// #define SYSTEM_DEBUG_MCTNF_RT
#ifdef SYSTEM_DEBUG_MCTNF_RT
#define MCTNFLINK_INFO_LOG_RT(...)                    Vps_rprintf(__VA_ARGS__);
#else
#define MCTNFLINK_INFO_LOG_RT(...)
#endif

#define MCTNFLINK_VARGS_NUMARGS(...)  (sizeof((int[]){(int)__VA_ARGS__})/sizeof(int))

#define MCTNFLINK_INTERNAL_ERROR_LOG(errCode,...)                                \
      do {                                                                     \
        Vps_printf("\n%d:!ERROR!:MCTNFLINK::%s:[%d]::INTERNAL ERROR:%d",         \
                   Clock_getTicks(),                                           \
                   __FILE__,                                                   \
                   __LINE__,                                                   \
                   errCode);                                                   \
        if(MCTNFLINK_VARGS_NUMARGS(__VA_ARGS__)) {                               \
            Vps_printf(__VA_ARGS__);                                           \
        }                                                                      \
      } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _MCTNF_LINK_PRIV_H_
                                                            */
