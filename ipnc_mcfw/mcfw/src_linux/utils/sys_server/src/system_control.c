/* ===========================================================================
* @path $(IPNCPATH)\sys_adm\system_server
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
* @file system_contorl.c
* @brief Send commands to system server.
*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system_control.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include "file_msg_drv.h"
#include <ApproDrvMsg.h>
#include "net_config.h"
#include "sd_config.h"
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <Appro_interface.h>
#include <sem_util.h>
#include <system_default.h>
#include <ipnc_ver.h>
#include <time.h>
#include <alarm_msg_drv.h>
#include <ipnc_gio_util.h>
#include <avi_mng.h>
#include <jpg_mng.h>
#include <schedule_mng.h>
#include <alarm_schedule.h>
#include <gio_alarmout.h>
#include <stream_env_ctrl.h>
#include <signal.h>
#include <rtsp_ctrl.h>
#include <semaphore.h>

#define PWM1_PERIOD			4166000
#define DROP_FILESYS_CMD	"echo 1 > /proc/sys/vm/drop_caches"

//#define SYS_CTRL_DEBUG
//#define SYS_CTRL_ERROR

#ifdef SYS_CTRL_DEBUG
#define DBG(fmt, args...)	fprintf(stderr, fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#ifdef SYS_CTRL_ERROR
#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)
#else
#define __E(fmt, args...)
#endif

unsigned char restart_flag = 0;
unsigned char default_flag = 0;
static SemHandl_t hndlSysSem = NULL;
static int network_en = 0;
static SysInfo* gpSysInfo;
/* Onvif Semaphore id for motion detection */
int id_motion_sem;

int UpdateEnvFromFile(SysInfo* pSysInfo);
/* Current recording file name in SD card */
extern void CleanSysMsg();

/**
* @brief If restart.
*
* @return restart flag.
*/
int IsRestart()
{
	int ret;
	SysInfo *pSysInfo = GetSysInfo();

	SemWait(hndlSysSem);
	ret = restart_flag;
	SemRelease(hndlSysSem);

	if(pSysInfo->net.rtsp_config.change) {
                ret = 1;
                pSysInfo->net.rtsp_config.change = 0;
        }

	return ret;
}

void SetRestart()
{
	SemWait(hndlSysSem);
	restart_flag = 1;
	SemRelease(hndlSysSem);
}

/**
* @brief Clean restart flag.
*
*/
void CleanRestart()
{
	SemWait(hndlSysSem);
	restart_flag = 0;
	SemRelease(hndlSysSem);
}

int IsResetDefault()
{
	int ret;
	SemWait(hndlSysSem);
	ret = default_flag;
	SemRelease(hndlSysSem);
	return ret;
}

void SetDefault()
{
	SemWait(hndlSysSem);
	default_flag = 1;
	SemRelease(hndlSysSem);
}

void CleanSetDefault()
{
	SemWait(hndlSysSem);
	default_flag = 0;
	SemRelease(hndlSysSem);
}


/**
* @brief Check if USB is insertpPrm->net.lan.MACed
*
* @retval 1 Inserted
* @retval 0 Not inserted
*/
int IsUsbInserted()
{
	int ret = 0;
	FILE *fp;
	fp = fopen(USB_PATH, "rb");
	if(fp){
		ret = 1;
		fclose(fp);
	}
	DBG("%s : %d\n", __func__, ret);
	return ret;
}
/**
* @brief Check if USB is connected
*
* @retval 1 Connect
* @retval 0 Not connect
*/
int IsUsbConnect()
{
	int ret = 0;
	FILE *fp;
	fp = fopen(USB_CONNECT_PATH, "rb");
	if(fp){
		ret = 1;
		fclose(fp);
	}
	DBG("%s : %d\n", __func__, ret);
	return ret;
}

int RestartNetwork()
{
	int RTSP_server_message_queue_id;
	int nExtFunc = 0;

	SysInfo *pSysInfo = GetSysInfo();
	RTSP_server_message_queue_id = msg_queue_wis_streamer();
    pSysInfo->net.rtsp_config.mtype = Message_Wis_Streamer;
        
	do
	{
		SetRTSPStreamingOFF();
		sleep(2);

		Msg_Send( RTSP_server_message_queue_id , (void*)(&(pSysInfo->net.rtsp_config)) , (sizeof(pSysInfo->net.rtsp_config)-sizeof(long)) );

		sleep(2);
		nExtFunc = GetRTSPStreamOption();
		SetRTSPStreamingON(nExtFunc);	
		
	}while(0);

	return 0;
}

int StartNetwork(SysInfo *pSysInfo)
{
	if(pSysInfo == NULL){
		return -1;
	}

	
	if(net_get_hwaddr(ETH_NAME, pSysInfo->net.net_config.MAC) != 0){
		__E("Error on get MAC address\n");
		return -1;
	}
		
	if(1){
		system("ifconfig eth0 up\n");
		if(net_set_ifaddr(ETH_NAME, inet_addr(pSysInfo->net.net_config.ip)) < 0){
			__E("Error on Set ip\n");
			return -1;
		}
			
		/* set net mask */
		printf("set net mask\n");
		if(net_set_netmask(ETH_NAME, inet_addr(pSysInfo->net.net_config.netmask)) < 0){
			__E("Fail on set netmask\n");
			return -1;
		}
		
		/* set gateway */
		printf("set net gateway\n");
		if(net_set_gateway(inet_addr(pSysInfo->net.net_config.gateway)) < 0){
			__E("Fail on set gateway\n");
			return -1;
		}	
		/* set DNS */
		printf("set net dns\n");
		if(net_set_dns(pSysInfo->net.net_config.dns) < 0){
			__E("Fail on set dns\n");
			return -1;
		}
	}
	/* set http port will be done by BOA */
	return 0;
}

void RestartSystem()
{
	system("reboot &");
}

static int BootProc1()
{
	DBG("%s\n", __func__);
		
	if(!network_en){
			if(StartNetwork(gpSysInfo)){
			__E("Network Init Fail\n");
			return -1;
		}
		network_en = 1;
	}
	return 0;
}

/**
* @brief System server thread
*/
static void *PollingThread(void *arg){
	int nExtFunc = 0;
	unsigned int period = 0;
	unsigned int flag = 0;
	
	while(1)
	{
		if(network_en)
		{
			nExtFunc = GetRTSPStreamOption();
			SetRTSPStreamingON(nExtFunc);

			network_en = 0;
			break;
		}
		
		sleep(1);
	}
#if 0
	while(1)
	{
		if(gpSysInfo->light_config.led[0].enable)
		{
			period = 1000000*gpSysInfo->light_config.led[0].pwmduty/100;

			ipnc_gio_write(GIO_FILL_LIGHT,1);
			usleep(period);	
			ipnc_gio_write(GIO_FILL_LIGHT,0);
			usleep(1000000 - period);	
		}			
		else
		{
			ipnc_gio_write(GIO_FILL_LIGHT,0);
			sleep(5);
		}
		
	}
#endif	
	return NULL;
}

/**
* @brief Initial system
*
* @retval 0 Success
* @retval -1 System initial error
*/
int SystemInit()
{
	pthread_t thread;
	StreamEnv_t tConfig;
	SysInfo *pSysInfo;
	int RTSP_server_message_queue_id ;

	if(Platform_check() == -1){
		__E("Platform_check fail\n");
		return -1;
	}

#ifndef TEARDOWN_LOAD_UNLOAD
		system("./scripts/load_vpss.sh&");
		system("./scripts/load_video.sh&");
	#ifdef IPNC_DSP_ON
		system("./scripts/load_c6xdsp.sh&");
	#endif

		system("./scripts/wait_cmd.sh s m3vpss");
		system("./scripts/wait_cmd.sh s m3video");
	#ifdef IPNC_DSP_ON
		system("./scripts/wait_cmd.sh s c6xdsp");
	#endif
#endif

	gpSysInfo = pSysInfo = GetSysInfo();
	if(pSysInfo == NULL){
		CleanupFileMsgDrv();
		return -1;
	}

	if((hndlSysSem = MakeSem()) == NULL){
		CleanupFileMsgDrv();
		pSysInfo = NULL;
		return -1;
	}
	if(RTSP_ControlInit() != 0){
		DestroySem(hndlSysSem);
		hndlSysSem = NULL;
		CleanupFileMsgDrv();
		pSysInfo = NULL;
		return -1;
	}

	RTSP_server_message_queue_id = msg_queue_wis_streamer();
        pSysInfo->net.rtsp_config.mtype = Message_Wis_Streamer;
        Msg_Send( RTSP_server_message_queue_id , (void*)(&(pSysInfo->net.rtsp_config)) , (sizeof(pSysInfo->net.rtsp_config)-sizeof(long)) );

	pthread_create(&thread, NULL, PollingThread, NULL);

	tConfig.nDayNight = pSysInfo->lan_config.nDayNight;
	tConfig.nWhiteBalance = pSysInfo->lan_config.nWhiteBalance;
	tConfig.nBackLight = pSysInfo->lan_config.nBackLight;
	tConfig.nBrightness = pSysInfo->lan_config.nBrightness;
	tConfig.nContrast = pSysInfo->lan_config.nContrast;
	tConfig.nSaturation = pSysInfo->lan_config.nSaturation;
	tConfig.nSharpness = pSysInfo->lan_config.nSharpness;
	tConfig.nStreamType = pSysInfo->codec_config.nStreamType;
	tConfig.nVideocodecmode = pSysInfo->codec_config.nVideocodecmode;
	tConfig.nVideocodecres = pSysInfo->codec_config.nVideocodecres;
	tConfig.nBinning = pSysInfo->lan_config.nBinning;
	tConfig.nMirror = pSysInfo->lan_config.mirror;
	tConfig.nJpegQuality = pSysInfo->codec_config.njpegquality;
	tConfig.nAEWswitch = pSysInfo->lan_config.nAEWswitch;
	tConfig.nAEWtype = pSysInfo->lan_config.nAEWtype;
	tConfig.expPriority = pSysInfo->lan_config.expPriority;
	tConfig.nRateControl1 = pSysInfo->codec_config.nRateControl1;
	tConfig.nRateControl2 = pSysInfo->codec_config.nRateControl2;
	tConfig.nFrameRate1 = pSysInfo->codec_config.nFrameRate1;
	tConfig.nFrameRate2 = pSysInfo->codec_config.nFrameRate2;
	tConfig.nFrameRate3 = pSysInfo->codec_config.nFrameRate3;
	tConfig.nMEConfig1 = pSysInfo->codec_config.codec_advconfig[0].meConfig;
	tConfig.nMEConfig2 = pSysInfo->codec_config.codec_advconfig[1].meConfig;
	tConfig.nMEConfig3 = pSysInfo->codec_config.codec_advconfig[2].meConfig;
	tConfig.histogram = pSysInfo->lan_config.histogram;
	tConfig.vsEnable = pSysInfo->lan_config.advanceMode.vsEnable;
	tConfig.ldcEnable = pSysInfo->lan_config.advanceMode.ldcEnable;
	tConfig.vnfEnable = pSysInfo->lan_config.advanceMode.vnfEnable;
	tConfig.vnfMode = pSysInfo->lan_config.advanceMode.vnfMode;
	tConfig.vnfStrength = pSysInfo->lan_config.advanceMode.vnfStrength;
	tConfig.dynRange = pSysInfo->lan_config.advanceMode.dynRange;
	tConfig.dynRangeStrength = pSysInfo->lan_config.advanceMode.dynRangeStrength;
	tConfig.framectrl = pSysInfo->lan_config.framectrl;
	tConfig.nBitrate1 = pSysInfo->codec_config.nMpeg41bitrate;
	tConfig.nBitrate2 = pSysInfo->codec_config.nMpeg42bitrate;
	tConfig.nCodectype1 = pSysInfo->codec_config.codectype1;
	tConfig.nCodectype2 = pSysInfo->codec_config.codectype2;
	tConfig.nCodectype3 = pSysInfo->codec_config.codectype3;

	tConfig.H264_PORTNUM_1 = pSysInfo->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[0];
	tConfig.H264_PORTNUM_2 = pSysInfo->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[1];
	tConfig.MPEG4_PORTNUM_1 = pSysInfo->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[2];
	tConfig.MPEG4_PORTNUM_2 = pSysInfo->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[3];
	tConfig.MJPEG_PORTNUM_1 = pSysInfo->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[4];

	StartStream(&tConfig);

	Init_StreamEnv(&tConfig);
			
	SendForceIFrameMsg(1); // Force IDR and Sync once streaming starts
	system(DROP_FILESYS_CMD);

	return 0;
}

/**
* @brief Clean up system message driver
*
*/
void SystemCleanup(void)
{
	RTSP_ControlExit();
	DestroySem(hndlSysSem);
	hndlSysSem = NULL;
	ApproInterfaceExit();
	CleanupFileMsgDrv();
}

int SetNetwork(Network_Config *net)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL){
		return -1;
	}
	
	memcpy(&(pSysInfo->net.net_config),net,sizeof(Network_Config));

	return fSetNetwork();	
}
/**
* @brief Get DHCP status
*
* @retval NULL for failed to get DHCP status
* @return DHCP status.
*/
int* GetDhcpEn()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return &pSysInfo->net.net_config.dhcp_enable;
}

/**
* @brief Get http port number
*
* @return Http prot nunber
* @retval NULL Failed to get http port
*/
unsigned short *GetHttpPort()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return &pSysInfo->net.net_config.http_port;
}

/**
* @brief Set netmask
*
* @return Function to set netmask
*/
int SetNetMask(char *mask)
{
	return fSetNetMask(mask);
}

/**
* @brief Get NTP server
*
* @return NTP server
* @retval NULL Fail.
*/
char *GetFqdn()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.net_config.ntp_server;
}


/**
* @brief Get mac address
*
* @return mac address
* @retval NULL for failed to get mac address
*/
unsigned char *GetMac()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.net_config.MAC;
}

/**
* @brief Get ip
*
* @return ip
* @retval NULL for failed to get ip
*/
char* GetIp()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.net_config.ip;
}

/**
* @brief Get gateway
*
* @return gateway
* @retval NULL for failed to get gateway
*/
char* GetGateWay()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.net_config.gateway;
}

/**
* @brief Get netmask
*
* @return netmask
* @retval NULL for failed to get netmask
*/
char* GetNetmask()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.net_config.netmask;
}

/**
* @brief Get server ip
*
* @return server ip
* @retval NULL for failed to get server ip
*/
char* GetFtpServerIp()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.ftp_config.servier_ip;
}

/**
* @brief Get ftp user name
*
* @return ftp user name
* @retval NULL for failed to get ftp user name
*/
char *GetFtpUsername()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.ftp_config.username;
}

/**
* @brief Get ftp password
*
* @return ftp password
* @retval NULL for failed to get password
*/
char *GetFtpPassword()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.ftp_config.password;
}

/**
* @brief Get ftp foldname
*
* @return ftp foldname
* @retval NULL for failed to get ftp foldname
*/
char *GetFtpFoldername()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.ftp_config.foldername;
}

/**
* @brief Get FTP port
* @retval NULL Fail.
* @return FTP port
*/
unsigned short *GetFtpPort()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return &pSysInfo->net.ftp_config.port;
}

/**
* @brief Get FTP pid
*
* @return FTP pid
* @retval NULL for failed to get ftp pid
*/
int *GetFtpPid()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return &pSysInfo->net.ftp_config.pid;
}

/**
* @brief Set IP
*
* @param ip IP value
* @return function to set IP
*/
int SetIp(char* ip)
{
	int ret;
	ret = fSetIp(ip);
	return ret;
}
/**
* @brief Get DNS
*
* @return DNS IP
* @retval NULL for failed to get DNS IP
*/
char* GetDns()
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return NULL;
	return pSysInfo->net.net_config.dns;
}
/**
* @brief Set DNS IP
*
* @param ip DNS IP
* @return function to set DNS IP
*/
int SetDns(char* ip)
{
	return fSetDns(ip);
}
/**
* @brief Set http port
*
* @param port port number
* @return function to set http port
*/
int SetHttpPort(unsigned short port)
{
	int ret;
	/* 0 ~ 1024 to be reserved */
	if(port == 80 || port > 1024){
		system("killall -9 boa\n");
		ret = fSetHttpPort(port);

	} else
		ret = -1;
	return ret;
}

/**
* @brief Set gateway
*
* @param gateway gateway value
* @return function to set getaway
*/
int SetGateway(char* gateway)
{
	return fSetGateway(gateway);
}

/**
* @brief Set dhcp status
*
* @param enable determine on or off
* @return function to set dhcp status
* @retval -1 error on set ip
*/
int SetDhcpEnable(int enable)
{
	return fSetDhcpEnable(enable);
}

/**
* @brief Set ftp server ip
* @param ip server ip
* @return function to set iftp server p
*/
int SetFtpFqdn(void * buf, int length)
{
	return fSetFtpFqdn(buf, length);
}

/**
* @brief Set ftp user name
*
* @param *buf user name
* @param length buffer length
* @return function to set ip
*/
int SetFtpUsername(void * buf, int length)
{
	return fSetFtpUsername(buf, length);
}

/**
* @brief Set ftp password
*
* @param *buf password
* @param length buffer length
* @return function to set ftp password
*/
int SetFtpPassword(void * buf, int length)
{
	return fSetFtpPassword(buf, length);
}

/**
* @brief Set ftp foldname
*
* @param *buf foldname
* @param length buffer length
* @return function to set ftp foldname
*/
int SetFtpFoldname(void * buf, int length)
{
	return fSetFtpFoldname(buf, length);
}


int SetFtpPort(unsigned short port)
{
	return fSetFtpPort(port);
}

/**
* @brief Set ftp pid
*
* @param pid ftp pid
* @return function to set ftp pid
*/
int SetFtpPid(int pid)
{
	return fSetFtpPid(pid);
}



/**
* @brief Set SNTP server
*
* @param *buf SNTP server
* @param length buffer length
* @return function to set SNTP server
*/
int SetSntpServer(void * buf, int length)
{
	return fSetSntpServer(buf, length);
}
/**
* @brief Set file format to save via FTP
*
* @param value file format.
*
* @return function to set file format
*/
int SetFTPFileFormat(unsigned char value)
{
	return fSetFTPFileFormat(value);
}

/**
* @brief enable record - upload via ftp
*
* @param value value  of enable record - upload via ftp
* @return function to set enable record - upload via ftp
*/
int SetRftpenable(unsigned char value)
{
	return fSetRftpenable(value);
}


int SetVideoCodecType(unsigned char mode, unsigned char res)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	switch(mode)
	{
		case 9:
			pSysInfo->codec_config.codectype1=MPEG4_CODEC;
			pSysInfo->codec_config.codectype2=MJPEG_CODEC;
			pSysInfo->codec_config.codectype3=MPEG4_CODEC;
			break;
		case 8:
			pSysInfo->codec_config.codectype1=H264_CODEC;
			pSysInfo->codec_config.codectype2=H264_CODEC;
			pSysInfo->codec_config.codectype3=MJPEG_CODEC;
			break;
		case 7:
			pSysInfo->codec_config.codectype1=H264_CODEC;
			pSysInfo->codec_config.codectype2=MPEG4_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
		case 6:
			pSysInfo->codec_config.codectype1=MPEG4_CODEC;
			pSysInfo->codec_config.codectype2=MPEG4_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
		case 5:
			pSysInfo->codec_config.codectype1=H264_CODEC;
			pSysInfo->codec_config.codectype2=H264_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
		case 4:
			pSysInfo->codec_config.codectype1=MPEG4_CODEC;
			pSysInfo->codec_config.codectype2=MJPEG_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
		case 3:
			pSysInfo->codec_config.codectype1=H264_CODEC;
			pSysInfo->codec_config.codectype2=MJPEG_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
		case 2:
			pSysInfo->codec_config.codectype1=MJPEG_CODEC;
			pSysInfo->codec_config.codectype2=NO_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
		case 1:
			pSysInfo->codec_config.codectype1=MPEG4_CODEC;
			pSysInfo->codec_config.codectype2=NO_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
		case 0:
		default:
			pSysInfo->codec_config.codectype1=H264_CODEC;
			pSysInfo->codec_config.codectype2=NO_CODEC;
			pSysInfo->codec_config.codectype3=NO_CODEC;
			break;
	}

	return 0;
}

//value 0:H.264 1:MPEG4 2:MJPEG
int SetVideoCodecMode(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->codec_config.nStreamType==0) //"SINGLESTREAM"
		value = value;
	if (pSysInfo->codec_config.nStreamType==1) //"DUALSTREAM"
		value = value+3;
	if (pSysInfo->codec_config.nStreamType==2) //"TRISTREAM"
		value = value+8;

	if(value != pSysInfo->codec_config.nVideocodecmode) {
		ret = fSetVideoCodecMode(value);
		return ret;
	}

	return 0;
}

/*
These three functions SetVideoMode, SetVideoCodecCombo and SetVideoCodecRes
will set nVideocodecmode and nVideocodecres varibale.
*/
int resmodecount = 0;
int resmodechangecount = 0;
#define RESMODE_MAX_VARIABLE (3)

void SetVideoCodecCfg(int flag)
{
	VideoCodecCfg vidCodecCfg;
	int resChanged;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	resmodecount = 0;

	SetVideoCodecMode(pSysInfo->codec_config.nVideocombo);

	SetVideoCodecType(pSysInfo->codec_config.nVideocodecmode, pSysInfo->codec_config.nVideocodecres);

	/* For codec mode not support JPEG, change the format to AVI */
	if((pSysInfo->codec_config.codectype1 != MJPEG_CODEC)	&&
	   (pSysInfo->codec_config.codectype2 != MJPEG_CODEC)	&&
	   (pSysInfo->codec_config.codectype3 != MJPEG_CODEC))
	{
		SetFTPFileFormat(0);
	}

	SetStreamingStack(pSysInfo->codec_config.nStreamType);

	if((flag == 1) && (resmodechangecount > 0))
	{	
		/*
		 *	1.If there is change in the stream codec types and resolution being 
		 *    same then it is handled dynamically by McFW.
		 *  2.If there is change in the stream resolution then McFW is restarted.
		 */ 
	
		resChanged = 0;
	
		if(pSysInfo->codec_config.codectype1 != H264_CODEC)
		{
			pSysInfo->codec_config.codec_advconfig[0].meConfig = ENCPRESET_AUTO;
			fSetMEConfig1(pSysInfo->codec_config.codec_advconfig[0].meConfig);
		}

		if(pSysInfo->codec_config.codectype2 != H264_CODEC)
		{
			pSysInfo->codec_config.codec_advconfig[1].meConfig = ENCPRESET_AUTO;
			fSetMEConfig2(pSysInfo->codec_config.codec_advconfig[1].meConfig);
		}

		switch(pSysInfo->codec_config.nVideocodecmode)
		{
			case CODEC_COMB_TRIPLE_MPEG4:
			{	// Dual MPEG4 + MJPEG
				if (pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode9\n");
				}

				vidCodecCfg.numStream = 3;

				vidCodecCfg.codecCfg[0].codecType = MPEG4_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				vidCodecCfg.codecCfg[1].codecType = MPEG4_CODEC;
				vidCodecCfg.codecCfg[1].encPreset = pSysInfo->codec_config.codec_advconfig[1].meConfig;

				vidCodecCfg.codecCfg[2].codecType = MJPEG_CODEC;
				vidCodecCfg.codecCfg[2].encPreset = pSysInfo->codec_config.codec_advconfig[2].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{
					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;

					vidCodecCfg.codecCfg[2].width = VS_W;
					vidCodecCfg.codecCfg[2].height = VS_H;
				}
				else
				{																					
					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;

					vidCodecCfg.codecCfg[2].width = 1920;
					vidCodecCfg.codecCfg[2].height = 1080;
				}

				break;
			}

			case CODEC_COMB_TRIPLE_H264:
			{	// Dual H.264 + MJPEG
				vidCodecCfg.numStream = 3;

				vidCodecCfg.codecCfg[0].codecType = H264_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				vidCodecCfg.codecCfg[1].codecType = H264_CODEC;
				vidCodecCfg.codecCfg[1].encPreset = pSysInfo->codec_config.codec_advconfig[1].meConfig;

				vidCodecCfg.codecCfg[2].codecType = MJPEG_CODEC;
				vidCodecCfg.codecCfg[2].encPreset = pSysInfo->codec_config.codec_advconfig[2].meConfig;

				if(pSysInfo->codec_config.nVideocodecres == 3)
				{							
					vidCodecCfg.codecCfg[0].width = 3648;
					vidCodecCfg.codecCfg[0].height = 2736;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;

					vidCodecCfg.codecCfg[2].width = 3648;
					vidCodecCfg.codecCfg[2].height = 2736;					
				}
				else if(pSysInfo->codec_config.nVideocodecres == 2)
				{
					vidCodecCfg.codecCfg[0].width = 3264;
					vidCodecCfg.codecCfg[0].height = 2448;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;

					vidCodecCfg.codecCfg[2].width = 3264;
					vidCodecCfg.codecCfg[2].height = 2448;
				}
				else if(pSysInfo->codec_config.nVideocodecres == 1)
				{
					vidCodecCfg.codecCfg[0].width = 2560;
					vidCodecCfg.codecCfg[0].height = 1920;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;

					vidCodecCfg.codecCfg[2].width = 2560;
					vidCodecCfg.codecCfg[2].height = 1920;
				}
				else {
					if(pSysInfo->codec_config.nVideocodecres != 0)
					{
						pSysInfo->codec_config.nVideocodecres = 0;
						fSetVideoCodecRes(0);
						__E("\nCODEC Resolution Error mode8\n");
					}

					if(pSysInfo->lan_config.advanceMode.vsEnable)
					{
						vidCodecCfg.codecCfg[0].width = VS_W;
						vidCodecCfg.codecCfg[0].height = VS_H;

						vidCodecCfg.codecCfg[1].width = 720;
						vidCodecCfg.codecCfg[1].height = 480;

						vidCodecCfg.codecCfg[2].width = VS_W;
						vidCodecCfg.codecCfg[2].height = VS_H;
					}
					else
					{
						vidCodecCfg.codecCfg[0].width = 1920;
						vidCodecCfg.codecCfg[0].height = 1080;

						vidCodecCfg.codecCfg[1].width = 720;
						vidCodecCfg.codecCfg[1].height = 480;

						vidCodecCfg.codecCfg[2].width = 1920;
						vidCodecCfg.codecCfg[2].height = 1080;
					}
				}

				break;
			}

			case CODEC_COMB_H264_MPEG4:
			{	// H.264 + MPEG4
				if(pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode7\n");
				}

				vidCodecCfg.numStream = 2;

				vidCodecCfg.codecCfg[0].codecType = H264_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				vidCodecCfg.codecCfg[1].codecType = MPEG4_CODEC;
				vidCodecCfg.codecCfg[1].encPreset = pSysInfo->codec_config.codec_advconfig[1].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{
					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}
				else
				{
																									
					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}

				break;
			}

			case CODEC_COMB_DUAL_MPEG4:
			{	// DUAL MPEG4
				if(pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode6\n");
				}

				vidCodecCfg.numStream = 2;

				vidCodecCfg.codecCfg[0].codecType = MPEG4_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				vidCodecCfg.codecCfg[1].codecType = MPEG4_CODEC;
				vidCodecCfg.codecCfg[1].encPreset = pSysInfo->codec_config.codec_advconfig[1].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{
					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}
				else
				{
					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}

				break;
			}

			case CODEC_COMB_DUAL_H264:
			{	// DUAL H.264
				vidCodecCfg.numStream = 2;

				vidCodecCfg.codecCfg[0].codecType = H264_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				vidCodecCfg.codecCfg[1].codecType = H264_CODEC;
				vidCodecCfg.codecCfg[1].encPreset = pSysInfo->codec_config.codec_advconfig[1].meConfig;

				if(pSysInfo->codec_config.nVideocodecres == 1)
				{
					if(pSysInfo->lan_config.advanceMode.vsEnable)
					{
						
						vidCodecCfg.codecCfg[0].width = VS_W;
						vidCodecCfg.codecCfg[0].height = VS_H;

						vidCodecCfg.codecCfg[1].width = VS_W;
						vidCodecCfg.codecCfg[1].height = VS_H;
					}
					else
					{
						vidCodecCfg.codecCfg[0].width = 1920;
						vidCodecCfg.codecCfg[0].height = 1080;

						vidCodecCfg.codecCfg[1].width = 1920;
						vidCodecCfg.codecCfg[1].height = 1080;
					}
				}
				else
				{
					if(pSysInfo->codec_config.nVideocodecres != 0)
					{
						pSysInfo->codec_config.nVideocodecres = 0;
						fSetVideoCodecRes(0);
						__E("\nCODEC Resolution Error mode5\n");
					}

					if(pSysInfo->lan_config.advanceMode.vsEnable)
					{
						vidCodecCfg.codecCfg[0].width = VS_W;
						vidCodecCfg.codecCfg[0].height = VS_H;

						vidCodecCfg.codecCfg[1].width = 720;
						vidCodecCfg.codecCfg[1].height = 480;
					}
					else
					{						
						vidCodecCfg.codecCfg[0].width = 1920;
						vidCodecCfg.codecCfg[0].height = 1080;

						vidCodecCfg.codecCfg[1].width = 720;
						vidCodecCfg.codecCfg[1].height = 480;
					}
				}

				break;
			}

			case CODEC_COMB_MPEG4_JPEG:
			{	// MPEG4 + JPEG
				if(pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode4\n");
				}

				vidCodecCfg.numStream = 2;

				vidCodecCfg.codecCfg[0].codecType = MPEG4_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				vidCodecCfg.codecCfg[1].codecType = MJPEG_CODEC;
				vidCodecCfg.codecCfg[1].encPreset = pSysInfo->codec_config.codec_advconfig[1].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{							

					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}
				else
				{									

					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}

				break;
			}

			case CODEC_COMB_H264_JPEG:
			{	// H.264 + JPEG
				if(pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode3\n");
				}

				vidCodecCfg.numStream = 2;

				vidCodecCfg.codecCfg[0].codecType = H264_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				vidCodecCfg.codecCfg[1].codecType = MJPEG_CODEC;
				vidCodecCfg.codecCfg[1].encPreset = pSysInfo->codec_config.codec_advconfig[1].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{																		

					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}
				else
				{
					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;

					vidCodecCfg.codecCfg[1].width = 720;
					vidCodecCfg.codecCfg[1].height = 480;
				}

				break;
			}

			case CODEC_COMB_SINGLE_JPEG:
			{	//Single JPEG

				if(pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode2\n");
				}

				vidCodecCfg.numStream = 1;

				vidCodecCfg.codecCfg[0].codecType = MJPEG_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{
					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;
				}
				else
				{
					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;
				}

				break;
			}

			case CODEC_COMB_SINGLE_MPEG4:
			{	// SINGLE MPEG4
				if(pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode1\n");
				}

				vidCodecCfg.numStream = 1;

				vidCodecCfg.codecCfg[0].codecType = MPEG4_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{
					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;
				}
				else
				{
					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;
				}

				break;
			}

			case CODEC_COMB_SINGLE_H264:
			default :
			{	// SINGLE H.264
				if(pSysInfo->codec_config.nVideocodecmode != CODEC_COMB_SINGLE_H264)
				{
					__E("\nCODEC Mode Error\n");
					pSysInfo->codec_config.nVideocodecmode = CODEC_COMB_SINGLE_H264;
					fSetVideoCodecMode(CODEC_COMB_SINGLE_H264);
				}

				if(pSysInfo->codec_config.nVideocodecres != 0)
				{
					pSysInfo->codec_config.nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode0\n");
				}

				vidCodecCfg.numStream = 1;

				vidCodecCfg.codecCfg[0].codecType = H264_CODEC;
				vidCodecCfg.codecCfg[0].encPreset = pSysInfo->codec_config.codec_advconfig[0].meConfig;

				if(pSysInfo->lan_config.advanceMode.vsEnable)
				{
					vidCodecCfg.codecCfg[0].width = VS_W;
					vidCodecCfg.codecCfg[0].height = VS_H;
				}
				else
				{
					vidCodecCfg.codecCfg[0].width = 1920;
					vidCodecCfg.codecCfg[0].height = 1080;
				}

				break;
			}
		}
				
		if(resChanged == 0)
			SendVideoCodecCfg(&vidCodecCfg);
		else
			vidCodecCfg.restartFlag = 1;

		if(vidCodecCfg.restartFlag == 1)
		{
			SetRestart();
		}
		else
		{
			if(vidCodecCfg.numStream >= 1)
			{
				SetMP41bitrate(pSysInfo->codec_config.nMpeg41bitrate);
				SetEncRateControl(0, pSysInfo->codec_config.nRateControl1);
				SetFramerate1(pSysInfo->codec_config.nFrameRate1);

				if(vidCodecCfg.codecCfg[0].codecType == MJPEG_CODEC)
					SetJpgQuality(pSysInfo->codec_config.njpegquality);
			}

			if(vidCodecCfg.numStream >= 2)
			{
				SetMP42bitrate(pSysInfo->codec_config.nMpeg42bitrate);
				SetEncRateControl(1, pSysInfo->codec_config.nRateControl2);
				SetFramerate2(pSysInfo->codec_config.nFrameRate2);

				if(vidCodecCfg.codecCfg[1].codecType == MJPEG_CODEC)
					SetJpgQuality(pSysInfo->codec_config.njpegquality);
			}

			if(vidCodecCfg.numStream >= 3)
			{
				SetFramerate3(pSysInfo->codec_config.nFrameRate3);
				SetJpgQuality(pSysInfo->codec_config.njpegquality);
			}
		}
	} // if((flag == 1) && (resmodechangecount > 0))	 
	
	resmodechangecount = 0;
}

int SetVideoMode(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	
	resmodecount++;

	if(value != pSysInfo->codec_config.nStreamType) {
		ret = fSetVideoMode(value);
		resmodechangecount ++;
	}	

	if(resmodecount == RESMODE_MAX_VARIABLE)
		SetVideoCodecCfg(1);

	return ret;
}

int SetVideoCodecCombo(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;	
	
	resmodecount++;

	if(value != pSysInfo->codec_config.nVideocombo) {
		ret = fSetVideoCodecCombo(value);
		resmodechangecount ++;
	}	

	if(resmodecount == RESMODE_MAX_VARIABLE)
		SetVideoCodecCfg(1);

	return ret;
}

int SetVideoCodecRes(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	resmodecount++;

	if(value != pSysInfo->codec_config.nVideocodecres) {
		ret = fSetVideoCodecRes(value);
		resmodechangecount ++;
	}

	if(resmodecount == RESMODE_MAX_VARIABLE)
		SetVideoCodecCfg(1);

	return ret;
}

/**
* @brief Get dhcp config
*
* @return dhcp config information
* @retval NULL for failed to get dhcp config information
*/
int GetDhcpCinfig()
{
	int dhcp_config = 0;

	return dhcp_config;
}

/**
* @brief Set image setting to default
*
* @param value default enable
* @return
*/
int SetImageParaDefault(unsigned char value)
{
	if (value == 1)
	{
		SetCamWhiteBalance(WHITEBALANCE_DEFAULT);
		SetBinning(BINNING_DEFAULT);
		SetBlc(BACKLIGHT_CONTROL_DEFAULT);
		SetCamBacklight(BACKLIGHT_VALUE_DEFAULT);
		SetCamBrightness(BRIGHTNESS_DEFAULT);
		SetCamContrast(CONTRAST_DEFAULT);
		SetCamSaturation(SATURATION_DEFAULT);
		SetCamSharpness(SHARPNESS_DEFAULT);
		SetCamDayNight(DAY_NIGHT_DEFAULT);
	}
	else __E ("SysCtrl: SetImageDefault called with invalid param\n");
	return 0;
}

/**
* @brief Add user
*
* @param *acout user account
* @retval 0: success
* @retval -1: failed to add user
*/
int AddUser(Acount_t *acout)
{
	int i;
	SysInfo *pSysInfo = GetSysInfo();
	Acount_t admin_acount = ADMIN_ACOUT;
	if(pSysInfo == NULL)
		return -1;
	for(i = 0;i < ACOUNT_NUM;i++){
		/* Skip exist and not matched acount */
		if(strlen(pSysInfo->acounts[i].user) &&
				strcmp(acout->user, pSysInfo->acounts[i].user))
			continue;
		if(i == 0){
			/* For admin, only password can be changed. */
			memcpy(&admin_acount.password, acout->password,
				sizeof(admin_acount.password));
			fSetAcount(0, &admin_acount);
		}else
			fSetAcount(i, acout);

		return 0;
	}
	__E("Account max reached, add user fail\n");
	return -1;
}
/**
* @brief Delete user
*
* @param *strUserName User name
* @retval 0: success
* @retval -1: failed to delete user
*/
int DelUser(char *strUserName)
{
	int i, target_i = -1;
	Acount_t none_acount = NONE_ACOUT;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL){
		return -1;
	}
	/* Never delete admin */

	for(i = 1;i < ACOUNT_NUM;i++){
		if(strlen(strUserName)!=strlen(pSysInfo->acounts[i].user))
			continue;	
		if(strcmp(strUserName, pSysInfo->acounts[i].user)){
			continue;
		}

		target_i = i;
		break;
	}
	if(target_i == -1){
		return -1;
	}
	/* copy last acount to replace with target one */
	for(i = 1;i < ACOUNT_NUM && strlen(pSysInfo->acounts[i].user);i++);
	if(target_i != i - 1)
		fSetAcount(target_i, &pSysInfo->acounts[i - 1]);
	fSetAcount(i - 1, &none_acount);
	return 0;
}
/**
* @brief Check if user login
* @param nIndex index
* @param strId user ID
* @param pIpStr Login IP
* @retval 0 SUCCESS
*/
time_t aLastLoginTime[ACOUNT_NUM] = ACOUNT_TIME_DEFAULT;
int CheckLogin(int nIndex, char* strId, char *pIpStr)
{
	LogEntry_t tLog;
	time_t rawtime;
	time ( &rawtime );
	if((rawtime - aLastLoginTime[nIndex]) < LOGIN_TIMEOUT) {
		DBG("LOG DEBUG MSG: timediff: %d %d\n", (rawtime - aLastLoginTime[nIndex]), LOGIN_TIMEOUT);
		return 0;
	}
	aLastLoginTime[nIndex] = rawtime;
	sprintf(tLog.event, "%s login on %s", strId, pIpStr);
	memcpy(&tLog.time, localtime(&rawtime), sizeof(struct tm));
	return fSetSysLog(&tLog);
}
/**
* @brief User login
* @param *pPrm login data
* @retval -1 Error
* @retval 0 Success
*/
int DoLogin(login_data_t* pPrm)
{
	int i;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	for(i = 0;i < ACOUNT_NUM;i++){
		if(strlen(pPrm->id)!=strlen(pSysInfo->acounts[i].user))
			continue;
		if(strcmp(pPrm->id, pSysInfo->acounts[i].user))
			continue;
		if(strcmp(pPrm->password, pSysInfo->acounts[i].password) == 0){
			CheckLogin(i, pPrm->id, pPrm->remote_ip);
			return 0;
		}
	}

	return -1;
}


/**
* @brief User login
* @param *pPrm login data
* @retval -1 Error
* @retval 0 Success
*/
int EzDoLogin(Acount_t* pPrm)
{
	int i;
	unsigned int LogUserLen = strlen(pPrm->user);
	unsigned int LogPasswdLen = strlen(pPrm->password);
	
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	for(i = 0;i < ACOUNT_NUM;i++){
		if(LogUserLen != strlen(pSysInfo->acounts[i].user))
			continue;	
		if(LogPasswdLen != strlen(pSysInfo->acounts[i].password))
			continue;	
			
		if(strcmp(pPrm->user, pSysInfo->acounts[i].user))
			continue;
		if(strcmp(pPrm->password, pSysInfo->acounts[i].password) == 0){
			pPrm->authority = pSysInfo->acounts[i].authority;
			return 0;
		}
	}

	return -1;
}


int SysGetSysInfo(EzSysInfo * pPrm)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
	else
	{
		/** copy net config data */
		memcpy(&(pPrm->net),&(pSysInfo->net),sizeof(Network_Config));

		/** copy camera config data */
		memcpy(&(pPrm->lan_config),&(pSysInfo->lan_config),sizeof(Cam_Config_Data));

		/** copy codec config data */
		memcpy(&(pPrm->codec_config),&(pSysInfo->codec_config),sizeof(Codec_Config_Data));

		/** copy system information */
		memcpy(&(pPrm->camInfo),&(pSysInfo->camInfo),sizeof(CamSystemInfo));

		/** copy light config data */
		memcpy(&(pPrm->light_config),&(pSysInfo->light_config),sizeof(CamLight_Config));

		/** copy system information */
		memcpy(&(pPrm->state),&(pSysInfo->state),sizeof(CamState));
	}
	
	return 0;
}

int SysSetSysInfo(EzSysInfo *pPrm)
{
	
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
	else
	{
		/** copy net config data */
//		memcpy(&(pSysInfo->net),&(pPrm->net),sizeof(Network_Config));

		/** copy camera config data */
		memcpy(&(pSysInfo->lan_config),&(pPrm->lan_config),sizeof(Cam_Config_Data));

		/** copy codec config data */
		memcpy(&(pSysInfo->codec_config),&(pPrm->codec_config),sizeof(Codec_Config_Data));

		/** copy system information */
//		memcpy(&(pSysInfo->camInfo),&(pPrm->camInfo),sizeof(CamSystemInfo));

		/** copy light config data */
		memcpy(&(pSysInfo->light_config),&(pPrm->light_config),sizeof(CamLight_Config));

		/** copy system state */
//		memcpy(&(pSysInfo->state),&(pPrm->state),sizeof(CamState));
	}

	return 0;
#if 0
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
	
	do
	{
		/*** camera info *****/
		memcpy(&(pSysInfo->camInfo),&(pPrm->camInfo),sizeof(EzCamInfo));

		/*** algparam info *****/
		memcpy(&(pSysInfo->algparam),&(pPrm->algparam),sizeof(EzsdsAlgParam));

		/*** algpasdslightram info *****/
		memcpy(&(pSysInfo->sdslight),&(pPrm->sdslight),sizeof(Ezsdslight));

		/*** camlight info *****/
		memcpy(&(pSysInfo->camlight),&(pPrm->camlight),sizeof(Ezcamlight));	

		memcpy(&(pSysInfo->H3AParams),&(pPrm->H3AParams),sizeof(EzCamH3AParam));
		/** camera parameter info *****/
	
		ret = SetWhiteBalance(pPrm->lan_config.nWhiteBalance);
		if(ret)	
			break;

		ret = SetBinning(pSysInfo->lan_config.nBinning);
		if(ret)	
			break;

		ret = SetCamDayNight(pPrm->lan_config.nDayNight);
		if(ret)	
			break;	
	
		ret = SetBrightness(pPrm->lan_config.nBrightness);
		if(ret)	
			break;
		
		ret = SetContrast(pPrm->lan_config.nContrast);
		if(ret)	
			break;	
	
		ret = SetSaturation(pPrm->lan_config.nSaturation);
		if(ret)	
			break;	
		
		ret = SetSharpness(pPrm->lan_config.nSharpness);
		if(ret)	
			break;
		
		ret = Set2APriority(pPrm->lan_config.expPriority);
		if(ret)	
			break;	
	
		ret = SetImage2AVendor(pPrm->lan_config.nAEWswitch);
		if(ret)	
			break;	
	
		ret = SetImage2AType(pPrm->lan_config.nAEWtype);
		if(ret)	
			break;	
			
		ret = SetMirror(pPrm->lan_config.mirror);
		if(ret)	
			break;

		ret = SetVstabValue(pPrm->lan_config.vsEnable);
		if(ret)	
			break;	
		
		ret = SetLdcValue(pPrm->lan_config.ldcEnable);
		if(ret)	
			break;	

		ret = SetVnfValue(pPrm->lan_config.vnfEnable);
		if(ret)	
			break;	

		ret = SetVnfMode(pPrm->lan_config.vnfMode);
		if(ret)	
			break;		
																					
	}while(0);
	
	return ret;
#endif	
}

int SysUpdateSysFile(EzSysInfo *pPrm)
{
	
	SysInfo *pSysInfo = GetSysInfo();
	if(NULL == pSysInfo)
		return -1;
	else
	{
		/** copy net config data */
//		memcpy(&(pSysInfo->net),&(pPrm->net),sizeof(Network_Config));

		/** copy camera config data */
		memcpy(&(pSysInfo->lan_config),&(pPrm->lan_config),sizeof(Cam_Config_Data));

		/** copy codec config data */
		memcpy(&(pSysInfo->codec_config),&(pPrm->codec_config),sizeof(Codec_Config_Data));

		/** copy system information */
//		memcpy(&(pSysInfo->camInfo),&(pPrm->camInfo),sizeof(CamSystemInfo));

		/** copy light config data */
		memcpy(&(pSysInfo->light_config),&(pPrm->light_config),sizeof(CamLight_Config));

		/** copy system information */
//		memcpy(&(pSysInfo->state),&(pPrm->state),sizeof(CamState));
	}


	return fUpdateDefaultCfg();
}

/**
* @brief User login
* @param *pPrm login data
* @retval -1 Error
* @retval 0 Success
*/

int GetUser(get_userdata_t* pPrm)
{
	int i;
	memset(pPrm,0,sizeof(get_userdata_t));
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	for(i = 0;i < ACOUNT_NUM;i++)
	{
		if(strcmp(pSysInfo->acounts[i].user, ""))
		{	
			strcpy(pPrm->user[pPrm->userCount].user,pSysInfo->acounts[i].user);
			pPrm->user[pPrm->userCount].authority = pSysInfo->acounts[i].authority;
			pPrm->userCount++;	
		}
	}

	return 0;
}

/**
* @brief Get user authority
* @param *user user
* @retval -1 Error
* @return User authority
*/
int GetUserAuthority(char* user)
{
	int i;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	for(i = 0;i < ACOUNT_NUM;i++){
		if(strcmp(user, pSysInfo->acounts[i].user) == 0)
			return pSysInfo->acounts[i].authority;
	}
	return -1;
}


/**
* @brief Get http port from file
* @param httpport [O] where http port will return
* @param pFile [I] pointer to a opened file
* @retval 0:SUCCESS
* @retval -1:FAIL
*/
int ReadHTTPort(unsigned short* httpport, FILE* pFile)
{
	const char sTarget[] = "HTTPPort=";
	char tmpBuff[128]="\0";
	char *offset,*ptr;
	char strPort[6];
	int i;

	if(fread(tmpBuff,1,128,pFile)==0){
			__E("File read error\n");
			return -1;
	}
	offset = strstr(tmpBuff , sTarget );
	if (offset == NULL){
		__E("File format error\n");
		return -1;
	}
	ptr = offset + strlen(sTarget);

	for(i = 0;i < sizeof(strPort);i++){//add =
		if(((*ptr != '\n') && (*ptr !='\r'))){
			strPort[i] = *ptr;
			ptr++;
      printf("strPort[%d]=%c\n", i, strPort[i]);

		}
		else {
			if(*ptr == '\n'){
				break;
			}else if(*ptr == '\r'){
				break;
			}else
				return -1;
     printf("return -1 \n");


		}
	}
	if(i == sizeof(strPort))
		return -1;
	strPort[i] = '\0';
	*httpport = atoi(strPort);
	return 0;
}

int SetNetMulticast(unsigned char value)
{
	int ret = 0, nExtFunc = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	if(value != pSysInfo->net.rtsp_config.multicast_enable) {
		ret = fSetNetMulticast(value);
		SetRTSPStreamingOFF();
		nExtFunc = GetRTSPStreamOption();
		SetRTSPStreamingON(nExtFunc);
	}

	return ret;
}

int DoBootProc(int value)
{
	int ret = 0;
	switch(value){
		case 1:
			ret = BootProc1();
			break;
		case 2:
			ret = 0;
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

int SetHttpsPort(unsigned char value)
{
	return fSetHttpsPort(value);
}


int SetSystemDefault(unsigned char value)
{
	SetDefault();
    return 0;
}

int SetSysDefaultDateTime(void)
{
	struct tm *tmnow;
	time_t tnow;
	static int timeboundReq = 1;

	if(timeboundReq == 0)
		return 0;

	time(&tnow);
	tmnow = localtime(&tnow);

	/* if Date is before Jan 1st, reset to Jan 1st */
	if((tmnow->tm_year<112)||((tmnow->tm_year==112)&&(tmnow->tm_mon<0)))
	{
		tmnow->tm_year 	= 112;
		tmnow->tm_mon 	= 0;
		tmnow->tm_mday 	= 1;
		tmnow->tm_hour 	= 0;
		tmnow->tm_min 	= 0;
		tmnow->tm_sec 	= 0;

		if ((tnow = mktime(tmnow)) < 0)
			return -1;

		stime(&tnow);
		system("hwclock -uw");

		usleep(100000);
		printf("Date & Time Reset to Jan 1, 2012, 00:00:00AM \n");
	}

	return 0;
}
