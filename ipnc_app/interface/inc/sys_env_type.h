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
* @file sys_env_type.h
* @brief System evironment structure and Global definition.
*/
#ifndef __SYS_ENV_TYPE_H__
#define __SYS_ENV_TYPE_H__

#include <asm/types.h>
#include <netinet/in.h>
#include <time.h>
#include <Dmva_scheduler.h>
#include <EzData_type.h>

/*
MAGIC_NUM log
0x3DA9E3DE
0x9ACFB904
0x9A131904
0x56313930
0x56323030
0x56313600
*/
#define MAGIC_NUM			0x3DA9E3DE

#define SUCCESS 0
#define FAIL	-1

#define SYS_ENV_SIZE		(sizeof(SysInfo)-sizeof(Alg_Params))
#define SYS_NET_SIZE		sizeof(Network_Config)

#define MAX_LOG_PAGE_NUM	20
#define NUM_LOG_PER_PAGE	20
#define LOG_ENTRY_SIZE		sizeof(LogEntry_t)

#define MAX_DMVA_STRING_LENGTH  32
#define MAX_DMVA_LABEL_STRING_LENGTH  16
#define MAX_DMVA_SAVELOAD_SETS     5

#define MAX_DOMAIN_NAME			40 		///< Maximum length of domain name. Ex: www.xxx.com
#define MAX_LANCAM_TITLE_LEN	11 		///< Maximum length of LANCAM title.
#define OSDTEXTLEN				24
#define MAX_FQDN_LENGTH			256 	///< Maximum length of FQDN.
#define MAX_NAME_LENGTH			256 	///< Maximum length of normal string.
#define MAX_OSD_STREAMS			3 		///< Maximum length of normal string.
#define MAX_CODEC_STREAMS		3 		///< Maximum length of normal string.
#define MAX_STREAM_NAME			75  //25///< Maximum length of normal string.
#define MAC_LENGTH				6 		///< Length of MAC address.
#define SCHDULE_NUM				8 		///< How many schedules will be stored in system.
#define MAX_FILE_NAME			128		///< Maximum length of file name.

#define ACOUNT_NUM				16 		///< How many acounts which are stored in system.
#define USER_LEN				32 		///< Maximum of acount username length.
#define MIN_USER_LEN			4 		///< Maximum of acount username length.
#define PASSWORD_LEN			16 		///< Maximum of acount password length.
#define MIN_PASSWORD_LEN		4 		///< Maximum of acount password length.
#define PASSWORD_ZERO           0               ///< minimum length of password>
#define IP_STR_LEN				20		///< IP string length
#define MAX_LENGTH              20
#define MOTION_BLK_LEN			(4) 	///< Motion block size

#define OSD_MSG_FALSE			(0)
#define OSD_MSG_TRUE			(1)
#define OSD_PARAMS_UPDATED		(4)

#define IPNC_LOW_POWER_MODE		(0)
#define IPNC_FULL_FEATURE_MODE	(1)

/*
 The following flags is defined for alarmstatus.
 Modify these sould also check where the alarmstatus is used.
 Be sure that you also check your web UI coding.
*/
#define FLG_UI_EXT 				(1 << 0)
#define FLG_UI_MOTION 			(1 << 1)
#define FLG_UI_ETH 				(1 << 2)
#define FLG_UI_AUDIO			(1 << 3)
#define FLG_UI_RECORD 			(1 << 4)
#define FLG_UI_AVI 				(1 << 5)
#define FLG_UI_JPG 				(1 << 6)

#define LOGIN_TIMEOUT		1800 	/* Set for 30 min */

#define INVALID_IP			0
#define ETH_NAME			"eth0"
#define USB_PATH			"/proc/usb_appro"
#define USB_CONNECT_PATH	"/proc/usb_connect"
#define USB_CFG_FILE		"/mnt/config/NET_CONFIG.TXT"

/* High : on */
#define LED_ON					1
#define LED_OFF					0
#define GIO_LED_1				62
#define GIO_LED_2				63
#define GIO_LED_3				64
#define GIO_TV_CONNECT			7
#define GIO_ALARMIN 			103
#define GIO_ALARMOUT 			104
#define GIO_ALARMRESET 			105
#define GIO_DCIRIS				106
#define GIO_RESET_TO_DEFAULT	85 /*Low triger*/
#define GIO_DHCP				107
#define GIO_FILL_LIGHT			90

#define H264_1_PORTNUM	(8557)
#define H264_2_PORTNUM	(8556)
#define MPEG4_1_PORTNUM	(8553)
#define MPEG4_2_PORTNUM	(8554)
#define MJPEG_PORTNUM	(8555)
#define MAX_PROF        4
#define MAX_PROF_TOKEN  20
#define MAX_CONF_TOKEN  20
#define MAX_IP_ADDR     30
#define MAX_RELAYS      3
#define MAX_IMAGING     4

#define RTSPURL_MAX_LEN	100

typedef enum{
	NO_STREAM = -1,
	SINGLE_STREAM = 0,
	DUAL_STREAM,
	TRI_STREAM
}STREAM_TYPE;

typedef enum{
	H264_CODEC = 0,
	MPEG4_CODEC,
	MJPEG_CODEC,
	NO_CODEC
}CODEC_TYPE;

enum {
	CODEC_COMB_SINGLE_H264,	// SINGLE H.264
	CODEC_COMB_SINGLE_MPEG4,	// SINGLE MPEG4
	CODEC_COMB_SINGLE_JPEG,	//MegaPixel
	CODEC_COMB_H264_JPEG,	// H.264 + JPEG
	CODEC_COMB_MPEG4_JPEG,	// MPEG4 + JPEG
	CODEC_COMB_DUAL_H264,	// DUAL H.264
	CODEC_COMB_DUAL_MPEG4,	// DUAL MPEG4
	CODEC_COMB_H264_MPEG4,	// H.264 + MPEG4
	CODEC_COMB_TRIPLE_H264,	// Dual H.264 + MJPEG
	CODEC_COMB_TRIPLE_MPEG4,	// Dual MPEG4 + MJPEG
	CODEC_COMB_NUM
};

typedef enum{
	BINNING = 0,
	SKIPPING,
	WINDOW
}SENSOR_MODE;

typedef enum{
	ENCPRESET_AUTO = 0,
	ENCPRESET_HIGH_SPEED,
	ENCPRESET_HIGH_QUALITY,
	ENCPRESET_SVC
}ENCPRESET_MODE;

/* Onvif Structures */
/* Onvif - Video source configuration structure */

typedef struct
{
	char                                   Vname[MAX_PROF_TOKEN];               //video source configuration name
	int                                    Vcount;         // count for number of users
	char                                   Vtoken[MAX_CONF_TOKEN];             //video source configuration token
	char                                   Vsourcetoken[MAX_PROF_TOKEN];
	int                                    windowx;             //x cordinate of window
	int                                    windowy;             //y cordinate of window
	int                                    windowwidth;          //width
	int                                    windowheight;        //height
}Add_VideoSource_configuration;

/* Onvif - Video multicast configuration structure */
typedef struct
{
	int                                    nIPType;               //ip address IPv4/IPv6
	char                                   IPv4Addr[MAX_IP_ADDR];               //ipv4 address
	char                                   IPv6Addr[MAX_IP_ADDR];               //ipv6 address
	int                                    port;                   //port number
	int                                    ttl;                     //TTL
	int                                    autostart;                //autostart

}Video_Multicast_configuration;

/* Onvif - Video encoder configuration structure */
typedef struct
{
	char                                   VEname[MAX_PROF_TOKEN] ;                 //video encoder name
	char                                   VEtoken[MAX_CONF_TOKEN];                 //video encoder token
	int                                    VEusercount;            //video encoder user count
	float                                  VEquality;              //video encoder quality
	int                                    Vencoder;             //video encoder for jpeg, mpeg, h.264
	int                                    Rwidth;               //resolution width
	int                                    Rheight;                //resolution height
	int                                    frameratelimit;        //frameratelimit
	int                                    encodinginterval;       //encodinginterval
	int                                    bitratelimit;            //bitratelimit
	int                                    Mpeggovlength;              //mpeg GoVLength
	int                                    Mpegprofile;                 //mpegprofile SP/ASP
	int                                    H264govlength;               //H264 govlength
	int                                    H264profile;                 //H264 profile  baseline/main/extended/high
	long                                   sessiontimeout;
	Video_Multicast_configuration          Multicast_Conf;

}Add_VideoEncoder_configuration;

/* Onvif - Video analyticengine configuration structure */
typedef struct
{
	int                                   VAMsize;                  //video analytic module size
	int                                   VAsizeitem;               //size of simple items
	char                                  VASname[MAX_LENGTH];                   //simple item name
	char                                  VASvalue[MAX_LENGTH];                 //simple item value
	int                                   VAEsize;                 //Element size
	char                                  VAEname[MAX_LENGTH];                   //element name
	char                                  Aconfigname[MAX_LENGTH];
	char                                  Aconfigtype[MAX_LENGTH];

}Video_AnalyticEngine;

/* Onvif - Video ruleengine configuration structure */
typedef struct
{

	int                                   VARsize;                 //video analytic rule size
	int                                   VARSsizeitem;               //size of simple items for ruleengine
	char                                  VARSname[MAX_LENGTH];                   //simple item name for rule engine
	char                                  VARSvalue[MAX_LENGTH];                 //simple item value for rule engine
	int                                   VAREsize;                 //Element size for rule engine
	char                                  VAREname[MAX_LENGTH];                   //element name for rule engine
	char                                  Rconfigname[MAX_LENGTH];
	char                                  Rconfigtype[MAX_LENGTH];

}Video_RuleEngine;

/* Onvif - VideoAnalytic configuration structure */
typedef struct
{
	char                                  VAname[MAX_PROF_TOKEN];                    //video analytic name
	char                                  VAtoken[MAX_CONF_TOKEN];                  //video analytic token
	int                                   VAusercount;              //video analytic user count
	Video_AnalyticEngine VAE;
	Video_RuleEngine VRE;
} Add_VideoAnalytics_Configuration;

typedef struct
{
	char name[MAX_CONF_TOKEN];
	int usecount;
	char token[MAX_CONF_TOKEN];
} Add_VideoOutput_Configuration;

/* Onvif - Audio configuration structure */
typedef struct
{
	char                                   Aname[MAX_LENGTH];                 //audio name
	int                                    Ausecount;                //audio user count
	char                                   Atoken[MAX_LENGTH];                //audio token
	char                                   Asourcetoken[MAX_LENGTH];          //audio source token
}Add_AudioSource_Configuration;

typedef struct
{
	char                                   AEname[MAX_LENGTH];                     //audio encoder name
	char                                   AEtoken[MAX_LENGTH];                   //audio encoder token
	int                                    AEusercount;               //audio encoder user count
	int                                    bitrate;                    //Bitrate
	int                                    samplerate;                //Samplerate
	int 				       AEencoding;

	Video_Multicast_configuration          Multicast_Conf;
}Add_AudioEncoder_Configuration;

typedef struct
{
    char                                   AOname[MAX_LENGTH];                 //audio name
    int                                    AOusecount;                //audio user count
    char                                   AOtoken[MAX_LENGTH];                //audio token
    int                                   AOvolume[MAX_LENGTH];          //audio source token
}Add_AudioOutput_Configuration;

typedef struct
{
	char 	                                MDname[MAX_LENGTH];
	int					MDusecount;
	char					MDtoken[MAX_LENGTH];
	int 					MDanalytics;
	long 					sessiontimeout;
	Video_Multicast_configuration           VMC;
}Metadata_configuration;
/* Onvif - Profile Structure*/
typedef struct
{
	char                                  profilename[MAX_PROF_TOKEN];       //name of profile
	char                                  profiletoken[MAX_CONF_TOKEN];      //token of profile
	int                                    fixed;             //profile fixed or not
	Add_VideoSource_configuration AVSC;

	Add_AudioSource_Configuration AASC;

	Add_VideoEncoder_configuration AESC;

	Add_AudioEncoder_Configuration AAEC;

	Add_VideoAnalytics_Configuration  AVAC;

	Add_VideoOutput_Configuration AVOC;

	Add_AudioOutput_Configuration AAOC;
	/* PTZ configuration */
	char                                   PTZname[MAX_PROF_TOKEN];                  //ptz name
	char                                   PTZtoken[MAX_CONF_TOKEN];                 //ptz token
	int                                    PTZusercount;             //ptz user count
	char                                   PTZnodetoken[MAX_CONF_TOKEN];             //ptz nade token
	char                                   DefaultAbsolutePantTiltPositionSpace[MAX_LENGTH];         //default absolute pant tilt position space
	char                                   DefaultAbsoluteZoomPositionSpace[MAX_LENGTH];             //default absolute zoom position space
	char                                   DefaultRelativePanTiltTranslationSpace[MAX_LENGTH];       //default relative pan tilt translation space
	char                                   DefaultRelativeZoomTranslationSpace[MAX_LENGTH];          //default relative zoom translation space
	char                                   DefaultContinuousPanTiltVelocitySpace[MAX_LENGTH];         //Default Continuous PanTilt Velocity Space
	char                                   DefaultContinuousZoomVelocitySpace[MAX_LENGTH];            //Default Continuous Zoom Velocity Space
	float                                  PTZspeedx;                                    //pan tilt speed
	float                                  PTZspeedy;                                    // pan tilt speed
	char                                   PTZspeedspace[MAX_LENGTH];                                // pan tilt speed
	float                                  PTZzoom;                                      //ptz zoom
	char                                   PTZzoomspce[MAX_LENGTH];                                  //ptz zoom space
	long*                                  DefaultPTZTimeout;                            //default time out for ptz
	char                                   PANURI[MAX_LENGTH];                                        //pan tilt limit uri
	float                                  PTZrangeminx;                                     //ptz min x  range
	float                                  PTZrangemaxx;                                     //ptz max x range
	float                                  PTZrangeminy;                                     //ptz min y range
	float                                  PTZrangemaxy;                                      //ptz max y range
	char                                   PTZzoomURI[MAX_IP_ADDR];                                        //zoom uri
	char                                   PTZMname[MAX_LENGTH];                                           //meta data name
	char                                   PTZMtoken[MAX_CONF_TOKEN];                                         //meta data token
	int                                    PTZMusercount;                                      //meta data user count
	int                                    PTZfilterstatus;                                    //ptz filter status
	int                                    PTZfilterposition;                                  //ptz filter position
	int                                    PTZManalytics;                                      //analytics
	/* Extension */
	char                                   AExname[MAX_PROF_TOKEN];                           //extension audio output configuration name
	int                                    AExusecount;                       //extension audio output configuration user count
	char                                   AExtoken[MAX_CONF_TOKEN];                          //extension audio output configuration token
	char                                   AExoutputtoken[MAX_CONF_TOKEN];                    //extension audio output configuration otput token
	char                                   AExsendprimacy[MAX_LENGTH];                    //extension audio output configuration primacy
	int                                    AExoutputlevel;                      //extension audio output configuration level
	char                                   AExDname[MAX_LENGTH];                            //audio decoder name
	int                                    AExDusecount;                       //audio decoder user count
	char                                   AExDtoken[MAX_LENGTH];                         //audio decoder token
	/* metadata configuration */
	Metadata_configuration                 MDC;

}Onvif_profile;

/* Onvif Scopes structures*/
typedef struct {
	char type[3][100];
	char name[50];
	char location[3][100];
	char hardware[50];
	char improper_input[3][100];
} Onvif_Scopes;


typedef struct {
	char token[MAX_CONF_TOKEN];
	int relaymode;
	long delaytime;
	int idlestate;
	//Relay_conf set_relay_conf;
} Onvif_Relay_in;

typedef struct
{
	int	relaylogicalstate;
	Onvif_Relay_in onvif_relay_in;
}Onvif_Relay;

/* End-Onvif Structures */
#if 0
/**
* @brief Infomation of network status and user settings.
*/
typedef struct
{
	struct in_addr	ip; ///< IP address in static IP mode
	struct in_addr	netmask; ///< netmask in static IP mode
	struct in_addr	gateway; ///< gateway in static IP mode
	struct in_addr	dns; ///< DNS IP in static IP mode
	__u16		http_port; ///< HTTP port in web site.
	__u16		https_port; ///< HTTP port in web site.
	int			dhcp_enable; ///< current DHCP status.
	int			dhcp_config; ///< DHCP config
	char		ntp_server[MAX_DOMAIN_NAME+1]; ///< NTP server FQDN
	__u8		ntp_timezone; ///< current time zone
	__u8		MAC[MAC_LENGTH]; ///< hardware MAC address
} Network_Config_Data;
#endif

#define IP_STR_MAX		16
/**
* @brief Infomation of network status and user settings.
*/
typedef struct
{
	char	ip[IP_STR_MAX];
	char	netmask[IP_STR_MAX];
	char	gateway[IP_STR_MAX];
	char	dns[IP_STR_MAX];
	__u16		http_port; ///< HTTP port in web site.
	__u16		https_port; ///< HTTP port in web site.
	int			dhcp_enable; ///< current DHCP status.
	int			dhcp_config; ///< DHCP config
	char		ntp_server[MAX_DOMAIN_NAME+1]; ///< NTP server FQDN
	__u8		ntp_timezone; ///< current time zone
	__u8		MAC[MAC_LENGTH]; ///< hardware MAC address
} Network_Config_Data;
/**
* @brief Infomation about ftp configuration.
*/
typedef struct
{
	char		servier_ip[MAX_DOMAIN_NAME+1]; ///< FTP server address
	char		username[USER_LEN+1]; ///< FTP login username
	char		password[PASSWORD_LEN+1]; ///< FTP login password
	char		foldername[MAX_FILE_NAME]; ///< FTP upload folder
	int			pid; ///< PID
	__u16		port; ///< FTP port
 	__u8		rftpenable; ///< RFTP enable
	__u8        ftpfileformat; ///< file format
} Ftp_Config_Data;

typedef struct
{
        long mtype;
        unsigned short rtspServerPort_H264_MPEG4_MJPEG[5];
        int size_Port;
        int RTSP_Enable;
        int change;
        __u8		multicast_enable; ///< multicast enable
        char rtspUrl[RTSPURL_MAX_LEN+1];
} rtspServerPort;

typedef struct
{
	Network_Config_Data	net_config;
	Ftp_Config_Data		ftp_config;
	rtspServerPort		rtsp_config;
}Network_Config;

/**
 * Imaging structure
 */
typedef struct
{
	char                    Isourcetoken[MAX_LENGTH];
	int			BacklightCompensationMode;
	float			BackLightLevel;
	float			Brightness;
	float			Saturation;
	float			Contrast;
	float			ExposureMode;
	float			ExposurePriority;
	float			bottom;
	float			top;
	float			right;
	float			left;
	float			MinExposureTime;
	float			MaxExposureTime;
	float			MinGain;
	float			MaxGain;
	float			MinIris;
	float			MaxIris;
	float			ExposureTime;
	float			Gain;
	float			Iris;
	int			AutoFocusMode ;
	float			DefaultSpeed;
	float 			NearLimit;
	float 			FarLimit;
	int 			IrCutFilterMode;
	float			Sharpness;
	int			WideDynamicMode;
	float			WideDMLevel;
	int			WhiteBalanceMode;
	float 			CrGain;
	float 			CbGain;

} Imaging_Conf;

typedef struct
{
	char			Isourcetoken_t[MAX_LENGTH];
	int			BacklightCompensationMode_t;
	float			BackLightLevel_t;
	float			Brightness_t;
	float			Saturation_t;
	float			Contrast_t;
	float			ExposureMode_t;
	float			ExposurePriority_t;
	float			bottom_t;
	float			top_t;
	float			right_t;
	float			left_t;
	float			MinExposureTime_t;
	float			MaxExposureTime_t;
	float			MinGain_t;
	float			MaxGain_t;
	float			MinIris_t;
	float			MaxIris_t;
	float			ExposureTime_t;
	float			Gain_t;
	float			Iris_t;
	int			AutoFocusMode_t ;
	float			DefaultSpeed_t;
	float 			NearLimit_t;
	float 			FarLimit_t;
	int 			IrCutFilterMode_t;
	float			Sharpness_t;
	int			WideDynamicMode_t;
	float			WideDMLevel_t;
	int			WhiteBalanceMode_t;
	float 			CrGain_t;
	float 			CbGain_t;

} Imaging_Conf_in;

typedef struct
{
	int      position;
	Imaging_Conf_in imaging_conf_in;
}Imaging_Conf_tmp;
/**
* @brief Infomation about SMTP configuration.
*/
typedef struct
{
	char		servier_ip[MAX_DOMAIN_NAME+1]; ///< SMTP server address
	__u16		server_port; ///< SMTP server port
	char		username[USER_LEN+1]; ///< SMTP username
	char		password[PASSWORD_LEN+1]; ///< SMTP password
	__u8		authentication; ///< SMTP authentication
	char		sender_email[MAX_NAME_LENGTH]; ///< sender E-mail address
	char		receiver_email[MAX_NAME_LENGTH]; ///< receiver E-mail address
	char		CC[MAX_NAME_LENGTH]; ///< CC E-mail address
	char		subject[MAX_NAME_LENGTH]; ///< mail subject
	char		text[MAX_NAME_LENGTH]; ///< mail text
	__u8		attachments; ///< mail attachment
	__u8		view; ///< smtp view
	__u8		asmtpattach; ///< attatched file numbers
	__u8        attfileformat; ///< attachment file format
} Smtp_Config_Data;

/**
* @brief custom data structure for time.
*/
typedef struct{
	__u8	nHour;	///< Hour from 0 to 23.
	__u8	nMin;	///< Minute from 0 to 59.
	__u8	nSec;	///< Second from 0 to 59.
} Time_t;

/**
* @brief custom data structure for schedule entry.
*/
typedef struct{
	__u8	bStatus;	///< schedule status ( 0:disable 1:enable }
	__u8	nDay;		///< schedule day of week (1:Mon 2:Tue 3:Wed 4:Thr 5:Fri 6:Sat 7:Sun 8:Everyday)
	Time_t	tStart;		///< schedule start time
	Time_t	tDuration;	///< schedule duration
} Schedule_t;

/**
* @brief IPCAM configuration data.
*/
typedef struct
{
	__u8			nWhiteBalance;					///< white balance mode
	__u8			nDayNight;						///< Daynight mode
	__u8			nBinning;						///< binning/skipping setting
	__u8			nBacklightControl;				///< backlight control setting
	__u8			nBackLight;						///< backlight value
	__u8			nBrightness;					///< brightness value
	__u8			nContrast;						///< contrast value
	__u8			nSaturation;					///< saturation value
	__u8			nSharpness;						///< sharpness value
	__u8 			nStreamType;
	__u8			nVideocodecmode;				///< selected codec mode
	__u8			nVideocombo;
	__u8			nVideocodecres;					///< selected codec resolution
	__u8			nFrameRate1;					///< MPEG4_1 frame rate
	__u8			nFrameRate2;					///< MPEG4_2 frame rate
	__u8			nFrameRate3;					///< JPEG frame rate
	__u8			njpegquality; 					///< jpeg quality
	int				nMpeg41bitrate;					///< MPEG4_1 bitrate
	int				nMpeg42bitrate;					///< MPEG4_2 bitrate
	__u8			nAEWswitch;						///< 2A engine selection
	__u8			mirror; 						///< mirroring
	__u8			nAEWtype;						///< 2A engine selection
	__u8			histogram;						///< Histogram switch ON/OFF
	__u8			framectrl;
	__u8			nRateControl1;					///< stream1 bitrate control setting
	__u8			nRateControl2;					///< stream2 bitrate control setting
	__u8			expPriority;
	__u8			codectype1;
	__u8			codectype2;
	__u8			codectype3;
	int				frameRateVal1;
	int				frameRateVal2;
	int				frameRateVal3;
} Lancam_Config_Data;

typedef struct _EzCamExposure
{
	unsigned int exposure;
	unsigned int step;
	unsigned int maxexposure;
	unsigned int minexposure;
	unsigned int sensorGain;
	unsigned int MaxsensorGain;
	unsigned int MinsensorGain;
	unsigned int sensorGainStep;
	unsigned int pipeGain;
	unsigned int MaxpipeGain;
	unsigned int MinpipeGain;
	unsigned int pipeGainStep;
}EzCamExposure;


typedef struct
{
	__u8			vsEnable;
	__u8			ldcEnable;
	__u8			vnfEnable;
	__u8			vnfMode;
	__u8			vnfStrength;
	__u8			dynRange;						///< Dynamic Range switch ON/OFF
	__u8			dynRangeStrength;				///< Dynamic Range Strength
} Advanced_Mode;

typedef struct
{
  char       hostname_t[MAX_FILE_NAME];
}hostname_tmp;

/**
* @brief SD card configuration data.
*/
typedef struct
{
   __u8			sdfileformat;	///< file format saved into SD card
   __u8			sdrenable;		///< enable SD card recording
   __u8			sdinsert;		///< SD card inserted
}Sdcard_Config_Data;

/**
* @brief IPCAM user account data.
*/
typedef struct
{
	char	user[USER_LEN+1];			///< username
	char	password[PASSWORD_LEN+1];	///< password
	__u8	authority;				///< user authority
}Acount_t;

/**
* @brief motion detection configuration data.
*/
typedef struct
{
  __u8      motionenable;					///< motion detection enable
  __u8      motioncenable;					///< customized sensitivity enable
  __u8      motionlevel;					///< predefined sensitivity level
  __u8      motioncvalue;					///< customized sensitivity value
  __u8	 	motionblock[MOTION_BLK_LEN];	///< motion detection block data
}Motion_Config_Data;

typedef struct
{
  __u8      audioON;
  __u8      audioenable;
  __u8      audiomode;				///< motion detection enable
  __u8      audioinvolume;				///< customized sensitivity enable
  __u8		codectype;
  __u8      samplerate;				///< predefined sensitivity level
  __u8      bitrate;				///< customized sensitivity value
  __u32     bitrateValue;			///< customized sensitivity value
  __u8 		alarmlevel;
  __u8 		alarmON;
  __u8	 	audiooutvolume;	        ///< motion detection block data
  __u8      audiorecvenable;    	///< audio receiver enable
  __u8      audioServerIp[MAX_NAME_LENGTH];        ///< audio server ip address
}Audio_Config_Data;

typedef struct
{
	__u8			dstampenable;					///< stream1 date stamp enable (ON/OFF)
	__u8			tstampenable;   				///< stream1 time stamp enable (ON/OFF)
	__u8			nLogoEnable;					///< stream1 logo enable (ON/OFF)
	__u8			nLogoPosition;   				///< stream1 logo position
	__u8			nTextEnable;					///< stream1 text enable (ON/OFF)
	__u8			nTextPosition;   				///< stream1 text position
	__u8			overlaytext[OSDTEXTLEN];		///< stream1 text
    __u8            nDetailInfo;
}OSD_config;

typedef struct
{
	__u32 ipRatio;
	__u8 fIframe;
	__u8 qpInit;
	__u8 qpMin;
	__u8 qpMax;
	__u8 meConfig;
	__u8 packetSize;
	__u8 unkown1;
	__u8 unkown2;
}CodecAdvParam;

typedef struct _ROI_Param{
	__u32 startx;
	__u32 starty;
	__u32 width;
	__u32 height;
	__u32 type;
}ROI_Param;

typedef struct _CodecROIParam{
	__u32 		numROI;
  	__u32     	prevwidth;
  	__u32     	prevheight;
	ROI_Param	roi[3];
}CodecROIParam;

/**
* @brief motion detection configuration data.
*/
typedef struct
{
  __u8	    fdetect; 					    ///< facedetect (ON/OFF)
  __u32     startX;
  __u32     startY;
  __u32     width;
  __u32     height;
  __u32     prevwidth;
  __u32     prevheight;
  __u8      fdconflevel;
  __u8      fddirection;
  __u8      frecog;
  __u8	    frconflevel;
  __u8	    frdatabase;
  __u8      pmask;
  __u8      maskoption;
}Face_Config_Data;

typedef struct
{
  __u16     width;
  __u16     height;
  __u16		portnum;
  __u8		name[MAX_STREAM_NAME];
  __u8 		portname[MAX_STREAM_NAME];
}StreamParam;

/**
* @brief structure for PTZ control data.
*/
typedef struct
{
  char      ptzdata[300];
}Ptz_Data;


/**
* @brief PTZ configuration data.
*/
typedef struct
{
  char      ptzzoomin[6];	///< zoom-in
  char      ptzzoomout[7];	///< zoom-out
  char      ptzpanup[2];	///< pan-up
  char      ptzpandown[4];	///< pan-down
  char	ptzpanleft[4];		///< pan-left
  char	ptzpanright[5];		///< pan-right
}Ptz_Config_Data;

/**
* @brief event log data structure.
*/
typedef struct LogEntry_t{
	char event[50];		///< event description
	struct tm time;		///< event log time
}LogEntry_t;

/**
* @brief event log data structure.
*/
typedef struct DmvaLogEntry_t{
  __u8  eventListEventDetails_sys[MAX_DMVA_STRING_LENGTH];///< event description
  __u8  eventListTimeStamp_sys[MAX_DMVA_STRING_LENGTH];///< event log time string
  __u8  eventListFileName[MAX_DMVA_STRING_LENGTH];///< event file name string    AYK - 0310
  __u32 eventListArchiveFlag_sys;
  __u32 eventListEventType_sys;
	struct tm time;		///< event log time
}DmvaLogEntry_t;
/**
* @brief user settings structure.
*/
typedef struct{
	char id[USER_LEN+1];
	char password[PASSWORD_LEN+1];
	char remote_ip[IP_STR_LEN];
}login_data_t;

typedef struct{
	char user_id[USER_LEN+1];
	int	authority;
}get_user_authority_t;

typedef struct{
	char	user_id[USER_LEN+1];
	char	password[PASSWORD_LEN+1];
	unsigned char	authority;
}add_user_t;

typedef struct
{
	int userCount;
	Acount_t user[ACOUNT_NUM];
}get_userdata_t;

/* Onvif Structures */
typedef struct {
	char profile_name[20];
	char profile_token[20];
}media_profile;

#define EZCAM_LED_NUM	2
#if 1
typedef struct
{
	unsigned int enable;
	unsigned int pwmduty;
}FillLightConfig;
typedef struct 
{
	FillLightConfig led[EZCAM_LED_NUM];
}CamLight_Config;
#else

typedef struct 
{
	unsigned char led[EZCAM_LED_NUM];
}CamLight_Config;
#endif
typedef enum 
{
	ALG_SDS = 0,
	ALG_GVSS,
	ALG_MAX
}ALG_TYPE;

typedef struct
{
	unsigned int algType;
	unsigned int algTriggle;
	unsigned int algImgsrc;
	unsigned int enableEncode;
}CamState;

typedef struct
{

	char                                   Vname_t[MAX_PROF_TOKEN];
	int                                    Vcount_t;         // count for number of users
	char                                   Vtoken_t[MAX_CONF_TOKEN];             //video source configuration token
	char                                   Vsourcetoken_t[MAX_PROF_TOKEN];        //video source profile token
	int                                    windowx_t;             //x cordinate of window
	int                                    windowy_t;             //y cordinate of window
	int                                    windowwidth_t;          //width
	int                                    windowheight_t;        //height
}video_source_conf_in;

typedef struct
{
	int                                    position;            //position of profile
	video_source_conf_in  video_source_conf_in_t;
	__u8			nWhiteBalance;					///< white balance mode
	__u8			nDayNight;						///< Daynight mode
	__u8			nBinning;						///< binning/skipping setting
	__u8			nBacklightControl;				///< backlight control setting
	__u8			nBackLight;						///< backlight value
	__u8			nBrightness;					///< brightness value
	__u8			nContrast;						///< contrast value
	__u8			nSaturation;					///< saturation value
	__u8			nSharpness;						///< sharpness value
	__u8 			nStreamType;
	__u8			nVideocodecmode;				///< selected codec mode
	__u8			nVideocombo;
	__u8			nVideocodecres;					///< selected codec resolution
	__u8			nFrameRate1;					///< MPEG4_1 frame rate
	__u8			nFrameRate2;					///< MPEG4_2 frame rate
	__u8			nFrameRate3;					///< JPEG frame rate
	__u8			njpegquality; 					///< jpeg quality
	int				nMpeg41bitrate;					///< MPEG4_1 bitrate
	int				nMpeg42bitrate;					///< MPEG4_2 bitrate
	__u8			nAEWswitch;						///< 2A engine selection
	__u8			mirror; 						///< mirroring
	__u8			nAEWtype;						///< 2A engine selection
	__u8			histogram;						///< Histogram switch ON/OFF
	__u8			framectrl;
	__u8			nRateControl1;					///< stream1 bitrate control setting
	__u8			nRateControl2;					///< stream2 bitrate control setting
	__u8			expPriority;
	__u8			codectype1;
	__u8			codectype2;
	__u8			codectype3;
	int				frameRateVal1;
	int				frameRateVal2;
	int				frameRateVal3;
}video_source_conf;

typedef struct
{
	int                                    nIPType_t;               //ip address IPv4/IPv6
	char                                   IPv4Addr_t[MAX_IP_ADDR];               //ipv4 address
	char                                   IPv6Addr_t[MAX_IP_ADDR];               //ipv6 address
	int                                    port_t;                   //port number
	int                                    ttl_t;                     //TTL
	int                                    autostart_t;                //autostart

}Video_Multicast_configuration_in;

typedef struct
{
	char                                   VEname_t[MAX_PROF_TOKEN] ;                 //video encoder name
	char                                   VEtoken_t[MAX_CONF_TOKEN];                 //video encoder token
	int                                    VEusercount_t;            //video encoder user count
	float                                  VEquality_t;              //video encoder quality
	int                                    Vencoder_t;             //video encoder for jpeg, mpeg, h.264
	int                                    Rwidth_t;               //resolution width
	int                                    Rheight_t;                //resolution height
	int                                    frameratelimit_t;        //frameratelimit
	int                                    encodinginterval_t;       //encodinginterval
	int                                    bitratelimit_t;            //bitratelimit
	int                                    Mpeggovlength_t;              //mpeg GoVLength
	int                                    Mpegprofile_t;                 //mpegprofile SP/ASP
	int                                    H264govlength_t;               //H264 govlength
	int                                    H264profile_t;                 //H264 profile  baseline/main/extended/high
	long                                   sessiontimeout_t;
	Video_Multicast_configuration_in       Multicast_Conf_t;

}video_encoder_conf_in;

typedef struct
{
	int                                    position;
	video_encoder_conf_in                  video_encoder_conf_in_t;
}video_encoder_conf;

typedef struct
{
	char                                  VAname_t[MAX_PROF_TOKEN];                    //video analytic name
	char                                  VAtoken_t[MAX_CONF_TOKEN];                  //video analytic token
	int                                   VAusercount_t;              //video analytic user count
	int                                   VARsize_t;                 //video analytic rule size
	int                                   VARSsizeitem_t;               //size of simple items for ruleengine
	char                                  VARSname_t[MAX_LENGTH];                   //simple item name for rule engine
	char                                  VARSvalue_t[MAX_LENGTH];                 //simple item value for rule engine
	int                                   VAREsize_t;                 //Element size for rule engine
	char                                  VAREname_t[MAX_LENGTH];                   //element name for rule engine
	char                                  Rconfigname_t[MAX_LENGTH];
	char                                  Rconfigtype_t[MAX_LENGTH];
	int                                   VAMsize_t;                  //video analytic module size
	int                                   VAsizeitem_t;               //size of simple items
	char                                  VASname_t[MAX_LENGTH];                   //simple item name
	char                                  VASvalue_t[MAX_LENGTH];                 //simple item value
	int                                   VAEsize_t;                 //Element size
	char                                  VAEname_t[MAX_LENGTH];                   //element name
	char                                  Aconfigname_t[MAX_LENGTH];
	char                                  Aconfigtype_t[MAX_LENGTH];
}video_analytics_conf_in;

typedef struct
{
	int                                   position;
	video_analytics_conf_in               video_analytics_conf_t;
}video_analytics_conf;

typedef struct
{
	char name[MAX_CONF_TOKEN];
	int usecount;
	char token[MAX_CONF_TOKEN];
}video_output_conf_in;

typedef struct
{
	int position;
	video_output_conf_in 		      video_output_conf_t;
}video_output_conf;

typedef struct {
	char type_t[3][100];
	char name_t[50];
	char location_t[3][100];
	char hardware_t[50];
	char improper_input_t[3][100];
} Onvif_Scopes_t;

typedef struct{
	char token_t[MAX_LENGTH];
	int relaymode_t;
	long delaytime_t;
	int idlestate_t;
} relay_conf_t;

typedef struct
{
	int position;
	int logicalstate_t;
	relay_conf_t relay_conf_in_t;
}relay_conf;
typedef struct
{
	char                                   Aname_t[MAX_LENGTH];                 //audio name
	int                                    Ausecount_t;                //audio user count
	char                                   Atoken_t[MAX_LENGTH];                //audio token
	char                                   Asourcetoken_t[MAX_LENGTH];          //audio source token
}Add_AudioSource_Configuration_t;

typedef struct
{
	int position;
	Add_AudioSource_Configuration_t Add_AudioSource_in;
}Add_AudioSource_Configuration_tmp;

typedef struct
{
	char                                   AEname_t[MAX_LENGTH];                     //audio encoder name
	char                                   AEtoken_t[MAX_LENGTH];                   //audio encoder token
	int                                    AEusercount_t;               //audio encoder user count
	int                                    bitrate_t;                    //Bitrate
	int                                    samplerate_t;                //Samplerate
	int 				       AEencoding_t;

}Add_AudioEncoder_Configuration_t;

typedef struct
{

	int position;
	Video_Multicast_configuration_in multicast_in;
	Add_AudioEncoder_Configuration_t Add_AudioEncoder_in;
}Add_AudioEncoder_Configuration_tmp;

typedef struct
{
	char                                    MDname_t[MAX_LENGTH];
	int                                     MDusecount_t;
	char                                    MDtoken_t[MAX_LENGTH];
	int                                     MDanalytics_t;
	long                                    sessiontimeout_t;
	Video_Multicast_configuration_in       VMC_t;
}Metadata_configuration_in;

typedef struct
{
	int 					position;
	Metadata_configuration_in		MDC_in;
}Metadata_configuration_tmp;

typedef struct
{

	unsigned int 	sensorGain;
	unsigned int 	pipeGain;	
	unsigned int 	exposure;
	unsigned char	nWhiteBalance;					///< white balance mode
	unsigned char	nDayNight;						///< Daynight mode
	unsigned char	nBinning;						///< binning/skipping setting
	unsigned char	nBacklightControl;				///< backlight control setting
	unsigned char	nBackLight;						///< backlight value
	unsigned char	nBrightness;					///< brightness value
	unsigned char	nContrast;						///< contrast value
	unsigned char	nSaturation;					///< saturation value
	unsigned char	nSharpness;						///< sharpness value
	unsigned char	mirror; 						///< mirroring	
	unsigned char	nAEWswitch;						///< 2A engine selection
	unsigned char	nAEWtype;						///< 2A engine selection
	unsigned char	expPriority;
	unsigned char	histogram;						///< Histogram switch ON/OFF
	unsigned char	framectrl;
	int				frameRateVal1;
	int				frameRateVal2;
	int				frameRateVal3;	
	EzCamH3AParam   H3AParams;
	EzCamH3AWeight  H3aWeight;
	Advanced_Mode	advanceMode;
}Cam_Config_Data;

typedef struct
{
	__u8 			nStreamType;
	__u8			nVideocodecmode;				///< selected codec mode
	__u8			nVideocombo;
	__u8			nVideocodecres;					///< selected codec resolution
	__u8			nFrameRate1;					///< MPEG4_1 frame rate
	__u8			nFrameRate2;					///< MPEG4_2 frame rate
	__u8			nFrameRate3;					///< JPEG frame rate
	__u8			njpegquality; 					///< jpeg quality
	int				nMpeg41bitrate;					///< MPEG4_1 bitrate
	int				nMpeg42bitrate;					///< MPEG4_2 bitrate
	__u8			nRateControl1;					///< stream1 bitrate control setting
	__u8			nRateControl2;					///< stream2 bitrate control setting
	__u8			codectype1;
	__u8			codectype2;
	__u8			codectype3;
	CodecAdvParam codec_advconfig[MAX_CODEC_STREAMS];
}Codec_Config_Data;
#define MAX_CORE	4
typedef struct 
{
	__u8 version;
	unsigned int total_mem;						//MB
	unsigned int free_mem;						//MB
	unsigned int share_mem;						//MB
	unsigned int core_mem[MAX_CORE];			//MB
	unsigned int storage_mem;					//MB	
	unsigned int storage_free_mem;				//MB
	unsigned int core_freq[MAX_CORE];			//MHZ
}CamSystemInfo;


typedef struct
{
	unsigned int prf_a8;
	unsigned int prf_dsp;
}EzCpuDynamicParam;


typedef struct __EzFrameInfo
{
	unsigned int udpPort;
	unsigned char udpIp[20];
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
}EzFrameInfo;


typedef struct _EzSysInfo{
	Network_Config	net;
	Cam_Config_Data lan_config;
	Codec_Config_Data codec_config;
	CamSystemInfo	camInfo;
	CamLight_Config	light_config;
	CamState	 state;
}EzSysInfo;
/**
* @brief system info main data structure.
*/
typedef struct SysInfo{
	unsigned long relUniqueId;			///< IPCAM device type
	Acount_t	acounts[ACOUNT_NUM];	///< user account data	
	Network_Config	net;
	Cam_Config_Data lan_config;
	Codec_Config_Data codec_config;
	CamSystemInfo	camInfo;
	CamLight_Config	light_config;
	CamState	 state;
	LogEntry_t	tCurLog;				///< event log
	Alg_Params	  algPrm;
}SysInfo;
#endif /* __SYS_ENV_TYPE_H__ */
