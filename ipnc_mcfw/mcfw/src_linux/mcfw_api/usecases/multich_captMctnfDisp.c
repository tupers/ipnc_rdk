/** ==================================================================
 *  @file   multich_Stream_CaptMctnfDisp.c
 *
 *  @path    ipnc_mcfw/mcfw/src_linux/mcfw_api/usecases/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/*-------------CAPTURE-MCTNF-DISPLAY demo: filtered & non-filtered side-by-side display-----

                                      SENSOR
                                ________||_______
                                |                |
                                |   CAMERA LINK  |
                                |____(VPSSM3)____|
                                 RSZA        RSZB
                             (1920x1080)  (1920x1080)
                               (420SP)      (420SP) ----- NULL(VPSSM3)
                                  |
                                  |     (0)
                            DUP(VPSSM3) ----------------|
                                  |(1)                  |
                                  |                     |
                         IPC FRAMES OUT(VPSSM3)         |
                                  |                     |
                                  |                     |
                         IPC FRAMES IN(VIDEOM3)         |
                                  |                     |
                                  |                     |
                            MCTNF(VIDEOM3)              |
                                  |                     |
                                  |                     |
                        IPC FRAMES OUT(VIDEOM3)         |
                                  |                     |
                                  |                     |
                         IPC FRAMES IN(VPSSM3)          |
                                  |                     |
                                  |                     |
                            ISS_NF(VPSSM3)              |
                                  |                     |
                                  |--------------- MERGE(VPSSM3)
                                                        |
                                                        |
                                                   SWMS(VPSSM3)
                                                        |
                                                        |
                                                  DIS_HDMI(VPSSM3)

------------------------------------------------------------------------------------------*/

#include "mcfw/src_linux/mcfw_api/usecases/multich_common.h"
#include "mcfw/src_linux/mcfw_api/usecases/multich_ipcbits.h"
#include "demos/mcfw_api_demos/multich_usecase/ti_mcfw_ipnc_main.h"

/* =============================================================================
 * Externs
 * ============================================================================= */

/* =============================================================================
 * Use case code
 * ============================================================================= */
#define NUM_DUP_LINK           (1)
#define CAPT_DUP_IDX           (0)

static SystemVideo_Ivahd2ChMap_Tbl systemVid_encDecIvaChMapTbl = {
    .isPopulated = 1,
    .ivaMap[0] = {
                  .EncNumCh = 2,
                  .EncChList = {0, 1},
                  .DecNumCh = 2,
                  .DecChList = {0, 1},
                  },

};

Void Mctnf_swMsGetDefaultLayoutPrm(int devId, SwMsLink_CreateParams *swMsCreateArgs, Bool forceLowCostScaling);
/* ===================================================================
 *  @func     MultiCh_createStreamCaptMctnfDisplay
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
#define VNF (1)
Void MultiCh_createStreamCaptMctnfDisplay()
{
    /* Link parameters */
    CameraLink_CreateParams  cameraPrm;
    CameraLink_VipInstParams *pCameraInstPrm;
    CameraLink_OutParams     *pCameraOutPrm;
    NullLink_CreateParams    nullPrm;
    DupLink_CreateParams     dupPrm[NUM_DUP_LINK];
    IpcLink_CreateParams     ipcOutVpssIdForMCTNF_params;
    IpcLink_CreateParams     ipcInVideoIdForMCTNF_params;
    MctnfLink_CreateParams   mctnfPrm;
    IpcLink_CreateParams     ipcOutVideoIdForMCTNF_params;
    IpcLink_CreateParams     ipcInVpssIdForMCTNF_params;
#if VNF
    VnfLink_CreateParams     vnfPrm;
#endif
    MergeLink_CreateParams   mergePrm;
    static SwMsLink_CreateParams       swMsPrm;
    DisplayLink_CreateParams displayPrm;

    /* Link Id defs */
    UInt32 nullId;
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 ipcOutVpssIdForMCTNF;
    UInt32 ipcInVideoIdForMCTNF;
    UInt32 ipcOutVideoIdForMCTNF;
    UInt32 ipcInVpssIdForMCTNF;
    UInt32 mctnfId;
    UInt32 mergeId;
    // capture, vnf, swms, display Id's are part of global structs

    /* Init Link parameters */
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssIdForMCTNF_params);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,  ipcInVideoIdForMCTNF_params);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVideoIdForMCTNF_params);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,  ipcInVpssIdForMCTNF_params);
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams ,displayPrm);

    OSA_printf
        ("\n********* capt-MCTNF-disp demo: filtered & non-filtered side-by-side display ********\n\n");

    MultiCh_detectBoard();

    System_linkControl(SYSTEM_LINK_ID_M3VPSS,
                       SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES, NULL, 0, TRUE);

    System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
                       SYSTEM_COMMON_CMD_SET_CH2IVAHD_MAP_TBL,
                       &systemVid_encDecIvaChMapTbl,
                       sizeof(SystemVideo_Ivahd2ChMap_Tbl), TRUE);

    /* Link Id setting */
    gVcamModuleContext.cameraId        = SYSTEM_LINK_ID_CAMERA;
    gVcamModuleContext.vnfId           = SYSTEM_LINK_ID_VNF;
    nullId                             = SYSTEM_VPSS_LINK_ID_NULL_0;
    dupId[CAPT_DUP_IDX]                = SYSTEM_VPSS_LINK_ID_DUP_0;
    ipcOutVpssIdForMCTNF               = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1;
    ipcInVideoIdForMCTNF               = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1;
    ipcOutVideoIdForMCTNF              = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
    ipcInVpssIdForMCTNF                = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
    mctnfId                            = SYSTEM_LINK_ID_MCTNF;
    mergeId                            = SYSTEM_VPSS_LINK_ID_MERGE_0;
    gVdisModuleContext.swMsId[0]       = SYSTEM_LINK_ID_SW_MS_MULTI_INST_0;
    gVdisModuleContext.displayId[0]    = SYSTEM_LINK_ID_DISPLAY_0; /* ON CHIP HDMI */

     /* Camera Link params */
    CameraLink_CreateParams_Init(&cameraPrm);
    cameraPrm.captureMode = CAMERA_LINK_CAPMODE_ISIF;
    cameraPrm.outQueParams[0].nextLink = dupId[CAPT_DUP_IDX];
    cameraPrm.outQueParams[1].nextLink = nullId;
    cameraPrm.vnfLinkId                = gVcamModuleContext.vnfId;
    cameraPrm.ispLinkId                = SYSTEM_LINK_ID_ISP_0;
    cameraPrm.mctnfLinkId              = mctnfId;
    cameraPrm.issVnfEnable             = 1; //LK

    /* This is for TVP5158 Audio Channels  */
    cameraPrm.numAudioChannels = 0;
    cameraPrm.numVipInst       = 1;
    cameraPrm.tilerEnable      = FALSE;

    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->vipInstId      = SYSTEM_CAMERA_INST_VP_CSI2;
    pCameraInstPrm->videoDecoderId = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
    pCameraInstPrm->inDataFormat   = SYSTEM_DF_YUV420SP_UV;
    pCameraInstPrm->sensorOutWidth = 1920;
    pCameraInstPrm->sensorOutHeight= 1080;
    pCameraInstPrm->standard       = SYSTEM_STD_1080P_30;
    pCameraInstPrm->numOutput      = 2;

    /* First stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[0];
    pCameraOutPrm->dataFormat  = SYSTEM_DF_YUV420SP_UV;
    pCameraOutPrm->scEnable    = FALSE;
    pCameraOutPrm->scOutWidth  = 1920;
    pCameraOutPrm->scOutHeight = 1080;
    pCameraOutPrm->outQueId    = 0;

   /* Second stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[1];
    pCameraOutPrm->dataFormat  = SYSTEM_DF_YUV420SP_UV;
    pCameraOutPrm->scEnable    = FALSE;
    pCameraOutPrm->scOutWidth  = 1920;
    pCameraOutPrm->scOutHeight = 1080;
    //pCameraOutPrm->standard    = SYSTEM_STD_NTSC;
    pCameraOutPrm->outQueId    = 1;

    /* 2A config */
    cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
    cameraPrm.t2aConfig.n2A_mode   = gUI_mcfw_config.n2A_mode;

    /* null Link Params */
    nullPrm.numInQue = 1;
    nullPrm.inQueParams[0].prevLinkId    = gVcamModuleContext.cameraId;
    nullPrm.inQueParams[0].prevLinkQueId = 1;

    /* dup frames: one goes via MCTNF+VNF and other directly to SwMs->Display */
    dupPrm[CAPT_DUP_IDX].inQueParams.prevLinkId    = gVcamModuleContext.cameraId;
    dupPrm[CAPT_DUP_IDX].inQueParams.prevLinkQueId = 0;
    dupPrm[CAPT_DUP_IDX].numOutQue = 2;
    dupPrm[CAPT_DUP_IDX].outQueParams[0].nextLink = ipcOutVpssIdForMCTNF;
    dupPrm[CAPT_DUP_IDX].outQueParams[1].nextLink = mergeId;
    dupPrm[CAPT_DUP_IDX].notifyNextLink           = TRUE;

     /* IPC Out VPSS link params */
    ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkId    = dupId[CAPT_DUP_IDX];
    ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    ipcOutVpssIdForMCTNF_params.numOutQue = 1;
    ipcOutVpssIdForMCTNF_params.outQueParams[0].nextLink  = ipcInVideoIdForMCTNF;
    ipcOutVpssIdForMCTNF_params.notifyNextLink = TRUE;
    ipcOutVpssIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcOutVpssIdForMCTNF_params.noNotifyMode   = FALSE;

    /* IPC In VIDEO params */
    ipcInVideoIdForMCTNF_params.inQueParams.prevLinkId    = ipcOutVpssIdForMCTNF;
    ipcInVideoIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    ipcInVideoIdForMCTNF_params.numOutQue = 1;
    ipcInVideoIdForMCTNF_params.outQueParams[0].nextLink = mctnfId;
    ipcInVideoIdForMCTNF_params.notifyNextLink = TRUE;
    ipcInVideoIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcInVideoIdForMCTNF_params.noNotifyMode   = FALSE;

        /* MCTNF link params */
        {
          MctnfLink_ChCreateParams *mctnfParams;

          mctnfPrm.inQueParams.prevLinkId    = ipcInVideoIdForMCTNF;
          mctnfPrm.inQueParams.prevLinkQueId = 0;
          mctnfPrm.outQueParams.nextLink     = ipcOutVideoIdForMCTNF;

          mctnfParams = &mctnfPrm.chCreateParams[0];
          mctnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;
        }

     /* IPC Out Video link params */
    ipcOutVideoIdForMCTNF_params.inQueParams.prevLinkId    = mctnfId;
    ipcOutVideoIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    ipcOutVideoIdForMCTNF_params.numOutQue = 1;
    ipcOutVideoIdForMCTNF_params.outQueParams[0].nextLink  = ipcInVpssIdForMCTNF;
    ipcOutVideoIdForMCTNF_params.notifyNextLink = TRUE;
    ipcOutVideoIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcOutVideoIdForMCTNF_params.noNotifyMode   = FALSE;

    /* IPC In Vpss params */
    ipcInVpssIdForMCTNF_params.inQueParams.prevLinkId      = ipcOutVideoIdForMCTNF;
    ipcInVpssIdForMCTNF_params.inQueParams.prevLinkQueId   = 0;
    ipcInVpssIdForMCTNF_params.numOutQue = 1;
    ipcInVpssIdForMCTNF_params.notifyNextLink = TRUE;
    ipcInVpssIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcInVpssIdForMCTNF_params.noNotifyMode   = FALSE;
#if VNF
    ipcInVpssIdForMCTNF_params.outQueParams[0].nextLink = gVcamModuleContext.vnfId;
    /* Vnf Link Params */
      {
        VnfLink_ChCreateParams *vnfParams;

        VnfLink_CreateParams_Init(&vnfPrm);

        vnfPrm.inQueParams.prevLinkId    = ipcInVpssIdForMCTNF;
        vnfPrm.inQueParams.prevLinkQueId = 0;
        vnfPrm.outQueParams.nextLink     = mergeId;

        vnfParams = &vnfPrm.chCreateParams[0];
        vnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;
        if (gUI_mcfw_config.ldcEnable)
        {
            vnfParams->sParams.eOperateMode = VNF_LINK_LDC_AFFINE_NSF_TNF;
        }
        else
        {
#ifdef USE_TNF3_FILTER
            vnfParams->sParams.eOperateMode =  VNF_LINK_TNF3;
#else
            vnfParams->sParams.eOperateMode = VNF_LINK_AFFINE_NSF_TNF;
#endif
            vnfParams->sParams.eNsfSet = VNF_LINK_NSF_LUMA_CHROMA;
        }

        //Reduce the additional resolution of vnf requirement if MCTNF is there in chain
        //as padding done by MCTNF can be used in the resolution loss
        //ispPrm.vnfFullResolution = FALSE;
        cameraPrm.vnfFullResolution = FALSE;

        vnfParams->dParams.ldcParams.eInterpolationLuma =
            VNF_LINK_YINTERPOLATION_BILINEAR;
        vnfParams->dParams.ldcParams.unPixelPad       = 4;

        vnfParams->dParams.nsfParams.bLumaEn          = TRUE;
        vnfParams->dParams.nsfParams.bChromaEn        = TRUE;
        vnfParams->dParams.nsfParams.eSmoothVal       = VNF_LINK_SMOOTH_0;
        vnfParams->dParams.nsfParams.bSmoothLumaEn    = TRUE;
        vnfParams->dParams.nsfParams.bSmoothChromaEn  = TRUE;

        vnfParams->dParams.tnfParams.useDefault       = TRUE;

        vnfParams->dParams.nsfFilter.eFilterParam     = VNF_LINK_PARAM_DEFAULT;
        vnfParams->dParams.nsfEdge.eEdgeParam         = VNF_LINK_PARAM_DISABLE;
        vnfParams->dParams.nsfShading.eShdParam       = VNF_LINK_PARAM_DISABLE;
        vnfParams->dParams.nsfDesaturation.eDesatParam = VNF_LINK_PARAM_DISABLE;

        vnfPrm.chCreateParams[0].bSnfEnable           = TRUE;
        vnfPrm.chCreateParams[0].bTnfEnable           = TRUE;
        vnfPrm.chCreateParams[0].strength             = NF_STRENGTH_AUTO;
    }
    mergePrm.inQueParams[0].prevLinkId    = gVcamModuleContext.vnfId;
#else
    ipcInVpssIdForMCTNF_params.outQueParams[0].nextLink = mergeId;
    mergePrm.inQueParams[0].prevLinkId    = ipcInVpssIdForMCTNF;

#endif
    /* Merge Link params */
    mergePrm.numInQue                     = 2;
    mergePrm.inQueParams[0].prevLinkQueId = 0;
    mergePrm.inQueParams[1].prevLinkId    = dupId[CAPT_DUP_IDX];
    mergePrm.inQueParams[1].prevLinkQueId = 1;
    mergePrm.outQueParams.nextLink        = gVdisModuleContext.swMsId[0];
    mergePrm.notifyNextLink               = TRUE;

    /* Software mosaic link params */
    swMsPrm.numSwMsInst               = 1;
    swMsPrm.swMsInstId[0]             = SYSTEM_SW_MS_SC_INST_SC5;
    swMsPrm.inQueParams.prevLinkId    = mergeId;
    swMsPrm.inQueParams.prevLinkQueId = 0;
    swMsPrm.outQueParams.nextLink     = gVdisModuleContext.displayId[0];
    swMsPrm.maxInputQueLen            = SYSTEM_SW_MS_DEFAULT_INPUT_QUE_LEN + 4;
    swMsPrm.maxOutRes                 = VSYS_STD_1080P_60;
    swMsPrm.lineSkipMode              = FALSE;
    swMsPrm.enableLayoutGridDraw      = gVdisModuleContext.vdisConfig.enableLayoutGridDraw;

    swMsPrm.enableOuputDup            = TRUE;
    swMsPrm.enableProcessTieWithDisplay = TRUE;

    Mctnf_swMsGetDefaultLayoutPrm(0, &swMsPrm, FALSE);

    /* display link params */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
    displayPrm.inQueParams[0].prevLinkId = gVdisModuleContext.swMsId[0] ;
    displayPrm.inQueParams[0].prevLinkQueId = 0;
    displayPrm.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    displayPrm.displayId  = DISPLAY_LINK_DISPLAY_SC2;

    /* Links Creation : Source to Sink*/
    System_linkCreate(gVcamModuleContext.cameraId, &cameraPrm,
                      sizeof(cameraPrm));
    System_linkControl(gVcamModuleContext.cameraId,
                       CAMERA_LINK_CMD_DETECT_VIDEO, NULL, 0, TRUE);
    System_linkCreate(dupId[CAPT_DUP_IDX], &dupPrm[CAPT_DUP_IDX], sizeof(dupPrm[CAPT_DUP_IDX]));
    System_linkCreate(nullId, &nullPrm, sizeof(nullPrm));

    printf("ipcOutVpssIdForMCTNF is 0x%x\n",ipcOutVpssIdForMCTNF);
    printf("ipcInVideoIdForMCTNF is 0x%x\n",ipcInVideoIdForMCTNF);
    printf("ipcOutVideoIdForMCTNF is 0x%x\n",ipcOutVideoIdForMCTNF);
    printf("ipcInVpssIdForMCTNF is 0x%x\n",ipcInVpssIdForMCTNF);
    printf("mctnfId is 0x%x\n",mctnfId);
    printf("Creating ipcOutVpssIdForMCTNF\n");
    System_linkCreate(ipcOutVpssIdForMCTNF, &ipcOutVpssIdForMCTNF_params, sizeof(ipcOutVpssIdForMCTNF_params));
    printf("Creating ipcInVideoIdForMCTNF\n");
    System_linkCreate(ipcInVideoIdForMCTNF, &ipcInVideoIdForMCTNF_params, sizeof(ipcInVideoIdForMCTNF_params));
    printf("Creating mctnfId with id 0x%x\n", mctnfId);
    System_linkCreate(mctnfId, &mctnfPrm, sizeof(mctnfPrm));
    printf("Creating ipcOutVideoIdForMCTNF\n");
    System_linkCreate(ipcOutVideoIdForMCTNF, &ipcOutVideoIdForMCTNF_params, sizeof(ipcOutVideoIdForMCTNF_params));
    printf("Creating ipcInVpssIdForMCTNF\n");
    System_linkCreate(ipcInVpssIdForMCTNF, &ipcInVpssIdForMCTNF_params, sizeof(ipcInVpssIdForMCTNF_params));

#if VNF
    System_linkCreate(gVcamModuleContext.vnfId, &vnfPrm, sizeof(vnfPrm));
#endif
    System_linkCreate(mergeId, &mergePrm, sizeof(mergePrm));
    System_linkCreate(gVdisModuleContext.swMsId[0], &swMsPrm, sizeof(swMsPrm));
    System_linkCreate(gVdisModuleContext.displayId[0], &displayPrm,
                      sizeof(displayPrm));

    MultiCh_memPrintHeapStatus();
    OSA_printf("USECASE SETUP DONE\n");
}

/* ===================================================================
 *  @func     MultiCh_deleteStreamCaptMctnfDisplay
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
Void MultiCh_deleteStreamCaptMctnfDisplay()
{
    UInt32 nullId;
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 ipcOutVpssIdForMCTNF;
    UInt32 ipcInVideoIdForMCTNF;
    UInt32 ipcOutVideoIdForMCTNF;
    UInt32 ipcInVpssIdForMCTNF;
    UInt32 mctnfId;
    UInt32 mergeId;

    /* Link Id setting */
    gVcamModuleContext.cameraId        = SYSTEM_LINK_ID_CAMERA;
    gVcamModuleContext.vnfId           = SYSTEM_LINK_ID_VNF;
    nullId                             = SYSTEM_VPSS_LINK_ID_NULL_0;
    dupId[CAPT_DUP_IDX]                = SYSTEM_VPSS_LINK_ID_DUP_0;
    ipcOutVpssIdForMCTNF               = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1;
    ipcInVideoIdForMCTNF               = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1;
    ipcOutVideoIdForMCTNF              = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
    ipcInVpssIdForMCTNF                = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
    mctnfId                            = SYSTEM_LINK_ID_MCTNF;
    mergeId                            = SYSTEM_VPSS_LINK_ID_MERGE_0;
    gVdisModuleContext.swMsId[0]       = SYSTEM_LINK_ID_SW_MS_MULTI_INST_0;
    gVdisModuleContext.displayId[0]    = SYSTEM_LINK_ID_DISPLAY_0;

    /* Links Deletion in reverse order: Sink to Source */

    System_linkDelete(gVdisModuleContext.displayId[0]);
    System_linkDelete(gVdisModuleContext.swMsId[0]);
    System_linkDelete(mergeId);

    System_linkDelete(nullId);

#if VNF
    System_linkDelete(gVcamModuleContext.vnfId);
#endif
    System_linkDelete(ipcInVpssIdForMCTNF);
    System_linkDelete(ipcOutVideoIdForMCTNF);
    System_linkDelete(mctnfId);
    System_linkDelete(ipcInVideoIdForMCTNF);
    System_linkDelete(ipcOutVpssIdForMCTNF);

    System_linkDelete(dupId[CAPT_DUP_IDX]);

    System_linkDelete(gVcamModuleContext.cameraId);

    /* Print the HWI, SWI and all tasks load */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

}

/* SwMs layout to take two 1080P streams as input and crop 'centre 960x1080' portion from each input
     and display side by side on 1080P frame */
Void Mctnf_swMsGetDefaultLayoutPrm(int devId, SwMsLink_CreateParams *swMsCreateArgs, Bool forceLowCostScaling)
{
    SwMsLink_LayoutPrm *layoutInfo;
    SwMsLink_LayoutWinInfo *winInfo;
    VDIS_MOSAIC_S *mosaicParam;

    if(devId>=VDIS_DEV_MAX)
        devId = VDIS_DEV_HDMI;

    layoutInfo = &swMsCreateArgs->layoutPrm;

    /* init to known default */
    memset(layoutInfo, 0, sizeof(*layoutInfo));

    mosaicParam = &(gVdisModuleContext.vdisConfig.mosaicParams[devId]);

    layoutInfo->onlyCh2WinMapChanged = FALSE;
    layoutInfo->outputFPS            = 30;
    layoutInfo->numWin               = 2;

    winInfo = &layoutInfo->winInfo[0];
    winInfo->width       = 960;
    winInfo->height      = 1080;
    winInfo->startX      = 0;
    winInfo->startY      = 0;
    winInfo->channelNum  = 1;
    winInfo->bypass      = FALSE;

    winInfo = &layoutInfo->winInfo[1];
    winInfo->width       = 960;
    winInfo->height      = 1080;
    winInfo->startX      = 960;
    winInfo->startY      = 0;
    winInfo->channelNum  = 0;
    winInfo->bypass      = FALSE;
  }


