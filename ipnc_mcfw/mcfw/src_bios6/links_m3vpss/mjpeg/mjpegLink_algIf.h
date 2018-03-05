/** ==================================================================
 *  @file  mjpegLink_algIf.h
 *
 *  @path   $(IPNC_PATH)/ipnc_mcfw/mcfw/src_bios6/links_m3vpss/mjpeg
 *
 *  @desc   This  File contains all the definitions of  data-
 *          structure used in the mjpeg link of create and dynamic
 *          parameters of the link.
 *  =====================================================================
 *  Copyright (c) Texas Instruments Inc 2012, 2011
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied    
 * ===================================================================*/

#ifndef _MJPEG_LINK_ALG_IF_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MJPEG_LINK_ALG_IF_H_

#include <mcfw/interfaces/link_api/mjpegLink.h>

#define MJPEG_LINK_H264_VERSION_STRING_MAX_LEN  (255)

typedef struct MjpegLink_AlgCreateParams {
    Bool singleBuf;
    Int32 maxWidth;
    Int32 maxHeight;
    Int32 inputContentType;
    Int32 inputChromaFormat;
    Int32 profile;
    Int32 level;
    Bool tilerEnable;
} MjpegLink_AlgCreateParams;

typedef struct MjpegLink_AlgDynamicParams {
    Int32 startX;
    Int32 startY;
    Int32 inputWidth;
    Int32 inputHeight;
    Int32 targetFrameRate;
    Int32 refFrameRate;
    Int32 mjpegQp;
    Int32 skipFrameFactor;
} MjpegLink_AlgDynamicParams;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _MJPEG_LINK_ALG_IF_H_ 
                                                            */
