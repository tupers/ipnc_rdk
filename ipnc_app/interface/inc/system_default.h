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
#ifndef __SYSTEM_DEFAULT_H__
#define __SYSTEM_DEFAULT_H__
/* Macro */
#define IPV4(a,b,c,d)		((d<<24)|(c<<16)|(b<<8)|(a))

#define SYS_FILE			"/mnt/config/sysenv.cfg"
#define ALG_PARAM_FILE		"/mnt/config/algParam.cfg"
#define LOG_FILE			"/mnt/config/logfile.log"
#define DMVA_LOG_FILE		"/mnt/config/dmvalogfile.log"
#define SYS_LOG_FILE 		"/mnt/config/systemlog.log"
#define NET_FILE			"/mnt/config/NET.cfg"

#define ACCESS_LOG_FILE 	"/mnt/nand/accesslog.log"
#define SUPPORT_REPORT 	    "/var/www/SupportReport.zip"
#define SD_PATH				"/mnt/mmc/ipnc"
#define TEMP_FOLDER			"/tmp"

#ifdef IPNC_DM385
#define HOST_CONSOLE_NAME			"DM385_IPNC"
#define HOST_DEFAULT                "DM385-IPNC"
#define MODELNAME_DEFAULT			"DM38x_IPNC"
#else
#define HOST_CONSOLE_NAME			"DM8127_IPNC"
#define HOST_DEFAULT                "DM8127-IPNC"
#define MODELNAME_DEFAULT			"DM812x_IPNC"
#endif

#define TITLE_DEFAULT				"TI_IPNC"
#if 0
#define IP_DEFAULT					IPV4(192,168,  1,224)
#define NETMASK_DEFAULT				IPV4(255,255,255,  0)
#define GATEWAY_DEAFULT				IPV4(192,168, 1,254)
#define DNS_DEFAULT					IPV4(192,168, 1,1)
#else
#define IP_DEFAULT					"192.168.1.224"
#define NETMASK_DEFAULT				"255.255.255.0"
#define GATEWAY_DEAFULT				"192.168.1.1"
#define DNS_DEFAULT					"192.168.1.1"
#endif
#define HTTP_PORT_DEFAULT			80
#define HTTPS_PORT_DEFAULT			81
#define DHCP_ENABLE_DEFAULT			0
#define NTP_SERVER_DEFAULT			"tw.pool.ntp.org"
#define NTP_TIMEZONE_DEFAULT		20
#define DAYLIGHT_DEFAULT			0
#define DEVICE_TYPE_DEFAULT			0x1687
#define MAC_DEFAULT					{0x12,0x34,0x56,0x78,0x9a,0xbc}
#define FTP_SERVER_IP_DEFAULT		"192.168.1.1"
#define FTP_USERNAME_DEFAULT		"ftpuser"
#define FTP_PASSWORD_DEFAULT		"9999"
#define FTP_FOLDERNAME_DEFAULT		"default_folder"
#define FTP_IMAGEAMOUNT_DEFAULT 	5
#define FTP_PID_DEFAULT				0
#define SMTP_SERVER_IP_DEFAULT		"192.168.1.1"
#define SMTP_SERVER_PORT_DEFAULT	25
#define SMTP_AUTHENTICATION_DEFAULT	0
#define SMTP_USERNAME_DEFAULT		"smtpuser"
#define SMTP_PASSWORD_DEFAULT		"9999"
#define SMTP_SNEDER_EMAIL_DEFAULT	"user@domain.com"
#define SMTP_RECEIVER_EMAIL_DEFAULT	"user@domain.com"
#define SMTP_CC_DEFAULT				""
#define SMTP_SUBJECT_DEFAULT		"TI"
#define SMTP_TEXT_DEFAULT			"ti mail."
#define SMTP_ATTACHMENTS_DEFAULT	2
#define SMTP_VIEW_DEFAULT			0
#define JPEGQUALITY_DEF				80
#define MIRROR_DEF					0
#define ENCRYPT_DEF					0
#define FDETECT_DEF 				0
#define	HISTOGRAM_DEF				0
#define DYNRANGE_DEF				0
#define DYNRANGE_STRENGTH			1
#define	DUMMY_DEF					0
#define FTP_PORT_DEFAULT			21
#define AEW_SWITCH_DEFAULT			2
#define AEW_TYPE_DEFAULT			0
#define AEW_EXPRIORITY_DEFAULT		0
#define IMAGESOURCE_DEFAULT			0
#define MAX_FRAMERATE_DEFAULT		60000
#define WHITEBALANCE_DEFAULT		0	/* auto white balance */
#define DAY_NIGHT_DEFAULT			1	/* Default: Day mode */
#define BINNING_DEFAULT				0
#define BACKLIGHT_CONTROL_DEFAULT	1
#define BACKLIGHT_VALUE_DEFAULT		1
#define BRIGHTNESS_DEFAULT			128
#define CONTRAST_DEFAULT			128
#define SATURATION_DEFAULT			128
#define SHARPNESS_DEFAULT			128
#define SENSORGAIN_DEFAULT			0
#define PIPEGAIN_DEFAULT			0
#define EXPOSURE_DEFAULT			78
#define STREAMTYPE_DEFAULT			2
#define VCODECMODE_DEFAULT			8
#define VCODECCOMBO_DEFAULT			0
#define VCODECRES_DEFAULT			0
#define RATECONTROL_DEFAULT			0		/* Set to VBR */
#define FRAMERATE1_DEFAULT			1
#define FRAMERATE2_DEFAULT			0
#define FRAMERATE3_DEFAULT			0
#define MPEG41BITRATE_DEFAULT		4000000
#define MPEG42BITRATE_DEFAULT		2000000
#define FRAMECTRL_DEFAULT			0
#define DHCP_CONFIG_DEFAULT			0
#define DISCOVERY_MODE              0
#define NO_SCHEDULE					{0,1,{0,0,0},{0,0,0}}
#define SCHEDULE_DEFAULT			{NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE}
#define LOSTALARM_DEFAULT			0
#define SDAENABLE_DEFAULT			0
#define AFTPENABLE_DEFAULT			0
#define ASMTPENABLE_DEFAULT			0
#define FTPFILEFORMAT_DEFAULT		0
#define SDFILEFORMAT_DEFAULT		0
#define ATTFILEFORMAT_DEFAULT		0
#define	VS_DEFAULT					0
#define	LDC_DEFAULT					0
#define	VNF_DEFAULT					0
#define	VNF_MODE_DEFAULT			5
#define	VNF_STRENGTH_DEFAULT		0
#define ASMTPATTACH_DEFAULT			0
#define RFTPENABLE_DEFAULT			0
#define SD_INSERT_DEFAULT			0
#define SDRENABLE_DEFAULT			0
#define AUDIOENABLE_DEFAULT			0
#define MOTIONENABLE_DEFAULT		0
#define MOTIONCENABLE_DEFAULT		0
#define MOTIONLEVEL_DEFAULT			1
#define MOTIONCVALUE_DEFAULT		50
#define MOTIONBLOCK_DEFAULT			"000"

#define PTZZOOMIN_DEFAULT			"0"	//"zoomin"
#define PTZZOOMOUT_DEFAULT			"0"	//"zoomout"
#define PTZPANUP_DEFAULT			"0"	//"up"
#define PTZPANDOWN_DEFAULT			"0"	//"down"
#define PTZPANLEFT_DEFAULT			"0"	//"left"
#define PTZPANRIGHT_DEFAULT			"0"	//"right"

#define TSTAMPENABLE_DEFAULT 		0
#define AUDIOINVOLUME_DEFAULT 		2
#define MULTICAST_ENABLE_DEFAULT	0

/* authority AUTHORITY_NONE = 9 */
#define NONE_ACOUT				{"", "", 9}
#define ADMIN_ACOUT				{"admin", "9999", 0}
#define ACOUNT_DEFAULT			{ADMIN_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, \
							 	NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT}
#define ACOUNT_TIME_DEFAULT		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define CUR_LOG_DEFAULT			{"", {0, 0, 0, 0, 0, 0, 0, 0, 0}}
#define PWM1_DUTY_DEFAULT			50
#define CUR_DMVA_LOG_DEFAULT	{"", "", "",0,0,{0, 0, 0, 0, 0, 0, 0, 0, 0}}

/* Onvif */
#define COM_PROF                        "", "", 0
#define VSC_PROF                        "", 0, "", "", 0, 0, 0, 0
#define ASC_PROF                        "", 0, "", ""
#define VEC_PROF                        "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 720000
#define MULC_PROF                       0, "", "", 0, 0, 0
#define AEC_PROF                        "", "", 0, 0, 0, 0
#define VAC_PROF                        "", "", 0,
#define VAENGINE                        0, 0, "", "", 0, "", "", ""
#define VRENGINE                        0, 0, "", "", 0, "", "", ""
#define VOC_PROF			"",0,""
#define AOC_PROF          		"",0,""                                     //Audio output conf
#define PTZ_PROF                        "", "", 0, "", "", "", "", "", "", "", 0, 0, "", 0, "", 0, "", 0, 0, 0, 0, "", "", "", 0, 0, 0, 0
#define EXT_PROF                        "", 0, "", "", "", 0, "", 0, ""
#define MDC_PROF			"" , 0, "", 0, 720000
#define NON_PROF                        {COM_PROF,{VSC_PROF},{ASC_PROF},{VEC_PROF,{MULC_PROF}},{AEC_PROF, {MULC_PROF}},{VAC_PROF{VAENGINE},{VRENGINE}},{VOC_PROF},{AOC_PROF},PTZ_PROF,EXT_PROF,\
					{MDC_PROF,{MULC_PROF}}}
#define COM_PROFD                        "Default_Profile", "Default_Token", 1
#define VSC_PROFD                        "DVS_Name", 1, "DVS_Token", "DVS_SourceToken", 0, 0, 1920, 1080
#define ASC_PROFD                        "G711", 1, "G711", "G711"
#define VEC_PROFD                        "DE_Name", "DE_Token", 1, 100, 2, 1920, 1080, 60, 1, 10000, 60, 0, 60, 3, 720000
#define MULC_PROFD                       0, "239.0.0.0", "",1234, 0, 0
//#define AEC_PROFD                        "DAE_Name", "G711", 1, 0, 1, 0
#define AEC_PROFD                        "", "", 1, 1, 1, 0
#define VAC_PROFD                        "video_analytics", "video_analytics", 1
#define VAENGINED                         0, 0, "", "", 0, "", "", ""
#define VRENGINED                         0, 0, "", "", 0, "", "", ""
#define VOC_PROFD			 "video_output", 1, "video_output"
#define AOC_PROFD          		 "audio_output", 1, "audio_output"
#define PTZ_PROFD                        "", "", 0, "", "", "", "", "", "", "", 0, 0, "", 0, "", 0, "", 0, 0, 0, 0, "", "", "", 0, 0, 0, 0
#define EXT_PROFD                        "", 0, "", "", "", 0, "", 0, ""
#define MDC_PROFD			"MD_name" , 1, "MD_token", 0, 720000
#define DEFAULT_PROF                     {COM_PROFD,{VSC_PROFD},{ASC_PROFD},{VEC_PROFD,{MULC_PROFD}},{AEC_PROFD, {MULC_PROFD}},{VAC_PROFD,{VAENGINED},{VRENGINED}},{VOC_PROFD},{AOC_PROFD},\
					PTZ_PROFD,EXT_PROFD,{MDC_PROFD,{MULC_PROFD}}}
#define ONVIF_PROFILE                   {DEFAULT_PROF, NON_PROF, NON_PROF, NON_PROF}
#define ONVIF_SCOPES            {{"onvif://www.onvif.org/type/NetworkVideoTransmitter",{0},{0}},"onvif://www.onvif.org/name/DM365_IPNC",{"onvif://www.onvif.org/location/SilvanLabs",{0},{0}},"onvif://www.onvif.org/hardware/00E00ADD0000",{"onvif://www.onvif.org/123456789", {0}, {0}}}
#define RELAYS_PROFD			{0 ,{"DR_Token", 0, 0, 0}}
#define RELAY_NON_PROF			{0 ,{{0}, 0, 0, 0}}
#define RELAYS_CONFIG			{RELAYS_PROFD, RELAY_NON_PROF, RELAY_NON_PROF, RELAY_NON_PROF}
#define IMAGING_CONFD			{"DVS_SourceToken" ,0, 1, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 1, 0, 50, 50}
#define IMAGING_CONFG			{"" ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define IMAGING_CONF			{IMAGING_CONFD,IMAGING_CONFG,IMAGING_CONFG,IMAGING_CONFG}
#define RTSP_HEADER				"rtsp://"
#define RTSP_URL				"/PSIA/Streaming/channels/2?videoCodecType=H.264"
#define RTSP_DEFAULT_URL		"rtsp://192.168.1.224:8556/PSIA/Streaming/channels/2?videoCodecType=H.264"
#define RTSPSERVERPORT 			{0,{8557,8556,8553,8554,8555},5,1,0,MULTICAST_ENABLE_DEFAULT,RTSP_DEFAULT_URL}
#define FACE_DETECT_CONFIG		{FDETECT_DEF,0,0,1920,1080,1920,1080,75,0,0,75,1,0,0}					/* face_config */
#define CODECADVPARAM			{CODECADV_DEF,CODECADV_DEF,CODECADV_DEF}
#define CODECADV_DEF			{30,0,28,10,40,0,100}

#define CODECROI_DEF			{0,1920,1080,{{0,0,0,0,},{0,0,0,0,},{0,0,0,0,},},}
#define CODECROIPARAM			{CODECROI_DEF,CODECROI_DEF,CODECROI_DEF}

#define ALG_TRIGGER_DEFAULT		0
#define ALG_TRIGGER_DEF			1
#define ALG_IMG_SRC_DEF			0
#define IMG_ENCODE_DEF			1

/******CAMERA INFO****************/
#define TOTAL_MEM				2048
#define FREE_MEM				604
#define SHARE_MEM				300
#define CORE_MEM				{80,512,20,20}
#define STORAGE_MEM				512
#define STORAGE_FREE_MEM		400
#define CORE_FREQ				{1000,500,200,200}

/* End-Onvif */
#endif   /* __SYSTEM_DEFAULT_H__ */
