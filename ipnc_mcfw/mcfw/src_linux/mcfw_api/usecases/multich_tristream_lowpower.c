/** ==================================================================
 *  @file   multich_tristream_lowPower.c
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
            |---SALDRE/WDR ON---|                |
            |                   |   CAMERA LINK  |
            |                   |____(VPSSM3)____|
    GLBCE_LINK/WDR(VPSSM3)         |        |
            |                      |        |
            |  _________________ SALDRE/WDR OFF
            |  |               |   |        |
            |--|    ISP LINK   |   |        |
               |____(VPSSM3)___|   |        |
                   |        |      |        |
                   |        |      |        |
                   |        |------|------| |
                   |------------ RSZA     RSZB
                             (1920x1080) (720x480)
                               (420SP)    (420SP)
                                  |         |
                       ___________|         |
                       |                    |
                DUP(VPSSM3)(0)----|         |
                      (1)         -----------
                       | ------------ |
                                      |
                                 MERGE(VPSSM3)
                                      |
                                      |
                                 SWOSD(VPSSM3)
                                      |
                                      |
                               IPC OUT(VPSSM3)
                                      |
                                      |
                                IPC IN(VIDEOM3)
                                      | --------------- |
                                      |                 |
                              H264ENC(VIDEOM3)   MJPEGENC(VIDEOM3)
                                      |                 |
                                      | --------------- |
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


extern UInt32 gIsGlbceInitDone;
extern UInt32 gIsDiplayToBeDisabled;
extern GlbcePreset gGLBCE_AlgoPreset;
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

/* ===================================================================
 *  @func     MultiCh_createTriStreamLowPwr
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
Void MultiCh_createTriStreamLowPwr()
{
    UInt32 i;

    CameraLink_CreateParams cameraPrm;

    IspLink_CreateParams  ispPrm;
    UInt32 ispId;
    GlbceLink_CreateParams  glbcePrm;
    UInt32 glbceId;

    // MjpegLink_CreateParams simcopPrm;

    MergeLink_CreateParams mergePrm[NUM_MERGE_LINK];

    IpcLink_CreateParams ipcOutVpssPrm;

    IpcLink_CreateParams ipcInVideoPrm;

    EncLink_CreateParams encPrm;

    DupLink_CreateParams dupPrm;

    SwosdLink_CreateParams swosdPrm;

    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;

    // IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVpssPrm;
    IpcBitsInLinkHLOS_CreateParams ipcBitsInHostPrm0;

    // IpcBitsInLinkHLOS_CreateParams ipcBitsInHostPrm1;

    VstabLink_CreateParams vstabPrm;
    GlbceSupportLink_CreateParams glbceSupportPrm;
    CameraLink_VipInstParams *pCameraInstPrm;

    CameraLink_OutParams *pCameraOutPrm;

    // System_LinkInfo bitsProducerLinkInfo;

    UInt32 mergeId[NUM_MERGE_LINK];

    UInt32 dupId;

    // UInt32 mjpepEncId;
    UInt32 ipcOutVpssId;

    UInt32 ipcInVideoId;

    // UInt32 swosdId;

    UInt32 vstabId;
    UInt32 glbceSupportId;

#ifdef WDR_ON
    WdrLink_CreateParams  wdrPrm;
    UInt32 wdrId;
#endif

    /* IPC struct init */
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcInVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkRTOS_CreateParams, ipcBitsOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkHLOS_CreateParams, ipcBitsInHostPrm0);

    OSA_printf("\n********** LOW POWER USECASE ********\n");
    OSA_printf
        ("********* Entered Tri Streaming usecase - H264 1080p @60fps + H264 D1 @30fps + MJPEG 1080p @5fps ********\n\n");

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
    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
    dupId = SYSTEM_VPSS_LINK_ID_DUP_0;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;

    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;
#ifdef WDR_ON
    wdrId = SYSTEM_LINK_ID_WDR;
#endif

    /* IPC frame Link IDs */
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;

    /* IPC bits link IDs */
    gVencModuleContext.ipcBitsOutRTOSId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gVencModuleContext.ipcBitsInHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;

    ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;

    if(gUI_mcfw_config.glbceEnable)
    {
        /* Camera Link params */
        CameraLink_CreateParams_Init(&cameraPrm);
        cameraPrm.captureMode = CAMERA_LINK_CAPMODE_DDR;
        cameraPrm.outQueParams[0].nextLink = glbceId;
        cameraPrm.ispLinkId = ispId;
#ifdef WDR_ON
        cameraPrm.outQueParams[0].nextLink = wdrId;
#endif

        gVcamModuleContext.glbceId = glbceId;
        gVcamModuleContext.ispId = ispId;
#ifdef WDR_ON
        gVcamModuleContext.glbceId = wdrId;
#endif

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
#elif defined IMGS_MICRON_MT9M034
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 736;
#else
        switch(gUI_mcfw_config.demoCfg.resolution_combo)
        {
            case RES_1080P_D1   :   pCameraInstPrm->sensorOutWidth = 1920;
                                    pCameraInstPrm->sensorOutHeight = 1080;
                                    break;
            case RES_1080P_1080P:   pCameraInstPrm->sensorOutWidth = 1920;
                                    pCameraInstPrm->sensorOutHeight = 1080;
                                    break;
            case RES_3MP_D1     :   pCameraInstPrm->sensorOutWidth = 2048;
                                    pCameraInstPrm->sensorOutHeight = 1536;
                                    break;
            case RES_5MP_D1     :   pCameraInstPrm->sensorOutWidth = 2560;
                                    pCameraInstPrm->sensorOutHeight = 1920;
                                    break;
            case RES_8MP_D1     :   pCameraInstPrm->sensorOutWidth = 3264;
                                    pCameraInstPrm->sensorOutHeight = 2448;
                                    break;
            case RES_10MP_D1    :   pCameraInstPrm->sensorOutWidth = 3648;
                                    pCameraInstPrm->sensorOutHeight = 2736;
                                    break;
            case RES_960P_D1    :   pCameraInstPrm->sensorOutWidth = 1280;
                                    pCameraInstPrm->sensorOutHeight = 960;
                                    break;
            default             :   pCameraInstPrm->sensorOutWidth = 1920;
                                    pCameraInstPrm->sensorOutHeight = 1080;
                                    break;
        }
#endif

        switch(gUI_mcfw_config.demoCfg.resolution_combo)
        {
            case RES_1080P_D1 :     pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
                                    break;
            case RES_1080P_1080P:   pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
                                    break;
            case RES_3MP_D1   :     pCameraInstPrm->standard = SYSTEM_STD_3MP_2048_1536;
                                    break;
            case RES_5MP_D1   :     pCameraInstPrm->standard = SYSTEM_STD_5MP_2560_1920;
                                    break;
            case RES_8MP_D1   :     pCameraInstPrm->standard = SYSTEM_STD_8MP_3264_2448;
                                    break;
            case RES_10MP_D1  :     pCameraInstPrm->standard = SYSTEM_STD_10MP_3648_2736;
                                    break;
            case RES_960P_D1  :     pCameraInstPrm->standard = SYSTEM_STD_960P_30;
                                    break;
            default           :     pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
                                    break;
        }

        pCameraInstPrm->numOutput = 1;

        pCameraOutPrm = &pCameraInstPrm->outParams[0];
        pCameraOutPrm->scEnable = FALSE;

#ifdef IMGS_OMNIVISION_OV10630
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV422I_UYVY;
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#elif defined IMGS_MICRON_MT9M034
        pCameraOutPrm->dataFormat = SYSTEM_DF_BAYER_RAW;
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
#else
        pCameraOutPrm->dataFormat = SYSTEM_DF_BAYER_RAW;
        switch(gUI_mcfw_config.demoCfg.resolution_combo)
        {
            case RES_1080P_D1   :   pCameraOutPrm->scOutWidth = 1920;
                                    pCameraOutPrm->scOutHeight = 1080;
                                    break;
            case RES_1080P_1080P:   pCameraOutPrm->scOutWidth = 1920;
                                    pCameraOutPrm->scOutHeight = 1080;
                                    break;
            case RES_3MP_D1     :   pCameraOutPrm->scOutWidth = 2048;
                                    pCameraOutPrm->scOutHeight = 1536;
                                    break;
            case RES_5MP_D1     :   pCameraOutPrm->scOutWidth = 2560;
                                    pCameraOutPrm->scOutHeight = 1920;
                                    break;
            case RES_8MP_D1     :   pCameraOutPrm->scOutWidth = 3264;
                                    pCameraOutPrm->scOutHeight = 2448;
                                    break;
            case RES_10MP_D1    :   pCameraOutPrm->scOutWidth = 3648;
                                    pCameraOutPrm->scOutHeight = 2736;
                                    break;
            case RES_960P_D1    :   pCameraOutPrm->scOutWidth = 1280;
                                    pCameraOutPrm->scOutHeight = 960;
                                    break;
            default             :   pCameraOutPrm->scOutWidth = 1920;
                                    pCameraOutPrm->scOutHeight = 1080;
                                    break;
        }
#endif

        pCameraOutPrm->outQueId = 0;

        /* 2A config */
        cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
        cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;
    }
    else
    {
        /* Camera Link params */
        CameraLink_CreateParams_Init(&cameraPrm);
        cameraPrm.captureMode = CAMERA_LINK_CAPMODE_ISIF;
        cameraPrm.outQueParams[0].nextLink = dupId;
        cameraPrm.outQueParams[1].nextLink = mergeId[CAM_STREAM_MERGE_IDX];

        gVcamModuleContext.glbceId = SYSTEM_LINK_ID_INVALID;

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
#elif defined IMGS_MICRON_MT9M034
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
#else
        switch(gUI_mcfw_config.demoCfg.resolution_combo)
        {
            case RES_1080P_D1   :   pCameraInstPrm->sensorOutWidth = 1920;
                                    pCameraInstPrm->sensorOutHeight = 1080;
                                    break;
            case RES_1080P_1080P:   pCameraInstPrm->sensorOutWidth = 1920;
                                    pCameraInstPrm->sensorOutHeight = 1080;
                                    break;
            case RES_3MP_D1     :   pCameraInstPrm->sensorOutWidth = 2048;
                                    pCameraInstPrm->sensorOutHeight = 1536;
                                    break;
            case RES_5MP_D1     :   pCameraInstPrm->sensorOutWidth = 2560;
                                    pCameraInstPrm->sensorOutHeight = 1920;
                                    break;
            case RES_8MP_D1     :   pCameraInstPrm->sensorOutWidth = 3264;
                                    pCameraInstPrm->sensorOutHeight = 2448;
                                    break;
            case RES_10MP_D1    :   pCameraInstPrm->sensorOutWidth = 3648;
                                    pCameraInstPrm->sensorOutHeight = 2736;
                                    break;
            case RES_960P_D1    :   pCameraInstPrm->sensorOutWidth = 1280;
                                    pCameraInstPrm->sensorOutHeight = 960;
                                    break;
            default             :   pCameraInstPrm->sensorOutWidth = 1920;
                                    pCameraInstPrm->sensorOutHeight = 1080;
                                    break;
        }
#endif

            switch(gUI_mcfw_config.demoCfg.resolution_combo)
            {
                case RES_1080P_D1 :     pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
                                        break;
                case RES_1080P_1080P:   pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
                                        break;
                case RES_3MP_D1   :     pCameraInstPrm->standard = SYSTEM_STD_3MP_2048_1536;
                                        break;
                case RES_5MP_D1   :     pCameraInstPrm->standard = SYSTEM_STD_5MP_2560_1920;
                                        break;
                case RES_8MP_D1   :     pCameraInstPrm->standard = SYSTEM_STD_8MP_3264_2448;
                                        break;
                case RES_10MP_D1  :     pCameraInstPrm->standard = SYSTEM_STD_10MP_3648_2736;
                                        break;
                case RES_960P_D1  :     pCameraInstPrm->standard = SYSTEM_STD_960P_30;
                                        break;
                default           :     pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
                                        break;
            }

        pCameraInstPrm->numOutput = 2;

        /* First stream */
        pCameraOutPrm = &pCameraInstPrm->outParams[0];
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
        pCameraOutPrm->scEnable = FALSE;

    #ifdef IMGS_OMNIVISION_OV10630
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
    #elif defined IMGS_MICRON_MT9M034
        pCameraOutPrm->scOutWidth = 1280;
        pCameraOutPrm->scOutHeight = 720;
    #else
        /* When VS is enabled then scOutWidth/scOutHeight cannot be equal to sensorOutWidth/sensorOutHeight */
        if(cameraPrm.vsEnable == 0)
        {
            switch(gUI_mcfw_config.demoCfg.resolution_combo)
            {
                case RES_1080P_D1   :   pCameraOutPrm->scOutWidth = 1920;
                                        pCameraOutPrm->scOutHeight = 1080;
                                        break;
                case RES_1080P_1080P:   pCameraOutPrm->scOutWidth = 1920;
                                        pCameraOutPrm->scOutHeight = 1080;
                                        break;
                case RES_3MP_D1     :   pCameraOutPrm->scOutWidth = 2048;
                                        pCameraOutPrm->scOutHeight = 1536;
                                        break;
                case RES_5MP_D1     :   pCameraOutPrm->scOutWidth = 2560;
                                        pCameraOutPrm->scOutHeight = 1920;
                                        break;
                case RES_8MP_D1     :   pCameraOutPrm->scOutWidth = 3264;
                                        pCameraOutPrm->scOutHeight = 2448;
                                        break;
                case RES_10MP_D1    :   pCameraOutPrm->scOutWidth = 3648;
                                        pCameraOutPrm->scOutHeight = 2736;
                                        break;
                case RES_960P_D1    :   pCameraOutPrm->scOutWidth = 1280;
                                        pCameraOutPrm->scOutHeight = 960;
                                        break;
                default             :   pCameraOutPrm->scOutWidth = 1920;
                                        pCameraOutPrm->scOutHeight = 1080;
                                        break;
            }
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
        pCameraOutPrm->scOutWidth = 720;
        pCameraOutPrm->scOutHeight = 480;
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
#ifdef WDR_ON
        cameraPrm.glbceLinkId = SYSTEM_LINK_ID_INVALID;
#endif

        /* Glbce Link Params */
        glbcePrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
        glbcePrm.inQueParams.prevLinkQueId = 0;
        glbcePrm.outQueParams.nextLink = ispId;
        glbcePrm.chCreateParams[0].preset  = 0;//gGLBCE_AlgoPreset;
        glbcePrm.glbceEnable = gUI_mcfw_config.glbceEnable;
        glbcePrm.glbceSupportID = glbceSupportId;
        glbcePrm.swosdLinkID = gVsysModuleContext.swOsdId;
#ifdef WDR_ON
        wdrPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
        wdrPrm.inQueParams.prevLinkQueId = 0;
        wdrPrm.outQueParams.nextLink = ispId;
        wdrPrm.chCreateParams[0].dParams.wdrMode = gUI_mcfw_config.glbceEnable > 3 ? gUI_mcfw_config.glbceEnable -3 : 0;
#endif
        /* isp link params */
        ispPrm.inQueParams.prevLinkId    = glbceId;
#ifdef WDR_ON
        ispPrm.inQueParams.prevLinkId     = wdrId;
#endif
        ispPrm.inQueParams.prevLinkQueId = 0;
        ispPrm.numOutQueue               = 2;
        ispPrm.outQueInfo[0].nextLink    = dupId;
        ispPrm.outQueInfo[1].nextLink    = mergeId[CAM_STREAM_MERGE_IDX];

        #if defined(TI_8107_BUILD)
            ispPrm.clkDivM                   = 10;
            ispPrm.clkDivN                   = 20;
        #else
            ispPrm.clkDivM                   = 20;
            ispPrm.clkDivN                   = 30;
        #endif

        ispPrm.vsEnable = gUI_mcfw_config.vsEnable;
        ispPrm.vnfFullResolution = FALSE;

        ispPrm.outQueuePrm[0].dataFormat = SYSTEM_DF_YUV420SP_UV;

        #ifdef IMGS_OMNIVISION_OV10630
            ispPrm.outQueuePrm[0].width      = 1280;
            ispPrm.outQueuePrm[0].height     = 720;
        #elif defined IMGS_MICRON_MT9M034
            ispPrm.outQueuePrm[0].width      = 1280;
            ispPrm.outQueuePrm[0].height     = 720;

        #else
            switch(gUI_mcfw_config.demoCfg.resolution_combo)
            {
                case RES_1080P_D1 :
                case RES_1080P_1080P :
                                    if(ispPrm.vsEnable == TRUE)
                                    {
                                        ispPrm.outQueuePrm[0].width = 1600;
                                        ispPrm.outQueuePrm[0].height = 900;
                                    }
                                    else
                                    {
                                        ispPrm.outQueuePrm[0].width = 1920;
                                        ispPrm.outQueuePrm[0].height = 1080;
                                    }
                                    break;
                case RES_3MP_D1 :   ispPrm.outQueuePrm[0].width = 2048;
                                    ispPrm.outQueuePrm[0].height = 1536;
                                    ispPrm.vsEnable = FALSE;
                                    break;
                case RES_5MP_D1 :   ispPrm.outQueuePrm[0].width = 2560;
                                    ispPrm.outQueuePrm[0].height = 1920;
                                    ispPrm.vsEnable = FALSE;
                                    break;
                case RES_8MP_D1 :   ispPrm.outQueuePrm[0].width = 3264;
                                    ispPrm.outQueuePrm[0].height = 2448;
                                    ispPrm.vsEnable = FALSE;
                                    break;
                case RES_10MP_D1:   ispPrm.outQueuePrm[0].width = 3648;
                                    ispPrm.outQueuePrm[0].height = 2736;
                                    ispPrm.vsEnable = FALSE;
                                    break;
                case RES_960P_D1:   ispPrm.outQueuePrm[0].width = 1280;
                                    ispPrm.outQueuePrm[0].height = 960;
                                    ispPrm.vsEnable = FALSE;
                                    break;
                default :           ispPrm.outQueuePrm[0].width = 1920;
                                    ispPrm.outQueuePrm[0].height = 1080;
                                    ispPrm.vsEnable = FALSE;
                                    break;
            }
        #endif

        ispPrm.outQueuePrm[1].dataFormat = SYSTEM_DF_YUV420SP_UV;
        ispPrm.outQueuePrm[1].width      = 720;
        ispPrm.outQueuePrm[1].height     = 480;

        ispPrm.outQueuePrm[0].tilerEnable = 0;
        ispPrm.outQueuePrm[1].tilerEnable = 0;
    }
    /* Dup Link params */
    if(gUI_mcfw_config.glbceEnable)
        dupPrm.inQueParams.prevLinkId = ispId;
    else
        dupPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;

    dupPrm.inQueParams.prevLinkQueId = 0;
    dupPrm.numOutQue = 2;
    dupPrm.outQueParams[0].nextLink = mergeId[CAM_STREAM_MERGE_IDX];
    dupPrm.outQueParams[1].nextLink = mergeId[CAM_STREAM_MERGE_IDX];
    dupPrm.notifyNextLink = TRUE;

    /* Merge Link params */
    mergePrm[CAM_STREAM_MERGE_IDX].numInQue = 3;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkId = dupId;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkQueId = 0;
    if(gUI_mcfw_config.glbceEnable)
        mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkId = ispId;
    else
        mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkId = gVcamModuleContext.cameraId;

    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkQueId = 1;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[2].prevLinkId = dupId;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[2].prevLinkQueId = 1;
    mergePrm[CAM_STREAM_MERGE_IDX].outQueParams.nextLink =
        gVsysModuleContext.swOsdId;
    mergePrm[CAM_STREAM_MERGE_IDX].notifyNextLink = TRUE;

    /* SWOSD Link Params */
    swosdPrm.inQueParams.prevLinkId = mergeId[CAM_STREAM_MERGE_IDX];
    swosdPrm.inQueParams.prevLinkQueId = 0;
    swosdPrm.outQueParams.nextLink = ipcOutVpssId;

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

            if ((gUI_mcfw_config.demoCfg.resolution_combo == RES_10MP_D1) && (i == 0))
            {
                encPrm.numBufPerCh[i] = 2;
            }
            else if ((gUI_mcfw_config.demoCfg.resolution_combo == RES_8MP_D1) && (i == 0))
            {
                encPrm.numBufPerCh[i] = 3;
            }
            else
            {
                encPrm.numBufPerCh[i] = 4;
            }
            gVencModuleContext.encFormat[i] = pLinkChPrm->format;
        }
    }

    /* Video Encoder Framerate */
#ifdef IMGS_OMNIVISION_OV10630
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#elif defined IMGS_MICRON_MT9M034
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#else
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 60;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
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
#ifndef WDR_ON
        System_linkCreate(glbceSupportId, &glbceSupportPrm,sizeof(glbceSupportPrm));
        /* glbce link create */
        System_linkCreate(glbceId, &glbcePrm,sizeof(glbcePrm));
#else
        System_linkCreate(wdrId, &wdrPrm,sizeof(wdrPrm));
#endif
        /* cam pp link create */
        System_linkCreate(ispId, &ispPrm,sizeof(ispPrm));
        gIsGlbceInitDone = 1;
    }
    else
    {
        gIsGlbceInitDone = 0;
    }

    /* Dup Link */
    System_linkCreate(dupId, &dupPrm, sizeof(dupPrm));

    /* Merge Link */
    System_linkCreate(mergeId[CAM_STREAM_MERGE_IDX],
                      &mergePrm[CAM_STREAM_MERGE_IDX],
                      sizeof(mergePrm[CAM_STREAM_MERGE_IDX]));

    /* SWOSD Link */
    System_linkCreate(gVsysModuleContext.swOsdId, &swosdPrm, sizeof(swosdPrm));

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

    OSA_printf("USECASE SETUP DONE\n");
}

/* ===================================================================
 *  @func     MultiCh_deleteTriStreamLowPwr
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
Void MultiCh_deleteTriStreamLowPwr(Void)
{
    UInt32 ispId;
    UInt32 glbceId;

    UInt32 dupId;

    // UInt32 grpxId[VDIS_DEV_MAX];
    UInt32 mergeId;

    // UInt32 swosdId;

    UInt32 ipcOutVpssId;

    UInt32 ipcInVideoId;

    UInt32 vstabId;
    UInt32 glbceSupportId;
#ifdef WDR_ON
    UInt32 wdrId;
#endif
    mergeId = SYSTEM_VPSS_LINK_ID_MERGE_0;
    dupId = SYSTEM_VPSS_LINK_ID_DUP_0;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;

    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;

    ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;

    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;
#ifdef WDR_ON
    wdrId = SYSTEM_LINK_ID_WDR;
#endif

    /* Deleting the links in order of sink to source */

    System_linkDelete(gVencModuleContext.ipcBitsOutRTOSId);
    System_linkDelete(gVencModuleContext.ipcBitsInHLOSId);

    /* VSTAB Link */
    System_linkDelete(vstabId);

    System_linkDelete(gVencModuleContext.encId);

    System_linkDelete(ipcOutVpssId);
    System_linkDelete(ipcInVideoId);

    System_linkDelete(mergeId);

    System_linkDelete(dupId);

    if(gIsGlbceInitDone)
    {
#ifndef WDR_ON
        System_linkDelete(glbceId);
       System_linkDelete(glbceSupportId);

#else
        System_linkDelete(wdrId);
#endif
        System_linkDelete(ispId);
        gIsGlbceInitDone = 0;
    }
    else
      OSA_printf("We come here to leave for the day\n");
    System_linkDelete(gVcamModuleContext.cameraId);

    /* SWOSD Link */
    System_linkDelete(gVsysModuleContext.swOsdId);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    OSA_printf("USECASE TEARDOWN DONE\n");
}


