/** ===========================================================================
* @file stream.h
*
* @path $(IPNCPATH)\multimedia\encode_stream\stream
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#ifndef _STREAM_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _STREAM_H_

#include <rendezvous.h>
#include <mem_mng.h>
#include <Msg_Def.h>
#include <cache_mng.h>
#include <gop_lock.h>
#include <semaphore_util.h>

#ifdef MEMORY_256MB
#define STREAM_BUF_MEM_SIZE 	(0x01E00000)
#else
#define STREAM_BUF_MEM_SIZE 	(0x03E00000)
#endif

enum {
    STREAM_FAILURE = -1,
    STREAM_SUCCESS = 0
};

enum {
    STREAM_MP4 = 0,
    STREAM_MJPG,
    STREAM_MP4_EXT,
    STREAM_AUDIO,
    STREAM_H264_1,
    STREAM_H264_2,
};

enum {
    STREAM_SEM_MPEG4 = 0,
    STREAM_SEM_MPEG4_EXT,
    STREAM_SEM_JPEG,
    STREAM_SEM_AUDIO,
    STREAM_SEM_MEMCPY,
    STREAM_SEM_CACHECPY,
    STREAM_SEM_GOP,
    STREAM_SEM_NUM
};

enum {
    STREAM_FEATURE_BIT_RATE1 = 0,
    STREAM_FEATURE_BIT_RATE2,
    STREAM_FEATURE_JPG_QUALITY,
    STREAM_FEATURE_AAC_BITRATE,
    STREAM_FEATURE_FRAMERATE1,
    STREAM_FEATURE_FRAMERATE2,
    STREAM_FEATURE_FRAMERATE3,
    STREAM_FEATURE_BLC,
    STREAM_FEATURE_SATURATION,
    STREAM_FEATURE_INCREASE_EXPOSURE,
    STREAM_FEATURE_DECREASE_EXPOSURE,
    STREAM_FEATURE_SET_EXPOSURE,
    STREAM_FEATURE_GET_EXPOSURE,
    STREAM_FEATURE_GET_MAX_EXPOSURE,
    STREAM_FEATURE_GET_MIN_EXPOSURE,

    STREAM_FEATURE_SET_GAIN,
    STREAM_FEATURE_GET_GAIN,
    STREAM_FEATURE_GET_MAX_GAIN,
    STREAM_FEATURE_GET_MIN_GAIN,
    
    STREAM_FEATURE_AWB_MODE,
    STREAM_FEATURE_AE_MODE,
    STREAM_FEATURE_BRIGHTNESS,
    STREAM_FEATURE_RESOLUTION,
    STREAM_FEATURE_FRAMERATE,
    STREAM_FEATURE_CONTRAST,
    STREAM_FEATURE_SHARPNESS,
    STREAM_FEATURE_AEW_VENDOR,
    STREAM_FEATURE_AEW_TYPE,
    STREAM_FEATURE_AEW_PRIORITY,
    STREAM_FEATURE_GET_2A_PARAM,
    STREAM_FEATURE_SET_2A_PARAM,
    STREAM_FEATURE_IRIS,
    STREAM_FEATURE_FRAMECTRL,
    STREAM_FEATURE_ENV_50_60HZ,
    STREAM_FEATURE_BINNING_SKIP,
    STREAM_FEATURE_LOCALDISPLAY,
    STREAM_FEATURE_TSTAMPENABLE,
    STREAM_FEATURE_PTZ,
    STREAM_FEATURE_MOTION,
    STREAM_FEATURE_ROICFG,
    STREAM_FEATURE_OSDTEXT_EN,
    STREAM_FEATURE_HIST_EN,
    STREAM_FEATURE_OSDLOGO_EN,
    STREAM_FEATURE_OSDTEXT,
    STREAM_FEATURE_FACE_SETUP,
    STREAM_FEATURE_DATETIMEPRM,
    STREAM_FEATURE_OSDPRM1,
    STREAM_FEATURE_OSDPRM2,
    STREAM_FEATURE_OSDPRM3,
    STREAM_FEATURE_CLICKNAME,
    STREAM_FEATURE_CLICKSTORAGE,
    STREAM_FEATURE_ROIPRM1,
    STREAM_FEATURE_ROIPRM2,
    STREAM_FEATURE_ROIPRM3,
    STREAM_FEATURE_ADVCODECPRM1,
    STREAM_FEATURE_ADVCODECPRM2,
    STREAM_FEATURE_ADVCODECPRM3,
    STREAM_FEATURE_OSDENABLE,
    STREAM_FEATURE_IFRAME,
    STREAM_FEATURE_MIRROR,
    STREAM_FEATURE_LDC_ENABLE,
    STREAM_FEATURE_VNF_PARAM,
    STREAM_FEATURE_AUDIO_ALARM_LEVEL,
    STREAM_FEATURE_AUDIO_ALARM_FLAG,
    STREAM_FEATURE_DYNRANGE,
    STREAM_FEATURE_STREAMSTACK,
    STREAM_FEATURE_STOPDEMO,
    STREAM_FEATURE_RATE_CONTROL1,
    STREAM_FEATURE_RATE_CONTROL2,
    STREAM_FEATURE_MCFW_CREATE_PARAM,

	/* DMVA Specific messages */
	STREAM_FEATURE_DMVA_SETUP,
    STREAM_FEATURE_DMVAALGOFRAMERATE_SETUP,
    STREAM_FEATURE_DMVAALGODETECTFREQ_SETUP,
    STREAM_FEATURE_DMVAEVTRECORDENABLE_SETUP,
    STREAM_FEATURE_DMVASMETATRACKERENABLE_SETUP,

    STREAM_FEATURE_DMVA_TZ_SETUP,
	STREAM_FEATURE_DMVA_MAIN_SETUP,
	STREAM_FEATURE_DMVA_CTD_SETUP,
	STREAM_FEATURE_DMVA_OC_SETUP,
	STREAM_FEATURE_DMVA_SMETA_SETUP,
	STREAM_FEATURE_DMVA_IMD_SETUP,

	/* DMVA Enc Roi Enable */
	STREAM_FEATURE_DMVA_ENCROI_ENABLE,

	STREAM_FEATURE_SET_CAMERA_RES,
	
	STREAM_FEATURE_SET_CAMERA_CODECCFG,

    /* IMG ALG MESSAGE*/
    STREAM_FEATURE_IMGALG_ENABLE,
    STREAM_FEATURE_IMGALG_DISABLE,
    STREAM_FEATURE_IMGALG_STATIC_IMG,
    STREAM_FEATURE_IMGALG_SENSOR_IMG,
    STREAM_FEATURE_SET_IMGALG_PARAM,
    STREAM_FEATURE_GET_IMGALG_PARAM,
    STREAM_FEATURE_ENABLE_IMGALG_DBGMODE,
    STREAM_FEATURE_DISABLE_IMGALG_DBGMODE,
    STREAM_FEATURE_NUM
};

enum {
    STREAM_EXT_MP4_CIF = 0,
    STREAM_EXT_JPG,
    STREAM_EXT_NUM
};

typedef struct _STREAM_SET {
    int ImageWidth;
    int ImageHeight;
    int ImageWidth_Ext[STREAM_EXT_NUM];
    int ImageHeight_Ext[STREAM_EXT_NUM];
    int JpgQuality;
    int Mpeg4Quality;
    int Mem_layout;
} STREAM_SET;

typedef struct _STREAM_PARM {
    MEM_MNG_INFO MemInfo;
    int MemMngSemId[STREAM_SEM_NUM];
    pthread_t threadControl;
    Rendezvous_Obj objRv[GOP_INDEX_NUM];
    int checkNewFrame[GOP_INDEX_NUM];
    int lockNewFrame[GOP_INDEX_NUM];
    int IsQuit;
    int qid;
    int mid;
    int ImageWidth;
    int ImageHeight;
    int ImageWidth_Ext[STREAM_EXT_NUM];
    int ImageHeight_Ext[STREAM_EXT_NUM];
    int JpgQuality;
    int Mpeg4Quality;
} STREAM_PARM;

/* ===================================================================
 *  @func     stream_get_handle
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
STREAM_PARM *stream_get_handle(void);

/* ===================================================================
 *  @func     stream_init
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
int stream_init(STREAM_PARM * pParm, STREAM_SET * pSet);
int stream_update_vol(STREAM_PARM * pParm,int streamType);
/* ===================================================================
 *  @func     stream_write
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
int stream_write(void *pAddr, int size, int frame_type, int stream_type,
                 unsigned int timestamp, unsigned int temporalId,
                 STREAM_PARM * pParm);
/* ===================================================================
 *  @func     stream_end
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
int stream_end(STREAM_PARM * pParm);

int App_msgHandlerInit(STREAM_PARM *pParm);
int App_msgHandlerExit(STREAM_PARM *pParm);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
