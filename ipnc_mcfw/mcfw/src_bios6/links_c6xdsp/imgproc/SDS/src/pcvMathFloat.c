/*
* @(#)$Id: MathLibFloat.cpp 1007 2011-05-25 03:13:32Z LvGangXiao $ 
* @(#) Declaration file of math lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/

#include "pcvMathFloat.h"

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

#ifndef CXX_PCVMISC_H
#	include "pcvMisc.h"
#endif

//Basic math
FLOAT MV_Abs_F(FLOAT value)
{

	return (value < 0.0) ? -value : value;
}

FLOAT MV_Mul_F(FLOAT a, FLOAT b)
{
	return a * b;
}

FLOAT MV_Div_F(FLOAT a, FLOAT b)
{
	if (0 == b){
		return 0;
	}

	return a / b;
}

FLOAT MV_Sin_F(FLOAT value)
{
	return (FLOAT)sin(value);
}

FLOAT MV_Cos_F(FLOAT value)
{
	return (FLOAT)cos(value);
}

FLOAT MV_Tan_F(FLOAT value)
{
	return (FLOAT)tan(value);
}

FLOAT MV_Atan_F(FLOAT value)
{
	return (FLOAT)atan(value);
}

FLOAT MV_Pow_F(FLOAT value, FLOAT exp)
{
	return (FLOAT)pow(value, exp);
}

FLOAT MV_Sqrt_F(FLOAT value)
{
	return (FLOAT)sqrt(value);
}

FLOAT MV_Exp_F(FLOAT value)
{
	return (FLOAT)exp(value);
}

BOOL MV_InitializeMatrixData_F(LPMV_MATRIX_F pMatrix, INT iRows, INT iCols)
{
	pMatrix->iRows = iRows;
	pMatrix->iCols = iCols;
	pMatrix->iPitch = pMatrix->iCols * sizeof(FLOAT);
	pMatrix->iLength = pMatrix->iPitch * pMatrix->iRows;
	pMatrix->pData = (BYTE*)MV_Malloc(pMatrix->iLength);
	pMatrix->pDataAlign = (FLOAT*)pMatrix->pData;
	if (pMatrix->pData != NULL)
	{
		return TRUE;
	}

	return FALSE;
}

LPMV_MATRIX_F MV_CreateMatrices_F(INT iRows, INT iCols, INT iNumber)
{
	INT i = 0;
	LPMV_MATRIX_F pMatrices = (LPMV_MATRIX_F)MV_Malloc(iNumber * sizeof(MV_MATRIX_F));
	if (pMatrices == NULL)
	{
		return NULL;
	}

	for (i = 0; i < iNumber; i++)
	{
		if (!MV_InitializeMatrixData_F(&pMatrices[i], iRows, (iCols == 0 ? iRows : iCols)))
		{
			return NULL;
		}
	}

	return pMatrices;
}

VOID MV_FreeMatrices_F(LPMV_MATRIX_F pMatrices, INT iNumber)
{
	INT i = 0;

	if (NULL != pMatrices) {
		for (i = 0; i < iNumber; i++)
		{
			if (pMatrices[i].pData != NULL)
			{
				MV_Free(pMatrices[i].pData);
				pMatrices[i].pData = NULL;
			}
		}

		MV_Free(pMatrices);
	}
}

FLOAT MV_MatrixGet_F(CONST LPMV_MATRIX_F pMatrix, INT iRow, INT iCol)
{
	if (NULL == pMatrix) {
		return 0.f;
	}
	return *((FLOAT*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol);
}

VOID MV_MatrixSet_F(LPMV_MATRIX_F pMatrix, INT iRow, INT iCol, FLOAT fxData)
{
	if (NULL != pMatrix) {
		*((FLOAT*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol) = fxData;
	}
}

LPMV_MATRIX_F MV_CreateIdentityMatrix_F(INT iRows)
{
	INT i = 0;
	LPMV_MATRIX_F pMatrix = NULL;

	pMatrix = (LPMV_MATRIX_F)MV_CreateMatrices_F(iRows, iRows, 1);
	if (pMatrix == NULL)
	{
		return NULL;
	}

	for (i = 0; i < iRows; i++)
	{
		MV_MatrixSet_F(pMatrix, i, i, (FLOAT)MV_ONE_F);
	}

	return pMatrix;	
}

VOID MV_PrintMat_F(LPMV_MATRIX_F pMat)
{
	INT i = 0, j = 0;

	if (NULL != pMat) {
		for (i = 0; i < pMat->iRows; i++)
		{
			for (j = 0; j < pMat->iCols; j++)
			{
				MV_Printf("%f\t", MV_MatrixGet_F(pMat, i, j));
			}
			MV_Printf("\n");
		}
	}
}

DWORD MV_MatrixGetSize_F(LPMV_MATRIX_F pMatrix)
{
	INT iSize = 0;
	if (NULL != pMatrix) {
		iSize = sizeof(MV_MATRIX_F) + pMatrix->iLength;
	}
	return iSize;
}

LPMV_MATRIX_F MV_MatrixCopy_F(LPMV_MATRIX_F pSrc)
{
	LPMV_MATRIX_F pMat = NULL;

	if (NULL != pSrc) {
		pMat = MV_CreateMatrices_F(pSrc->iRows, pSrc->iCols, 1);
		if (pMat == NULL)
		{
			return NULL;
		}

		MV_Memcpy((VOID*)pMat->pDataAlign, (VOID*)pSrc->pDataAlign, pSrc->iLength);
	}

	return pMat;
}

VOID MV_MatrixCopy2_F(LPMV_MATRIX_F pSrc, LPMV_MATRIX_F pDst)
{
	INT i = 0, j = 0;

	if (!pSrc || !pDst || (pDst->iCols < pSrc->iCols) 
		|| (pDst->iRows < pSrc->iRows)) {
			return;
	}

	for (i = 0; i < pSrc->iRows; i++)
	{
		for (j = 0; j < pSrc->iCols; j++)
		{
			MV_MatrixSet_F(pDst, i, j, MV_MatrixGet_F(pSrc, i, j));
		}
	}
}

VOID MV_MatrixSetAll_F(LPMV_MATRIX_F pMatrix, FLOAT fxData)
{
	INT i = 0, j = 0;

	if (NULL != pMatrix) {
		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet_F(pMatrix, i, j, fxData);
			}
		}
	}
}

LPMV_MATRIX_F MV_MatrixAddSub_F(CONST LPMV_MATRIX_F pFirst, CONST LPMV_MATRIX_F pSecond, BOOL bAdd)
{
	INT i = 0, j = 0;
	FLOAT fxValue = 0.f;
	LPMV_MATRIX_F pMat = NULL;

	if ((NULL == pFirst) || (NULL == pSecond)) {
		return NULL;
	}

	if ((pFirst->iRows != pSecond->iRows) || (pFirst->iCols != pSecond->iCols))
	{
		return NULL;
	}

	pMat =MV_CreateMatrices_F(pFirst->iRows, pFirst->iCols, 1);
	if (pMat == NULL)
	{
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; i++)
	{
		for (j = 0; j < pFirst->iCols; j++)
		{
			fxValue = MV_MatrixGet_F(pFirst, i, j) + 
				(bAdd ? MV_MatrixGet_F(pSecond, i, j) : MV_MatrixGet_F(pSecond, i, j) * -1);
			MV_MatrixSet_F(pMat, i, j, fxValue);
		}
	}

	return pMat;
}

LPMV_MATRIX_F MV_MatrixDotMultiply_F(CONST LPMV_MATRIX_F pFirst, CONST FLOAT fxValue)
{
	INT i = 0, j = 0;
	FLOAT fxRes = 0.f;
	LPMV_MATRIX_F pMat = NULL;

	if (NULL == pFirst) {
		return NULL;
	}

	pMat = MV_CreateMatrices_F(pFirst->iRows, pFirst->iCols, 1);
	if (pMat == NULL)
	{
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; i++)
	{
		for (j = 0; j < pFirst->iCols; j++)
		{
			fxRes = MV_Mul_F(MV_MatrixGet_F(pFirst, i, j), fxValue);
			MV_MatrixSet_F(pMat, i, j, fxRes);
		}
	}

	return pMat;
}

LPMV_MATRIX_F MV_MatrixMultiply_F(CONST LPMV_MATRIX_F pFirst, CONST LPMV_MATRIX_F pSecond)
{
	INT i = 0, j = 0, k = 0;
	FLOAT iSum = 0;
	LPMV_MATRIX_F pMat = NULL;

	if ((NULL == pFirst) || (NULL == pSecond) 
		|| (pFirst->iCols != pSecond->iRows)) {
			return NULL;
	}

	pMat = MV_CreateMatrices_F(pFirst->iRows, pSecond->iCols, 1);
	if (pMat == NULL)
	{
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; ++i)
	{
		for (j = 0; j < pSecond->iCols; ++j)
		{
			iSum = 0.0;
			for (k = 0; k < pSecond->iRows; ++k)
			{
				iSum += MV_Mul_F(MV_MatrixGet_F(pFirst, i, k), MV_MatrixGet_F(pSecond, k, j));
			}

			MV_MatrixSet_F(pMat, i, j, iSum);
		}
	}

	return pMat;
}

LPMV_MATRIX_F MV_MatrixTranspose_F(CONST LPMV_MATRIX_F pMatrix)
{
	INT i = 0, j = 0;
	LPMV_MATRIX_F pMat = NULL;

	if (NULL == pMatrix) {
		return NULL;
	}

	pMat = MV_CreateMatrices_F(pMatrix->iCols, pMatrix->iRows, 1);
	if (pMat == NULL)
	{
		return NULL;
	}

	for (i = 0; i < pMatrix->iRows; i++)
	{
		for (j = 0; j < pMatrix->iCols; j++)
		{
			MV_MatrixSet_F(pMat, j, i, MV_MatrixGet_F(pMatrix, i, j));
		}
	}

	return pMat;
}

LPMV_MATRIX_F MV_MatrixInverse_F(CONST LPMV_MATRIX_F pMatrix)
{
	INT i = 0, j = 0, k = 0;
	LPMV_MATRIX_F pMatL = MV_CreateIdentityMatrix_F(pMatrix->iRows);
	LPMV_MATRIX_F pMatD = MV_CreateMatrices_F(pMatrix->iRows, pMatrix->iRows, 1);
	LPMV_MATRIX_F pMatTranspose = NULL, pMatStep = NULL, pMatResult = NULL, pMatLI = NULL;
	FLOAT sum = 0;

	if ((NULL == pMatrix) || (pMatrix->iRows != pMatrix->iCols))
	{
		return NULL;
	}
	if (NULL == pMatL) {
		return NULL;
	}
	if (NULL == pMatD) {
		MV_FreeMatrices_F(pMatL, 1);
		return NULL;
	}

	for (j = 0; j < pMatrix->iRows; j++)
	{
		sum = MV_MatrixGet_F(pMatrix, j, j);
		for (k = 0; k < j; k++)
		{
			sum -= MV_Mul_F(MV_Mul_F(MV_MatrixGet_F(pMatL, j, k), MV_MatrixGet_F(pMatL, j, k)), 
				MV_MatrixGet_F(pMatD, k, k));
		}
		MV_MatrixSet_F(pMatD, j, j, sum);

		for (i = j + 1; i < pMatrix->iRows; i++)
		{
			FLOAT fValue = 0.0f;
			sum = MV_MatrixGet_F(pMatrix, i, j);
			for (k = 0; k < j; k++)
			{
				sum -= MV_Mul_F(MV_Mul_F(MV_MatrixGet_F(pMatL, i, k), MV_MatrixGet_F(pMatL, j, k)), 
					MV_MatrixGet_F(pMatD, k, k));
			}

			fValue = MV_MatrixGet_F(pMatD, j, j);
			if (0.0f != fValue){
				MV_MatrixSet_F(pMatL, i, j, MV_Div_F(sum, fValue));
			}
			
		}
	}

	pMatLI = MV_CreateMatrices_F(pMatrix->iRows, pMatrix->iRows, 1);
	if (NULL == pMatLI) {
		MV_FreeMatrices_F(pMatD, 1);
		MV_FreeMatrices_F(pMatL, 1);
		return NULL;
	}
	for (i = 0; i < pMatrix->iRows; i++)
	{
		MV_MatrixSet_F(pMatLI, i, i, MV_ONE_F);
		for (j = 0; j < i; j++)
		{
			sum = 0;
			for (k = 0; k <= i; k++)
			{
				sum += MV_Mul_F(MV_MatrixGet_F(pMatL, i, k), MV_MatrixGet_F(pMatLI, k, j));
			}
			MV_MatrixSet_F(pMatLI, i, j, MV_Mul_F(sum, MV_MatrixGet_F(pMatLI, i, i)) * -1);
		}
	}

	for (i = 0; i < pMatD->iRows; i++)
	{
		for (j = 0; j < pMatD->iRows; j++)
		{
			if (MV_MatrixGet_F(pMatD, i, j) != 0)
			{
				MV_MatrixSet_F(pMatD, i, j, MV_Div_F(MV_ONE_F, MV_MatrixGet_F(pMatD, i, j)));
			}
		}
	}

	pMatTranspose = MV_MatrixTranspose_F(pMatLI);
	pMatStep = MV_MatrixMultiply_F(pMatTranspose, pMatD);
	pMatResult = MV_MatrixMultiply_F(pMatStep, pMatLI);

	MV_FreeMatrices_F(pMatTranspose, 1);
	MV_FreeMatrices_F(pMatStep, 1);
	MV_FreeMatrices_F(pMatD, 1);
	MV_FreeMatrices_F(pMatLI, 1);
	MV_FreeMatrices_F(pMatL, 1);
	return pMatResult;
}
