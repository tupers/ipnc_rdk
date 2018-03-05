/*
* @(#)$Id: MathLib.h 1085 2011-08-08 03:24:21Z LvGangXiao $ 
* @(#) Declaration file of math lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/

#ifndef CXX_MATHLIB_FLOAT_H
#define CXX_MATHLIB_FLOAT_H

#include "pcvPlatformdef.h"
#include "pcvBasicStruct.h"

#ifdef __cplusplus
extern "C" {
#endif


//Basic math
FLOAT MV_Abs_F(FLOAT value);
FLOAT MV_Mul_F(FLOAT a, FLOAT b);
FLOAT MV_Div_F(FLOAT a, FLOAT b);
FLOAT MV_Sin_F(FLOAT value);
FLOAT MV_Cos_F(FLOAT value);
FLOAT MV_Tan_F(FLOAT value);
FLOAT MV_Atan_F(FLOAT value);
FLOAT MV_Pow_F(FLOAT value, FLOAT exp);
FLOAT MV_Sqrt_F(FLOAT value);
FLOAT MV_Exp_F(FLOAT value);



//Basic matrix operation
BOOL MV_InitializeMatrixData_F(LPMV_MATRIX_F pMatrix, INT iRows, INT iCols);
LPMV_MATRIX_F MV_CreateMatrices_F(INT iRows, INT iCols, INT iNumber);
VOID MV_FreeMatrices_F(LPMV_MATRIX_F pMatrices, INT iNumber);
FLOAT MV_MatrixGet_F(CONST LPMV_MATRIX_F pMatrix, INT iRow, INT iCol);
VOID MV_MatrixSet_F(LPMV_MATRIX_F pMatrix, INT iRow, INT iCol, FLOAT fxData);
LPMV_MATRIX_F MV_CreateIdentityMatrix_F(INT iRows);
VOID MV_PrintMat_F(LPMV_MATRIX_F pMat);
DWORD MV_MatrixGetSize_F(LPMV_MATRIX_F pMatrix);
LPMV_MATRIX_F MV_MatrixCopy_F(LPMV_MATRIX_F pSrc);
VOID MV_MatrixCopy2_F(LPMV_MATRIX_F pSrc, LPMV_MATRIX_F pDst);
VOID MV_MatrixSetAll_F(LPMV_MATRIX_F pMatrix, FLOAT fxData);


//Basic matrix function
LPMV_MATRIX_F MV_MatrixAddSub_F(CONST LPMV_MATRIX_F pFirst, CONST LPMV_MATRIX_F pSecond, BOOL bAdd);
LPMV_MATRIX_F MV_MatrixMultiply_F(CONST LPMV_MATRIX_F pFirst, CONST LPMV_MATRIX_F pSecond);
LPMV_MATRIX_F MV_MatrixDotMultiply_F(CONST LPMV_MATRIX_F pFirst, CONST FLOAT fxValue);
LPMV_MATRIX_F MV_MatrixTranspose_F(CONST LPMV_MATRIX_F pMatrix);
LPMV_MATRIX_F MV_MatrixInverse_F(CONST LPMV_MATRIX_F pMatrix);


#ifdef __cplusplus
}
#endif

#endif /* CXX_MATHLIB_FLOAT_H */
