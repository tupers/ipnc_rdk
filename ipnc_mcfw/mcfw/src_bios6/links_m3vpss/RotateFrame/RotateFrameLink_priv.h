
/** ==================================================================
 *  @file   imgLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_c6xdsp/imgproc/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/rotateFrameLink.h>
#include <mcfw/src_bios6/utils/utils_dma.h>

#define ROTATEFRAME_LINK_EDMA3_QUEUE_ID			(0)
#define ROTATEFRAME_LINK_OBJ_MAX						(1)
#define ROTATEFRAME_LINK_MAX_OUT_FRAMES			(SYSTEM_LINK_FRAMES_PER_CH)
#define ROTATEFRAME_LINK_DSP_FREQ_MHZ			(500)


typedef struct {
    Utils_TskHndl tsk;
    SystemCommon_edmaChPrm copyEdmaHndl;
    
    RotateFrameLink_CreateParams createArgs;

    FVID2_Frame outFrames[ROTATEFRAME_LINK_MAX_OUT_FRAMES];
    FVID2_Format bufferFrameFormat;	
    System_FrameInfo freeFrameInfoMem[ROTATEFRAME_LINK_MAX_OUT_FRAMES];
    System_LinkInfo info;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;
    Utils_BufHndl outFrameQue;

	unsigned char isNeedRotateFrame;
} RotateFrameLink_Obj;

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
