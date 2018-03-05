/* ===========================================================================
* @file ApproDrvMsg.h
*
* @path $(IPNCPATH)/include/
*
* @desc Message driver for Appro_interface
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#ifndef _APPRODRVMSG_
#define _APPRODRVMSG_

#if defined (__cplusplus)
extern "C" {
#endif

/**
  \file ApproDrvMsg.h

  \brief Message driver for Appro_interface
*/

#include <Msg_Def.h>

/**
  \defgroup AV_MSG_DRV Appro interface message driver datatypes, data structures, functions

  The section defines some common datatypes, data structures, callbacks which are used by all parts of the Appro interface framework.
  System init APIs are also defined in this section.

  \section AV_MSG_DRV_COMMON_DIR_STRUCT Directory Structure

  \subsection AV_MSG_DRV_COMMON_INTERFACE Interface

  \code
  ApproDrvMsg.h
  \endcode

  \subsection AV_MSG_DRV_COMMON_SOURCE Source

  \code
  ApproDrvMsg.c
  \endcode


  @{
*/
/* This function should be called at process innitial !!! */
void ApproDrv_SetProcId(int proc_id);
void ApproDrv_SaveQid(int iqid);
/* Only one ApproDrvInit is allowed in each process */
int ApproDrvInit(int proc_id);
int ApproDrvExit();
/* API */
MSG_MEM GetPhyMem();
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt);
FrameInfo_t WaitNewFrame(FrameFormat_t fmt);
void SendQuitCmd();
int LockFrame(FrameInfo_t *pFrame);
int LockMpeg4IFrame(FrameInfo_t *pFrame,int fmt_type);
void UnlockFrame(FrameInfo_t *pFrame);
int GetVolInfo(FrameInfo_t *pFrame,int fmt_type);
int GetMediaInfo(FrameInfo_t *pFrame);
int SetDayNight(unsigned char nDayNight);
int SetWhiteBalance(unsigned char nValue);
int SetExposure(unsigned int nValue);
int GetExposure(unsigned int *nValue);
int Get_MaxExposure(unsigned int *nValue);
int Get_MinExposure(unsigned int *nValue);
int SetGain(unsigned int sensorGain,unsigned int pipeGain);
int GetGain(unsigned int *sensorGain,unsigned int *pipeGain);
int Get_MaxGain(unsigned int *sensorGain,unsigned int *pipeGain);
int Get_MinGain(unsigned int *sensorGain,unsigned int *pipeGain);
int SetH3AParam(EzCamH3AParam *Prm);
int GetH3AParam(EzCamH3AParam *pPrm);
int SetH3AWeight(char *weight,int size);
int GetH3AWeight(char *weight,int size);
int SetBacklight(unsigned char nValue);
int SetBrightness(unsigned char nValue);
int SetContrast(unsigned char nValue);
int SetSaturation(unsigned char nValue);
int SetSharpness(unsigned char nValue);
int SetAACBitrate(int nValue);
int SetEncBitrate(unsigned char stream, int nValue);
int SetEncFramerate(unsigned char stream, int nValue);
int SetJpgQuality(int nValue);
int SetRot(int nValue);
int SetMirr(int nValue);
int SetOSDWindow(int nValue);
int SetImage2AType(unsigned char nValue);
int SetDCIRIS(unsigned char value);
int SetFramectrl(unsigned char nValue);
int SetTvSystem(unsigned char nValue);
int SetBinningSkip(unsigned char nValue);
int SetTStamp(unsigned char nValue);
int SetPtz(int nValue);
int ApproSetMotion(ApproMotionPrm* pMotionPrm);
int SetLdcEnableMsg(unsigned char nValue);
int SetVNFParamMsg(VnfParam* vnfParam);
int SetROIConfig(unsigned char nValue);
int SetAvOsdTextEnable(int enable);
int SetHistEnable(int enable);
int SetAvOsdLogoEnable(int enable);
int SetImage2AVendor(unsigned char nValue);
int Set2APriority(unsigned char nValue);
int SetDisplayValue(unsigned char nValue);
int SetOSDEnable(unsigned char nValue);
int SetStreamFramerate(unsigned char stream, unsigned int nValue);
int SendFaceDetectMsg(FaceDetectParam* faceParam);
int SetDateTimeFormat(DateTimePrm* datetimeParam);
int SetOSDPrmMsg(int id, OSDPrm* osdprm);
int SetCodecROIMsg(int id, CodecROIPrm* codecROIPrm);
int SetCodecAdvPrmMsg(int id, CodecAdvPrm* codecAdvPrm);
int SetClipSnapName(char* strText, int nLength);
int SetClipSnapLocation(unsigned char nValue);
int SendForceIFrameMsg(unsigned char nValue);
int SetAudioAlarmValue(unsigned char value);
int SetAudioAlarmFlag(unsigned char nValue);
int SetDynRangeParam(void);
int SetDynRangePrmMsg(DynRangePrm* dynRangePrm);
int ApproPollingMsg(void);
int SetStreamingStack(unsigned char nValue);
int StopMcFWUsecase(unsigned char nValue);
int SetMcfwCreatePrmMsg(McfwCreatePrm* mcfwCreatePrm);
int SetEncRateControl(unsigned char stream, int nValue);

/* DMVA Message declarations */
int SendDmvaEnableMsg(DmvaParam* dmvaParam);
int SendDmvaExptAlgoFrameRateMsg(DmvaParam* dmvaParam);
int SendDmvaExptAlgoDetectFreqMsg(DmvaParam* dmvaParam);
int SendDmvaExptEvtRecordEnableMsg(DmvaParam* dmvaParam);
int SendDmvaExptSmetaTrackerEnableMsg(DmvaParam* dmvaParam);

int SendDmvaTZSetupMsg(DmvaTZParam* dmvaTZParam);
int SendDmvaMainSetupMsg(DmvaMainParam* dmvaMainParam);
int SendDmvaCTDSetupMsg(DmvaCTDParam* dmvaCTDParam);
int SendDmvaOCSetupMsg(DmvaOCParam* dmvaOCParam);
int SendDmvaSMETASetupMsg(DmvaSMETAParam* dmvaSMETAParam);
int SendDmvaIMDSetupMsg(DmvaIMDParam* dmvaIMDParam);

/* DMVA Enc Roi Enable */
int SendDmvaEncRoiEnableMsg(int *pEnable);

int SendVideoCodecRes(StreamResolution *strmRes);
int SendVideoCodecCfg(VideoCodecCfg *pVidCodecCfg);
int GetMcFWInitStatus(McFWInitStatus *pStatus);

int SaveFrameImg(char *filename);
int EnableEncodeImg();
int DisableEncodeImg();
int EnableSaveErrImg();
int DisableSaveErrImg();
int GetAlgParam(char *pPrm,int *prmSize);
int SetAlgParam(char *pPrm,int prmSize);
int Update_Alg_DbgImg(char *data,unsigned size);
int ResetPayload();
int StartPayload();
int PrintPayload();
int GetPayload(EzCpuDynamicParam *prf);
int GetAlgResult(Alg_Result *result);
int DisableImgAlg();
int EnableImgAlg();
int SetAlgMode(unsigned int debugMode);
int SetOutPut_ImgSource();
int ImgAlg_UseSensorImg();
int ImgAlg_UseStaticImg();
/* @} */

#if defined (__cplusplus)
}
#endif

#endif

