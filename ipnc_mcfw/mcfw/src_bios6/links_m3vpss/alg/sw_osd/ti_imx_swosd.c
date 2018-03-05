/** ==================================================================
 *  @file   ti_swosd.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/sw_osd/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/******************************************************************************

              SW OSD implementation for IPNC application

******************************************************************************/

/* include files */
#include "alg_ti_imx_swosd.h"

#include <stdio.h>
#include <string.h>
#include <xdc/std.h>
#include "swosd_priv.h"
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ivideo.h>
#include <mcfw/src_bios6/utils/utils_mem.h>
#include <mcfw/src_bios6/utils/utils_tiler.h>

#include <ti/psp/iss/alg/evf/inc/cpisCore.h>
#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

/* SWOSD interface header file */
#include <ti/psp/iss/alg/swosd/inc/sw_osd.h>
#include <ti/psp/iss/alg/rm/inc/rm.h>

#define MAX_CHANNELS 4

#define GLOBAL_ALPHA_VALUE (127)

Int32 isTiled[MAX_CHANNELS];
Int32 lineOffsets[MAX_CHANNELS];

void * /* RM_HANDLE */ pRMHandle[MAX_CHANNELS];
SWOSD_Obj gOsdHndl[MAX_CHANNELS];
SWOSD_MemAllocPrm gMemInfo;
SWOSD_DynamicPrm gDynamicPrm;
SWOSD_BlendFramePrm osdFrameList;
SWOSD_CreatePrm createPrm;

SWOSD_StaticPrm staticPrm;
SWOSD_DynamicPrm dynPrm;

void * memBlockAddr[10];

Int32 isMemoryAllocationDone = 0;


 typedef struct {
     Uint8    isAnyWindowEnabled; /* Windows enabled and how many */
	 SWOSD_DynamicPrm dynPrm[3];  /* Each Dynamic Params can support 8 Windows and Hence 24 */

 }StreamWindows;

StreamWindows  streamWindows[MAX_CHANNELS];    


#ifdef 	 TO_REPRODUCE_HUAWEI_ISSUES
#define HUAWEI_MAX_WINDOWS (13)

short Huawei_startX[HUAWEI_MAX_WINDOWS] = {611, 793, 255, 793, 1, 517, 44, 506, 19, 426, 848, 898, 41};
short Huawei_startY[HUAWEI_MAX_WINDOWS] = {101, 398, 291, 680, 616, 536, 436, 363, 233, 533, 206, 33};
short Huawei_width[HUAWEI_MAX_WINDOWS] = {384, 216, 96, 216, 672, 240, 364, 244, 400, 338, 432, 382, 470};
short Huawei_height[HUAWEI_MAX_WINDOWS] = {40, 40,40,40,40, 58, 116,130, 130, 122,108, 142, 146};
short Huawei_alpha[HUAWEI_MAX_WINDOWS] = {215, 215, 215, 215, 215, 255, 255, 255, 255, 255, 255, 255, 255};
short Huawei_lineOffset[HUAWEI_MAX_WINDOWS] = {1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920};

char Huawei_bmpWinAddr[MAX_GRAPHIC_WINDOW_HEIGHT][MAX_GRAPHIC_WINDOW_WIDTH];
char Huawei_bmpWinAddrUV[MAX_GRAPHIC_WINDOW_HEIGHT][MAX_GRAPHIC_WINDOW_WIDTH];

#endif /* TO_REPRODUCE_HUAWEI_ISSUES */
int AcquireResource(void * pObj)
{
   int streamId = -1;
    do
	{
	   streamId++;
	}while((unsigned int ) pObj != (unsigned int )&gOsdHndl[streamId]);

	/* Acquire the SIMCOP Resource */
	{
    RM_SIMCOP_STATUS eRMStatus = RM_SIMCOP_STATUS_SUCCESS;
	unsigned int bitFieldOfResources;
    RM_SIMCOP_RESOURCE_STATUS resStatus;

    bitFieldOfResources = RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_HWSEQ | RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DMA ;
	resStatus.bitFieldDirtyResources = 0;
    eRMStatus = RM_SIMCOP_AcquireMultiResources((RM_SIMCOP_CLIENT_HANDLE) pRMHandle[streamId],
          bitFieldOfResources,
          RM_SIMCOP_ACQUIRE_WAIT,
          &resStatus);
    UTILS_assert(eRMStatus==RM_SIMCOP_STATUS_SUCCESS);	
	return resStatus.bitFieldDirtyResources;
	}
}

void ReleaseResource(void * pObj)
{
   int streamId = -1;
    do
	{
	   streamId++;
	}while((unsigned int ) pObj != (unsigned int )&gOsdHndl[streamId]);

	/* Release the SIMCOP Resource */
	{
	RM_SIMCOP_STATUS eRMStatus = RM_SIMCOP_STATUS_SUCCESS;
	unsigned int bitFieldOfResources;
    //RM_SIMCOP_RESOURCE_STATUS resStatus;

    bitFieldOfResources = RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_HWSEQ | RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DMA ;
	eRMStatus = RM_SIMCOP_ReleaseMultiResources((RM_SIMCOP_CLIENT_HANDLE) pRMHandle[streamId],
      bitFieldOfResources);
    UTILS_assert(eRMStatus==RM_SIMCOP_STATUS_SUCCESS);
	}		
			


}


int SWOSD_collateWindows(int streamID, SWOSD_Hndl * Hndl)
{
    int winIndex = 0;
	
	StreamWindows  * sWin = &streamWindows[streamID];
	SWOSD_DynamicPrm * dynPrmm;
	
	sWin->isAnyWindowEnabled = 0;
	dynPrmm = &sWin->dynPrm[0];
    dynPrmm->numWindows = 0;
	
#ifdef 	 TO_REPRODUCE_HUAWEI_ISSUES
{
    static int frameCnt = 1000;
	if(frameCnt < 1000) frameCnt++; 
    if(streamID || (frameCnt < 1000)) return (SWOSD_RRETSUCCESS);
	
	for(winIndex = 0 ; winIndex < HUAWEI_MAX_WINDOWS; winIndex++)
	{
			dynPrmm->winPrm[dynPrmm->numWindows].startX = Huawei_startX[winIndex];
			dynPrmm->winPrm[dynPrmm->numWindows].startY = Huawei_startY[winIndex];
			dynPrmm->winPrm[dynPrmm->numWindows].width  = Huawei_width[winIndex];
			dynPrmm->winPrm[dynPrmm->numWindows].height = Huawei_height[winIndex];
			dynPrmm->winPrm[dynPrmm->numWindows].alpha  = Huawei_alpha[winIndex];
			dynPrmm->winPrm[dynPrmm->numWindows].lineOffset[0] = Huawei_lineOffset[winIndex]; 
			dynPrmm->winPrm[dynPrmm->numWindows].lineOffset[1] = Huawei_lineOffset[winIndex];
			dynPrmm->winPrm[dynPrmm->numWindows].graphicsWindowAddr[0] = &Huawei_bmpWinAddr[0][0];
			dynPrmm->winPrm[dynPrmm->numWindows].graphicsWindowAddr[1] = &Huawei_bmpWinAddrUV[0][0];
				
			dynPrmm->numWindows++;
			sWin->isAnyWindowEnabled++;
			
			if(SWOSD_MAX_WINDOWS == dynPrmm->numWindows)
            {
              dynPrmm++;
			  dynPrmm->numWindows = 0;
			}
	  
	}
	
}
#else  /* TO_REPRODUCE_HUAWEI_ISSUES */
#ifdef SWOSD_DEBUGPRINT
    printf("\r\nSWOSD_winDrawHandle():Inside WinDrawHandle ");
#endif
    for(winIndex = Hndl->numBmpWin - 1; winIndex >= 0; winIndex--)
    {
        if(Hndl->bmpWinPrm[winIndex].enable == 1)
        {
#if 0
			Vps_printf("---------------------------------------------------------\n");
			Vps_printf("  Window Index is %d \n", winIndex);
			Vps_printf("  SteamID  is %d \n", streamID);
			Vps_printf("---------------------------------------------------------\n");
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].startX = %d\n", Hndl->bmpWinPrm[winIndex].startX);
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].startY = %d\n", Hndl->bmpWinPrm[winIndex].startY);
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].width  = %d\n", Hndl->bmpWinPrm[winIndex].width);
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].height = %d\n", Hndl->bmpWinPrm[winIndex].height);
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].alpha  = %d\n", GLOBAL_ALPHA_VALUE);
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].lineOffset[0] = %d\n", Hndl->bmpWinPrm[winIndex].lineOffset); 
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].lineOffset[1] = %d\n", Hndl->bmpWinPrm[winIndex].lineOffset);
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].graphicsWindowAddr[0] = %d\n", (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddr);
			Vps_printf("dynPrmm.winPrm[dynPrmm.numWindows].graphicsWindowAddr[1] = %d\n", (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddrUV);
			
			Vps_printf("---------------------------------------------------------\n");
#endif			

			dynPrmm->winPrm[dynPrmm->numWindows].startX = Hndl->bmpWinPrm[winIndex].startX;
			dynPrmm->winPrm[dynPrmm->numWindows].startY = Hndl->bmpWinPrm[winIndex].startY;
			dynPrmm->winPrm[dynPrmm->numWindows].width  = Hndl->bmpWinPrm[winIndex].width;
			dynPrmm->winPrm[dynPrmm->numWindows].height = Hndl->bmpWinPrm[winIndex].height;
			dynPrmm->winPrm[dynPrmm->numWindows].alpha  = GLOBAL_ALPHA_VALUE;
			dynPrmm->winPrm[dynPrmm->numWindows].lineOffset[0] = Hndl->bmpWinPrm[winIndex].lineOffset; 
			dynPrmm->winPrm[dynPrmm->numWindows].lineOffset[1] = Hndl->bmpWinPrm[winIndex].lineOffset;
			dynPrmm->winPrm[dynPrmm->numWindows].graphicsWindowAddr[0] = (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddr;
			dynPrmm->winPrm[dynPrmm->numWindows].graphicsWindowAddr[1] = (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddrUV;
				
			dynPrmm->numWindows++;
			sWin->isAnyWindowEnabled++;
			
			if(SWOSD_MAX_WINDOWS == dynPrmm->numWindows)
            {
              dynPrmm++;
			  dynPrmm->numWindows = 0;
			}
        }
    }
#endif	
	

    return (SWOSD_RRETSUCCESS);
   
}

/*
 * Dynamic memory allocation
 *
 */
Int32 SWOSD_allocMemory(SWOSD_MemAllocPrm *pPrm)
{
	Int32 i=0;
	Int32 totalMem = 0;
	//Vps_printf("[SWOSD] Total %d memories are getting allocated\n", pPrm->numMemBlocks);
	for(;i<pPrm->numMemBlocks; i++)
	{
		pPrm->memBlockAddr[i] = (void *)Utils_memAlloc(pPrm->memBlockSize[i], 128);
		if(pPrm->memBlockAddr[i] == NULL)
		{
		//Vps_printf("[SWOSD] Mem allocation failed\n");
		   return SWOSD_ERROR;
	     }
		 //Vps_printf("[SWOSD] Mem#%d allocated of size %d\n", i, pPrm->memBlockSize[i]);
		 totalMem += pPrm->memBlockSize[i];
		 
		 memBlockAddr[i] = pPrm->memBlockAddr[i];
	}
	     //Vps_printf("[SWOSD] Total mem allocated of size %d\n", totalMem);
		 
	return SWOSD_OK;
}

/*
 * Freeing dynamic memory
 *
 */
Int32 SWOSD_freeMemory(SWOSD_MemAllocPrm *pPrm)
{
	Int32 i=0;
	for(;i<pPrm->numMemBlocks; i++)
	   if(Utils_memFree((Ptr)pPrm->memBlockAddr[i], pPrm->memBlockSize[i]))
	      return SWOSD_ERROR;

	return SWOSD_OK;
}

int SWOSD_imxUpdateLineOffset(int streamID, int offset)
{
    SWOSD_updateLineOffset(&gOsdHndl[streamID], offset);
	
	return SWOSD_OK;
}


int SWOSD_imxCreate(int streamID, int isTiled, int offset)
{
    int   status;
	int i;

	StreamWindows  * sWin = &streamWindows[streamID];
	
	sWin->isAnyWindowEnabled = 0;
	
#ifdef TO_REPRODUCE_HUAWEI_ISSUES
	staticPrm.maxWidth    = MAX_GRAPHIC_WINDOW_WIDTH;
	staticPrm.maxHeight   = MAX_GRAPHIC_WINDOW_HEIGHT;
#else
	staticPrm.maxWidth    = SWOSD_MAX_STRING_LENGTH*SWOSD_CHARWIDTH_1540_40;
	staticPrm.maxHeight   = SWOSD_STRINGHEIGHT_1540_40;
#endif
	staticPrm.dataFormat  = SWOSD_FORMAT_YUV420SP_UV;
	staticPrm.isTiledMem  = isTiled; 
	staticPrm.isInterlaced = 0; 
	staticPrm.videoLineOffset[0] = offset; 
	staticPrm.videoLineOffset[1] = offset; 

	   //Vps_printf("StreamID is %d \n", streamID);
	   //Vps_printf("staticPrm.isInterlaced = %d\n", staticPrm.isInterlaced);
	   //Vps_printf("staticPrm.videoLineOffset[0]  = %d\n",staticPrm.videoLineOffset[0] );
	   //Vps_printf("staticPrm.videoLineOffset[1] = %d\n",staticPrm.videoLineOffset[1]);

	
	dynPrm.numWindows = 1;
	dynPrm.winPrm[0].startX = 0;
    dynPrm.winPrm[0].startY = 0;
	dynPrm.winPrm[0].width  = 160;
	dynPrm.winPrm[0].height = 160;
	dynPrm.winPrm[0].alpha  = GLOBAL_ALPHA_VALUE;
	dynPrm.winPrm[0].lineOffset[0] = 160; 
	dynPrm.winPrm[0].lineOffset[1] = 160;
	dynPrm.winPrm[0].graphicsWindowAddr[0] = 0;
	dynPrm.winPrm[0].graphicsWindowAddr[1] = 0;

	
	createPrm.numChannels = 1;
	createPrm.colorKey[0] = 255; //Revisit
	createPrm.colorKey[1] = 255; //Revisit
	createPrm.transparencyEnable = FALSE;
#ifdef TO_REPRODUCE_HUAWEI_ISSUES
	createPrm.useGlobalAlpha = 0;
#else
	createPrm.useGlobalAlpha = 1;
#endif
	createPrm.globalAlphaValue = GLOBAL_ALPHA_VALUE;
	createPrm.acquire = AcquireResource;
	createPrm.release = ReleaseResource;
	
	
	for(i=0; i < createPrm.numChannels; i++)
	{
	  memcpy(&createPrm.chStaticPrm[i], &staticPrm, sizeof(SWOSD_StaticPrm)); 
	  memcpy(&createPrm.chDynamicPrm[i], &dynPrm, sizeof(SWOSD_DynamicPrm)); 
	}
	
	//Vps_printf("[SWOSD] SWOSD_imxCreate for stream %d \n", streamID);
	
     /* Create osd handle*/
    if(status = SWOSD_open(&gOsdHndl[streamID], &createPrm))
	{   Vps_printf("SWOSD Open failed for stream#%d !!!!!!!\n\n", streamID);
     	 return status;
	}
	
    {
         RM_SIMCOP_CLIENT_PARAMS stRMClientParams;
         RM_SIMCOP_STATUS eRMStatus = RM_SIMCOP_STATUS_SUCCESS;

         stRMClientParams.nSize = sizeof(RM_SIMCOP_CLIENT_PARAMS);
         stRMClientParams.pClientHandle = (RM_SIMCOP_CLIENT_HANDLE)&gOsdHndl[streamID];
         /* Get Resource Manager Handle independent of resources And register Client */
         eRMStatus = RM_SIMCOP_RegisterClient(stRMClientParams, (RM_SIMCOP_CLIENT_HANDLE *) &pRMHandle[streamID]);
         UTILS_assert(eRMStatus==RM_SIMCOP_STATUS_SUCCESS);

    }		

     /* Get memory for osd algorithm */
     SWOSD_getMemAllocInfo(&gOsdHndl[streamID], &gMemInfo);

     if(isMemoryAllocationDone == 0)
	 {
        if(SWOSD_allocMemory(&gMemInfo) != SWOSD_OK)
	    {
		  RM_SIMCOP_DeRegisterClient(pRMHandle[streamID]);
    	  SWOSD_close(&gOsdHndl[streamID]);
     	  Vps_printf("SWOSD_allocMemory failed!!!!!!!\n\n");
		  return SWOSD_ERROR;
	    }
        isMemoryAllocationDone = 1;
	 }
	 else
	 {
	    Int32 i;
		for(i=0;i<gMemInfo.numMemBlocks; i++)
		{
			gMemInfo.memBlockAddr[i] = memBlockAddr[i];
		}
	 }
	 
     /* Set memory for osd algorithm */
     if(SWOSD_setMemAllocInfo(&gOsdHndl[streamID], &gMemInfo) != SWOSD_OK)
     	return SWOSD_ERROR;
		
		

  return 0;
}

int SWOSD_imxDelete(int streamID)
{
    if(streamID == 0)
		SWOSD_freeMemory(&gMemInfo);
		
	RM_SIMCOP_DeRegisterClient(pRMHandle[streamID]);
		
    SWOSD_close(&gOsdHndl[streamID]);
	
	isMemoryAllocationDone = 0;
	
	return 0;
}


int SWOSD_imxPrepUpdateDynamicParamsAndTrigger(SWOSD_Hndl * Hndl, short *mainWinAddrY,
                        short *mainWinAddrUV, int streamId)
{
    int   status;
	
	
			/* update dynamic channel params */
			if(status = SWOSD_setDynamicPrm(&gOsdHndl[streamId], 0, &dynPrm))
			{  
			   Vps_printf("Error occured in the Setting of Dynamic Params\n");
			   return status;
			}
			
    osdFrameList.numFrames = 1;
    osdFrameList.frames[0].channelNum = 0; /* channelNum */
    osdFrameList.frames[0].fid        = 0; /* field Id   */
	
	if(isTiled[streamId])
	{
	osdFrameList.frames[0].addr[0]    = (Ptr)Utils_tilerAddr2CpuAddr( (UInt32)mainWinAddrY );
	osdFrameList.frames[0].addr[1]    = (Ptr)Utils_tilerAddr2CpuAddr( (UInt32)mainWinAddrUV );
	}
	else
	{
    osdFrameList.frames[0].addr[0]    = mainWinAddrY;
    osdFrameList.frames[0].addr[1]    = mainWinAddrUV;
	}
	
	
	status = SWOSD_blendFrames(&gOsdHndl[streamId], &osdFrameList, 0);
	if(SWOSD_OK != status)
	  Vps_printf("SWOSD_ IMX Blend Failed!!!!!!!!!!!\n");
	


  return 0;
}


int SWOSD_imxDrawHandle1(SWOSD_Hndl * Hndl, short *mainWinAddrY,
                        short *mainWinAddrUV, int streamID)
{
  
    int winIndex = 0;

    dynPrm.numWindows = 0;
#ifdef SWOSD_DEBUGPRINT
    printf("\r\nSWOSD_winDrawHandle():Inside WinDrawHandle ");
#endif
    for(winIndex = Hndl->numBmpWin - 1; winIndex >= 0; winIndex--)
    {
        if(Hndl->bmpWinPrm[winIndex].enable == 1)
        {
			dynPrm.winPrm[dynPrm.numWindows].startX = Hndl->bmpWinPrm[winIndex].startX;
			dynPrm.winPrm[dynPrm.numWindows].startY = Hndl->bmpWinPrm[winIndex].startY;
			dynPrm.winPrm[dynPrm.numWindows].width  = Hndl->bmpWinPrm[winIndex].width;
			dynPrm.winPrm[dynPrm.numWindows].height = Hndl->bmpWinPrm[winIndex].height;
			dynPrm.winPrm[dynPrm.numWindows].alpha  = GLOBAL_ALPHA_VALUE;
			dynPrm.winPrm[dynPrm.numWindows].lineOffset[0] = Hndl->bmpWinPrm[winIndex].lineOffset; 
			dynPrm.winPrm[dynPrm.numWindows].lineOffset[1] = Hndl->bmpWinPrm[winIndex].lineOffset;
			dynPrm.winPrm[dynPrm.numWindows].graphicsWindowAddr[0] = (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddr;
			dynPrm.winPrm[dynPrm.numWindows].graphicsWindowAddr[1] = (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddrUV;
				
			dynPrm.numWindows++;
			
#if 0
			Vps_printf("---------------------------------------------------------\n");
			Vps_printf("  Window Index is %d \n", winIndex);
			Vps_printf("  SteamID  is %d \n", streamID);
			Vps_printf("---------------------------------------------------------\n");
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].startX = %d\n", Hndl->bmpWinPrm[winIndex].startX);
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].startY = %d\n", Hndl->bmpWinPrm[winIndex].startY);
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].width  = %d\n", Hndl->bmpWinPrm[winIndex].width);
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].height = %d\n", Hndl->bmpWinPrm[winIndex].height);
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].alpha  = %d\n", GLOBAL_ALPHA_VALUE);
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].lineOffset[0] = %d\n", Hndl->bmpWinPrm[winIndex].lineOffset); 
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].lineOffset[1] = %d\n", Hndl->bmpWinPrm[winIndex].lineOffset);
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].graphicsWindowAddr[0] = %d\n", (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddr);
			Vps_printf("dynPrm.winPrm[dynPrm.numWindows].graphicsWindowAddr[1] = %d\n", (char*)(&(Hndl->bmpWinPrm[winIndex]))->bmpWinAddrUV);
			
			Vps_printf("---------------------------------------------------------\n");
#endif			
			if(SWOSD_MAX_WINDOWS == dynPrm.numWindows)
            {
              SWOSD_imxPrepUpdateDynamicParamsAndTrigger(Hndl, mainWinAddrY, mainWinAddrUV, streamID);
			  dynPrm.numWindows = 0;
			}
        }
    }
	
	if(dynPrm.numWindows)
    {
      SWOSD_imxPrepUpdateDynamicParamsAndTrigger(Hndl, mainWinAddrY, mainWinAddrUV, streamID);
	  dynPrm.numWindows = 0;
	}

    return (SWOSD_RRETSUCCESS);
}
								
int SWOSD_imxDrawHandle(SWOSD_Hndl * Hndl, short *mainWinAddrY,
                        short *mainWinAddrUV, int streamID)
{
  
    int cnt = 0;
	StreamWindows  * sWin = &streamWindows[streamID];
	SWOSD_DynamicPrm * dynPrmm = sWin->dynPrm;
	
	if(sWin->isAnyWindowEnabled)
    {	
	  cnt = sWin->isAnyWindowEnabled;
	  do
	  {
	    memcpy(&dynPrm, dynPrmm, sizeof(SWOSD_DynamicPrm));
        SWOSD_imxPrepUpdateDynamicParamsAndTrigger(Hndl, mainWinAddrY, mainWinAddrUV, streamID);
		cnt -= 8;
		dynPrmm++;
	  }while(cnt > 0);
      return (SWOSD_RRETSUCCESS);
	}
	else 
    {
        return (SWOSD_RRETSUCCESS);
	}
	

}
