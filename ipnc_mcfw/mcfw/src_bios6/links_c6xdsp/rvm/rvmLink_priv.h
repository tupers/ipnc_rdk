/** ==================================================================
 *  @file   rvmLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_c6xdsp/rvm/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _RVM_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _RVM_LINK_PRIV_H_

#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/rvmLink.h>
#include <mcfw/src_bios6/links_c6xdsp/alg/rvm/inc/RVM_wrapper.h>

#define RVM_LINK_OBJ_MAX				(1)
#define RVM_LINK_MAX_OUT_FRAMES			(SYSTEM_LINK_FRAMES_PER_CH)
#define RVM_LINK_SCRATCHMEM_SIZE		(32 * KB)	
#define RVM_LINK_PERIODIC_PRINT			(300)
#define RVM_LINK_LUT_SIZE				(32 * MB)
#define RVM_LINK_OUT_BLKSIZE			(32)
#define RVM_LINK_DSP_FREQ_MHZ			(500)

typedef struct {
    Utils_TskHndl tsk;
    RvmLink_CreateParams createArgs;

    FVID2_Frame outFrames[RVM_LINK_MAX_OUT_FRAMES];
    FVID2_Format bufferFrameFormat;	
	
    System_LinkInfo info;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;

    Utils_BufHndl outFrameQue;

	UInt8 *pLut;
	UInt32 totalTime;
	UInt32 frameCnt;
	
	UInt32 inWidth;
	UInt32 inHeight;
	UInt32 outWidth;
	UInt32 outHeight;
		
} RvmLink_Obj;
		
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
