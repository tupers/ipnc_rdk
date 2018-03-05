/** ==================================================================
 *  @file   encLink_h264_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/iva_enc/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ENC_LINK_MPEG4_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ENC_LINK_MPEG4_PRIV_H_

#include <mcfw/interfaces/link_api/encLink.h>
#include <impeg4enc.h>
#include "encLink_algIf.h"

/**
 *  Analytic info output buffer size, this buffer is used to place MV & SAD of
 *  encoded frame, should be big enough to hold the size of  typical HD sequence
*/
#define ANALYTICINFO_OUTPUT_BUFF_SIZE      0x00028000

#define ALG_VID_ENC_RATE_CONTROL_VBR    0
#define ALG_VID_ENC_RATE_CONTROL_CBR    1
#define ALG_VID_ENC_RATE_CONTROL_CVBR   2

#define ALG_VID_ENC_PRESET_AUTO     		0
#define ALG_VID_ENC_PRESET_HIGHSPEED   		1
#define ALG_VID_ENC_PRESET_HIGHQUALITY 		2
#define ALG_VID_ENC_PRESET_SVC_AUTO        	3

#define ALG_VID_ENC_ROI_PRIORITY_NONE      0
#define ALG_VID_ENC_ROI_PRIORITY_LOW       2
#define ALG_VID_ENC_ROI_PRIORITY_MEDIUM    5
#define ALG_VID_ENC_ROI_PRIORITY_HIGH      8

typedef struct EncLink_Mpeg4Obj {
    IMPEG4ENC_Handle algHandle;
    Int8 versionInfo[ENC_LINK_ALG_VERSION_STRING_MAX_LEN];
    Int linkID;
    Int channelID;
    Int scratchID;
    UInt32 ivaChID;
    IMPEG4ENC_DynamicParams dynamicParams;
    IMPEG4ENC_Status status;
    IMPEG4ENC_Params staticParams;
    IMPEG4ENC_InArgs inArgs;
    IMPEG4ENC_OutArgs outArgs;
    IVIDEO2_BufDesc inBufs;
    XDM2_BufDesc outBufs;
    IVIDEO_Format format;
    UInt32 memUsed[UTILS_MEM_MAXHEAPS];
} EncLink_Mpeg4Obj;

/* ===================================================================
 *  @func     EncLinkH264_algCreate
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
Int EncLinkMpeg4_algCreate(EncLink_Mpeg4Obj * hObj,
                          EncLink_AlgCreateParams * algCreateParams,
                          EncLink_AlgDynamicParams * algDynamicParams,
                          Int linkID, Int channelID, Int scratchGroupID);
/* ===================================================================
 *  @func     EncLinkH264_algDelete
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
Void EncLinkMpeg4_algDelete(EncLink_Mpeg4Obj * hObj);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ENC_LINK_PRIV_H_
                                                            */
