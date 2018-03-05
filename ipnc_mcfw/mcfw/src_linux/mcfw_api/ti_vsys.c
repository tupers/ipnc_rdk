/*==============================================================================
 * @file:       ti_vsys.c
 *
 * @brief:      System mcfw function definition.
 *
 * @vers:       0.5.0.0 2011-06
 *
 *==============================================================================
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include "ti_vsys_priv.h"
#include <cmem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ti/syslink/utils/Cache.h>

/* =============================================================================
 * Globals
 * =============================================================================
 */

VSYS_MODULE_CONTEXT_S gVsysModuleContext = {

	.enableFastUsecaseSwitch = FALSE,
    .initDone   = FALSE
};

VSYS_TIMER_OBJ gTimerThObj;

/* =============================================================================
 * Vsys module APIs
 * =============================================================================
 */
#define MMAP_MEM_PAGEALIGN (0xFFF)	
static Int32 memDevFd;
 
Void *MMAP(Int32 memFd,UInt32 physAddr,UInt32 size)
{
    Uint8 *virtAddr;
	Uint32 memOffset,mmapMemAddr,mmapMemSize;

	/* mmap should be done at page level */	
    memOffset   = physAddr & MMAP_MEM_PAGEALIGN;
    mmapMemAddr = physAddr - memOffset;
    mmapMemSize = size + memOffset;

	virtAddr = mmap((Void*)NULL,
		            mmapMemSize,
					PROT_READ|PROT_WRITE|PROT_EXEC,
					MAP_SHARED,
					memFd,
					mmapMemAddr);

	if(virtAddr == NULL)
	{
		OSA_printf(" ERROR: mmap() failed !!!\n");
		return NULL;
	}

	virtAddr = (Uint8 *)((UInt32)virtAddr + memOffset);

	return virtAddr;
} 

Void MUNMAP(UInt32 virtAddr,UInt32 size)
{
	Uint32 memOffset,munmapMemAddr,munmapMemSize;

	/* munmap should be done at page level */	
    memOffset     = virtAddr & MMAP_MEM_PAGEALIGN;
    munmapMemAddr = virtAddr - memOffset;
    munmapMemSize = size + memOffset;

	munmap((Void*)munmapMemAddr,munmapMemSize);
}

/**
 * \brief:
 *      Vsys Event Handler
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_eventHandler(UInt32 eventId, Ptr pPrm, Ptr appData)
{
	Int32 status;
	SystemCommon_edmaChPrm *pEdmaPrm;
	SystemCommon_rvmLutPrm *pRvmLutPrm;
	Void* virtLutBufAddr;
	UInt32 fileSize,sdCardMounted;
			
	switch(eventId)
	{
		case SYSTEM_COMMON_CMD_GET_EDMA_CH:
			pEdmaPrm = (SystemCommon_edmaChPrm*)pPrm;
			
			status = OSA_dmaInit();
			if(status == 0)
			{
				status = OSA_dmaOpen(&pEdmaPrm->dmaHndl,OSA_DMA_MODE_NORMAL,1);
				if(status != 0)
				{
					OSA_printf("%s:OSA_dmaOpen failed - %d\n", __func__, status);
					pEdmaPrm->dmaHndl.chId = 0xFF;
				}	
				else
				{
					OSA_printf("%s:OSA_dmaOpen passed with ch id = %d\n", __func__,pEdmaPrm->dmaHndl.chId);			
				}
			}
			else
			{
				OSA_printf("%s:OSA_dmaInit failed - %d\n", __func__, status);
				pEdmaPrm->dmaHndl.chId = 0xFF;
			}
			
			break;
			
		case SYSTEM_COMMON_CMD_PUT_EDMA_CH:	
			pEdmaPrm = (SystemCommon_edmaChPrm*)pPrm;
			
			OSA_printf("%s:OSA_dmaClose for ch - %d\n", __func__, pEdmaPrm->dmaHndl.chId);
			OSA_dmaClose(&pEdmaPrm->dmaHndl);
			
			break;	

		case SYSTEM_COMMON_READ_RVM_LUT:
			pRvmLutPrm = (SystemCommon_rvmLutPrm*)pPrm;
			
			pRvmLutPrm->errorCode = 0;
			sdCardMounted         = 0;
						
			memDevFd = open("/dev/mem",O_RDWR|O_SYNC);
			if(memDevFd < 0)
			{
				OSA_printf(" ERROR: /dev/mem open failed !!!\n");
				pRvmLutPrm->errorCode = -1;
			}
			else
			{
				/* Open LUT file and find the file size */
				FILE *fp = fopen(pRvmLutPrm->fileName,"rb");				
				if(fp == NULL)
				{
					/* May be SD card is not mounted,try mounting it */
					if(system("mount -t vfat /dev/mmcblk0 /mnt/mmc\n") != 0)
					{
						OSA_printf(" ERROR: SD card mount failed !!!\n");
						pRvmLutPrm->errorCode = -1;		
					}
					else
					{
						sdCardMounted = 1;
						fp = fopen(pRvmLutPrm->fileName,"rb");
					}
				}
								
				if(fp == NULL)
				{
					OSA_printf(" ERROR: LUT file open failed !!!\n");
					pRvmLutPrm->errorCode = -1;
				}				
				else
				{
					/* Go to end of file */
					fseek(fp,0,SEEK_END);
					
					fileSize = ftell(fp);
					pRvmLutPrm->lutFileSize = fileSize;
					
					/* Go to beginning of file */
					fseek(fp,0,SEEK_SET);
					
					if(fileSize > pRvmLutPrm->lutBufSize)
					{
						OSA_printf(" ERROR: LUT buffer is insufficient !!!\n");
						pRvmLutPrm->errorCode = -1;					
					}
					else
					{
						/* Map the LUT buffer */
						virtLutBufAddr = MMAP(memDevFd,pRvmLutPrm->lutBufAddr,fileSize);
						if(virtLutBufAddr == NULL)
						{
							OSA_printf(" ERROR: mmap of LUT buffer failed !!!\n");
							pRvmLutPrm->errorCode = -1;
						}			
						else
						{						
							/* Read LUT file into LUT buffer */
							if(fread(virtLutBufAddr,1,fileSize,fp) != fileSize)
							{
								OSA_printf(" ERROR: File read failed !!!\n");
								pRvmLutPrm->errorCode = -1;							
							}
							else
							{
								Cache_wb((Ptr)virtLutBufAddr,fileSize,Cache_Type_ALL,TRUE);	
							}
							
							/* Unmap the address space */
							MUNMAP((UInt32)virtLutBufAddr,fileSize);							
						}
					}
					
					fclose(fp);					
				}
											
				close(memDevFd);			
				
				if(sdCardMounted == 1)
				{
					/* Unmount SD card */
					system("umount /mnt/mmc\n");					
				}				
			}
						
			break;
	}
	
	return 0;
} 
/**
 * \brief:
 *      Initialize parameters to be passed to init
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_params_init(VSYS_PARAMS_S * pContext)
{
    memset(pContext, 0, sizeof(VSYS_PARAMS_S));
    pContext->enableCapture = FALSE;
    pContext->enableNsf     = FALSE;
    pContext->enableOsd     = FALSE;
    pContext->enableScd     = FALSE;
    pContext->enableEncode  = FALSE;
    pContext->enableDecode  = FALSE;
    pContext->numDeis       = 0;
    pContext->numSwMs       = 0;
    pContext->numDisplays   = 0;
    pContext->systemUseCase = VSYS_USECASE_MAX;

    return 0;
}
static Void *Vsys_dateTimeTaskMain(Void * prm)
{
    time_t timep;

    struct tm *p;

    SwosdLink_dateTime dateTime;

    while (gTimerThObj.exitTimerTh == FALSE)
    {
        tzset();
        time(&timep);
        p = (struct tm *) localtime(&timep);

        dateTime.year = 1900 + p->tm_year;
        dateTime.month = 1 + p->tm_mon;
        dateTime.day = p->tm_mday;
        dateTime.hour = p->tm_hour;
        dateTime.min = p->tm_min;
        dateTime.sec = p->tm_sec;

        if ((gTimerThObj.startCnt >= 20) && (gTimerThObj.exitTimerTh == FALSE))
        {
        	if (gVsysModuleContext.swOsdId != SYSTEM_LINK_ID_INVALID)
        	{
				System_linkControl(gVsysModuleContext.swOsdId, // dest link id
								   SWOSDLINK_CMD_DATETIME,     // cmd
								   &dateTime,                  // prm
								   sizeof(SwosdLink_dateTime), // prm size
								   TRUE);                      // wait for ack
			}
        }
        else
        {
            gTimerThObj.startCnt++;
        }

        /* sleep for 1 sec */
        OSA_waitMsecs(1000);
    }

    gTimerThObj.exitTimerDone = TRUE;

    return NULL;
}
Int32 Vsys_datetimeCreate()
{
    gTimerThObj.startCnt = 0;
    gTimerThObj.exitTimerTh = FALSE;
    gTimerThObj.exitTimerDone = FALSE;

    OSA_thrCreate(&gTimerThObj.hndl,                       // hndl
                  Vsys_dateTimeTaskMain,                   // entryFunc
                  VSYS_TIMER_TSK_PRI,                      // pri
                  VSYS_TIMER_TSK_STACK_SIZE,               // stackSize
                  NULL);                                   // prm

    return 0;
}
Int32 Vsys_datetimeDelete()
{
    gTimerThObj.exitTimerTh = TRUE;

    while (gTimerThObj.exitTimerDone == FALSE)
    {
        OSA_waitMsecs(1000);
    }

    OSA_thrDelete(&gTimerThObj.hndl);

	OSA_printf("Date time task deleted\n");

    return 0;
}
/**
 * \brief:
 *      Initialize Vsys instance
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_init(VSYS_PARAMS_S * pContext)
{

	gVsysModuleContext.fdId 	= SYSTEM_LINK_ID_INVALID;
	gVsysModuleContext.swOsdId 	= SYSTEM_LINK_ID_INVALID;
    gVsysModuleContext.muxId    = SYSTEM_LINK_ID_INVALID;

    if(pContext==NULL)
    {
        Vsys_params_init(&gVsysModuleContext.vsysConfig);
    }
    else
    {
        memcpy(&gVsysModuleContext.vsysConfig, pContext, sizeof(VSYS_PARAMS_S));
    }
    Vcap_init(NULL);
    Vcam_init(NULL);
    Mjpeg_init(NULL);
    Vdis_init(NULL);

    Vdec_init(NULL);
    Venc_init(NULL);

    if(gVsysModuleContext.enableFastUsecaseSwitch==FALSE)
    {
        if(gVsysModuleContext.initDone == FALSE)
        {
            gVsysModuleContext.initDone = TRUE;
	    	System_init();
    	}
	}

	/* Register Host Event Handler */	
	Vsys_registerEventHandler(Vsys_eventHandler,NULL);	
	
    return 0;
}

/**
 * \brief:
 *      Finalize Vsys instance
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_exit()
{

    Vdec_exit();

    if(gVsysModuleContext.enableFastUsecaseSwitch==FALSE)
	{
        if(gVsysModuleContext.initDone == TRUE)
        {
            gVsysModuleContext.initDone = FALSE;
	        System_deInit();
    	}
	}

    return 0;
}

/**
 * \brief:
 *      This function configures display controller
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_configureDisplay()
{
    Int32 status = 0;
	VDIS_PARAMS_S * prm = &gVdisModuleContext.vdisConfig;

	if (gVsysModuleContext.enableFastUsecaseSwitch == FALSE)
	{
		if(gVdisModuleContext.displayConfigInitDone == FALSE)
		{
			OSA_printf("Before System_linkControl SYSTEM_M3VPSS_CMD_GET_DISPLAYCTRL_INIT\n");
		    status = System_linkControl(
		        SYSTEM_LINK_ID_M3VPSS,
		        SYSTEM_M3VPSS_CMD_GET_DISPLAYCTRL_INIT,
		        &(*prm),
		        sizeof(*prm),
		        TRUE
		        );
		    UTILS_assert(status==OSA_SOK);
	
            gVdisModuleContext.displayConfigInitDone = TRUE;
		}
	}

    return status;
}

/**
 * \brief:
 *      This function de-configures display controller
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_deConfigureDisplay()
{
    Int32 status = 0;
	VDIS_PARAMS_S * prm = &gVdisModuleContext.vdisConfig;

	if (gVsysModuleContext.enableFastUsecaseSwitch == FALSE)
	{
		if(gVdisModuleContext.displayConfigInitDone == TRUE)
		{
            gVdisModuleContext.displayConfigInitDone = FALSE;
	
		    status = System_linkControl(
		        SYSTEM_LINK_ID_M3VPSS,
		        SYSTEM_M3VPSS_CMD_GET_DISPLAYCTRL_DEINIT,
		        &(*prm),
		        sizeof(*prm),
		        TRUE
		        );
		    UTILS_assert(status==OSA_SOK);
		}
	}

    return status;
}


/**
 * \brief:
 *      Creates instances of links and prepares chains
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_create()
{
	/* Configuring display if application has not done already */
	if((gVsysModuleContext.vsysConfig.systemUseCase != VSYS_USECASE_MULTICHN_TRISTREAM_LOWPWR)&&( gVsysModuleContext.vsysConfig.systemUseCase != VSYS_USECASE_MACH_VISION))
	{
		Vsys_configureDisplay();
	}	

    switch (gVsysModuleContext.vsysConfig.systemUseCase) {

        case VSYS_USECASE_MULTICHN_TRISTREAM_LOWPWR:
            MultiCh_createTriStreamLowPwr();
            break;
		case VSYS_USECASE_TRISTREAM_SMARTANALYTICS:			
        case VSYS_USECASE_MULTICHN_TRISTREAM_FULLFTR:
            MultiCh_createTriStreamFullFtr();
            break;
        case VSYS_USECASE_DUALCHN_DISPLAY:
            MultiCh_createStreamCaptureDisplay();
            break;
		case VSYS_USECASE_ENC_A8_DEC:
			MultiCh_createEncA8Dec();
			break;
		case VSYS_USECASE_MULTICHN_QUADSTREAM_NF:
			MultiCh_createQuadStreamNf();
			break;
        case VSYS_USECASE_DUALCHN_DEMO:
            MultiCh_createStreamDualStreamDemo();
            break;
		case VSYS_USECASE_MCTNF_DEMO:
			MultiCh_createStreamCaptMctnfDisplay();
			break;
		case VSYS_USECASE_RVM_DEMO:
			MultiCh_createRvm();
			break;
		case VSYS_USECASE_MACH_VISION:
			MultiCh_createMachVision();
			break;			
        case VSYS_USECASE_MAX:
        default:
            break;
    }
    return 0;
}

/**
 * \brief:
 *      Delete instances of links
 * \input:
 *      NA
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_delete()
{

    switch (gVsysModuleContext.vsysConfig.systemUseCase) {
        case VSYS_USECASE_MULTICHN_TRISTREAM_LOWPWR:
            MultiCh_deleteTriStreamLowPwr();
            break;
		case VSYS_USECASE_TRISTREAM_SMARTANALYTICS:			
        case VSYS_USECASE_MULTICHN_TRISTREAM_FULLFTR:
            MultiCh_deleteTriStreamFullFtr();
            break;
        case VSYS_USECASE_DUALCHN_DISPLAY:
            MultiCh_deleteStreamCaptureDisplay();
            break;
		case VSYS_USECASE_ENC_A8_DEC:
			MultiCh_deleteEncA8Dec();
            break;
		case VSYS_USECASE_MULTICHN_QUADSTREAM_NF:
			MultiCh_deleteQuadStreamNf();
			break;
        case VSYS_USECASE_DUALCHN_DEMO:
            MultiCh_deleteStreamDualStreamDemo();
            break;
		case VSYS_USECASE_MCTNF_DEMO:
			MultiCh_deleteStreamCaptMctnfDisplay();
			break;
		case VSYS_USECASE_RVM_DEMO:
			MultiCh_deleteRvm();
			break;	
	case VSYS_USECASE_MACH_VISION:
			MultiCh_deleteMachVision();
			break;		
        case VSYS_USECASE_MAX:
        default:
            break;
    }
	
	if(gVsysModuleContext.vsysConfig.systemUseCase != VSYS_USECASE_MULTICHN_TRISTREAM_LOWPWR) 
	{	
		Vsys_deConfigureDisplay();
	}
	
    return 0;
}

Int32 Vsys_allocBuf(UInt32 srRegId, UInt32 bufSize, UInt32 bufAlign, Vsys_AllocBufInfo *bufInfo)
{
#if 0
    IHeap_Handle heapHndl;

    heapHndl = SharedRegion_getHeap(srRegId);
    OSA_assert(heapHndl != NULL);

    bufInfo->virtAddr = NULL;
    bufInfo->physAddr = NULL;
    bufInfo->srPtr    = 0;

    bufInfo->virtAddr = Memory_alloc(heapHndl, bufSize, bufAlign, NULL);

    if(bufInfo->virtAddr==NULL)
        return -1;

    bufInfo->physAddr = Memory_translate (bufInfo->virtAddr, Memory_XltFlags_Virt2Phys);

    if(bufInfo->physAddr==NULL)
        return -1;

    bufInfo->srPtr = SharedRegion_getSRPtr(bufInfo->virtAddr,srRegId);
#else
    CMEM_AllocParams  prm;

    prm.type      = CMEM_HEAP;
    prm.flags     = CMEM_NONCACHED;
    prm.alignment = bufAlign;

    bufInfo->virtAddr = (Uint8*)CMEM_alloc(bufSize,&prm);
    bufInfo->physAddr = (Uint8*)CMEM_getPhys(bufInfo->virtAddr);

    OSA_printf("%s - addr = 0x%x,size = %d\n",__func__,(unsigned int)bufInfo->virtAddr,bufSize);
#endif

    return 0;
}

Int32 Vsys_freeBuf(UInt32 srRegId, UInt8 *virtAddr, UInt32 bufSize)
{
#if 0
    IHeap_Handle heapHndl;

    heapHndl = SharedRegion_getHeap(srRegId);
    OSA_assert(heapHndl != NULL);

    OSA_assert(virtAddr != NULL);

    Memory_free(heapHndl, virtAddr, bufSize);
#else
    CMEM_AllocParams  prm;

    prm.type      = CMEM_HEAP;
    prm.flags     = 0;
    prm.alignment = 0;

    CMEM_free(virtAddr,&prm);

    OSA_printf("%s - addr = 0x%x\n",__func__,(unsigned int)virtAddr);
#endif

    return 0;
}

VSYS_USECASES_E Vsys_getSystemUseCase()
{
    return gVsysModuleContext.vsysConfig.systemUseCase;
}

Int32 Vsys_setSwOsdPrm(VSYS_SWOSD_SETPARAM swOsdSetPrm,
                       Vsys_swOsdPrm * pSwOsdPrm)
{
    SwosdLink_GuiParams swOsdGuiPrm;

    switch (swOsdSetPrm)
    {
        case VSYS_SWOSDGUIPRM:
            swOsdGuiPrm.streamId = pSwOsdPrm->streamId;
            swOsdGuiPrm.transparencyEnable = pSwOsdPrm->transparencyEnable;

            swOsdGuiPrm.dateEnable = pSwOsdPrm->dateEnable;
            swOsdGuiPrm.timeEnable = pSwOsdPrm->timeEnable;
            swOsdGuiPrm.logoEnable = pSwOsdPrm->logoEnable;
            swOsdGuiPrm.logoPos = pSwOsdPrm->logoPos;
            swOsdGuiPrm.textEnable = pSwOsdPrm->textEnable;
            swOsdGuiPrm.textPos = pSwOsdPrm->textPos;
            swOsdGuiPrm.detailedInfo = pSwOsdPrm->detailedInfo;
            swOsdGuiPrm.encFormat =
                gVencModuleContext.encFormat[swOsdGuiPrm.streamId];
            strcpy((char *)swOsdGuiPrm.usrString, (const char *)pSwOsdPrm->pUsrString);

			if (gVsysModuleContext.swOsdId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.swOsdId, // dest link id
								   SWOSDLINK_CMD_GUIPRM,       // cmd
								   &swOsdGuiPrm,               // prm
								   sizeof(SwosdLink_GuiParams), // prm size
								   TRUE);                      // wait for ack
			}
            break;

        case VSYS_SWOSDBR:
            swOsdGuiPrm.streamId = pSwOsdPrm->streamId;
            swOsdGuiPrm.bitRate = pSwOsdPrm->bitRate;

			if (gVsysModuleContext.swOsdId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.swOsdId, // dest link id
								   SWOSDLINK_CMD_BR,           // cmd
								   &swOsdGuiPrm,               // prm
								   sizeof(SwosdLink_GuiParams), // prm size
								   TRUE);                      // wait for ack
			}
            break;

        case VSYS_SWOSDRC:
            swOsdGuiPrm.streamId = pSwOsdPrm->streamId;
            swOsdGuiPrm.rateControl = pSwOsdPrm->rateControl;

			if (gVsysModuleContext.swOsdId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.swOsdId, // dest link id
								   SWOSDLINK_CMD_RC,           // cmd
								   &swOsdGuiPrm,               // prm
								   sizeof(SwosdLink_GuiParams), // prm size
								   TRUE);                      // wait for ack
			}
            break;

        case VSYS_SWOSDFR:
            swOsdGuiPrm.streamId = pSwOsdPrm->streamId;
            swOsdGuiPrm.frameRate = pSwOsdPrm->frameRate;

			if (gVsysModuleContext.swOsdId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.swOsdId, // dest link id
								   SWOSDLINK_CMD_FR,           // cmd
								   &swOsdGuiPrm,               // prm
								   sizeof(SwosdLink_GuiParams), // prm size
								   TRUE);                      // wait for ack
			}
            break;

        case VSYS_SWOSDDATETIME:
            swOsdGuiPrm.streamId = pSwOsdPrm->streamId;
            swOsdGuiPrm.dateFormat = pSwOsdPrm->dateFormat;
            swOsdGuiPrm.datePos = pSwOsdPrm->datePos;
            swOsdGuiPrm.timeFormat = pSwOsdPrm->timeFormat;
            swOsdGuiPrm.timePos = pSwOsdPrm->timePos;

			if (gVsysModuleContext.swOsdId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.swOsdId, // dest link id
								   SWOSDLINK_CMD_DATETIMEPOSN, // cmd
								   &swOsdGuiPrm,               // prm
								   sizeof(SwosdLink_GuiParams), // prm size
								   TRUE);                      // wait for ack
			}
            break;
		case VSYS_SWOSDHISTEN:
            swOsdGuiPrm.histEnable = pSwOsdPrm->histEnable;

			if (gVsysModuleContext.swOsdId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.swOsdId, // dest link id
								   SWOSDLINK_CMD_HISTENABLE, // cmd
								   &swOsdGuiPrm,               // prm
								   sizeof(SwosdLink_GuiParams), // prm size
								   TRUE);                      // wait for ack
			}
            break;
    }

    return 0;
}

Int32 Vsys_setFdPrm(Vsys_fdPrm * pFdPrm)
{
    FdLink_GuiParams fdLinkGuiPrm;

    fdLinkGuiPrm.fdetect = pFdPrm->fdetect;
    fdLinkGuiPrm.startX = pFdPrm->startX;
    fdLinkGuiPrm.startY = pFdPrm->startY;
    fdLinkGuiPrm.width = pFdPrm->width;
    fdLinkGuiPrm.height = pFdPrm->height;
    fdLinkGuiPrm.fdconflevel = pFdPrm->fdconflevel;
    fdLinkGuiPrm.fddirection = pFdPrm->fddirection;
    fdLinkGuiPrm.pmask = pFdPrm->pmask;

	if (gVsysModuleContext.fdId != SYSTEM_LINK_ID_INVALID)
	{
		System_linkControl(gVsysModuleContext.fdId,            // dest link id
						   FD_CMD_PRM,                         // cmd
						   &fdLinkGuiPrm,                      // prm
						   sizeof(FdLink_GuiParams),           // prm size
						   TRUE);                              // wait for ack
	}

    return 0;
}

Int32 Vsys_setVaGuiPrm(VSYS_VA_GUISETPARAM vaGuiSetPrm,
					   VaLink_GuiParams *pVaGuiPrm)
{
    switch(vaGuiSetPrm)
	{
	    case VSYS_VAGUI_SETUP:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUP,       			// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;

		case VSYS_VAGUI_SETUPALGOFRAMERATE:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPALGOFRAMERATE,   	// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
        case VSYS_VAGUI_SETUPALGODETECTFREQ:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPALGODETECTFREQ,   // cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPEVTRECORDENABLE:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPEVTRECORDENABLE,  // cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPSMETATRACKENABLE:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPSMETATRACKENABLE, // cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
        case VSYS_VAGUI_SETUPTZPRM:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPTZPRM, 			// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPMAINPRM:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPMAINPRM, 			// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPCTDPRM:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPCTDPRM, 			// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPOCPRM:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPOCPRM, 			// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPSMETAPRM:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPSMETAPRM, 		// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPIMDPRM:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPIMDPRM, 			// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
		case VSYS_VAGUI_SETUPENCROIENABLE:
			if (gVsysModuleContext.vaId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.vaId, 			// dest link id
								   VALINK_CMD_GUISETUPENCROIENABLE, 	// cmd
								   pVaGuiPrm,                  			// prm
								   sizeof(VaLink_GuiParams), 			// prm size
								   TRUE);                      			// wait for ack
			}
            break;
	}

	return 0;
}

Int32 Vsys_printDetailedStatistics()
{
    UInt32 devId;

    if(gVcamModuleContext.cameraId!=SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVcamModuleContext.cameraId,
                            CAMERA_LINK_CMD_PRINT_ADV_STATISTICS,
                            NULL, 0, TRUE);
    }
    if(gVsysModuleContext.fdId!=SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVsysModuleContext.fdId,
                            FD_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
    }
    if(gVsysModuleContext.swOsdId!=SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVsysModuleContext.swOsdId,
                            SWOSD_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
    }

	if(gVcamModuleContext.sclrId[0]!=SYSTEM_LINK_ID_INVALID)
	{
		 System_linkControl(gVcamModuleContext.sclrId[0],
							SCLR_LINK_CMD_PRINT_STATISTICS,
							NULL, 0, TRUE);
	}



	if(gVcamModuleContext.nsfId!=SYSTEM_LINK_ID_INVALID)
	{
		 System_linkControl(gVcamModuleContext.nsfId,
							NSF_LINK_CMD_PRINT_STATISTICS,
							NULL, 0, TRUE);
	}


    OSA_waitMsecs(2000); // allow for print to complete

    for(devId=0; devId<MAX_DEI_LINK; devId++)
    {
        if(gVcamModuleContext.deiId[devId]!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVcamModuleContext.deiId[devId],
                            DEI_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
        }
    }

    for(devId=0; devId<VDIS_DEV_MAX; devId++)
    {
        if(gVdisModuleContext.swMsId[devId]!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVdisModuleContext.swMsId[devId],
                            SYSTEM_SW_MS_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
        }
    }

    for(devId=0; devId<VDIS_DEV_MAX; devId++)
    {
        if(gVdisModuleContext.displayId[devId]!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVdisModuleContext.displayId[devId],
                            DISPLAY_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
        }
    }
    for(devId=0; devId<MAX_IPC_FRAMES_LINK; devId++)
    {
        if(gVcamModuleContext.ipcFramesOutVpssId!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVcamModuleContext.ipcFramesOutVpssId,
                            IPCFRAMESOUTRTOS_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
        }
    }


    OSA_waitMsecs(2000); // allow for print to complete

    for(devId=0; devId<MAX_ALG_LINK; devId++)
    {
        if(gVcapModuleContext.dspAlgId[devId]!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVcapModuleContext.dspAlgId[devId],
                            ALG_LINK_SCD_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
        }
    }

    OSA_waitMsecs(200); // allow for print to complete

    if(gVencModuleContext.encId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVencModuleContext.encId,
                            ENC_LINK_CMD_PRINT_IVAHD_STATISTICS,
                            NULL, 0, TRUE);
    }
    else if (gVdecModuleContext.decId !=SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVdecModuleContext.decId,
                            DEC_LINK_CMD_PRINT_IVAHD_STATISTICS,
                            NULL, 0, TRUE);
     }

    if(gVencModuleContext.encId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVencModuleContext.encId,
                            ENC_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
    }
    if(gVdecModuleContext.decId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVdecModuleContext.decId,
                            DEC_LINK_CMD_PRINT_STATISTICS,
                            NULL, 0, TRUE);
    }

    OSA_waitMsecs(2000); // allow for print to complete

    System_linkControl(SYSTEM_VPSS_LINK_ID_MUX_0,
                        MUX_LINK_CMD_PRINT_STATISTICS,
                        NULL, 0, TRUE);

    OSA_waitMsecs(2000); // allow for print to complete

    if(gVdisModuleContext.mpSclrId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(SYSTEM_VPSS_LINK_ID_MP_SCLR_INST_0,
                                MP_SCLR_LINK_CMD_PRINT_STATISTICS,
                                NULL, 0, TRUE);
    }

    MultiCh_prfLoadPrint(TRUE,TRUE);
    return ERROR_NONE;
}

Int32 Vsys_printBufferStatistics()
{
    UInt32 devId;

    if(gVcapModuleContext.captureId!=SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVcapModuleContext.captureId,
                            CAPTURE_LINK_CMD_PRINT_BUFFER_STATISTICS,
                            NULL, 0, TRUE);
    }

   for(devId=0; devId<MAX_NSF_LINK; devId++)
    {
        if(gVcamModuleContext.nsfId!=SYSTEM_LINK_ID_INVALID)
        {
             System_linkControl(gVcamModuleContext.nsfId,
                                NSF_LINK_CMD_PRINT_BUFFER_STATISTICS,
                                NULL, 0, TRUE);
        }
    }

    for(devId=0; devId<MAX_DEI_LINK; devId++)
    {
        if(gVcamModuleContext.deiId[devId]!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVcamModuleContext.deiId[devId],
                            DEI_LINK_CMD_PRINT_BUFFER_STATISTICS,
                            NULL, 0, TRUE);
        }
    }

    for(devId=0; devId<VDIS_DEV_MAX; devId++)
    {
        if(gVdisModuleContext.swMsId[devId]!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVdisModuleContext.swMsId[devId],
                            SYSTEM_SW_MS_LINK_CMD_PRINT_BUFFER_STATISTICS,
                            NULL, 0, TRUE);
        }
    }

    for(devId=0; devId<VDIS_DEV_MAX; devId++)
    {
        if(gVdisModuleContext.displayId[devId]!=SYSTEM_LINK_ID_INVALID)
        {
            System_linkControl(gVdisModuleContext.displayId[devId],
                            DISPLAY_LINK_CMD_PRINT_BUFFER_STATISTICS,
                            NULL, 0, TRUE);
        }
    }

    OSA_waitMsecs(500);

    if(gVencModuleContext.encId !=SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVencModuleContext.encId,
                        ENC_LINK_CMD_PRINT_BUFFER_STATISTICS,
                        NULL, 0, TRUE);
    }

    if(gVdecModuleContext.decId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkControl(gVdecModuleContext.decId,
                            DEC_LINK_CMD_PRINT_BUFFER_STATISTICS,
                            NULL, 0, TRUE);
    }

    OSA_waitMsecs(500);
    return ERROR_NONE;
}



/**
 * \brief:
 *      This function gives CPU ID, board type and revision.
 * \input:
 *      VSYS_PLATFORM_INFO_S sturcture
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_getPlatformInfo(VSYS_PLATFORM_INFO_S *pInfo)
{
    Int32 status;

    SystemVpss_PlatformInfo  platformInfo;

    status = System_linkControl(
                SYSTEM_LINK_ID_M3VPSS,
                SYSTEM_M3VPSS_CMD_GET_PLATFORM_INFO,
                &platformInfo,
                sizeof(platformInfo),
                TRUE
                );

    UTILS_assert(status==OSA_SOK);

    /* Get CPU version */
    pInfo->cpuRev = platformInfo.cpuRev;

    if (pInfo->cpuRev >= SYSTEM_PLATFORM_CPU_REV_MAX)
    {
        pInfo->cpuRev = SYSTEM_PLATFORM_CPU_REV_UNKNOWN;
    }
    /* Detect board */
    pInfo->boardId = platformInfo.boardId;

    if (pInfo->boardId >= SYSTEM_PLATFORM_BOARD_MAX)
    {
        pInfo->boardId = SYSTEM_PLATFORM_BOARD_UNKNOWN;
    }

    /* Get base board revision */
    pInfo->boardRev = platformInfo.baseBoardRev;

    if (pInfo->boardRev >= SYSTEM_PLATFORM_BOARD_REV_MAX)
    {
        pInfo->boardRev = SYSTEM_PLATFORM_BOARD_REV_UNKNOWN;
    }

    if (pInfo->boardId != SYSTEM_PLATFORM_BOARD_UNKNOWN)
    {
        /* Get daughter card revision */
        pInfo->boardRev = platformInfo.dcBoardRev;
        if (pInfo->boardRev >= SYSTEM_PLATFORM_BOARD_REV_MAX)
        {
            pInfo->boardRev = SYSTEM_PLATFORM_BOARD_REV_UNKNOWN;
        }
    }

    return ERROR_NONE;
}

Void Vsys_getContext(VSYS_PARAMS_S * contextInfo)
{
    if (contextInfo != NULL)
    {
        memcpy(contextInfo,
               &gVsysModuleContext.vsysConfig,
               sizeof(VSYS_PARAMS_S));
    }

}

Void Vsys_switchFormatSD(Int32 standard)
{
	VCAM_CHN_DYNAMIC_PARAM_S psCamChnDynaParam;

	if ((standard == SYSTEM_STD_NTSC) || (standard == SYSTEM_STD_PAL))
	{
		if ((gVsysModuleContext.vsysConfig.systemUseCase != VSYS_USECASE_MULTICHN_TRISTREAM_LOWPWR) &&
			(gVsysModuleContext.vsysConfig.systemUseCase != VSYS_USECASE_DUALCHN_DEMO))
		{
			psCamChnDynaParam.sdFormat = standard;
			Vcam_setDynamicParamChn(1, &psCamChnDynaParam, VCAM_SD_FORMAT);		
		
			Vdis_stopDrv(VDIS_DEV_SD);

			Vdis_setResolution(VDIS_DEV_SD, standard);

			Vdis_startDrv(VDIS_DEV_SD);
		}
	}
}

Int32 Vsys_printMuxQChanMap(UInt32 instId)
{
    Int32 status;
    MuxLink_InQueChNum prm[SYSTEM_MAX_CH_PER_OUT_QUE];
    UInt32 i;

    if (instId == 0xFFFFFFFF)
    {
        instId = gVsysModuleContext.muxId;
    }

    memset(&prm, 0xFF, sizeof(prm));
    status = System_linkControl(instId, 
                        MUX_LINK_CMD_GET_QUE_CH_MAP,
                        &prm[0], 
                        (sizeof(MuxLink_InQueChNum) * SYSTEM_MAX_CH_PER_OUT_QUE)
                        , TRUE);
    if (status != ERROR_NONE)
    {
        OSA_printf("\n MUX Error - Could not get the map !!!\n");
        return status;
    }
    OSA_printf(" \n \n");
    OSA_printf("Mux Link Instance %d Channel map", 
        instId - SYSTEM_VPSS_LINK_ID_MUX_0);
    OSA_printf("+-------------------------+");
    OSA_printf("|Input  | Input  | Output |");
    OSA_printf("|Q No   | Chan   | Chan   |");
    OSA_printf("+-------------------------+");
    for (i = 0; i < SYSTEM_MAX_CH_PER_OUT_QUE; i++)
    {
        if (prm[i].inQueId < MUX_LINK_MAX_IN_QUE)
        {
            OSA_printf("|%7d| %7d| %7d|", prm[i].inQueId, prm[i].inChNum, i);
        }
    }
    OSA_printf("+-------------------------+\n\n");
    return status;
}


Int32 Vsys_setMuxQChanMap(UInt32 instId, 
                            VSYS_MUX_IN_Q_CHAN_MAP_INFO_S *mapParams)
{
    Int32 status;
    MuxLink_InQueChNum prm[SYSTEM_MAX_CH_PER_OUT_QUE];

    memset(&prm, 0xFF, sizeof(prm));
    if (instId == 0xFFFFFFFF)
    {
        instId = gVsysModuleContext.muxId;
    }
    memcpy(prm, mapParams,
        (sizeof(VSYS_MUX_IN_Q_CHAN_MAP_INFO_S) * SYSTEM_MAX_CH_PER_OUT_QUE));
    status = System_linkControl(instId, 
                        MUX_LINK_CMD_SET_QUE_CH_MAP,
                        &prm[0], 
                        (sizeof(MuxLink_InQueChNum) * SYSTEM_MAX_CH_PER_OUT_QUE)
                        , TRUE);
    if (status != ERROR_NONE)
    {
        OSA_printf("\n MUX Error - Could not set the map !!!\n");
        return status;
    }

    return ERROR_NONE;
}


Int32 Vsys_getMuxQChanMap(UInt32 instId, 
                            VSYS_MUX_IN_Q_CHAN_MAP_INFO_S *mapParams)
{
    Int32 status;
    MuxLink_InQueChNum prm[SYSTEM_MAX_CH_PER_OUT_QUE];

    if (instId == 0xFFFFFFFF)
    {
        instId = gVsysModuleContext.muxId;
    }

    memset(&prm, 0xFF, sizeof(prm));
    status = System_linkControl(instId, 
                        MUX_LINK_CMD_GET_QUE_CH_MAP,
                        &prm[0], 
                        (sizeof(MuxLink_InQueChNum) * SYSTEM_MAX_CH_PER_OUT_QUE)
                        , TRUE);
    if (status != ERROR_NONE)
    {
        OSA_printf("\n MUX Error - Could not get the map !!!\n");
        return status;
    }
    memcpy(mapParams, prm, 
        (sizeof(VSYS_MUX_IN_Q_CHAN_MAP_INFO_S) * SYSTEM_MAX_CH_PER_OUT_QUE));
    return ERROR_NONE;
}

Int32 Vsys_setDupDelayPrm(UInt32 linkId,UInt32 queueId,UInt32 syncQueueId)
{
	DupLink_DelayParams delayPrm;

	if(linkId != SYSTEM_LINK_ID_INVALID)
	{
		delayPrm.outQueId     = queueId;
		delayPrm.syncOutQueId = syncQueueId;
		
		System_linkControl(linkId, 										// dest link id
						   DUP_LINK_CMD_SET_DELAY_OUTPUT_QUE_PARAMS, 	// cmd
						   &delayPrm,                  					// prm
						   sizeof(DupLink_DelayParams), 				// prm size
						   TRUE);                      					// wait for ack
	}
			
	return ERROR_NONE;
}

Int32 Vsys_setNoiseFilter(UInt32 channelId,UI_NF_MODE noisefilterMode)
{
	CameraLink_outputDataFormat outDataFmt;
	VSYS_MUX_IN_Q_CHAN_MAP_INFO_S muxMap[70];
	UInt32 devId;
	
#if !defined(VA_ON_DSP) && !defined(USE_MCTNF)
	/* If VA is running on VPSS M3 then don't switch the NF */
	if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)	
	{
		return ERROR_NONE;
	}
#endif	

#if defined(ENABLE_TILER_CAMERA) || defined(MEMORY_256MB)
	/* 
	 * CASE 1:	
	 * HDVPSS NSF doesn't support TILED input memory.
	 * So if Camera TILER is enabled only ISS VNF is enabled. 
	 * 
	 * CASE 2:	
	 * In 256 MB memory scheme there is NOT enough memory to create
	 * both DSS and ISS NF so dynamic switching between NF is not
	 * supported.	
	 */	 
	return ERROR_NONE;
#endif
	
	if(noisefilterMode != gUI_mcfw_config.noisefilterMode)
	{
		gUI_mcfw_config.noisefilterMode = noisefilterMode;	
	}
	else
	{
		return ERROR_NONE;
	}
	
	/* Disable enc and display */
	Venc_disableChn(channelId);
	for(devId = 0; devId < VDIS_DEV_MAX; devId++)
    {
        if(gVdisModuleContext.displayId[devId] != SYSTEM_LINK_ID_INVALID)
        {
			Vdis_stopDrv(devId);
        }
    }
	
	/* 1. Disable NSF and VNF */
	if(gVcamModuleContext.nsfId != SYSTEM_LINK_ID_INVALID)
	{
		System_linkControl(gVcamModuleContext.nsfId, 					// dest link id
						   NSF_LINK_CMD_DISABLE_CHANNEL, 				// cmd
						   &channelId,                  				// prm
						   sizeof(channelId), 							// prm size
						   TRUE);                      					// wait for ack						   
	}
	else
	{
		return ERROR_FAIL;
	}	

#ifdef USE_MCTNF
	if(gVcamModuleContext.mctnfId != SYSTEM_LINK_ID_INVALID)
	{
		System_linkControl(gVcamModuleContext.mctnfId, 					// dest link id
						   MCTNF_LINK_CMD_DISABLE_CHANNEL, 				// cmd
						   &channelId,                  				// prm
						   sizeof(channelId), 							// prm size
						   TRUE);                      					// wait for ack	
	}	
	else
	{
		return ERROR_FAIL;
	}
#endif
	
	if(gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
	{
		System_linkControl(gVcamModuleContext.vnfId, 					// dest link id
						   VNF_LINK_CMD_DISABLE_CHANNEL, 				// cmd
						   &channelId,                  				// prm
						   sizeof(channelId), 							// prm size
						   TRUE);                      					// wait for ack	
	}	
	else
	{
		return ERROR_FAIL;
	}	
	
	/* 2. Restart camera/isp with new data format and width and height */
	if (gUI_mcfw_config.glbceEnable)
	{
		if(gVcamModuleContext.ispId != SYSTEM_LINK_ID_INVALID)
		{
			outDataFmt.streamId = channelId;
			
			if(noisefilterMode == DSS_VNF_ON)
			{
				outDataFmt.dataFmt = SYSTEM_DF_YUV422I_UYVY;
				outDataFmt.isVnfON = 0;
			}
			else
			{
				outDataFmt.dataFmt = SYSTEM_DF_YUV420SP_UV;
				outDataFmt.isVnfON = 1;
			}
		
			System_linkControl(gVcamModuleContext.ispId, 					// dest link id
							   ISPLINK_CMD_SET_OUTDATAFMT, 					// cmd
							   &outDataFmt,                  				// prm
							   sizeof(outDataFmt), 							// prm size
							   TRUE);                      					// wait for ack	
		}
		else
		{
			return ERROR_FAIL;
		}	
	}
	else
	{
		if(gVcamModuleContext.cameraId != SYSTEM_LINK_ID_INVALID)
		{
			outDataFmt.streamId = channelId;
			
			if(noisefilterMode == DSS_VNF_ON)
			{
				outDataFmt.dataFmt = SYSTEM_DF_YUV422I_UYVY;
				outDataFmt.isVnfON = 0;
			}
			else
			{
				outDataFmt.dataFmt = SYSTEM_DF_YUV420SP_UV;
				outDataFmt.isVnfON = 1;
			}
		
			System_linkControl(gVcamModuleContext.cameraId, 				// dest link id
							   CAMERA_LINK_CMD_SET_OUTPUT_DATAFORMAT, 		// cmd
							   &outDataFmt,                  				// prm
							   sizeof(outDataFmt), 							// prm size
							   TRUE);                      					// wait for ack	
		}
		else
		{
			return ERROR_FAIL;
		}
	}	
	
	/* 3. Configure MUX link output channel map */
	if(gVsysModuleContext.muxIdVnf != SYSTEM_LINK_ID_INVALID)
	{
		Vsys_getMuxQChanMap(gVsysModuleContext.muxIdVnf,&muxMap[0]);
		
		if(noisefilterMode == DSS_VNF_ON)
		{
			muxMap[0].inQueId = 0;
			muxMap[0].inChNum = 0;			
		}
		else
		{
			muxMap[0].inQueId = 1;
			muxMap[0].inChNum = 0;		
		}
		
		Vsys_setMuxQChanMap(gVsysModuleContext.muxIdVnf,&muxMap[0]);
	}
	else
	{
		return ERROR_FAIL;
	}
		
	/* 4. Enable selected Noise filter */
	if(noisefilterMode == DSS_VNF_ON)
	{
		System_linkControl(gVcamModuleContext.nsfId, 					// dest link id
						   NSF_LINK_CMD_ENABLE_CHANNEL, 				// cmd
						   &channelId,                  				// prm
						   sizeof(channelId), 							// prm size
						   TRUE);                      					// wait for ack	
	}
	else
	{
#ifdef USE_MCTNF
		System_linkControl(gVcamModuleContext.mctnfId, 					// dest link id
						   MCTNF_LINK_CMD_ENABLE_CHANNEL, 				// cmd
						   &channelId,                  				// prm
						   sizeof(channelId), 							// prm size
						   TRUE);                      					// wait for ack
#endif	
		System_linkControl(gVcamModuleContext.vnfId, 					// dest link id
						   VNF_LINK_CMD_ENABLE_CHANNEL, 				// cmd
						   &channelId,                  				// prm
						   sizeof(channelId), 							// prm size
						   TRUE);                      					// wait for ack		
	}
	
	/* 50 msec delay */
	usleep(50 * 1000);
	
	/* Enable enc and display */
	Venc_enableChn(channelId);
	for(devId = 0; devId < VDIS_DEV_MAX; devId++)
    {
        if(gVdisModuleContext.displayId[devId] != SYSTEM_LINK_ID_INVALID)
        {
			Vdis_startDrv(devId);
        }
    }
	
	return ERROR_NONE;
}

Int32 Vsys_setImgAlgPrm(VSYS_DSPIMG_SETPARAM cmd,void * pParam)
{
    int ret = 0;
    Vsys_DspImgAlgPrm params;

    switch(cmd)
    {
    	case SYS_IMGALG_ENABLE:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						IMGLINK_CMD_ALG_ENABLE,   							// cmd
						&params,                  								// prm
						sizeof(Vsys_DspImgAlgPrm), 		// prm size
						TRUE);                      		// wait for ack
			}
        break;
    	case SYS_IMGALG_DISABLE:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						SYS_IMGALG_DISABLE,   							// cmd
						&params,                  								// prm
						sizeof(Vsys_DspImgAlgPrm), 		// prm size
						TRUE);                      		// wait for ack
			}
        break;
    	case SYS_IMGALG_STATIC_IMG:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						IMGLINK_CMD_STATIC_IMG,   							// cmd
						&params,                  								// prm
						sizeof(Vsys_DspImgAlgPrm), 		// prm size
						TRUE);                      		// wait for ack
			}
        break;
    	case SYS_IMGALG_SENSOR_IMG:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						IMGLINK_CMD_SENSOR_IMG,   							// cmd
						&params,                  								// prm
						sizeof(Vsys_DspImgAlgPrm), 		// prm size
						TRUE);                      		// wait for ack
			}
        break; 
    	case SYS_SET_IMGALG_PARAM:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				memcpy(&params,pParam,sizeof(Vsys_DspImgAlgPrm));
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						IMGLINK_CMD_SET_ALG_PARAM,   							// cmd
						&params,                  								// prm
						sizeof(Vsys_DspImgAlgPrm), 		// prm size
						TRUE);                      		// wait for ack
			}
        break; 
    	case SYS_GET_IMGALG_PARAM:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						IMGLINK_CMD_GET_ALG_PARAM,   							// cmd
						&params,                  								// prm
						sizeof(Vsys_DspImgAlgPrm), 							// prm size
						TRUE);  											// wait for ack
				memcpy(pParam,&params,sizeof(Vsys_DspImgAlgPrm));		                    						
			}
        break;
    	case SYS_ENABLE_IMGALG_DBGMODE:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						IMGLINK_CMD_DEBUG_MODE,   							// cmd
						NULL,                  								// prm
						0, 							// prm size
						TRUE);  											// wait for ack	                    						
			}
        break; 
     	case SYS_DISABLE_IMGALG_DBGMODE:
			if (gVsysModuleContext.dspImgId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gVsysModuleContext.dspImgId, 				// dest link id
						IMGLINK_CMD_NORMAL_MODE,   							// cmd
						NULL,                  								// prm
						0, 							// prm size
						TRUE);  						                						
			}
        break;                                                        
   	    default:
        	ret = -1;
        	break;
    }
    
    return ret;
}
