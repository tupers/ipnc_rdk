#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/errno.h>
#include "util_shmem.h"

#define MMAP_MEM_PAGEALIGN         (4*1024-1)

int Utils_ShareMemInit(int key,unsigned int size)
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
void Utils_ShareMemWrite(int mid,int offset,void *buf, unsigned int length)
{
	char *pDst = shmat(mid,0,0);              	
	memcpy(pDst + offset, buf, length);
	shmdt(pDst);
}

void *Utils_ShaaeMemGetPtr(int mid)
{
	return shmat(mid,0,0); 
}
void Utils_ShareMemRead(int mid,int offset,void *buf, int length)
{
	char *pSrc = shmat(mid,0,0);
	memcpy(buf, pSrc + offset, length);
	shmdt(pSrc);
}
