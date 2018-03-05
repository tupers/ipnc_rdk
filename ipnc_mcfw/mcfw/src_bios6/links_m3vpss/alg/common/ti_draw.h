/** ==================================================================
 *  @file   facedetect_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/common/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _TI_DRAW_H_
#define _TI_DRAW_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>

#define DRAW_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )
#define DRAW_floor(value, align)   (( (value) / (align) ) * (align) )

#define DRAW_EDMA_QUEUEID_DRAW	(0)

/* DRAW input frame info */
typedef struct {
    UInt8 *videoInOutAddrY;
    UInt8 *videoInOutAddrUV;
    UInt32 videoWidth;
    UInt32 videoHeight;
    UInt32 videoOffsetH;
    UInt32 videoOffsetV;
    UInt32 videoDataFormat;
    UInt32 maskEnable;

} DRAW_InFrame_Info_t;

/* DRAW Line info */
typedef struct {
    UInt32 startX;
    UInt32 startY;
    UInt32 length;
    Bool vertLine;
    UInt32 colorY;
    UInt32 colorC;
    UInt32 thickness;

} DRAW_Line_Info_t;

/* FD Rectangle info */
typedef struct {
    UInt32 numFace;
    UInt32 startX;
    UInt32 startY;
    UInt32 width;
    UInt32 height;
    UInt32 colorY;
    UInt32 colorC;
    UInt32 thickness;

} DRAW_Rect_Info_t;
	
Int32 DM812X_DRAW_line(DRAW_InFrame_Info_t * pInFrameInfo,
                       DRAW_Line_Info_t * pLineInfo);
Int32 DM812X_DRAW_rectangle(DRAW_InFrame_Info_t * pInFrameInfo,
                            DRAW_Rect_Info_t * pRectInfo, Int32 pitch);
void DM81XX_EDMA3_memset(int chId, int dmaQueue, unsigned int setValue,
                         unsigned int dstAddr, unsigned short setWidth,
                         unsigned short setHeight, short lineOffset);							  

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     // _TI_DRAW_H_
