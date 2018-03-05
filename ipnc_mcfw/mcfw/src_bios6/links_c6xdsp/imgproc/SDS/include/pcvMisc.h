/*
 * @(#)$Id: pcvMisc.h 2786 2012-08-21 06:27:37Z LvGangXiao $
 * @(#) Declaration file of class pcvMisc
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file
 * Declaration of pcvMisc
 *
 * TO DESC : FILE DETAIL DESCRIPTION, BUT DON'T DESCRIBE CLASS DETAIL HERE.
 */
#ifndef CXX_PCVMISC_H
#define CXX_PCVMISC_H

#include "pcvPlatformdef.h"

/////////// ***  Memory Operation *** ////////////////
#define MV_MIN_ALIGN 4
#define MV_ALIGN(v, a) ((v) + (a) - 1) & ~((a) - 1)
#define MV_ALIGNADDR(p, a) ((p) + (a) - ((UINT)p & ((a) - 1)))

#define PRNT_BUFF_SIZE 1024
typedef FILE MV_FILE;

#ifdef WIN32
#define PERF_COUNTER LONGLONG
#elif defined(TRON)
#define PERF_COUNTER UDLONG
#define MAKE_UDLONG(u, l) ((static_cast<UDLONG>(u) << 32) | (l))
#elif defined(C64X)
#define PERF_COUNTER UINT
#elif defined(ANDROID)
#define PERF_COUNTER DOUBLE
#elif defined(_FOR_IPHONE_)
#define PERF_COUNTER DOUBLE
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif
    //Memory Operation Functions
    VOID* MV_Malloc(UINT size);
    VOID MV_Free(VOID *pBlock);
    VOID MV_Memcpy(VOID *pDst, CONST VOID *pSrc, UINT iLen);
    VOID MV_Memset(VOID *pDst, INT iValue, UINT iCount);
    VOID* MV_MallocAlign(UINT size, UINT alignment, VOID **pBlockAlign);
    MV_FIXED Sat16(MV_FIXED iNum, INT iShift);

    //Message Output Functions
    VOID MV_Printf1(CONST CHAR *pString);
    VOID MV_Printf(CONST CHAR *format, ...);

#ifdef WIN32
    //Performance Functions
    PERF_COUNTER MV_GetTickCount();
	PERF_COUNTER MV_GetRealTickCount();
    PERF_COUNTER MV_GetTimeFromTick(PERF_COUNTER tick);
#endif

    //File Operation Functions
    MV_FILE* MV_OpenFile(CONST CHAR *szName, CONST CHAR *szMode);
    VOID MV_CloseFile(MV_FILE *pFile);
    UINT MV_ReadFile(VOID *pBuffer, UINT iSize, MV_FILE *pFile);
    CHAR* MV_GetLineOfFile(CHAR *pBuffer, UINT iSize, MV_FILE *pFile);
    UINT MV_WriteFile(VOID *pBuffer, UINT iSize, MV_FILE *pFile);

#ifdef __cplusplus
}
#endif


#endif // CXX_PCVMISC_H
/* EOF */
