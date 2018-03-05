/** ==================================================================
 *  @file   glbceLink_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/glbce/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _GLBCE_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GLBCE_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/glbceLink.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
#include <ti/psp/iss/iss_capture.h>
#include <ti/psp/iss/alg/dcc/inc/idcc.h>

#define GLBCE_LINK_OBJ_MAX                     	(1)
#define GLBCE_LINK_MAX_OUT_FRAMES_PER_CH      	(SYSTEM_LINK_FRAMES_PER_CH)
#define GLBCE_LINK_MAX_OUT_FRAMES              	(GLBCE_LINK_MAX_CH * GLBCE_LINK_MAX_OUT_FRAMES_PER_CH)

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/iss/core/msp_types.h>
#include <ti/psp/iss/alg/glbce/msp_glbce.h>
#include <ti/psp/iss/alg/glbce/inc/dataTypeDefs.h>
#include <ti/psp/iss/alg/glbce/inc/TI_glbce.h>
#include <ti/psp/iss/alg/glbce/inc/glbceStruct.h>
#include <ti/psp/iss/alg/glbce/inc/vicpGlbce.h>
#include <ti/psp/iss/alg/rm/inc/rm.h>
/* ==========================================================================
 */
/**
* @struct GlbceCreateParams
* This structure is used for providing create parameters to the framework of the MSP GLBCE component.
*
* @param ulInputWidth          : Input width of the input to LDC in pixels
* @param ulInputHeight         : Input height of the input to LDC in pixels
* @param ulComputeWidth        : Compute width used for processing data
* @param ulComputeHeight       : Compute height used for processing data
* @param ulInputStride         : Input stride of the input to LDC in bytes for YUV422 Interleaved or luma in case of YUV420 NV12 input
* @param ulInputStrideChroma  : Input stride of the input to LDC in bytes for chroma in case of YUV420 NV12 input
* @param ulOutputStrideLuma    : Output stride for the luma component in pixels
* @param ulOutputStrideChroma  : Output stride for the chroma component in pixels
* @param eOperateMode          : Operate mode for GLBCE component
* @param eInputFormat          : Input format
* @param eOutputFormat         : Output format
* @param eInterpolationLuma    : Interpolation method to be used for Y data by LDC module
* @param bLumaEn               : Enable luma filtering within NSF module
* @param bChromaEn             : Enable chroma filtering within NSF module
* @param eSmoothVal            : Smoothness parameter for NSF
* @param bSmoothLumaEn         : Enable smoothing operation of luma component
* @param bSmoothChromaEn       : Enable smoothing operation of chroma component
*
*/
/* ==========================================================================
 */
typedef struct {
    MSP_U32 ulInputWidth;
    MSP_U32 ulInputHeight;
    MSP_U32 ulComputeWidth;
    MSP_U32 ulComputeHeight;
    MSP_U32 ulInputStride;
    MSP_U32 ulInputStrideChroma;
    MSP_U32 ulOutputStrideLuma;
    MSP_U32 ulOutputStrideChroma;
} GlbceCreateParams;

/*
 * Per Channel Information */
typedef struct GlbceLink_ChObj {
    /* Channel number */
    UInt32 channelId;

    /*
     * Input frame queue - per channel
     * Utils_BufHndl.fullQue will be used to fill frames coming from
     * capture link.
     * Utils_BufHndl.emptyQue will be used to fill the noise-filtered frames.
     * These frames will be allocated during initialization and used in the
     * MSP_GLBCE_ProcessFrame() API.
     */
    Utils_BufHndl bufInQue;

    /*
     * Maximum FVID2 Frames that may be used for noise filtering
     */
    FVID2_Frame frames[GLBCE_LINK_MAX_OUT_FRAMES_PER_CH];

    /*
     * Run-time parameter structure for every possible frame
     */
    System_FrameInfo frameInfo[GLBCE_LINK_MAX_OUT_FRAMES_PER_CH];

    /* previous output frame */
    FVID2_Frame *pPrevOutFrame;

    UInt32 inFrameSkipCount;

    UInt32 nextFid;

    GlbceCreateParams chCreateArgs;

    Bool disable;

    FVID2_Format format;
} GlbceLink_ChObj;

typedef struct GlbceLink_Obj {
    UInt32 linkId;

    /* GLBCE task structure */
    Utils_TskHndl tsk;

    /* GLBCE Link create params - to be passed from the application */
    GlbceLink_CreateParams createArgs;

    /* GLBCE link info */
    System_LinkInfo info;

    /* Previous link (Capture) info */
    System_LinkInfo inTskInfo;

    /* Previous link's (Capture) queue info - only 1 input queue is allowed */
    System_LinkQueInfo inQueInfo;

    /* Output frame queue */
    Utils_BufHndl bufOutQue;

    /* GLBCE channels related info */
    GlbceLink_ChObj linkChInfo[GLBCE_LINK_MAX_CH];

    FVID2_FrameList inFrameList;
    FVID2_FrameList outFrameList;
    FVID2_ProcessList processList;


    /* GLBCE MSP handle */
    void *mspHandleGlbce;

	GLBCE_CONTEXT glbceHandle; //This Handle handles the tonc curve etc

    void * /* RM_HANDLE */ pRMHandle;


	GLBCE_CreationInputParamsStruct *glbceCIP;
	GLBCE_ImageInputParamsStruct    *glbceIIP;
	GLBCEstruct *glbce;


    CPIS_Init vicpInit;
    CPIS_BaseParms base;
	CPIS_BaseParms baseRef;
    CPIS_BoxcarParms paramsBoxCar;
    CPIS_GlbceParms params;
    CPIS_Handle handle;
    CPIS_Handle boxcar_handle;

	char *BOXCAR;
	UInt32 memSizeForBoxCarData;
	UInt32 memSizeForWeightData;


	char *WEIGHT;

	UInt32 WIDTH;
	UInt32 HEIGHT;


	UInt32 boxcarDownsampleRatio;
	Int16 gGain;
	Uint16 sensorType;

    //External scratch memory
    Word16 *externalScratchMemory;

    //External scratch memory
    Word16 *PersistentMemory;

    //Histogram memory
    Int32 *histogramMemory;

	UInt32 frameNum;
    UInt32 frequencyTonalCurve;


    UInt32 inFrameReceiveCount;
    UInt32 inFrameGivenCount;
    UInt32 outFrameReceiveCount;
    UInt32 outFrameGivenCount;
    UInt32 processFrameReqCount;
    UInt32 getProcessFrameReqCount;

    UInt32 curTime;
    UInt32 totalTime;

    Semaphore_Handle sem;
  /**< Read semaphore */

    Semaphore_Struct semMem;
  /**< Memory for read semaphore */

} GlbceLink_Obj;

/* ===================================================================
 *  @func     GlbceLink_getInfo
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
Int32 GlbceLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info);

/* ===================================================================
 *  @func     GlbceLink_getFullFrames
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
Int32 GlbceLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList);
/* ===================================================================
 *  @func     GlbceLink_putEmptyFrames
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
Int32 GlbceLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     GlbceLink_drvCreate
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
Int32 GlbceLink_drvCreate(GlbceLink_Obj * pObj, GlbceLink_CreateParams * pPrm);

/* ===================================================================
 *  @func     GlbceLink_drvDelete
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
Int32 GlbceLink_drvDelete(GlbceLink_Obj * pObj);

/* ===================================================================
 *  @func     GlbceLink_drvProcessData
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
Int32 GlbceLink_drvProcessData(GlbceLink_Obj * pObj);

/* ===================================================================
 *  @func     GlbceLink_drvReleaseFrames
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
Int32 GlbceLink_drvReleaseFrames(GlbceLink_Obj * pObj,
                               FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     GlbceLink_drvStart
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
Int32 GlbceLink_drvStart(GlbceLink_Obj * pObj);

/* ===================================================================
 *  @func     GlbceLink_drvStop
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
Int32 GlbceLink_drvStop(GlbceLink_Obj * pObj);

/* ===================================================================
 *  @func     GlbceLink_drvGetDynamicParams
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
Int32 GlbceLink_drvGetDynamicParams(GlbceLink_Obj * pObj,
                                  GlbceLink_ChDynamicParams * params);
/* ===================================================================
 *  @func     GlbceLink_drvSetDynamicParams
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
Int32 GlbceLink_drvSetDynamicParams(GlbceLink_Obj * pObj,
                                  GlbceLink_PresetDynamicParams * params);

/* ===================================================================
 *  @func     GlbceLink_drvGetStaticParams
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
Int32 GlbceLink_drvGetStaticParams(GlbceLink_Obj * pObj,
                                 GlbceLink_ChStaticParams * params);

void  GlbceLink_drvUpdateDCC( GlbceRunTimeParams *pGlbceParam);

#ifdef SYSTEM_DEBUG_GLBCE
#define GLBCELINK_INFO_LOG(...)       do {                           \
                                                     Vps_printf(               \
                                                      "\n %d: GLBCE: ", \
                                                      Clock_getTicks()); \
                                                      Vps_printf(__VA_ARGS__);\
                                                } while(0)
#else
#define GLBCELINK_INFO_LOG(...)
#endif

// #define SYSTEM_DEBUG_GLBCE_RT
#ifdef SYSTEM_DEBUG_GLBCE_RT
#define GLBCELINK_INFO_LOG_RT(...)                    Vps_rprintf(__VA_ARGS__);
#else
#define GLBCELINK_INFO_LOG_RT(...)
#endif

#define GLBCELINK_VARGS_NUMARGS(...)  (sizeof((int[]){(int)__VA_ARGS__})/sizeof(int))

#define GLBCELINK_INTERNAL_ERROR_LOG(errCode,...)                                \
      do {                                                                     \
        Vps_printf("\n%d:!ERROR!:GLBCELINK::%s:[%d]::INTERNAL ERROR:%d",         \
                   Clock_getTicks(),                                           \
                   __FILE__,                                                   \
                   __LINE__,                                                   \
                   errCode);                                                   \
        if(GLBCELINK_VARGS_NUMARGS(__VA_ARGS__)) {                               \
            Vps_printf(__VA_ARGS__);                                           \
        }                                                                      \
      } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _GLBCE_LINK_PRIV_H_
                                                            */
