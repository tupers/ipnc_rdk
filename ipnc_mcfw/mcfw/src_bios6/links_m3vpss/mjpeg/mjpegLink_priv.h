/** ==================================================================
 *  @file  mjpegLink_priv.h
 *
 *  @path   $(IPNC_PATH)/ipnc_mcfw/mcfw/src_bios6/links_m3vpss/mjpeg
 *
 *  @desc   This  File contains all the definitions of macros, data-
 *          structure used in the mjpeg link.
 *  =====================================================================
 *  Copyright (c) Texas Instruments Inc 2012, 2011
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _MJPEG_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MJPEG_LINK_PRIV_H_

#include <xdc/std.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/mjpegLink.h>
#include <mcfw/src_bios6/links_m3video/system/system_priv_m3video.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include "mjpegLink_algIf.h"

/* =========================================================================
 * All success and failure codes for the module
 * ========================================================================= */

/** @brief Operation successful. */
#define MJPEG_LINK_S_SUCCESS                   (0)

/** @brief General Failure */
#define MJPEG_LINK_E_FAIL                      (-1)

/** @brief Argument passed to function is invalid. */
#define MJPEG_LINK_E_INVALIDARG                (-2)

/** @brief Encoder algorithm create failed */
#define MJPEG_LINK_E_ALGCREATEFAILED           (-3)

/** @brief RMAN assign resource failed */
#define MJPEG_LINK_E_RMANRSRCASSIGNFAILED      (-4)

/** @brief XDM_SETPARAM failed */
#define MJPEG_LINK_E_ALGSETPARAMSFAILED        (-5)

/** @brief Unknown codec type failed */
#define MJPEG_LINK_E_UNSUPPORTEDCODEC          (-6)

/** @brief Creation of task failed */

#define MJPEG_LINK_E_TSKCREATEFAILED           (-7)

/** @brief XDM_GETBUFINFO failed */
#define MJPEG_LINK_E_ALGGETBUFINFOFAILED       (-8)

#define MJPEG_LINK_OBJ_MAX                     (1)

#define MJPEG_LINK_MAX_FRAMES_PER_OUT_QUE      (4)

#define MJPEG_LINK_MAX_OUT_FRAMES_PER_CH       (SYSTEM_LINK_FRAMES_PER_CH)

#define MJPEG_LINK_MAX_OUT_FRAMES              (MJPEG_LINK_MAX_CH*MJPEG_LINK_MAX_OUT_FRAMES_PER_CH)

#define MJPEG_LINK_CMD_GET_PROCESSED_DATA      (0x6000)

#define MJPEG_LINK_MAX_REQ                     (MJPEG_LINK_MAX_OUT_FRAMES)

#define MJPEG_LINK_PROCESS_TSK_SIZE            (8 * KB)

#define MJPEG_LINK_MAX_TASK_NAME_SIZE          (32)

#define MJPEG_LINK_TASK_POLL_DURATION_MS       (8)

#define MJPEG_LINK_PROCESS_DONE_PERIOD_MS      (4)

#define MJPEG_LINK_STATS_START_THRESHOLD       (5)

#define MJPEG_LINK_REQLIST_MAX_REQOBJS         (4)

#define MJPEG_LINK_DEFAULT_ALGPARAMS_STARTX    (0)

#define MJPEG_LINK_DEFAULT_ALGPARAMS_STARTY    (0)

#define MJPEG_LINK_DEFAULT_ALGPARAMS_TARGETBITRATE          (20 * 1000 * 1000)

#define MJPEG_LINK_SETCONFIG_BITMASK_BITRARATE              (0)

#define MJPEG_LINK_SETCONFIG_BITMASK_FPS                    (1)

#define MJPEG_LINK_SETCONFIG_BITMASK_RESET_VALUE            (0xFFFFFFFF)

#define MJPEG_LINK_FPS_WRAPAROUND_COUNT                     (30)

typedef struct MjpegLink_processFrameRate {
    Int32 firstTime;
    Int32 inCnt;
    Int32 outCnt;
    Int32 multipleCnt;
} MjpegLink_processFrameRate;

typedef struct MjpegLink_OutObj {
    Utils_BitBufHndl bufOutQue;
    UInt32 numAllocPools;
    Bitstream_Buf outBufs[MJPEG_LINK_MAX_OUT_FRAMES];
    System_FrameInfo frameInfo[MJPEG_LINK_MAX_OUT_FRAMES];
    UInt32 outNumBufs[UTILS_BITBUF_MAX_ALLOC_POOLS];
    UInt32 buf_size[UTILS_BITBUF_MAX_ALLOC_POOLS];
    UInt32 ch2poolMap[MJPEG_LINK_MAX_CH];
} MjpegLink_OutObj;

typedef struct MjpegLink_ReqObj {
    FVID2_FrameList InFrameList;
    Bitstream_Buf *OutBuf;
} MjpegLink_ReqObj;

typedef struct MjpegLink_ReqList {
    UInt32 numReqObj;
    MjpegLink_ReqObj *reqObj[MJPEG_LINK_REQLIST_MAX_REQOBJS];
} MjpegLink_ReqList;

typedef struct MjpegLink_algObj {
    union {
        MSP_COMPONENT_TYPE *jpegEncComponent;
    } alg;
    MjpegLink_AlgCreateParams algCreateParams;
    MjpegLink_AlgDynamicParams algDynamicParams;
    UInt32 setConfigBitMask;
    UInt32 getConfigFlag;
} MjpegLink_algObj;

typedef struct MjpegLink_periodicObj {
    Clock_Struct clkStruct;
    Clock_Handle clkHandle;
    Bool clkStarted;
} MjpegLink_periodicObj;

typedef struct MjpegLink_ChObj {
    Utils_QueHandle inQue;
    MjpegLink_algObj algObj;
    FVID2_Frame *inFrameMem[MJPEG_LINK_MAX_REQ];

    MjpegLink_processFrameRate frameStatus;
    Bool disableChn;
    Uint32 frameSkipModulo;
    Uint32 curFrameNum;
    UInt32 nextFid;
    UInt32 allocPoolID;
    UInt32 prevFrmRecvTime;
    UInt32 totalProcessTime;
    UInt32 totalFrameIntervalTime;
    UInt32 totalInFrameCnt;
    UInt32 inFrameSkipCount;
    UInt32 inFrameQueCount;
    UInt32 processReqestCount;
    UInt32 getProcessedFrameCount;
    UInt32 numReqObjPerProcess;
    UInt32 inFrameRecvCount;
    UInt32 inFrameRejectCount;
    UInt32 inFrameUserSkipCount;
    UInt32 outFrameCount;
    UInt32 minLatency;
    UInt32 maxLatency;

    UInt32 mjpegQp;
    UInt32 skipFrameFactor;
    UInt32 width;
    UInt32 height;
    UInt32 pitch;
    UInt32 startX;
    UInt32 startY;
	UInt32 inputChromaFormat;
	UInt32 isChromaFormat420;

} MjpegLink_ChObj;

typedef struct MjpegLink_Obj {
    UInt32 linkId;
    Utils_TskHndl tsk;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;
    MjpegLink_OutObj outObj;
    UInt32 reqQueCount;
    MjpegLink_ReqObj reqObj[MJPEG_LINK_MAX_REQ];
    Utils_QueHandle reqQue;
    MjpegLink_ReqObj *reqQueMem[MJPEG_LINK_MAX_REQ];
    volatile System_VideoLinkState state;
    Utils_BufHndl outFrameQue[MJPEG_LINK_MAX_OUT_QUE];
    FVID2_Frame frames[MJPEG_LINK_MAX_OUT_QUE *
                       MJPEG_LINK_MAX_FRAMES_PER_OUT_QUE];

    System_FrameInfo frameInfo[MJPEG_LINK_MAX_OUT_QUE *
                               MJPEG_LINK_MAX_FRAMES_PER_OUT_QUE];
    Bool isReqPend;
    UInt32 inFrameGetCount;
    UInt32 inFramePutCount;
    System_LinkInfo info;
    UInt32 ch2ProcessTskId[MJPEG_LINK_MAX_CH];
    MjpegLink_ChObj chObj[MJPEG_LINK_MAX_CH];
    MjpegLink_CreateParams createArgs;
    MjpegLink_periodicObj periodicObj;
    UInt32 codecInstId;
    Utils_QueHandle processDoneQue;
    MjpegLink_ReqObj *processDoneQueMem[MJPEG_LINK_MAX_OUT_FRAMES];
    struct simcopProcessTsk_s {
        Task_Struct tskStruct;
        Task_Handle tsk;
        char name[MJPEG_LINK_MAX_TASK_NAME_SIZE];
        Utils_QueHandle processQue;
         MjpegLink_ReqObj
            * processQueMem[(MJPEG_LINK_MAX_OUT_FRAMES / MJPEG_LINK_MAX_CH)];
    } simcopProcessTsk[MJPEG_LINK_MAX_CH];
    UInt32 statsStartTime;
    UInt32 isSomethingSubmitted;
} MjpegLink_Obj;

/* ===================================================================
 *  @func     MjpegLink_codecSetqpParamP
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
Int32 MjpegLink_codecSetqpParamP(MjpegLink_Obj * pObj,
                                 MjpegLink_ChQpParams * params);
Int32 MjpegLink_codecSetFrameRate(MjpegLink_Obj * pObj,
                                 MjpegLink_ChFpsParams * params);
/* ===================================================================
 *  @func     MjpegLink_getInfo
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
Int32 MjpegLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info);

/* ===================================================================
 *  @func     MjpegLink_getFullBufs
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
Int32 MjpegLink_getFullBufs(Utils_TskHndl * pTsk, UInt16 queId,
                            Bitstream_BufList * pBufList);
/* ===================================================================
 *  @func     MjpegLink_putEmptyBufs
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
Int32 MjpegLink_putEmptyBufs(Utils_TskHndl * pTsk, UInt16 queId,
                             Bitstream_BufList * pBufList);
/* ===================================================================
 *  @func     MjpegLink_codecCreate
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
Int32 MjpegLink_codecCreate(MjpegLink_Obj * pObj,
                            MjpegLink_CreateParams * pPrm);
/* ===================================================================
 *  @func     MjpegLink_codecProcessData
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
Int32 MjpegLink_codecProcessData(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecGetProcessedDataMsgHandler
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
Int32 MjpegLink_codecGetProcessedDataMsgHandler(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecStop
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
Int32 MjpegLink_codecStop(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecDelete
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
Int32 MjpegLink_codecDelete(MjpegLink_Obj * pObj,
                            MSP_COMPONENT_TYPE * jComponent);
/* ===================================================================
 *  @func     MjpegLink_getCurLinkID
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
Int32 MjpegLink_getCurLinkID(Void * key);

/* ===================================================================
 *  @func     MjpegLink_codecGetProcessedDataMsgHandler
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
Int32 MjpegLink_codecGetProcessedDataMsgHandler(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_codecGetDynParams
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
Int32 MjpegLink_codecGetDynParams(MjpegLink_Obj * pObj,
                                  MjpegLink_GetDynParams * params);
/* ===================================================================
 *  @func     MjpegLink_codecSetFps
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
Int32 MjpegLink_codecSetFps(MjpegLink_Obj * pObj,
                            MjpegLink_ChFpsParams * params);
/* ===================================================================
 *  @func     MjpegLink_codecDisableChannel
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
Int32 MjpegLink_codecDisableChannel(MjpegLink_Obj * pObj,
                                    MjpegLink_ChannelInfo * params);
/* ===================================================================
 *  @func     MjpegLink_codecEnableChannel
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
Int32 MjpegLink_codecEnableChannel(MjpegLink_Obj * pObj,
                                   MjpegLink_ChannelInfo * params);
/* ===================================================================
 *  @func     MjpegLink_resetStatistics
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
Int32 MjpegLink_resetStatistics(MjpegLink_Obj * pObj);

/* ===================================================================
 *  @func     MjpegLink_printStatistics
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
Int32 MjpegLink_printStatistics(MjpegLink_Obj * pObj, Bool resetAfterPrint);

#ifdef SYSTEM_DEBUG_MJPEG
#define MJPEGLINK_INFO_LOG(linkID,chID,...)       do {                           \
                                                     Vps_printf(               \
                                                      "\n %d: MJPEG SIMCOP: CH%d: ", \
                                                      Clock_getTicks(),chID); \
                                                      Vps_printf(__VA_ARGS__);\
                                                } while(0)
#else
#define MJPEGLINK_INFO_LOG(...)
#endif

#ifdef SYSTEM_DEBUG_MJPEG_VERBOSE
#define MJPEGLINK_VERBOSE_INFO_LOG(linkID,chID,...)  MJPEGLINK_INFO_LOG(linkID, \
                                                                    chID,   \
                                                                    __VA_ARGS__);
#else
#define MJPEGLINK_VERBOSE_INFO_LOG(...)
#endif

#ifdef SYSTEM_DEBUG_MJPEG_RT
#define MJPEGLINK_INFO_LOG_RT(info,...)                    Vps_rprintf(__VA_ARGS__);
#else
#define MJPEGLINK_INFO_LOG_RT(info)
#endif

#define MJPEGLINK_VARGS_NUMARGS(...)  (sizeof((int[]){(int)__VA_ARGS__})/sizeof(int))

#define MJPEGLINK_INTERNAL_ERROR_LOG(errCode,...)                                \
      do {                                                                     \
        Vps_rprintf("\n%d:!ERROR!:MJPEGLINK::%s:[%d]::INTERNAL ERROR:%d",         \
                   Utils_getCurTimeInMsec(),                                           \
                   __FILE__,                                                   \
                   __LINE__,                                                   \
                   errCode);                                                   \
        if(MJPEGLINK_VARGS_NUMARGS(__VA_ARGS__)) {                               \
            Vps_rprintf(__VA_ARGS__);                                           \
        }                                                                      \
      } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _MJPEG_LINK_PRIV_H_
                                                            */
