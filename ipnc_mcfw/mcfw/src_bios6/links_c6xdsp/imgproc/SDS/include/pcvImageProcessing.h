/*
 * @(#)$Id: pcvImageProcessing.h 1942 2015-11-25 04:40:01Z LvGangXiao $ 
 * @(#) Declaration file of class pcvImageProcessing
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file   
 * Declaration of pcvImageProcessing
 * 
 * TO DESC : FILE DETAIL DESCRIPTION, BUT DON'T DESCRIBE CLASS DETAIL HERE.
 */
#ifndef CXX_PCVIMAGEPROCESSING_H
#define CXX_PCVIMAGEPROCESSING_H

#include "pcvBasicStruct.h"

enum PCV_DIRECTION{
	PCV_DIRT_X,
	PCV_DIRT_Y,
};

enum PCV_SOBEL_TYPE{
	PCV_SOBEL_3X3,
};

#ifdef __cplusplus
extern "C" {
#endif

BOOL pcvCreateImage(pcvImage *pImage, INT iWidth, INT iHeight);
BOOL pcvDestroyImage(pcvImage *pImage);
BOOL pcvSetDataByImage(pcvImage *pSrc, pcvImage *pDest, pcvRect *pROI);
BOOL pcvSetDataByData(BYTE *pSrc, INT iWidth, INT iHeight, enum PCVIMAGE_FORMAT eFormat, pcvImage *pDest);
LPMV_MATRIX_S pcvGetImageMatrix(pcvImage *pImage, enum PCVIMAGE_TYPE_SUB eSubType);
BOOL pcvMakeSubMatrix(pcvImage *pImage, enum PCVIMAGE_TYPE_SUB eSubType);


BOOL pcvIntergral(LPMV_MATRIX_S pSrc, LPMV_MATRIX pDest);
BOOL pcvIntergral_NOFUNC(LPMV_MATRIX_S pSrc, LPMV_MATRIX pDest);
BOOL pcvIntergral_NEON(LPMV_MATRIX_S pSrc, LPMV_MATRIX pDest);

BOOL pcvIntergral_ROI(LPMV_MATRIX_S pSrc, pcvRect* rROI, LPMV_MATRIX pDest);
BOOL pcvIntergral_ROI_NOFUNC(LPMV_MATRIX_S pSrc, pcvRect* rROI, LPMV_MATRIX pDest);
BOOL pcvIntergral_ROI_NEON(LPMV_MATRIX_S pSrc, pcvRect* rROI, LPMV_MATRIX pDest);

BOOL pcvLineIntergral(LPMV_MATRIX pSrc, INT iStep, enum PCV_DIRECTION eDirect, LPMV_MATRIX pDest);
BOOL pcvLineIntergral_NOFUNC(LPMV_MATRIX pSrc, INT iStep, enum PCV_DIRECTION eDirect, LPMV_MATRIX pDest);
BOOL pcvLineIntergral_NEON(LPMV_MATRIX pSrc, INT iStep, enum PCV_DIRECTION eDirect, LPMV_MATRIX pDest);

BOOL pcvSobel(LPMV_MATRIX_S pSrc, LPMV_MATRIX_S pDest, enum PCV_DIRECTION eDirect, enum PCV_SOBEL_TYPE eType);


BOOL pcvHistogram_S(LPMV_MATRIX_S pSrc, MV_FIXED_S sMin, MV_FIXED_S sMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest);
BOOL pcvHistogram_S_NOFUNC(LPMV_MATRIX_S pSrc, MV_FIXED_S sMin, MV_FIXED_S sMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest);

BOOL pcvHistogram(LPMV_MATRIX pSrc, MV_FIXED iMin, MV_FIXED iMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest);
BOOL pcvHistogram_NOFUNC(LPMV_MATRIX pSrc, MV_FIXED iMin, MV_FIXED iMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest);

BOOL pcvLineFitting(INT iPointNum, 	pcvPoint *pPoint, MV_FIXED* pfRatioInverse,	INT* piDist);

BOOL pcvThreshold(LPMV_MATRIX pHistogram, MV_FIXED coefficient, INT *piThreshold);//¶þÖµ»¯

BOOL pcvHoughTransform(INT* piCoordianteData, INT iDataLength, CONST HOUGH_REGION *pRoi, 
					   MV_FIXED fxrho, MV_FIXED fxTheta, INT threshold, INT iLineMax, LPMV_MATRIX pMatRes,
					   INT *pNum, VOID *pWorkBuffer, INT iBuffSize,
					   INT iValidAngleSize, MV_FIXED fMinAngle);

BOOL pcvDisTransform(INT* piDistanceGraph,
					 INT iDistanceGraphDataNum,
					 LPMV_MATRIX_S psEdgeDetResult,
					 pcvDTParam* pDTParam);



#ifdef __cplusplus
}
#endif

#endif // CXX_PCVIMAGEPROCESSING_H
/* EOF */
