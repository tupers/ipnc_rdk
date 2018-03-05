/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "system_priv_m3vpss.h"
#include <mcfw/src_bios6/utils/utils_tiler_allocator.h>
#include <mcfw/src_bios6/utils/utils_dmtimer.h>
#include <mcfw/src_bios6/utils/utils_dma.h>
#include <ti/psp/devices/vps_device.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/drivers/iss_init.h>
#include <ti/psp/devices/tvp5158/vpsdrv_tvp5158.h>
#include <ti/psp/devices/tvp5158/vpsdrv_tvp5158.h>
#include <mcfw/interfaces/link_api/swosdLink.h>
#include <mcfw/interfaces/link_api/fdLink.h>
#include <mcfw/interfaces/link_api/ispLink.h>
#include <mcfw/interfaces/link_api/vaLink.h>
#include <mcfw/interfaces/link_api/mpSclrLink.h>
#include <mcfw/interfaces/link_api/rotateFrameLink.h>

System_VpssObj gSystem_objVpss;

volatile int waitFlag = 0;

void wait (void)
{
    while (waitFlag == 1) ;
}

Int32 System_init()
{
    Int32 status;

#ifdef IPNC_DSS_ON
#ifdef SYSTEM_DEBUG
    const Char *versionStr;
#endif
#endif

#ifdef IPNC_DSS_ON
    Vps_PlatformDeviceInitParams initPrms;
#endif
    Semaphore_Params semParams;
#ifdef IPNC_DSS_ON
    Vps_PlatformInitParams platformInitPrms;
#endif
    Iss_PlatformDeviceInitParams issInitPrms;

    Iss_PlatformInitParams issPlatInitPrms;

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System VPSS Init in progress !!!\n",
               Utils_getCurTimeInMsec());
#endif
    System_dispCheckStopList();

    Utils_tilerAllocatorInit();

#ifdef SYSTEM_DEBUG_TILER_ALLOC
    Utils_tilerDebugLogEnable(TRUE);
#endif

#if defined(TI_814X_BUILD)
    /* Need to set this bit only for ti814x to support tied vencs, pin mux settings */
    /* Need to call this before accessing HD VENC registers in FVID2_init()
        Other wise it may hang in access of HDVENC registers
    */
    (* (UInt32 *)0x481C52C8) = 0x01000000;
#endif

#ifdef IPNC_DSS_ON
    platformInitPrms.isPinMuxSettingReq = TRUE;
    status = Vps_platformInit(&platformInitPrms);
    UTILS_assert(status == 0);
#endif

    issPlatInitPrms.isPinMuxSettingReq = TRUE;
    status = Iss_platformInit(&issPlatInitPrms);
    UTILS_assert(status == 0);

    Utils_dmTimerInit();
    IpcOutM3Link_init();
    IpcInM3Link_init();
    IpcFramesInLink_init();
    IpcFramesOutLink_init();
    IpcBitsInLink_init();
    IpcBitsOutLink_init();

#ifdef IPNC_DSS_ON
#ifdef SYSTEM_DEBUG
    /*
     * Get the version string
     */
    versionStr = FVID2_getVersionString();
    Vps_printf(" %d: SYSTEM : HDVPSS Drivers Version: %s\n",
               Utils_getCurTimeInMsec(), versionStr);
#endif

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : FVID2 Init in progress !!!\n",
               Utils_getCurTimeInMsec());
#endif
    /*
     * FVID2 system init
     */
    status = FVID2_init(NULL);
    UTILS_assert(status == 0);

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : FVID2 Init in progress DONE !!!\n",
               Utils_getCurTimeInMsec());
#endif
#endif /* IPNC_DSS_ON*/

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : Device Init in progress !!!\n",
               Utils_getCurTimeInMsec());
#endif

    status = Iss_init(NULL);
    UTILS_assert(status == 0);

#ifdef IPNC_DSS_ON
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
  #ifdef TI814X_EVM_WITHOUT_VS_CARD
    initPrms.isI2cInitReq = FALSE;
    initPrms.isI2cProbingReq = FALSE;
  #else
    initPrms.isI2cInitReq = TRUE;
    initPrms.isI2cProbingReq = FALSE;
  #endif
#else
    initPrms.isI2cInitReq = TRUE;
    initPrms.isI2cProbingReq = FALSE;
#endif

#ifndef _IPNC_HW_PLATFORM_
#ifndef SYSTEM_USE_VIDEO_DECODER
    initPrms.isI2cInitReq = FALSE;
    initPrms.isI2cProbingReq = FALSE;
#endif
#endif // #ifndef IPNC

#ifdef TI814X_EVM
    initPrms.isI2cInitReq = TRUE;
    initPrms.isI2cProbingReq = TRUE;
#endif

    Vps_printf("\ninitPrms.isI2cInitReq = %d\n", initPrms.isI2cInitReq);
    Vps_printf("\ninitPrms.isI2cInitReq = %d\n", initPrms.isI2cProbingReq);
    status = Vps_platformDeviceInit(&initPrms);
    UTILS_assert(status == 0);
#endif /* IPNC_DSS_ON */

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : Device Init in progress DONE !!!\n",
               Utils_getCurTimeInMsec());
#endif

#ifdef TI816X_DVR
    status = Vps_tvp5158Init();
#endif

    issInitPrms.isI2cInitReq = TRUE;
    status = Iss_platformDeviceInit(&issInitPrms);
    UTILS_assert(status == 0);

#ifdef SYSTEM_DEBUG_VIP_RES_ALLOC
   /*
     * enable logs from VIP resource allocator
     */
   Vcore_vipResDebugLogEnable(TRUE);
#endif

   Semaphore_Params_init(&semParams);
   semParams.mode = Semaphore_Mode_BINARY;

   gSystem_objVpss.vipLock[SYSTEM_VIP_0] =
        Semaphore_create(1u, &semParams, NULL);
   UTILS_assert(gSystem_objVpss.vipLock[SYSTEM_VIP_0] != NULL);

   Semaphore_Params_init(&semParams);
   semParams.mode = Semaphore_Mode_BINARY;

   gSystem_objVpss.vipLock[SYSTEM_VIP_1] =
       Semaphore_create(1u, &semParams, NULL);
   UTILS_assert(gSystem_objVpss.vipLock[SYSTEM_VIP_1] != NULL);

   System_clearVipResetFlag(SYSTEM_VIP_0);
   System_clearVipResetFlag(SYSTEM_VIP_1);

   System_allocBlankFrame();

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System VPSS Init Done !!!\n", Utils_getCurTimeInMsec());
#endif

   Utils_dmaInit();

   System_initLinks();

   return status;
}


Int32 System_deInit()
{
   Int32 status = FVID2_SOK;
   IpcInM3Link_deInit();
   IpcOutM3Link_deInit();
   IpcFramesInLink_deInit();
   IpcFramesOutLink_deInit();
   IpcBitsInLink_deInit();
   IpcBitsOutLink_deInit();

   Utils_dmaDeInit();

   Utils_dmTimerDeInit();
   System_deInitLinks();

#ifdef SYSTEM_DEBUG
   Vps_printf(" %d: SYSTEM  : System VPSS De-Init in progress !!!\n", Utils_getCurTimeInMsec());
#endif

   System_freeBlankFrame();

   Semaphore_delete(&gSystem_objVpss.vipLock[SYSTEM_VIP_0]);
   Semaphore_delete(&gSystem_objVpss.vipLock[SYSTEM_VIP_1]);

#ifdef SYSTEM_DEBUG
   Vps_printf(" %d: SYSTEM  : Platform Device De-init in progress !!!\n", Utils_getCurTimeInMsec());
#endif

#ifdef TI816X_DVR
   status = Vps_tvp5158DeInit();
   UTILS_assert(status == FVID2_SOK);
#endif

#ifdef IPNC_DSS_ON
   Vps_platformDeviceDeInit();
#endif /* IPNC_DSS_ON */

   Iss_platformDeviceDeInit();
	Iss_deInit(NULL);
#ifdef IPNC_DSS_ON
#ifdef SYSTEM_DEBUG
   Vps_printf(" %d: SYSTEM  : FVID2 De-init in progress !!!\n", Utils_getCurTimeInMsec());
#endif

   /*
     * FVID2 system de-init
     */
   FVID2_deInit(NULL);

#ifdef SYSTEM_DEBUG
   Vps_printf(" %d: SYSTEM  : Platform De-init in progress !!!\n", Utils_getCurTimeInMsec());
#endif

   Vps_platformDeInit();
#endif /* IPNC_DSS_ON */

   Iss_platformDeInit();

#ifdef SYSTEM_USE_TILER
   Utils_tilerAllocatorDeInit();
#endif

#ifdef SYSTEM_DEBUG
   Vps_printf(" %d: SYSTEM  : System VPSS De-Init Done !!!\n",

              Utils_getCurTimeInMsec());
#endif

   return status;
}


Void System_initLinks()
{

   Vps_printf(" %d: SYSTEM  : Initializing Links !!! \r\n", Utils_getCurTimeInMsec());
   System_memPrintHeapStatus();

   CaptureLink_init();
   CameraLink_init();
   MjpegLink_init();
   IspLink_init();
   SwosdLink_init();
   RotateFrameLink_init();
#ifdef IPNC_DSS_ON
   NsfLink_init();
   DeiLink_init();
   DisplayLink_init();
   NullLink_init();
   NullSrcLink_init();
#endif /* IPNC_DSS_ON */

   DupLink_init();
#ifdef IPNC_DSS_ON   
   SclrLink_init();
   SwMsLink_init();
#endif   
   MergeLink_init();
   SelectLink_init();
#ifdef IPNC_DSS_ON   
   FdLink_init();
#endif   
   GlbceLink_init();
   WdrLink_init();

    VnfLink_init();
	
	VaLink_init();

#if AVSYNC_COMP_ENABLE
    Vps_printf(" %d: SYSTEM  : Initializing AVsync ********************** !!! \r\n", Utils_getCurTimeInMsec());
    AVSYNC_M3_Init();
#endif

    MuxLink_init();
    
    MpSclrLink_init();

   Vps_printf(" %d: SYSTEM  : Initializing Links ... DONE !!! \r\n", Utils_getCurTimeInMsec());
}


Void System_deInitLinks()
{


   Vps_printf(" %d: SYSTEM  : De-Initializing Links !!! \r\n", Utils_getCurTimeInMsec());

   SelectLink_deInit();
   MergeLink_deInit();
   RotateFrameLink_deInit();
#ifdef IPNC_DSS_ON
   NullLink_deInit();
   DisplayLink_deInit();
   NullSrcLink_deInit();
   DeiLink_deInit();
   NsfLink_deInit();
#endif /* IPNC_DSS_ON */

   CaptureLink_deInit();
   CameraLink_deInit();
   IspLink_deInit();
   SwosdLink_deInit();
   DupLink_deInit();
#ifdef IPNC_DSS_ON   
   SclrLink_deInit();
   SwMsLink_deInit();
#endif   
   GlbceLink_deInit();
   WdrLink_deInit();

#ifdef IPNC_DSS_ON
   FdLink_deInit();
#endif /* IPNC_DSS_ON */
   VnfLink_deInit();
   
   VaLink_deInit();

   System_memPrintHeapStatus();

#if AVSYNC_COMP_ENABLE
    Vps_printf(" %d: SYSTEM  : De-Initializing Links ...  avsync !!! \r\n", Utils_getCurTimeInMsec());
    AVSYNC_M3_DeInit();
#endif

    MuxLink_deInit();
    
    MpSclrLink_deInit();

    Vps_printf(" %d: SYSTEM  : De-Initializing Links ... DONE !!! \r\n", Utils_getCurTimeInMsec());
}


Int32 System_lockVip(UInt32 vipInst)
{
   if (vipInst < SYSTEM_VIP_MAX)
   {
       Semaphore_pend(gSystem_objVpss.vipLock[vipInst], BIOS_WAIT_FOREVER);
   }

   return FVID2_SOK;
}


Int32 System_unlockVip(UInt32 vipInst)
{

   if (vipInst < SYSTEM_VIP_MAX)
   {
       Semaphore_post(gSystem_objVpss.vipLock[vipInst]);
   }

   return FVID2_SOK;
}


Int32 System_setVipResetFlag(UInt32 vipInst)
{

   if (vipInst < SYSTEM_VIP_MAX)
   {
       gSystem_objVpss.vipResetFlag[vipInst] = TRUE;
   }

   return FVID2_SOK;
}


Bool System_clearVipResetFlag(UInt32 vipInst)
{

   Bool isReset = FALSE;

   if (vipInst < SYSTEM_VIP_MAX)
   {
       isReset = gSystem_objVpss.vipResetFlag[vipInst];
       gSystem_objVpss.vipResetFlag[vipInst] = FALSE;
   }

   return isReset;

}


Int32 System_videoResetVideoDevices()
{

   Int32 status = FVID2_SOK;

#ifndef TI8107_DVR
        status = Vps_platformVideoResetVideoDevices();
#endif

   return status;
}


UInt8 System_getVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId)
{

   UInt8 devAddr = NULL;

#ifdef TI814X_DVR
   UInt8 devAddrTvp5158[VPS_CAPT_INST_MAX] = { 0x58, 0x59, 0x5a, 0x5b };

   switch (vidDecId)
   {
       case FVID2_VPS_VID_DEC_TVP5158_DRV:
           devAddr = devAddrTvp5158[vipInstId];
           break;

       default:
           break;
   }
#endif

#ifdef TI816X_DVR
   UInt8 devAddrTvp5158[VPS_CAPT_INST_MAX] = { 0x5c, 0x5d, 0x5e, 0x5f };
   UInt8 devAddrSii9135[VPS_CAPT_INST_MAX] = { 0x31, 0x00, 0x30, 0x00 };
   UInt8 devAddrTvp7002[VPS_CAPT_INST_MAX] = { 0x5d, 0x00, 0x5c, 0x00 };

   // GT_assert( GT_DEFAULT_MASK, vipInstId<VPS_CAPT_INST_MAX);

   switch (vidDecId)
   {
       case FVID2_VPS_VID_DEC_TVP5158_DRV:
           devAddr = devAddrTvp5158[vipInstId];
           break;

       case FVID2_VPS_VID_DEC_SII9135_DRV:
           devAddr = devAddrSii9135[vipInstId];
           break;

       case FVID2_VPS_VID_DEC_TVP7002_DRV:
           devAddr = devAddrTvp7002[vipInstId];
           break;

       default:
           break;
           // GT_assert( GT_DEFAULT_MASK, 0);
   }
#endif

   devAddr = Vps_platformGetVidDecI2cAddr(vidDecId, vipInstId);

   return (devAddr);

}


System_PlatformBoardRev System_getBaseBoardRev()
{
#if defined (TI816X_DVR)
   return SYSTEM_PLATFORM_BOARD_DVR_REV_NONE;
#else
   return (System_PlatformBoardRev)(Vps_platformGetBaseBoardRev());
#endif

}


System_PlatformBoardRev System_getDcBoardRev()
{
#if defined (TI816X_DVR)
   return SYSTEM_PLATFORM_BOARD_DVR_REV_NONE;
#else
   return (System_PlatformBoardRev)(Vps_platformGetDcBoardRev());
#endif
}


System_PlatformBoardId System_getBoardId()
{
#if defined (TI816X_DVR)
   return SYSTEM_PLATFORM_BOARD_DVR;
#else
   return (System_PlatformBoardId)(Vps_platformGetBoardId());
#endif
}


Int32 System_ths7360SetSfParams(System_Ths7360SfCtrl ths7360SfCtrl)
{
   Int32 status = FVID2_SOK;

#ifdef TI816X_EVM
   status = Vps_ths7360SetSfParams((Vps_Ths7360SfCtrl)ths7360SfCtrl);
#endif

   return status;
}


Int32 System_ths7360SetSdParams(System_ThsFilterCtrl ths7360SdCtrl)
{

   Int32 status = FVID2_SOK;

#ifdef TI816X_EVM
   status = Vps_ths7360SetSdParams((Vps_ThsFilterCtrl)ths7360SdCtrl);
#endif

   return status;
}

Int32 System_platformSelectHdCompClkSrc(System_VPllOutputClk clkSrc)
{
    Int32 status = FVID2_SOK;

#if defined(TI_8107_BUILD)
	status = Vps_platformSelectHdCompClkSrc((Vps_VPllOutputClk)clkSrc);
#endif

    return (status);
}
Int32 System_allocBlankFrame()
{
    UInt32 memSize;

    memSize = SYSTEM_BLANK_FRAME_WIDTH*SYSTEM_BLANK_FRAME_HEIGHT*SYSTEM_BLANK_FRAME_BYTES_PER_PIXEL;

    gSystem_objVpss.nonTiledBlankFrameAddr = Utils_memAlloc(memSize, VPS_BUFFER_ALIGNMENT*2);

    UTILS_assert(gSystem_objVpss.nonTiledBlankFrameAddr!=NULL);
	
#ifdef ENABLE_TILER	
	gSystem_objVpss.tiledBlankFrameAddr[0] = (UInt8*)SystemTiler_alloc(SYSTEM_TILER_CNT_8BIT,
																	   SYSTEM_BLANK_FRAME_WIDTH,
																	   SYSTEM_BLANK_FRAME_HEIGHT);
	gSystem_objVpss.tiledBlankFrameAddr[1] = (UInt8*)SystemTiler_alloc(SYSTEM_TILER_CNT_16BIT,
																	   SYSTEM_BLANK_FRAME_WIDTH,
																       SYSTEM_BLANK_FRAME_HEIGHT);		
#else
	gSystem_objVpss.tiledBlankFrameAddr[0] = NULL;
	gSystem_objVpss.tiledBlankFrameAddr[1] = NULL;	
#endif

    return 0;
}

Int32 System_getBlankFrame(FVID2_Frame *pFrame,UInt32 tilerEnable)
{
    memset(pFrame, 0, sizeof(*pFrame));

	if(tilerEnable == 0)
	{
		pFrame->addr[0][0] = gSystem_objVpss.nonTiledBlankFrameAddr;
		pFrame->addr[0][1] = gSystem_objVpss.nonTiledBlankFrameAddr;
	}
	else
	{
		pFrame->addr[0][0] = gSystem_objVpss.tiledBlankFrameAddr[0];
		pFrame->addr[0][1] = gSystem_objVpss.tiledBlankFrameAddr[1];
	}	

    return 0;
}

Int32 System_freeBlankFrame()
{
    UInt32 memSize;

    memSize = SYSTEM_BLANK_FRAME_WIDTH*SYSTEM_BLANK_FRAME_HEIGHT*SYSTEM_BLANK_FRAME_BYTES_PER_PIXEL;

    Utils_memFree(gSystem_objVpss.nonTiledBlankFrameAddr, memSize);
	
#ifdef ENABLE_TILER
	SystemTiler_freeAll();
#endif	

    return 0;
}

Int32 System_getOutSize(UInt32 outRes, UInt32 * width, UInt32 * height)
{
    switch (outRes)
    {
        case VSYS_STD_MAX:
            *width = 1920;
            *height = 1200;
            break;

        case VSYS_STD_720P_60:
            *width = 1280;
            *height = 720;
            break;
        case VSYS_STD_XGA_60:
            *width = 1024;
            *height = 768;
            break;
        case VSYS_STD_SXGA_60:
            *width = 1280;
            *height = 1024;
            break;
        case VSYS_STD_NTSC:
            *width = 720;
            *height = 480;
            break;

        case VSYS_STD_PAL:
            *width = 720;
            *height = 576;
            break;

        default:
        case VSYS_STD_1080I_60:
        case VSYS_STD_1080P_60:
        case VSYS_STD_1080P_30:
            *width = 1920;
            *height = 1080;
            break;

    }
    return 0;
}
