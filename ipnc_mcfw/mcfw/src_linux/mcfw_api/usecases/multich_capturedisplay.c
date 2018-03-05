/** ==================================================================
 *  @file   multich_Stream_CaptureDisplay.c
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

/*------------------------------ CAPTURE->DISPLAY ------------------------------------------

                                    SENSOR
                               _______||________
                               |               |
                               |  CAMERA LINK  |
                               |___ (VPSSM3)___|
                                RSZA        RSZB
                             (1920x1080)  (720x480)
                               (420SP)     (420SP)
                                  |           |
                               (1)|           |(1)
   NULL_HDMI(VPSSM3) --- DUP_HDMI(VPSSM3)   DUP_SD(VPSSM3) --- NULL_SD(VPSSM3)
                               (0)|           |(0)
                                  |           |
                        DIS_HDMI(VPSSM3)   DIS_SD(VPSSM3)

------------------------------------------------------------------------------------------*/

#include "mcfw/src_linux/mcfw_api/usecases/multich_common.h"
#include "mcfw/src_linux/mcfw_api/usecases/multich_ipcbits.h"
#include "demos/mcfw_api_demos/multich_usecase/ti_mcfw_ipnc_main.h"

//#define INCLUDE_DEMO_MPSCLR

/* =============================================================================
 * Externs
 * ============================================================================= */

/* =============================================================================
 * Use case code
 * ============================================================================= */
#define NUM_DUP_LINK           (3)
#define HDMIDIS_DUP_IDX        (0)
#define TVOUT_DUP_IDX          (1)
#define RAW_DUP_IDX            (2)


/* ===================================================================
 *  @func     MultiCh_createStreamCaptureDisplay
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
Void MultiCh_createStreamCaptureDisplay()
{
    CameraLink_CreateParams cameraPrm;

    NullLink_CreateParams hdNullPrm;
    NullLink_CreateParams sdNullPrm;

    DisplayLink_CreateParams displayPrm;
    DisplayLink_CreateParams displayPrm_SD;

    CameraLink_VipInstParams *pCameraInstPrm;

    CameraLink_OutParams *pCameraOutPrm;

    UInt32 hdNullId;
    UInt32 sdNullId;

    UInt32 dupId[NUM_DUP_LINK];
    DupLink_CreateParams dupPrm[NUM_DUP_LINK];

#ifdef INCLUDE_DEMO_MPSCLR
    MpSclrLink_CreateParams mpSclrPrm;
#endif /* INCLUDE_DEMO_MPSCLR */

#ifdef YUV_FRAMES_TO_A8
    System_LinkInfo framesProducerLinkInfo;

    IpcFramesOutLinkRTOS_CreateParams  ipcFramesOutVpssToHostPrm;
    IpcFramesInLinkHLOS_CreateParams   ipcFramesInHostPrm;
    IpcFramesOutLinkHLOS_CreateParams  ipcFramesOutHostPrm;
    IpcFramesInLinkRTOS_CreateParams   ipcFramesInVpssFromHostPrm;

    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams ,ipcFramesOutVpssToHostPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkHLOS_CreateParams  ,ipcFramesInHostPrm);
    MULTICH_INIT_STRUCT(IpcFramesOutLinkHLOS_CreateParams ,ipcFramesOutHostPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams  ,ipcFramesInVpssFromHostPrm);

    gVcamModuleContext.ipcFramesOutVpssToHostId  = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_1;
    gVcamModuleContext.ipcFramesInHostId         = SYSTEM_HOST_LINK_ID_IPC_FRAMES_IN_0;
    gVdisModuleContext.ipcFramesOutHostId        = SYSTEM_HOST_LINK_ID_IPC_FRAMES_OUT_0;
    gVdisModuleContext.ipcFramesInVpssFromHostId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_0;
#endif
#ifdef RAW_FRAMES_TO_A8
    IspLink_CreateParams  ispPrm;
    UInt32 ispId;
    IpcFramesOutLinkRTOS_CreateParams  ipcFramesOutVpssToHostPrm;
    IpcFramesInLinkHLOS_CreateParams   ipcFramesInHostPrm;

    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams ,ipcFramesOutVpssToHostPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkHLOS_CreateParams  ,ipcFramesInHostPrm);

    gVcamModuleContext.ipcFramesOutVpssToHostId  = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_1;
    gVcamModuleContext.ipcFramesInHostId         = SYSTEM_HOST_LINK_ID_IPC_FRAMES_IN_0;

    dupId[RAW_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_2;
    ispId = SYSTEM_LINK_ID_ISP_0;
#endif

    /* System init */
//    System_init();

    OSA_printf
        ("\n********* Entered Capture + Display usecase - H264 1080p @60 fps, H264 D1 @30fps ********\n\n");

    MultiCh_detectBoard();

    System_linkControl(SYSTEM_LINK_ID_M3VPSS,
                       SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES, NULL, 0, TRUE);

    gVcamModuleContext.cameraId = SYSTEM_LINK_ID_CAMERA;

    /* dup Link IDs*/
    dupId[HDMIDIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[TVOUT_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;

    /* display IDs */
    gVdisModuleContext.displayId[0] = SYSTEM_LINK_ID_DISPLAY_0; /* ON AND OFF
                                                                * CHIP HDMI */
    gVdisModuleContext.displayId[VDIS_DEV_SD] = SYSTEM_LINK_ID_DISPLAY_2;

    /* null link IDs*/
    hdNullId = SYSTEM_VPSS_LINK_ID_NULL_0;
    sdNullId = SYSTEM_VPSS_LINK_ID_NULL_1;

#ifdef RAW_FRAMES_TO_A8
    /* Camera Link params */
    CameraLink_CreateParams_Init(&cameraPrm);
    cameraPrm.captureMode = CAMERA_LINK_CAPMODE_DDR;
    cameraPrm.outQueParams[0].nextLink = dupId[RAW_DUP_IDX];
    cameraPrm.ispLinkId = ispId;

    /* This is for TVP5158 Audio Channels - Change it to 16 if there are 16
     * audio channels connected in cascade */
    cameraPrm.numAudioChannels = 0;
    cameraPrm.numVipInst = 1;
    cameraPrm.tilerEnable = FALSE;

    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->vipInstId = SYSTEM_CAMERA_INST_VP_CSI2;
    pCameraInstPrm->videoDecoderId = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
    pCameraInstPrm->inDataFormat = SYSTEM_DF_YUV420SP_UV;
    pCameraInstPrm->sensorOutWidth  = 1920;
    pCameraInstPrm->sensorOutHeight = 1080;
    pCameraInstPrm->standard = SYSTEM_STD_1080P_30;

    pCameraInstPrm->numOutput = 1;

    pCameraOutPrm = &pCameraInstPrm->outParams[0];
    pCameraOutPrm->dataFormat = SYSTEM_DF_BAYER_RAW;
    pCameraOutPrm->scEnable = FALSE;
    pCameraOutPrm->scOutWidth = 1920;
    pCameraOutPrm->scOutHeight = 1080;
    pCameraOutPrm->outQueId = 0;

    /* 2A config */
    cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
    cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;

    dupPrm[RAW_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.cameraId;
    dupPrm[RAW_DUP_IDX].inQueParams.prevLinkQueId = 0;
    dupPrm[RAW_DUP_IDX].numOutQue = 2;
    dupPrm[RAW_DUP_IDX].outQueParams[0].nextLink = ispId;
    dupPrm[RAW_DUP_IDX].outQueParams[1].nextLink = gVcamModuleContext.ipcFramesOutVpssToHostId;
    dupPrm[RAW_DUP_IDX].notifyNextLink = TRUE;
#else
    /* Camera Link params */
    CameraLink_CreateParams_Init(&cameraPrm);
    cameraPrm.captureMode = CAMERA_LINK_CAPMODE_ISIF;
    cameraPrm.outQueParams[0].nextLink = dupId[HDMIDIS_DUP_IDX] ;
    cameraPrm.outQueParams[1].nextLink = dupId[TVOUT_DUP_IDX];
    /* This is for TVP5158 Audio Channels - Change it to 16 if there are 16
     * audio channels connected in cascade */
    cameraPrm.numAudioChannels = 0;
    cameraPrm.numVipInst = 1;
    cameraPrm.tilerEnable = FALSE;

    cameraPrm.vsEnable = 0;
    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->vipInstId = SYSTEM_CAMERA_INST_VP_CSI2;
    pCameraInstPrm->videoDecoderId = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
    pCameraInstPrm->inDataFormat = SYSTEM_DF_YUV420SP_UV;
    pCameraInstPrm->sensorOutWidth  = 1920;
    pCameraInstPrm->sensorOutHeight = 1080;
#ifdef YUV_FRAMES_TO_A8
    pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
#else
    pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
#endif
    pCameraInstPrm->numOutput = 2;

    /* First stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[0];
    pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV; //SYSTEM_DF_YUV420SP_VU;//SYSTEM_DF_YUV422I_VYUY;
    pCameraOutPrm->scEnable = FALSE;
    pCameraOutPrm->scOutWidth = 1920;
    pCameraOutPrm->scOutHeight = 1080;
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
#endif

#ifdef RAW_FRAMES_TO_A8
    /* isp link params */
    ispPrm.inQueParams.prevLinkId    = dupId[RAW_DUP_IDX];
    ispPrm.inQueParams.prevLinkQueId = 0;
    ispPrm.numOutQueue               = 2;
    ispPrm.outQueInfo[0].nextLink    = dupId[HDMIDIS_DUP_IDX];
    ispPrm.outQueInfo[1].nextLink    = dupId[TVOUT_DUP_IDX];
    ispPrm.clkDivM                   = 10;
    ispPrm.clkDivN                   = 20;
    ispPrm.vsEnable                  = FALSE;

    ispPrm.outQueuePrm[0].dataFormat = SYSTEM_DF_YUV422I_VYUY;
    ispPrm.outQueuePrm[0].width      = 1920;
    ispPrm.outQueuePrm[0].height     = 1080;

    ispPrm.outQueuePrm[1].dataFormat = SYSTEM_DF_YUV420SP_VU;
    ispPrm.outQueuePrm[1].width      = 720;
    ispPrm.outQueuePrm[1].height     = 480;
    ispPrm.outQueuePrm[1].standard   = SYSTEM_STD_NTSC;//SYSTEM_STD_PAL;//SYSTEM_STD_NTSC;
#endif

    /* dup link parameters*/
#ifdef RAW_FRAMES_TO_A8
    dupPrm[HDMIDIS_DUP_IDX].inQueParams.prevLinkId = ispId;
#else
    dupPrm[HDMIDIS_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.cameraId;
#endif
    dupPrm[HDMIDIS_DUP_IDX].inQueParams.prevLinkQueId = 0;
    dupPrm[HDMIDIS_DUP_IDX].numOutQue = 2;
    dupPrm[HDMIDIS_DUP_IDX].outQueParams[0].nextLink = gVdisModuleContext.displayId[0];
    dupPrm[HDMIDIS_DUP_IDX].outQueParams[1].nextLink = hdNullId;
    dupPrm[HDMIDIS_DUP_IDX].notifyNextLink = TRUE;

#ifdef INCLUDE_DEMO_MPSCLR

    MULTICH_INIT_STRUCT(MpSclrLink_CreateParams, mpSclrPrm);
    gVdisModuleContext.mpSclrId = SYSTEM_VPSS_LINK_ID_MP_SCLR_INST_0;

    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->sensorOutWidth  = 3648;
    pCameraInstPrm->sensorOutHeight = 2736;
    pCameraInstPrm->standard = SYSTEM_STD_10MP_3648_2736;

    pCameraOutPrm = &pCameraInstPrm->outParams[0];
    pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;

    pCameraOutPrm->scOutWidth = 3648;
    pCameraOutPrm->scOutHeight = 2736;

    cameraPrm.outQueParams[0].nextLink = gVdisModuleContext.mpSclrId;

    mpSclrPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
    mpSclrPrm.outQueParams.nextLink = dupId[HDMIDIS_DUP_IDX];

    dupPrm[HDMIDIS_DUP_IDX].inQueParams.prevLinkId = gVdisModuleContext.mpSclrId;
    dupPrm[HDMIDIS_DUP_IDX].inQueParams.prevLinkQueId = 0;

    mpSclrPrm.pathId = MP_SCLR_LINK_SC5;
    mpSclrPrm.numCh = 1;
    mpSclrPrm.enableLineSkip = FALSE;


#endif /* INCLUDE_DEMO_MPSCLR */

#ifdef RAW_FRAMES_TO_A8
    dupPrm[TVOUT_DUP_IDX].inQueParams.prevLinkId = ispId;
#else
    dupPrm[TVOUT_DUP_IDX].inQueParams.prevLinkId = gVcamModuleContext.cameraId;
#endif
    dupPrm[TVOUT_DUP_IDX].inQueParams.prevLinkQueId = 1;
    dupPrm[TVOUT_DUP_IDX].numOutQue = 2;
#ifdef YUV_FRAMES_TO_A8
    dupPrm[TVOUT_DUP_IDX].outQueParams[0].nextLink = gVcamModuleContext.ipcFramesOutVpssToHostId;

    /* Set ipcFramesOutVpss link info */
    ipcFramesOutVpssToHostPrm.baseCreateParams.noNotifyMode = TRUE;
    ipcFramesOutVpssToHostPrm.baseCreateParams.notifyNextLink = FALSE;
    ipcFramesOutVpssToHostPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesOutVpssToHostPrm.baseCreateParams.inQueParams.prevLinkId = dupId[TVOUT_DUP_IDX];
    ipcFramesOutVpssToHostPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesOutVpssToHostPrm.baseCreateParams.outQueParams[0].nextLink = gVcamModuleContext.ipcFramesInHostId;

    ipcFramesInHostPrm.baseCreateParams.noNotifyMode = TRUE;
    ipcFramesInHostPrm.baseCreateParams.notifyNextLink = FALSE;
    ipcFramesInHostPrm.baseCreateParams.notifyPrevLink = FALSE;
    ipcFramesInHostPrm.baseCreateParams.inQueParams.prevLinkId = gVcamModuleContext.ipcFramesOutVpssToHostId;
    ipcFramesInHostPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInHostPrm.baseCreateParams.outQueParams[0].nextLink = SYSTEM_LINK_ID_INVALID;
    ipcFramesInHostPrm.exportOnlyPhyAddr = TRUE;

    ipcFramesInHostPrm.cbCtx = &gVcamModuleContext;
    ipcFramesInHostPrm.cbFxn = Vcam_ipcFramesInCbFxn;

    ipcFramesOutHostPrm.baseCreateParams.noNotifyMode = TRUE;
    ipcFramesOutHostPrm.baseCreateParams.notifyNextLink = FALSE;
    ipcFramesOutHostPrm.baseCreateParams.notifyPrevLink = FALSE;
    ipcFramesOutHostPrm.baseCreateParams.inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    ipcFramesOutHostPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesOutHostPrm.baseCreateParams.outQueParams[0].nextLink = gVdisModuleContext.ipcFramesInVpssFromHostId;

    ipcFramesInVpssFromHostPrm.baseCreateParams.noNotifyMode = TRUE;
    ipcFramesInVpssFromHostPrm.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesInVpssFromHostPrm.baseCreateParams.notifyPrevLink = FALSE;
    ipcFramesInVpssFromHostPrm.baseCreateParams.inQueParams.prevLinkId = gVdisModuleContext.ipcFramesOutHostId;
    ipcFramesInVpssFromHostPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInVpssFromHostPrm.baseCreateParams.outQueParams[0].nextLink = gVdisModuleContext.displayId[VDIS_DEV_SD];
#else
    dupPrm[TVOUT_DUP_IDX].outQueParams[0].nextLink = gVdisModuleContext.displayId[VDIS_DEV_SD];
#endif

#ifdef RAW_FRAMES_TO_A8
    /* Set ipcFramesOutVpss link info */
    ipcFramesOutVpssToHostPrm.baseCreateParams.noNotifyMode = TRUE;
    ipcFramesOutVpssToHostPrm.baseCreateParams.notifyNextLink = FALSE;
    ipcFramesOutVpssToHostPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesOutVpssToHostPrm.baseCreateParams.inQueParams.prevLinkId = dupId[RAW_DUP_IDX];
    ipcFramesOutVpssToHostPrm.baseCreateParams.inQueParams.prevLinkQueId = 1;
    ipcFramesOutVpssToHostPrm.baseCreateParams.outQueParams[0].nextLink = gVcamModuleContext.ipcFramesInHostId;

    ipcFramesInHostPrm.baseCreateParams.noNotifyMode = TRUE;
    ipcFramesInHostPrm.baseCreateParams.notifyNextLink = FALSE;
    ipcFramesInHostPrm.baseCreateParams.notifyPrevLink = FALSE;
    ipcFramesInHostPrm.baseCreateParams.inQueParams.prevLinkId = gVcamModuleContext.ipcFramesOutVpssToHostId;
    ipcFramesInHostPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInHostPrm.baseCreateParams.outQueParams[0].nextLink = SYSTEM_LINK_ID_INVALID;
    ipcFramesInHostPrm.exportOnlyPhyAddr = TRUE;

    ipcFramesInHostPrm.cbCtx = &gVcamModuleContext;
    ipcFramesInHostPrm.cbFxn = Vcam_ipcFramesInCbFxn;
#endif

    dupPrm[TVOUT_DUP_IDX].outQueParams[1].nextLink = sdNullId;
    dupPrm[TVOUT_DUP_IDX].notifyNextLink = TRUE;

    /* display link params */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
    displayPrm.inQueParams[0].prevLinkId = dupId[HDMIDIS_DUP_IDX] ;
    displayPrm.inQueParams[0].prevLinkQueId = 0;
    displayPrm.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    displayPrm.displayId  = DISPLAY_LINK_DISPLAY_SC2;

    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm_SD);
#ifdef YUV_FRAMES_TO_A8
    displayPrm_SD.inQueParams[0].prevLinkId = gVcamModuleContext.ipcFramesInHostId;
#else
    displayPrm_SD.inQueParams[0].prevLinkId = dupId[TVOUT_DUP_IDX];
#endif
    displayPrm_SD.inQueParams[0].prevLinkQueId = 0;
    displayPrm_SD.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_SD].resolution;
    displayPrm_SD.displayId  = DISPLAY_LINK_DISPLAY_SD;

 //   MultiCh_displayCtrlInit(&gVdisModuleContext.vdisConfig);

    /* null Link Params */
    hdNullPrm.numInQue = 1;
    hdNullPrm.inQueParams[0].prevLinkId = dupId[HDMIDIS_DUP_IDX];
    hdNullPrm.inQueParams[0].prevLinkQueId = 1;

    sdNullPrm.numInQue = 1;
    sdNullPrm.inQueParams[0].prevLinkId = dupId[TVOUT_DUP_IDX];
    sdNullPrm.inQueParams[0].prevLinkQueId = 1;

    /* Links Creation */
    /* Camera Link */
    System_linkCreate(gVcamModuleContext.cameraId, &cameraPrm,
                      sizeof(cameraPrm));
    System_linkControl(gVcamModuleContext.cameraId,
                       CAMERA_LINK_CMD_DETECT_VIDEO, NULL, 0, TRUE);

#ifdef RAW_FRAMES_TO_A8
    System_linkCreate(dupId[RAW_DUP_IDX], &dupPrm[RAW_DUP_IDX], sizeof(dupPrm[RAW_DUP_IDX]));
    /* cam pp link create */
    System_linkCreate(ispId, &ispPrm,sizeof(ispPrm));

    System_linkCreate(gVcamModuleContext.ipcFramesOutVpssToHostId, &ipcFramesOutVpssToHostPrm, sizeof(ipcFramesOutVpssToHostPrm));
    System_linkCreate(gVcamModuleContext.ipcFramesInHostId, &ipcFramesInHostPrm, sizeof(ipcFramesInHostPrm));
#endif

#ifdef INCLUDE_DEMO_MPSCLR
    System_linkCreate(gVdisModuleContext.mpSclrId, &mpSclrPrm, sizeof(mpSclrPrm));
#endif /* INCLUDE_DEMO_MPSCLR */

    /*dup link*/
    System_linkCreate(dupId[HDMIDIS_DUP_IDX], &dupPrm[HDMIDIS_DUP_IDX], sizeof(dupPrm[HDMIDIS_DUP_IDX]));
    System_linkCreate(dupId[TVOUT_DUP_IDX], &dupPrm[TVOUT_DUP_IDX], sizeof(dupPrm[TVOUT_DUP_IDX]));

    /* null Link */
    System_linkCreate(hdNullId, &hdNullPrm, sizeof(hdNullPrm));
    System_linkCreate(sdNullId,&sdNullPrm, sizeof(sdNullPrm));

#ifdef YUV_FRAMES_TO_A8
    System_linkCreate(gVcamModuleContext.ipcFramesOutVpssToHostId, &ipcFramesOutVpssToHostPrm, sizeof(ipcFramesOutVpssToHostPrm));
    System_linkCreate(gVcamModuleContext.ipcFramesInHostId, &ipcFramesInHostPrm, sizeof(ipcFramesInHostPrm));

    System_linkGetInfo(gVcamModuleContext.ipcFramesInHostId,&framesProducerLinkInfo);
    OSA_assert(framesProducerLinkInfo.numQue == 1);
    ipcFramesOutHostPrm.inQueInfo = framesProducerLinkInfo.queInfo[0];

    System_linkCreate(gVdisModuleContext.ipcFramesOutHostId, &ipcFramesOutHostPrm, sizeof(ipcFramesOutHostPrm));
    System_linkCreate(gVdisModuleContext.ipcFramesInVpssFromHostId, &ipcFramesInVpssFromHostPrm, sizeof(ipcFramesInVpssFromHostPrm));
#endif

    /* display link */
    System_linkCreate(gVdisModuleContext.displayId[0], &displayPrm,
                      sizeof(displayPrm));
    System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_SD], &displayPrm_SD,
                      sizeof(displayPrm_SD));

    MultiCh_memPrintHeapStatus();
    OSA_printf("USECASE SETUP DONE\n");
}

/* ===================================================================
 *  @func     MultiCh_deleteStreamCaptureDisplay
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
Void MultiCh_deleteStreamCaptureDisplay()
{
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 hdNullId,sdNullId;

#ifdef RAW_FRAMES_TO_A8
    UInt32 ispId = SYSTEM_LINK_ID_ISP_0;
    dupId[RAW_DUP_IDX]   = SYSTEM_VPSS_LINK_ID_DUP_2;
#endif

    /* dup Link IDs*/
    dupId[HDMIDIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[TVOUT_DUP_IDX]   = SYSTEM_VPSS_LINK_ID_DUP_1;

    /* null link IDs*/
    hdNullId = SYSTEM_VPSS_LINK_ID_NULL_0;
    sdNullId = SYSTEM_VPSS_LINK_ID_NULL_1;

    /* Links Deletion in reverse order */

    /* Display links */
    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_SD]);
    System_linkDelete(gVdisModuleContext.displayId[0]);

    /* Null links */
    System_linkDelete(sdNullId);
    System_linkDelete(hdNullId);

#ifdef INCLUDE_DEMO_MPSCLR
    System_linkDelete(gVdisModuleContext.mpSclrId);
#endif /* INCLUDE_DEMO_MPSCLR */

    /* Dup links */
    System_linkDelete(dupId[TVOUT_DUP_IDX]);
    System_linkDelete(dupId[HDMIDIS_DUP_IDX]);

#ifdef RAW_FRAMES_TO_A8
    System_linkDelete(dupId[RAW_DUP_IDX]);
    System_linkDelete(ispId);
#endif

#ifdef YUV_FRAMES_TO_A8
    System_linkDelete(gVcapModuleContext.ipcFramesOutVpssToHostId);
    System_linkDelete(gVcapModuleContext.ipcFramesInHostId);
    System_linkDelete(gVdisModuleContext.ipcFramesOutHostId);
    System_linkDelete(gVdisModuleContext.ipcFramesInVpssFromHostId);
#endif

    /* Camera Link */
    System_linkDelete(gVcamModuleContext.cameraId);

 //   MultiCh_displayCtrlDeInit(&gVdisModuleContext.vdisConfig);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    //System_deInit();
}

