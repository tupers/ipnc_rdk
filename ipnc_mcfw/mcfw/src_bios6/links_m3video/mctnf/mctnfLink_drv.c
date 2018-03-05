/** ==================================================================
 *  @file   mctnfLink_drv.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/vnf/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <mcfw/src_bios6/utils/utils_mem.h>

#include "mctnfLink_priv.h"
extern Iss_CaptDccPrm gDccPrm_for_mctnf;
extern ti2a_output ti2a_output_params_for_mctnf;
extern iss_drv_config_t *pIss_drv_config_for_mctnf;
extern iss_drv_config_t iss_drv_config_for_mctnf;

//#define _PROFILE

static Int32 MctnfLink_drvInitCh(MctnfLink_Obj * pObj)
{
    Int32 status;

    UInt32 chId, /*frameId,*/ queueId = MCTNF_LINK_OUT_QUE_ID;

    //FVID2_Frame *frame;

    MctnfLink_ChObj *MctnfChObj;

    MctnfCreateParams *chMctnfCreateParams;
	IMCTNF_Params        *chParams;
	MCTNF_DynamicParams *chDynamicParams;

    MctnfLink_ChCreateParams *chCreateParams;

    System_LinkChInfo *inChInfo, *outChInfo;

    /*
     * For each channel, do the initialization and
     * FVID2 frame / buffer allocation.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        MctnfChObj = &pObj->linkChInfo[chId];
        chMctnfCreateParams = &MctnfChObj->chCreateArgs;
		chParams = &MctnfChObj->params;
		chDynamicParams = &MctnfChObj->dynParams;
        chCreateParams = &pObj->createArgs.chCreateParams[chId];
        inChInfo = &pObj->inQueInfo.chInfo[chId];
        outChInfo = &pObj->info.queInfo[queueId].chInfo[chId];

        //chCreateParams->dParams.featureMask = 0;
        chCreateParams->dParams.chId = chId;
        chCreateParams->sParams.chId = chId;

        /* Set the channel number first */
        MctnfChObj->channelId = chId;
        MctnfChObj->inFrameSkipCount = 0;
        MctnfChObj->nextFid = FVID2_FID_TOP;
        MctnfChObj->disable = FALSE;

		if(pObj->createArgs.enable[chId] == 0)
		{
			/*
			 * If the channel is disabled for McTNF then McTNF is not there in the data path.
			 * And input data format may be different than YUV420SP.
			 * VNF create should happen successfully so that it may be enabled at later point of time.
			 * Therefore pitch is halved.	
			 */		
			if((inChInfo->dataFormat == FVID2_DF_YUV422I_UYVY) ||
			   (inChInfo->dataFormat == FVID2_DF_YUV422I_YUYV) ||
			   (inChInfo->dataFormat == FVID2_DF_YUV422I_YVYU) ||
			   (inChInfo->dataFormat == FVID2_DF_YUV422I_VYUY))
			{
				inChInfo->pitch[0]   = inChInfo->pitch[0] >> 1;
				inChInfo->pitch[1]   = inChInfo->pitch[0];
				inChInfo->dataFormat = FVID2_DF_YUV420SP_UV;
			}	
		}		
		
        MCTNFLINK_INFO_LOG
            ("Input Channel CH%d of %d x %d, pitch = (%d, %d) [%d,%d][%s] [%d] ... \n",
             chId, inChInfo->width, inChInfo->height, inChInfo->pitch[0],
             inChInfo->pitch[1], inChInfo->startX, inChInfo->startY,
             gSystem_nameScanFormat[inChInfo->scanFormat],
             inChInfo->dataFormat);

        chMctnfCreateParams->ulInputWidth = inChInfo->width;
        chMctnfCreateParams->ulInputHeight = inChInfo->height;

		memset(&MctnfChObj->params, 0 , sizeof(IMCTNF_Params));
		memset(&MctnfChObj->dynParams, 0, sizeof(IMCTNF_DynamicParams));
		memset(&MctnfChObj->status, 0, sizeof(IMCTNF_Status));
		memset(&MctnfChObj->inArgs, 0, sizeof(IMCTNF_InArgs));
		memset(&MctnfChObj->outArgs, 0, sizeof(IMCTNF_OutArgs));
		memset(&MctnfChObj->gInputBufDesc, 0, sizeof(IVIDEO2_BufDesc));
		memset(&MctnfChObj->gOutputBufDesc, 0, sizeof(IVIDEO2_BufDesc));

		MctnfChObj->status.vidnf1Status.size  = sizeof(MCTNF_Status);
		MctnfChObj->inArgs.vidnf1InArgs.size  = sizeof(MCTNF_InArgs);
		MctnfChObj->outArgs.vidnf1OutArgs.size = sizeof(MCTNF_OutArgs);

		Vps_rprintf("inChInfo->width = %d \n", inChInfo->width);
		Vps_rprintf("inChInfo->height = %d \n", inChInfo->height);
		Vps_rprintf("inChInfo->pitch[0] = %d \n", inChInfo->pitch[0]);


        MCTNF_SetInitParams(chParams, chDynamicParams, inChInfo->width, (((inChInfo->height+15)>>4)<<4), inChInfo->pitch[0]);



		if (inChInfo->memType == VPS_VPDMA_MT_TILEDMEM)
		{
			chMctnfCreateParams->tilerEn = MSP_TRUE;
        	outChInfo->memType = VPS_VPDMA_MT_TILEDMEM;
		}
		else
		{
			chMctnfCreateParams->tilerEn = MSP_FALSE;
			outChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
		}

        chMctnfCreateParams->ulInputStride = inChInfo->pitch[0];
        switch (inChInfo->dataFormat)
        {
            case FVID2_DF_YUV422I_UYVY:
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_YVYU:
            case FVID2_DF_YUV422I_VYUY:
                chMctnfCreateParams->eInputFormat = MCTNF_YUV_FORMAT_YCBCR422;
                 break;
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                chMctnfCreateParams->eInputFormat = MCTNF_YUV_FORMAT_YCBCR420;
                chMctnfCreateParams->ulInputStrideChroma = inChInfo->pitch[1];
                break;
        }

        switch (chCreateParams->sParams.eOutputFormat)
        {
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                chMctnfCreateParams->eOutputFormat = MCTNF_YUV_FORMAT_YCBCR420;
                break;
        }


        {
	                chCreateParams->sParams.computeWidth = inChInfo->width ;
					// align to 16 byte, to the higher side: like 1080 to 1088, not to 1072
	                chCreateParams->sParams.computeHeight = (((inChInfo->height+15)>>4)<<4) ;
	                chCreateParams->sParams.outputStrideLuma = inChInfo->width;
	                chCreateParams->sParams.outputStrideChroma = inChInfo->width ;
        }

        chMctnfCreateParams->ulComputeWidth =
            chCreateParams->sParams.computeWidth;
        chMctnfCreateParams->ulComputeHeight =
            chCreateParams->sParams.computeHeight;
        chMctnfCreateParams->ulOutputStrideLuma =
            chCreateParams->sParams.outputStrideLuma;
        chMctnfCreateParams->ulOutputStrideChroma =
            chCreateParams->sParams.outputStrideChroma;


        /*
         * Per channel bufInQue structure needs to be created & initialized.
         * bufInQue.fullQue will be populated with captured frames and
         * bufInQue.emptyQuewill be allocated here so that they can be
         * used later on for noise filtering.
         * Frames need to be allocated for bufInQue.emptyQue here.
         */
        /* Create the per channel bufInQue */
        status = Utils_bufCreate(&MctnfChObj->bufInQue, TRUE, FALSE);
        UTILS_assert(status == FVID2_SOK);

        /* Fill format with channel specific values to allocate frame */
        MctnfChObj->format.channelNum = chId;
        MctnfChObj->format.width = chMctnfCreateParams->ulComputeWidth;
		// align to 16 byte, to the higher side: like 1080 to 1088, not to 1072
        MctnfChObj->format.height = (((inChInfo->height+15)>>4)<<4);

        MctnfChObj->format.pitch[0] = chMctnfCreateParams->ulOutputStrideLuma;
        MctnfChObj->format.pitch[1] = chMctnfCreateParams->ulOutputStrideChroma;
        MctnfChObj->format.pitch[2] = 0;
        MctnfChObj->format.fieldMerged[0] = FALSE;
        MctnfChObj->format.fieldMerged[1] = FALSE;
        MctnfChObj->format.fieldMerged[2] = FALSE;
        MctnfChObj->format.dataFormat = FVID2_DF_YUV420SP_UV;
        MctnfChObj->format.scanFormat = FVID2_SF_PROGRESSIVE;
        MctnfChObj->format.bpp = FVID2_BPP_BITS8;            /* ignored */

        /* Increase the number of channels now */
        pObj->info.queInfo[queueId].numCh++;
    }

    return FVID2_SOK;
}

Int32 MctnfLink_drvCreate(MctnfLink_Obj * pObj, MctnfLink_CreateParams * pPrm)
{
    Int32 status;

    MCTNFLINK_INFO_LOG("MCTNF Create in progress ... !!!\n");

    /* Store the create params coming from the app */
    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->inFrameReceiveCount = 0;
    pObj->inFrameGivenCount = 0;
    pObj->outFrameReceiveCount = 0;
    pObj->outFrameGivenCount = 0;
    pObj->processFrameReqCount = 0;
    pObj->getProcessFrameReqCount = 0;
    pObj->totalTime = 0;
    pObj->curTime = 0;

    status = Utils_bufCreate(&pObj->bufOutQue, FALSE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    /* Copy previous link (capture) info */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* Copy previous link's queue info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));
    UTILS_assert(pObj->inQueInfo.numCh <= MCTNF_LINK_MAX_CH);

    pObj->info.numQue = 1;

    /*
     * Configure the channels, allocate frames for each channel and
     * put those frames in channel specific buffer queue.
     */
    MctnfLink_drvInitCh(pObj);

    /* TODO : support multi channels */
	/*
	 *  This does the codec instantiation
	 */
    status = MCTNF_Engine_Init(pObj);
    UTILS_assert(status == XDM_EOK);

    MCTNFLINK_INFO_LOG("MCTNF Create ... DONE !!!\n");

    return FVID2_SOK;
}

static Int32 MctnfLink_drvDoMctnfFilter(MctnfLink_Obj * pObj)
{
    FVID2_Frame *pFullFrame;

    FVID2_Frame *pEmptyFrame;

    Int32 status;

    UInt32 chId;

    UInt32 numFrames;
	
	UInt32 mctnfResetDone,startX,startY;

    MctnfLink_ChObj *pChObj;
    System_FrameInfo *pInFrameInfo;
    System_FrameInfo *pOutFrameInfo;
	System_LinkChInfo *inChInfo;

    numFrames = 0;

#ifdef _PROFILE
   static unsigned long int startTime = 0,stopTime = 0;
   static unsigned long int totalTimeProcess=0,cnt =0;
#endif

    MCTNFLINK_INFO_LOG_RT("MCTNF: MctnfLink_drvDoMctnfFilter !!!\n");

    pObj->tskEnv.size = sizeof(pObj->tskEnv);
    pObj->tskEnv.ivaID = 0;
    pObj->tskEnv.batchSize = 1;
    Task_setEnv(Task_self(), &pObj->tskEnv);


    /** For all the available channels, look into the respective
     * bufInQue.fullQueue and take the frames out of that queue.
     * Take the same number of frames from the respective bufInQue.emptyQueue.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->linkChInfo[chId];

        while (1)
        {
        	MctnfCreateParams *chMctnfCreateParams;
			MctnfLink_ChCreateParams *chCreateParams;

            status = Utils_bufGetFullFrame(&pChObj->bufInQue,
                                           &pFullFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
                break;

            /* First check whether it can be accomodated or not */
            status = Utils_bufGetEmptyFrame(&pChObj->bufInQue,
                                            &pEmptyFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
            {
            	pChObj->inFrameSkipCount++;
                MCTNFLINK_INFO_LOG_RT("MCTNF: Skip frames !!!\n");
                break;
            }
			pEmptyFrame->timeStamp = pFullFrame->timeStamp;
            pObj->processFrameReqCount++;

            pInFrameInfo = (System_FrameInfo *) pFullFrame->appData;
            pOutFrameInfo = (System_FrameInfo *) pEmptyFrame->appData;

			pOutFrameInfo->rtChInfoUpdate = FALSE;
			mctnfResetDone = 1;
			
            if ((pInFrameInfo != NULL) && (pInFrameInfo->rtChInfoUpdate == TRUE))
            {
				chMctnfCreateParams = &pChObj->chCreateArgs;
				chCreateParams = &pObj->createArgs.chCreateParams[chId];

				/* Temporal Hack For Full Resolution */
				if ((chMctnfCreateParams->ulInputWidth != (pInFrameInfo->rtChInfo.width)) ||
					(chMctnfCreateParams->ulInputHeight != pInFrameInfo->rtChInfo.height))
				{
					chMctnfCreateParams->ulInputWidth = pInFrameInfo->rtChInfo.width;
					chMctnfCreateParams->ulInputHeight = pInFrameInfo->rtChInfo.height;

					{
						chCreateParams->sParams.computeWidth = chMctnfCreateParams->ulInputWidth;
						chCreateParams->sParams.computeHeight = chMctnfCreateParams->ulInputHeight;
					}

					chMctnfCreateParams->ulComputeWidth =
						chCreateParams->sParams.computeWidth;
					chMctnfCreateParams->ulComputeHeight =
						chCreateParams->sParams.computeHeight;

					pOutFrameInfo->rtChInfo.width	 = chMctnfCreateParams->ulComputeWidth;
					pOutFrameInfo->rtChInfo.height	 = chMctnfCreateParams->ulComputeHeight;
					pOutFrameInfo->rtChInfo.pitch[0] = chMctnfCreateParams->ulOutputStrideLuma;
					pOutFrameInfo->rtChInfo.pitch[1] = chMctnfCreateParams->ulOutputStrideChroma;
					pOutFrameInfo->rtChInfoUpdate	 = TRUE;

					pInFrameInfo->rtChInfoUpdate = FALSE;

					MCTNF_Engine_Reset(pObj);
					
					mctnfResetDone = 1;					
	            }
            }

			/*
			 *	To support dynamic resolution change we have to set rtChInfoUpdate = TRUE for every frame.
			 */			
			pOutFrameInfo->rtChInfo.width	 = chMctnfCreateParams->ulComputeWidth;
			
			inChInfo = &pObj->inQueInfo.chInfo[chId];
			pOutFrameInfo->rtChInfo.height	 = inChInfo->height;	//chMctnfCreateParams->ulComputeHeight;
			
			pOutFrameInfo->rtChInfo.pitch[0] = chMctnfCreateParams->ulOutputStrideLuma;
			pOutFrameInfo->rtChInfo.pitch[1] = chMctnfCreateParams->ulOutputStrideChroma;
			pOutFrameInfo->rtChInfoUpdate	 = TRUE;			
			
            pObj->curTime = Clock_getTicks();

            MCTNFLINK_INFO_LOG_RT("Start %d MCTNF: Noise Filtering frame %d !!!\n",
                                pObj->curTime, numFrames);


#ifdef _PROFILE
            startTime = Utils_getCurTimeInUsec();
            cnt++;
#endif
			if (mctnfResetDone == 1)
			{
				status =
					MCTNF_ProcessFrame(pObj, pFullFrame, pEmptyFrame,  &pChObj->pPrevOutFrame );
			}
			else
			{
            	status =
                	MCTNF_ProcessFrame(pObj, pFullFrame, pEmptyFrame, &pChObj->pPrevOutFrame );
			}
            UTILS_assert(status == XDM_EOK);
#ifdef _PROFILE
            stopTime  = Utils_getCurTimeInUsec();
            totalTimeProcess += stopTime-startTime;
            if(cnt%100 == 0)
            {
               	Vps_printf("Total Time for Process = %ld\n",totalTimeProcess/100);
            	cnt = totalTimeProcess = 0;
			}
            startTime = Utils_getCurTimeInUsec();
#endif

            pObj->curTime = Clock_getTicks() - pObj->curTime;

            MCTNFLINK_INFO_LOG_RT
                ("Stop %d MCTNF: Noise Filtering of frame %d Done !!!\n",
                 pObj->curTime, numFrames);

            pObj->totalTime += pObj->curTime;

            pObj->getProcessFrameReqCount++;
			
			/*
			 *	Output frame address is adjusted for vertical and horizontal skips.
			 */								
			startX = pObj->linkChInfo[chId].status.vidnf1Status.startX;
			startY = pObj->linkChInfo[chId].status.vidnf1Status.startY;	
			
			pEmptyFrame->addr[0][0] = (Ptr)((UInt32)pEmptyFrame->addr[1][0] + (startX + (startY * chMctnfCreateParams->ulOutputStrideLuma)));
			pEmptyFrame->addr[0][1] = (Ptr)((UInt32)pEmptyFrame->addr[1][1] + (startX + (startY/2 * chMctnfCreateParams->ulOutputStrideChroma)));			
			
            /* Put the processed frames in the appropriate output queue */
            if (pChObj->pPrevOutFrame)
            {
                status =
                    Utils_bufPutFullFrame(&pObj->bufOutQue,
                                          pChObj->pPrevOutFrame);
                UTILS_assert(status == FVID2_SOK);
                MCTNFLINK_INFO_LOG_RT("MCTNF: Put frame %d to Out Que !!!\n",
                                    numFrames);

			    //Vps_printf("Pushed the frame 0x%x into the Out Queque \n", pChObj->pPrevOutFrame);
                numFrames++;
            }

            break;
        }
    }

    if (numFrames)
    {
        pObj->outFrameGivenCount += numFrames;

        /* Send data available message to next task */
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
	    //Vps_printf("Notified the next link Id 0x%x of the frame 0x%x \n", pObj->createArgs.outQueParams.nextLink, pChObj->pPrevOutFrame);
        MCTNFLINK_INFO_LOG_RT("MCTNF: Send %d frames to the next link !!!\n",
                            numFrames);
    }
    else
    {
        MCTNFLINK_INFO_LOG_RT("MCTNF: No frames available for filtering !!!\n");

        /* no more frame availble so exit the loop outside of this function */
        status = FVID2_EFAIL;
    }

    return status;
}

/* This function gets called when capture driver has captured frames. This
 * function takes those frames from the capture driver and stores them in the
 * channel specific buf-queue for noise filtering stage. */
Int32 MctnfLink_drvProcessData(MctnfLink_Obj * pObj)
{
    Int32 frameId;

    FVID2_FrameList frameList;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    Int32 status;

    MctnfLink_ChObj *pChObj;

    MCTNFLINK_INFO_LOG_RT("MCTNF: MctnfLink_drvProcessData !!!\n");
    /* Pointer to the input link's queue */
    pInQueParams = &pObj->createArgs.inQueParams;

    /* Get the captured frames from the capture driver */
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    MCTNFLINK_INFO_LOG_RT("MCTNF: %d frames in !!!\n", frameList.numFrames);
    if (frameList.numFrames)
    {
        pObj->inFrameReceiveCount += frameList.numFrames;

        /* For each captured frame, check the channel no and copy it
         * accordingly in the fullQueue. */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            UTILS_assert(pFrame->channelNum < pObj->inQueInfo.numCh);

            pChObj = &pObj->linkChInfo[pFrame->channelNum];
			
			if(pObj->createArgs.enable[pFrame->channelNum] == 1)
			{
				status =
					Utils_bufPutFullFrame(&pChObj->bufInQue, pFrame);
				UTILS_assert(status == FVID2_SOK);
			}
        }

        do
        {
            status = MctnfLink_drvDoMctnfFilter(pObj);
        }while (status == FVID2_SOK);

        /* Return frames back to the capture link as well */
        MCTNFLINK_INFO_LOG_RT("VNF    : Release %d IN frames !!!\n",
                            frameList.numFrames);
        pObj->inFrameGivenCount += frameList.numFrames;
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }
    return FVID2_SOK;
}

Int32 MctnfLink_drvStart(MctnfLink_Obj * pObj)
{
    return FVID2_SOK;
}

Int32 MctnfLink_drvStop(MctnfLink_Obj * pObj)
{
    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    Int32 status = FVID2_SOK;

    UInt32 chId;

    MctnfLink_ChObj *pChObj;

    MCTNFLINK_INFO_LOG("VNFLINK: Stop in progress !!!\n");

    pInQueParams = &pObj->createArgs.inQueParams;
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);
    if (frameList.numFrames)
    {
        /* Free input frames */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->linkChInfo[chId];
        if (pChObj->pPrevOutFrame)
        {
            status =
                Utils_bufPutEmptyFrame(&pChObj->bufInQue,
                                       pChObj->pPrevOutFrame);
            UTILS_assert(status == FVID2_SOK);
            pChObj->pPrevOutFrame = NULL;
        }
    }

    MCTNFLINK_INFO_LOG("VNFLINK: Stop done !!!\n");

    return (status);
}

Int32 MctnfLink_drvDelete(MctnfLink_Obj * pObj)
{
    UInt32 chId;

    MctnfLink_ChObj *vnfChObj;

    Int32 status;

    /* TODO : support multi channels */
    status = MCTNF_Engine_DeInit(pObj);
    UTILS_assert(status == XDM_EOK);

    MCTNFLINK_INFO_LOG("MCTNF: Delete in progress !!!\n");

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        vnfChObj = &pObj->linkChInfo[chId];

        /* Free the allocated frames */
        Utils_memFrameFree_Capture(&vnfChObj->format, vnfChObj->frames,
                                   MCTNF_LINK_MAX_OUT_FRAMES_PER_CH,0,0);

        /* Free the channel specific buf-queue */
        Utils_bufDelete(&vnfChObj->bufInQue);
    }

    /* Free the VNF link output queues */
    Utils_bufDelete(&pObj->bufOutQue);

    MCTNFLINK_INFO_LOG("MCTNF: Delete done !!!\n");

    return FVID2_SOK;
}

/* This function will be called from the next link in the chain when the
 * frames are no more required. This function restores channel number for
 * each frame from the appData and pushes the frame back to the channel
 * specific bufInQue.emptyQueue. */
Int32 MctnfLink_drvReleaseFrames(MctnfLink_Obj * pObj, FVID2_FrameList * pFrameList)
{
    Int32 status;

    UInt32 frameId;

    FVID2_Frame *pFrame;

    System_FrameInfo *pFrameInfo;

    pObj->outFrameReceiveCount += pFrameList->numFrames;
    if (pFrameList->numFrames)
    {
        for (frameId = 0; frameId < pFrameList->numFrames; frameId++)
        {
            pFrame = pFrameList->frames[frameId];

            /* Channel number might have changed, restore it with the
             * orignial stored value. */
            pFrameInfo = (System_FrameInfo *) pFrame->appData;
            UTILS_assert(pFrameInfo != NULL);
            pFrame->channelNum = pFrameInfo->vnfChannelNum;

            UTILS_assert(pFrame->channelNum < pObj->inQueInfo.numCh);

            /* Return each frame to its original channel specific
             * MctnfLink_ChObj.bufInQue.emptyQue. */
            status =
                Utils_bufPutEmptyFrame(&pObj->
                                       linkChInfo[pFrame->channelNum].bufInQue,
                                       pFrame);
            UTILS_assert(status == FVID2_SOK);

            MCTNFLINK_INFO_LOG_RT
                ("VNF    : Release frame %d to VNF channel !!!\n", frameId);
        }

        MCTNFLINK_INFO_LOG_RT("MCTNF: Returned %d frames to VNF channel %d !!!\n",
                            pFrameList->numFrames, pFrame->channelNum);
    }

    return FVID2_SOK;
}

Int32 MctnfLink_drvSetStrength(MctnfLink_Obj * pObj, MCTNF_LINK_VNF_STRENGTH strength)
{
	Int32 status = FVID2_SOK;

	UInt key;

	//UInt32 chId;

	MCTNFLINK_INFO_LOG("SetStrength current strength %d => new %d!!!\n",
					 pObj->createArgs.chCreateParams[0].strength, strength);

	key = Hwi_disable();
	//for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
	//	pObj->createArgs.chCreateParams[chId].strength = strength;
	//	pObj->linkChInfo[chId].chCreateArgs.strength = (MSP_VNF_STRENGTH)strength;
	}
	Hwi_restore(key);

	return status;
}


Int32 MctnfLink_drvGetDynamicParams(MctnfLink_Obj * pObj,
                                  MctnfLink_ChDynamicParams * params)
{
    UInt32 chId = params->chId;

    if (chId < pObj->inQueInfo.numCh)
    {
        memcpy(&params->motionParams, &pObj->linkChInfo[chId].dynParams.motionSearchParams,
               sizeof(MctnfLink_MotionSearchParams));
        memcpy(&params->noiseFilterParams, &pObj->linkChInfo[chId].dynParams.noiseFilterParams,
               sizeof(MctnfLink_NoiseFilterParams));
        return FVID2_SOK;
    }

    return FVID2_EINVALID_PARAMS;
}

Int32 MctnfLink_drvGetStaticParams(MctnfLink_Obj * pObj,
                                 MctnfLink_ChStaticParams * params)
{
    UInt32 chId = params->chId;

    if (chId < pObj->inQueInfo.numCh)
    {
        memcpy(params, &pObj->createArgs.chCreateParams[chId].sParams,
               sizeof(*params));
        return FVID2_SOK;
    }

    return FVID2_EINVALID_PARAMS;
}

Int32 MctnfLink_drvSetDynamicParams(MctnfLink_Obj * pObj,
                                  MctnfLink_ChDynamicParams * params)
{
    UInt key;

    UInt32 chId = params->chId;

    MCTNFLINK_INFO_LOG("SetDynamicParams ch %d features %d!!!\n", chId,
                     params->featureMask);

    if (chId < pObj->inQueInfo.numCh)
    {
        key = Hwi_disable();

        {
			memcpy( &pObj->linkChInfo[chId].dynParams.motionSearchParams, &params->motionParams,
				   sizeof(MctnfLink_MotionSearchParams));
			memcpy( &pObj->linkChInfo[chId].dynParams.noiseFilterParams, &params->noiseFilterParams,
				   sizeof(MctnfLink_NoiseFilterParams));

			pObj->isControlToBeCalled = 1;
		}
        Hwi_restore(key);
        return FVID2_SOK;
    }

    return FVID2_EINVALID_PARAMS;

}


void  MctnfLink_drvUpdateDCC( MctnfRunTimeParams *pMctnfRunTimeParams)
{
			Int32 tempMinWtForCurQ8;
            dcc_parser_input_params_t input_params;

			      dcc_parser_output_params_t output_params;
            //int i, j;
            //extern Int8 dcc_buf[];

			      output_params.iss_drv_config   = NULL;

            input_params.dcc_buf           = (Int8 *)gDccPrm_for_mctnf.dcc_Default_Param;

            input_params.dcc_buf_size      = gDccPrm_for_mctnf.dccSize;

            input_params.color_temparature = ti2a_output_params_for_mctnf.colorTemparaure;

            input_params.exposure_time     = ti2a_output_params_for_mctnf.sensorExposure;

            input_params.analog_gain       = (ti2a_output_params_for_mctnf.sensorGain / 1000) * (ti2a_output_params_for_mctnf.ipipe_awb_gain.dGain/256);

		    output_params.pMctnfParam	   = pMctnfRunTimeParams;
            output_params.pGlbceParams     = NULL;
			output_params.pVnfParam   	   = NULL;
#if 0
			int j;
            Vps_printf("input_params.dcc_buf_size = %d input_params.dcc_buf = %x \n",input_params.dcc_buf_size,input_params.dcc_buf);

            if(input_params.dcc_buf_size)
            {
              for(j = 0; (j < 100*16) && (j < input_params.dcc_buf_size); j+= 16)
              {
                  Vps_printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                                       (unsigned char)input_params.dcc_buf[j],(unsigned char)input_params.dcc_buf[j+1],(unsigned char)input_params.dcc_buf[j+2],(unsigned char)input_params.dcc_buf[j+3],(unsigned char)input_params.dcc_buf[j+4],(unsigned char)input_params.dcc_buf[j+5],(unsigned char)input_params.dcc_buf[j+6],(unsigned char)input_params.dcc_buf[j+7],
                                       (unsigned char)input_params.dcc_buf[j+8],(unsigned char)input_params.dcc_buf[j+9],(unsigned char)input_params.dcc_buf[j+10],(unsigned char)input_params.dcc_buf[j+11],(unsigned char)input_params.dcc_buf[j+12],(unsigned char)input_params.dcc_buf[j+13],(unsigned char)input_params.dcc_buf[j+14],(unsigned char)input_params.dcc_buf[j+15]);
              }
            }
#endif
			/* minWtForCurQ8 is used to turn ON/OFF McTNF */
			tempMinWtForCurQ8 = pMctnfRunTimeParams->pMctnfQueryParam->noiseFilterParams.minWtForCurQ8;
            dcc_update(&input_params, &output_params);
			pMctnfRunTimeParams->pMctnfQueryParam->noiseFilterParams.minWtForCurQ8 = tempMinWtForCurQ8;
#if 0
            Vps_printf("pMctnfRunTimeParams->mctnf_dcc_params_exist = %d \n",pMctnfRunTimeParams->mctnf_dcc_params_exist);
#endif


			return;
}


Int32 MctnfLink_enableChannel(MctnfLink_Obj *pObj,UInt32 channelNum)
{
	if(channelNum < pObj->inQueInfo.numCh)
	{
		pObj->createArgs.enable[channelNum] = 1;
	}
	
	return FVID2_SOK;
} 
 

Int32 MctnfLink_disableChannel(MctnfLink_Obj *pObj,UInt32 channelNum)
{
	if(channelNum < pObj->inQueInfo.numCh)
	{
		pObj->createArgs.enable[channelNum] = 0;
	}
	
	return FVID2_SOK;
}
