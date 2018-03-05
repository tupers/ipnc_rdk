/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2011 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \file ti_mcfw_ipcframes.c
    \brief
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <osa_que.h>
#include <osa_mutex.h>
#include <osa_thr.h>
#include <osa_sem.h>
#include "ti_mcfw_ipcframes.h"
#include "mcfw/interfaces/ti_vcam.h"
#include "mcfw/interfaces/ti_vdis.h"

#ifdef CUSTOM_SD_DEMO
#include <osa_dma.h>

App_IpcFramesMMapCtrl gMMapCtrl;
#define FILE_WRITE_STOPPED  (0)
#define FILE_WRITE_RUNNING  (1)
#endif

#define MCFW_IPC_FRAMES_NONOTIFYMODE_FRAMESIN                          (TRUE)
#define MCFW_IPC_FRAMES_NONOTIFYMODE_FRAMESOUT                         (TRUE)

#define MCFW_IPCFRAMES_SENDRECVFXN_TSK_PRI                             (2)
#define MCFW_IPCFRAMES_SENDRECVFXN_TSK_STACK_SIZE                      (0) /* 0 means system default will be used */
#define MCFW_IPCFRAMES_SENDRECVFXN_PERIOD_MS                           (16)


#define MCFW_IPCFRAMES_INFO_PRINT_INTERVAL                             (8192)

#define MCFW_IPCFRAMES_MAX_PENDING_RECV_SEM_COUNT                      (10)
#define MCFW_IPC_FRAMES_TRACE_ENABLE_FXN_ENTRY_EXIT                    (1)
#define MCFW_IPC_FRAMES_TRACE_INFO_PRINT_INTERVAL                      (8192)


#if MCFW_IPC_FRAMES_TRACE_ENABLE_FXN_ENTRY_EXIT
#define MCFW_IPC_FRAMES_TRACE_FXN(str,...)         do {                           \
                                                     static Int printInterval = 0;\
                                                     if ((printInterval % MCFW_IPC_FRAMES_TRACE_INFO_PRINT_INTERVAL) == 0) \
                                                     {                                                          \
                                                         OSA_printf("MCFW_IPCFRAMES:%s function:%s",str,__func__);     \
                                                         OSA_printf(__VA_ARGS__);                               \
                                                     }                                                          \
                                                     printInterval++;                                           \
                                                   } while (0)
#define MCFW_IPC_FRAMES_TRACE_FXN_ENTRY(...)                  MCFW_IPC_FRAMES_TRACE_FXN("Entered",__VA_ARGS__)
#define MCFW_IPC_FRAMES_TRACE_FXN_EXIT(...)                   MCFW_IPC_FRAMES_TRACE_FXN("Leaving",__VA_ARGS__)
#else
#define MCFW_IPC_FRAMES_TRACE_FXN_ENTRY(...)
#define MCFW_IPC_FRAMES_TRACE_FXN_EXIT(...)
#endif



static void  App_ipcFramesPrintFrameInfo(VIDEO_FRAMEBUF_S *buf)
{
    OSA_printf("MCFW_IPCFRAMES:VIDFRAME_INFO:"
               "chNum:%d\t"
               "fid:%d\t"
               "frameWidth:%d\t"
               "frameHeight:%d\t"
               "timeStamp:%d\t"
               "virtAddr[0][0]:%p\t"
               "phyAddr[0][0]:%p",
                buf->channelNum,
                buf->fid,
                buf->frameWidth,
                buf->frameHeight,
                buf->timeStamp,
                buf->addr[0][0],
                buf->phyAddr[0][0]);

}


static void  App_ipcFramesPrintFullFrameListInfo(VIDEO_FRAMEBUF_LIST_S *bufList,
                                                char *listName)
{
    static Int printStatsInterval = 0;
    if ((printStatsInterval % MCFW_IPCFRAMES_INFO_PRINT_INTERVAL) == 0)
    {
        Int i;

        OSA_printf("MCFW_IPCFRAMES:VIDFRAMELIST_INFO:%s\t"
                   "numFullFrames:%d",
                   listName,
                   bufList->numFrames);
        for (i = 0; i < bufList->numFrames; i++)
        {
             App_ipcFramesPrintFrameInfo(&bufList->frames[i]);
        }
    }
    printStatsInterval++;
}


static Void * App_ipcFramesSendRecvFxn(Void * prm)
{
     App_IpcFramesCtrlThrObj *thrObj = ( App_IpcFramesCtrlThrObj *) prm;
    static Int printStatsInterval = 0;
    VIDEO_FRAMEBUF_LIST_S bufList;
    Int status;

#ifdef CUSTOM_SD_DEMO
    App_ipcFramesFileOpen();
#endif

    OSA_printf("MCFW_IPCFRAMES:%s:Entered...",__func__);
    OSA_semWait(&thrObj->framesInNotifySem,OSA_TIMEOUT_FOREVER);
    OSA_printf("MCFW_IPCFRAMES:Received first frame notify...");
    while (FALSE == thrObj->exitFramesInOutThread)
    {
        status =  Vcam_getFullVideoFrames(&bufList,0);
        OSA_assert(0 == status);
        if (bufList.numFrames)
        {
             App_ipcFramesPrintFullFrameListInfo(&bufList,"FullFrameList");
#ifdef CUSTOM_SD_DEMO
             App_ipcFrameFileWrite(&bufList);
#endif
            status = Vdis_putFullVideoFrames(&bufList);
            OSA_assert(0 == status);
        }
        status =  Vdis_getEmptyVideoFrames(&bufList,0);
        OSA_assert(0 == status);

        if (bufList.numFrames)
        {
             App_ipcFramesPrintFullFrameListInfo(&bufList,"EmptyFrameList");
            status = Vcam_putEmptyVideoFrames(&bufList);
            OSA_assert(0 == status);
        }
#ifdef IPC_FRAMES_DEBUG
        if ((printStatsInterval % MCFW_IPCFRAMES_INFO_PRINT_INTERVAL) == 0)
        {
            OSA_printf("MCFW_IPCFRAMES:%s:INFO: periodic print..",__func__);
        }
#endif
        printStatsInterval++;
        OSA_waitMsecs(MCFW_IPCFRAMES_SENDRECVFXN_PERIOD_MS);
    }
#ifdef CUSTOM_SD_DEMO
    if(gApp_ipcFramesCtrl.fileFrameWriteEnable)
    {
        if(gApp_ipcFramesCtrl.fileFrameWriteState==FILE_WRITE_RUNNING)
        {
            fclose(gApp_ipcFramesCtrl.fp);
            OSA_printf(" Closing file [%s] for CH%d\n", gApp_ipcFramesCtrl.fileFrameWriteName, gApp_ipcFramesCtrl.fileFrameWriteChn);
        }
    }
#endif
    OSA_printf("MCFW_IPCFRAMES:%s:Leaving...",__func__);
    return NULL;
}


static Void  App_ipcFramesInitThrObj( App_IpcFramesCtrlThrObj *thrObj)
{

    OSA_semCreate(&thrObj->framesInNotifySem,
                  MCFW_IPCFRAMES_MAX_PENDING_RECV_SEM_COUNT,0);
    thrObj->exitFramesInOutThread = FALSE;
    OSA_thrCreate(&thrObj->thrHandleFramesInOut,
                   App_ipcFramesSendRecvFxn,
                  MCFW_IPCFRAMES_SENDRECVFXN_TSK_PRI,
                  MCFW_IPCFRAMES_SENDRECVFXN_TSK_STACK_SIZE,
                  thrObj);

}

static Void  App_ipcFramesDeInitThrObj( App_IpcFramesCtrlThrObj *thrObj)
{
    thrObj->exitFramesInOutThread = TRUE;
    OSA_thrDelete(&thrObj->thrHandleFramesInOut);
    OSA_semDelete(&thrObj->framesInNotifySem);
}


static
Void  App_ipcFramesInCbFxn (Ptr cbCtx)
{
     App_IpcFramesCtrl *ipcFramesCtrl;
    static Int printInterval;

    OSA_assert(cbCtx = &gApp_ipcFramesCtrl.ipcFrames);
    ipcFramesCtrl = cbCtx;
    OSA_semSignal(&ipcFramesCtrl->thrObj.framesInNotifySem);
    if ((printInterval % MCFW_IPCFRAMES_INFO_PRINT_INTERVAL) == 0)
    {
        OSA_printf("MCFW_IPCFRAMES: Callback function:%s",__func__);
    }
    printInterval++;
}


Void  App_ipcFramesInSetCbInfo ()
{
    VCAM_CALLBACK_S VcamCallback;

    VcamCallback.newDataAvailableCb =  App_ipcFramesInCbFxn;

    Vcam_registerCallback(&VcamCallback, &gApp_ipcFramesCtrl.ipcFrames);
}

Int32  App_ipcFramesCreate(int demoId)
{

#ifdef CUSTOM_SD_DEMO
    if(demoId == 1)
    {
         App_ipcFramesFileWriteCreate();
    }
    else
    {
        gApp_ipcFramesCtrl.fileFrameWriteChn    = 0;
        gApp_ipcFramesCtrl.fileFrameWriteEnable = FALSE;

    }
#endif
     App_ipcFramesInitThrObj(&gApp_ipcFramesCtrl.ipcFrames.thrObj);
    return OSA_SOK;
}

Void  App_ipcFramesStop(void)
{
    gApp_ipcFramesCtrl.ipcFrames.thrObj.exitFramesInOutThread = TRUE;
}

Int32  App_ipcFramesDelete()
{
    OSA_printf("Entered:%s...",__func__);
    App_ipcFramesDeInitThrObj(&gApp_ipcFramesCtrl.ipcFrames.thrObj);
    OSA_printf("Leaving:%s...",__func__);
    return OSA_SOK;
}

#ifdef CUSTOM_SD_DEMO

Void App_ipcFramesFileWriteCreate()
{
    OSA_printf("\nEnable RAW Frame Write\n\n");
    gApp_ipcFramesCtrl.fileFrameWriteChn = 0;

    gApp_ipcFramesCtrl.fileFrameWriteEnable = Demo_getFileWriteEnable();

    if(gApp_ipcFramesCtrl.fileFrameWriteEnable)
    {
        char path[256];

        Demo_getFileWritePath(path, "/dev/shm");

        gApp_ipcFramesCtrl.fileFrameWriteChn = Demo_getChId("FRAME File Write", gDemo_info.maxVcamChannels);

        sprintf(gApp_ipcFramesCtrl.fileFrameWriteName, "%s/VID_CH%02d.yuv", path, gApp_ipcFramesCtrl.fileFrameWriteChn);
    }
}

Void App_ipcFramesFileOpen()
{
    gApp_ipcFramesCtrl.fp = NULL;

    gApp_ipcFramesCtrl.fileFrameWriteState = FILE_WRITE_STOPPED;
    if(gApp_ipcFramesCtrl.fileFrameWriteEnable)
    {
        gApp_ipcFramesCtrl.fp = fopen(gApp_ipcFramesCtrl.fileFrameWriteName, "wb");
        if(gApp_ipcFramesCtrl.fp!=NULL)
        {
            gApp_ipcFramesCtrl.fileFrameWriteState = FILE_WRITE_RUNNING;
            OSA_printf(" Opened file [%s] for writing CH%d\n", gApp_ipcFramesCtrl.fileFrameWriteName, gApp_ipcFramesCtrl.fileFrameWriteChn);
        }
        else
        {
            OSA_printf(" ERROR: File open [%s] for writing CH%d FAILED !!!!\n", gApp_ipcFramesCtrl.fileFrameWriteName, gApp_ipcFramesCtrl.fileFrameWriteChn);
        }
    }
}
Void App_ipcFrameFileWrite(VIDEO_FRAMEBUF_LIST_S *pFrameBufList)
{
    UInt32 writeDataSize;
    VIDEO_FRAMEBUF_S *pBuf;
    App_IpcFramesChInfo *pChInfo;
    UInt32 frameSize = 0, frameId;

    for(frameId=0; frameId<pFrameBufList->numFrames; frameId++)
    {
        pBuf = &pFrameBufList->frames[frameId];
        if(pBuf->channelNum<VENC_CHN_MAX)
        {

            pChInfo = &gApp_ipcFramesCtrl.chFrameInfo[pBuf->channelNum];

            frameSize = (pBuf->frameWidth * pBuf->frameHeight) << 1;

            pChInfo->totalDataSize += frameSize;

            pChInfo->numFrames++;

            if(pBuf->frameWidth > pChInfo->maxWidth)
                pChInfo->maxWidth = pBuf->frameWidth;

            if(pBuf->frameHeight > pChInfo->maxHeight)
                pChInfo->maxHeight = pBuf->frameHeight;

        }
        if(gApp_ipcFramesCtrl.fileFrameWriteEnable)
        {
            if(pBuf->channelNum== gApp_ipcFramesCtrl.fileFrameWriteChn && gApp_ipcFramesCtrl.fileFrameWriteState == FILE_WRITE_RUNNING)
            {
                UInt32 pMemVirtAddr;
                pMemVirtAddr = 0;
                App_mMap((UInt32)(pBuf->phyAddr[0][0]), frameSize, &pMemVirtAddr);

                writeDataSize = fwrite((Ptr) pMemVirtAddr, 1, frameSize, gApp_ipcFramesCtrl.fp);
                if(writeDataSize!=frameSize)
                {
                    gApp_ipcFramesCtrl.fileFrameWriteState = FILE_WRITE_STOPPED;
                    fclose(gApp_ipcFramesCtrl.fp);
                    OSA_printf(" Closing file [%s] for CH%d\n", gApp_ipcFramesCtrl.fileFrameWriteName, gApp_ipcFramesCtrl.fileFrameWriteChn);
                }
                App_unmapMem();
            }
        }
    }
}
Int32 App_mMap(UInt32 physAddr, Uint32 memSize , UInt32 *pMemVirtAddr)
{
    gMMapCtrl.memDevFd = open("/dev/mem",O_RDWR|O_SYNC);

    if(gMMapCtrl.memDevFd < 0)
    {
      OSA_printf(" ERROR: /dev/mem open failed !!!\n");
      return -1;
    }


    gMMapCtrl.memOffset   = physAddr & MMAP_MEM_PAGEALIGN;

    gMMapCtrl.mmapMemAddr = physAddr - gMMapCtrl.memOffset;

    gMMapCtrl.mmapMemSize = memSize + gMMapCtrl.memOffset;

    gMMapCtrl.pMemVirtAddr = mmap(
           (void	*)gMMapCtrl.mmapMemAddr,
           gMMapCtrl.mmapMemSize,
           PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,
           gMMapCtrl.memDevFd,
           gMMapCtrl.mmapMemAddr
           );

   if (gMMapCtrl.pMemVirtAddr==NULL)
   {
     OSA_printf(" ERROR: mmap() failed !!!\n");
     return -1;
   }
    *pMemVirtAddr = (UInt32)((UInt32)gMMapCtrl.pMemVirtAddr + gMMapCtrl.memOffset);

    return 0;
}

Int32 App_unmapMem()
{
    if(gMMapCtrl.pMemVirtAddr)
      munmap((void*)gMMapCtrl.pMemVirtAddr, gMMapCtrl.mmapMemSize);

    if(gMMapCtrl.memDevFd >= 0)
      close(gMMapCtrl.memDevFd);

    return 0;
}

#endif
