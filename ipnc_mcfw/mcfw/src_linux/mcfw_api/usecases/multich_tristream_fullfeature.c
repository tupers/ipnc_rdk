/** ==================================================================
 *  @file   multich_tristream_fullFeature.c
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

/*------------------------------ TRIPLE OUT <1080p + D1 + 1080p> ---------------------------

                                      SENSOR
                                ________||_______
            +---SALDRE/WDR ON---|                |
            |                   |   CAMERA LINK  |
            |                   |____(VPSS M3)___|
   GLBCE_LINK/WDR(VPSSM3)          |           |
            |                      |           |
            |   ____________      SALDRE/WDR OFF
            |   |          |       |           |
            +---| ISP LINK |       |           |
                |_(VPSSM3)_|       |           |
                  |      |      |--|           |--|
                  |      |      |                 |
                  |      |------|---------------| |
                  |             |               | |
                  |----------- RSZA             RSZB
                            (1920x1080)      (720x480)
                            (420SP/422I)      (420SP)
                                |                |
                                |                |          (2)
   |--- MCTNF ON ------- DUP_VNF(VPSSM3)   DUP_SCLR(VPSSM3) ----- DISPLAY_SD(VPSSM3)
   |               |     (1)        |(0)    (0)|    |(1)
   |               | MCTNF OFF      |          |    |
   |               |                |(422I)    |    |
 IPC FRAMES  ISS_NF(VPSSME)   DSS_NF(VPSSM3)   |  SCLR(VPSSM3) --- VA ON --|
 OUT(VPSSM3)   |   |                |(420SP)   |    |(320x240)             |
   |           |   |                |          |    | VA OFF               |
   |           |  MUX_VNF(VPSSM3) --|          |    |                      |
 IPC FRAMES    |   |                           |  FD(VPSSM3)----|        DUP_VA
 IN(VIDEOM3)   |   |                           |                |       (0)| (1)
   |           |  DUP_MJPEG(VPSSM3)            |                |----------|---- VA ON VPSSM3 ---|
   |           |   |   |-----------------------|                 VA ON DSP |(1)                  |
 MCTNF(VIDEOM3)|   |   |                                                   |                     |
   |           |  MUX(VPSSM3)                                     IPC FRAMES OUT(VPSSM3)     VA(VPSSM3)
   |           |   |                                                       |
 IPC FRAMES    |   |                                                       |
 OUT(VIDEOM3)  |  SWOSD(VPSSM3)                                    IPC FRAMES IN(DSP)
   |           |   |                                                       |
   |           |   |                                                       |
 IPC FRAMES ---|  DUP_DIS(VPSSM3) ----- DISPLAY_HDMI(VPSSM3)             VA(DSP)
 IN(VPSSM3)        |(0)           (1)
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

#ifdef IPNC_DSP_ON
    #define VA_ON_DSP
#endif

#define VS_W    1600
#define VS_H    900

/*in YUV sensor case, Enabling GLBCE means enabling Memory 2 Memory  */
#ifdef IMGS_OMNIVISION_OV10630
    #define YUV_M2M_DRV_TEST
#endif

UInt32 gNoiseFilterMode;

/* =============================================================================
 * Externs
 * ============================================================================= */
extern UInt32 gIsGlbceInitDone;
extern GlbcePreset gGLBCE_AlgoPreset;

/* =============================================================================
 * Use case code
 * ============================================================================= */
static SystemVideo_Ivahd2ChMap_Tbl systemVid_encDecIvaChMapTbl = {
    .isPopulated = 1,
    .ivaMap[0] = {
                  .EncNumCh = 2,
                  .EncChList = {0, 1},
                  .DecNumCh = 2,
                  .DecChList = {0, 1},
                  },

};
#ifdef ENABLE_TILER
    #define TILER_ENABLE    TRUE
#else
    #define TILER_ENABLE    FALSE
#endif

#define INCLUDE_MUX_LINK_INSTEAD_OF_MERGE

/* Merge Link for camera streams */
#define NUM_MERGE_LINK                  (1)
#define CAM_STREAM_MERGE_IDX            (0)

/* Dup Links */
#define NUM_DUP_LINK                    (5)
#define DIS_DUP_IDX                     (0)
#define MJPEG_DUP_LINK_IDX              (1)
#define SCALER_DUP_IDX                  (2)
#define VA_DUP_IDX                      (3)
#define VNF_DUP_IDX                     (4)

/* ===================================================================
 *  @func     MultiCh_createTriStreamFullFtr
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
Void MultiCh_createTriStreamFullFtr()
{
    UInt32 i;
    CameraLink_CreateParams cameraPrm;
    IspLink_CreateParams  ispPrm;
    GlbceLink_CreateParams  glbcePrm;
    UInt32 glbceId;
    NsfLink_CreateParams nsfPrm;
    VnfLink_CreateParams vnfPrm;
#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    MuxLink_CreateParams muxPrm;
#else
    MergeLink_CreateParams mergePrm[NUM_MERGE_LINK];
#endif
    IpcLink_CreateParams ipcOutVpssPrm;
    IpcLink_CreateParams ipcInVideoPrm;
    EncLink_CreateParams encPrm;
    DupLink_CreateParams dupPrm[NUM_DUP_LINK];
    SclrLink_CreateParams       sclrPrm;
    DisplayLink_CreateParams displayPrm;
    DisplayLink_CreateParams displayPrm_SD;
    FdLink_CreateParams fdPrm;
    SwosdLink_CreateParams swosdPrm;
    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;
    IpcBitsInLinkHLOS_CreateParams ipcBitsInHostPrm0;
    VstabLink_CreateParams vstabPrm;
    GlbceSupportLink_CreateParams glbceSupportPrm;
    VaLink_CreateParams vaPrm;
    IpcFramesOutLinkRTOS_CreateParams ipcFramesOutVpssPrm;
    IpcFramesInLinkRTOS_CreateParams ipcFramesInDspPrm;
    MuxLink_CreateParams muxPrmVnf;

    CameraLink_VipInstParams *pCameraInstPrm;
    CameraLink_OutParams *pCameraOutPrm;

#ifndef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    UInt32 mergeId[NUM_MERGE_LINK];
#endif

    UInt32 ipcOutVpssId;
    UInt32 ipcInVideoId;
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 vstabId;
    UInt32 glbceSupportId;
    UInt32 ipcFramesOutVpssId;
    UInt32 ipcFramesInDspId;

#ifdef WDR_ON
    WdrLink_CreateParams  wdrPrm;
    UInt32 wdrId;
#endif

#ifdef USE_MCTNF
    UInt32 ipcOutVpssIdForMCTNF;
    UInt32 ipcInVideoIdForMCTNF;
    UInt32 ipcOutVideoIdForMCTNF;
    UInt32 ipcInVpssIdForMCTNF;

    IpcLink_CreateParams ipcOutVpssIdForMCTNF_params;
    IpcLink_CreateParams ipcInVideoIdForMCTNF_params;
    IpcLink_CreateParams ipcOutVideoIdForMCTNF_params;
    IpcLink_CreateParams ipcInVpssIdForMCTNF_params;
    MctnfLink_CreateParams mctnfPrm;
#endif

    /* IPC struct init */
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcInVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkRTOS_CreateParams, ipcBitsOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkHLOS_CreateParams, ipcBitsInHostPrm0);
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, ipcFramesOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams, ipcFramesInDspPrm);

#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    MULTICH_INIT_STRUCT(MuxLink_CreateParams, muxPrm);
#endif

#ifdef USE_MCTNF
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssIdForMCTNF_params);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,  ipcInVideoIdForMCTNF_params);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVideoIdForMCTNF_params);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,  ipcInVpssIdForMCTNF_params);
#endif
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        OSA_printf("\n********** SMART ANALYTICS USECASE ********\n");
        OSA_printf
            ("********* Entered Smart Analytics usecase - H264 1080p @60fps + H264 D1 @30fps + MJPEG 1080p @5fps ********\n\n");
    }
    else
    {
        OSA_printf("\n********** FULL FEATURE USECASE ********\n");
        OSA_printf
            ("********* Entered Tri Streaming usecase - H264 1080p @60fps + H264 D1 @30fps + MJPEG 1080p @5fps ********\n\n");
    }

    MultiCh_detectBoard();

    System_linkControl(SYSTEM_LINK_ID_M3VPSS,
                       SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES, NULL, 0, TRUE);

    System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
                       SYSTEM_COMMON_CMD_SET_CH2IVAHD_MAP_TBL,
                       &systemVid_encDecIvaChMapTbl,
                       sizeof(SystemVideo_Ivahd2ChMap_Tbl), TRUE);

    /* Link IDs */
    gVcamModuleContext.cameraId = SYSTEM_LINK_ID_CAMERA;
    gVencModuleContext.encId = SYSTEM_LINK_ID_VENC_0;
    gMjpegModuleContext.mjpepEncId = SYSTEM_LINK_ID_MJPEG;
#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    gVsysModuleContext.muxId = SYSTEM_VPSS_LINK_ID_MUX_0;
#else
    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
#endif
    dupId[DIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[MJPEG_DUP_LINK_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    dupId[SCALER_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_2;
    dupId[VA_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_3;
    dupId[VNF_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_4;
    gVsysModuleContext.muxIdVnf = SYSTEM_VPSS_LINK_ID_MUX_1;
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVsysModuleContext.fdId = SYSTEM_LINK_ID_FD;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;
    gVcamModuleContext.ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;
    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;
#ifdef WDR_ON
    wdrId = SYSTEM_LINK_ID_WDR;
#endif
    gVdisModuleContext.displayId[VDIS_DEV_HDMI] = SYSTEM_LINK_ID_DISPLAY_0;
    gVdisModuleContext.displayId[VDIS_DEV_SD] = SYSTEM_LINK_ID_DISPLAY_2;
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    gVencModuleContext.ipcBitsOutRTOSId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gVencModuleContext.ipcBitsInHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;
    ipcFramesOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0;
    ipcFramesInDspId   = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;

#ifdef USE_MCTNF
    ipcOutVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1;
    ipcInVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1;
    ipcOutVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
    ipcInVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
    gVcamModuleContext.mctnfId = SYSTEM_LINK_ID_MCTNF;
#endif

#ifdef VA_ON_DSP
    gVsysModuleContext.vaId = SYSTEM_DSP_LINK_ID_VA;
#else
    gVsysModuleContext.vaId = SYSTEM_VPSS_LINK_ID_VA;
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
#ifndef USE_MCTNF
        /* When VA is running on VPSS M3 then if we run ISS NF which runs
           in nseries with VA will bring down system fps */
        gUI_mcfw_config.noisefilterMode = DSS_VNF_ON;
#endif
    }
#endif

#ifdef ENABLE_TILER_CAMERA
    /*
     * HDVPSS NSF doesn't support TILED input memory.
     * So if Camera TILER is enabled only ISS VNF is enabled.
     */
    gUI_mcfw_config.noisefilterMode = ISS_VNF_ON;
#endif

    if(gUI_mcfw_config.glbceEnable)
    {
        gVcamModuleContext.glbceId = glbceId;
#ifdef WDR_ON
        gVcamModuleContext.glbceId = wdrId;
#endif

        /* Camera Link params */
        CameraLink_CreateParams_Init(&cameraPrm);

        gVcamModuleContext.nsfId = SYSTEM_LINK_ID_NSF_0;
        gVcamModuleContext.vnfId = SYSTEM_LINK_ID_VNF;
        if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON)
        {
            cameraPrm.vnfLinkId = SYSTEM_LINK_ID_VNF;
        }

        cameraPrm.captureMode = CAMERA_LINK_CAPMODE_DDR;
#ifdef YUV_M2M_DRV_TEST
        cameraPrm.outQueParams[0].nextLink = gVcamModuleContext.ispId;
#else
        cameraPrm.outQueParams[0].nextLink = glbceId;
    #ifdef WDR_ON
        cameraPrm.outQueParams[0].nextLink = wdrId;
    #endif
#endif
        cameraPrm.ispLinkId = gVcamModuleContext.ispId;

        /* This is for TVP5158 Audio Channels - Change it to 16 if there are 16
         * audio channels connected in cascade */
        cameraPrm.numAudioChannels = 0;
        cameraPrm.numVipInst = 1;

#ifdef ENABLE_TILER_CAMERA
        cameraPrm.tilerEnable = TRUE;
#else
        cameraPrm.tilerEnable = FALSE;
#endif

        cameraPrm.vsEnable = gUI_mcfw_config.vsEnable;
        cameraPrm.vstabLinkId = vstabId;

        pCameraInstPrm = &cameraPrm.vipInst[0];
        pCameraInstPrm->vipInstId = SYSTEM_CAMERA_INST_VP_CSI2;
        pCameraInstPrm->videoDecoderId = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
        pCameraInstPrm->inDataFormat = SYSTEM_DF_YUV420SP_UV;

#ifdef IMGS_OMNIVISION_OV10630
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
#elif defined IMGS_SONY_IMX104
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
#elif defined IMGS_MICRON_MT9M034
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
#else
        pCameraInstPrm->sensorOutWidth  = 1920;
        pCameraInstPrm->sensorOutHeight = 1080;
#endif

        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
        }
        else
        {
            pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
            cameraPrm.issVnfEnable = 1;
        }

        pCameraInstPrm->numOutput = 1;

        pCameraOutPrm = &pCameraInstPrm->outParams[0];
        pCameraOutPrm->scEnable = FALSE;

#ifdef IMGS_OMNIVISION_OV10630
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV422I_UYVY;        //SYSTEM_DF_YUV422I_VYUY;
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#elif defined IMGS_SONY_IMX104
        pCameraOutPrm->dataFormat = SYSTEM_DF_BAYER_RAW;
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#elif defined IMGS_MICRON_MT9M034
        pCameraOutPrm->dataFormat = SYSTEM_DF_BAYER_RAW;
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#else
        pCameraOutPrm->dataFormat = SYSTEM_DF_BAYER_RAW;
        pCameraOutPrm->scOutWidth = 1920;
        pCameraOutPrm->scOutHeight = 1080;
#endif

        pCameraOutPrm->outQueId = 0;

        /* 2A config */
        cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
        cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;
    }
    else /*GLBCE OFF*/
    {

        gVcamModuleContext.glbceId = SYSTEM_LINK_ID_INVALID;
        /* Camera Link params */
        CameraLink_CreateParams_Init(&cameraPrm);

        gVcamModuleContext.nsfId = SYSTEM_LINK_ID_NSF_0;
        gVcamModuleContext.vnfId = SYSTEM_LINK_ID_VNF;
        if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON)
        {
            cameraPrm.vnfLinkId = SYSTEM_LINK_ID_VNF;
        }

        cameraPrm.captureMode = CAMERA_LINK_CAPMODE_ISIF;


#ifdef USE_MCTNF
        cameraPrm.mctnfLinkId = SYSTEM_LINK_ID_MCTNF;
#endif
        cameraPrm.outQueParams[0].nextLink = dupId[VNF_DUP_IDX];
        cameraPrm.outQueParams[1].nextLink = dupId[SCALER_DUP_IDX];

        /* This is for TVP5158 Audio Channels - Change it to 16 if there are 16
         * audio channels connected in cascade */
        cameraPrm.numAudioChannels = 1;
        cameraPrm.numVipInst = 1;

#ifdef ENABLE_TILER_CAMERA
        cameraPrm.tilerEnable = TRUE;
#else
        cameraPrm.tilerEnable = FALSE;
#endif

        cameraPrm.vsEnable = gUI_mcfw_config.vsEnable;
        cameraPrm.vstabLinkId = vstabId;

        pCameraInstPrm = &cameraPrm.vipInst[0];
        pCameraInstPrm->vipInstId = SYSTEM_CAMERA_INST_VP_CSI2;
        pCameraInstPrm->videoDecoderId = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
        pCameraInstPrm->inDataFormat = SYSTEM_DF_YUV420SP_UV;

#ifdef IMGS_OMNIVISION_OV10630
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
#elif defined IMGS_SONY_IMX104
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
#elif defined IMGS_MICRON_MT9M034
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
#else
        pCameraInstPrm->sensorOutWidth  = 1920;
        pCameraInstPrm->sensorOutHeight = 1080;
#endif

        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
        }
        else
        {
            pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
            cameraPrm.issVnfEnable = 1;
        }

#ifdef USE_MCTNF
        pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
#endif
        pCameraInstPrm->numOutput = 2;

        /* First stream */
        pCameraOutPrm = &pCameraInstPrm->outParams[0];

        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            pCameraOutPrm->dataFormat = SYSTEM_DF_YUV422I_UYVY;
        }
        else
        {
            pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
        }

        pCameraOutPrm->scEnable = FALSE;

#ifdef IMGS_OMNIVISION_OV10630
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#elif defined IMGS_SONY_IMX104
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#elif defined IMGS_MICRON_MT9M034
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#else
        /* When VS is enabled then scOutWidth/scOutHeight cannot be equal to sensorOutWidth/sensorOutHeight */
        if(cameraPrm.vsEnable == 0)
        {
            pCameraOutPrm->scOutWidth = 1920;
            pCameraOutPrm->scOutHeight = 1080;
        }
        else
        {
            pCameraOutPrm->scOutWidth = VS_W;
            pCameraOutPrm->scOutHeight = VS_H;
        }
#endif

        pCameraOutPrm->outQueId = 0;

        /* Second stream */
        pCameraOutPrm = &pCameraInstPrm->outParams[1];
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
        pCameraOutPrm->scEnable = FALSE;
        pCameraOutPrm->scOutWidth = 720;
        pCameraOutPrm->scOutHeight = 480;
        pCameraOutPrm->standard = SYSTEM_STD_NTSC;
        pCameraOutPrm->outQueId = 1;

        /* 2A config */
        cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
        cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;
    }

    /* vstab Link params */
    vstabPrm.totalFrameWidth  = pCameraInstPrm->sensorOutWidth;
    vstabPrm.totalFrameHeight = pCameraInstPrm->sensorOutHeight;

    glbceSupportPrm.glbceLinkID = glbceId;
    glbceSupportPrm.totalFrameWidth  = pCameraInstPrm->sensorOutWidth;
    glbceSupportPrm.totalFrameHeight = pCameraInstPrm->sensorOutHeight;

    if(gUI_mcfw_config.glbceEnable)
    {
        cameraPrm.glbceLinkId = glbceId;
#ifdef WDR_ON
        cameraPrm.glbceLinkId = SYSTEM_LINK_ID_INVALID;
#endif
        /* Glbce Link Params */
        glbcePrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
        glbcePrm.inQueParams.prevLinkQueId = 0;
        glbcePrm.outQueParams.nextLink = gVcamModuleContext.ispId;
        glbcePrm.chCreateParams[0].preset  = 0;//gGLBCE_AlgoPreset;
        glbcePrm.glbceEnable = gUI_mcfw_config.glbceEnable;
        glbcePrm.glbceSupportID = glbceSupportId;
        glbcePrm.swosdLinkID = gVsysModuleContext.swOsdId;

#ifdef WDR_ON
        wdrPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
        wdrPrm.inQueParams.prevLinkQueId = 0;
        wdrPrm.outQueParams.nextLink = gVcamModuleContext.ispId;
        wdrPrm.chCreateParams[0].dParams.wdrMode = gUI_mcfw_config.glbceEnable > 3 ? gUI_mcfw_config.glbceEnable -3 : 0;
#endif

        /* isp link params */
#ifdef YUV_M2M_DRV_TEST
        ispPrm.inQueParams.prevLinkId    = gVcamModuleContext.cameraId;
#else
        ispPrm.inQueParams.prevLinkId    = glbceId;
#ifdef WDR_ON
        ispPrm.inQueParams.prevLinkId     = wdrId;
#endif
#endif
        ispPrm.inQueParams.prevLinkQueId = 0;
        ispPrm.numOutQueue               = 2;

#ifdef USE_MCTNF
        cameraPrm.mctnfLinkId = SYSTEM_LINK_ID_MCTNF;
#endif
        ispPrm.outQueInfo[0].nextLink  = dupId[VNF_DUP_IDX];
        ispPrm.outQueInfo[1].nextLink    = dupId[SCALER_DUP_IDX];

#if defined(TI_8107_BUILD)
        ispPrm.clkDivM                     = 10;
        ispPrm.clkDivN                    = 30;//30;
#else
        ispPrm.clkDivM                     = 10;
        ispPrm.clkDivN                    = 20;
#endif

        ispPrm.vsEnable = gUI_mcfw_config.vsEnable;

        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
            ispPrm.outQueuePrm[0].dataFormat = SYSTEM_DF_YUV422I_UYVY;
        else
            ispPrm.outQueuePrm[0].dataFormat = SYSTEM_DF_YUV420SP_UV;

#ifdef IMGS_OMNIVISION_OV10630
        ispPrm.outQueuePrm[0].width      = 1280;
        ispPrm.outQueuePrm[0].height     = 720;
#elif defined IMGS_SONY_IMX104
        ispPrm.outQueuePrm[0].width      = 1280;
        ispPrm.outQueuePrm[0].height     = 720; //960;
#elif defined IMGS_MICRON_MT9M034
        ispPrm.outQueuePrm[0].width      = 1280;
        ispPrm.outQueuePrm[0].height     = 720;
#else
        if(ispPrm.vsEnable == TRUE)
        {
            ispPrm.outQueuePrm[0].width      = VS_W;
            ispPrm.outQueuePrm[0].height     = VS_H;
        }
        else
        {
            ispPrm.outQueuePrm[0].width      = 1920;
            ispPrm.outQueuePrm[0].height     = 1080;
        }
#endif

        ispPrm.outQueuePrm[0].tilerEnable = 0;
        ispPrm.outQueuePrm[1].tilerEnable = 0;

        ispPrm.outQueuePrm[1].dataFormat = SYSTEM_DF_YUV420SP_UV;
        ispPrm.outQueuePrm[1].width      = 720;
        ispPrm.outQueuePrm[1].height     = 480;//576;
        ispPrm.outQueuePrm[1].standard   = SYSTEM_STD_NTSC;//SYSTEM_STD_PAL;//SYSTEM_STD_NTSC;
    }

    ispPrm.vnfFullResolution = FALSE;
    cameraPrm.vnfFullResolution = FALSE;

    /* Dup link for VNF */
    if(gUI_mcfw_config.glbceEnable)
    {
        dupPrm[VNF_DUP_IDX].inQueParams.prevLinkId    = gVcamModuleContext.ispId;
        dupPrm[VNF_DUP_IDX].inQueParams.prevLinkQueId = 0;
    }
    else
    {
        dupPrm[VNF_DUP_IDX].inQueParams.prevLinkId    = gVcamModuleContext.cameraId;
        dupPrm[VNF_DUP_IDX].inQueParams.prevLinkQueId = 0;
    }

    dupPrm[VNF_DUP_IDX].numOutQue                 = 2;
    dupPrm[VNF_DUP_IDX].outQueParams[0].nextLink  = gVcamModuleContext.nsfId;

#ifdef USE_MCTNF
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        dupPrm[VNF_DUP_IDX].outQueParams[1].nextLink  = ipcOutVpssIdForMCTNF;
    }
    else
    {
    #ifdef VNF_BEFORE_MCTNF
        dupPrm[VNF_DUP_IDX].outQueParams[1].nextLink  = gVcamModuleContext.vnfId;
    #else
        dupPrm[VNF_DUP_IDX].outQueParams[1].nextLink  = ipcOutVpssIdForMCTNF;
    #endif
    }
#else
    dupPrm[VNF_DUP_IDX].outQueParams[1].nextLink  = gVcamModuleContext.vnfId;
#endif

#ifdef MEMORY_256MB
    dupPrm[VNF_DUP_IDX].numOutQue = 1;
    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        dupPrm[VNF_DUP_IDX].outQueParams[0].nextLink  = gVcamModuleContext.nsfId;
    else
        dupPrm[VNF_DUP_IDX].outQueParams[0].nextLink  = gVcamModuleContext.vnfId;
#endif

    dupPrm[VNF_DUP_IDX].notifyNextLink = TRUE;

#ifdef USE_MCTNF
     /* IPC Out VPSS link params */
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkId = dupId[VNF_DUP_IDX];
        ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkQueId = 1;
    }
    else
    {
    #ifdef VNF_BEFORE_MCTNF
        ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkId = gVcamModuleContext.vnfId;
        ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    #else
        ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkId = dupId[VNF_DUP_IDX];
        ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkQueId = 1;
    #endif
    }

    ipcOutVpssIdForMCTNF_params.numOutQue = 1;
    ipcOutVpssIdForMCTNF_params.outQueParams[0].nextLink = ipcInVideoIdForMCTNF;
    ipcOutVpssIdForMCTNF_params.notifyNextLink = TRUE;
    ipcOutVpssIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcOutVpssIdForMCTNF_params.noNotifyMode = FALSE;

    /* IPC In VIDEO params */
    ipcInVideoIdForMCTNF_params.inQueParams.prevLinkId = ipcOutVpssIdForMCTNF;
    ipcInVideoIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    ipcInVideoIdForMCTNF_params.numOutQue = 1;
    ipcInVideoIdForMCTNF_params.outQueParams[0].nextLink = gVcamModuleContext.mctnfId;
    ipcInVideoIdForMCTNF_params.notifyNextLink = TRUE;
    ipcInVideoIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcInVideoIdForMCTNF_params.noNotifyMode = FALSE;

    /* MCTNF link params */
    MctnfLink_ChCreateParams *mctnfParams;
    MctnfLink_CreateParams_Init(&mctnfPrm);
    mctnfPrm.inQueParams.prevLinkId = ipcInVideoIdForMCTNF;
    mctnfPrm.inQueParams.prevLinkQueId = 0;
    mctnfPrm.outQueParams.nextLink = ipcOutVideoIdForMCTNF;

    mctnfParams = &mctnfPrm.chCreateParams[0];
    mctnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;

    if(gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        mctnfPrm.enable[0] = 0;
    }

     /* IPC Out VPSS link params */
    ipcOutVideoIdForMCTNF_params.inQueParams.prevLinkId = gVcamModuleContext.mctnfId;
    ipcOutVideoIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    ipcOutVideoIdForMCTNF_params.numOutQue = 1;
    ipcOutVideoIdForMCTNF_params.outQueParams[0].nextLink = ipcInVpssIdForMCTNF;
    ipcOutVideoIdForMCTNF_params.notifyNextLink = TRUE;
    ipcOutVideoIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcOutVideoIdForMCTNF_params.noNotifyMode = FALSE;

    /* IPC In VIDEO params */
    ipcInVpssIdForMCTNF_params.inQueParams.prevLinkId = ipcOutVideoIdForMCTNF;
    ipcInVpssIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    ipcInVpssIdForMCTNF_params.numOutQue = 1;

    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        ipcInVpssIdForMCTNF_params.outQueParams[0].nextLink = gVsysModuleContext.muxIdVnf;
    }
    else
    {
    #ifdef VNF_BEFORE_MCTNF
        ipcInVpssIdForMCTNF_params.outQueParams[0].nextLink = gVsysModuleContext.muxIdVnf;
    #else
        ipcInVpssIdForMCTNF_params.outQueParams[0].nextLink = gVcamModuleContext.vnfId;
    #endif
    }

    ipcInVpssIdForMCTNF_params.notifyNextLink = TRUE;
    ipcInVpssIdForMCTNF_params.notifyPrevLink = TRUE;
    ipcInVpssIdForMCTNF_params.noNotifyMode = FALSE;
#endif

    /* NSF link params */
    NsfLink_CreateParams_Init(&nsfPrm);
    if(gUI_mcfw_config.snfEnable)
    {
        if (gUI_mcfw_config.tnfEnable)
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_NONE;
        else
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_TNF;
    }
    else
    {
        if (gUI_mcfw_config.tnfEnable)
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_SNF;
        else
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_SNF_TNF;
    }
#if (TILER_ENABLE == TRUE)
    nsfPrm.tilerEnable = TRUE;
#else
    nsfPrm.tilerEnable = FALSE;
#endif

    nsfPrm.inQueParams.prevLinkId = dupId[VNF_DUP_IDX];
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.numOutQue = 1;
    nsfPrm.inputFrameRate = 60;
    nsfPrm.outputFrameRate = 60;

#ifdef MEMORY_256MB
    nsfPrm.numBufsPerCh = 5;
    nsfPrm.outQueParams[0].nextLink = dupId[MJPEG_DUP_LINK_IDX];
#else
    /*
     *  More than 6 output buffers are neede to realize 60 fps at encoder input
     */
    if(pCameraInstPrm->standard == SYSTEM_STD_1080P_60)
        nsfPrm.numBufsPerCh = 8;
    else
        nsfPrm.numBufsPerCh = 6;
    nsfPrm.outQueParams[0].nextLink = gVsysModuleContext.muxIdVnf;
#endif

    nsfPrm.nsfStrength = gUI_mcfw_config.vnfStrength;

    /* Channel enable/disable */
    if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON)
    {
        nsfPrm.enable[0] = 0;
    }

    /* VNF link params */
    VnfLink_ChCreateParams *vnfParams;

    VnfLink_CreateParams_Init(&vnfPrm);

    /* Vnf Link Params */
#ifdef USE_MCTNF
    #ifdef VNF_BEFORE_MCTNF
        vnfPrm.inQueParams.prevLinkId = dupId[VNF_DUP_IDX];
        vnfPrm.inQueParams.prevLinkQueId = 1;
    #else
        vnfPrm.inQueParams.prevLinkId = ipcInVpssIdForMCTNF;
        vnfPrm.inQueParams.prevLinkQueId = 0;
    #endif
#else
    vnfPrm.inQueParams.prevLinkId = dupId[VNF_DUP_IDX];
    vnfPrm.inQueParams.prevLinkQueId = 1;
    #ifdef MEMORY_256MB
        vnfPrm.inQueParams.prevLinkQueId = 0;
    #endif
#endif

    vnfPrm.outQueParams.nextLink = gVsysModuleContext.muxIdVnf;
#if defined(USE_MCTNF) && defined(VNF_BEFORE_MCTNF)
    vnfPrm.outQueParams.nextLink = ipcOutVpssIdForMCTNF;
#endif

#ifdef MEMORY_256MB
    vnfPrm.outQueParams.nextLink = dupId[MJPEG_DUP_LINK_IDX];
#endif

    vnfParams = &vnfPrm.chCreateParams[0];
    vnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;
    if (gUI_mcfw_config.ldcEnable)
    {
        vnfParams->sParams.eOperateMode = VNF_LINK_LDC_AFFINE_NSF_TNF;
    }
    else
    {
#ifdef USE_TNF2_FILTER
        vnfParams->sParams.eOperateMode =  VNF_LINK_3DNF;
#elif defined (USE_TNF3_FILTER)
        vnfParams->sParams.eOperateMode =  VNF_LINK_TNF3;
#ifdef USE_MCTNF
        //LK- to check TNF3/MCTNF by controlling enable/disable from DCC tool
        vnfParams->sParams.eOperateMode =  VNF_LINK_NSF2;
#endif
#else
        vnfParams->sParams.eOperateMode = VNF_LINK_AFFINE_NSF_TNF;
#endif
        vnfParams->sParams.eNsfSet = VNF_LINK_NSF_LUMA_CHROMA;
    }

    if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON)
    {
        if (((vnfParams->sParams.eOperateMode == VNF_LINK_TNF3) && (vnfParams->sParams.eNsfSet == VNF_LINK_NSF_LUMA_CHROMA)) ||
            (vnfParams->sParams.eOperateMode == VNF_LINK_NSF2) || (vnfParams->sParams.eOperateMode == VNF_LINK_LDC_AFFINE_NSF_TNF))
        {
            if (gUI_mcfw_config.glbceEnable)
            {
                ispPrm.vnfFullResolution = TRUE;
                cameraPrm.vnfFullResolution = FALSE;
            }
            else
            {
                ispPrm.vnfFullResolution = FALSE;
                cameraPrm.vnfFullResolution = TRUE;
            }
        }
    }

    vnfParams->dParams.ldcParams.eInterpolationLuma = VNF_LINK_YINTERPOLATION_BILINEAR;
    vnfParams->dParams.ldcParams.unPixelPad = 4;

    vnfParams->dParams.nsfParams.bLumaEn = TRUE;
    vnfParams->dParams.nsfParams.bChromaEn = TRUE;
    vnfParams->dParams.nsfParams.eSmoothVal = VNF_LINK_SMOOTH_2;
    vnfParams->dParams.nsfParams.bSmoothLumaEn = TRUE;
    vnfParams->dParams.nsfParams.bSmoothChromaEn = FALSE;

    vnfParams->dParams.tnfParams.useDefault = TRUE;

    vnfParams->dParams.nsfFilter.eFilterParam = VNF_LINK_PARAM_DEFAULT;
    vnfParams->dParams.nsfEdge.eEdgeParam = VNF_LINK_PARAM_DISABLE;
    vnfParams->dParams.nsfShading.eShdParam = VNF_LINK_PARAM_DISABLE;
    vnfParams->dParams.nsfDesaturation.eDesatParam = VNF_LINK_PARAM_DISABLE;

    vnfPrm.chCreateParams[0].bSnfEnable = gUI_mcfw_config.snfEnable;
    vnfPrm.chCreateParams[0].bTnfEnable = gUI_mcfw_config.tnfEnable;
    vnfPrm.chCreateParams[0].strength   = gUI_mcfw_config.vnfStrength;

    /* Channel enable/disable */
    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        vnfPrm.enable[0] = 0;
    }

#if (TILER_ENABLE == TRUE)
    vnfPrm.tilerEnable = TRUE;
#else
    vnfPrm.tilerEnable = FALSE;
#endif

    /* MUX link params */
    muxPrmVnf.numInQue = 2;
    muxPrmVnf.inQueParams[0].prevLinkId    = gVcamModuleContext.nsfId;
    muxPrmVnf.inQueParams[0].prevLinkQueId = 0;

#ifdef USE_MCTNF
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        muxPrmVnf.inQueParams[1].prevLinkId    = ipcInVpssIdForMCTNF;
        muxPrmVnf.inQueParams[1].prevLinkQueId = 0;
    }
    else
    {
    #ifdef VNF_BEFORE_MCTNF
        muxPrmVnf.inQueParams[1].prevLinkId    = ipcInVpssIdForMCTNF;
        muxPrmVnf.inQueParams[1].prevLinkQueId = 0;
    #else
        muxPrmVnf.inQueParams[1].prevLinkId    = gVcamModuleContext.vnfId;
        muxPrmVnf.inQueParams[1].prevLinkQueId = 0;
    #endif
    }
#else
    muxPrmVnf.inQueParams[1].prevLinkId    = gVcamModuleContext.vnfId;
    muxPrmVnf.inQueParams[1].prevLinkQueId = 0;
#endif

    muxPrmVnf.outQueParams.nextLink = dupId[MJPEG_DUP_LINK_IDX];

    muxPrmVnf.muxNumOutChan = 1;

    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        muxPrmVnf.outChMap[0].inQueId = 0;
        muxPrmVnf.outChMap[0].inChNum = 0;
    }
    else
    {
        muxPrmVnf.outChMap[0].inQueId = 1;
        muxPrmVnf.outChMap[0].inChNum = 0;
    }

#if defined(USE_MCTNF) && !defined(VNF_BEFORE_MCTNF)
    //Reduce the additional resolution of vnf requirement if MCTNF is there in chain
    //as padding done by MCTNF can be used in the resolution loss
    ispPrm.vnfFullResolution = FALSE;
    cameraPrm.vnfFullResolution = FALSE;
#endif

    /* Dup Link params */
#ifdef MEMORY_256MB
    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        dupPrm[MJPEG_DUP_LINK_IDX].inQueParams.prevLinkId = gVcamModuleContext.nsfId;
    else
        dupPrm[MJPEG_DUP_LINK_IDX].inQueParams.prevLinkId = gVcamModuleContext.vnfId;
#else
    dupPrm[MJPEG_DUP_LINK_IDX].inQueParams.prevLinkId = gVsysModuleContext.muxIdVnf;
#endif

    dupPrm[MJPEG_DUP_LINK_IDX].inQueParams.prevLinkQueId = 0;
    dupPrm[MJPEG_DUP_LINK_IDX].numOutQue = 2;

#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    dupPrm[MJPEG_DUP_LINK_IDX].outQueParams[0].nextLink =
                                                    gVsysModuleContext.muxId;
    dupPrm[MJPEG_DUP_LINK_IDX].outQueParams[1].nextLink =
                                                    gVsysModuleContext.muxId;
#else
    dupPrm[MJPEG_DUP_LINK_IDX].outQueParams[0].nextLink =
        mergeId[CAM_STREAM_MERGE_IDX];
    dupPrm[MJPEG_DUP_LINK_IDX].outQueParams[1].nextLink =
        mergeId[CAM_STREAM_MERGE_IDX];
#endif
    dupPrm[MJPEG_DUP_LINK_IDX].notifyNextLink = TRUE;

    /* Dup link for Scaler params */
    if(gUI_mcfw_config.glbceEnable)
        dupPrm[SCALER_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.ispId;
    else
        dupPrm[SCALER_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.cameraId;

    dupPrm[SCALER_DUP_IDX].inQueParams.prevLinkQueId = 1;
    dupPrm[SCALER_DUP_IDX].numOutQue = 3;
#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    dupPrm[SCALER_DUP_IDX].outQueParams[0].nextLink = gVsysModuleContext.muxId;
#else
    dupPrm[SCALER_DUP_IDX].outQueParams[0].nextLink =
        mergeId[CAM_STREAM_MERGE_IDX];
#endif
    dupPrm[SCALER_DUP_IDX].outQueParams[1].nextLink = gVcamModuleContext.sclrId[0];
    dupPrm[SCALER_DUP_IDX].outQueParams[2].nextLink =
        gVdisModuleContext.displayId[VDIS_DEV_SD];
    dupPrm[SCALER_DUP_IDX].notifyNextLink = TRUE;

    /* display link params */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm_SD);
    displayPrm_SD.inQueParams[0].prevLinkId = dupId[SCALER_DUP_IDX];
    displayPrm_SD.inQueParams[0].prevLinkQueId = 2;
    displayPrm_SD.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_SD].resolution;
    displayPrm_SD.displayId  = DISPLAY_LINK_DISPLAY_SD;

#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE

    muxPrm.numInQue = 3;
    muxPrm.inQueParams[0].prevLinkId = dupId[MJPEG_DUP_LINK_IDX];
    muxPrm.inQueParams[0].prevLinkQueId = 0;

    muxPrm.inQueParams[1].prevLinkId = dupId[SCALER_DUP_IDX];
    muxPrm.inQueParams[1].prevLinkQueId = 0;

    muxPrm.inQueParams[2].prevLinkId = dupId[MJPEG_DUP_LINK_IDX];
    muxPrm.inQueParams[2].prevLinkQueId = 1;

    muxPrm.outQueParams.nextLink = gVsysModuleContext.swOsdId;

    muxPrm.muxNumOutChan = 3;
    muxPrm.outChMap[0].inQueId = 0;
    muxPrm.outChMap[0].inChNum = 0;

    muxPrm.outChMap[1].inQueId = 1;
    muxPrm.outChMap[1].inChNum = 0;

    muxPrm.outChMap[2].inQueId = 2;
    muxPrm.outChMap[2].inChNum = 0;

#else
    /* Merge Link params */
    mergePrm[CAM_STREAM_MERGE_IDX].numInQue = 3;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkId =
        dupId[MJPEG_DUP_LINK_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkQueId = 0;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkId =
        dupId[SCALER_DUP_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkQueId = 0;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[2].prevLinkId =
        dupId[MJPEG_DUP_LINK_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[2].prevLinkQueId = 1;
    mergePrm[CAM_STREAM_MERGE_IDX].outQueParams.nextLink =
        gVsysModuleContext.swOsdId;
    mergePrm[CAM_STREAM_MERGE_IDX].notifyNextLink = TRUE;
#endif

    /* SWOSD Link Params */
#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    swosdPrm.inQueParams.prevLinkId = gVsysModuleContext.muxId;
#else
    swosdPrm.inQueParams.prevLinkId = mergeId[CAM_STREAM_MERGE_IDX];
#endif

    swosdPrm.inQueParams.prevLinkQueId = 0;
    swosdPrm.outQueParams.nextLink = dupId[DIS_DUP_IDX];

    /* Display dup link params */
    dupPrm[DIS_DUP_IDX].inQueParams.prevLinkId = gVsysModuleContext.swOsdId;
    dupPrm[DIS_DUP_IDX].inQueParams.prevLinkQueId = 0;
    dupPrm[DIS_DUP_IDX].numOutQue = 2;
    dupPrm[DIS_DUP_IDX].outQueParams[0].nextLink = ipcOutVpssId;
    dupPrm[DIS_DUP_IDX].outQueParams[1].nextLink = gVdisModuleContext.displayId[VDIS_DEV_HDMI];
    dupPrm[DIS_DUP_IDX].notifyNextLink = TRUE;

    /* display link params */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
    displayPrm.inQueParams[0].prevLinkId = dupId[DIS_DUP_IDX];
    displayPrm.inQueParams[0].prevLinkQueId = 1;
    displayPrm.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    displayPrm.displayId  = DISPLAY_LINK_DISPLAY_SC2;

    /* Scaler Link Params */
    SclrLink_CreateParams_Init(&sclrPrm);
    sclrPrm.inQueParams.prevLinkId             = dupId[SCALER_DUP_IDX];
    sclrPrm.inQueParams.prevLinkQueId          = 1;
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
        sclrPrm.outQueParams.nextLink          = dupId[VA_DUP_IDX];
    else
        sclrPrm.outQueParams.nextLink          = gVsysModuleContext.fdId;
    sclrPrm.tilerEnable                        = FALSE;
    sclrPrm.enableLineSkipSc                   = FALSE;
    sclrPrm.inputFrameRate                     = 30;
    sclrPrm.outputFrameRate                    = 30;
    sclrPrm.scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm.scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm.outScaleFactor.absoluteResolution.outWidth   = 320;
    sclrPrm.outScaleFactor.absoluteResolution.outHeight  = 192;
    sclrPrm.outDataFormat = VF_YUV420SP_UV;
    sclrPrm.pathId = SCLR_LINK_SEC0_SC3;

    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        dupPrm[VA_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.sclrId[0];
        dupPrm[VA_DUP_IDX].inQueParams.prevLinkQueId = 0;
        dupPrm[VA_DUP_IDX].numOutQue = 2;
        dupPrm[VA_DUP_IDX].outQueParams[0].nextLink = gVsysModuleContext.fdId;
#ifdef VA_ON_DSP
        dupPrm[VA_DUP_IDX].outQueParams[1].nextLink = ipcFramesOutVpssId;
#else
        dupPrm[VA_DUP_IDX].outQueParams[1].nextLink = gVsysModuleContext.vaId;
#endif
        dupPrm[VA_DUP_IDX].notifyNextLink = TRUE;

#ifdef VA_ON_DSP
        /* IPC Frames Out VPSS for VA link params */
        ipcFramesOutVpssPrm.baseCreateParams.inQueParams.prevLinkId = dupId[VA_DUP_IDX];
        ipcFramesOutVpssPrm.baseCreateParams.inQueParams.prevLinkQueId = 1;
        ipcFramesOutVpssPrm.baseCreateParams.notifyPrevLink = TRUE;
        ipcFramesOutVpssPrm.baseCreateParams.inputFrameRate = 30;
        ipcFramesOutVpssPrm.baseCreateParams.outputFrameRate = 30;

        ipcFramesOutVpssPrm.baseCreateParams.numOutQue = 1;
        ipcFramesOutVpssPrm.baseCreateParams.outQueParams[0].nextLink = ipcFramesInDspId;
        ipcFramesOutVpssPrm.baseCreateParams.notifyNextLink = TRUE;

        ipcFramesOutVpssPrm.baseCreateParams.processLink = SYSTEM_LINK_ID_INVALID;
        ipcFramesOutVpssPrm.baseCreateParams.notifyProcessLink = FALSE;
        ipcFramesOutVpssPrm.baseCreateParams.noNotifyMode = FALSE;

        /* IPC Frames in DSP for VA link params */
        ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkId = ipcFramesOutVpssId;
        ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
        ipcFramesInDspPrm.baseCreateParams.numOutQue   = 1;
        ipcFramesInDspPrm.baseCreateParams.outQueParams[0].nextLink = gVsysModuleContext.vaId;
        ipcFramesInDspPrm.baseCreateParams.notifyPrevLink = TRUE;
        ipcFramesInDspPrm.baseCreateParams.notifyNextLink = TRUE;
        ipcFramesInDspPrm.baseCreateParams.noNotifyMode   = FALSE;

        /* VA link params */
        VaLink_CreateParams_Init(&vaPrm);
        vaPrm.inQueParams.prevLinkId    = ipcFramesInDspId;
        vaPrm.inQueParams.prevLinkQueId = 0;
        vaPrm.encLinkId                 = gVencModuleContext.encId;
        vaPrm.swosdLinkId               = gVsysModuleContext.swOsdId;
        vaPrm.cameraLinkId              = gVcamModuleContext.cameraId;
        vaPrm.vaFrameRate               = 25;
#else
        /* VA link params */
        VaLink_CreateParams_Init(&vaPrm);
        vaPrm.inQueParams.prevLinkId    = dupId[VA_DUP_IDX];
        vaPrm.inQueParams.prevLinkQueId = 1;
        vaPrm.encLinkId                 = gVencModuleContext.encId;
        vaPrm.swosdLinkId               = gVsysModuleContext.swOsdId;
        vaPrm.cameraLinkId              = gVcamModuleContext.cameraId;
        vaPrm.vaFrameRate               = 25;
#endif
    }

    /* FD Link Params */
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
        fdPrm.inQueParams.prevLinkId = dupId[VA_DUP_IDX];
    else
        fdPrm.inQueParams.prevLinkId = gVcamModuleContext.sclrId[0];
    fdPrm.inQueParams.prevLinkQueId = 0;

    /* IPC Out VPSS link params */
    ipcOutVpssPrm.inQueParams.prevLinkId = dupId[DIS_DUP_IDX];
    ipcOutVpssPrm.inQueParams.prevLinkQueId = 0;
    ipcOutVpssPrm.numOutQue = 1;
    ipcOutVpssPrm.outQueParams[0].nextLink = ipcInVideoId;
    ipcOutVpssPrm.notifyNextLink = TRUE;
    ipcOutVpssPrm.notifyPrevLink = TRUE;
    ipcOutVpssPrm.noNotifyMode = FALSE;

    /* IPC In VIDEO params */
    ipcInVideoPrm.inQueParams.prevLinkId = ipcOutVpssId;
    ipcInVideoPrm.inQueParams.prevLinkQueId = 0;
    ipcInVideoPrm.numOutQue = 1;
    ipcInVideoPrm.outQueParams[0].nextLink = gVencModuleContext.encId;
    ipcInVideoPrm.notifyNextLink = TRUE;
    ipcInVideoPrm.notifyPrevLink = TRUE;
    ipcInVideoPrm.noNotifyMode = FALSE;

    /* Video Encoder Link params */
    MULTICH_INIT_STRUCT(EncLink_CreateParams, encPrm);
    {
        EncLink_ChCreateParams *pLinkChPrm;
        EncLink_ChDynamicParams *pLinkDynPrm;
        VENC_CHN_DYNAMIC_PARAM_S *pDynPrm;
        VENC_CHN_PARAMS_S *pChPrm;

        for (i = 0; i < VENC_PRIMARY_CHANNELS; i++)
        {
            pLinkChPrm  = &encPrm.chCreateParams[i];
            pLinkDynPrm = &pLinkChPrm->defaultDynamicParams;

            pChPrm      = &gVencModuleContext.vencConfig.encChannelParams[i];
            pDynPrm     = &pChPrm->dynamicParam;

            switch(gUI_mcfw_config.demoCfg.codec_combo) {
                case 0: pLinkChPrm->format = IVIDEO_H264HP; break; //"SINGLE_H264"
                case 1: pLinkChPrm->format = IVIDEO_MPEG4SP; break; //"SINGLE_MPEG4"
                case 2: pLinkChPrm->format = IVIDEO_MJPEG; break; //"SINGLE_JPEG"
                case 3: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_MJPEG; break; //"H264_JPEG"
                case 4: pLinkChPrm->format = (i==0)? IVIDEO_MPEG4SP:IVIDEO_MJPEG; break; //"MPEG4_JPEG"
                case 5: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_H264HP; break; //"DUAL_H264"
                case 6: pLinkChPrm->format = (i==0)? IVIDEO_MPEG4SP:IVIDEO_MPEG4SP; break; //"DUAL_MPEG4"
                case 7: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_MPEG4SP; break;  //"H264_MPEG4"
                case 8: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_H264HP; break;  //"TRIPLE_H264"
                case 9: pLinkChPrm->format = (i==0)? IVIDEO_MPEG4SP:IVIDEO_MPEG4SP; break; //"TRIPLE_MPEG4"
                default: pLinkChPrm->format = IVIDEO_H264HP;
            }

            pLinkChPrm->profile                 = gVencModuleContext.vencConfig.h264Profile[i];
            pLinkChPrm->dataLayout              = IVIDEO_PROGRESSIVE;
            pLinkChPrm->fieldMergeEncodeEnable  = FALSE;
            pLinkChPrm->enableAnalyticinfo      = pChPrm->enableAnalyticinfo;
            pLinkChPrm->maxBitRate              = pChPrm->maxBitRate;
            pLinkChPrm->encodingPreset          = pChPrm->encodingPreset;
            pLinkChPrm->rateControlPreset       = IVIDEO_USER_DEFINED; //pChPrm->rcType;
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

            encPrm.numBufPerCh[i] = 4;
            gVencModuleContext.encFormat[i] = pLinkChPrm->format;
        }
    }

    /* Video Encoder Framerate */
#ifdef IMGS_OMNIVISION_OV10630
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#elif defined IMGS_SONY_IMX104
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#elif defined IMGS_MICRON_MT9M034
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  //
#else
#ifdef USE_MCTNF
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#else
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 60;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#endif
#endif
    encPrm.chCreateParams[1].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;

    encPrm.vsEnable = gUI_mcfw_config.vsEnable;

    if(gUI_mcfw_config.vaUseCase == TRUE)
    {
        encPrm.isVaUseCase = 1;
    }
    else
    {
        encPrm.isVaUseCase = 0;
    }

    for (i = VENC_PRIMARY_CHANNELS; i < (VENC_CHN_MAX - 1); i++)
    {
        encPrm.chCreateParams[i].format = IVIDEO_MJPEG;
        encPrm.chCreateParams[i].profile = 0;
        encPrm.chCreateParams[i].dataLayout = IVIDEO_PROGRESSIVE;
        encPrm.chCreateParams[i].fieldMergeEncodeEnable = FALSE;
        encPrm.chCreateParams[i].defaultDynamicParams.intraFrameInterval = 0;
        encPrm.chCreateParams[i].encodingPreset = 0;
        encPrm.chCreateParams[i].enableAnalyticinfo = 0;
        encPrm.chCreateParams[i].enableWaterMarking = 0;
        encPrm.chCreateParams[i].defaultDynamicParams.inputFrameRate = 60;
        encPrm.chCreateParams[i].rateControlPreset = 0;
        encPrm.chCreateParams[i].defaultDynamicParams.targetBitRate =
            100 * 1000;
        encPrm.chCreateParams[i].defaultDynamicParams.interFrameInterval = 0;
        encPrm.chCreateParams[i].defaultDynamicParams.mvAccuracy = 0;

        gVencModuleContext.encFormat[i] = encPrm.chCreateParams[i].format;
    }
    encPrm.inQueParams.prevLinkId = ipcInVideoId;
    encPrm.inQueParams.prevLinkQueId = 0;
    encPrm.outQueParams.nextLink = gVencModuleContext.ipcBitsOutRTOSId;

    /* IPC Bits Out VIDEO Link params */
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkId =
        gVencModuleContext.encId;
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcBitsOutVideoPrm.baseCreateParams.numOutQue = 1;
    ipcBitsOutVideoPrm.baseCreateParams.outQueParams[0].nextLink =
        gVencModuleContext.ipcBitsInHLOSId;
    MultiCh_ipcBitsInitCreateParams_BitsOutRTOS(&ipcBitsOutVideoPrm, TRUE);

    /* IPC Bits In HOST Link params */
    ipcBitsInHostPrm0.baseCreateParams.inQueParams.prevLinkId =
        gVencModuleContext.ipcBitsOutRTOSId;
    ipcBitsInHostPrm0.baseCreateParams.inQueParams.prevLinkQueId = 0;
    MultiCh_ipcBitsInitCreateParams_BitsInHLOS(&ipcBitsInHostPrm0);

    /* Links Creation */
    /* Camera Link */
    System_linkCreate(gVcamModuleContext.cameraId, &cameraPrm,
                      sizeof(cameraPrm));
    System_linkControl(gVcamModuleContext.cameraId,
                       CAMERA_LINK_CMD_DETECT_VIDEO, NULL, 0, TRUE);

    if(gUI_mcfw_config.glbceEnable)
    {
#ifdef YUV_M2M_DRV_TEST
#else
#ifndef WDR_ON
        System_linkCreate(glbceSupportId, &glbceSupportPrm,sizeof(glbceSupportPrm));
        /* glbce link create */
        System_linkCreate(glbceId, &glbcePrm,sizeof(glbcePrm));
#else
        System_linkCreate(wdrId, &wdrPrm,sizeof(wdrPrm));
#endif
#endif
        /* cam pp link create */
        System_linkCreate(gVcamModuleContext.ispId, &ispPrm,sizeof(ispPrm));
        gIsGlbceInitDone = 1;
    }
    else
    {
        gIsGlbceInitDone = 0;
    }

    System_linkCreate(dupId[VNF_DUP_IDX],&dupPrm[VNF_DUP_IDX],sizeof(dupPrm[VNF_DUP_IDX]));

#ifdef USE_MCTNF
    if(Vsys_getSystemUseCase() != VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
    #ifdef VNF_BEFORE_MCTNF
        System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
    #endif
    }

    System_linkCreate(ipcOutVpssIdForMCTNF, &ipcOutVpssIdForMCTNF_params, sizeof(ipcOutVpssIdForMCTNF_params));
    System_linkCreate(ipcInVideoIdForMCTNF, &ipcInVideoIdForMCTNF_params, sizeof(ipcInVideoIdForMCTNF_params));
    System_linkCreate(gVcamModuleContext.mctnfId, &mctnfPrm, sizeof(mctnfPrm));
    System_linkCreate(ipcOutVideoIdForMCTNF, &ipcOutVideoIdForMCTNF_params, sizeof(ipcOutVideoIdForMCTNF_params));
    System_linkCreate(ipcInVpssIdForMCTNF, &ipcInVpssIdForMCTNF_params, sizeof(ipcInVpssIdForMCTNF_params));

    if(Vsys_getSystemUseCase() != VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
    #ifndef VNF_BEFORE_MCTNF
        System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
    #endif
    }
#else
    #ifdef MEMORY_256MB
        if(gUI_mcfw_config.noisefilterMode == ISS_VNF_ON)
            System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
    #else
        System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
    #endif
#endif

#ifdef MEMORY_256MB
    if(gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        System_linkCreate(gVcamModuleContext.nsfId,&nsfPrm,sizeof(nsfPrm));
#else
    System_linkCreate(gVcamModuleContext.nsfId,&nsfPrm,sizeof(nsfPrm));
    System_linkCreate(gVsysModuleContext.muxIdVnf,&muxPrmVnf,sizeof(muxPrmVnf));
#endif

    /* Dup link */
    System_linkCreate(dupId[MJPEG_DUP_LINK_IDX], &dupPrm[MJPEG_DUP_LINK_IDX],
                      sizeof(dupPrm[MJPEG_DUP_LINK_IDX]));

    /* Dup Link for Scaler */
    System_linkCreate(dupId[SCALER_DUP_IDX], &dupPrm[SCALER_DUP_IDX],
                      sizeof(dupPrm[SCALER_DUP_IDX]));

#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    System_linkCreate(gVsysModuleContext.muxId, &muxPrm, sizeof(muxPrm));
#else
    /* Merge Link */
    System_linkCreate(mergeId[CAM_STREAM_MERGE_IDX],
                      &mergePrm[CAM_STREAM_MERGE_IDX],
                      sizeof(mergePrm[CAM_STREAM_MERGE_IDX]));
#endif
    /* SWOSD Link */
    System_linkCreate(gVsysModuleContext.swOsdId, &swosdPrm, sizeof(swosdPrm));

    /* Dup link for display */
    System_linkCreate(dupId[DIS_DUP_IDX], &dupPrm[DIS_DUP_IDX],
                      sizeof(dupPrm[DIS_DUP_IDX]));

    /* Scaler Link */
    System_linkCreate(gVcamModuleContext.sclrId[0], &sclrPrm, sizeof(sclrPrm));

    //System_linkCreate(SYSTEM_VPSS_LINK_ID_NULL_0, &nullPrm, sizeof(nullPrm));

    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        /* Dup link for VA */
        System_linkCreate(dupId[VA_DUP_IDX], &dupPrm[VA_DUP_IDX], sizeof(dupPrm[VA_DUP_IDX]));

#ifdef VA_ON_DSP
        /* IPC Frames links */
        System_linkCreate(ipcFramesOutVpssId, &ipcFramesOutVpssPrm, sizeof(ipcFramesOutVpssPrm));
        System_linkCreate(ipcFramesInDspId, &ipcFramesInDspPrm, sizeof(ipcFramesInDspPrm));
#endif

        /* VA link */
        System_linkCreate(gVsysModuleContext.vaId, &vaPrm, sizeof(vaPrm));
    }

    /* FD Link */
    System_linkCreate(gVsysModuleContext.fdId, &fdPrm, sizeof(fdPrm));

    /* IPC Links */
    System_linkCreate(ipcOutVpssId, &ipcOutVpssPrm, sizeof(ipcOutVpssPrm));
    System_linkCreate(ipcInVideoId, &ipcInVideoPrm, sizeof(ipcInVideoPrm));

    /* Video Encoder Link */
    System_linkCreate(gVencModuleContext.encId, &encPrm, sizeof(encPrm));

    /* VSTAB link */
    System_linkCreate(vstabId, &vstabPrm, sizeof(vstabPrm));

    /* IPC Bits Links */
    System_linkCreate(gVencModuleContext.ipcBitsOutRTOSId, &ipcBitsOutVideoPrm,
                      sizeof(ipcBitsOutVideoPrm));
    System_linkCreate(gVencModuleContext.ipcBitsInHLOSId, &ipcBitsInHostPrm0,
                      sizeof(ipcBitsInHostPrm0));

    /* display link */
    System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_HDMI], &displayPrm,
                      sizeof(displayPrm));

    System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_SD], &displayPrm_SD,
                      sizeof(displayPrm_SD));

    /*
     *  Scaler Dup should formward the frame on outQue 0 (outQueId)
     *  only when outQue 1 (syncOutQueId) has returned the frame back.
     */
    Vsys_setDupDelayPrm(dupId[SCALER_DUP_IDX],
                        0,                      // outQueId - should be a valid out queue Id.
                        1);                     // syncOutQueId - can be valid queue Id or
                                                //              - an invlaid queueId(DUP_LINK_DELAYED_OUTPUT_QUE_INVALID_ID)
                                                //              - In the latter case it waits on all the other output queues.

    gNoiseFilterMode = gUI_mcfw_config.noisefilterMode;

    OSA_printf("USECASE SETUP DONE\n");
}

/* ===================================================================
 *  @func     MultiCh_deleteTriStreamFullFtr
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
Void MultiCh_deleteTriStreamFullFtr()
{
    //UInt32 ispId;
    UInt32 glbceId;
    UInt32 mergeId[NUM_MERGE_LINK];
    UInt32 ipcOutVpssId;
    UInt32 ipcInVideoId;
    UInt32 ipcFramesOutVpssId;
    UInt32 ipcFramesInDspId;
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 vstabId;
    UInt32 glbceSupportId;
#ifdef WDR_ON
    UInt32 wdrId;
#endif

#ifdef USE_MCTNF
    UInt32 ipcOutVpssIdForMCTNF;
    UInt32 ipcInVideoIdForMCTNF;
    UInt32 ipcOutVideoIdForMCTNF;
    UInt32 ipcInVpssIdForMCTNF;
#endif

    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
    dupId[DIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[MJPEG_DUP_LINK_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    dupId[SCALER_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_2;
    dupId[VA_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_3;
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVsysModuleContext.fdId = SYSTEM_LINK_ID_FD;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    ipcFramesOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0;
    ipcFramesInDspId   = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;
    gVcamModuleContext.ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;
    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;

#ifdef WDR_ON
    wdrId = SYSTEM_LINK_ID_WDR;
#endif
#ifdef USE_MCTNF
    ipcOutVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1;
    ipcInVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1;
    ipcOutVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
    ipcInVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
#endif

    dupId[VNF_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_4;
    gVsysModuleContext.muxIdVnf = SYSTEM_VPSS_LINK_ID_MUX_1;

    /* Links Deletion in reverse order from sink to source */

    /* display link */
    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_HDMI]);

    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_SD]);

    /* IPC Bits Links */
    System_linkDelete(gVencModuleContext.ipcBitsOutRTOSId);
    System_linkDelete(gVencModuleContext.ipcBitsInHLOSId);

    /* VSTAB Link */
    System_linkDelete(vstabId);

    /* Video Encoder Link */
    System_linkDelete(gVencModuleContext.encId);

    /* IPC Links */
    System_linkDelete(ipcOutVpssId);
    System_linkDelete(ipcInVideoId);

    /* FD Link */
    System_linkDelete(gVsysModuleContext.fdId);

    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        /* VA link */
        System_linkDelete(gVsysModuleContext.vaId);

#ifdef VA_ON_DSP
        /* IPC Frames Links */
        System_linkDelete(ipcFramesOutVpssId);
        System_linkDelete(ipcFramesInDspId);
#endif

        /* Dup for VA link */
        System_linkDelete(dupId[VA_DUP_IDX]);
    }

    /* Scaler Link */
    System_linkDelete(gVcamModuleContext.sclrId[0]);

    /* dup link for display */
    System_linkDelete(dupId[DIS_DUP_IDX]);

    /* SWOSD Link */
    System_linkDelete(gVsysModuleContext.swOsdId);

#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    System_linkDelete(gVsysModuleContext.muxId);
#else
    /* Merge Link */
    System_linkDelete(mergeId[CAM_STREAM_MERGE_IDX]);
#endif

    /* Dup link for Scaler */
    System_linkDelete(dupId[SCALER_DUP_IDX]);

    /* Dup Link */
    System_linkDelete(dupId[MJPEG_DUP_LINK_IDX]);

#ifdef MEMORY_256MB
    if(gNoiseFilterMode == DSS_VNF_ON)
        System_linkDelete(gVcamModuleContext.nsfId);
#else
    System_linkDelete(gVsysModuleContext.muxIdVnf);
    System_linkDelete(gVcamModuleContext.nsfId);
#endif

#ifdef USE_MCTNF
    if(Vsys_getSystemUseCase() != VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
    #ifndef VNF_BEFORE_MCTNF
        System_linkDelete(gVcamModuleContext.vnfId);
    #endif
    }

    System_linkDelete(ipcInVpssIdForMCTNF);
    System_linkDelete(ipcOutVideoIdForMCTNF);
    System_linkDelete(gVcamModuleContext.mctnfId);
    System_linkDelete(ipcInVideoIdForMCTNF);
    System_linkDelete(ipcOutVpssIdForMCTNF);

    if(Vsys_getSystemUseCase() != VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
    #ifdef VNF_BEFORE_MCTNF
        System_linkDelete(gVcamModuleContext.vnfId);
    #endif
    }
#else
    #ifdef MEMORY_256MB
        if(gNoiseFilterMode == ISS_VNF_ON)
            System_linkDelete(gVcamModuleContext.vnfId);
    #else
        System_linkDelete(gVcamModuleContext.vnfId);
    #endif
#endif
    System_linkDelete(dupId[VNF_DUP_IDX]);

    if(gIsGlbceInitDone)
    {
#ifdef YUV_M2M_DRV_TEST
#else
#ifndef WDR_ON
        System_linkDelete(glbceId);
        System_linkDelete(glbceSupportId);
#else
        System_linkDelete(wdrId);
#endif
#endif
        System_linkDelete(gVcamModuleContext.ispId);
        gIsGlbceInitDone = 0;
    }

    /* Camera Link */
    System_linkDelete(gVcamModuleContext.cameraId);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    OSA_printf("USECASE TEARDOWN DONE\n");
}

