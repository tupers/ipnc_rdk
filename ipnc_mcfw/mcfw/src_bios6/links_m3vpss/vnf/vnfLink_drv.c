/** ==================================================================
 *  @file   vnfLink_drv.c
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

#include "vnfLink_priv.h"
#define VNF_ENGINE_INIT_ONLY_ONCE
extern Iss_CaptDccPrm gDccPrm;
extern ti2a_output ti2a_output_params;
//#define _PROFILE
/* ===================================================================
 *  @func     VnfLink_drvInitCh
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
static Int32 VnfLink_drvInitCh(VnfLink_Obj * pObj)
{
    Int32 status;

    UInt32 chId, frameId, queueId = VNF_LINK_OUT_QUE_ID;

    FVID2_Frame *frame;

    VnfLink_ChObj *VnfChObj;

    VnfCreateParams *chVnfCreateParams;

    VnfLink_ChCreateParams *chCreateParams;

    System_LinkChInfo *inChInfo, *outChInfo;

    /*
     * For each channel, do the initialization and
     * FVID2 frame / buffer allocation.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        VnfChObj = &pObj->linkChInfo[chId];
        chVnfCreateParams = &VnfChObj->chCreateArgs;
        chCreateParams = &pObj->createArgs.chCreateParams[chId];
        inChInfo = &pObj->inQueInfo.chInfo[chId];
        outChInfo = &pObj->info.queInfo[queueId].chInfo[chId];

        chCreateParams->dParams.featureMask = 0;
        chCreateParams->dParams.chId = chId;
        chCreateParams->sParams.chId = chId;

        /* Set the channel number first */
        VnfChObj->channelId = chId;
        VnfChObj->inFrameSkipCount = 0;
        VnfChObj->nextFid = FVID2_FID_TOP;
        VnfChObj->disable = FALSE;

		if(pObj->createArgs.enable[chId] == 0)
		{
			/*
			 * If the channel is disabled for VNF then VNF is not there in the data path.
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
				inChInfo->width     += 32;
				inChInfo->height    += 32;
			}	
		}		
		
        VNFLINK_INFO_LOG
            ("Input Channel CH%d of %d x %d, pitch = (%d, %d) [%d,%d][%s] [%d] ... \n",
             chId, inChInfo->width, inChInfo->height, inChInfo->pitch[0],
             inChInfo->pitch[1], inChInfo->startX, inChInfo->startY,
             gSystem_nameScanFormat[inChInfo->scanFormat],
             inChInfo->dataFormat);

#if defined(USE_MCTNF) && !defined(VNF_BEFORE_MCTNF)
		//input 1952*1112 to output 1920*1080, offset (16, 18)
		/* Adjust the input frame only if previous link is McTNF */
		if(pObj->createArgs.prevLinkMcTNF == 1)
		{			
            inChInfo->height += 32; 	
            inChInfo->width  += 32;
			
			if(chCreateParams->sParams.eOperateMode != VNF_LINK_LDC_AFFINE_NSF_TNF)
			{
				inChInfo->startY -= 18;
				inChInfo->startX -= 16;
			}
		}
#endif
        chVnfCreateParams->ulInputWidth = inChInfo->width;
        chVnfCreateParams->ulInputHeight = inChInfo->height;
		
		if(pObj->createArgs.tilerEnable == TRUE)
		{
			chVnfCreateParams->tilerEn = MSP_TRUE;
        	outChInfo->memType = VPS_VPDMA_MT_TILEDMEM;
		}
		else
		{
			chVnfCreateParams->tilerEn = MSP_FALSE;
			outChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
		}

		chVnfCreateParams->ulInputStride = inChInfo->pitch[0];				
        chVnfCreateParams->ulStartX = inChInfo->startX;
        chVnfCreateParams->ulStartY = inChInfo->startY;

		VNFLINK_INFO_LOG(" [VNF] eOperateMode=%d [6-TNF3,7-NSF2]chVnfCreateParams->ulStartX = %d \
			chVnfCreateParams->ulStartY = %d chVnfCreateParams->ulInputStride = %d	\
			chVnfCreateParams->ulInputWidth=%d,chVnfCreateParams->ulInputHeight=%d ", \
          chCreateParams->sParams.eOperateMode, chVnfCreateParams->ulStartX , chVnfCreateParams->ulStartY , \
		  chVnfCreateParams->ulInputStride, chVnfCreateParams->ulInputWidth, chVnfCreateParams->ulInputHeight);

        switch (inChInfo->dataFormat)
        {
            case FVID2_DF_YUV422I_UYVY:
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_YVYU:
            case FVID2_DF_YUV422I_VYUY:
                chVnfCreateParams->eInputFormat = MSP_VNF_YUV_FORMAT_YCBCR422;
                 break;
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                chVnfCreateParams->eInputFormat = MSP_VNF_YUV_FORMAT_YCBCR420;
                chVnfCreateParams->ulInputStrideChroma = inChInfo->pitch[1];
                break;
        }

        switch (chCreateParams->sParams.eOutputFormat)
        {
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
                chVnfCreateParams->eOutputFormat = MSP_VNF_YUV_FORMAT_YCBCR420;
                break;
        }

        chVnfCreateParams->eOperateMode =
            (MSP_VNF_OPERATION_MODE) chCreateParams->sParams.eOperateMode;

        chVnfCreateParams->nsfParams.eNsfSet = (MSP_VNF_NSF_SET)chCreateParams->sParams.eNsfSet;

   		switch (chCreateParams->sParams.eOperateMode)
        {
        	case VNF_LINK_3DNF:
            	if(chVnfCreateParams->nsfParams.eNsfSet != MSP_VNF_NSF_DISABLE)
            	{
                	chCreateParams->sParams.computeWidth = inChInfo->width - 32;
                	chCreateParams->sParams.computeHeight = inChInfo->height - 32;
                	chCreateParams->sParams.outputStrideLuma = inChInfo->width;
                	chCreateParams->sParams.outputStrideChroma = inChInfo->width;
            	}
	            else
	            {
	                chCreateParams->sParams.computeWidth = inChInfo->width ;
	                chCreateParams->sParams.computeHeight = inChInfo->height ;
	                chCreateParams->sParams.outputStrideLuma = inChInfo->width+32;
	                chCreateParams->sParams.outputStrideChroma = inChInfo->width+32 ;
	            }
				break;
            case VNF_LINK_TNF3:
            	if(chVnfCreateParams->nsfParams.eNsfSet != MSP_VNF_NSF_DISABLE)
            	{
                	chCreateParams->sParams.computeWidth = inChInfo->width - 32;
                	chCreateParams->sParams.computeHeight = inChInfo->height - 32;
					
					if(pObj->createArgs.tilerEnable == TRUE)
					{
						chCreateParams->sParams.outputStrideLuma = VPSUTILS_TILER_CNT_8BIT_PITCH;
						chCreateParams->sParams.outputStrideChroma = VPSUTILS_TILER_CNT_16BIT_PITCH;
					}
					else
					{
                		chCreateParams->sParams.outputStrideLuma = inChInfo->width;
                		chCreateParams->sParams.outputStrideChroma = inChInfo->width;
					}
            	}
	            else
	            {
	                chCreateParams->sParams.computeWidth = inChInfo->width ;
	                chCreateParams->sParams.computeHeight = inChInfo->height ;
	                chCreateParams->sParams.outputStrideLuma = inChInfo->width;
	                chCreateParams->sParams.outputStrideChroma = inChInfo->width ;
	            }
				break;

        	/* In the current version, we will not allow free dimension settings */
   		 	case VNF_LINK_TNF:
			case VNF_LINK_LDC_TNF:
			case VNF_LINK_LDC:
	            chCreateParams->sParams.computeWidth = inChInfo->width;
	            chCreateParams->sParams.computeHeight = inChInfo->height;
	            chCreateParams->sParams.outputStrideLuma = inChInfo->width;
	            chCreateParams->sParams.outputStrideChroma = inChInfo->width;
				break;

			case VNF_LINK_NSF2:
			default:
	            chCreateParams->sParams.computeWidth = inChInfo->width - 32;
	            chCreateParams->sParams.computeHeight = inChInfo->height - 32;
	            chCreateParams->sParams.outputStrideLuma = inChInfo->width - 32;
	            chCreateParams->sParams.outputStrideChroma = inChInfo->width - 32;
				break;
        }

        chVnfCreateParams->ulComputeWidth =
            chCreateParams->sParams.computeWidth;
        chVnfCreateParams->ulComputeHeight =
            chCreateParams->sParams.computeHeight;
        chVnfCreateParams->ulOutputStrideLuma =
            chCreateParams->sParams.outputStrideLuma;
        chVnfCreateParams->ulOutputStrideChroma =
            chCreateParams->sParams.outputStrideChroma;

        chVnfCreateParams->eInterpolationLuma =
            (MSP_VNF_Y_INTERPOLATE_METHOD) chCreateParams->dParams.
            ldcParams.eInterpolationLuma;
        chVnfCreateParams->unPixelPad =
            chCreateParams->dParams.ldcParams.unPixelPad;

        chVnfCreateParams->nsfParams.bLumaEnable =
            (MSP_BOOL) chCreateParams->dParams.nsfParams.bLumaEn;
        chVnfCreateParams->nsfParams.bChromaEnable =
            (MSP_BOOL) chCreateParams->dParams.nsfParams.bChromaEn;
        chVnfCreateParams->nsfParams.bSmoothLuma =
            (MSP_BOOL) chCreateParams->dParams.nsfParams.bSmoothLumaEn;
        chVnfCreateParams->nsfParams.bSmoothChroma =
            (MSP_BOOL) chCreateParams->dParams.nsfParams.bSmoothChromaEn;
        chVnfCreateParams->nsfParams.eSmoothVal =
            (MSP_VNF_SMOOTH_PARAM) chCreateParams->dParams.nsfParams.eSmoothVal;

        chVnfCreateParams->nsfParams.eFilterParam =
            (MSP_VNF_PARAM_SET) chCreateParams->dParams.nsfFilter.eFilterParam;
        chVnfCreateParams->nsfParams.rgnFilterVal =
            (MSP_S16 *) chCreateParams->dParams.nsfFilter.rgnFilterVal;
        chVnfCreateParams->nsfParams.eShdParam =
            (MSP_VNF_PARAM_SET) chCreateParams->dParams.nsfShading.eShdParam;
        chVnfCreateParams->nsfParams.rgnShdVal =
            (MSP_S16 *) chCreateParams->dParams.nsfShading.rgnShdVal;
        chVnfCreateParams->nsfParams.eEdgeParam =
            (MSP_VNF_PARAM_SET) chCreateParams->dParams.nsfEdge.eEdgeParam;
        chVnfCreateParams->nsfParams.rgnEdgeVal =
            (MSP_S16 *) chCreateParams->dParams.nsfEdge.rgnEdgeVal;
        chVnfCreateParams->nsfParams.eDesatParam =
            (MSP_VNF_PARAM_SET) chCreateParams->dParams.nsfDesaturation.eDesatParam;
        chVnfCreateParams->nsfParams.rgnDesatVal =
            (MSP_S16 *) chCreateParams->dParams.nsfDesaturation.rgnDesatVal;

        chVnfCreateParams->nsfParams.rgnHalfFilterVal =
            (MSP_S16 *) chCreateParams->dParams.nsfFilter.rgnHalfFilterVal;
        chVnfCreateParams->nsfParams.rgnQuadFilterVal =
            (MSP_S16 *) chCreateParams->dParams.nsfFilter.rgnQuadFilterVal;
        //chVnfCreateParams->nsfParams.eHalfFilterParam=
        //    (VNF_LINK_PARAM_SET)VNF_LINK_PARAM_DEFAULT;    //REVISIT
        //chVnfCreateParams->nsfParams.eQuadFilterParam=
        //    (VNF_LINK_PARAM_SET)VNF_LINK_PARAM_DEFAULT;     //REVISIT

        chVnfCreateParams->tnfParams.unYComponentWt =
            chCreateParams->dParams.tnfParams.unYComponentWt;
        chVnfCreateParams->tnfParams.unUComponentWt =
            chCreateParams->dParams.tnfParams.unUComponentWt;
        chVnfCreateParams->tnfParams.unVComponentWt =
            chCreateParams->dParams.tnfParams.unVComponentWt;
        chVnfCreateParams->tnfParams.unMaxBlendingFactor =
            chCreateParams->dParams.tnfParams.unMaxBlendingFactor;
        chVnfCreateParams->tnfParams.unMotionThreshold =
            chCreateParams->dParams.tnfParams.unMotionThreshold;
        //chVnfCreateParams->tnfParams.eTnfAlphaLUTParam =
        //    (VNF_LINK_PARAM_SET)VNF_LINK_PARAM_DEFAULT;   //REVISIT

        chVnfCreateParams->btnfUseDefault =
            (MSP_BOOL) chCreateParams->dParams.tnfParams.useDefault;

        chVnfCreateParams->bTnfEn = (MSP_BOOL) chCreateParams->bTnfEnable;
        chVnfCreateParams->bSnfEn = (MSP_BOOL) chCreateParams->bSnfEnable;
		chVnfCreateParams->strength = (MSP_VNF_STRENGTH)chCreateParams->strength;

		if ((chCreateParams->sParams.eOperateMode == VNF_LINK_LDC) ||
			(chCreateParams->sParams.eOperateMode == VNF_LINK_LDC_TNF) ||
			(chCreateParams->sParams.eOperateMode == VNF_LINK_LDC_AFFINE_NSF_TNF))
			chVnfCreateParams->bLdcEn = MSP_TRUE;
		else
			chVnfCreateParams->bLdcEn = MSP_FALSE;

        /*
         * Per channel bufInQue structure needs to be created & initialized.
         * bufInQue.fullQue will be populated with captured frames and
         * bufInQue.emptyQuewill be allocated here so that they can be
         * used later on for noise filtering.
         * Frames need to be allocated for bufInQue.emptyQue here.
         */
        /* Create the per channel bufInQue */
        status = Utils_bufCreate(&VnfChObj->bufInQue, TRUE, FALSE);
        UTILS_assert(status == FVID2_SOK);

        /* Fill format with channel specific values to allocate frame */
        VnfChObj->format.channelNum = chId;
        VnfChObj->format.width = chVnfCreateParams->ulComputeWidth;
        VnfChObj->format.height = inChInfo->height;

        VnfChObj->format.pitch[0] = chVnfCreateParams->ulOutputStrideLuma;
        VnfChObj->format.pitch[1] = chVnfCreateParams->ulOutputStrideChroma;
        VnfChObj->format.pitch[2] = 0;
        VnfChObj->format.fieldMerged[0] = FALSE;
        VnfChObj->format.fieldMerged[1] = FALSE;
        VnfChObj->format.fieldMerged[2] = FALSE;
        VnfChObj->format.dataFormat = FVID2_DF_YUV420SP_UV;
        // format.dataFormat = chVnfCreateParams->eOutputFormat;
        VnfChObj->format.scanFormat = FVID2_SF_PROGRESSIVE;
        // format.scanFormat = inChInfo->scanFormat;
        VnfChObj->format.bpp = FVID2_BPP_BITS8;            /* ignored */

        /*
         * Alloc memory based on 'format'
         * Allocated frame info is put in frames[]
         */        
		if(pObj->createArgs.tilerEnable == TRUE)
			Utils_tilerFrameAlloc(&VnfChObj->format, VnfChObj->frames,
							  VNF_LINK_MAX_OUT_FRAMES_PER_CH);
		else
			Utils_memFrameAlloc_capture(&VnfChObj->format, VnfChObj->frames,
									    VNF_LINK_MAX_OUT_FRAMES_PER_CH,0,0);

        /* Set remaining parameters for the allocated frames */
        for (frameId = 0; frameId < VNF_LINK_MAX_OUT_FRAMES_PER_CH; frameId++)
        {
            /* Point to the frame's array */
            frame = &VnfChObj->frames[frameId];
            frame->perFrameCfg = NULL;
            frame->subFrameInfo = NULL;
            frame->appData = &VnfChObj->frameInfo[frameId];

            VnfChObj->frameInfo[frameId].vnfChannelNum = frame->channelNum;

            /* Finally, add this frame to the VnfLink_ChObj.bufInQue.emptyQue
             */
            status = Utils_bufPutEmptyFrame(&VnfChObj->bufInQue, frame);
            UTILS_assert(status == FVID2_SOK);
        }

        /* no previous frame for the first frame */
        VnfChObj->pPrevOutFrame = NULL;

        /* Populate the remaining fileds of
         * VnfLink_Obj.System_LinkInfo.System_LinkQueInfo.System_LinkChInfo
         * This information will be used by the next link to configure itself
         * properly. */
        UTILS_assert(queueId < pObj->info.numQue);

        outChInfo->bufType = SYSTEM_BUF_TYPE_VIDFRAME;
        outChInfo->dataFormat = VnfChObj->format.dataFormat;
        outChInfo->scanFormat = VnfChObj->format.scanFormat;

		/*
		 *	Vertical skip for VNF output is taken care by pointing output frame address to valid sub frame.
		 *  i.e addr[0][0] and addr[0][1] point to start of filtered sub frame. 	
		 */		
        outChInfo->startX = 0;			
		outChInfo->startY = 0;		
		
        outChInfo->width = chVnfCreateParams->ulComputeWidth;
        outChInfo->height = chVnfCreateParams->ulComputeHeight;
        outChInfo->pitch[0] = VnfChObj->format.pitch[0];
        outChInfo->pitch[1] = VnfChObj->format.pitch[1];
        outChInfo->pitch[2] = VnfChObj->format.pitch[2];

        /* Increase the number of channels now */
        pObj->info.queInfo[queueId].numCh++;

		VNFLINK_INFO_LOG(" [VNF] outChInfo->StartX = %d outChInfo->startY = %d outChInfo->pitch[0] = %d	outChInfo->width=%d,outChInfo->height=%d ", \
		  outChInfo->startX , outChInfo->startY , outChInfo->pitch[0], outChInfo->width,  outChInfo->height);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VnfLink_drvCreate
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
Int32 VnfLink_drvCreate(VnfLink_Obj * pObj, VnfLink_CreateParams * pPrm)
{
    Int32 status;

    VNFLINK_INFO_LOG("Create in progress ... !!!\n");

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
    UTILS_assert(pObj->inQueInfo.numCh <= VNF_LINK_MAX_CH);

    pObj->info.numQue = 1;

    /*
     * Configure the channels, allocate frames for each channel and
     * put those frames in channel specific buffer queue.
     */
    VnfLink_drvInitCh(pObj);

#ifdef VNF_ENGINE_INIT_ONLY_ONCE
    /* TODO : support multi channels */
    status = MSP_VNF_Engine_Init(&pObj->mspHandleVnf);
    UTILS_assert(status == MSP_ERROR_NONE);
#endif

    VNFLINK_INFO_LOG("Create ... DONE !!!\n");

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VnfLink_drvDoVnfFilter
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
static Int32 VnfLink_drvDoVnfFilter(VnfLink_Obj * pObj)
{
    FVID2_Frame *pFullFrame;

    FVID2_Frame *pEmptyFrame;

    Int32 status;

    UInt32 chId;

    UInt32 numFrames;

    VnfLink_ChObj *pChObj;
    System_FrameInfo *pInFrameInfo;
    System_FrameInfo *pOutFrameInfo;
	
	UInt32 vnfResetDone;
	UInt32 startY;
	UInt32 rtPrmChanged;
	
    numFrames = 0;

#ifdef _PROFILE
   static unsigned long int startTime = 0,stopTime = 0;
   static unsigned long int totalTimeOpen = 0,totalTimeProcess=0,totalTimeClose=0,cnt =0;
#endif

    VNFLINK_INFO_LOG_RT("VNF: VnfLink_drvDoVnfFilter !!!\n");

    /** For all the available channels, look into the respective
     * bufInQue.fullQueue and take the frames out of that queue.
     * Take the same number of frames from the respective bufInQue.emptyQueue.
     */
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->linkChInfo[chId];
        //VnfCreateParams * chVnfCrParams = &pObj->linkChInfo[chId].chCreateArgs;

        while (1)
        {
        	VnfCreateParams *chVnfCreateParams;
			VnfLink_ChCreateParams *chCreateParams;

      		chVnfCreateParams = &pChObj->chCreateArgs;
		    chCreateParams = &pObj->createArgs.chCreateParams[chId];

            status = Utils_bufGetFullFrame(&pChObj->bufInQue,
                                           &pFullFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
                break;

            /* First check whether it can be accomodated or not */
            status = Utils_bufGetEmptyFrame(&pChObj->bufInQue,
                                            &pEmptyFrame, BIOS_NO_WAIT);
            if (status != FVID2_SOK)
            {
                VNFLINK_INFO_LOG_RT("VNF: Skip frames !!!\n");
                break;
            }
			pEmptyFrame->timeStamp = pFullFrame->timeStamp;
            pObj->processFrameReqCount++;

            pInFrameInfo = (System_FrameInfo *) pFullFrame->appData;
            pOutFrameInfo = (System_FrameInfo *) pEmptyFrame->appData;

			pOutFrameInfo->rtChInfoUpdate = FALSE;
			vnfResetDone = 0;
            if ((pInFrameInfo != NULL) && (pInFrameInfo->rtChInfoUpdate == TRUE))
            {
				/* Temporal Hack For Full Resolution */
				rtPrmChanged = 0;
#if defined(USE_MCTNF) && !defined(VNF_BEFORE_MCTNF)
				if (pObj->createArgs.prevLinkMcTNF == 1) 
				{
					if((chVnfCreateParams->ulInputWidth != (pInFrameInfo->rtChInfo.width + 32)) ||
					   (chVnfCreateParams->ulInputHeight != (pInFrameInfo->rtChInfo.height + 32)))
					{
						rtPrmChanged = 1;
					}					
				}
				else
				{
					if ((chVnfCreateParams->ulInputWidth != pInFrameInfo->rtChInfo.width) ||
						(chVnfCreateParams->ulInputHeight != pInFrameInfo->rtChInfo.height))
					{
						rtPrmChanged = 1;	
					}				
				}	
#else				
				if ((chVnfCreateParams->ulInputWidth != pInFrameInfo->rtChInfo.width) ||
					(chVnfCreateParams->ulInputHeight != pInFrameInfo->rtChInfo.height))
				{
					rtPrmChanged = 1;	
				}	
#endif				
				if(rtPrmChanged == 1)
				{	
					Vps_printf("VNF: chVnfCreateParams->ulInputWidth = %d,pInFrameInfo->rtChInfo.width = %d\n",
							   chVnfCreateParams->ulInputWidth,pInFrameInfo->rtChInfo.width);
				
					chVnfCreateParams->ulInputWidth        = pInFrameInfo->rtChInfo.width;
					chVnfCreateParams->ulInputHeight       = pInFrameInfo->rtChInfo.height;
					chVnfCreateParams->ulInputStride       = pInFrameInfo->rtChInfo.pitch[0];
					chVnfCreateParams->ulInputStrideChroma = pInFrameInfo->rtChInfo.pitch[1];

					switch (chCreateParams->sParams.eOperateMode)
					{
						case VNF_LINK_3DNF:
							if(chVnfCreateParams->nsfParams.eNsfSet != MSP_VNF_NSF_DISABLE)
							{
								chCreateParams->sParams.computeWidth = chVnfCreateParams->ulInputWidth - 32;
								chCreateParams->sParams.computeHeight = chVnfCreateParams->ulInputHeight - 32;
							}
							else
							{
								chCreateParams->sParams.computeWidth = chVnfCreateParams->ulInputWidth ;
								chCreateParams->sParams.computeHeight = chVnfCreateParams->ulInputHeight ;
							}
							break;
						case VNF_LINK_TNF3:
							if(chVnfCreateParams->nsfParams.eNsfSet != MSP_VNF_NSF_DISABLE)
							{
								chCreateParams->sParams.computeWidth = chVnfCreateParams->ulInputWidth - 32;
								chCreateParams->sParams.computeHeight = chVnfCreateParams->ulInputHeight - 32;

								if(pObj->createArgs.tilerEnable == TRUE)
								{
									chCreateParams->sParams.outputStrideLuma = VPSUTILS_TILER_CNT_8BIT_PITCH;
									chCreateParams->sParams.outputStrideChroma = VPSUTILS_TILER_CNT_16BIT_PITCH;
								}
								else
								{
									chCreateParams->sParams.outputStrideLuma = chVnfCreateParams->ulInputWidth;
									chCreateParams->sParams.outputStrideChroma = chVnfCreateParams->ulInputWidth;
								}
							}
							else
							{
								chCreateParams->sParams.computeWidth = chVnfCreateParams->ulInputWidth ;
								chCreateParams->sParams.computeHeight = chVnfCreateParams->ulInputHeight ;
							}
							break;

						/* In the current version, we will not allow free dimension settings */
						case VNF_LINK_TNF:
						case VNF_LINK_LDC_TNF:
						case VNF_LINK_LDC:
							chCreateParams->sParams.computeWidth = chVnfCreateParams->ulInputWidth;
							chCreateParams->sParams.computeHeight = chVnfCreateParams->ulInputHeight;
							break;

						case VNF_LINK_NSF2:
						default:
							chCreateParams->sParams.computeWidth = chVnfCreateParams->ulInputWidth - 32;
							chCreateParams->sParams.computeHeight = chVnfCreateParams->ulInputHeight - 32;
							break;
					}

					chVnfCreateParams->ulComputeWidth       = chCreateParams->sParams.computeWidth;
					chVnfCreateParams->ulComputeHeight      = chCreateParams->sParams.computeHeight;
					chVnfCreateParams->ulOutputStrideLuma   = chCreateParams->sParams.outputStrideLuma;
					chVnfCreateParams->ulOutputStrideChroma = chCreateParams->sParams.outputStrideChroma;

					pInFrameInfo->rtChInfoUpdate = FALSE;

					MSP_VNF_Engine_Reset(pObj->mspHandleVnf);
					
					vnfResetDone = 1;					
	            }
            }

			/*
			 *	To support dynamic resolution change we have to set rtChInfoUpdate = TRUE for every frame.
			 */	
			pOutFrameInfo->rtChInfo.width	 = chVnfCreateParams->ulComputeWidth;
			pOutFrameInfo->rtChInfo.height	 = chVnfCreateParams->ulComputeHeight;
			pOutFrameInfo->rtChInfo.pitch[0] = chVnfCreateParams->ulOutputStrideLuma;
			pOutFrameInfo->rtChInfo.pitch[1] = chVnfCreateParams->ulOutputStrideChroma;
			pOutFrameInfo->rtChInfoUpdate	 = TRUE;
						
            pObj->curTime = Clock_getTicks();

            VNFLINK_INFO_LOG_RT("Start %d VNF: Noise Filtering frame %d !!!\n",
                                pObj->curTime, numFrames);

#ifndef VNF_ENGINE_INIT_ONLY_ONCE
            /* TODO : support multi channels */
            status = MSP_VNF_Engine_Init(&pObj->mspHandleVnf);
            UTILS_assert(status == MSP_ERROR_NONE);
#endif
#ifdef _PROFILE
            startTime = Utils_getCurTimeInUsec();
#endif
          status =
                MSP_VNF_Engine_Open(pObj->mspHandleVnf, &pChObj->chCreateArgs);
            UTILS_assert(status == MSP_ERROR_NONE);
#ifdef _PROFILE
            stopTime  = Utils_getCurTimeInUsec();
            totalTimeOpen += stopTime-startTime;
            cnt++;
            if(cnt%100 == 0)
               Vps_printf("Total Time for open = %ld\n",totalTimeOpen/100);
            startTime = Utils_getCurTimeInUsec();
#endif
			if (vnfResetDone == 1)	
			{
				status =
					MSP_VNF_ProcessFrame(pObj->mspHandleVnf, NULL,
                                     pFullFrame, pEmptyFrame, chVnfCreateParams->ulStartX, chVnfCreateParams->ulStartY, chVnfCreateParams->ulInputStride, chVnfCreateParams->ulInputStrideChroma);
			}
			else
			{
            	status =
                	MSP_VNF_ProcessFrame(pObj->mspHandleVnf, pChObj->pPrevOutFrame,
                                     pFullFrame, pEmptyFrame, chVnfCreateParams->ulStartX, chVnfCreateParams->ulStartY, chVnfCreateParams->ulInputStride, chVnfCreateParams->ulInputStrideChroma);
			}
            UTILS_assert(status == FVID2_SOK);
#ifdef _PROFILE
            stopTime  = Utils_getCurTimeInUsec();
            totalTimeProcess += stopTime-startTime;
            if(cnt%100 == 0)
               Vps_printf("Total Time for Process = %ld\n",totalTimeProcess/100);

            startTime = Utils_getCurTimeInUsec();
#endif
            status = MSP_VNF_Engine_Close(pObj->mspHandleVnf);
            UTILS_assert(status == MSP_ERROR_NONE);
#ifdef _PROFILE
            stopTime  = Utils_getCurTimeInUsec();
            totalTimeClose += stopTime-startTime;
            if(cnt%100 == 0)
            {
            Vps_printf("Total Time for Close = %ld\n",totalTimeClose/100);
            cnt = totalTimeOpen = totalTimeProcess = totalTimeClose = 0;
            }
#endif

#ifndef VNF_ENGINE_INIT_ONLY_ONCE
            /* VNF handle */
            MSP_VNF_Engine_DeInit(pObj->mspHandleVnf);
#endif
            pObj->curTime = Clock_getTicks() - pObj->curTime;

            VNFLINK_INFO_LOG_RT
                ("Stop %d VNF: Noise Filtering of frame %d Done !!!\n",
                 pObj->curTime, numFrames);

            pObj->totalTime += pObj->curTime;

            pObj->getProcessFrameReqCount++;

			/*
			 *	Output frame address is adjusted for vertical skip.
			 */					
			startY = (chVnfCreateParams->ulInputHeight - chVnfCreateParams->ulComputeHeight);
			pEmptyFrame->addr[0][0] = (Ptr)((UInt32)pEmptyFrame->addr[1][0] + (startY * pOutFrameInfo->rtChInfo.pitch[0]));
			pEmptyFrame->addr[0][1] = (Ptr)((UInt32)pEmptyFrame->addr[1][1] + (startY/2 * pOutFrameInfo->rtChInfo.pitch[1]));			
			
            /* Put the processed frames in the appropriate output queue */
            if (pChObj->pPrevOutFrame)
            {
                status =
                    Utils_bufPutFullFrame(&pObj->bufOutQue,
                                          pChObj->pPrevOutFrame);
                UTILS_assert(status == FVID2_SOK);
                VNFLINK_INFO_LOG_RT("VNF: Put frame %d to Out Que !!!\n",
                                    numFrames);
                numFrames++;
            }

            /* next previous output frame is current output frame */
            pChObj->pPrevOutFrame = pEmptyFrame;

            break;
        }
    }

    if (numFrames)
    {
        pObj->outFrameGivenCount += numFrames;

        /* Send data available message to next task */
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
        VNFLINK_INFO_LOG_RT("VNF: Send %d frames to the next link !!!\n",
                            numFrames);
    }
    else
    {
        VNFLINK_INFO_LOG_RT("VNF: No frames available for filtering !!!\n");

        /* no more frame availble so exit the loop outside of this function */
        status = FVID2_EFAIL;
    }

    return status;
}

/* This function gets called when capture driver has captured frames. This
 * function takes those frames from the capture driver and stores them in the
 * channel specific buf-queue for noise filtering stage. */
/* ===================================================================
 *  @func     VnfLink_drvProcessData
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
Int32 VnfLink_drvProcessData(VnfLink_Obj * pObj)
{
    Int32 frameId;

    FVID2_FrameList frameList;

    FVID2_Frame *pFrame;

    System_LinkInQueParams *pInQueParams;

    Int32 status;

    VnfLink_ChObj *pChObj;

    FVID2_Frame *framePerCh[VNF_LINK_MAX_CH];

    Int32 chId;

    VNFLINK_INFO_LOG_RT("VNF: VnfLink_drvProcessData !!!\n");
    /* Pointer to the input link's queue */
    pInQueParams = &pObj->createArgs.inQueParams;

    /* Get the captured frames from the capture driver */
    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    VNFLINK_INFO_LOG_RT("VNF: %d frames in !!!\n", frameList.numFrames);
    if (frameList.numFrames)
    {
        pObj->inFrameReceiveCount += frameList.numFrames;

        for (chId = 0; chId < VNF_LINK_MAX_CH; chId++)
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
			
			if(pObj->createArgs.enable[pFrame->channelNum] == 1)
			{			
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
							VNFLINK_INFO_LOG_RT("VNF: Skip frame same FID !!!\n");
						}
						else
						{
							pChObj->inFrameSkipCount++;
							VNFLINK_INFO_LOG_RT
								("VNF: Skip frame different FID !!!\n");
						}
					}
				}
				else
				{
					VNFLINK_INFO_LOG_RT("VNF: Skip frame %d !!!\n", frameId);
					pChObj->inFrameSkipCount++;
					// frame is not of expected FID, so skip it
				}
			}
			else
			{
				VNFLINK_INFO_LOG_RT("VNF: Skip frame %d !!!\n", frameId);
				pChObj->inFrameSkipCount++;
				// frame is not of expected FID, so skip it
			}	
        }

        for (chId = 0; chId < VNF_LINK_MAX_CH; chId++)
        {
            if (framePerCh[chId] != NULL)
            {
                status =
                    Utils_bufPutFullFrame(&pChObj->bufInQue, framePerCh[chId]);
                UTILS_assert(status == FVID2_SOK);

                VNFLINK_INFO_LOG_RT("VNF: Put frame %d to In Que !!!\n",
                                    frameId);
                break;
            }
        }

        /* with the previous frame skipping algo, we are sure that there is
         * only one frame to process per channel */
        // do
        {
            status = VnfLink_drvDoVnfFilter(pObj);
        }                                                  // while (status
        // == FVID2_SOK);

        /* Return frames back to the capture link as well */
        VNFLINK_INFO_LOG_RT("VNF    : Release %d IN frames !!!\n",
                            frameList.numFrames);
        pObj->inFrameGivenCount += frameList.numFrames;
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VnfLink_drvStart
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
Int32 VnfLink_drvStart(VnfLink_Obj * pObj)
{
    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VnfLink_drvStop
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
Int32 VnfLink_drvStop(VnfLink_Obj * pObj)
{
    System_LinkInQueParams *pInQueParams;

    FVID2_FrameList frameList;

    Int32 status = FVID2_SOK;

    UInt32 chId;

    VnfLink_ChObj *pChObj;

    VNFLINK_INFO_LOG("VNFLINK: Stop in progress !!!\n");

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

    VNFLINK_INFO_LOG("VNFLINK: Stop done !!!\n");

    return (status);
}

/* ===================================================================
 *  @func     VnfLink_drvDelete
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
Int32 VnfLink_drvDelete(VnfLink_Obj * pObj)
{
    UInt32 chId;
    VnfLink_ChObj *vnfChObj;
	UInt32 tilerUsed = FALSE;

#ifdef VNF_ENGINE_INIT_ONLY_ONCE
    Int32 status;

    /* TODO : support multi channels */
    status = MSP_VNF_Engine_DeInit(pObj->mspHandleVnf);
    UTILS_assert(status == MSP_ERROR_NONE);
#endif

    VNFLINK_INFO_LOG("VNF: Delete in progress !!!\n");

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        vnfChObj = &pObj->linkChInfo[chId];

		if(pObj->createArgs.tilerEnable == TRUE)
		{
			tilerUsed = TRUE;
		}
		else
		{
			Utils_memFrameFree_Capture(&vnfChObj->format, vnfChObj->frames,
									   VNF_LINK_MAX_OUT_FRAMES_PER_CH,0,0);
		}						   

        /* Free the channel specific buf-queue */
        Utils_bufDelete(&vnfChObj->bufInQue);
    }

    if (tilerUsed)
    {
        SystemTiler_freeAll();
    }	
	
    /* Free the VNF link output queues */
    Utils_bufDelete(&pObj->bufOutQue);

    VNFLINK_INFO_LOG("VNF: Delete done !!!\n");

    return FVID2_SOK;
}

/* This function will be called from the next link in the chain when the
 * frames are no more required. This function restores channel number for
 * each frame from the appData and pushes the frame back to the channel
 * specific bufInQue.emptyQueue. */
/* ===================================================================
 *  @func     VnfLink_drvReleaseFrames
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
Int32 VnfLink_drvReleaseFrames(VnfLink_Obj * pObj, FVID2_FrameList * pFrameList)
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
             * VnfLink_ChObj.bufInQue.emptyQue. */
            status =
                Utils_bufPutEmptyFrame(&pObj->
                                       linkChInfo[pFrame->channelNum].bufInQue,
                                       pFrame);
            UTILS_assert(status == FVID2_SOK);

            VNFLINK_INFO_LOG_RT
                ("VNF    : Release frame %d to VNF channel !!!\n", frameId);
        }

        VNFLINK_INFO_LOG_RT("VNF: Returned %d frames to VNF channel %d !!!\n",
                            pFrameList->numFrames, pFrame->channelNum);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VnfLink_drvEnableLDC
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
Int32 VnfLink_drvEnableLDC(VnfLink_Obj * pObj, UInt32 enableLDC)
{
    Int32 status = FVID2_SOK;

    UInt key;

    UInt32 chId;

    VnfLink_ChStaticParams *sParams;

    VNFLINK_INFO_LOG("EnableLDC current LDC %d => new %d!!!\n",
                     pObj->linkChInfo[0].chCreateArgs.bLdcEn,
                     enableLDC);

    key = Hwi_disable();

    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pObj->linkChInfo[chId].chCreateArgs.bLdcEn = (MSP_BOOL) enableLDC;
        sParams = &pObj->createArgs.chCreateParams[chId].sParams;
		if (enableLDC)
		{
			if (sParams->eOperateMode == VNF_LINK_AFFINE_NSF_TNF)
				sParams->eOperateMode = VNF_LINK_LDC_AFFINE_NSF_TNF;
			if (sParams->eOperateMode == VNF_LINK_TNF)
				sParams->eOperateMode = VNF_LINK_LDC_TNF;
		}
		else
		{
			if (sParams->eOperateMode == VNF_LINK_LDC_AFFINE_NSF_TNF)
				sParams->eOperateMode = VNF_LINK_AFFINE_NSF_TNF;
			if (sParams->eOperateMode == VNF_LINK_LDC_TNF)
				sParams->eOperateMode = VNF_LINK_TNF;
		}
		pObj->linkChInfo[chId].chCreateArgs.eOperateMode =
			(MSP_VNF_OPERATION_MODE) sParams->eOperateMode;
    }

    Hwi_restore(key);

    return status;
}

/* ===================================================================
 *  @func     VnfLink_drvEnableTNF
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
Int32 VnfLink_drvEnableTNF(VnfLink_Obj * pObj, UInt32 enableTNF)
{
    Int32 status = FVID2_SOK;

    UInt key;

    UInt32 chId;

    VNFLINK_INFO_LOG("EnableTNF current TNF %d => new %d!!!\n",
                     pObj->createArgs.chCreateParams[0].bTnfEnable, enableTNF);

    key = Hwi_disable();
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pObj->createArgs.chCreateParams[chId].bTnfEnable = enableTNF;
        pObj->linkChInfo[chId].chCreateArgs.bTnfEn = (MSP_BOOL) enableTNF;
    }
    Hwi_restore(key);

    return status;
}

/* ===================================================================
 *  @func     VnfLink_drvEnableSNF
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
Int32 VnfLink_drvEnableSNF(VnfLink_Obj * pObj, UInt32 enableSNF)
{
    Int32 status = FVID2_SOK;

    UInt key;

    UInt32 chId;

    VNFLINK_INFO_LOG("EnableSNF current SNF %d => new %d!!!\n",
                     pObj->createArgs.chCreateParams[0].bSnfEnable, enableSNF);

    key = Hwi_disable();
    for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
    {
        pObj->createArgs.chCreateParams[chId].bSnfEnable = enableSNF;
        pObj->linkChInfo[chId].chCreateArgs.bSnfEn = (MSP_BOOL) enableSNF;
    }
    Hwi_restore(key);

    return status;
}

Int32 VnfLink_drvSetStrength(VnfLink_Obj * pObj, VNF_LINK_VNF_STRENGTH strength)
{
	Int32 status = FVID2_SOK;

	UInt key;

	UInt32 chId;

	VNFLINK_INFO_LOG("SetStrength current strength %d => new %d!!!\n",
					 pObj->createArgs.chCreateParams[0].strength, strength);

	key = Hwi_disable();
	for (chId = 0; chId < pObj->inQueInfo.numCh; chId++)
	{
		pObj->createArgs.chCreateParams[chId].strength = strength;
		pObj->linkChInfo[chId].chCreateArgs.strength = (MSP_VNF_STRENGTH)strength;
	}
	Hwi_restore(key);

	return status;
}


/* ===================================================================
 *  @func     VnfLink_drvGetDynamicParams
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
Int32 VnfLink_drvGetDynamicParams(VnfLink_Obj * pObj,
                                  VnfLink_ChDynamicParams * params)
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
 *  @func     VnfLink_drvGetStaticParams
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
Int32 VnfLink_drvGetStaticParams(VnfLink_Obj * pObj,
                                 VnfLink_ChStaticParams * params)
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
 *  @func     VnfLink_drvSetDynamicParams
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
Int32 VnfLink_drvSetDynamicParams(VnfLink_Obj * pObj,
                                  VnfLink_ChDynamicParams * params)
{
    VnfLink_ChDynamicParams *dParams;

    VnfCreateParams *chVnfCreateParams;

    UInt key;

    UInt32 chId = params->chId;

    VNFLINK_INFO_LOG("SetDynamicParams ch %d features %d!!!\n", chId,
                     params->featureMask);

    if (chId < pObj->inQueInfo.numCh)
    {
        dParams = &pObj->createArgs.chCreateParams[chId].dParams;
        chVnfCreateParams = &pObj->linkChInfo[chId].chCreateArgs;
        key = Hwi_disable();

        if (params->featureMask & VNF_LINK_FEATURE_NSF_GLOBAL)
        {
            memcpy(&dParams->nsfParams, &params->nsfParams,
                   sizeof(params->nsfParams));

            chVnfCreateParams->nsfParams.bLumaEnable =
                (MSP_BOOL) dParams->nsfParams.bLumaEn;
            chVnfCreateParams->nsfParams.bChromaEnable =
                (MSP_BOOL) dParams->nsfParams.bChromaEn;
            chVnfCreateParams->nsfParams.bSmoothLuma =
                (MSP_BOOL) dParams->nsfParams.bSmoothLumaEn;
            chVnfCreateParams->nsfParams.bSmoothChroma =
                (MSP_BOOL) dParams->nsfParams.bSmoothChromaEn;
            chVnfCreateParams->nsfParams.eSmoothVal =
                (MSP_VNF_SMOOTH_PARAM) dParams->nsfParams.eSmoothVal;

            VNFLINK_INFO_LOG
                ("bLumaEnable %d bChromaEnable %d bSmoothLuma %d bSmoothChroma %d eSmoothVal %d!!!\n",
                 chVnfCreateParams->nsfParams.bLumaEnable,
                 chVnfCreateParams->nsfParams.bChromaEnable,
                 chVnfCreateParams->nsfParams.bSmoothLuma,
                 chVnfCreateParams->nsfParams.bSmoothChroma,
                 chVnfCreateParams->nsfParams.eSmoothVal);
        }

        if (params->featureMask & VNF_LINK_FEATURE_TNF)
        {
            memcpy(&dParams->tnfParams, &params->tnfParams,
                   sizeof(params->tnfParams));

            chVnfCreateParams->tnfParams.unYComponentWt =
                dParams->tnfParams.unYComponentWt;
            chVnfCreateParams->tnfParams.unUComponentWt =
                dParams->tnfParams.unUComponentWt;
            chVnfCreateParams->tnfParams.unVComponentWt =
                dParams->tnfParams.unVComponentWt;
            chVnfCreateParams->tnfParams.unMaxBlendingFactor =
                dParams->tnfParams.unMaxBlendingFactor;
            chVnfCreateParams->tnfParams.unMotionThreshold =
                dParams->tnfParams.unMotionThreshold;

            chVnfCreateParams->btnfUseDefault =
                (MSP_BOOL) dParams->tnfParams.useDefault;

            if (chVnfCreateParams->btnfUseDefault)
                VNFLINK_INFO_LOG("btnfUseDefault %d!!!\n",
                                 chVnfCreateParams->btnfUseDefault);
            else
                VNFLINK_INFO_LOG
                    ("unYComponentWt %d unUComponentWt %d unVComponentWt %d unMaxBlendingFactor %d unMotionThreshold %d!!!\n",
                     chVnfCreateParams->tnfParams.unYComponentWt,
                     chVnfCreateParams->tnfParams.unUComponentWt,
                     chVnfCreateParams->tnfParams.unVComponentWt,
                     chVnfCreateParams->tnfParams.unMaxBlendingFactor,
                     chVnfCreateParams->tnfParams.unMotionThreshold);
        }

        if (params->featureMask & VNF_LINK_FEATURE_LDC)
        {
            memcpy(&dParams->ldcParams, &params->ldcParams,
                   sizeof(params->ldcParams));

            chVnfCreateParams->eInterpolationLuma =
                (MSP_VNF_Y_INTERPOLATE_METHOD) dParams->
                ldcParams.eInterpolationLuma;
            chVnfCreateParams->unPixelPad = dParams->ldcParams.unPixelPad;

            VNFLINK_INFO_LOG("eInterpolationLuma %d unPixelPad %d!!!\n",
                             chVnfCreateParams->eInterpolationLuma,
                             chVnfCreateParams->unPixelPad);
        }

        if (params->featureMask & VNF_LINK_FEATURE_NSF_FILTER)
        {
            memcpy(&dParams->nsfFilter, &params->nsfFilter,
                   sizeof(params->nsfFilter));

            chVnfCreateParams->nsfParams.eFilterParam =
                (MSP_VNF_PARAM_SET) dParams->nsfFilter.eFilterParam;
        }

        if (params->featureMask & VNF_LINK_FEATURE_NSF_SHADING)
        {
            memcpy(&dParams->nsfShading, &params->nsfShading,
                   sizeof(params->nsfShading));

            chVnfCreateParams->nsfParams.eShdParam =
                (MSP_VNF_PARAM_SET) dParams->nsfShading.eShdParam;
        }

        if (params->featureMask & VNF_LINK_FEATURE_NSF_EDGE)
        {
            memcpy(&dParams->nsfEdge, &params->nsfEdge,
                   sizeof(params->nsfEdge));

            chVnfCreateParams->nsfParams.eEdgeParam =
                (MSP_VNF_PARAM_SET) dParams->nsfEdge.eEdgeParam;
        }

        if (params->featureMask & VNF_LINK_FEATURE_NSF_DESATURATION)
        {
            memcpy(&dParams->nsfDesaturation, &params->nsfDesaturation,
                   sizeof(params->nsfDesaturation));

            chVnfCreateParams->nsfParams.eDesatParam =
                (MSP_VNF_PARAM_SET) dParams->nsfDesaturation.eDesatParam;
        }

        Hwi_restore(key);
        return FVID2_SOK;
    }

    return FVID2_EINVALID_PARAMS;

}

void  VnfLink_drvUpdateDCC( VnfRunTimeParams *pVnfRunTimeParams)
{
            dcc_parser_input_params_t input_params;

			      dcc_parser_output_params_t output_params;
            extern iss_drv_config_t *iss_drv_config;
            //int i, j;
            extern Int8 dcc_buf[];

			output_params.iss_drv_config   = iss_drv_config;

            input_params.dcc_buf           = (Int8 *)gDccPrm.dcc_Default_Param;

            input_params.dcc_buf_size      = gDccPrm.dccSize;

            input_params.color_temparature = ti2a_output_params.colorTemparaure;

            input_params.exposure_time     = ti2a_output_params.sensorExposure;

            input_params.analog_gain       = (ti2a_output_params.sensorGain / 1000) * (ti2a_output_params.ipipe_awb_gain.dGain/256);

			output_params.pVnfParam   = pVnfRunTimeParams;
            output_params.pGlbceParams   = NULL;
			output_params.pMctnfParam   = NULL;
#if 0
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
			dcc_update(&input_params, &output_params);
#if 0
            Vps_printf("pVnfRunTimeParams->vnf_dcc_params_exist = %d \n",pVnfRunTimeParams->vnf_dcc_params_exist);
#endif


			return;
}

/* ===================================================================
 *  @func     VnfLink_enableChannel
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
Int32 VnfLink_enableChannel(VnfLink_Obj *pObj,UInt32 channelNum)
{
	if(channelNum < pObj->inQueInfo.numCh)
	{
		pObj->createArgs.enable[channelNum] = 1;
	}
	
	return FVID2_SOK;
} 
 
 /* ===================================================================
 *  @func     VnfLink_disableChannel
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
Int32 VnfLink_disableChannel(VnfLink_Obj *pObj,UInt32 channelNum)
{
	if(channelNum < pObj->inQueInfo.numCh)
	{
		pObj->createArgs.enable[channelNum] = 0;
	}
	
	return FVID2_SOK;
}
