/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file sys_msg_drv.h
* @brief System message driver
*/
#ifndef _SYS_MSG_DRV_H_
#define _SYS_MSG_DRV_H_
#include <Msg_Def.h>
/**
* @defgroup SYS_MSG_DRV System message driver.
* @brief APIs to control system server.

* This is the communication interface to control system server. The interface
* is implemented by Linux POSIX queue and share memory. So it will pause you
*  thread until command result return.
* @note Call SysDrvInit() at initial, and call SysDrvExit() at the end of
 process.

* @see FILE_MSG_DRV
 * @{
*/
void SetSysProcId(int iProcId);
void SaveSysMsgId(int qid);
/* This function should be called at process initial !!! */
int SysDrvInit(int proc_id);
int SysDrvExit();
/* API */
void SendSysQuitCmd();
int SetSysCommon(void * data, unsigned int len, int cmd);
int GetDhcpEn(void *data, unsigned int len);
int GetHttpPort(void *data, unsigned int len);
int GetSntpFqdn(void *data, unsigned int len);
int SetSysDaylight(void *data, unsigned int len);
int SetSysTimeZone(void *data, unsigned int len);
int GetTitle(void *data, unsigned int len);
int GetMAC(void *data, unsigned int len);
int GetIP(void *data, unsigned int len);
int GetGateway(void *data, unsigned int len);
int GetNetmask(void *data, unsigned int len);
int GetFtpServerIp(void *data, unsigned int len);
int GetFtpUsername(void *data, unsigned int len);
int GetFtpPassword(void *data, unsigned int len);
int GetFtpFoldername(void *data, unsigned int len);
int GetFtpImageAmount(void *data, unsigned int len);
int GetFtpPid(void *data, unsigned int len);
int GetSysFtpPort(void *data, unsigned int len);
int GetSmtpServerIp(void *data, unsigned int len);
int GetSmtpServerPort(void *data, unsigned int len);
int GetSmtpAuthentication(void *data, unsigned int len);
int GetsmtpUsername(void *data, unsigned int len);
int GetSmtpPassword(void *data, unsigned int len);
int GetSmtpSenderEmail(void *data, unsigned int len);
int GetSmtpReceiverEmail(void *data, unsigned int len);
int SysGetSdFileName(void *data, unsigned int len);
int GetSmtpCC(void *data, unsigned int len);
int GetSmtpSubject(void *data, unsigned int len);
int GetSmtpText(void *data, unsigned int len);
int GetSmtpAttachments(void *data, unsigned int len);
int GetSmtpView(void *data, unsigned int len);
int GetSysDhcpConfig(void *data, unsigned int len);
int GetSysUpnpPort(void *data, unsigned int len);
int GetDns(void *data, unsigned int len);
int SetNetMask(void *data, unsigned int len);
int SetIp(void *data, unsigned int len);
int SetHttpPort(void *data, unsigned int len);
int SetTitle(void *data, unsigned int len);
int SetGateway(void *data, unsigned int len);
int SetDhcpEnable(void *data, unsigned int len);
int SetSysFtpFqdn(void *data, unsigned int len);
int SetFtpUsername(void *data, unsigned int len);
int SetFtpPassword(void *data, unsigned int len);
int SetFtpFoldname(void *data, unsigned int len);
int SetFtpImageacount(void *data, unsigned int len);
int SetFtpPid(void *data, unsigned int len);
int SetSysFtpPort(void *data, unsigned int len);
int SetSmtpServerIp(void *data, unsigned int len);
int SetSmtpServerPort(void *data, unsigned int len);
int SetSmtpAuthentication(void *data, unsigned int len);
int SetSmtpUsername(void *data, unsigned int len);
int SetSmtpPassword(void *data, unsigned int len);
int SetSmtpSenderEmail(void *data, unsigned int len);
int SetSmtpReceiverEmail(void *data, unsigned int len);
int SetSmtpCC(void *data, unsigned int len);
int SetSmtpSubject(void *data, unsigned int len);
int SetSmtpText(void *data, unsigned int len);
int SetSmtpAttachments(void *data, unsigned int len);
int SetSmtpView(void *data, unsigned int len);
int SetDns(void *data, unsigned int len);
int SetSntpServer(void *data, unsigned int len);
int SetSysDayNight(void *data, unsigned int len);
int SetSysWhiteBalance(void *data, unsigned int len);
int SetSysBacklight(void *data, unsigned int len);
int SetSysBrightness(void *data, unsigned int len);
int SetSysContrast(void *data, unsigned int len);
int SetSysSaturation(void *data, unsigned int len);
int SetSysSharpness(void *data, unsigned int len);
int SetSysClearSysLog(void *data, unsigned int len);
int SetSysClearAccessLog(void *data, unsigned int len);
int SetSysJpegQuality(void *data, unsigned int len);
int SetSysMirror(void *data, unsigned int len);
int SetSysDemoCfg(void *data, unsigned int len);
int SetSysHistogram(void *data, unsigned int len);
int SetSysDynRange(void *data, unsigned int len);
int SetSysVideoCodecRes(void *data, unsigned int len);
int SetSysImageSource(void *data, unsigned int len);
int SetSysVideoMode(void *data, unsigned int len);
int SetSysSchedule(void *data, unsigned int len);
int DelSysSchedule(void *data, unsigned int len);
int SetSysSDAlarmEnable(void *data, unsigned int len);
int SetSysFtpAlarmEnable(void *data, unsigned int len);
int SetSysSmtpAlarmEnable(void *data, unsigned int len);
int SetSysAlarmDuration(void *data, unsigned int len);
int SetAVIDuration(void *data, unsigned int len);
int SetAVIFormat(void *data, unsigned int len);
int SetFTPFileFormat(void *data, unsigned int len);
int SetSDFileFormat(void *data, unsigned int len);
int SetAttFileFormat(void *data, unsigned int len);
int SetAudioEnable(void *data, unsigned int len);
int SetASmtpAttach(void *data, unsigned int len);
int SetSysLostAlarm(void *data, unsigned int len);
int SetRftpEnable(void *data, unsigned int len);
int SetSdReEnable(void *data, unsigned int len);
int SetSysImage2A(void *data, unsigned int len);
int SetSysImage2AType(void *data, unsigned int len);
int SetSysBinning(void *data, unsigned int len);
int SetSysBLC(void *data, unsigned int len);
int SetMotionEnable(void *data, unsigned int len);
int SetMotionCEnable(void *data, unsigned int len);
int SetMotionLevel(void *data, unsigned int len);
int SetMotionCValue(void *data, unsigned int len);
int SetMotionBlock(void *data, unsigned int len);
int SetMPEG41Bitrate(void *data, unsigned int len);
int SetMPEG42Bitrate(void *data, unsigned int len);
int SetImageDefault(void *data, unsigned int len);
int DoSysLogin(void *data, unsigned int len);
int GetSysUserAuthority(void *data, unsigned int len);
int SysAddUser(void *data, unsigned int len);
int SysDelUser(void *data, unsigned int len);
int SetGIOInEnable(void *data, unsigned int len);
int SetGIOInType(void *data, unsigned int len);
int SetGIOOutEnable(void *data, unsigned int len);
int SetGIOOutType(void *data, unsigned int len);
int SetAudioinVolume(void *data, unsigned int len);
int SetMPEG41Framerate(void *data, unsigned int len);
int SetMPEG42Framerate(void *data, unsigned int len);
int SetJPEGFramerate(void *data, unsigned int len);
int SysSdFormat(void *data, unsigned int len);
int SysSdUnmount(void *data, unsigned int len);
int SysScheduleStop(void *data, unsigned int len);
int SysScheduleResume(void *data, unsigned int len);
int SetSysfdetect(void * data, unsigned int len);
int SetSysfdx(void * data, unsigned int len);
int SetSysfdy(void * data, unsigned int len);
int SetSysfdw(void * data, unsigned int len);
int SetSysfdh(void * data, unsigned int len);
int SetSysfdconf(void * data, unsigned int len);
int SetSysfddir(void * data, unsigned int len);
int SetSysfrecog(void * data, unsigned int len);
int SetSysfrconf(void * data, unsigned int len);
int SetSysfrdbase(void * data, unsigned int len);
int SetSyspmask(void * data, unsigned int len);
int SetSyspmaskopt(void * data, unsigned int len);
int SysDoBootProc(void *data, unsigned int len);
int SetSysVideoCodecCombo(void *data, unsigned int len);
int SetSysClickSnapFilename(void *data, unsigned int len);
int SetSysClickSnapStorage(void *data, unsigned int len);
int SetSysRateControl1(void *data, unsigned int len);
int SetSysRateControl2(void *data, unsigned int len);
int SetSysDateStampEnable1(void *data, unsigned int len);
int SetSysDateStampEnable2(void *data, unsigned int len);
int SetSysDateStampEnable3(void *data, unsigned int len);
int SetSysTimeStampEnable1(void *data, unsigned int len);
int SetSysTimeStampEnable2(void *data, unsigned int len);
int SetSysTimeStampEnable3(void *data, unsigned int len);
int SetSysLogoEnable1(void *data, unsigned int len);
int SetSysLogoEnable2(void *data, unsigned int len);
int SetSysLogoEnable3(void *data, unsigned int len);
int SetSysLogoPosition1(void *data, unsigned int len);
int SetSysLogoPosition2(void *data, unsigned int len);
int SetSysLogoPosition3(void *data, unsigned int len);
int SetSysTextEnable1(void *data, unsigned int len);
int SetSysTextEnable2(void *data, unsigned int len);
int SetSysTextEnable3(void *data, unsigned int len);
int SetSysTextPosition1(void *data, unsigned int len);
int SetSysTextPosition2(void *data, unsigned int len);
int SetSysTextPosition3(void *data, unsigned int len);
int SetSysOverlayText1(void *data, unsigned int len);
int SetSysOverlayText2(void *data, unsigned int len);
int SetSysOverlayText3(void *data, unsigned int len);
int SetSysEncryption(void *data, unsigned int len);
int SetSysLocalDisplay(void *data, unsigned int len);
int SetSysIpratio1(void *data, unsigned int len);
int SetSysIpratio2(void *data, unsigned int len);
int SetSysIpratio3(void *data, unsigned int len);
int SetSysForceIframe1(void *data, unsigned int len);
int SetSysForceIframe2(void *data, unsigned int len);
int SetSysForceIframe3(void *data, unsigned int len);
int SetSysQPInit1(void *data, unsigned int len);
int SetSysQPInit2(void *data, unsigned int len);
int SetSysQPInit3(void *data, unsigned int len);
int SetSysQPMin1(void *data, unsigned int len);
int SetSysQPMin2(void *data, unsigned int len);
int SetSysQPMin3(void *data, unsigned int len);
int SetSysQPMax1(void *data, unsigned int len);
int SetSysQPMax2(void *data, unsigned int len);
int SetSysQPMax3(void *data, unsigned int len);
int SetSysMEConfig1(void *data, unsigned int len);
int SetSysMEConfig2(void *data, unsigned int len);
int SetSysMEConfig3(void *data, unsigned int len);
int SetSysPacketSize1(void *data, unsigned int len);
int SetSysPacketSize2(void *data, unsigned int len);
int SetSysPacketSize3(void *data, unsigned int len);
int SetSysROIEnable1(void *data, unsigned int len);
int SetSysROIEnable2(void *data, unsigned int len);
int SetSysROIEnable3(void *data, unsigned int len);
int SetSysStr1X1(void *data, unsigned int len);
int SetSysStr1Y1(void *data, unsigned int len);
int SetSysStr1W1(void *data, unsigned int len);
int SetSysStr1H1(void *data, unsigned int len);
int SetSysStr1X2(void *data, unsigned int len);
int SetSysStr1Y2(void *data, unsigned int len);
int SetSysStr1W2(void *data, unsigned int len);
int SetSysStr1H2(void *data, unsigned int len);
int SetSysStr1X3(void *data, unsigned int len);
int SetSysStr1Y3(void *data, unsigned int len);
int SetSysStr1W3(void *data, unsigned int len);
int SetSysStr1H3(void *data, unsigned int len);
int SetSysStr2X1(void *data, unsigned int len);
int SetSysStr2Y1(void *data, unsigned int len);
int SetSysStr2W1(void *data, unsigned int len);
int SetSysStr2H1(void *data, unsigned int len);
int SetSysStr2X2(void *data, unsigned int len);
int SetSysStr2Y2(void *data, unsigned int len);
int SetSysStr2W2(void *data, unsigned int len);
int SetSysStr2H2(void *data, unsigned int len);
int SetSysStr2X3(void *data, unsigned int len);
int SetSysStr2Y3(void *data, unsigned int len);
int SetSysStr2W3(void *data, unsigned int len);
int SetSysStr2H3(void *data, unsigned int len);
int SetSysStr3X1(void *data, unsigned int len);
int SetSysStr3Y1(void *data, unsigned int len);
int SetSysStr3W1(void *data, unsigned int len);
int SetSysStr3H1(void *data, unsigned int len);
int SetSysStr3X2(void *data, unsigned int len);
int SetSysStr3Y2(void *data, unsigned int len);
int SetSysStr3W2(void *data, unsigned int len);
int SetSysStr3H2(void *data, unsigned int len);
int SetSysStr3X3(void *data, unsigned int len);
int SetSysStr3Y3(void *data, unsigned int len);
int SetSysStr3W3(void *data, unsigned int len);
int SetSysStr3H3(void *data, unsigned int len);
int SetSysFramerate1(void *data, unsigned int len);
int SetSysFramerate2(void *data, unsigned int len);
int SetSysFramerate3(void *data, unsigned int len);
int SetSysMulticast(void *data, unsigned int len);
int SetSysAudioReceiverEnable(void *data, unsigned int len);
int SetSysAudioServerIp(void *data, unsigned int len);
int CreateMedProfilename(void *data, unsigned int len);
int CreateMedProfiletoken(void *data, unsigned int len);
int SetHostname(void *data, unsigned int len);
int CreateMedProfile(void *data, unsigned int len);
int AddVideoSourceConfiguration(void *data,unsigned int len);
int SetVideoSourceConfiguration(void *data,unsigned int len);
int AddVideoEncoderConfiguration(void *data,unsigned int len);
int SetVideoEncoderConfiguration(void *data,unsigned int len);
int DelProfile(void *data,unsigned int len);
int SetDiscoveryMode(void *data,unsigned int len);
int AddScopes(void *data,unsigned int len);
int DeleteScopes(void *data,unsigned int len);
int SetVideoAnalytics(void *data,unsigned int len);
int SetVideoOutput(void *data,unsigned int len);
int SetRelays(void *data,unsigned int len);
int SetSysAutoIrisVoltage(void *data, unsigned int len);

/* DMVA Specific System Drive Functions */
int SetSysDmvaEnable(void * data, unsigned int len);
int SetSysDmvaExptAlgoFrameRate(void * data, unsigned int len);
int SetSysDmvaExptAlgoDetectFreq(void * data, unsigned int len);
int SetSysDmvaExptEvtRecordEnable(void * data, unsigned int len);
int SetSysDmvaExptSmetaTrackerEnable(void * data, unsigned int len);

/* DMVA TZ Specific System Drive Functions */
int SetSysDmvaTZSensitivity(void * data, unsigned int len);
int SetSysDmvaTZPersonMinWidth(void * data, unsigned int len);
int SetSysDmvaTZPersonMinHeight(void * data, unsigned int len);
int SetSysDmvaTZVehicleMinWidth(void * data, unsigned int len);
int SetSysDmvaTZVehicleMinHeight(void * data, unsigned int len);
int SetSysDmvaTZPersonMaxWidth(void * data, unsigned int len);
int SetSysDmvaTZPersonMaxHeight(void * data, unsigned int len);
int SetSysDmvaTZVehicleMaxWidth(void * data, unsigned int len);
int SetSysDmvaTZVehicleMaxHeight(void * data, unsigned int len);
int SetSysDmvaTZDirection(void * data, unsigned int len);
int SetSysDmvaTZPresentAdjust(void * data, unsigned int len);
int SetSysDmvaTZEnable(void * data, unsigned int len);
int SetSysDmvaTZZone1Numsides(void * data, unsigned int len);
int SetSysDmvaTZZone2Numsides(void * data, unsigned int len);
int SetSysDmvaTZZone1_x01(void * data, unsigned int len);
int SetSysDmvaTZZone1_x02(void * data, unsigned int len);
int SetSysDmvaTZZone1_x03(void * data, unsigned int len);
int SetSysDmvaTZZone1_x04(void * data, unsigned int len);
int SetSysDmvaTZZone1_x05(void * data, unsigned int len);
int SetSysDmvaTZZone1_x06(void * data, unsigned int len);
int SetSysDmvaTZZone1_x07(void * data, unsigned int len);
int SetSysDmvaTZZone1_x08(void * data, unsigned int len);
int SetSysDmvaTZZone1_x09(void * data, unsigned int len);
int SetSysDmvaTZZone1_x10(void * data, unsigned int len);
int SetSysDmvaTZZone1_x11(void * data, unsigned int len);
int SetSysDmvaTZZone1_x12(void * data, unsigned int len);
int SetSysDmvaTZZone1_x13(void * data, unsigned int len);
int SetSysDmvaTZZone1_x14(void * data, unsigned int len);
int SetSysDmvaTZZone1_x15(void * data, unsigned int len);
int SetSysDmvaTZZone1_x16(void * data, unsigned int len);
int SetSysDmvaTZZone1_y01(void * data, unsigned int len);
int SetSysDmvaTZZone1_y02(void * data, unsigned int len);
int SetSysDmvaTZZone1_y03(void * data, unsigned int len);
int SetSysDmvaTZZone1_y04(void * data, unsigned int len);
int SetSysDmvaTZZone1_y05(void * data, unsigned int len);
int SetSysDmvaTZZone1_y06(void * data, unsigned int len);
int SetSysDmvaTZZone1_y07(void * data, unsigned int len);
int SetSysDmvaTZZone1_y08(void * data, unsigned int len);
int SetSysDmvaTZZone1_y09(void * data, unsigned int len);
int SetSysDmvaTZZone1_y10(void * data, unsigned int len);
int SetSysDmvaTZZone1_y11(void * data, unsigned int len);
int SetSysDmvaTZZone1_y12(void * data, unsigned int len);
int SetSysDmvaTZZone1_y13(void * data, unsigned int len);
int SetSysDmvaTZZone1_y14(void * data, unsigned int len);
int SetSysDmvaTZZone1_y15(void * data, unsigned int len);
int SetSysDmvaTZZone1_y16(void * data, unsigned int len);
int SetSysDmvaTZZone2_x01(void * data, unsigned int len);
int SetSysDmvaTZZone2_x02(void * data, unsigned int len);
int SetSysDmvaTZZone2_x03(void * data, unsigned int len);
int SetSysDmvaTZZone2_x04(void * data, unsigned int len);
int SetSysDmvaTZZone2_x05(void * data, unsigned int len);
int SetSysDmvaTZZone2_x06(void * data, unsigned int len);
int SetSysDmvaTZZone2_x07(void * data, unsigned int len);
int SetSysDmvaTZZone2_x08(void * data, unsigned int len);
int SetSysDmvaTZZone2_x09(void * data, unsigned int len);
int SetSysDmvaTZZone2_x10(void * data, unsigned int len);
int SetSysDmvaTZZone2_x11(void * data, unsigned int len);
int SetSysDmvaTZZone2_x12(void * data, unsigned int len);
int SetSysDmvaTZZone2_x13(void * data, unsigned int len);
int SetSysDmvaTZZone2_x14(void * data, unsigned int len);
int SetSysDmvaTZZone2_x15(void * data, unsigned int len);
int SetSysDmvaTZZone2_x16(void * data, unsigned int len);
int SetSysDmvaTZZone2_y01(void * data, unsigned int len);
int SetSysDmvaTZZone2_y02(void * data, unsigned int len);
int SetSysDmvaTZZone2_y03(void * data, unsigned int len);
int SetSysDmvaTZZone2_y04(void * data, unsigned int len);
int SetSysDmvaTZZone2_y05(void * data, unsigned int len);
int SetSysDmvaTZZone2_y06(void * data, unsigned int len);
int SetSysDmvaTZZone2_y07(void * data, unsigned int len);
int SetSysDmvaTZZone2_y08(void * data, unsigned int len);
int SetSysDmvaTZZone2_y09(void * data, unsigned int len);
int SetSysDmvaTZZone2_y10(void * data, unsigned int len);
int SetSysDmvaTZZone2_y11(void * data, unsigned int len);
int SetSysDmvaTZZone2_y12(void * data, unsigned int len);
int SetSysDmvaTZZone2_y13(void * data, unsigned int len);
int SetSysDmvaTZZone2_y14(void * data, unsigned int len);
int SetSysDmvaTZZone2_y15(void * data, unsigned int len);
int SetSysDmvaTZZone2_y16(void * data, unsigned int len);
int SetSysDmvaTZSave(void * data, unsigned int len);
int SetSysDmvaTZLoad(void * data, unsigned int len);

/* Set DMVA Main Page specific Parameters */
int SetSysDmvaCamID(void * data, unsigned int len);
int SetSysDmvaGetSchedule(void * data, unsigned int len);
int SetSysDmvaCfgFD(void * data, unsigned int len);
int SetSysDmvaCfgCTDImdSmetaTzOc(void * data, unsigned int len);
int SetSysDmvaDisplayOptions(void * data, unsigned int len);
int SetSysDmvaEventRecordingVAME(void * data, unsigned int len);
int SetSysDmvaEventListActionPlaySendSearchTrash(void * data, unsigned int len);
int SetSysDmvaEventListSelectEvent(void * data, unsigned int len);
int SetSysDmvaEventListArchiveFlag(void * data, unsigned int len);
int SetSysDmvaEventListEventType(void * data, unsigned int len);
int SetSysDmvaEventListStartMonth(void * data, unsigned int len);
int SetSysDmvaEventListStartDay(void * data, unsigned int len);
int SetSysDmvaEventListStartTime(void * data, unsigned int len);
int SetSysDmvaEventListEndMonth(void * data, unsigned int len);
int SetSysDmvaEventListEndDay(void * data, unsigned int len);
int SetSysDmvaEventListEndTime(void * data, unsigned int len);
int SetSysDmvaEventListTimeStamp(void * data, unsigned int len);
int SetSysDmvaEventListEventDetails(void * data, unsigned int len);

/* Set DMVA Advanced Settings Page specific Parameters */
int SetSysDmvaObjectMetaData(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesFontType(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesFontSize(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesFontColor(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesBoxColor(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesCentroidColor(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesActiveZoneColor(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesInactiveZoneColor(void * data, unsigned int len);
int SetSysDmvaOverlayPropertiesVelocityColor(void * data, unsigned int len);
int SetSysDmvaEnvironmentProfileLoad(void * data, unsigned int len);
int SetSysDmvaEnvironmentProfileStore(void * data, unsigned int len);
int SetSysDmvaEnvironmentProfileClear(void * data, unsigned int len);
int SetSysDmvaEnvironmentProfileState(void * data, unsigned int len);
int SetSysDmvaEnvironmentProfileDesc(void * data, unsigned int len);

/* DMVA event delete */
int SetSysDmvaEventDeleteIndex(void * data, unsigned int len);
int SetSysDmvaEventDeleteStartIndex(void * data, unsigned int len);
int SetSysDmvaEventDeleteEndIndex(void * data, unsigned int len);
int SetSysDmvaEventDeleteAll(void * data, unsigned int len);

/* Set DMVA CTD Page specific Parameters */
int SetSysDmvaCfgTDEnv1(void * data, unsigned int len);
int SetSysDmvaCfgTDEnv2(void * data, unsigned int len);
int SetSysDmvaCfgTDSensitivity(void * data, unsigned int len);
int SetSysDmvaCfgTDResetTime(void * data, unsigned int len);

/* Set DMVA OC Page specific Parameters */
int SetSysDmvaOCSensitivity(void * data, unsigned int len);
int SetSysDmvaOCObjectWidth(void * data, unsigned int len);
int SetSysDmvaOCObjectHeight(void * data, unsigned int len);
int SetSysDmvaOCDirection(void * data, unsigned int len);
int SetSysDmvaOCEnable(void * data, unsigned int len);
int SetSysDmvaOCLeftRight(void * data, unsigned int len);
int SetSysDmvaOCTopBottom(void * data, unsigned int len);
int SetSysDmvaOCSave(void * data, unsigned int len);
int SetSysDmvaOCLoad(void * data, unsigned int len);

/* Set DMVA SMETA Page specific Parameters */
int SetSysDmvaSMETASensitivity(void * data, unsigned int len);
int SetSysDmvaSMETAEnableStreamingData(void * data, unsigned int len);
int SetSysDmvaSMETAStreamBB(void * data, unsigned int len);
int SetSysDmvaSMETAStreamVelocity(void * data, unsigned int len);
int SetSysDmvaSMETAStreamHistogram(void * data, unsigned int len);
int SetSysDmvaSMETAStreamMoments(void * data, unsigned int len);
int SetSysDmvaSMETAPresentAdjust(void * data, unsigned int len);
int SetSysDmvaSMETAPersonMinWidth(void * data, unsigned int len);
int SetSysDmvaSMETAPersonMinHeight(void * data, unsigned int len);
int SetSysDmvaSMETAVehicleMinWidth(void * data, unsigned int len);
int SetSysDmvaSMETAVehicleMinHeight(void * data, unsigned int len);
int SetSysDmvaSMETAPersonMaxWidth(void * data, unsigned int len);
int SetSysDmvaSMETAPersonMaxHeight(void * data, unsigned int len);
int SetSysDmvaSMETAVehicleMaxWidth(void * data, unsigned int len);
int SetSysDmvaSMETAVehicleMaxHeight(void * data, unsigned int len);
int SetSysDmvaSMETANumZones(void * data, unsigned int len);
int SetSysDmvaSMETAZone1Numsides(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x01(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x02(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x03(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x04(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x05(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x06(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x07(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x08(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x09(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x10(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x11(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x12(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x13(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x14(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x15(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_x16(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y01(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y02(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y03(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y04(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y05(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y06(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y07(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y08(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y09(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y10(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y11(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y12(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y13(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y14(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y15(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_y16(void * data, unsigned int len);
int SetSysDmvaSMETAZone1_Label(void * data, unsigned int len);
int SetSysDmvaSMETASave(void * data, unsigned int len);
int SetSysDmvaSMETALoad(void * data, unsigned int len);

/* Set DMVA IMD Page specific Parameters */
int SetSysDmvaIMDSensitivity(void * data, unsigned int len);
int SetSysDmvaIMDPresentAdjust(void * data, unsigned int len);
int SetSysDmvaIMDPersonMinWidth(void * data, unsigned int len);
int SetSysDmvaIMDPersonMinHeight(void * data, unsigned int len);
int SetSysDmvaIMDVehicleMinWidth(void * data, unsigned int len);
int SetSysDmvaIMDVehicleMinHeight(void * data, unsigned int len);
int SetSysDmvaIMDPersonMaxWidth(void * data, unsigned int len);
int SetSysDmvaIMDPersonMaxHeight(void * data, unsigned int len);
int SetSysDmvaIMDVehicleMaxWidth(void * data, unsigned int len);
int SetSysDmvaIMDVehicleMaxHeight(void * data, unsigned int len);
int SetSysDmvaIMDNumZones(void * data, unsigned int len);
int SetSysDmvaIMDZoneEnable(void * data, unsigned int len);
int SetSysDmvaIMDZone1Numsides(void * data, unsigned int len);
int SetSysDmvaIMDZone2Numsides(void * data, unsigned int len);
int SetSysDmvaIMDZone3Numsides(void * data, unsigned int len);
int SetSysDmvaIMDZone4Numsides(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x01(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x02(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x03(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x04(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x05(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x06(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x07(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x08(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x09(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x10(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x11(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x12(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x13(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x14(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x15(void * data, unsigned int len);
int SetSysDmvaIMDZone1_x16(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y01(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y02(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y03(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y04(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y05(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y06(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y07(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y08(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y09(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y10(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y11(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y12(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y13(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y14(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y15(void * data, unsigned int len);
int SetSysDmvaIMDZone1_y16(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x01(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x02(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x03(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x04(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x05(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x06(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x07(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x08(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x09(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x10(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x11(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x12(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x13(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x14(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x15(void * data, unsigned int len);
int SetSysDmvaIMDZone2_x16(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y01(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y02(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y03(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y04(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y05(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y06(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y07(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y08(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y09(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y10(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y11(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y12(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y13(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y14(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y15(void * data, unsigned int len);
int SetSysDmvaIMDZone2_y16(void * data, unsigned int len);

int SetSysDmvaIMDZone3_x01(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x02(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x03(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x04(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x05(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x06(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x07(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x08(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x09(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x10(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x11(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x12(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x13(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x14(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x15(void * data, unsigned int len);
int SetSysDmvaIMDZone3_x16(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y01(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y02(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y03(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y04(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y05(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y06(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y07(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y08(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y09(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y10(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y11(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y12(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y13(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y14(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y15(void * data, unsigned int len);
int SetSysDmvaIMDZone3_y16(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x01(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x02(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x03(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x04(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x05(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x06(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x07(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x08(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x09(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x10(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x11(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x12(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x13(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x14(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x15(void * data, unsigned int len);
int SetSysDmvaIMDZone4_x16(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y01(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y02(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y03(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y04(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y05(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y06(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y07(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y08(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y09(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y10(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y11(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y12(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y13(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y14(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y15(void * data, unsigned int len);
int SetSysDmvaIMDZone4_y16(void * data, unsigned int len);
int SetSysDmvaIMDZone1_Label(void * data, unsigned int len);
int SetSysDmvaIMDZone2_Label(void * data, unsigned int len);
int SetSysDmvaIMDZone3_Label(void * data, unsigned int len);
int SetSysDmvaIMDZone4_Label(void * data, unsigned int len);
int SetSysDmvaIMDSave(void * data, unsigned int len);
int SetSysDmvaIMDLoad(void * data, unsigned int len);

/* DMVA Scheduler */
int SetSysDmvaSchEnable(void * data, unsigned int len);
int SetSysDmvaSchRuleType(void * data, unsigned int len);
int SetSysDmvaSchLoadSetting(void * data, unsigned int len);
int SetSysDmvaSchRuleLabel(void * data, unsigned int len);
int SetSysDmvaSchStartDay(void * data, unsigned int len);
int SetSysDmvaSchStartTime(void * data, unsigned int len);
int SetSysDmvaSchEndDay(void * data, unsigned int len);
int SetSysDmvaSchEndTime(void * data, unsigned int len);
int SetSysDmvaSchStart(void * data, unsigned int len);
int SetSysDmvaSchDelRule(void * data, unsigned int len);

/* DMVA Enc Roi Enable */
int SetSysDmvaEncRoiEnable(void * data, unsigned int len);




/****** add by wuzhongbing 2016-10-17*********/
int SetImaging(void *data,unsigned int len);
int RemoveVideoEncoderConfiguration(void *data,unsigned int len);
int SetAudioSourceConfiguration(void *data,unsigned int len);
int AddAudioSourceConfiguration(void *data,unsigned int len);
int AddAudioEncoderConfiguration(void *data,unsigned int len);
int SetAudioEncoderConfiguration(void *data,unsigned int len);
int RemoveAudioSourceConfiguration(void *data,unsigned int len);
int RemoveAudioEncoderConfiguration(void *data,unsigned int len);
int SetScopes(void *data,unsigned int len);
int SetRelayLogicalState(void *data,unsigned int len);
int SetMetadata(void *data,unsigned int len);
int AddMetadata(void *data,unsigned int len);
int DelMetadata(void *data,unsigned int len);

#endif /* _SYS_MSG_DRV_H_ */
/**
* @}
* @page SYS_MSG_DRV_HOW How to use system message driver?

-# Add new message ID in \ref Sys_Msg_Def.h if needed.
-# Call the API SysDrvInit() with the ID you added to initialize file
	message driver.
-# Now you can use any system message driver functions as you want.
-# Call the API SysDrvExit() to cleanup system message driver.

* Your code may like following:
* @code
#include <sys_msg_drv.h>
int main()
{
	if(SysDrvInit(SYS_BOA_MSG) < 0){
		exit(1);
	}
	// Use system message driver API here.
	SysDrvExit();
	return 0;
}
* @endcode
* \b See \b also \ref FILE_MSG_DRV_HOW
*/
