/** ===========================================================================
* @file av_server_ctrl.c** @path $(IPNCPATH)\sys_server\src\** @desc* .
* Copyright (c) Appro Photoelectron Inc.  2009
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <asm/types.h>
#include <file_msg_drv.h>
#include <stream_env_ctrl.h>
#include <ApproDrvMsg.h>
#include <Appro_interface.h>
#include <system_default.h>
#include <system_control.h>
#include <rtsp_ctrl.h>
#include <Frame_rate_ctrl.h>
#include <ipnc_ver.h>
#include <remote_debug_msg_drv.h>
#include <netserver_msg_drv.h>
#include <ipnc_gio_util.h>

#define FLAG_STREAM1	(0x01)
#define FLAG_STREAM2	(0x02)
#define FLAG_STREAM3	(0x04)

#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)

int CheckValueBound(int value, int min, int max)
{
	value = (value<min) ? min:value;
	value = (value>max) ? max:value;

	return value;
}
static void attach_opt(char *dst, char *src)
{
	if (strlen(dst) > 0)
		strcat(dst, " ");

	strcat(dst, src);
}

/**
* @brief Start stream
*
* @param *pConfig Pointer to stream settings
* @retval 0 Success
* @retval -1 Fail.
*/
int StartStream(StreamEnv_t* pConfig)
{
	char cmd[1024] = {0};
	char cmdopt[1024] = " ";
	char *stream_type[3] = {"SINGLESTREAM", "DUALSTREAM", "TRISTREAM"};
	char *video_codec_mode[CODEC_COMB_NUM] =
		{"SINGLE_H264", "SINGLE_MPEG4","SINGLE_JPEG",
		 "H264_JPEG", "MPEG4_JPEG", "DUAL_H264", "DUAL_MPEG4", "H264_MPEG4",
		 "TRIPLE_H264", "TRIPLE_MPEG4"};
	char *mecfg_config1[4] = {"HIGH_SPEED1", "HIGH_SPEED1", "HIGH_QUALITY1", "SVC1"};
	char *mecfg_config2[4] = {"HIGH_SPEED2", "HIGH_SPEED2", "HIGH_QUALITY2", "SVC2"};
	char *aewb_con[4] = {"", "AE", "AWB","AEWB"};
	char *aewb_ven[3] = {"NONE2A", "APPRO2A", "TI2A"};
	char adv_VS[] 	= "VS";
	char adv_LDC[] 	= "LDC";
	char adv_SNF[] 	= "SNF";
	char adv_TNF[] 	= "TNF";
	char *adv_VNFMode[2] = {"VNF_SPEED", "VNF_QUALITY"};
	char *glbceMode[3] = {"", "GLBCE HS", "GLBCE HQ"};
	char *glbceStr[3] = {"LOW", "MED", "HIGH"};
	char *sw_mecfg1, *sw_mecfg2;
	static int loaddone=0;


	printf("\n*****************************************************************\n");
	printf("\n    IPNC BUILD VERSION: %s	\n", IPNC_APP_VERSION);
	printf("\n*****************************************************************\n\n");

	if (pConfig -> vsEnable) {
		attach_opt(cmdopt, adv_VS);
	}
	fSetVstabValue(pConfig -> vsEnable);

	if (pConfig -> ldcEnable) {
		attach_opt(cmdopt, adv_LDC);
	}
	fSetLdcValue(pConfig -> ldcEnable);

	if (pConfig -> vnfEnable) {
		if((pConfig -> vnfMode!=0)&&(pConfig -> vnfMode!=3))
			attach_opt(cmdopt, adv_SNF);
		if((pConfig -> vnfMode!=1)&&(pConfig -> vnfMode!=4))
			attach_opt(cmdopt, adv_TNF);
	}
	fSetVnfValue(pConfig -> vnfEnable);

	if(pConfig -> vnfMode>2)
		attach_opt(cmdopt, adv_VNFMode[1]);
	else
		attach_opt(cmdopt, adv_VNFMode[0]);

	attach_opt(cmdopt, stream_type[pConfig -> nStreamType]);
	attach_opt(cmdopt, video_codec_mode[pConfig -> nVideocodecmode]);

	if(pConfig -> nCodectype1!=H264_CODEC) {
		pConfig -> nMEConfig1 = ENCPRESET_AUTO;
		fSetMEConfig1(pConfig -> nMEConfig1);
	}
	if(pConfig -> nCodectype2!=H264_CODEC) {
		pConfig -> nMEConfig2 = ENCPRESET_AUTO;
		fSetMEConfig2(pConfig -> nMEConfig2);
	}

	sw_mecfg1 = mecfg_config1[pConfig -> nMEConfig1];
	sw_mecfg2 = mecfg_config2[pConfig -> nMEConfig2];

#ifdef _ONLY_TI2A
	if(pConfig -> nAEWswitch==1) {
		pConfig -> nAEWswitch = 2;
		fSetImageAEW(pConfig -> nAEWswitch);
	}
#endif

#ifdef _ONLY_APPRO2A
	if(pConfig -> nAEWswitch==2) {
		pConfig -> nAEWswitch = 1;
		fSetImageAEW(pConfig -> nAEWswitch);
	}
#endif

#ifdef WDR_ON_MODE /* Switch OFF AE */
	if(pConfig -> nAEWtype&1) {
		pConfig -> nAEWtype &= 2;
		fSetImageAEWType(pConfig -> nAEWtype);
	}
#endif

	if(pConfig->nAEWswitch >= 0 && pConfig->nAEWswitch < 3)
		attach_opt(cmdopt, aewb_ven[pConfig->nAEWswitch]);

	if(pConfig->nAEWtype >= 0 && pConfig->nAEWtype < 4)
		attach_opt(cmdopt, aewb_con[pConfig->nAEWtype]);

	if(pConfig -> dynRange) {
		attach_opt(cmdopt, glbceMode[pConfig -> dynRange]);
		attach_opt(cmdopt, glbceStr[pConfig -> dynRangeStrength]);
	}

	switch (pConfig -> nVideocodecmode) {
		case CODEC_COMB_TRIPLE_MPEG4:
		{	// Dual MPEG4 + MJPEG
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode9\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s MPEG4 %s MJPEG %d &\n",
				cmdopt,sw_mecfg1,sw_mecfg2, pConfig -> nJpegQuality);

			break;
		}
		case CODEC_COMB_TRIPLE_H264:
		{	// Dual H.264 + MJPEG
			if (pConfig -> nVideocodecres == 3)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 10MP_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);
			}
			else if (pConfig -> nVideocodecres == 2)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 8MP_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);

			}
			else if (pConfig -> nVideocodecres == 1)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 5MP_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);

			}
			else {
				if (pConfig -> nVideocodecres != 0)
				{
					pConfig -> nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode8\n");
				}
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);
				fprintf(stderr, "\n vsenable : %d\n", pConfig -> vsEnable);
			}

			break;
		}
		case CODEC_COMB_H264_MPEG4:
		{	// H.264 + MPEG4
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode7\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s MPEG4 %s &\n",
				cmdopt,sw_mecfg1,sw_mecfg2);

			break;
		}
		case CODEC_COMB_DUAL_MPEG4:
		{	// DUAL MPEG4
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode6\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s MPEG4 %s &\n",
				cmdopt,sw_mecfg1,sw_mecfg2);

			break;
		}
		case CODEC_COMB_DUAL_H264:
		{	// DUAL H.264
		    if(pConfig -> nVideocodecres == 1)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_1080P H264 %s H264 %s &\n",
							cmdopt,sw_mecfg1,sw_mecfg2);
			}
			else
			{
				if (pConfig -> nVideocodecres != 0)
				{
					pConfig -> nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode5\n");
				}
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s H264 %s &\n",
							cmdopt,sw_mecfg1,sw_mecfg2);
			}

			break;
		}
		case CODEC_COMB_MPEG4_JPEG:
		{	// MPEG4 + JPEG
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode4\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s MJPEG %d &\n",
				cmdopt,sw_mecfg1, pConfig -> nJpegQuality);

			break;
		}
		case CODEC_COMB_H264_JPEG:
		{	// H.264 + JPEG
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode3\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s MJPEG %d &\n",
				cmdopt,sw_mecfg1, pConfig -> nJpegQuality);
				
			break;
		}
		case CODEC_COMB_SINGLE_JPEG:
		{	//Single JPEG

			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode2\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MJPEG %d &\n",
				cmdopt, pConfig -> nJpegQuality);

			break;
		}
		case CODEC_COMB_SINGLE_MPEG4:
		{	// SINGLE MPEG4
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode1\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s &\n",
				cmdopt,sw_mecfg1);

			break;
		}
		case CODEC_COMB_SINGLE_H264:
		default :
		{	// SINGLE H.264
			if (pConfig -> nVideocodecmode != CODEC_COMB_SINGLE_H264)
			{
				__E("\nCODEC Mode Error\n");
				pConfig -> nVideocodecmode = CODEC_COMB_SINGLE_H264;
				fSetVideoCodecMode(CODEC_COMB_SINGLE_H264);
			}
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode0\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s &\n",
				cmdopt,sw_mecfg1);

			break;
		}
	}	
	
	if(loaddone==0)
	{
		system(cmd);
	}

	return 0;
}

/**
* @brief Wait AV server ready
*
* @param stream_flag Stream flag to tell which stream is available.
* @retval 0 Success
* @retval -1 Fail.
*/
int WaitStreamReady(__u8 stream_flag)
{
#define BOOT_PROC_TIMEOUT_CNT	(2000)
	AV_DATA vol_data;
	int count=0, ret;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	vol_data.serial = -1;

	if (stream_flag == 0) {
		while(count < BOOT_PROC_TIMEOUT_CNT){
			if(pSysInfo->codec_config.codectype1 == MJPEG_CODEC)
				ret = GetAVData(AV_OP_GET_MJPEG_SERIAL, -1, &vol_data);
			else
				ret = GetAVData(AV_OP_GET_MPEG4_SERIAL, -1, &vol_data);
			if(ret == RET_SUCCESS && vol_data.serial > 10){
				return 0;
			}
			usleep(33333);
		}
	}
	if (stream_flag == 1){
		while(count < BOOT_PROC_TIMEOUT_CNT){
			if(pSysInfo->codec_config.codectype2 == MJPEG_CODEC)
				ret = GetAVData(AV_OP_GET_MJPEG_SERIAL, -1, &vol_data);
			else
				ret = GetAVData(AV_OP_GET_MPEG4_CIF_SERIAL, -1, &vol_data);
			if(ret == RET_SUCCESS && vol_data.serial > 10){
				return 0;
			}
			usleep(33333);			
		}
	}
	if (stream_flag == 2){
		while(count < BOOT_PROC_TIMEOUT_CNT){
			ret = GetAVData(AV_OP_GET_MPEG4_CIF_SERIAL, -1, &vol_data);
			if(ret == RET_SUCCESS && vol_data.serial > 10){
				return 0;
			}
			usleep(33333);
		}
	}
	return -1;
}
static unsigned gExpStep = 0;
static unsigned gSensorGainStep = 0;
static unsigned gPipeGainStep = 0;
static unsigned gmaxsensorGain=0;
static unsigned gminsensorGain=0;
static unsigned gmaxpipeGain=0;
static unsigned gminpipeGain=0;
static unsigned gmaxexposure=0;
static unsigned gminexposure=0;

void InitCamparam()
{
	SysInfo *pSysInfo = GetSysInfo();
	unsigned int expStep,sensorStep,pipeStep;

	GetCam_MinExposure(&gminexposure);

	GetCam_MaxExposure(&gmaxexposure);

	EnvGet_MaxGain(&gmaxsensorGain,&gmaxpipeGain);

	EnvGet_MinGain(&gminsensorGain,&gminpipeGain);
	
	expStep = (gmaxexposure - gminexposure)/256;
	if(expStep == 0)
		expStep = 1;

	sensorStep = (gmaxsensorGain-gminsensorGain)/256;
	if(sensorStep == 0)
		sensorStep = 1;

	pipeStep = 	(gmaxpipeGain-gminpipeGain)/256;
	if(pipeStep == 0)
		pipeStep = 1;
			
	gExpStep = expStep;	
	gSensorGainStep = sensorStep;
	gPipeGainStep = pipeStep;
	
	SetMirr(pSysInfo->lan_config.mirror);
	SetBinningSkip(pSysInfo->lan_config.nBinning);
	SetCamDayNight(pSysInfo->lan_config.nDayNight);
	SetWhiteBalance(pSysInfo->lan_config.nWhiteBalance);
	SetBacklight(pSysInfo->lan_config.nBackLight);
	SetBrightness(pSysInfo->lan_config.nBrightness);
	SetContrast(pSysInfo->lan_config.nContrast);
	SetSaturation(pSysInfo->lan_config.nSaturation);
	SetSharpness(pSysInfo->lan_config.nSharpness);	

	SetFramectrl(pSysInfo->lan_config.framectrl);
	SetAdvMode();

	/**FILL LIGHT***/
	ipnc_gio_write(GIO_FILL_LIGHT,pSysInfo->light_config.led[0].enable);
	
	/****** Config AEWB ********/
	SetImage2AType(0);
	EnvSetH2AWeight((char *)&(pSysInfo->lan_config.H3aWeight),sizeof(EzCamH3AWeight));
	EnvSetH2AParams(&(pSysInfo->lan_config.H3AParams));
	SetImageAEW(pSysInfo->lan_config.nAEWswitch);
	SetImageAEWType(pSysInfo->lan_config.nAEWtype);
	Set2APriority(pSysInfo->lan_config.expPriority);
	
	if(pSysInfo->lan_config.nAEWtype == 0)
	{
		EnvSetGain(pSysInfo->lan_config.sensorGain,pSysInfo->lan_config.pipeGain);
		SetCamExposure(pSysInfo->lan_config.exposure);
	}	
		
	/** Config alg param ***/
	Alg_Params	*pPrm;
	pPrm = &(pSysInfo->algPrm);
	EnvSetAlgParam(pPrm->prm,pPrm->prmSize);
	
	if(pSysInfo->state.algTriggle)
		EnvSetAlgMode(1);	
	else
		EnvSetAlgMode(0);	

	if(pSysInfo->state.algImgsrc)
		EnvImgAlg_UseSensorImg();
	else
		EnvImgAlg_UseStaticImg();	

	if(pSysInfo->state.enableEncode)
		EnvEnableEncodeImg();
	else
		EnvDisableEncodeImg();			
}
/**
* @brief Initial Stream server
*
* @param *pConfig Stream server comunicate setting
* @retval 0 Success
* @retval -1 Fail.
*/
int Init_StreamEnv(StreamEnv_t* pConfig)
{
	__u8 stream_flag = 0;
	McFWInitStatus initStatus;

	if(ApproDrvInit(SYS_MSG_TYPE)){
		__E("ApproDrvInit fail\n");
		return -1;
	}

	/*	This code makes sure McFW message handler is up and running */
	while(ApproPollingMsg() < 0){
		usleep(240000);
	}

	/* Wait for McFW init Done */
	initStatus.initDone = 0;
	do
	{
		GetMcFWInitStatus(&initStatus);
		if(initStatus.initDone == 0)
		{
			usleep(500 * 1000);
		}
	}while(initStatus.initDone == 0);

	/* Get the stream memory address from McFW */
	if(func_get_mem(NULL)){
		ApproDrvExit();
		__E("CMEM map fail\n");
		return -1;
	}

	SetVideoCodecType(pConfig -> nVideocodecmode, pConfig->nVideocodecres);

	if(pConfig -> nVideocodecmode >= CODEC_COMB_TRIPLE_H264) {
		stream_flag = (FLAG_STREAM1|FLAG_STREAM2|FLAG_STREAM3);
	}
	else if(pConfig -> nVideocodecmode >= CODEC_COMB_H264_JPEG) {
		stream_flag = (FLAG_STREAM1|FLAG_STREAM2);
	}
	else {
		stream_flag = FLAG_STREAM1;
	}

	if(WaitStreamReady(pConfig -> nStreamType) != 0) {
		ApproDrvExit();
		__E("WaitStreamReady Fail.\n");
		return -1;
	}

	if((stream_flag & FLAG_STREAM1) > 0) {
		SetMP41bitrate(pConfig -> nBitrate1);
		SetEncRateControl(0, pConfig -> nRateControl1);
		SetFramerate1(pConfig -> nFrameRate1);
	}
	if((stream_flag & FLAG_STREAM2) > 0) {
		SetMP42bitrate(pConfig -> nBitrate2);
		SetEncRateControl(1, pConfig -> nRateControl2);
		SetFramerate2(pConfig -> nFrameRate2);
	}
	if((stream_flag & FLAG_STREAM3) > 0) {
		SetJpgQuality(pConfig -> nJpegQuality);
		SetFramerate3(pConfig -> nFrameRate3);
	}

	InitCamparam();
	
	/** start netserver **/
	SendNetServerStartCmd();
	
	return 0;
}

/**
* @brief Set Binning
*
* @param value 0:binning ; 1:skipping
* @return function to set Binning
*/
int SetBinning(unsigned char value) // 0:binning / 1:skipping
{
	int ret = 0;
	unsigned char prev;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.nBinning!=value) {
		prev = pSysInfo->lan_config.nBinning;
		if((ret = fSetBinning(value)) == 0)
		{
			SetBinningSkip(value);
		}
	}

	return ret;
}
int GetBinning(unsigned char *value) // 0:binning / 1:skipping
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.nBinning;

	return 0;
}

int GetCamDayNight(unsigned char *value) 
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.nDayNight;

	return 0;
}
/**
* @brief Set day or night
*
* @param value value of day or night
* @return function to set day or night
*/
int SetCamDayNight(unsigned char value)
{
	int i, j;
	unsigned int frameRateVal1, frameRateVal2, frameRateVal3;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	if (pSysInfo->lan_config.nDayNight == value)
		return 0;

	i = pSysInfo->codec_config.nVideocodecmode;
	j = pSysInfo->codec_config.nVideocodecres;

	if(fSetCamDayNight(value)==0) {
		SetDayNight(value);
		frameRateVal1 = frame_rate_control(value, i, j, 1, 0);
		frameRateVal2 = frame_rate_control(value, i, j, 2, 0);
		frameRateVal3 = frame_rate_control(value, i, j, 3, 0);
		fSetFramerate1(0);fSetFramerate2(0);fSetFramerate3(0);
		fSetFramerateVal1(frameRateVal1);fSetFramerateVal2(frameRateVal2);fSetFramerateVal3(frameRateVal3);
		SetStreamFramerate(0, frameRateVal1*1000);
		SetStreamFramerate(1, frameRateVal2*1000);
		SetStreamFramerate(2, frameRateVal3*1000);
	}
	else
		return -1;

	return 0;
}

int SetCamExposure(unsigned char value)
{
	unsigned int exposure;

	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	exposure =  gExpStep * value + gminexposure;
	
	SetExposure(exposure);
	pSysInfo->lan_config.exposure = value;

	return 0;	
}

int GetCamExposure(unsigned char *value)
{
	unsigned int exposure;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	GetExposure(&exposure);

	*value = (exposure - gminexposure)/gExpStep;
	
	pSysInfo->lan_config.exposure = *value;

	return 0;	
}

int GetCam_MaxExposure(unsigned int *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	Get_MaxExposure(value);

	return 0;	
}

int GetCam_MinExposure(unsigned int *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	Get_MinExposure(value);

	return 0;	
}

int EnvGet_MaxGain(unsigned int *sensorGain,unsigned int *pipeGain)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	Get_MaxGain(sensorGain,pipeGain);

	*pipeGain = 2050;
	
	return 0;	
}

int EnvGet_MinGain(unsigned int *sensorGain,unsigned int *pipeGain)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	Get_MinGain(sensorGain,pipeGain);
	
	return 0;	
}

int EnvSetGain(unsigned char sensorGain,unsigned char pipeGain)
{
	unsigned int sensor_Gain,pipe_Gain;

	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;
	
	sensor_Gain = gSensorGainStep * sensorGain + gminsensorGain;
	pipe_Gain = gPipeGainStep * pipeGain + gminpipeGain;
	
	SetGain(sensor_Gain,pipe_Gain);
	
	pSysInfo->lan_config.sensorGain = sensorGain;
	pSysInfo->lan_config.pipeGain = pipeGain;
	
	return 0;	

}

int EnvGetGain(unsigned char *sensorGain,unsigned char *pipeGain)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;
		
	unsigned int gain_sensor,gain_pipe;

	GetGain(&gain_sensor,&gain_pipe);

	*sensorGain = (gain_sensor - gminsensorGain)/gSensorGainStep;
	*pipeGain = (gain_pipe-gminpipeGain)/gPipeGainStep;
	pSysInfo->lan_config.sensorGain = *sensorGain;
	pSysInfo->lan_config.pipeGain = *pipeGain;

	return 0;
	
}

/************* H3A SETTING ***************/
int EnvGetH2AParams(EzCamH3AParam *pPrm)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;
	
	GetH3AParam(pPrm);

	memcpy(&(pSysInfo->lan_config.H3AParams),pPrm,sizeof(EzCamH3AParam));
	
	return 0;	
}

int EnvSetH2AParams(EzCamH3AParam *pPrm)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	SetH3AParam(pPrm);

	memcpy(&(pSysInfo->lan_config.H3AParams),pPrm,sizeof(EzCamH3AParam));
	return 0;	
}

int EnvGetH2AWeight(char *weight,int *size)
{	
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;
			
	*size = sizeof(EzCamH3AWeight);
		
	GetH3AWeight(weight,*size);

	memcpy(&(pSysInfo->lan_config.H3aWeight),weight,sizeof(EzCamH3AWeight));
	
	return 0;	
}

int EnvSetH2AWeight(char *weight,int size)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;
		
	SetH3AWeight(weight,size);

	memcpy(&(pSysInfo->lan_config.H3aWeight),weight,sizeof(EzCamH3AWeight));
	return 0;	
}
/**
* @brief Set MPEG4-1 frame rate
*
* @param "unsigned char value": selected frame rate
* @return function to set MPEG4-1 frame rate
* @retval 0: success
* @retval -1: failed to set MPEG4-1 frame rate
*/
int SetFramerate1(unsigned char value)
{
	int i = 0, j = 0, ret = 0;
	unsigned int frameRateVal;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	i = pSysInfo->codec_config.nVideocodecmode;
	j = pSysInfo->codec_config.nVideocodecres;

	ret = fSetFramerate1(value);

	if(ret == 0) {
		frameRateVal = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 1, value);
		ret = fSetFramerateVal1(frameRateVal);
		SetStreamFramerate(0, frameRateVal*1000);
	}

	return ret;
}

int SetFramerate2(unsigned char value)
{
	int i = 0, j = 0, ret = 0;
	unsigned int frameRateVal;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	i = pSysInfo->codec_config.nVideocodecmode;
	j = pSysInfo->codec_config.nVideocodecres;

	ret = fSetFramerate2(value);

	if(ret == 0) {
		frameRateVal = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 2, value);
		ret = fSetFramerateVal2(frameRateVal);
		SetStreamFramerate(1, frameRateVal*1000);
	}

	return ret;
}
int SetFramerate3(unsigned char value)
{
	int i = 0, j = 0, ret = 0;
	unsigned int frameRateVal;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	i = pSysInfo->codec_config.nVideocodecmode;
	j = pSysInfo->codec_config.nVideocodecres;

	ret = fSetFramerate3(value);

	if(ret == 0) {
		frameRateVal = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 3, value);
		ret = fSetFramerateVal3(frameRateVal);
		SetStreamFramerate(2, frameRateVal*1000);
	}

	return ret;
}

/**
* @brief Set white balance
*
* @param value value of white balance
* @return function to set white balance
*/
int SetCamWhiteBalance(unsigned char value)
{
	SetWhiteBalance(value);
	return fSetCamWhiteBalance(value);
}

int GetCamWhiteBalance(unsigned char *value)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	else
		*value = pSysInfo->lan_config.nWhiteBalance;		

	return 0;	
}
/**
* @brief Set backlight value
*
* @param value backlight value
* @return function to set backlight value
*/
int SetCamBacklight(unsigned char value)
{
	SetBacklight(value);
	return fSetCamBacklight(value);
}

/**
* @brief Set brightness
*
* @param value brightness value
* @return function to set brightness value
*/
int IncreaseCamExposure(unsigned char value)
{
//	InCreaseExposure(value);

	return 0;
}


/**
* @brief Set brightness
*
* @param value brightness value
* @return function to set brightness value
*/
int DecreaseCamExposure(unsigned char value)
{
//	DeCreaseExposure(value);

	return 0;
}

/**
* @brief Set brightness
*
* @param value brightness value
* @return function to set brightness value
*/
int SetCamBrightness(unsigned char value)
{
	SetBrightness(value);
	return fSetCamBrightness(value);
}

int GetCamBrightness(unsigned char *value)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	else		
		*value = pSysInfo->lan_config.nBrightness;	
		
	return 0;	
}
/**
* @brief Set contrast
*
* @param value contrast value
* @return function to set contrast value
*/
int GetCamContrast(unsigned char *value)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	else
		*value = pSysInfo->lan_config.nContrast;	

	return 0;	
}
/**
* @brief Set saturation
*
* @param value saturation value
* @return function to set saturation value
*/
int SetCamContrast(unsigned char value)
{
	SetSaturation(value);
	return fSetCamSaturation(value);
}

int SetCamSaturation(unsigned char value)
{
	SetSaturation(value);
	return fSetCamSaturation(value);
}
int GetCamSaturation(unsigned char *value)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	else
		*value =  pSysInfo->lan_config.nSaturation;	

	return 0;	
}
/**
* @brief Set sharpness
*
* @param value sharpness value
* @return function to set sharpness value
*/
int SetCamSharpness(unsigned char value)
{
	SetSharpness(value);
	return fSetCamSharpness(value);
}

int GetCamSharpness(unsigned char *value)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	else
		*value =  pSysInfo->lan_config.nSharpness;		

	return 0;	
}

/**
 * @brief Set backlight status
 *
 * @param value on or off of backlight
 * @return function to set backlight status
 */
int SetBlc(unsigned char value)
{
		return fSetBlc(value);
}

int GetBlc(unsigned char *value)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	else
		*value =  pSysInfo->lan_config.expPriority;	

	return 0;		
}

/**
* @brief Clear System Log
*
* @param value sharpness value
* @return function to set sharpness value
*/
int SetClearSysLog(unsigned char value)
{
	return fSetClearSysLog(value);
}

/**
* @brief Clear Access Log
*
* @param value sharpness value
* @return function to set sharpness value
*/
int SetClearAccessLog(unsigned char value)
{
	return fSetClearAccessLog(value);
}


#define ADVANCED_ALL_VALUES	(5)
unsigned int advFeatCount=0;

int SetAdvMode(void)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount=0;

	if(pSysInfo->lan_config.advanceMode.vnfMode>2)
	{
		SetLdcEnableMsg(pSysInfo->lan_config.advanceMode.ldcEnable);
	}

	if(1)
	{
		VnfParam vnfParam;

		vnfParam.enable 	= pSysInfo->lan_config.advanceMode.vnfEnable;
		vnfParam.mode 		= pSysInfo->lan_config.advanceMode.vnfMode;
		vnfParam.strength 	= pSysInfo->lan_config.advanceMode.vnfStrength;

		SetVNFParamMsg(&vnfParam);
	}

	return ret;
}

int SetVstabValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	if(value != pSysInfo->lan_config.advanceMode.vsEnable){
		if ((ret = fSetVstabValue(value)) == 0) {
            SetRestart();
		}
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}

int GetVstabValue(unsigned char *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.advanceMode.vsEnable;

	return 0;
}
int SetLdcValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	if(value != pSysInfo->lan_config.advanceMode.ldcEnable){
		if ((ret = fSetLdcValue(value)) == 0) {
            SetRestart();
		}
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}
int GetLdcValue(unsigned char *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.advanceMode.ldcEnable;

	return 0;
}
int SetVnfValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	if(value != pSysInfo->lan_config.advanceMode.vnfEnable){
		ret = fSetVnfValue(value);
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}

int GetVnfValue(unsigned char *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.advanceMode.vnfEnable;

	return 0;
}

int SetVnfMode(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	if(value != pSysInfo->lan_config.advanceMode.vnfMode){
		ret = fSetVnfMode(value);
#ifdef MEMORY_256MB
		int prev_mode 	= (pSysInfo->lan_config.advanceMode.vnfMode>2)?1:0;
		int new_mode 	= (value>2)?1:0;
		if(ret == 0)
		{
				/*
				 * In 256 MB memory scheme dynamic switching between NF is NOT supported.
				 * So there will be McFW restart.
				 */
				if(prev_mode != new_mode)
				{
						SetRestart();
				}
		}
#endif
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}

int GetVnfMode(unsigned char *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	*value = pSysInfo->lan_config.advanceMode.vnfMode;

	return 0;
}
int SetVnfStrength(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	ret = fSetVnfStrength(value);

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}

int GetVnfStrength(unsigned char *value)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	else
		*value = pSysInfo->lan_config.advanceMode.vnfStrength;	

	return 0;	
}


#define DYNRANGE_ALL_VALUES	(2)
unsigned int dynRangeCount=0;

int SetDynRangeParam(void)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	dynRangeCount=0;

	DynRangePrm dynRangePrm;

	dynRangePrm.enable 		= (pSysInfo->lan_config.advanceMode.dynRange>0)?1:0;
    dynRangePrm.mode        = pSysInfo->lan_config.advanceMode.dynRange;
    dynRangePrm.level       = pSysInfo->lan_config.advanceMode.dynRangeStrength;

	SetDynRangePrmMsg(&dynRangePrm);

	return 0;
}

int SetDynRange(unsigned char value)
{
	int ret=0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	dynRangeCount++;

	int prev_val = pSysInfo->lan_config.advanceMode.dynRange;

	if(value != prev_val) {
		if ((ret = fSetDynRange(value)) == 0) {
			if((prev_val==0) || (value==0)) {
                SetRestart();
		    }
		}
	}

	if(dynRangeCount==DYNRANGE_ALL_VALUES)
		SetDynRangeParam();

	return ret;
}

int SetDynRangeStrength(unsigned char value)
{
	int ret=0;

	dynRangeCount++;

	ret = fSetDynRangeStrength(value);

	if(dynRangeCount==DYNRANGE_ALL_VALUES)
		SetDynRangeParam();

	return ret;
}

int SetFramectrlValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	if (value != pSysInfo->lan_config.framectrl)
		SetFramectrl(value);
	ret = fSetFrameCtrl(value);
	return ret;
}
void ResetBasicDefault(void)
{
	fSetVstabValue(VS_DEFAULT);fSetLdcValue(LDC_DEFAULT);
	fSetVnfValue(VNF_DEFAULT);fSetVnfMode(VNF_MODE_DEFAULT);fSetVnfStrength(VNF_STRENGTH_DEFAULT);
	fSetDynRange(DYNRANGE_DEF);fSetDynRangeStrength(DYNRANGE_STRENGTH);
	fSetMP41bitrate(MPEG41BITRATE_DEFAULT); fSetMP42bitrate(MPEG42BITRATE_DEFAULT);
	fSetVideoCodecMode(VCODECMODE_DEFAULT); fSetVideoCodecRes(VCODECRES_DEFAULT);
	fSetVideoCodecCombo(VCODECCOMBO_DEFAULT); fSetVideoMode(STREAMTYPE_DEFAULT);
}

int SetHistogram(unsigned char value)
{
	int ret = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->lan_config.histogram) {
		ret = fSetHistogram(value);
		SetHistEnable(value);
	}

	return 0;
}


int SetExpPriority(unsigned char value)
{
	int ret = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->lan_config.expPriority) {
		ret = fSetExpPriority(value);
		Set2APriority(value);
	}

	return 0;
}
int SetIRIS(unsigned char value)
{
	SetDCIRIS(value);
	return 0;
}
/**
* @brief Set MPEG4-1 bitrate
*
* @param value MPEG4-1 bitrate value
* @return function to set MPEG4-1 bitrate
* @retval -1 failed to set MPEG4-1 bitrate
*/

int SetMP41bitrate(unsigned int value)
{
	SysInfo *pSysInfo = GetSysInfo();
	unsigned char nVideocodecmode, nVideocodecres;
	if(pSysInfo == NULL)
		return -1;

	nVideocodecmode= pSysInfo->codec_config.nVideocodecmode;
	nVideocodecres = pSysInfo->codec_config.nVideocodecres;

	value = CheckValueBound(value, 64000, 20000000);

	SetEncBitrate(0, value);
	return fSetMP41bitrate(value);
}

/**
* @brief Set MPEG4-2 bitrate
*
* @param value MPEG4-2 bitrate value
* @return function to set MPEG4-2 bitrate
* @retval -1 failed to set MPEG4-2 bitrate
*/
int SetMP42bitrate(unsigned int value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = CheckValueBound(value, 64000, 8000000);

	SetEncBitrate(1, value);
	return fSetMP42bitrate(value);
}

/**
* @brief Set motion JPEG quality
*
* @param value : QP value
* @return function to set motion JPEG quality
*/
int SetMJPEGQuality(unsigned char value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = (unsigned char)CheckValueBound((int)value, 2, 97);

	if(value != pSysInfo->codec_config.njpegquality) {
		SetJpgQuality(value);
		return fSetJpegQuality(value);
	}
	else
		return 0;
}

int SetMirror(unsigned char value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->lan_config.mirror) {

		SetMirr(value);
		return fSetMirror(value);

	}
	return 0;

}

int GetMirror(unsigned char *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.mirror;

	return 0;
}

/**
* @brief Set image AEW
*
* @param "unsigned char value": 0: OFF, 1: APPRO
* @return SUCCESS: 0, FAIL: -1
*/
int SetImageAEW(unsigned char value)/*img2a*/
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

#ifdef _ONLY_TI2A
	if(value==1) {
		value = 2;
	}
#endif

#ifdef _ONLY_APPRO2A
	if(value==2) {
		value = 1;
	}
#endif

	if ((ret = fSetImageAEW(value)) == 0) {
			SetImage2AVendor(value);
		}

	return ret;
}

int GetImageAEW(unsigned char *value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.nAEWswitch;

	return 0;
}

/**
* @brief Set image AEW
*
* @param "unsigned char value": 0: OFF, 1: APPRO
* @return SUCCESS: 0, FAIL: -1
*/
int SetImageAEWType(unsigned char value)/*img2a*/
{
	int ret = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

#ifdef WDR_ON_MODE /* Switch OFF AE */
	if(value&1) {
		value &= 2;
	}
#endif

	if ((ret = fSetImageAEWType(value)) == 0) 
	{
		SetImage2AType(value);
	}
	
	return ret;
}

int GetImageAEWType(unsigned char *value)/*img2a*/
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	*value = pSysInfo->lan_config.nAEWtype;

	return 0;
}


int EnvResetPayload()
{
	ResetPayload();
	
	return 0;
}

int EnvStartPayload()
{
	StartPayload();
	
	return 0;
}
int EnvPrintPayload()
{
	PrintPayload();
	
	return 0;
}
int EnvGetPayload(EzCpuDynamicParam *prf)
{
	GetPayload(prf);

	return 0;
}

int EnvGetFrameImg(EzFrameInfo *info)
{
	int ret = 0;
	printf("EnvGetFrameImg \n");
	ret = GetFrameImg(info);
	if(ret<0)
	{
		printf("GetFrameImg fail\n");
		return -1;
	}

	return 0;
}

int EnvReleaseFrameImg(EzFrameInfo *info)
{
	int ret = 0;
	ret = ReleaseFrameImg(info);
	if(ret<0)
	{
		printf("ReleaseFrameImg fail\n");
		return -1;
	}

	return 0;
}

int EnvEnableEncodeImg()
{
	int ret;
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
			
	ret = EnableEncodeImg();
	if(ret < 0)
		return -1;
	pSysInfo->state.enableEncode = 1;
		
	return 0;
}

int EnvDisableEncodeImg()
{
	int ret;
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
			
	ret = DisableEncodeImg();
	if(ret < 0)
		return -1;
	pSysInfo->state.enableEncode = 0;
		
	return 0;
}

/********* FILL LIGHT *********/
int EnvSetLedConfig(CamLight_Config *light)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
#if 1
	if(light->led[0].enable != pSysInfo->light_config.led[0].enable)
		ipnc_gio_write(GIO_FILL_LIGHT,light->led[0].enable);
#endif			
	memcpy(&(pSysInfo->light_config),light,sizeof(CamLight_Config));
		
	return 0;
}

int EnvGetLedConfig(CamLight_Config *light)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
			
	memcpy(light,&(pSysInfo->light_config),sizeof(CamLight_Config));
		
	return 0;
}

/********* ALG **********/
int EnvGetAlgResult(Alg_Result *result)
{
	return GetAlgResult(result);
}

int EnvImgAlg_UseSensorImg()
{
	int ret;
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
		
	ret = ImgAlg_UseSensorImg();	

	if(ret < 0)
		return -1;
	else
		pSysInfo->state.algImgsrc = 1;

	return 0;
}

int EnvImgAlg_UseStaticImg()
{
	int ret;
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
		
	ret = ImgAlg_UseSensorImg();	

	if(ret < 0)
		return -1;
	else
		pSysInfo->state.algImgsrc = 0;	

	return 0;	
}

int EnvDisableImgAlg()
{
	return DisableImgAlg();	
}

int EnvEnableImgAlg()
{
	return EnableImgAlg();	
}

int EnvSetAlgParam(char *pPrm,int prmSize)
{
	Alg_Params *pAlgPrm;
	if(prmSize > ALGPARAM_MAX_SIZE)
		return -1;
		
	pAlgPrm = GetAlgPrmInfo();
	if(NULL == pAlgPrm)
		return -1;
	
	if(SetAlgParam(pPrm,prmSize)<0)
		return -1;
	else
	{
		pAlgPrm->prmSize = prmSize;
		memcpy(pAlgPrm->prm,pPrm,prmSize);
	}			

	return 0;
}

int EnvGetAlgParam(char *pPrm,int *prmSize)
{
	Alg_Params *pAlgPrm;
	pAlgPrm = GetAlgPrmInfo();
	if(NULL == pAlgPrm)
		return -1;
	if(pAlgPrm->prmSize >= ALGPARAM_MAX_SIZE)
	{
		*prmSize = 0;
		return -1;
	}
	else
	{
		*prmSize = pAlgPrm->prmSize;
		memcpy(pPrm,pAlgPrm->prm,pAlgPrm->prmSize);
	}

	return 0;
}

int EnvUpdate_AlgPrmfile(char *pPrm,int prmSize)
{
	Alg_Params *pAlgPrm;

	if((pPrm == NULL) || (prmSize >= ALGPARAM_MAX_SIZE))
		return -1;
		
	pAlgPrm = GetAlgPrmInfo();
	if(NULL == pAlgPrm)
		return -1;

	Alg_Params *oldPrm = (Alg_Params *)malloc(ALGPARAM_MAX_SIZE);

	if(oldPrm == NULL)
		return -1;
	oldPrm->prmSize = pAlgPrm->prmSize;

	if(oldPrm->prmSize < ALGPARAM_MAX_SIZE)
		memcpy(oldPrm->prm,pAlgPrm->prm,oldPrm->prmSize);

	pAlgPrm->prmSize = prmSize;
	memcpy(pAlgPrm->prm,pPrm,prmSize);
	if(fUpdateAlgPrmFile() == 0)
	{
		free(oldPrm);
		return 0;
	}
	else
	{
		memcpy(pAlgPrm,oldPrm,sizeof(Alg_Params) + sizeof(oldPrm->prmSize));
		free(oldPrm);
		return -1;
	}
}
int EnvSetAlgMode(unsigned debugMode)
{
	int ret;
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;

	ret = SetAlgMode(debugMode);
	if(ret < 0)
		return -1;
		
	pSysInfo->state.algTriggle = debugMode;
	
	return 0;
}



/***
 *
 *Remoter Debug Client
 *
 */
void EnvSendRemoteDebugQuitCmd()
{
	SendRemoteDebugQuitCmd();
}

void EnvSendRemoteDebugSerialCmd()
{
	SendRemoteDebugSerialCmd();		
}

int EnvSendRemoteDebugSocketCmd(unsigned int port,char *ip)
{
	return SendRemoteDebugSocketCmd(port,ip);		
}

void EnvSendRemoteDebugNONECmd()
{
	SendRemoteDebugNONECmd();	
} 
