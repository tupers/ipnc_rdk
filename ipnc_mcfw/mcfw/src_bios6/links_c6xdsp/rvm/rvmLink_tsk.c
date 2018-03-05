/** ==================================================================
 *  @file   rvmLink_tsk.c
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

#include "rvmLink_priv.h"

#pragma DATA_ALIGN(gRvmLink_tskStack, 32)
#pragma DATA_SECTION(gRvmLink_tskStack, ".bss:taskStackSection")
UInt8 gRvmLink_tskStack[RVM_LINK_OBJ_MAX][RVM_LINK_TSK_STACK_SIZE];

/* Scratch memory allocated from L2 RAM */
#pragma DATA_ALIGN(gScratch, 32)
#pragma DATA_SECTION(gScratch, ".internalHeap")
UInt8 gScratch[RVM_LINK_SCRATCHMEM_SIZE];

RvmLink_Obj gRvmLink_obj[RVM_LINK_OBJ_MAX];

//#define FULL_PROFILE

#ifdef FULL_PROFILE
	UInt32 gCpyStart1 = 0;
	UInt32 gCpyStart2 = 0;
	UInt32 gCpyStart3 = 0;
	UInt32 gCpyStart4 = 0;
	UInt32 gCpyWait1 = 0;
	UInt32 gCpyWait2 = 0;
	UInt32 gCacheInvTime = 0;
	UInt32 gRvmProcessTime = 0;
	UInt32 gCacheWbTime = 0;
	UInt32 gOutputCpyTime = 0;
	UInt32 gFrameProcessTime = 0;
#endif

Int32 RvmLink_cacheInv(UInt32 addr,UInt32 size)
{
	Cache_inv((Ptr)addr,size,Cache_Type_ALL,TRUE);	
	return FVID2_SOK;
}

Int32 RvmLink_cacheWb(UInt32 addr,UInt32 size)
{
	Cache_wb((Ptr)addr,size,Cache_Type_ALL,TRUE);	
	return FVID2_SOK;
}

/* Rvm link create */

/* ===================================================================
 *  @func     RvmLink_create
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
Int32 RvmLink_create(RvmLink_Obj * pObj, RvmLink_CreateParams * pPrm)
{
    Int32 status;
	UInt32 frameId,chId,scratchSize;
	FVID2_Frame *pFrame;
    System_LinkQueInfo *pOutQueInfo;
    System_LinkChInfo *pInChInfo, *pOutChInfo;
	SystemCommon_rvmLutPrm rvmLutPrm;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    /* Get the link info from prev link */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);
	
    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));	
	
	UTILS_assert(pObj->inQueInfo.numCh == 1);

	/* Allocate LUT memory */
	pObj->pLut = Utils_memAlloc(RVM_LINK_LUT_SIZE,32);	
	UTILS_assert(pObj->pLut != NULL);	
	
	/* Read LUT data into LUT memory */
	//strcpy((char*)rvmLutPrm.fileName,"/mnt/mmc/lut_9j003_5mp_2nd.dat");
	//strcpy((char*)rvmLutPrm.fileName,"/mnt/mmc/lut_2560x1920.dat");
	strcpy((char*)rvmLutPrm.fileName,"/mnt/mmc/lut_2048x1152.dat");
	//strcpy((char*)rvmLutPrm.fileName,"/mnt/mmc/RVM_lut.bin");
	rvmLutPrm.lutBufAddr = (UInt32)pObj->pLut;
	rvmLutPrm.lutBufSize = RVM_LINK_LUT_SIZE;
		
    System_linkControl(SYSTEM_LINK_ID_HOST,
                       SYSTEM_COMMON_READ_RVM_LUT,
					   &rvmLutPrm,
					   sizeof(SystemCommon_rvmLutPrm),
					   TRUE);	
	//UTILS_assert(rvmLutPrm.errorCode == 0);				   
					   
	RvmLink_cacheInv(rvmLutPrm.lutBufAddr,rvmLutPrm.lutFileSize);

	/*
	 *	Get input and output resolutions from the LUT
     */	 
	pObj->inWidth   = *((UInt32*)pObj->pLut + 5); 
	pObj->inHeight  = *((UInt32*)pObj->pLut + 6); 
	pObj->outWidth  = *((UInt32*)pObj->pLut + 7); 
	pObj->outHeight = *((UInt32*)pObj->pLut + 8); 	
	
	/*
	 *	Get scratch buf size from LUT
     */	 
	scratchSize     = *((UInt32*)pObj->pLut + 9);
	scratchSize    += *((UInt32*)pObj->pLut + 10);
	scratchSize    += *((UInt32*)pObj->pLut + 11);
	scratchSize    += *((UInt32*)pObj->pLut + 12);
	
	/* Size of scratch memory allocated should be >= above size calculated from LUT */
	UTILS_assert(scratchSize <= RVM_LINK_SCRATCHMEM_SIZE);
	
	UTILS_assert(pObj->inWidth <= pObj->inQueInfo.chInfo[0].width);
	UTILS_assert(pObj->inHeight <= pObj->inQueInfo.chInfo[0].height);
	
    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue,           // pHndl
                             FALSE,                        // blockOnGet
                             FALSE);                       // blockOnPut
    UTILS_assert(status == FVID2_SOK);

	/* Allocate output buffers */
    pObj->bufferFrameFormat.width = pObj->outWidth;
    pObj->bufferFrameFormat.height = pObj->outHeight;
    pObj->bufferFrameFormat.dataFormat = pObj->inQueInfo.chInfo[0].dataFormat;
    
	if((pObj->bufferFrameFormat.dataFormat == SYSTEM_DF_YUV420SP_UV) ||
	   (pObj->bufferFrameFormat.dataFormat == SYSTEM_DF_YUV420SP_VU))
	{
		pObj->bufferFrameFormat.pitch[0] = pObj->outWidth;
		pObj->bufferFrameFormat.pitch[1] = pObj->outWidth;	
	}
	else
	{
		pObj->bufferFrameFormat.pitch[0] = pObj->outWidth << 1;
		pObj->bufferFrameFormat.pitch[1] = pObj->outWidth << 1;
	}

    status = Utils_memFrameAlloc(&pObj->bufferFrameFormat, 	// pFormat
                                 pObj->outFrames,          	// pFrame
                                 RVM_LINK_MAX_OUT_FRAMES);  // numFrames

    UTILS_assert(status == FVID2_SOK);	
	
    /* Queue all the buffers to empty queue */
    for (frameId = 0; frameId < RVM_LINK_MAX_OUT_FRAMES; frameId++)
    {
        pFrame = &pObj->outFrames[frameId];
        status = Utils_bufPutEmptyFrame(&pObj->outFrameQue, pFrame);

        UTILS_assert(status == FVID2_SOK);
    }	

    /* Populating the link info structure */
    pObj->info.numQue = 1;
	pOutQueInfo = &pObj->info.queInfo[0];
	pObj->info.queInfo[0].numCh = pObj->inQueInfo.numCh;

	for (chId = 0; chId < pObj->info.queInfo[0].numCh; chId++)
	{
		pInChInfo = &pObj->inQueInfo.chInfo[chId];
		pOutChInfo = &pOutQueInfo->chInfo[chId];

		pOutChInfo->bufType      = pInChInfo->bufType;
		pOutChInfo->codingformat = pInChInfo->codingformat;
		pOutChInfo->dataFormat   = pInChInfo->dataFormat;
		pOutChInfo->memType      = pInChInfo->memType;
		pOutChInfo->startX       = 0;
		pOutChInfo->startY       = 0;
		pOutChInfo->width        = pObj->outWidth;
		pOutChInfo->height       = pObj->outHeight;
		pOutChInfo->pitch[0]     = pObj->bufferFrameFormat.pitch[0];
		pOutChInfo->pitch[1]     = pObj->bufferFrameFormat.pitch[1];
		pOutChInfo->scanFormat   = pInChInfo->scanFormat;
	}
		
	pObj->totalTime = 0;
	pObj->frameCnt  = 0;	
	
#ifdef SYSTEM_DEBUG_RVM
    Vps_printf(" %d: RVM   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Rvm process frames */

/* ===================================================================
 *  @func     RvmLink_processFrames
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
Int32 RvmLink_processFrames(RvmLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;
    UInt32 frameId;
    FVID2_FrameList frameList;
    FVID2_Frame *pFullFrame,*pEmptyFrame;
    System_LinkInQueParams *pInQueParams;
	System_LinkQueInfo *pOutQueInfo;
	System_LinkChInfo *pInChInfo, *pOutChInfo;	
	Rvm_Image_t input, output;
    RVM_colorformat_e color;
	UInt32 scratchSize;
	UInt32 start,end;
	UInt32 paramEntry[4];
	UInt32 tccVal[4];
	UInt32 processTime,totalBlks;
	float processTimePerBlk,dspCyclesPerBlk,dspCyclesPerPixel;
	
	color       = RVM_COLORFORMAT_YUV422;
    scratchSize = RVM_getScratchSize(pObj->pLut, color);	
	
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        /* Apply RVM on the full frames */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFullFrame = frameList.frames[frameId];
			
			pInChInfo   = &pObj->inQueInfo.chInfo[pFullFrame->channelNum];
			pOutQueInfo = &pObj->info.queInfo[0];
			pOutChInfo  = &pOutQueInfo->chInfo[pFullFrame->channelNum];

			/* Get the empty frame from out queue */
			status = Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame, BIOS_NO_WAIT);
			if((status != FVID2_SOK) || (pEmptyFrame == NULL))
			{
				/* Empty buffer is not avialbale,return the full frames back */
				System_putLinksEmptyFrames(pInQueParams->prevLinkId,
										   pInQueParams->prevLinkQueId, &frameList);				
											  
				return FVID2_SOK;							  
			}
			
			start = Utils_getCurTimeInUsec();
			
			input.pData[0]   = (UInt8*)pFullFrame->addr[0][0];
			input.stride[0]  = pInChInfo->pitch[0];		
			output.pData[0]  = (UInt8*)pEmptyFrame->addr[0][0];
			output.stride[0] = pOutChInfo->pitch[0];	

			/*
			 *	Provide free PaRAM entries in the range 0..511
			 */	
			paramEntry[0] = 64;
			paramEntry[1] = 65;
			paramEntry[2] = 66;
			paramEntry[3] = 67;
			
			/*
			 *	Provide unused TCC values in the range 0..63
			 *  Make sure all 4 TCC values fall within either first or second 32 values
			 *  i.e in the range (0..31) or (32..63)
			 */			
			tccVal[0] = 40;
			tccVal[1] = 41;
			tccVal[2] = 42;
			tccVal[3] = 43;
			
			if (RVM_wrapper(&input,				
							&output,			
							color,		
							pObj->pLut,
							(UInt8*)gScratch,
							scratchSize,
							1,					// 0:use memcpy,1:use QDMA
							NULL,				// Cache invalidate function pointer
							NULL,				// Cache writeback function pointer
							paramEntry,
							tccVal))							
			{
				Vps_printf("RVM: Process call failed\n");
			}			
			
			end = Utils_getCurTimeInUsec();
			pObj->totalTime += (end - start);
			pObj->frameCnt ++;
			
			if((pObj->frameCnt % RVM_LINK_PERIODIC_PRINT) == 0)
			{
				processTime       = pObj->totalTime/pObj->frameCnt;
				totalBlks         = (pObj->outWidth/RVM_LINK_OUT_BLKSIZE) * (pObj->outHeight/RVM_LINK_OUT_BLKSIZE);
				processTimePerBlk = (float)processTime/totalBlks;
				dspCyclesPerBlk   = processTimePerBlk * RVM_LINK_DSP_FREQ_MHZ;
				dspCyclesPerPixel = dspCyclesPerBlk/(RVM_LINK_OUT_BLKSIZE * RVM_LINK_OUT_BLKSIZE);
				
				Vps_printf("RVM: Output Resolution = [%4d x %4d]\n",pObj->outWidth,pObj->outHeight);
				Vps_printf("RVM: Process time      = %d usec\n",processTime);
				Vps_printf("RVM: Performance       = %2.2f cycles/pixel:%2.2f fps\n",
				           dspCyclesPerPixel,((float)1000000/processTime));				
#ifdef FULL_PROFILE				
				Vps_printf("RVM: Copy Start times  = %d %d %d %d usec\n",gCpyStart1,gCpyStart2,gCpyStart3,gCpyStart4);
				Vps_printf("RVM: Copy Wait times   = %d %d usec\n",gCpyWait1,gCpyWait2);
				Vps_printf("RVM: Cache Inv time    = %d usec\n",gCacheInvTime);
				Vps_printf("RVM: RMV processtime   = %d usec\n",gRvmProcessTime);
				Vps_printf("RVM: Cache Wb time     = %d usec\n",gCacheWbTime);
				Vps_printf("RVM: Full Process Time = %d usec\n",gFrameProcessTime);
				Vps_printf(" \n");
#endif				
				
				pObj->frameCnt  = 0;
				pObj->totalTime = 0;
			}				
			
            /* Put the full buffer into full queue */
            status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
            UTILS_assert(status == FVID2_SOK);
        }

		/* Put the empty buffers back to previous link */
		System_putLinksEmptyFrames(pInQueParams->prevLinkId,
								   pInQueParams->prevLinkQueId, &frameList);		
		
        /* send SYSTEM_CMD_NEW_DATA to next link */
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
    }

    return FVID2_SOK;
}

/* Rvm link drv delete */

/* ===================================================================
 *  @func     RvmLink_delete
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
Int32 RvmLink_delete(RvmLink_Obj * pObj)
{
    Int32 status;	

	RVM_exit();
	
    /* Free the frame memory */
    status = Utils_memFrameFree(&pObj->bufferFrameFormat,  	// pFormat
                                pObj->outFrames,           	// pFrame
                                RVM_LINK_MAX_OUT_FRAMES);   // numFrames
    UTILS_assert(status == FVID2_SOK);	
	
    /* Delete the buffers */
    status = Utils_bufDelete(&pObj->outFrameQue);
    UTILS_assert(status == FVID2_SOK);
	
	/* Delete LUT memory */
	Utils_memFree(pObj->pLut,RVM_LINK_LUT_SIZE);
	
#ifdef SYSTEM_DEBUG_RVM
    Vps_printf(" %d: RVM   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}
 
/* Rvm link task main function */

/* ===================================================================
 *  @func     RvmLink_tskMain
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
Void RvmLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    RvmLink_Obj *pObj = (RvmLink_Obj *) pTsk->appData;
    UInt32 flushCmds[2];

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the rvm algorithm */
    status = RvmLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done   = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done   = TRUE;
                ackMsg = TRUE;
                break;

            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);

                RvmLink_processFrames(pObj);
                break;
				
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the rvm algorithm */
    RvmLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* Rvm link get info */

/* ===================================================================
 *  @func     RvmLink_getInfo
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
Int32 RvmLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    RvmLink_Obj *pObj = (RvmLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* Rvm get full frames */

/* ===================================================================
 *  @func     RvmLink_getFullFrames
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
Int32 RvmLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;
    RvmLink_Obj *pObj = (RvmLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Rvm put empty frames */

/* ===================================================================
 *  @func     RvmLink_putEmptyFrames
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
Int32 RvmLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
	Int32 status;
    RvmLink_Obj *pObj = (RvmLink_Obj *) pTsk->appData;

    status = Utils_bufPutEmpty(&pObj->outFrameQue, pFrameList);
    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Rvm Link init */

/* ===================================================================
 *  @func     RvmLink_init
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
Int32 RvmLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 rvmId;

    RvmLink_Obj *pObj;

    char tskName[32];
    
    for (rvmId = 0; rvmId < RVM_LINK_OBJ_MAX; rvmId++)
    {
        pObj = &gRvmLink_obj[rvmId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = RvmLink_getFullFrames;
        linkObj.linkPutEmptyFrames = RvmLink_putEmptyFrames;
        linkObj.getLinkInfo = RvmLink_getInfo;

        System_registerLink(SYSTEM_LINK_ID_RVM_0 + rvmId, &linkObj);

        sprintf(tskName, "RVM%d", rvmId);

        status = Utils_tskCreate(&pObj->tsk,
                                 RvmLink_tskMain,
                                 RVM_LINK_TSK_PRI,
                                 gRvmLink_tskStack[rvmId],
                                 RVM_LINK_TSK_STACK_SIZE, pObj, tskName);

        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* Rvm link de-init */

/* ===================================================================
 *  @func     RvmLink_deInit
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
Int32 RvmLink_deInit()
{
    UInt32 rvmId;

    for (rvmId = 0; rvmId < RVM_LINK_OBJ_MAX; rvmId++)
    {
        Utils_tskDelete(&gRvmLink_obj[rvmId].tsk);
    }

    return FVID2_SOK;
}
