
#ifndef CXX_PCVMISC_H
#include "pcvMisc.h"
#endif


#include <math.h>
#ifdef MV_USE_IQMATH
#include "IQmath.h"
#endif

#ifdef C64X
#include "IMG_conv_3x3_i16s_c16s.h"
#include "IMG_conv_5x5_i16s_c16s.h"
#include "IMG_conv_7x7_i16s_c16s.h"
#include "IMG_conv_11x11_i16s_c16s.h"
#include "IMG_mad_16x16.h"
#elif defined(MV_USE_NEON)
#include <arm_neon.h>
#else
#endif


UINT g_iMemSize = 0;

MV_FIXED Sat16(MV_FIXED iNum, INT iShift)
{
#ifndef MV_USE_FP
	iNum >>= iShift;
	iNum = (iNum > MV_SHORTMAX) ? MV_SHORTMAX : (iNum < MV_SHORTMIN ? MV_SHORTMIN : iNum);
	return iNum;
#endif
}


VOID MV_Memcpy(VOID *pDst, CONST VOID *pSrc, UINT iLen)
{
	if ((NULL != pDst) && (NULL != pSrc)) {
		CHAR *pd = (CHAR*)pDst;
		CONST CHAR *ps = (CONST CHAR*)pSrc;

		while (iLen-- > 0)
		{
			*pd++ = *ps++;
		}
	}
}

VOID MV_Memset(VOID *pDst, INT iValue, UINT iCount)
{
	if (NULL != pDst) {
		CHAR *pd = (CHAR*)pDst;

		while (iCount-- > 0)
		{
			*pd++ = iValue;
		}
	}
}

VOID* MV_Malloc(UINT size)
{
    BYTE *pBlock = (BYTE*)malloc(size);
    if (pBlock != NULL)
    {
        g_iMemSize += size;
        MV_Memset(pBlock, 0, size);
        return pBlock;
    }

    return NULL;
}

VOID* MV_MallocAlign(UINT size, UINT alignment, VOID **pBlockAlign)
{
    BYTE *pBlock = (BYTE*)MV_Malloc(size + alignment);
    if (pBlock != NULL)
    {
        g_iMemSize += (size + alignment);
        *pBlockAlign = (VOID*)MV_ALIGNADDR(pBlock, alignment);
        return pBlock;
    }

    return NULL;
}

VOID MV_Free(VOID *pBlock)
{
    free(pBlock);
}



VOID MV_Printf1(CONST CHAR *pString)
{
#ifdef WIN32
    OutputDebugStringA(pString);
#elif defined(C64X)
    printf("%s", pString);
#elif defined(TRON)
    printf("%s", pString);
#elif defined(ANDROID)
    __android_log_print(ANDROID_LOG_INFO, "MYPROG", "%s", pString);
#elif defined (_FOR_IPHONE_)
    printf("%s", pString);
#else
#endif
}

VOID MV_Printf(CONST CHAR *format, ...)
{
    static CHAR buffer[PRNT_BUFF_SIZE];
    va_list ap;
    va_start(ap, format);
 //   vsnprintf(buffer, PRNT_BUFF_SIZE, format, ap);
    va_end(ap);

    MV_Printf1(buffer);
}

#ifdef WIN32

PERF_COUNTER MV_GetTickCount()
{
#ifdef WIN32
    LARGE_INTEGER lTick;
    QueryPerformanceCounter(&lTick);
    return lTick.QuadPart;
#elif defined(C64X)
    return CLK_gethtime();
#elif defined(TRON)
    DWORD upper, lower;
    OS_TimeManager::GetTickTime(&upper, &lower);
    return MAKE_UDLONG(upper, lower);
#elif defined(ANDROID)
    struct timespec request;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &request);
    return 1000.0 * request.tv_sec + (DOUBLE)request.tv_nsec / 1e6;
#else
    return GetTickCount();
#endif
    return 0;
}

PERF_COUNTER MV_GetRealTickCount()
{
#ifdef WIN32
	LARGE_INTEGER lTick;
	QueryPerformanceCounter(&lTick);
	return lTick.QuadPart;
#elif defined(C64X)
	return CLK_gethtime();
#elif defined(TRON)
	DWORD upper, lower;
	OS_TimeManager::GetTickTime(&upper, &lower);
	return MAKE_UDLONG(upper, lower);
#elif defined(ANDROID)
	struct timespec request;
	clock_gettime(CLOCK_REALTIME, &request);
	return 1000.0 * request.tv_sec + (DOUBLE)request.tv_nsec / 1e6;
#else
    return GetTickCount();
#endif
    
    return 0;
}

PERF_COUNTER MV_GetTimeFromTick(PERF_COUNTER tick)
{
#ifdef WIN32
    LARGE_INTEGER lFreq;
    QueryPerformanceFrequency(&lFreq);
    return (tick / (double)lFreq.QuadPart  * 1000 );
#elif defined(C64X)
    return (tick * CLK_cpuCyclesPerHtime()) / GBL_getFrequency();
#elif defined(TRON)
    return tick;
#elif defined(ANDROID)
    return tick;
#else
    return tick;
#endif
    
    return 0;	
}

#endif

MV_FILE* MV_OpenFile(CONST CHAR *szName, CONST CHAR *szMode)
{
    FILE *pf = fopen(szName, szMode);
    return pf;
}

VOID MV_CloseFile(MV_FILE *pFile)
{
    fclose(pFile);
}

UINT MV_ReadFile(VOID *pBuffer, UINT iSize, MV_FILE *pFile)
{
    return fread(pBuffer, iSize, 1, pFile);
}

CHAR* MV_GetLineOfFile(CHAR *pBuffer, UINT iSize, MV_FILE *pFile)
{
    return fgets(pBuffer, iSize, pFile);
}

UINT MV_WriteFile(VOID *pBuffer, UINT iSize, MV_FILE *pFile)
{
    return fwrite(pBuffer, iSize, 1, pFile);
}


/* EOF */
