/** ==================================================================
 *  @file   swosd_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/sw_osd/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _DM81XX_SWOSD_H_
#define _DM81XX_SWOSD_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* include files */
#include <xdc/std.h>
#include <ti/psp/iss/alg/swosd/inc/alg_ti_swosd.h>
#include "alg_ti_imx_swosd.h"
#include <mcfw/interfaces/link_api/systemLink_common.h>

    /* macros */
	
	
/* Defining this macro tries to reproduce the issues reported by Huawei  */
//#define TO_REPRODUCE_HUAWEI_ISSUES (1)

#ifdef TO_REPRODUCE_HUAWEI_ISSUES
#define MAX_GRAPHIC_WINDOW_WIDTH  (672)
#define MAX_GRAPHIC_WINDOW_HEIGHT  (142)
#endif /* #ifdef TO_REPRODUCE_HUAWEI_ISSUES */	

#define SWOSD_USER_TRANSPARANCY_ON                         // <<< SW OSD
    // transperancy is
    // turned ON

#define MAX_STR_BUFF                    12
#define AVSERVER_MAX_STREAMS            4                  // <<< TBD - move
    // to appropriate
    // place

#define SWOSD_align(value,align)   ((((value) + ((align) - 1 )) / (align)) * (align))

#define SWOSD_floor(value, align)   (((value) / (align)) * (align))
#define SWOSD_ceil(value, align)    SWOSD_align(value, align)

#define SWOSD_max(a, b)    ((a) > (b) ? (a) : (b))
#define SWOSD_min(a, b)    ((a) < (b) ? (a) : (b))

    /* SW OSD EDMA CH ID and QUEUE ID */
#define SWOSD_EDMA_QUEUEID				0

    /* SW OSD window IDs */
#define SWOSD_WINDOW_ONE                0                  // <<< Logo Window
#define SWOSD_WINDOW_TWO                1                  // <<< User String
    // "EVALUATION"
    // Window
#define SWOSD_WINDOW_THREE              2                  // <<< Resolution
    // Window
#define SWOSD_WINDOW_FOUR               3                  // <<< Codec Type
    // Window
#define SWOSD_WINDOW_FIVE               4                  // <<< Frame Rate
    // Window
#define SWOSD_WINDOW_SIX                5                  // <<< Bit Rate
    // window
#define SWOSD_WINDOW_SEVEN              6                  // <<< Bitrate
    // mode window -
    // OFF/CBR/VBR
#define SWOSD_WINDOW_EIGHT              7                  // <<< Date Info
    // Window
#define SWOSD_WINDOW_NINE               8                  // <<< Time Info
    // Window
#define SWOSD_WINDOW_TEN                9                  // <<< Auto Focus
    // Value
#define SWOSD_WINDOW_ELEVEN             10                 // <<< Eval
    // Version Pop-up
#define SWOSD_WINDOW_TWELVE             11                 // <<< For VA string	
#define SWOSD_WINDOW_THIRTEEN             12                 // <<< For Gain/Color Temperature

#define SWOSD_TIICON_160      "/mnt/mmc/TI_Logo_160_64.yyuv420"
#define SWOSD_TIICON_80       "/mnt/mmc/TI_Logo_80_32.yyuv420"

#define SWOSD_STRINGPATTERN   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:; '/\\+-_()&$#.0123456789"

#define OSD_YES                         1
#define OSD_NO                          0
#define STRING_OSD                      1
#define IMAGE_OSD                       2

#define SUCCESS                         0

#define SWOSD_MAX_STRING_LENGTH         24

#define SWOSD_FRAMERATE_STR_LEN         17
#define SWOSD_BITRATE_STR_LEN           18
#define SWOSD_QUALITY_STR_LEN           12
#define SWOSD_AUTOFOCUS_STR_LEN         19
#define SWOSD_DATE_STR_LEN              15
#define SWOSD_TIME_STR_LEN              15
#define SWOSD_MAX_DATE_TIME_LENGHT      15
#define SWOSD_EVAL_STR_LEN              12
#define SWOSD_AEWB_STR_LEN              18

#define SWOSD_FRAMERATE_PREFIX          12
#define SWOSD_BITRATE_PREFIX            9
#define SWOSD_BITRATE_SUFFIX            4
#define SWOSD_QUALITY_PREFIX            9
#define SWOSD_QUALITY_SUFFIX            0
#define SWOSD_AUTOFOCUS_PREFIX          12
#define SWOSD_DATE_PREFIX               5
#define SWOSD_TIME_PREFIX               5
#define SWOSD_AEWB_PREFIX               8

#define VIDEO_RESOL_WIDTH_LARGE         1000
#define VIDEO_RESOL_WIDTH_MEDIUM        750
#define VIDEO_RESOL_WIDTH_SMALL         400

#define SWOSD_CHARWIDTHTILOGO_160_64    160
#define SWOSD_CHARHEIGHTTILOGO_160_64   64
#define SWOSD_CHARWIDTHTILOGO_80_32     80
#define SWOSD_CHARHEIGHTTILOGO_80_32    32

#define SWOSD_OSDWHITETRANSVAL          0x80FF
#define SWOSD_OSDWHITETRANSRANGE        0x48
#define SWOSD_OSDBLACKTRANSVAL          0x00
#define SWOSD_OSDBLACKTRANSRANGE        0x48

#define SWOSD_CHARWIDTH_1540_40         20
#define SWOSD_CHARHEIGHT_1540_40        38
#define SWOSD_STRINGWIDTH_1540_40       1540
#define SWOSD_STRINGHEIGHT_1540_40      38

#define SWOSD_CHARWIDTH_1232_30         16
#define SWOSD_CHARHEIGHT_1232_30        28
#define SWOSD_STRINGWIDTH_1232_30       1232
#define SWOSD_STRINGHEIGHT_1232_30      28

#define SWOSD_CHARWIDTH_1078_28         14
#define SWOSD_CHARHEIGHT_1078_28        26
#define SWOSD_STRINGWIDTH_1078_28       1078
#define SWOSD_STRINGHEIGHT_1078_28      26

#define SWOSD_CHARWIDTH_770_20          10
#define SWOSD_CHARHEIGHT_770_20         18
#define SWOSD_STRINGWIDTH_770_20        770
#define SWOSD_STRINGHEIGHT_770_20       18

#define SWOSD_CHARWIDTH_616_18          8
#define SWOSD_CHARHEIGHT_616_18         16
#define SWOSD_STRINGWIDTH_616_18        616
#define SWOSD_STRINGHEIGHT_616_18       16

#define SWOSD_CHARWIDTH_462_12          6
#define SWOSD_CHARHEIGHT_462_12         12
#define SWOSD_STRINGWIDTH_462_12        462
#define SWOSD_STRINGHEIGHT_462_12       12

#define HIST_LINE_THICKNESS				4

    /* typedefs */

    /* Data structure definitions */

    typedef struct _DATE_TIME_INFO {
        int year;
        int month;
        int mday;
        int hours;
        int min;
        int sec;

    } DATE_TIME_INFO;

    /* Codec Type */

    typedef enum {
        ALG_VID_CODEC_H264 = 0,
        ALG_VID_CODEC_MPEG4,
        ALG_VID_CODEC_MJPEG,
        ALG_VID_CODEC_VNF,
        ALG_VID_CODEC_NONE = -1
    } ALG_VID_CODEC_TYPE;

    /* codec rate control type */

    typedef enum {
        ALG_VID_ENC_RATE_CONTROL_VBR = 0,
        ALG_VID_ENC_RATE_CONTROL_CBR,
        ALG_VID_ENC_RATE_CONTROL_CVBR
    } ALG_VID_RATECONTROL_TYPE;

    /* SW OSD position format */
    typedef enum {
        SWOSD_FMT_TOP_LEFT = 0,
        SWOSD_FMT_TOP_RIGHT,
        SWOSD_FMT_BOTTOM_LEFT = 0,
        SWOSD_FMT_BOTTOM_RIGHT,
        SWOSD_FMT_NONE
    } SWOSD_POS_FMT;

    /* SW OSD TYPE */

    typedef enum {
        SWOSD_BASIC = 0,
        SWOSD_DETAIL,
        SWOSD_COMPLETE
    } SWOSD_Type;

    /* Time format */

    typedef enum {
        SWOSD_FMT_12HR = 0,
        SWOSD_FMT_24HR
    } SWOSD_TIME_FMT;

    /* Year format */

    typedef enum {
        SWOSD_FMT_YMD = 0,
        SWOSD_FMT_MDY,
        SWOSD_FMT_DMY
    } SWOSD_DATE_FMT;

    /* SWOSD config for each stream */

    typedef struct {
        Bool swosdEnable;                                  // <<< 0 - SW OSD
        // is disabled,1 -
        // SW OSD enabled
        SWOSD_Hndl *swosdHndl;                             // <<< Handle for
        // SW OSD
        int osd_count;                                     // <<< Frame count
                                                           //
        // reference for
        // SW OSD

        int capLineOffsetY;                                 // <<< Line offset
                                                           //
		int capLineOffsetUV;
		
        // of the capture
        // buffer
        int capWidth;                                      // <<< Width of
        // capture buffer
        int capHeight;                                     // <<< Height of
        // capture buffer

        int cropWidth;                                     // <<< Encode
        // frame width
        int cropHeight;                                    // <<< Encode
        // frame height
		int memType; 									   //System_MemoryType 
		//- Tiled / non-tiled */
		
        ALG_VID_CODEC_TYPE codecType;                      // <<< Codec type
        ALG_VID_RATECONTROL_TYPE rateControlType;          // <<< Rate
        // Control Type
        int frameRateBase;                                 // <<< Codec Frame
                                                           //
        // rate in fps
        int codecBitrate;                                  // <<< Codec bit
        // rate

        SWOSD_Type swosdType;
        SWOSD_POS_FMT swosdTextPos;
        SWOSD_POS_FMT swosdLogoPos;
        SWOSD_POS_FMT swosdDatePos;
        SWOSD_POS_FMT swosdTimePos;
        SWOSD_TIME_FMT swosdTimeFormat;
        SWOSD_DATE_FMT swosdDateFormat;

        char swosdDispString[SWOSD_MAX_STRING_LENGTH];
		char swosdVaString[SWOSD_MAX_STRING_LENGTH];

        Bool swosdLogoStringUpdate;                        // <<< To update
        // the positions
        // of LOGO and
        // Text windows
        Bool swosdDateTimeUpdate;                          // <<< To update
        // the positions
        // of Date and
        // Time Windows

        Bool swosdTransparencyEnable;
        Bool swosdDateEnable;
        Bool swosdTimeEnable;
        Bool swosdLogoEnable;
        Bool swosdTextEnable;
        Bool swosdEvalEnable;
		Bool vaStrEnable;
		Bool vaDisFlag;
		Bool histEnable;

    } SWOSD_streamConfig_t;

    /* SW OSD config structure */

    typedef struct {
        int numCaptureStream;
        int captureYuvFormat;
        int autoFocusVal;
        int aewbVal1;
        int aewbVal2;
        int aewbVal3;
        SWOSD_streamConfig_t swosdConfig[AVSERVER_MAX_STREAMS];
        DATE_TIME_INFO dateTimeInfo;
		SystemCommon_edmaChPrm edmaHndl;

    } SWOSD_config_t;

	/* externs */
	extern SystemCommon_edmaChPrm gDrawEdmaHndl;
	
    /* Function prototypes */

    int DM81XX_SWOSD_create(SWOSD_config_t * pSwOsdPrm);
    int DM81XX_SWOSD_delete(void);
    int DM81XX_SWOSD_apply(int streamId, int frameBufAddrY, int frameBufAddrUV);
    int DM81XX_SWOSD_Reset(int streamId,int pitch0,int pitch1,int width,int height);
	int DM81XX_SWOSD_drawHistogram(int streamId,int frameBufAddrY,int frameBufAddrUV,int *pHistDataAddr);

    void DM81XX_SWOSD_setSwOsdEnable(int streamId, int enable);
    void DM81XX_SWOSD_setCodecFrameRate(int streamId, int frameRate);
    void DM81XX_SWOSD_setCodecBitRate(int streamId, int bitRate);
    void DM81XX_SWOSD_setRateControl(int streamId, int rateControlType);
    void DM81XX_SWOSD_setAutoFocus(int streamId, int autoFocusVal);
    void DM81XX_SWOSD_setDateEnable(int streamId, Bool swosdDateEnable);
    void DM81XX_SWOSD_setTransparencyEnable(int streamId, Bool swosdTransparencyEnable);
    void DM81XX_SWOSD_setTimeEnable(int streamId, Bool swosdTimeEnable);
    void DM81XX_SWOSD_setTextEnable(int streamId, Bool swosdTextEnable);
    void DM81XX_SWOSD_setEvalEnable(int streamId, Bool swosdEvalEnable);
    void DM81XX_SWOSD_setTextPosn(int streamId, int textPosn);
    void DM81XX_SWOSD_setLogoPosn(int streamId, int logoPosn);
    void DM81XX_SWOSD_setDtailedInfo(int streamId, int detailedInfo);
    void DM81XX_SWOSD_setUserString(int streamId, char *usrString);
    void DM81XX_SWOSD_setDateTime(int streamId, int *pPrm);
    void DM81XX_SWOSD_setDateTimePosn(int streamId, int *pPrm);
    void DM81XX_SWOSD_setCodecType(int streamId, int codecType);
    void DM81XX_SWOSD_setLogoEnable(int streamId, Bool swosdLogoEnable);
	void DM81XX_SWOSD_setVaString(int streamId,char *pVaString,int disFlag);
	void DM81XX_SWOSD_setHistEnable(int streamId,int histEnable);	
	int DM81XX_SWOSD_getResolution(int streamId,int *pPitch0,int *pPitch1,int *pWidth,int *pHeight);
	int DM81XX_SWOSD_isTransparencyEnable(int streamId);
	
#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _DM81XX_SWOSD_H_
                                                            */
