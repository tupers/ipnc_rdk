/* ===========================================================================
* @path $(IPNCPATH)\sys_adm\system_server
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2009
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file stream_env_ctrl.h
* @brief Functions about AV Server control.
*/
#ifndef __AV_SERVER_CTRL_H__
#define __AV_SERVER_CTRL_H__
#include <sys_env_type.h>

#define VS_W    1600
#define VS_H    900

/**
* @brief Stream settings.
*/
typedef struct StreamEnv_t{
	unsigned char nDayNight;
	unsigned char nWhiteBalance;
	unsigned char nBackLight;
	unsigned char nBrightness;
	unsigned char nContrast;
	unsigned char nSaturation;
	unsigned char nSharpness;
	unsigned char nVideocodec;
	unsigned char nVideocodecmode;
	unsigned char nVideocodecres;
	unsigned char nStreamType;
	unsigned char nBinning;
	unsigned char nJpegQuality;
	unsigned char nMirror;
	unsigned char powerMode;
	unsigned char tStampEnable;
	unsigned char nAEWswitch;
	unsigned char nAEWtype;
	unsigned char expPriority;
	unsigned char nRateControl1;
	unsigned char nRateControl2;
	unsigned char nFrameRate1;
	unsigned char nFrameRate2;
	unsigned char nFrameRate3;
	unsigned char nMEConfig1;
	unsigned char nMEConfig2;
	unsigned char nMEConfig3;
	unsigned char histogram;
	unsigned char vsEnable;
	unsigned char ldcEnable;
	unsigned char vnfEnable;
	unsigned char vnfMode;
	unsigned char vnfStrength;
	unsigned char dynRange;
	unsigned char dynRangeStrength;
	unsigned char framectrl;
	unsigned char nCodectype1;
	unsigned char nCodectype2;
	unsigned char nCodectype3;
	int nBitrate1;
	int nBitrate2;
	int H264_PORTNUM_1;
	int H264_PORTNUM_2;
	int MPEG4_PORTNUM_1;
	int MPEG4_PORTNUM_2;
	int MJPEG_PORTNUM_1;
} StreamEnv_t;

extern const unsigned int enc_framerate_day_1[CODEC_COMB_NUM][6][4];
extern const unsigned int enc_framerate_day_2[CODEC_COMB_NUM][6][4];
extern const unsigned int enc_framerate_day_3[CODEC_COMB_NUM][6][4];
extern const unsigned int enc_framerate_night_1[CODEC_COMB_NUM][6][4];
extern const unsigned int enc_framerate_night_2[CODEC_COMB_NUM][6][4];
extern const unsigned int enc_framerate_night_3[CODEC_COMB_NUM][6][4];

int StartStream(StreamEnv_t* pConfig);
int SetBinning(unsigned char value);
int GetBinning(unsigned char *value);
int SetCamDayNight(unsigned char value);
int GetCamDayNight(unsigned char *value);
int SetCamWhiteBalance(unsigned char value);
int GetCamWhiteBalance(unsigned char *value);
int SetCamBacklight(unsigned char value);
int SetCamBrightness(unsigned char value);
int GetCamBrightness(unsigned char *value);
int SetCamContrast(unsigned char value);
int GetCamContrast(unsigned char *value);
int SetCamSaturation(unsigned char value);
int GetCamSaturation(unsigned char *value);
int SetCamSharpness(unsigned char value);
int GetCamSharpness(unsigned char *value);
int IncreaseCamExposure(unsigned char value);
int DecreaseCamExposure(unsigned char value);
int GetBlc(unsigned char *value);
int SetCamExposure(unsigned char value);
int GetCamExposure(unsigned char *value);
int GetCam_MaxExposure(unsigned int *value);
int GetCam_MinExposure(unsigned int *value);

int EnvSetGain(unsigned char sensorGain,unsigned char pipeGain);
int EnvGetGain(unsigned char *sensorGain,unsigned char *pipeGain);
int EnvGet_MaxGain(unsigned int *sensorGain,unsigned int *pipeGain);
int EnvGet_MinGain(unsigned int *sensorGain,unsigned int *pipeGain);

/*********** H3A ***************/
int EnvGetH2AParams(EzCamH3AParam *pPrm);
int EnvSetH2AParams(EzCamH3AParam *pPrm);
int EnvGetH2AWeight(char *weight,int *size);
int EnvSetH2AWeight(char *weight,int size);

/********* FILL LIGHT**********/
int EnvSetLedConfig(CamLight_Config *light);
int EnvGetLedConfig(CamLight_Config *light);
/*********** ALG ***************/
int EnvGetPayload(EzCpuDynamicParam *prf);
int EnvPrintPayload();
int EnvStartPayload();
int EnvResetPayload();
int EnvSaveFrameImg(char *filename);
int EnvEnableEncodeImg();
int EnvDisableEncodeImg();
/***********IMG ALG ***************/
int EnvGetAlgResult(Alg_Result *result);
int EnvImgAlg_UseSensorImg();
int EnvImgAlg_UseStaticImg();
int EnvDisableImgAlg();
int EnvEnableImgAlg();
int EnvGetAlgParam(char *pPrm,int *prmSize);
int EnvUpdate_AlgPrmfile(char *pPrm,int prmSize);
int EnvSetAlgParam(char *pPrm,int prmSize);
int EnvSetAlgMode(unsigned debugMode);
int EnvSetOuput_ImgSource();
int EnvUpdateAlg_DbgImg(char *data,unsigned size);

int SetClearSysLog(unsigned char value);
int SetClearAccessLog(unsigned char value);
int SetMJPEGQuality(unsigned char value);
int SetMirror(unsigned char value);
int GetMirror(unsigned char *value);
int SetAdvMode(void);
int SetFaceDetect(unsigned char value);
int SetDemoCfg(unsigned char value);
int SetOSDHistogram(unsigned char value);
int SetMP41bitrate(unsigned int value);
int SetMP42bitrate(unsigned int value);
int SetImageAEW(unsigned char value);
int GetImageAEW(unsigned char *value);
int SetImageAEWType(unsigned char value);
int GetImageAEWType(unsigned char *value);
int SetImagesource(unsigned char value);
int SetTStampFormat(unsigned char value);
int SetFramerate1(unsigned char value);
int SetFramerate2(unsigned char value);
int SetFramerate3(unsigned char value);
int Init_StreamEnv(StreamEnv_t* pConfig);

/** ADVANCE MODE*/
int SetVstabValue(unsigned char value);
int GetVstabValue(unsigned char *value);
int SetLdcValue(unsigned char value);
int GetLdcValue(unsigned char *value);
int SetVnfValue(unsigned char value);
int GetVnfValue(unsigned char *value);
int SetVnfMode(unsigned char value);
int GetVnfMode(unsigned char *value);
int SetVnfStrength(unsigned char value);
int GetVnfStrength(unsigned char *value);

int SetAudioMode(unsigned char value);
int SetAudioEncode(unsigned char value);
int SetAudioSampleRate(unsigned char value);
int SetAudioBitrate(unsigned char value);
int SetRateControl1(unsigned char value);
int SetRateControl2(unsigned char value);
int SetDateStampEnable1(unsigned char value);
int SetDateStampEnable2(unsigned char value);
int SetDateStampEnable3(unsigned char value);
int SetExpPriority(unsigned char value);
int SetIRIS(unsigned char value);
int SetFramectrlValue(unsigned char value);
int SetClickSnapStorage(unsigned char value);
int SetClickSnapFilename(void * buf, int length);
int SetTimeStampEnable1(unsigned char value);
int SetTimeStampEnable2(unsigned char value);
int SetTimeStampEnable3(unsigned char value);
int SetLogoEnable1(unsigned char value);
int SetLogoEnable2(unsigned char value);
int SetLogoEnable3(unsigned char value);
int SetLogoPosition1(unsigned char value);
int SetLogoPosition2(unsigned char value);
int SetLogoPosition3(unsigned char value);
int SetTextEnable1(unsigned char value);
int SetTextEnable2(unsigned char value);
int SetTextEnable3(unsigned char value);
int SetOverlayText1(void * buf, int length);
int SetOverlayText2(void * buf, int length);
int SetOverlayText3(void * buf, int length);
int SetTextPosition1(unsigned char value);
int SetTextPosition2(unsigned char value);
int SetTextPosition3(unsigned char value);
int SetLocalDisplay(unsigned char value);
int SetIpratio1(unsigned int value);
int SetIpratio2(unsigned int value);
int SetIpratio3(unsigned int value);
int SetForceIframe1(unsigned char value);
int SetForceIframe2(unsigned char value);
int SetForceIframe3(unsigned char value);
int SetQPInit1(unsigned char value);
int SetQPInit2(unsigned char value);
int SetQPInit3(unsigned char value);
int SetQPMin1(unsigned char value);
int SetQPMin2(unsigned char value);
int SetQPMin3(unsigned char value);
int SetQPMax1(unsigned char value);
int SetQPMax2(unsigned char value);
int SetQPMax3(unsigned char value);
int SetMEConfig1(unsigned char value);
int SetMEConfig2(unsigned char value);
int SetMEConfig3(unsigned char value);
int SetPacketSize1(unsigned char value);
int SetPacketSize2(unsigned char value);
int SetPacketSize3(unsigned char value);
int SetROIEnable1(unsigned char value);
int SetROIEnable2(unsigned char value);
int SetROIEnable3(unsigned char value);
int SetStr1X1(unsigned int value);
int SetStr1Y1(unsigned int value);
int SetStr1W1(unsigned int value);
int SetStr1H1(unsigned int value);
int SetStr1X2(unsigned int value);
int SetStr1Y2(unsigned int value);
int SetStr1W2(unsigned int value);
int SetStr1H2(unsigned int value);
int SetStr1X3(unsigned int value);
int SetStr1Y3(unsigned int value);
int SetStr1W3(unsigned int value);
int SetStr1H3(unsigned int value);
int SetStr2X1(unsigned int value);
int SetStr2Y1(unsigned int value);
int SetStr2W1(unsigned int value);
int SetStr2H1(unsigned int value);
int SetStr2X2(unsigned int value);
int SetStr2Y2(unsigned int value);
int SetStr2W2(unsigned int value);
int SetStr2H2(unsigned int value);
int SetStr2X3(unsigned int value);
int SetStr2Y3(unsigned int value);
int SetStr2W3(unsigned int value);
int SetStr2H3(unsigned int value);
int SetStr3X1(unsigned int value);
int SetStr3Y1(unsigned int value);
int SetStr3W1(unsigned int value);
int SetStr3H1(unsigned int value);
int SetStr3X2(unsigned int value);
int SetStr3Y2(unsigned int value);
int SetStr3W2(unsigned int value);
int SetStr3H2(unsigned int value);
int SetStr3X3(unsigned int value);
int SetStr3Y3(unsigned int value);
int SetStr3W3(unsigned int value);
int SetStr3H3(unsigned int value);
int SetDetailInfo1(unsigned char value);
int SetDetailInfo2(unsigned char value);
int SetDetailInfo3(unsigned char value);
int SetHistogram(unsigned char value);
int SetDynRange(unsigned char value);
int SetDynRangeStrength(unsigned char value);
int SetAudioON(unsigned char value);
int SetFDX(unsigned int value);
int SetFDY(unsigned int value);
int SetFDW(unsigned int value);
int SetFDH(unsigned int value);
int SetFDConfLevel(unsigned char value);
int SetFDDirection(unsigned char value);
int SetFRecognition(unsigned char value);
int SetFRConfLevel(unsigned char value);
int SetFRDatabase(unsigned char value);
int SetPrivacyMask(unsigned char value);
int SetMaskOptions(unsigned char value);
int SetDateFormat(unsigned char value);
int SetDatePosition(unsigned char value);
int SetTimePosition(unsigned char value);
int SetAudioAlarmLevel(unsigned char value);
int SetEncryption(unsigned char value);
void SetFaceDetectPrm(void);
void CheckAudioParam(void);
void SetAudioParams(void);
int SetOSDDetail(int id);
int SetTimeDateDetail(void);
void ResetBasicDefault(void);
void SetCodecROIDemoParam(int width, int height);
void SetCodecROIParam(int id);
void SetCodecAdvParam(int id);
int SetAudioReceiverEnable(unsigned char value);
int SetAudioSeverIp(void * buf, int length);
int SetMcfwCreatePrm(void);


/* DMVA specific parameters */
int SetDmvaEnable(unsigned int value);
int SetDmvaExptAlgoFrameRate(unsigned int value);
int SetDmvaExptAlgoDetectFreq(unsigned int value);
int SetDmvaExptEvtRecordEnable(unsigned int value);
int SetDmvaExptSmetaTrackerEnable(unsigned int value);
/* DMVA TZ specific parameters */
int SetDmvaTZSensitivity(unsigned int value);
int SetDmvaTZPersonMinWidth(unsigned int value);
int SetDmvaTZPersonMinHeight(unsigned int value);
int SetDmvaTZVehicleMinWidth(unsigned int value);
int SetDmvaTZVehicleMinHeight(unsigned int value);
int SetDmvaTZPersonMaxWidth(unsigned int value);
int SetDmvaTZPersonMaxHeight(unsigned int value);
int SetDmvaTZVehicleMaxWidth(unsigned int value);
int SetDmvaTZVehicleMaxHeight(unsigned int value);
int SetDmvaTZDirection(unsigned int value);
int SetDmvaTZPresentAdjust(unsigned int value);
int SetDmvaTZEnable(unsigned int value);

int SetDmvaTZZone1Numsides(unsigned int value);
int SetDmvaTZZone2Numsides(unsigned int value);
int SetDmvaTZZone1_x01(unsigned int value);
int SetDmvaTZZone1_x02(unsigned int value);
int SetDmvaTZZone1_x03(unsigned int value);
int SetDmvaTZZone1_x04(unsigned int value);
int SetDmvaTZZone1_x05(unsigned int value);
int SetDmvaTZZone1_x06(unsigned int value);
int SetDmvaTZZone1_x07(unsigned int value);
int SetDmvaTZZone1_x08(unsigned int value);
int SetDmvaTZZone1_x09(unsigned int value);
int SetDmvaTZZone1_x10(unsigned int value);
int SetDmvaTZZone1_x11(unsigned int value);
int SetDmvaTZZone1_x12(unsigned int value);
int SetDmvaTZZone1_x13(unsigned int value);
int SetDmvaTZZone1_x14(unsigned int value);
int SetDmvaTZZone1_x15(unsigned int value);
int SetDmvaTZZone1_x16(unsigned int value);
int SetDmvaTZZone1_y01(unsigned int value);
int SetDmvaTZZone1_y02(unsigned int value);
int SetDmvaTZZone1_y03(unsigned int value);
int SetDmvaTZZone1_y04(unsigned int value);
int SetDmvaTZZone1_y05(unsigned int value);
int SetDmvaTZZone1_y06(unsigned int value);
int SetDmvaTZZone1_y07(unsigned int value);
int SetDmvaTZZone1_y08(unsigned int value);
int SetDmvaTZZone1_y09(unsigned int value);
int SetDmvaTZZone1_y10(unsigned int value);
int SetDmvaTZZone1_y11(unsigned int value);
int SetDmvaTZZone1_y12(unsigned int value);
int SetDmvaTZZone1_y13(unsigned int value);
int SetDmvaTZZone1_y14(unsigned int value);
int SetDmvaTZZone1_y15(unsigned int value);
int SetDmvaTZZone1_y16(unsigned int value);
int SetDmvaTZZone2_x01(unsigned int value);
int SetDmvaTZZone2_x02(unsigned int value);
int SetDmvaTZZone2_x03(unsigned int value);
int SetDmvaTZZone2_x04(unsigned int value);
int SetDmvaTZZone2_x05(unsigned int value);
int SetDmvaTZZone2_x06(unsigned int value);
int SetDmvaTZZone2_x07(unsigned int value);
int SetDmvaTZZone2_x08(unsigned int value);
int SetDmvaTZZone2_x09(unsigned int value);
int SetDmvaTZZone2_x10(unsigned int value);
int SetDmvaTZZone2_x11(unsigned int value);
int SetDmvaTZZone2_x12(unsigned int value);
int SetDmvaTZZone2_x13(unsigned int value);
int SetDmvaTZZone2_x14(unsigned int value);
int SetDmvaTZZone2_x15(unsigned int value);
int SetDmvaTZZone2_x16(unsigned int value);
int SetDmvaTZZone2_y01(unsigned int value);
int SetDmvaTZZone2_y02(unsigned int value);
int SetDmvaTZZone2_y03(unsigned int value);
int SetDmvaTZZone2_y04(unsigned int value);
int SetDmvaTZZone2_y05(unsigned int value);
int SetDmvaTZZone2_y06(unsigned int value);
int SetDmvaTZZone2_y07(unsigned int value);
int SetDmvaTZZone2_y08(unsigned int value);
int SetDmvaTZZone2_y09(unsigned int value);
int SetDmvaTZZone2_y10(unsigned int value);
int SetDmvaTZZone2_y11(unsigned int value);
int SetDmvaTZZone2_y12(unsigned int value);
int SetDmvaTZZone2_y13(unsigned int value);
int SetDmvaTZZone2_y14(unsigned int value);
int SetDmvaTZZone2_y15(unsigned int value);
int SetDmvaTZZone2_y16(unsigned int value);

/* DMVA TZ save/load params */
int SetDmvaTZSave(void * buf, int length);
int SetDmvaTZLoad(unsigned int value);

/* Set DMVA Main Page specific Parameters */
int SetDmvaCamID(unsigned int value);
int SetDmvaGetSchedule(unsigned int value);
int SetDmvaCfgFD(unsigned int value);
int SetDmvaCfgCTDImdSmetaTzOc(unsigned int value);
int SetDmvaDisplayOptions(unsigned int value);
int SetDmvaEventRecordingVAME(unsigned int value);
int SetDmvaEventListActionPlaySendSearchTrash(unsigned int value);
int SetDmvaEventListSelectEvent(unsigned int value);
int SetDmvaEventListArchiveFlag(unsigned int value);
int SetDmvaEventListEventType(unsigned int value);

int SetDmvaEventListStartMonth(unsigned int value);
int SetDmvaEventListStartDay(unsigned int value);
int SetDmvaEventListStartTime(unsigned int value);
int SetDmvaEventListEndMonth(unsigned int value);
int SetDmvaEventListEndDay(unsigned int value);
int SetDmvaEventListEndTime(unsigned int value);
int SetDmvaEventListTimeStamp(void * buf, int length);
int SetDmvaEventListEventDetails(void * buf, int length);

/* Set DMVA Advanced Settings Page specific Parameters */
int SetDmvaObjectMetaData(unsigned int value);
int SetDmvaOverlayPropertiesFontType(unsigned int value);
int SetDmvaOverlayPropertiesFontSize(unsigned int value);
int SetDmvaOverlayPropertiesFontColor(unsigned int value);
int SetDmvaOverlayPropertiesBoxColor(unsigned int value);
int SetDmvaOverlayPropertiesCentroidColor(unsigned int value);
int SetDmvaOverlayPropertiesActiveZoneColor(unsigned int value);
int SetDmvaOverlayPropertiesInactiveZoneColor(unsigned int value);
int SetDmvaOverlayPropertiesVelocityColor(unsigned int value);
int SetDmvaEnvironmentProfileLoad(unsigned int value);
int SetDmvaEnvironmentProfileStore(unsigned int value);
int SetDmvaEnvironmentProfileClear(unsigned int value);
int SetDmvaEnvironmentProfileState(unsigned int value);
int SetDmvaEnvironmentProfileDesc(void * buf, int length);


/* Set DMVA event delete specific Parameters */
int SetDmvaEventDeleteIndex(unsigned int value);
int SetDmvaEventDeleteStartIndex(unsigned int value);
int SetDmvaEventDeleteEndIndex(unsigned int value);
int SetDmvaEventDeleteAll(unsigned int value);

/* Set DMVA CTD Page specific Parameters */
int SetDmvaCfgTDEnv1(unsigned int value);
int SetDmvaCfgTDEnv2(unsigned int value);
int SetDmvaCfgTDSensitivity(unsigned int value);
int SetDmvaCfgTDResetTime(unsigned int value);
/* Set DMVA OC Page specific Parameters */
int SetDmvaOCSensitivity(unsigned int value);
int SetDmvaOCObjectWidth(unsigned int value);
int SetDmvaOCObjectHeight(unsigned int value);
int SetDmvaOCDirection(unsigned int value);
int SetDmvaOCEnable(unsigned int value);
int SetDmvaOCLeftRight(unsigned int value);
int SetDmvaOCTopBottom(unsigned int value);
int SetDmvaOCSave(void * buf, int length);
int SetDmvaOCLoad(unsigned int value);
/* Set DMVA SMETA Page specific Parameters */
int SetDmvaSMETASensitivity(unsigned int value);
int SetDmvaSMETAEnableStreamingData(unsigned int value);
int SetDmvaSMETAStreamBB(unsigned int value);
int SetDmvaSMETAStreamVelocity(unsigned int value);
int SetDmvaSMETAStreamHistogram(unsigned int value);
int SetDmvaSMETAStreamMoments(unsigned int value);
int SetDmvaSMETAPresentAdjust(unsigned int value);
int SetDmvaSMETAPersonMinWidth(unsigned int value);
int SetDmvaSMETAPersonMinHeight(unsigned int value);
int SetDmvaSMETAVehicleMinWidth(unsigned int value);
int SetDmvaSMETAVehicleMinHeight(unsigned int value);
int SetDmvaSMETAPersonMaxWidth(unsigned int value);
int SetDmvaSMETAPersonMaxHeight(unsigned int value);
int SetDmvaSMETAVehicleMaxWidth(unsigned int value);
int SetDmvaSMETAVehicleMaxHeight(unsigned int value);
int SetDmvaSMETANumZones(unsigned int value);
int SetDmvaSMETAZone1Numsides(unsigned int value);
int SetDmvaSMETAZone1_x01(unsigned int value);
int SetDmvaSMETAZone1_x02(unsigned int value);
int SetDmvaSMETAZone1_x03(unsigned int value);
int SetDmvaSMETAZone1_x04(unsigned int value);
int SetDmvaSMETAZone1_x05(unsigned int value);
int SetDmvaSMETAZone1_x06(unsigned int value);
int SetDmvaSMETAZone1_x07(unsigned int value);
int SetDmvaSMETAZone1_x08(unsigned int value);
int SetDmvaSMETAZone1_x09(unsigned int value);
int SetDmvaSMETAZone1_x10(unsigned int value);
int SetDmvaSMETAZone1_x11(unsigned int value);
int SetDmvaSMETAZone1_x12(unsigned int value);
int SetDmvaSMETAZone1_x13(unsigned int value);
int SetDmvaSMETAZone1_x14(unsigned int value);
int SetDmvaSMETAZone1_x15(unsigned int value);
int SetDmvaSMETAZone1_x16(unsigned int value);
int SetDmvaSMETAZone1_y01(unsigned int value);
int SetDmvaSMETAZone1_y02(unsigned int value);
int SetDmvaSMETAZone1_y03(unsigned int value);
int SetDmvaSMETAZone1_y04(unsigned int value);
int SetDmvaSMETAZone1_y05(unsigned int value);
int SetDmvaSMETAZone1_y06(unsigned int value);
int SetDmvaSMETAZone1_y07(unsigned int value);
int SetDmvaSMETAZone1_y08(unsigned int value);
int SetDmvaSMETAZone1_y09(unsigned int value);
int SetDmvaSMETAZone1_y10(unsigned int value);
int SetDmvaSMETAZone1_y11(unsigned int value);
int SetDmvaSMETAZone1_y12(unsigned int value);
int SetDmvaSMETAZone1_y13(unsigned int value);
int SetDmvaSMETAZone1_y14(unsigned int value);
int SetDmvaSMETAZone1_y15(unsigned int value);
int SetDmvaSMETAZone1_y16(unsigned int value);
int SetDmvaSMETAZone1_Label(void * buf, int length);
int SetDmvaSMETASave(void * buf, int length);
int SetDmvaSMETALoad(unsigned int value);
/* Set DMVA IMD Page specific Parameters */
int SetDmvaIMDSensitivity(unsigned int value);
int SetDmvaIMDPresentAdjust(unsigned int value);
int SetDmvaIMDPersonMinWidth(unsigned int value);
int SetDmvaIMDPersonMinHeight(unsigned int value);
int SetDmvaIMDVehicleMinWidth(unsigned int value);
int SetDmvaIMDVehicleMinHeight(unsigned int value);
int SetDmvaIMDPersonMaxWidth(unsigned int value);
int SetDmvaIMDPersonMaxHeight(unsigned int value);
int SetDmvaIMDVehicleMaxWidth(unsigned int value);
int SetDmvaIMDVehicleMaxHeight(unsigned int value);
int SetDmvaIMDNumZones(unsigned int value);
int SetDmvaIMDZoneEnable(unsigned int value);
int SetDmvaIMDZone1Numsides(unsigned int value);
int SetDmvaIMDZone2Numsides(unsigned int value);
int SetDmvaIMDZone3Numsides(unsigned int value);
int SetDmvaIMDZone4Numsides(unsigned int value);
int SetDmvaIMDZone1_x01(unsigned int value);
int SetDmvaIMDZone1_x02(unsigned int value);
int SetDmvaIMDZone1_x03(unsigned int value);
int SetDmvaIMDZone1_x04(unsigned int value);
int SetDmvaIMDZone1_x05(unsigned int value);
int SetDmvaIMDZone1_x06(unsigned int value);
int SetDmvaIMDZone1_x07(unsigned int value);
int SetDmvaIMDZone1_x08(unsigned int value);
int SetDmvaIMDZone1_x09(unsigned int value);
int SetDmvaIMDZone1_x10(unsigned int value);
int SetDmvaIMDZone1_x11(unsigned int value);
int SetDmvaIMDZone1_x12(unsigned int value);
int SetDmvaIMDZone1_x13(unsigned int value);
int SetDmvaIMDZone1_x14(unsigned int value);
int SetDmvaIMDZone1_x15(unsigned int value);
int SetDmvaIMDZone1_x16(unsigned int value);
int SetDmvaIMDZone1_y01(unsigned int value);
int SetDmvaIMDZone1_y02(unsigned int value);
int SetDmvaIMDZone1_y03(unsigned int value);
int SetDmvaIMDZone1_y04(unsigned int value);
int SetDmvaIMDZone1_y05(unsigned int value);
int SetDmvaIMDZone1_y06(unsigned int value);
int SetDmvaIMDZone1_y07(unsigned int value);
int SetDmvaIMDZone1_y08(unsigned int value);
int SetDmvaIMDZone1_y09(unsigned int value);
int SetDmvaIMDZone1_y10(unsigned int value);
int SetDmvaIMDZone1_y11(unsigned int value);
int SetDmvaIMDZone1_y12(unsigned int value);
int SetDmvaIMDZone1_y13(unsigned int value);
int SetDmvaIMDZone1_y14(unsigned int value);
int SetDmvaIMDZone1_y15(unsigned int value);
int SetDmvaIMDZone1_y16(unsigned int value);
int SetDmvaIMDZone2_x01(unsigned int value);
int SetDmvaIMDZone2_x02(unsigned int value);
int SetDmvaIMDZone2_x03(unsigned int value);
int SetDmvaIMDZone2_x04(unsigned int value);
int SetDmvaIMDZone2_x05(unsigned int value);
int SetDmvaIMDZone2_x06(unsigned int value);
int SetDmvaIMDZone2_x07(unsigned int value);
int SetDmvaIMDZone2_x08(unsigned int value);
int SetDmvaIMDZone2_x09(unsigned int value);
int SetDmvaIMDZone2_x10(unsigned int value);
int SetDmvaIMDZone2_x11(unsigned int value);
int SetDmvaIMDZone2_x12(unsigned int value);
int SetDmvaIMDZone2_x13(unsigned int value);
int SetDmvaIMDZone2_x14(unsigned int value);
int SetDmvaIMDZone2_x15(unsigned int value);
int SetDmvaIMDZone2_x16(unsigned int value);
int SetDmvaIMDZone2_y01(unsigned int value);
int SetDmvaIMDZone2_y02(unsigned int value);
int SetDmvaIMDZone2_y03(unsigned int value);
int SetDmvaIMDZone2_y04(unsigned int value);
int SetDmvaIMDZone2_y05(unsigned int value);
int SetDmvaIMDZone2_y06(unsigned int value);
int SetDmvaIMDZone2_y07(unsigned int value);
int SetDmvaIMDZone2_y08(unsigned int value);
int SetDmvaIMDZone2_y09(unsigned int value);
int SetDmvaIMDZone2_y10(unsigned int value);
int SetDmvaIMDZone2_y11(unsigned int value);
int SetDmvaIMDZone2_y12(unsigned int value);
int SetDmvaIMDZone2_y13(unsigned int value);
int SetDmvaIMDZone2_y14(unsigned int value);
int SetDmvaIMDZone2_y15(unsigned int value);
int SetDmvaIMDZone2_y16(unsigned int value);

int SetDmvaIMDZone3_x01(unsigned int value);
int SetDmvaIMDZone3_x02(unsigned int value);
int SetDmvaIMDZone3_x03(unsigned int value);
int SetDmvaIMDZone3_x04(unsigned int value);
int SetDmvaIMDZone3_x05(unsigned int value);
int SetDmvaIMDZone3_x06(unsigned int value);
int SetDmvaIMDZone3_x07(unsigned int value);
int SetDmvaIMDZone3_x08(unsigned int value);
int SetDmvaIMDZone3_x09(unsigned int value);
int SetDmvaIMDZone3_x10(unsigned int value);
int SetDmvaIMDZone3_x11(unsigned int value);
int SetDmvaIMDZone3_x12(unsigned int value);
int SetDmvaIMDZone3_x13(unsigned int value);
int SetDmvaIMDZone3_x14(unsigned int value);
int SetDmvaIMDZone3_x15(unsigned int value);
int SetDmvaIMDZone3_x16(unsigned int value);
int SetDmvaIMDZone3_y01(unsigned int value);
int SetDmvaIMDZone3_y02(unsigned int value);
int SetDmvaIMDZone3_y03(unsigned int value);
int SetDmvaIMDZone3_y04(unsigned int value);
int SetDmvaIMDZone3_y05(unsigned int value);
int SetDmvaIMDZone3_y06(unsigned int value);
int SetDmvaIMDZone3_y07(unsigned int value);
int SetDmvaIMDZone3_y08(unsigned int value);
int SetDmvaIMDZone3_y09(unsigned int value);
int SetDmvaIMDZone3_y10(unsigned int value);
int SetDmvaIMDZone3_y11(unsigned int value);
int SetDmvaIMDZone3_y12(unsigned int value);
int SetDmvaIMDZone3_y13(unsigned int value);
int SetDmvaIMDZone3_y14(unsigned int value);
int SetDmvaIMDZone3_y15(unsigned int value);
int SetDmvaIMDZone3_y16(unsigned int value);
int SetDmvaIMDZone4_x01(unsigned int value);
int SetDmvaIMDZone4_x02(unsigned int value);
int SetDmvaIMDZone4_x03(unsigned int value);
int SetDmvaIMDZone4_x04(unsigned int value);
int SetDmvaIMDZone4_x05(unsigned int value);
int SetDmvaIMDZone4_x06(unsigned int value);
int SetDmvaIMDZone4_x07(unsigned int value);
int SetDmvaIMDZone4_x08(unsigned int value);
int SetDmvaIMDZone4_x09(unsigned int value);
int SetDmvaIMDZone4_x10(unsigned int value);
int SetDmvaIMDZone4_x11(unsigned int value);
int SetDmvaIMDZone4_x12(unsigned int value);
int SetDmvaIMDZone4_x13(unsigned int value);
int SetDmvaIMDZone4_x14(unsigned int value);
int SetDmvaIMDZone4_x15(unsigned int value);
int SetDmvaIMDZone4_x16(unsigned int value);
int SetDmvaIMDZone4_y01(unsigned int value);
int SetDmvaIMDZone4_y02(unsigned int value);
int SetDmvaIMDZone4_y03(unsigned int value);
int SetDmvaIMDZone4_y04(unsigned int value);
int SetDmvaIMDZone4_y05(unsigned int value);
int SetDmvaIMDZone4_y06(unsigned int value);
int SetDmvaIMDZone4_y07(unsigned int value);
int SetDmvaIMDZone4_y08(unsigned int value);
int SetDmvaIMDZone4_y09(unsigned int value);
int SetDmvaIMDZone4_y10(unsigned int value);
int SetDmvaIMDZone4_y11(unsigned int value);
int SetDmvaIMDZone4_y12(unsigned int value);
int SetDmvaIMDZone4_y13(unsigned int value);
int SetDmvaIMDZone4_y14(unsigned int value);
int SetDmvaIMDZone4_y15(unsigned int value);
int SetDmvaIMDZone4_y16(unsigned int value);
int SetDmvaIMDZone1_Label(void * buf, int length);
int SetDmvaIMDZone2_Label(void * buf, int length);
int SetDmvaIMDZone3_Label(void * buf, int length);
int SetDmvaIMDZone4_Label(void * buf, int length);

/* DMVA IMD save/load params */
int SetDmvaIMDSave(void * buf, int length);
int SetDmvaIMDLoad(unsigned int value);

/* DMVA Scheduler */
int SetDmvaSchEnable(unsigned int value);
int SetDmvaSchRuleType(unsigned int value);
int SetDmvaSchLoadSetting(unsigned int value);
int SetDmvaSchRuleLabel(void * buf, int length);
int SetDmvaSchStartDay(unsigned int value);
int SetDmvaSchStartTime(unsigned int value);
int SetDmvaSchEndDay(unsigned int value);
int SetDmvaSchEndTime(unsigned int value);
int SetDmvaSchStart(unsigned int value);
int SetDmvaSchDelRule(void * buf, int length);

/* DMVA Enc Roi Enable */
int SetDmvaEncRoiEnable(unsigned int value);
int SetAudioReceiverEnable(unsigned char value);
int SetAudioSeverIp(void * buf, int length);


/******** Remote Debug Client ************/
void EnvSendRemoteDebugQuitCmd();
void EnvSendRemoteDebugSerialCmd();
int EnvSendRemoteDebugSocketCmd(unsigned int port,char *ip);
void EnvSendRemoteDebugNONECmd();
#endif /*__AV_SERVER_CTRL_H__*/
