/** ==================================================================
 *  @file   glbceSupportLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/glbceSupport/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _FRAMEPROCESS_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _FRAMEPROCESS_LINK_PRIV_H_

#include <stddef.h>
#include <stdio.h>
#include <osa.h>
#include <osa_que.h>
#include <osa_tsk.h>
#include <osa_debug.h>
#include <ti/ipc/SharedRegion.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/system_debug.h>

#include <mcfw/src_linux/links/system/system_priv_common.h>
#include <mcfw/src_linux/links/system/systemLink_priv.h>
#include <mcfw/interfaces/link_api/frameProcessHost.h>

#define FRAMEPROCESS_MAX_WIDTH				2048
#define FRAMEPROCESS_MAX_HEIGHT				1080
				
#define FRAMEPROCESS_LINK_OBJ_MAX				(1)
#define FRAMEPROCESS_LINK_MAX_FRAMES			5
/**
 *  @def   IPC_BITS_IN_LINK_S_SUCCESS
 *  @brief Define used to indicate successful execution
 */
#define FRAMEPROCESS_LINK_S_SUCCESS   		(0)
#define FRAMEPROCESS_LINK_S_FAIL   			(-1)
#include "FrameSave.h"

#define ALG_SHAREDREGION_ID			0

#define IMG_SAVE_INTER_MS		500

typedef struct FrameProcessLink_Obj {
    UInt32 tskId;

    OSA_TskHndl tsk;
	OSA_TskHndl frameSavetsk;
	OSA_TskHndl AlgApptsk;
    FrameProcessHostLink_CreateParams createArgs;
	VIDFrame_Buf FrameBuf[FRAMEPROCESS_LINK_MAX_FRAMES];
	FrameNet_Buf NetFrame;
	ErrFrame_Buf errBuf[FRAMEPROCESS_LINK_MAX_FRAMES];
    OSA_QueHndl outFrameBufQue;
    OSA_QueHndl freeFrameBufQue;
    OSA_QueHndl freeErrFrameBufQue;
    OSA_QueHndl outErrFrameBufQue;

	UInt8	fileName[50];
	UInt8	getFrameFlag;
	UInt8	encodeEnable;
	UInt8	saveErrImg;
    UInt8  *physAddr;
    UInt8  *virtAddr;
    UInt32 size;
    /* IpcFramesIn link info */
    System_LinkInfo info;
    System_LinkInfo inQueInfo;

	Ptr Algbuf;
    int qSerialId;
    int mAlgshmid;
    int AlgStartFlag;
    
    int memDevFd;
    
} FrameProcessLink_Obj;

Int32 FrameProcessHostLink_getFullFrames(OSA_TskHndl * pTsk, UInt16 queId,
                                   VIDFrame_BufList * pFrameBufList);

Int32 FrameProcessHostLink_putEmptyFrames(OSA_TskHndl * pTsk, UInt16 queId,
                                   VIDFrame_BufList * pFrameBufList);

Int32 FrameProcessLink_getFullFrame(FrameNet_Buf * pFrame);
Int32 FrameProcessLink_putEmptyFrame(FrameNet_Buf * pFrame);

Int32 FrameProcessLink_getErrFrame(FrameProcessLink_Obj *pObj,ErrFrame_Buf **pFrame);
Int32 FrameProcessLink_ReleaseErrFrame(FrameProcessLink_Obj *pObj,ErrFrame_Buf * pFrame);
/* Function prototypes */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
