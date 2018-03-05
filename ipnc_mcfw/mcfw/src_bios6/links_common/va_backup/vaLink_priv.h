/** ==================================================================
 *  @file   vaLink_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_c6xdsp/va/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _VA_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _VA_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/vaLink.h>
#include <time.h>

#ifdef IPNC_DSP_ON
	#include <mcfw/src_bios6/links_c6xdsp/alg/dmva/inc/DMVAL.h>
#else
	#include <mcfw/src_bios6/links_m3vpss/alg/dmva/inc/DMVAL.h>
	#include <ti/psp/iss/alg/rm/inc/rm.h>
#endif

#define VA_LINK_OBJ_MAX         		(1)
#define VA_LINK_MAX_OUT_FRAMES  		(SYSTEM_LINK_FRAMES_PER_CH)

#define VA_LINK_EDMA3_QUEUE_ID			(0)

#define VA_LINK_INFRAMERATE				(30)
#define VA_LINK_DETECTIONRATE			(3)

#define VA_LINK_MINPERSONWIDTH			(10)	// in percent
#define VA_LINK_MINPERSONHEIGHT			(10)	// in percent
#define VA_LINK_MINVEHICLEWIDTH			(10)	// in percent
#define VA_LINK_MINVEHICLEHEIGHT		(10)	// in percent
#define VA_LINK_MAXPERSONWIDTH			(20)	// in percent
#define VA_LINK_MAXPERSONHEIGHT			(20)	// in percent
#define VA_LINK_MAXVEHICLEWIDTH			(20)	// in percent
#define VA_LINK_MAXVEHICLEHEIGHT		(20)	// in percent

#define VA_LINK_EVT_RECTIME_MSEC		(5 * 1000)	// 5 seconds

#define VA_LINK_DEFAULT_FOCUSVALUE		(0x7FFF)

#define VA_LINK_EDMA_BASE_ADDR			(0x49000000)

#define VA_LINK_START_CNT				(10)

typedef struct {
    Int16 bufId;

} VaLink_perFrameConfig;

typedef struct {
	Int16 inFrameRate;
	UInt32 totalProcessTime;
	UInt32 processFrCnt;
	UInt32 maxProcessTime;
	UInt32 startCnt;

} VaLink_algObj;

typedef struct {
	UInt32 tskId;
	Utils_TskHndl tsk;
	Utils_TskHndl processTsk;
	VaLink_CreateParams createArgs;
	UInt32 receviedFrameCount;
	System_LinkInfo info;
	System_LinkInfo inTskInfo;
	System_LinkQueInfo inQueInfo;
	FVID2_Frame outFrames[VA_LINK_MAX_OUT_FRAMES];
	VaLink_perFrameConfig perFrameCfg[VA_LINK_MAX_OUT_FRAMES];
	FVID2_Format bufferFrameFormat;
	Utils_BufHndl outFrameQue;
	System_FrameInfo frameInfo[VA_LINK_MAX_OUT_FRAMES];
	VaLink_algObj algObj;
	VaLink_GuiParams guiPrm;
	VaLink_GuiParams prevGuiPrm;
	UInt32 inFrameRate;

	SystemCommon_edmaChPrm copyEdmaHndl;
	SystemCommon_edmaChPrm algEdmaHndl;

#ifndef IPNC_DSP_ON
	Void *pRMHandle;
#endif

} VaLink_Obj;

/* Function prototypes */

void DM81XX_EDMA3_setParams(int chId, int dmaQueue, unsigned int srcAddr,
                            unsigned int dstAddr, unsigned short edmaWidth,
                            unsigned short edmaHeight, short srcLineOffset,
                            short dstLineOffset);

void DM81XX_EDMA3_triggerTransfer(int chId);

/* ===================================================================
 *  @func     VaLink_drvCreateFvidObj
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
Int32 VaLink_algCreate(VaLink_Obj * pObj);

Int32 VaLink_algCopyFrames(VaLink_Obj * pObj);
Int32 VaLink_algProcessFrames(VaLink_Obj * pObj);

/* ===================================================================
 *  @func     VaLink_drvDeleteFvidObj
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
Int32 VaLink_algDelete(VaLink_Obj * pObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
