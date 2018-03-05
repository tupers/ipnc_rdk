/** ==================================================================
 *  @file   multich_video_conf.c
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

/*------------------------------ ENC->A8->DEC ----------------------------------------------

                                    SENSOR
                               _______||________
                               |               |
                               |  CAMERA LINK  |
                               |___ (VPSSM3)___|
                                     RSZA
                                 (1920x1080)
                                   (420SP)
                                      |
                                      |
                                 DUP(VPSSM3) -------------|
                                      |                   |
                                      |                   |
                            IPC FRAMES OUT(VPSSM3)        |
                                      |                   |
                                      |                   |
                            IPC FRAMES IN(VIDEOM3)        |
                                      |                   |
                                      |                   |
                                 ENC(VIDEOM3)             |
                                      |                   |
                                      |                   |
                             IPC BITS OUT(VIDEOM3)        |
                                      |                   |
                                      |                   |
                                IPC BITS IN(A8)           |
                                      |                   |
                                      |___                |
                                          |               |
                                          |               |
                                   IPC BITS OUT(A8)       |
                                          |               |
                                          |               |
                                 IPC BITS IN(VIDEOM3)     |
                                          |               |
                                          |               |
                                     DEC(VIDEOM3)         |
                                          |               |
                                          |               |
                                IPC FRAMES OUT(VIDEOM3)   |
                                          |               |
                                          |               |
                                 IPC FRAME IN(VPSSM3)     |
                                          |               |
                                          |               |
                                     MERGE(VPSSM3)--------|
                                          |
                                          |
                                   SW MOSAIC(VPSSM3)
                                          |
                                          |
                                    DIS_HDMI(VPSSM3)

------------------------------------------------------------------------------------------*/

#include "mcfw/src_linux/mcfw_api/usecases/multich_common.h"
#include "mcfw/src_linux/mcfw_api/usecases/multich_ipcbits.h"
#include "demos/mcfw_api_demos/multich_usecase/ti_mcfw_ipnc_main.h"
#include "mcfw/src_linux/links/ipcBitsOut/ipcBitsOutLink_priv.h"
#include "mcfw/src_linux/osa/inc/osa_dma.h"

/* Define this to make encoded bitstream directly goto decoder bypassing A8 */
//#define DIR_DEC

#define TILER_ENABLE            FALSE
#define NUM_ENCODE_BUFFERS      4
#define NUM_DECODE_BUFFERS      4
#define NUM_IPC_OUT_BUFFERS     4
#define DEC_MIN_BUF_SIZE        (1920 * 1080)
#define IPC_BITS_TSK_PRI        (2)
#define IPC_BITS_TSK_STACK_SIZE (0)

/* Structure Definitions */
typedef struct
{
    OSA_ThrHndl hndl;
    UInt32 exitTh;
    UInt32 exitDone;

}Ipc_Bits_Th_Obj;

/* Global variables */
UInt32 gCameraId;
UInt32 gDupId;
UInt32 gIpcOutVpssId;
UInt32 gIpcInVideoId;
UInt32 gEncId;
UInt32 gIpcBitsOutVideoId;
UInt32 gIpcBitsInHLOSId;
UInt32 gIpcBitsOutHLOSId;
UInt32 gIpcBitsInVideoId;
UInt32 gDecId;
UInt32 gIpcOutVideoId;
UInt32 gIpcInVpssId;
UInt32 gMergeId;
UInt32 gSwMsId;
UInt32 gDisplayId;

static SystemVideo_Ivahd2ChMap_Tbl systemVid_encDecIvaChMapTbl = {
    .isPopulated = 1,
    .ivaMap[0] = {
                  .EncNumCh  = 2,
                  .EncChList = {0, 1},
                  .DecNumCh  = 2,
                  .DecChList = {0, 1},
                  },

};

OSA_DmaChHndl gDmaHndl;

Ipc_Bits_Th_Obj gIpcBitsThObj;
OSA_SemHndl     gIpcBitsNotifySem;

/* Function Prototypes */
unsigned long CMEM_getPhys(void *ptr);

/* ===================================================================
 *  @func     MultiCh_ipcBitsInHostCb
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
Void MultiCh_ipcBitsInHostCb(Ptr cbCtx)
{
    OSA_semSignal(&gIpcBitsNotifySem);
}

/* ===================================================================
 *  @func     MultiCh_ipcBitsMain
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
Void *MultiCh_ipcBitsMain(Void *prm)
{
    UInt32 i;
    OSA_DmaCopy1D copy1D;
    Bitstream_BufList fullBitsBufList;
    Bitstream_BufList emptyBitsBufList;
    IpcBitsOutLinkHLOS_BitstreamBufReqInfo ipcReqInfo;

    OSA_printf("Entered IPC Bits Handler function\n");
    gIpcBitsThObj.exitTh   = FALSE;
    gIpcBitsThObj.exitDone = FALSE;

    while(gIpcBitsThObj.exitTh == FALSE)
    {
        OSA_semWait(&gIpcBitsNotifySem,OSA_TIMEOUT_FOREVER);

        IpcBitsInLink_getFullVideoBitStreamBufs(gIpcBitsInHLOSId,&fullBitsBufList);

        if(fullBitsBufList.numBufs > 0)
        {
            for(i = 0;i < fullBitsBufList.numBufs;i++)
            {
                ipcReqInfo.numBufs       = 1;
                ipcReqInfo.minBufSize[0] = DEC_MIN_BUF_SIZE;

                /* Wait till empty buffer is got from IPC Bits Out link */
                do
                {
                    IpcBitsOutLink_getEmptyVideoBitStreamBufs(gIpcBitsOutHLOSId,&emptyBitsBufList,&ipcReqInfo);
                    OSA_waitMsecs(10);
                }while(emptyBitsBufList.numBufs == 0);

                /* Copy bitstream */
                emptyBitsBufList.numBufs = 1;
                emptyBitsBufList.bufs[0]->channelNum = fullBitsBufList.bufs[i]->channelNum;
                emptyBitsBufList.bufs[0]->fillLength = fullBitsBufList.bufs[i]->fillLength;
                emptyBitsBufList.bufs[0]->timeStamp  = fullBitsBufList.bufs[i]->timeStamp;

                if(gDmaHndl.chId == 0xFF)
                {
                    memcpy(emptyBitsBufList.bufs[0]->addr,
                           (fullBitsBufList.bufs[i]->addr + fullBitsBufList.bufs[i]->startOffset),
                           fullBitsBufList.bufs[i]->fillLength);
                }
                else
                {
                    copy1D.srcPhysAddr = (unsigned long)CMEM_getPhys(fullBitsBufList.bufs[i]->addr + fullBitsBufList.bufs[i]->startOffset);
                    copy1D.dstPhysAddr = (unsigned long)CMEM_getPhys(emptyBitsBufList.bufs[0]->addr);
                    copy1D.size        = fullBitsBufList.bufs[i]->fillLength;
                    OSA_dmaCopy1D(&gDmaHndl,&copy1D,1);
                }

                IpcBitsOutLink_putFullVideoBitStreamBufs(gIpcBitsOutHLOSId,&emptyBitsBufList);
            }
        }

        IpcBitsInLink_putEmptyVideoBitStreamBufs(gIpcBitsInHLOSId,&fullBitsBufList);
    }

    gIpcBitsThObj.exitDone = TRUE;

    OSA_printf("Exiting IPC Bits Handler function\n");

    return NULL;
}

/* ===================================================================
 *  @func     MultiCh_createVideoConf
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
Void MultiCh_createEncA8Dec()
{
    UInt32 i;
    Int32 status;
    CameraLink_CreateParams         cameraPrm;
    DupLink_CreateParams            dupPrm;
    IpcLink_CreateParams            ipcOutVpssPrm;
    IpcLink_CreateParams            ipcInVideoPrm;
    EncLink_CreateParams            encPrm;
    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;
    IpcBitsInLinkHLOS_CreateParams  ipcBitsInHostPrm;

    IpcBitsOutLinkHLOS_CreateParams ipcBitsOutHostPrm;
    IpcBitsInLinkRTOS_CreateParams  ipcBitsInVideoPrm;
    DecLink_CreateParams            decPrm;
    IpcLink_CreateParams            ipcOutVideoPrm;
    IpcLink_CreateParams            ipcInVpssPrm;
    MergeLink_CreateParams          mergePrm;
    SwMsLink_CreateParams           swMsPrm;
    DisplayLink_CreateParams        displayPrm;

    CameraLink_VipInstParams *pCameraInstPrm;
    CameraLink_OutParams     *pCameraOutPrm;
    System_LinkInfo          bitsProducerLinkInfo;

    /* Semaphore for IPC Bits */
    OSA_semCreate(&gIpcBitsNotifySem,1,0);

    /* IPC Bits handling thread */
    OSA_thrCreate(&gIpcBitsThObj.hndl,                   // hndl
                  MultiCh_ipcBitsMain,                   // entryFunc
                  IPC_BITS_TSK_PRI,                      // pri
                  IPC_BITS_TSK_STACK_SIZE,               // stackSize
                  NULL);                                 // prm

    /* DMA for memcpy */
    status = OSA_dmaInit();
    if(status == 0)
    {
        status = OSA_dmaOpen(&gDmaHndl,OSA_DMA_MODE_NORMAL,1);
        if (status != 0)
        {
            OSA_printf("%s:OSA_dmaOpen failed - %d\n", __func__, status);
            gDmaHndl.chId = 0xFF;
        }
        else
        {
            OSA_printf("%s:OSA_dmaOpen passed with ch id = %d\n", __func__,gDmaHndl.chId);
        }
    }
    else
    {
        OSA_printf("%s:OSA_dmaInit failed - %d\n", __func__, status);
        gDmaHndl.chId = 0xFF;
    }

    /* IPC structures init */
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcInVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkRTOS_CreateParams,ipcBitsOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkHLOS_CreateParams,ipcBitsInHostPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkHLOS_CreateParams,ipcBitsOutHostPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkRTOS_CreateParams,ipcBitsInVideoPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcInVpssPrm);

    OSA_printf("\n********** ENC A8 DEC USE CASE ********\n");
    OSA_printf("\n********* Entered Encode + Decode usecase - H264 1080p @30fps Encode + H264 1080p @30fps Decode ********\n");

    MultiCh_detectBoard();

    System_linkControl(SYSTEM_LINK_ID_M3VPSS,
                       SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES, NULL, 0, TRUE);

    System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
                       SYSTEM_COMMON_CMD_SET_CH2IVAHD_MAP_TBL,
                       &systemVid_encDecIvaChMapTbl,
                       sizeof(SystemVideo_Ivahd2ChMap_Tbl), TRUE);
    /* Link IDs */
    gCameraId          = SYSTEM_LINK_ID_CAMERA;
    gDupId             = SYSTEM_VPSS_LINK_ID_DUP_0;
    gIpcOutVpssId      = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    gIpcInVideoId      = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    gEncId             = SYSTEM_LINK_ID_VENC_0;
    gIpcBitsOutVideoId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gIpcBitsInHLOSId   = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;

    gIpcBitsOutHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_OUT_0;
    gIpcBitsInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_IN_0;
    gDecId            = SYSTEM_LINK_ID_VDEC_0;
    gIpcOutVideoId    = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
    gIpcInVpssId      = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
    gMergeId          = SYSTEM_VPSS_LINK_ID_MERGE_0;
    gSwMsId           = SYSTEM_LINK_ID_SW_MS_MULTI_INST_0;
    gDisplayId        = SYSTEM_LINK_ID_DISPLAY_0;

    gVcamModuleContext.cameraId               = gCameraId;
    gVencModuleContext.encId                  = gEncId;
    gVencModuleContext.ipcBitsInHLOSId        = gIpcBitsInHLOSId;
    gVencModuleContext.ipcBitsOutRTOSId       = gIpcBitsOutVideoId;
    gVdecModuleContext.decId                  = gDecId;
    gVdecModuleContext.ipcBitsOutHLOSId       = gIpcBitsOutHLOSId;
    gVdecModuleContext.ipcBitsInRTOSId        = gIpcBitsInVideoId;
    gVdisModuleContext.displayId[VDIS_DEV_SD] = gDisplayId;
    gVdisModuleContext.swMsId[0]              = gSwMsId;

    /* Camera Link */
    CameraLink_CreateParams_Init(&cameraPrm);

    cameraPrm.captureMode              = CAMERA_LINK_CAPMODE_ISIF;
    cameraPrm.numAudioChannels         = 1;
    cameraPrm.numVipInst               = 1;
    cameraPrm.tilerEnable              = FALSE;
    cameraPrm.vsEnable                 = FALSE;
    cameraPrm.outQueParams[0].nextLink = gDupId;
    cameraPrm.t2aConfig.n2A_vendor     = gUI_mcfw_config.n2A_vendor;
    cameraPrm.t2aConfig.n2A_mode       = gUI_mcfw_config.n2A_mode;

    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->vipInstId       = SYSTEM_CAMERA_INST_VP_CSI2;
    pCameraInstPrm->videoDecoderId  = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
    pCameraInstPrm->inDataFormat    = SYSTEM_DF_YUV420SP_UV;
    pCameraInstPrm->sensorOutWidth  = 1920;
    pCameraInstPrm->sensorOutHeight = 1080;
    pCameraInstPrm->standard        = SYSTEM_STD_1080P_30;
    pCameraInstPrm->numOutput       = 1;

    pCameraOutPrm = &pCameraInstPrm->outParams[0];
    pCameraOutPrm->dataFormat  = SYSTEM_DF_YUV420SP_UV;
    pCameraOutPrm->scEnable    = FALSE;
    pCameraOutPrm->scOutWidth  = 1920;
    pCameraOutPrm->scOutHeight = 1080;
    pCameraOutPrm->outQueId    = 0;

    /* DUP Link */
    dupPrm.inQueParams.prevLinkId    = gCameraId;
    dupPrm.inQueParams.prevLinkQueId = 0;
    dupPrm.numOutQue                 = 2;
    dupPrm.outQueParams[0].nextLink  = gIpcOutVpssId;
    dupPrm.outQueParams[1].nextLink  = gMergeId;
    dupPrm.notifyNextLink            = TRUE;

    /* IPC FRAMES OUT VPSS Link */
    ipcOutVpssPrm.inQueParams.prevLinkId    = gDupId;
    ipcOutVpssPrm.inQueParams.prevLinkQueId = 0;
    ipcOutVpssPrm.numOutQue                 = 1;
    ipcOutVpssPrm.outQueParams[0].nextLink  = gIpcInVideoId;
    ipcOutVpssPrm.notifyNextLink            = TRUE;
    ipcOutVpssPrm.notifyPrevLink            = TRUE;
    ipcOutVpssPrm.noNotifyMode              = FALSE;

    /* IPC FRAMES IN VIDEO Link */
    ipcInVideoPrm.inQueParams.prevLinkId    = gIpcOutVpssId;
    ipcInVideoPrm.inQueParams.prevLinkQueId = 0;
    ipcInVideoPrm.numOutQue                 = 1;
    ipcInVideoPrm.outQueParams[0].nextLink  = gEncId;
    ipcInVideoPrm.notifyNextLink            = TRUE;
    ipcInVideoPrm.notifyPrevLink            = TRUE;
    ipcInVideoPrm.noNotifyMode              = FALSE;

    /* ENC link */
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
            pLinkChPrm->enableAnalyticinfo      = pChPrm->enableAnalyticinfo;
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

#ifdef DIR_DEC
    encPrm.outQueParams.nextLink                                 = gDecId;
#else
    encPrm.outQueParams.nextLink                                 = gIpcBitsOutVideoId;
#endif

    /* IPC BITS OUT VIDEO Link */
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkId    = gEncId;
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcBitsOutVideoPrm.baseCreateParams.numOutQue                 = 1;
    ipcBitsOutVideoPrm.baseCreateParams.outQueParams[0].nextLink  = gIpcBitsInHLOSId;
    ipcBitsOutVideoPrm.baseCreateParams.noNotifyMode              = FALSE;
    ipcBitsOutVideoPrm.baseCreateParams.notifyNextLink            = TRUE;
    ipcBitsOutVideoPrm.baseCreateParams.notifyPrevLink            = TRUE;

    /* IPC BITS IN HLOS Link */
    ipcBitsInHostPrm.baseCreateParams.inQueParams.prevLinkId    = gIpcBitsOutVideoId;
    ipcBitsInHostPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcBitsInHostPrm.baseCreateParams.noNotifyMode              = FALSE;
    ipcBitsInHostPrm.baseCreateParams.notifyNextLink            = TRUE;
    ipcBitsInHostPrm.baseCreateParams.notifyPrevLink            = TRUE;
    ipcBitsInHostPrm.cbFxn                                      = MultiCh_ipcBitsInHostCb;
    ipcBitsInHostPrm.cbCtx                                      = NULL;

    /* Link creation */
    System_linkCreate(gCameraId,&cameraPrm,sizeof(CameraLink_CreateParams));
    System_linkControl(gCameraId,CAMERA_LINK_CMD_DETECT_VIDEO,NULL,0,TRUE);

    System_linkCreate(gDupId,&dupPrm,sizeof(DupLink_CreateParams));
    System_linkCreate(gIpcOutVpssId,&ipcOutVpssPrm,sizeof(IpcLink_CreateParams));
    System_linkCreate(gIpcInVideoId,&ipcInVideoPrm,sizeof(IpcLink_CreateParams));
    System_linkCreate(gEncId,&encPrm,sizeof(EncLink_CreateParams));

#ifdef DIR_DEC
    System_linkGetInfo(gEncId,&bitsProducerLinkInfo);
#else
    System_linkCreate(gIpcBitsOutVideoId,&ipcBitsOutVideoPrm,sizeof(IpcBitsOutLinkRTOS_CreateParams));
    System_linkCreate(gIpcBitsInHLOSId,&ipcBitsInHostPrm,sizeof(IpcBitsInLinkHLOS_CreateParams));
    System_linkGetInfo(gIpcBitsInHLOSId,&bitsProducerLinkInfo);
#endif

    /* IPC BITS OUT HLOS Link */
    ipcBitsOutHostPrm.baseCreateParams.outQueParams[0].nextLink = gIpcBitsInVideoId;
    ipcBitsOutHostPrm.baseCreateParams.notifyNextLink           = TRUE;
    ipcBitsOutHostPrm.baseCreateParams.notifyPrevLink           = TRUE;
    ipcBitsOutHostPrm.numBufPerCh[0]                            = NUM_IPC_OUT_BUFFERS;
    ipcBitsOutHostPrm.inQueInfo                                 = bitsProducerLinkInfo.queInfo[0];

    /* IPC BITS IN VIDEO Link */
    ipcBitsInVideoPrm.baseCreateParams.inQueParams.prevLinkId    = gIpcBitsOutHLOSId;
    ipcBitsInVideoPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcBitsInVideoPrm.baseCreateParams.numOutQue                 = 1;
    ipcBitsInVideoPrm.baseCreateParams.outQueParams[0].nextLink  = gDecId;
    ipcBitsInVideoPrm.baseCreateParams.noNotifyMode              = FALSE;
    ipcBitsInVideoPrm.baseCreateParams.notifyNextLink            = TRUE;
    ipcBitsInVideoPrm.baseCreateParams.notifyPrevLink            = TRUE;

    /* DEC Link */
    DecLink_CreateParams_Init(&decPrm);
    for (i = 0; i < bitsProducerLinkInfo.queInfo[0].numCh; i++)
    {
        decPrm.chCreateParams[i].format                               = IVIDEO_H264HP;
        decPrm.chCreateParams[i].profile                              = IH264VDEC_PROFILE_ANY;
        decPrm.chCreateParams[i].fieldMergeDecodeEnable               = FALSE;
        decPrm.chCreateParams[i].targetMaxWidth                       = bitsProducerLinkInfo.queInfo[0].chInfo[i].width;
        decPrm.chCreateParams[i].targetMaxHeight                      = bitsProducerLinkInfo.queInfo[0].chInfo[i].height;
        decPrm.chCreateParams[i].defaultDynamicParams.targetFrameRate = encPrm.chCreateParams[i].defaultDynamicParams.inputFrameRate;
        decPrm.chCreateParams[i].defaultDynamicParams.targetBitRate   = gVdecModuleContext.vdecConfig.decChannelParams[i].dynamicParam.targetBitRate;

        /* Max ref frames is only 2 as this is closed loop decoder */
        decPrm.chCreateParams[i].dpbBufSizeInFrames = 2;
    }

#ifdef DIR_DEC
    decPrm.inQueParams.prevLinkId    = gEncId;
#else
    decPrm.inQueParams.prevLinkId    = gIpcBitsInVideoId;
#endif
    decPrm.inQueParams.prevLinkQueId = 0;
    decPrm.outQueParams.nextLink     = gIpcOutVideoId;
    decPrm.tilerEnable               = TILER_ENABLE;
    decPrm.numBufPerPool[0]          = NUM_DECODE_BUFFERS;

    /* FRAMES OUT VIDEO Link */
    ipcOutVideoPrm.inQueParams.prevLinkId    = gDecId;
    ipcOutVideoPrm.inQueParams.prevLinkQueId = 0;
    ipcOutVideoPrm.numOutQue                 = 1;
    ipcOutVideoPrm.outQueParams[0].nextLink  = gIpcInVpssId;
    ipcOutVideoPrm.notifyNextLink            = TRUE;
    ipcOutVideoPrm.notifyPrevLink            = TRUE;
    ipcOutVideoPrm.noNotifyMode              = FALSE;

    /* FRAMES IN VPSS Link */
    ipcInVpssPrm.inQueParams.prevLinkId    = gIpcOutVideoId;
    ipcInVpssPrm.inQueParams.prevLinkQueId = 0;
    ipcInVpssPrm.numOutQue                 = 1;
    ipcInVpssPrm.outQueParams[0].nextLink  = gMergeId;
    ipcInVpssPrm.notifyNextLink            = TRUE;
    ipcInVpssPrm.notifyPrevLink            = TRUE;
    ipcInVpssPrm.noNotifyMode              = FALSE;

    /* MERGE Link */
    mergePrm.numInQue                     = 2;
    mergePrm.inQueParams[0].prevLinkId    = gDupId;
    mergePrm.inQueParams[0].prevLinkQueId = 1;
    mergePrm.inQueParams[1].prevLinkId    = gIpcInVpssId;
    mergePrm.inQueParams[1].prevLinkQueId = 0;
    mergePrm.outQueParams.nextLink        = gSwMsId;
    mergePrm.notifyNextLink               = TRUE;

    /* SW MOSAIC Link */
    SwMsLink_CreateParams_Init(&swMsPrm);
    swMsPrm.numSwMsInst                 = 1;
    swMsPrm.swMsInstId[0]               = SYSTEM_SW_MS_SC_INST_VIP0_SC;
    swMsPrm.swMsInstStartWin[0]         = 0;
    swMsPrm.inQueParams.prevLinkId      = gMergeId;
    swMsPrm.inQueParams.prevLinkQueId   = 0;
    swMsPrm.outQueParams.nextLink       = gDisplayId;
    swMsPrm.maxInputQueLen              = SYSTEM_SW_MS_DEFAULT_INPUT_QUE_LEN;
    swMsPrm.maxOutRes                   = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    swMsPrm.lineSkipMode                = FALSE;
    swMsPrm.enableLayoutGridDraw        = gVdisModuleContext.vdisConfig.enableLayoutGridDraw;
    swMsPrm.enableOuputDup              = FALSE;
    swMsPrm.enableProcessTieWithDisplay = FALSE;
    swMsPrm.outDataFormat               = SYSTEM_DF_YUV422I_YUYV;

    MultiCh_swMsGetDefaultLayoutPrm(SYSTEM_DC_VENC_HDMI,&swMsPrm,FALSE);

    /* DISPLAY Link */
    MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
    displayPrm.inQueParams[0].prevLinkId    = gSwMsId;
    displayPrm.inQueParams[0].prevLinkQueId = 0;
    displayPrm.displayRes                   = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    displayPrm.displayId                    = DISPLAY_LINK_DISPLAY_SC2;

    /* Link creation */
#ifndef DIR_DEC
    System_linkCreate(gIpcBitsOutHLOSId,&ipcBitsOutHostPrm,sizeof(IpcBitsOutLinkHLOS_CreateParams));
    System_linkCreate(gIpcBitsInVideoId,&ipcBitsInVideoPrm,sizeof(IpcBitsInLinkRTOS_CreateParams));
#endif
    System_linkCreate(gDecId,&decPrm,sizeof(DecLink_CreateParams));
    System_linkCreate(gIpcOutVideoId,&ipcOutVideoPrm,sizeof(IpcLink_CreateParams));
    System_linkCreate(gIpcInVpssId,&ipcInVpssPrm,sizeof(IpcLink_CreateParams));
    System_linkCreate(gMergeId,&mergePrm,sizeof(MergeLink_CreateParams));
    System_linkCreate(gSwMsId,&swMsPrm,sizeof(SwMsLink_CreateParams));
    System_linkCreate(gDisplayId,&displayPrm,sizeof(DisplayLink_CreateParams));

    OSA_printf("USECASE SETUP DONE\n");
}

/* ===================================================================
 *  @func     MultiCh_deleteVideoConf
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
Void MultiCh_deleteEncA8Dec()
{
    /* Deleting the links in reverse order */
    System_linkDelete(gDisplayId);
    System_linkDelete(gSwMsId);
    System_linkDelete(gMergeId);
    System_linkDelete(gIpcInVpssId);
    System_linkDelete(gIpcOutVideoId);
    System_linkDelete(gDecId);
#ifndef DIR_DEC
    System_linkDelete(gIpcBitsInVideoId);
    System_linkDelete(gIpcBitsOutHLOSId);
    System_linkDelete(gIpcBitsInHLOSId);
    System_linkDelete(gIpcBitsOutVideoId);
#endif
    System_linkDelete(gEncId);
    System_linkDelete(gIpcInVideoId);
    System_linkDelete(gIpcOutVpssId);
    System_linkDelete(gDupId);
    System_linkDelete(gCameraId);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    OSA_dmaClose(&gDmaHndl);

    gIpcBitsThObj.exitTh = TRUE;
    OSA_semSignal(&gIpcBitsNotifySem);
    while(gIpcBitsThObj.exitDone == FALSE)
    {
        OSA_waitMsecs(1000);
    }
    OSA_thrDelete(&gIpcBitsThObj.hndl);
    OSA_semDelete(&gIpcBitsNotifySem);

    OSA_printf("USECASE TEARDOWN DONE\n");
}

