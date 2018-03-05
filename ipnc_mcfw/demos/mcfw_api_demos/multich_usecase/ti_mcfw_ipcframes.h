/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2011 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/



#ifndef APP_IPCFRAMES_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define APP_IPCFRAMES_H_

#include <sys/mman.h>
#include <osa_thr.h>
#include <osa_sem.h>
#include <ti_venc.h>

//#define CUSTOM_SD_DEMO
#define IPC_FRAMES_DEBUG

#define MMAP_MEM_PAGEALIGN         (4*1024-1)

typedef struct {

    UInt32 totalDataSize;
    UInt32 numKeyFrames;
    UInt32 numFrames;
    UInt32 maxWidth;
    UInt32 minWidth;
    UInt32 maxHeight;
    UInt32 minHeight;
    UInt32 maxLatency;
    UInt32 minLatency;

} App_IpcFramesChInfo;

typedef struct App_IpcFramesCtrlThrObj {
    OSA_ThrHndl thrHandleFramesInOut;
    OSA_SemHndl framesInNotifySem;
    volatile Bool exitFramesInOutThread;
} App_IpcFramesCtrlThrObj;

typedef struct App_IpcFramesCtrl {
    Bool  noNotifyFramesInHLOS;
    Bool  noNotifyFramesOutHLOS;
    App_IpcFramesCtrlThrObj  thrObj;
} App_IpcFramesCtrl;

typedef struct {

    OSA_ThrHndl wrThrHndl;
    OSA_SemHndl wrSem;
    Bool exitWrThr;
    Bool isWrThrStopDone;

    App_IpcFramesChInfo chInfo[VENC_CHN_MAX];

    UInt32 statsStartTime;

    Bool fileWriteEnable;
    char fileWriteName[512];
    int  fileWriteChn;
    App_IpcFramesCtrl ipcFrames;

#ifdef CUSTOM_SD_DEMO
    App_IpcFramesChInfo chFrameInfo[VCAP_CHN_MAX];
    FILE *fp;
    Bool fileFrameWriteEnable;
    char fileFrameWriteName[512];
    int  fileFrameWriteChn;
    int fileFrameWriteState;
#endif
} App_Ctrl;

#ifdef CUSTOM_SD_DEMO
typedef struct
{
  unsigned int memAddr;
  unsigned int memSize;
  unsigned int mmapMemAddr;
  unsigned int mmapMemSize;  
  unsigned int memOffset;

  int    memDevFd;
  volatile unsigned int *pMemVirtAddr;
  
} App_IpcFramesMMapCtrl;
#endif

extern App_Ctrl gApp_ipcFramesCtrl;


Int32 App_ipcFramesCreate(int demoId);
Int32 App_ipcFramesDelete();
Void  App_ipcFramesStop(void);
Void  App_ipcFramesInSetCbInfo (void);

#ifdef CUSTOM_SD_DEMO
Void  App_ipcFramesFileWriteCreate();
Void  App_ipcFramesFileOpen();
Void  App_ipcFrameFileWrite(VIDEO_FRAMEBUF_LIST_S * pFrameBufList);

Int32 App_mMap(UInt32 physAddr, Uint32 memSize , UInt32 * pMemVirtAddr);
Int32 App_unmapMem();
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
