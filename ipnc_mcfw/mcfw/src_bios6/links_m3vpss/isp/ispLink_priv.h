/** ==================================================================
 *  @file   ispLink_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/isp/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISP_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISP_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/ispLink.h>

#define ISP_LINK_OBJ_MAX                (1)
#define ISP_LINK_MAX_OUT_FRAMES         (SYSTEM_LINK_FRAMES_PER_CH)
#define ISP_LINK_DEFAULT_FRAMERATE      (60)

typedef struct {
    UInt32 drvInstId;
    Iss_IspCreateParams createParams;
    Iss_IspCreateStatus createStatus;
    FVID2_Handle fvidHandle;
    FVID2_ProcessList errCbProcessList;
    FVID2_FrameList inFrameList;
    FVID2_FrameList outFrameList;
    FVID2_ProcessList processList;
    Semaphore_Handle complete;

} IspLink_drvObj;

typedef struct {
    Utils_TskHndl tsk;
    IspLink_CreateParams createArgs;
    UInt32 receviedFrameCount;
    System_LinkInfo info;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;
    FVID2_Frame outFrames[ISPLINK_MAX_OUT_QUE][ISP_LINK_MAX_OUT_FRAMES];
    FVID2_Format bufferFrameFormat[ISPLINK_MAX_OUT_QUE];
    Utils_BufHndl outFrameQue[ISPLINK_MAX_OUT_QUE];
    System_FrameInfo frameInfo[ISPLINK_MAX_OUT_QUE][ISP_LINK_MAX_OUT_FRAMES];
    Iss_IspPerFrameCfg perFrameCfg[ISPLINK_MAX_OUT_QUE][ISP_LINK_MAX_OUT_FRAMES];
    UInt32 pitch[ISPLINK_MAX_OUT_QUE][3];
    Utils_frameSkipContext frameSkipContext[ISPLINK_MAX_OUT_QUE];
    IspLink_drvObj drvObj;
    UInt32 mirrorMode;
    UInt32 doGetProcessFrames;
} IspLink_Obj;

/* Function prototypes */

/* ===================================================================
 *  @func     IspLink_drvCreateFvidObj
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
Int32 IspLink_drvCreateFvidObj(IspLink_Obj * pObj);

/* ===================================================================
 *  @func     IspLink_drvDeleteFvidObj
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
Int32 IspLink_drvDeleteFvidObj(IspLink_Obj * pObj);

Int32 IspLink_drvProcessFrames(IspLink_Obj * pObj);

Int32 IspLink_setOutputDataFormat(IspLink_Obj * pObj,IspLink_outputDataFormat *pOutDataFmt);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
