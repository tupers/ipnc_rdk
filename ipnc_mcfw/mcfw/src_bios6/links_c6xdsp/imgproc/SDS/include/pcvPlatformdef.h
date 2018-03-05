/*
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file   
 * Declaration of pcvPlatform_def
 * 
 * TO DESC : FILE DETAIL DESCRIPTION, BUT DON'T DESCRIBE CLASS DETAIL HERE.
 */
#ifndef CXX_PCVPLATFORM_DEF_H
#define CXX_PCVPLATFORM_DEF_H

#ifdef WIN32
#pragma warning(error: 4013)
#pragma warning(error: 4701)
#pragma warning(error: 4189) 
#pragma warning(error: 4020)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#ifdef WIN32
#include "windows.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define FILE_NAME   __FILE__
#define FUNCTION_NAME  __FUNCTION__
#define LINE_NUM  __LINE__
#elif defined(C64X)
#include <std.h>
#include <stdio.h>
#include <stdlib.h>
#include <log.h>
#include <sys.h>
#include <time.h>
#include <utl.h>
#include <clk.h>
#include <gbl.h>
#elif defined(TRON)
#include "OS_TimeManager.h"
#elif defined(ANDROID)
#include <android/log.h>
#define FILE_NAME   __FILE__
#define FUNCTION_NAME  __FUNCTION__
#define LINE_NUM  __LINE__
#elif defined (_FOR_IPHONE_)
#include <TargetConditionals.h>
#ifndef CXX_AIL_NEWTYPESDEFINE_H
#include "AIL_NewTypesDefine.h"
#define FILE_NAME   __FILE__
#define FUNCTION_NAME  __FUNCTION__
#define LINE_NUM  __LINE__
#endif
#endif

#ifdef C64X
#define MV_USE_DSPBIOS
#define MV_USE_PROFLE
#endif

//#define MV_USE_FP
//#define MV_USE_IQMATH
#ifdef _FOR_IPHONE_
    #if TARGET_IPHONE_SIMULATOR == 0
    #define MV_USE_NEON
    #endif
#else
    #ifdef __HAS_NEON__
    #define MV_USE_NEON
    #endif
#endif

#define ALGORITHM_WIN32_DEBUG 
#define USE_3GP_FILE_SPEED
//#define UI_CONTROL_DRAW 
//#define IR_MVCTL_ANALYZE_LOG

#ifndef CXX_AIL_NEWTYPESDEFINE_H
typedef int INT;
typedef long LONG;
typedef long long LONGLONG;
typedef float FLOAT;
typedef short SHORT;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef double DOUBLE;
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#endif

typedef LONG HRESULT;

typedef LONGLONG MV_64;
typedef INT MV_32;
typedef SHORT MV_16;//WXF 2016-10-18
typedef BYTE MV_8;
typedef MV_8 MV_FIXED_S;
typedef MV_8 MV_FIXED_B;


//32 BIT
typedef MV_32 MV_FIXED;
#define MV_PRECISION 15
#define MV_HALF (1 << (MV_PRECISION - 1))
#define MV_ONE (1 << MV_PRECISION)
#define MV_TWO ((1 << (MV_PRECISION + 1)))
//#define MV_FLOAT_HALF (MV_64)(1 << (2 * MV_PRECISION)) * 5 / 100000
#define MV_FLOAT_HALF (1 << (MV_PRECISION - 1))
#define MV_PI (MV_FIXED)(3.1415926535897932f * MV_ONE)

//64 BIT
typedef MV_64 MV_FIXED_L;
#define MV_PRECISION_L 20
#define MV_HALF_L ((MV_64)1 << (MV_PRECISION_L - 1))
#define MV_FLOAT_HALF_L ((MV_64)1 << (2 * MV_PRECISION_L)) * 5 / 10000000
#define MV_ONE_L ((MV_64)1 << MV_PRECISION_L)
#define MV_TWO_L ((MV_64)1 << (MV_PRECISION_L + 1))
#define MV_PI_L (MV_FIXED_L)(3.1415926535897932f * MV_ONE_L)

//FLOAT
#define MV_PI_F	3.1415926535897932f
#define MV_ONE_F 1.0f
#define MV_TWO_F 2.0f

#define NUMANGLE 180

#define SETBIT(data, bit) ((data) |= (1 << (bit)))
#define CLEARBIT(data, bit) ((data) &= ~(1 << (bit)))
#define TESTBIT(data, bit) ((data) & (1 << (bit)))


#undef VOID
#define VOID void
#undef NULL
#define NULL 0
#undef CONST
#define CONST const
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

#ifndef _MAX_PATH
#define _MAX_PATH 255
#endif

#ifndef INT_MIN
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
#endif

#ifndef INT_MAX
#define INT_MAX       2147483647    /* maximum (signed) int value */
#endif

#ifndef UINT_MAX
#define UINT_MAX      0xffffffff    /* maximum unsigned int value */11:57
#endif

#define MV_SHORTMAX ((USHORT)-1 >> 1)
#define MV_SHORTMIN (SHORT)(MV_SHORTMAX + 1)
#define MV_INTMAX ((UINT)-1 >> 1)
#define MV_INTMIN (INT)(MV_INTMAX + 1)

#ifdef WIN32
#define ADAS_INLINE __inline
#elif defined(C64X)
#define ADAS_INLINE inline
#elif defined(ANDROID)
#define ADAS_INLINE static inline __attribute__((__gnu_inline__))
#elif defined(_FOR_IPHONE_)
#define ADAS_INLINE static
#else
#endif

#define W2DW(lo, hi) ((DWORD)(lo) | (hi) << 16)
#define MV_MIN(a,b) ((a) > (b) ? (b) : (a))
#define MV_MAX(a,b) ((a) < (b) ? (b) : (a))
#define MV_SWAP(a,b,t) ((t) = (a), (a) = (b), (b) = (t))

#define PCV_PI    ((FLOAT)  3.141592654f)

#define PCVToRadian( degree ) ((degree) * (PCV_PI / 180.0f))
#define PCVToDegree( radian ) ((radian) * (180.0f / PCV_PI))



#endif // CXX_PCVPLATFORM_DEF_H
/* EOF */
