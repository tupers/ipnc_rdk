/** ==================================================================
 *  @file   motiondetect_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/alg/motion_detection/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*
 * DM81XX Motion Detection Algotithm
 */

#ifndef _DM81XX_MD_ALG_H_
#define _DM81XX_MD_ALG_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

#include <mcfw/src_bios6/utils/utils_tsk.h>

#define MD_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )

#define ALG_MOTION_DETECT_HORZ_REGIONS  			(4)
#define ALG_MOTION_DETECT_VERT_REGIONS  			(3)

    // Following values are Sensitivity levels that can come from UI
#define ALG_MOTION_DETECT_SENSITIVITY_LOW           (0)
#define ALG_MOTION_DETECT_SENSITIVITY_MEDIUM        (1)
#define ALG_MOTION_DETECT_SENSITIVITY_HIGH          (2)

#define ALG_MOTION_DETECT_MAX_DETECT_CNT            (5)

    enum {
        ALG_MOTION_S_DETECT = 100,
        ALG_MOTION_S_NO_DETECT,

        ALG_MOTION_S_FAIL = -1,
        ALG_MOTION_S_OK = 0
    };

    /* MV and SAD structures */

    typedef struct {
        /* Starting position of data from the buffer base address */
        unsigned int StartPos;
        /* No. of bytes to jump from the current position to get the next
         * data of this element group */
        unsigned short Jump;
        /* Number of data elements in this group */
        unsigned int Count;

    } ElementInfo;

    typedef struct {
        unsigned int NumElements;
        ElementInfo elementInfoField0SAD;
        ElementInfo elementInfoField1SAD;
        ElementInfo elementInfoField0MVL0;
        ElementInfo elementInfoField0MVL1;
        ElementInfo elementInfoField1MVL0;
        ElementInfo elementInfoField1MVL1;

    } AnalyticHeaderInfo;

    typedef struct {
        unsigned int mbMvInfo;
        unsigned int mvJump;

        int ImageWidth;
        int ImageHeight;
        int windowWidth;
        int windowHeight;
        int isKeyFrame;
        int codecType;
        int isDateTimeDraw;
        char windowEnableMap[ALG_MOTION_DETECT_HORZ_REGIONS]
            [ALG_MOTION_DETECT_VERT_REGIONS];

    } MD_RunPrm_t;

    /* The following structure will store the information for motion detected
     * for a particular selected window */

    typedef struct {
        char windowMotionDetected[ALG_MOTION_DETECT_HORZ_REGIONS]
            [ALG_MOTION_DETECT_VERT_REGIONS];

    } MD_RunStatus_t;

    typedef struct {
        int frame_width;
        int frame_height;
        int frameCnt;
        int startCnt;
        int SAD_THRESHOLD;
        int threshold;
        int win_width;
        int win_height;
        int MvDataOffset;
        int warning_count;
        int detectCnt;

        int motionenable;
        int motioncenable;
        int motioncvalue;
        int motionlevel;
        int motionsens;
        int blockNum;

        MD_RunPrm_t runPrm;
        MD_RunStatus_t runStatus;

        Utils_TskHndl tskHndl;

        UInt32 sadBuf[2];
        MD_RunPrm_t mdRunPrm[2];
        Int32 curSadBuf;
        Int32 prevSadBuf;

    } MD_Hndl_t;

    int DM81XX_MD_Create();
    int DM81XX_MD_Apply(Uint32 mvBuf, Uint32 frameWidth, Uint32 frameHeight,
                        Uint32 isKeyFrame);
    int DM81XX_MD_Delete();

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _DM81XX_MD_ALG_H_
                                                            */
