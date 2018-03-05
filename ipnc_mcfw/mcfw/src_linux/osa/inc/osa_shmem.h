#ifndef OSA_SHARE_MEM_H
#define OSA_SHARE_MEM_H

typedef struct
{
  unsigned int memAddr;
  unsigned int memSize;
  unsigned int mmapMemAddr;
  unsigned int mmapMemSize;  
  unsigned int memOffset;

  int    memDevFd;
  volatile unsigned int *pMemVirtAddr;
  
}OSA_FramesMMapCtrl;


int OSA_ShareMemInit(int key,unsigned int size);

void *OSA_ShaaeMemGetPtr(int mid);

void OSA_ShareMemWrite(int mid,int offset,void *buf, unsigned int length);

void OSA_ShareMemRead(int mid,int offset,void *buf, int length);

int OSA_FrameUnmapMem(OSA_FramesMMapCtrl *pCtl);

int OSA_FrameMapMem(OSA_FramesMMapCtrl *pCtl,unsigned physAddr, unsigned memSize , unsigned *pMemVirtAddr);

int OSA_FrameUnmapMem(OSA_FramesMMapCtrl *pCtl);
#endif
