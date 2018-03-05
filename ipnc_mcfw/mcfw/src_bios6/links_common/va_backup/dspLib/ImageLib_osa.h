/*
 * ImageLib_osa.h
 *
 *  Created on: 2015-9-25
 *      Author: Administrator
 */

#ifndef IMAGELIB_OSA_H_
#define IMAGELIB_OSA_H_

extern void* Bios_Malloc(unsigned int numBytes);
extern void  Bios_Free (void* ptr, unsigned int size);
extern void* Bios_CreateSem(void);
extern void  Bios_DeleteSem(void* semHandle);
extern void  Bios_PendSem(void* semHandle);
extern void  Bios_PostSem(void* semHandle);


#endif /* IMAGELIB_OSA_H_ */
