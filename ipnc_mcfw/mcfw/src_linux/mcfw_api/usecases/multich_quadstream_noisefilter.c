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

/*---------------------QUADRUPLE OUT <H264 1080P + D1 + CIF / 1080P MJPEG> -----------------

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
                            (1920x1080)      (320x240)
                            (420SP/422I)      (420SP) ------- FD(VPSSM3)
                              |      |
                       ISS NF ON    DSS_NF ON
                              |      |
                   ISS_NF(VPSSM3)   DSS_NF(VPSSM3)
                              |      |
                              |--|---|
                                 |         (0)
            |-------------DUP_JPEG(VPSSM3) ---|
            |                    | |(1)       |
      SCLR_D1(VPSSM3)   |------| | |----------|
            |           | |--| | | |
            |       (0) | |  | | | |
      DUP_SD(VPSSM3)----| | MERGE(VPSSM3)
            |(1)          |      |
            |             |      |
     SCLR_CIF(VPSSM3)-----| SWOSD(VPSSM3)
                                 |
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

/*in YUV sensor case, Enabling GLBCE means enabling Memory 2 Memory  */
#ifdef IMGS_OMNIVISION_OV10630
#define YUV_M2M_DRV_TEST
#endif
extern UInt32 gIsGlbceInitDone;
extern UInt32 gIsDiplayToBeDisabled;
extern GlbcePreset gGLBCE_AlgoPreset;

//#define USING_HD_SCALER

/* =============================================================================
 * Externs
 * ============================================================================= */

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
/* Merge Link for camera streams */
#define NUM_MERGE_LINK              (1)
#define CAM_STREAM_MERGE_IDX        (0)

/* Dup Link */
#define NUM_DUP_LINK                (3)
#define JPEG_DUP_IDX                (0)
#define SD_DUP_IDX                  (1)
#define HD_DUP_IDX                  (2)


/* Scaler Link */
#define NUM_SCALER_LINK             (3)
#define D1_SCALER_IDX               (0)
#define CIF_SCALER_IDX              (1)
#define HD_SCALER_IDX               (2)

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
Void MultiCh_createQuadStreamNf()
{
    UInt32 i;

    CameraLink_CreateParams cameraPrm;

    IspLink_CreateParams  ispPrm;
    UInt32 ispId;
    GlbceLink_CreateParams  glbcePrm;
    UInt32 glbceId;

    NsfLink_CreateParams nsfPrm;

    VnfLink_CreateParams vnfPrm;

    MergeLink_CreateParams mergePrm[NUM_MERGE_LINK];

    IpcLink_CreateParams ipcOutVpssPrm;

    IpcLink_CreateParams ipcInVideoPrm;

    EncLink_CreateParams encPrm;

    DupLink_CreateParams dupPrm[NUM_DUP_LINK];

    SclrLink_CreateParams sclrPrm[NUM_SCALER_LINK];

    DisplayLink_CreateParams displayPrm;

    DisplayLink_CreateParams displayPrm_SD;

    FdLink_CreateParams fdPrm;

    SwosdLink_CreateParams swosdPrm;

    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;


    IpcBitsInLinkHLOS_CreateParams ipcBitsInHostPrm0;

    VstabLink_CreateParams vstabPrm;

    GlbceSupportLink_CreateParams glbceSupportPrm;

    CameraLink_VipInstParams *pCameraInstPrm;

    CameraLink_OutParams *pCameraOutPrm;

    UInt32 mergeId[NUM_MERGE_LINK];

    UInt32 ipcOutVpssId;

    UInt32 ipcInVideoId;

    UInt32 dupId[NUM_DUP_LINK];

    UInt32 vstabId;

    UInt32 glbceSupportId;

    gIsDiplayToBeDisabled = 1;

    /* System init */
    //System_init();

    /* IPC struct init */
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcInVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkRTOS_CreateParams, ipcBitsOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkHLOS_CreateParams, ipcBitsInHostPrm0);

    OSA_printf("\n********** FULL FEATURE USECASE ********\n");
    OSA_printf
        ("********* Entered Quad Streaming usecase - H264 1080p @30fps + H264 D1 @30fps + H264 CIF @30fps + MJPEG 1080p @5fps ********\n\n");

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
    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
    dupId[JPEG_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[SD_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    dupId[HD_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_2;
    gVcamModuleContext.sclrId[D1_SCALER_IDX] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVcamModuleContext.sclrId[CIF_SCALER_IDX] = SYSTEM_LINK_ID_SCLR_INST_1;
    gVcamModuleContext.sclrId[HD_SCALER_IDX] = SYSTEM_LINK_ID_SCLR_INST_2;

    gVsysModuleContext.fdId = SYSTEM_LINK_ID_FD;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;

    ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;

    vstabId = SYSTEM_LINK_ID_VSTAB_0;

    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;

    /* display IDs */
    gVdisModuleContext.displayId[0] = SYSTEM_LINK_ID_DISPLAY_0; /* ON AND OFF
                                                                 * CHIP HDMI */
    gVdisModuleContext.displayId[VDIS_DEV_SD] = SYSTEM_LINK_ID_DISPLAY_2;

    /* IPC frame Link IDs */
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;

    /* IPC bits link IDs */
    gVencModuleContext.ipcBitsOutRTOSId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gVencModuleContext.ipcBitsInHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;


    if(gUI_mcfw_config.glbceEnable)
    {
        gVcamModuleContext.glbceId = glbceId;

      /* Camera Link params */
        CameraLink_CreateParams_Init(&cameraPrm);
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            gVcamModuleContext.nsfId = SYSTEM_LINK_ID_NSF_0;
        }
        else
        {
            gVcamModuleContext.vnfId = SYSTEM_LINK_ID_VNF;
            cameraPrm.vnfLinkId = SYSTEM_LINK_ID_VNF;
        }

        cameraPrm.captureMode = CAMERA_LINK_CAPMODE_DDR;
#ifdef YUV_M2M_DRV_TEST
        cameraPrm.outQueParams[0].nextLink = ispId;
#else
        cameraPrm.outQueParams[0].nextLink = glbceId;
#endif
        cameraPrm.ispLinkId = ispId;

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
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV422I_UYVY;
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#elif defined IMGS_SONY_IMX104
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
    else
    {
        gVcamModuleContext.glbceId = SYSTEM_LINK_ID_INVALID;
        /* Camera Link params */
        CameraLink_CreateParams_Init(&cameraPrm);

        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            gVcamModuleContext.nsfId = SYSTEM_LINK_ID_NSF_0;
        }
        else
        {
            gVcamModuleContext.vnfId = SYSTEM_LINK_ID_VNF;
            cameraPrm.vnfLinkId = SYSTEM_LINK_ID_VNF;
        }

        cameraPrm.captureMode = CAMERA_LINK_CAPMODE_ISIF;
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
            cameraPrm.outQueParams[0].nextLink  = gVcamModuleContext.nsfId;
        else
            cameraPrm.outQueParams[0].nextLink  = gVcamModuleContext.vnfId;

        cameraPrm.outQueParams[1].nextLink = gVsysModuleContext.fdId;

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

        pCameraInstPrm->numOutput = 2;

        /* First stream */
        pCameraOutPrm = &pCameraInstPrm->outParams[0];
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            pCameraOutPrm->dataFormat = SYSTEM_DF_YUV422I_UYVY;
        }
        else
        {
#ifdef USE_TNF3_FILTER
            pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
#else
            pCameraOutPrm->dataFormat = SYSTEM_DF_YUV422I_UYVY;
#endif
        }

        pCameraOutPrm->scEnable = FALSE;

#ifdef IMGS_OMNIVISION_OV10630
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#elif defined IMGS_SONY_IMX104
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
            pCameraOutPrm->scOutWidth = 1600;
            pCameraOutPrm->scOutHeight = 900;
        }
#endif

        pCameraOutPrm->outQueId = 0;

        /* Second stream */
        pCameraOutPrm = &pCameraInstPrm->outParams[1];
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
        pCameraOutPrm->scEnable = FALSE;
        pCameraOutPrm->scOutWidth = 320;
        pCameraOutPrm->scOutHeight = 240;
        pCameraOutPrm->standard = SYSTEM_STD_NTSC;//SYSTEM_STD_PAL;
        pCameraOutPrm->outQueId = 1;

        /* 2A config */
        cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
        cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;
    }

    /* vstab Link params */
    vstabPrm.totalFrameWidth  = pCameraInstPrm->sensorOutWidth;
    vstabPrm.totalFrameHeight = pCameraInstPrm->sensorOutHeight;

    glbceSupportPrm.glbceLinkID = glbceId;

    if(gUI_mcfw_config.glbceEnable)
    {
        cameraPrm.glbceLinkId = glbceId;
        /* Glbce Link Params */
        glbcePrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
        glbcePrm.inQueParams.prevLinkQueId = 0;
        glbcePrm.outQueParams.nextLink = ispId;
        glbcePrm.chCreateParams[0].preset  = 0;//gGLBCE_AlgoPreset;
        glbcePrm.glbceEnable = gUI_mcfw_config.glbceEnable;
        glbcePrm.glbceSupportID = glbceSupportId;
        glbcePrm.swosdLinkID = gVsysModuleContext.swOsdId;

        /* isp link params */
#ifdef YUV_M2M_DRV_TEST
        ispPrm.inQueParams.prevLinkId    = gVcamModuleContext.cameraId;
#else
        ispPrm.inQueParams.prevLinkId    = glbceId;
#endif
        ispPrm.inQueParams.prevLinkQueId = 0;
        ispPrm.numOutQueue               = 2;

        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
            ispPrm.outQueInfo[0].nextLink  = gVcamModuleContext.nsfId;
        else
            ispPrm.outQueInfo[0].nextLink  = gVcamModuleContext.vnfId;

        ispPrm.outQueInfo[1].nextLink    = gVsysModuleContext.fdId;

        #if defined(TI_8107_BUILD)
            ispPrm.clkDivM                   = 10;
            ispPrm.clkDivN                   = 50;//30;
        #else
            ispPrm.clkDivM                   = 10;
            ispPrm.clkDivN                   = 20;
        #endif

        ispPrm.vsEnable = gUI_mcfw_config.vsEnable;

        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            ispPrm.outQueuePrm[0].dataFormat = SYSTEM_DF_YUV422I_UYVY;
        }
        else
        {
#ifdef USE_TNF3_FILTER
            ispPrm.outQueuePrm[0].dataFormat = SYSTEM_DF_YUV420SP_UV;
#else
            ispPrm.outQueuePrm[0].dataFormat = SYSTEM_DF_YUV422I_UYVY;
#endif
        }

#ifdef IMGS_OMNIVISION_OV10630
        ispPrm.outQueuePrm[0].width      = 1280;
        ispPrm.outQueuePrm[0].height     = 720;
 #elif defined IMGS_SONY_IMX104
            ispPrm.outQueuePrm[0].width      = 1280;
            ispPrm.outQueuePrm[0].height     = 720;
#else
        if(ispPrm.vsEnable == TRUE)
        {
            ispPrm.outQueuePrm[0].width      = 1600;
            ispPrm.outQueuePrm[0].height     = 900;
        }
        else
        {
            ispPrm.outQueuePrm[0].width      = 1920;
            ispPrm.outQueuePrm[0].height     = 1080;
        }
#endif

        ispPrm.outQueuePrm[1].dataFormat = SYSTEM_DF_YUV420SP_UV;
        ispPrm.outQueuePrm[1].width      = 320;
        ispPrm.outQueuePrm[1].height     = 240;
        ispPrm.outQueuePrm[1].standard   = SYSTEM_STD_NTSC;//SYSTEM_STD_PAL;//SYSTEM_STD_NTSC;
    }

    ispPrm.vnfFullResolution = FALSE;
    cameraPrm.vnfFullResolution = FALSE;

    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        /* NSF Link params */
        NsfLink_CreateParams_Init(&nsfPrm);
        if (gUI_mcfw_config.snfEnable)
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
        if(gUI_mcfw_config.glbceEnable)
            nsfPrm.inQueParams.prevLinkId = ispId;
        else
            nsfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;

        nsfPrm.inQueParams.prevLinkQueId = 0;
        nsfPrm.numOutQue = 1;

#ifdef MEMORY_256MB
        nsfPrm.numBufsPerCh = 6;
#else
        /*
         *  More than 6 output buffers are neede to realize 60 fps at encoder input
         */
        if(pCameraInstPrm->standard == SYSTEM_STD_1080P_60)
            nsfPrm.numBufsPerCh = 8;
        else
            nsfPrm.numBufsPerCh = 6;
#endif

        nsfPrm.inputFrameRate = 60;
        nsfPrm.outputFrameRate = 60;
        nsfPrm.outQueParams[0].nextLink = dupId[JPEG_DUP_IDX];
        nsfPrm.nsfStrength = gUI_mcfw_config.vnfStrength;
    }
    else
    {
        VnfLink_ChCreateParams *vnfParams;

        /* Vnf Link Params */
        VnfLink_CreateParams_Init(&vnfPrm);

        if(gUI_mcfw_config.glbceEnable)
            vnfPrm.inQueParams.prevLinkId = ispId;
        else
            vnfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;

        vnfPrm.inQueParams.prevLinkQueId = 0;
        vnfPrm.outQueParams.nextLink = dupId[JPEG_DUP_IDX];

        vnfParams = &vnfPrm.chCreateParams[0];
        vnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;
        if (gUI_mcfw_config.ldcEnable)
            vnfParams->sParams.eOperateMode = VNF_LINK_LDC_AFFINE_NSF_TNF;
        else
        {
#ifdef USE_TNF3_FILTER
            vnfParams->sParams.eOperateMode =  VNF_LINK_TNF3;
#else
            vnfParams->sParams.eOperateMode = VNF_LINK_AFFINE_NSF_TNF;
#endif
            vnfParams->sParams.eNsfSet = VNF_LINK_NSF_LUMA_CHROMA;
        }

        if ((vnfParams->sParams.eOperateMode == VNF_LINK_TNF3) && (vnfParams->sParams.eNsfSet == VNF_LINK_NSF_LUMA_CHROMA))
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

        vnfParams->dParams.ldcParams.eInterpolationLuma =
            VNF_LINK_YINTERPOLATION_BILINEAR;
        vnfParams->dParams.ldcParams.unPixelPad = 4;

        vnfParams->dParams.nsfParams.bLumaEn = TRUE;
        vnfParams->dParams.nsfParams.bChromaEn = TRUE;
        vnfParams->dParams.nsfParams.eSmoothVal = VNF_LINK_SMOOTH_0;
        vnfParams->dParams.nsfParams.bSmoothLumaEn = TRUE;
        vnfParams->dParams.nsfParams.bSmoothChromaEn = TRUE;

        vnfParams->dParams.tnfParams.useDefault = TRUE;

        vnfParams->dParams.nsfFilter.eFilterParam = VNF_LINK_PARAM_DEFAULT;
        vnfParams->dParams.nsfEdge.eEdgeParam = VNF_LINK_PARAM_DISABLE;
        vnfParams->dParams.nsfShading.eShdParam = VNF_LINK_PARAM_DISABLE;
        vnfParams->dParams.nsfDesaturation.eDesatParam = VNF_LINK_PARAM_DISABLE;

        vnfPrm.chCreateParams[0].bSnfEnable = gUI_mcfw_config.snfEnable;
        vnfPrm.chCreateParams[0].bTnfEnable = gUI_mcfw_config.tnfEnable;
        vnfPrm.chCreateParams[0].strength   = gUI_mcfw_config.vnfStrength;
    }

    /* HD dup link params */
    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        dupPrm[JPEG_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.nsfId;
    else
        dupPrm[JPEG_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.vnfId;
    dupPrm[JPEG_DUP_IDX].inQueParams.prevLinkQueId = 0;

    /* full resolution MJPEG */
    dupPrm[JPEG_DUP_IDX].outQueParams[0].nextLink = mergeId[CAM_STREAM_MERGE_IDX];
    /* 1080P H264, add a scaler link for synamic resolution change */
#ifdef USING_HD_SCALER
    dupPrm[JPEG_DUP_IDX].numOutQue = 2;
    dupPrm[JPEG_DUP_IDX].outQueParams[1].nextLink = gVcamModuleContext.sclrId[HD_SCALER_IDX];
    if (gIsDiplayToBeDisabled == 0)
    {
        dupPrm[JPEG_DUP_IDX].numOutQue = 3;
        dupPrm[JPEG_DUP_IDX].outQueParams[2].nextLink = gVdisModuleContext.displayId[0];
    }
#else
    dupPrm[JPEG_DUP_IDX].numOutQue = 3;
    dupPrm[JPEG_DUP_IDX].outQueParams[1].nextLink = mergeId[CAM_STREAM_MERGE_IDX];
    dupPrm[JPEG_DUP_IDX].outQueParams[2].nextLink = gVcamModuleContext.sclrId[D1_SCALER_IDX];
    if (gIsDiplayToBeDisabled == 0)
    {
        dupPrm[JPEG_DUP_IDX].numOutQue = 4;
        dupPrm[JPEG_DUP_IDX].outQueParams[3].nextLink = gVdisModuleContext.displayId[0];
    }
#endif

    dupPrm[JPEG_DUP_IDX].notifyNextLink = TRUE;

#ifdef USING_HD_SCALER
    SclrLink_CreateParams_Init(&sclrPrm[HD_SCALER_IDX]);
    sclrPrm[HD_SCALER_IDX].inQueParams.prevLinkId             = dupId[JPEG_DUP_IDX];
    sclrPrm[HD_SCALER_IDX].inQueParams.prevLinkQueId          = 1;
    sclrPrm[HD_SCALER_IDX].outQueParams.nextLink              = dupId[HD_DUP_IDX];
    sclrPrm[HD_SCALER_IDX].tilerEnable                        = FALSE;
    sclrPrm[HD_SCALER_IDX].enableLineSkipSc                   = FALSE;
    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        sclrPrm[HD_SCALER_IDX].inputFrameRate                 = 60;
        sclrPrm[HD_SCALER_IDX].outputFrameRate                = 60;
    }
    else
    {
        sclrPrm[HD_SCALER_IDX].inputFrameRate                 = 30;
        sclrPrm[HD_SCALER_IDX].outputFrameRate                = 30;
    }
    sclrPrm[HD_SCALER_IDX].scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm[HD_SCALER_IDX].scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outWidth   = 1920;
    sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outHeight  = 1080;
    if (gUI_mcfw_config.noisefilterMode == ISS_VNF_ON)
    {
        switch (vnfPrm.chCreateParams[0].sParams.eOperateMode)
        {
            case VNF_LINK_LDC_AFFINE_NSF_TNF:
            case VNF_LINK_AFFINE_NSF_TNF:
                sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outWidth   = 1888;
                sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outHeight  = 1048;
                break;
            case VNF_LINK_3DNF:
                if (vnfPrm.chCreateParams[0].sParams.eNsfSet != VNF_LINK_NSF_DISABLE)
                {
                    sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outWidth   = 1888;
                    sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outHeight  = 1048;
                }
                break;
            case VNF_LINK_TNF3:
                if (vnfPrm.chCreateParams[0].sParams.eNsfSet != VNF_LINK_NSF_CHROMA_ONLY)
                {
                    sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outWidth   = 1888;
                    sclrPrm[HD_SCALER_IDX].outScaleFactor.absoluteResolution.outHeight  = 1048;
                }
            default:
                break;
        }
    }

    sclrPrm[HD_SCALER_IDX].outDataFormat = VF_YUV420SP_UV;
    sclrPrm[HD_SCALER_IDX].pathId = SCLR_LINK_SEC0_SC3;

    dupPrm[HD_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.sclrId[HD_SCALER_IDX];
    dupPrm[HD_DUP_IDX].inQueParams.prevLinkQueId = 0;
    dupPrm[HD_DUP_IDX].numOutQue = 2;
    dupPrm[HD_DUP_IDX].outQueParams[0].nextLink =
        mergeId[CAM_STREAM_MERGE_IDX];
    dupPrm[HD_DUP_IDX].outQueParams[1].nextLink = gVcamModuleContext.sclrId[D1_SCALER_IDX];
    dupPrm[HD_DUP_IDX].notifyNextLink = TRUE;
#endif

    /* display link params */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
    displayPrm.inQueParams[0].prevLinkId = dupId[JPEG_DUP_IDX];
#ifdef USING_HD_SCALER
    displayPrm.inQueParams[0].prevLinkQueId = 2;
#else
    displayPrm.inQueParams[0].prevLinkQueId = 3;
#endif
    displayPrm.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    displayPrm.displayId  = DISPLAY_LINK_DISPLAY_SC2;

    /* Scaler Link Params */
    SclrLink_CreateParams_Init(&sclrPrm[D1_SCALER_IDX]);
#ifdef USING_HD_SCALER
    sclrPrm[D1_SCALER_IDX].inQueParams.prevLinkId             = dupId[HD_DUP_IDX];
    sclrPrm[D1_SCALER_IDX].inQueParams.prevLinkQueId          = 1;
#else
    sclrPrm[D1_SCALER_IDX].inQueParams.prevLinkId             = dupId[JPEG_DUP_IDX];
    sclrPrm[D1_SCALER_IDX].inQueParams.prevLinkQueId          = 2;
#endif
    sclrPrm[D1_SCALER_IDX].outQueParams.nextLink              = dupId[SD_DUP_IDX];
    sclrPrm[D1_SCALER_IDX].tilerEnable                        = FALSE;
    sclrPrm[D1_SCALER_IDX].enableLineSkipSc                   = FALSE;
    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        sclrPrm[D1_SCALER_IDX].inputFrameRate                 = 60;
    }
    else
    {
        sclrPrm[D1_SCALER_IDX].inputFrameRate                 = 30;
    }
    sclrPrm[D1_SCALER_IDX].outputFrameRate                    = 30;
    sclrPrm[D1_SCALER_IDX].scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm[D1_SCALER_IDX].scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm[D1_SCALER_IDX].outScaleFactor.absoluteResolution.outWidth   = 720;
    sclrPrm[D1_SCALER_IDX].outScaleFactor.absoluteResolution.outHeight  = 480;
    sclrPrm[D1_SCALER_IDX].outDataFormat = VF_YUV420SP_UV;
    sclrPrm[D1_SCALER_IDX].pathId = SCLR_LINK_SEC0_SC3;

    dupPrm[SD_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.sclrId[D1_SCALER_IDX];
    dupPrm[SD_DUP_IDX].inQueParams.prevLinkQueId = 0;
    dupPrm[SD_DUP_IDX].numOutQue = 3;
    dupPrm[SD_DUP_IDX].outQueParams[0].nextLink =
        mergeId[CAM_STREAM_MERGE_IDX];
    dupPrm[SD_DUP_IDX].outQueParams[1].nextLink = gVcamModuleContext.sclrId[CIF_SCALER_IDX];
    dupPrm[SD_DUP_IDX].outQueParams[2].nextLink =
        gVdisModuleContext.displayId[VDIS_DEV_SD];
    dupPrm[SD_DUP_IDX].notifyNextLink = TRUE;

    SclrLink_CreateParams_Init(&sclrPrm[CIF_SCALER_IDX]);
    sclrPrm[CIF_SCALER_IDX].inQueParams.prevLinkId             = dupId[SD_DUP_IDX];
    sclrPrm[CIF_SCALER_IDX].inQueParams.prevLinkQueId          = 1;
    sclrPrm[CIF_SCALER_IDX].outQueParams.nextLink              = mergeId[CAM_STREAM_MERGE_IDX];
    sclrPrm[CIF_SCALER_IDX].tilerEnable                        = FALSE;
    sclrPrm[CIF_SCALER_IDX].enableLineSkipSc                   = FALSE;
    sclrPrm[CIF_SCALER_IDX].inputFrameRate                     = 30;
    sclrPrm[CIF_SCALER_IDX].outputFrameRate                    = 30;
    sclrPrm[CIF_SCALER_IDX].scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm[CIF_SCALER_IDX].scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm[CIF_SCALER_IDX].outScaleFactor.absoluteResolution.outWidth   = 352;
    sclrPrm[CIF_SCALER_IDX].outScaleFactor.absoluteResolution.outHeight  = 288;
    sclrPrm[CIF_SCALER_IDX].outDataFormat = VF_YUV420SP_UV;
    sclrPrm[CIF_SCALER_IDX].pathId = SCLR_LINK_SEC0_SC3;

    /* display link params */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm_SD);
    displayPrm_SD.numInputQueues = 1;
    displayPrm_SD.activeQueue = 0;
    displayPrm_SD.inQueParams[0].prevLinkId = dupId[SD_DUP_IDX];
    displayPrm_SD.inQueParams[0].prevLinkQueId = 2;
    displayPrm_SD.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_SD].resolution;
    displayPrm_SD.displayId  = DISPLAY_LINK_DISPLAY_SD;

    /* Merge Link params */
    mergePrm[CAM_STREAM_MERGE_IDX].numInQue = 4;
#ifdef USING_HD_SCALER
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkId = dupId[HD_DUP_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkQueId = 0;
#else
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkId =
        dupId[JPEG_DUP_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkQueId = 1;
#endif
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkId =
        dupId[SD_DUP_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkQueId = 0;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[2].prevLinkId =
        gVcamModuleContext.sclrId[CIF_SCALER_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[2].prevLinkQueId = 0;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[3].prevLinkId =
        dupId[JPEG_DUP_IDX];
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[3].prevLinkQueId = 0;
    mergePrm[CAM_STREAM_MERGE_IDX].outQueParams.nextLink =
        gVsysModuleContext.swOsdId;
    mergePrm[CAM_STREAM_MERGE_IDX].notifyNextLink = TRUE;

    /* SWOSD Link Params */
    swosdPrm.inQueParams.prevLinkId = mergeId[CAM_STREAM_MERGE_IDX];
    swosdPrm.inQueParams.prevLinkQueId = 0;
    swosdPrm.outQueParams.nextLink = ipcOutVpssId;

    /* FD Link Params */
    if(gUI_mcfw_config.glbceEnable)
        fdPrm.inQueParams.prevLinkId = ispId;
    else
        fdPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
    fdPrm.inQueParams.prevLinkQueId = 1;

    /* IPC Out VPSS link params */
    ipcOutVpssPrm.inQueParams.prevLinkId = gVsysModuleContext.swOsdId;
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

        for (i = 0; i < VENC_CHN_MAX; i++)
        {
            if (i == VENC_PRIMARY_CHANNELS)
                continue;

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
#else
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 60;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#endif
    encPrm.chCreateParams[1].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
    encPrm.chCreateParams[2].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;

    encPrm.vsEnable = gUI_mcfw_config.vsEnable;

    if(gUI_mcfw_config.vaUseCase == TRUE)
    {
        encPrm.isVaUseCase = 1;
    }
    else
    {
        encPrm.isVaUseCase = 0;
    }

    for (i = VENC_PRIMARY_CHANNELS; i < VENC_CHN_MAX - 1; i++)
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

    //MultiCh_displayCtrlInit(&gVdisModuleContext.vdisConfig);

    /* Links Creation */
    /* Camera Link */
    System_linkCreate(gVcamModuleContext.cameraId, &cameraPrm,
                      sizeof(cameraPrm));
    System_linkControl(gVcamModuleContext.cameraId,
                       CAMERA_LINK_CMD_DETECT_VIDEO, NULL, 0, TRUE);

    if(gUI_mcfw_config.glbceEnable)
    {

        System_linkCreate(glbceSupportId, &glbceSupportPrm,sizeof(glbceSupportPrm));
        /* glbce link create */
#ifndef YUV_M2M_DRV_TEST
        System_linkCreate(glbceId, &glbcePrm,sizeof(glbcePrm));
#endif
        /* cam pp link create */
        System_linkCreate(ispId, &ispPrm,sizeof(ispPrm));
        gIsGlbceInitDone = 1;
    }
    else
    {
        gIsGlbceInitDone = 0;
    }

    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        /* NSF Link */
        System_linkCreate(gVcamModuleContext.nsfId, &nsfPrm, sizeof(nsfPrm));
    else
        /* Vnf Link */
        System_linkCreate(gVcamModuleContext.vnfId, &vnfPrm, sizeof(vnfPrm));

    /* Dup link */
    System_linkCreate(dupId[JPEG_DUP_IDX], &dupPrm[JPEG_DUP_IDX],
                      sizeof(dupPrm[JPEG_DUP_IDX]));

#ifdef USING_HD_SCALER
    System_linkCreate(gVcamModuleContext.sclrId[HD_SCALER_IDX], &sclrPrm[HD_SCALER_IDX],
        sizeof(sclrPrm[HD_SCALER_IDX]));

    System_linkCreate(dupId[HD_DUP_IDX], &dupPrm[HD_DUP_IDX],
                      sizeof(dupPrm[HD_DUP_IDX]));
#endif

    System_linkCreate(gVcamModuleContext.sclrId[D1_SCALER_IDX], &sclrPrm[D1_SCALER_IDX],
        sizeof(sclrPrm[D1_SCALER_IDX]));

    System_linkCreate(dupId[SD_DUP_IDX], &dupPrm[SD_DUP_IDX],
                      sizeof(dupPrm[SD_DUP_IDX]));

    System_linkCreate(gVcamModuleContext.sclrId[CIF_SCALER_IDX], &sclrPrm[CIF_SCALER_IDX],
        sizeof(sclrPrm[CIF_SCALER_IDX]));

    /* Merge Link */
    System_linkCreate(mergeId[CAM_STREAM_MERGE_IDX],
                      &mergePrm[CAM_STREAM_MERGE_IDX],
                      sizeof(mergePrm[CAM_STREAM_MERGE_IDX]));

    /* SWOSD Link */
    System_linkCreate(gVsysModuleContext.swOsdId, &swosdPrm, sizeof(swosdPrm));

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
    if(gIsDiplayToBeDisabled == 0)
        System_linkCreate(gVdisModuleContext.displayId[0], &displayPrm,
                      sizeof(displayPrm));

    System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_SD], &displayPrm_SD,
                      sizeof(displayPrm_SD));
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
Void MultiCh_deleteQuadStreamNf()
{
    UInt32 ispId;
    UInt32 glbceId;

    UInt32 mergeId[NUM_MERGE_LINK];

    UInt32 ipcOutVpssId;

    UInt32 ipcInVideoId;

    UInt32 dupId[NUM_DUP_LINK];

    UInt32 vstabId;
    UInt32 glbceSupportId;

    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
    dupId[JPEG_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[SD_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    dupId[HD_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_2;

    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;

    ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;

    vstabId = SYSTEM_LINK_ID_VSTAB_0;

    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;

    /* Links Deletion in reverse order from sink to source */

    /* display link */
    if(gIsDiplayToBeDisabled == 0)
        System_linkDelete(gVdisModuleContext.displayId[0]);
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

    /* Scaler Link */
    System_linkDelete(gVcamModuleContext.sclrId[D1_SCALER_IDX]);
    System_linkDelete(gVcamModuleContext.sclrId[CIF_SCALER_IDX]);
#ifdef USING_HD_SCALER
    System_linkDelete(gVcamModuleContext.sclrId[HD_SCALER_IDX]);
#endif

    /* SWOSD Link */
    System_linkDelete(gVsysModuleContext.swOsdId);

    /* Merge Link */
    System_linkDelete(mergeId[CAM_STREAM_MERGE_IDX]);

    /* Dup Link */
    System_linkDelete(dupId[JPEG_DUP_IDX]);
    System_linkDelete(dupId[SD_DUP_IDX]);
#ifdef USING_HD_SCALER
    System_linkDelete(dupId[HD_DUP_IDX]);
#endif

    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        /* NSF Link */
        System_linkDelete(gVcamModuleContext.nsfId);
    else
        /* VNF Link */
        System_linkDelete(gVcamModuleContext.vnfId);

    //gIsDiplayToBeDisabled = 0;


    if(gIsGlbceInitDone)
    {
        System_linkDelete(ispId);
#ifndef YUV_M2M_DRV_TEST
        System_linkDelete(glbceId);
        System_linkDelete(glbceSupportId);
#endif
        gIsGlbceInitDone = 0;
    }

    /* Camera Link */
    System_linkDelete(gVcamModuleContext.cameraId);

    //MultiCh_displayCtrlDeInit(&gVdisModuleContext.vdisConfig);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    //System_deInit();

    OSA_printf("USECASE TEARDOWN DONE\n");
}

