/* ===========================================================================
* @file sysctrl.c
*
* @path $(IPNCPATH)/util/
*
* @desc System control interface.
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include "sysctrl.h"
#include <sys_env_type.h>
#include "sys_msg_drv.h"
/**
* @brief Send commands.

* Send commands to system server.
* @param field [I ] command.
* @param *data [I ] pointer to data needed for command.
* @param len [I ] Data size.
* @return Function to excute command.
* @retval -1 Fail to send command.
*/
int ControlSystemData(unsigned int field, void *data, unsigned int len)
{
	int ret = 0;
	if(len == 0 || data == NULL)
		return -1;
	switch(field){
		case SFIELD_GET_DHCPC_ENABLE:
			ret = GetDhcpEn(data, len);
			break;
		case SFIELD_GET_HTTPPORT:
			ret = GetHttpPort(data, len);
			break;
		case SFIELD_GET_SNTP_FQDN:
			ret = GetSntpFqdn(data, len);
			break;
		case SFIELD_GET_TITLE:
			ret = GetTitle(data, len);
			break;
		case SFIELD_GET_MAC:
			ret = GetMAC(data, len);
			break;
		case SFIELD_GET_IP:
			ret = GetIP(data, len);
			break;
		case SFIELD_GET_GATEWAY:
			ret = GetGateway(data, len);
			break;
		case SFIELD_GET_NETMASK:
			ret = GetNetmask(data, len);
			break;
		case SFIELD_GET_FTP_SERVER_IP:
			ret = GetFtpServerIp(data, len);
			break;
		case SFIELD_GET_FTP_USERNAME:
			ret = GetFtpUsername(data, len);
			break;
		case SFIELD_GET_FTP_PASSWORD:
			ret = GetFtpPassword(data, len);
			break;
		case SFIELD_GET_FTP_FOLDNAME:
			ret = GetFtpFoldername(data, len);
			break;
		case SFIELD_GET_FTP_IMAGEAMOUNT:
			ret = GetFtpImageAmount(data, len);
			break;
		case SFIELD_GET_FTP_PID:
			ret = GetFtpPid(data, len);
			break;
		case SFIELD_GET_FTP_PORT:
			ret = GetSysFtpPort(data, len);
			break;
		case SFIELD_GET_SMTP_SERVER_IP:
			ret = GetSmtpServerIp(data, len);
			break;
		case SFIELD_GET_SMTP_PORT:
			ret = GetSmtpServerPort(data, len);
			break;
		case SFIELD_GET_SMTP_AUTHENTICATION:
			ret = GetSmtpAuthentication(data, len);
			break;
		case SFIELD_GET_SMTP_USERNAME:
			ret = GetsmtpUsername(data, len);
			break;
		case SFIELD_GET_SMTP_PASSWORD:
			ret = GetSmtpPassword(data, len);
			break;
		case SFIELD_GET_SMTP_SENDER_EMAIL_ADDRESS:
			ret = GetSmtpSenderEmail(data, len);
			break;
		case SFIELD_GET_SMTP_RECEIVER_EMAIL_ADDRESS:
			ret = GetSmtpReceiverEmail(data, len);
			break;
		case SFIELD_GET_SMTP_CC:
			ret = GetSmtpCC(data, len);
			break;
		case SFIELD_GET_SD_FILE_NAME:
			ret = SysGetSdFileName(data, len);
			break;
		case SFIELD_GET_SMTP_SUBJECT:
			ret = GetSmtpSubject(data, len);
			break;
		case SFIELD_GET_SMTP_TEXT:
			ret = GetSmtpText(data, len);
			break;
		case SFIELD_GET_SMTP_ATTACHMENTS:
			ret = GetSmtpAttachments(data, len);
			break;
		case SFIELD_GET_SMTP_VIEW:
			ret = GetSmtpView(data, len);
			break;
		/*case SFIELD_GET_UPNP_PORT:
			ret = GetSysUpnpPort(data, len);
			break;*/
		case SFIELD_SET_NETMASK:
			ret = SetNetMask(data, len);
			break;
		case SFIELD_SET_DAYLIGHT:
			ret = SetSysDaylight(data, len);
			break;
		case SFIELD_SET_TIMEZONE:
			ret = SetSysTimeZone(data, len);
			break;
		case SFIELD_SET_IP:
			ret = SetIp(data, len);
			break;
		case SFIELD_SET_HTTPPORT:
			ret = SetHttpPort(data, len);
			break;
		case SFIELD_SET_TITLE:
			ret = SetTitle(data, len);
			break;
		case SFIELD_SET_GATEWAY:
			ret = SetGateway(data, len);
			break;
		case SFIELD_SET_DHCPC_ENABLE:
			ret = SetDhcpEnable(data, len);
			break;
		case SFIELD_SET_FTP_SERVER_IP:
			ret = SetSysFtpFqdn(data, len);
			break;
		case SFIELD_SET_FTP_USERNAME:
			ret = SetFtpUsername(data, len);
			break;
		case SFIELD_SET_FTP_PASSWORD:
			ret = SetFtpPassword(data, len);
			break;
		case SFIELD_SET_FTP_FOLDNAME:
			ret = SetFtpFoldname(data, len);
			break;
		case SFIELD_SET_FTP_IMAGEAMOUNT:
			ret = SetFtpImageacount(data, len);
			break;
		case SFIELD_SET_FTP_PID:
			ret = SetFtpPid(data, len);
			break;
		case SFIELD_SET_FTP_PORT:
			ret = SetSysFtpPort(data, len);
			break;
		case SFIELD_SET_SMTP_SERVER_IP:
			ret = SetSmtpServerIp(data, len);
			break;
		case SFIELD_SET_SMTP_PORT:
			ret = SetSmtpServerPort(data, len);
			break;
		case SFIELD_SET_SMTP_AUTHENTICATION:
			ret = SetSmtpAuthentication(data, len);
			break;
		case SFIELD_SET_SMTP_USERNAME:
			ret = SetSmtpUsername(data, len);
			break;
		case SFIELD_SET_SMTP_PASSWORD:
			ret = SetSmtpPassword(data, len);
			break;
		case SFIELD_SET_SMTP_SENDER_EMAIL_ADDRESS:
			ret = SetSmtpSenderEmail(data, len);
			break;
		case SFIELD_SET_SMTP_RECEIVER_EMAIL_ADDRESS:
			ret = SetSmtpReceiverEmail(data, len);
			break;
		case SFIELD_SET_SMTP_CC:
			ret = SetSmtpCC(data, len);
			break;
		case SFIELD_SET_SMTP_SUBJECT:
			ret = SetSmtpSubject(data, len);
			break;
		case SFIELD_SET_SMTP_TEXT:
			ret = SetSmtpText(data, len);
			break;
		case SFIELD_SET_SMTP_ATTACHMENTS:
			ret = SetSmtpAttachments(data, len);
			break;
		case SFIELD_SET_SMTP_VIEW:
			ret = SetSmtpView(data, len);
			break;
		case SFIELD_GET_DNS:
			ret = GetDns(data, len);
			break;
		case SFIELD_SET_HOSTNAME:
			ret = SetHostname(data, len);
			break;
		case SFIELD_SET_DNS:
			ret = SetDns(data, len);
			break;
		case SFIELD_SET_SNTP_SERVER:
			ret = SetSntpServer(data, len);
			break;
		case SFIELD_SET_DAY_NIGHT:
			ret = SetSysDayNight(data, len);
			break;
		case SFIELD_SET_WHITE_BALANCE:
			ret = SetSysWhiteBalance(data, len);
			break;
		case SFIELD_SET_BACKLIGHT:
			ret = SetSysBacklight(data, len);
			break;
		case SFIELD_SET_BRIGHTNESS:
			ret = SetSysBrightness(data, len);
			break;
		case SFIELD_SET_CONTRAST:
			ret = SetSysContrast(data, len);
			break;
		case SFIELD_SET_SATURATION:
			ret = SetSysSaturation(data, len);
			break;
		case SFIELD_SET_SHARPNESS:
			ret = SetSysSharpness(data, len);
			break;
		case SFIELD_SET_IMAGING:
			ret = SetImaging(data, len);
			break;
		case SFIELD_SET_VSTAB:
			ret = SetSysCommon(data, len, SYS_MSG_SET_VSTAB);
			break;
		case SFIELD_SET_LDC:
			ret = SetSysCommon(data, len, SYS_MSG_SET_LDC);
			break;
		case SFIELD_SET_VNF:
			ret = SetSysCommon(data, len, SYS_MSG_SET_VNF);
			break;
		case SFIELD_SET_VNFMODE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_VNFMODE);
			break;
		case SFIELD_SET_VNFSTRENGTH:
			ret = SetSysCommon(data, len, SYS_MSG_SET_VNFSTRENGTH);
			break;
		case SFIELD_SET_FRAMECTRL:
			ret = SetSysCommon(data, len, SYS_MSG_SET_FRAMECTRL);
			break;
		case SFIELD_SET_EXPPRIORITY:
			ret = SetSysCommon(data, len, SYS_MSG_SET_EXPPRIORITY);
			break;
 		case SFIELD_SET_AUDIOMODE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_AUDIOMODE);
			break;
		case SFIELD_SET_AUDIO_ENCODE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_AUDIO_ENCODE);
			break;
		case SFIELD_SET_AUDIO_SAMPLERATE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_AUDIO_SAMPLERATE);
			break;
		case SFIELD_SET_AUDIO_BITRATE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_AUDIO_BITRATE);
			break;
		case SFIELD_SET_AUDIO_ALARMLEVEL:
			ret = SetSysCommon(data, len, SYS_MSG_SET_AUDIO_ALARMLEVEL);
			break;
		case SFIELD_SET_AUDIOOUTVOLUME:
			ret = SetSysCommon(data, len, SYS_MSG_SET_AUDIOOUTVOLUME);
			break;
		case SFIELD_SET_AUDIORECEIVERENABLE:
			ret = SetSysAudioReceiverEnable(data, len);
			break;
		case SFIELD_SET_AUTOIRIS_VOL:
			ret = SetSysAutoIrisVoltage(data, len);
			break;
		case SFIELD_SET_AUDIOSERVERIP:
			ret = SetSysAudioServerIp(data, len);
			break;
		case SFIELD_SET_DEFAULT:
			ret = SetSysCommon(data, len, SYS_MSG_SET_DEFAULT);
			break;
		case SFIELD_SET_CLEARSYSLOG:
			ret = SetSysClearSysLog(data, len);
			break;
		case SFIELD_SET_CLEARACCESSLOG:
			ret = SetSysClearAccessLog(data, len);
			break;
		case SFIELD_SET_JQUALITY:
			ret = SetSysJpegQuality(data, len);
			break;
		case SFIELD_SET_FDETECT:
			ret = SetSysfdetect(data, len);
			break;
		case SFIELD_SET_FDX:
			ret = SetSysfdx(data, len);
			break;
		case SFIELD_SET_FDY:
			ret = SetSysfdy(data, len);
			break;
		case SFIELD_SET_FDW:
			ret = SetSysfdw(data, len);
			break;
		case SFIELD_SET_FDH:
			ret = SetSysfdh(data, len);
			break;
		case SFIELD_SET_FDCONF_LEVEL:
			ret = SetSysfdconf(data, len);
			break;
		case SFIELD_SET_FD_DIRECTION:
			ret = SetSysfddir(data, len);
			break;
	       case SFIELD_SET_FRECOGNITION:
		   	ret = SetSysfrecog(data, len);
			break;
		case SFIELD_SET_FRCONF_LEVEL:
			ret = SetSysfrconf(data, len);
			break;
		case SFIELD_SET_FR_DATABASE:
			ret = SetSysfrdbase(data, len);
			break;
		case SFIELD_SET_PRIVACY_MASK:
			ret = SetSyspmask(data, len);
			break;
		case SFIELD_SET_MASK_OPTIONS:
			ret = SetSyspmaskopt(data, len);
			break;
		case SFIELD_SET_MIRROR:
			ret = SetSysMirror(data, len);
			break;
		case SFIELD_SET_DEMOCFG:
			ret = SetSysDemoCfg(data, len);
			break;
		case SFIELD_SET_HISTOGRAM:
			ret = SetSysHistogram(data, len);
			break;
		case SFIELD_SET_DYNRANGE:
			ret = SetSysDynRange(data, len);
			break;
		case SFIELD_SET_DYNRANGESTRENGTH:
			ret = SetSysCommon(data, len, SYS_MSG_SET_DYNRANGESTRENGTH);
			break;
		case SFIELD_SET_VIDEO_MODE:
			ret = SetSysVideoMode(data, len);
			break;
		case SFIELD_SET_VIDEOCODECCOMBO:
			ret = SetSysVideoCodecCombo(data, len);
			break;
		case SFIELD_SET_VIDEOCODECRES:
			ret = SetSysVideoCodecRes(data, len);
			break;
		case SFIELD_SET_IMAGESOURCE:
			ret = SetSysImageSource(data, len);
			break;
		case SFIELD_SET_SCHEDULE:
			ret = SetSysSchedule(data, len);
			break;
        case SFIELD_SET_LOSTALARM:
			ret = SetSysLostAlarm(data, len);
			break;
		case SFIELD_SET_SDAENABLE:
			ret = SetSysSDAlarmEnable(data, len);
			break;
		case SFIELD_SET_AFTPENABLE:
			ret = SetSysFtpAlarmEnable(data, len);
			break;
		case SFIELD_SET_ASMTPENABLE:
			ret = SetSysSmtpAlarmEnable(data, len);
			break;
		case SFIELD_SET_ALARMDURATION:
			ret = SetSysAlarmDuration(data, len);
			break;
		case SFIELD_SET_AVIDURATION:
			ret = SetAVIDuration(data, len);
			break;
		case SFIELD_SET_AVIFORMAT:
			ret = SetAVIFormat(data, len);
			break;
		case SFIELD_SET_FTPFILEFORMAT:
			ret = SetFTPFileFormat(data, len);
			break;
		case SFIELD_SET_SDFILEFORMAT:
			ret = SetSDFileFormat(data, len);
			break;
		case SFIELD_SET_ATTFILEFORMAT:
			ret = SetAttFileFormat(data, len);
			break;
		case SFIELD_SET_AUDIOENABLE:
			ret = SetAudioEnable(data, len);
			break;
		case SFIELD_SET_ASMTPATTACH:
			ret = SetASmtpAttach(data, len);
			break;
		case SFIELD_SET_FTP_RFTPENABLE:
			ret = SetRftpEnable(data, len);
			break;
		case SFIELD_SET_SD_SDRENABLE:
			ret = SetSdReEnable(data, len);
			break;
		case SFIELD_SET_MOTIONENABLE:
			ret = SetMotionEnable(data, len);
			break;
		case SFIELD_SET_MOTIONCENABLE:
			ret = SetMotionCEnable(data, len);
			break;
		case SFIELD_SET_MOTIONLEVEL:
			ret = SetMotionLevel(data, len);
			break;
		case SFIELD_SET_MOTIONCVALUE:
			ret = SetMotionCValue(data, len);
			break;
		case SFIELD_SET_MOTIONBLOCK:
			ret = SetMotionBlock(data, len);
			break;
		case SFIELD_GET_DHCP_CONFIG:
			ret = GetSysDhcpConfig(data, len);
			break;
		case SFIELD_DEL_SCHEDULE:
			ret = DelSysSchedule(data, len);
			break;
		case SFIELD_SET_IMAGE2A:
			ret = SetSysImage2A(data, len);
			break;
		case SFIELD_SET_IMAGE2ATYPE:
			ret = SetSysImage2AType(data, len);
			break;
		case SFIELD_SET_BINNING:
			ret = SetSysBinning(data, len);
			break;
		case SFIELD_SET_BLC:
			ret = SetSysBLC(data, len);
			break;
		case SFIELD_SET_MPEG41_BITRATE:
			ret = SetMPEG41Bitrate(data, len);
			break;
		case SFIELD_SET_MPEG42_BITRATE:
			ret = SetMPEG42Bitrate(data, len);
			break;
		case SFIELD_SET_CLICK_SNAP_FILENAME :
			ret = SetSysClickSnapFilename(data, len);
			break;
		case  SFIELD_SET_CLICK_SNAP_STORAGE :
			ret = SetSysClickSnapStorage(data, len);
			break;
		case SFIELD_SET_RATE_CONTROL1:
			 ret = SetSysRateControl1(data, len);
			 break;
		case SFIELD_SET_RATE_CONTROL2:
			 ret = SetSysRateControl2(data, len);
			 break;
	       case SFIELD_SET_DSTAMPENABLE1:
		   	ret = SetSysDateStampEnable1(data, len);
			break;
		case SFIELD_SET_DSTAMPENABLE2:
		   	ret = SetSysDateStampEnable2(data, len);
			break;
		case SFIELD_SET_DSTAMPENABLE3:
		   	ret = SetSysDateStampEnable3(data, len);
			break;
		case SFIELD_SET_TSTAMPENABLE1:
			ret = SetSysTimeStampEnable1(data, len);
			break;
		case SFIELD_SET_TSTAMPENABLE2:
			ret = SetSysTimeStampEnable2(data, len);
			break;
		case SFIELD_SET_TSTAMPENABLE3:
			ret = SetSysTimeStampEnable3(data, len);
			break;
		case SFIELD_SET_LOGOENABLE1:
			ret = SetSysLogoEnable1(data, len);
			break;
		case SFIELD_SET_LOGOENABLE2:
			ret = SetSysLogoEnable2(data, len);
			break;
		case SFIELD_SET_LOGOENABLE3:
			ret = SetSysLogoEnable3(data, len);
			break;
		case SFIELD_SET_LOGO_POSITION1:
			ret = SetSysLogoPosition1(data, len);
			break;
		case SFIELD_SET_LOGO_POSITION2:
			ret = SetSysLogoPosition2(data, len);
			break;
		case SFIELD_SET_LOGO_POSITION3:
			ret = SetSysLogoPosition3(data, len);
			break;
		case SFIELD_SET_TEXTENABLE1:
			ret = SetSysTextEnable1(data, len);
			break;
		case SFIELD_SET_TEXTENABLE2:
			ret = SetSysTextEnable2(data, len);
			break;
		case SFIELD_SET_TEXTENABLE3:
			ret = SetSysTextEnable3(data, len);
			break;
		case SFIELD_SET_TEXT_POSITION1:
			ret = SetSysTextPosition1(data, len);
			break;
		case SFIELD_SET_TEXT_POSITION2:
			ret = SetSysTextPosition2(data, len);
			break;
		case SFIELD_SET_TEXT_POSITION3:
			ret = SetSysTextPosition3(data, len);
			break;
		case SFIELD_SET_OVERLAY_TEXT1:
			ret = SetSysOverlayText1(data, len);
			break;
		case SFIELD_SET_OVERLAY_TEXT2:
			ret = SetSysOverlayText2(data, len);
			break;
		case SFIELD_SET_OVERLAY_TEXT3:
			ret = SetSysOverlayText3(data, len);
			break;
		case SFIELD_SET_DETAIL_INFO1:
			ret = SetSysCommon(data, len, SYS_MSG_SET_DETAIL_INFO1);
			break;
		case SFIELD_SET_DETAIL_INFO2:
			ret = SetSysCommon(data, len, SYS_MSG_SET_DETAIL_INFO2);
			break;
		case SFIELD_SET_DETAIL_INFO3:
			ret = SetSysCommon(data, len, SYS_MSG_SET_DETAIL_INFO3);
			break;
		case SFIELD_SET_ALARMLOCAL:
			ret = SetSysCommon(data, len, SYS_MSG_SET_ALARMLOCAL);
			break;
		case SFIELD_SET_RECORDLOCAL:
			ret = SetSysCommon(data, len, SYS_MSG_SET_RECORDLOCAL);
			break;
		case SFIELD_SET_ENCRYPTION:
			ret = SetSysEncryption(data, len);
			break;
		case SFIELD_SET_LOCAL_DISPLAY:
			ret = SetSysLocalDisplay(data, len);
			break;
		case SFIELD_SET_IPRATIO1:
			ret = SetSysIpratio1(data, len);
			break;
		case SFIELD_SET_IPRATIO2:
			ret = SetSysIpratio2(data, len);
			break;
		case SFIELD_SET_IPRATIO3:
			ret = SetSysIpratio3(data, len);
			break;
		case SFIELD_SET_FORCE_IFRAME1:
			ret = SetSysForceIframe1(data, len);
			break;
		case SFIELD_SET_FORCE_IFRAME2:
			ret = SetSysForceIframe2(data, len);
			break;
		case SFIELD_SET_FORCE_IFRAME3:
			ret = SetSysForceIframe3(data, len);
			break;
		case SFIELD_SET_QPINIT1:
			ret = SetSysQPInit1(data, len);
			break;
		case SFIELD_SET_QPINIT2:
			ret = SetSysQPInit2(data, len);
			break;
		case SFIELD_SET_QPINIT3:
			ret = SetSysQPInit3(data, len);
			break;
		case SFIELD_SET_QPMIN1:
			ret = SetSysQPMin1(data, len);
			break;
		case SFIELD_SET_QPMIN2:
			ret = SetSysQPMin2(data, len);
			break;
		case SFIELD_SET_QPMIN3:
			ret = SetSysQPMin3(data, len);
			break;
		case SFIELD_SET_QPMAX1:
			ret = SetSysQPMax1(data, len);
			break;
		case SFIELD_SET_QPMAX2:
			ret = SetSysQPMax2(data, len);
			break;
		case SFIELD_SET_QPMAX3:
			ret = SetSysQPMax3(data, len);
			break;
		case SFIELD_SET_MECONFIG1:
			ret = SetSysMEConfig1(data, len);
			break;
		case SFIELD_SET_MECONFIG2:
			ret = SetSysMEConfig2(data, len);
			break;
		case SFIELD_SET_MECONFIG3:
			ret = SetSysMEConfig3(data, len);
			break;
		case SFIELD_SET_PACKETSIZE1:
			ret = SetSysPacketSize1(data, len);
			break;
		case SFIELD_SET_PACKETSIZE2:
			ret = SetSysPacketSize2(data, len);
			break;
		case SFIELD_SET_PACKETSIZE3:
			ret = SetSysPacketSize3(data, len);
			break;
		case SFIELD_SET_ROI_ENABLE1:
			ret = SetSysROIEnable1(data, len);
			break;
		case SFIELD_SET_ROI_ENABLE2:
			ret = SetSysROIEnable2(data, len);
			break;
	    case SFIELD_SET_ROI_ENABLE3:
			ret = SetSysROIEnable3(data, len);
			break;
		case SFIELD_SET_STR1X1:
			ret = SetSysStr1X1(data, len);
			break;
		case SFIELD_SET_STR1Y1:
			ret = SetSysStr1Y1(data, len);
			break;
        case SFIELD_SET_STR1W1:
			ret = SetSysStr1W1(data, len);
			break;
		case SFIELD_SET_STR1H1:
			ret = SetSysStr1H1(data, len);
			break;
		case SFIELD_SET_STR1X2:
			ret = SetSysStr1X2(data, len);
			break;
		case SFIELD_SET_STR1Y2:
			ret = SetSysStr1Y2(data, len);
			break;
        case SFIELD_SET_STR1W2:
			ret = SetSysStr1W2(data, len);
			break;
		case SFIELD_SET_STR1H2:
			ret = SetSysStr1H2(data, len);
			break;
		case SFIELD_SET_STR1X3:
			ret = SetSysStr1X3(data, len);
			break;
		case SFIELD_SET_STR1Y3:
			ret = SetSysStr1Y3(data, len);
			break;
        case SFIELD_SET_STR1W3:
			ret = SetSysStr1W3(data, len);
			break;
		case SFIELD_SET_STR1H3:
			ret = SetSysStr1H3(data, len);
			break;
		case SFIELD_SET_STR2X1:
			ret = SetSysStr2X1(data, len);
			break;
		case SFIELD_SET_STR2Y1:
			ret = SetSysStr2Y1(data, len);
			break;
        case SFIELD_SET_STR2W1:
			ret = SetSysStr2W1(data, len);
			break;
		case SFIELD_SET_STR2H1:
			ret = SetSysStr2H1(data, len);
			break;
		case SFIELD_SET_STR2X2:
			ret = SetSysStr2X2(data, len);
			break;
		case SFIELD_SET_STR2Y2:
			ret = SetSysStr2Y2(data, len);
			break;
        case SFIELD_SET_STR2W2:
			ret = SetSysStr2W2(data, len);
			break;
		case SFIELD_SET_STR2H2:
			ret = SetSysStr2H2(data, len);
			break;
		case SFIELD_SET_STR2X3:
			ret = SetSysStr2X3(data, len);
			break;
		case SFIELD_SET_STR2Y3:
			ret = SetSysStr2Y3(data, len);
			break;
        case SFIELD_SET_STR2W3:
			ret = SetSysStr2W3(data, len);
			break;
		case SFIELD_SET_STR2H3:
			ret = SetSysStr2H3(data, len);
			break;
		case SFIELD_SET_STR3X1:
			ret = SetSysStr3X1(data, len);
			break;
		case SFIELD_SET_STR3Y1:
			ret = SetSysStr3Y1(data, len);
			break;
        case SFIELD_SET_STR3W1:
			ret = SetSysStr3W1(data, len);
			break;
		case SFIELD_SET_STR3H1:
			ret = SetSysStr3H1(data, len);
			break;
		case SFIELD_SET_STR3X2:
			ret = SetSysStr3X2(data, len);
			break;
		case SFIELD_SET_STR3Y2:
			ret = SetSysStr3Y2(data, len);
			break;
        case SFIELD_SET_STR3W2:
			ret = SetSysStr3W2(data, len);
			break;
		case SFIELD_SET_STR3H2:
			ret = SetSysStr3H2(data, len);
			break;
		case SFIELD_SET_STR3X3:
			ret = SetSysStr3X3(data, len);
			break;
		case SFIELD_SET_STR3Y3:
			ret = SetSysStr3Y3(data, len);
			break;
        case SFIELD_SET_STR3W3:
			ret = SetSysStr3W3(data, len);
			break;
		case SFIELD_SET_STR3H3:
			ret = SetSysStr3H3(data, len);
			break;
		case SFIELD_SET_IMAGEDEFAULT:
			ret = SetImageDefault(data, len);
			break;
		case SFIELD_DO_LOGIN:
			ret = DoSysLogin(data, len);
			break;
		case SFIELD_GET_USER_AUTHORITY:
			ret = GetSysUserAuthority(data, len);
			break;
		case SFIELD_ADD_USER:
			ret = SysAddUser(data, len);
			break;
		case SFIELD_DEL_USER:
			ret = SysDelUser(data, len);
			break;
		case SFIELD_SD_FORMAT:
			ret = SysSdFormat(data, len);
			break;
		case SFIELD_SD_UNMOUNT:
			ret = SysSdUnmount(data, len);
			break;
		case SFIELD_SET_GIOINENABLE:
			ret = SetGIOInEnable(data, len);
			break;
		case SFIELD_SET_GIOINTYPE:
			ret = SetGIOInType(data, len);
			break;
		case SFIELD_SET_GIOOUTENABLE:
			ret = SetGIOOutEnable(data, len);
			break;
		case SFIELD_SET_GIOOUTTYPE:
			ret = SetGIOOutType(data, len);
			break;
		case SFIELD_SET_DATEFORMAT:
		    ret = SetSysCommon(data, len, SYS_MSG_SET_DATEFORMAT);
			break;
		case SFIELD_SET_TSTAMPFORMAT:
		    ret = SetSysCommon(data, len, SYS_MSG_SET_TSTAMPFORMAT);
			break;
		case SFIELD_SET_DATEPPOS:
		    ret = SetSysCommon(data, len, SYS_MSG_SET_DATEPPOS);
			break;
		case SFIELD_SET_TIMEPOS:
		    ret = SetSysCommon(data, len, SYS_MSG_SET_TIMEPOS);
			break;
		case SFIELD_SET_AUDIOINVOLUME:
			ret = SetAudioinVolume(data, len);
			break;
		case SFIELD_SET_FRAMERATE1:
			ret = SetSysFramerate1(data, len);
			break;
		case SFIELD_SET_FRAMERATE2:
			ret = SetSysFramerate2(data, len);
			break;
		case SFIELD_SET_FRAMERATE3:
			ret = SetSysFramerate3(data, len);
			break;
		case SFIELD_SCHEDULE_STOP:
			ret = SysScheduleStop(data, len);
			break;
		case SFIELD_SCHEDULE_RESUME:
			ret = SysScheduleResume(data, len);
			break;
		case SFIELD_SET_MULTICAST:
			ret = SetSysMulticast(data, len);
			break;
		case SFIELD_SET_HTTPSPORT:
			ret = SetSysCommon(data, len, SYS_MSG_SET_HTTPSPORT);
			break;
		case SFIELD_SET_RS485:
			ret = SetSysCommon(data, len, SYS_MSG_SET_RS485);
			break;
		case SFIELD_SET_ALARM_ENABLE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_ALARM_ENABLE);
			break;
		case SFIELD_SET_EXT_ALARM:
			ret = SetSysCommon(data, len, SYS_MSG_SET_EXT_ALARM);
			break;
		case SFIELD_SET_AUDIOALARM:
			ret = SetSysCommon(data, len, SYS_MSG_SET_AUDIOALARM);
			break;
		case SFIELD_SET_ALARM_AUDIOPLAY:
			ret = SetSysCommon(data, len, SYS_MSG_SET_ALARM_AUDIOPLAY);
			break;
		case SFIELD_SET_ALARM_AUDIOFILE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_ALARM_AUDIOFILE);
			break;
		case SFIELD_SET_SCHEDULE_REPEAT_ENABLE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_SCHEDULE_REPEAT_ENABLE);
			break;
		case SFIELD_SET_SCHEDULE_NUMWEEKS:
			ret = SetSysCommon(data, len, SYS_MSG_SET_SCHEDULE_NUM_WEEKS);
			break;
		case SFIELD_SET_SCHEDULE_INFINITE_ENABLE:
			ret = SetSysCommon(data, len, SYS_MSG_SET_SCHEDULE_INFINITE_ENABLE);
			break;
                /* Onvif */
                case SFIELD_ADD_PROFILE:
                        ret = CreateMedProfile(data, len);
                        break;
                case SFIELD_ADD_VIDEOSOURCE_CONF:
                        ret = AddVideoSourceConfiguration(data, len);
                        break;
                case SFIELD_SET_VIDEOSOURCE_CONF:
                        ret = SetVideoSourceConfiguration(data, len);
                        break;
                case SFIELD_ADD_VIDEOENCODER_CONF:
                        ret = AddVideoEncoderConfiguration(data, len);
                        break;
                case SFIELD_SET_VIDEOENCODER_CONF:
                        ret = SetVideoEncoderConfiguration(data, len);
                        break;
		case SFIELD_DEL_VIDEOENCODER_CONF:
                        ret = RemoveVideoEncoderConfiguration(data, len);
                        break;
		case SFIELD_SET_AUDIO_SOURCE:
                        ret = SetAudioSourceConfiguration(data, len);
                        break;
		case SFIELD_ADD_AUDIOSOURCE_CONF:
			ret = AddAudioSourceConfiguration(data, len);
			break;
		case SFIELD_ADD_AUDIOENCODER_CONF:
			ret = AddAudioEncoderConfiguration(data, len);
			break;
		case SFIELD_SET_AUDIOENCODER_CONF:
			ret = SetAudioEncoderConfiguration(data, len);
			break;
		case SFIELD_DEL_AUDIO_SOURCE:
			ret = RemoveAudioSourceConfiguration(data, len);
			break;
		case SFIELD_DEL_AUDIO_ENCODER:
			ret = RemoveAudioEncoderConfiguration(data, len);
			break;
		case SFIELD_DEL_PROFILE:
			ret = DelProfile(data, len);
                        break;
                case SFIELD_DIS_MODE:
                        ret = SetDiscoveryMode(data,len);
                        break;
                case SFIELD_ADD_SCOPE:
                        ret = AddScopes(data,len);
                        break;
                case SFIELD_DEL_SCOPE:
                        ret = DeleteScopes(data,len);
                        break;
		case SFIELD_SET_SCOPE:
                        ret = SetScopes(data, len);
                        break;
                case SFIELD_SET_VIDEOANALYTICS_CONF:
                        ret = SetVideoAnalytics(data, len);
                        break;
		case SFIELD_SET_VIDEOOUTPUT_CONF:
                        ret = SetVideoOutput(data, len);
                        break;
		case SFIELD_SET_RELAYS_CONF:
                        ret = SetRelays(data, len);
                        break;
		case  SFIELD_SET_RELAY_LOGICALSTATE:
			ret = SetRelayLogicalState(data, len);
			break;
		case SFIELD_SET_METADATA:
                        ret = SetMetadata(data, len);
                        break;
		case SFIELD_ADD_METADATA:
                        ret = AddMetadata(data, len);
                        break;
		case SFIELD_DEL_METADATA:
                        ret = DelMetadata(data, len);
                        break;

               /* End-Onvif */
		/* DMVA Specific Parameter Message settings */
		case SFIELD_SET_DMVAENABLE:
			ret = SetSysDmvaEnable(data, len);
			break;
		case SFIELD_SET_DMVAALGOFRAMERATE:
			ret = SetSysDmvaExptAlgoFrameRate(data, len);
			break;
		case SFIELD_SET_DMVAALGODETECTFREQ:
			ret = SetSysDmvaExptAlgoDetectFreq(data, len);
			break;
		case SFIELD_SET_DMVAEVTRECORDENABLE:
			ret = SetSysDmvaExptEvtRecordEnable(data, len);
			break;
		case SFIELD_SET_DMVASMETATRACKERENABLE:
			ret = SetSysDmvaExptSmetaTrackerEnable(data, len);
			break;
		/* DMVA TZ Specific Parameter Message settings */
		case SFIELD_SET_DMVA_TZ_SENSITIVITY:
			ret = SetSysDmvaTZSensitivity(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_PERSONMINWIDTH:
			ret = SetSysDmvaTZPersonMinWidth(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_PERSONMINHEIGHT:
			ret = SetSysDmvaTZPersonMinHeight(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_VEHICLEMINWIDTH:
			ret = SetSysDmvaTZVehicleMinWidth(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_VEHICLEMINHEIGHT:
			ret = SetSysDmvaTZVehicleMinHeight(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_PERSONMAXWIDTH:
			ret = SetSysDmvaTZPersonMaxWidth(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_PERSONMAXHEIGHT:
			ret = SetSysDmvaTZPersonMaxHeight(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_VEHICLEMAXWIDTH:
			ret = SetSysDmvaTZVehicleMaxWidth(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_VEHICLEMAXHEIGHT:
			ret = SetSysDmvaTZVehicleMaxHeight(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_DIRECTION:
			ret = SetSysDmvaTZDirection(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_PRESENTADJUST:
			ret = SetSysDmvaTZPresentAdjust(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ENABLE:
			ret = SetSysDmvaTZEnable(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_ROI_NUMSIDES:
			ret = SetSysDmvaTZZone1Numsides(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_ROI_NUMSIDES:
			ret = SetSysDmvaTZZone2Numsides(data, len);
			break;

		case SFIELD_SET_DMVA_TZ_ZONE1_X01:
			ret = SetSysDmvaTZZone1_x01(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X02:
			ret = SetSysDmvaTZZone1_x02(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X03:
			ret = SetSysDmvaTZZone1_x03(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X04:
			ret = SetSysDmvaTZZone1_x04(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X05:
			ret = SetSysDmvaTZZone1_x05(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X06:
			ret = SetSysDmvaTZZone1_x06(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X07:
			ret = SetSysDmvaTZZone1_x07(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X08:
			ret = SetSysDmvaTZZone1_x08(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X09:
			ret = SetSysDmvaTZZone1_x09(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X10:
			ret = SetSysDmvaTZZone1_x10(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X11:
			ret = SetSysDmvaTZZone1_x11(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X12:
			ret = SetSysDmvaTZZone1_x12(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X13:
			ret = SetSysDmvaTZZone1_x13(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X14:
			ret = SetSysDmvaTZZone1_x14(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X15:
			ret = SetSysDmvaTZZone1_x15(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_X16:
			ret = SetSysDmvaTZZone1_x16(data, len);
			break;

		case SFIELD_SET_DMVA_TZ_ZONE1_Y01:
			ret = SetSysDmvaTZZone1_y01(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y02:
			ret = SetSysDmvaTZZone1_y02(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y03:
			ret = SetSysDmvaTZZone1_y03(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y04:
			ret = SetSysDmvaTZZone1_y04(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y05:
			ret = SetSysDmvaTZZone1_y05(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y06:
			ret = SetSysDmvaTZZone1_y06(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y07:
			ret = SetSysDmvaTZZone1_y07(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y08:
			ret = SetSysDmvaTZZone1_y08(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y09:
			ret = SetSysDmvaTZZone1_y09(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y10:
			ret = SetSysDmvaTZZone1_y10(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y11:
			ret = SetSysDmvaTZZone1_y11(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y12:
			ret = SetSysDmvaTZZone1_y12(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y13:
			ret = SetSysDmvaTZZone1_y13(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y14:
			ret = SetSysDmvaTZZone1_y14(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y15:
			ret = SetSysDmvaTZZone1_y15(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE1_Y16:
			ret = SetSysDmvaTZZone1_y16(data, len);
			break;

		case SFIELD_SET_DMVA_TZ_ZONE2_X01:
			ret = SetSysDmvaTZZone2_x01(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X02:
			ret = SetSysDmvaTZZone2_x02(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X03:
			ret = SetSysDmvaTZZone2_x03(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X04:
			ret = SetSysDmvaTZZone2_x04(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X05:
			ret = SetSysDmvaTZZone2_x05(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X06:
			ret = SetSysDmvaTZZone2_x06(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X07:
			ret = SetSysDmvaTZZone2_x07(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X08:
			ret = SetSysDmvaTZZone2_x08(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X09:
			ret = SetSysDmvaTZZone2_x09(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X10:
			ret = SetSysDmvaTZZone2_x10(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X11:
			ret = SetSysDmvaTZZone2_x11(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X12:
			ret = SetSysDmvaTZZone2_x12(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X13:
			ret = SetSysDmvaTZZone2_x13(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X14:
			ret = SetSysDmvaTZZone2_x14(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X15:
			ret = SetSysDmvaTZZone2_x15(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_X16:
			ret = SetSysDmvaTZZone2_x16(data, len);
			break;

		case SFIELD_SET_DMVA_TZ_ZONE2_Y01:
			ret = SetSysDmvaTZZone2_y01(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y02:
			ret = SetSysDmvaTZZone2_y02(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y03:
			ret = SetSysDmvaTZZone2_y03(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y04:
			ret = SetSysDmvaTZZone2_y04(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y05:
			ret = SetSysDmvaTZZone2_y05(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y06:
			ret = SetSysDmvaTZZone2_y06(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y07:
			ret = SetSysDmvaTZZone2_y07(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y08:
			ret = SetSysDmvaTZZone2_y08(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y09:
			ret = SetSysDmvaTZZone2_y09(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y10:
			ret = SetSysDmvaTZZone2_y10(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y11:
			ret = SetSysDmvaTZZone2_y11(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y12:
			ret = SetSysDmvaTZZone2_y12(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y13:
			ret = SetSysDmvaTZZone2_y13(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y14:
			ret = SetSysDmvaTZZone2_y14(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y15:
			ret = SetSysDmvaTZZone2_y15(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_ZONE2_Y16:
			ret = SetSysDmvaTZZone2_y16(data, len);
			break;

        /* DMVA TZ save/load params */
		case SFIELD_SET_DMVA_TZ_SAVE:
			ret = SetSysDmvaTZSave(data, len);
			break;
		case SFIELD_SET_DMVA_TZ_LOAD:
			ret = SetSysDmvaTZLoad(data, len);
			break;

		/* Set DMVA Main Page specific Parameters */
		case SFIELD_SET_DMVA_MAIN_CAMID:
			ret = SetSysDmvaCamID(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_GETSCHEDULE:
			ret = SetSysDmvaGetSchedule(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_FD:
			ret = SetSysDmvaCfgFD(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_CTDIMDSMETATZOC:
			ret = SetSysDmvaCfgCTDImdSmetaTzOc(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_DISPLAYOPTIONS:
			ret = SetSysDmvaDisplayOptions(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_RECORDINGVAME:
			ret = SetSysDmvaEventRecordingVAME(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_ACTIONPLAYSENDSEARCHTRASH:
			ret = SetSysDmvaEventListActionPlaySendSearchTrash(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_SELECTEVENT:
			ret = SetSysDmvaEventListSelectEvent(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_ARCHIVEFLAG:
			ret = SetSysDmvaEventListArchiveFlag(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EVENTTYPE:
			ret = SetSysDmvaEventListEventType(data, len);
			break;

		case SFIELD_SET_DMVA_MAIN_EL_START_MONTH:
			ret = SetSysDmvaEventListStartMonth(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EL_START_DAY:
			ret = SetSysDmvaEventListStartDay(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EL_START_TIME:
			ret = SetSysDmvaEventListStartTime(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EL_END_MONTH:
			ret = SetSysDmvaEventListEndMonth(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EL_END_DAY:
			ret = SetSysDmvaEventListEndDay(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EL_END_TIME:
			ret = SetSysDmvaEventListEndTime(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EL_TIME_STAMP_LABEL:
			ret = SetSysDmvaEventListTimeStamp(data, len);
			break;
		case SFIELD_SET_DMVA_MAIN_EL_EVENT_DETAILS_LABEL:
			ret = SetSysDmvaEventListEventDetails(data, len);
			break;

		/* Set DMVA Advanced settings Page specific Parameters */
		case SFIELD_SET_DMVA_ADVANCED_OBJECTMETADATA:
			ret = SetSysDmvaObjectMetaData(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPFONTTYPE:
			ret = SetSysDmvaOverlayPropertiesFontType(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPFONTSIZE:
			ret = SetSysDmvaOverlayPropertiesFontSize(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPFONTCOLOR:
			ret = SetSysDmvaOverlayPropertiesFontColor(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPBOXCOLOR:
			ret = SetSysDmvaOverlayPropertiesBoxColor(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPCENTROIDCOLOR:
			ret = SetSysDmvaOverlayPropertiesCentroidColor(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPACTIVEZONECOLOR:
			ret = SetSysDmvaOverlayPropertiesActiveZoneColor(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPINACTIVEZONECOLOR:
			ret = SetSysDmvaOverlayPropertiesInactiveZoneColor(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_OVLPROPVELOCITYCOLOR:
			ret = SetSysDmvaOverlayPropertiesVelocityColor(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_ENVPROFILELOAD:
			ret = SetSysDmvaEnvironmentProfileLoad(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_ENVPROFILESTORE:
			ret = SetSysDmvaEnvironmentProfileStore(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_ENVPROFILECLEAR:
			ret = SetSysDmvaEnvironmentProfileClear(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_ENVPROFILESTATE:
			ret = SetSysDmvaEnvironmentProfileState(data, len);
			break;
		case SFIELD_SET_DMVA_ADVANCED_ENVPROFILEDESC:
			ret = SetSysDmvaEnvironmentProfileDesc(data, len);
			break;

        /* DMVA event delete */
		case SFIELD_SET_DMVA_EVENT_DELETE_INDEX:
			ret = SetSysDmvaEventDeleteIndex(data, len);
			break;
		case SFIELD_SET_DMVA_EVENT_DELETE_START_INDEX:
			ret = SetSysDmvaEventDeleteStartIndex(data, len);
			break;
		case SFIELD_SET_DMVA_EVENT_DELETE_END_INDEX:
			ret = SetSysDmvaEventDeleteEndIndex(data, len);
			break;
		case SFIELD_SET_DMVA_EVENT_DELETE_ALL:
			ret = SetSysDmvaEventDeleteAll(data, len);
			break;

		/* Set DMVA CTD Page specific Parameters */
		case SFIELD_SET_DMVA_CTD_ENV1:
			ret = SetSysDmvaCfgTDEnv1(data, len);
			break;
		case SFIELD_SET_DMVA_CTD_ENV2:
			ret = SetSysDmvaCfgTDEnv2(data, len);
			break;
		case SFIELD_SET_DMVA_CTD_SENSITIVITY:
			ret = SetSysDmvaCfgTDSensitivity(data, len);
			break;
		case SFIELD_SET_DMVA_CTD_RESETTIME:
			ret = SetSysDmvaCfgTDResetTime(data, len);
			break;
		/* Set DMVA OC Page specific Parameters */
		case SFIELD_SET_DMVA_OC_SENSITIVITY:
			ret = SetSysDmvaOCSensitivity(data, len);
			break;
		case SFIELD_SET_DMVA_OC_OBJECTWIDTH:
			ret = SetSysDmvaOCObjectWidth(data, len);
			break;
		case SFIELD_SET_DMVA_OC_OBJECTHEIGHT:
			ret = SetSysDmvaOCObjectHeight(data, len);
			break;
		case SFIELD_SET_DMVA_OC_DIRECTION:
			ret = SetSysDmvaOCDirection(data, len);
			break;
		case SFIELD_SET_DMVA_OC_ENABLE:
			ret = SetSysDmvaOCEnable(data, len);
			break;
		case SFIELD_SET_DMVA_OC_LEFTRIGHT:
			ret = SetSysDmvaOCLeftRight(data, len);
			break;
		case SFIELD_SET_DMVA_OC_TOPBOTTOM:
			ret = SetSysDmvaOCTopBottom(data, len);
			break;

        /* DMVA OC save/load params */
		case SFIELD_SET_DMVA_OC_SAVE:
			ret = SetSysDmvaOCSave(data, len);
			break;
		case SFIELD_SET_DMVA_OC_LOAD:
			ret = SetSysDmvaOCLoad(data, len);
			break;

		/* Set DMVA SMETA Page specific Parameters */
		case SFIELD_SET_DMVA_SMETA_SENSITIVITY:
			ret = SetSysDmvaSMETASensitivity(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ENABLESTREAMINGDATA:
			ret = SetSysDmvaSMETAEnableStreamingData(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_STREAMBB:
			ret = SetSysDmvaSMETAStreamBB(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_STREAMVELOCITY:
			ret = SetSysDmvaSMETAStreamVelocity(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_STREAMHISTOGRAM:
			ret = SetSysDmvaSMETAStreamHistogram(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_STREAMMOMENTS:
			ret = SetSysDmvaSMETAStreamMoments(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_PRESENTADJUST:
			ret = SetSysDmvaSMETAPresentAdjust(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_PERSONMINWIDTH:
			ret = SetSysDmvaSMETAPersonMinWidth(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_PERSONMINHEIGHT:
			ret = SetSysDmvaSMETAPersonMinHeight(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_VEHICLEMINWIDTH:
			ret = SetSysDmvaSMETAVehicleMinWidth(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_VEHICLEMINHEIGHT:
			ret = SetSysDmvaSMETAVehicleMinHeight(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_PERSONMAXWIDTH:
			ret = SetSysDmvaSMETAPersonMaxWidth(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_PERSONMAXHEIGHT:
			ret = SetSysDmvaSMETAPersonMaxHeight(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_VEHICLEMAXWIDTH:
			ret = SetSysDmvaSMETAVehicleMaxWidth(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_VEHICLEMAXHEIGHT:
			ret = SetSysDmvaSMETAVehicleMaxHeight(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_NUMZONES:
			ret = SetSysDmvaSMETANumZones(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_ROI_NUMSIDES:
			ret = SetSysDmvaSMETAZone1Numsides(data, len);
			break;

		/* Zone 1 */
		case SFIELD_SET_DMVA_SMETA_ZONE1_X01:
			ret = SetSysDmvaSMETAZone1_x01(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X02:
			ret = SetSysDmvaSMETAZone1_x02(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X03:
			ret = SetSysDmvaSMETAZone1_x03(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X04:
			ret = SetSysDmvaSMETAZone1_x04(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X05:
			ret = SetSysDmvaSMETAZone1_x05(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X06:
			ret = SetSysDmvaSMETAZone1_x06(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X07:
			ret = SetSysDmvaSMETAZone1_x07(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X08:
			ret = SetSysDmvaSMETAZone1_x08(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X09:
			ret = SetSysDmvaSMETAZone1_x09(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X10:
			ret = SetSysDmvaSMETAZone1_x10(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X11:
			ret = SetSysDmvaSMETAZone1_x11(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X12:
			ret = SetSysDmvaSMETAZone1_x12(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X13:
			ret = SetSysDmvaSMETAZone1_x13(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X14:
			ret = SetSysDmvaSMETAZone1_x14(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X15:
			ret = SetSysDmvaSMETAZone1_x15(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_X16:
			ret = SetSysDmvaSMETAZone1_x16(data, len);
			break;

		case SFIELD_SET_DMVA_SMETA_ZONE1_Y01:
			ret = SetSysDmvaSMETAZone1_y01(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y02:
			ret = SetSysDmvaSMETAZone1_y02(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y03:
			ret = SetSysDmvaSMETAZone1_y03(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y04:
			ret = SetSysDmvaSMETAZone1_y04(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y05:
			ret = SetSysDmvaSMETAZone1_y05(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y06:
			ret = SetSysDmvaSMETAZone1_y06(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y07:
			ret = SetSysDmvaSMETAZone1_y07(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y08:
			ret = SetSysDmvaSMETAZone1_y08(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y09:
			ret = SetSysDmvaSMETAZone1_y09(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y10:
			ret = SetSysDmvaSMETAZone1_y10(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y11:
			ret = SetSysDmvaSMETAZone1_y11(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y12:
			ret = SetSysDmvaSMETAZone1_y12(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y13:
			ret = SetSysDmvaSMETAZone1_y13(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y14:
			ret = SetSysDmvaSMETAZone1_y14(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y15:
			ret = SetSysDmvaSMETAZone1_y15(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_ZONE1_Y16:
			ret = SetSysDmvaSMETAZone1_y16(data, len);
			break;

		case SFIELD_SET_DMVA_SMETA_ZONE1_LABEL:
			ret = SetSysDmvaSMETAZone1_Label(data, len);
			break;

		/* DMVA SMETA save/load params */
		case SFIELD_SET_DMVA_SMETA_SAVE:
			ret = SetSysDmvaSMETASave(data, len);
			break;
		case SFIELD_SET_DMVA_SMETA_LOAD:
			ret = SetSysDmvaSMETALoad(data, len);
			break;

		/* Set DMVA IMD Page specific Parameters */
		case SFIELD_SET_DMVA_IMD_SENSITIVITY:
			ret = SetSysDmvaIMDSensitivity(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_PRESENTADJUST:
			ret = SetSysDmvaIMDPresentAdjust(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_PERSONMINWIDTH:
			ret = SetSysDmvaIMDPersonMinWidth(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_PERSONMINHEIGHT:
			ret = SetSysDmvaIMDPersonMinHeight(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_VEHICLEMINWIDTH:
			ret = SetSysDmvaIMDVehicleMinWidth(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_VEHICLEMINHEIGHT:
			ret = SetSysDmvaIMDVehicleMinHeight(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_PERSONMAXWIDTH:
			ret = SetSysDmvaIMDPersonMaxWidth(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_PERSONMAXHEIGHT:
			ret = SetSysDmvaIMDPersonMaxHeight(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_VEHICLEMAXWIDTH:
			ret = SetSysDmvaIMDVehicleMaxWidth(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_VEHICLEMAXHEIGHT:
			ret = SetSysDmvaIMDVehicleMaxHeight(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_NUMZONES:
			ret = SetSysDmvaIMDNumZones(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONEENABLE:
			ret = SetSysDmvaIMDZoneEnable(data, len);
			break;

		case SFIELD_SET_DMVA_IMD_ZONE1_ROI_NUMSIDES:
			ret = SetSysDmvaIMDZone1Numsides(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_ROI_NUMSIDES:
			ret = SetSysDmvaIMDZone2Numsides(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_ROI_NUMSIDES:
			ret = SetSysDmvaIMDZone3Numsides(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_ROI_NUMSIDES:
			ret = SetSysDmvaIMDZone4Numsides(data, len);
			break;

		/* Zone 1 & 2 */
		case SFIELD_SET_DMVA_IMD_ZONE1_X01:
			ret = SetSysDmvaIMDZone1_x01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X02:
			ret = SetSysDmvaIMDZone1_x02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X03:
			ret = SetSysDmvaIMDZone1_x03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X04:
			ret = SetSysDmvaIMDZone1_x04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X05:
			ret = SetSysDmvaIMDZone1_x05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X06:
			ret = SetSysDmvaIMDZone1_x06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X07:
			ret = SetSysDmvaIMDZone1_x07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X08:
			ret = SetSysDmvaIMDZone1_x08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X09:
			ret = SetSysDmvaIMDZone1_x09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X10:
			ret = SetSysDmvaIMDZone1_x10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X11:
			ret = SetSysDmvaIMDZone1_x11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X12:
			ret = SetSysDmvaIMDZone1_x12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X13:
			ret = SetSysDmvaIMDZone1_x13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X14:
			ret = SetSysDmvaIMDZone1_x14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X15:
			ret = SetSysDmvaIMDZone1_x15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_X16:
			ret = SetSysDmvaIMDZone1_x16(data, len);
			break;

		case SFIELD_SET_DMVA_IMD_ZONE1_Y01:
			ret = SetSysDmvaIMDZone1_y01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y02:
			ret = SetSysDmvaIMDZone1_y02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y03:
			ret = SetSysDmvaIMDZone1_y03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y04:
			ret = SetSysDmvaIMDZone1_y04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y05:
			ret = SetSysDmvaIMDZone1_y05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y06:
			ret = SetSysDmvaIMDZone1_y06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y07:
			ret = SetSysDmvaIMDZone1_y07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y08:
			ret = SetSysDmvaIMDZone1_y08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y09:
			ret = SetSysDmvaIMDZone1_y09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y10:
			ret = SetSysDmvaIMDZone1_y10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y11:
			ret = SetSysDmvaIMDZone1_y11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y12:
			ret = SetSysDmvaIMDZone1_y12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y13:
			ret = SetSysDmvaIMDZone1_y13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y14:
			ret = SetSysDmvaIMDZone1_y14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y15:
			ret = SetSysDmvaIMDZone1_y15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_Y16:
			ret = SetSysDmvaIMDZone1_y16(data, len);
			break;

		case SFIELD_SET_DMVA_IMD_ZONE2_X01:
			ret = SetSysDmvaIMDZone2_x01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X02:
			ret = SetSysDmvaIMDZone2_x02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X03:
			ret = SetSysDmvaIMDZone2_x03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X04:
			ret = SetSysDmvaIMDZone2_x04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X05:
			ret = SetSysDmvaIMDZone2_x05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X06:
			ret = SetSysDmvaIMDZone2_x06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X07:
			ret = SetSysDmvaIMDZone2_x07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X08:
			ret = SetSysDmvaIMDZone2_x08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X09:
			ret = SetSysDmvaIMDZone2_x09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X10:
			ret = SetSysDmvaIMDZone2_x10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X11:
			ret = SetSysDmvaIMDZone2_x11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X12:
			ret = SetSysDmvaIMDZone2_x12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X13:
			ret = SetSysDmvaIMDZone2_x13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X14:
			ret = SetSysDmvaIMDZone2_x14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X15:
			ret = SetSysDmvaIMDZone2_x15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_X16:
			ret = SetSysDmvaIMDZone2_x16(data, len);
			break;

		case SFIELD_SET_DMVA_IMD_ZONE2_Y01:
			ret = SetSysDmvaIMDZone2_y01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y02:
			ret = SetSysDmvaIMDZone2_y02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y03:
			ret = SetSysDmvaIMDZone2_y03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y04:
			ret = SetSysDmvaIMDZone2_y04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y05:
			ret = SetSysDmvaIMDZone2_y05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y06:
			ret = SetSysDmvaIMDZone2_y06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y07:
			ret = SetSysDmvaIMDZone2_y07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y08:
			ret = SetSysDmvaIMDZone2_y08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y09:
			ret = SetSysDmvaIMDZone2_y09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y10:
			ret = SetSysDmvaIMDZone2_y10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y11:
			ret = SetSysDmvaIMDZone2_y11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y12:
			ret = SetSysDmvaIMDZone2_y12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y13:
			ret = SetSysDmvaIMDZone2_y13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y14:
			ret = SetSysDmvaIMDZone2_y14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y15:
			ret = SetSysDmvaIMDZone2_y15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_Y16:
			ret = SetSysDmvaIMDZone2_y16(data, len);
			break;

		/* Zone 3 & 4 */
		case SFIELD_SET_DMVA_IMD_ZONE3_X01:
			ret = SetSysDmvaIMDZone3_x01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X02:
			ret = SetSysDmvaIMDZone3_x02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X03:
			ret = SetSysDmvaIMDZone3_x03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X04:
			ret = SetSysDmvaIMDZone3_x04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X05:
			ret = SetSysDmvaIMDZone3_x05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X06:
			ret = SetSysDmvaIMDZone3_x06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X07:
			ret = SetSysDmvaIMDZone3_x07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X08:
			ret = SetSysDmvaIMDZone3_x08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X09:
			ret = SetSysDmvaIMDZone3_x09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X10:
			ret = SetSysDmvaIMDZone3_x10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X11:
			ret = SetSysDmvaIMDZone3_x11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X12:
			ret = SetSysDmvaIMDZone3_x12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X13:
			ret = SetSysDmvaIMDZone3_x13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X14:
			ret = SetSysDmvaIMDZone3_x14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X15:
			ret = SetSysDmvaIMDZone3_x15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_X16:
			ret = SetSysDmvaIMDZone3_x16(data, len);
			break;

		case SFIELD_SET_DMVA_IMD_ZONE3_Y01:
			ret = SetSysDmvaIMDZone3_y01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y02:
			ret = SetSysDmvaIMDZone3_y02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y03:
			ret = SetSysDmvaIMDZone3_y03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y04:
			ret = SetSysDmvaIMDZone3_y04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y05:
			ret = SetSysDmvaIMDZone3_y05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y06:
			ret = SetSysDmvaIMDZone3_y06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y07:
			ret = SetSysDmvaIMDZone3_y07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y08:
			ret = SetSysDmvaIMDZone3_y08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y09:
			ret = SetSysDmvaIMDZone3_y09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y10:
			ret = SetSysDmvaIMDZone3_y10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y11:
			ret = SetSysDmvaIMDZone3_y11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y12:
			ret = SetSysDmvaIMDZone3_y12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y13:
			ret = SetSysDmvaIMDZone3_y13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y14:
			ret = SetSysDmvaIMDZone3_y14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y15:
			ret = SetSysDmvaIMDZone3_y15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_Y16:
			ret = SetSysDmvaIMDZone3_y16(data, len);
			break;

		case SFIELD_SET_DMVA_IMD_ZONE4_X01:
			ret = SetSysDmvaIMDZone4_x01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X02:
			ret = SetSysDmvaIMDZone4_x02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X03:
			ret = SetSysDmvaIMDZone4_x03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X04:
			ret = SetSysDmvaIMDZone4_x04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X05:
			ret = SetSysDmvaIMDZone4_x05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X06:
			ret = SetSysDmvaIMDZone4_x06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X07:
			ret = SetSysDmvaIMDZone4_x07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X08:
			ret = SetSysDmvaIMDZone4_x08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X09:
			ret = SetSysDmvaIMDZone4_x09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X10:
			ret = SetSysDmvaIMDZone4_x10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X11:
			ret = SetSysDmvaIMDZone4_x11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X12:
			ret = SetSysDmvaIMDZone4_x12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X13:
			ret = SetSysDmvaIMDZone4_x13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X14:
			ret = SetSysDmvaIMDZone4_x14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X15:
			ret = SetSysDmvaIMDZone4_x15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_X16:
			ret = SetSysDmvaIMDZone4_x16(data, len);
			break;

		case SFIELD_SET_DMVA_IMD_ZONE4_Y01:
			ret = SetSysDmvaIMDZone4_y01(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y02:
			ret = SetSysDmvaIMDZone4_y02(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y03:
			ret = SetSysDmvaIMDZone4_y03(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y04:
			ret = SetSysDmvaIMDZone4_y04(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y05:
			ret = SetSysDmvaIMDZone4_y05(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y06:
			ret = SetSysDmvaIMDZone4_y06(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y07:
			ret = SetSysDmvaIMDZone4_y07(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y08:
			ret = SetSysDmvaIMDZone4_y08(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y09:
			ret = SetSysDmvaIMDZone4_y09(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y10:
			ret = SetSysDmvaIMDZone4_y10(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y11:
			ret = SetSysDmvaIMDZone4_y11(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y12:
			ret = SetSysDmvaIMDZone4_y12(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y13:
			ret = SetSysDmvaIMDZone4_y13(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y14:
			ret = SetSysDmvaIMDZone4_y14(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y15:
			ret = SetSysDmvaIMDZone4_y15(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_Y16:
			ret = SetSysDmvaIMDZone4_y16(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE1_LABEL:
			ret = SetSysDmvaIMDZone1_Label(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE2_LABEL:
			ret = SetSysDmvaIMDZone2_Label(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE3_LABEL:
			ret = SetSysDmvaIMDZone3_Label(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_ZONE4_LABEL:
			ret = SetSysDmvaIMDZone4_Label(data, len);
			break;

		/* DMVA IMD save/load */
		case SFIELD_SET_DMVA_IMD_SAVE:
			ret = SetSysDmvaIMDSave(data, len);
			break;
		case SFIELD_SET_DMVA_IMD_LOAD:
			ret = SetSysDmvaIMDLoad(data, len);
			break;

	    /* DMVA Scheduler */
		case SFIELD_SET_DMVA_SCH_ENABLE:
			ret = SetSysDmvaSchEnable(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_RULETYPE:
			ret = SetSysDmvaSchRuleType(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_LOADSETTING:
			ret = SetSysDmvaSchLoadSetting(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_RULELABEL:
			ret = SetSysDmvaSchRuleLabel(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_STARTDAY:
			ret = SetSysDmvaSchStartDay(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_STARTTIME:
			ret = SetSysDmvaSchStartTime(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_ENDDAY:
			ret = SetSysDmvaSchEndDay(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_ENDTIME:
			ret = SetSysDmvaSchEndTime(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_START:
			ret = SetSysDmvaSchStart(data, len);
			break;
		case SFIELD_SET_DMVA_SCH_DELRULE:
			ret = SetSysDmvaSchDelRule(data, len);
			break;

        /* DMVA Enc ROI enable */
 		case SFIELD_SET_DMVA_ENC_ROI_ENABLE:
			ret = SetSysDmvaEncRoiEnable(data, len);
			break;

		default:
			ret = -1;
			break;
	}
	return ret;
}
