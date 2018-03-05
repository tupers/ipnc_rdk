#include "ImgDspOsa.h"

void* Bios_Malloc(Uint32 numBytes)
{
	Ptr p;
	p = Utils_memAlloc(numBytes, 0);

	UTILS_assert(p!=NULL);

	return p;
}

void  Bios_Free (void* ptr, Uint32 size)
{
	if(NULL!=ptr)
		Utils_memFree(ptr,size);
}

void* Bios_CreateSem()
{
	//Semaphore_Params semParams;
	Semaphore_Handle semhdl;

	//Semaphore_Params_init(&semParams);
	//semParams.mode = Semaphore_Mode_BINARY;


	//semhdl = Semaphore_create(0, semParams,NULL);
	semhdl = Semaphore_create(0, NULL,NULL);
	
	UTILS_assert(semhdl!=NULL);

	return semhdl;
}
void  Bios_DeleteSem(void* semHandle)
{
	Semaphore_delete((Semaphore_Handle *)semHandle);
}

void  Bios_PendSem(void* semHandle)
{
	Semaphore_pend((Semaphore_Handle)semHandle, BIOS_WAIT_FOREVER);
}

void  Bios_PostSem(void* semHandle)
{
	Semaphore_post((Semaphore_Handle)semHandle);
}
