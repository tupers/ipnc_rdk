/*
 * @(#)$Id: pcvColorConversion.h 2237 2012-06-05 01:27:05Z LvGangXiao $ 
 * @(#) Declaration file of class pcvColorConversion
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file   
 * Declaration of pcvColorConversion
 * 
 * TO DESC : FILE DETAIL DESCRIPTION, BUT DON'T DESCRIBE CLASS DETAIL HERE.
 */
#ifndef CXX_PCVCOLORCONVERSION_H
#define CXX_PCVCOLORCONVERSION_H


#ifndef CXX_PCVPLATFORM_DEF_H
#include "pcvPlatformdef.h"
#endif

#ifndef CXX_PCVBASICSTRUCT_H
#include "pcvBasicStruct.h"
#endif


//#ifndef CXX_PCVMATHOPE_H
//#include "pcvMathOpe.h"
//#endif

typedef enum _MV_CVT_COLOR_TYPE
{
    PCV_COLOR_YUV2HSV,    
    PCV_COLOR_CVTUNKNOWN,    
} pcv_E_CvtColorType;

typedef enum _MV_COLOR_FILTER_TYPE
{
    PCV_COLOR_FILTER_AND,
    PCV_COLOR_FILTER_OR,
    PCV_COLOR_FILTER_UNKNOWN
} pcv_E_ColorFilterType;

typedef struct _pcvHSVColorRange
{
    INT iHMin;
    INT iHMax;
    INT iSMin;
    INT iSMax;
    pcv_E_ColorFilterType  eHFilter;
    pcv_E_ColorFilterType  eSFilter;
} pcvHSVColorRange;

#ifdef __cplusplus
extern "C" {
#endif

    BOOL pcvCvtYUV2HSV(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV, 
              LPMV_MATRIX_S psH, LPMV_MATRIX_S psS,  LPMV_MATRIX_S psV);

    BOOL pcvCvtYUV2HSVAndCompOp(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV, 
        pcvHSVColorRange psColorRange, LPMV_MATRIX_S psFlag);

	BOOL pcvCvtYUV2HSVAndCompOpEx(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV, 
		pcvHSVColorRange* psColorRange, INT iColorCount,
		pcvRect* psFrameRect, LPMV_MATRIX_S psFlag);

	BOOL pcvCvtYUV2HSV_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV, 
		LPMV_MATRIX_S psH, LPMV_MATRIX_S psS,  LPMV_MATRIX_S psV);

	BOOL pcvCvtYUV2HSVAndCompOp_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV, 
		pcvHSVColorRange sColorRange, LPMV_MATRIX_S psFlag);

	BOOL pcvCvtYUV2HSVAndCompOpEx_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV, 
		pcvHSVColorRange* psColorRange, INT iColorCount,
		pcvRect* psFrameRect, LPMV_MATRIX_S psFlag);


#ifdef WIN32
    void  pcvRGB2YUV(BYTE r, BYTE g, BYTE b, BYTE *lpY, BYTE *lpU, BYTE *lpV);
#endif

#ifdef __cplusplus
}
#endif


#endif // CXX_PCVCOLORCONVERSION_H
/* EOF */
