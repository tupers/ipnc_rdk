/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <xdc/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
#include <ti/sdo/fc/rman/rman.h>
#include <ti/xdais/dm/ividenc2.h>
#include <ih264enc.h>
#include <h264enc_ti.h>

#include "encLink_priv.h"
#include "encLink_h264_priv.h"
#include <mcfw/src_bios6/links_m3video/codec_utils/utils_encdec.h>
#include <mcfw/src_bios6/links_m3video/codec_utils/iresman_hdvicp2_earlyacquire.h>
#include <mcfw/src_bios6/links_m3video/alg/motion_detection/motiondetect_priv.h>
#include <mcfw/interfaces/link_api/fdLink.h>

#define ENCLINK_H264_SETNALU_MASK_SPS(naluMask) ((naluMask) |= (1 << IH264_NALU_TYPE_SPS_WITH_VUI))
#define ENCLINK_H264_SETNALU_MASK_PPS(naluMask) ((naluMask) |= (1 << IH264_NALU_TYPE_PPS))
#define ENCLINK_H264_SETNALU_MASK_SEI(naluMask) ((naluMask) |= (1 << IH264_NALU_TYPE_SEI))



static IH264ENC_Handle enc_link_h264_create(const IH264ENC_Fxns * fxns,
                                            const IH264ENC_Params * prms,
                                            Int scratchGroupID );
static Void enc_link_h264_delete(Int scratchGroupID,IH264ENC_Handle handle);
static Int32 enclink_h264_control(IH264ENC_Handle handle,
                                  IH264ENC_Cmd cmd,
                                  IH264ENC_DynamicParams * params,
                                  IH264ENC_Status * status);
static Int enclink_h264_set_static_params(IH264ENC_Params * staticParams,
                                          EncLink_AlgCreateParams *
                                          algCreateParams);
static Int enclink_h264_set_algObject(EncLink_H264Obj * algObj,
                                      EncLink_AlgCreateParams * algCreateParams,
                                      EncLink_AlgDynamicParams *
                                      algDynamicParams);
static Int enclink_h264_set_dynamic_params(IH264ENC_DynamicParams *
                                           dynamicParams,
                                           EncLink_AlgDynamicParams *
                                           algDynamicParams);
static Int enclink_h264_set_params_stream_preset(Int32 StreamPreset,
                                                 IH264ENC_Params * staticParams,
                                                 IH264ENC_DynamicParams *
                                                 dynamicParams);
static Void enclink_h264_freersrc(EncLink_H264Obj * hObj, Int rsrcMask);
#ifdef CODEC_PRIV_MASK
static Int32 EncLinkH264_algSetFdRoi(EncLink_algObj * pAlgObj,
                              EncLink_ReqList * reqList, Int32 chId);
#endif

static Int enclink_print_dynamic_params(IVIDENC2_DynamicParams *videnc2DynamicParams);
static Int enclink_h264_print_dynamic_params(UInt32 chId, IH264ENC_Status* h264_enc_status);
static Int enclink_h264_print_IH264ENC_RateControlParams(const IH264ENC_RateControlParams *prms);
static Int enclink_h264_print_IH264ENC_InterCodingParams(const IH264ENC_InterCodingParams *prms);
static Int enclink_h264_print_IH264ENC_IntraCodingParams(const IH264ENC_IntraCodingParams*prms);
extern IRES_Fxns H264ENC_TI_IRES;
extern const IH264ENC_DynamicParams H264ENC_TI_DYNAMICPARAMS;

#ifdef STATIC_PARAMETER_PRINT
IH264ENC_Params createParamSave[2];
static Int enclink_h264_print_static_params(const IH264ENC_Params * prms , int chId);
#endif

typedef struct sErrorMapping{
  XDAS_Int8 *errorName;
}sErrorMapping;

static sErrorMapping gErrorStrings[32] =
{
  (XDAS_Int8 *)"IH264ENC_LEVEL_INCOMPLAINT_PARAMETER , 0, \0",
  (XDAS_Int8 *)"IH264ENC_PROFILE_INCOMPLAINT_CONTENTTYPE = 1,\0",
  (XDAS_Int8 *)"IH264ENC_PROFILE_INCOMPLAINT_FMO_SETTING = 2,",
  (XDAS_Int8 *)"IH264ENC_PROFILE_INCOMPLAINT_TRANSFORMBLOCKSIZE = 3,\0",
  (XDAS_Int8 *)"IH264ENC_PROFILE_INCOMPLAINT_INTERFRAMEINTERVAL = 4,\0",
  (XDAS_Int8 *)"IH264ENC_PROFILE_INCOMPLAINT_SCALINGMATRIXPRESET = 5,\0",
  (XDAS_Int8 *)"IH264ENC_PROFILE_INCOMPLAINT_ENTROPYCODINGMODE = 6,\0",
  (XDAS_Int8 *)"IH264ENC_MAX_BIT_RATE_VOILATION  = 7,\0",
  (XDAS_Int8 *)"XDM_PARAMSCHANGE = 8,\0",
  (XDAS_Int8 *)"XDM_APPLIEDCONCEALMENT = 9,\0",
  (XDAS_Int8 *)"XDM_INSUFFICIENTDATA = 10,\0",
  (XDAS_Int8 *)"XDM_CORRUPTEDDATA = 11,\0",
  (XDAS_Int8 *)"XDM_CORRUPTEDHEADER = 12,\0",
  (XDAS_Int8 *)"XDM_UNSUPPORTEDINPUT = 13,\0",
  (XDAS_Int8 *)"XDM_UNSUPPORTEDPARAM = 14,\0",
  (XDAS_Int8 *)"XDM_FATALERROR = 15\0",
  (XDAS_Int8 *)"IH264ENC_IMPROPER_HDVICP2_STATE = 16\0",
  (XDAS_Int8 *)"IH264ENC_IMPROPER_STREAMFORMAT = 17,\0",
  (XDAS_Int8 *)"IH264ENC_IMPROPER_POCTYPE = 18,\0",
  (XDAS_Int8 *)"IH264ENC_IMPROPER_DATASYNC_SETTING = 19,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_VIDENC2PARAMS = 20,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_RATECONTROLPARAMS = 21,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_INTERCODINGPARAMS = 22,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_INTRACODINGPARAMS = 23,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_NALUNITCONTROLPARAMS = 24,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_SLICECODINGPARAMS = 25,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_LOOPFILTERPARAMS = 26,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_FMOCODINGPARAMS = 27,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_VUICODINGPARAMS = 28,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_H264ENCPARAMS = 29,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_VIDENC2DYNAMICPARAMS = 30,\0",
  (XDAS_Int8 *)"IH264ENC_UNSUPPORTED_H264ENCDYNAMICPARAMS = 31, \0"
};

/**
********************************************************************************
 *  @fn     H264ENC_TI_Report_Error
 *  @brief  This function will print error messages
 *
 *          This function will check for codec errors which are mapped to
 *          extended errors in videnc2status structure and prints them in cosole
 *          Returns XDM_EFAIL in case of fatal error
 *
 *  @param[in]  uiErrorMsg  : Extended error message
 *
 *  @param[in]  fTrace_file : File pointer to the trace log file
 *
 *  @return     XDM_EOK -  when there is no fatal error
 *              XDM_EFAIL - when it is fatal error
********************************************************************************
*/


XDAS_Int32 H264ENC_TI_Report_Error(XDAS_Int32 uiErrorMsg)
{
  int i;
  if(uiErrorMsg)
  {
    /*------------------------------------------------------------------------*/
    /* Loop through all the bits in error message and map to the glibal       */
    /* error string                                                           */
    /*------------------------------------------------------------------------*/
    for (i = 0; i < 32; i ++)
    {
      if (uiErrorMsg & (1 << i))
      {
        Vps_printf("ERROR: %s \n",  gErrorStrings[i].errorName);
      }
    }
  }
  if (XDM_ISFATALERROR(uiErrorMsg))
  {
    return XDM_EFAIL;
  }
  else
  {
    return XDM_EOK;
  }
}


/*
 *  ======== enc_link_h264_create ========
 *  Create an H264ENC instance object (using parameters specified by prms)
 */

static IH264ENC_Handle enc_link_h264_create(const IH264ENC_Fxns * fxns,
                                            const IH264ENC_Params * prms,
                                            Int scratchGroupID)
{
    return ((IH264ENC_Handle) ALG_create(scratchGroupID,
                                         (IALG_Fxns *) fxns,
                                         NULL, (IALG_Params *) prms));
}

/*
 *  ======== enc_link_h264_delete ========
 *  Delete the H264ENC instance object specified by handle
 */

static Void enc_link_h264_delete(Int scratchGroupID,IH264ENC_Handle handle)
{
    ALG_delete(scratchGroupID,(IALG_Handle) handle);
}

/*
 *  ======== enc_link_h264_control ========
 */

static Int32 enclink_h264_control(IH264ENC_Handle handle,
                                  IH264ENC_Cmd cmd,
                                  IH264ENC_DynamicParams * params,
                                  IH264ENC_Status * status)
{
    int error = 0;
    IALG_Fxns *fxns = (IALG_Fxns *) handle->fxns;

    fxns->algActivate((IALG_Handle) handle);

    error = handle->fxns->ividenc.control((IVIDENC2_Handle) handle, cmd,
                                          (IVIDENC2_DynamicParams *) params,
                                          (IVIDENC2_Status *) status);
    fxns->algDeactivate((IALG_Handle) handle);

    if (error != XDM_EOK)
    {
        ENCLINK_INTERNAL_ERROR_LOG(error, "ALGCONTROL FAILED:CMD:%d\n", cmd);
        H264ENC_TI_Report_Error(status->videnc2Status.extendedError);
    }
    return error;
}

/*
 *  ======== Enclink_h264CalcSecondFieldOffsets ========
 */

Int32 Enclink_h264CalcSecondFieldOffsets(IVIDEO2_BufDesc *inputBufDesc,
                                         FVID2_Frame *secField,
                                         Bool tilerEnable)
{
    Int retVal = ENC_LINK_S_SUCCESS;
    UInt32 addr, i;
    Int32 addrOffset, secondFieldOffsetHeight, secondFieldOffsetWidth;

    for (i=0; i<inputBufDesc->numPlanes; i++)
    {
        if (tilerEnable)
        {
            addr = Utils_tilerAddr2CpuAddr((UInt32) (secField->addr[0][i]));
        }
        else
        {
            addr = (UInt32) secField->addr[0][i];
        }

        addrOffset = addr - (UInt32)inputBufDesc->planeDesc[i].buf;


        secondFieldOffsetHeight = addrOffset/inputBufDesc->imagePitch[i];

        secondFieldOffsetWidth = addrOffset -
                       (secondFieldOffsetHeight * inputBufDesc->imagePitch[i]);

        inputBufDesc->secondFieldOffsetHeight[i] = secondFieldOffsetHeight;
        inputBufDesc->secondFieldOffsetWidth[i] = secondFieldOffsetWidth;
    }

    inputBufDesc->secondFieldOffsetHeight[2] =
                  inputBufDesc->secondFieldOffsetHeight[1];
    inputBufDesc->secondFieldOffsetWidth[2] =
                  inputBufDesc->secondFieldOffsetWidth[1];

    return (retVal);
}

/*
 *  ======== Enclink_h264EncodeFrame ========
 */
Int32 Enclink_h264EncodeFrameBatch(EncLink_Obj * pObj,
                                   EncLink_ReqBatch * reqObjBatch,
                                   Int32 tskId)
{
    int error = XDM_EFAIL;
    Int32 i;
    Int32 reqObjIndex;
    IH264ENC_InArgs *inArgs;
    IH264ENC_OutArgs *outArgs;
    IVIDEO2_BufDesc *inputBufDesc;
    XDM2_BufDesc *outputBufDesc;
    IH264ENC_Handle handle = NULL;
    EncLink_ReqObj *pReqObj;
    IVIDEO_ContentType contentType;
    //IH264ENC_ProcessParamsList processList;
    EncLink_ChObj * pChObj;
    Int32 status = ENC_LINK_S_SUCCESS;
	UInt32 mvDataBuf;
	void *allocatedRingBufferAddr;
	UInt32 allocatedRingBufferLength;
	UInt32 poolId;
	UInt32 frameWidth,frameHeight;
	RingBufferHandle_t ringBufferHnl;

#ifdef SYSTEM_DEBUG_MULTI_CHANNEL_ENC
    Vps_printf ("IVAHDID : %d Entering Enclink_h264EncodeFrameBatch\n", tskId);
#endif
    //processList.numEntries = 0;

    /*Make sure that the req Object Batch is not empty*/
     UTILS_assert (reqObjBatch->numReqObjsInBatch > 0);

#ifdef CODEC_PRIV_MASK
	Int32 chId;
    EncLink_ReqList reqList;
    reqList.numReqObj = 0;
    for (reqObjIndex = 0; reqObjIndex < reqObjBatch->numReqObjsInBatch;
         reqObjIndex++)
	{
        pReqObj = reqObjBatch->pReqObj[reqObjIndex];
        pChObj  = &pObj->chObj[pReqObj->OutBuf->channelNum];
		/*FD request list*/
		chId = pReqObj->OutBuf->channelNum;
		reqList.reqObj[reqList.numReqObj++] = pReqObj;
	}
	status =
		EncLinkH264_algSetFdRoi(&pChObj->algObj, &reqList,
                                                chId);
#endif

    /*Prepare the Process List that can be queued in via multiProcess call*/
    for (reqObjIndex = 0; reqObjIndex < reqObjBatch->numReqObjsInBatch;
         reqObjIndex++)
    {

        pReqObj = reqObjBatch->pReqObj[reqObjIndex];
	  pChObj  = &pObj->chObj[pReqObj->OutBuf->channelNum];

	 //sheng: update ringbuffer space passed to encoder
	 extern BitStreamRingBuffer ringBuffer[UTILS_BITBUF_MAX_ALLOC_POOLS];
	 poolId = pObj->outObj.ch2poolMap[pReqObj->OutBuf->channelNum];
	 ringBufferHnl = &ringBuffer[poolId];

	 //allocate proper ring buffer space to encoder, vary in two different cases
	 if(ringBufferHnl->wrOfst >= ringBufferHnl->rdOfst) // case1
	{
	     allocatedRingBufferAddr = (void *)((UInt32)ringBufferHnl->baseAddr + ringBufferHnl->wrOfst);
	     allocatedRingBufferLength = ringBufferHnl->totalSize - ringBufferHnl->wrOfst;

		frameWidth = pReqObj->OutBuf->frameWidth;
		frameHeight = pReqObj->OutBuf->frameHeight;
		//early termination if buffer at physical tail less than minimal requirement, then allocate from ring buffer head
	     if(allocatedRingBufferLength <= MIN_TERMINATION_SIZE(frameWidth,frameHeight))
	     {
	     		//Vps_printf ("*******early termination********\n");
			Utils_ringBuffer_updateWrOfst(ringBufferHnl, allocatedRingBufferLength);
	     		allocatedRingBufferAddr = (void *)((UInt32)ringBufferHnl->baseAddr + ringBufferHnl->wrOfst);
	     		allocatedRingBufferLength = ringBufferHnl->rdOfst - ringBufferHnl->wrOfst;
	     }
	}
	else // (ringBufferHnl->wrOfst < ringBufferHnl->rdOfst) // case2
	{
	     allocatedRingBufferAddr = (void *)((UInt32)ringBufferHnl->baseAddr + ringBufferHnl->wrOfst);
	     allocatedRingBufferLength = ringBufferHnl->rdOfst - ringBufferHnl->wrOfst;
	}

        status = EncLinkH264_algSetConfig(&pChObj->algObj);
        status = EncLinkH264_algGetConfig(&pChObj->algObj);

        if (ENC_LINK_S_SUCCESS != status)
        {
          return XDM_EFAIL;
        }

        inArgs = &pChObj->algObj.u.h264AlgIfObj.inArgs;
        outArgs = &pChObj->algObj.u.h264AlgIfObj.outArgs;
        inputBufDesc = &pChObj->algObj.u.h264AlgIfObj.inBufs;
        outputBufDesc = &pChObj->algObj.u.h264AlgIfObj.outBufs;
        handle = pChObj->algObj.u.h264AlgIfObj.algHandle;

        inArgs->videnc2InArgs.inputID =
                (UInt32) pReqObj->InFrameList.frames[0]->addr[0][0];

        for (i = 0; i < inputBufDesc->numPlanes; i++)
        {
            /* Set proper buffer addresses for Frame data */
          /*---------------------------------------------------------------*/
            if (pChObj->algObj.algCreateParams.tilerEnable)
            {
                inputBufDesc->planeDesc[i].buf =
                    (Ptr)
                    Utils_tilerAddr2CpuAddr((UInt32)
                          (pReqObj->InFrameList.frames[0]->addr[0][i]));
            }
            else
            {
                inputBufDesc->planeDesc[i].buf =
                              pReqObj->InFrameList.frames[0]->addr[0][i];
            }
        }

        if (pReqObj->InFrameList.numFrames == 2)
        {
            UTILS_assert (FVID2_FID_TOP == (FVID2_Fid)pReqObj->InFrameList.frames[0]->fid);
            UTILS_assert (FVID2_FID_BOTTOM == (FVID2_Fid)pReqObj->InFrameList.frames[1]->fid);
            Enclink_h264CalcSecondFieldOffsets(inputBufDesc,
                                   pReqObj->InFrameList.frames[1],
                                   pChObj->algObj.algCreateParams.tilerEnable);


            UTILS_assert ((UInt32) pReqObj->InFrameList.frames[0]->addr[0][0] +
                          inputBufDesc->imagePitch[0] *
                          (inputBufDesc->secondFieldOffsetHeight[0])
                          + inputBufDesc->secondFieldOffsetWidth[0] ==
                          (UInt32) pReqObj->InFrameList.frames[1]->addr[0][0]);

            UTILS_assert ((UInt32) pReqObj->InFrameList.frames[0]->addr[0][1] +
                          inputBufDesc->imagePitch[1]*
                          (inputBufDesc->secondFieldOffsetHeight[1])
                          + inputBufDesc->secondFieldOffsetWidth[1] ==
                          (UInt32) pReqObj->InFrameList.frames[1]->addr[0][1]);
        }

        pReqObj->OutBuf->mvDataFilledSize = 0;
        for (i = outputBufDesc->numBufs-1; i>=0; i--)
        {

            if(i == 1)
            {
                /*-------------------------------------------------------------------*/
                    /* Set proper buffer addresses for MV & SAD data */
                /*-------------------------------------------------------------------*/
                outputBufDesc->descs[1].buf = allocatedRingBufferAddr;
                pReqObj->OutBuf->mvDataFilledSize = outputBufDesc->descs[1].bufSize.bytes;
                pReqObj->OutBuf->mvDataOffset = 0;
            }
		if(i == 0)
            {
			    if(pObj->createArgs.isVaUseCase == 1)
				{
					/* Set proper buffer addresses for bitstream data */
					/*---------------------------------------------------------------*/
					if(outputBufDesc->numBufs == 2)
						{
							/* copy VA meta data */
							memcpy((void *)((UInt32)allocatedRingBufferAddr+outputBufDesc->descs[1].bufSize.bytes),pObj->vaMetaData,VALINK_METADATASIZE);
							outputBufDesc->descs[i].buf = (Int8*)((UInt32)allocatedRingBufferAddr +outputBufDesc->descs[1].bufSize.bytes + VALINK_METADATASIZE);
							outputBufDesc->descs[i].bufSize.bytes = allocatedRingBufferLength - outputBufDesc->descs[1].bufSize.bytes - VALINK_METADATASIZE;
						}
					else
						{
							/* copy VA meta data */
							memcpy(allocatedRingBufferAddr,pObj->vaMetaData,VALINK_METADATASIZE);
							outputBufDesc->descs[i].buf = (Int8*)((UInt32)allocatedRingBufferAddr + VALINK_METADATASIZE);
							outputBufDesc->descs[i].bufSize.bytes = allocatedRingBufferLength - VALINK_METADATASIZE;
						}
				}
				else
				{
					if(outputBufDesc->numBufs == 2)
                    /* Set proper buffer addresses for MV & SAD data */
						{
							outputBufDesc->descs[i].buf = (Int8*)((UInt32)allocatedRingBufferAddr + pReqObj->OutBuf->mvDataFilledSize);
							outputBufDesc->descs[i].bufSize.bytes = allocatedRingBufferLength - pReqObj->OutBuf->mvDataFilledSize;
						}
					else
						{
							outputBufDesc->descs[i].buf = (Int8*)allocatedRingBufferAddr;
							outputBufDesc->descs[i].bufSize.bytes = allocatedRingBufferLength;
						}
				}
            }
        }

	//changed the process multi to single process
   if (handle != NULL)
   {
        //Vps_printf("[ENC] H264 Encoder is called for process !!!!!!!!! \n  ");
        error = ((IVIDENC2_Handle)handle)->fxns->process((IVIDENC2_Handle)handle,inputBufDesc,outputBufDesc,(IVIDENC2_InArgs *)inArgs,(IVIDENC2_OutArgs *)outArgs);
        //Vps_printf("[ENC] H264 Encoder completed process !!!!!!!!! \n  ");
   }
   else
   {
        UTILS_assert (FALSE);
   }

    if (error != XDM_EOK)
    {
    	Vps_printf ("Extended error %x for reqObjIndex %d\n", outArgs->videnc2OutArgs.extendedError, reqObjIndex);
    }

	//upadting outBuf parameters
	pReqObj->OutBuf->addr = allocatedRingBufferAddr;
	pReqObj->OutBuf->fillLength = outArgs->videnc2OutArgs.bytesGenerated;
	pReqObj->OutBuf->bufSize = outArgs->videnc2OutArgs.bytesGenerated;
	pReqObj->OutBuf->startOffset = 0;
	pReqObj->OutBuf->phyAddr = (UInt32 )pReqObj->OutBuf->addr;
	if(pObj->createArgs.isVaUseCase == 1)
	{
		pReqObj->OutBuf->fillLength += VALINK_METADATASIZE;
		pReqObj->OutBuf->bufSize += VALINK_METADATASIZE;
	}
	if (outputBufDesc->numBufs == 2)
      {
      		pReqObj->OutBuf->startOffset += pReqObj->OutBuf->mvDataFilledSize;
		pReqObj->OutBuf->bufSize += pReqObj->OutBuf->mvDataFilledSize;
	}
	Utils_ringBuffer_updateWrOfst(ringBufferHnl, pReqObj->OutBuf->bufSize);
	Utils_ringBuffer_updateEncodedStreamCount(ringBufferHnl, TRUE);

        pReqObj->OutBuf->codingType = pChObj->algObj.u.h264AlgIfObj.format;


        pReqObj->OutBuf->bottomFieldBitBufSize = outArgs->bytesGeneratedBotField;


        if (pChObj->algObj.u.h264AlgIfObj.staticParams.videnc2Params.inputContentType
            ==
        IVIDEO_PROGRESSIVE)
        {
            contentType = IVIDEO_PROGRESSIVE;
        }
        else
        {
            contentType = Utils_encdecMapFVID2FID2XDMContentType(
                         (FVID2_Fid)pReqObj->InFrameList.frames[0]->fid);
        }

        pReqObj->OutBuf->isKeyFrame = Utils_encdecIsGopStart(outArgs->
                                                             videnc2OutArgs.
                                                             encodedFrameType,
                                                             contentType);
        pReqObj->OutBuf->frameWidth =
          inputBufDesc->imageRegion.bottomRight.x - inputBufDesc->imageRegion.topLeft.x;
        pReqObj->OutBuf->frameHeight =
          inputBufDesc->imageRegion.bottomRight.y - inputBufDesc->imageRegion.topLeft.y;
		if(pChObj->algObj.u.h264AlgIfObj.staticParams.numTemporalLayer > 1)
			pReqObj->OutBuf->temporalId = outArgs->temporalId + 1;
		else
			pReqObj->OutBuf->temporalId = outArgs->temporalId;

        /* If SAD values are available then run Motion Detection Algorithm */
        if (outputBufDesc->numBufs == 2)
        {
        	//changed address because memory layout has changed
            mvDataBuf = (UInt32) (outputBufDesc->descs[1].buf);

            DM81XX_MD_Apply(mvDataBuf,                     // MV buf addr
                            pReqObj->OutBuf->frameWidth,   // frame width
                            pReqObj->OutBuf->frameHeight,  // frame height
                            pReqObj->OutBuf->isKeyFrame);  // isKeyFrame
        }

        //IRESMAN_HDVICP2_EarlyRelease((IALG_Handle) handle,
        //                             pChObj->algObj.u.h264AlgIfObj.ivaChID);
    }
#ifdef SYSTEM_DEBUG_MULTI_CHANNEL_ENC
   Vps_printf ("IVAHDID : %d Leaving Enclink_h264EncodeFrameBatch with error code : %d\n",
               tskId, error);
#endif
    return (error);
}

static Int enclink_h264_set_static_params(IH264ENC_Params * staticParams,
                                          EncLink_AlgCreateParams *
                                          algCreateParams)
{
    /* Initialize default values for static params */
    *staticParams = H264ENC_TI_PARAMS;

    /* Both width & height needs to be align with 2 bytes */
    staticParams->videnc2Params.maxHeight =
                  VpsUtils_align(algCreateParams->maxHeight, 2);

    staticParams->videnc2Params.maxWidth =
                  VpsUtils_align(algCreateParams->maxWidth, 16);

    staticParams->videnc2Params.maxInterFrameInterval =
        algCreateParams->maxInterFrameInterval;

    staticParams->videnc2Params.inputContentType =
        algCreateParams->inputContentType;

    staticParams->videnc2Params.inputChromaFormat =
        algCreateParams->inputChromaFormat;

    staticParams->videnc2Params.profile = algCreateParams->profile;
    /* CABAC should be used for all applications
     * where base profile is not must. */
    if(staticParams->videnc2Params.profile > IH264_BASELINE_PROFILE)
        staticParams->entropyCodingMode = IH264_ENTROPYCODING_CABAC;

    staticParams->videnc2Params.level = algCreateParams->level;

    staticParams->videnc2Params.encodingPreset = algCreateParams->encodingPreset;

    if ((staticParams->videnc2Params.encodingPreset == XDM_USER_DEFINED) &&
        (algCreateParams->enableHighSpeed == TRUE))
    {
        staticParams->interCodingParams.interCodingPreset =
            IH264_INTERCODING_HIGH_SPEED;
        staticParams->interCodingParams.meAlgoMode =
            IH264ENC_MOTIONESTMODE_HIGH_SPEED;
        staticParams->intraCodingParams.intraCodingPreset =
            IH264_INTRACODING_HIGH_SPEED;
        staticParams->transformBlockSize = IH264_TRANSFORM_8x8;
    }
    staticParams->enableAnalyticinfo = algCreateParams->enableAnalyticinfo;
	staticParams->enableWatermark = algCreateParams->enableWaterMarking;
    staticParams->videnc2Params.rateControlPreset = IVIDEO_USER_DEFINED;

    staticParams->rateControlParams.rateControlParamsPreset = IH264_RATECONTROLPARAMS_USERDEFINED;
    staticParams->rateControlParams.rcAlgo = algCreateParams->rateControlPreset;
    staticParams->rateControlParams.scalingMatrixPreset = IH264_SCALINGMATRIX_NORMAL;
    staticParams->rateControlParams.frameSkipThMulQ5 = 0;
    staticParams->rateControlParams.vbvUseLevelThQ5 = 0;

    staticParams->videnc2Params.maxBitRate = algCreateParams->maxBitRate;

    staticParams->videnc2Params.inputDataMode = IVIDEO_ENTIREFRAME;
    staticParams->videnc2Params.outputDataMode = IVIDEO_ENTIREFRAME;

    /*Slice Coding Parameters*/
    staticParams->sliceCodingParams.sliceCodingPreset = IH264_SLICECODING_DEFAULT;
    staticParams->sliceCodingParams.sliceMode = IH264_SLICEMODE_NONE;
    staticParams->sliceCodingParams.streamFormat = IH264_STREAM_FORMAT_DEFAULT;


    /* To set IDR frame periodically instead of I Frame */
    staticParams->IDRFrameInterval = 1;

    /*To trigger workaround inside codec, where SAME_CODEC is overridden as same
      codec type*/
    staticParams->reservedParams[1] = 0x5A3EC0DE;

    /* Enabling debug logging inside the codec. Details in appendix E in H.264
     * encoder user guide.
     */
     staticParams->debugTraceLevel = 1;
     staticParams->lastNFramesToLog = 31;

    /* We want SPS and PPS to be set for every intra frame. Hence configure the
     * the NALU control params to force encoder to insert SPS/PPS on every
     * I frame
     */
    staticParams->nalUnitControlParams.naluControlPreset =
                                             IH264_NALU_CONTROL_USERDEFINED;
    ENCLINK_H264_SETNALU_MASK_SPS(staticParams->nalUnitControlParams.
                                                naluPresentMaskIntraPicture);
    ENCLINK_H264_SETNALU_MASK_PPS(staticParams->nalUnitControlParams.
                                                naluPresentMaskIntraPicture);

    ENCLINK_H264_SETNALU_MASK_SPS(staticParams->nalUnitControlParams.
                                                naluPresentMaskIDRPicture);
    ENCLINK_H264_SETNALU_MASK_PPS(staticParams->nalUnitControlParams.
                                                naluPresentMaskIDRPicture);
    ENCLINK_H264_SETNALU_MASK_SEI(staticParams->nalUnitControlParams.
                                                naluPresentMaskIDRPicture);

    ENCLINK_H264_SETNALU_MASK_SPS(staticParams->nalUnitControlParams.
                                                naluPresentMaskStartOfSequence);
    ENCLINK_H264_SETNALU_MASK_PPS(staticParams->nalUnitControlParams.
                                                naluPresentMaskStartOfSequence);
    ENCLINK_H264_SETNALU_MASK_PPS(staticParams->nalUnitControlParams.
                                                naluPresentMaskStartOfSequence);

	/*VUI parameters setup*/
    staticParams->vuiCodingParams.vuiCodingPreset = IH264_VUICODING_USERDEFINED;
    /* By the setting the following Flag the codec would encode the aspect ratio in bitstream.
     * But becuase of the VLC player default aspect ration issue, here by not coding the
     * aspect ratio in VUI information.
     */

    staticParams->vuiCodingParams.aspectRatioInfoPresentFlag = 0;
    staticParams->vuiCodingParams.aspectRatioIdc = IH264ENC_ASPECTRATIO_EXTENDED;
    /* As we are setting the aspectRatioIdc to EXTENDED value, we need to provide
     * the SAR WIDTH and SAR HEIGHT also, which is in Dynamic parameters
     */

    staticParams->vuiCodingParams.videoSignalTypePresentFlag = 1;
	if(staticParams->videnc2Params.maxHeight == 576 && staticParams->videnc2Params.maxWidth == 720)
		staticParams->vuiCodingParams.videoFormat = IH264ENC_VIDEOFORMAT_PAL;
	else if(staticParams->videnc2Params.maxHeight == 480 && staticParams->videnc2Params.maxWidth == 720)
		staticParams->vuiCodingParams.videoFormat = IH264ENC_VIDEOFORMAT_NTSC;
	else
		staticParams->vuiCodingParams.videoFormat = IH264ENC_VIDEOFORMAT_UNSPECIFIED;

    staticParams->vuiCodingParams.videoFullRangeFlag = 1; //[0, 255] is the range of video signal
    staticParams->vuiCodingParams.hrdParamsPresentFlag = 1;
    staticParams->vuiCodingParams.timingInfoPresentFlag = 1;

    return 0;
}

static Int enclink_h264_set_params_stream_preset(Int32 StreamPreset,
                                                 IH264ENC_Params * staticParams,
                                                 IH264ENC_DynamicParams *
                                                 dynamicParams)
{

    if (StreamPreset == ALG_VID_ENC_PRESET_SVC_AUTO)
    {
		staticParams->videnc2Params.encodingPreset = XDM_USER_DEFINED;
		staticParams->interCodingParams.interCodingPreset =
            IH264_INTERCODING_HIGH_SPEED;
        staticParams->interCodingParams.meAlgoMode =
            IH264ENC_MOTIONESTMODE_HIGH_SPEED;
		staticParams->intraCodingParams.intraCodingPreset =
            IH264_INTRACODING_HIGH_SPEED;
		staticParams->transformBlockSize  = IH264_TRANSFORM_8x8 ;
        staticParams->svcCodingParams.svcExtensionFlag =
            IH264_SVC_EXTENSION_FLAG_ENABLE;
        staticParams->referencePicMarking = IH264_LONG_TERM_PICTURE;
        staticParams->numTemporalLayer = IH264_TEMPORAL_LAYERS_4;
    }
    else if (StreamPreset == ALG_VID_ENC_PRESET_HIGHQUALITY)
    {
        staticParams->videnc2Params.encodingPreset = XDM_USER_DEFINED;
        staticParams->interCodingParams.interCodingPreset =
            IH264_INTERCODING_DEFAULT;
        staticParams->interCodingParams.meAlgoMode =
            IH264ENC_MOTIONESTMODE_DEFAULT;
        staticParams->intraCodingParams.intraCodingPreset =
            IH264_INTRACODING_DEFAULT;
        staticParams->transformBlockSize = IH264_TRANSFORM_DEFAULT;
    }
    else /*if ((StreamPreset == ALG_VID_ENC_PRESET_AUTO)||
    		(StreamPreset == ALG_VID_ENC_PRESET_HIGHSPEED)) */
    {
        staticParams->videnc2Params.encodingPreset = XDM_USER_DEFINED;
        staticParams->interCodingParams.interCodingPreset =
            IH264_INTERCODING_HIGH_SPEED;
        staticParams->interCodingParams.meAlgoMode =
            IH264ENC_MOTIONESTMODE_HIGH_SPEED;
        staticParams->intraCodingParams.intraCodingPreset =
            IH264_INTRACODING_HIGH_SPEED;
        staticParams->transformBlockSize = IH264_TRANSFORM_8x8;
    }

	if(staticParams->videnc2Params.profile != IH264_HIGH_PROFILE )
	{
		staticParams->transformBlockSize = IH264_TRANSFORM_4x4;
		staticParams->rateControlParams.scalingMatrixPreset = IH264_SCALINGMATRIX_NONE;
		dynamicParams->rateControlParams.scalingMatrixPreset = IH264_SCALINGMATRIX_NONE;
		staticParams->intraCodingParams.intraCodingPreset = IH264_INTRACODING_DEFAULT;
		dynamicParams->intraCodingParams.intraCodingPreset = IH264_INTRACODING_DEFAULT;
		staticParams->interCodingParams.interCodingPreset = IH264_INTERCODING_USERDEFINED;
		dynamicParams->interCodingParams.interCodingPreset = IH264_INTERCODING_USERDEFINED;
	}

	staticParams->rateControlParams.maxPicSizeRatioI = 640;
	dynamicParams->rateControlParams.maxPicSizeRatioI = 640;

	/*
	Disable enablePRC will make High frequency area consume more bit
	*/
	/*
	staticParams->rateControlParams.enablePRC = 0;
	dynamicParams->rateControlParams.enablePRC = 0;
	*/

    staticParams->maxIntraFrameInterval = 60;

    return 0;
}

static Int enclink_h264_set_algObject(EncLink_H264Obj * algObj,
                                      EncLink_AlgCreateParams * algCreateParams,
                                      EncLink_AlgDynamicParams *
                                      algDynamicParams)
{
    IH264ENC_InArgs *inArgs;
    IH264ENC_OutArgs *outArgs;
    IVIDEO2_BufDesc *inputBufDesc;
    XDM2_BufDesc *outputBufDesc;
    IH264ENC_Status *status;
    Int i;

    inArgs = &algObj->inArgs;
    outArgs = &algObj->outArgs;
    inputBufDesc = &algObj->inBufs;
    outputBufDesc = &algObj->outBufs;
    status = &algObj->status;

     /*-----------------------------------------------------------------------*/
    /* Initialize the input ID in input arguments to the bufferid of the */
    /* buffer element returned from getfreebuffer() function.  */
     /*-----------------------------------------------------------------------*/
    /* inputID need to update before every encode process call */

    inArgs->videnc2InArgs.inputID = 0;
    inArgs->videnc2InArgs.control = IVIDENC2_CTRL_DEFAULT;

    outArgs->control = IH264ENC_CTRL_WRITE_NOREFUPDATE;
    outArgs->numStaticMBs = 0;
    outArgs->vbvBufferLevel = 0;
    outArgs->bytesGeneratedBotField = 1;
    outArgs->videnc2OutArgs.extendedError = 0;
    outArgs->videnc2OutArgs.bytesGenerated = 0;
    outArgs->videnc2OutArgs.encodedFrameType = IVIDEO_I_FRAME;
    outArgs->videnc2OutArgs.inputFrameSkip = 0;
    memset(&outArgs->videnc2OutArgs.freeBufID, 0,
           sizeof(outArgs->videnc2OutArgs.freeBufID));
    outArgs->videnc2OutArgs.reconBufs.planeDesc[0].buf = NULL;
    outArgs->videnc2OutArgs.reconBufs.planeDesc[1].buf = NULL;
    outArgs->videnc2OutArgs.reconBufs.imagePitch[0] = 0;

    /*------------------------------------------------------------------------*/
    /* Initialise output discriptors */
    /*------------------------------------------------------------------------*/
    outputBufDesc->numBufs = 0;
    for (i = 0; i < algObj->status.videnc2Status.bufInfo.minNumOutBufs; i++)
    {

        outputBufDesc->numBufs++;
        outputBufDesc->descs[i].memType = XDM_MEMTYPE_RAW;
        outputBufDesc->descs[i].bufSize.bytes =
            algObj->status.videnc2Status.bufInfo.minOutBufSize[i].bytes;

        if (i == 0)
        {
        /*-------------------------------------------------------------------*/
            /* Set proper buffer addresses for bitstream data */
        /*-------------------------------------------------------------------*/
            outputBufDesc->descs[0].buf = NULL;
        }
        else
        {
			if(algCreateParams->mvDataSize < status->videnc2Status.bufInfo.minOutBufSize[i].bytes)
			{
				Vps_printf("ENCLINK:H264Enc !!WARNING!!!Allocated MV data size (%d) is less than calculated by codec (%d)\n",
						   algCreateParams->mvDataSize,status->videnc2Status.bufInfo.minOutBufSize[i].bytes);
			}
			
            /* Check for required size vs Memory allocated for Analytic info buffer.*/
            //UTILS_assert((status->videnc2Status.bufInfo.minOutBufSize[i].bytes <
            //            algCreateParams->mvDataSize));

        /*-------------------------------------------------------------------*/
            /* Set proper buffer addresses for MV & SAD data */
        /*-------------------------------------------------------------------*/
            outputBufDesc->descs[i].buf = NULL;
        }
    }

    /*------------------------------------------------------------------------*/
    /* Video buffer layout, field interleaved or field separated */
    /* Only IVIDEO_FIELD_INTERLEAVED and IVIDEO_FIELD_SEPARATED are supported
     */
    /*------------------------------------------------------------------------*/
    inputBufDesc->dataLayout = algCreateParams->dataLayout;

    /*------------------------------------------------------------------------*/
    /* Flag to indicate field order in interlaced content */
    /* Supported values are */
    /* 0 - Bottom field first */
    /* 1 - Top filed first */
    /* TODO : need to find defalut parameter */
    /*------------------------------------------------------------------------*/
    inputBufDesc->topFieldFirstFlag = 1;

    /*------------------------------------------------------------------------*/
    /* Initialize the input buffer properties as required by algorithm */
    /* based on info received by preceding GETBUFINFO call.  */
    /*------------------------------------------------------------------------*/
    inputBufDesc->numPlanes = 2;/* status.videnc2Status.bufInfo.minNumInBufs; */
    inputBufDesc->numMetaPlanes = 0;
    /*------------------------------------------------------------------------*/
    /* Set entire Image region in the buffer by using config parameters */
    /*------------------------------------------------------------------------*/
    inputBufDesc->imageRegion.topLeft.x = algDynamicParams->startX;
    inputBufDesc->imageRegion.topLeft.y = algDynamicParams->startY;
    inputBufDesc->imageRegion.bottomRight.x = algDynamicParams->startX +
        algObj->dynamicParams.videnc2DynamicParams.inputWidth;
    inputBufDesc->imageRegion.bottomRight.y = algDynamicParams->startY +
        algObj->dynamicParams.videnc2DynamicParams.inputHeight;
    /*------------------------------------------------------------------------*/
    /* Set proper Image region in the buffer by using config parameters */
    /*------------------------------------------------------------------------*/
    inputBufDesc->activeFrameRegion.topLeft.x = algDynamicParams->startX;
    inputBufDesc->activeFrameRegion.topLeft.y = algDynamicParams->startY;
    inputBufDesc->activeFrameRegion.bottomRight.x = algDynamicParams->startX +
        algObj->dynamicParams.videnc2DynamicParams.inputWidth;
    inputBufDesc->activeFrameRegion.bottomRight.y = algDynamicParams->startY +
        algObj->dynamicParams.videnc2DynamicParams.inputHeight;
    /*------------------------------------------------------------------------*/
    /* Image pitch is capture width */
    /*------------------------------------------------------------------------*/
    if (algCreateParams->tilerEnable)
    {
        inputBufDesc->imagePitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
    }
    else
    {
        inputBufDesc->imagePitch[0] =
            algObj->dynamicParams.videnc2DynamicParams.captureWidth;
    }
    if (algCreateParams->tilerEnable)
    {
        inputBufDesc->imagePitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
    }
    else
    {
        inputBufDesc->imagePitch[1] =
            algObj->dynamicParams.videnc2DynamicParams.captureWidth;
    }
    /*------------------------------------------------------------------------*/
    /* Set Content type and chroma format from encoder parameters */
    /*------------------------------------------------------------------------*/
    inputBufDesc->contentType =
        algObj->staticParams.videnc2Params.inputContentType;
    inputBufDesc->chromaFormat =
        algObj->staticParams.videnc2Params.inputChromaFormat;

    /*------------------------------------------------------------------------*/
    /* Assign memory pointers adn sizes for the all the input buffers */
    /*------------------------------------------------------------------------*/
    for (i = 0; i < algObj->status.videnc2Status.bufInfo.minNumInBufs; i++)
    {
        inputBufDesc->planeDesc[i].buf = NULL;
        if (algCreateParams->tilerEnable)
        {
            if (i & 0x1)
            {
                inputBufDesc->planeDesc[i].memType = XDM_MEMTYPE_TILED16;
            }
            else
            {
                inputBufDesc->planeDesc[i].memType = XDM_MEMTYPE_TILED8;
            }
            inputBufDesc->planeDesc[i].bufSize.tileMem.width =
                algObj->status.videnc2Status.bufInfo.minInBufSize[i].tileMem.
                width;
            inputBufDesc->planeDesc[i].bufSize.tileMem.height =
                algObj->status.videnc2Status.bufInfo.minInBufSize[i].tileMem.
                height;
        }
        else
        {
            inputBufDesc->planeDesc[i].memType = XDM_MEMTYPE_RAW;
            inputBufDesc->planeDesc[i].bufSize.bytes =
                algObj->status.videnc2Status.bufInfo.minInBufSize[i].tileMem.
                width *
                algObj->status.videnc2Status.bufInfo.minInBufSize[i].tileMem.
                height;

        }
    }
    /*------------------------------------------------------------------------*/
    /* Set second field offset width and height according to input data */
    /* When second field of the input data starts at 0 it represents 2 fields
     */
    /* are seperated and provided at 2 diff process calls (60 process call) */
    /*------------------------------------------------------------------------*/
    if ((inputBufDesc->dataLayout == IVIDEO_FIELD_SEPARATED) &&
        (algCreateParams->singleBuf == FALSE) &&
        (algObj->staticParams.videnc2Params.inputContentType ==
         IVIDEO_INTERLACED))
    {
        inputBufDesc->secondFieldOffsetHeight[0] = 0;
        inputBufDesc->secondFieldOffsetHeight[1] = 0;
        inputBufDesc->secondFieldOffsetHeight[2] = 0;
    }
    else
    {
        inputBufDesc->secondFieldOffsetHeight[0] =
            algObj->dynamicParams.videnc2DynamicParams.inputHeight;
        inputBufDesc->secondFieldOffsetHeight[1] =
            (algObj->dynamicParams.videnc2DynamicParams.inputHeight >> 1);
        inputBufDesc->secondFieldOffsetHeight[2] =
            (algObj->dynamicParams.videnc2DynamicParams.inputHeight >> 1);
    }
    inputBufDesc->secondFieldOffsetWidth[0] = 0;
    inputBufDesc->secondFieldOffsetWidth[1] = 0;
    inputBufDesc->secondFieldOffsetWidth[2] = 0;

    /*------------------------------------------------------------------------*/
    /* Set The address of unregistered user data in meta data plane desc */
    /*------------------------------------------------------------------------*/
    inputBufDesc->numMetaPlanes = 0;
    /*------------------------------------------------------------------------*/
    /* Provide approprate buffer addresses for both the supported meta data: */
    /* A. USer defined SEI message */
    /* B. User Defined Scaling MAtrices */
    /*------------------------------------------------------------------------*/
    if (algObj->staticParams.videnc2Params.
        metadataType[inputBufDesc->numMetaPlanes] ==
        IH264_SEI_USER_DATA_UNREGISTERED)
    {
        inputBufDesc->metadataPlaneDesc[inputBufDesc->numMetaPlanes].buf = NULL;
        inputBufDesc->metadataPlaneDesc[inputBufDesc->numMetaPlanes].bufSize.
            bytes = -1;
        inputBufDesc->numMetaPlanes++;
    }
    /*------------------------------------------------------------------------*/
    /* Set proper buffer addresses for user defined scaling matrix */
    /*------------------------------------------------------------------------*/
    if (algObj->staticParams.videnc2Params.
        metadataType[inputBufDesc->numMetaPlanes] ==
        IH264_USER_DEFINED_SCALINGMATRIX)
    {
        inputBufDesc->metadataPlaneDesc[inputBufDesc->numMetaPlanes].buf = NULL;
        inputBufDesc->metadataPlaneDesc[inputBufDesc->numMetaPlanes].bufSize.
            bytes =
            /* -1; */
            896;
        inputBufDesc->numMetaPlanes++;
    }

    return 0;
}
static XDAS_Int32 enclink_h264_dummy_get_buffer_fxn(XDM_DataSyncHandle dataSyncHandle,
                                                    XDM_DataSyncDesc *dataSyncDesc)
{
    Vps_printf("ENCLINK:H264Enc !!WARNING!!!Unable to handle runtime output buffer request\n");
    return -1;
}


static Int enclink_h264_set_dynamic_params(IH264ENC_DynamicParams *
                                           dynamicParams,
                                           EncLink_AlgDynamicParams *
                                           algDynamicParams)
{
    *dynamicParams = H264ENC_TI_DYNAMICPARAMS;

    dynamicParams->videnc2DynamicParams.inputWidth =
        algDynamicParams->inputWidth;
    dynamicParams->videnc2DynamicParams.inputHeight =
        algDynamicParams->inputHeight;
    dynamicParams->videnc2DynamicParams.sampleAspectRatioWidth =
        algDynamicParams->inputWidth;
    dynamicParams->videnc2DynamicParams.sampleAspectRatioHeight =
        algDynamicParams->inputHeight;

    dynamicParams->videnc2DynamicParams.captureWidth =
        algDynamicParams->inputPitch;
    dynamicParams->videnc2DynamicParams.targetBitRate =
        algDynamicParams->targetBitRate;
    dynamicParams->videnc2DynamicParams.targetFrameRate =
        algDynamicParams->targetFrameRate;
    dynamicParams->videnc2DynamicParams.interFrameInterval =
        algDynamicParams->interFrameInterval;
    dynamicParams->videnc2DynamicParams.intraFrameInterval =
        algDynamicParams->intraFrameInterval;
    dynamicParams->videnc2DynamicParams.mvAccuracy =
        algDynamicParams->mvAccuracy;
    dynamicParams->videnc2DynamicParams.refFrameRate =
        algDynamicParams->refFrameRate;
    dynamicParams->videnc2DynamicParams.ignoreOutbufSizeFlag = XDAS_TRUE;
    dynamicParams->videnc2DynamicParams.getBufferFxn =
                                       enclink_h264_dummy_get_buffer_fxn;

	/* Rate control params */
    dynamicParams->rateControlParams.VBRDuration =
        algDynamicParams->vbrDuration;
    dynamicParams->rateControlParams.VBRsensitivity =
        algDynamicParams->vbrSensitivity;
    dynamicParams->rateControlParams.rateControlParamsPreset
        = IH264_RATECONTROLPARAMS_USERDEFINED;
    dynamicParams->rateControlParams.qpMinI = algDynamicParams->qpMinI;
    dynamicParams->rateControlParams.qpMaxI = algDynamicParams->qpMaxI;
    dynamicParams->rateControlParams.qpI    = algDynamicParams->qpInitI;
    dynamicParams->rateControlParams.qpMinP = algDynamicParams->qpMinP;
    dynamicParams->rateControlParams.qpMaxP = algDynamicParams->qpMaxP;
    dynamicParams->rateControlParams.qpP    = algDynamicParams->qpInitP;
    dynamicParams->rateControlParams.rcAlgo = algDynamicParams->rcAlg;

    dynamicParams->rateControlParams.discardSavedBits = 1;

    if(dynamicParams->rateControlParams.rcAlgo == IH264_RATECONTROL_PRC)
    {
        dynamicParams->rateControlParams.HRDBufferSize
            = 2 * algDynamicParams->targetBitRate;
    }
    else
    {
        dynamicParams->rateControlParams.HRDBufferSize
            = algDynamicParams->targetBitRate/2;
    }

    dynamicParams->rateControlParams.initialBufferLevel
        = dynamicParams->rateControlParams.HRDBufferSize;

    dynamicParams->rateControlParams.frameSkipThMulQ5 = 0;
    dynamicParams->rateControlParams.vbvUseLevelThQ5 = 0;
    dynamicParams->rateControlParams.skipDistributionWindowLength = 5;
    dynamicParams->rateControlParams.numSkipInDistributionWindow = 1;
    dynamicParams->rateControlParams.scalingMatrixPreset = IH264_SCALINGMATRIX_NORMAL;

    dynamicParams->sliceCodingParams.sliceCodingPreset =
        IH264_SLICECODING_USERDEFINED;

	if (algDynamicParams->encodePreset == ALG_VID_ENC_PRESET_HIGHQUALITY)
	{
		dynamicParams->interCodingParams.interCodingPreset =
			IH264_INTERCODING_DEFAULT;
		dynamicParams->interCodingParams.meAlgoMode =
			IH264ENC_MOTIONESTMODE_DEFAULT;
		dynamicParams->intraCodingParams.intraCodingPreset =
			IH264_INTRACODING_DEFAULT;
	}
	else
	{
		dynamicParams->interCodingParams.interCodingPreset =
			IH264_INTERCODING_HIGH_SPEED;
		dynamicParams->interCodingParams.meAlgoMode =
			IH264ENC_MOTIONESTMODE_HIGH_SPEED;
		dynamicParams->intraCodingParams.intraCodingPreset =
			IH264_INTRACODING_HIGH_SPEED;
	}

    return 0;
}

#define ENCLINKH264_ALGREATE_RSRC_NONE                                       (0)
#define ENCLINKH264_ALGREATE_RSRC_ALGCREATED                           (1 <<  0)
#define ENCLINKH264_ALGREATE_RSRC_IRES_ASSIGNED                        (1 <<  1)
#define ENCLINKH264_ALGREATE_RSRC_ALL (                                        \
                                       ENCLINKH264_ALGREATE_RSRC_ALGCREATED |  \
                                       ENCLINKH264_ALGREATE_RSRC_IRES_ASSIGNED \
                                      )

static Void enclink_h264_freersrc(EncLink_H264Obj * hObj, Int rsrcMask)
{
    if (rsrcMask & ENCLINKH264_ALGREATE_RSRC_IRES_ASSIGNED)
    {
        IRES_Status iresStatus;

        IRESMAN_TiledMemoryForceDisableTileAlloc_UnRegister((IALG_Handle) hObj->algHandle);
        iresStatus =
            RMAN_freeResources((IALG_Handle) hObj->algHandle,
                               &H264ENC_TI_IRES, hObj->scratchID);
        if (iresStatus != IRES_OK)
        {
            ENCLINK_INTERNAL_ERROR_LOG(iresStatus, "RMAN_freeResources");
        }
    }
    if (rsrcMask & ENCLINKH264_ALGREATE_RSRC_ALGCREATED)
    {
        enc_link_h264_delete(hObj->scratchID,hObj->algHandle);
        hObj->algHandle = NULL;
    }
}



Int EncLinkH264_algCreate(EncLink_H264Obj * hObj,
                          EncLink_AlgCreateParams * algCreateParams,
                          EncLink_AlgDynamicParams * algDynamicParams,
                          Int linkID, Int channelID, Int scratchGroupID)
{
    Int retVal = ENC_LINK_S_SUCCESS;
    Int rsrcMask = ENCLINKH264_ALGREATE_RSRC_NONE;
    Int algStatus;

    UTILS_assert(Utils_encdecIsH264(algCreateParams->format) == TRUE);
    hObj->format = algCreateParams->format;
    hObj->linkID = linkID;
    hObj->channelID = channelID;
    hObj->scratchID = scratchGroupID;

    memset(&hObj->inArgs, 0, sizeof(hObj->inArgs));
    memset(&hObj->outArgs, 0, sizeof(hObj->outArgs));
    memset(&hObj->inBufs, 0, sizeof(hObj->inBufs));
    memset(&hObj->outBufs, 0, sizeof(hObj->outBufs));
    memset(&hObj->status, 0, sizeof(hObj->status));
    memset(&hObj->memUsed, 0, sizeof(hObj->memUsed));

    hObj->status.videnc2Status.size = sizeof(IH264ENC_Status);
    hObj->inArgs.videnc2InArgs.size = sizeof(IH264ENC_InArgs);
    hObj->outArgs.videnc2OutArgs.size = sizeof(IH264ENC_OutArgs);
    hObj->staticParams.videnc2Params.size = sizeof(IH264ENC_Params);
    hObj->dynamicParams.videnc2DynamicParams.size =
        sizeof(IH264ENC_DynamicParams);

    algDynamicParams->encodePreset = algCreateParams->StreamPreset;

    enclink_h264_set_static_params(&hObj->staticParams, algCreateParams);
    enclink_h264_set_dynamic_params(&hObj->dynamicParams, algDynamicParams);
    enclink_h264_set_params_stream_preset(algCreateParams->StreamPreset,
                                          &hObj->staticParams,
                                          &hObj->dynamicParams);

    if(hObj->staticParams.rateControlParams.rcAlgo == IH264_RATECONTROL_PRC)
    {
        /* [IH264_RATECONTROL_PRC] Variable Bitrate*/
        hObj->staticParams.rateControlParams.HRDBufferSize
            = 2 * hObj->dynamicParams.videnc2DynamicParams.targetBitRate;
        hObj->staticParams.rateControlParams.initialBufferLevel
            =     hObj->staticParams.rateControlParams.HRDBufferSize;
    }
    else if(hObj->staticParams.rateControlParams.rcAlgo == IH264_RATECONTROL_PRC_LOW_DELAY)
    {
        hObj->staticParams.rateControlParams.HRDBufferSize
            = hObj->dynamicParams.videnc2DynamicParams.targetBitRate/2;
        hObj->staticParams.rateControlParams.initialBufferLevel
            = hObj->staticParams.rateControlParams.HRDBufferSize;
    }

#ifdef STATIC_PARAMETER_PRINT
 if(channelID < 2)
        memcpy(&createParamSave[channelID], &hObj->staticParams, sizeof(IH264ENC_Params) );
#endif

    UTILS_MEMLOG_USED_START();
    hObj->algHandle =
        enc_link_h264_create((IH264ENC_Fxns *) & H264ENC_TI_IH264ENC,
                             &hObj->staticParams,scratchGroupID);
    UTILS_assertError((NULL != hObj->algHandle),
                      retVal, ENC_LINK_E_ALGCREATEFAILED, linkID, channelID);
    if (!UTILS_ISERROR(retVal))
    {
        IRES_Status iresStatus;

        rsrcMask |= ENCLINKH264_ALGREATE_RSRC_ALGCREATED;
        if (FALSE == algCreateParams->tilerEnable)
        {
            IRESMAN_TiledMemoryForceDisableTileAlloc_Register((IALG_Handle) hObj->algHandle);
        }
        iresStatus = RMAN_assignResources((IALG_Handle) hObj->algHandle,
                                          &H264ENC_TI_IRES, scratchGroupID);
        UTILS_assertError((iresStatus == IRES_OK), retVal,
                          ENC_LINK_E_RMANRSRCASSIGNFAILED, linkID, channelID);
    }
    if (!UTILS_ISERROR(retVal))
    {
        Int ivaChID;

        rsrcMask |= ENCLINKH264_ALGREATE_RSRC_IRES_ASSIGNED;

        hObj->status.videnc2Status.data.buf = &(hObj->versionInfo[0]);
        hObj->status.videnc2Status.data.bufSize = sizeof(hObj->versionInfo);
        algStatus = enclink_h264_control(hObj->algHandle, XDM_GETVERSION,
                                         &(hObj->dynamicParams),
                                         &(hObj->status));
        if (algStatus == XDM_EOK)
        {
            ENCLINK_VERBOSE_INFO_LOG(hObj->linkID, hObj->channelID,
                                     "H264EncCreated:%s", hObj->versionInfo);

        }
        ivaChID = Utils_encdecMapLinkChID2IvaChID(linkID, channelID);
        ENCLINK_VERBOSE_INFO_LOG(hObj->linkID, hObj->channelID, "IVAChID:%d",
                                 ivaChID);
        hObj->dynamicParams.videnc2DynamicParams.lateAcquireArg = ivaChID;
        hObj->ivaChID = ivaChID;
        algStatus = enclink_h264_control(hObj->algHandle,
                                         XDM_SETLATEACQUIREARG,
                                         &hObj->dynamicParams, &hObj->status);
        if (algStatus == XDM_EOK)
        {
            ENCLINK_VERBOSE_INFO_LOG(hObj->linkID, hObj->channelID,
                                     "LateAcquireArg set:%d", ivaChID);
        }
        Utils_encdecRegisterAlgHandle(ivaChID, hObj->algHandle);
        algStatus = enclink_h264_control(hObj->algHandle,
                                         XDM_SETDEFAULT,
                                         &hObj->dynamicParams, &hObj->status);
        UTILS_assertError((algStatus == XDM_EOK), retVal,
                          ENC_LINK_E_ALGSETPARAMSFAILED, linkID, channelID);
    }
    if (!UTILS_ISERROR(retVal))
    {
        algStatus = enclink_h264_control(hObj->algHandle,
                                         XDM_SETPARAMS,
                                         &hObj->dynamicParams, &hObj->status);
        UTILS_assertError((algStatus == XDM_EOK), retVal,
                          ENC_LINK_E_ALGSETPARAMSFAILED, linkID, channelID);
    }

    if (!UTILS_ISERROR(retVal))
    {
        enclink_h264_control(hObj->algHandle,
                             XDM_GETSTATUS,
                             &hObj->dynamicParams, &hObj->status);
    }
    if (!UTILS_ISERROR(retVal))
    {
        algStatus =
            enclink_h264_control(hObj->algHandle,
                                 XDM_GETBUFINFO,
                                 &hObj->dynamicParams, &hObj->status);
        UTILS_assertError((algStatus == XDM_EOK), retVal,
                          ENC_LINK_E_ALGGETBUFINFOFAILED, linkID, channelID);
    }
    if (UTILS_ISERROR(retVal))
    {
        enclink_h264_freersrc(hObj, rsrcMask);
    }
    else
    {
        /* Initialize the Inarg, OutArg, InBuf & OutBuf objects */
        enclink_h264_set_algObject(hObj, algCreateParams, algDynamicParams);
    }

    UTILS_MEMLOG_USED_END(hObj->memUsed);
    UTILS_MEMLOG_PRINT("ENCLINK_H264",
                       hObj->memUsed,
                       (sizeof(hObj->memUsed) / sizeof(hObj->memUsed[0])));

    return retVal;
}

Void EncLinkH264_algDelete(EncLink_H264Obj * hObj)
{
    UTILS_MEMLOG_FREE_START();
    if (hObj->algHandle)
    {
        enclink_h264_freersrc(hObj, ENCLINKH264_ALGREATE_RSRC_ALL);
    }

    if (hObj->algHandle)
    {
        enc_link_h264_delete(hObj->scratchID,hObj->algHandle);
    }
    UTILS_MEMLOG_FREE_END(hObj->memUsed, 0 /* dont care */ );
}

Int32 EncLinkH264_algSetConfig(EncLink_algObj * algObj)
{
    Int32 status = ENC_LINK_S_SUCCESS;
    UInt32 bitMask;
    Bool setConfigFlag = FALSE;
    UInt key;

    key = Hwi_disable();
    bitMask = algObj->setConfigBitMask;

    /* Set the modified encoder bitRate value */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_BITRATE) & 0x1)
    {

        algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.
                targetBitRate = algObj->algDynamicParams.targetBitRate;
        if (algObj->algDynamicParams.rateControl ==
            ALG_VID_ENC_RATE_CONTROL_CBR) {
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                HRDBufferSize = algObj->algDynamicParams.targetBitRate * 2;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = algObj->algDynamicParams.targetBitRate * 2;
        }
        else
        {
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                HRDBufferSize = algObj->algDynamicParams.targetBitRate * 2;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = algObj->algDynamicParams.targetBitRate * 2;
        }

        Vps_printf("\n ENCLINK: new targetbitrate to set:%d \n",
                algObj->algDynamicParams.targetBitRate);

        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_BITRATE));
        setConfigFlag = TRUE;
    }

    /* Set the modified encoder Fps value */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_FPS) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.
                targetFrameRate = algObj->algDynamicParams.targetFrameRate;
        algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.
                refFrameRate  = algObj->algDynamicParams.targetFrameRate;
        algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.
                targetBitRate = algObj->algDynamicParams.targetBitRate;

        Vps_printf("\n ENCLINK: new targetframerate to set:%d \n",
                algObj->algDynamicParams.targetFrameRate);

        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_FPS));
        setConfigFlag = TRUE;
    }

	/* user selects 1080p@30 and not 60 fps
	 * we can enable high quality encoder  */
	/* also if it's not high profile, should not use HIGH SPEED*/
	/* 2013/02/27 make this check always happen */
	if(algObj->algCreateParams.StreamPreset == ALG_VID_ENC_PRESET_AUTO ||
		algObj->algCreateParams.StreamPreset == ALG_VID_ENC_PRESET_SVC_AUTO)
	{
		 if(algObj->u.h264AlgIfObj.staticParams.videnc2Params.profile != IH264_HIGH_PROFILE)
		 {

			algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.interCodingPreset = IH264_INTERCODING_USERDEFINED;
			algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.meAlgoMode = IH264ENC_MOTIONESTMODE_DEFAULT;
			algObj->u.h264AlgIfObj.dynamicParams.intraCodingParams.intraCodingPreset = IH264_INTRACODING_USERDEFINED;
			algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeP = IH264_BLOCKSIZE_8x8;
			algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeB = IH264_BLOCKSIZE_8x8;
	 	}
	 	else {
#ifdef USE_MCTNF
		 	if(algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.targetFrameRate <= 25000)
		 	{
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.interCodingPreset = IH264_INTERCODING_USERDEFINED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.meAlgoMode = IH264ENC_MOTIONESTMODE_DEFAULT;
				algObj->u.h264AlgIfObj.dynamicParams.intraCodingParams.intraCodingPreset = IH264_INTRACODING_USERDEFINED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeP = IH264_BLOCKSIZE_8x8;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeB = IH264_BLOCKSIZE_8x8;
		 	}
		 	else { /* Back to HIGH SPEED */
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.interCodingPreset = IH264_INTERCODING_HIGH_SPEED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.meAlgoMode = IH264ENC_MOTIONESTMODE_HIGH_SPEED;
				algObj->u.h264AlgIfObj.dynamicParams.intraCodingParams.intraCodingPreset = IH264_INTRACODING_HIGH_SPEED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeP = IH264_BLOCKSIZE_16x16;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeB = IH264_BLOCKSIZE_16x16;
		 	}
#else
		 	if((algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.inputWidth >= 1280) &&
		  		(algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.inputHeight >= 720) &&
			 	(algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.targetFrameRate <= 30000))
		 	{
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.interCodingPreset = IH264_INTERCODING_USERDEFINED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.meAlgoMode = IH264ENC_MOTIONESTMODE_DEFAULT;
				algObj->u.h264AlgIfObj.dynamicParams.intraCodingParams.intraCodingPreset = IH264_INTRACODING_USERDEFINED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeP = IH264_BLOCKSIZE_8x8;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeB = IH264_BLOCKSIZE_8x8;
		 	}
		 	else { /* Back to HIGH SPEED */
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.interCodingPreset = IH264_INTERCODING_HIGH_SPEED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.meAlgoMode = IH264ENC_MOTIONESTMODE_HIGH_SPEED;
				algObj->u.h264AlgIfObj.dynamicParams.intraCodingParams.intraCodingPreset = IH264_INTRACODING_HIGH_SPEED;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeP = IH264_BLOCKSIZE_16x16;
				algObj->u.h264AlgIfObj.dynamicParams.interCodingParams.minBlockSizeB = IH264_BLOCKSIZE_16x16;
		 	}
#endif
	   	}
	}

    /* Set the modified encoder Intra Frame Interval(GOP) value */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_INTRAI) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.
                intraFrameInterval = algObj->algDynamicParams.intraFrameInterval;

		/*
		 *  maxPicSizeRatioI should be adjusted according to intraFrameInterval value to
		 *  satisfy the following condition:
		 *  maxPicSizeRatioI <= (intraFrameInterval * 32)
		 */
		if(algObj->algDynamicParams.intraFrameInterval > 1)
		{
			algObj->u.h264AlgIfObj.dynamicParams.
					rateControlParams.maxPicSizeRatioI = \
						(algObj->algDynamicParams.intraFrameInterval * 32);
		}
		else
		{
			algObj->u.h264AlgIfObj.dynamicParams.
					rateControlParams.maxPicSizeRatioI = 0;
		}

        Vps_printf("\n ENCLINK: new intraFrameInterval to set:%d,new maxPicSizeRatioI to set = %d \n",
                algObj->algDynamicParams.intraFrameInterval,algObj->u.h264AlgIfObj.dynamicParams.
				rateControlParams.maxPicSizeRatioI);
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_INTRAI));
        setConfigFlag = TRUE;
    }

    /* toggle Force IDR */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_FORCEI) & 0x1)
    {

        algObj->algDynamicParams.forceFrame = TRUE;
        algObj->algDynamicParams.forceFrameStatus = FALSE;

		//algObj->getConfigFlag = TRUE;
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_FORCEI));
        setConfigFlag = TRUE;
    }
    /** to support Force IDR frame: Entry **/
    if ((algObj->algDynamicParams.forceFrame == TRUE) &&
        (algObj->algDynamicParams.forceFrameStatus == FALSE))
    {
        /** SET forceIDR **/
        algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.forceFrame =
                IVIDEO_IDR_FRAME;
        algObj->algDynamicParams.forceFrameStatus = TRUE;
    }
    else if((algObj->algDynamicParams.forceFrame == TRUE) &&
            (algObj->algDynamicParams.forceFrameStatus == TRUE))
    {
        /** UNSET forceIDR **/
        algObj->u.h264AlgIfObj.dynamicParams.videnc2DynamicParams.forceFrame =
                IVIDEO_NA_FRAME;
        algObj->algDynamicParams.forceFrame = FALSE;

        setConfigFlag = TRUE;
    }
    /** to support Force IDR frame: Exit **/

    /* Set the modified encoder QP range values for Intra Frame */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_QPI) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rateControlParamsPreset = IH264_RATECONTROLPARAMS_USERDEFINED;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMinI 	= algObj->algDynamicParams.qpMinI;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxI 	= algObj->algDynamicParams.qpMaxI;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpI 		= algObj->algDynamicParams.qpInitI;

        Vps_printf("\n ENCLINK: new QP I Param to set:%d %d %d\n",
                algObj->algDynamicParams.qpMinI, algObj->algDynamicParams.qpMaxI, algObj->algDynamicParams.qpInitI);
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_QPI));
        setConfigFlag = TRUE;
    }

    /* Set the modified encoder QP range values for Inter Frame */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_QPP) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rateControlParamsPreset = IH264_RATECONTROLPARAMS_USERDEFINED;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMinP 	= algObj->algDynamicParams.qpMinP;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxP 	= algObj->algDynamicParams.qpMaxP;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpP 		= algObj->algDynamicParams.qpInitP;
        Vps_printf("\n ENCLINK: new QP P Param to set:%d %d %d\n",
                algObj->algDynamicParams.qpMinP, algObj->algDynamicParams.qpMaxP, algObj->algDynamicParams.qpInitP);

        /* This is a workaround for the codec bug of SDOCM00087325*/
        {
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpOffsetB = 0;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxB = algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxP;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMinB = algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxB;
        }

        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_QPP));
        setConfigFlag = TRUE;
    }

	/* Set the modified encoder packet size */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_PACKETSIZE) & 0x1)
    {
        /* 0 will not be accpeted by encoder */
        if (algObj->algDynamicParams.packetSize == 100 ||
            algObj->algDynamicParams.packetSize == 0)
        {
            algObj->u.h264AlgIfObj.dynamicParams.sliceCodingParams.sliceMode =
                IH264_SLICEMODE_DEFAULT;
            Vps_printf("\n ENCLINK: new PacketSize Param to set:%d \n",
                       algObj->algDynamicParams.packetSize);
        }
        else
        {
            algObj->u.h264AlgIfObj.dynamicParams.sliceCodingParams.sliceMode =
                IH264_SLICEMODE_MBUNIT;
            int inputWidth =
                algObj->u.h264AlgIfObj.dynamicParams.
                videnc2DynamicParams.inputWidth;
            inputWidth = ((inputWidth + 15) / 16) * 16;
            int inputHeight =
                algObj->u.h264AlgIfObj.dynamicParams.
                videnc2DynamicParams.inputHeight;

            /* in case that height like 1080 will be aligned by 16 bytes to
             * 1088 */
            inputHeight = ((inputHeight + 15) / 16) * 16;
            int macroBlockSize = 16 * 16;

            int sliceSize =
                inputWidth * inputHeight * algObj->algDynamicParams.packetSize /
                (macroBlockSize * 100);
            algObj->u.h264AlgIfObj.dynamicParams.
                sliceCodingParams.sliceUnitSize = sliceSize;

            Vps_printf("\n ENCLINK: new PacketSize Param to set:%d \n",
                       algObj->u.h264AlgIfObj.dynamicParams.
                       sliceCodingParams.sliceUnitSize);
        }
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 <<
                                      ENC_LINK_SETCONFIG_BITMASK_PACKETSIZE));
        setConfigFlag = TRUE;
    }

	/* Set the modified ROI params */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_ROI) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.enableROI =
            algObj->algDynamicParams.roiNumOfRegion;
        IH264ENC_InArgs *inArgs = &algObj->u.h264AlgIfObj.inArgs;

        inArgs->roiInputParams.numOfROI =
            algObj->u.h264AlgIfObj.dynamicParams.enableROI;
        int i = 0;

        for (i = 0; i < inArgs->roiInputParams.numOfROI; i++)
        {
            inArgs->roiInputParams.listROI[i].topLeft.x =
                algObj->algDynamicParams.roiStartX[i];
            inArgs->roiInputParams.listROI[i].topLeft.y =
                algObj->algDynamicParams.roiStartY[i];
            inArgs->roiInputParams.listROI[i].bottomRight.x =
                algObj->algDynamicParams.roiStartX[i] +
                algObj->algDynamicParams.roiWidth[i];
            inArgs->roiInputParams.listROI[i].bottomRight.y =
                algObj->algDynamicParams.roiStartY[i] +
                algObj->algDynamicParams.roiHeight[i];
            inArgs->roiInputParams.roiType[i] = algObj->algDynamicParams.roiType[i];    /* IH264_FACE_OBJECT
                                                                                         * 0
                                                                                         * ,
                                                                                         * IH264_FOREGROUND_OBJECT
                                                                                         * 2 */
            inArgs->roiInputParams.roiPriority[i] = ALG_VID_ENC_ROI_PRIORITY_HIGH;  /* -8
                                                                                     * <->
                                                                                     * 8 */
#if 0
            Vps_printf
                ("\n ==> ENCLINK: new ROI Param to set:%d :%d %d %d %d %d %d\n",
                 i, inArgs->roiInputParams.listROI[i].topLeft.x,
                 inArgs->roiInputParams.listROI[i].topLeft.y,
                 inArgs->roiInputParams.listROI[i].bottomRight.x,
                 inArgs->roiInputParams.listROI[i].bottomRight.y,
                 inArgs->roiInputParams.roiType[i],
                 inArgs->roiInputParams.roiPriority[i]);
#endif
        }

        if (algObj->algDynamicParams.FDNumOfRegion > 0)
        {
            int j = 0;

            for (i = algObj->algDynamicParams.roiNumOfRegion;
                 i <
                 algObj->algDynamicParams.roiNumOfRegion +
                 algObj->algDynamicParams.FDNumOfRegion; i++, j++)
            {
                inArgs->roiInputParams.listROI[i].topLeft.x =
                    algObj->algDynamicParams.FDStartX[j],
                    inArgs->roiInputParams.listROI[i].topLeft.y =
                    algObj->algDynamicParams.FDStartY[j],
                    inArgs->roiInputParams.listROI[i].bottomRight.x =
                    algObj->algDynamicParams.FDStartX[j] +
                    algObj->algDynamicParams.FDWidth[j],
                    inArgs->roiInputParams.listROI[i].bottomRight.y =
                    algObj->algDynamicParams.FDStartY[j] +
                    algObj->algDynamicParams.FDHeight[j];
                inArgs->roiInputParams.roiType[i] = IH264_PRIVACY_MASK;
                inArgs->roiInputParams.roiPriority[i] =
                    ALG_VID_ENC_ROI_PRIORITY_HIGH;
#if 0
                Vps_printf
                    ("\n ===> ENCLINK: new ROI Param to set:%d :%d %d %d %d %d %d\n",
                     i, inArgs->roiInputParams.listROI[i].topLeft.x,
                     inArgs->roiInputParams.listROI[i].topLeft.y,
                     inArgs->roiInputParams.listROI[i].bottomRight.x,
                     inArgs->roiInputParams.listROI[i].bottomRight.y,
                     inArgs->roiInputParams.roiType[i],
                     inArgs->roiInputParams.roiPriority[i]);
#endif
            }
            algObj->u.h264AlgIfObj.dynamicParams.enableROI +=
                algObj->algDynamicParams.FDNumOfRegion;
            inArgs->roiInputParams.numOfROI =
                algObj->u.h264AlgIfObj.dynamicParams.enableROI;
        }

        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ENC_LINK_SETCONFIG_BITMASK_ROI));
        setConfigFlag = TRUE;
    }

	/* Set the modified encoder input key */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_INPUTKEY) & 0x1)
    {
        IH264ENC_InArgs *inArgs = &algObj->u.h264AlgIfObj.inArgs;
        inArgs->inputKey = algObj->algDynamicParams.inputKey;

        Vps_printf("\n ENCLINK: new Encode WaterMask inputkey Param to set:%d\n",
                   algObj->algDynamicParams.inputKey );

        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 <<
                                      ENC_LINK_SETCONFIG_BITMASK_INPUTKEY));
        setConfigFlag = TRUE;
    }

	/* Set the modified encoder Encode preset */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_ENCODEPRESET) & 0x1)
    {
		/* will do nothing, please see enclink_h264_set_params_stream_preset */
        Vps_printf("\n ENCLINK: new Encode Preset Param to set:%d\n",
                   algObj->algDynamicParams.encodePreset);

        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 <<
                                      ENC_LINK_SETCONFIG_BITMASK_ENCODEPRESET));
        setConfigFlag = TRUE;
    }

    /* Set the modified encoder rateControl value */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_RCALGO) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
            rateControlParamsPreset = IH264_RATECONTROLPARAMS_USERDEFINED;

        /* 0=> VBR, 1=> CBR, 2=> CVBR */
        if (algObj->algDynamicParams.rateControl ==
            ALG_VID_ENC_RATE_CONTROL_CBR)
        {                                                  /* IH264_RATECONTROL_PRC_LOW_DELAY
                                                            * will set CBR */
          /* Before GA3.0, set HRDBufferSize=initialBufferLevel=targetBitRate/2
             when it's CBR, but t will make the quality in low bitrate not good sometime,
             change it to the same buffer size as VBR.
          */
		  algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rcAlgo =
			IH264_RATECONTROL_PRC_LOW_DELAY;
          algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                HRDBufferSize = algObj->algDynamicParams.targetBitRate * 2;
          algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = algObj->algDynamicParams.targetBitRate * 2;
        }
        else if (algObj->algDynamicParams.rateControl ==
                 ALG_VID_ENC_RATE_CONTROL_VBR)
        {                                                  /* IH264_RATECONTROL_PRC/IH264_RATECONTROL_DEFAULT
                                                            * will set VBR */
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rcAlgo =
                IH264_RATECONTROL_PRC;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                HRDBufferSize = algObj->algDynamicParams.targetBitRate * 2;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = algObj->algDynamicParams.targetBitRate * 2;
        }
        else if (algObj->algDynamicParams.rateControl ==
                 ALG_VID_ENC_RATE_CONTROL_CVBR)
        {
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rcAlgo =
                IH264_RATECONTROL_PRC;
            /*
             * 1. qpInit = QP (or -1) 2. qpMin = QP 3. qpMax = Max(Default
             * Codec QPmax, QP) 4..rcAlgo = VBR 5. discardSavedBits = 1 (if
             * version is 00.02.00.03) 6. targetBitRate = max bit rate 7.
             * maxBitRate = -1 */
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpI =
                algObj->algDynamicParams.qpInitI;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMinI =
                algObj->algDynamicParams.qpInitI;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxI = 36;

            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpP =
                algObj->algDynamicParams.qpInitI;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMinP =
                algObj->algDynamicParams.qpInitI;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxP = 40;

            algObj->u.h264AlgIfObj.dynamicParams.
                rateControlParams.discardSavedBits = 1;
            algObj->u.h264AlgIfObj.dynamicParams.
                videnc2DynamicParams.targetBitRate =
                algObj->algDynamicParams.targetBitRate;
            algObj->u.h264AlgIfObj.staticParams.videnc2Params.maxBitRate = -1;

        }
        else
        {	/* TURN OFF RC */
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rcAlgo =
                IH264_RATECONTROL_PRC;

            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                HRDBufferSize = algObj->algDynamicParams.targetBitRate * 2;
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = algObj->algDynamicParams.targetBitRate * 2;

            /* In order to overwrite qp value, put rcAlg behind qp process */
            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMinI =
                algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxI =
                algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpI =
                algObj->algDynamicParams.qpInitI;

            algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMinP =
                algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpMaxP =
                algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.qpP =
                algObj->algDynamicParams.qpInitP;
        }

        Vps_printf("\n ENCLINK: new rate control algorithm  to set:%d \n",
                   algObj->algDynamicParams.rateControl);
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ENC_LINK_SETCONFIG_BITMASK_RCALGO));
        setConfigFlag = TRUE;
    }

    /* Set the modified encoder VBRDuration value for CVBR */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_VBRD) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rateControlParamsPreset = IH264_RATECONTROLPARAMS_USERDEFINED;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.VBRDuration = algObj->algDynamicParams.vbrDuration;
        Vps_printf("\n ENCLINK: new VBR Duration Param to set:%d\n",
                   algObj->algDynamicParams.vbrDuration);
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_VBRD));
        setConfigFlag = TRUE;
    }
    /* Set the modified encoder VBRsensitivity value for CVBR */
    if ((bitMask >>  ENC_LINK_SETCONFIG_BITMASK_VBRS) & 0x1)
    {
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.rateControlParamsPreset = IH264_RATECONTROLPARAMS_USERDEFINED;
        algObj->u.h264AlgIfObj.dynamicParams.rateControlParams.VBRsensitivity = algObj->algDynamicParams.vbrSensitivity;
        Vps_printf("\n ENCLINK: new VBR Sensitivity Param to set:%d\n",
                   algObj->algDynamicParams.vbrSensitivity);
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                    (1 << ENC_LINK_SETCONFIG_BITMASK_VBRS));
        setConfigFlag = TRUE;
    }

    Hwi_restore(key);

    if (setConfigFlag)
    {
        status = enclink_h264_control(algObj->u.h264AlgIfObj.algHandle,
                                         XDM_SETPARAMS,
                                         &algObj->u.h264AlgIfObj.dynamicParams,
                                         &algObj->u.h264AlgIfObj.status);
        if (UTILS_ISERROR(status))
        {
            UTILS_warn("\n ENCLINK: ERROR in Run time parameters changes, "
                  "Extended Error code:%d \n",
                  algObj->u.h264AlgIfObj.status.videnc2Status.extendedError);
        }
        /*else
        {
            Vps_printf("\n ENCLINK: Run time parameters changed %d\n",
                algObj->u.h264AlgIfObj.status.videnc2Status.extendedError);
        }*/
     }

    return (status);
}

Int32 EncLinkH264_algGetConfig(EncLink_algObj * algObj)
{
    Int retVal = ENC_LINK_S_SUCCESS, chId;
    IH264ENC_DynamicParams dynamicParams;
    IH264ENC_Status status;

    if(algObj->getConfigFlag == TRUE)
    {
        status.videnc2Status.size = sizeof(IH264ENC_Status);
        dynamicParams.videnc2DynamicParams.size = sizeof(IH264ENC_DynamicParams);

        retVal = enclink_h264_control(algObj->u.h264AlgIfObj.algHandle,
                                         XDM_GETSTATUS,
                                         &dynamicParams,
                                         &status);
        if (UTILS_ISERROR(retVal))
        {
            UTILS_warn("\n ENCLINK: ERROR in Run time parameters changes,"
                  "Extended Error code:%d \n",
            status.videnc2Status.extendedError);
        }

        chId = algObj->u.h264AlgIfObj.channelID;

#ifdef STATIC_PARAMETER_PRINT
        enclink_h264_print_static_params(&createParamSave[chId], chId);
#endif
        enclink_h264_print_dynamic_params(chId, (IH264ENC_Status*)&status.videnc2Status);

        algObj->getConfigFlag = FALSE;

        algObj->algDynamicParams.inputWidth =
              status.videnc2Status.encDynamicParams.inputWidth;
        algObj->algDynamicParams.inputHeight =
              status.videnc2Status.encDynamicParams.inputHeight;
        algObj->algDynamicParams.targetBitRate =
              status.videnc2Status.encDynamicParams.targetBitRate;
        algObj->algDynamicParams.targetFrameRate =
              status.videnc2Status.encDynamicParams.targetFrameRate;
        algObj->algDynamicParams.intraFrameInterval =
              status.videnc2Status.encDynamicParams.intraFrameInterval;
        algObj->algDynamicParams.forceFrame =
              status.videnc2Status.encDynamicParams.forceFrame;
        algObj->algDynamicParams.refFrameRate =
              status.videnc2Status.encDynamicParams.refFrameRate;
    }

    return (retVal);
}

Int EncLinkH264_algDynamicParamUpdate(EncLink_H264Obj * hObj,
                               EncLink_AlgCreateParams * algCreateParams,
                               EncLink_AlgDynamicParams * algDynamicParams)
{
    Int retVal = ENC_LINK_S_SUCCESS;

    enclink_h264_set_dynamic_params(&hObj->dynamicParams, algDynamicParams);
    enclink_h264_set_algObject(hObj, algCreateParams, algDynamicParams);

    return (retVal);
}

Int32 EncLinkH264_checkFdRoi(UInt32 startX, UInt32 startY, UInt32 width,
                             UInt32 height, UInt32 frameWidth,
                             UInt32 frameHeight)
{
    if ((startX + width) > frameWidth)
        return ENC_LINK_E_FAIL;

    if ((startY + height) > frameHeight)
        return ENC_LINK_E_FAIL;

    return ENC_LINK_S_SUCCESS;
}

#ifdef CODEC_PRIV_MASK

Int32 EncLinkH264_algSetFdRoi(EncLink_algObj * pAlgObj,
                              EncLink_ReqList * reqList, Int32 chId)
{
    Int32 i, status = ENC_LINK_S_SUCCESS;

    EncLink_ReqObj *pReqObj;

    UInt32 frameAddrY, frameAddrUV,  frameDataFormat;

    UInt32 framePitch, frameWidth, frameHeight;

    UInt32 blankingDataSize, fdResultOffset;

    FdLink_result *pFdResult;

    pReqObj = reqList->reqObj[0];

    frameAddrY = (UInt32) pReqObj->InFrameList.frames[0]->addr[0][0];
    frameAddrUV = (UInt32) pReqObj->InFrameList.frames[0]->addr[0][1];
    framePitch = pAlgObj->algDynamicParams.inputPitch;
    frameWidth = pAlgObj->algDynamicParams.inputWidth;
    frameHeight = pAlgObj->algDynamicParams.inputHeight;
    frameDataFormat = pAlgObj->algCreateParams.inputChromaFormat;

    if (chId == 0)
    {
        blankingDataSize = BLANKING_DATA_SIZE;
    }
    else
    {
        blankingDataSize = 0;
    }

    if ((frameDataFormat == FVID2_DF_YUV420SP_VU) ||
        (frameDataFormat == FVID2_DF_YUV420SP_UV) ||
        (frameDataFormat == FVID2_DF_YUV420P))
    {
        //fdResultOffset =
        //    ((framePitch * frameHeight * 3) >> 1) + blankingDataSize;
        fdResultOffset = (frameAddrUV - frameAddrY) +
                ((framePitch * frameHeight) >> 1  )  + blankingDataSize;
    }
    else
    {
        fdResultOffset = (framePitch * frameHeight << 1) + blankingDataSize;
    }

    pFdResult = (FdLink_result *) ((UInt32) frameAddrY + fdResultOffset);

    if (pFdResult->numFaces > 35)
    {
        return (status);
    }

    pAlgObj->algDynamicParams.FDNumOfRegion = pFdResult->numFaces;

    for (i = 0; i < pAlgObj->algDynamicParams.FDNumOfRegion; i++)
    {
        status = EncLinkH264_checkFdRoi(pFdResult->faceInfo[i].startX,
                                        pFdResult->faceInfo[i].startY,
                                        pFdResult->faceInfo[i].width,
                                        pFdResult->faceInfo[i].height,
                                        frameWidth, frameHeight);

        if (status != ENC_LINK_S_SUCCESS)
        {
            return (status);
        }

        pAlgObj->algDynamicParams.FDStartX[i] = pFdResult->faceInfo[i].startX;
        pAlgObj->algDynamicParams.FDStartY[i] = pFdResult->faceInfo[i].startY;
        pAlgObj->algDynamicParams.FDWidth[i] = pFdResult->faceInfo[i].width;
        pAlgObj->algDynamicParams.FDHeight[i] = pFdResult->faceInfo[i].height;
    }

    pAlgObj->setConfigBitMask |= (1 << ENC_LINK_SETCONFIG_BITMASK_ROI);

    return (status);
}

#endif

static Int enclink_print_dynamic_params(IVIDENC2_DynamicParams *videnc2DynamicParams)
{
    Vps_printf("videnc2DynamicParams -> inputHeight             : %d\n", videnc2DynamicParams->inputHeight);
    Vps_printf("videnc2DynamicParams -> inputWidth              : %d\n", videnc2DynamicParams->inputWidth);
    Vps_printf("videnc2DynamicParams -> refFrameRate            : %d\n", videnc2DynamicParams->refFrameRate);
    Vps_printf("videnc2DynamicParams -> targetFrameRate         : %d\n", videnc2DynamicParams->targetFrameRate);
    Vps_printf("videnc2DynamicParams -> targetBitRate           : %d\n", videnc2DynamicParams->targetBitRate);
    Vps_printf("videnc2DynamicParams -> intraFrameInterval      : %d\n", videnc2DynamicParams->intraFrameInterval);
    Vps_printf("videnc2DynamicParams -> generateHeader          : %d\n", videnc2DynamicParams->generateHeader);
    Vps_printf("videnc2DynamicParams -> captureWidth            : %d\n", videnc2DynamicParams->captureWidth);
    Vps_printf("videnc2DynamicParams -> forceFrame              : %d\n", videnc2DynamicParams->forceFrame);
    Vps_printf("videnc2DynamicParams -> interFrameInterval      : %d\n", videnc2DynamicParams->interFrameInterval);
    Vps_printf("videnc2DynamicParams -> mvAccuracy              : %d\n", videnc2DynamicParams->mvAccuracy);
    Vps_printf("videnc2DynamicParams -> sampleAspectRatioHeight : %d\n", videnc2DynamicParams->sampleAspectRatioHeight);
    Vps_printf("videnc2DynamicParams -> sampleAspectRatioWidth  : %d\n", videnc2DynamicParams->sampleAspectRatioWidth);
    Vps_printf("videnc2DynamicParams -> ignoreOutbufSizeFlag    : %d\n", videnc2DynamicParams->ignoreOutbufSizeFlag);
    Vps_printf("videnc2DynamicParams -> lateAcquireArg          : %d\n", videnc2DynamicParams->lateAcquireArg);
    return 0;
}


static Int enclink_h264_print_IH264ENC_RateControlParams(const IH264ENC_RateControlParams *prms)
{
	Vps_printf("rateControlParams -> rateControlParamsPreset	: %d\n", prms -> rateControlParamsPreset);
	Vps_printf("rateControlParams -> scalingMatrixPreset		: %d\n", prms -> scalingMatrixPreset);
	Vps_printf("rateControlParams -> rcAlgo						: %d\n", prms -> rcAlgo);
	Vps_printf("rateControlParams -> qpI						: %d\n", prms -> qpI);
	Vps_printf("rateControlParams -> qpMaxI						: %d\n", prms -> qpMaxI);
	Vps_printf("rateControlParams -> qpMinI						: %d\n", prms -> qpMinI);
	Vps_printf("rateControlParams -> qpP						: %d\n", prms -> qpP);
	Vps_printf("rateControlParams -> qpMaxP						: %d\n", prms -> qpMaxP);
	Vps_printf("rateControlParams -> qpMinP						: %d\n", prms -> qpMinP);
	Vps_printf("rateControlParams -> qpOffsetB					: %d\n", prms -> qpOffsetB);
	Vps_printf("rateControlParams -> qpMaxB						: %d\n", prms -> qpMaxB);
	Vps_printf("rateControlParams -> qpMinB						: %d\n", prms -> qpMinB);
	Vps_printf("rateControlParams -> allowFrameSkip				: %d\n", prms -> allowFrameSkip);
	Vps_printf("rateControlParams -> removeExpensiveCoeff		: %d\n", prms -> removeExpensiveCoeff);
	Vps_printf("rateControlParams -> chromaQPIndexOffset		: %d\n", prms -> chromaQPIndexOffset);
	Vps_printf("rateControlParams -> IPQualityFactor			: %d\n", prms -> IPQualityFactor);
	Vps_printf("rateControlParams -> initialBufferLevel			: %d\n", prms -> initialBufferLevel);
	Vps_printf("rateControlParams -> HRDBufferSize				: %d\n", prms -> HRDBufferSize);
	Vps_printf("rateControlParams -> minPicSizeRatioI			: %d\n", prms -> minPicSizeRatioI);
	Vps_printf("rateControlParams -> maxPicSizeRatioI			: %d\n", prms -> maxPicSizeRatioI);
	Vps_printf("rateControlParams -> minPicSizeRatioP			: %d\n", prms -> minPicSizeRatioP);
	Vps_printf("rateControlParams -> maxPicSizeRatioP			: %d\n", prms -> maxPicSizeRatioP);
	Vps_printf("rateControlParams -> minPicSizeRatioB			: %d\n", prms -> minPicSizeRatioB);
	Vps_printf("rateControlParams -> maxPicSizeRatioB			: %d\n", prms -> maxPicSizeRatioB);
	Vps_printf("rateControlParams -> enablePRC					: %d\n", prms -> enablePRC);
	Vps_printf("rateControlParams -> enablePartialFrameSkip		: %d\n", prms -> enablePartialFrameSkip);
	Vps_printf("rateControlParams -> discardSavedBits			: %d\n", prms -> discardSavedBits);
	Vps_printf("rateControlParams -> VBRDuration				: %d\n", prms ->VBRDuration         )   ;
	Vps_printf("rateControlParams -> VBRsensitivity				: %d\n", prms ->VBRsensitivity      )    ;
	Vps_printf("rateControlParams -> skipDistributionWindowLength: %d\n", prms ->skipDistributionWindowLength);
	Vps_printf("rateControlParams -> numSkipInDistributionWindow : %d\n", prms ->numSkipInDistributionWindow);
	Vps_printf("rateControlParams -> enableHRDComplianceMode	: %d\n", prms ->enableHRDComplianceMode) ;
	Vps_printf("rateControlParams -> frameSkipThMulQ5			: %d\n", prms ->frameSkipThMulQ5   )     ;
	Vps_printf("rateControlParams -> vbvUseLevelThQ5			: %d\n", prms ->vbvUseLevelThQ5    )     ;
	return 0;
}

static Int enclink_h264_print_IH264ENC_InterCodingParams(const IH264ENC_InterCodingParams *prms)
{
	Vps_printf("interCodingParams-> interCodingPreset			: %d\n", prms -> interCodingPreset) ;
	Vps_printf("interCodingParams-> searchRangeHorP				: %d\n", prms -> searchRangeHorP) ;
	Vps_printf("interCodingParams-> searchRangeVerP				: %d\n", prms -> searchRangeVerP) ;
	Vps_printf("interCodingParams-> searchRangeHorB				: %d\n", prms -> searchRangeHorB) ;
	Vps_printf("interCodingParams-> searchRangeVerB				: %d\n", prms -> searchRangeVerB) ;
	Vps_printf("interCodingParams-> interCodingBias				: %d\n", prms -> interCodingBias) ;
	Vps_printf("interCodingParams-> skipMVCodingBias			: %d\n", prms -> skipMVCodingBias) ;
	Vps_printf("interCodingParams-> minBlockSizeP				: %d\n", prms -> minBlockSizeP) ;
	Vps_printf("interCodingParams-> minBlockSizeB				: %d\n", prms -> minBlockSizeB) ;
	Vps_printf("interCodingParams-> meAlgoMode					: %d\n", prms -> meAlgoMode) ;
	return 0;
}

static Int  enclink_h264_print_IH264ENC_IntraCodingParams(const IH264ENC_IntraCodingParams *prms)
{
	Vps_printf("intraCodingParams-> intraCodingPreset			: %d\n", prms -> intraCodingPreset) ;
	Vps_printf("intraCodingParams-> lumaIntra4x4Enable			: %d\n", prms -> lumaIntra4x4Enable) ;
	Vps_printf("intraCodingParams-> lumaIntra8x8Enable			: %d\n", prms -> lumaIntra8x8Enable) ;
	Vps_printf("intraCodingParams-> lumaIntra8x8Enable			: %d\n", prms -> lumaIntra16x16Enable) ;
	Vps_printf("intraCodingParams-> chromaIntra8x8Enable		: %d\n", prms -> chromaIntra8x8Enable) ;
	Vps_printf("intraCodingParams-> chromaComponentEnable		: %d\n", prms -> chromaComponentEnable) ;
	Vps_printf("intraCodingParams-> intraRefreshMethod			: %d\n", prms -> intraRefreshMethod) ;
	Vps_printf("intraCodingParams-> intraRefreshRate			: %d\n", prms -> intraRefreshRate) ;
	Vps_printf("intraCodingParams-> gdrOverlapRowsBtwFrames		: %d\n", prms -> gdrOverlapRowsBtwFrames) ;
	Vps_printf("intraCodingParams-> constrainedIntraPredEnable	: %d\n", prms -> constrainedIntraPredEnable) ;
	Vps_printf("intraCodingParams-> constrainedIntraPredEnable	: %d\n", prms -> intraCodingBias) ;
	return 0;
}

static Int  enclink_h264_print_IH264ENC_NALUControlParams(const IH264ENC_NALUControlParams *prms)
{
	Vps_printf("nalUnitControlParams -> naluControlPreset				: %d\n", prms -> naluControlPreset) ;
	Vps_printf("nalUnitControlParams -> naluPresentMaskStartOfSequence  : %d\n", prms -> naluPresentMaskStartOfSequence) ;
	Vps_printf("nalUnitControlParams -> naluPresentMaskIDRPicture		: %d\n", prms -> naluPresentMaskIDRPicture) ;
	Vps_printf("nalUnitControlParams -> naluPresentMaskIntraPicture		: %d\n", prms -> naluPresentMaskIntraPicture) ;
	Vps_printf("nalUnitControlParams -> naluPresentMaskNonIntraPicture	: %d\n", prms -> naluPresentMaskNonIntraPicture) ;
	Vps_printf("nalUnitControlParams -> naluPresentMaskEndOfSequence	: %d\n", prms -> naluPresentMaskEndOfSequence) ;
	return 0;
}

static Int  enclink_h264_print_IH264ENC_SliceCodingParams(const IH264ENC_SliceCodingParams *prms)
{
	Vps_printf("sliceCodingParams-> sliceCodingPreset 			: %d\n", prms -> sliceCodingPreset);
	Vps_printf("sliceCodingParams-> sliceMode  					: %d\n", prms -> sliceMode);
	Vps_printf("sliceCodingParams-> sliceUnitSize  				: %d\n", prms -> sliceUnitSize);
	Vps_printf("sliceCodingParams-> sliceStartOffset[IH264ENC_MAX_NUM_SLICE_START_OFFSET] : %d %d %d\n",
			prms -> sliceStartOffset[0], prms -> sliceStartOffset[1], prms -> sliceStartOffset[2]);
	Vps_printf("sliceCodingParams-> streamFormat  						: %d\n", prms -> streamFormat);
	return 0;
}

static Int  enclink_h264_print_IH264ENC_LoopFilterParams(const IH264ENC_LoopFilterParams *prms)
{
	Vps_printf("loopFilterParams-> loopfilterPreset				: %d\n", prms ->loopfilterPreset);
	Vps_printf("loopFilterParams-> loopfilterDisableIDC			: %d\n", prms ->loopfilterDisableIDC);
	Vps_printf("loopFilterParams-> filterOffsetA				: %d\n", prms ->filterOffsetA);
	Vps_printf("loopFilterParams-> filterOffsetB				: %d\n", prms ->filterOffsetB);
	return 0;
}

static Int  enclink_h264_print_IH264ENC_FMOCodingParams(const IH264ENC_FMOCodingParams *prms)
{
	Vps_printf("fmoCodingParams-> fmoCodingPreset				: %d\n", prms -> fmoCodingPreset);
	Vps_printf("fmoCodingParams-> numSliceGroups				: %d\n", prms -> numSliceGroups);
	Vps_printf("fmoCodingParams-> liceGroupMapType 				: %d\n", prms -> sliceGroupMapType);
	Vps_printf("fmoCodingParams-> liceGroupChangeDirectionFlag 	: %d\n", prms -> sliceGroupChangeDirectionFlag);
	Vps_printf("fmoCodingParams-> liceGroupChangeRate 			: %d\n", prms -> sliceGroupChangeRate);
	Vps_printf("fmoCodingParams-> sliceGroupChangeCycle			: %d\n", prms -> sliceGroupChangeCycle);
	Vps_printf("fmoCodingParams-> sliceGroupParams				: [%d %d] \n",
			prms -> sliceGroupParams[0], prms -> sliceGroupParams[1]) ;
	return 0;
}

static Int  enclink_h264_print_IH264ENC_VUICodingParams(const IH264ENC_VUICodingParams *prms)
{
	Vps_printf("vuiCodingParams-> vuiCodingPreset				: %d\n", prms -> vuiCodingPreset);
	Vps_printf("vuiCodingParams-> aspectRatioInfoPresentFlag	: %d\n", prms -> aspectRatioInfoPresentFlag);
	Vps_printf("vuiCodingParams-> aspectRatioIdc				: %d\n", prms -> aspectRatioIdc);
	Vps_printf("vuiCodingParams-> videoSignalTypePresentFlag	: %d\n", prms -> videoSignalTypePresentFlag);
	Vps_printf("vuiCodingParams-> videoFormat					: %d\n", prms -> videoFormat);
	Vps_printf("vuiCodingParams-> videoFullRangeFlag			: %d\n", prms -> videoFullRangeFlag);
	Vps_printf("vuiCodingParams-> timingInfoPresentFlag			: %d\n", prms -> timingInfoPresentFlag);
	Vps_printf("vuiCodingParams-> hrdParamsPresentFlag			: %d\n", prms -> hrdParamsPresentFlag);
	Vps_printf("vuiCodingParams-> numUnitsInTicks				: %d\n", prms -> numUnitsInTicks);
	return 0;
}
static Int  enclink_h264_print_IH264ENC_StereoInfoParams(const IH264ENC_StereoInfoParams *prms)
{
	Vps_printf("stereoInfoParams-> stereoInfoPreset				: %d\n", prms -> stereoInfoPreset);
	Vps_printf("stereoInfoParams-> topFieldIsLeftViewFlag		: %d\n", prms -> topFieldIsLeftViewFlag) ;
	Vps_printf("stereoInfoParams-> viewSelfContainedFlag		: %d\n", prms -> viewSelfContainedFlag) ;
	return 0;
}

static Int  enclink_h264_print_IH264ENC_FramePackingSEIParams(const IH264ENC_FramePackingSEIParams *prms)
{
	Vps_printf("framePackingSEIParams-> framePackingPreset		: %d\n", prms -> framePackingPreset);
	Vps_printf("framePackingSEIParams-> framePackingType		: %d\n", prms -> framePackingType) ;
	Vps_printf("framePackingSEIParams-> frame0PositionX			: %d\n", prms -> frame0PositionX) ;
	Vps_printf("framePackingSEIParams-> frame0PositionY			: %d\n", prms -> frame0PositionY) ;
	Vps_printf("framePackingSEIParams-> frame1PositionX			: %d\n", prms -> frame1PositionX) ;
	Vps_printf("framePackingSEIParams-> frame1PositionY			: %d\n", prms -> frame1PositionY) ;
	return 0;
}

static Int  enclink_h264_print_IH264ENC_SVCCodingParams(const IH264ENC_SVCCodingParams *prms)
{
	Vps_printf("svcCodingParams-> svcExtensionFlag				: %d\n",  prms -> svcExtensionFlag);
	Vps_printf("svcCodingParams-> dependencyID					: %d\n",  prms -> dependencyID);
	Vps_printf("svcCodingParams-> qualityID						: %d\n",  prms -> qualityID);
	Vps_printf("svcCodingParams-> enhancementProfileID			: %d\n",  prms -> enhancementProfileID);
	Vps_printf("svcCodingParams-> layerIndex					: %d\n",  prms -> layerIndex);
	Vps_printf("svcCodingParams-> refLayerDQId					: %d\n",  prms -> refLayerDQId);
	return 0;
}

#ifdef STATIC_PARAMETER_PRINT
static Int enclink_h264_print_static_params(const IH264ENC_Params * prms , int chId)
{
	Vps_printf(" \n");
	Vps_printf("--------- CH %d : H264 ENC : Create Params -------\n", chId);
	Vps_printf(" \n");
	Vps_printf("videnc2Params-> encodingPreset				: %d\n", prms -> videnc2Params.encodingPreset );
	Vps_printf("videnc2Params-> rateControlPreset			: %d\n", prms -> videnc2Params.rateControlPreset );
	Vps_printf("videnc2Params-> maxHeight					: %d\n", prms -> videnc2Params.maxHeight );
	Vps_printf("videnc2Params-> maxWidth					: %d\n", prms -> videnc2Params.maxWidth );
	Vps_printf("videnc2Params-> dataEndianness				: %d\n", prms -> videnc2Params.dataEndianness );
	Vps_printf("videnc2Params-> maxInterFrameInterval		: %d\n", prms -> videnc2Params.maxInterFrameInterval );
	Vps_printf("videnc2Params-> maxBitRate					: %d\n", prms -> videnc2Params.maxBitRate );
	Vps_printf("videnc2Params-> minBitRate					: %d\n", prms -> videnc2Params.minBitRate);
	Vps_printf("videnc2Params-> inputChromaFormat			: %d\n", prms -> videnc2Params.inputChromaFormat);
	Vps_printf("videnc2Params-> inputContentType			: %d\n", prms -> videnc2Params.inputContentType);
	Vps_printf("videnc2Params-> operatingMode				: %d\n", prms -> videnc2Params.operatingMode);
	Vps_printf("videnc2Params-> profile						: %d\n", prms -> videnc2Params.profile);
	Vps_printf("videnc2Params-> level						: %d\n", prms -> videnc2Params.level);
	Vps_printf("videnc2Params-> inputDataMode				: %d\n", prms -> videnc2Params.inputDataMode);
	Vps_printf("videnc2Params-> outputDataMode				: %d\n", prms -> videnc2Params.outputDataMode);

	enclink_h264_print_IH264ENC_RateControlParams(&prms -> rateControlParams) ; Vps_printf(" \n");
	enclink_h264_print_IH264ENC_InterCodingParams(&prms -> interCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_IntraCodingParams(&prms -> intraCodingParams ); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_NALUControlParams(&prms -> nalUnitControlParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_SliceCodingParams(&prms -> sliceCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_LoopFilterParams(&prms -> loopFilterParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_FMOCodingParams(&prms -> fmoCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_VUICodingParams(&prms -> vuiCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_StereoInfoParams(&prms -> stereoInfoParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_FramePackingSEIParams(&prms -> framePackingSEIParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_SVCCodingParams(&prms -> svcCodingParams); Vps_printf(" \n");

	Vps_printf("IH264ENC_Params-> interlaceCodingType		: %d\n", prms -> interlaceCodingType);
	Vps_printf("IH264ENC_Params-> bottomFieldIntra			: %d\n", prms -> bottomFieldIntra)   ;
	Vps_printf("IH264ENC_Params-> gopStructure				: %d\n", prms -> gopStructure)       ;
	Vps_printf("IH264ENC_Params-> entropyCodingMode			: %d\n", prms ->entropyCodingMode)  ;
	Vps_printf("IH264ENC_Params-> transformBlockSize		: %d\n", prms -> transformBlockSize );
	Vps_printf("IH264ENC_Params-> log2MaxFNumMinus4			: %d\n", prms -> log2MaxFNumMinus4)  ;
	Vps_printf("IH264ENC_Params-> picOrderCountType			: %d\n", prms -> picOrderCountType)  ;
	Vps_printf("IH264ENC_Params-> enableWatermark			: %d\n", prms ->enableWatermark)     ;
	Vps_printf("IH264ENC_Params-> IDRFrameInterval			: %d\n", prms ->IDRFrameInterval)   ;
	Vps_printf("IH264ENC_Params-> pConstantMemory			: %d\n", prms ->  pConstantMemory)    ;
	Vps_printf("IH264ENC_Params-> maxIntraFrameInterval		: %d\n", prms -> maxIntraFrameInterval)  ;
	Vps_printf("IH264ENC_Params-> debugTraceLevel			: %d\n", prms -> debugTraceLevel );
	Vps_printf("IH264ENC_Params-> lastNFramesToLog			: %d\n", prms -> lastNFramesToLog);
	Vps_printf("IH264ENC_Params-> enableAnalyticinfo		: %d\n", prms -> enableAnalyticinfo) ;
	Vps_printf("IH264ENC_Params-> enableGMVSei				: %d\n", prms -> enableGMVSei)  ;
	Vps_printf("IH264ENC_Params-> constraintSetFlags		: %d\n", prms -> constraintSetFlags);
	Vps_printf("IH264ENC_Params-> enableRCDO				: %d\n", prms -> enableRCDO);
	Vps_printf("IH264ENC_Params-> enableLongTermRefFrame	: %d\n", prms -> enableLongTermRefFrame)  ;
	Vps_printf("IH264ENC_Params-> LTRPPeriod				: %d\n", prms -> LTRPPeriod );
	Vps_printf("IH264ENC_Params-> numTemporalLayer			: %d\n", prms -> numTemporalLayer)  ;
	Vps_printf("IH264ENC_Params-> referencePicMarking		: %d\n", prms -> referencePicMarking)   ;
	return 0;
}
#endif


static Int enclink_h264_print_dynamic_params(UInt32 chId, IH264ENC_Status*
                                          h264_enc_status)
{
	Vps_printf(" \n");
	Vps_printf("--------- CH %d : H264 ENC : Dynamic Params -------\n", chId);
	Vps_printf(" \n");
	enclink_print_dynamic_params(&h264_enc_status->videnc2Status.encDynamicParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_RateControlParams(&h264_enc_status->rateControlParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_InterCodingParams(&h264_enc_status->interCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_IntraCodingParams(&h264_enc_status->intraCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_NALUControlParams(&h264_enc_status -> nalUnitControlParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_SliceCodingParams(&h264_enc_status -> sliceCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_LoopFilterParams(&h264_enc_status -> loopFilterParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_FMOCodingParams(&h264_enc_status -> fmoCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_VUICodingParams(&h264_enc_status -> vuiCodingParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_StereoInfoParams(&h264_enc_status -> stereoInfoParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_FramePackingSEIParams(&h264_enc_status -> framePackingSEIParams); Vps_printf(" \n");
	enclink_h264_print_IH264ENC_SVCCodingParams(&h264_enc_status -> svcCodingParams); Vps_printf(" \n");
	Vps_printf("interlaceCodingType				: %d\n", h264_enc_status->interlaceCodingType);
	Vps_printf("bottomFieldIntra				: %d\n", h264_enc_status->bottomFieldIntra);
	Vps_printf("gopStructure					: %d\n", h264_enc_status->gopStructure);
	Vps_printf("entropyCodingMode				: %d\n", h264_enc_status->entropyCodingMode);
	Vps_printf("transformBlockSize				: %d\n", h264_enc_status->transformBlockSize);
	Vps_printf("log2MaxFNumMinus4				: %d\n", h264_enc_status->log2MaxFNumMinus4);
	Vps_printf("picOrderCountType				: %d\n", h264_enc_status->picOrderCountType);
	Vps_printf("enableWatermark					: %d\n", h264_enc_status->enableWatermark);
	Vps_printf("IDRFrameInterval				: %d\n", h264_enc_status->IDRFrameInterval);
	Vps_printf("maxIntraFrameInterval			: %d\n", h264_enc_status->maxIntraFrameInterval);
	Vps_printf("debugTraceLevel					: %d\n", h264_enc_status->debugTraceLevel);
	Vps_printf("lastNFramesToLog				: %d\n", h264_enc_status->lastNFramesToLog);
	Vps_printf("enableAnalyticinfo				: %d\n", h264_enc_status->enableAnalyticinfo);
	Vps_printf("enableGMVSei					: %d\n", h264_enc_status->enableGMVSei);
	Vps_printf("constraintSetFlags				: %d\n", h264_enc_status->constraintSetFlags);
	Vps_printf("enableRCDO						: %d\n", h264_enc_status->enableRCDO);
	Vps_printf("enableLongTermRefFrame			: %d\n", h264_enc_status->enableLongTermRefFrame);
	Vps_printf("LTRPPeriod						: %d\n", h264_enc_status->LTRPPeriod);
	Vps_printf("searchCenter                    : %d\n", h264_enc_status->searchCenter);
	Vps_printf("enableStaticMBCount				: %d\n", h264_enc_status->enableStaticMBCount);
	Vps_printf("extMemoryDebugTraceAddr			: %d\n", h264_enc_status->extMemoryDebugTraceAddr);
	Vps_printf("numTemporalLayer				: %d\n", h264_enc_status->numTemporalLayer);
	Vps_printf("referencePicMarking:			%d\n", h264_enc_status->referencePicMarking);
	Vps_printf("extMemoryDebugTraceSize			: %d\n", h264_enc_status->extMemoryDebugTraceSize);
	Vps_printf("enableROI                       : %d\n", h264_enc_status->enableROI);
	Vps_printf(" \n");
	Vps_printf(" \n");
	return 0;
}

