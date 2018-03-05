/*
* @(#)$Id: MathLib.h 1085 2011-08-08 03:24:21Z LvGangXiao $ 
* @(#) Declaration file of math lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/

#ifndef CXX_MATHLIB32_H
#define CXX_MATHLIB32_H
//#include <xdc/std.h>
#include "pcvPlatformdef.h"
#include "pcvBasicStruct.h"


#ifdef __cplusplus
extern "C" {
#endif

VOID MV_InitMathLib32();
MV_FIXED MV_FixedFromFloat(FLOAT value);
FLOAT MV_FloatFromFixed(MV_FIXED value);

MV_FIXED MV_FixedFromInt(INT value);

INT MV_IntFromFixed(MV_FIXED value);

INT MV_IntFromFixed_Floor(MV_FIXED value);
MV_FIXED MV_FixScaleDown(MV_64 value);
MV_64 MV_FixScaleUp(MV_FIXED value);
MV_FIXED MV_Floor(MV_FIXED value);
MV_FIXED MV_Ceil(MV_FIXED value);
MV_FIXED MV_Abs(MV_FIXED value);
MV_FIXED MV_Mul(MV_FIXED a, MV_FIXED b);
MV_64 MV_Mul64(MV_FIXED a, MV_FIXED b);
MV_64 MV_Div64(MV_FIXED a, MV_FIXED b);
MV_FIXED MV_Div(MV_FIXED a, MV_FIXED b);
MV_FIXED MV_Sin(MV_FIXED value);
MV_FIXED MV_Cos(MV_FIXED value);
MV_FIXED MV_Tan(MV_FIXED value);
MV_FIXED MV_Atan(MV_FIXED value);
MV_FIXED MV_Pow(MV_FIXED value, MV_FIXED exp);
MV_FIXED MV_Sqrt(MV_FIXED value);
MV_FIXED MV_ISqrt(MV_FIXED value);
MV_FIXED MV_Mag(MV_FIXED a, MV_FIXED b);
MV_FIXED MV_Exp(MV_FIXED value);


LPMV_MATRIX MV_CreateIdentityMatrix(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign);
LPMV_MATRIX MV_CreateIdentityMatrixB(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem);
LPMV_MATRIX MV_CreateMatrices(INT iRows, INT iCols, INT iNumber, INT iStartAddrAlign, INT iLineAddrAlign);
LPMV_MATRIX MV_CreateMatrixB(INT iRows, INT iCols, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem);
VOID MV_FreeMatrices(LPMV_MATRIX pMatrices, INT iNumber);
DWORD MV_MatrixGetSize(LPMV_MATRIX pMatrix);
LPMV_MATRIX MV_MatrixCopy(LPMV_MATRIX pSrc);
VOID MV_MatrixCopy2(LPMV_MATRIX pSrc, LPMV_MATRIX pDst);
MV_FIXED MV_MatrixGet(CONST LPMV_MATRIX pMatrix, INT iRow, INT iCol);
MV_FIXED* MV_MatrixGetLinePtr(LPMV_MATRIX pMatrix, INT iRow);
VOID MV_MatrixSet(LPMV_MATRIX pMatrix, INT iRow, INT iCol, MV_FIXED fxData);
VOID MV_MatrixToIntAll(LPMV_MATRIX pMatrix);
VOID MV_MatrixToFixedAll(LPMV_MATRIX pMatrix);
VOID MV_MatrixSetAll(LPMV_MATRIX pMatrix, MV_FIXED fxData);
LPMV_MATRIX MV_MatrixAddSub(CONST LPMV_MATRIX pFirst, CONST LPMV_MATRIX pSecond, BOOL bAdd);
LPMV_MATRIX MV_MatrixMultiply(CONST LPMV_MATRIX pFirst, CONST LPMV_MATRIX pSecond);
LPMV_MATRIX MV_MatrixDotMultiply(CONST LPMV_MATRIX pFirst, CONST MV_FIXED fxValue);
LPMV_MATRIX MV_MatrixTranspose(CONST LPMV_MATRIX pMatrix);
LPMV_MATRIX MV_MatrixInverse(CONST LPMV_MATRIX pMatrix);
VOID MV_PrintMat(LPMV_MATRIX pMat);


LPMV_MATRIX_S MV_CreateIdentityMatrix_S(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign);
LPMV_MATRIX_S MV_CreateIdentityMatrixB_S(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem);
LPMV_MATRIX_S MV_CreateMatrices_S(INT iRows, INT iCols, INT iNumber, INT iStartAddrAlign, INT iLineAddrAlign);
LPMV_MATRIX_S MV_CreateMatrixB_S(INT iRows, INT iCols, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem);
VOID MV_FreeMatrices_S(LPMV_MATRIX_S pMatrices, INT iNumber);
DWORD MV_MatrixGetSize_S(LPMV_MATRIX_S pMatrix);
LPMV_MATRIX_S MV_MatrixCopy_S(LPMV_MATRIX_S pSrc);
VOID MV_MatrixCopy2_S(LPMV_MATRIX_S pSrc, LPMV_MATRIX_S pDst);
MV_FIXED_S MV_MatrixGet_S(CONST LPMV_MATRIX_S pMatrix, INT iRow, INT iCol);
//MV_FIXED_S* MV_MatrixGetLinePtr_S(LPMV_MATRIX_S pMatrix, INT iRow, INT iCol);
inline MV_FIXED_S* MV_MatrixGetLinePtr_S(LPMV_MATRIX_S pMatrix, INT iRow, INT iCol)
{

	if (NULL == pMatrix) {
		return NULL;
	}

	return ((MV_FIXED_S*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol);

}
VOID MV_MatrixSet_S(LPMV_MATRIX_S pMatrix, INT iRow, INT iCol, MV_FIXED_S fxData);
VOID MV_MatrixToIntAll_S(LPMV_MATRIX_S pMatrix);
VOID MV_MatrixToFixedAll_S(LPMV_MATRIX_S pMatrix);
VOID MV_MatrixSetAll_S(LPMV_MATRIX_S pMatrix, MV_FIXED_S fxData);
LPMV_MATRIX_S MV_MatrixAddSub_S(CONST LPMV_MATRIX_S pFirst, CONST LPMV_MATRIX_S pSecond, BOOL bAdd);
LPMV_MATRIX_S MV_MatrixMultiply_S(CONST LPMV_MATRIX_S pFirst, CONST LPMV_MATRIX_S pSecond);
LPMV_MATRIX_S MV_MatrixDotMultiply_S(CONST LPMV_MATRIX_S pFirst, CONST MV_FIXED_S fxValue);
LPMV_MATRIX_S MV_MatrixTranspose_S(CONST LPMV_MATRIX_S pMatrix);
LPMV_MATRIX_S MV_MatrixInverse_S(CONST LPMV_MATRIX_S pMatrix);
VOID MV_PrintMat_S(LPMV_MATRIX_S pMat);

BOOL MV_InitializeMatrixData_B(LPMV_MATRIX_B pMatrix, INT iRows, INT iCols);
BOOL MV_InitializeMatrixDataB_B(LPMV_MATRIX_B pMatrix, INT iRows, INT iCols, BYTE *pMem);
LPMV_MATRIX_B MV_CreateMatrices_B(INT iRows, INT iCols, INT iNumber, INT iStartAddrAlign, INT iLineAddrAlign);
LPMV_MATRIX_B MV_CreateMatrixB_B(INT iRows, INT iCols, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem);
VOID MV_FreeMatrices_B(LPMV_MATRIX_B pMatrices, INT iNumber);
MV_FIXED_B MV_MatrixGet_B(CONST LPMV_MATRIX_B pMatrix, INT iRow, INT iCol);
VOID MV_MatrixSet_B(LPMV_MATRIX_B pMatrix, INT iRow, INT iCol, MV_FIXED_B fxData);
MV_FIXED_B* MV_MatrixGetLinePtr_B(LPMV_MATRIX_B pMatrix, INT iRow);

/* 
 * convolution precision limit: src: short, mask: fixed, dst: short
 * dsp only support short type, so keep in mind to input data within short type
 * output are also short type, so in big kernel convolution(with accumulation) there is precision loss
 */
VOID MV_Convolution(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift);
VOID MV_Smooth(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift, INT iDiv);
VOID MV_ConvolutionB(MV_MATRIX_S *pMatSrc, SHORT iX, SHORT iY, SHORT iWidth, SHORT iHeight, 
					 MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift);
VOID MV_SmoothB(MV_MATRIX_S *pMatSrc, SHORT iX, SHORT iY, SHORT iWidth, SHORT iHeight, 
				MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift, INT iDiv);
VOID MV_HoughSortDescent32s(INT *array, INT total, const INT *aux);

BOOL MV_HoughLinesStandardB(LPMV_MATRIX_S piCovResResult, CONST HOUGH_REGION *pRoi, MV_FIXED rho, 
							MV_FIXED fxTheta, INT threshold, INT iLineMax, LPMV_MATRIX pfHoughTransResults, 
							INT *pNum, VOID *pWorkBuffer, INT iBuffSize,
							INT iValidAngleSize, MV_FIXED fMinAngle);

BOOL MV_HoughLinesStandardC(INT* piCoordianteData, INT iDataLength, CONST HOUGH_REGION *pRoi, MV_FIXED fxrho, 
							MV_FIXED fxTheta, INT threshold, INT iLineMax, LPMV_MATRIX pMatRes, 
							INT *pNum, VOID *pWorkBuffer, INT iBuffSize, 
							INT iValidAngleSize, MV_FIXED fMinAngle);

VOID MV_ComputeAccumGraph(LPMV_MATRIX_S pMatSrc, INT* piarrDst, INT iOffX, INT iOffY, INT iWidth, INT iHeight);
VOID MV_MAD_16X16(MV_MATRIX_B *pMatSrc, MV_MATRIX_B *pMatMask, INT iX, INT iY, INT iWidth, INT iHeight, UINT *piMatch);

#ifdef __cplusplus
}
#endif

#endif /* CXX_MATHLIB32_H */
