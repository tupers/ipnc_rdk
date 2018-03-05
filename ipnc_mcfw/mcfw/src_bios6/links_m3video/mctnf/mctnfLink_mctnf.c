/** ==================================================================
 *  @file   mctnfLink_mctnf.c
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

#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <mcfw/src_bios6/utils/utils_mem.h>

#include <ti/psp/vps/fvid2.h>

#include <ti/psp/iss/alg/aewb/ti2a/awb/inc/TI_aaa_awb.h>
#include <ti/psp/iss/drivers/alg/2A/src/issdrv_algTIawbCalcData_default.c>

#include "mctnfLink_mctnf.h"
#include "mctnfLink_priv.h"

#include <xdc/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
#include <ti/sdo/fc/rman/rman.h>
#include <ti/xdais/dm/ividenc2.h>

#include <mcfw/src_bios6/links_m3video/codec_utils/utils_encdec.h>
#include <mcfw/src_bios6/links_m3video/codec_utils/iresman_hdvicp2_earlyacquire.h>
#include <mcfw/src_bios6/links_m3video/alg/motion_detection/motiondetect_priv.h>
#include <mcfw/interfaces/link_api/fdLink.h>
#include <mcfw/interfaces/link_api/systemLink_m3video.h>

//#define PRINT_MCTNF_PARAM

int new_awb_data_available;

/*
 *  ======== MCTNF_create ========
 *  Create an MCTNF instance object (using parameters specified by prms)
 */

MCTNF_Handle MCTNF_create
(
	const IMCTNF_Fxns   *fxns,
    const IMCTNF_Params *prms,
    Int scratchGroupID )
{
  return ((MCTNF_Handle) ALG_create(scratchGroupID, (IALG_Fxns *) fxns,
                                      NULL, (IALG_Params *) prms));
}

/*
 *  ======== MCTNF_delete ========
 *  Delete the MCTNF instance object specified by handle
 */

Void MCTNF_delete(Int scratchGroupID, IMCTNF_Handle handle)
{
  ALG_delete(scratchGroupID,(IALG_Handle) handle);
}

/*
 *  ======== MCTNF_init ========
 *  MCTNF module initialization
 */

Void MCTNF_init(Void)
{
}

/*
 *  ======== MCTNF_exit ========
 *  MCTNF module finalization
 */

Void MCTNF_exit(Void)
{
}

/*
 *  ======== MCTNF_control ========
 */

XDAS_Int32 MCTNF_control
(
	MCTNF_Handle         handle,
    MCTNF_Cmd            cmd,
    MCTNF_DynamicParams *params,
    MCTNF_Status        *status
)
{
  int error = 0;
  IALG_Fxns *fxns = (IALG_Fxns*)handle->fxns;
  fxns->algActivate((IALG_Handle)handle);

  error = handle->fxns->ividnf.control((IVIDNF1_Handle) handle, cmd,
                                        (IVIDNF1_DynamicParams *) params,
                                        (IVIDNF1_Status *) status);
  fxns->algDeactivate((IALG_Handle)handle);

  return error;
}

/*
 *  ======== MCTNF_encode ========
 */

XDAS_Int32 MCTNF_processFrame
(
	MCTNF_Handle      handle,
    IVIDEO2_BufDesc  *inBufs,
    IVIDEO2_BufDesc  *outBufs,
    MCTNF_InArgs     *inargs,
    MCTNF_OutArgs    *outargs
)
{
  int error;
  IALG_Fxns *fxns = (IALG_Fxns*)handle->fxns;
  fxns->algActivate((IALG_Handle)handle);
  error = handle->fxns->ividnf.process((IVIDNF1_Handle) handle, inBufs,
                                        outBufs,
                                        (IVIDNF1_InArgs *) inargs,
                                        (IVIDNF1_OutArgs *) outargs);
  fxns->algDeactivate((IALG_Handle)handle);

  return (error);
}


XDAS_Int32 MCTNFTest_DataSyncGetBufFxn(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc)
{
   return 0;
}


ti2a_output ti2a_output_params_for_mctnf;

Iss_CaptDccPrm gDccPrm_for_mctnf;

static MctnfRunTimeParams pMctnfParam;

iss_drv_config_t iss_drv_config_for_mctnf;
iss_drv_config_t *pIss_drv_config_for_mctnf = &iss_drv_config_for_mctnf;

//int ANYRESOURCE = 0;
static int frameCnt_for_mctnf = 0;
static int firstFrame_for_mctnf = 1;
// ##################################################################################################
// ######
// ##################################################################################################


Int32 MCTNF_Engine_Reset(MctnfLink_Obj * pObj)
{


    return MSP_ERROR_NONE;
}

void MCTNF_printCreateParameters(IMCTNF_Params * params)
{
   Vps_printf("params->vidnf1Params.size = %d \n", params->vidnf1Params.size);
   Vps_printf("params->vidnf1Params.filterPreset = %d \n", params->vidnf1Params.filterPreset);
   Vps_printf("params->vidnf1Params.maxHeight = %d \n", params->vidnf1Params.maxHeight);
   Vps_printf("params->vidnf1Params.maxWidth = %d \n", params->vidnf1Params.maxWidth);
   Vps_printf("params->vidnf1Params.dataEndianness = %d \n", params->vidnf1Params.dataEndianness);
   Vps_printf("params->vidnf1Params.inputChromaFormat = %d \n", params->vidnf1Params.inputChromaFormat);
   Vps_printf("params->vidnf1Params.inputContentType = %d \n", params->vidnf1Params.inputContentType);
   Vps_printf("params->vidnf1Params.inputDataMode = %d \n", params->vidnf1Params.inputDataMode);
   Vps_printf("params->vidnf1Params.outputDataMode = %d \n", params->vidnf1Params.outputDataMode);
   Vps_printf("params->vidnf1Params.numInputDataUnits = %d \n", params->vidnf1Params.numInputDataUnits);
   Vps_printf("params->vidnf1Params.numOutputDataUnits = %d \n", params->vidnf1Params.numOutputDataUnits);
   Vps_printf("params->motionSearchParams.motionSearchPreset = %d \n", params->motionSearchParams.motionSearchPreset);
   Vps_printf("params->motionSearchParams.searchRangeHor = %d \n", params->motionSearchParams.searchRangeHor);
   Vps_printf("params->motionSearchParams.searchRangeVer = %d \n", params->motionSearchParams.searchRangeVer);
//   Vps_printf("params->motionSearchParams.minBlockSize = %d \n", params->motionSearchParams.minBlockSize);

return;
}
void MCTNF_printDynParameters(IMCTNF_DynamicParams * dynParams)
{
   Vps_printf("dynParams->vidnf1DynamicParams.size = %d \n", dynParams->vidnf1DynamicParams.size);
   Vps_printf("dynParams->vidnf1DynamicParams.inputHeight = %d \n", dynParams->vidnf1DynamicParams.inputHeight);
   Vps_printf("dynParams->vidnf1DynamicParams.inputWidth = %d \n", dynParams->vidnf1DynamicParams.inputWidth);
   Vps_printf("dynParams->vidnf1DynamicParams.captureWidth = %d \n", dynParams->vidnf1DynamicParams.captureWidth);
   Vps_printf("dynParams->vidnf1DynamicParams.numPastRef = %d \n", dynParams->vidnf1DynamicParams.numPastRef);
   Vps_printf("dynParams->vidnf1DynamicParams.numFutureRef = %d \n", dynParams->vidnf1DynamicParams.numFutureRef);
   Vps_printf("dynParams->vidnf1DynamicParams.lateAcquireArg = %d \n", dynParams->vidnf1DynamicParams.lateAcquireArg);
   Vps_printf("dynParams->motionSearchParams.motionSearchPreset = %d \n", dynParams->motionSearchParams.motionSearchPreset);
   Vps_printf("dynParams->motionSearchParams.searchRangeHor = %d \n", dynParams->motionSearchParams.searchRangeHor);
   Vps_printf("dynParams->motionSearchParams.searchRangeVer = %d \n", dynParams->motionSearchParams.searchRangeVer);
   Vps_printf("dynParams->noiseFilterParams.meEffectivenessTh = %d \n", dynParams->noiseFilterParams.meEffectivenessTh);
   Vps_printf("dynParams->noiseFilterParams.blendingFactorQ4 = %d \n", dynParams->noiseFilterParams.blendingFactorQ4);
   Vps_printf("dynParams->noiseFilterParams.minBlendQ4 = %d \n", dynParams->noiseFilterParams.minBlendQ4);
   Vps_printf("dynParams->noiseFilterParams.maxBlendQ4 = %d \n", dynParams->noiseFilterParams.maxBlendQ4);
   Vps_printf("dynParams->noiseFilterParams.meLambdaType = %d \n", dynParams->noiseFilterParams.meEffectivenessTh);
   Vps_printf("dynParams->noiseFilterParams.meLambdaFactorQ2 = %d \n", dynParams->noiseFilterParams.meLambdaFactorQ2);
   Vps_printf("dynParams->noiseFilterParams.maxLambdaQ2 = %d \n", dynParams->noiseFilterParams.maxLambdaQ2);
   Vps_printf("dynParams->noiseFilterParams.sadForMinLambda = %d \n", dynParams->noiseFilterParams.sadForMinLambda);
   Vps_printf("dynParams->noiseFilterParams.fixWtForCurQ8 = %d \n", dynParams->noiseFilterParams.fixWtForCurQ8);
   Vps_printf("dynParams->noiseFilterParams.minWtForCurQ8 = %d \n", dynParams->noiseFilterParams.minWtForCurQ8);
   Vps_printf("dynParams->noiseFilterParams.biasZeroMotion = %d \n", dynParams->noiseFilterParams.biasZeroMotion);
   Vps_printf("dynParams->noiseFilterParams.staticMBThZeroMV = %d \n", dynParams->noiseFilterParams.staticMBThZeroMV);
   Vps_printf("dynParams->noiseFilterParams.staticMBThNonZeroMV = %d \n", dynParams->noiseFilterParams.staticMBThNonZeroMV);
   Vps_printf("dynParams->noiseFilterParams.blockinessRemFactor = %d \n", dynParams->noiseFilterParams.blockinessRemFactor);
   Vps_printf("dynParams->noiseFilterParams.sadForMaxStrength = %d \n", dynParams->noiseFilterParams.sadForMaxStrength);

   Vps_printf("dynParams->searchCenter.x = %d \n", dynParams->searchCenter.x);
   Vps_printf("dynParams->searchCenter.y = %d \n", dynParams->searchCenter.y);


 return;
}

Int32 MCTNF_Engine_Init(MctnfLink_Obj * pObj)
{
  IRES_Status iresStatus;
  int scratchGroupID = -1;
  XDAS_Int8     version[128];
  int chId = 0, retValue, i =0;
  MctnfLink_ChObj *MctnfChObj, *pChObj;
    UInt32  frameId, queueId = MCTNF_LINK_OUT_QUE_ID;
	MctnfCreateParams *chMctnfCreateParams;
			//MctnfLink_ChCreateParams *chCreateParams;
  FVID2_Frame *frame;
  Int32 status;
  System_LinkChInfo  *outChInfo,*inChInfo;
  UInt32 ivaChID;

  frameCnt_for_mctnf = 0;
  firstFrame_for_mctnf = 1;


  MctnfChObj = &pObj->linkChInfo[chId];
  pChObj = &pObj->linkChInfo[chId];
  outChInfo = &pObj->info.queInfo[queueId].chInfo[chId];

  chMctnfCreateParams = &pChObj->chCreateArgs;

  /*--------------------------------------------------------------------------*/
  /*  Algorithm create call. This function will allocate memory for the main  */
  /*  algorithm handle algorithm and initializes accordingly. The alg handle  */
  /*  is returnd.                                                             */
  /*--------------------------------------------------------------------------*/
  pObj->algHandle = MCTNF_create((MCTNF_Fxns *)&MCTNF_TI_IMCTNF, &pObj->linkChInfo[chId].params,scratchGroupID);

  if(pObj->algHandle == NULL)
  {
    Vps_printf(" MCTNF Algorithm Create failure!!!!!!!!!!!!!!!!!!!!!!!!");
    return XDM_EFAIL;
  }

  if (FALSE == MctnfChObj->chCreateArgs.tilerEn)
  {
      IRESMAN_TiledMemoryForceDisableTileAlloc_Register((IALG_Handle)
                                                        pObj->algHandle);
  }
  iresStatus = RMAN_assignResources((IALG_Handle) pObj->algHandle,
                                    &MCTNF_TI_IRES, scratchGroupID);
  if(iresStatus != IRES_OK)
  {
    Vps_printf(" MCTNF RMAN Create failure!!!!!!!!!!!!!!!!!!!!!!!!");
    return XDM_EFAIL;
  }

  /*--------------------------------------------------------------------------*/
  /* Get the version of codec and print it                                    */
  /*--------------------------------------------------------------------------*/
  pObj->linkChInfo[chId].status.vidnf1Status.data.buf = (XDAS_Int8*)&version[0];
  pObj->linkChInfo[chId].status.vidnf1Status.data.bufSize = 128;
  retValue = MCTNF_control
             (
				 pObj->algHandle,
				 XDM_GETVERSION,
                 (MCTNF_DynamicParams *)&pObj->linkChInfo[chId].dynParams,
                 (MCTNF_Status *)&pObj->linkChInfo[chId].status
             );
  if(retValue == XDM_EFAIL)
  {
    Vps_printf(" MCTNF Control() failure!!!!!!!!!!!!!!!!!!!!!!!!");
    return XDM_EFAIL;
  }
  Vps_printf("\nMCTNF Algorithm Instance Creation for the Module %s Done...\n", version);
  pObj->linkChInfo[chId].status.vidnf1Status.data.bufSize = 0;


  /*--------------------------------------------------------------------------*/
  /*  XDM_SETPARAMS is called to pass the Dynamic Parameters to the codec.    */
  /*  The Control function shall validate the parameters. If an error         */
  /*  is found then the Control function returns failure.                     */
  /*--------------------------------------------------------------------------*/
  retValue = MCTNF_control
             (
				 pObj->algHandle,
				 XDM_SETPARAMS,
                 (MCTNF_DynamicParams *)&pObj->linkChInfo[chId].dynParams,
                 (MCTNF_Status *)&pObj->linkChInfo[chId].status
             );

  //MCTNF_printCreateParameters(&pObj->linkChInfo[chId].params);
  //MCTNF_printDynParameters(&pObj->linkChInfo[chId].dynParams);
  if(retValue == XDM_EFAIL)
  {
     Vps_printf("\nMCTNF Algorithm Instance Control() call FAILED!!!!...\n");
     return XDM_EFAIL;
  }

  /*--------------------------------------------------------------------------*/
  /*  XDM_SETLATEACQUIREARG is called to set the channel ID to codec          */
  /*--------------------------------------------------------------------------*/
  ivaChID = 5;
  pObj->linkChInfo[chId].dynParams.vidnf1DynamicParams.lateAcquireArg = ivaChID ;
  retValue = MCTNF_control
             (
				 pObj->algHandle,
				 XDM_SETLATEACQUIREARG,
                 (MCTNF_DynamicParams *)&pObj->linkChInfo[chId].dynParams,
                 (MCTNF_Status *)&pObj->linkChInfo[chId].status
             );

  if(retValue == XDM_EFAIL)
  {
     Vps_printf("\nMCTNF Algorithm Instance Control() XDM_SETLATEACQUIREARG call FAILED!!!!...\n");
     return XDM_EFAIL;
  }

  retValue = MCTNF_control
             (
				 pObj->algHandle,
				 XDM_GETBUFINFO,
                 (MCTNF_DynamicParams *)&pObj->linkChInfo[chId].dynParams,
                 (MCTNF_Status *)&pObj->linkChInfo[chId].status
             );

  if(retValue == XDM_EFAIL)
  {
     Vps_printf("\nMCTNF Algorithm Instance Control() XDM_SETLATEACQUIREARG call FAILED!!!!...\n");
     return XDM_EFAIL;
  }


        /* Fill format with channel specific values to allocate frame */
        MctnfChObj->format.channelNum = chId;
        MctnfChObj->format.width = pObj->linkChInfo[chId].status.vidnf1Status.bufInfo.minOutBufSize[i].tileMem.width;
        MctnfChObj->format.height = pObj->linkChInfo[chId].status.vidnf1Status.bufInfo.minOutBufSize[i].tileMem.height;

        MctnfChObj->format.pitch[0] = chMctnfCreateParams->ulOutputStrideLuma = pObj->linkChInfo[chId].status.vidnf1Status.bufInfo.minOutBufSize[i].tileMem.width;
        MctnfChObj->format.pitch[1] = chMctnfCreateParams->ulOutputStrideChroma = pObj->linkChInfo[chId].status.vidnf1Status.bufInfo.minOutBufSize[i].tileMem.width;
        MctnfChObj->format.pitch[2] = 0;
        MctnfChObj->format.fieldMerged[0] = FALSE;
        MctnfChObj->format.fieldMerged[1] = FALSE;
        MctnfChObj->format.fieldMerged[2] = FALSE;
        MctnfChObj->format.dataFormat = FVID2_DF_YUV420SP_UV;
        // format.dataFormat = chMctnfCreateParams->eOutputFormat;
        MctnfChObj->format.scanFormat = FVID2_SF_PROGRESSIVE;
        // format.scanFormat = inChInfo->scanFormat;
        MctnfChObj->format.bpp = FVID2_BPP_BITS8;            /* ignored */

        /*
         * Alloc memory based on 'format'
         * Allocated frame info is put in frames[]
         */
        if (chMctnfCreateParams->tilerEn)
			Utils_tilerFrameAlloc(&MctnfChObj->format, MctnfChObj->frames,
							  MCTNF_LINK_MAX_OUT_FRAMES_PER_CH);
		else
        	Utils_memFrameAlloc_capture(&MctnfChObj->format, MctnfChObj->frames,
                                        MCTNF_LINK_MAX_OUT_FRAMES_PER_CH,0,0);

        /* Set remaining parameters for the allocated frames */
        for (frameId = 0; frameId < MCTNF_LINK_MAX_OUT_FRAMES_PER_CH; frameId++)
        {
            /* Point to the frame's array */
            frame = &MctnfChObj->frames[frameId];
            frame->perFrameCfg = NULL;
            frame->subFrameInfo = NULL;
            frame->appData = &MctnfChObj->frameInfo[frameId];

            MctnfChObj->frameInfo[frameId].vnfChannelNum = frame->channelNum;

            /* Finally, add this frame to the MctnfLink_ChObj.bufInQue.emptyQue
             */
            status = Utils_bufPutEmptyFrame(&MctnfChObj->bufInQue, frame);
            UTILS_assert(status == FVID2_SOK);
        }

        /* no previous frame for the first frame */
        MctnfChObj->pPrevOutFrame = NULL;

        /* Populate the remaining fileds of
         * MctnfLink_Obj.System_LinkInfo.System_LinkQueInfo.System_LinkChInfo
         * This information will be used by the next link to configure itself
         * properly. */
        UTILS_assert(queueId < pObj->info.numQue);

        outChInfo->bufType = SYSTEM_BUF_TYPE_VIDFRAME;
        outChInfo->dataFormat = MctnfChObj->format.dataFormat;
        outChInfo->scanFormat = MctnfChObj->format.scanFormat;

        outChInfo->startX = 0;	//pObj->linkChInfo[chId].status.vidnf1Status.startX;
		outChInfo->startY = 0;	//pObj->linkChInfo[chId].status.vidnf1Status.startY;

		Vps_printf(" [MCTNF] startX = %d \n", outChInfo->startX);
		Vps_printf(" [MCTNF] startY = %d \n", outChInfo->startY);

        outChInfo->width = chMctnfCreateParams->ulComputeWidth;
		
		/* ulComputeHeight should be multiple of 16,1088 is used instead of 1080 */
		inChInfo = &pObj->inQueInfo.chInfo[chId];
        outChInfo->height = inChInfo->height;	//chMctnfCreateParams->ulComputeHeight;
		
        outChInfo->pitch[0] = MctnfChObj->format.pitch[0];
        outChInfo->pitch[1] = MctnfChObj->format.pitch[1];
        outChInfo->pitch[2] = MctnfChObj->format.pitch[2];

        Vps_printf("[MCTNF]  outChInfo->width = %d\n", outChInfo->width);
        Vps_printf("[MCTNF]  outChInfo->height = %d\n", outChInfo->height);
        Vps_printf("[MCTNF]  outChInfo->pitch[0] = %d\n", outChInfo->pitch[0]);
        Vps_printf("[MCTNF]  outChInfo->pitch[1] = %d\n", outChInfo->pitch[1]);


  //Now Initialize the InBufDesc and OutBufDesc
  MCTNF_TI_initInputBufDesc( pObj,
     (MCTNF_Status *)&pObj->linkChInfo[chId].status,
    pObj->linkChInfo[chId].dynParams.vidnf1DynamicParams.captureWidth);

  MCTNF_TI_initOutputBufDesc( pObj,
     (MCTNF_Status *)&pObj->linkChInfo[chId].status);


  Utils_encdecRegisterAlgHandle(pObj->linkChInfo[chId].dynParams.vidnf1DynamicParams.lateAcquireArg, pObj->algHandle);

  pObj->isControlToBeCalled = 0;

  return XDM_EOK;
}



Int32 MCTNF_Engine_DeInit(MctnfLink_Obj * pObj)
{
        IRES_Status iresStatus;
  int scratchGroupID = -1;
        IRESMAN_TiledMemoryForceDisableTileAlloc_UnRegister((IALG_Handle) pObj->algHandle);
        iresStatus =
            RMAN_freeResources((IALG_Handle) pObj->algHandle,
                               &MCTNF_TI_IRES, scratchGroupID);
        if (iresStatus != IRES_OK)
        {
            return XDM_EFAIL;
        }
        MCTNF_delete(scratchGroupID,pObj->algHandle);
        pObj->algHandle = NULL;
    return XDM_EOK;
}

#if 0
int gMeLambdaTypeDcc;
int gGMVx;
int gGMVy;
Void MCTNF_AdaptDynamicParams(MCTNF_OutArgs *outArgs,
                              MCTNF_DynamicParams *dynParams)
{
  int thX = 4;
  int thY = 4;

  if ((abs(gGMVx)> thX) || (abs(gGMVy) > thY) )
    {
      dynParams->noiseFilterParams.meLambdaType = 0;
      Vps_printf("GMV TH reached: Using updated Lmda %d", dynParams->noiseFilterParams.meLambdaType);
      //dynParams->noiseFilterParams.meLambdaFactorQ2
      //dynParams->noiseFilterParams.maxLambdaQ2
    }
  else
    {
      dynParams->noiseFilterParams.meLambdaType = gMeLambdaTypeDcc;
      //Vps_printf(" Using DCC Lmda %d", dynParams->noiseFilterParams.meLambdaType);
    }


}
#endif

Int32 MCTNF_ProcessFrame(MctnfLink_Obj * pObj,
                           FVID2_Frame * currentImageBuffer,
                           FVID2_Frame * outputImageBufferTobeFilled,
						   FVID2_Frame **outputImageBufferTobeFreed)
{
	XDAS_Int32 i;
	int chId = 0;
	MctnfLink_ChObj *MctnfChObj;
	int bufSize[2];
	IMCTNF_Status *status;
	//static int frameCnt_for_mctnf = 0;
	XDAS_Int32 retValue = IVIDNF1_EOK;


	MctnfChObj = &pObj->linkChInfo[chId];

	//fxns = (IALG_Fxns *) pObj->algHandle->fxns;

	status = &MctnfChObj->status;



	bufSize[0] = MctnfChObj->dynParams.vidnf1DynamicParams.captureWidth* MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight;
	bufSize[1] = bufSize[0]/2;
    //populate the various structures

	//InArgs
	MctnfChObj->inArgs.vidnf1InArgs.inBufID   = (unsigned int) currentImageBuffer;
	MctnfChObj->inArgs.vidnf1InArgs.outBufID  = (unsigned int) outputImageBufferTobeFilled;

	//OutArgs
	memset(&MctnfChObj->outArgs, 0, sizeof(IMCTNF_OutArgs));
	MctnfChObj->outArgs.vidnf1OutArgs.size = sizeof(IMCTNF_OutArgs);

	//InBufDesc
	MctnfChObj->gInputBufDesc.numPlanes = 2;//status->vidnf1Status.bufInfo.minNumInBufs;
    /*------------------------------------------------------------------------*/
    /* Assign memory pointers adn sizes for the all the input buffers         */
    /*------------------------------------------------------------------------*/
    for(i = 0; i < MctnfChObj->gInputBufDesc.numPlanes; i++)
    {
       MctnfChObj->gInputBufDesc.planeDesc[i].bufSize.bytes  = (XDAS_Int32)bufSize[i];
	   if(MctnfChObj->chCreateArgs.tilerEn)
	      MctnfChObj->gInputBufDesc.planeDesc[i].buf = (XDAS_Int8 *) Utils_tilerAddr2CpuAddr((UInt32)currentImageBuffer->addr[1][i]);
	   else
	      MctnfChObj->gInputBufDesc.planeDesc[i].buf = currentImageBuffer->addr[1][i];

	   MctnfChObj->gOutputBufDesc.planeDesc[i].memType = MctnfChObj->chCreateArgs.tilerEn ? XDM_MEMTYPE_TILED16 : XDM_MEMTYPE_RAW;
	}

	//OutBufDesc
	MctnfChObj->gOutputBufDesc.numPlanes = 2;//status->vidnf1Status.bufInfo.minNumOutBufs;
	//Vps_printf("Outside Codec ::: MctnfChObj->gOutputBufDesc.numPlanes = %d \n", MctnfChObj->gOutputBufDesc.numPlanes);

	bufSize[0] = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width* status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.height;
	bufSize[1] = bufSize[0]/2;
    for(i = 0; i < MctnfChObj->gOutputBufDesc.numPlanes; i++)
    {
       MctnfChObj->gOutputBufDesc.planeDesc[i].bufSize.bytes  = (XDAS_Int32)bufSize[i];
	   if(MctnfChObj->chCreateArgs.tilerEn)
			MctnfChObj->gOutputBufDesc.planeDesc[i].buf = (XDAS_Int8 *) Utils_tilerAddr2CpuAddr((UInt32)outputImageBufferTobeFilled->addr[1][i]);
	   else
			MctnfChObj->gOutputBufDesc.planeDesc[i].buf = outputImageBufferTobeFilled->addr[1][i];

	   MctnfChObj->gOutputBufDesc.planeDesc[i].memType = MctnfChObj->chCreateArgs.tilerEn ? XDM_MEMTYPE_TILED16 : XDM_MEMTYPE_RAW;
	}


	{
	    if(firstFrame_for_mctnf)
        {
          pMctnfParam.mctnf_dcc_params_exist = 0;
          firstFrame_for_mctnf = 0;
       }

    if((frameCnt_for_mctnf ) %300 == 100)
    {


		pMctnfParam.pMctnfQueryParam = &MctnfChObj->dynParams;

		MctnfLink_drvUpdateDCC(&pMctnfParam);

#ifdef PRINT_MCTNF_PARAM

		Vps_printf("=============MCTNF Parameters DCC :%s =======================\n",pMctnfParam.mctnf_dcc_params_exist==1?"true":"false");
        //if(pMctnfParam.mctnf_dcc_params_exist)
        {
            //int i;
            Vps_printf("[MCTNF] blendingFactorQ4 = %d \n",MctnfChObj->dynParams.noiseFilterParams.blendingFactorQ4);
			Vps_printf("[MCTNF] minBlendQ4       = %d \n",MctnfChObj->dynParams.noiseFilterParams.minBlendQ4);
			Vps_printf("[MCTNF] maxBlendQ4       = %d \n",MctnfChObj->dynParams.noiseFilterParams.maxBlendQ4);
			Vps_printf("[MCTNF] meLambdaType     = %d \n",MctnfChObj->dynParams.noiseFilterParams.meLambdaType);
			Vps_printf("[MCTNF] meLambdaFactorQ2 = %d \n",MctnfChObj->dynParams.noiseFilterParams.meLambdaFactorQ2);
			Vps_printf("[MCTNF] maxLambdaQ2      = %d \n",MctnfChObj->dynParams.noiseFilterParams.maxLambdaQ2);
			Vps_printf("[MCTNF] sadForMinLambda  = %d \n",MctnfChObj->dynParams.noiseFilterParams.sadForMinLambda);
			Vps_printf("[MCTNF] fixWtForCurQ8      = %d \n",MctnfChObj->dynParams.noiseFilterParams.fixWtForCurQ8);
			Vps_printf("[MCTNF] biasZeroMotion   = %d \n",MctnfChObj->dynParams.noiseFilterParams.biasZeroMotion);
			Vps_printf("[MCTNF] staticMBThZeroMV = %d \n",MctnfChObj->dynParams.noiseFilterParams.staticMBThZeroMV);
			Vps_printf("[MCTNF] staticMBThNonZeroMV = %d \n",MctnfChObj->dynParams.noiseFilterParams.staticMBThNonZeroMV);
			Vps_printf("[MCTNF] blockinessRemFactor = %d \n",MctnfChObj->dynParams.noiseFilterParams.blockinessRemFactor);
			Vps_printf("[MCTNF] meEffectivenessTh= %d \n",MctnfChObj->dynParams.noiseFilterParams.meEffectivenessTh);
			Vps_printf("[MCTNF] minWtForCurQ8      = %d \n",MctnfChObj->dynParams.noiseFilterParams.minWtForCurQ8);
			Vps_printf("[MCTNF] sadForMaxStrength  = %d \n",MctnfChObj->dynParams.noiseFilterParams.sadForMaxStrength);
            Vps_printf("[MCTNF] aGain = %4d, dGain = %4d\n",(ti2a_output_params_for_mctnf.sensorGain),(ti2a_output_params_for_mctnf.ipipe_awb_gain.dGain * 1000 /256));

        }
#endif

		pObj->isControlToBeCalled = 1;
		//DO THE TRANSLATION OF DCC VNF PARAMS TO MCTNF DYNAMIC PARAMS (pObj->linkChInfo[chId].dynParams)
		//TO DO

    }
	//if (frameCnt_for_mctnf == 30)
	//   frameCnt_for_mctnf = 1;


	}




	if(1) //pObj->isControlToBeCalled)
	{

	  /*--------------------------------------------------------------------------*/
	  /*  XDM_SETPARAMS is called to pass the Dynamic Parameters to the codec.    */
	  /*  The Control function shall validate the parameters. If an error         */
	  /*  is found then the Control function returns failure.                     */
	  /*--------------------------------------------------------------------------*/


	  retValue = MCTNF_control
				 (
					 pObj->algHandle,
					 XDM_SETPARAMS,
					 (MCTNF_DynamicParams *)&pObj->linkChInfo[chId].dynParams,
					 (MCTNF_Status *)&pObj->linkChInfo[chId].status
				 );

	  //Following two fucntions print out the create and synamic parameters for debug
	  //MCTNF_printCreateParameters(&pObj->linkChInfo[chId].params);
	  //MCTNF_printDynParameters(&pObj->linkChInfo[chId].dynParams);
	  pObj->isControlToBeCalled = 0;
	  if(retValue == XDM_EFAIL)
	  {
		 Vps_printf("\nMCTNF Algorithm Instance Control() call FAILED!!!!...\n");
		 return XDM_EFAIL;
	  }
	}
    //Vps_printf("MCTNF Process call being CALLEDDDD !!!!!!!!!!!!.\n");
	//Process Call
	retValue = MCTNF_processFrame(
				   pObj->algHandle,
				   &MctnfChObj->gInputBufDesc,
				   &MctnfChObj->gOutputBufDesc,
				   &MctnfChObj->inArgs,
				   &MctnfChObj->outArgs
               );

	//Vps_printf("MCTNF Process call finished !!!!!!!!!!!!.\n");
     if(frameCnt_for_mctnf == 0)
     {
     Vps_printf("[MCTNF]  MctnfChObj->dynParams.vidnf1DynamicParams.inputWidth = %d \n", MctnfChObj->dynParams.vidnf1DynamicParams.inputWidth);
     Vps_printf("[MCTNF]  MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight = %d \n", MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight);
     Vps_printf("[MCTNF]  MctnfChObj->dynParams.vidnf1DynamicParams.captureWidth = %d \n", MctnfChObj->dynParams.vidnf1DynamicParams.captureWidth);
     Vps_printf("[MCTNF]  MctnfChObj->gOutputBufDesc.imagePitch[0] = %d \n", MctnfChObj->gOutputBufDesc.imagePitch[0]);
     Vps_printf("[MCTNF]  MctnfChObj->format.pitch[0] = %d \n", MctnfChObj->format.pitch[0]);
     //Vps_printf(" = %d \n", );
     //dynamicParams.vidnf1DynamicParams.inputWidth
     }

#ifdef PRINT_MCTNF_PARAM
	 if((frameCnt_for_mctnf ) %300 == 100)
      {
        Vps_printf("\n");

		Vps_printf("[MCTNF] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
        Vps_printf("[MCTNF]  avgZeroLumaSadSubBlk = %d \n",MctnfChObj->outArgs.nfStats.avgZeroLumaSadSubBlk);
        Vps_printf("[MCTNF]  numMBWithZeroMV = %d \n",MctnfChObj->outArgs.nfStats.numMBWithZeroMV);
        Vps_printf("[MCTNF]  numStaticMBs = %d \n",MctnfChObj->outArgs.nfStats.numStaticMBs);
        Vps_printf("[MCTNF]  avgYCbCrMECostSubBlock = %d \n",MctnfChObj->outArgs.nfStats.avgYCbCrMECostSubBlock);
        Vps_printf("[MCTNF]  GMVx = %d \n",MctnfChObj->outArgs.nfStats.GMVx);
        Vps_printf("[MCTNF]  GMVy = %d \n",MctnfChObj->outArgs.nfStats.GMVy);
        Vps_printf("[MCTNF]  maxBlendPic = %d \n",MctnfChObj->outArgs.nfStats.maxBlendPic);
        Vps_printf("[MCTNF]  minBlendPic = %d \n",MctnfChObj->outArgs.nfStats.minBlendPic);
        Vps_printf("[MCTNF]  filtStrength = %d \n",MctnfChObj->outArgs.nfStats.filtStrength);
        Vps_printf("[MCTNF]  mePassed = %d \n",MctnfChObj->outArgs.nfStats.mePassed);
        Vps_printf("[MCTNF]  picLambdaQ2 = %d \n",MctnfChObj->outArgs.nfStats.picLambdaQ2);
        Vps_printf("[MCTNF]  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
      }
#endif


     if(retValue != IVIDNF1_EOK)
    {
      Vps_printf("MCTNF Process call failed !!!!!!!!!!!!.\n");
	  Vps_printf("MCTNF Extended Error is 0x%x \n", MctnfChObj->outArgs.vidnf1OutArgs.extendedError);
	  return XDM_EFAIL;
    }


	frameCnt_for_mctnf++;

	*outputImageBufferTobeFreed = (FVID2_Frame *) MctnfChObj->outArgs.vidnf1OutArgs.freeOutBufID[0];

    return XDM_EOK;
}

void MCTNF_SetInitParams(MCTNF_Params *params,
                                           MCTNF_DynamicParams *dynamicParams, int width, int height, int pitch)
{
  params->vidnf1Params.size       = sizeof(IMCTNF_Params);
  /*--------------------------------------------------------------------------*/
  /* First set default values to base class XDM params structure              */
  /* Please refer User guide Table 4.5 for further details on values          */
  /*--------------------------------------------------------------------------*/
  params->vidnf1Params.filterPreset                	 = XDM_DEFAULT;
  params->vidnf1Params.maxHeight                     = height;
  params->vidnf1Params.maxWidth                      = width;
  params->vidnf1Params.dataEndianness                = XDM_BYTE;
  params->vidnf1Params.inputChromaFormat             = XDM_YUV_420SP ;
  params->vidnf1Params.inputContentType              = IVIDEO_PROGRESSIVE;
  params->vidnf1Params.inputDataMode                 = IVIDEO_ENTIREFRAME;
  params->vidnf1Params.outputDataMode                = IVIDEO_ENTIREFRAME;
  params->vidnf1Params.numInputDataUnits             = 1;
  params->vidnf1Params.numOutputDataUnits            = 2;

  /*--------------------------------------------------------------------------*/
  /* Set Inter coding params in extended XDM params structure                 */
  /* Please refer User guide Table 4.8 for further details on values          */
  /*--------------------------------------------------------------------------*/
  params->motionSearchParams.motionSearchPreset        = 0;
  params->motionSearchParams.searchRangeHor            = 144;
  params->motionSearchParams.searchRangeVer            = 32;

  /*--------------------------------------------------------------------------*/
  /* Set Extended XDM Encode parameters in extended XDM params structure      */
  /* Please refer User guide Table 4.15 for further details on values         */
  /*--------------------------------------------------------------------------*/

  params->pConstantMemory                           = 0;
  params->reservedParams[0]                         = 0;
  params->reservedParams[1]                         = 0;
  params->reservedParams[2]                         = 0;
  params->debugTraceLevel                           = 1;
  params->lastNFramesToLog                          = 0;

  params->enableAnalyticinfo = 0;
  /*--------------------------------------------------------------------------*/
  /* Set dynamic parameters                                                   */
  /* Most of the default parameters can be copied from Endoer params          */
  /*--------------------------------------------------------------------------*/
  memcpy(&((dynamicParams)->motionSearchParams),&(params->motionSearchParams),
                                           sizeof (IMCTNF_MotionSearchParams));
  /*--------------------------------------------------------------------------*/
  /* Set all the XDM dynamic values to default values                         */
  /*--------------------------------------------------------------------------*/
  dynamicParams->vidnf1DynamicParams.captureWidth = pitch;//width;
  dynamicParams->vidnf1DynamicParams.inputHeight = height;//1088;
  dynamicParams->vidnf1DynamicParams.inputWidth  = width;//1920;
  dynamicParams->vidnf1DynamicParams.numPastRef = 1;
  dynamicParams->vidnf1DynamicParams.numFutureRef = 0;
  dynamicParams->vidnf1DynamicParams.putDataFxn = NULL;
  dynamicParams->vidnf1DynamicParams.putDataHandle = 0;

  dynamicParams->noiseFilterParams.meEffectivenessTh = 2000;
  dynamicParams->noiseFilterParams.blendingFactorQ4 = 0;
  dynamicParams->noiseFilterParams.minBlendQ4 = 32; //24;//0;//
  dynamicParams->noiseFilterParams.maxBlendQ4 = 40; //32;//0;//
  dynamicParams->noiseFilterParams.meLambdaType = 1;
  dynamicParams->noiseFilterParams.meLambdaFactorQ2 = 36;
  dynamicParams->noiseFilterParams.maxLambdaQ2 = 192; //128;
  dynamicParams->noiseFilterParams.sadForMinLambda = 300;
  dynamicParams->noiseFilterParams.fixWtForCurQ8 = 0;
  dynamicParams->noiseFilterParams.biasZeroMotion = 1;
  dynamicParams->noiseFilterParams.staticMBThZeroMV = 8;
  dynamicParams->noiseFilterParams.staticMBThNonZeroMV = 10;
  dynamicParams->noiseFilterParams.blockinessRemFactor = 5; //2;//

  //NEW
  dynamicParams->noiseFilterParams.minWtForCurQ8 = 41;
  dynamicParams->noiseFilterParams.sadForMaxStrength = 300; //250;
  dynamicParams->noiseFilterParams.mvXThForStaticLamda =5;
  dynamicParams->noiseFilterParams.mvYThForStaticLamda = 5;

  dynamicParams->searchCenter.x                     = 32767;
  dynamicParams->searchCenter.y                     = 0;
  dynamicParams->reservedDynParams[0]               = 0;
  dynamicParams->reservedDynParams[1]               = 0;
  dynamicParams->reservedDynParams[2]               = 0;
  dynamicParams->reservedDynParams[3]               = 0;

  dynamicParams->vidnf1DynamicParams.size = sizeof(IMCTNF_DynamicParams);
  dynamicParams->vidnf1DynamicParams.getBufferFxn = MCTNFTest_DataSyncGetBufFxn;


}


void MCTNF_TI_initInputBufDesc
(
    MctnfLink_Obj * pObj,
    MCTNF_Status       *status,
    XDAS_Int32            uiCaptureWidth
)
{
	//XDAS_Int32 i;
	int chId = 0;
	MctnfLink_ChObj *MctnfChObj;
	MctnfChObj = &pObj->linkChInfo[chId];
    /*------------------------------------------------------------------------*/
    /*  Initialize the input buffer properties as required by algorithm       */
    /*  based on info received by preceding GETBUFINFO call.                  */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gInputBufDesc.numPlanes = 2;
    MctnfChObj->gInputBufDesc.numMetaPlanes = 0;

    /*------------------------------------------------------------------------*/
    /* Set entire Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gInputBufDesc.imageRegion.topLeft.x = 0;
    MctnfChObj->gInputBufDesc.imageRegion.topLeft.y = 0;
    MctnfChObj->gInputBufDesc.imageRegion.bottomRight.x = uiCaptureWidth;
    MctnfChObj->gInputBufDesc.imageRegion.bottomRight.y = MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight;

    /*------------------------------------------------------------------------*/
    /* Set proper Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gInputBufDesc.activeFrameRegion.topLeft.x = 0;
    MctnfChObj->gInputBufDesc.activeFrameRegion.topLeft.y = 0;
    MctnfChObj->gInputBufDesc.activeFrameRegion.bottomRight.x = 0 + MctnfChObj->dynParams.vidnf1DynamicParams.inputWidth;
    MctnfChObj->gInputBufDesc.activeFrameRegion.bottomRight.y = 0 + MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight;
    /*------------------------------------------------------------------------*/
    /* Image pitch is capture width                                           */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gInputBufDesc.imagePitch[0] = MctnfChObj->dynParams.vidnf1DynamicParams.captureWidth;
    MctnfChObj->gInputBufDesc.imagePitch[1] = MctnfChObj->dynParams.vidnf1DynamicParams.captureWidth;

    /*------------------------------------------------------------------------*/
    /* Set Content type and chroma format from encoder parameters             */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gInputBufDesc.contentType = MctnfChObj->params.vidnf1Params.inputContentType;
    MctnfChObj->gInputBufDesc.chromaFormat = MctnfChObj->params.vidnf1Params.inputChromaFormat;


  /*------------------------------------------------------------------------*/
  /* Set second field offset width and height according to input data       */
  /* When second field of the input data starts at 0 it represents 2 fields */
  /* are seperated and provided at 2 diff process calls (60 process call)   */
  /*------------------------------------------------------------------------*/
  {
    MctnfChObj->gInputBufDesc.secondFieldOffsetHeight[0] = MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight;
    MctnfChObj->gInputBufDesc.secondFieldOffsetHeight[1] = (MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight >> 1);
    MctnfChObj->gInputBufDesc.secondFieldOffsetHeight[2] = (MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight >> 1);
  }
  MctnfChObj->gInputBufDesc.secondFieldOffsetWidth[0]  = 0;
  MctnfChObj->gInputBufDesc.secondFieldOffsetWidth[1]  = 0;
  MctnfChObj->gInputBufDesc.secondFieldOffsetWidth[2]  = 0;

}

void MCTNF_TI_initOutputBufDesc
(
    MctnfLink_Obj * pObj,
    MCTNF_Status *status
)
{
	//XDAS_Int32 i;
	int chId = 0;
	MctnfLink_ChObj *MctnfChObj;
	MctnfChObj = &pObj->linkChInfo[chId];
    /*------------------------------------------------------------------------*/
    /*  Initialize the output buffer properties as required by algorithm      */
    /*  based on info received by preceding GETBUFINFO call.                  */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gOutputBufDesc.numPlanes = 2;
    MctnfChObj->gOutputBufDesc.numMetaPlanes = 0;

    /*------------------------------------------------------------------------*/
    /* Set entire Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gOutputBufDesc.imageRegion.topLeft.x = 0;
    MctnfChObj->gOutputBufDesc.imageRegion.topLeft.y = 0;
    MctnfChObj->gOutputBufDesc.imageRegion.bottomRight.x = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;
    MctnfChObj->gOutputBufDesc.imageRegion.bottomRight.y = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.height;

    /*------------------------------------------------------------------------*/
    /* Set proper Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gOutputBufDesc.activeFrameRegion.topLeft.x = 0;
    MctnfChObj->gOutputBufDesc.activeFrameRegion.topLeft.y = 0;
    MctnfChObj->gOutputBufDesc.activeFrameRegion.bottomRight.x = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;
    MctnfChObj->gOutputBufDesc.activeFrameRegion.bottomRight.y = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.height;

    /*------------------------------------------------------------------------*/
    /* Image pitch is capture width                                           */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gOutputBufDesc.imagePitch[0] = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;
    MctnfChObj->gOutputBufDesc.imagePitch[1] = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;

    /*------------------------------------------------------------------------*/
    /* Set Content type and chroma format from encoder parameters             */
    /*------------------------------------------------------------------------*/
    MctnfChObj->gOutputBufDesc.contentType = MctnfChObj->params.vidnf1Params.inputContentType;
    MctnfChObj->gOutputBufDesc.chromaFormat = MctnfChObj->params.vidnf1Params.inputChromaFormat;


    /*------------------------------------------------------------------------*/
    /* Set second field offset width and height according to input data       */
    /* When second field of the input data starts at 0 it represents 2 fields */
    /* are seperated and provided at 2 diff process calls (60 process call)   */
    /*------------------------------------------------------------------------*/

    {
      MctnfChObj->gOutputBufDesc.secondFieldOffsetHeight[0] =  MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight;
      MctnfChObj->gOutputBufDesc.secondFieldOffsetHeight[1] = (MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight >> 1);
      MctnfChObj->gOutputBufDesc.secondFieldOffsetHeight[2] = (MctnfChObj->dynParams.vidnf1DynamicParams.inputHeight >> 1);
    }
    MctnfChObj->gOutputBufDesc.secondFieldOffsetWidth[0]  = 0;
    MctnfChObj->gOutputBufDesc.secondFieldOffsetWidth[1]  = 0;
    MctnfChObj->gOutputBufDesc.secondFieldOffsetWidth[2]  = 0;
}

