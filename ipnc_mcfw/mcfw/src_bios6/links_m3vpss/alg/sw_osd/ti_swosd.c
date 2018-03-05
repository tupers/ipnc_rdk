/** ==================================================================
 *  @file   ti_swosd.c
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

/******************************************************************************

              SW OSD implementation for IPNC application

******************************************************************************/

/* include files */

#include <stdio.h>
#include <string.h>
#include <xdc/std.h>
#include "swosd_priv.h"
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ivideo.h>
#include <mcfw/src_bios6/utils/utils_mem.h>
#include "alg_ti_imx_swosd.h"
#include <mcfw/src_bios6/links_m3vpss/alg/common/ti_draw.h>
#include <mcfw/src_bios6/links_m3vpss/camera/cameraLink_priv.h>

/* macros */

//#define SWOSD_PRINT_DEBUG

#define SWOSD_ERROR(fmt,args...) Vps_printf(fmt,## args)

#ifdef SWOSD_PRINT_DEBUG
#define SWOSD_DBG(fmt,args...) Vps_printf(fmt,## args)
#else
#define SWOSD_DBG(fmt,args...)
#endif

//static int imxSwosdEnable = 0;

/* extern variables */

extern char TILogo_Y_160_64[];

extern char TILogo_UV_160_64[];

extern char TILogo_Y_80_32[];

extern char TILogo_UV_80_32[];

#ifdef SWOSD_USER_TRANSPARANCY_ON

extern char YData_1540_40_WFONT[];

extern char UVData_1540_40_WFONT[];

extern char YData_1232_30[];

extern char UVData_1232_30[];

extern char YData_1232_30_WFONT[];

extern char UVData_1232_30_WFONT[];

extern char YData_1078_28[];

extern char UVData_1078_28[];

extern char YData_1078_28_WFONT[];

extern char UVData_1078_28_WFONT[];

extern char YData_770_20[];

extern char UVData_770_20[];

extern char YData_770_20_WFONT[];

extern char UVData_770_20_WFONT[];

extern char YData_462_12[];

extern char UVData_462_12[];

extern char YData_462_12_WFONT[];

extern char UVData_462_12_WFONT[];

#else

extern char YData_1540_40_WFONT_PURE[];

extern char UVData_1540_40_WFONT_PURE[];

extern char YData_1232_30_WFONT_PURE[];

extern char UVData_1232_30_WFONT_PURE[];

extern char YData_1078_28_WFONT_PURE[];

extern char UVData_1078_28_WFONT_PURE[];

extern char YData_770_20_WFONT_PURE[];

extern char UVData_770_20_WFONT_PURE[];

extern char YData_462_12_WFONT_PURE[];

extern char UVData_462_12_WFONT_PURE[];

#endif

/* Gobal variables */

static FILE *fpLogo160;

static FILE *fpLogo80;

char *iconBuff160_Y = NULL, *iconBuff160_UV = NULL;

char *iconBuff80_Y = NULL, *iconBuff80_UV = NULL;

char *strBuff1540_Y[AVSERVER_MAX_STREAMS][2],
    *strBuff1540_UV[AVSERVER_MAX_STREAMS][2];
char *strBuff1232_Y[AVSERVER_MAX_STREAMS][MAX_STR_BUFF],
    *strBuff1232_UV[AVSERVER_MAX_STREAMS][MAX_STR_BUFF];
char *strBuff1078_Y[AVSERVER_MAX_STREAMS][MAX_STR_BUFF],
    *strBuff1078_UV[AVSERVER_MAX_STREAMS][MAX_STR_BUFF];
char *strBuff770_Y[AVSERVER_MAX_STREAMS][MAX_STR_BUFF],
    *strBuff770_UV[AVSERVER_MAX_STREAMS][MAX_STR_BUFF];
char *strBuff462_Y[AVSERVER_MAX_STREAMS][MAX_STR_BUFF],
    *strBuff462_UV[AVSERVER_MAX_STREAMS][MAX_STR_BUFF];

char *fontBuff770_Y = NULL, *fontBuff770_UV = NULL;

int fontDB770_Y[256], fontDB770_UV[256];

#ifdef SWOSD_USER_TRANSPARANCY_ON
char *fontBuff1540_Y1 = NULL, *fontBuff1540_UV1 = NULL;

char *fontBuff1232_Y = NULL, *fontBuff1232_UV = NULL, *fontBuff1232_Y1 =
    NULL, *fontBuff1232_UV1 = NULL;
char *fontBuff1078_Y = NULL, *fontBuff1078_UV = NULL, *fontBuff1078_Y1 =
    NULL, *fontBuff1078_UV1 = NULL;
char *fontBuff770_Y1 = NULL, *fontBuff770_UV1 = NULL;

char *fontBuff462_Y = NULL, *fontBuff462_UV = NULL, *fontBuff462_Y1 =
    NULL, *fontBuff462_UV1 = NULL;

int fontDB1540_Y1[256], fontDB1540_UV1[256];

int fontDB1232_Y[256], fontDB1232_UV[256], fontDB1232_Y1[256],
    fontDB1232_UV1[256];
int fontDB1078_Y[256], fontDB1078_UV[256], fontDB1078_Y1[256],
    fontDB1078_UV1[256];
int fontDB770_Y1[256], fontDB770_UV1[256];

int fontDB462_Y[256], fontDB462_UV[256], fontDB462_Y1[256], fontDB462_UV1[256];
#else
char *fontBuff1540_Y1 = NULL, *fontBuff1540_UV1 = NULL;

char *fontBuff1232_Y1 = NULL, *fontBuff1232_UV1 = NULL;

char *fontBuff1078_Y1 = NULL, *fontBuff1078_UV1 = NULL;

char *fontBuff770_Y1 = NULL, *fontBuff770_UV1 = NULL;

char *fontBuff462_Y1 = NULL, *fontBuff462_UV1 = NULL;

int fontDB1540_Y1[256], fontDB1540_UV1[256];

int fontDB1232_Y1[256], fontDB1232_UV1[256];

int fontDB1078_Y1[256], fontDB1078_UV1[256];

int fontDB770_Y1[256], fontDB770_UV1[256];

int fontDB462_Y1[256], fontDB462_UV1[256];
#endif

int idxHndl[AVSERVER_MAX_STREAMS];

static char autoFocus_str[AVSERVER_MAX_STREAMS][SWOSD_MAX_STRING_LENGTH] =
    { "", "", "" };
static char codecBitRate_str[AVSERVER_MAX_STREAMS][SWOSD_MAX_STRING_LENGTH] =
    { "", "", "" };
static char frameRate_str[AVSERVER_MAX_STREAMS][SWOSD_MAX_STRING_LENGTH] =
    { "", "", "" };
static char date_str[AVSERVER_MAX_STREAMS][SWOSD_MAX_STRING_LENGTH] =
    { "", "", "" };
static char time_str[AVSERVER_MAX_STREAMS][SWOSD_MAX_STRING_LENGTH] =
    { "", "", "" };
static char AEWB_str[AVSERVER_MAX_STREAMS][SWOSD_MAX_STRING_LENGTH] =
    { "", "", "" };

/* Global SWOSD config structure */

SWOSD_config_t gSWOSD_config;

/* Function Prototypes */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getCodecType
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
static int DM81XX_SWOSD_getCodecType(int streamId, char *codecStr);

/* ===================================================================
 *  @func     DM81XX_SWOSD_getRateControl
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
static int DM81XX_SWOSD_getRateControl(int streamId, char *rateStr);

/******************************************************************************
                             FUNCTION DEFINITIONS
******************************************************************************/

/* Memory alloc */

/* ===================================================================
 *  @func     SWOSD_ALLOC
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
char *SWOSD_ALLOC(int size, int alignment)
{
    char *ptr;

    ptr = (char *) Utils_memAlloc(size, alignment);
    return ptr;
}

/* Memory free */

/* ===================================================================
 *  @func     SWOSD_FREE
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
void SWOSD_FREE(char *ptr, int size)
{
    Utils_memFree(ptr, size);
}

/* SET functions */

/* Set SW OSD enable */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setSwOsdEnable
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
void DM81XX_SWOSD_setSwOsdEnable(int streamId, int enable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdEnable = enable;
    }
}

/* Set codec type */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setCodecType
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
void DM81XX_SWOSD_setCodecType(int streamId, int codecType)
{
    char codecStr[SWOSD_MAX_STRING_LENGTH] = "Codec Name: ";

    char tempStr[SWOSD_MAX_STRING_LENGTH];

    SWOSD_Fontdata fontInfo;

    SWOSD_Hndl *osdHndl;

    if (streamId < gSWOSD_config.numCaptureStream)
    {
        if ((codecType == IVIDEO_H264BP) ||
            (codecType == IVIDEO_H264MP) || (codecType == IVIDEO_H264HP))
        {
            gSWOSD_config.swosdConfig[streamId].codecType = ALG_VID_CODEC_H264;
        }
        else if(codecType == IVIDEO_MPEG4SP)
			 {
				 gSWOSD_config.swosdConfig[streamId].codecType = ALG_VID_CODEC_MPEG4;
			 }
			 else
			 {
			     gSWOSD_config.swosdConfig[streamId].codecType = ALG_VID_CODEC_MJPEG;
			 }

        osdHndl = gSWOSD_config.swosdConfig[streamId].swosdHndl;

        strcpy(tempStr, "\0");
        DM81XX_SWOSD_getCodecType(streamId, tempStr);
        strcat(codecStr, tempStr);

        if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
            VIDEO_RESOL_WIDTH_SMALL)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;
            fontInfo.fontAddr = fontBuff462_Y1;
            fontInfo.fontAddrUV = fontBuff462_UV1;
            fontInfo.fontDB_Y = fontDB462_Y1;
            fontInfo.fontDB_UV = fontDB462_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, codecStr,
                                   strBuff462_Y[streamId][1],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                                   strlen(codecStr));
            strBuff462_UV[streamId][1] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][1],
                                strBuff462_UV[streamId][1], SWOSD_WINDOW_FOUR);
        }
        else if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
                 VIDEO_RESOL_WIDTH_MEDIUM)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_770_20;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_770_20;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_770_20;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_770_20;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_770_20;
            fontInfo.fontAddr = fontBuff770_Y1;
            fontInfo.fontAddrUV = fontBuff770_UV1;
            fontInfo.fontDB_Y = fontDB770_Y1;
            fontInfo.fontDB_UV = fontDB770_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, codecStr,
                                   strBuff770_Y[streamId][1],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                                   strlen(codecStr));
            strBuff770_UV[streamId][1] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][1],
                                strBuff770_UV[streamId][1], SWOSD_WINDOW_FOUR);
        }
        else if (gSWOSD_config.swosdConfig[streamId].cropWidth <
                 VIDEO_RESOL_WIDTH_LARGE)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_1078_28;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_1078_28;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1078_28;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1078_28;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1078_28;
            fontInfo.fontAddr = fontBuff1078_Y1;
            fontInfo.fontAddrUV = fontBuff1078_UV1;
            fontInfo.fontDB_Y = fontDB1078_Y1;
            fontInfo.fontDB_UV = fontDB1078_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, codecStr,
                                   strBuff1078_Y[streamId][1],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                                   strlen(codecStr));
            strBuff1078_UV[streamId][1] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][1],
                                strBuff1078_UV[streamId][1], SWOSD_WINDOW_FOUR);
        }
        else if (gSWOSD_config.swosdConfig[streamId].cropWidth >=
                 VIDEO_RESOL_WIDTH_LARGE)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_1232_30;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_1232_30;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1232_30;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1232_30;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1232_30;
            fontInfo.fontAddr = fontBuff1232_Y1;
            fontInfo.fontAddrUV = fontBuff1232_UV1;
            fontInfo.fontDB_Y = fontDB1232_Y1;
            fontInfo.fontDB_UV = fontDB1232_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, codecStr,
                                   strBuff1232_Y[streamId][1],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                                   strlen(codecStr));
            strBuff1232_UV[streamId][1] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][1],
                                strBuff1232_UV[streamId][1], SWOSD_WINDOW_FOUR);
        }
    }
}

/* Set codec framerate */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setCodecFrameRate
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
void DM81XX_SWOSD_setCodecFrameRate(int streamId, int frameRate)
{

    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].frameRateBase = frameRate;
    }
}

/* Set codec bitrate */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setCodecBitRate
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
void DM81XX_SWOSD_setCodecBitRate(int streamId, int bitRate)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].codecBitrate = bitRate;
    }
}

/* Set Rate Control string */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setRateControl
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
void DM81XX_SWOSD_setRateControl(int streamId, int rateControlType)
{
    char rateStr[SWOSD_MAX_STRING_LENGTH] = "Rate Control: ";

    char tempStr[SWOSD_MAX_STRING_LENGTH];

    SWOSD_Fontdata fontInfo;

    SWOSD_Hndl *osdHndl;

    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].rateControlType =
            (ALG_VID_RATECONTROL_TYPE) rateControlType;

        osdHndl = gSWOSD_config.swosdConfig[streamId].swosdHndl;

        strcpy(tempStr, "\0");
        DM81XX_SWOSD_getRateControl(streamId, tempStr);
        strcat(rateStr, tempStr);

        if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
            VIDEO_RESOL_WIDTH_SMALL)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;
            fontInfo.fontAddr = fontBuff462_Y1;
            fontInfo.fontAddrUV = fontBuff462_UV1;
            fontInfo.fontDB_Y = fontDB462_Y1;
            fontInfo.fontDB_UV = fontDB462_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, rateStr,
                                   strBuff462_Y[streamId][4],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                                   strlen(rateStr));
            strBuff462_UV[streamId][4] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][4],
                                strBuff462_UV[streamId][4], SWOSD_WINDOW_SEVEN);
        }
        else if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
                 VIDEO_RESOL_WIDTH_MEDIUM)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_770_20;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_770_20;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_770_20;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_770_20;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_770_20;
            fontInfo.fontAddr = fontBuff770_Y1;
            fontInfo.fontAddrUV = fontBuff770_UV1;
            fontInfo.fontDB_Y = fontDB770_Y1;
            fontInfo.fontDB_UV = fontDB770_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, rateStr,
                                   strBuff770_Y[streamId][4],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                                   strlen(rateStr));
            strBuff770_UV[streamId][4] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][4],
                                strBuff770_UV[streamId][4], SWOSD_WINDOW_SEVEN);
        }
        else if (gSWOSD_config.swosdConfig[streamId].cropWidth <
                 VIDEO_RESOL_WIDTH_LARGE)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_1078_28;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_1078_28;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1078_28;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1078_28;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1078_28;
            fontInfo.fontAddr = fontBuff1078_Y1;
            fontInfo.fontAddrUV = fontBuff1078_UV1;
            fontInfo.fontDB_Y = fontDB1078_Y1;
            fontInfo.fontDB_UV = fontDB1078_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, rateStr,
                                   strBuff1078_Y[streamId][4],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                                   strlen(rateStr));
            strBuff1078_UV[streamId][4] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][4],
                                strBuff1078_UV[streamId][4],
                                SWOSD_WINDOW_SEVEN);
        }
        else if (gSWOSD_config.swosdConfig[streamId].cropWidth >=
                 VIDEO_RESOL_WIDTH_LARGE)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_1232_30;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_1232_30;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1232_30;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1232_30;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1232_30;
            fontInfo.fontAddr = fontBuff1232_Y1;
            fontInfo.fontAddrUV = fontBuff1232_UV1;
            fontInfo.fontDB_Y = fontDB1232_Y1;
            fontInfo.fontDB_UV = fontDB1232_UV1;

            SWOSD_MakeOsdwinstring(&fontInfo, rateStr,
                                   strBuff1232_Y[streamId][4],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                                   strlen(rateStr));
            strBuff1232_UV[streamId][4] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][4],
                                strBuff1232_UV[streamId][4],
                                SWOSD_WINDOW_SEVEN);
        }
    }
}

/* Set Auto Focus value */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setAutoFocus
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
void DM81XX_SWOSD_setAutoFocus(int streamId, int autoFocusVal)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.autoFocusVal = autoFocusVal;
    }
}

/* Set Transparency Enable */
void DM81XX_SWOSD_setTransparencyEnable(int streamId, Bool swosdTransparencyEnable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdTransparencyEnable = swosdTransparencyEnable;
    }
}



/* Set Date Enable */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setDateEnable
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
void DM81XX_SWOSD_setDateEnable(int streamId, Bool swosdDateEnable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdDateEnable = swosdDateEnable;
    }
}

/* Set Time Enable */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setTimeEnable
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
void DM81XX_SWOSD_setTimeEnable(int streamId, Bool swosdTimeEnable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdTimeEnable = swosdTimeEnable;
    }
}

/* Set Logo Enable */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setLogoEnable
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
void DM81XX_SWOSD_setLogoEnable(int streamId, Bool swosdLogoEnable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdLogoEnable = swosdLogoEnable;
    }
}

/* Set Text Enable */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setTextEnable
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
void DM81XX_SWOSD_setTextEnable(int streamId, Bool swosdTextEnable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdTextEnable = swosdTextEnable;
    }
}

/* Set Eval Enable */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setEvalEnable
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
void DM81XX_SWOSD_setEvalEnable(int streamId, Bool swosdEvalEnable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdEvalEnable = swosdEvalEnable;
    }
}

/* Set Text Position */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setTextPosn
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
void DM81XX_SWOSD_setTextPosn(int streamId, int textPosn)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdTextPos =
            (SWOSD_POS_FMT) textPosn;
        gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate = OSD_YES;
    }
}

/* Set Logo Position */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setLogoPosn
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
void DM81XX_SWOSD_setLogoPosn(int streamId, int logoPosn)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdLogoPos =
            (SWOSD_POS_FMT) logoPosn;
        gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate = OSD_YES;
    }
}

/* Set Detailed Info */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setDtailedInfo
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
void DM81XX_SWOSD_setDtailedInfo(int streamId, int detailedInfo)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdType =
            (SWOSD_Type) detailedInfo;
    }
}

/* Set SW OSD user string */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setUserString
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
void DM81XX_SWOSD_setUserString(int streamId, char *usrString)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        strcpy(gSWOSD_config.swosdConfig[streamId].swosdDispString, usrString);
        gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate = OSD_YES;
    }
}

/* Set Date and Time */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setDateTime
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
void DM81XX_SWOSD_setDateTime(int streamId, int *pPrm)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.dateTimeInfo.year = pPrm[0];
        gSWOSD_config.dateTimeInfo.month = pPrm[1];
        gSWOSD_config.dateTimeInfo.mday = pPrm[2];
        gSWOSD_config.dateTimeInfo.hours = pPrm[3];
        gSWOSD_config.dateTimeInfo.min = pPrm[4];
        gSWOSD_config.dateTimeInfo.sec = pPrm[5];
    }
}

/* Set Date Time format */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setDateTimePosn
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
void DM81XX_SWOSD_setDateTimePosn(int streamId, int *pPrm)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].swosdDateFormat =
            (SWOSD_DATE_FMT) pPrm[0];
        gSWOSD_config.swosdConfig[streamId].swosdDatePos =
            (SWOSD_POS_FMT) pPrm[1];
        gSWOSD_config.swosdConfig[streamId].swosdTimeFormat =
            (SWOSD_TIME_FMT) pPrm[2];
        gSWOSD_config.swosdConfig[streamId].swosdTimePos =
            (SWOSD_POS_FMT) pPrm[3];
        gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate = OSD_YES;
    }
}

/* Set Date Time format */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setVaString
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
void DM81XX_SWOSD_setVaString(int streamId,char *pVaString,int disFlag)
{
    if (streamId < gSWOSD_config.numCaptureStream)
	{        
		gSWOSD_config.swosdConfig[streamId].vaStrEnable = 1;
		gSWOSD_config.swosdConfig[streamId].vaDisFlag   = disFlag;
		strcpy(gSWOSD_config.swosdConfig[streamId].swosdVaString,pVaString);
	}	
}

/* Set Date Time format */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setHistEnable
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
void DM81XX_SWOSD_setHistEnable(int streamId,int histEnable)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        gSWOSD_config.swosdConfig[streamId].histEnable = histEnable;
    }	
}

/* GET functions */

/* Get Capture frame rate */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getCaptureFrameRate
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
static float DM81XX_SWOSD_getCaptureFrameRate(int streamId, float *pFrameRate)
{
    /* To be Implemented */
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        *pFrameRate = gSWOSD_config.swosdConfig[streamId].frameRateBase;
    }
    else
    {
        *pFrameRate = 0;
    }

    return SWOSD_SOK;
}

/* Get Codec type string */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getCodecType
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
static int DM81XX_SWOSD_getCodecType(int streamId, char *codecStr)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        if (gSWOSD_config.swosdConfig[streamId].codecType ==
            ALG_VID_CODEC_MJPEG)
            strcpy(codecStr, "MJPEG");
        else if (gSWOSD_config.swosdConfig[streamId].codecType ==
                 ALG_VID_CODEC_H264)
            strcpy(codecStr, "H.264");
        else if (gSWOSD_config.swosdConfig[streamId].codecType ==
                 ALG_VID_CODEC_MPEG4)
            strcpy(codecStr, "MPEG4");
    }
    else
    {
        strcpy(codecStr, "\0");
    }

    return SWOSD_SOK;
}

/* Get Rate Control string */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getRateControl
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
static int DM81XX_SWOSD_getRateControl(int streamId, char *rateStr)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
    	if (gSWOSD_config.swosdConfig[streamId].rateControlType ==
                 ALG_VID_ENC_RATE_CONTROL_VBR)
            strcpy(rateStr, "VBR");
        else if (gSWOSD_config.swosdConfig[streamId].rateControlType ==
                 ALG_VID_ENC_RATE_CONTROL_CBR)
            strcpy(rateStr, "CBR");
		else if (gSWOSD_config.swosdConfig[streamId].rateControlType ==
                 ALG_VID_ENC_RATE_CONTROL_CVBR)
            strcpy(rateStr, "CVBR");    }
    else
    {
        strcpy(rateStr, "\0");
    }

    return SWOSD_SOK;
}

/* Get codec bit rate */

static unsigned int DM81XX_SWOSD_getCodecBitRate(int streamId)
{
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        return gSWOSD_config.swosdConfig[streamId].codecBitrate;
    }
    else
    {
        return 0;
    }
}

/* Get Auto Focus string */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getAutoFocus
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
static int DM81XX_SWOSD_getAutoFocus(int streamId, char *autoFocusStr)
{
    unsigned int afValue;
    if (streamId < gSWOSD_config.numCaptureStream)
    {
	    CameraLink_getAfData(&afValue);
		gSWOSD_config.autoFocusVal = afValue;
        sprintf(autoFocusStr, "%7d", gSWOSD_config.autoFocusVal);
    }
    else
    {
        sprintf(autoFocusStr, "0");
    }

    return SWOSD_SOK;
}

static int DM81XX_SWOSD_getAEWB(int streamId, char *aewbStr)
{
    unsigned int aewbValue;
    if (streamId < gSWOSD_config.numCaptureStream)
    {
        CameraLink_getAEWBData(&aewbValue);
        gSWOSD_config.aewbVal1 = aewbValue;
        sprintf(aewbStr, "%3d %6d", gSWOSD_config.aewbVal1 & ((1<<10) -1) , ( gSWOSD_config.aewbVal1 >> 10)  );
    }
    else
    {
        sprintf(aewbStr, "0");
    }

    return SWOSD_SOK;
}

/* Get current time */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getCurTime
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
static inline void DM81XX_SWOSD_getCurTime(DATE_TIME_INFO * pInfo)
{

    pInfo->year = gSWOSD_config.dateTimeInfo.year;
    pInfo->month = gSWOSD_config.dateTimeInfo.month;
    pInfo->mday = gSWOSD_config.dateTimeInfo.mday;
    pInfo->hours = gSWOSD_config.dateTimeInfo.hours;
    pInfo->min = gSWOSD_config.dateTimeInfo.min;
    pInfo->sec = gSWOSD_config.dateTimeInfo.sec;
}

/* Get Date Time string */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getDateTimeString
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
static int DM81XX_SWOSD_getDateTimeString(char *timeStr, char *dateStr,
                                          int streamId)
{
    DATE_TIME_INFO DTIMEInfo;

    DM81XX_SWOSD_getCurTime(&DTIMEInfo);

    if (gSWOSD_config.swosdConfig[streamId].swosdTimeFormat == SWOSD_FMT_12HR)
    {
        if (DTIMEInfo.hours >= 13)
            sprintf(timeStr, "%02d:%02d:%02dPM", DTIMEInfo.hours - 12,
                    DTIMEInfo.min, DTIMEInfo.sec);
        else if (DTIMEInfo.hours == 12)
            sprintf(timeStr, "%02d:%02d:%02dPM", DTIMEInfo.hours, DTIMEInfo.min,
                    DTIMEInfo.sec);
        else if (DTIMEInfo.hours == 0)
            sprintf(timeStr, "%02d:%02d:%02dAM", DTIMEInfo.hours + 12,
                    DTIMEInfo.min, DTIMEInfo.sec);
        else
            sprintf(timeStr, "%02d:%02d:%02dAM", DTIMEInfo.hours, DTIMEInfo.min,
                    DTIMEInfo.sec);
    }
    else if (gSWOSD_config.swosdConfig[streamId].swosdTimeFormat ==
             SWOSD_FMT_24HR)
    {
        sprintf(timeStr, "%02d:%02d:%02d  ", DTIMEInfo.hours, DTIMEInfo.min,
                DTIMEInfo.sec);
    }

    switch (gSWOSD_config.swosdConfig[streamId].swosdDateFormat)
    {
        case SWOSD_FMT_YMD:
            sprintf(dateStr, "%04d/%02d/%02d", DTIMEInfo.year, DTIMEInfo.month,
                    DTIMEInfo.mday);
            break;
        case SWOSD_FMT_MDY:
            sprintf(dateStr, "%02d/%02d/%04d", DTIMEInfo.month, DTIMEInfo.mday,
                    DTIMEInfo.year);
            break;
        default:
        case SWOSD_FMT_DMY:
            sprintf(dateStr, "%02d/%02d/%04d", DTIMEInfo.mday, DTIMEInfo.month,
                    DTIMEInfo.year);
            break;
    }

    return SWOSD_SOK;
}

/* Get Resolution */

/* ===================================================================
 *  @func     DM81XX_SWOSD_getResolution
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
int DM81XX_SWOSD_getResolution(int streamId,int *pPitch0,int *pPitch1,int *pWidth,int *pHeight)
{
    if (streamId < gSWOSD_config.numCaptureStream)
	{
	    *pPitch0 = gSWOSD_config.swosdConfig[streamId].capLineOffsetY;
		*pPitch1 = gSWOSD_config.swosdConfig[streamId].capLineOffsetUV;
		*pWidth  = gSWOSD_config.swosdConfig[streamId].capWidth;
		*pHeight = gSWOSD_config.swosdConfig[streamId].capHeight;
	}
	else
	{
	    *pPitch0 = 0;
		*pPitch1 = 0;
		*pWidth  = 0;
		*pHeight = 0;	
	}
	
	return SWOSD_SOK;
}

/* ===================================================================
 *  @func     DM81XX_SWOSD_isTransparencyEnable
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
int DM81XX_SWOSD_isTransparencyEnable(int streamId)
{
	return gSWOSD_config.swosdConfig[streamId].swosdTransparencyEnable;
}

/* Set LOGO parameters */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setLogoPrm
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
static inline void DM81XX_SWOSD_setLogoPrm(void)
{
    fpLogo80 = NULL;                                       // fopen
    // (SWOSD_TIICON_80,"rb");

    if (fpLogo80 == NULL)
    {
        SWOSD_DBG("SWOSD_DBG:Default Icon TI Logo\n");
        memcpy(iconBuff80_Y, TILogo_Y_80_32,
               SWOSD_CHARWIDTHTILOGO_80_32 * SWOSD_CHARHEIGHTTILOGO_80_32);
        memcpy(iconBuff80_UV, TILogo_UV_80_32,
               (SWOSD_CHARWIDTHTILOGO_80_32 * SWOSD_CHARHEIGHTTILOGO_80_32) /
               2);
    }
    else
    {
        fread(iconBuff80_Y, 1,
              SWOSD_CHARWIDTHTILOGO_80_32 * SWOSD_CHARHEIGHTTILOGO_80_32,
              fpLogo80);
        fread(iconBuff80_UV, 1,
              (SWOSD_CHARWIDTHTILOGO_80_32 * SWOSD_CHARHEIGHTTILOGO_80_32) / 2,
              fpLogo80);
        fclose(fpLogo80);
    }

    fpLogo160 = NULL;                                      // fopen
    // (SWOSD_TIICON_160
    // ,"rb");

    if (fpLogo160 == NULL)
    {
        SWOSD_DBG("SWOSD_DBG:Default Icon TI Logo\n");
        memcpy(iconBuff160_Y, TILogo_Y_160_64,
               SWOSD_CHARWIDTHTILOGO_160_64 * SWOSD_CHARHEIGHTTILOGO_160_64);
        memcpy(iconBuff160_UV, TILogo_UV_160_64,
               (SWOSD_CHARWIDTHTILOGO_160_64 * SWOSD_CHARHEIGHTTILOGO_160_64) /
               2);
    }
    else
    {
        fread(iconBuff160_Y, 1,
              SWOSD_CHARWIDTHTILOGO_160_64 * SWOSD_CHARHEIGHTTILOGO_160_64,
              fpLogo160);
        fread(iconBuff160_UV, 1,
              (SWOSD_CHARWIDTHTILOGO_160_64 * SWOSD_CHARHEIGHTTILOGO_160_64) /
              2, fpLogo160);
        fclose(fpLogo160);
    }
}

/* create font DB */

/* ===================================================================
 *  @func     DM81XX_SWOSD_createFontDB
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
static inline void DM81XX_SWOSD_createFontDB()
{
    SWOSD_Fontdata fontInfo;

    /* font width = 6,height = 12 */
    fontInfo.format = SWOSD_FORMATYUV420p;
    fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
    fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
    fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
    fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
    fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;
    fontInfo.fontAddr = fontBuff462_Y1;
    fontInfo.fontAddrUV = fontBuff462_UV1;

    SWOSD_createFontDatabase(fontDB462_Y1, fontDB462_UV1,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);

#ifdef SWOSD_USER_TRANSPARANCY_ON
    fontInfo.fontAddr = fontBuff462_Y;
    fontInfo.fontAddrUV = fontBuff462_UV;

    SWOSD_createFontDatabase(fontDB462_Y, fontDB462_UV,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);
#endif

    /* font width = 10,height = 18 */
    fontInfo.format = SWOSD_FORMATYUV420p;
    fontInfo.charWidth = SWOSD_CHARWIDTH_770_20;
    fontInfo.charHeight = SWOSD_CHARHEIGHT_770_20;
    fontInfo.totFontWidth = SWOSD_STRINGWIDTH_770_20;
    fontInfo.totFontHeight = SWOSD_CHARHEIGHT_770_20;
    fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_770_20;
    fontInfo.fontAddr = fontBuff770_Y1;
    fontInfo.fontAddrUV = fontBuff770_UV1;

    SWOSD_createFontDatabase(fontDB770_Y1, fontDB770_UV1,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);

#ifdef SWOSD_USER_TRANSPARANCY_ON
    fontInfo.fontAddr = fontBuff770_Y;
    fontInfo.fontAddrUV = fontBuff770_UV;

    SWOSD_createFontDatabase(fontDB770_Y, fontDB770_UV,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);
#endif

    /* font width = 14,height = 26 */
    fontInfo.format = SWOSD_FORMATYUV420p;
    fontInfo.charWidth = SWOSD_CHARWIDTH_1078_28;
    fontInfo.charHeight = SWOSD_CHARHEIGHT_1078_28;
    fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1078_28;
    fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1078_28;
    fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1078_28;
    fontInfo.fontAddr = fontBuff1078_Y1;
    fontInfo.fontAddrUV = fontBuff1078_UV1;

    SWOSD_createFontDatabase(fontDB1078_Y1, fontDB1078_UV1,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);

#ifdef SWOSD_USER_TRANSPARANCY_ON
    fontInfo.fontAddr = fontBuff1078_Y;
    fontInfo.fontAddrUV = fontBuff1078_UV;

    SWOSD_createFontDatabase(fontDB1078_Y, fontDB1078_UV,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);
#endif

    /* font width = 16,height = 28 */
    fontInfo.format = SWOSD_FORMATYUV420p;
    fontInfo.charWidth = SWOSD_CHARWIDTH_1232_30;
    fontInfo.charHeight = SWOSD_CHARHEIGHT_1232_30;
    fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1232_30;
    fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1232_30;
    fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1232_30;
    fontInfo.fontAddr = fontBuff1232_Y1;
    fontInfo.fontAddrUV = fontBuff1232_UV1;

    SWOSD_createFontDatabase(fontDB1232_Y1, fontDB1232_UV1,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);

#ifdef SWOSD_USER_TRANSPARANCY_ON
    fontInfo.fontAddr = fontBuff1232_Y;
    fontInfo.fontAddrUV = fontBuff1232_UV;

    SWOSD_createFontDatabase(fontDB1232_Y, fontDB1232_UV,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);
#endif

    /* font width = 20,height = 38 */
    fontInfo.format = SWOSD_FORMATYUV420p;
    fontInfo.charWidth = SWOSD_CHARWIDTH_1540_40;
    fontInfo.charHeight = SWOSD_CHARHEIGHT_1540_40;
    fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1540_40;
    fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1540_40;
    fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1540_40;
    fontInfo.fontAddr = fontBuff1540_Y1;
    fontInfo.fontAddrUV = fontBuff1540_UV1;

    SWOSD_createFontDatabase(fontDB1540_Y1, fontDB1540_UV1,
                             (char *) SWOSD_STRINGPATTERN, &fontInfo);
}

/* Set win parameters */

/* ===================================================================
 *  @func     DM81XX_SWOSD_setWinParam
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
static int DM81XX_SWOSD_setWinParam(SWOSD_Hndl * osdHndl, int height, int width)
{
    SWOSD_BmpWinPrm bmpWinPrm;

#ifdef SWOSD_USER_TRANSPARANCY_ON
    bmpWinPrm.userTransparancy = SWOSD_RBOOLTRUE;
#else
    bmpWinPrm.userTransparancy = SWOSD_RBOOLFALSE;
#endif

    if (width <= VIDEO_RESOL_WIDTH_SMALL)                  // For QVGA Types
        // of Resolution
    {
        // for window 1 - LOGO window
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.format = SWOSD_FORMATYUV420p;
        bmpWinPrm.startX = 0;
        bmpWinPrm.startY = 0;
        bmpWinPrm.width = SWOSD_CHARWIDTHTILOGO_80_32;
        bmpWinPrm.height = SWOSD_CHARHEIGHTTILOGO_80_32;
        bmpWinPrm.lineOffset = SWOSD_CHARWIDTHTILOGO_80_32;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDBLACKTRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = iconBuff80_Y;
        bmpWinPrm.bmpWinAddrUV = iconBuff80_UV;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ONE, &bmpWinPrm);

        // for window 2
        bmpWinPrm.enable = SWOSD_RBOOLTRUE;
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_462_12, 4);
        bmpWinPrm.startY = SWOSD_CHARWIDTH_462_12 / 3;
        bmpWinPrm.width = SWOSD_STRINGWIDTH_462_12;
        bmpWinPrm.height = SWOSD_CHARHEIGHT_462_12;
        bmpWinPrm.lineOffset = SWOSD_STRINGWIDTH_462_12;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDBLACKTRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = SWOSD_NULL;
        bmpWinPrm.bmpWinAddrUV = SWOSD_NULL;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWO, &bmpWinPrm);
		
        // for window 12
        bmpWinPrm.startX = SWOSD_align((width >> 1) - SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_462_12, 4);
        bmpWinPrm.startY = height - SWOSD_CHARHEIGHT_462_12;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWELVE, &bmpWinPrm);		

        // for window 3
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.startX = SWOSD_align(SWOSD_CHARWIDTH_462_12, 4);
        bmpWinPrm.startY = height / 2 - 2 * SWOSD_CHARHEIGHT_462_12;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THREE, &bmpWinPrm);

        // for window 4
        bmpWinPrm.startY = height / 2 - 1 * SWOSD_CHARHEIGHT_462_12;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FOUR, &bmpWinPrm);

        // for window 5
        bmpWinPrm.startY = height / 2 - 0 * SWOSD_CHARHEIGHT_462_12;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FIVE, &bmpWinPrm);

        // for window 6
        bmpWinPrm.startY = height / 2 + 1 * SWOSD_CHARHEIGHT_462_12;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SIX, &bmpWinPrm);

        // for window 7
        bmpWinPrm.startY = height / 2 + 2 * SWOSD_CHARHEIGHT_462_12;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SEVEN, &bmpWinPrm);

        // for window 10
        bmpWinPrm.startY = height / 2 + 3 * SWOSD_CHARHEIGHT_462_12;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TEN, &bmpWinPrm);

        // for window 11
        bmpWinPrm.startX =
            SWOSD_align(((width -
                          SWOSD_MAX_STRING_LENGTH) / 2) *
                        SWOSD_CHARWIDTH_462_12, 4);
        bmpWinPrm.startY = 0;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ELEVEN, &bmpWinPrm);

        // for window 8
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_462_12, 4);
        bmpWinPrm.startY = height - 3 * SWOSD_CHARHEIGHT_462_12;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_EIGHT, &bmpWinPrm);

        // for window 9
        bmpWinPrm.startY = height - 2 * SWOSD_CHARHEIGHT_462_12;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_NINE, &bmpWinPrm);				

        // for window 13
        bmpWinPrm.startY = height / 2 + 4 * SWOSD_CHARHEIGHT_462_12;
        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THIRTEEN, &bmpWinPrm);				
    }
    else if (width <= VIDEO_RESOL_WIDTH_MEDIUM)            // For VGA Types
        // of Resolution
    {
        // for window 1 - LOGO window
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.format = SWOSD_FORMATYUV420p;
        bmpWinPrm.startX = 0;
        bmpWinPrm.startY = 0;
        bmpWinPrm.width = SWOSD_CHARWIDTHTILOGO_160_64;
        bmpWinPrm.height = SWOSD_CHARHEIGHTTILOGO_160_64;
        bmpWinPrm.lineOffset = SWOSD_CHARWIDTHTILOGO_160_64;
        bmpWinPrm.transperencyVal = SWOSD_OSDWHITETRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDWHITETRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = iconBuff160_Y;
        bmpWinPrm.bmpWinAddrUV = iconBuff160_UV;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ONE, &bmpWinPrm);

        // for window 2
        bmpWinPrm.enable = SWOSD_RBOOLTRUE;
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1078_28, 4);
        bmpWinPrm.startY = SWOSD_CHARWIDTH_1078_28 / 3;
        bmpWinPrm.width = SWOSD_STRINGWIDTH_1078_28;
        bmpWinPrm.height = SWOSD_CHARHEIGHT_1078_28;
        bmpWinPrm.lineOffset = SWOSD_STRINGWIDTH_1078_28;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDBLACKTRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = SWOSD_NULL;
        bmpWinPrm.bmpWinAddrUV = SWOSD_NULL;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWO, &bmpWinPrm);
		
        // for window 12
        bmpWinPrm.startX = SWOSD_align((width >> 1) - SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1078_28, 4);
        bmpWinPrm.startY = height - SWOSD_CHARHEIGHT_1078_28;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWELVE, &bmpWinPrm);		

        // for window 3
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.startX = SWOSD_align(SWOSD_CHARWIDTH_770_20, 4);
        bmpWinPrm.startY = height / 2 - 2 * SWOSD_CHARHEIGHT_770_20;
        bmpWinPrm.width = SWOSD_STRINGWIDTH_770_20;
        bmpWinPrm.height = SWOSD_CHARHEIGHT_770_20;
        bmpWinPrm.lineOffset = SWOSD_STRINGWIDTH_770_20;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDBLACKTRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = SWOSD_NULL;
        bmpWinPrm.bmpWinAddrUV = SWOSD_NULL;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THREE, &bmpWinPrm);

        // for window 4
        bmpWinPrm.startY = height / 2 - 1 * SWOSD_CHARHEIGHT_770_20;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FOUR, &bmpWinPrm);

        // for window 5
        bmpWinPrm.startY = height / 2 - 0 * SWOSD_CHARHEIGHT_770_20;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FIVE, &bmpWinPrm);

        // for window 6
        bmpWinPrm.startY = height / 2 + 1 * SWOSD_CHARHEIGHT_770_20;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SIX, &bmpWinPrm);

        // for window 7
        bmpWinPrm.startY = height / 2 + 2 * SWOSD_CHARHEIGHT_770_20;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SEVEN, &bmpWinPrm);

        // for window 10
        bmpWinPrm.startY = height / 2 + 3 * SWOSD_CHARHEIGHT_770_20;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TEN, &bmpWinPrm);

        // for window 11
        bmpWinPrm.startX =
            SWOSD_align((width -
                         (SWOSD_MAX_STRING_LENGTH * SWOSD_CHARHEIGHT_770_20)) /
                        2, 4);
        bmpWinPrm.startY = 0;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ELEVEN, &bmpWinPrm);

        // for window 8
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_770_20, 4);
        bmpWinPrm.startY = height - 3 * SWOSD_CHARHEIGHT_770_20;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_EIGHT, &bmpWinPrm);

        // for window 9
        bmpWinPrm.startY = height - 2 * SWOSD_CHARHEIGHT_770_20;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_NINE, &bmpWinPrm);				

        // for window 13
        bmpWinPrm.startY = height / 2 + 4 * SWOSD_CHARHEIGHT_770_20;
        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THIRTEEN, &bmpWinPrm);
    }
    else if (width < VIDEO_RESOL_WIDTH_LARGE)              // For SVGA Types
        // of Resolution
    {
        // for window 1 - LOGO window
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.format = SWOSD_FORMATYUV420p;
        bmpWinPrm.startX = 0;
        bmpWinPrm.startY = 0;
        bmpWinPrm.width = SWOSD_CHARWIDTHTILOGO_160_64;
        bmpWinPrm.height = SWOSD_CHARHEIGHTTILOGO_160_64;
        bmpWinPrm.lineOffset = SWOSD_CHARWIDTHTILOGO_160_64;
        bmpWinPrm.transperencyVal = SWOSD_OSDWHITETRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDWHITETRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = iconBuff160_Y;
        bmpWinPrm.bmpWinAddrUV = iconBuff160_UV;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ONE, &bmpWinPrm);

        // for window 2
        bmpWinPrm.enable = SWOSD_RBOOLTRUE;
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1232_30, 4);
        bmpWinPrm.startY = SWOSD_CHARWIDTH_1232_30 / 3;
        bmpWinPrm.width = SWOSD_STRINGWIDTH_1232_30;
        bmpWinPrm.height = SWOSD_CHARHEIGHT_1232_30;
        bmpWinPrm.lineOffset = SWOSD_STRINGWIDTH_1232_30;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDWHITETRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = SWOSD_NULL;
        bmpWinPrm.bmpWinAddrUV = SWOSD_NULL;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWO, &bmpWinPrm);
		
        // for window 12
        bmpWinPrm.startX = SWOSD_align((width >> 1) - SWOSD_MAX_STRING_LENGTH * SWOSD_STRINGWIDTH_1232_30, 4);
        bmpWinPrm.startY = height - SWOSD_CHARHEIGHT_1232_30;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWELVE, &bmpWinPrm);		

        // for window 3
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.startX = SWOSD_align(SWOSD_CHARWIDTH_1078_28, 4);
        bmpWinPrm.startY = height / 2 - 2 * SWOSD_CHARHEIGHT_1078_28;
        bmpWinPrm.width = SWOSD_STRINGWIDTH_1078_28;
        bmpWinPrm.height = SWOSD_CHARHEIGHT_1078_28;
        bmpWinPrm.lineOffset = SWOSD_STRINGWIDTH_1078_28;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDBLACKTRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = SWOSD_NULL;
        bmpWinPrm.bmpWinAddrUV = SWOSD_NULL;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THREE, &bmpWinPrm);

        // for window 4
        bmpWinPrm.startY = height / 2 - 1 * SWOSD_CHARHEIGHT_1078_28;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FOUR, &bmpWinPrm);

        // for window 5
        bmpWinPrm.startY = height / 2 - 0 * SWOSD_CHARHEIGHT_1078_28;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FIVE, &bmpWinPrm);

        // for window 6
        bmpWinPrm.startY = height / 2 + 1 * SWOSD_CHARHEIGHT_1078_28;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SIX, &bmpWinPrm);

        // for window 7
        bmpWinPrm.startY = height / 2 + 2 * SWOSD_CHARHEIGHT_1078_28;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SEVEN, &bmpWinPrm);

        // for window 10
        bmpWinPrm.startY = height / 2 + 3 * SWOSD_CHARHEIGHT_1078_28;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TEN, &bmpWinPrm);

        // for window 11
        bmpWinPrm.startX =
            SWOSD_align(((width -
                          SWOSD_MAX_STRING_LENGTH) / 2) *
                        SWOSD_CHARHEIGHT_1078_28, 4);
        bmpWinPrm.startY = 0;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ELEVEN, &bmpWinPrm);

        // for window 8
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1078_28, 4);
        bmpWinPrm.startY = height - 3 * SWOSD_CHARHEIGHT_1078_28;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_EIGHT, &bmpWinPrm);

        // for window 9
        bmpWinPrm.startY = height - 2 * SWOSD_CHARHEIGHT_1078_28;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_NINE, &bmpWinPrm);				

        // for window 13
        bmpWinPrm.startY = height / 2 + 4 * SWOSD_CHARHEIGHT_1078_28;
        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THIRTEEN, &bmpWinPrm);
    }
    else if (width >= VIDEO_RESOL_WIDTH_LARGE)
    {
        // for window 1 - LOGO window
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.format = SWOSD_FORMATYUV420p;
        bmpWinPrm.startX = 0;
        bmpWinPrm.startY = 0;
        bmpWinPrm.width = SWOSD_CHARWIDTHTILOGO_160_64;
        bmpWinPrm.height = SWOSD_CHARHEIGHTTILOGO_160_64;
        bmpWinPrm.lineOffset = SWOSD_CHARWIDTHTILOGO_160_64;
        bmpWinPrm.transperencyVal = SWOSD_OSDWHITETRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDWHITETRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = iconBuff160_Y;
        bmpWinPrm.bmpWinAddrUV = iconBuff160_UV;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ONE, &bmpWinPrm);

        // for window 2
        bmpWinPrm.enable = SWOSD_RBOOLTRUE;
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1540_40, 4);
        bmpWinPrm.startY = SWOSD_CHARWIDTH_1540_40 / 3;
        bmpWinPrm.width = SWOSD_STRINGWIDTH_1540_40;
        bmpWinPrm.height = SWOSD_CHARHEIGHT_1540_40;
        bmpWinPrm.lineOffset = SWOSD_STRINGWIDTH_1540_40;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDBLACKTRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = SWOSD_NULL;
        bmpWinPrm.bmpWinAddrUV = SWOSD_NULL;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWO, &bmpWinPrm);
		
        // for window 12
        bmpWinPrm.startX = SWOSD_align((width >> 1) - SWOSD_MAX_STRING_LENGTH * SWOSD_STRINGWIDTH_1540_40, 4);
        bmpWinPrm.startY = height - SWOSD_CHARHEIGHT_1540_40;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TWELVE, &bmpWinPrm);		

        // for window 3
        bmpWinPrm.enable = SWOSD_RBOOLFALSE;
        bmpWinPrm.startX = SWOSD_align(SWOSD_CHARWIDTH_1232_30, 4);
        bmpWinPrm.startY = height / 2 - 2 * SWOSD_CHARHEIGHT_1232_30;
        bmpWinPrm.width = SWOSD_STRINGWIDTH_1232_30;
        bmpWinPrm.height = SWOSD_CHARHEIGHT_1232_30;
        bmpWinPrm.lineOffset = SWOSD_STRINGWIDTH_1232_30;
        bmpWinPrm.transperencyVal = SWOSD_OSDBLACKTRANSVAL;
        bmpWinPrm.transperencyRange = SWOSD_OSDBLACKTRANSRANGE;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;
        bmpWinPrm.resizeEnable = 0;
        bmpWinPrm.bmpWinAddr = SWOSD_NULL;
        bmpWinPrm.bmpWinAddrUV = SWOSD_NULL;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THREE, &bmpWinPrm);

        // for window 4
        bmpWinPrm.startY = height / 2 - 1 * SWOSD_CHARHEIGHT_1232_30;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FOUR, &bmpWinPrm);

        // for window 5
        bmpWinPrm.startY = height / 2 - 0 * SWOSD_CHARHEIGHT_1232_30;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_FIVE, &bmpWinPrm);

        // for window 6
        bmpWinPrm.startY = height / 2 + 1 * SWOSD_CHARHEIGHT_1232_30;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SIX, &bmpWinPrm);

        // for window 7
        bmpWinPrm.startY = height / 2 + 2 * SWOSD_CHARHEIGHT_1232_30;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_SEVEN, &bmpWinPrm);

        // for window 10
        bmpWinPrm.startY = height / 2 + 3 * SWOSD_CHARHEIGHT_1232_30;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_TEN, &bmpWinPrm);

        // for window 11
        bmpWinPrm.startX =
            SWOSD_align(((width -
                          SWOSD_MAX_STRING_LENGTH) / 2) *
                        SWOSD_CHARHEIGHT_1232_30, 4);
        bmpWinPrm.startY = 0;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_ELEVEN, &bmpWinPrm);

        // for window 8
        bmpWinPrm.startX =
            SWOSD_align(width -
                        SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1232_30, 4);
        bmpWinPrm.startY = height - 3 * SWOSD_CHARHEIGHT_1232_30;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_EIGHT, &bmpWinPrm);

        // for window 9
        bmpWinPrm.startY = height - 2 * SWOSD_CHARHEIGHT_1232_30;
        bmpWinPrm.transperencyEnable = SWOSD_BMPTRANSDISABLE;

        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_NINE, &bmpWinPrm);				

        // for window 13
        bmpWinPrm.startY = height / 2 + 4 * SWOSD_CHARHEIGHT_1232_30;
        SWOSD_setBmpWinPrm(osdHndl, SWOSD_WINDOW_THIRTEEN, &bmpWinPrm);
    }

    return SWOSD_SOK;
}

/* create fixed window strings */

/* ===================================================================
 *  @func     DM81XX_SWOSD_makeFixWinString
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
static int DM81XX_SWOSD_makeFixWinString(SWOSD_Hndl * osdHndl, int streamId)
{
    char resolStr[SWOSD_MAX_STRING_LENGTH], bitRateStr[SWOSD_MAX_STRING_LENGTH];

    char codecStr[SWOSD_MAX_STRING_LENGTH] = "Codec Name: ";

    char rateStr[SWOSD_MAX_STRING_LENGTH] = "Rate Control: ";

    char framerateStr[SWOSD_MAX_STRING_LENGTH] = "Frame Rate: 00.00";

    char tempStr[SWOSD_MAX_STRING_LENGTH];

    char autoFocusStr[SWOSD_MAX_STRING_LENGTH] = "Auto Focus: ";
    char AEWBStr[SWOSD_MAX_STRING_LENGTH] = "AE:AWB: ";

    char evalVerStr[SWOSD_MAX_STRING_LENGTH] = "EVAL VERSION";

    Uint32 bitRate;

    SWOSD_Fontdata fontInfo;

    strcpy(tempStr, "\0");
    DM81XX_SWOSD_getCodecType(streamId, tempStr);
    strcat(codecStr, tempStr);

    strcpy(tempStr, "\0");
    DM81XX_SWOSD_getRateControl(streamId, tempStr);
    strcat(rateStr, tempStr);

    strcpy(tempStr, "\0");
    DM81XX_SWOSD_getAutoFocus(streamId, tempStr);
    strcat(autoFocusStr, tempStr);

    strcpy(tempStr, "\0");
    DM81XX_SWOSD_getAEWB(streamId, tempStr);
    strcat(AEWBStr, tempStr);

    bitRate = DM81XX_SWOSD_getCodecBitRate(streamId);
    if (gSWOSD_config.swosdConfig[streamId].codecType == ALG_VID_CODEC_MJPEG)
    {
        sprintf(bitRateStr, "QP Value:%3d", bitRate);
    }
    else
    {
        sprintf(bitRateStr, "Bit Rate:%5dKbps", (bitRate / 1000));
    }

    sprintf(resolStr, "Resolution: %dX%d",
            gSWOSD_config.swosdConfig[streamId].cropWidth,
            gSWOSD_config.swosdConfig[streamId].cropHeight);

    if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
        VIDEO_RESOL_WIDTH_SMALL)
    {
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;
        fontInfo.fontAddr = fontBuff462_Y1;
        fontInfo.fontAddrUV = fontBuff462_UV1;
        fontInfo.fontDB_Y = fontDB462_Y1;
        fontInfo.fontDB_UV = fontDB462_UV1;

        SWOSD_MakeOsdwinstring(&fontInfo, resolStr, strBuff462_Y[streamId][0],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE],
                               strlen(resolStr));
        strBuff462_UV[streamId][0] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][0],
                            strBuff462_UV[streamId][0], SWOSD_WINDOW_THREE);

        SWOSD_MakeOsdwinstring(&fontInfo, codecStr, strBuff462_Y[streamId][1],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                               strlen(codecStr));
        strBuff462_UV[streamId][1] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][1],
                            strBuff462_UV[streamId][1], SWOSD_WINDOW_FOUR);

        SWOSD_MakeOsdwinstring(&fontInfo, framerateStr,
                               strBuff462_Y[streamId][2],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                               strlen(framerateStr));
        strBuff462_UV[streamId][2] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][2],
                            strBuff462_UV[streamId][2], SWOSD_WINDOW_FIVE);

        SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr, strBuff462_Y[streamId][3],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                               strlen(bitRateStr));
        strBuff462_UV[streamId][3] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][3],
                            strBuff462_UV[streamId][3], SWOSD_WINDOW_SIX);

        SWOSD_MakeOsdwinstring(&fontInfo, rateStr, strBuff462_Y[streamId][4],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                               strlen(rateStr));
        strBuff462_UV[streamId][4] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][4],
                            strBuff462_UV[streamId][4], SWOSD_WINDOW_SEVEN);

        SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                               strBuff462_Y[streamId][8],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                               strlen(autoFocusStr));
        strBuff462_UV[streamId][8] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][8],
                            strBuff462_UV[streamId][8], SWOSD_WINDOW_TEN);

        SWOSD_MakeOsdwinstring(&fontInfo, AEWBStr,
                               strBuff462_Y[streamId][11],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN],
                               strlen(AEWBStr));
        strBuff462_UV[streamId][11] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][11],
                            strBuff462_UV[streamId][11], SWOSD_WINDOW_THIRTEEN);

        SWOSD_MakeOsdwinstring(&fontInfo, evalVerStr, strBuff462_Y[streamId][9],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN],
                               strlen(evalVerStr));
        strBuff462_UV[streamId][9] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][9],
                            strBuff462_UV[streamId][9], SWOSD_WINDOW_ELEVEN);

        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff462_Y;
        fontInfo.fontAddrUV = fontBuff462_UV;
        fontInfo.fontDB_Y = fontDB462_Y;
        fontInfo.fontDB_UV = fontDB462_UV;
#else
        fontInfo.fontAddr = fontBuff462_Y1;
        fontInfo.fontAddrUV = fontBuff462_UV1;
        fontInfo.fontDB_Y = fontDB462_Y1;
        fontInfo.fontDB_UV = fontDB462_UV1;
#endif

        SWOSD_MakeOsdwinstring(&fontInfo, "DATE:0000/00/00",
                               strBuff462_Y[streamId][5],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                               strlen("DATE:0000/00/00"));
        strBuff462_UV[streamId][5] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][5],
                            strBuff462_UV[streamId][5], SWOSD_WINDOW_EIGHT);

        SWOSD_MakeOsdwinstring(&fontInfo, "TIME:00:00:00  ",
                               strBuff462_Y[streamId][6],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                               strlen("TIME:00:00:00  "));
        strBuff462_UV[streamId][6] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][6],
                            strBuff462_UV[streamId][6], SWOSD_WINDOW_NINE);
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
             VIDEO_RESOL_WIDTH_MEDIUM)
    {
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_770_20;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_770_20;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_770_20;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_770_20;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_770_20;
        fontInfo.fontAddr = fontBuff770_Y1;
        fontInfo.fontAddrUV = fontBuff770_UV1;
        fontInfo.fontDB_Y = fontDB770_Y1;
        fontInfo.fontDB_UV = fontDB770_UV1;

        SWOSD_MakeOsdwinstring(&fontInfo, resolStr, strBuff770_Y[streamId][0],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE],
                               strlen(resolStr));
        strBuff770_UV[streamId][0] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][0],
                            strBuff770_UV[streamId][0], SWOSD_WINDOW_THREE);

        SWOSD_MakeOsdwinstring(&fontInfo, codecStr, strBuff770_Y[streamId][1],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                               strlen(codecStr));
        strBuff770_UV[streamId][1] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][1],
                            strBuff770_UV[streamId][1], SWOSD_WINDOW_FOUR);

        SWOSD_MakeOsdwinstring(&fontInfo, framerateStr,
                               strBuff770_Y[streamId][2],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                               strlen(framerateStr));
        strBuff770_UV[streamId][2] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][2],
                            strBuff770_UV[streamId][2], SWOSD_WINDOW_FIVE);

        SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr, strBuff770_Y[streamId][3],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                               strlen(bitRateStr));
        strBuff770_UV[streamId][3] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][3],
                            strBuff770_UV[streamId][3], SWOSD_WINDOW_SIX);

        SWOSD_MakeOsdwinstring(&fontInfo, rateStr, strBuff770_Y[streamId][4],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                               strlen(rateStr));
        strBuff770_UV[streamId][4] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][4],
                            strBuff770_UV[streamId][4], SWOSD_WINDOW_SEVEN);

        SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                               strBuff770_Y[streamId][8],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                               strlen(autoFocusStr));
        strBuff770_UV[streamId][8] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][8],
                            strBuff770_UV[streamId][8], SWOSD_WINDOW_TEN);

        SWOSD_MakeOsdwinstring(&fontInfo, AEWBStr,
                               strBuff770_Y[streamId][11],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN],
                               strlen(AEWBStr));
        strBuff770_UV[streamId][11] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][11],
                            strBuff770_UV[streamId][11], SWOSD_WINDOW_THIRTEEN);

        SWOSD_MakeOsdwinstring(&fontInfo, evalVerStr, strBuff770_Y[streamId][9],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN],
                               strlen(evalVerStr));
        strBuff770_UV[streamId][9] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][9],
                            strBuff770_UV[streamId][9], SWOSD_WINDOW_ELEVEN);

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff770_Y;
        fontInfo.fontAddrUV = fontBuff770_UV;
        fontInfo.fontDB_Y = fontDB770_Y;
        fontInfo.fontDB_UV = fontDB770_UV;
#else
        fontInfo.fontAddr = fontBuff770_Y1;
        fontInfo.fontAddrUV = fontBuff770_UV1;
        fontInfo.fontDB_Y = fontDB770_Y1;
        fontInfo.fontDB_UV = fontDB770_UV1;
#endif

        SWOSD_MakeOsdwinstring(&fontInfo, "DATE:0000/00/00",
                               strBuff770_Y[streamId][5],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                               strlen("DATE:0000/00/00"));
        strBuff770_UV[streamId][5] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][5],
                            strBuff770_UV[streamId][5], SWOSD_WINDOW_EIGHT);

        SWOSD_MakeOsdwinstring(&fontInfo, "TIME:00:00:00  ",
                               strBuff770_Y[streamId][6],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                               strlen("TIME:00:00:00  "));
        strBuff770_UV[streamId][6] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][6],
                            strBuff770_UV[streamId][6], SWOSD_WINDOW_NINE);
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth <
             VIDEO_RESOL_WIDTH_LARGE)
    {
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_1078_28;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_1078_28;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1078_28;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1078_28;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1078_28;
        fontInfo.fontAddr = fontBuff1078_Y1;
        fontInfo.fontAddrUV = fontBuff1078_UV1;
        fontInfo.fontDB_Y = fontDB1078_Y1;
        fontInfo.fontDB_UV = fontDB1078_UV1;

        SWOSD_MakeOsdwinstring(&fontInfo, resolStr, strBuff1078_Y[streamId][0],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE],
                               strlen(resolStr));
        strBuff1078_UV[streamId][0] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][0],
                            strBuff1078_UV[streamId][0], SWOSD_WINDOW_THREE);

        SWOSD_MakeOsdwinstring(&fontInfo, codecStr, strBuff1078_Y[streamId][1],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                               strlen(codecStr));
        strBuff1078_UV[streamId][1] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][1],
                            strBuff1078_UV[streamId][1], SWOSD_WINDOW_FOUR);

        SWOSD_MakeOsdwinstring(&fontInfo, framerateStr,
                               strBuff1078_Y[streamId][2],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                               strlen(framerateStr));
        strBuff1078_UV[streamId][2] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][2],
                            strBuff1078_UV[streamId][2], SWOSD_WINDOW_FIVE);

        SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr,
                               strBuff1078_Y[streamId][3],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                               strlen(bitRateStr));
        strBuff1078_UV[streamId][3] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][3],
                            strBuff1078_UV[streamId][3], SWOSD_WINDOW_SIX);

        SWOSD_MakeOsdwinstring(&fontInfo, rateStr, strBuff1078_Y[streamId][4],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                               strlen(rateStr));
        strBuff1078_UV[streamId][4] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][4],
                            strBuff1078_UV[streamId][4], SWOSD_WINDOW_SEVEN);

        SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                               strBuff1078_Y[streamId][8],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                               strlen(autoFocusStr));
        strBuff1078_UV[streamId][8] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][8],
                            strBuff1078_UV[streamId][8], SWOSD_WINDOW_TEN);

        SWOSD_MakeOsdwinstring(&fontInfo, AEWBStr,
                               strBuff1078_Y[streamId][11],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN],
                               strlen(AEWBStr));
        strBuff1078_UV[streamId][11] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][11],
                            strBuff1078_UV[streamId][11], SWOSD_WINDOW_THIRTEEN);

        SWOSD_MakeOsdwinstring(&fontInfo, evalVerStr,
                               strBuff1078_Y[streamId][9],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN],
                               strlen(evalVerStr));
        strBuff1078_UV[streamId][9] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][9],
                            strBuff1078_UV[streamId][9], SWOSD_WINDOW_ELEVEN);

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff1078_Y;
        fontInfo.fontAddrUV = fontBuff1078_UV;
        fontInfo.fontDB_Y = fontDB1078_Y;
        fontInfo.fontDB_UV = fontDB1078_UV;
#else
        fontInfo.fontAddr = fontBuff1078_Y1;
        fontInfo.fontAddrUV = fontBuff1078_UV1;
        fontInfo.fontDB_Y = fontDB1078_Y1;
        fontInfo.fontDB_UV = fontDB1078_UV1;
#endif

        SWOSD_MakeOsdwinstring(&fontInfo, "DATE:0000/00/00",
                               strBuff1078_Y[streamId][5],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                               strlen("DATE:0000/00/00"));
        strBuff1078_UV[streamId][5] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][5],
                            strBuff1078_UV[streamId][5], SWOSD_WINDOW_EIGHT);

        SWOSD_MakeOsdwinstring(&fontInfo, "TIME:00:00:00  ",
                               strBuff1078_Y[streamId][6],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                               strlen("TIME:00:00:00  "));
        strBuff1078_UV[streamId][6] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][6],
                            strBuff1078_UV[streamId][6], SWOSD_WINDOW_NINE);
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth >=
             VIDEO_RESOL_WIDTH_LARGE)
    {
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_1232_30;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_1232_30;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1232_30;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1232_30;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1232_30;
        fontInfo.fontAddr = fontBuff1232_Y1;
        fontInfo.fontAddrUV = fontBuff1232_UV1;
        fontInfo.fontDB_Y = fontDB1232_Y1;
        fontInfo.fontDB_UV = fontDB1232_UV1;

        SWOSD_MakeOsdwinstring(&fontInfo, resolStr, strBuff1232_Y[streamId][0],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE],
                               strlen(resolStr));
        strBuff1232_UV[streamId][0] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THREE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][0],
                            strBuff1232_UV[streamId][0], SWOSD_WINDOW_THREE);

        SWOSD_MakeOsdwinstring(&fontInfo, codecStr, strBuff1232_Y[streamId][1],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR],
                               strlen(codecStr));
        strBuff1232_UV[streamId][1] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FOUR].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][1],
                            strBuff1232_UV[streamId][1], SWOSD_WINDOW_FOUR);

        SWOSD_MakeOsdwinstring(&fontInfo, framerateStr,
                               strBuff1232_Y[streamId][2],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                               strlen(framerateStr));
        strBuff1232_UV[streamId][2] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][2],
                            strBuff1232_UV[streamId][2], SWOSD_WINDOW_FIVE);

        SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr,
                               strBuff1232_Y[streamId][3],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                               strlen(bitRateStr));
        strBuff1232_UV[streamId][3] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][3],
                            strBuff1232_UV[streamId][3], SWOSD_WINDOW_SIX);

        SWOSD_MakeOsdwinstring(&fontInfo, rateStr, strBuff1232_Y[streamId][4],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN],
                               strlen(rateStr));
        strBuff1232_UV[streamId][4] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_SEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][4],
                            strBuff1232_UV[streamId][4], SWOSD_WINDOW_SEVEN);

        SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                               strBuff1232_Y[streamId][8],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                               strlen(autoFocusStr));
        strBuff1232_UV[streamId][8] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][8],
                            strBuff1232_UV[streamId][8], SWOSD_WINDOW_TEN);

        SWOSD_MakeOsdwinstring(&fontInfo, AEWBStr,
                               strBuff1232_Y[streamId][11],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN],
                               strlen(AEWBStr));
        strBuff1232_UV[streamId][11] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][11],
                            strBuff1232_UV[streamId][11], SWOSD_WINDOW_THIRTEEN);

        SWOSD_MakeOsdwinstring(&fontInfo, evalVerStr,
                               strBuff1232_Y[streamId][9],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN],
                               strlen(evalVerStr));
        strBuff1232_UV[streamId][9] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_ELEVEN].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][9],
                            strBuff1232_UV[streamId][9], SWOSD_WINDOW_ELEVEN);

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff1232_Y;
        fontInfo.fontAddrUV = fontBuff1232_UV;
        fontInfo.fontDB_Y = fontDB1232_Y;
        fontInfo.fontDB_UV = fontDB1232_UV;
#else
        fontInfo.fontAddr = fontBuff1232_Y1;
        fontInfo.fontAddrUV = fontBuff1232_UV1;
        fontInfo.fontDB_Y = fontDB1232_Y1;
        fontInfo.fontDB_UV = fontDB1232_UV1;
#endif

        SWOSD_MakeOsdwinstring(&fontInfo, "DATE:0000/00/00",
                               strBuff1232_Y[streamId][5],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                               strlen("DATE:0000/00/00"));
        strBuff1232_UV[streamId][5] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][5],
                            strBuff1232_UV[streamId][5], SWOSD_WINDOW_EIGHT);

        SWOSD_MakeOsdwinstring(&fontInfo, "TIME:00:00:00  ",
                               strBuff1232_Y[streamId][6],
                               &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                               strlen("TIME:00:00:00  "));
        strBuff1232_UV[streamId][6] =
            (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][6],
                            strBuff1232_UV[streamId][6], SWOSD_WINDOW_NINE);
    }

    return SWOSD_SOK;
}

/* Construct the strings for all the windows */

/* ===================================================================
 *  @func     DM81XX_SWOSD_makeWinString
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
static int DM81XX_SWOSD_makeWinString(int streamId, SWOSD_Hndl * osdHndl)
{
    SWOSD_Fontdata fontInfo;

    char timeStr[SWOSD_MAX_STRING_LENGTH], dateStr[SWOSD_MAX_STRING_LENGTH];

    char frameRateStr[SWOSD_MAX_STRING_LENGTH],
        bitRateStr[SWOSD_MAX_STRING_LENGTH],
        autoFocusStr[SWOSD_MAX_STRING_LENGTH],
        AEWBStr[SWOSD_MAX_STRING_LENGTH];
    int codecBitRate, bitratePrefix, bitrateLen;

    float frameRate;

    DM81XX_SWOSD_getDateTimeString(timeStr, dateStr, streamId);

    DM81XX_SWOSD_getCaptureFrameRate(streamId, &frameRate);	
    sprintf(frameRateStr, "%2.2f", frameRate);

    codecBitRate = DM81XX_SWOSD_getCodecBitRate(streamId);

    if (gSWOSD_config.swosdConfig[streamId].codecType == ALG_VID_CODEC_MJPEG)
    {
        sprintf(bitRateStr, "%3d", codecBitRate);
        bitratePrefix = SWOSD_QUALITY_PREFIX;
        bitrateLen = bitratePrefix + strlen(bitRateStr) + SWOSD_QUALITY_SUFFIX;
    }
    else
    {
        sprintf(bitRateStr, "%5d", (codecBitRate / 1000));
        bitratePrefix = SWOSD_BITRATE_PREFIX;
        bitrateLen = bitratePrefix + strlen(bitRateStr) + SWOSD_BITRATE_SUFFIX;
    }

    DM81XX_SWOSD_getAutoFocus(streamId, autoFocusStr);

    DM81XX_SWOSD_getAEWB(streamId, AEWBStr);

    if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
        VIDEO_RESOL_WIDTH_SMALL)
    {
        if (gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate ==
            OSD_YES)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;
			
            fontInfo.fontAddr = fontBuff462_Y1;
            fontInfo.fontAddrUV = fontBuff462_UV1;
            fontInfo.fontDB_Y = fontDB462_Y1;
            fontInfo.fontDB_UV = fontDB462_UV1;

            if (gSWOSD_config.swosdConfig[streamId].swosdTextPos ==
                SWOSD_FMT_TOP_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0,
                                               SWOSD_CHARHEIGHTTILOGO_80_32);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_80_32, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0, 0);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_462_12),
                                                           4), 1);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_80_32, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_462_12),
                                                           4),
                                               SWOSD_CHARHEIGHTTILOGO_80_32);
                }
            }

            SWOSD_MakeOsdwinstring(&fontInfo,
                                   gSWOSD_config.
                                   swosdConfig[streamId].swosdDispString,
                                   strBuff462_Y[streamId][7],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO],
                                   strlen(gSWOSD_config.
                                          swosdConfig
                                          [streamId].swosdDispString));
            strBuff462_UV[streamId][7] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][7],
                                strBuff462_UV[streamId][7], SWOSD_WINDOW_TWO);					
            gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate = OSD_NO;			
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate
                                                           //
        // == OSD_YES)

        /* VA string */
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;
		
        fontInfo.fontAddr = fontBuff462_Y1;
        fontInfo.fontAddrUV = fontBuff462_UV1;
        fontInfo.fontDB_Y = fontDB462_Y1;
        fontInfo.fontDB_UV = fontDB462_UV1;		
		
		SWOSD_MakeOsdwinstring(&fontInfo,
							   gSWOSD_config.swosdConfig[streamId].swosdVaString,
							   strBuff462_Y[streamId][10],
							   &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE],
							   strlen(gSWOSD_config.swosdConfig[streamId].swosdVaString));
								   
        strBuff462_UV[streamId][10] = (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][10],strBuff462_UV[streamId][10], SWOSD_WINDOW_TWELVE);		
		
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_462_12;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_462_12;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_462_12;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_462_12;

        if (gSWOSD_config.swosdConfig[streamId].swosdType == SWOSD_DETAIL)
        {
            if (strcmp(frameRateStr, frameRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff462_Y1;
                fontInfo.fontAddrUV = fontBuff462_UV1;
                fontInfo.fontDB_Y = fontDB462_Y1;
                fontInfo.fontDB_UV = fontDB462_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, frameRateStr,
                                       strBuff462_Y[streamId][2] +
                                       (SWOSD_FRAMERATE_PREFIX *
                                        SWOSD_CHARWIDTH_462_12),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                                       SWOSD_FRAMERATE_STR_LEN);
                strBuff462_UV[streamId][2] =
                    (char *) (strBuff462_Y[streamId][2] +
                              (SWOSD_FRAMERATE_STR_LEN *
                               SWOSD_CHARWIDTH_462_12 *
                               SWOSD_CHARHEIGHT_462_12));
                SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][2],
                                    strBuff462_UV[streamId][2],
                                    SWOSD_WINDOW_FIVE);
                strcpy(frameRate_str[streamId], frameRateStr);
            }

            if (strcmp(bitRateStr, codecBitRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff462_Y1;
                fontInfo.fontAddrUV = fontBuff462_UV1;
                fontInfo.fontDB_Y = fontDB462_Y1;
                fontInfo.fontDB_UV = fontDB462_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr,
                                       strBuff462_Y[streamId][3] +
                                       (bitratePrefix * SWOSD_CHARWIDTH_462_12),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                                       bitrateLen);
                strBuff462_UV[streamId][3] =
                    (char *) (strBuff462_Y[streamId][3] +
                              (bitrateLen * SWOSD_CHARWIDTH_462_12 *
                               SWOSD_CHARHEIGHT_462_12));
                SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][3],
                                    strBuff462_UV[streamId][3],
                                    SWOSD_WINDOW_SIX);
                strcpy(codecBitRate_str[streamId], bitRateStr);
            }

            if (strcmp(autoFocusStr, autoFocus_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff462_Y1;
                fontInfo.fontAddrUV = fontBuff462_UV1;
                fontInfo.fontDB_Y = fontDB462_Y1;
                fontInfo.fontDB_UV = fontDB462_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                                       strBuff462_Y[streamId][8] +
                                       (SWOSD_AUTOFOCUS_PREFIX *
                                        SWOSD_CHARWIDTH_462_12),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                                       SWOSD_AUTOFOCUS_STR_LEN);
                strBuff462_UV[streamId][8] =
                    (char *) (strBuff462_Y[streamId][8] +
                              (SWOSD_AUTOFOCUS_STR_LEN *
                               SWOSD_CHARWIDTH_462_12 *
                               SWOSD_CHARHEIGHT_462_12));
                SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][8],
                                    strBuff462_UV[streamId][8],
                                    SWOSD_WINDOW_TEN);
                strcpy(autoFocus_str[streamId], autoFocusStr);
            }
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdType
                                                           //
        // ==
        // SWOSD_DETAIL)

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff462_Y;
        fontInfo.fontAddrUV = fontBuff462_UV;
        fontInfo.fontDB_Y = fontDB462_Y;
        fontInfo.fontDB_UV = fontDB462_UV;
#else
        fontInfo.fontAddr = fontBuff462_Y1;
        fontInfo.fontAddrUV = fontBuff462_UV1;
        fontInfo.fontDB_Y = fontDB462_Y1;
        fontInfo.fontDB_UV = fontDB462_UV1;
#endif

        if (gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate == OSD_YES)
        {
            if (gSWOSD_config.swosdConfig[streamId].swosdDatePos ==
                SWOSD_FMT_BOTTOM_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               3 * SWOSD_CHARHEIGHT_462_12);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_462_12);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_462_12);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_462_12), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_462_12);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_462_12), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_462_12);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_462_12);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_462_12), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 3 * SWOSD_CHARHEIGHT_462_12);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_462_12), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_462_12);
                }
            }

            gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate = OSD_NO;
        }

        if (gSWOSD_config.swosdConfig[streamId].swosdDateEnable)
        {
            if (strcmp(dateStr, date_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, dateStr,
                                       strBuff462_Y[streamId][5] +
                                       (SWOSD_DATE_PREFIX *
                                        SWOSD_CHARWIDTH_462_12),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                                       SWOSD_DATE_STR_LEN);
                strBuff462_UV[streamId][5] =
                    (char *) (strBuff462_Y[streamId][5] +
                              (SWOSD_DATE_STR_LEN * SWOSD_CHARWIDTH_462_12 *
                               SWOSD_CHARHEIGHT_462_12));
                SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][5],
                                    strBuff462_UV[streamId][5],
                                    SWOSD_WINDOW_EIGHT);
                strcpy(date_str[streamId], dateStr);
            }
        }

        if (gSWOSD_config.swosdConfig[streamId].swosdTimeEnable)
        {
            if (strcmp(timeStr, time_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, timeStr,
                                       strBuff462_Y[streamId][6] +
                                       (SWOSD_TIME_PREFIX *
                                        SWOSD_CHARWIDTH_462_12),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                                       SWOSD_TIME_STR_LEN);
                strBuff462_UV[streamId][6] =
                    (char *) (strBuff462_Y[streamId][6] +
                              (SWOSD_TIME_STR_LEN * SWOSD_CHARWIDTH_462_12 *
                               SWOSD_CHARHEIGHT_462_12));
                SWOSD_setBmpWinAddr(osdHndl, strBuff462_Y[streamId][6],
                                    strBuff462_UV[streamId][6],
                                    SWOSD_WINDOW_NINE);
                strcpy(time_str[streamId], timeStr);
            }
        }
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
             VIDEO_RESOL_WIDTH_MEDIUM)
    {
        if (gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate ==
            OSD_YES)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_1078_28;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_1078_28;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1078_28;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1078_28;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1078_28;

            fontInfo.fontAddr = fontBuff1078_Y1;
            fontInfo.fontAddrUV = fontBuff1078_UV1;
            fontInfo.fontDB_Y = fontDB1078_Y1;
            fontInfo.fontDB_UV = fontDB1078_UV1;

            if (gSWOSD_config.swosdConfig[streamId].swosdTextPos ==
                SWOSD_FMT_TOP_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0,
                                               SWOSD_CHARHEIGHTTILOGO_160_64);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_160_64, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0, 0);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_1078_28),
                                                           4), 0);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_160_64, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_1078_28),
                                                           4),
                                               SWOSD_CHARHEIGHTTILOGO_160_64);
                }
            }

            SWOSD_MakeOsdwinstring(&fontInfo,
                                   gSWOSD_config.
                                   swosdConfig[streamId].swosdDispString,
                                   strBuff1078_Y[streamId][7],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO],
                                   strlen(gSWOSD_config.
                                          swosdConfig
                                          [streamId].swosdDispString));
            strBuff1078_UV[streamId][7] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][7],
                                strBuff1078_UV[streamId][7], SWOSD_WINDOW_TWO);
            gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate = OSD_NO;
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate
                                                           //
        // == OSD_YES)
		
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_1078_28;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_1078_28;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1078_28;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1078_28;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1078_28;

        fontInfo.fontAddr = fontBuff1078_Y1;
        fontInfo.fontAddrUV = fontBuff1078_UV1;
        fontInfo.fontDB_Y = fontDB1078_Y1;
        fontInfo.fontDB_UV = fontDB1078_UV1;		
		
        /* VA string */
		SWOSD_MakeOsdwinstring(&fontInfo,
							  gSWOSD_config.swosdConfig[streamId].swosdVaString,
							  strBuff1078_Y[streamId][10],
							  &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE],
							  strlen(gSWOSD_config.swosdConfig[streamId].swosdVaString));
								   
        strBuff1078_UV[streamId][10] = (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE].bmpWinAddrUV);		
        SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][10],strBuff1078_UV[streamId][10], SWOSD_WINDOW_TWELVE);		

        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_770_20;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_770_20;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_770_20;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_770_20;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_770_20;

        if (gSWOSD_config.swosdConfig[streamId].swosdType == SWOSD_DETAIL)
        {
            if (strcmp(frameRateStr, frameRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff770_Y1;
                fontInfo.fontAddrUV = fontBuff770_UV1;
                fontInfo.fontDB_Y = fontDB770_Y1;
                fontInfo.fontDB_UV = fontDB770_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, frameRateStr,
                                       strBuff770_Y[streamId][2] +
                                       (SWOSD_FRAMERATE_PREFIX *
                                        SWOSD_CHARWIDTH_770_20),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                                       SWOSD_FRAMERATE_STR_LEN);
                strBuff770_UV[streamId][2] =
                    (char *) (strBuff770_Y[streamId][2] +
                              (SWOSD_FRAMERATE_STR_LEN *
                               SWOSD_CHARWIDTH_770_20 *
                               SWOSD_CHARHEIGHT_770_20));
                SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][2],
                                    strBuff770_UV[streamId][2],
                                    SWOSD_WINDOW_FIVE);
                strcpy(frameRate_str[streamId], frameRateStr);
            }

            if (strcmp(bitRateStr, codecBitRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff770_Y1;
                fontInfo.fontAddrUV = fontBuff770_UV1;
                fontInfo.fontDB_Y = fontDB770_Y1;
                fontInfo.fontDB_UV = fontDB770_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr,
                                       strBuff770_Y[streamId][3] +
                                       (bitratePrefix * SWOSD_CHARWIDTH_770_20),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                                       bitrateLen);
                strBuff770_UV[streamId][3] =
                    (char *) (strBuff770_Y[streamId][3] +
                              (bitrateLen * SWOSD_CHARWIDTH_770_20 *
                               SWOSD_CHARHEIGHT_770_20));
                SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][3],
                                    strBuff770_UV[streamId][3],
                                    SWOSD_WINDOW_SIX);
                strcpy(codecBitRate_str[streamId], bitRateStr);
            }

            if (strcmp(autoFocusStr, autoFocus_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff770_Y1;
                fontInfo.fontAddrUV = fontBuff770_UV1;
                fontInfo.fontDB_Y = fontDB770_Y1;
                fontInfo.fontDB_UV = fontDB770_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                                       strBuff770_Y[streamId][8] +
                                       (SWOSD_AUTOFOCUS_PREFIX *
                                        SWOSD_CHARWIDTH_770_20),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                                       SWOSD_AUTOFOCUS_STR_LEN);
                strBuff770_UV[streamId][8] =
                    (char *) (strBuff770_Y[streamId][8] +
                              (SWOSD_AUTOFOCUS_STR_LEN *
                               SWOSD_CHARWIDTH_770_20 *
                               SWOSD_CHARHEIGHT_770_20));
                SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][8],
                                    strBuff770_UV[streamId][8],
                                    SWOSD_WINDOW_TEN);
                strcpy(autoFocus_str[streamId], autoFocusStr);
            }
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdType
                                                           //
        // ==
        // SWOSD_DETAIL)

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff770_Y;
        fontInfo.fontAddrUV = fontBuff770_UV;
        fontInfo.fontDB_Y = fontDB770_Y;
        fontInfo.fontDB_UV = fontDB770_UV;
#else
        fontInfo.fontAddr = fontBuff770_Y1;
        fontInfo.fontAddrUV = fontBuff770_UV1;
        fontInfo.fontDB_Y = fontDB770_Y1;
        fontInfo.fontDB_UV = fontDB770_UV1;
#endif

        if (gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate == OSD_YES)
        {
            if (gSWOSD_config.swosdConfig[streamId].swosdDatePos ==
                SWOSD_FMT_BOTTOM_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               3 * SWOSD_CHARHEIGHT_770_20);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_770_20);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_770_20);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_770_20), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_770_20);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_770_20), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_770_20);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_770_20);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_770_20), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 3 * SWOSD_CHARHEIGHT_770_20);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_770_20), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_770_20);
                }
            }

            gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate = OSD_NO;
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate
                                                           //
        // == OSD_YES)

        if (gSWOSD_config.swosdConfig[streamId].swosdDateEnable)
        {
            if (strcmp(dateStr, date_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, dateStr,
                                       strBuff770_Y[streamId][5] +
                                       (SWOSD_DATE_PREFIX *
                                        SWOSD_CHARWIDTH_770_20),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                                       SWOSD_DATE_STR_LEN);
                strBuff770_UV[streamId][5] =
                    (char *) (strBuff770_Y[streamId][5] +
                              (SWOSD_DATE_STR_LEN * SWOSD_CHARWIDTH_770_20 *
                               SWOSD_CHARHEIGHT_770_20));
                SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][5],
                                    strBuff770_UV[streamId][5],
                                    SWOSD_WINDOW_EIGHT);
                strcpy(date_str[streamId], dateStr);
            }
        }

        if (gSWOSD_config.swosdConfig[streamId].swosdTimeEnable)
        {
            if (strcmp(timeStr, time_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, timeStr,
                                       strBuff770_Y[streamId][6] +
                                       (SWOSD_TIME_PREFIX *
                                        SWOSD_CHARWIDTH_770_20),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                                       SWOSD_TIME_STR_LEN);
                strBuff770_UV[streamId][6] =
                    (char *) (strBuff770_Y[streamId][6] +
                              (SWOSD_TIME_STR_LEN * SWOSD_CHARWIDTH_770_20 *
                               SWOSD_CHARHEIGHT_770_20));
                SWOSD_setBmpWinAddr(osdHndl, strBuff770_Y[streamId][6],
                                    strBuff770_UV[streamId][6],
                                    SWOSD_WINDOW_NINE);
                strcpy(time_str[streamId], timeStr);
            }
        }
    }                                                      // else
    // if(gSWOSD_config.swosdConfig[streamId].cropWidth
    // <=
    // VIDEO_RESOL_WIDTH_MEDIUM)
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
             VIDEO_RESOL_WIDTH_LARGE)
    {
        if (gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate ==
            OSD_YES)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_1232_30;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_1232_30;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1232_30;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1232_30;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1232_30;
			
            fontInfo.fontAddr = fontBuff1232_Y1;
            fontInfo.fontAddrUV = fontBuff1232_UV1;
            fontInfo.fontDB_Y = fontDB1232_Y1;
            fontInfo.fontDB_UV = fontDB1232_UV1;

            if (gSWOSD_config.swosdConfig[streamId].swosdTextPos ==
                SWOSD_FMT_TOP_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0,
                                               SWOSD_CHARHEIGHTTILOGO_160_64);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_160_64, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0, 0);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_1232_30),
                                                           4), 0);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_160_64, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_1232_30),
                                                           4),
                                               SWOSD_CHARHEIGHTTILOGO_160_64);
                }
            }

            SWOSD_MakeOsdwinstring(&fontInfo,
                                   gSWOSD_config.
                                   swosdConfig[streamId].swosdDispString,
                                   strBuff1232_Y[streamId][7],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO],
                                   strlen(gSWOSD_config.
                                          swosdConfig
                                          [streamId].swosdDispString));
            strBuff1232_UV[streamId][7] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][7],
                                strBuff1232_UV[streamId][7], SWOSD_WINDOW_TWO);
            gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate = OSD_NO;
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate
                                                           //
        // == OSD_YES)
		
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_1232_30;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_1232_30;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1232_30;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1232_30;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1232_30;
			
        fontInfo.fontAddr = fontBuff1232_Y1;
        fontInfo.fontAddrUV = fontBuff1232_UV1;
        fontInfo.fontDB_Y = fontDB1232_Y1;
        fontInfo.fontDB_UV = fontDB1232_UV1;		
		
        /* VA string */
		SWOSD_MakeOsdwinstring(&fontInfo,
							  gSWOSD_config.swosdConfig[streamId].swosdVaString,
							  strBuff1232_Y[streamId][10],
							  &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE],
							  strlen(gSWOSD_config.swosdConfig[streamId].swosdVaString));
								   
        strBuff1232_UV[streamId][10] = (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][10],strBuff1232_UV[streamId][10], SWOSD_WINDOW_TWELVE);		

        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_1078_28;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_1078_28;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1078_28;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1078_28;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1078_28;

        if (gSWOSD_config.swosdConfig[streamId].swosdType == SWOSD_DETAIL)
        {
            if (strcmp(frameRateStr, frameRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff1078_Y1;
                fontInfo.fontAddrUV = fontBuff1078_UV1;
                fontInfo.fontDB_Y = fontDB1078_Y1;
                fontInfo.fontDB_UV = fontDB1078_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, frameRateStr,
                                       strBuff1078_Y[streamId][2] +
                                       (SWOSD_FRAMERATE_PREFIX *
                                        SWOSD_CHARWIDTH_1078_28),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                                       SWOSD_FRAMERATE_STR_LEN);
                strBuff1078_UV[streamId][2] =
                    (char *) (strBuff1078_Y[streamId][2] +
                              (SWOSD_FRAMERATE_STR_LEN *
                               SWOSD_CHARWIDTH_1078_28 *
                               SWOSD_CHARHEIGHT_1078_28));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][2],
                                    strBuff1078_UV[streamId][2],
                                    SWOSD_WINDOW_FIVE);
                strcpy(frameRate_str[streamId], frameRateStr);
            }

            if (strcmp(bitRateStr, codecBitRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff1078_Y1;
                fontInfo.fontAddrUV = fontBuff1078_UV1;
                fontInfo.fontDB_Y = fontDB1078_Y1;
                fontInfo.fontDB_UV = fontDB1078_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr,
                                       strBuff1078_Y[streamId][3] +
                                       (bitratePrefix *
                                        SWOSD_CHARWIDTH_1078_28),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                                       bitrateLen);
                strBuff1078_UV[streamId][3] =
                    (char *) (strBuff1078_Y[streamId][3] +
                              (bitrateLen * SWOSD_CHARWIDTH_1078_28 *
                               SWOSD_CHARHEIGHT_1078_28));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][3],
                                    strBuff1078_UV[streamId][3],
                                    SWOSD_WINDOW_SIX);
                strcpy(codecBitRate_str[streamId], bitRateStr);
            }

            if (strcmp(autoFocusStr, autoFocus_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff1078_Y1;
                fontInfo.fontAddrUV = fontBuff1078_UV1;
                fontInfo.fontDB_Y = fontDB1078_Y1;
                fontInfo.fontDB_UV = fontDB1078_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                                       strBuff1078_Y[streamId][8] +
                                       (SWOSD_AUTOFOCUS_PREFIX *
                                        SWOSD_CHARWIDTH_1078_28),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                                       SWOSD_AUTOFOCUS_STR_LEN);
                strBuff1078_UV[streamId][8] =
                    (char *) (strBuff1078_Y[streamId][8] +
                              (SWOSD_AUTOFOCUS_STR_LEN *
                               SWOSD_CHARWIDTH_1078_28 *
                               SWOSD_CHARHEIGHT_1078_28));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][8],
                                    strBuff1078_UV[streamId][8],
                                    SWOSD_WINDOW_TEN);
                strcpy(autoFocus_str[streamId], autoFocusStr);
            }
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdType
                                                           //
        // ==
        // SWOSD_DETAIL)

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff1078_Y;
        fontInfo.fontAddrUV = fontBuff1078_UV;
        fontInfo.fontDB_Y = fontDB1078_Y;
        fontInfo.fontDB_UV = fontDB1078_UV;
#else
        fontInfo.fontAddr = fontBuff1078_Y1;
        fontInfo.fontAddrUV = fontBuff1078_UV1;
        fontInfo.fontDB_Y = fontDB1078_Y1;
        fontInfo.fontDB_UV = fontDB1078_UV1;
#endif

        if (gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate == OSD_YES)
        {
            if (gSWOSD_config.swosdConfig[streamId].swosdDatePos ==
                SWOSD_FMT_BOTTOM_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               3 * SWOSD_CHARHEIGHT_1078_28);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_1078_28);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_1078_28);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1078_28), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_1078_28);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1078_28), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_1078_28);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_1078_28);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1078_28), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 3 * SWOSD_CHARHEIGHT_1078_28);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1078_28), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_1078_28);
                }
            }

            gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate = OSD_NO;
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate
                                                           //
        // == OSD_YES)

        if (gSWOSD_config.swosdConfig[streamId].swosdDateEnable)
        {
            if (strcmp(dateStr, date_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, dateStr,
                                       strBuff1078_Y[streamId][5] +
                                       (SWOSD_DATE_PREFIX *
                                        SWOSD_CHARWIDTH_1078_28),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                                       SWOSD_DATE_STR_LEN);
                strBuff1078_UV[streamId][5] =
                    (char *) (strBuff1078_Y[streamId][5] +
                              (SWOSD_DATE_STR_LEN * SWOSD_CHARWIDTH_1078_28 *
                               SWOSD_CHARHEIGHT_1078_28));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][5],
                                    strBuff1078_UV[streamId][5],
                                    SWOSD_WINDOW_EIGHT);
                strcpy(date_str[streamId], dateStr);
            }
        }

        if (gSWOSD_config.swosdConfig[streamId].swosdTimeEnable)
        {
            if (strcmp(timeStr, time_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, timeStr,
                                       strBuff1078_Y[streamId][6] +
                                       (SWOSD_TIME_PREFIX *
                                        SWOSD_CHARWIDTH_1078_28),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                                       SWOSD_TIME_STR_LEN);
                strBuff1078_UV[streamId][6] =
                    (char *) (strBuff1078_Y[streamId][6] +
                              (SWOSD_TIME_STR_LEN * SWOSD_CHARWIDTH_1078_28 *
                               SWOSD_CHARHEIGHT_1078_28));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1078_Y[streamId][6],
                                    strBuff1078_UV[streamId][6],
                                    SWOSD_WINDOW_NINE);
                strcpy(time_str[streamId], timeStr);
            }
        }
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth >=
             VIDEO_RESOL_WIDTH_LARGE)
    {
        if (gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate ==
            OSD_YES)
        {
            fontInfo.format = SWOSD_FORMATYUV420p;
            fontInfo.charWidth = SWOSD_CHARWIDTH_1540_40;
            fontInfo.charHeight = SWOSD_CHARHEIGHT_1540_40;
            fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1540_40;
            fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1540_40;
            fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1540_40;
			
            fontInfo.fontAddr = fontBuff1540_Y1;
            fontInfo.fontAddrUV = fontBuff1540_UV1;
            fontInfo.fontDB_Y = fontDB1540_Y1;
            fontInfo.fontDB_UV = fontDB1540_UV1;

            if (gSWOSD_config.swosdConfig[streamId].swosdTextPos ==
                SWOSD_FMT_TOP_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0,
                                               SWOSD_CHARHEIGHTTILOGO_160_64);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_160_64, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO, 0, 0);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdLogoPos ==
                    SWOSD_FMT_TOP_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE, 0, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_1540_40),
                                                           4), 0);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ONE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_CHARWIDTHTILOGO_160_64, 0);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWO,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((strlen
                                                            (gSWOSD_config.swosdConfig
                                                             [streamId].swosdDispString)
                                                            *
                                                            SWOSD_CHARWIDTH_1540_40),
                                                           4),
                                               SWOSD_CHARHEIGHTTILOGO_160_64);
                }
            }

            SWOSD_MakeOsdwinstring(&fontInfo,
                                   gSWOSD_config.
                                   swosdConfig[streamId].swosdDispString,
                                   strBuff1540_Y[streamId][0],
                                   &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO],
                                   strlen(gSWOSD_config.
                                          swosdConfig
                                          [streamId].swosdDispString));
            strBuff1540_UV[streamId][0] =
                (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWO].bmpWinAddrUV);
            SWOSD_setBmpWinAddr(osdHndl, strBuff1540_Y[streamId][0],
                                strBuff1540_UV[streamId][0], SWOSD_WINDOW_TWO);
            gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate = OSD_NO;
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdLogoStringUpdate
                                                           //
        // == OSD_YES)

        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_1540_40;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_1540_40;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1540_40;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1540_40;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1540_40;
			
        fontInfo.fontAddr = fontBuff1540_Y1;
        fontInfo.fontAddrUV = fontBuff1540_UV1;
        fontInfo.fontDB_Y = fontDB1540_Y1;
        fontInfo.fontDB_UV = fontDB1540_UV1;		
		
        /* VA string */
		SWOSD_MakeOsdwinstring(&fontInfo,
							  gSWOSD_config.swosdConfig[streamId].swosdVaString,
							  strBuff1540_Y[streamId][1],
							  &osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE],
							  strlen(gSWOSD_config.swosdConfig[streamId].swosdVaString));
								   
        strBuff1540_UV[streamId][1] = (char *) (osdHndl->bmpWinPrm[SWOSD_WINDOW_TWELVE].bmpWinAddrUV);
        SWOSD_setBmpWinAddr(osdHndl, strBuff1540_Y[streamId][1],strBuff1540_UV[streamId][1], SWOSD_WINDOW_TWELVE);		
		
        fontInfo.format = SWOSD_FORMATYUV420p;
        fontInfo.charWidth = SWOSD_CHARWIDTH_1232_30;
        fontInfo.charHeight = SWOSD_CHARHEIGHT_1232_30;
        fontInfo.totFontWidth = SWOSD_STRINGWIDTH_1232_30;
        fontInfo.totFontHeight = SWOSD_CHARHEIGHT_1232_30;
        fontInfo.totFontLineoffset = SWOSD_STRINGWIDTH_1232_30;

        if (gSWOSD_config.swosdConfig[streamId].swosdType == SWOSD_DETAIL)
        {
            if (strcmp(frameRateStr, frameRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff1232_Y1;
                fontInfo.fontAddrUV = fontBuff1232_UV1;
                fontInfo.fontDB_Y = fontDB1232_Y1;
                fontInfo.fontDB_UV = fontDB1232_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, frameRateStr,
                                       strBuff1232_Y[streamId][2] +
                                       (SWOSD_FRAMERATE_PREFIX *
                                        SWOSD_CHARWIDTH_1232_30),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_FIVE],
                                       SWOSD_FRAMERATE_STR_LEN);
                strBuff1232_UV[streamId][2] =
                    (char *) (strBuff1232_Y[streamId][2] +
                              (SWOSD_FRAMERATE_STR_LEN *
                               SWOSD_CHARWIDTH_1232_30 *
                               SWOSD_CHARHEIGHT_1232_30));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][2],
                                    strBuff1232_UV[streamId][2],
                                    SWOSD_WINDOW_FIVE);
                strcpy(frameRate_str[streamId], frameRateStr);
            }

            if (strcmp(bitRateStr, codecBitRate_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff1232_Y1;
                fontInfo.fontAddrUV = fontBuff1232_UV1;
                fontInfo.fontDB_Y = fontDB1232_Y1;
                fontInfo.fontDB_UV = fontDB1232_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, bitRateStr,
                                       strBuff1232_Y[streamId][3] +
                                       (bitratePrefix *
                                        SWOSD_CHARWIDTH_1232_30),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_SIX],
                                       bitrateLen);
                strBuff1232_UV[streamId][3] =
                    (char *) (strBuff1232_Y[streamId][3] +
                              (bitrateLen * SWOSD_CHARWIDTH_1232_30 *
                               SWOSD_CHARHEIGHT_1232_30));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][3],
                                    strBuff1232_UV[streamId][3],
                                    SWOSD_WINDOW_SIX);
                strcpy(codecBitRate_str[streamId], bitRateStr);
            }

            if (strcmp(autoFocusStr, autoFocus_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff1232_Y1;
                fontInfo.fontAddrUV = fontBuff1232_UV1;
                fontInfo.fontDB_Y = fontDB1232_Y1;
                fontInfo.fontDB_UV = fontDB1232_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, autoFocusStr,
                                       strBuff1232_Y[streamId][8] +
                                       (SWOSD_AUTOFOCUS_PREFIX *
                                        SWOSD_CHARWIDTH_1232_30),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_TEN],
                                       SWOSD_AUTOFOCUS_STR_LEN);
                strBuff1232_UV[streamId][8] =
                    (char *) (strBuff1232_Y[streamId][8] +
                              (SWOSD_AUTOFOCUS_STR_LEN *
                               SWOSD_CHARWIDTH_1232_30 *
                               SWOSD_CHARHEIGHT_1232_30));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][8],
                                    strBuff1232_UV[streamId][8],
                                    SWOSD_WINDOW_TEN);
                strcpy(autoFocus_str[streamId], autoFocusStr);
            }

            if (strcmp(AEWBStr, AEWB_str[streamId]) != 0)
            {
                fontInfo.fontAddr = fontBuff1232_Y1;
                fontInfo.fontAddrUV = fontBuff1232_UV1;
                fontInfo.fontDB_Y = fontDB1232_Y1;
                fontInfo.fontDB_UV = fontDB1232_UV1;

                SWOSD_MakeOsdwinstring(&fontInfo, AEWBStr,
                                       strBuff1232_Y[streamId][11] +
                                       (SWOSD_AEWB_PREFIX *
                                        SWOSD_CHARWIDTH_1232_30),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_THIRTEEN],
                                       SWOSD_AEWB_STR_LEN);
                strBuff1232_UV[streamId][11] =
                    (char *) (strBuff1232_Y[streamId][11] +
                              (SWOSD_AEWB_STR_LEN*
                               SWOSD_CHARWIDTH_1232_30 *
                               SWOSD_CHARHEIGHT_1232_30));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][11],
                                    strBuff1232_UV[streamId][11],
                                    SWOSD_WINDOW_THIRTEEN);
                strcpy(AEWB_str[streamId], AEWBStr);
            }
        }                                                  // if(gSWOSD_config.swosdConfig[streamId].swosdType
                                                           //
        // ==
        // SWOSD_DETAIL)

#ifdef SWOSD_USER_TRANSPARANCY_ON
        fontInfo.fontAddr = fontBuff1232_Y;
        fontInfo.fontAddrUV = fontBuff1232_UV;
        fontInfo.fontDB_Y = fontDB1232_Y;
        fontInfo.fontDB_UV = fontDB1232_UV;
#else
        fontInfo.fontAddr = fontBuff1232_Y1;
        fontInfo.fontAddrUV = fontBuff1232_UV1;
        fontInfo.fontDB_Y = fontDB1232_Y1;
        fontInfo.fontDB_UV = fontDB1232_UV1;
#endif

        if (gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate == OSD_YES)
        {
            if (gSWOSD_config.swosdConfig[streamId].swosdDatePos ==
                SWOSD_FMT_BOTTOM_LEFT)
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               3 * SWOSD_CHARHEIGHT_1232_30);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_1232_30);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_1232_30);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1232_30), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_1232_30);
                }
            }
            else
            {
                if (gSWOSD_config.swosdConfig[streamId].swosdTimePos ==
                    SWOSD_FMT_BOTTOM_LEFT)
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1232_30), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_1232_30);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE, 0,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropHeight -
                                               2 * SWOSD_CHARHEIGHT_1232_30);
                }
                else
                {
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_EIGHT,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1232_30), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 3 * SWOSD_CHARHEIGHT_1232_30);
                    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_NINE,
                                               gSWOSD_config.swosdConfig
                                               [streamId].cropWidth -
                                               SWOSD_align((SWOSD_MAX_DATE_TIME_LENGHT * SWOSD_CHARWIDTH_1232_30), 4), gSWOSD_config.swosdConfig[streamId].cropHeight - 2 * SWOSD_CHARHEIGHT_1232_30);
                }
            }

            gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate = OSD_NO;
        }

        if (gSWOSD_config.swosdConfig[streamId].swosdDateEnable)
        {
            if (strcmp(dateStr, date_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, dateStr,
                                       strBuff1232_Y[streamId][5] +
                                       (SWOSD_DATE_PREFIX *
                                        SWOSD_CHARWIDTH_1232_30),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_EIGHT],
                                       SWOSD_DATE_STR_LEN);
                strBuff1232_UV[streamId][5] =
                    (char *) (strBuff1232_Y[streamId][5] +
                              (SWOSD_DATE_STR_LEN * SWOSD_CHARWIDTH_1232_30 *
                               SWOSD_CHARHEIGHT_1232_30));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][5],
                                    strBuff1232_UV[streamId][5],
                                    SWOSD_WINDOW_EIGHT);
                strcpy(date_str[streamId], dateStr);
            }
        }

        if (gSWOSD_config.swosdConfig[streamId].swosdTimeEnable)
        {
            if (strcmp(timeStr, time_str[streamId]) != 0)
            {
                SWOSD_MakeOsdwinstring(&fontInfo, timeStr,
                                       strBuff1232_Y[streamId][6] +
                                       (SWOSD_TIME_PREFIX *
                                        SWOSD_CHARWIDTH_1232_30),
                                       &osdHndl->bmpWinPrm[SWOSD_WINDOW_NINE],
                                       SWOSD_TIME_STR_LEN);
                strBuff1232_UV[streamId][6] =
                    (char *) (strBuff1232_Y[streamId][6] +
                              (SWOSD_TIME_STR_LEN * SWOSD_CHARWIDTH_1232_30 *
                               SWOSD_CHARHEIGHT_1232_30));
                SWOSD_setBmpWinAddr(osdHndl, strBuff1232_Y[streamId][6],
                                    strBuff1232_UV[streamId][6],
                                    SWOSD_WINDOW_NINE);
                strcpy(time_str[streamId], timeStr);
            }
        }
    }

    return SWOSD_SOK;
}

/* change the XY parameters */

/* ===================================================================
 *  @func     DM81XX_SWOSD_changeXYParmDetail
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
static int DM81XX_SWOSD_changeXYParmDetail(int streamId, SWOSD_Hndl * osdHndl)
{
    int width = gSWOSD_config.swosdConfig[streamId].cropWidth / 2;

    int height = gSWOSD_config.swosdConfig[streamId].cropHeight / 2;

    int osdwidth = 0, osdheight = 0;

    if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
        VIDEO_RESOL_WIDTH_SMALL)
    {
        osdwidth = SWOSD_CHARWIDTH_462_12;
        osdheight = SWOSD_CHARHEIGHT_462_12;
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth <=
             VIDEO_RESOL_WIDTH_MEDIUM)
    {
        osdwidth = SWOSD_CHARWIDTH_770_20;
        osdheight = SWOSD_CHARHEIGHT_770_20;
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth <
             VIDEO_RESOL_WIDTH_LARGE)
    {
        osdwidth = SWOSD_CHARWIDTH_1078_28;
        osdheight = SWOSD_CHARHEIGHT_1078_28;
    }
    else if (gSWOSD_config.swosdConfig[streamId].cropWidth >=
             VIDEO_RESOL_WIDTH_LARGE)
    {
        osdwidth = SWOSD_CHARWIDTH_1232_30;
        osdheight = SWOSD_CHARHEIGHT_1232_30;
    }

    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_THREE, 0,
                               height - 2 * osdheight);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_FOUR, 0,
                               height - 1 * osdheight);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_FIVE, 0,
                               height + 0 * osdheight);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_SIX, 0,
                               height + 1 * osdheight);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_SEVEN, 0,
                               height + 2 * osdheight);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TEN, 0,
                               height + 3 * osdheight);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_THIRTEEN, 0,
                               height + 4 * osdheight);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_ELEVEN,
                               (width - (SWOSD_EVAL_STR_LEN * osdwidth) / 2),
                               0);
    SWOSD_setBmpchangeWinXYPrm(osdHndl, SWOSD_WINDOW_TWELVE,
                               (width - (strlen(gSWOSD_config.swosdConfig[streamId].swosdVaString) * osdwidth) / 2),
                               (height << 1) - 2 * osdheight);
    
	return SWOSD_SOK;
}

/* Check if EVAL VERSION string is to be displayed */

/* ===================================================================
 *  @func     DM81XX_SWOSD_checkEvalVersion
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
static int DM81XX_SWOSD_checkEvalVersion(int streamId)
{
    int osdEvalFlag = 0;

    if (gSWOSD_config.swosdConfig[streamId].swosdEvalEnable)
        osdEvalFlag = 1;
    else
        osdEvalFlag = 0;

    return osdEvalFlag;
}

/* SW OSD window update */

/* ===================================================================
 *  @func     DM81XX_SWOSD_display
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
static int DM81XX_SWOSD_display(int streamId, SWOSD_Hndl * osdHndl)
{
	DM81XX_SWOSD_makeWinString(streamId, osdHndl);
    DM81XX_SWOSD_changeXYParmDetail(streamId, osdHndl);

    if (gSWOSD_config.swosdConfig[streamId].swosdLogoEnable)
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_ONE, SWOSD_BMPWINENABLE);
    else
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_ONE, SWOSD_BMPWINDISABLE);

    if (gSWOSD_config.swosdConfig[streamId].swosdTextEnable)
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_TWO, SWOSD_BMPWINENABLE);
    else
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_TWO, SWOSD_BMPWINDISABLE);

    if (gSWOSD_config.swosdConfig[streamId].swosdType == SWOSD_DETAIL)
    {
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_THREE, SWOSD_BMPWINENABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_FOUR, SWOSD_BMPWINENABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_FIVE, SWOSD_BMPWINENABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_SIX, SWOSD_BMPWINENABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_SEVEN, SWOSD_BMPWINENABLE);
        SWOSD_setBmpWinEnable(osdHndl,SWOSD_WINDOW_TEN,SWOSD_BMPWINENABLE);
        SWOSD_setBmpWinEnable(osdHndl,SWOSD_WINDOW_THIRTEEN,SWOSD_BMPWINENABLE);
        //SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_TEN, SWOSD_BMPWINDISABLE);
    }
    else
    {
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_THREE, SWOSD_BMPWINDISABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_FOUR, SWOSD_BMPWINDISABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_FIVE, SWOSD_BMPWINDISABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_SIX, SWOSD_BMPWINDISABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_SEVEN, SWOSD_BMPWINDISABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_TEN, SWOSD_BMPWINDISABLE);
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_THIRTEEN, SWOSD_BMPWINDISABLE);
    }

    if (gSWOSD_config.swosdConfig[streamId].swosdDateEnable)
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_EIGHT, SWOSD_BMPWINENABLE);
    else
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_EIGHT, SWOSD_BMPWINDISABLE);

    if (gSWOSD_config.swosdConfig[streamId].swosdTimeEnable)
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_NINE, SWOSD_BMPWINENABLE);
    else
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_NINE, SWOSD_BMPWINDISABLE);

    if (DM81XX_SWOSD_checkEvalVersion(streamId))
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_ELEVEN, SWOSD_BMPWINENABLE);
    else
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_ELEVEN,
                              SWOSD_BMPWINDISABLE);

    if (gSWOSD_config.swosdConfig[streamId].vaStrEnable)					
        SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_TWELVE, SWOSD_BMPWINENABLE);	
	else
	    SWOSD_setBmpWinEnable(osdHndl, SWOSD_WINDOW_TWELVE, SWOSD_BMPWINDISABLE);	
							  
    return SWOSD_SOK;
}

/* SWOSD create function */

/* ===================================================================
 *  @func     DM81XX_SWOSD_create
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
int DM81XX_SWOSD_create(SWOSD_config_t * pSwOsdPrm)
{
    int i, j;

    SWOSD_Hndl *osdHndl = NULL;

    /* copy the SW OSD params to the global structure */
    memcpy(&gSWOSD_config, pSwOsdPrm, sizeof(SWOSD_config_t));
	
	System_getEdmaCh(&gSWOSD_config.edmaHndl);
	System_getEdmaCh(&gDrawEdmaHndl);
	
    /* initialize the SWOSD lib */
	if (SWOSD_init(gSWOSD_config.edmaHndl.dmaHndl.chId, SWOSD_EDMA_QUEUEID) != 0)
    {
        SWOSD_ERROR("SWOSD_ERROR:SWOSD_init Failure\n");
        return SWOSD_EFAIL;
    }

    /* Allocate buffer for LOGO and set the LOGO parameters */
    iconBuff160_Y =
        (char *)
        SWOSD_ALLOC((SWOSD_CHARWIDTHTILOGO_160_64 *
                     SWOSD_CHARHEIGHTTILOGO_160_64), 32);
    iconBuff160_UV =
        (char *)
        SWOSD_ALLOC(((SWOSD_CHARWIDTHTILOGO_160_64 *
                      SWOSD_CHARHEIGHTTILOGO_160_64) / 2), 32);
    iconBuff80_Y =
        (char *)
        SWOSD_ALLOC((SWOSD_CHARWIDTHTILOGO_80_32 *
                     SWOSD_CHARHEIGHTTILOGO_80_32), 32);
    iconBuff80_UV =
        (char *)
        SWOSD_ALLOC(((SWOSD_CHARWIDTHTILOGO_80_32 *
                      SWOSD_CHARHEIGHTTILOGO_80_32) / 2), 32);

    DM81XX_SWOSD_setLogoPrm();

    /* Allocate the buffers for holding the display strings for each stream */
    for (i = 0; i < gSWOSD_config.numCaptureStream; i++)
    {
        strBuff1540_Y[i][0] =
            (char *)
            SWOSD_ALLOC((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1540_40 *
                         SWOSD_CHARHEIGHT_1540_40 * 3) / 2, 32);
        strBuff1540_Y[i][1] =
            (char *)
            SWOSD_ALLOC((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1540_40 *
                         SWOSD_CHARHEIGHT_1540_40 * 3) / 2, 32);
        for (j = 0; j < MAX_STR_BUFF; j++)
        {
            strBuff1232_Y[i][j] =
                (char *)
                SWOSD_ALLOC((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1232_30 *
                             SWOSD_CHARHEIGHT_1232_30 * 3) / 2, 32);
            strBuff1078_Y[i][j] =
                (char *)
                SWOSD_ALLOC((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1078_28 *
                             SWOSD_CHARHEIGHT_1078_28 * 3) / 2, 32);
            strBuff770_Y[i][j] =
                (char *)
                SWOSD_ALLOC((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_770_20 *
                             SWOSD_CHARHEIGHT_770_20 * 3) / 2, 32);
            strBuff462_Y[i][j] =
                (char *)
                SWOSD_ALLOC((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_462_12 *
                             SWOSD_CHARHEIGHT_462_12 * 3) / 2, 32);
        }                                                  // for(j = 0; j <
        // MAX_STR_BUFF;
        // j++)
    }                                                      // for(i = 0; i <
    // gSWOSD_config.numCaptureStream;
    // i++)

    /* Allocate buffers for the fonts */
#ifdef SWOSD_USER_TRANSPARANCY_ON
    fontBuff1540_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1540_40 * SWOSD_CHARHEIGHT_1540_40), 32);
    fontBuff1540_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1540_40 *
                     (SWOSD_CHARHEIGHT_1540_40 / 2)), 32);
    memcpy(fontBuff1540_Y1, YData_1540_40_WFONT,
           SWOSD_STRINGWIDTH_1540_40 * SWOSD_CHARHEIGHT_1540_40);
    memcpy(fontBuff1540_UV1, UVData_1540_40_WFONT,
           SWOSD_STRINGWIDTH_1540_40 * (SWOSD_CHARHEIGHT_1540_40 / 2));

    fontBuff1232_Y =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30), 32);
    fontBuff1232_UV =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1232_30 *
                     (SWOSD_CHARHEIGHT_1232_30 / 2)), 32);
    memcpy(fontBuff1232_Y, YData_1232_30,
           SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30);
    memcpy(fontBuff1232_UV, UVData_1232_30,
           SWOSD_STRINGWIDTH_1232_30 * (SWOSD_CHARHEIGHT_1232_30 / 2));

    fontBuff1232_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30), 32);
    fontBuff1232_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1232_30 *
                     (SWOSD_CHARHEIGHT_1232_30 / 2)), 32);
    memcpy(fontBuff1232_Y1, YData_1232_30_WFONT,
           SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30);
    memcpy(fontBuff1232_UV1, UVData_1232_30_WFONT,
           SWOSD_STRINGWIDTH_1232_30 * (SWOSD_CHARHEIGHT_1232_30 / 2));

    fontBuff1078_Y =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28), 32);
    fontBuff1078_UV =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1078_28 *
                     (SWOSD_CHARHEIGHT_1078_28 / 2)), 32);
    memcpy(fontBuff1078_Y, YData_1078_28,
           SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28);
    memcpy(fontBuff1078_UV, UVData_1078_28,
           SWOSD_STRINGWIDTH_1078_28 * (SWOSD_CHARHEIGHT_1078_28 / 2));

    fontBuff1078_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28), 32);
    fontBuff1078_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1078_28 *
                     (SWOSD_CHARHEIGHT_1078_28 / 2)), 32);
    memcpy(fontBuff1078_Y1, YData_1078_28_WFONT,
           SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28);
    memcpy(fontBuff1078_UV1, UVData_1078_28_WFONT,
           SWOSD_STRINGWIDTH_1078_28 * (SWOSD_CHARHEIGHT_1078_28 / 2));

    fontBuff770_Y =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20), 32);
    fontBuff770_UV =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2)),
                    32);
    memcpy(fontBuff770_Y, YData_770_20,
           SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20);
    memcpy(fontBuff770_UV, UVData_770_20,
           SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2));

    fontBuff770_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20), 32);
    fontBuff770_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2)),
                    32);
    memcpy(fontBuff770_Y1, YData_770_20_WFONT,
           SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20);
    memcpy(fontBuff770_UV1, UVData_770_20_WFONT,
           SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2));

    fontBuff462_Y =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12), 32);
    fontBuff462_UV =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2)),
                    32);
    memcpy(fontBuff462_Y, YData_462_12,
           SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12);
    memcpy(fontBuff462_UV, UVData_462_12,
           SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2));

    fontBuff462_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12), 32);
    fontBuff462_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2)),
                    32);
    memcpy(fontBuff462_Y1, YData_462_12_WFONT,
           SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12);
    memcpy(fontBuff462_UV1, UVData_462_12_WFONT,
           SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2));
#else
    fontBuff1540_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1540_40 * SWOSD_CHARHEIGHT_1540_40), 32);
    fontBuff1540_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1540_40 *
                     (SWOSD_CHARHEIGHT_1540_40 / 2)), 32);
    memcpy(fontBuff1540_Y1, YData_1540_40_WFONT_PURE,
           SWOSD_STRINGWIDTH_1540_40 * SWOSD_CHARHEIGHT_1540_40);
    memcpy(fontBuff1540_UV1, UVData_1540_40_WFONT_PURE,
           SWOSD_STRINGWIDTH_1540_40 * (SWOSD_CHARHEIGHT_1540_40 / 2));

    fontBuff1232_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30), 32);
    fontBuff1232_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1232_30 *
                     (SWOSD_CHARHEIGHT_1232_30 / 2)), 32);
    memcpy(fontBuff1232_Y1, YData_1232_30_WFONT_PURE,
           SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30);
    memcpy(fontBuff1232_UV1, UVData_1232_30_WFONT_PURE,
           SWOSD_STRINGWIDTH_1232_30 * (SWOSD_CHARHEIGHT_1232_30 / 2));

    fontBuff1078_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28), 32);
    fontBuff1078_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_1078_28 *
                     (SWOSD_CHARHEIGHT_1078_28 / 2)), 32);
    memcpy(fontBuff1078_Y1, YData_1078_28_WFONT_PURE,
           SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28);
    memcpy(fontBuff1078_UV1, UVData_1078_28_WFONT_PURE,
           SWOSD_STRINGWIDTH_1078_28 * (SWOSD_CHARHEIGHT_1078_28 / 2));

    fontBuff770_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20), 32);
    fontBuff770_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2)),
                    32);
    memcpy(fontBuff770_Y1, YData_770_20_WFONT_PURE,
           SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20);
    memcpy(fontBuff770_UV1, UVData_770_20_WFONT_PURE,
           SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2));

    fontBuff462_Y1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12), 32);
    fontBuff462_UV1 =
        (char *)
        SWOSD_ALLOC((SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2)),
                    32);
    memcpy(fontBuff462_Y1, YData_462_12_WFONT_PURE,
           SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12);
    memcpy(fontBuff462_UV1, UVData_462_12_WFONT_PURE,
           SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2));
#endif                                                     // #ifdef
    // SWOSD_USER_TRANSPARANCY_ON

    /* create font DB from the font buffers */
    DM81XX_SWOSD_createFontDB();

    /* create SWOSD handles for each stream */
    for (i = 0; i < gSWOSD_config.numCaptureStream; i++)
    {
        if (SWOSD_createHandle(&osdHndl, &idxHndl[i]) != SUCCESS)
        {
            SWOSD_ERROR("SWOSD_ERROR:Handle Creation Failed in SWOSD(%d)\n", i);
            return SWOSD_EFAIL;
        }
        else
        {
            SWOSD_setNumWindows(osdHndl, SWOSD_WINMAX);
            DM81XX_SWOSD_setWinParam(osdHndl,
                                     gSWOSD_config.swosdConfig[i].cropHeight,
                                     gSWOSD_config.swosdConfig[i].cropWidth);
            DM81XX_SWOSD_makeFixWinString(osdHndl, i);
        }

        gSWOSD_config.swosdConfig[i].swosdHndl = osdHndl;
    }

    return SWOSD_SOK;
}

/* DM81XX SW OSD Reset */

/* ===================================================================
 *  @func     DM81XX_SWOSD_Reset
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
int DM81XX_SWOSD_Reset(int streamId,int pitch0,int pitch1,int width,int height)
{
	SWOSD_Hndl *osdHndl;

	if (streamId < gSWOSD_config.numCaptureStream)
	{
        if((pitch0 == gSWOSD_config.swosdConfig[streamId].capLineOffsetY) &&
		   (pitch1 == gSWOSD_config.swosdConfig[streamId].capLineOffsetUV) &&
		   (width  == gSWOSD_config.swosdConfig[streamId].cropWidth) &&
		   (height == gSWOSD_config.swosdConfig[streamId].cropHeight))
		{
            return SWOSD_SOK;
		}		
	
	    gSWOSD_config.swosdConfig[streamId].capLineOffsetY      = pitch0;
		gSWOSD_config.swosdConfig[streamId].capLineOffsetUV     = pitch1;
		gSWOSD_config.swosdConfig[streamId].capWidth            = width;
	    gSWOSD_config.swosdConfig[streamId].capHeight           = height;
	    gSWOSD_config.swosdConfig[streamId].cropWidth           = width;
		gSWOSD_config.swosdConfig[streamId].cropHeight          = height;
		gSWOSD_config.swosdConfig[streamId].swosdDateTimeUpdate = OSD_YES;
		
		strcpy(codecBitRate_str[streamId],"");
		strcpy(frameRate_str[streamId],"");
		strcpy(date_str[streamId],"");
		strcpy(time_str[streamId],"");

		osdHndl = gSWOSD_config.swosdConfig[streamId].swosdHndl;		
	    SWOSD_setNumWindows(osdHndl, SWOSD_WINMAX);
		DM81XX_SWOSD_setWinParam(osdHndl,
	                             gSWOSD_config.swosdConfig[streamId].cropHeight,
	                             gSWOSD_config.swosdConfig[streamId].cropWidth);
        DM81XX_SWOSD_makeFixWinString(osdHndl, streamId);								 
	}

	return SWOSD_SOK;
}

/* DM81XX SW OSD delete */

/* ===================================================================
 *  @func     DM81XX_SWOSD_delete
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
int DM81XX_SWOSD_delete(void)
{
    int i, j;

    /* free the allocated memory */

    /* LOGO buffers */
    if (iconBuff160_Y != NULL)
        SWOSD_FREE(iconBuff160_Y,
                   (SWOSD_CHARWIDTHTILOGO_160_64 *
                    SWOSD_CHARHEIGHTTILOGO_160_64));

    if (iconBuff160_UV != NULL)
        SWOSD_FREE(iconBuff160_UV,
                   ((SWOSD_CHARWIDTHTILOGO_160_64 *
                     SWOSD_CHARHEIGHTTILOGO_160_64) / 2));

    if (iconBuff80_Y != NULL)
        SWOSD_FREE(iconBuff80_Y,
                   (SWOSD_CHARWIDTHTILOGO_80_32 *
                    SWOSD_CHARHEIGHTTILOGO_80_32));

    if (iconBuff80_UV != NULL)
        SWOSD_FREE(iconBuff80_UV,
                   ((SWOSD_CHARWIDTHTILOGO_80_32 *
                     SWOSD_CHARHEIGHTTILOGO_80_32) / 2));

    /* display string buffers */
    for (i = 0; i < gSWOSD_config.numCaptureStream; i++)
    {
        if (strBuff1540_Y[i][0] != NULL)
            SWOSD_FREE(strBuff1540_Y[i][0],
                       ((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1540_40 *
                         SWOSD_CHARHEIGHT_1540_40 * 3) / 2));
        if (strBuff1540_Y[i][1] != NULL)
            SWOSD_FREE(strBuff1540_Y[i][1],
                       ((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1540_40 *
                         SWOSD_CHARHEIGHT_1540_40 * 3) / 2));
        for (j = 0; j < MAX_STR_BUFF; j++)
        {
            if (strBuff1232_Y[i][j] != NULL)
                SWOSD_FREE(strBuff1232_Y[i][j],
                           ((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1232_30 *
                             SWOSD_CHARHEIGHT_1232_30 * 3) / 2));

            if (strBuff1078_Y[i][j] != NULL)
                SWOSD_FREE(strBuff1078_Y[i][j],
                           ((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_1078_28 *
                             SWOSD_CHARHEIGHT_1078_28 * 3) / 2));

            if (strBuff770_Y[i][j] != NULL)
                SWOSD_FREE(strBuff770_Y[i][j],
                           ((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_770_20 *
                             SWOSD_CHARHEIGHT_770_20 * 3) / 2));

            if (strBuff462_Y[i][j] != NULL)
                SWOSD_FREE(strBuff462_Y[i][j],
                           ((SWOSD_MAX_STRING_LENGTH * SWOSD_CHARWIDTH_462_12 *
                             SWOSD_CHARHEIGHT_462_12 * 3) / 2));
        }
		
		strcpy(codecBitRate_str[i],"");
		strcpy(frameRate_str[i],"");
    }

    /* font buffers */
#ifdef SWOSD_USER_TRANSPARANCY_ON
    if (fontBuff1540_Y1 != NULL)
        SWOSD_FREE(fontBuff1540_Y1,
                   (SWOSD_STRINGWIDTH_1540_40 * SWOSD_CHARHEIGHT_1540_40));

    if (fontBuff1540_UV1 != NULL)
        SWOSD_FREE(fontBuff1540_UV1,
                   (SWOSD_STRINGWIDTH_1540_40 *
                    (SWOSD_CHARHEIGHT_1540_40 / 2)));

    if (fontBuff1232_Y != NULL)
        SWOSD_FREE(fontBuff1232_Y,
                   (SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30));

    if (fontBuff1232_UV != NULL)
        SWOSD_FREE(fontBuff1232_UV,
                   (SWOSD_STRINGWIDTH_1232_30 *
                    (SWOSD_CHARHEIGHT_1232_30 / 2)));

    if (fontBuff1232_Y1 != NULL)
        SWOSD_FREE(fontBuff1232_Y1,
                   (SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30));

    if (fontBuff1232_UV1 != NULL)
        SWOSD_FREE(fontBuff1232_UV1,
                   (SWOSD_STRINGWIDTH_1232_30 *
                    (SWOSD_CHARHEIGHT_1232_30 / 2)));

    if (fontBuff1078_Y != NULL)
        SWOSD_FREE(fontBuff1078_Y,
                   (SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28));

    if (fontBuff1078_UV != NULL)
        SWOSD_FREE(fontBuff1078_UV,
                   (SWOSD_STRINGWIDTH_1078_28 *
                    (SWOSD_CHARHEIGHT_1078_28 / 2)));

    if (fontBuff1078_Y1 != NULL)
        SWOSD_FREE(fontBuff1078_Y1,
                   (SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28));

    if (fontBuff1078_UV1 != NULL)
        SWOSD_FREE(fontBuff1078_UV1,
                   (SWOSD_STRINGWIDTH_1078_28 *
                    (SWOSD_CHARHEIGHT_1078_28 / 2)));

    if (fontBuff770_Y != NULL)
        SWOSD_FREE(fontBuff770_Y,
                   (SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20));

    if (fontBuff770_UV != NULL)
        SWOSD_FREE(fontBuff770_UV,
                   (SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2)));

    if (fontBuff770_Y1 != NULL)
        SWOSD_FREE(fontBuff770_Y1,
                   (SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20));

    if (fontBuff770_UV1 != NULL)
        SWOSD_FREE(fontBuff770_UV1,
                   (SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2)));

    if (fontBuff462_Y != NULL)
        SWOSD_FREE(fontBuff462_Y,
                   (SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12));

    if (fontBuff462_UV != NULL)
        SWOSD_FREE(fontBuff462_UV,
                   (SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2)));

    if (fontBuff462_Y1 != NULL)
        SWOSD_FREE(fontBuff462_Y1,
                   (SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12));

    if (fontBuff462_UV1 != NULL)
        SWOSD_FREE(fontBuff462_UV1,
                   (SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2)));
#else
    if (fontBuff1540_Y1 != NULL)
        SWOSD_FREE(fontBuff1540_Y1,
                   (SWOSD_STRINGWIDTH_1540_40 * SWOSD_CHARHEIGHT_1540_40));

    if (fontBuff1540_UV1 != NULL)
        SWOSD_FREE(fontBuff1540_UV1,
                   (SWOSD_STRINGWIDTH_1540_40 *
                    (SWOSD_CHARHEIGHT_1540_40 / 2)));

    if (fontBuff1232_Y1 != NULL)
        SWOSD_FREE(fontBuff1232_Y1,
                   (SWOSD_STRINGWIDTH_1232_30 * SWOSD_CHARHEIGHT_1232_30));

    if (fontBuff1232_UV1 != NULL)
        SWOSD_FREE(fontBuff1232_UV1,
                   (SWOSD_STRINGWIDTH_1232_30 *
                    (SWOSD_CHARHEIGHT_1232_30 / 2)));

    if (fontBuff1078_Y1 != NULL)
        SWOSD_FREE(fontBuff1078_Y1,
                   (SWOSD_STRINGWIDTH_1078_28 * SWOSD_CHARHEIGHT_1078_28));

    if (fontBuff1078_UV1 != NULL)
        SWOSD_FREE(fontBuff1078_UV1,
                   (SWOSD_STRINGWIDTH_1078_28 *
                    (SWOSD_CHARHEIGHT_1078_28 / 2)));

    if (fontBuff770_Y1 != NULL)
        SWOSD_FREE(fontBuff770_Y1,
                   (SWOSD_STRINGWIDTH_770_20 * SWOSD_CHARHEIGHT_770_20));

    if (fontBuff770_UV1 != NULL)
        SWOSD_FREE(fontBuff770_UV1,
                   (SWOSD_STRINGWIDTH_770_20 * (SWOSD_CHARHEIGHT_770_20 / 2)));

    if (fontBuff462_Y1 != NULL)
        SWOSD_FREE(fontBuff462_Y1,
                   (SWOSD_STRINGWIDTH_462_12 * SWOSD_CHARHEIGHT_462_12));

    if (fontBuff462_UV1 != NULL)
        SWOSD_FREE(fontBuff462_UV1,
                   (SWOSD_STRINGWIDTH_462_12 * (SWOSD_CHARHEIGHT_462_12 / 2)));
#endif                                                     // #ifdef
    // SWOSD_USER_TRANSPARANCY_ON

    // SWOSD exit
    SWOSD_exit();
	
	System_putEdmaCh(&gSWOSD_config.edmaHndl);
	System_putEdmaCh(&gDrawEdmaHndl);

    return SWOSD_SOK;
}

/* DM81XX SWOSD apply */

/* ===================================================================
 *  @func     DM81XX_SWOSD_apply
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
int DM81XX_SWOSD_apply(int streamId, int frameBufAddrY, int frameBufAddrUV)
{
    SWOSD_Hndl *osdHndl;

    SWOSD_MainWinPrm mainWinPrm;

    if (streamId < gSWOSD_config.numCaptureStream)
    {
        if (gSWOSD_config.swosdConfig[streamId].swosdEnable)
        {
            osdHndl =
                (SWOSD_Hndl *) gSWOSD_config.swosdConfig[streamId].swosdHndl;

            /* Set main window parameters */
            mainWinPrm.format = gSWOSD_config.captureYuvFormat;
            mainWinPrm.lineOffsetY =
                gSWOSD_config.swosdConfig[streamId].capLineOffsetY;
            mainWinPrm.lineOffsetUV =
                gSWOSD_config.swosdConfig[streamId].capLineOffsetUV;				
            mainWinPrm.width = gSWOSD_config.swosdConfig[streamId].capWidth;
            mainWinPrm.height = gSWOSD_config.swosdConfig[streamId].capHeight;

            SWOSD_setMainWinPrm(osdHndl, &mainWinPrm);

            /* SW OSD windows are updated once in a second */
            if (gSWOSD_config.swosdConfig[streamId].osd_count >=
                gSWOSD_config.swosdConfig[streamId].frameRateBase)
            {
				DM81XX_SWOSD_display(streamId, osdHndl);	
#ifdef IMX_BASED_SWOSD_ENABLED
                SWOSD_collateWindows(streamId, osdHndl);					
#endif				
                gSWOSD_config.swosdConfig[streamId].osd_count = 0;
				
				if(gSWOSD_config.swosdConfig[streamId].vaDisFlag == 0)
				{
				    gSWOSD_config.swosdConfig[streamId].vaStrEnable = 0;
				}
            }
            else
            { 
                gSWOSD_config.swosdConfig[streamId].osd_count++;
            }
 
			if(gSWOSD_config.swosdConfig[streamId].swosdTransparencyEnable)								
				/* Draw the SW OSD windows */
				SWOSD_imxDrawHandle(osdHndl, (short *) frameBufAddrY,
									(short *) frameBufAddrUV, streamId);
			else
				/* Draw the SW OSD windows */
				SWOSD_winDrawHandle(osdHndl, (short *) frameBufAddrY,
									(short *) frameBufAddrUV);
        }
    }                                                      // if(streamId <
    // gSWOSD_config.numCaptureStream)

    return SWOSD_SOK;
}

/* DM81XX SWOSD Draw Histogram Lines */

/* ===================================================================
 *  @func     DM812X_SWOSD_drawHistLines
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
int DM812X_SWOSD_drawHistLines(DRAW_InFrame_Info_t *pInFrameInfo,int *pHistDataAddr)
{
    DRAW_Line_Info_t lineInfo;
	int i,histHeight,clip_value;
	
	clip_value = pInFrameInfo->videoHeight >> 2;
	
	lineInfo.colorY     = 0xFF;  
	lineInfo.colorC     = 0x80;
	lineInfo.vertLine   = TRUE;
	lineInfo.thickness  = HIST_LINE_THICKNESS;
	lineInfo.startX     = 16;	
	
	for(i = 0; i < 256; i = i + lineInfo.thickness)
	{
		histHeight = (*(pHistDataAddr + i) >> 5);
		
		if(histHeight > clip_value)
		{
		    histHeight = clip_value;
		}

		lineInfo.startX += lineInfo.thickness;
		lineInfo.startY = pInFrameInfo->videoHeight - 16 - histHeight;
		lineInfo.length = histHeight;
		
		DM812X_DRAW_line(pInFrameInfo, &lineInfo);
	}
	
    return SWOSD_SOK;
}

/* DM81XX SWOSD Draw Histogram */

/* ===================================================================
 *  @func     DM81XX_SWOSD_drawHistogram
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
int DM81XX_SWOSD_drawHistogram(int streamId,int frameBufAddrY,int frameBufAddrUV,int *pHistDataAddr)
{
    DRAW_InFrame_Info_t inFrameInfo;
	DRAW_Rect_Info_t rectInfo;
	int frameWidth,frameHeight,framePitch;
	int frameDataFormat;
	
	/* Drawing histogram only on the first stream */
	if((streamId == 0) && (gSWOSD_config.swosdConfig[streamId].histEnable == 1))
	{
		frameWidth  = gSWOSD_config.swosdConfig[streamId].capWidth;
		frameHeight = gSWOSD_config.swosdConfig[streamId].capHeight; 
		framePitch  = gSWOSD_config.swosdConfig[streamId].capLineOffsetY;
		
		if(gSWOSD_config.captureYuvFormat == SWOSD_FORMATYUV422i)
		{
		    frameDataFormat = FVID2_DF_YUV422I_UYVY;
		}
		else
		{
		    frameDataFormat = FVID2_DF_YUV420SP_UV;
		}

		/* input frame info */
		inFrameInfo.videoInOutAddrY  = (UInt8 *) frameBufAddrY;
		inFrameInfo.videoInOutAddrUV = (UInt8 *) frameBufAddrUV;
		inFrameInfo.videoWidth       = frameWidth;
		inFrameInfo.videoHeight      = frameHeight;
		inFrameInfo.videoOffsetH     = framePitch;
		inFrameInfo.videoOffsetV     = frameHeight;
		inFrameInfo.videoDataFormat  = frameDataFormat;
		inFrameInfo.maskEnable       = FALSE;	
		
		rectInfo.colorY      = 0xFF;
		rectInfo.colorC      = 0x80;
		rectInfo.thickness   = 1;		
		rectInfo.startX  	 = 16 + HIST_LINE_THICKNESS;
		rectInfo.startY  	 = frameHeight - 16 - (frameHeight >> 2);
		rectInfo.width   	 = 256;
		rectInfo.height  	 = frameHeight >> 2;
		
		/* Draw Histogram rectangle */
		DM812X_DRAW_rectangle(&inFrameInfo,&rectInfo,framePitch);
		
		/* Draw histogram lines */
		DM812X_SWOSD_drawHistLines(&inFrameInfo,pHistDataAddr);
	}	
	
	return SWOSD_SOK;
}
