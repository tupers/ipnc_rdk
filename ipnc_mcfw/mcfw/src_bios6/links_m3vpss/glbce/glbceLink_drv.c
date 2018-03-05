/** ==================================================================
 *  @file   glbceLink_drv.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/glbce/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <mcfw/src_bios6/utils/utils_mem.h>

#include "glbceLink_priv.h"
#include "glbceLink_glbce.h"
#define GLBCE_ENGINE_INIT_ONLY_ONCE

UInt32 gGlbceEnable = 0;
extern Iss_CaptDccPrm ggDccPrm;
extern ti2a_output ti2a_output_params;

/* ===================================================================
 *  @func     GlbceLink_drvInitCh
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
static Int32 GlbceLink_drvInitCh(GlbceLink_Obj * pObj)
{
    Int32 status;

    UInt32 chId, frameId, queueId = GLBCE_LINK_OUT_QUE_ID;

    FVID2_Frame *frame;

    GlbceLink_ChObj *GlbceChObj;

    GlbceCreateParams *chGlbceCreateParams;

    GlbceLink_ChCreateParams *chCreateParams;

    System_LinkChInfo *inChInfo, *outChInfo;

	gGlbceEnable = pObj->createArgs.glbceEnable;

    /*
     * For each channel, do the initialization and
     * FVID2 frame / buffer allocation.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        GlbceChObj = &pObj->linkChInfo[chId];
        chGlbceCreateParams = &GlbceChObj->chCreateArgs;
        chCreateParams = &pObj->createArgs.chCreateParams[chId];
        inChInfo = &pObj->inQueInfo.chInfo[chId];
        outChInfo = &pObj->info.queInfo[queueId].chInfo[chId];

        chCreateParams->dParams.featureMask = 0;
        chCreateParams->dParams.chId = chId;
        chCreateParams->sParams.chId = chId;

        /* Set the channel number first */
        GlbceChObj->channelId = chId;
        GlbceChObj->inFrameSkipCount = 0;
        GlbceChObj->nextFid = FVID2_FID_TOP;
        GlbceChObj->disable = FALSE;

        GLBCELINK_INFO_LOG
            ("Input Channel CH%d of %d x %d, pitch = (%d, %d) [%d,%d][%s] [%d] ... \n",
             chId, inChInfo->width, inChInfo->height, inChInfo->pitch[0],
             inChInfo->pitch[1], inChInfo->startX, inChInfo->startY,
             gSystem_nameScanFormat[inChInfo->scanFormat],
             inChInfo->dataFormat);


	    //Vps_rprintf("GLBCEEEEEEEEEEEEEEEEE Width is %d and Height is %d \n", inChInfo->width, inChInfo->height);

        chGlbceCreateParams->ulInputWidth = inChInfo->width;
        chGlbceCreateParams->ulInputHeight = inChInfo->height;

        chGlbceCreateParams->ulInputStride = inChInfo->pitch[0];
        /*switch (inChInfo->dataFormat)
        {
            case FVID2_DF_YUV422I_UYVY:
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_YVYU:
            case FVID2_DF_YUV422I_VYUY:
                chGlbceCreateParams->eInputFormat = MSP_GLBCE_YUV_FORMAT_YCBCR422;
                 break;
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                chGlbceCreateParams->eInputFormat = MSP_GLBCE_YUV_FORMAT_YCBCR420;
                chGlbceCreateParams->ulInputStrideChroma = inChInfo->pitch[1];
                break;
        }*/
		UTILS_assert(inChInfo->dataFormat == FVID2_DF_BAYER_RAW || inChInfo->dataFormat == SYSTEM_DF_BAYER_RAW); //TO BE BAYER

        /*switch (chCreateParams->sParams.eOutputFormat)
        {
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                chGlbceCreateParams->eOutputFormat = MSP_GLBCE_YUV_FORMAT_YCBCR420;
                break;
        }*/

        //chGlbceCreateParams->eOperateMode =
        //    (MSP_GLBCE_OPERATION_MODE) chCreateParams->sParams.eOperateMode;

        /* In the current version, we will not allow free dimension settings */
		/*if (chGlbceCreateParams->eOperateMode != MODE_TNF)
		{
			chCreateParams->sParams.computeWidth = inChInfo->width - 32;
			chCreateParams->sParams.computeHeight = inChInfo->height - 32;
			chCreateParams->sParams.outputStrideLuma = inChInfo->width - 32;
			chCreateParams->sParams.outputStrideChroma = inChInfo->width - 32;
		}
		else*/
		{
			chCreateParams->sParams.computeWidth = inChInfo->width;
			chCreateParams->sParams.computeHeight = inChInfo->height;
			chCreateParams->sParams.outputStrideLuma = inChInfo->width;
			chCreateParams->sParams.outputStrideChroma = inChInfo->width;
		}

        chGlbceCreateParams->ulComputeWidth =
            chCreateParams->sParams.computeWidth;
        chGlbceCreateParams->ulComputeHeight =
            chCreateParams->sParams.computeHeight;
        chGlbceCreateParams->ulOutputStrideLuma =
            chCreateParams->sParams.outputStrideLuma;
        chGlbceCreateParams->ulOutputStrideChroma =
            chCreateParams->sParams.outputStrideChroma;

        /*
         * Per channel bufInQue structure needs to be created & initialized.
         * bufInQue.fullQue will be populated with captured frames and
         * bufInQue.emptyQuewill be allocated here so that they can be
         * used later on for noise filtering.
         * Frames need to be allocated for bufInQue.emptyQue here.
         */
        /* Create the per channel bufInQue */
        status = Utils_bufCreate(&GlbceChObj->bufInQue, TRUE, FALSE);
        UTILS_assert(status == FVID2_SOK);

        /* Fill format with channel specific values to allocate frame */
        GlbceChObj->format.channelNum = chId;
        GlbceChObj->format.width = chGlbceCreateParams->ulComputeWidth;
        /*if (chGlbceCreateParams->eOperateMode != MODE_TNF)
            GlbceChObj->format.height = chGlbceCreateParams->ulComputeHeight + 32;
        else*/
            GlbceChObj->format.height = (chGlbceCreateParams->ulComputeHeight+8) & 0xfffffff0;

        GlbceChObj->format.pitch[0] = chGlbceCreateParams->ulOutputStrideLuma*2;
        GlbceChObj->format.pitch[1] = chGlbceCreateParams->ulOutputStrideChroma;
        GlbceChObj->format.pitch[2] = 0;
        GlbceChObj->format.fieldMerged[0] = FALSE;
        GlbceChObj->format.fieldMerged[1] = FALSE;
        GlbceChObj->format.fieldMerged[2] = FALSE;
		//Vps_rprintf("I have reached this place!!!!!!!! \n");
        GlbceChObj->format.dataFormat = FVID2_DF_BAYER_RAW;//SYSTEM_DF_BAYER_RAW;  ///REVISIT
        // format.dataFormat = chGlbceCreateParams->eOutputFormat;
        GlbceChObj->format.scanFormat = FVID2_SF_PROGRESSIVE; //REVISIT
        // format.scanFormat = inChInfo->scanFormat;
        GlbceChObj->format.bpp = FVID2_BPP_BITS8;     //REVISIT       /* ignored */

        /*
         * Alloc memory based on 'format'
         * Allocated frame info is put in frames[]
         */
        Utils_memFrameAlloc(&GlbceChObj->format, GlbceChObj->frames,
                            GLBCE_LINK_MAX_OUT_FRAMES_PER_CH);

        /* Set remaining parameters for the allocated frames */
        for (frameId = 0; frameId < GLBCE_LINK_MAX_OUT_FRAMES_PER_CH; frameId++)
        {
            /* Point to the frame's array */
            frame = &GlbceChObj->frames[frameId];
            frame->perFrameCfg = NULL;
            frame->subFrameInfo = NULL;
            frame->appData = &GlbceChObj->frameInfo[frameId];

            GlbceChObj->frameInfo[frameId].glbceChannelNum = frame->channelNum;

            /* Finally, add this frame to the GlbceLink_ChObj.bufInQue.emptyQue
             */
            status = Utils_bufPutEmptyFrame(&GlbceChObj->bufInQue, frame);
            UTILS_assert(status == FVID2_SOK);
        }

        /* no previous frame for the first frame */
        GlbceChObj->pPrevOutFrame = NULL;

        /* Populate the remaining fileds of
         * GlbceLink_Obj.System_LinkInfo.System_LinkQueInfo.System_LinkChInfo
         * This information will be used by the next link to configure itself
         * properly. */
        UTILS_assert(queueId < pObj->info.numQue);

        outChInfo->bufType = SYSTEM_BUF_TYPE_VIDFRAME; //REVISIT
        outChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM; //REVISIT
        outChInfo->dataFormat = GlbceChObj->format.dataFormat;
        outChInfo->scanFormat = GlbceChObj->format.scanFormat;

        outChInfo->startX = 0;
        outChInfo->startY = 0;

        outChInfo->width = chGlbceCreateParams->ulComputeWidth;
        outChInfo->height = chGlbceCreateParams->ulComputeHeight;
        outChInfo->pitch[0] = GlbceChObj->format.pitch[0];
        outChInfo->pitch[1] = GlbceChObj->format.pitch[1];
        outChInfo->pitch[2] = GlbceChObj->format.pitch[2];

        /* Increase the number of channels now */
        pObj->info.queInfo[queueId].numCh++;

		pObj->frequencyTonalCurve =  (gGlbceEnable && gGlbceEnable > GLBCE_HS_FH ) ? 3 : 100;//chCreateParams->frequencyTonalCurve;
		pObj->frameNum = 0;


    }



    return FVID2_SOK;
}

/* ===================================================================
 *  @func     GlbceLink_drvCreate
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
Int32 GlbceLink_drvCreate(GlbceLink_Obj * pObj, GlbceLink_CreateParams * pPrm)
{
    Int32 status;

    GLBCELINK_INFO_LOG("Create in progress ... !!!\n");

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
    UTILS_assert(pObj->inQueInfo.numCh <= GLBCE_LINK_MAX_CH);

    pObj->info.numQue = 1;

    /*
     * Configure the channels, allocate frames for each channel and
     * put those frames in channel specific buffer queue.
     */
    GlbceLink_drvInitCh(pObj);

#ifdef GLBCE_ENGINE_INIT_ONLY_ONCE
    if(gGlbceEnable)
	{
	/* TODO : support multi channels */
    status = MSP_GLBCE_Engine_Init(pObj);
    UTILS_assert(status == MSP_ERROR_NONE);
	}
#endif

    GLBCELINK_INFO_LOG("Create ... DONE !!!\n");

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     GlbceLink_drvDoGlbceFilter
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
//static 			   int cnt = 0;
static Int32 GlbceLink_drvDoGlbceFilter(GlbceLink_Obj * pObj)
{
    FVID2_Frame *pFullFrame;

    FVID2_Frame *pEmptyFrame;

    Int32 status;

    UInt32 chId;

    UInt32 numFrames;

    GlbceLink_ChObj *pChObj;

    numFrames = 0;

    GLBCELINK_INFO_LOG_RT("GLBCE: GlbceLink_drvDoGlbceFilter !!!\n");

    /** For all the available channels, look into the respective
     * bufInQue.fullQueue and take the frames out of that queue.
     * Take the same number of frames from the respective bufInQue.emptyQueue.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->linkChInfo[chId];

        while (1)
        {
            status = Utils_bufGetFullFrame(&pChObj->bufInQue,
                                           &pFullFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
                break;

            /* First check whether it can be accomodated or not */
            status = Utils_bufGetEmptyFrame(&pChObj->bufInQue,
                                            &pEmptyFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
            {
                GLBCELINK_INFO_LOG_RT("GLBCE: Skip frames !!!\n");
                break;
            }

            pObj->processFrameReqCount++;

            pObj->curTime = Clock_getTicks();

            GLBCELINK_INFO_LOG_RT("Start %d GLBCE: Noise Filtering frame %d !!!\n",
                                pObj->curTime, numFrames);

#ifndef GLBCE_ENGINE_INIT_ONLY_ONCE
            /* TODO : support multi channels */
			//if(cnt)
            if(gGlbceEnable)
			{
            status = MSP_GLBCE_Engine_Init(pObj);
            UTILS_assert(status == MSP_ERROR_NONE);
			}
#endif
    pEmptyFrame->timeStamp = pFullFrame->timeStamp;
    //Vps_rprintf("Opening MSP_GLBCE_Engine_Open \n");
    if(gGlbceEnable)
    {
        	status =
                MSP_GLBCE_Engine_Open(pObj);
            UTILS_assert(status == MSP_ERROR_NONE);
    }
    //Vps_rprintf("Opened MSP_GLBCE_Engine_Open \n");
    if(gGlbceEnable /*&& (pObj->frameNum == 0)*/)
    {
    //Vps_rprintf("Calling MSP_GLBCE_ProcessFrame \n");
            status =
                MSP_GLBCE_ProcessFrame(pObj,
                                     pFullFrame, pEmptyFrame);
            UTILS_assert(status == FVID2_SOK);
    //Vps_rprintf("Came out of MSP_GLBCE_ProcessFrame \n");
	}
	else
	{
	   pEmptyFrame = pFullFrame; //All pass filter
	}

    if(gGlbceEnable)
    {
    //Vps_rprintf("Calling MSP_GLBCE_Engine_Close \n");
            status = MSP_GLBCE_Engine_Close(pObj);
    //Vps_rprintf("Came out of MSP_GLBCE_Engine_Close \n");
            UTILS_assert(status == MSP_ERROR_NONE);
    }

#ifndef GLBCE_ENGINE_INIT_ONLY_ONCE
            /* GLBCE handle */
    if(gGlbceEnable)
            MSP_GLBCE_Engine_DeInit(pObj);
#endif
            pObj->curTime = Clock_getTicks() - pObj->curTime;

            GLBCELINK_INFO_LOG_RT
                ("Stop %d GLBCE: Noise Filtering of frame %d Done !!!\n",
                 pObj->curTime, numFrames);

            pObj->totalTime += pObj->curTime;

            pObj->getProcessFrameReqCount++;

            /* next previous output frame is current output frame */
            pChObj->pPrevOutFrame = pEmptyFrame;

            /* Put the processed frames in the appropriate output queue */
            if (pChObj->pPrevOutFrame)
            {
                status =
                    Utils_bufPutFullFrame(&pObj->bufOutQue,
                                          pChObj->pPrevOutFrame);
                UTILS_assert(status == FVID2_SOK);
                GLBCELINK_INFO_LOG_RT("GLBCE: Put frame %d to Out Que !!!\n",
                                    numFrames);
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
        GLBCELINK_INFO_LOG_RT("GLBCE: Send %d frames to the next link !!!\n",
                            numFrames);
    }
    else
    {
        GLBCELINK_INFO_LOG_RT("GLBCE: No frames available for filtering !!!\n");

        /* no more frame availble so exit the loop outside of this function */
        status = FVID2_EFAIL;
    }

    return status;
}

/* This function gets called when capture driver has captured frames. This
 * function takes those frames from the capture driver and stores them in the
 * channel specific buf-queue for noise filtering stage. */
/* ===================================================================
 *  @func     GlbceLink_drvProcessData
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
Int32 GlbceLink_drvProcessData(GlbceLink_Obj * pObj)
{
    Int32 frameId;

    FVID2_FrameList frameList;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    Int32 status;

    GlbceLink_ChObj *pChObj;

    FVID2_Frame *framePerCh[GLBCE_LINK_MAX_CH];

    Int32 chId;

    GLBCELINK_INFO_LOG_RT("GLBCE: GlbceLink_drvProcessData !!!\n");
    /* Pointer to the input link's queue */
    pInQueParams = &pObj->createArgs.inQueParams;

    /* Get the captured frames from the capture driver */
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    GLBCELINK_INFO_LOG_RT("GLBCE: %d frames in !!!\n", frameList.numFrames);
    if (frameList.numFrames)
    {
        pObj->inFrameReceiveCount += frameList.numFrames;

        for (chId = 0; chId < GLBCE_LINK_MAX_CH; chId++)
        {
            framePerCh[chId] = NULL;
        }

        /* For each captured frame, check the channel no and copy it
         * accordingly in the fullQueue. */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            UTILS_assert(pFrame->channelNum < pObj->inQueInfo.numCh);

            pChObj = &pObj->linkChInfo[pFrame->channelNum];
            if (pChObj->format.scanFormat == FVID2_SF_PROGRESSIVE)
            {
                pChObj->nextFid = pFrame->fid;
            }

            if (pChObj->nextFid == pFrame->fid)
            {
                if (framePerCh[pFrame->channelNum] == NULL)
                {
                    framePerCh[pFrame->channelNum] = pFrame;
                    // frame is of the expected FID use it, else drop it
                    pChObj->nextFid ^= 1;                  // toggle to next
                    // required FID
                }
                else
                {
                    if (framePerCh[pFrame->channelNum]->fid == pFrame->fid)
                    {
                        framePerCh[pFrame->channelNum] = pFrame;
                        pChObj->inFrameSkipCount++;
                        GLBCELINK_INFO_LOG_RT("GLBCE: Skip frame same FID !!!\n");
                    }
                    else
                    {
                        pChObj->inFrameSkipCount++;
                        GLBCELINK_INFO_LOG_RT
                            ("GLBCE: Skip frame different FID !!!\n");
                    }
                }
            }
            else
            {
                GLBCELINK_INFO_LOG_RT("GLBCE: Skip frame %d !!!\n", frameId);
                pChObj->inFrameSkipCount++;
                // frame is not of expected FID, so skip it
            }
        }

        for (chId = 0; chId < GLBCE_LINK_MAX_CH; chId++)
        {
            if (framePerCh[chId] != NULL)
            {
                status =
                    Utils_bufPutFullFrame(&pChObj->bufInQue, framePerCh[chId]);
                UTILS_assert(status == FVID2_SOK);

                GLBCELINK_INFO_LOG_RT("GLBCE: Put frame %d to In Que !!!\n",
                                    frameId);
                break;
            }
        }

        //Vps_rprintf("Calling GlbceLink_drvDoGlbceFilter\n")           ;
        /* with the previous frame skipping algo, we are sure that there is
         * only one frame to process per channel */
        // do
        {
            status = GlbceLink_drvDoGlbceFilter(pObj);
        }                                                  // while (status
        // == FVID2_SOK);
        //Vps_rprintf("Done Calling GlbceLink_drvDoGlbceFilter\n")           ;

        /* Return frames back to the capture link as well */
        GLBCELINK_INFO_LOG_RT("GLBCE	: Release %d IN frames !!!\n",
                            frameList.numFrames);
        pObj->inFrameGivenCount += frameList.numFrames;
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     GlbceLink_drvStart
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
Int32 GlbceLink_drvStart(GlbceLink_Obj * pObj)
{
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     GlbceLink_drvStop
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
Int32 GlbceLink_drvStop(GlbceLink_Obj * pObj)
{
    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    Int32 status = FVID2_SOK;

    UInt32 chId;

    GlbceLink_ChObj *pChObj;

    GLBCELINK_INFO_LOG("GLBCELINK: Stop in progress !!!\n");

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

    GLBCELINK_INFO_LOG("GLBCELINK: Stop done !!!\n");

    return (status);
}

/* ===================================================================
 *  @func     GlbceLink_drvDelete
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
Int32 GlbceLink_drvDelete(GlbceLink_Obj * pObj)
{
    UInt32 chId;

    GlbceLink_ChObj *glbceChObj;

#ifdef GLBCE_ENGINE_INIT_ONLY_ONCE
    Int32 status;

    if(gGlbceEnable)
	{
	/* TODO : support multi channels */
    status = MSP_GLBCE_Engine_DeInit(pObj);
    UTILS_assert(status == MSP_ERROR_NONE);
	}
#endif

    GLBCELINK_INFO_LOG("GLBCE: Delete in progress !!!\n");

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        glbceChObj = &pObj->linkChInfo[chId];

        /* Free the allocated frames */
        Utils_memFrameFree(&glbceChObj->format, glbceChObj->frames,
                           GLBCE_LINK_MAX_OUT_FRAMES_PER_CH);

        /* Free the channel specific buf-queue */
        Utils_bufDelete(&glbceChObj->bufInQue);
    }

    /* Free the GLBCE link output queues */
    Utils_bufDelete(&pObj->bufOutQue);

    GLBCELINK_INFO_LOG("GLBCE: Delete done !!!\n");

    return FVID2_SOK;
}

/* This function will be called from the next link in the chain when the
 * frames are no more required. This function restores channel number for
 * each frame from the appData and pushes the frame back to the channel
 * specific bufInQue.emptyQueue. */
/* ===================================================================
 *  @func     GlbceLink_drvReleaseFrames
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
Int32 GlbceLink_drvReleaseFrames(GlbceLink_Obj * pObj, FVID2_FrameList * pFrameList)
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
            pFrame->channelNum = pFrameInfo->glbceChannelNum;

            UTILS_assert(pFrame->channelNum < pObj->inQueInfo.numCh);

            /* Return each frame to its original channel specific
             * GlbceLink_ChObj.bufInQue.emptyQue. */
            status =
                Utils_bufPutEmptyFrame(&pObj->
                                       linkChInfo[pFrame->channelNum].bufInQue,
                                       pFrame);
            UTILS_assert(status == FVID2_SOK);

            GLBCELINK_INFO_LOG_RT
                ("GLBCE	: Release frame %d to GLBCE channel !!!\n", frameId);
        }

        GLBCELINK_INFO_LOG_RT("GLBCE: Returned %d frames to GLBCE channel %d !!!\n",
                            pFrameList->numFrames, pFrame->channelNum);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     GlbceLink_drvGetDynamicParams
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
Int32 GlbceLink_drvGetDynamicParams(GlbceLink_Obj * pObj,
                                  GlbceLink_ChDynamicParams * params)
{
    UInt32 chId = params->chId;

    if (chId < pObj->inQueInfo.numCh)
    {
        memcpy(params, &pObj->createArgs.chCreateParams[chId].dParams,
               sizeof(*params));
        return FVID2_SOK;
    }

    return FVID2_EINVALID_PARAMS;
}

/* ===================================================================
 *  @func     GlbceLink_drvGetStaticParams
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
Int32 GlbceLink_drvGetStaticParams(GlbceLink_Obj * pObj,
                                 GlbceLink_ChStaticParams * params)
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

/* ===================================================================
 *  @func     GlbceLink_drvSetDynamicParams
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
Int32 GlbceLink_drvSetDynamicParams(GlbceLink_Obj * pObj,
                                  GlbceLink_PresetDynamicParams * params)
{
    //GlbceLink_ChDynamicParams *dParams;

    //GlbceCreateParams *chGlbceCreateParams;

    UInt key;

    //UInt32 chId = 0;//params->chId;

    //GLBCELINK_INFO_LOG("SetDynamicParams ch %d features %d!!!\n", chId,
    //                 params->featureMask);

    //if (chId < pObj->inQueInfo.numCh)
    {
        //dParams = &pObj->createArgs.chCreateParams[chId].dParams;
        //chGlbceCreateParams = &pObj->linkChInfo[chId].chCreateArgs;
        key = Hwi_disable();

		gGlbceEnable = params->glbceEnable;
		pObj->frequencyTonalCurve =  (gGlbceEnable && gGlbceEnable > GLBCE_HS_FH ) ? 3 : 100;//chCreateParams->frequencyTonalCurve;

		Vps_rprintf(" gGlbceEnable got updated to value of %d \n", gGlbceEnable);


        Hwi_restore(key);
        return FVID2_SOK;
    }

    //return FVID2_EINVALID_PARAMS;
}



void  GlbceLink_drvUpdateDCC( GlbceRunTimeParams *pGlbceParam)
{
            dcc_parser_input_params_t input_params;

			dcc_parser_output_params_t output_params;
            extern iss_drv_config_t *iss_drv_config;
            //int i, j;
            extern Int8 dcc_buf[];

			output_params.iss_drv_config   = iss_drv_config;

            input_params.dcc_buf           = (Int8 *)ggDccPrm.dcc_Default_Param;

            input_params.dcc_buf_size      = ggDccPrm.dccSize;

            input_params.color_temparature = ti2a_output_params.colorTemparaure;

            input_params.exposure_time     = ti2a_output_params.sensorExposure;

            input_params.analog_gain       = (ti2a_output_params.sensorGain / 1000) * (ti2a_output_params.ipipe_awb_gain.dGain/256);

			output_params.pVnfParam      = NULL;
			output_params.pMctnfParam    = NULL;
            output_params.pGlbceParams   = pGlbceParam;
#if 0
            Vps_printf("input_params.dcc_buf_size = %d input_params.dcc_buf = %x \n",input_params.dcc_buf_size,input_params.dcc_buf);

#endif
			dcc_update(&input_params, &output_params);
#if 0
            Vps_printf("pGlbceParam->glbce_dcc_params_exist = %d \n",pGlbceParam->glbce_dcc_params_exist);
            Vps_printf("pGlbceParam->gce_strength = %d \n",pGlbceParam->gce_strength);
            Vps_printf("pGlbceParam->lce_strength = %d \n",pGlbceParam->lce_strength);
            Vps_printf("pGlbceParam->gbe_strength = %d \n",pGlbceParam->gbe_strength);
            Vps_printf("pGlbceParam->lbe_strength = %d \n",pGlbceParam->lbe_strength);

#endif


			return;
}

