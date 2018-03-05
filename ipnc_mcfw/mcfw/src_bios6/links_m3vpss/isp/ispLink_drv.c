/** ==================================================================
 *  @file   ispLink_drv.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/isp/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "ispLink_priv.h"

extern volatile int request_yuv_data;
extern volatile int yuv_data_ready;
extern UInt16 *captureBuff_Y[3];
/* isp driver callback functions */

/* ===================================================================
 *  @func     IspLink_drvScFvidCb
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
Int32 IspLink_drvScFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    IspLink_Obj *pObj = (IspLink_Obj *)appData;

    Semaphore_post(pObj->drvObj.complete);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IspLink_drvFvidErrCb
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
Int32 IspLink_drvFvidErrCb(FVID2_Handle handle, Ptr appData, Ptr errList,
                           Ptr reserved)
{
    UInt32 frmListCnt, queId;
    IspLink_Obj *pObj = (IspLink_Obj *)appData;
    FVID2_ProcessList *errProcList = (FVID2_ProcessList *)errList;
    System_LinkInQueParams *pInQueParams;

    /* There could be overflow or errorneous frame, do not get
       frame back from the driver*/
    pObj->doGetProcessFrames = 0;

    pInQueParams = &pObj->createArgs.inQueParams;

    /* Put back the output frames in the outputQue */
    for (frmListCnt = 0u; frmListCnt < errProcList->numOutLists; frmListCnt ++)
    {
        for(queId = 0; queId < pObj->createArgs.numOutQueue; queId += 2)
        {
            Utils_bufPutFullFrame(
                &pObj->outFrameQue[queId],
                errProcList->outFrameList[frmListCnt]->frames[queId]);
            Utils_bufPutFullFrame(
                &pObj->outFrameQue[queId + 1],
                errProcList->outFrameList[frmListCnt]->frames[queId + 1]);
        }

        /* Put the empty buffers back to previous link */
        System_putLinksEmptyFrames(
            pInQueParams->prevLinkId,
            pInQueParams->prevLinkQueId,
            errProcList->inFrameList[frmListCnt]);
    }

    /* Unblock the process Task */
    Semaphore_post(pObj->drvObj.complete);

    return FVID2_SOK;
}

/* Isp FVID2 driver create */

/* ===================================================================
 *  @func     IspLink_drvCreateFvidObj
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
Int32 IspLink_drvCreateFvidObj(IspLink_Obj *pObj)
{
    Semaphore_Params semParams;
    IspLink_drvObj *pDrvObj;
    FVID2_CbParams cbParams;

    pDrvObj = &pObj->drvObj;
    pDrvObj->drvInstId = 0;

    /* create a semaphore for sync */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pDrvObj->complete = Semaphore_create(0u, &semParams, NULL);

    UTILS_assert(pDrvObj->complete != NULL);

    /* Driver create params */
    pDrvObj->createParams.clkDivM   = pObj->createArgs.clkDivM;
    pDrvObj->createParams.clkDivN   = pObj->createArgs.clkDivN;
    pDrvObj->createParams.vsEnable  = pObj->createArgs.vsEnable;

    memset(&cbParams, 0, sizeof(cbParams));
    cbParams.cbFxn    = IspLink_drvScFvidCb;
    cbParams.errCbFxn = IspLink_drvFvidErrCb;
    cbParams.errList  = &pDrvObj->errCbProcessList;
    cbParams.appData  = pObj;

	pDrvObj->createParams.memType0  = pObj->createArgs.outQueuePrm[0].tilerEnable;
	pDrvObj->createParams.memType1  = pObj->createArgs.outQueuePrm[1].tilerEnable;
	if (pDrvObj->createParams.memType0)
		pDrvObj->createParams.enableBte0 = TRUE;
	else
		pDrvObj->createParams.enableBte0 = FALSE;

	if (pDrvObj->createParams.memType1)
		pDrvObj->createParams.enableBte1 = TRUE;
	else
		pDrvObj->createParams.enableBte1 = FALSE;

    pDrvObj->fvidHandle = FVID2_create(FVID2_ISS_M2M_ISP_DRV,
                                       pDrvObj->drvInstId,
                                       &pDrvObj->createParams,
                                       &pDrvObj->createStatus,
                                       &cbParams);

    UTILS_assert(pDrvObj->fvidHandle != NULL);

    pDrvObj->processList.numInLists      = 1;
    pDrvObj->processList.numOutLists     = 1;
    pDrvObj->processList.inFrameList[0]  = &pDrvObj->inFrameList;
    pDrvObj->processList.outFrameList[0] = &pDrvObj->outFrameList;

#ifdef SYSTEM_DEBUG_ISP
    Vps_printf(" %d: ISP   : FVID2 Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Isp link process frames */

/* ===================================================================
 *  @func     IspLink_drvProcessFrames
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
Int32 IspLink_drvProcessFrames(IspLink_Obj * pObj)
{
    UInt32 frameId,queId, pCnt;
    UInt32 cnt,outQueId, tFrmCnt = 0;
    Int32 status;
    Bool skipFrame[ISPLINK_MAX_OUT_QUE];
    System_LinkInQueParams *pInQueParams;
    FVID2_Frame *pEmptyFrame;
    FVID2_FrameList frameList, tFrmList, tSkipList;
    IspLink_drvObj *pDrvObj;
    Iss_IspRszConfig rszCfg;
    Iss_IspPerFrameCfg *pPerFrameCfg;
	System_FrameInfo *pFrameInfo;
	int i;

#ifdef SYSTEM_DEBUG_ISP_RT
    UInt32 start,end;
#endif

    pDrvObj = &pObj->drvObj;
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
#ifdef SYSTEM_DEBUG_ISP_RT
        Vps_printf(" %d: ISP   : Received and returning %d frames !!!\n",
                   Clock_getTicks(), frameList.numFrames);
#endif

        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pDrvObj->inFrameList.numFrames  = 0;
            pDrvObj->outFrameList.numFrames = 0;
            pObj->doGetProcessFrames = 1;

            pDrvObj->inFrameList.frames[pDrvObj->inFrameList.numFrames] = frameList.frames[frameId];
            pDrvObj->inFrameList.numFrames = 1;

            for(queId = 0;queId < pObj->createArgs.numOutQueue;queId += 2)
            {
                pDrvObj->outFrameList.numFrames = 0;

                skipFrame[queId] = Utils_doSkipFrame(&pObj->frameSkipContext[queId]);
                if(skipFrame[queId] == FALSE)
                {
                    /* Get frame buffer from empty queue */
                    status = Utils_bufGetEmptyFrame(&pObj->outFrameQue[queId],
                                                    &pEmptyFrame,
                                                    BIOS_NO_WAIT);

                    if ((status != FVID2_SOK) || (pEmptyFrame == NULL))
                    {
                        /* No empty buffers available from the output queue */
                        /* So return the full frames back to previous link */
						tSkipList.frames[0] = frameList.frames[frameId];
						tSkipList.numFrames = 1;
                        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                                   pInQueParams->prevLinkQueId,
                                                   &tSkipList);
						//need to release the input buffers to the previous link
                        continue;
                    }

                    pEmptyFrame->timeStamp = frameList.frames[frameId]->timeStamp;
                    pDrvObj->outFrameList.frames[pDrvObj->outFrameList.numFrames] = pEmptyFrame;
                    pDrvObj->outFrameList.numFrames++;
					
					/*
					 *	When NF mode is changed between NSF and VNF corresponding resolution changes should
					 *  be propogated to subsequent links.
					 */	
					if(queId == 0)
					{
						pFrameInfo = (System_FrameInfo *)pEmptyFrame->appData;
					
						if(pObj->createArgs.vnfFullResolution)
						{
							pFrameInfo->rtChInfo.width    = pObj->createArgs.outQueuePrm[queId].width + 32;
							pFrameInfo->rtChInfo.height   = pObj->createArgs.outQueuePrm[queId].height + 32;
							pFrameInfo->rtChInfo.pitch[0] = pObj->pitch[queId][0]; 
							pFrameInfo->rtChInfo.pitch[1] = pObj->pitch[queId][1];
						}
						else
						{
							pFrameInfo->rtChInfo.width    = pObj->createArgs.outQueuePrm[queId].width;
							pFrameInfo->rtChInfo.height   = pObj->createArgs.outQueuePrm[queId].height;						
							pFrameInfo->rtChInfo.pitch[0] = pObj->pitch[queId][0];
							pFrameInfo->rtChInfo.pitch[1] = pObj->pitch[queId][1];							
						}
						
						pFrameInfo->rtChInfoUpdate = TRUE;
					}
                }

                if((queId + 1) < pObj->createArgs.numOutQueue)
                {
                    skipFrame[queId + 1] = Utils_doSkipFrame(&pObj->frameSkipContext[queId + 1]);
                    if(skipFrame[queId + 1] == FALSE)
                    {
                        /* Get frame buffer from empty queue */
                        status = Utils_bufGetEmptyFrame(&pObj->outFrameQue[queId + 1],
                                                        &pEmptyFrame,
                                                        BIOS_NO_WAIT);

                        if ((status == FVID2_SOK) || (pEmptyFrame != NULL))
						{
                        	pEmptyFrame->timeStamp = frameList.frames[frameId]->timeStamp;
                        	pDrvObj->outFrameList.frames[pDrvObj->outFrameList.numFrames] = pEmptyFrame;
                        	pDrvObj->outFrameList.numFrames++;
						}
                    }
                }

				if(pDrvObj->outFrameList.numFrames > 0)
				{
					rszCfg.inDataFmt   = (FVID2_DataFormat)pObj->inQueInfo.chInfo[0].dataFormat;
					rszCfg.outDataFmt0 = (FVID2_DataFormat)pObj->createArgs.outQueuePrm[queId].dataFormat;
					rszCfg.outDataFmt1 = (FVID2_DataFormat)pObj->createArgs.outQueuePrm[queId + 1].dataFormat;
					rszCfg.inWidth     = pObj->inQueInfo.chInfo[0].width;
					rszCfg.inHeight    = pObj->inQueInfo.chInfo[0].height;
					rszCfg.inPitch     = pObj->inQueInfo.chInfo[0].pitch[0];

					if(pObj->createArgs.vsEnable == TRUE)
					{
						rszCfg.outWidth0   = (pObj->createArgs.outQueuePrm[queId].width * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
						rszCfg.outHeight0  = (pObj->createArgs.outQueuePrm[queId].height * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
						rszCfg.outWidth1   = (pObj->createArgs.outQueuePrm[queId + 1].width * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
						rszCfg.outHeight1  = (pObj->createArgs.outQueuePrm[queId + 1].height * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
					}
					else
					{
						rszCfg.outWidth0   = pObj->createArgs.outQueuePrm[queId].width;
						rszCfg.outHeight0  = pObj->createArgs.outQueuePrm[queId].height;
						rszCfg.outWidth1   = pObj->createArgs.outQueuePrm[queId + 1].width;
						rszCfg.outHeight1  = pObj->createArgs.outQueuePrm[queId + 1].height;
					}

					for (pCnt = 0; pCnt < 3; pCnt ++)
					{
						rszCfg.pitch[queId][pCnt] = pObj->pitch[queId][pCnt];
						rszCfg.pitch[queId + 1][pCnt] = pObj->pitch[queId + 1][pCnt];
					}
					
					rszCfg.numStream   = pDrvObj->outFrameList.numFrames;
					rszCfg.mirrorMode  = pObj->mirrorMode;

					if ((queId == 0) && (pObj->createArgs.vnfFullResolution))
					{
						rszCfg.outStartX = 16;
						rszCfg.outStartY = 18;
					}
					else
					{
						rszCfg.outStartX = 0;
						rszCfg.outStartY = 0;
					}

					/*
					*  VS algorithm should be run only for the first ipipe pass for a raw frame,
					*  for the subsequent ipipe passes same stabilized co-ordniates will be used.
					*/
					if(queId == 0)
					{
						rszCfg.runVs = TRUE;
					}
					else
					{
						rszCfg.runVs = FALSE;
					}

					if(skipFrame[queId] == TRUE)
					{
						rszCfg.outDataFmt0 = rszCfg.outDataFmt1;	
						rszCfg.outWidth0   = rszCfg.outWidth1;
						rszCfg.outHeight0  = rszCfg.outHeight1;
						
						for(pCnt = 0; pCnt < 3; pCnt ++)
						{
							rszCfg.pitch[queId][pCnt] = rszCfg.pitch[queId + 1][pCnt];
						}	

						rszCfg.outStartX = 0;
						rszCfg.outStartY = 0;	
						rszCfg.runVs     = FALSE;					
					}
					
					status = FVID2_control(pDrvObj->fvidHandle,
										   IOCTL_ISS_ISP_RSZ_CONFIG,
										   &rszCfg,
										   NULL);
					UTILS_assert(status == FVID2_SOK);

	#ifdef SYSTEM_DEBUG_ISP_RT
					start = Utils_getCurTimeInMsec();
	#endif

					/* Process the frames */
					status = FVID2_processFrames(pDrvObj->fvidHandle, &pDrvObj->processList);
					UTILS_assert(status == FVID2_SOK);

					Semaphore_pend(pDrvObj->complete, BIOS_WAIT_FOREVER);

	#ifdef SYSTEM_DEBUG_ISP_RT
					end = Utils_getCurTimeInMsec();
					Vps_printf(" %d: ISP   : Process time = %d msec !!!\n",
							   Clock_getTicks(), (end - start));
	#endif

					/* Frame Cannot be deuqued if this flag is reset */
					if (!pObj->doGetProcessFrames)
					{
						continue;
					}

					tFrmList.frames[tFrmCnt ++] = frameList.frames[frameId];

					/* Get the processed frames */
					status = FVID2_getProcessedFrames(pDrvObj->fvidHandle,&pDrvObj->processList,BIOS_NO_WAIT);
					UTILS_assert(status == FVID2_SOK);

					for(cnt = 0;cnt < pDrvObj->processList.outFrameList[0]->numFrames;cnt ++)
					{
						pPerFrameCfg = (Iss_IspPerFrameCfg*)(pDrvObj->outFrameList.frames[cnt]->perFrameCfg);
						outQueId     = pPerFrameCfg->queueId;

						/* In one shot mode, copy yuv frames to DCC buffers here */
						if ((queId == 0) && (request_yuv_data))
						{
							char *buffY = pDrvObj->outFrameList.frames[0]->addr[0][0];
							char *buffC = pDrvObj->outFrameList.frames[0]->addr[0][1];
							int size = pObj->createArgs.outQueuePrm[queId].width * pObj->createArgs.outQueuePrm[queId].height;
							Vps_printf("IspLink_drvProcessFrames copy yuv %x/%x to %x\n", buffY, buffC, captureBuff_Y[0]);
							if ((pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_UYVY) ||
								(pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YUYV) ||
								(pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_YVYU) ||
								(pObj->createArgs.outQueuePrm[queId].dataFormat == FVID2_DF_YUV422I_VYUY))
								{
									if (pObj->createArgs.vnfFullResolution == 1)
									{
										buffY += pObj->pitch[queId][0] * 18 + 16 * 2;
									}
									for (i = 0; i < pObj->createArgs.outQueuePrm[queId].height; i++)
										memcpy((char*)captureBuff_Y[0] + pObj->createArgs.outQueuePrm[queId].width * 2 * i,
											buffY + pObj->pitch[queId][0] * i, pObj->createArgs.outQueuePrm[queId].width * 2);
								}
							else
							{
								if (pObj->createArgs.vnfFullResolution == 1)
								{
									buffY += pObj->pitch[queId][0] * 18 + 16;
									buffC += pObj->pitch[queId][1] * 9 + 16;
								}
								for (i = 0; i < pObj->createArgs.outQueuePrm[queId].height; i++)
									memcpy((char*)captureBuff_Y[0] + pObj->createArgs.outQueuePrm[queId].width * i,
										buffY + pObj->pitch[queId][0] * i, pObj->createArgs.outQueuePrm[queId].width);
								for (i = 0; i < pObj->createArgs.outQueuePrm[queId].height/2; i++)							
									memcpy((char*)captureBuff_Y[0] + (size + pObj->createArgs.outQueuePrm[queId].width * i),
									buffC + pObj->pitch[queId][1] * i, pObj->createArgs.outQueuePrm[queId].width);
							}

							request_yuv_data = 0;
							yuv_data_ready = 1;
						}
						
						/* Put the full buffers */
						status = Utils_bufPutFullFrame(&pObj->outFrameQue[outQueId], pDrvObj->outFrameList.frames[cnt]);
						UTILS_assert(status == FVID2_SOK);

						/* send SYSTEM_CMD_NEW_DATA to next link */
						System_sendLinkCmd(pObj->createArgs.outQueInfo[outQueId].nextLink,
										   SYSTEM_CMD_NEW_DATA);
					}
				}// if(pDrvObj->outFrameList.numFrames > 0)  
				else
				{
					tFrmList.frames[tFrmCnt ++] = frameList.frames[frameId];
				}	
            }
        }

        tFrmList.numFrames = tFrmCnt;
        /* Put the empty buffers back to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &tFrmList);
    }

    return FVID2_SOK;
}

/* Delete the driver instance */

/* ===================================================================
 *  @func     IspLink_drvDeleteFvidObj
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
Int32 IspLink_drvDeleteFvidObj(IspLink_Obj * pObj)
{
    IspLink_drvObj *pDrvObj;

    pDrvObj = &pObj->drvObj;
    FVID2_delete(pDrvObj->fvidHandle, NULL);
    Semaphore_delete(&pDrvObj->complete);

#ifdef SYSTEM_DEBUG_ISP
    Vps_printf(" %d: ISP   : FVID2 Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IspLink_setOutputDataFormat
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
Int32 IspLink_setOutputDataFormat(IspLink_Obj * pObj,IspLink_outputDataFormat *pOutDataFmt)
{
	UInt32 streamId;
	
	streamId = pOutDataFmt->streamId;
	
	if(((pObj->createArgs.outQueuePrm[streamId].dataFormat == FVID2_DF_YUV420SP_UV) ||
	    (pObj->createArgs.outQueuePrm[streamId].dataFormat == FVID2_DF_YUV420SP_VU)) &&
	   ((pOutDataFmt->dataFmt == FVID2_DF_YUV420SP_UV) || 
	    (pOutDataFmt->dataFmt == FVID2_DF_YUV420SP_VU)))
	{
		return FVID2_SOK;
	}	
	
	if(((pObj->createArgs.outQueuePrm[streamId].dataFormat == FVID2_DF_YUV422I_UYVY) ||
		(pObj->createArgs.outQueuePrm[streamId].dataFormat == FVID2_DF_YUV422I_YUYV) ||
		(pObj->createArgs.outQueuePrm[streamId].dataFormat == FVID2_DF_YUV422I_YVYU) ||
	    (pObj->createArgs.outQueuePrm[streamId].dataFormat == FVID2_DF_YUV422I_VYUY)) &&
	   ((pOutDataFmt->dataFmt == FVID2_DF_YUV422I_UYVY) ||
		(pOutDataFmt->dataFmt == FVID2_DF_YUV422I_YUYV) ||
		(pOutDataFmt->dataFmt == FVID2_DF_YUV422I_YVYU) || 	
	    (pOutDataFmt->dataFmt == FVID2_DF_YUV422I_VYUY)))
	{
		return FVID2_SOK;
	}
	
	if(streamId < pObj->createArgs.numOutQueue)
	{				
#if defined(USE_MCTNF) && !defined(VNF_BEFORE_MCTNF)
		if(pOutDataFmt->isVnfON == 1)
		{
			UTILS_assert((pOutDataFmt->dataFmt == FVID2_DF_YUV420SP_UV) || 
			             (pOutDataFmt->dataFmt == FVID2_DF_YUV420SP_VU)); 
		
			pObj->pitch[streamId][0] /= 2;			
			pObj->pitch[streamId][1] /= 2;			
		}
		
		if(pOutDataFmt->isVnfON == 0)
		{
			UTILS_assert((pOutDataFmt->dataFmt == FVID2_DF_YUV422I_UYVY) || 
			             (pOutDataFmt->dataFmt == FVID2_DF_YUV422I_YUYV) ||		
						 (pOutDataFmt->dataFmt == FVID2_DF_YUV422I_YVYU) ||		
						 (pOutDataFmt->dataFmt == FVID2_DF_YUV422I_VYUY));
					
			pObj->pitch[streamId][0] *= 2;			
			pObj->pitch[streamId][1] *= 2;				
		}
#else	
		if((pOutDataFmt->isVnfON == 1) && (pObj->createArgs.vnfFullResolution == 0))
		{
			UTILS_assert((pOutDataFmt->dataFmt == FVID2_DF_YUV420SP_UV) || 
			             (pOutDataFmt->dataFmt == FVID2_DF_YUV420SP_VU)); 
		
			pObj->createArgs.vnfFullResolution = 1;		

			pObj->pitch[streamId][0] /= 2;			
			pObj->pitch[streamId][1] /= 2;			
		}
		
		if((pOutDataFmt->isVnfON == 0) && (pObj->createArgs.vnfFullResolution == 1))
		{
			UTILS_assert((pOutDataFmt->dataFmt == FVID2_DF_YUV422I_UYVY) || 
			             (pOutDataFmt->dataFmt == FVID2_DF_YUV422I_YUYV) ||		
						 (pOutDataFmt->dataFmt == FVID2_DF_YUV422I_YVYU) ||		
						 (pOutDataFmt->dataFmt == FVID2_DF_YUV422I_VYUY));
		
			pObj->createArgs.vnfFullResolution = 0;
			
			pObj->pitch[streamId][0] *= 2;			
			pObj->pitch[streamId][1] *= 2;				
		}
#endif		

		pObj->createArgs.outQueuePrm[streamId].dataFormat = pOutDataFmt->dataFmt;	
	}
	
	return FVID2_SOK;
} 
