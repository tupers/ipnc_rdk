/** ==================================================================
 *  @file   vaLink_alg.c
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

#include "vaLink_priv.h"
#include "dspLib/DM8127ImageProcess.h"
#include <mcfw/interfaces/link_api/encLink.h>
#include <mcfw/interfaces/link_api/swosdLink.h>
#include <mcfw/interfaces/link_api/cameraLink.h>

#ifdef LPR_DEMO
#include <mcfw/src_bios6/links_c6xdsp/alg/VI_LPRAlg/inc/LPRAlg.h>
extern int nLPRNotInitialized;
#define MAXCHANNEL 1
extern unsigned char *pucRecogResult[MAXCHANNEL];
extern unsigned char isLPRResult[MAXCHANNEL];
unsigned char ucRecogResult[8];
int siLPRShowCount;	
#endif

#define VA_PROFILE

#define UTILS_PRINTF_MAX_ARG1_LEN (500)
char utils_strArg1[UTILS_PRINTF_MAX_ARG1_LEN];

Int32 utils_printf(const char *format, ...)
{
	va_list va;
	int len;
	va_start(va, format);
	len = vsnprintf(utils_strArg1, UTILS_PRINTF_MAX_ARG1_LEN, format, va);
	utils_strArg1[len] = '\0';
	Vps_printf("DMVAL:%s",utils_strArg1);
	va_end(va);
	return 0 ;
}

/* ===================================================================
 *  @func     VaLink_algCreate
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
Int32 VaLink_algCreate(VaLink_Obj *pObj)
{
	//Int32 status;

	System_getEdmaCh(&pObj->algEdmaHndl);	
	System_getEdmaCh(&pObj->copyEdmaHndl);

	/***************************
	  VA ALG CREATE HERE
	 ***************************/

#ifdef SYSTEM_DEBUG_VA
	Vps_printf(" %d: VA   : ALG Init Done !!!\n", Clock_getTicks());
#endif

#ifdef SYSTEM_DEBUG_VA
	Vps_printf(" %d: VA   : ALG Create Done !!!\n", Clock_getTicks());
#endif

	return FVID2_SOK;
}

/* Va link copy frames */

/* ===================================================================
 *  @func     VaLink_algCopyFrames
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
Int32 VaLink_algCopyFrames(VaLink_Obj *pObj)
{
	Int32 status;
	Int32 ret = FVID2_EFAIL;
	Int32 frameId;
	UInt32 edmaWidth, edmaHeight;
	FVID2_Frame *pEmptyFrame;
	FVID2_Frame *pFullFrame;
	FVID2_FrameList frameList;
	System_LinkInQueParams *pInQueParams;
	//VaLink_algObj *pAlgObj = &pObj->algObj;

	/* Get the full frames from the previous link */
	pInQueParams = &pObj->createArgs.inQueParams;

	System_getLinksFullFrames(pInQueParams->prevLinkId,
			pInQueParams->prevLinkQueId, &frameList);

	if (frameList.numFrames)
	{
		for (frameId = 0; frameId < frameList.numFrames; frameId++)
		{
			{
				/* Get the VA empty bufs if any */
				status = Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame,BIOS_NO_WAIT);

				if ((status == FVID2_SOK) && (pEmptyFrame != NULL))
				{
					pFullFrame = frameList.frames[frameId];

					/* copy the frame */
					edmaWidth  = pObj->inQueInfo.chInfo[0].width;
					edmaHeight = pObj->inQueInfo.chInfo[0].height;

					/* copy Y plane */
					DM81XX_EDMA3_setParams(pObj->copyEdmaHndl.dmaHndl.chId, 			// chId
							VA_LINK_EDMA3_QUEUE_ID,  					// dmaQueue
							(UInt32)pFullFrame->addr[0][0], 				// srcAddr
							(UInt32)pEmptyFrame->addr[0][0],				// dstAddr
							edmaWidth,          							// edmaWidth
							edmaHeight,         							// edmaHeight
							edmaWidth,          							// srcLineOffset
							edmaWidth);         							// dstLineOffset

					/* Trigger the edma transfer */
					DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndl.dmaHndl.chId);

					pEmptyFrame->timeStamp = pFullFrame->timeStamp;

					/* put the buffer into full queue */
					status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
					UTILS_assert(status == FVID2_SOK);

					ret = FVID2_SOK;
				}
			}
		}

		/* Put the full buffers bacl to previous link */
		System_putLinksEmptyFrames(pInQueParams->prevLinkId,
				pInQueParams->prevLinkQueId, &frameList);
	}

	return ret;
}

SystemCommon_dspPrm temsg;
int vaT = 0;

Int32 VaLink_algProcessFrames(VaLink_Obj *pObj)
{
	Int32 status;
	FVID2_Frame *pFullFrame;	
	Int32 width = pObj->bufferFrameFormat.width;
	Int32 height = pObj->bufferFrameFormat.height;

	unsigned char *ptr = (unsigned char *)malloc(width*height);

	vaT++;

	if(vaT==500)
		vaT = 0;

	do
	{
		/* Get the frame from the full queue */
		status = Utils_bufGetFullFrame(&pObj->outFrameQue, &pFullFrame,BIOS_NO_WAIT);

		if ((status == FVID2_SOK) && (pFullFrame != NULL))
		{
			status = Utils_bufPutEmptyFrame(&pObj->outFrameQue,pFullFrame);
			UTILS_assert(status == FVID2_SOK);

		}

		ImageProcess(ptr, pFullFrame->addr[0][0],width, height);


		temsg.x = vaT;
		temsg.y = vaT;
		temsg.errorCode = 0;
		temsg.Ybuf = (UInt32)(pFullFrame->addr[0][0]);
		temsg.Rbuf = (UInt32)0x500;
		temsg.Gbuf = (UInt32)0x300;
		temsg.Bbuf = (UInt32)0x200;
		temsg.width = pObj->bufferFrameFormat.width;
		temsg.height = pObj->bufferFrameFormat.height;

		System_linkControl(SYSTEM_LINK_ID_HOST,
				SYSTEM_COMMON_CMD_DSP_MSG,
				&temsg,
				sizeof(SystemCommon_dspPrm),
				TRUE);	

	}while ((status == FVID2_SOK) && (pFullFrame != NULL));

	free(ptr);
#if 0
	Int32 status;
	VaLink_algObj *pAlgObj = &pObj->algObj;	
	Int32 libStatus;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;
	UInt32 frameId;
	FVID2_Frame *pEmptyFrame;		
	
	DMVALimage inImage;
	FVID2_Frame *pFullFrame;	
	
	RM_SIMCOP_STATUS eRMStatus = RM_SIMCOP_STATUS_SUCCESS;
	UInt32 bitFieldOfResources;
	RM_SIMCOP_RESOURCE_STATUS resStatus;	

#ifdef VA_PROFILE
	UInt32 start,end;
#endif

	do
	{
		/* Get the frame from the full queue */
		status = Utils_bufGetFullFrame(&pObj->outFrameQue, &pFullFrame,BIOS_NO_WAIT);

		if ((status == FVID2_SOK) && (pFullFrame != NULL))
		{
			/***************************
			  VA ALG APPLY HERE
			 ***************************/
			inImage.size		= sizeof(DMVALimage);
			inImage.timeStamp   = VaLink_algGetFrameTS(pObj,pFullFrame->timeStamp);
			inImage.type        = DMVAL_IMG_YUV420_PLANARINTERLEAVED;
			inImage.pixelDepth  = DMVAL_PIXEL_U08;
			inImage.width       = pAlgObj->params.maxWidth;
			inImage.height      = pAlgObj->params.maxHeight;
			inImage.imageStride = pAlgObj->inStride;
			inImage.horzOffset  = 0;
			inImage.vertOffset  = 0;
			inImage.imageData   = pFullFrame->addr[0][0];
			inImage.inputId     = ((VaLink_perFrameConfig*)pFullFrame->perFrameCfg)->bufId;
			inImage.focusValue  = VaLink_algGetFocusValue(pObj);
			inImage.type        = DMVAL_IMG_YUV420_PLANARINTERLEAVED;

#ifdef VA_PROFILE
			start = Utils_getCurTimeInUsec();
#endif

#ifndef IPNC_DSP_ON
			bitFieldOfResources              = RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_HWSEQ | RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DMA;
			resStatus.bitFieldDirtyResources = 0;
			eRMStatus = RM_SIMCOP_AcquireMultiResources((RM_SIMCOP_CLIENT_HANDLE)pObj->pRMHandle,
					bitFieldOfResources,
					RM_SIMCOP_ACQUIRE_WAIT,
					&resStatus);
			UTILS_assert(eRMStatus == RM_SIMCOP_STATUS_SUCCESS);
#endif

			Cache_inv((Ptr)inImage.imageData,(inImage.width * inImage.height),Cache_Type_ALL,TRUE);

#ifdef LPR_DEMO			
			LPRAlg(inImage.imageData,0);
#else
			if(pAlgObj->startCnt >= VA_LINK_START_CNT)
			{
				libStatus = DMVAL_process(pDmvaHndl,
						&inImage,
						&pAlgObj->output);
				UTILS_assert(libStatus == DMVAL_NO_ERROR);
			}
			else
			{
				pAlgObj->output.freeBufId[0] = inImage.inputId;	
				pAlgObj->output.freeBufId[1] = 0;
				pAlgObj->startCnt++;
			}
#endif			

#ifndef IPNC_DSP_ON
			bitFieldOfResources = RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_HWSEQ | RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DMA;
			eRMStatus = RM_SIMCOP_ReleaseMultiResources((RM_SIMCOP_CLIENT_HANDLE)pObj->pRMHandle,
					bitFieldOfResources);
			UTILS_assert(eRMStatus == RM_SIMCOP_STATUS_SUCCESS);
#endif			

#ifdef LPR_DEMO		
			vaStrPrm.disFlag     = 0;
			vaStrPrm.vaString[0] = '\0';

			if(isLPRResult[0]) 
			{
				isLPRResult[0] = 0;
				siLPRShowCount = 150;	
				strcpy((char*)ucRecogResult,(char*)pucRecogResult[0]);				
			}

			if(siLPRShowCount) 
			{
				strcpy((char*)vaStrPrm.vaString,(char*)ucRecogResult);
				siLPRShowCount--;

				if(pObj->createArgs.swosdLinkId != SYSTEM_LINK_ID_INVALID)	
				{
					System_linkControl(pObj->createArgs.swosdLinkId,
							SWOSDLINK_CMD_VASTRING,
							&vaStrPrm,
							sizeof(SwosdLink_vaStrPrm),TRUE);				
				}				   
			}			
#else
			status = VaLink_algCreateMetaData(pObj);
			UTILS_assert(status == FVID2_SOK);
#endif			

#ifdef VA_PROFILE
			end = Utils_getCurTimeInUsec();
			if((end - start) > pAlgObj->maxProcessTime)
			{
				pAlgObj->maxProcessTime = (end - start);
			}
			pAlgObj->totalProcessTime += (end - start);
#endif

			pAlgObj->processFrCnt ++;

			if(pAlgObj->processFrCnt >= 900)
			{
				Vps_printf(" %d: VA   : ######################################### !!!\n",
						Clock_getTicks());
#ifdef VA_PROFILE
				Vps_printf(" %d: VA   : Peroidic Avg Process time = %d usec/frame !!!\n",
						Clock_getTicks(),pAlgObj->totalProcessTime/pAlgObj->processFrCnt);
				Vps_printf(" %d: VA   : Maximum Process time      = %d usec/frame !!!\n",
						Clock_getTicks(),pAlgObj->maxProcessTime);						   
#else
				Vps_printf(" %d: VA   : Peroidic print !!!\n", Clock_getTicks());
#endif
				Vps_printf(" %d: VA   : ######################################### !!!\n",
						Clock_getTicks());
				pAlgObj->processFrCnt     = 0;
				pAlgObj->totalProcessTime = 0;
			}

#ifdef LPR_DEMO
			status = Utils_bufPutEmptyFrame(&pObj->outFrameQue,pFullFrame);
			UTILS_assert(status == FVID2_SOK);
#else			
			for(frameId = 0;frameId < DMVAL_MAX_BUFFER_LOCK;frameId ++)
			{
				/* The array ends with Id equal to 0 */
				if(pAlgObj->output.freeBufId[frameId] == 0)
				{
					break;
				}

				pEmptyFrame = &pObj->outFrames[pAlgObj->output.freeBufId[frameId] - 1];

				/* Put the frames back which are marked as free by DMVAL_process */
				status = Utils_bufPutEmptyFrame(&pObj->outFrameQue,pEmptyFrame);
				UTILS_assert(status == FVID2_SOK);
			}
#endif			
		}
	} while ((status == FVID2_SOK) && (pFullFrame != NULL));
#endif
	return FVID2_SOK;
}

#if 0
/* ===================================================================
 *  @func     VaLink_algDelete
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
Int32 VaLink_algDmvalDelete(VaLink_Obj *pObj)
{
	VaLink_algObj *pAlgObj = &pObj->algObj;

	/* Free ALG mem */
	Utils_memFree_cached(pAlgObj->ptr,pAlgObj->size);

#ifndef IPNC_DSP_ON
	RM_SIMCOP_DeRegisterClient(pObj->pRMHandle);
#endif

	return FVID2_SOK;
}
#endif
/* Delete the driver instance */

/* ===================================================================
 *  @func     VaLink_algDelete
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
Int32 VaLink_algDelete(VaLink_Obj *pObj)
{
	Int32 status;

	/***************************
	  VA ALG DELETE HERE
	 ***************************/
	//status = VaLink_algDmvalDelete(pObj);
	UTILS_assert(status == FVID2_SOK);

	System_putEdmaCh(&pObj->copyEdmaHndl);
	System_putEdmaCh(&pObj->algEdmaHndl);		

#ifdef SYSTEM_DEBUG_VA
	Vps_printf(" %d: VA   : ALG Delete Done !!!\n", Clock_getTicks());
#endif

	return FVID2_SOK;
}
