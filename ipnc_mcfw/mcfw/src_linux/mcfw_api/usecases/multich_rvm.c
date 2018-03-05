/** ==================================================================
 *  @file   multich_rvm.c
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

/*------------------------------ RVM (REAR VIEW MODULE) ------------------------------------

                                      SENSOR
                                ________||_______
                                |                |
                                |   CAMERA LINK  |
                                |____(VPSSM3)____|
                                 RSZA        RSZB
                              (2560x1920)  (720x480)
                                (422I)      (420SP) ----- DIS_SD(VPSSM3)
                                  |
                                  |
                         IPC FRAMES OUT(VPSSM3)
                                  |
                                  |
                          IPC FRAMES IN(DSP)
                                  |
                                  |
                               RVM(DSP)
                                  |
                                  |
                          IPC FRAMES OUT(DSP)
                                  |
                                  |
                          IPC FRAMES IN(VPSSM3)
                                  |
                                  |
                            DSS_NF(VPSSM3)
                                  |(420SP)
                                  |       (1)                  (1920x1080)
                              DUP(VPSSM3) ----- MPSCLR(VPSSM3) ---------- DIS_HDMI(VPSSM3)
                                  |(0)                           (422I)
                                  |
                            IPC OUT(VPSSM3)
                                  |
                                  |
                           IPC IN(VIDEOM3)
                                  |
                                  |
                             ENC(VIDEOM3)
                                  |
                                  |
                         IPC BITS OUT(VIDEOM3)
                                  |
                                  |
                            IPC BITS IN(A8)
                                  |
                                  |
                             *************
                             * STREAMING *
                             *************
------------------------------------------------------------------------------------------*/

#include "mcfw/src_linux/mcfw_api/usecases/multich_common.h"
#include "mcfw/src_linux/mcfw_api/usecases/multich_ipcbits.h"
#include "demos/mcfw_api_demos/multich_usecase/ti_mcfw_ipnc_main.h"

//#define NO_ENC_DIS

#ifdef NO_ENC_DIS
    #define TEST_MPSCLR_SCLR
#endif

#define NUM_ENCODE_BUFFERS      (3)

static SystemVideo_Ivahd2ChMap_Tbl systemVid_encDecIvaChMapTbl = {
    .isPopulated = 1,
    .ivaMap[0] = {
                  .EncNumCh = 2,
                  .EncChList = {0, 1},
                  .DecNumCh = 2,
                  .DecChList = {0, 1},
                  },

};

UInt32 gIpcFramesOutVpssId;
UInt32 gIpcFramesInDspId;
UInt32 gRvmId;
UInt32 gIpcFramesOutDspId;
UInt32 gIpcFramesInVpssId;
UInt32 gIpcOutVpssId;
UInt32 gIpcInVideoId;
UInt32 gDupId;
#ifdef NO_ENC_DIS
    UInt32 gNullId0,gNullId1;
#endif

/* ===================================================================
 *  @func     MultiCh_createRvm
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
Void MultiCh_createRvm()
{
    UInt32 i;
    CameraLink_CreateParams cameraPrm;
#ifdef NO_ENC_DIS
    NullLink_CreateParams nullPrm0,nullPrm1;
#endif
#ifdef TEST_MPSCLR_SCLR
    SclrLink_CreateParams sclrPrm;
#endif
    IpcFramesOutLinkRTOS_CreateParams ipcFramesOutVpssPrm;
    IpcFramesInLinkRTOS_CreateParams ipcFramesInDspPrm;
    RvmLink_CreateParams rvmPrm;
    IpcFramesOutLinkRTOS_CreateParams ipcFramesOutDspPrm;
    IpcFramesInLinkRTOS_CreateParams ipcFramesInVpssPrm;
    NsfLink_CreateParams nsfPrm;
    DupLink_CreateParams dupPrm;
    MpSclrLink_CreateParams mpSclrPrm;
    DisplayLink_CreateParams displayPrm;
    DisplayLink_CreateParams displayPrm_SD;
    IpcLink_CreateParams ipcOutVpssPrm;
    IpcLink_CreateParams ipcInVideoPrm;
    EncLink_CreateParams encPrm;
    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;
    IpcBitsInLinkHLOS_CreateParams ipcBitsInA8Prm;

    CameraLink_VipInstParams *pCameraInstPrm;
    CameraLink_OutParams *pCameraOutPrm;

    /* IPC Struct Init */
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams,ipcFramesOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams,ipcFramesInDspPrm);
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams,ipcFramesOutDspPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams,ipcFramesInVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcInVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkRTOS_CreateParams,ipcBitsOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkHLOS_CreateParams,ipcBitsInA8Prm);

    OSA_printf("\n********** REAR VIEW MODULE USECASE ********\n");

    MultiCh_detectBoard();

    System_linkControl(SYSTEM_LINK_ID_M3VPSS,
                       SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES,
                       NULL,
                       0,
                       TRUE);

    System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
                       SYSTEM_COMMON_CMD_SET_CH2IVAHD_MAP_TBL,
                       &systemVid_encDecIvaChMapTbl,
                       sizeof(SystemVideo_Ivahd2ChMap_Tbl), TRUE);

    /* Link IDs */
    gVcamModuleContext.cameraId = SYSTEM_LINK_ID_CAMERA;
    gIpcFramesOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0;
    gIpcFramesInDspId = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;
    gRvmId = SYSTEM_LINK_ID_RVM_0;
    gIpcFramesOutDspId = SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_0;
    gIpcFramesInVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_0;
    gVcamModuleContext.nsfId = SYSTEM_LINK_ID_NSF_0;
    gIpcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    gIpcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    gVencModuleContext.encId = SYSTEM_LINK_ID_VENC_0;
    gVencModuleContext.ipcBitsOutRTOSId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gVencModuleContext.ipcBitsInHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;
    gDupId = SYSTEM_VPSS_LINK_ID_DUP_0;
    gVdisModuleContext.mpSclrId = SYSTEM_VPSS_LINK_ID_MP_SCLR_INST_0;
    gVdisModuleContext.displayId[VDIS_DEV_HDMI] = SYSTEM_LINK_ID_DISPLAY_0;
    gVdisModuleContext.displayId[VDIS_DEV_SD] = SYSTEM_LINK_ID_DISPLAY_2;
#ifdef NO_ENC_DIS
    gNullId0 = SYSTEM_VPSS_LINK_ID_NULL_0;
    gNullId1 = SYSTEM_VPSS_LINK_ID_NULL_1;
#endif
#ifdef TEST_MPSCLR_SCLR
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
#endif

    /* CAMERA LINK */
    CameraLink_CreateParams_Init(&cameraPrm);

    cameraPrm.captureMode              = CAMERA_LINK_CAPMODE_ISIF;
    cameraPrm.numAudioChannels         = 1;
    cameraPrm.numVipInst               = 1;
    cameraPrm.tilerEnable              = FALSE;
    cameraPrm.vsEnable                 = FALSE;
#ifdef TEST_MPSCLR_SCLR
    cameraPrm.outQueParams[0].nextLink = gVdisModuleContext.mpSclrId;
#else
    cameraPrm.outQueParams[0].nextLink = gIpcFramesOutVpssId;
#endif

#ifdef NO_ENC_DIS
    cameraPrm.outQueParams[1].nextLink = gNullId1;
#else
    cameraPrm.outQueParams[1].nextLink = gVdisModuleContext.displayId[VDIS_DEV_SD];
#endif
    cameraPrm.t2aConfig.n2A_vendor     = gUI_mcfw_config.n2A_vendor;
    cameraPrm.t2aConfig.n2A_mode       = gUI_mcfw_config.n2A_mode;

    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->vipInstId       = SYSTEM_CAMERA_INST_VP_CSI2;
    pCameraInstPrm->videoDecoderId  = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
    pCameraInstPrm->inDataFormat    = SYSTEM_DF_YUV422I_UYVY;
    pCameraInstPrm->sensorOutWidth  = 2560;
    pCameraInstPrm->sensorOutHeight = 1920;
    pCameraInstPrm->standard        = SYSTEM_STD_5MP_2560_1920;
    pCameraInstPrm->numOutput       = 2;

    pCameraOutPrm = &pCameraInstPrm->outParams[0];
#ifdef TEST_MPSCLR_SCLR
    pCameraOutPrm->dataFormat  = SYSTEM_DF_YUV420SP_UV;
#else
    pCameraOutPrm->dataFormat  = SYSTEM_DF_YUV422I_UYVY;
#endif
    pCameraOutPrm->scEnable    = FALSE;
    pCameraOutPrm->standard    = SYSTEM_STD_5MP_2560_1920;
    pCameraOutPrm->scOutWidth  = 2560;
    pCameraOutPrm->scOutHeight = 1920;
    pCameraOutPrm->outQueId    = 0;

    pCameraOutPrm = &pCameraInstPrm->outParams[1];
    pCameraOutPrm->dataFormat  = SYSTEM_DF_YUV420SP_UV;
    pCameraOutPrm->scEnable    = FALSE;
    pCameraOutPrm->standard    = SYSTEM_STD_NTSC;
    pCameraOutPrm->scOutWidth  = 720;
    pCameraOutPrm->scOutHeight = 480;
    pCameraOutPrm->outQueId    = 1;

#ifdef NO_ENC_DIS
    nullPrm1.numInQue = 1;
    nullPrm1.inQueParams[0].prevLinkId    = gVcamModuleContext.cameraId;
    nullPrm1.inQueParams[0].prevLinkQueId = 1;
#endif

#ifdef TEST_MPSCLR_SCLR
    SclrLink_CreateParams_Init(&sclrPrm);
    sclrPrm.inQueParams.prevLinkId             = gVcamModuleContext.nsfId;
    sclrPrm.inQueParams.prevLinkQueId          = 0;
    sclrPrm.outQueParams.nextLink              = gVdisModuleContext.displayId[VDIS_DEV_HDMI];
    sclrPrm.tilerEnable                        = FALSE;
    sclrPrm.enableLineSkipSc                   = FALSE;
    sclrPrm.inputFrameRate                     = 30;
    sclrPrm.outputFrameRate                    = 30;
    sclrPrm.scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm.scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm.outScaleFactor.absoluteResolution.outWidth   = 1600;
    sclrPrm.outScaleFactor.absoluteResolution.outHeight  = 1200;
    sclrPrm.outDataFormat = VF_YUV420SP_UV;
    sclrPrm.pathId = SCLR_LINK_SEC1_SC4;
#endif

    /* SD Display Link */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm_SD);
    displayPrm_SD.inQueParams[0].prevLinkId = gVcamModuleContext.cameraId;
    displayPrm_SD.inQueParams[0].prevLinkQueId = 1;
    displayPrm_SD.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_SD].resolution;
    displayPrm_SD.displayId  = DISPLAY_LINK_DISPLAY_SD;

    /* VPSS IPC FRAMES OUT */
    ipcFramesOutVpssPrm.baseCreateParams.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
    ipcFramesOutVpssPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesOutVpssPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesOutVpssPrm.baseCreateParams.inputFrameRate = 30;
    ipcFramesOutVpssPrm.baseCreateParams.outputFrameRate = 30;
    ipcFramesOutVpssPrm.baseCreateParams.numOutQue = 1;
    ipcFramesOutVpssPrm.baseCreateParams.outQueParams[0].nextLink = gIpcFramesInDspId;
    ipcFramesOutVpssPrm.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesOutVpssPrm.baseCreateParams.processLink = SYSTEM_LINK_ID_INVALID;
    ipcFramesOutVpssPrm.baseCreateParams.notifyProcessLink = FALSE;
    ipcFramesOutVpssPrm.baseCreateParams.noNotifyMode = FALSE;

    /* DSP IPC FRAMES IN */
    ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkId = gIpcFramesOutVpssId;
    ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInDspPrm.baseCreateParams.numOutQue = 1;
    ipcFramesInDspPrm.baseCreateParams.outQueParams[0].nextLink = gRvmId;
    ipcFramesInDspPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesInDspPrm.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesInDspPrm.baseCreateParams.noNotifyMode = FALSE;

    /* DSP RVM LINK */
    rvmPrm.inQueParams.prevLinkId = gIpcFramesInDspId;
    rvmPrm.inQueParams.prevLinkQueId = 0;
    rvmPrm.outQueParams.nextLink = gIpcFramesOutDspId;

    /* DSP IPC FRAMES OUT */
    ipcFramesOutDspPrm.baseCreateParams.inQueParams.prevLinkId = gRvmId;
    ipcFramesOutDspPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesOutDspPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesOutDspPrm.baseCreateParams.inputFrameRate = 30;
    ipcFramesOutDspPrm.baseCreateParams.outputFrameRate = 30;
    ipcFramesOutDspPrm.baseCreateParams.numOutQue = 1;
    ipcFramesOutDspPrm.baseCreateParams.outQueParams[0].nextLink = gIpcFramesInVpssId;
    ipcFramesOutDspPrm.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesOutDspPrm.baseCreateParams.processLink = SYSTEM_LINK_ID_INVALID;
    ipcFramesOutDspPrm.baseCreateParams.notifyProcessLink = FALSE;
    ipcFramesOutDspPrm.baseCreateParams.noNotifyMode = FALSE;

    /* VPSS IPC FRAMES IN */
    ipcFramesInVpssPrm.baseCreateParams.inQueParams.prevLinkId = gIpcFramesOutDspId;
    ipcFramesInVpssPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInVpssPrm.baseCreateParams.numOutQue = 1;
#ifdef NO_ENC_DIS
    ipcFramesInVpssPrm.baseCreateParams.outQueParams[0].nextLink = gNullId0;
#else
    ipcFramesInVpssPrm.baseCreateParams.outQueParams[0].nextLink = gVcamModuleContext.nsfId;
#endif
    ipcFramesInVpssPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesInVpssPrm.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesInVpssPrm.baseCreateParams.noNotifyMode = FALSE;

#ifdef NO_ENC_DIS
    nullPrm0.numInQue = 1;
    nullPrm0.inQueParams[0].prevLinkId    = gIpcFramesInVpssId;
    nullPrm0.inQueParams[0].prevLinkQueId = 0;
#endif

    /* VPSS NSF */
    NsfLink_CreateParams_Init(&nsfPrm);
    nsfPrm.bypassNsf = NSF_LINK_BYPASS_NONE;
    nsfPrm.tilerEnable = FALSE;
#ifdef TEST_MPSCLR_SCLR
    nsfPrm.inQueParams.prevLinkId = gVdisModuleContext.mpSclrId;
    nsfPrm.outQueParams[0].nextLink = gVcamModuleContext.sclrId[0];
#else
    nsfPrm.inQueParams.prevLinkId = gIpcFramesInVpssId;
    nsfPrm.outQueParams[0].nextLink = gDupId;
#endif
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.numOutQue = 1;
    nsfPrm.numBufsPerCh = 6;
    nsfPrm.inputFrameRate = 60;
    nsfPrm.outputFrameRate = 60;
    nsfPrm.nsfStrength = gUI_mcfw_config.vnfStrength;

    /* DUP link */
    dupPrm.inQueParams.prevLinkId    = gVcamModuleContext.nsfId;
    dupPrm.outQueParams[0].nextLink  = gVdisModuleContext.mpSclrId;
    dupPrm.outQueParams[1].nextLink  = gIpcOutVpssId;
    dupPrm.inQueParams.prevLinkQueId = 0;
    dupPrm.numOutQue                 = 2;
    dupPrm.notifyNextLink            = TRUE;

    /* MP scaler link */
    MpSclrLink_CreateParams_Init(&mpSclrPrm);
#ifdef TEST_MPSCLR_SCLR
    mpSclrPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
    mpSclrPrm.outQueParams.nextLink = gVcamModuleContext.nsfId;
#else
    mpSclrPrm.inQueParams.prevLinkId = gDupId;
    mpSclrPrm.outQueParams.nextLink = gVdisModuleContext.displayId[VDIS_DEV_HDMI];
#endif
    mpSclrPrm.pathId = MP_SCLR_LINK_SC5;
    mpSclrPrm.numCh = 1;
    mpSclrPrm.enableLineSkip = FALSE;

    /* HDMI display link */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
#ifdef TEST_MPSCLR_SCLR
    displayPrm.inQueParams[0].prevLinkId = gVcamModuleContext.sclrId[0];
    displayPrm.inQueParams[0].prevLinkQueId = 0;
#else
    displayPrm.inQueParams[0].prevLinkId = gVdisModuleContext.mpSclrId;
    displayPrm.inQueParams[0].prevLinkQueId = 0;
#endif
    displayPrm.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    displayPrm.displayId = DISPLAY_LINK_DISPLAY_SC2;

    /* VPSS IPC OUT */
    ipcOutVpssPrm.inQueParams.prevLinkId = gDupId;
    ipcOutVpssPrm.inQueParams.prevLinkQueId = 1;
    ipcOutVpssPrm.numOutQue = 1;
    ipcOutVpssPrm.outQueParams[0].nextLink = gIpcInVideoId;
    ipcOutVpssPrm.notifyNextLink = TRUE;
    ipcOutVpssPrm.notifyPrevLink = TRUE;
    ipcOutVpssPrm.noNotifyMode = FALSE;

    /* VIDEO IPC IN */
    ipcInVideoPrm.inQueParams.prevLinkId = gIpcOutVpssId;
    ipcInVideoPrm.inQueParams.prevLinkQueId = 0;
    ipcInVideoPrm.numOutQue = 1;
    ipcInVideoPrm.outQueParams[0].nextLink = gVencModuleContext.encId;
    ipcInVideoPrm.notifyNextLink = TRUE;
    ipcInVideoPrm.notifyPrevLink = TRUE;
    ipcInVideoPrm.noNotifyMode = FALSE;

    /* VIDEO ENCODER */
    MULTICH_INIT_STRUCT(EncLink_CreateParams,encPrm);
    {
        EncLink_ChCreateParams   *pLinkChPrm;
        EncLink_ChDynamicParams  *pLinkDynPrm;
        VENC_CHN_DYNAMIC_PARAM_S *pDynPrm;
        VENC_CHN_PARAMS_S        *pChPrm;

        for (i = 0; i < VENC_PRIMARY_CHANNELS; i++)
        {
            pLinkChPrm  = &encPrm.chCreateParams[i];
            pLinkDynPrm = &pLinkChPrm->defaultDynamicParams;

            pChPrm      = &gVencModuleContext.vencConfig.encChannelParams[i];
            pDynPrm     = &pChPrm->dynamicParam;

            pLinkChPrm->format                  = IVIDEO_H264HP;
            pLinkChPrm->profile                 = gVencModuleContext.vencConfig.h264Profile[i];
            pLinkChPrm->dataLayout              = IVIDEO_PROGRESSIVE;
            pLinkChPrm->fieldMergeEncodeEnable  = FALSE;
            pLinkChPrm->enableAnalyticinfo      = 0;
            pLinkChPrm->maxBitRate              = pChPrm->maxBitRate;
            pLinkChPrm->encodingPreset          = pChPrm->encodingPreset;
            pLinkChPrm->rateControlPreset       = IVIDEO_USER_DEFINED;
            pLinkChPrm->enableHighSpeed         = FALSE;
            pLinkChPrm->enableWaterMarking      = pChPrm->enableWaterMarking;
            pLinkChPrm->StreamPreset            = gUI_mcfw_config.StreamPreset[i];

            pLinkDynPrm->intraFrameInterval     = pDynPrm->intraFrameInterval;
            pLinkDynPrm->targetBitRate          = pDynPrm->targetBitRate;
            pLinkDynPrm->interFrameInterval     = 1;
            pLinkDynPrm->mvAccuracy             = IVIDENC2_MOTIONVECTOR_QUARTERPEL;
            pLinkDynPrm->inputFrameRate         = pDynPrm->inputFrameRate;
            pLinkDynPrm->rcAlg                  = pDynPrm->rcAlg;
            pLinkDynPrm->qpMin                  = pDynPrm->qpMin;
            pLinkDynPrm->qpMax                  = pDynPrm->qpMax;
            pLinkDynPrm->qpInit                 = pDynPrm->qpInit;
            pLinkDynPrm->vbrDuration            = pDynPrm->vbrDuration;
            pLinkDynPrm->vbrSensitivity         = pDynPrm->vbrSensitivity;

            encPrm.numBufPerCh[i]           = NUM_ENCODE_BUFFERS;
            gVencModuleContext.encFormat[i] = pLinkChPrm->format;
        }
    }

    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;
    encPrm.chCreateParams[1].defaultDynamicParams.inputFrameRate = 30;
    encPrm.vsEnable                                              = 0;
    encPrm.isVaUseCase                                           = 0;
    encPrm.inQueParams.prevLinkId                                = gIpcInVideoId;
    encPrm.inQueParams.prevLinkQueId                             = 0;
    encPrm.outQueParams.nextLink                                 = gVencModuleContext.ipcBitsOutRTOSId;

    /* VIDEO IPC BITS OUT */
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkId = gVencModuleContext.encId;
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcBitsOutVideoPrm.baseCreateParams.numOutQue = 1;
    ipcBitsOutVideoPrm.baseCreateParams.outQueParams[0].nextLink = gVencModuleContext.ipcBitsInHLOSId;
    MultiCh_ipcBitsInitCreateParams_BitsOutRTOS(&ipcBitsOutVideoPrm, TRUE);

    /* A8 BITS IN */
    ipcBitsInA8Prm.baseCreateParams.inQueParams.prevLinkId = gVencModuleContext.ipcBitsOutRTOSId;
    ipcBitsInA8Prm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    MultiCh_ipcBitsInitCreateParams_BitsInHLOS(&ipcBitsInA8Prm);

    /* Link Creation */
    System_linkCreate(gVcamModuleContext.cameraId,&cameraPrm,sizeof(CameraLink_CreateParams));
    System_linkControl(gVcamModuleContext.cameraId,CAMERA_LINK_CMD_DETECT_VIDEO,NULL,0,TRUE);

#ifdef NO_ENC_DIS
    System_linkCreate(gNullId1,&nullPrm1,sizeof(NullLink_CreateParams));
#else
    System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_SD],&displayPrm_SD,sizeof(DisplayLink_CreateParams));
#endif

#ifndef TEST_MPSCLR_SCLR
    System_linkCreate(gIpcFramesOutVpssId,&ipcFramesOutVpssPrm,sizeof(IpcFramesOutLinkRTOS_CreateParams));
    System_linkCreate(gIpcFramesInDspId,&ipcFramesInDspPrm,sizeof(IpcFramesInLinkRTOS_CreateParams));
    System_linkCreate(gRvmId,&rvmPrm,sizeof(RvmLink_CreateParams));
    System_linkCreate(gIpcFramesOutDspId,&ipcFramesOutDspPrm,sizeof(IpcFramesOutLinkRTOS_CreateParams));
    System_linkCreate(gIpcFramesInVpssId,&ipcFramesInVpssPrm,sizeof(IpcFramesInLinkRTOS_CreateParams));
#endif

#ifdef NO_ENC_DIS
    #ifdef TEST_MPSCLR_SCLR
        System_linkCreate(gVdisModuleContext.mpSclrId, &mpSclrPrm, sizeof(MpSclrLink_CreateParams));
        System_linkCreate(gVcamModuleContext.nsfId,&nsfPrm,sizeof(NsfLink_CreateParams));
        System_linkCreate(gVcamModuleContext.sclrId[0], &sclrPrm, sizeof(SclrLink_CreateParams));
        System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_HDMI],&displayPrm,sizeof(DisplayLink_CreateParams));
    #else
        System_linkCreate(gNullId0,&nullPrm0,sizeof(NullLink_CreateParams));
    #endif
#else
    System_linkCreate(gVcamModuleContext.nsfId,&nsfPrm,sizeof(NsfLink_CreateParams));
    System_linkCreate(gDupId,&dupPrm,sizeof(DupLink_CreateParams));
    System_linkCreate(gVdisModuleContext.mpSclrId, &mpSclrPrm, sizeof(MpSclrLink_CreateParams));
    System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_HDMI],&displayPrm,sizeof(DisplayLink_CreateParams));
    System_linkCreate(gIpcOutVpssId,&ipcOutVpssPrm,sizeof(IpcLink_CreateParams));
    System_linkCreate(gIpcInVideoId,&ipcInVideoPrm,sizeof(IpcLink_CreateParams));
    System_linkCreate(gVencModuleContext.encId,&encPrm,sizeof(EncLink_CreateParams));
    System_linkCreate(gVencModuleContext.ipcBitsOutRTOSId,&ipcBitsOutVideoPrm,sizeof(IpcBitsOutLinkRTOS_CreateParams));
    System_linkCreate(gVencModuleContext.ipcBitsInHLOSId,&ipcBitsInA8Prm,sizeof(IpcBitsInLinkHLOS_CreateParams));
#endif

    OSA_printf("USECASE SETUP DONE\n");
}

/* ===================================================================
 *  @func     MultiCh_deleteRvm
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
Void MultiCh_deleteRvm()
{
#ifdef NO_ENC_DIS
    #ifdef TEST_MPSCLR_SCLR
        System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_HDMI]);
        System_linkDelete(gVcamModuleContext.sclrId[0]);
        System_linkDelete(gVcamModuleContext.nsfId);
        System_linkDelete(gVdisModuleContext.mpSclrId);
    #else
        System_linkDelete(gNullId0);
    #endif
#else
    System_linkDelete(gVencModuleContext.ipcBitsInHLOSId);
    System_linkDelete(gVencModuleContext.ipcBitsOutRTOSId);
    System_linkDelete(gVencModuleContext.encId);
    System_linkDelete(gIpcInVideoId);
    System_linkDelete(gIpcOutVpssId);
    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_HDMI]);
    System_linkDelete(gVdisModuleContext.mpSclrId);
    System_linkDelete(gDupId);
    System_linkDelete(gVcamModuleContext.nsfId);
#endif

#ifndef TEST_MPSCLR_SCLR
    System_linkDelete(gIpcFramesInVpssId);
    System_linkDelete(gIpcFramesOutDspId);
    System_linkDelete(gRvmId);
    System_linkDelete(gIpcFramesInDspId);
    System_linkDelete(gIpcFramesOutVpssId);
#endif

#ifdef NO_ENC_DIS
    System_linkDelete(gNullId1);
#else
    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_SD]);
#endif

    System_linkDelete(gVcamModuleContext.cameraId);

    OSA_printf("USECASE TEARDOWN DONE\n");
}

