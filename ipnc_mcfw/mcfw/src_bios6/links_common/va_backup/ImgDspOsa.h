#ifndef IMGDSPOSA_H
#define IMGDSPOSA_H

#include <mcfw/src_bios6/utils/utils_mem.h>
#include <mcfw/src_bios6/utils/utils.h>

extern void* Bios_Malloc(Uint32 numBytes);
extern void  Bios_Free (void* ptr, Uint32 size);
extern void* Bios_CreateSem(void);
extern void  Bios_DeleteSem(void* semHandle);
extern void  Bios_PendSem(void* semHandle);
extern void  Bios_PostSem(void* semHandle);



#endif
