#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/errno.h>
#include "osa_shmem.h"
#include "osa.h"

#define MMAP_MEM_PAGEALIGN         (4*1024-1)

int OSA_ShareMemInit(int key,unsigned int size)
{
	int mid;
	mid = shmget(key, size, IPC_CREAT | 0660);
	if(mid < 0)
		mid = shmget(key, 0, 0);

	if(mid < 0)
		return -1;
	return mid;
}


/**
* @brief Write shared memory driver.

* Write shared momory.
* @param offset [i]  memory offset
* @param *buf [i]  pointer to memory buffer
* @param length [i]  data length to read
*/
void OSA_ShareMemWrite(int mid,int offset,void *buf, unsigned int length)
{
	char *pDst = shmat(mid,0,0);              	
	memcpy(pDst + offset, buf, length);
	shmdt(pDst);
}

void *OSA_ShaaeMemGetPtr(int mid)
{
	return shmat(mid,0,0); 
}
void OSA_ShareMemRead(int mid,int offset,void *buf, int length)
{
	char *pSrc = shmat(mid,0,0);
	memcpy(buf, pSrc + offset, length);
	shmdt(pSrc);
}


int OSA_FrameMapMem(OSA_FramesMMapCtrl *pCtrl,unsigned physAddr, unsigned memSize , unsigned *pMemVirtAddr)
{
    pCtrl->memDevFd = open("/dev/mem",O_RDWR|O_SYNC);

    if(pCtrl->memDevFd < 0)
    {
      OSA_printf(" ERROR: /dev/mem open failed !!!\n");
      return -1;
    }

    pCtrl->memOffset   = physAddr & MMAP_MEM_PAGEALIGN;

    pCtrl->mmapMemAddr = physAddr - pCtrl->memOffset;

    pCtrl->mmapMemSize = memSize + pCtrl->memOffset;

    pCtrl->pMemVirtAddr = mmap(
           (void	*)pCtrl->mmapMemAddr,
           pCtrl->mmapMemSize,
           PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,
           pCtrl->memDevFd,
           pCtrl->mmapMemAddr
           );

   if (pCtrl->pMemVirtAddr==NULL)
   {
     OSA_printf(" ERROR: mmap() failed !!!\n");
     return -1;
   }
    *pMemVirtAddr = (UInt32)((UInt32)pCtrl->pMemVirtAddr + pCtrl->memOffset);

    return 0;
}

int OSA_FrameUnmapMem(OSA_FramesMMapCtrl *pCtrl)
{
    if(pCtrl->pMemVirtAddr)
      munmap((void*)pCtrl->pMemVirtAddr, pCtrl->mmapMemSize);

    if(pCtrl->memDevFd >= 0)
      close(pCtrl->memDevFd);

    return 0;
}
