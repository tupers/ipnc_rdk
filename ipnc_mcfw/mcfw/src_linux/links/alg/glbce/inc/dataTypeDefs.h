/*=======================================================================
 *
 *            Texas Instruments Internal Reference Software
 *
 *                 Systems and Applications R&D Center
 *                    Video and Image Processing Lab
 *                           Imaging Branch         
 *
 *         Copyright (c) 2010 Texas Instruments, Incorporated.
 *                        All Rights Reserved.
 *      
 *
 *          FOR TI INTERNAL USE ONLY. NOT TO BE REDISTRIBUTED.
 *
 *							TI CONFIDENTIAL
 *
 *======================================================================*/



#ifndef _DATATYPE_DEFS_INCLUDED_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
 
#define _DATATYPE_DEFS_INCLUDED_

#define NO_PRINTF

#define RNDOFST(bits)      (1 << ((bits) - 1))
#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))


#ifndef MIN
//#define MIN(x, y)          ((x) < (y) ? (x) : (y))
 #define MIN(a, b)   ((a) < (b) ? (a) : (b))

#endif
#ifndef MAX
#define MAX(x, y)          ((x) > (y) ? (x) : (y))
#endif

typedef unsigned char  Byte;
typedef signed char    sByte;
typedef short          Word16;
typedef unsigned short uWord16;
typedef signed short   sWord16;
typedef int            Word32;
typedef unsigned int   uWord32;
typedef signed int     sWord32;
//typedef long           Word32;

typedef enum {YUV444PLANAR,YUV422PLANAR,YUV422INTERLIEVED,BMP,BAYER} imgFormatType;
typedef enum {Y,U,V,R,Gr,Gb,B} pixComponentType;
typedef enum {LITTLEENDIAN,BIGENDIAN} endianType;
typedef enum {BINARYFILE,SIMULATORFILE,ISPFILE} gainFileFormatType;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
