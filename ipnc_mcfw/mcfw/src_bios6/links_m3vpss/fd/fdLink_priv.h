/** ==================================================================
 *  @file   fdLink_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/fd/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _FD_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _FD_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/fdLink.h>
#include <mcfw/src_bios6/links_m3vpss/alg/face_detect/facedetect_priv.h>

#define FD_LINK_MAX_OUT_FRAMES			(SYSTEM_LINK_FRAMES_PER_CH)
#define FD_LINK_EDMA3_QUEUE_ID       	(0)
#define FD_LINK_MAX_WIDTH				(320)
#define FD_LINK_MAX_HEIGHT				(240)

typedef struct {
    Utils_TskHndl tsk;
    FdLink_CreateParams createArgs;

    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;

    FVID2_Frame outFrames[FD_LINK_MAX_OUT_FRAMES];
    FVID2_Format bufferFrameFormat;

    Utils_BufHndl outFrameQue;

    /* FD Alg handle */
    FD_ALG_Hndl_t algHndl;

	UInt32 minLatency;
    /* minimum capture/source to SW Mosaic output latency in msecs */

    UInt32 maxLatency;
    /* maximum capture/source to SW Mosaic output latency in msecs */

	SystemCommon_edmaChPrm edmaHndl;

} FdLink_Obj;

/* ===================================================================
 *  @func     DM81XX_EDMA3_setParams
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
void DM81XX_EDMA3_setParams(int chId,
                            int dmaQueue,
                            unsigned int srcAddr,
                            unsigned int dstAddr,
                            unsigned short edmaWidth,
                            unsigned short edmaHeight,
                            short srcLineOffset, short dstLineOffset);

/* ===================================================================
 *  @func     DM81XX_EDMA3_triggerTransfer
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
void DM81XX_EDMA3_triggerTransfer(int chId);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     // _FD_LINK_PRIV_H_
