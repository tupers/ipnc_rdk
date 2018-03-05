/** ==================================================================
 *  @file   encLink_mpeg4.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/iva_enc/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <xdc/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
#include <ti/sdo/fc/rman/rman.h>
#include <ti/xdais/dm/ividenc2.h>
#include <impeg4enc.h>
#include <mpeg4enc_ti.h>

#include "encLink_priv.h"
#include "encLink_mpeg4_priv.h"
#include <mcfw/src_bios6/links_m3video/codec_utils/utils_encdec.h>
#include <mcfw/src_bios6/links_m3video/codec_utils/iresman_hdvicp2_earlyacquire.h>
#include <mcfw/interfaces/link_api/fdLink.h>
#include "../alg/motion_detection/motiondetect_priv.h"

#if 0
#define ENCLINK_MPEG4_SETNALU_MASK_SPS(naluMask) ((naluMask) |= (1 << IMPEG4_NALU_TYPE_SPS))
#define ENCLINK_MPEG4_SETNALU_MASK_PPS(naluMask) ((naluMask) |= (1 << IMPEG4_NALU_TYPE_PPS))
#endif

#define ENCLINKMPEG4_ALGREATE_RSRC_NONE                                       (0)
#define ENCLINKMPEG4_ALGREATE_RSRC_ALGCREATED                           (1 <<  0)
#define ENCLINKMPEG4_ALGREATE_RSRC_IRES_ASSIGNED                        (1 <<  1)
#define ENCLINKMPEG4_ALGREATE_RSRC_ALL (                                        \
                                       ENCLINKMPEG4_ALGREATE_RSRC_ALGCREATED |  \
                                       ENCLINKMPEG4_ALGREATE_RSRC_IRES_ASSIGNED \
                                      )


/* ===================================================================
 *  @func     enc_link_Mpeg4_create
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
static IMPEG4ENC_Handle enc_link_mpeg4_create(Int scratchGroupID,
                                            const IMPEG4ENC_Fxns* fxns,
                                            const IMPEG4ENC_Params* prms);
/* ===================================================================
 *  @func     enc_link_mpeg4_delete
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
static Void enc_link_mpeg4_delete(IMPEG4ENC_Handle handle,Int scratchGroupID);

/* ===================================================================
 *  @func     enclink_mpeg4_control
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
static Int32 enclink_mpeg4_control(IMPEG4ENC_Handle handle,
                                  IMPEG4ENC_Cmd cmd,
                                  IMPEG4ENC_DynamicParams * params,
                                  IMPEG4ENC_Status * status);
/* ===================================================================
 *  @func     enclink_mpeg4_set_static_params
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
static Int enclink_mpeg4_set_static_params(IMPEG4ENC_Params * staticParams,
                                          EncLink_AlgCreateParams *
                                          algCreateParams);
/* ===================================================================
 *  @func     enclink_mpeg4_set_algObject
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
static Int enclink_mpeg4_set_algObject(EncLink_Mpeg4Obj * algObj,
                                      EncLink_AlgCreateParams * algCreateParams,
                                      EncLink_AlgDynamicParams *
                                      algDynamicParams);
/* ===================================================================
 *  @func     enclink_mpeg4_set_dynamic_params
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
static Int enclink_mpeg4_set_dynamic_params(IMPEG4ENC_DynamicParams *
                                           dynamicParams,
                                           EncLink_AlgDynamicParams *
                                           algDynamicParams);
/* ===================================================================
 *  @func     enclink_mpeg4_set_params_stream_preset
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
static Int enclink_mpeg4_set_params_stream_preset(Int32 StreamPreset,
                                                 IMPEG4ENC_Params* staticParams,
                                                 IMPEG4ENC_DynamicParams *
                                                 dynamicParams);

/* ===================================================================
 *  @func     enclink_mpeg4_freersrc
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
static Void enclink_mpeg4_freersrc(EncLink_Mpeg4Obj * hObj, Int rsrcMask);

extern IRES_Fxns MPEG4ENC_TI_IRES;

//extern const IMPEG4ENC_DynamicParams MPEG4ENC_TI_DYNAMICPARAMS;
extern IMPEG4ENC_DynamicParams MPEG4ENC_TI_DYNAMICPARAMS;

typedef struct sErrorMapping {
    XDAS_Int8 *errorName;
} sErrorMapping;

static sErrorMapping gErrorStringsMpeg4[32] = {
    (XDAS_Int8 *) "IMPEG4ENC_LEVEL_INCOMPLAINT_PARAMETER , 0, \0",
    (XDAS_Int8 *) "IMPEG4ENC_PROFILE_INCOMPLAINT_CONTENTTYPE = 1,\0",
    (XDAS_Int8 *) "IMPEG4ENC_PROFILE_INCOMPLAINT_FMO_SETTING = 2,",
    (XDAS_Int8 *) "IMPEG4ENC_PROFILE_INCOMPLAINT_TRANSFORMBLOCKSIZE = 3,\0",
    (XDAS_Int8 *) "IMPEG4ENC_PROFILE_INCOMPLAINT_INTERFRAMEINTERVAL = 4,\0",
    (XDAS_Int8 *) "IMPEG4ENC_PROFILE_INCOMPLAINT_SCALINGMATRIXPRESET = 5,\0",
    (XDAS_Int8 *) "IMPEG4ENC_PROFILE_INCOMPLAINT_ENTROPYCODINGMODE = 6,\0",
    (XDAS_Int8 *) "IMPEG4ENC_MAX_BIT_RATE_VOILATION  = 7,\0",
    (XDAS_Int8 *) "XDM_PARAMSCHANGE = 8,\0",
    (XDAS_Int8 *) "XDM_APPLIEDCONCEALMENT = 9,\0",
    (XDAS_Int8 *) "XDM_INSUFFICIENTDATA = 10,\0",
    (XDAS_Int8 *) "XDM_CORRUPTEDDATA = 11,\0",
    (XDAS_Int8 *) "XDM_CORRUPTEDHEADER = 12,\0",
    (XDAS_Int8 *) "XDM_UNSUPPORTEDINPUT = 13,\0",
    (XDAS_Int8 *) "XDM_UNSUPPORTEDPARAM = 14,\0",
    (XDAS_Int8 *) "XDM_FATALERROR = 15\0",
    (XDAS_Int8 *) "IMPEG4ENC_IMPROPER_HDVICP2_STATE = 16\0",
    (XDAS_Int8 *) "IMPEG4ENC_IMPROPER_STREAMFORMAT = 17,\0",
    (XDAS_Int8 *) "IMPEG4ENC_IMPROPER_POCTYPE = 18,\0",
    (XDAS_Int8 *) "IMPEG4ENC_IMPROPER_DATASYNC_SETTING = 19,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_VIDENC2PARAMS = 20,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_RATECONTROLPARAMS = 21,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_INTERCODINGPARAMS = 22,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_INTRACODINGPARAMS = 23,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_NALUNITCONTROLPARAMS = 24,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_SLICECODINGPARAMS = 25,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_LOOPFILTERPARAMS = 26,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_FMOCODINGPARAMS = 27,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_VUICODINGPARAMS = 28,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_MPEG4ENCPARAMS = 29,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_VIDENC2DYNAMICPARAMS = 30,\0",
    (XDAS_Int8 *) "IMPEG4ENC_UNSUPPORTED_MPEG4ENCDYNAMICPARAMS = 31, \0"
};

/**
********************************************************************************
 *  @fn     MPEG4ENC_TI_Report_Error
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

/* ===================================================================
 *  @func     MPEG4ENC_TI_Report_Error
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
XDAS_Int32 MPEG4ENC_TI_Report_Error(XDAS_Int32 uiErrorMsg)
{
    int i;

    if (uiErrorMsg)
    {
    /*------------------------------------------------------------------------*/
        /* Loop through all the bits in error message and map to the glibal */
        /* error string */
    /*------------------------------------------------------------------------*/
        for (i = 0; i < 32; i++)
        {
            if (uiErrorMsg & (1 << i))
            {
                Vps_printf("ERROR: %s \n", gErrorStringsMpeg4[i].errorName);
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
 *  ======== enc_link_mpeg4_create ========
 *  Create an MPEG4ENC instance object (using parameters specified by prms)
 */

/* ===================================================================
 *  @func     enc_link_mpeg4_create
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
static IMPEG4ENC_Handle enc_link_mpeg4_create(Int scratchGroupID,
                                             const IMPEG4ENC_Fxns * fxns,
                                            const IMPEG4ENC_Params * prms)
{
    return ((IMPEG4ENC_Handle) ALG_create(scratchGroupID,
                                         (IALG_Fxns *) fxns,
                                         NULL, (IALG_Params *) prms));
}

/*
 *  ======== enc_link_mpeg4_delete ========
 *  Delete the MPEG4ENC instance object specified by handle
 */

/* ===================================================================
 *  @func     enc_link_mpeg4_delete
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
static Void enc_link_mpeg4_delete(IMPEG4ENC_Handle handle,Int scratchGroupID)
{
    ALG_delete(scratchGroupID,(IALG_Handle) handle);
}

/*
 *  ======== enc_link_mpeg4_control ========
 */

/* ===================================================================
 *  @func     enclink_mpeg4_control
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
static Int32 enclink_mpeg4_control(IMPEG4ENC_Handle handle,
                                  IMPEG4ENC_Cmd cmd,
                                  IMPEG4ENC_DynamicParams * params,
                                  IMPEG4ENC_Status * status)
{
    int error = 0;
#if 1

    IALG_Fxns *fxns = (IALG_Fxns *) handle->fxns;

    fxns->algActivate((IALG_Handle) handle);

    error = handle->fxns->ividenc.control((IVIDENC2_Handle) handle, cmd,
                                          (IVIDENC2_DynamicParams *) params,
                                          (IVIDENC2_Status *) status);
    fxns->algDeactivate((IALG_Handle) handle);

    if (error != XDM_EOK)
    {
        ENCLINK_INTERNAL_ERROR_LOG(error, "ALGCONTROL FAILED:CMD:%d\n", cmd);
        MPEG4ENC_TI_Report_Error(status->videnc2Status.extendedError);
    }
#endif
    return error;
}

/*
 *  ======== Enclink_Mpeg4CalcSecondFieldOffsets ========
 */

/* ===================================================================
 *  @func     Enclink_mpeg4CalcSecondFieldOffsets
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
Int32 Enclink_mpeg4CalcSecondFieldOffsets(IVIDEO2_BufDesc * inputBufDesc,
                                         FVID2_Frame * secField,
                                         Bool tilerEnable)
{
    Int retVal = ENC_LINK_S_SUCCESS;


    UInt32 addr, i;

    Int32 addrOffset, secondFieldOffsetHeight, secondFieldOffsetWidth;

    for (i = 0; i < inputBufDesc->numPlanes; i++)
    {
        if (tilerEnable)
        {
            addr = Utils_tilerAddr2CpuAddr((UInt32) (secField->addr[0][i]));
        }
        else
        {
            addr = (UInt32) secField->addr[0][i];
        }

        addrOffset = addr - (UInt32) inputBufDesc->planeDesc[i].buf;

        secondFieldOffsetHeight = addrOffset / inputBufDesc->imagePitch[i];

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
 *  ======== Enclink_Mpeg4EncodeFrame ========
 */
/* ===================================================================
 *  @func     Enclink_Mpeg4EncodeFrame
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
Int32 Enclink_mpeg4EncodeFrame(EncLink_ChObj * pChObj, EncLink_ReqObj * pReqObj)
{
    int error = XDM_EOK;

    Int32 i;

    //Int32 reqObjIndex = 0;

    IMPEG4ENC_InArgs *inArgs;

    IMPEG4ENC_OutArgs *outArgs;

    IVIDEO2_BufDesc *inputBufDesc;

    XDM2_BufDesc *outputBufDesc;

    IMPEG4ENC_Handle handle;

    IALG_Fxns *fxns = NULL;

    //EncLink_ReqObj *pReqObj;

    IVIDEO_ContentType contentType;

    UInt32 mvDataBuf;

    void *allocatedRingBufferAddr;
    UInt32 allocatedRingBufferLength;
    //UInt32 frameWidth,frameHeight;
    RingBufferHandle_t ringBufferHnl;

    inArgs = &pChObj->algObj.u.mpeg4AlgIfObj.inArgs;
    outArgs = &pChObj->algObj.u.mpeg4AlgIfObj.outArgs;
    inputBufDesc = &pChObj->algObj.u.mpeg4AlgIfObj.inBufs;
    outputBufDesc = &pChObj->algObj.u.mpeg4AlgIfObj.outBufs;
    handle = pChObj->algObj.u.mpeg4AlgIfObj.algHandle;
    UTILS_assert(handle != NULL);
    fxns = (IALG_Fxns *) handle->fxns;

    //for (reqObjIndex = 0; reqObjIndex < reqList->numReqObj; reqObjIndex++)
    //{
        //pReqObj = reqList->reqObj[reqObjIndex];
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
                                            (pReqObj->InFrameList.
                                             frames[0]->addr[0][i]));
            }
            else
            {
                inputBufDesc->planeDesc[i].buf =
                    pReqObj->InFrameList.frames[0]->addr[0][i];
            }
        }

        if (pReqObj->InFrameList.numFrames == 2)
        {
            Enclink_mpeg4CalcSecondFieldOffsets(inputBufDesc,
                                               pReqObj->InFrameList.frames[1],
                                               pChObj->algObj.
                                               algCreateParams.tilerEnable);
        }

			//made the same changed to MJPEG
	//todo: make a uniform way to handle all codecs including H264 and MJPEG
	extern BitStreamRingBuffer ringBuffer[UTILS_BITBUF_MAX_ALLOC_POOLS];
	ringBufferHnl = &ringBuffer[pChObj->allocPoolID];

	 if(ringBufferHnl->wrOfst >= ringBufferHnl->rdOfst) // case1
	{
	     allocatedRingBufferAddr = (void *)((UInt32)ringBufferHnl->baseAddr + ringBufferHnl->wrOfst);
	     allocatedRingBufferLength = ringBufferHnl->totalSize - ringBufferHnl->wrOfst;

		//frameWidth = pReqObj->OutBuf->frameWidth;
		//frameHeight = pReqObj->OutBuf->frameHeight;
	     if(allocatedRingBufferLength <= ((inputBufDesc->imageRegion.bottomRight.x - inputBufDesc->imageRegion.topLeft.x) *
					(inputBufDesc->imageRegion.bottomRight.y - inputBufDesc->imageRegion.topLeft.y) * 3 /2))
	     {
	     		//Vps_printf ("*******early termination********\n");
			Utils_ringBuffer_updateWrOfst(ringBufferHnl, allocatedRingBufferLength);
	     		allocatedRingBufferAddr = (void *)((UInt32)ringBufferHnl->baseAddr + ringBufferHnl->wrOfst);
	     		allocatedRingBufferLength = ringBufferHnl->rdOfst - ringBufferHnl->wrOfst;
	     }
	}
	else // (ringBufferHnl->wrOfst < ringBufferHnl->rdOfst) // case1
	{
	     allocatedRingBufferAddr = (void *)((UInt32)ringBufferHnl->baseAddr + ringBufferHnl->wrOfst);
	     allocatedRingBufferLength = ringBufferHnl->rdOfst - ringBufferHnl->wrOfst;
	}

        pReqObj->OutBuf->mvDataFilledSize = 0;
        for (i = 0; i < outputBufDesc->numBufs; i++)
        {
            if (i == 0)
            {
                /* Set proper buffer addresses for bitstream data */
              /*---------------------------------------------------------------*/
                outputBufDesc->descs[i].buf = allocatedRingBufferAddr;
                outputBufDesc->descs[i].bufSize.bytes
					//= pReqObj->OutBuf->bufSize;
					= (inputBufDesc->imageRegion.bottomRight.x - inputBufDesc->imageRegion.topLeft.x) *
					(inputBufDesc->imageRegion.bottomRight.y - inputBufDesc->imageRegion.topLeft.y) * 3 /2;

            }
            if (i == 1)
            {
                /*-------------------------------------------------------------------*/
                /* Set proper buffer addresses for MV & SAD data */
                /*-------------------------------------------------------------------*/
                outputBufDesc->descs[1].buf = outputBufDesc->descs[0].buf +
                    (outputBufDesc->descs[0].bufSize.bytes -
                     ENC_LINK_MV_DATA_SIZE);

                pReqObj->OutBuf->mvDataFilledSize =
                    outputBufDesc->descs[1].bufSize.bytes;
                pReqObj->OutBuf->mvDataOffset =
                    outputBufDesc->descs[0].bufSize.bytes -
                    ENC_LINK_MV_DATA_SIZE;
            }
        }

        fxns->algActivate((IALG_Handle) handle);
        error =
            handle->fxns->ividenc.process((IVIDENC2_Handle) handle,
                                          inputBufDesc, outputBufDesc,
                                          (IVIDENC2_InArgs *) inArgs,
                                          (IVIDENC2_OutArgs *) outArgs);
        fxns->algDeactivate((IALG_Handle) handle);

        pReqObj->OutBuf->codingType = pChObj->algObj.u.mpeg4AlgIfObj.format;

	 pReqObj->OutBuf->addr = allocatedRingBufferAddr;
	 pReqObj->OutBuf->fillLength = outArgs->videnc2OutArgs.bytesGenerated;
	 pReqObj->OutBuf->bufSize =   outArgs->videnc2OutArgs.bytesGenerated;
	 pReqObj->OutBuf->startOffset = 0;

	 pReqObj->OutBuf->phyAddr = (UInt32 )pReqObj->OutBuf->addr;
	 Utils_ringBuffer_updateWrOfst(ringBufferHnl, pReqObj->OutBuf->bufSize);
	 Utils_ringBuffer_updateEncodedStreamCount(ringBufferHnl, TRUE);

        if (pChObj->algObj.u.mpeg4AlgIfObj.staticParams.
            videnc2Params.inputContentType == IVIDEO_PROGRESSIVE)
        {
            contentType = IVIDEO_PROGRESSIVE;
        }
        else
        {
            contentType = Utils_encdecMapFVID2FID2XDMContentType((FVID2_Fid)
                                                                 pReqObj->InFrameList.frames
                                                                 [0]->fid);
        }
        pReqObj->OutBuf->isKeyFrame =
            Utils_encdecIsGopStart(outArgs->videnc2OutArgs.encodedFrameType,
                                   contentType);
        pReqObj->OutBuf->frameWidth =
            inputBufDesc->imageRegion.bottomRight.x -
            inputBufDesc->imageRegion.topLeft.x;
        pReqObj->OutBuf->frameHeight =
            inputBufDesc->imageRegion.bottomRight.y -
            inputBufDesc->imageRegion.topLeft.y;

        if (error != XDM_EOK)
        {
            ENCLINK_INTERNAL_ERROR_LOG(error, "ALGPROCESS FAILED");
        }

        /* If SAD values are available then run Motion Detection Algorithm */
        if (outputBufDesc->numBufs == 2)
        {
            mvDataBuf =
                (UInt32) (outputBufDesc->descs[0].buf +
                          (outputBufDesc->descs[0].bufSize.bytes -
                           ENC_LINK_MV_DATA_SIZE));

            DM81XX_MD_Apply(mvDataBuf,                     // MV buf addr
                            pReqObj->OutBuf->frameWidth,   // frame width
                            pReqObj->OutBuf->frameHeight,  // frame height
                            pReqObj->OutBuf->isKeyFrame);  // isKeyFrame
        }
    //}

    return (error);
}

/* ===================================================================
 *  @func     enclink_mpeg4_set_static_params
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
static Int enclink_mpeg4_set_static_params(IMPEG4ENC_Params* staticParams,
                                          EncLink_AlgCreateParams *
                                          algCreateParams)
{

    /* Initialize default values for static params */
    *staticParams = MPEG4ENC_TI_PARAMS;

	staticParams->videnc2Params.size = sizeof(IMPEG4ENC_Params);

    /* Both width & height needs to be align with 16 bytes */
    staticParams->videnc2Params.maxHeight =
        VpsUtils_align(algCreateParams->maxHeight, 16);

    staticParams->videnc2Params.maxWidth =
        VpsUtils_align(algCreateParams->maxWidth, 16);

    staticParams->videnc2Params.maxInterFrameInterval =
        0;//algCreateParams->maxInterFrameInterval;

    staticParams->videnc2Params.inputContentType =
        algCreateParams->inputContentType;

    staticParams->videnc2Params.inputChromaFormat =
        algCreateParams->inputChromaFormat;

    staticParams->videnc2Params.profile = MPEG4_SIMPLE_PROFILE_IDC; //algCreateParams->profile;

    staticParams->videnc2Params.level = IMPEG4ENC_SP_LEVEL_5 ;//algCreateParams->level;

    staticParams->vopTimeIncrementResolution = 60 ;//algCreateParams->level;

    staticParams->videnc2Params.encodingPreset =
        algCreateParams->encodingPreset;
	if(algCreateParams->enableAnalyticinfo == 1) {
		staticParams->enableAnalyticinfo = IVIDEO_METADATAPLANE_MBINFO;
	} else {
		staticParams->enableAnalyticinfo = IVIDEO_METADATAPLANE_NONE;
	}

    //staticParams->enableWatermark = algCreateParams->enableWaterMarking;
    staticParams->videnc2Params.rateControlPreset =
        algCreateParams->rateControlPreset;
    staticParams->sliceCodingParams.sliceCodingPreset =
        IMPEG4_SLICECODING_USERDEFINED;
	staticParams->sliceCodingParams.sliceMode =
		IMPEG4_SLICEMODE_MBUNIT;
    staticParams->rateControlParams.rateControlParamsPreset =
        IMPEG4_RATECONTROLPARAMS_USERDEFINED;

    return 0;
}

/* ===================================================================
 *  @func     enclink_mpeg4_set_params_stream_preset
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
static Int enclink_mpeg4_set_params_stream_preset(Int32 StreamPreset,
                                                 IMPEG4ENC_Params* staticParams,
                                                 IMPEG4ENC_DynamicParams *
                                                 dynamicParams)
{
    if (StreamPreset == ALG_VID_ENC_PRESET_AUTO)
	{
		staticParams->videnc2Params.encodingPreset = XDM_USER_DEFINED;
	}
   	else if(StreamPreset == ALG_VID_ENC_PRESET_HIGHSPEED)
    {
        staticParams->videnc2Params.encodingPreset = XDM_HIGH_SPEED ;
    }
    else if (StreamPreset == ALG_VID_ENC_PRESET_HIGHQUALITY)
    {
        staticParams->videnc2Params.encodingPreset = XDM_HIGH_QUALITY;
    }


    return 0;
}

/* ===================================================================
 *  @func     enclink_mpeg4_set_algObject
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
static Int enclink_mpeg4_set_algObject(EncLink_Mpeg4Obj* algObj,
                                      EncLink_AlgCreateParams * algCreateParams,
                                      EncLink_AlgDynamicParams *
                                      algDynamicParams)
{
    IMPEG4ENC_InArgs *inArgs;

    IMPEG4ENC_OutArgs *outArgs;

    IVIDEO2_BufDesc *inputBufDesc;

    XDM2_BufDesc *outputBufDesc;

    IMPEG4ENC_Status *status;

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
            if (status->videnc2Status.bufInfo.minOutBufSize[i].bytes
                > ANALYTICINFO_OUTPUT_BUFF_SIZE)
            {
                Vps_printf
                    ("\nMemory could not get allocated for Analytic info buffer\n");
            }
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
    inputBufDesc->numPlanes = 2;                           /* status.videnc2Status.bufInfo.minNumInBufs;
                                                            */
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
        inputBufDesc->imagePitch[0] = algDynamicParams->inputPitch;
    }
    if (algCreateParams->tilerEnable)
    {
        inputBufDesc->imagePitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
    }
    else
    {
        inputBufDesc->imagePitch[1] = algDynamicParams->inputPitch;
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
#if 0
    /*------------------------------------------------------------------------*/
    /* Provide approprate buffer addresses for both the supported meta data: */
    /* A. USer defined SEI message */
    /* B. User Defined Scaling MAtrices */
    /*------------------------------------------------------------------------*/
    if (algObj->staticParams.videnc2Params.
        metadataType[inputBufDesc->numMetaPlanes] ==
        IMPEG4_SEI_USER_DATA_UNREGISTERED)
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
        IMPEG4_USER_DEFINED_SCALINGMATRIX)
    {
        inputBufDesc->metadataPlaneDesc[inputBufDesc->numMetaPlanes].buf = NULL;
        inputBufDesc->metadataPlaneDesc[inputBufDesc->numMetaPlanes].bufSize.
            bytes =
            /* -1; */
            896;
        inputBufDesc->numMetaPlanes++;
    }
#endif

    return 0;
}

/* ===================================================================
 *  @func     enclink_mpeg4_set_dynamic_params
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
static Int enclink_mpeg4_set_dynamic_params(IMPEG4ENC_DynamicParams *
                                           dynamicParams,
                                           EncLink_AlgDynamicParams *
                                           algDynamicParams)
{

    *dynamicParams = MPEG4ENC_TI_DYNAMICPARAMS;
    dynamicParams->videnc2DynamicParams.inputWidth =
        algDynamicParams->inputWidth;
    dynamicParams->videnc2DynamicParams.inputHeight =
        algDynamicParams->inputHeight;
    dynamicParams->videnc2DynamicParams.captureWidth =
        algDynamicParams->inputWidth;
    dynamicParams->videnc2DynamicParams.targetBitRate =
        algDynamicParams->targetBitRate;
    dynamicParams->videnc2DynamicParams.targetFrameRate =
        algDynamicParams->targetFrameRate;
    dynamicParams->videnc2DynamicParams.interFrameInterval =
        0;//algDynamicParams->interFrameInterval;
    dynamicParams->videnc2DynamicParams.intraFrameInterval =
        algDynamicParams->intraFrameInterval;
    dynamicParams->videnc2DynamicParams.mvAccuracy =
        IVIDENC2_MOTIONVECTOR_HALFPEL;//algDynamicParams->mvAccuracy;
    dynamicParams->videnc2DynamicParams.refFrameRate =
        algDynamicParams->refFrameRate;
    dynamicParams->videnc2DynamicParams.ignoreOutbufSizeFlag = XDAS_TRUE;

    dynamicParams->rateControlParams.rateControlParamsPreset =
		IMPEG4_RATECONTROLPARAMS_USERDEFINED;
    dynamicParams->rateControlParams.rcAlgo =
        algDynamicParams->rcAlg;
    dynamicParams->sliceCodingParams.sliceCodingPreset =
		IMPEG4_SLICECODING_USERDEFINED;

    return 0;
}

/* ===================================================================
 *  @func     enclink_mpeg4_freersrc
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
static Void enclink_mpeg4_freersrc(EncLink_Mpeg4Obj* hObj, Int rsrcMask)
{
    if (rsrcMask & ENCLINKMPEG4_ALGREATE_RSRC_IRES_ASSIGNED)
    {
        IRES_Status iresStatus;

        IRESMAN_TiledMemoryForceDisableTileAlloc_UnRegister((IALG_Handle)
                                                            hObj->algHandle);
        iresStatus =
            RMAN_freeResources((IALG_Handle) hObj->algHandle, &MPEG4ENC_TI_IRES,
                               hObj->scratchID);
        if (iresStatus != IRES_OK)
        {
            ENCLINK_INTERNAL_ERROR_LOG(iresStatus, "RMAN_freeResources");
        }
    }
    if (rsrcMask & ENCLINKMPEG4_ALGREATE_RSRC_ALGCREATED)
    {
        enc_link_mpeg4_delete(hObj->algHandle,hObj->scratchID);
        hObj->algHandle = NULL;
    }
}

/* ===================================================================
 *  @func     EncLinkMpeg4_algCreate
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
Int EncLinkMpeg4_algCreate(EncLink_Mpeg4Obj * hObj,
                          EncLink_AlgCreateParams * algCreateParams,
                          EncLink_AlgDynamicParams * algDynamicParams,
                          Int linkID, Int channelID, Int scratchGroupID)
{

    Int retVal = ENC_LINK_S_SUCCESS;
    Int rsrcMask = ENCLINKMPEG4_ALGREATE_RSRC_NONE;

#if 1
    Int algStatus;

    UTILS_assert(Utils_encdecIsMPEG4(algCreateParams->format) == TRUE);
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

    hObj->status.videnc2Status.size = sizeof(IMPEG4ENC_Status);
    hObj->inArgs.videnc2InArgs.size = sizeof(IMPEG4ENC_InArgs);
    hObj->outArgs.videnc2OutArgs.size = sizeof(IMPEG4ENC_OutArgs);
    hObj->staticParams.videnc2Params.size = sizeof(IMPEG4ENC_Params);
    hObj->dynamicParams.videnc2DynamicParams.size =
        sizeof(IMPEG4ENC_DynamicParams);

    enclink_mpeg4_set_static_params(&hObj->staticParams, algCreateParams);
    enclink_mpeg4_set_dynamic_params(&hObj->dynamicParams, algDynamicParams);
    enclink_mpeg4_set_params_stream_preset(algCreateParams->StreamPreset,
                                          &hObj->staticParams,
                                          &hObj->dynamicParams);

    UTILS_MEMLOG_USED_START();
    hObj->algHandle =
        enc_link_mpeg4_create(scratchGroupID,
                             (IMPEG4ENC_Fxns *) & MPEG4ENC_TI_IMPEG4ENC,
                             &hObj->staticParams);
    UTILS_assertError((NULL != hObj->algHandle),
                      retVal, ENC_LINK_E_ALGCREATEFAILED, linkID, channelID);

    if (!UTILS_ISERROR(retVal))
    {
        IRES_Status iresStatus;
#if 1
        rsrcMask |= ENCLINKMPEG4_ALGREATE_RSRC_ALGCREATED;
#endif
        if (FALSE == algCreateParams->tilerEnable)
        {
            IRESMAN_TiledMemoryForceDisableTileAlloc_Register((IALG_Handle)
                                                              hObj->algHandle);
        }
        iresStatus = RMAN_assignResources((IALG_Handle) hObj->algHandle,
                                          &MPEG4ENC_TI_IRES, scratchGroupID);
        UTILS_assertError((iresStatus == IRES_OK), retVal,
                          ENC_LINK_E_RMANRSRCASSIGNFAILED, linkID, channelID);
    }

    if (!UTILS_ISERROR(retVal))
    {
        Int ivaChID;
#if 1
        rsrcMask |= ENCLINKMPEG4_ALGREATE_RSRC_IRES_ASSIGNED;
#endif

        hObj->status.videnc2Status.data.buf = &(hObj->versionInfo[0]);
        hObj->status.videnc2Status.data.bufSize = sizeof(hObj->versionInfo);
        algStatus = enclink_mpeg4_control(hObj->algHandle, XDM_GETVERSION,
                                         &(hObj->dynamicParams),
                                         &(hObj->status));
        if (algStatus == XDM_EOK)
        {
            ENCLINK_VERBOSE_INFO_LOG(hObj->linkID, hObj->channelID,
                                     "MPEG4EncCreated:%s", hObj->versionInfo);

        }
        ivaChID = Utils_encdecMapLinkChID2IvaChID(linkID, channelID);
        ENCLINK_VERBOSE_INFO_LOG(hObj->linkID, hObj->channelID, "IVAChID:%d",
                                 ivaChID);
        hObj->dynamicParams.videnc2DynamicParams.lateAcquireArg = ivaChID;
        hObj->ivaChID = ivaChID;
        algStatus = enclink_mpeg4_control(hObj->algHandle,
                                         XDM_SETLATEACQUIREARG,
                                         &hObj->dynamicParams, &hObj->status);
        if (algStatus == XDM_EOK)
        {
            ENCLINK_VERBOSE_INFO_LOG(hObj->linkID, hObj->channelID,
                                     "LateAcquireArg set:%d", ivaChID);
        }
        Utils_encdecRegisterAlgHandle(ivaChID, hObj->algHandle);
        algStatus = enclink_mpeg4_control(hObj->algHandle,
                                         XDM_SETDEFAULT,
                                         &hObj->dynamicParams, &hObj->status);
        UTILS_assertError((algStatus == XDM_EOK), retVal,
                          ENC_LINK_E_ALGSETPARAMSFAILED, linkID, channelID);
    }
    if (!UTILS_ISERROR(retVal))
    {
        algStatus = enclink_mpeg4_control(hObj->algHandle,
                                         XDM_SETPARAMS,
                                         &hObj->dynamicParams, &hObj->status);
        UTILS_assertError((algStatus == XDM_EOK), retVal,
                          ENC_LINK_E_ALGSETPARAMSFAILED, linkID, channelID);
    }

    if (!UTILS_ISERROR(retVal))
    {
        enclink_mpeg4_control(hObj->algHandle,
                             XDM_GETSTATUS,
                             &hObj->dynamicParams, &hObj->status);
    }

    if (!UTILS_ISERROR(retVal))
    {
        algStatus =
            enclink_mpeg4_control(hObj->algHandle,
                                 XDM_GETBUFINFO,
                                 &hObj->dynamicParams, &hObj->status);
        UTILS_assertError((algStatus == XDM_EOK), retVal,
                          ENC_LINK_E_ALGGETBUFINFOFAILED, linkID, channelID);
    }
    if (UTILS_ISERROR(retVal))
    {
        enclink_mpeg4_freersrc(hObj, rsrcMask);
    }
    else
    {
    /* Initialize the Inarg, OutArg, InBuf & OutBuf objects */
        enclink_mpeg4_set_algObject(hObj, algCreateParams, algDynamicParams);
    }

    UTILS_MEMLOG_USED_END(hObj->memUsed);
    UTILS_MEMLOG_PRINT("ENCLINK_MPEG4",hObj->memUsed,
                       (sizeof(hObj->memUsed) / sizeof(hObj->memUsed[0])));
#endif

    return retVal;
}

/* ===================================================================
 *  @func     EncLinkMpeg4_algDelete
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
Void EncLinkMpeg4_algDelete(EncLink_Mpeg4Obj * hObj)
{
    UTILS_MEMLOG_FREE_START();
    if (hObj->algHandle)
    {
        enclink_mpeg4_freersrc(hObj, ENCLINKMPEG4_ALGREATE_RSRC_ALL);
    }

    if (hObj->algHandle)
    {    
        enc_link_mpeg4_delete(hObj->algHandle,hObj->scratchID);
    }
    UTILS_MEMLOG_FREE_END(hObj->memUsed, 0 /* dont care */ );
}

/* ===================================================================
 *  @func     EncLinkMpeg4_algSetConfig
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
Int32 EncLinkMpeg4_algSetConfig(EncLink_algObj * algObj)
{
    Int32 status = ENC_LINK_S_SUCCESS;


    UInt32 bitMask;

    Bool setConfigFlag = FALSE;

    UInt key;

    key = Hwi_disable();
    bitMask = algObj->setConfigBitMask;

    /* Set the modified encoder bitRate value */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_BITRATE) & 0x1)
    {

        algObj->u.mpeg4AlgIfObj.dynamicParams.videnc2DynamicParams.
            targetBitRate = algObj->algDynamicParams.targetBitRate;
        Vps_printf("\n ENCLINK: new targetbitrate to set:%d \n",
                   algObj->algDynamicParams.targetBitRate);
        algObj->setConfigBitMask &=
            (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
             (1 << ENC_LINK_SETCONFIG_BITMASK_BITRATE));
        setConfigFlag = TRUE;
    }
    /* Set the modified encoder Fps value */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_FPS) & 0x1)
    {
        //Pay attention that FPS >30 will not be accepted by MPEG4 codec lib
        if( algObj->algDynamicParams.targetFrameRate <= 30000)
        {
            algObj->u.mpeg4AlgIfObj.dynamicParams.videnc2DynamicParams.
                targetFrameRate = algObj->algDynamicParams.targetFrameRate;
        }

        Vps_printf("\n ENCLINK: new targetframerate to set:%d \n",
                   algObj->algDynamicParams.targetFrameRate);
        algObj->setConfigBitMask &=
            (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
             (1 << ENC_LINK_SETCONFIG_BITMASK_FPS));

        setConfigFlag = TRUE;
    }

    /* Set the modified encoder Intra Frame Interval(GOP) value */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_INTRAI) & 0x1)
    {
        algObj->u.mpeg4AlgIfObj.dynamicParams.videnc2DynamicParams.
            intraFrameInterval = algObj->algDynamicParams.intraFrameInterval;
        Vps_printf("\n ENCLINK: new intraFrameInterval to set:%d \n",
                   algObj->algDynamicParams.intraFrameInterval);
        algObj->setConfigBitMask &=
            (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
             (1 << ENC_LINK_SETCONFIG_BITMASK_INTRAI));
        setConfigFlag = TRUE;
    }

    /* toggle Force IDR */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_FORCEI) & 0x1)
    {

        algObj->algDynamicParams.forceFrame = TRUE;
        algObj->algDynamicParams.forceFrameStatus = FALSE;

        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ENC_LINK_SETCONFIG_BITMASK_FORCEI));
        setConfigFlag = TRUE;
    }
    /** to support Force IDR frame: Entry **/
    if ((algObj->algDynamicParams.forceFrame == TRUE) &&
        (algObj->algDynamicParams.forceFrameStatus == FALSE))
    {
        /** SET forceIDR **/
        algObj->u.mpeg4AlgIfObj.dynamicParams.videnc2DynamicParams.forceFrame =
            IVIDEO_I_FRAME; //IVIDEO_IDR_FRAME;
        algObj->algDynamicParams.forceFrameStatus = TRUE;
    }
    else if ((algObj->algDynamicParams.forceFrame == TRUE) &&
             (algObj->algDynamicParams.forceFrameStatus == TRUE))
    {
        /** UNSET forceIDR **/
        algObj->u.mpeg4AlgIfObj.dynamicParams.videnc2DynamicParams.forceFrame =
            IVIDEO_NA_FRAME;
        algObj->algDynamicParams.forceFrame = FALSE;

        setConfigFlag = TRUE;
    }
    /** to support Force IDR frame: Exit **/

    /* Set the modified encoder QP range values for Intra Frame */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_QPI) & 0x1)
    {
        algObj->u.mpeg4AlgIfObj.dynamicParams.
            rateControlParams.rateControlParamsPreset =
            IMPEG4_RATECONTROLPARAMS_USERDEFINED;

        /*will be fixed as qpI=-1,qpMaxI=36,qpMinI=10*/
        algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.qpI =
            algObj->algDynamicParams.qpInitP;

        Vps_printf("\n ENCLINK: new QP I Param to set:%d\n",
                   algObj->algDynamicParams.qpInitI);
        algObj->setConfigBitMask &=
            (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
             (1 << ENC_LINK_SETCONFIG_BITMASK_QPI));
        setConfigFlag = TRUE;
    }

    /* Set the modified encoder QP range values for Inter Frame */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_QPP) & 0x1)
    {
        algObj->u.mpeg4AlgIfObj.dynamicParams.
            rateControlParams.rateControlParamsPreset =
            IMPEG4_RATECONTROLPARAMS_USERDEFINED;

		if(algObj->algDynamicParams.qpMinP < 1)
			algObj->algDynamicParams.qpMinP = 1;
		if(algObj->algDynamicParams.qpMinP > 5)
			 algObj->algDynamicParams.qpMinP = 5;
		if(algObj->algDynamicParams.qpMaxP > 31)
			algObj->algDynamicParams.qpMaxP = 31;

        if (algObj->algDynamicParams.qpMinP <= algObj->algDynamicParams.qpInitP
            && algObj->algDynamicParams.qpInitP <=
            algObj->algDynamicParams.qpMaxP)
        {
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.qpMin =
                algObj->algDynamicParams.qpMinP ;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.qpMax =
                algObj->algDynamicParams.qpMaxP ;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.qpP =
                algObj->algDynamicParams.qpInitP ;
        }
        else
        {
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.qpMin =
                algObj->algDynamicParams.qpInitP;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.qpMax =
                algObj->algDynamicParams.qpInitP;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.qpP =
                algObj->algDynamicParams.qpInitP;
        }
        Vps_printf("\n ENCLINK: new QP P Param to set:%d %d %d\n",
                   algObj->algDynamicParams.qpMinP,
                   algObj->algDynamicParams.qpMaxP,
                   algObj->algDynamicParams.qpInitP);

        algObj->setConfigBitMask &=
            (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
             (1 << ENC_LINK_SETCONFIG_BITMASK_QPP));
        setConfigFlag = TRUE;
    }


    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_PACKETSIZE) & 0x1)
    {
        /* 0 will not be accpeted by encoder */
        if ( algObj->algDynamicParams.packetSize == 0)
        {
            Vps_printf("\n ENCLINK: new PacketSize Param to set:%d \n",
                       algObj->algDynamicParams.packetSize);
        }
        else
        {
            algObj->u.mpeg4AlgIfObj.dynamicParams.sliceCodingParams.sliceMode =
                IMPEG4_SLICEMODE_MBUNIT;
            int inputWidth =
                algObj->u.mpeg4AlgIfObj.dynamicParams.
                videnc2DynamicParams.inputWidth;
            inputWidth = ((inputWidth + 15) / 16) * 16;
            int inputHeight =
                algObj->u.mpeg4AlgIfObj.dynamicParams.
                videnc2DynamicParams.inputHeight;

            /* in case that height like 1080 will be aligned by 16 bytes to
             * 1088 */
            inputHeight = ((inputHeight + 15) / 16) * 16;
            int macroBlockSize = 16 * 16;

            int sliceSize =
                inputWidth * inputHeight * algObj->algDynamicParams.packetSize /
                (macroBlockSize * 100);
            algObj->u.mpeg4AlgIfObj.dynamicParams.
                sliceCodingParams.sliceUnitSize = sliceSize;

            Vps_printf("\n ENCLINK: new PacketSize Param to set:%d \n",
                       algObj->u.mpeg4AlgIfObj.dynamicParams.
                       sliceCodingParams.sliceUnitSize);
        }
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 <<
                                      ENC_LINK_SETCONFIG_BITMASK_PACKETSIZE));
        setConfigFlag = TRUE;
    }

	/* Set the value of encoding Preset */
    if ((bitMask >> ENC_LINK_SETCONFIG_BITMASK_ENCODEPRESET) & 0x1)
    {
        if (algObj->algDynamicParams.encodePreset == ALG_VID_ENC_PRESET_AUTO)
		{
			algObj->u.mpeg4AlgIfObj.staticParams.videnc2Params.encodingPreset = XDM_USER_DEFINED;
		}
		else if (algObj->algDynamicParams.encodePreset == ALG_VID_ENC_PRESET_HIGHSPEED)
        {
			algObj->u.mpeg4AlgIfObj.staticParams.videnc2Params.encodingPreset = XDM_HIGH_SPEED ;
        }
        else if (algObj->algDynamicParams.encodePreset == ALG_VID_ENC_PRESET_HIGHQUALITY)
        {
			algObj->u.mpeg4AlgIfObj.staticParams.videnc2Params.encodingPreset = XDM_HIGH_QUALITY;
        }
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
        algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.
            rateControlParamsPreset = IMPEG4_RATECONTROLPARAMS_USERDEFINED;

        /* 0=> VBR, 1=> CBR, 2=> CVBR */
        if (algObj->algDynamicParams.rateControl ==
            ALG_VID_ENC_RATE_CONTROL_CBR)
        {
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.rcAlgo =
                IMPEG4_RATECONTROLALGO_CBR;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.
                vbvBufferSize = algObj->algDynamicParams.targetBitRate * 0.1;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = algObj->algDynamicParams.targetBitRate * 0.1;
        }
        else if (algObj->algDynamicParams.rateControl ==
                 ALG_VID_ENC_RATE_CONTROL_VBR)
        {
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.rcAlgo =
                IMPEG4_RATECONTROLALGO_VBR;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.
                vbvBufferSize = algObj->algDynamicParams.targetBitRate * 0.6;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = algObj->algDynamicParams.targetBitRate * 0.4;
        }
        else
        {	/* TURN OFF RC */
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.rcAlgo =
                IMPEG4_RATECONTROLALGO_NONE;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.
                vbvBufferSize = 0;
            algObj->u.mpeg4AlgIfObj.dynamicParams.rateControlParams.
                initialBufferLevel = 0;
        }

        Vps_printf("\n ENCLINK: new rate control algorithm  to set:%d \n",
                   algObj->algDynamicParams.rateControl);
        algObj->setConfigBitMask &= (ENC_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ENC_LINK_SETCONFIG_BITMASK_RCALGO));
        setConfigFlag = TRUE;
    }



    Hwi_restore(key);

    if (setConfigFlag)
    {
        status = enclink_mpeg4_control(algObj->u.mpeg4AlgIfObj.algHandle,
                                      XDM_SETPARAMS,
                                      &algObj->u.mpeg4AlgIfObj.dynamicParams,
                                      &algObj->u.mpeg4AlgIfObj.status);
        if (UTILS_ISERROR(status))
        {
            UTILS_warn("\n ENCLINK: ERROR in Run time parameters changes, "
                       "Extended Error code:%d \n",
                       algObj->u.mpeg4AlgIfObj.status.
                       videnc2Status.extendedError);
        }
        else
        {
            // Vps_printf("\n ENCLINK: Run time parameters changed %d\n",
            // algObj->u.mpeg4AlgIfObj.status.videnc2Status.extendedError);
        }
    }
    return (status);
}

/* ===================================================================
 *  @func     EncLinkMpeg4_algGetConfig
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
Int32 EncLinkMpeg4_algGetConfig(EncLink_algObj * algObj,
                               EncLink_AlgDynamicParams * algDynamicParams)
{
    Int retVal = ENC_LINK_S_SUCCESS;

    IMPEG4ENC_DynamicParams dynamicParams;

    IMPEG4ENC_Status status;

    if (algObj->getConfigFlag == TRUE)
    {
        status.videnc2Status.size = sizeof(IMPEG4ENC_Status);
        dynamicParams.videnc2DynamicParams.size =
            sizeof(IMPEG4ENC_DynamicParams);

        retVal = enclink_mpeg4_control(algObj->u.mpeg4AlgIfObj.algHandle,
                                      XDM_GETSTATUS, &dynamicParams, &status);
        if (UTILS_ISERROR(retVal))
        {
            UTILS_warn("\n ENCLINK: ERROR in Run time parameters changes,"
                       "Extended Error code:%d \n",
                       status.videnc2Status.extendedError);
        }

        algObj->getConfigFlag = FALSE;
        algDynamicParams->inputWidth =
            status.videnc2Status.encDynamicParams.inputWidth;
        algDynamicParams->inputHeight =
            status.videnc2Status.encDynamicParams.inputHeight;
        algDynamicParams->targetBitRate =
            status.videnc2Status.encDynamicParams.targetBitRate;
        algDynamicParams->targetFrameRate =
            status.videnc2Status.encDynamicParams.targetFrameRate;
        algDynamicParams->intraFrameInterval =
            status.videnc2Status.encDynamicParams.intraFrameInterval;
        algDynamicParams->forceFrame =
            status.videnc2Status.encDynamicParams.forceFrame;
        algDynamicParams->refFrameRate =
            status.videnc2Status.encDynamicParams.refFrameRate;
    }

    return (retVal);
}

/* ===================================================================
 *  @func     EncLinkMpeg4_algDynamicParamUpdate
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
Int EncLinkMpeg4_algDynamicParamUpdate(EncLink_Mpeg4Obj * hObj,
                                      EncLink_AlgCreateParams * algCreateParams,
                                      EncLink_AlgDynamicParams *
                                      algDynamicParams)
{
    Int retVal = ENC_LINK_S_SUCCESS;

    enclink_mpeg4_set_dynamic_params(&hObj->dynamicParams, algDynamicParams);
    enclink_mpeg4_set_algObject(hObj, algCreateParams, algDynamicParams);

    return (retVal);
}

extern EncLink_Obj gEncLink_obj[ENC_LINK_OBJ_MAX];

#if 0
/* ===================================================================
 *  @func     DM8148_EncLink_codecSetPrivacyMask
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
int DM8148_EncLink_codecSetPrivacyMask(int streamId, int *pPrm)
{
    EncLink_ChObj *pChObj;

    Int32 status = ENC_LINK_S_SUCCESS;

    EncLink_Obj *pObj = &gEncLink_obj[0];

    /* chId */
    pChObj = &pObj->chObj[streamId];

    UInt key;

    key = Hwi_disable();

    /* Id 1~36, mostly 35 FD result could be supported */
    if (pPrm[0] >= 1 && pPrm[0] < 36)
    {
        if (pPrm[0] > pChObj->algObj.algDynamicParams.FDNumOfRegion)
        {
            pChObj->algObj.algDynamicParams.FDNumOfRegion = pPrm[0];
        }
        pChObj->algObj.algDynamicParams.FDStartX[pPrm[0] - 1] = pPrm[1];
        pChObj->algObj.algDynamicParams.FDStartY[pPrm[0] - 1] = pPrm[2];
        pChObj->algObj.algDynamicParams.FDWidth[pPrm[0] - 1] = pPrm[3];
        pChObj->algObj.algDynamicParams.FDHeight[pPrm[0] - 1] = pPrm[4];
    }
    else if (pPrm[0] == 0)
    {
        pChObj->algObj.algDynamicParams.FDNumOfRegion = 0;
    }

    pChObj->algObj.setConfigBitMask |= (1 << ENC_LINK_SETCONFIG_BITMASK_ROI);
    Hwi_restore(key);

    return (status);
}

/* ===================================================================
 *  @func     EncLinkMpeg4_checkFdRoi
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
Int32 EncLinkMpeg4_checkFdRoi(UInt32 startX, UInt32 startY, UInt32 width,
                             UInt32 height, UInt32 frameWidth,
                             UInt32 frameHeight)
{
    if ((startX + width) > frameWidth)
        return ENC_LINK_E_FAIL;

    if ((startY + height) > frameHeight)
        return ENC_LINK_E_FAIL;

    return ENC_LINK_S_SUCCESS;
}

/* ===================================================================
 *  @func     EncLinkMpeg4_algSetFdRoi
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
Int32 EncLinkMpeg4_algSetFdRoi(EncLink_algObj * pAlgObj,
                              EncLink_ReqList * reqList, Int32 chId)
{
    Int32 i, status = ENC_LINK_S_SUCCESS;

    EncLink_ReqObj *pReqObj;

    UInt32 frameAddrY, frameDataFormat;

    UInt32 framePitch, frameWidth, frameHeight;

    UInt32 blankingDataSize, fdResultOffset;

    FdLink_result *pFdResult;

    pReqObj = reqList->reqObj[0];

    frameAddrY = (UInt32) pReqObj->InFrameList.frames[0]->addr[0][0];
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
        fdResultOffset =
            ((framePitch * frameHeight * 3) >> 1) + blankingDataSize;
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
        status = EncLinkMpeg4_checkFdRoi(pFdResult->faceInfo[i].startX,
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
