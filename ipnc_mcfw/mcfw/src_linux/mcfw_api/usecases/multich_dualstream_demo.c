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

/*------------------------------ DUAL OUT <1080p + 1080p> ----------------------------------

                                      SENSOR
                                ________||_______
                                |                |
                                |   CAMERA LINK  |
                                |____(VPSS M3)___|
                               RSZA             RSZB
         |--- MCTNF ON ---- (1920x1080)       (720x480)
         |                 (420SP/422I)        (420SP)
    IPC FRAMES               |      |             |
    OUT(VPSSM3)        ISS_VNF ON  DSS_NF ON      |
         |                   |      |             |
         |        |-ISS_NF(VPSSM3) DSS_NF(VPSSM3) |
    IPC FRAMES    |          |      |             |
    ON(VIDEOM3)   |          |--|---|             |
         |        |             |                 |
         |        |        MERGE(VPSSM3) ---------|
   MCTNF(VIDEOM3) |             |
         |        |             |
         |        |        SWOSD(VPSSM3)
    IPC FRAMES    |             |
    OUT(VIDEOM3)  |             |       (1)
         |        |         DUP(VPSSM3) ------ DIS_HDMI(VPSSM3)
         |        |             |(0)
    IPC FRAMES ---|             |
    IN(VPSSM3)            IPC OUT(VPSSM3)
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
#define _BYPASS_DISPLAY //Enable This to run the 3DNF

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

/* Dup Links */
#define NUM_DUP_LINK                (3)
#define DIS_DUP_IDX                 (0)
#define MJPEG_DUP_LINK_IDX          (1)
#define SCALER_DUP_IDX              (2)

UInt32 gMctnfLinkCreated;

UInt32 IsMcTNFDemo()
{
#ifdef USE_MCTNF
    if((gUI_mcfw_config.vnfUseCase == TRUE) &&
       (gUI_mcfw_config.noisefilterMode == ISS_VNF_ON) &&
       (gUI_mcfw_config.ldcEnable == 0) &&
       (gUI_mcfw_config.vsEnable == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif
}

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
Void MultiCh_createStreamDualStreamDemo()
{
    UInt32 i;

    CameraLink_CreateParams cameraPrm;

    UInt32 ispId;

    UInt32 glbceId;

    NsfLink_CreateParams nsfPrm;

    VnfLink_CreateParams vnfPrm;

    MergeLink_CreateParams mergePrm[NUM_MERGE_LINK];

    IpcLink_CreateParams ipcOutVpssPrm;

    IpcLink_CreateParams ipcInVideoPrm;

    EncLink_CreateParams encPrm;

    SwosdLink_CreateParams swosdPrm;

    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;

    IpcBitsInLinkHLOS_CreateParams ipcBitsInHostPrm0;

    VstabLink_CreateParams vstabPrm;

    DupLink_CreateParams dupPrm;

    DisplayLink_CreateParams displayPrm;

    CameraLink_VipInstParams *pCameraInstPrm;

    CameraLink_OutParams *pCameraOutPrm;

    UInt32 mergeId[NUM_MERGE_LINK];

    UInt32 ipcOutVpssId;

    UInt32 ipcInVideoId;

    UInt32 vstabId;

    UInt32 glbceSupportId;

    UInt32 dupId;

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

#ifdef USE_MCTNF
    if(IsMcTNFDemo() == 1)
    {
        MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssIdForMCTNF_params);
        MULTICH_INIT_STRUCT(IpcLink_CreateParams,  ipcInVideoIdForMCTNF_params);
        MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVideoIdForMCTNF_params);
        MULTICH_INIT_STRUCT(IpcLink_CreateParams,  ipcInVpssIdForMCTNF_params);
    }
#endif

    OSA_printf("\n********** DEMO USECASE ********\n");
    OSA_printf
        ("********* Entered DUAL Streaming usecase - H264 1080p @30fps + H264 1080p @30fps ********\n\n");

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
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;
    ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;
    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    gVencModuleContext.ipcBitsOutRTOSId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gVencModuleContext.ipcBitsInHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;
    gVcamModuleContext.glbceId = SYSTEM_LINK_ID_INVALID;
    gVdisModuleContext.displayId[VDIS_DEV_HDMI] = SYSTEM_LINK_ID_DISPLAY_0;
    dupId = SYSTEM_VPSS_LINK_ID_DUP_0;

#ifdef USE_MCTNF
    if(IsMcTNFDemo() == 1)
    {
        ipcOutVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1;
        ipcInVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1;
        ipcOutVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
        ipcInVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
        gVcamModuleContext.mctnfId = SYSTEM_LINK_ID_MCTNF;
    }
#endif

    /* Camera Link params */
    CameraLink_CreateParams_Init(&cameraPrm);
    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            gVcamModuleContext.nsfId = SYSTEM_LINK_ID_NSF_0;
        }
        else
        {
            gVcamModuleContext.vnfId = SYSTEM_LINK_ID_VNF;
            cameraPrm.vnfLinkId = SYSTEM_LINK_ID_VNF;
        }
    }

    cameraPrm.captureMode = CAMERA_LINK_CAPMODE_ISIF;

#ifdef USE_MCTNF
    if(IsMcTNFDemo() == 1)
    {
        cameraPrm.mctnfLinkId = SYSTEM_LINK_ID_MCTNF;
    }
#endif

    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            cameraPrm.outQueParams[0].nextLink = gVcamModuleContext.nsfId;
        }
        else
        {
#ifdef USE_MCTNF
            if(IsMcTNFDemo() == 1)
            {
    #ifdef VNF_BEFORE_MCTNF
                cameraPrm.outQueParams[0].nextLink = gVcamModuleContext.vnfId;
    #else
                cameraPrm.outQueParams[0].nextLink = ipcOutVpssIdForMCTNF;
    #endif
            }
            else
#endif
            {
                cameraPrm.outQueParams[0].nextLink = gVcamModuleContext.vnfId;
            }
        }
    }
    else
    {
        cameraPrm.outQueParams[0].nextLink = mergeId[CAM_STREAM_MERGE_IDX];
    }

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

    cameraPrm.vsDemoEnable = 0;

    cameraPrm.vstabLinkId = vstabId;

    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->vipInstId = SYSTEM_CAMERA_INST_VP_CSI2;
    pCameraInstPrm->videoDecoderId = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
    pCameraInstPrm->inDataFormat = SYSTEM_DF_YUV420SP_UV;

#ifdef IMGS_OMNIVISION_OV10630
    pCameraInstPrm->sensorOutWidth  = 1280;
    pCameraInstPrm->sensorOutHeight = 720;
#else
    pCameraInstPrm->sensorOutWidth  = 1920;
    pCameraInstPrm->sensorOutHeight = 1080;
#endif

    if(IsMcTNFDemo() == 1)
    {
        pCameraInstPrm->sensorOutWidth  = 1280;
        pCameraInstPrm->sensorOutHeight = 720;
    }

    pCameraInstPrm->standard = SYSTEM_STD_1080P_30;

    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
        }
        else
        {
            pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
            cameraPrm.issVnfEnable = 1;
        }
    }

    pCameraInstPrm->numOutput = 2;

    /* First stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[0];
    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
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
    }
    else
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;

    pCameraOutPrm->scEnable = FALSE;

#ifdef IMGS_OMNIVISION_OV10630
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

    if(IsMcTNFDemo() == 1)
    {
        pCameraOutPrm->scOutWidth  = 1280;
        pCameraOutPrm->scOutHeight = 720;
    }

    pCameraOutPrm->outQueId = 0;

    /* Second stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[1];
    pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
    pCameraOutPrm->scEnable = FALSE;

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

    if(IsMcTNFDemo() == 1)
    {
        pCameraOutPrm->scOutWidth  = 1280;
        pCameraOutPrm->scOutHeight = 720;
    }

    pCameraOutPrm->standard = SYSTEM_STD_1080P_30;
    pCameraOutPrm->outQueId = 1;

    /* 2A config */
    cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
    cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;

    if(cameraPrm.vsEnable == 1)
    {
        /* vstab Link params */
        vstabPrm.totalFrameWidth  = pCameraInstPrm->sensorOutWidth;
        vstabPrm.totalFrameHeight = pCameraInstPrm->sensorOutHeight;
    }

    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
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

            nsfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;

            nsfPrm.inQueParams.prevLinkQueId = 0;
            nsfPrm.numOutQue = 1;
            nsfPrm.numBufsPerCh = 6;
            nsfPrm.inputFrameRate = 60;
            nsfPrm.outputFrameRate = 60;
            nsfPrm.outQueParams[0].nextLink = mergeId[CAM_STREAM_MERGE_IDX];
            nsfPrm.nsfStrength = gUI_mcfw_config.vnfStrength;
        }
        else
        {
#ifdef USE_MCTNF
            if(IsMcTNFDemo() == 1)
            {
                 /* IPC Out VPSS link params */
    #ifdef VNF_BEFORE_MCTNF
                ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkId = gVcamModuleContext.vnfId;
                ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    #else
                ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
                ipcOutVpssIdForMCTNF_params.inQueParams.prevLinkQueId = 0;
    #endif
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
    #ifdef VNF_BEFORE_MCTNF
                ipcInVpssIdForMCTNF_params.outQueParams[0].nextLink = mergeId[CAM_STREAM_MERGE_IDX];
    #else
                ipcInVpssIdForMCTNF_params.outQueParams[0].nextLink = gVcamModuleContext.vnfId;
    #endif
                ipcInVpssIdForMCTNF_params.notifyNextLink = TRUE;
                ipcInVpssIdForMCTNF_params.notifyPrevLink = TRUE;
                ipcInVpssIdForMCTNF_params.noNotifyMode = FALSE;
            }
#endif

            VnfLink_ChCreateParams *vnfParams;

            /* Vnf Link Params */
            VnfLink_CreateParams_Init(&vnfPrm);

#ifdef USE_MCTNF
            if(IsMcTNFDemo() == 1)
            {
    #ifdef VNF_BEFORE_MCTNF
                vnfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
                vnfPrm.inQueParams.prevLinkQueId = 0;
    #else
                vnfPrm.inQueParams.prevLinkId = ipcInVpssIdForMCTNF;
                vnfPrm.inQueParams.prevLinkQueId = 0;
    #endif
            }
            else
#endif
            {
                vnfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
                vnfPrm.inQueParams.prevLinkQueId = 0;
            }

            vnfPrm.outQueParams.nextLink = mergeId[CAM_STREAM_MERGE_IDX];

            if(IsMcTNFDemo() == 1)
            {
#ifdef VNF_BEFORE_MCTNF
                vnfPrm.outQueParams.nextLink = ipcOutVpssIdForMCTNF;
#endif
            }

            vnfParams = &vnfPrm.chCreateParams[0];
            vnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;

            if (gUI_mcfw_config.ldcEnable)
            {
                vnfParams->sParams.eOperateMode = VNF_LINK_LDC_AFFINE_NSF_TNF;
                vnfPrm.prevLinkMcTNF = 0;
            }
            else
            {
                if(IsMcTNFDemo() == 1)
                {
                    vnfParams->sParams.eOperateMode =  VNF_LINK_NSF2;
                }
                else
                {
#ifdef USE_TNF3_FILTER
                    vnfParams->sParams.eOperateMode =  VNF_LINK_TNF3;
#else
                    vnfParams->sParams.eOperateMode = VNF_LINK_AFFINE_NSF_TNF;
#endif
                    vnfPrm.prevLinkMcTNF = 0;
                }

                vnfParams->sParams.eNsfSet = VNF_LINK_NSF_LUMA_CHROMA;
            }

            if(((vnfParams->sParams.eOperateMode == VNF_LINK_TNF3) &&
                (vnfParams->sParams.eNsfSet == VNF_LINK_NSF_LUMA_CHROMA)) ||
                (vnfParams->sParams.eOperateMode == VNF_LINK_NSF2) ||
                (vnfParams->sParams.eOperateMode == VNF_LINK_LDC_AFFINE_NSF_TNF))
            {
                cameraPrm.vnfFullResolution = TRUE;
            }
            else
            {
                cameraPrm.vnfFullResolution = FALSE;
            }

            if(IsMcTNFDemo() == 1)
            {
#ifndef VNF_BEFORE_MCTNF
                cameraPrm.vnfFullResolution = FALSE;
#endif
            }

            vnfParams->dParams.ldcParams.eInterpolationLuma =
                VNF_LINK_YINTERPOLATION_BILINEAR;
            vnfParams->dParams.ldcParams.unPixelPad = 4;

            vnfParams->dParams.nsfParams.bLumaEn = TRUE;
            vnfParams->dParams.nsfParams.bChromaEn = TRUE;
            vnfParams->dParams.nsfParams.eSmoothVal = VNF_LINK_SMOOTH_1;
            vnfParams->dParams.nsfParams.bSmoothLumaEn = TRUE;
            vnfParams->dParams.nsfParams.bSmoothChromaEn = TRUE;

            vnfParams->dParams.tnfParams.useDefault = TRUE;

            vnfParams->dParams.nsfFilter.eFilterParam = VNF_LINK_PARAM_DEFAULT;
            vnfParams->dParams.nsfEdge.eEdgeParam = VNF_LINK_PARAM_DEFAULT;
            vnfParams->dParams.nsfShading.eShdParam = VNF_LINK_PARAM_DEFAULT;
            vnfParams->dParams.nsfDesaturation.eDesatParam = VNF_LINK_PARAM_DEFAULT;

            vnfPrm.chCreateParams[0].bSnfEnable = gUI_mcfw_config.snfEnable;
            vnfPrm.chCreateParams[0].bTnfEnable = gUI_mcfw_config.tnfEnable;
            vnfPrm.chCreateParams[0].strength   = gUI_mcfw_config.vnfStrength;

            vnfPrm.tilerEnable = FALSE;
        }
    }

    /* Merge Link params */
    mergePrm[CAM_STREAM_MERGE_IDX].numInQue = 2;
    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
            mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkId =
                gVcamModuleContext.nsfId;
        else
            mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkId =
                gVcamModuleContext.vnfId;
    }
    else
    {
        mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkId = gVcamModuleContext.cameraId;
    }

    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[0].prevLinkQueId = 0;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkId = gVcamModuleContext.cameraId;
    mergePrm[CAM_STREAM_MERGE_IDX].inQueParams[1].prevLinkQueId = 1;
    mergePrm[CAM_STREAM_MERGE_IDX].outQueParams.nextLink = gVsysModuleContext.swOsdId;
    mergePrm[CAM_STREAM_MERGE_IDX].notifyNextLink = TRUE;

    /* SWOSD Link Params */
    swosdPrm.inQueParams.prevLinkId = mergeId[CAM_STREAM_MERGE_IDX];
    swosdPrm.inQueParams.prevLinkQueId = 0;
    swosdPrm.outQueParams.nextLink = dupId;

    /* Dup for display */
    dupPrm.inQueParams.prevLinkId    = gVsysModuleContext.swOsdId;
    dupPrm.inQueParams.prevLinkQueId = 0;
    dupPrm.numOutQue                 = 2;
    dupPrm.outQueParams[0].nextLink  = ipcOutVpssId;
    dupPrm.outQueParams[1].nextLink  = gVdisModuleContext.displayId[VDIS_DEV_HDMI];
    dupPrm.notifyNextLink            = TRUE;

    /* Display Link */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
    displayPrm.inQueParams[0].prevLinkId = dupId;
    displayPrm.inQueParams[0].prevLinkQueId = 1;
    displayPrm.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    displayPrm.displayId  = DISPLAY_LINK_DISPLAY_SC2;

    /* IPC Out VPSS link params */
    ipcOutVpssPrm.inQueParams.prevLinkId = dupId;
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
#else
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 60;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#endif
    encPrm.chCreateParams[1].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;

    encPrm.vsEnable = gUI_mcfw_config.vsEnable;

    encPrm.isVaUseCase = 0;

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

    gMctnfLinkCreated = 0;

    /* Links Creation */
    /* Camera Link */
    System_linkCreate(gVcamModuleContext.cameraId, &cameraPrm,
                      sizeof(cameraPrm));
    System_linkControl(gVcamModuleContext.cameraId,
                       CAMERA_LINK_CMD_DETECT_VIDEO, NULL, 0, TRUE);

    gIsGlbceInitDone = 0;
    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            /* NSF Link */
            System_linkCreate(gVcamModuleContext.nsfId, &nsfPrm, sizeof(nsfPrm));
        }
        else
        {
#ifdef USE_MCTNF
            if(IsMcTNFDemo() == 1)
            {
    #ifdef VNF_BEFORE_MCTNF
                System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
    #endif
                System_linkCreate(ipcOutVpssIdForMCTNF, &ipcOutVpssIdForMCTNF_params, sizeof(ipcOutVpssIdForMCTNF_params));
                System_linkCreate(ipcInVideoIdForMCTNF, &ipcInVideoIdForMCTNF_params, sizeof(ipcInVideoIdForMCTNF_params));
                System_linkCreate(gVcamModuleContext.mctnfId, &mctnfPrm, sizeof(mctnfPrm));
                System_linkCreate(ipcOutVideoIdForMCTNF, &ipcOutVideoIdForMCTNF_params, sizeof(ipcOutVideoIdForMCTNF_params));
                System_linkCreate(ipcInVpssIdForMCTNF, &ipcInVpssIdForMCTNF_params, sizeof(ipcInVpssIdForMCTNF_params));
    #ifndef VNF_BEFORE_MCTNF
                System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
    #endif
                gMctnfLinkCreated = 1;
            }
            else
#endif
            {
                /* Vnf Link */
                System_linkCreate(gVcamModuleContext.vnfId, &vnfPrm, sizeof(vnfPrm));
            }
        }
    }

    /* Merge Link */
    System_linkCreate(mergeId[CAM_STREAM_MERGE_IDX],
                      &mergePrm[CAM_STREAM_MERGE_IDX],
                      sizeof(mergePrm[CAM_STREAM_MERGE_IDX]));

    /* SWOSD Link */
    System_linkCreate(gVsysModuleContext.swOsdId, &swosdPrm, sizeof(swosdPrm));

    /* Dup and Display links */
    System_linkCreate(dupId, &dupPrm, sizeof(dupPrm));
    System_linkCreate(gVdisModuleContext.displayId[VDIS_DEV_HDMI], &displayPrm, sizeof(displayPrm));

    /* IPC Links */
    System_linkCreate(ipcOutVpssId, &ipcOutVpssPrm, sizeof(ipcOutVpssPrm));
    System_linkCreate(ipcInVideoId, &ipcInVideoPrm, sizeof(ipcInVideoPrm));

    /* Video Encoder Link */
    System_linkCreate(gVencModuleContext.encId, &encPrm, sizeof(encPrm));

    if((gUI_mcfw_config.vsEnable == TRUE))
    {
        System_linkCreate(vstabId, &vstabPrm, sizeof(vstabPrm));
    }

    /* IPC Bits Links */
    System_linkCreate(gVencModuleContext.ipcBitsOutRTOSId, &ipcBitsOutVideoPrm,
                      sizeof(ipcBitsOutVideoPrm));
    System_linkCreate(gVencModuleContext.ipcBitsInHLOSId, &ipcBitsInHostPrm0,
                      sizeof(ipcBitsInHostPrm0));

    /* display link */
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
Void MultiCh_deleteStreamDualStreamDemo()
{
    UInt32 ispId;
    UInt32 glbceId;

    UInt32 mergeId[NUM_MERGE_LINK];

    UInt32 ipcOutVpssId;

    UInt32 ipcInVideoId;


    UInt32 vstabId;
    UInt32 glbceSupportId;

    UInt32 dupId;

#ifdef USE_MCTNF
    UInt32 ipcOutVpssIdForMCTNF;
    UInt32 ipcInVideoIdForMCTNF;
    UInt32 ipcOutVideoIdForMCTNF;
    UInt32 ipcInVpssIdForMCTNF;
#endif

    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVsysModuleContext.fdId = SYSTEM_LINK_ID_FD;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;
    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;
    dupId = SYSTEM_VPSS_LINK_ID_DUP_0;

#ifdef USE_MCTNF
    if(gMctnfLinkCreated == 1)
    {
        ipcOutVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1;
        ipcInVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1;
        ipcOutVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
        ipcInVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
    }
#endif

    /* Links Deletion in reverse order from sink to source */

    /* IPC Bits Links */
    System_linkDelete(gVencModuleContext.ipcBitsOutRTOSId);
    System_linkDelete(gVencModuleContext.ipcBitsInHLOSId);

    /* Video Encoder Link */
    if((gUI_mcfw_config.vsEnable == TRUE))
    {
        System_linkDelete(vstabId);
    }
    System_linkDelete(gVencModuleContext.encId);

    /* IPC Links */
    System_linkDelete(ipcOutVpssId);
    System_linkDelete(ipcInVideoId);

    /* Merge Link */
    System_linkDelete(mergeId[CAM_STREAM_MERGE_IDX]);

    if(gUI_mcfw_config.vnfUseCase == TRUE)
    {
        if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
        {
            /* NSF Link */
            System_linkDelete(gVcamModuleContext.nsfId);
        }
        else
        {
#ifdef USE_MCTNF
            if(gMctnfLinkCreated == 1)
            {
    #ifndef VNF_BEFORE_MCTNF
                System_linkDelete(gVcamModuleContext.vnfId);
    #endif
                System_linkDelete(ipcInVpssIdForMCTNF);
                System_linkDelete(ipcOutVideoIdForMCTNF);
                System_linkDelete(gVcamModuleContext.mctnfId);
                System_linkDelete(ipcInVideoIdForMCTNF);
                System_linkDelete(ipcOutVpssIdForMCTNF);
    #ifdef VNF_BEFORE_MCTNF
                System_linkDelete(gVcamModuleContext.vnfId);
    #endif
            }
            else
#endif
            {
                /* VNF Link */
                System_linkDelete(gVcamModuleContext.vnfId);
            }
        }
    }

    if(gIsGlbceInitDone)
    {
        gIsGlbceInitDone = 0;
    }

    /* Camera Link */
    System_linkDelete(gVcamModuleContext.cameraId);

    /* Display and Dup links */
    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_HDMI]);
    System_linkDelete(dupId);

    /* SWOSD Link */
    System_linkDelete(gVsysModuleContext.swOsdId);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    OSA_printf("USECASE TEARDOWN DONE\n");
}


