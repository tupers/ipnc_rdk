
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

#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/imgLink.h>
#include <SDS_Export.h>
#include <SDS_Dynamic_Params.h>
#include "Alg_drvMgr.h"

#define IMG_LINK_EDMA3_QUEUE_ID			(0)
#define IMG_LINK_OBJ_MAX				(1)
#define IMG_LINK_MAX_OUT_FRAMES			(SYSTEM_LINK_FRAMES_PER_CH)
#define IMG_LINK_DSP_FREQ_MHZ			(500)

/** img alg command */
#define SYSTEM_IMG_ALG_DONE					(0x200)
#define SYSTEM_IMG_ALG_UPDATE_PARAM			(0x211)
#define SYSTEM_IMG_ALG_GET_PARAM			(0x212)
#define SYSTEM_IMG_OUTPUT_SOURCE			(0x213)
#define SYSTEM_IMG_UPDATE_DBGIMG			(0x214)

#define ALG_SHAREDREGION_ID					0
typedef struct
{
	SystemCommon_edmaChPrm algEdmaHndl;
	unsigned char	*imgData;
	unsigned width;
	unsigned height;
	unsigned pitch;
	unsigned algCount;
	unsigned needRotate;
	Alg_Params *pPrm;
	Alg_DrvHandle algDrvHandle;
	Semaphore_Handle semRd;
	Semaphore_Handle semSync;
}ImgAlg_Obj;

typedef struct {
    Utils_TskHndl tsk;
    Utils_TskHndl processTsk;
    ImgAlg_Obj algObj;
    ImgLink_CreateParams createArgs;
    FVID2_Frame outFrames[IMG_LINK_MAX_OUT_FRAMES];
    FVID2_Format bufferFrameFormat;	
    System_FrameInfo freeFrameInfoMem[IMG_LINK_MAX_OUT_FRAMES];
    System_LinkInfo info;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;   
    Utils_BufHndl outFrameQue;

	/** for alg result */
	Ptr	pAlgInfo;
	Ptr pAlgShBuf;
	SharedRegion_SRPtr srPtr;

	/** for alg param */
	Ptr pAlgPrm;
	Ptr pAlgPrmShbuf;
	SharedRegion_SRPtr algPrmsrPtr;

    UInt32 totalTime;
    UInt32 frameCnt;

    UInt32 inWidth;
    UInt32 inHeight;
    UInt32 outWidth;
    UInt32 outHeight;

	unsigned useStaticImg;

    SystemCommon_edmaChPrm copyEdmaHndl;
} ImgLink_Obj;

/* Function prototypes */

Int32 ImgLink_algCreate(ImgLink_Obj *pObj);

void DM81XX_EDMA3_setParams(int chId, int dmaQueue, unsigned int srcAddr,
                            unsigned int dstAddr, unsigned short edmaWidth,
                            unsigned short edmaHeight, short srcLineOffset,
                            short dstLineOffset);

void DM81XX_EDMA3_triggerTransfer(int chId);

Int32 ImgLink_algCopyFrames(ImgLink_Obj *pObj,FVID2_Frame *pSrc,FVID2_Frame *pDst);
Void ImgLink_processTskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg);
Int32 ImgLink_cacheInv(UInt32 addr,UInt32 size);
Int32 ImgLink_cacheWb(UInt32 addr,UInt32 size);

void ImgAlg_PrmUpdate(ImgAlg_Obj *pAlgObj,char *pPrm,int prmSize);
void ImgAlg_PrmGet(ImgAlg_Obj *pAlgObj,char *pPrm);
int ImgAlg_isParamValid(char *pPrm);
#ifdef __cplusplus
}
#endif /* __cplusplus */
