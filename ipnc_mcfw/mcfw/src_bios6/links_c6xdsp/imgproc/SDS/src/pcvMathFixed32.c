/*
* @(#)$Id: MathLib32.cpp 1007 2011-05-25 03:13:32Z LvGangXiao $ 
* @(#) Declaration file of math lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/

#include "pcvMathFixed32.h"
#include "pcvMisc.h"

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




MV_FIXED fxtabSin[NUMANGLE];
MV_FIXED fxtabCos[NUMANGLE];

VOID MV_InitMathLib32()
{
	INT n = 0;
	MV_FIXED fxang = 0;
	MV_FIXED fxrho = MV_ONE;
	MV_FIXED fxTheta = MV_Div(MV_PI, MV_FixedFromInt(NUMANGLE));

	// prepare sin/cos table
	for (fxang = 0, n = 0; n < NUMANGLE; fxang += fxTheta, n++)
	{
		fxtabSin[n] = MV_Div(MV_Sin(fxang), fxrho);
		fxtabCos[n] = MV_Div(MV_Cos(fxang), fxrho);
	}
}

MV_FIXED MV_FixedFromFloat(FLOAT value)
{
#ifdef MV_USE_IQMATH
	return _FtoIQ(value);
#else
	if (value >= 2147483647.5f)
		return (MV_FIXED)0x7fffffff;
	else if (value <= -2147483648.0f)
		return (MV_FIXED)0x80000000;
	else
		return (MV_FIXED)(value * (FLOAT)(MV_ONE));

#endif
}


MV_FIXED MV_FixedFromInt(INT value) {
#ifndef MV_USE_FP
	return (MV_FIXED)(value << MV_PRECISION);
#else
	return (MV_FIXED)value;
#endif
}

INT MV_IntFromFixed(MV_FIXED value) {
#ifndef MV_USE_FP
	return ((INT)((value + MV_HALF) >> MV_PRECISION));
#else
	return (INT)value;
#endif
}

INT MV_IntFromFixed_Floor(MV_FIXED value) {
#ifndef MV_USE_FP
	return (INT)(value >> MV_PRECISION);
#else
	return (INT)value;
#endif
}
MV_FIXED MV_FixScaleDown(MV_64 value) {
#ifndef MV_USE_FP
	return ((MV_FIXED)((value + MV_FLOAT_HALF) >> MV_PRECISION));
#else
	return (MV_FIXED)value;
#endif
}
MV_64 MV_FixScaleUp(MV_FIXED value) {
#ifndef MV_USE_FP
	return (MV_64)((MV_64)value << MV_PRECISION);
#else
	return (MV_64)value;
#endif
}
MV_FIXED MV_Floor(MV_FIXED value) {
#ifndef MV_USE_FP
	return ((value >> MV_PRECISION) << MV_PRECISION);
#else
	return value;
#endif
}
MV_FIXED MV_Ceil(MV_FIXED value) {
#ifndef MV_USE_FP
	return (((value >> MV_PRECISION) + 1) << MV_PRECISION);
#else
	return value;
#endif
}


FLOAT MV_FloatFromFixed(MV_FIXED value)
{
#ifdef MV_USE_IQMATH
	return _IQtoF(value);
#else
	return (FLOAT)((DOUBLE)value * (1.0f / (FLOAT)(MV_ONE)));
#endif
}

MV_FIXED MV_Abs(MV_FIXED value)
{
#ifdef MV_USE_IQMATH
	return _IQabs(value);
#else
	return (value < 0) ? -value : value;
#endif
}

MV_FIXED MV_Mul(MV_FIXED a, MV_FIXED b)
{
#ifndef MV_USE_FP
#ifdef MV_USE_IQMATH
	return _IQrsmpy(a, b);
#else
	MV_64 tmp = (MV_64)a * (MV_64)b;
	return ((MV_FIXED)((tmp + MV_FLOAT_HALF) >> MV_PRECISION));
#endif
#else
	return a * b;
#endif
}

MV_64 MV_Mul64(MV_FIXED a, MV_FIXED b)
{
	return ((MV_64)a * (MV_64)b);
}

MV_FIXED MV_Div(MV_FIXED a, MV_FIXED b)
{
	if (0 == b){
		return 0;
	}

#ifdef MV_USE_IQMATH
	return _IQdiv(a, b);
#else
	return (MV_FIXED)(MV_FixScaleUp(a) / b);
#endif
}

MV_64 MV_Div64(MV_FIXED a, MV_FIXED b)
{
	if (0 == b){
		return 0;
	}

	return ((MV_64)a / (MV_64)b);
}

MV_FIXED MV_Sin(MV_FIXED value)
{
#ifdef MV_USE_IQMATH
	return _IQsin(value);
#else
	return MV_FixedFromFloat((FLOAT)sin(MV_FloatFromFixed(value)));
#endif
}

MV_FIXED MV_Cos(MV_FIXED value)
{
#ifdef MV_USE_IQMATH
	return _IQcos(value);
#else
	return MV_FixedFromFloat((FLOAT)cos(MV_FloatFromFixed(value)));
#endif
}

MV_FIXED MV_Tan(MV_FIXED value)
{
#ifdef MV_USE_IQMATH
	return _IQdiv(_IQsin(value), _IQcos(value));
#else
	return MV_FixedFromFloat((FLOAT)tan(MV_FloatFromFixed(value)));
#endif
}

MV_FIXED MV_Atan(MV_FIXED value)
{
#ifdef MV_USE_IQMATH
	return _IQatan(value);
#else
	return MV_FixedFromFloat((FLOAT)atan(MV_FloatFromFixed(value)));
#endif
}

MV_FIXED MV_Pow(MV_FIXED value, MV_FIXED exp)
{
#ifdef MV_USE_IQMATH
	return (value == 0) ? 0 : _IQpow(value, exp);
#else
	return MV_FixedFromFloat((FLOAT)pow(MV_FloatFromFixed(value), MV_FloatFromFixed(exp)));
#endif
}

MV_FIXED MV_Sqrt(MV_FIXED value)
{
#ifdef MV_USE_IQMATH
	return _IQsqrt(value);
#else
	return MV_FixedFromFloat((FLOAT)sqrt(MV_FloatFromFixed(value)));
#endif
}

MV_FIXED MV_ISqrt(MV_FIXED value)
{
	if (0 == value){
		return 0;
	}

#ifdef MV_USE_IQMATH
	return _IQisqrt(value);
#else
	return MV_Div(MV_ONE, MV_Sqrt(value));
#endif
}

MV_FIXED MV_Exp(MV_FIXED value)
{
	return MV_FixedFromFloat((FLOAT)exp(MV_FloatFromFixed(value)));
}

MV_FIXED MV_Mag(MV_FIXED a, MV_FIXED b)
{
#ifdef MV_USE_IQMATH
	return _IQmag(a, b);
#else
	return MV_Sqrt(MV_Mul(a, a) + MV_Mul(b, b));
#endif
}

BOOL MV_InitializeMatrixData(LPMV_MATRIX pMatrix, INT iRows, INT iCols)
{
	if (NULL != pMatrix) {
		pMatrix->iRows = iRows;
		pMatrix->iCols = iCols;
		pMatrix->iPitch = MV_ALIGN(pMatrix->iCols * sizeof(MV_FIXED), pMatrix->iLineAddrAlign);
		pMatrix->iLength = pMatrix->iPitch * pMatrix->iRows;
		pMatrix->pData = (BYTE*)MV_MallocAlign(pMatrix->iLength, pMatrix->iStartAddrAlign, (VOID**)&pMatrix->pDataAlign);
		if (pMatrix->pData != NULL) {
			return TRUE;
		}
	}

	return FALSE;
}

BOOL MV_InitializeMatrixDataB(LPMV_MATRIX pMatrix, INT iRows, INT iCols, BYTE *pMem)
{
	if (NULL == pMatrix) {
		return FALSE;
	}

	pMatrix->iRows = iRows;
	pMatrix->iCols = iCols;
	pMatrix->iPitch = MV_ALIGN(pMatrix->iCols * sizeof(MV_FIXED), pMatrix->iLineAddrAlign);
	pMatrix->iLength = pMatrix->iPitch * pMatrix->iRows;
	pMatrix->pData = pMem;
	pMatrix->pDataAlign = (MV_FIXED*)MV_ALIGNADDR(pMatrix->pData, pMatrix->iStartAddrAlign);
	return TRUE;
}

LPMV_MATRIX MV_CreateIdentityMatrix(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign)
{
	INT i = 0;
	LPMV_MATRIX pMatrix = NULL;

	pMatrix = (LPMV_MATRIX)MV_CreateMatrices(iRows, iRows, 1, iStartAddrAlign, iLineAddrAlign);
	if (pMatrix == NULL)
	{
		return NULL;
	}

	for (i = 0; i < iRows; i++)
	{
		MV_MatrixSet(pMatrix, i, i, (MV_FIXED)MV_ONE);
	}

	return pMatrix;	
}

LPMV_MATRIX MV_CreateIdentityMatrixB(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem)
{
	INT i = 0;
	LPMV_MATRIX pMatrix = NULL;

	pMatrix = (LPMV_MATRIX)MV_CreateMatrixB(iRows, iRows, iStartAddrAlign, iLineAddrAlign, pMem);
	if (pMatrix == NULL)
	{
		return NULL;
	}

	for (i = 0; i < iRows; i++)
	{
		MV_MatrixSet(pMatrix, i, i, (MV_FIXED)MV_ONE);
	}

	return pMatrix;	
}

LPMV_MATRIX MV_CreateMatrices(INT iRows, INT iCols, INT iNumber, 
							  INT iStartAddrAlign, INT iLineAddrAlign)
{
	INT i = 0;
	LPMV_MATRIX pMatrices = (LPMV_MATRIX)MV_Malloc(iNumber * sizeof(MV_MATRIX));
	if (pMatrices == NULL)
	{
		return NULL;
	}

	for (i = 0; i < iNumber; i++)
	{
		pMatrices[i].iStartAddrAlign = MV_MAX(iStartAddrAlign, MV_MIN_ALIGN);
		pMatrices[i].iLineAddrAlign = iLineAddrAlign;
		if (!MV_InitializeMatrixData(&pMatrices[i], iRows, (iCols == 0 ? iRows : iCols)))
		{
			return NULL;
		}
	}

	return pMatrices;
}

LPMV_MATRIX MV_CreateMatrixB(INT iRows, INT iCols, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem)
{
	LPMV_MATRIX pMatrix = (LPMV_MATRIX)pMem;
	if (pMatrix == NULL)
	{
		return NULL;
	}

	pMem += ((DWORD)&pMatrix->pData - (DWORD)pMatrix);
	pMatrix->iStartAddrAlign = MV_MAX(iStartAddrAlign, MV_MIN_ALIGN);
	pMatrix->iLineAddrAlign = iLineAddrAlign;
	if (!MV_InitializeMatrixDataB(pMatrix, iRows, (iCols == 0 ? iRows : iCols), pMem))
	{
		return NULL;
	}

	return pMatrix;
}

VOID MV_FreeMatrices(LPMV_MATRIX pMatrices, INT iNumber)
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

DWORD MV_MatrixGetSize(LPMV_MATRIX pMatrix)
{
	INT iSize = 0;

	if (NULL != pMatrix) {
		iSize = sizeof(MV_MATRIX) + pMatrix->iLength + pMatrix->iStartAddrAlign;
	} 

	return iSize;
}

LPMV_MATRIX MV_MatrixCopy(LPMV_MATRIX pSrc)
{
	LPMV_MATRIX pMat = NULL;

	if (NULL != pSrc) {
		pMat = MV_CreateMatrices(pSrc->iRows, pSrc->iCols, 1, 
			pSrc->iStartAddrAlign, pSrc->iLineAddrAlign);
		if (pMat == NULL) {
			return NULL;
		}

		MV_Memcpy((VOID*)pMat->pDataAlign, (VOID*)pSrc->pDataAlign, pSrc->iLength);
	}
	
	return pMat;
}

VOID MV_MatrixCopy2(LPMV_MATRIX pSrc, LPMV_MATRIX pDst)
{
	INT i = 0, j = 0;
	
	if ((NULL == pSrc) || (NULL == pDst)
		|| (pDst->iRows < pSrc->iRows) || (pDst->iCols < pSrc->iCols)) {
		return;
	}

	for (i = 0; i < pSrc->iRows; i++)
	{
		for (j = 0; j < pSrc->iCols; j++)
		{
			MV_MatrixSet(pDst, i, j, MV_MatrixGet(pSrc, i, j));
		}
	}
}

MV_FIXED MV_MatrixGet(CONST LPMV_MATRIX pMatrix, INT iRow, INT iCol)
{
	if (NULL == pMatrix) {
		return 0;
	}

	return *((MV_FIXED*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol);
}

MV_FIXED* MV_MatrixGetLinePtr(LPMV_MATRIX pMatrix, INT iRow)
{
	if (NULL == pMatrix) {
		return NULL;
	}

	return ((MV_FIXED*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch));
}

VOID MV_MatrixSet(LPMV_MATRIX pMatrix, INT iRow, INT iCol, MV_FIXED fxData)
{
	if (NULL == pMatrix) {
		return;
	}

	*((MV_FIXED*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol) = fxData;
}

VOID MV_MatrixToIntAll(LPMV_MATRIX pMatrix)
{
	INT i = 0, j = 0;

	if (NULL != pMatrix) {
		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet(pMatrix, i, j, MV_IntFromFixed(MV_MatrixGet(pMatrix, i, j)));
			}
		}
	}
}

VOID MV_MatrixToFixedAll(LPMV_MATRIX pMatrix)
{
	INT i = 0, j = 0;

	if (NULL != pMatrix) {
		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet(pMatrix, i, j, MV_FixedFromInt(MV_MatrixGet(pMatrix, i, j)));
			}
		}
	}
}

VOID MV_MatrixSetAll(LPMV_MATRIX pMatrix, MV_FIXED fxData)
{
	INT i = 0, j = 0;

	if (NULL != pMatrix) {
		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet(pMatrix, i, j, fxData);
			}
		}
	}
}

LPMV_MATRIX MV_MatrixAddSub(CONST LPMV_MATRIX pFirst, CONST LPMV_MATRIX pSecond, BOOL bAdd)
{
	INT i = 0, j = 0;
	MV_FIXED fxValue = 0;
	LPMV_MATRIX pMat = NULL;

	if ((NULL == pFirst) || (NULL == pSecond)) {
		return NULL;
	}

	if ((pFirst->iRows != pSecond->iRows) || (pFirst->iCols != pSecond->iCols)) {
		return NULL;
	}

	pMat = MV_CreateMatrices(pFirst->iRows, pFirst->iCols, 1, 
		pFirst->iStartAddrAlign, pFirst->iLineAddrAlign);
	if (pMat == NULL) {
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; i++)
	{
		for (j = 0; j < pFirst->iCols; j++)
		{
			fxValue = MV_MatrixGet(pFirst, i, j) + 
				(bAdd ? MV_MatrixGet(pSecond, i, j) : MV_MatrixGet(pSecond, i, j) * -1);
			MV_MatrixSet(pMat, i, j, fxValue);
		}
	}

	return pMat;
}

LPMV_MATRIX MV_MatrixDotMultiply(CONST LPMV_MATRIX pFirst, CONST MV_FIXED fxValue)
{
	INT i = 0, j = 0;
	MV_FIXED fxRes = 0;
	LPMV_MATRIX pMat = NULL;

	if (NULL == pFirst) {
		return NULL;
	}

	pMat = MV_CreateMatrices(pFirst->iRows, pFirst->iCols, 1, 
		pFirst->iStartAddrAlign, pFirst->iLineAddrAlign);
	if (pMat == NULL) {
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; i++)
	{
		for (j = 0; j < pFirst->iCols; j++)
		{
			fxRes = MV_Mul(MV_MatrixGet(pFirst, i, j), fxValue);
			MV_MatrixSet(pMat, i, j, fxRes);
		}
	}

	return pMat;
}

LPMV_MATRIX MV_MatrixMultiply(CONST LPMV_MATRIX pFirst, CONST LPMV_MATRIX pSecond)
{
	INT i = 0, j = 0, k = 0;
	MV_64 iSum = 0;
	LPMV_MATRIX pMat = NULL;

	if ((NULL == pFirst) || (NULL == pSecond) 
		|| (pFirst->iCols != pSecond->iRows)) {
		return NULL;
	}

	pMat = MV_CreateMatrices(pFirst->iRows, pSecond->iCols, 1, 
		pSecond->iStartAddrAlign, pSecond->iLineAddrAlign);
	if (pMat == NULL) {
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; ++i)
	{
		for (j = 0; j < pSecond->iCols; ++j)
		{
			iSum = 0;
			for (k = 0; k < pSecond->iRows; ++k)
			{
				iSum += MV_Mul64(MV_MatrixGet(pFirst, i, k), MV_MatrixGet(pSecond, k, j));
			}

			MV_MatrixSet(pMat, i, j, MV_FixScaleDown(iSum));
		}
	}

	return pMat;
}

LPMV_MATRIX MV_MatrixTranspose(CONST LPMV_MATRIX pMatrix)
{
	INT i = 0, j = 0;
	LPMV_MATRIX pMat = NULL;

	if (NULL == pMatrix) {
		return NULL;
	}

	pMat = MV_CreateMatrices(pMatrix->iCols, pMatrix->iRows, 1, 
		pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
	if (pMat == NULL) {
		return NULL;
	}

	for (i = 0; i < pMatrix->iRows; i++)
	{
		for (j = 0; j < pMatrix->iCols; j++)
		{
			MV_MatrixSet(pMat, j, i, MV_MatrixGet(pMatrix, i, j));
		}
	}

	return pMat;
}

LPMV_MATRIX MV_MatrixInverse(CONST LPMV_MATRIX pMatrix)
{
	INT i = 0, j = 0, k = 0;
	LPMV_MATRIX pMatL= NULL, pMatD = NULL, pMatLI = NULL;
	LPMV_MATRIX pMatTranspose = NULL, pMatStep = NULL, pMatResult = NULL;
	MV_64 sum = 0;

	if ((NULL == pMatrix) || (pMatrix->iRows != pMatrix->iCols)) {
		return NULL;
	}

	pMatL = MV_CreateIdentityMatrix(pMatrix->iRows, 
		pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
	if (NULL == pMatL) {
		return NULL;
	}

	pMatD = MV_CreateMatrices(pMatrix->iRows, pMatrix->iRows, 1, 
		pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
	if (NULL == pMatD) {
		MV_FreeMatrices(pMatL, 1);
		return NULL;
	}

	for (j = 0; j < pMatrix->iRows; j++)
	{
		sum = MV_FixScaleUp(MV_MatrixGet(pMatrix, j, j));
		for (k = 0; k < j; k++)
		{
			sum -= MV_Mul64(MV_Mul(MV_MatrixGet(pMatL, j, k), MV_MatrixGet(pMatL, j, k)), 
				MV_MatrixGet(pMatD, k, k));
		}
		MV_MatrixSet(pMatD, j, j, MV_FixScaleDown(sum));

		for (i = j + 1; i < pMatrix->iRows; i++)
		{
			MV_FIXED fixValue = 0;
			sum = MV_FixScaleUp(MV_MatrixGet(pMatrix, i, j));
			for (k = 0; k < j; k++)
			{
				sum -= MV_Mul64(MV_Mul(MV_MatrixGet(pMatL, i, k), MV_MatrixGet(pMatL, j, k)), 
					MV_MatrixGet(pMatD, k, k));
			}

			fixValue = MV_MatrixGet(pMatD, j, j);

			if (0 != fixValue){
				MV_MatrixSet(pMatL, i, j, (MV_FIXED)MV_Div(MV_FixScaleDown(sum),fixValue));
			}
			
		}
	}

	pMatLI = MV_CreateMatrices(pMatrix->iRows, pMatrix->iRows, 1, 
		pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
	if (NULL == pMatLI) {
		MV_FreeMatrices(pMatD, 1);
		MV_FreeMatrices(pMatL, 1);
		return NULL;
	}

	for (i = 0; i < pMatrix->iRows; i++)
	{
		MV_MatrixSet(pMatLI, i, i, (MV_FIXED)MV_ONE);
		for (j = 0; j < i; j++)
		{
			sum = 0;
			for (k = 0; k <= i; k++)
			{
				sum += MV_Mul64(MV_MatrixGet(pMatL, i, k), MV_MatrixGet(pMatLI, k, j));
			}
			MV_MatrixSet(pMatLI, i, j, MV_Mul(MV_FixScaleDown(sum), MV_MatrixGet(pMatLI, i, i)) * -1);
		}
	}

	for (i = 0; i < pMatD->iRows; i++)
	{
		for (j = 0; j < pMatD->iRows; j++)
		{
			if (MV_MatrixGet(pMatD, i, j) != 0)
			{
				MV_MatrixSet(pMatD, i, j, (MV_FIXED)(MV_Div((MV_FIXED)MV_ONE, MV_MatrixGet(pMatD, i, j))));
			}
		}
	}

	pMatTranspose = MV_MatrixTranspose(pMatLI);
	pMatStep = MV_MatrixMultiply(pMatTranspose, pMatD);
	pMatResult = MV_MatrixMultiply(pMatStep, pMatLI);

	MV_FreeMatrices(pMatTranspose, 1);
	MV_FreeMatrices(pMatStep, 1);
	MV_FreeMatrices(pMatD, 1);
	MV_FreeMatrices(pMatLI, 1);
	MV_FreeMatrices(pMatL, 1);
	return pMatResult;
}

VOID MV_PrintMat(LPMV_MATRIX pMat)
{
	INT i = 0, j = 0;

	if (NULL != pMat) {
		for (i = 0; i < pMat->iRows; i++)
		{
			for (j = 0; j < pMat->iCols; j++)
			{
				MV_Printf("%d\t", MV_MatrixGet(pMat, i, j));
			}
			MV_Printf("\n");
		}
	}
}

////////////////////////////////////////////////////////////////////////

BOOL MV_InitializeMatrixData_S(LPMV_MATRIX_S pMatrix, INT iRows, INT iCols)
{
	if (NULL != pMatrix) {
		pMatrix->iRows = iRows;
		pMatrix->iCols = iCols;
		pMatrix->iPitch = MV_ALIGN(pMatrix->iCols * sizeof(MV_FIXED_S), pMatrix->iLineAddrAlign);
		pMatrix->iLength = pMatrix->iPitch * pMatrix->iRows;
		pMatrix->pData = (BYTE*)MV_MallocAlign(pMatrix->iLength, pMatrix->iStartAddrAlign, (VOID**)&pMatrix->pDataAlign);
		if (pMatrix->pData != NULL)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL MV_InitializeMatrixDataB_S(LPMV_MATRIX_S pMatrix, INT iRows, INT iCols, BYTE *pMem)
{
	if (NULL == pMatrix) {
		return FALSE;
	}

	pMatrix->iRows = iRows;
	pMatrix->iCols = iCols;
	pMatrix->iPitch = MV_ALIGN(pMatrix->iCols * sizeof(MV_FIXED_S), pMatrix->iLineAddrAlign);
	pMatrix->iLength = pMatrix->iPitch * pMatrix->iRows;
	pMatrix->pData = pMem;
	pMatrix->pDataAlign = (MV_FIXED_S*)MV_ALIGNADDR(pMatrix->pData, pMatrix->iStartAddrAlign);
	return TRUE;
}

LPMV_MATRIX_S MV_CreateIdentityMatrix_S(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign)
{
	INT i = 0;
	LPMV_MATRIX_S pMatrix = (LPMV_MATRIX_S)MV_CreateMatrices_S(iRows, iRows, 1, iStartAddrAlign, iLineAddrAlign);
	if (pMatrix == NULL) {
		return NULL;
	}

	for (i = 0; i < iRows; i++)
	{
		MV_MatrixSet_S(pMatrix, i, i, (MV_FIXED_S)MV_ONE);
	}

	return pMatrix;	
}

LPMV_MATRIX_S MV_CreateIdentityMatrixB_S(INT iRows, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem)
{
	INT i = 0;
	LPMV_MATRIX_S pMatrix = (LPMV_MATRIX_S)MV_CreateMatrixB_S(iRows, iRows, iStartAddrAlign, iLineAddrAlign, pMem);
	if (pMatrix == NULL) {
		return NULL;
	}

	for (i = 0; i < iRows; i++)
	{
		MV_MatrixSet_S(pMatrix, i, i, (MV_FIXED_S)MV_ONE);
	}

	return pMatrix;
}

LPMV_MATRIX_S MV_CreateMatrices_S(INT iRows, INT iCols, INT iNumber, 
								  INT iStartAddrAlign, INT iLineAddrAlign)
{
	INT i = 0;
	LPMV_MATRIX_S pMatrices = (LPMV_MATRIX_S)MV_Malloc(iNumber * sizeof(MV_MATRIX_S));
	if (pMatrices == NULL)
	{
		return NULL;
	}

	for (i = 0; i < iNumber; i++)
	{
		pMatrices[i].iStartAddrAlign = MV_MAX(iStartAddrAlign, MV_MIN_ALIGN);
		pMatrices[i].iLineAddrAlign = iLineAddrAlign;
		if (!MV_InitializeMatrixData_S(&pMatrices[i], iRows, (iCols == 0 ? iRows : iCols)))
		{
			return NULL;
		}
	}

	return pMatrices;
}

LPMV_MATRIX_S MV_CreateMatrixB_S(INT iRows, INT iCols, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem)
{
	LPMV_MATRIX_S pMatrix = (LPMV_MATRIX_S)pMem;
	if (pMatrix == NULL)
	{
		return NULL;
	}

	pMem += ((DWORD)&pMatrix->pData - (DWORD)pMatrix);
	pMatrix->iStartAddrAlign = MV_MAX(iStartAddrAlign, MV_MIN_ALIGN);
	pMatrix->iLineAddrAlign = iLineAddrAlign;
	if (!MV_InitializeMatrixDataB_S(pMatrix, iRows, (iCols == 0 ? iRows : iCols), pMem))
	{
		return NULL;
	}

	return pMatrix;
}

VOID MV_FreeMatrices_S(LPMV_MATRIX_S pMatrices, INT iNumber)
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

DWORD MV_MatrixGetSize_S(LPMV_MATRIX_S pMatrix)
{
	INT iSize = 0;
	
	if (NULL != pMatrix) {
		iSize = sizeof(MV_MATRIX_S) + pMatrix->iLength + pMatrix->iStartAddrAlign;
	}

	return iSize;
}

LPMV_MATRIX_S MV_MatrixCopy_S(LPMV_MATRIX_S pSrc)
{
	LPMV_MATRIX_S pMat = NULL;

	if (NULL == pSrc) {
		return NULL;
	}

	pMat = MV_CreateMatrices_S(pSrc->iRows, pSrc->iCols, 1, 
		pSrc->iStartAddrAlign, pSrc->iLineAddrAlign);
	if (pMat == NULL) {
		return NULL;
	}

	MV_Memcpy((VOID*)pMat->pDataAlign, (VOID*)pSrc->pDataAlign, pSrc->iLength);
	return pMat;
}

VOID MV_MatrixCopy2_S(LPMV_MATRIX_S pSrc, LPMV_MATRIX_S pDst)
{
	INT i = 0;//, j = 0;

	if ((NULL != pSrc) && (NULL != pDst)) {
		BYTE *pDataDst = (BYTE*)pDst->pDataAlign;
		BYTE *pDataSrc = (BYTE*)pSrc->pDataAlign;

		if ((NULL != pDataSrc) && (NULL != pDataDst) 
			&& (pDst->iRows >= pSrc->iRows) && (pDst->iCols >= pSrc->iCols)) {
			for (i = 0; i < pSrc->iRows; i++)
			{
				MV_Memcpy(pDataDst, pDataSrc, pSrc->iCols * sizeof(MV_FIXED_S));
				pDataDst += pDst->iPitch;
				pDataSrc += pSrc->iPitch;
			}
		}
	}
}

MV_FIXED_S MV_MatrixGet_S(CONST LPMV_MATRIX_S pMatrix, INT iRow, INT iCol)
{
	if (NULL == pMatrix) {
		return 0;
	}

	return *((MV_FIXED_S*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol);
}

//inline MV_FIXED_S* MV_MatrixGetLinePtr_S(LPMV_MATRIX_S pMatrix, INT iRow, INT iCol)
//{
//
//	if (NULL == pMatrix) {
//		return NULL;
//	}
//
//	return ((MV_FIXED_S*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol);
//
//}

VOID MV_MatrixSet_S(LPMV_MATRIX_S pMatrix, INT iRow, INT iCol, MV_FIXED_S fxData)
{
	if (NULL == pMatrix) {
		return;
	}

	*((MV_FIXED_S*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol) = fxData;
}

VOID MV_MatrixToIntAll_S(LPMV_MATRIX_S pMatrix)
{
	INT i = 0, j = 0;

	if (NULL != pMatrix) {
		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet_S(pMatrix, i, j, MV_IntFromFixed(MV_MatrixGet_S(pMatrix, i, j)));
			}
		}
	}
}

VOID MV_MatrixToFixedAll_S(LPMV_MATRIX_S pMatrix)
{
	INT i = 0, j = 0;

	if (NULL != pMatrix) {
		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet_S(pMatrix, i, j, MV_FixedFromInt(MV_MatrixGet_S(pMatrix, i, j)));
			}
		}
	}
}

VOID MV_MatrixSetAll_S(LPMV_MATRIX_S pMatrix, MV_FIXED_S fxData)
{
	INT i = 0, j = 0;

	if (NULL != pMatrix) {
		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet_S(pMatrix, i, j, fxData);
			}
		}
	}
}

LPMV_MATRIX_S MV_MatrixAddSub_S(CONST LPMV_MATRIX_S pFirst, CONST LPMV_MATRIX_S pSecond, BOOL bAdd)
{
	INT i = 0, j = 0;
	MV_FIXED_S fxValue = 0;
	LPMV_MATRIX_S pMat = NULL;

	if ((NULL == pFirst) || (NULL == pSecond) ||
		(pFirst->iRows != pSecond->iRows) || (pFirst->iCols != pSecond->iCols))
	{
		return NULL;
	}

	pMat = MV_CreateMatrices_S(pFirst->iRows, pFirst->iCols, 1, 
		pFirst->iStartAddrAlign, pFirst->iLineAddrAlign);
	if (pMat == NULL) {
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; i++)
	{
		for (j = 0; j < pFirst->iCols; j++)
		{
			fxValue = MV_MatrixGet_S(pFirst, i, j) + 
				(bAdd ? MV_MatrixGet_S(pSecond, i, j) : MV_MatrixGet_S(pSecond, i, j) * -1);
			MV_MatrixSet_S(pMat, i, j, fxValue);
		}
	}

	return pMat;
}

LPMV_MATRIX_S MV_MatrixDotMultiply_S(CONST LPMV_MATRIX_S pFirst, CONST MV_FIXED_S fxValue)
{
	INT i = 0, j = 0;
	MV_FIXED_S fxRes = 0;
	LPMV_MATRIX_S pMat = NULL;

	if (NULL != pFirst) {
		pMat = MV_CreateMatrices_S(pFirst->iRows, pFirst->iCols, 1, 
			pFirst->iStartAddrAlign, pFirst->iLineAddrAlign);
		if (pMat == NULL) {
			return NULL;
		}

		for (i = 0; i < pFirst->iRows; i++)
		{
			for (j = 0; j < pFirst->iCols; j++)
			{
				fxRes = MV_Mul(MV_MatrixGet_S(pFirst, i, j), fxValue);
				MV_MatrixSet_S(pMat, i, j, fxRes);
			}
		}
	}

	return pMat;
}

LPMV_MATRIX_S MV_MatrixMultiply_S(CONST LPMV_MATRIX_S pFirst, CONST LPMV_MATRIX_S pSecond)
{
	INT i = 0, j = 0, k = 0;
	MV_64 iSum = 0;
	LPMV_MATRIX_S pMat = NULL;

	if ((NULL == pFirst) || (NULL == pSecond) || (pFirst->iCols != pSecond->iRows)) {
		return NULL;
	}

	pMat = MV_CreateMatrices_S(pFirst->iRows, pSecond->iCols, 1, 
		pSecond->iStartAddrAlign, pSecond->iLineAddrAlign);
	if (pMat == NULL) {
		return NULL;
	}

	for (i = 0; i < pFirst->iRows; ++i)
	{
		for (j = 0; j < pSecond->iCols; ++j)
		{
			iSum = 0;

			for (k = 0; k < pSecond->iRows; ++k)
			{
				iSum += MV_Mul64(MV_MatrixGet_S(pFirst, i, k), MV_MatrixGet_S(pSecond, k, j));
			}

			MV_MatrixSet_S(pMat, i, j, MV_FixScaleDown(iSum));
		}
	}

	return pMat;
}

LPMV_MATRIX_S MV_MatrixTranspose_S(CONST LPMV_MATRIX_S pMatrix)
{
	INT i = 0, j = 0;
	LPMV_MATRIX_S  pMat = NULL;

	if (NULL != pMatrix) {
		pMat = MV_CreateMatrices_S(pMatrix->iCols, pMatrix->iRows, 1, 
			pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
		if (pMat == NULL) {
			return NULL;
		}

		for (i = 0; i < pMatrix->iRows; i++)
		{
			for (j = 0; j < pMatrix->iCols; j++)
			{
				MV_MatrixSet_S(pMat, j, i, MV_MatrixGet_S(pMatrix, i, j));
			}
		}
	}

	return pMat;
}

LPMV_MATRIX_S MV_MatrixInverse_S(CONST LPMV_MATRIX_S pMatrix)
{
	INT i = 0, j = 0, k = 0;
	LPMV_MATRIX_S pMatL = NULL, pMatD = NULL, pMatLI = NULL;
	LPMV_MATRIX_S pMatTranspose = NULL, pMatStep = NULL, pMatResult = NULL;
	MV_64 sum = 0;

	if ((NULL == pMatrix) || (pMatrix->iRows != pMatrix->iCols)) {
		return NULL;
	}

	pMatL = MV_CreateIdentityMatrix_S(pMatrix->iRows, 
		pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
	if (NULL == pMatL) {
		return NULL;
	}

	pMatD = MV_CreateMatrices_S(pMatrix->iRows, pMatrix->iRows, 1, 
		pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
	if (NULL == pMatD) {
		MV_FreeMatrices_S(pMatL, 1);
		return NULL;
	}

	for (j = 0; j < pMatrix->iRows; j++)
	{
		sum = MV_FixScaleUp(MV_MatrixGet_S(pMatrix, j, j));
		for (k = 0; k < j; k++)
		{
			sum -= MV_Mul64(MV_Mul(MV_MatrixGet_S(pMatL, j, k), MV_MatrixGet_S(pMatL, j, k)), 
				MV_MatrixGet_S(pMatD, k, k));
		}
		MV_MatrixSet_S(pMatD, j, j, MV_FixScaleDown(sum));

		for (i = j + 1; i < pMatrix->iRows; i++)
		{
			MV_FIXED_S fixsValue = 0;
			sum = MV_FixScaleUp(MV_MatrixGet_S(pMatrix, i, j));
			for (k = 0; k < j; k++)
			{
				sum -= MV_Mul64(MV_Mul(MV_MatrixGet_S(pMatL, i, k), MV_MatrixGet_S(pMatL, j, k)), 
					MV_MatrixGet_S(pMatD, k, k));
			}

			fixsValue = MV_MatrixGet_S(pMatD, j, j);
			if (0 != fixsValue){
				MV_MatrixSet_S(pMatL, i, j, (MV_FIXED_S)MV_Div(MV_FixScaleDown(sum), fixsValue));
			}
			
		}
	}

	pMatLI = MV_CreateMatrices_S(pMatrix->iRows, pMatrix->iRows, 1, 
		pMatrix->iStartAddrAlign, pMatrix->iLineAddrAlign);
	if (NULL == pMatLI) {
		MV_FreeMatrices_S(pMatD, 1);
		MV_FreeMatrices_S(pMatLI, 1);
		return NULL;
	}

	for (i = 0; i < pMatrix->iRows; i++)
	{
		MV_MatrixSet_S(pMatLI, i, i, (MV_FIXED_S)MV_ONE);
		for (j = 0; j < i; j++)
		{
			sum = 0;
			for (k = 0; k <= i; k++)
			{
				sum += MV_Mul64(MV_MatrixGet_S(pMatL, i, k), MV_MatrixGet_S(pMatLI, k, j));
			}
			MV_MatrixSet_S(pMatLI, i, j, MV_Mul(MV_FixScaleDown(sum), MV_MatrixGet_S(pMatLI, i, i)) * -1);
		}
	}

	for (i = 0; i < pMatD->iRows; i++)
	{
		for (j = 0; j < pMatD->iRows; j++)
		{
			if (MV_MatrixGet_S(pMatD, i, j) != 0)
			{
				MV_MatrixSet_S(pMatD, i, j, (MV_FIXED_S)(MV_Div((MV_FIXED_S)MV_ONE, MV_MatrixGet_S(pMatD, i, j))));
			}
		}
	}

	pMatTranspose = MV_MatrixTranspose_S(pMatLI);
	pMatStep = MV_MatrixMultiply_S(pMatTranspose, pMatD);
	pMatResult = MV_MatrixMultiply_S(pMatStep, pMatLI);

	MV_FreeMatrices_S(pMatTranspose, 1);
	MV_FreeMatrices_S(pMatStep, 1);
	MV_FreeMatrices_S(pMatD, 1);
	MV_FreeMatrices_S(pMatLI, 1);
	MV_FreeMatrices_S(pMatL, 1);
	return pMatResult;
}

VOID MV_PrintMat_S(LPMV_MATRIX_S pMat)
{
	INT i = 0, j = 0;

	if (NULL != pMat) {
		for (i = 0; i < pMat->iRows; i++)
		{
			for (j = 0; j < pMat->iCols; j++)
			{
				MV_Printf("%d\t", MV_MatrixGet_S(pMat, i, j));
			}
			MV_Printf("\n");
		}
	}
}

////////////////////////////////////////////////////////////////////////

BOOL MV_InitializeMatrixData_B(LPMV_MATRIX_B pMatrix, INT iRows, INT iCols)
{
	if (NULL != pMatrix) {
		pMatrix->iRows = iRows;
		pMatrix->iCols = iCols;
		pMatrix->iPitch = MV_ALIGN(pMatrix->iCols * sizeof(MV_FIXED_B), pMatrix->iLineAddrAlign);
		pMatrix->iLength = pMatrix->iPitch * pMatrix->iRows;
		pMatrix->pData = (BYTE*)MV_MallocAlign(pMatrix->iLength, pMatrix->iStartAddrAlign, (VOID**)&pMatrix->pDataAlign);
		if (pMatrix->pData != NULL)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL MV_InitializeMatrixDataB_B(LPMV_MATRIX_B pMatrix, INT iRows, INT iCols, BYTE *pMem)
{
	if (NULL == pMatrix) {
		return FALSE;
	}

	pMatrix->iRows = iRows;
	pMatrix->iCols = iCols;
	pMatrix->iPitch = MV_ALIGN(pMatrix->iCols * sizeof(MV_FIXED_B), pMatrix->iLineAddrAlign);
	pMatrix->iLength = pMatrix->iPitch * pMatrix->iRows;
	pMatrix->pData = pMem;
	pMatrix->pDataAlign = (MV_FIXED_B*)MV_ALIGNADDR(pMatrix->pData, pMatrix->iStartAddrAlign);
	return TRUE;
}

LPMV_MATRIX_B MV_CreateMatrices_B(INT iRows, INT iCols, INT iNumber, INT iStartAddrAlign, INT iLineAddrAlign)
{
	INT i = 0;
	LPMV_MATRIX_B pMatrices = (LPMV_MATRIX_B)MV_Malloc(iNumber * sizeof(MV_MATRIX_B));
	if (pMatrices == NULL) {
		return NULL;
	}

	for (i = 0; i < iNumber; i++)
	{
		pMatrices[i].iStartAddrAlign = MV_MAX(iStartAddrAlign, MV_MIN_ALIGN);
		pMatrices[i].iLineAddrAlign = iLineAddrAlign;
		if (!MV_InitializeMatrixData_B(&pMatrices[i], iRows, (iCols == 0 ? iRows : iCols)))
		{
			return NULL;
		}
	}

	return pMatrices;
}

LPMV_MATRIX_B MV_CreateMatrixB_B(INT iRows, INT iCols, INT iStartAddrAlign, INT iLineAddrAlign, BYTE *pMem)
{
	LPMV_MATRIX_B pMatrix = (LPMV_MATRIX_B)pMem;
	if (pMatrix == NULL) {
		return NULL;
	}

	pMem += ((DWORD)&pMatrix->pData - (DWORD)pMatrix);
	pMatrix->iStartAddrAlign = MV_MAX(iStartAddrAlign, MV_MIN_ALIGN);
	pMatrix->iLineAddrAlign = iLineAddrAlign;
	if (!MV_InitializeMatrixDataB_B(pMatrix, iRows, (iCols == 0 ? iRows : iCols), pMem))
	{
		return NULL;
	}

	return pMatrix;
}

VOID MV_FreeMatrices_B(LPMV_MATRIX_B pMatrices, INT iNumber)
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

MV_FIXED_B MV_MatrixGet_B(CONST LPMV_MATRIX_B pMatrix, INT iRow, INT iCol)
{
	if (NULL == pMatrix) {
		return 0;
	}

	return *((MV_FIXED_B*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol);
}

VOID MV_MatrixSet_B(LPMV_MATRIX_B pMatrix, INT iRow, INT iCol, MV_FIXED_B fxData)
{
	if (NULL == pMatrix) {
		return;
	}

	*((MV_FIXED_B*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch) + iCol) = fxData;
}

MV_FIXED_B* MV_MatrixGetLinePtr_B(LPMV_MATRIX_B pMatrix, INT iRow)
{
	if (NULL == pMatrix) {
		return NULL;
	}

	return ((MV_FIXED_B*)(((BYTE*)pMatrix->pDataAlign) + iRow * pMatrix->iPitch));
}

////////////////////////////////////////////////////////////////////////

VOID MV_Convolution_Int(CONST MV_FIXED_S *pInData, MV_FIXED_S *pOutData, 
						SHORT iWidth, SHORT iPitch, CONST MV_FIXED_S *pMask, 
						SHORT iMaskSize, SHORT iShift)
{
	INT i = 0, j = 0, k = 0, iSkipTo = 0;
	MV_FIXED iSum = 0;

	if (!pInData || !pOutData || !pMask) {
		return;
	}

#ifndef MV_USE_NEON
	for (i = 0; i < iWidth - iMaskSize + 1; i++)
	{
		iSum = 0;
		for (j = 0; j < iMaskSize; j++)
		{
			for (k = 0; k < iMaskSize; k++)
			{
				MV_FIXED_S k1 = (*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + k));
				MV_FIXED_S k2 = pMask[(j * iMaskSize + k)];
				iSum += (MV_FIXED)k1 * k2;
			}
		}

#ifndef MV_USE_FP
		iSum >>= iShift;
		iSum = (iSum > MV_SHORTMAX) ? MV_SHORTMAX : (iSum < MV_SHORTMIN ? MV_SHORTMIN : iSum);
#endif
		pOutData[i + iSkipTo] = (MV_FIXED_S)iSum;
	}
#else // MV_USE_NEON
	if (iMaskSize == 3)
	{
        //Rewriting(TEMP)
        int16x4_t kernel_vec[3];  //iMaskSize = 3
        for (j = 0; j < iMaskSize; j++) 
        {
            kernel_vec[j] = vdup_n_s16(0);
            kernel_vec[j] = vset_lane_s16(*(MV_FIXED_S*)(pMask + j * iMaskSize), kernel_vec[j], 0);
            kernel_vec[j] = vset_lane_s16(*(MV_FIXED_S*)(pMask + j * iMaskSize + 1), kernel_vec[j], 1);
            kernel_vec[j] = vset_lane_s16(*(MV_FIXED_S*)(pMask + j * iMaskSize + 2), kernel_vec[j], 2);
        }

        for (i = 0; i < iWidth - iMaskSize + 1; i++)
        {
            iSum = 0;
            int32x4_t sum_vec = vdupq_n_s32(0);
            for (j = 0; j < iMaskSize; j++)
            {
                int16x4_t input_vec = vdup_n_s16(0);
                input_vec = vset_lane_s16(*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i), input_vec, 0);
                input_vec = vset_lane_s16(*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + 1), input_vec, 1);
                input_vec = vset_lane_s16(*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + 2), input_vec, 2);
                sum_vec = vmlal_s16(sum_vec, kernel_vec[j], input_vec);
            }

            iSum += vgetq_lane_s32(sum_vec, 0);
            iSum += vgetq_lane_s32(sum_vec, 1);
            iSum += vgetq_lane_s32(sum_vec, 2);
            //iSum = Sat16(iSum, iShift);  //iShift is always 0, so comment out it here.
            pOutData[i + iSkipTo] = (MV_FIXED_S)iSum;
        }

	}
	else if (iMaskSize == 5)
	{
		for (i = 0; i < iWidth - iMaskSize + 1; i++)
		{
			iSum = 0;
			int32x4_t sum_vec = vdupq_n_s32(0);
			for (j = 0; j < iMaskSize; j++)
			{
				int16x4_t input_vec = vld1_s16((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i);
				int16x4_t kernel_vec = vld1_s16((MV_FIXED_S*)(pMask + j * iMaskSize));
				sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);

				input_vec = vdup_n_s16(0);
				kernel_vec = vdup_n_s16(0);
				vset_lane_s16(*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + 4), input_vec, 0);
				vset_lane_s16(*(MV_FIXED_S*)(pMask + j * iMaskSize + 4), kernel_vec, 0);
				sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);
			}

			iSum += vgetq_lane_s32(sum_vec, 0);
			iSum += vgetq_lane_s32(sum_vec, 1);
			iSum += vgetq_lane_s32(sum_vec, 2);
			iSum += vgetq_lane_s32(sum_vec, 3);
			iSum = Sat16(iSum, iShift);
			pOutData[i + iSkipTo] = (MV_FIXED_S)iSum;
		}
	}
	else if (iMaskSize == 11)
	{
		for (i = 0; i < iWidth - iMaskSize + 1; i++)
		{
			iSum = 0;
			int32x4_t sum_vec = vdupq_n_s32(0);
			for (j = 0; j < iMaskSize; j++)
			{
				int16x4_t input_vec = vld1_s16((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i);
				int16x4_t kernel_vec = vld1_s16((MV_FIXED_S*)(pMask + j * iMaskSize));
				sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);

				input_vec = vld1_s16((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + 4);
				kernel_vec = vld1_s16((MV_FIXED_S*)(pMask + j * iMaskSize + 4));
				sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);

				input_vec = vdup_n_s16(0);
				kernel_vec = vdup_n_s16(0);
				vset_lane_s16(*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + 8), input_vec, 0);
				vset_lane_s16(*(MV_FIXED_S*)(pMask + j * iMaskSize + 8), kernel_vec, 0);
				vset_lane_s16(*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + 9), input_vec, 1);
				vset_lane_s16(*(MV_FIXED_S*)(pMask + j * iMaskSize + 9), kernel_vec, 1);
				vset_lane_s16(*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + 10), input_vec, 2);
				vset_lane_s16(*(MV_FIXED_S*)(pMask + j * iMaskSize + 10), kernel_vec, 2);
				sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);
			}

			iSum += vgetq_lane_s32(sum_vec, 0);
			iSum += vgetq_lane_s32(sum_vec, 1);
			iSum += vgetq_lane_s32(sum_vec, 2);
			iSum += vgetq_lane_s32(sum_vec, 3);
			iSum = Sat16(iSum, iShift);
			pOutData[i + iSkipTo] = (MV_FIXED_S)iSum;
		}
	}
	else
	{
		for (i = 0; i < iWidth - iMaskSize + 1; i++)
		{
			iSum = 0;
			for (j = 0; j < iMaskSize; j++)
			{
				int32x4_t sum_vec = vdupq_n_s32(0);
				MV_FIXED_S *input_addr = (MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i;
				MV_FIXED_S *mask_addr = (MV_FIXED_S*)(pMask + j * iMaskSize);
				for (k = 0; k < iMaskSize / 4; k++)
				{
					int16x4_t input_vec = vld1_s16(input_addr + k * 4);
					int16x4_t kernel_vec = vld1_s16(mask_addr + k * 4);
					sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);
				}

				iSum += vgetq_lane_s32(sum_vec, 0);
				iSum += vgetq_lane_s32(sum_vec, 1);
				iSum += vgetq_lane_s32(sum_vec, 2);
				iSum += vgetq_lane_s32(sum_vec, 3);

				if (iMaskSize & 3)
				{
					for (k = iMaskSize - (iMaskSize & 3); k < iMaskSize; k++)
					{
						MV_FIXED_S k1 = (*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + k));
						MV_FIXED_S k2 = pMask[(j * iMaskSize + k)];
						iSum += (MV_FIXED)k1 * k2;
					}
	        	}
			}

			iSum = Sat16(iSum, iShift);
			pOutData[i + iSkipTo] = (MV_FIXED_S)iSum;
		}
	}
#endif
}

// for unit matrix convolution, directly to pMatDst
VOID MV_Convolution_Mid(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
						SHORT iMaskSize, SHORT iShift)
{
	// ASSERT(pMatMask->iRows == pMatMask->iCols == iMaskSize);
	// ASSERT(iMaskSize <= pMatSrc->iCols <= pMatDst->iCols);
	// ASSERT(pMatSrc->iRows <= pMatDst->iRows);

	INT i = 0;
	MV_FIXED_S *pLineSrc = NULL, *pLineDst = NULL;
	INT iJumpLine = iMaskSize / 2;

	if (!pMatSrc || !pMatDst || !pMatMask) {
		return;
	}

	if ((pMatSrc->iCols < iMaskSize) || (pMatSrc->iRows < iMaskSize) || 
		(pMatDst->iCols < pMatSrc->iCols) || (pMatDst->iRows < pMatSrc->iRows) ||
		(pMatMask->iCols != pMatMask->iRows || pMatMask->iCols != iMaskSize)) {
		return;
	}

	//MV_MatrixCopy2_S(pMatSrc, pMatDst);
	//MV_MatrixToIntAll_S(pMatSrc);

	pLineSrc = MV_MatrixGetLinePtr_S(pMatSrc, 0, 0);
	pLineDst = MV_MatrixGetLinePtr_S(pMatDst, iJumpLine, 0);
	for (i = 0; i < pMatSrc->iRows - iJumpLine * 2; i++)
	{
#ifdef C64X
		if (iMaskSize == 3)
		{
			IMG_conv_3x3_i16s_c16s(pLineSrc, pLineDst, pMatSrc->iCols, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
		if (iMaskSize == 5)
		{
			IMG_conv_5x5_i16s_c16s(pLineSrc, pLineDst, pMatSrc->iCols, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
		else if (iMaskSize == 7)
		{
			IMG_conv_7x7_i16s_c16s(pLineSrc, pLineDst, pMatSrc->iCols, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
		else if (iMaskSize == 11)
		{
			IMG_conv_11x11_i16s_c16s(pLineSrc, pLineDst, pMatSrc->iCols, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
#else
		MV_Convolution_Int(pLineSrc, pLineDst, pMatSrc->iCols, 
			pMatSrc->iPitch, MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iMaskSize, iShift);
#endif

		// clear tail to 0
		MV_Memset(pLineDst + (pMatSrc->iCols - iMaskSize + 1), 
			0, (iMaskSize - 1) * sizeof(MV_FIXED_S));

		pLineSrc += (pMatSrc->iPitch / sizeof(MV_FIXED_S));
		pLineDst += (pMatDst->iPitch / sizeof(MV_FIXED_S));
	}

	//MV_MatrixToFixedAll_S(pMatSrc);
}

// for unit matrix convolution, directly to pMatDst
VOID MV_Convolution_MidB(MV_MATRIX_S *pMatSrc, SHORT iX, SHORT iY, SHORT iWidth, SHORT iHeight, 
						 MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift)
{
	// ASSERT(pMatMask->iRows == pMatMask->iCols == iMaskSize);
	// ASSERT(iMaskSize <= pMatSrc->iCols <= pMatDst->iCols);
	// ASSERT(pMatSrc->iRows <= pMatDst->iRows);

	INT i = 0;
	MV_FIXED_S *pLineSrc = NULL, *pLineDst = NULL;
	INT iJumpLine = iMaskSize / 2;
	INT iEndHeight = 0;

	if (!pMatSrc || !pMatDst || !pMatMask) {
		return;
	}

	if ((iX < 0) ||(iY < 0) || (iX + iWidth > pMatSrc->iCols)
		|| (pMatMask->iCols != pMatMask->iRows || pMatMask->iCols != iMaskSize)
		|| (iMaskSize > pMatSrc->iRows) || (iMaskSize > pMatSrc->iCols) 
		|| (pMatDst->iCols < pMatSrc->iCols) || (pMatDst->iRows < pMatSrc->iRows)) {
		return;
	}

	//MV_MatrixCopy2_S(pMatSrc, pMatDst);
	//MV_MatrixToIntAll_S(pMatSrc);

	iEndHeight = MV_MIN((iY + iHeight), (pMatSrc->iRows - iMaskSize + 1));
	iEndHeight -= iY;

	pLineSrc = MV_MatrixGetLinePtr_S(pMatSrc, iY, iX);
	pLineDst = MV_MatrixGetLinePtr_S(pMatDst, iY + iJumpLine, iX);
	for (i = 0; i < iEndHeight; i++)
	{
#ifdef C64X
		if (iMaskSize == 3)
		{
			IMG_conv_3x3_i16s_c16s(pLineSrc, pLineDst, iWidth, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
		if (iMaskSize == 5)
		{
			IMG_conv_5x5_i16s_c16s(pLineSrc, pLineDst, iWidth, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
		else if (iMaskSize == 7)
		{
			IMG_conv_7x7_i16s_c16s(pLineSrc, pLineDst, iWidth, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
		else if (iMaskSize == 11)
		{
			IMG_conv_11x11_i16s_c16s(pLineSrc, pLineDst, iWidth, 
				pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);
		}
#else
		MV_Convolution_Int(pLineSrc, pLineDst, iWidth, 
			pMatSrc->iPitch, MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iMaskSize, iShift);
#endif

		// clear tail to 0
		MV_Memset(pLineDst + (iWidth - iMaskSize + 1), 
			0, (iMaskSize - 1) * sizeof(MV_FIXED_S));

		pLineSrc += (pMatSrc->iPitch / sizeof(MV_FIXED_S));
		pLineDst += (pMatDst->iPitch / sizeof(MV_FIXED_S));
	}

	//MV_MatrixToFixedAll_S(pMatSrc);
}

// calc unit mask, and write result to iUnitMaskNumxiUnitMaskNum buffer
VOID MV_Convolution_MidInt(CONST MV_FIXED_S *pInData, MV_FIXED_S *pOutData, 
						   SHORT iWidth, SHORT iPitchSrc, SHORT iPitchDst, CONST MV_MATRIX_S *pMask, 
						   MV_MATRIX_S *pUnitMask, SHORT iMaskSize, SHORT iUnitMaskSize, SHORT iShift)
{
	INT i = 0, j = 0, k = 0, l = 0;
	CONST MV_FIXED_S *pLineSrc = NULL, *pLineMatrix = NULL;
	MV_FIXED_S *pLineDst = NULL;
	INT iUnitMaskNum = 0;

	if (!pInData || !pOutData || !pMask || !pUnitMask) {
		return;
	}

	if ((pMask->iCols != pMask->iRows || pMask->iCols != iMaskSize) ||
		(pUnitMask->iCols != pUnitMask->iRows || pUnitMask->iCols != iUnitMaskSize)
		|| (0 == iUnitMaskSize)) {
		return;
	}

	iUnitMaskNum = iMaskSize / iUnitMaskSize;

	pLineSrc = pInData;
	pLineDst = pOutData;

	for (i = 0; i < iUnitMaskNum; i++)
	{
		for (j = 0; j < iUnitMaskNum; j++)
		{
			// prepare unit mask
			for (k = 0; k < iUnitMaskSize; k++)
			{
				for (l = 0; l < iUnitMaskSize; l++)
				{
					MV_MatrixSet_S(pUnitMask, k, l, 
						MV_MatrixGet_S((LPMV_MATRIX_S)pMask, i * iUnitMaskSize + k, j * iUnitMaskSize + l));
				}
			}

			pLineMatrix = MV_MatrixGetLinePtr_S(pUnitMask, 0, 0);
#ifdef C64X
			if (iMaskSize == 3)
			{
				IMG_conv_3x3_i16s_c16s(pLineSrc, pLineDst, iWidth, 
					iPitchSrc / sizeof(MV_FIXED_S), pLineMatrix, iShift);
			}
			else if (iUnitMaskSize == 5)
			{
				IMG_conv_5x5_i16s_c16s(pLineSrc, pLineDst, iWidth, 
					iPitchSrc / sizeof(MV_FIXED_S), pLineMatrix, iShift);
			}
			else if (iUnitMaskSize == 7)
			{
				IMG_conv_7x7_i16s_c16s(pLineSrc, pLineDst, iWidth, 
					iPitchSrc / sizeof(MV_FIXED_S), pLineMatrix, iShift);
			}
			else if (iUnitMaskSize == 11)
			{
				IMG_conv_11x11_i16s_c16s(pLineSrc, pLineDst, iWidth, 
					iPitchSrc / sizeof(MV_FIXED_S), pLineMatrix, iShift);
			}
#else
			MV_Convolution_Int(pLineSrc, pLineDst, iWidth, 
				iPitchSrc, pLineMatrix, iUnitMaskSize, iShift);
#endif

			pLineDst = (MV_FIXED_S*)((BYTE*)pLineDst + iPitchDst);
		}

		pLineSrc = (MV_FIXED_S*)((BYTE*)pLineSrc + iUnitMaskSize * iPitchSrc);
	}
}

// accumulate to final result
VOID MV_Convolution_MidComp(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
							SHORT iMaskSize, SHORT iUnitMaskSize, SHORT iShift)
{
	// ASSERT(pMatMask->iRows == pMatMask->iCols == iMaskSize);
	// ASSERT(iMaskSize <= pMatSrc->iCols <= pMatDst->iCols);
	// ASSERT(pMatSrc->iRows <= pMatDst->iRows);

	MV_FIXED_S iSum = 0;
	MV_FIXED_S *pLineSrc = NULL, *pLineDst = NULL;
	INT iUnitMaskNum = 0;
	INT iJumpLine = 0;
	INT i = 0, j = 0, k = 0;
	MV_FIXED_S *pBuffer = NULL, *pBufferAlign = NULL;
	MV_MATRIX_S *pUnitMask = NULL;
	INT iBuffSize = 0;
	INT iBuffPitch = 0;
	INT iSkipTo = 0;
	INT iSkipToUnit = 0;

	if (!pMatSrc || !pMatDst || !pMatMask) {
		return;
	}

	if ((pMatDst->iCols < pMatSrc->iCols || pMatDst->iRows < pMatSrc->iRows) ||
		(pMatMask->iCols != pMatMask->iRows || pMatMask->iCols != iMaskSize) ||
		(iMaskSize > pMatSrc->iCols || iMaskSize > pMatSrc->iRows) || 
		(iUnitMaskSize > iMaskSize) || (0 >= iUnitMaskSize)) {
		return;
	}

	iUnitMaskNum = iMaskSize / iUnitMaskSize;
	iJumpLine = iMaskSize / 2;


	//MV_MatrixCopy2_S(pMatSrc, pMatDst);
	//MV_MatrixToIntAll_S(pMatSrc);

	// create work buffer
	iBuffPitch = MV_ALIGN(pMatSrc->iCols * sizeof(MV_FIXED_S), 8);
	iBuffSize = iBuffPitch * iUnitMaskNum * iUnitMaskNum;
	pBuffer = (MV_FIXED_S*)MV_MallocAlign(iBuffSize, 8, (VOID**)&pBufferAlign);
	if (pBuffer == NULL)
	{
		return;
	}

	pUnitMask = MV_CreateMatrices_S(iUnitMaskSize, iUnitMaskSize, 1, 2, 1);
	if (pUnitMask == NULL)
	{
		MV_Free(pBuffer);
		return;
	}

	pLineSrc = MV_MatrixGetLinePtr_S(pMatSrc, 0, 0);
	pLineDst = MV_MatrixGetLinePtr_S(pMatDst, iJumpLine, 0);
	for (i = 0; i < pMatSrc->iRows - iJumpLine * 2; i++)
	{
		// get n line of mxm unit convolution result
		MV_Convolution_MidInt(pLineSrc, pBufferAlign, pMatSrc->iCols, 
			pMatSrc->iPitch, pMatDst->iPitch, pMatMask, pUnitMask, iMaskSize, iUnitMaskSize, iShift);

		// get sum from m lines of nxn convolution result buffer
		for (j = 0; j < pMatSrc->iCols - iMaskSize + 1; j++)
		{
			iSum = 0;
			for (k = 0; k < iUnitMaskNum * iUnitMaskNum; k++)
			{
				iSum += *((MV_FIXED_S*)((BYTE*)pBufferAlign + k * iBuffPitch)
					+ j + (k % iUnitMaskNum) + iSkipToUnit);
			}
			*(pLineDst + j + iSkipTo) = iSum;
		}

		// clear tail to 0
		MV_Memset(pLineDst + (pMatSrc->iCols - iMaskSize + 1), 0, (iMaskSize - 1) * sizeof(MV_FIXED_S));

		pLineSrc += (pMatSrc->iPitch / sizeof(MV_FIXED_S));
		pLineDst += (pMatDst->iPitch / sizeof(MV_FIXED_S));
	}

	//MV_MatrixToFixedAll_S(pMatSrc);
	MV_FreeMatrices_S(pUnitMask, 1);
	MV_Free(pBuffer);
}

// accumulate to final result
VOID MV_Convolution_MidCompB(MV_MATRIX_S *pMatSrc, SHORT iX, SHORT iY, SHORT iWidth, SHORT iHeight, 
							 MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
							 SHORT iMaskSize, SHORT iUnitMaskSize, SHORT iShift)
{
	// ASSERT(pMatMask->iRows == pMatMask->iCols == iMaskSize);
	// ASSERT(iMaskSize <= pMatSrc->iCols <= pMatDst->iCols);
	// ASSERT(pMatSrc->iRows <= pMatDst->iRows);

	MV_FIXED_S iSum = 0;
	MV_FIXED_S *pLineSrc = NULL, *pLineDst = NULL;
	INT iUnitMaskNum = 0;
	INT iJumpLine = 0;
	INT i = 0, j = 0, k = 0;
	MV_FIXED_S *pBuffer = NULL, *pBufferAlign = NULL;
	MV_MATRIX_S *pUnitMask = NULL;
	INT iBuffSize = 0;
	INT iBuffPitch = 0;
	INT iSkipTo = 0;
	INT iSkipToUnit = 0;
	INT iEndHeight = 0;

	if (!pMatSrc || !pMatDst || !pMatMask) {
		return;
	}

	if ((iX < 0) || (iY < 0) || (iX + iWidth > pMatMask->iCols)
		|| (pMatMask->iCols != pMatMask->iRows || pMatMask->iCols != iMaskSize) 
		|| (pMatDst->iCols < pMatSrc->iCols) || (0 >= iUnitMaskSize)
		|| (pMatDst->iRows < pMatSrc->iRows) || (iUnitMaskSize > iMaskSize)
		|| (iMaskSize > pMatSrc->iRows) || (iMaskSize > pMatSrc->iCols)) {
		return;
	}

	iEndHeight = MV_MIN((iY + iHeight), (pMatSrc->iRows - iMaskSize + 1));
	iEndHeight -= iY;

	iUnitMaskNum = iMaskSize / iUnitMaskSize;
	iJumpLine = iMaskSize / 2;

	//MV_MatrixCopy2_S(pMatSrc, pMatDst);
	//MV_MatrixToIntAll_S(pMatSrc);

	// create work buffer
	iBuffPitch = MV_ALIGN(pMatSrc->iCols * sizeof(MV_FIXED_S), 8);
	iBuffSize = iBuffPitch * iUnitMaskNum * iUnitMaskNum;
	pBuffer = (MV_FIXED_S*)MV_MallocAlign(iBuffSize, 8, (VOID**)&pBufferAlign);
	if (pBuffer == NULL)
	{
		return;
	}

	pUnitMask = MV_CreateMatrices_S(iUnitMaskSize, iUnitMaskSize, 1, 2, 1);
	if (pUnitMask == NULL)
	{
		MV_Free(pBuffer);
		return;
	}

	pLineSrc = MV_MatrixGetLinePtr_S(pMatSrc, iY, iX);
	pLineDst = MV_MatrixGetLinePtr_S(pMatDst, iY + iJumpLine, iX);
	for (i = 0; i < iEndHeight; i++)
	{
		// get n line of mxm unit convolution result
		MV_Convolution_MidInt(pLineSrc, pBufferAlign, iWidth, 
			pMatSrc->iPitch, pMatDst->iPitch, pMatMask, pUnitMask, iMaskSize, iUnitMaskSize, iShift);

		// get sum from m lines of nxn convolution result buffer
		for (j = 0; j < iWidth - iMaskSize + 1; j++)
		{
			iSum = 0;
			for (k = 0; k < iUnitMaskNum * iUnitMaskNum; k++)
			{
				iSum += *((MV_FIXED_S*)((BYTE*)pBufferAlign + k * iBuffPitch)
					+ j + (k % iUnitMaskNum) + iSkipToUnit);
			}
			*(pLineDst + j + iSkipTo) = iSum;
		}

		// clear tail to 0
		MV_Memset(pLineDst + (iWidth - iMaskSize + 1), 0, (iMaskSize - 1) * sizeof(MV_FIXED_S));

		pLineSrc += (pMatSrc->iPitch / sizeof(MV_FIXED_S));
		pLineDst += (pMatDst->iPitch / sizeof(MV_FIXED_S));
	}

	//MV_MatrixToFixedAll_S(pMatSrc);
	MV_FreeMatrices_S(pUnitMask, 1);
	MV_Free(pBuffer);
}

VOID MV_Convolution(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
					SHORT iMaskSize, SHORT iShift)
{
#ifdef C64X
	switch (iMaskSize)
	{
	case 3:
	case 5:
	case 7:
	case 11:
		MV_Convolution_Mid(pMatSrc, pMatDst, pMatMask, iMaskSize, iShift);
		break;
	case 15:
		MV_Convolution_MidComp(pMatSrc, pMatDst, pMatMask, 15, 5, iShift);
		break;
	case 21:
		MV_Convolution_MidComp(pMatSrc, pMatDst, pMatMask, 21, 7, iShift);
		break;
	case 25:
		MV_Convolution_MidComp(pMatSrc, pMatDst, pMatMask, 25, 5, iShift);
		break;
	default:
		// ASSERT(FALSE);
		break;
	}
#else
	MV_Convolution_Mid(pMatSrc, pMatDst, pMatMask, iMaskSize, iShift);
#endif
}

VOID MV_ConvolutionB(MV_MATRIX_S *pMatSrc, SHORT iX, SHORT iY, SHORT iWidth, SHORT iHeight, 
					 MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift)
{
#ifdef C64X
	switch (iMaskSize)
	{
	case 3:
	case 5:
	case 7:
	case 11:
		MV_Convolution_MidB(pMatSrc, iX, iY, iWidth, iHeight, pMatDst, pMatMask, iMaskSize, iShift);
		break;
	case 15:
		MV_Convolution_MidCompB(pMatSrc, iX, iY, iWidth, iHeight, pMatDst, pMatMask, 15, 5, iShift);
		break;
	case 21:
		MV_Convolution_MidCompB(pMatSrc, iX, iY, iWidth, iHeight, pMatDst, pMatMask, 21, 7, iShift);
		break;
	case 25:
		MV_Convolution_MidCompB(pMatSrc, iX, iY, iWidth, iHeight, pMatDst, pMatMask, 25, 5, iShift);
		break;
	default:
		// ASSERT(FALSE);
		break;
	}
#else
	MV_Convolution_MidB(pMatSrc, iX, iY, iWidth, iHeight, pMatDst, pMatMask, iMaskSize, iShift);
#endif
}

VOID MV_Smooth_Int(CONST MV_FIXED_S *pInData, MV_FIXED_S *pOutData, 
						SHORT iWidth, SHORT iPitch, CONST MV_FIXED_S *pMask, 
						SHORT iMaskSize, SHORT iShift, INT iDiv)
{
	INT i = 0, j = 0, k = 0, iSkipTo = 0;
	MV_FIXED iSum = 0;

	if((!pInData) || (!pOutData) || (!pMask) || (0 == iDiv)){
		return;
	}

	for (i = 0; i < iWidth - iMaskSize + 1; i++)
	{
		iSum = 0;
		for (j = 0; j < iMaskSize; j++)
		{
			for (k = 0; k < iMaskSize; k++)
			{
				MV_FIXED_S k1 = (*((MV_FIXED_S*)((BYTE*)pInData + j * iPitch) + i + k));
				MV_FIXED_S k2 = pMask[(j * iMaskSize + k)];
				iSum += (MV_FIXED)k1 * k2;
			}
		}

#ifndef MV_USE_FP
		iSum /= iDiv;
		iSum >>= iShift;
		iSum = (iSum > MV_SHORTMAX) ? MV_SHORTMAX : (iSum < MV_SHORTMIN ? MV_SHORTMIN : iSum);
#endif
		iSum /= iDiv;
		pOutData[i + iSkipTo] = (MV_FIXED_S)iSum;
	}
}

VOID MV_Smooth_Mid(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
				   SHORT iMaskSize, SHORT iShift, INT iDiv)
{
	// ASSERT(pMatMask->iRows == pMatMask->iCols == iMaskSize);
	// ASSERT(iMaskSize <= pMatSrc->iCols <= pMatDst->iCols);
	// ASSERT(pMatSrc->iRows <= pMatDst->iRows);

	INT i = 0;
	MV_FIXED_S *pLineSrc = NULL, *pLineDst = NULL;
	INT iJumpLine = iMaskSize / 2;

	if (!pMatSrc || !pMatDst || !pMatMask) {
		return;
	}

	if ((pMatDst->iCols < pMatSrc->iCols) || (pMatDst->iRows < pMatSrc->iRows)
		|| (pMatMask->iCols != pMatMask->iRows) || (pMatMask->iCols != iMaskSize)
		|| (iMaskSize > pMatSrc->iCols) || (iMaskSize > pMatSrc->iRows)) {
		return;
	}

	//MV_MatrixCopy2_S(pMatSrc, pMatDst);
	//MV_MatrixToIntAll_S(pMatSrc);

	pLineSrc = MV_MatrixGetLinePtr_S(pMatSrc, 0, 0);
	pLineDst = MV_MatrixGetLinePtr_S(pMatDst, iJumpLine, 0);
	for (i = 0; i < pMatSrc->iRows - iJumpLine * 2; i++)
	{
		MV_Smooth_Int(pLineSrc, pLineDst, pMatSrc->iCols, 
			pMatSrc->iPitch, MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iMaskSize, iShift, iDiv);

		// clear tail to 0
		MV_Memset(pLineDst + (pMatSrc->iCols - iMaskSize + 1), 
			0, (iMaskSize - 1) * sizeof(MV_FIXED_S));

		pLineSrc += (pMatSrc->iPitch / sizeof(MV_FIXED_S));
		pLineDst += (pMatDst->iPitch / sizeof(MV_FIXED_S));
	}

	//MV_MatrixToFixedAll_S(pMatSrc);
}

VOID MV_Smooth_MidB(MV_MATRIX_S *pMatSrc, SHORT iX, SHORT iY, SHORT iWidth, SHORT iHeight, 
					MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift, INT iDiv)
{
	// ASSERT(pMatMask->iRows == pMatMask->iCols == iMaskSize);
	// ASSERT(iMaskSize <= pMatSrc->iCols <= pMatDst->iCols);
	// ASSERT(pMatSrc->iRows <= pMatDst->iRows);

	INT i = 0;
	MV_FIXED_S *pLineSrc = NULL, *pLineDst = NULL;
	INT iJumpLine = iMaskSize / 2;
	INT iEndHeight = 0;

	if (!pMatSrc || !pMatDst || !pMatMask) {
		return;
	}

	if ((iX < 0) || (iY < 0) || (iX + iWidth > pMatSrc->iCols) 		
		|| (pMatMask->iCols != pMatMask->iRows || pMatMask->iCols != iMaskSize) 
		|| (pMatDst->iCols < pMatSrc->iCols) || (pMatDst->iRows < pMatSrc->iRows)
		|| (iMaskSize > pMatSrc->iCols) || (iMaskSize > pMatSrc->iRows)) {
		return;
	}

	iEndHeight = MV_MIN((iY + iHeight), (pMatSrc->iRows - iMaskSize + 1));
	iEndHeight -= iY;

	//MV_MatrixCopy2_S(pMatSrc, pMatDst);
	//MV_MatrixToIntAll_S(pMatSrc);

	pLineSrc = MV_MatrixGetLinePtr_S(pMatSrc, iY, iX);
	pLineDst = MV_MatrixGetLinePtr_S(pMatDst, iY + iJumpLine, iX);
	for (i = 0; i < iEndHeight; i++)
	{
		MV_Smooth_Int(pLineSrc, pLineDst, iWidth, 
			pMatSrc->iPitch, MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iMaskSize, iShift, iDiv);

		// clear tail to 0
		MV_Memset(pLineDst + (iWidth - iMaskSize + 1), 
			0, (iMaskSize - 1) * sizeof(MV_FIXED_S));

		pLineSrc += (pMatSrc->iPitch / sizeof(MV_FIXED_S));
		pLineDst += (pMatDst->iPitch / sizeof(MV_FIXED_S));
	}

	//MV_MatrixToFixedAll_S(pMatSrc);
}

VOID MV_Smooth(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
			   SHORT iMaskSize, SHORT iShift, INT iDiv)
{
	MV_Smooth_Mid(pMatSrc, pMatDst, pMatMask, iMaskSize, iShift, iDiv);
}

VOID MV_SmoothB(MV_MATRIX_S *pMatSrc, SHORT iX, SHORT iY, SHORT iWidth, SHORT iHeight, 
				MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift, INT iDiv)
{
	MV_Smooth_MidB(pMatSrc, iX, iY, iWidth, iHeight, pMatDst, pMatMask, iMaskSize, iShift, iDiv);
}

#define hough_cmp_gt(l1, l2) (aux[l1] > aux[l2])
VOID MV_HoughSortDescent32s(INT *array, INT total, const INT *aux)
{
    INT isort_thresh = 7;
    INT t = 0;
    INT sp = 0;
    struct
    {
        INT *lb;
        INT *ub;
    }
    stack[48];
    aux = aux;

	if( (total <= 1) || (!array) ) {
        return;
	}

	stack[0].lb = array;
    stack[0].ub = array + (total - 1);

    while (sp >= 0)
    {
        INT* left = stack[sp].lb;
        INT* right = stack[sp--].ub;

        for(;;)
        {
            int i, n = (int)((right - left) + 1), m;
            INT *ptr;
            INT *ptr2;

            if (n <= isort_thresh)
            {
            insert_sort:
                for( ptr = left + 1; ptr <= right; ptr++ )
                {
                    for( ptr2 = ptr; ptr2 > left && hough_cmp_gt(ptr2[0],ptr2[-1]); ptr2--)
                        MV_SWAP( ptr2[0], ptr2[-1], t );
                }
                break;
            }
            else
            {
                INT* left0;
                INT* left1;
                INT* right0;
                INT* right1;
                INT* pivot;
                INT* a;
                INT* b;
                INT* c;
                int swap_cnt = 0;

                left0 = left;
                right0 = right;
                pivot = left + (n/2);

                if( n > 40 )
                {
                    int d = n / 8;
                    a = left, b = left + d, c = left + 2*d;
                    left = hough_cmp_gt(*a, *b) ? (hough_cmp_gt(*b, *c) ? b : (hough_cmp_gt(*a, *c) ? c : a))
                                      : (hough_cmp_gt(*c, *b) ? b : (hough_cmp_gt(*a, *c) ? a : c));

                    a = pivot - d, b = pivot, c = pivot + d;
                    pivot = hough_cmp_gt(*a, *b) ? (hough_cmp_gt(*b, *c) ? b : (hough_cmp_gt(*a, *c) ? c : a))
                                      : (hough_cmp_gt(*c, *b) ? b : (hough_cmp_gt(*a, *c) ? a : c));

                    a = right - 2*d, b = right - d, c = right;
                    right = hough_cmp_gt(*a, *b) ? (hough_cmp_gt(*b, *c) ? b : (hough_cmp_gt(*a, *c) ? c : a))
                                      : (hough_cmp_gt(*c, *b) ? b : (hough_cmp_gt(*a, *c) ? a : c));
                }

                a = left, b = pivot, c = right;
                pivot = hough_cmp_gt(*a, *b) ? (hough_cmp_gt(*b, *c) ? b : (hough_cmp_gt(*a, *c) ? c : a))
                                   : (hough_cmp_gt(*c, *b) ? b : (hough_cmp_gt(*a, *c) ? a : c));
                if( pivot != left0 )
                {
                    MV_SWAP( *pivot, *left0, t );
                    pivot = left0;
                }
                left = left1 = left0 + 1;
                right = right1 = right0;

                for(;;)
                {
                    while( left <= right && !hough_cmp_gt(*pivot, *left) )
                    {
                        if( !hough_cmp_gt(*left, *pivot) )
                        {
                            if( left > left1 )
                                MV_SWAP( *left1, *left, t );
                            swap_cnt = 1;
                            left1++;
                        }
                        left++;
                    }

                    while( left <= right && !hough_cmp_gt(*right, *pivot) )
                    {
                        if( !hough_cmp_gt(*pivot, *right) )
                        {
                            if( right < right1 )
                                MV_SWAP( *right1, *right, t );
                            swap_cnt = 1;
                            right1--;
                        }
                        right--;
                    }

                    if( left > right )
                        break;
                    MV_SWAP( *left, *right, t );
                    swap_cnt = 1;
                    left++;
                    right--;
                }

                if( swap_cnt == 0 )
                {
                    left = left0, right = right0;
                    goto insert_sort;
                }

                n = MV_MIN( (INT)(left1 - left0), (INT)(left - left1) );
                for( i = 0; i < n; i++ )
                    MV_SWAP( left0[i], left[i-n], t );

                n = MV_MIN( (INT)(right0 - right1), (INT)(right1 - right) );
                for( i = 0; i < n; i++ )
                    MV_SWAP( left[i], right0[i-n+1], t );
                n = (int)(left - left1);
                m = (int)(right1 - right);
                if( n > 1 )
                {
                    if( m > 1 )
                    {
                        if( n > m )
                        {
                            stack[++sp].lb = left0;
                            stack[sp].ub = left0 + n - 1;
                            left = right0 - m + 1, right = right0;
                        }
                        else
                        {
                            stack[++sp].lb = right0 - m + 1;
                            stack[sp].ub = right0;
                            left = left0, right = left0 + n - 1;
                        }
                    }
                    else
                        left = left0, right = left0 + n - 1;
                }
                else if( m > 1 )
                    left = right0 - m + 1, right = right0;
                else
                    break;
            }
        }
    }
}

BOOL MV_HoughLinesStandardB(LPMV_MATRIX_S pMatIn, CONST HOUGH_REGION *pRoi, MV_FIXED fxrho, 
									MV_FIXED fxTheta, INT threshold, INT iLineMax, LPMV_MATRIX pMatRes, 
									INT *pNum, VOID *pWorkBuffer, INT iBuffSize, 
									INT iValidAngleSize, MV_FIXED fMinAngle)
{
	INT *accum = NULL;
	INT *sort_buf = NULL;
	INT numangle = 0, numrho = 0;
	INT total = 0;
	INT iMinAng = 0;
	MV_FIXED fxang = 0, fxtmp = 0;
	INT r = 0, n = 0, i = 0, j = 0;
	INT idx = 0, rIdx = 0, nIdx = 0;
	INT base = 0;
	BYTE *pBuffer = NULL;
	INT iAccuBufSize = 0;

	if (!pMatIn || !pRoi || !pMatRes || !pNum || !pWorkBuffer) {
		return FALSE;
	}

	if ((pRoi->iX + pRoi->iWidth > pMatIn->iCols) || (pRoi->iY + pRoi->iHeight > pMatIn->iRows)
		|| (0 == fxTheta) || (0 == fxrho)) {
		return FALSE;
	}

	numangle = iValidAngleSize;
	iMinAng = MV_IntFromFixed(MV_Div(fMinAngle, fxTheta)) + 1;
	numrho = MV_IntFromFixed(MV_Div(MV_FixedFromInt((pRoi->iWidth + pRoi->iHeight) * 2 + 1), fxrho));

	pBuffer = (BYTE*)pWorkBuffer;
	iAccuBufSize = sizeof(INT) * (numangle + 2) * (numrho + 2);

	if (iAccuBufSize > iBuffSize) {
		return FALSE;
	}
	MV_Memset(pBuffer, 0, iAccuBufSize);

	accum = (INT*)pBuffer;
	sort_buf = (INT*)(pBuffer + iAccuBufSize);

	// fill accumulator
	for (i = 0; i < pRoi->iHeight; i++)
	{
		for (j = 0; j < pRoi->iWidth; j++)
		{
			if (MV_MatrixGet_S(pMatIn, i + pRoi->iY, j + pRoi->iX) != 0)
			{
				for (n = 0; n < numangle; n++)
				{
					rIdx = MV_Mul(j, fxtabCos[n + iMinAng + 1]) + MV_Mul(i, fxtabSin[n + iMinAng + 1]);
					rIdx += (numrho - 1) / 2;
					accum[(n + 1) * (numrho + 2) + rIdx + 1]++;
				}
			}
		}
	}

	// find local maximums
	for (r = 0; r < numrho; r++)
	{
		for (n = 0; n < numangle; n++)
		{
			base = (n + 1) * (numrho + 2) + r + 1;
			if (accum[base] > threshold &&
				accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
				accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2])
			{
				sort_buf[total++] = base;
			}
		}
	}

	// sort
	MV_HoughSortDescent32s(sort_buf, total, accum);

	// create line storage
	iLineMax = MV_MIN(iLineMax, total);

	// store first x min
	for (i = 0; i < iLineMax; i++)
	{
		idx = sort_buf[i];
		nIdx = MV_IntFromFixed_Floor(MV_Div(idx, numrho + 2)) - 1;
		rIdx = idx - (nIdx + 1) * (numrho + 2) - 1;

		fxtmp = MV_Mul((MV_FixedFromInt(rIdx) - (MV_FixedFromInt(numrho - 1) / 2)), fxrho); // rho
		fxang = MV_Mul(MV_FixedFromInt(nIdx), fxTheta) + fMinAngle; // angle
		MV_MatrixSet(pMatRes, i, 6, fxtmp);
		MV_MatrixSet(pMatRes, i, 7, fxang);
	}

	*pNum = iLineMax;
	return TRUE;
}

BOOL MV_HoughLinesStandardC(INT* piCoordianteData, INT iDataLength, CONST HOUGH_REGION *pRoi, MV_FIXED fxrho, 
							MV_FIXED fxTheta, INT threshold, INT iLineMax, LPMV_MATRIX pMatRes, 
							INT *pNum, VOID *pWorkBuffer, INT iBuffSize, 
							INT iValidAngleSize, MV_FIXED fMinAngle)
{
	INT *accum = NULL;
	INT *sort_buf = NULL;
	INT numangle = 0, numrho = 0;
	INT total = 0;
	INT iMinAng = 0;
	MV_FIXED fxang = 0, fxtmp = 0;
	INT r = 0, n = 0, i = 0;
	INT idx = 0, rIdx = 0, nIdx = 0;
	INT base = 0;
	BYTE *pBuffer = NULL;
	INT iAccuBufSize = 0;
	INT iX = 0, iY = 0;

	if (!piCoordianteData || !pRoi || !pMatRes || !pNum || !pWorkBuffer) {
		return FALSE;
	}

	if (0 == fxTheta || 0 == fxrho){
		return FALSE;
	}

	numangle = iValidAngleSize;
	iMinAng = MV_IntFromFixed(MV_Div(fMinAngle, fxTheta)) + 1;
	numrho = MV_IntFromFixed(MV_Div(MV_FixedFromInt((pRoi->iWidth + pRoi->iHeight) * 2 + 1), fxrho));

	pBuffer = (BYTE*)pWorkBuffer;
	iAccuBufSize = sizeof(INT) * (numangle + 2) * (numrho + 2);

	if (iAccuBufSize > iBuffSize) {
		return FALSE;
	}
	MV_Memset(pBuffer, 0, iAccuBufSize);

	accum = (INT*)pBuffer;
	sort_buf = (INT*)(pBuffer + iAccuBufSize);

	// fill accumulator
	for (i = 0; i < iDataLength; i++)
	{
		iX = piCoordianteData[i * 2];
		iY = piCoordianteData[i * 2 + 1];

		for (n = 0; n < numangle; n++)
		{
			rIdx = MV_Mul(iX, fxtabCos[n + iMinAng + 1]) + MV_Mul(iY, fxtabSin[n + iMinAng + 1]);
			rIdx += (numrho - 1) / 2;
			accum[(n + 1) * (numrho + 2) + rIdx + 1]++;
		}
	}

	// find local maximums
	for (r = 0; r < numrho; r++)
	{
		for (n = 0; n < numangle; n++)
		{
			base = (n + 1) * (numrho + 2) + r + 1;
			if (accum[base] > threshold &&
				accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
				accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2])
			{
				sort_buf[total++] = base;
			}
		}
	}

	// sort
	MV_HoughSortDescent32s(sort_buf, total, accum);

	// create line storage
	iLineMax = MV_MIN(iLineMax, total);

	// store first x min
	for (i = 0; i < iLineMax; i++)
	{
		idx = sort_buf[i];
		nIdx = MV_IntFromFixed_Floor(MV_Div(idx, numrho + 2)) - 1;
		rIdx = idx - (nIdx + 1) * (numrho + 2) - 1;

		fxtmp = MV_Mul((MV_FixedFromInt(rIdx) - (MV_FixedFromInt(numrho - 1) / 2)), fxrho); // rho
		fxang = MV_Mul(MV_FixedFromInt(nIdx), fxTheta) + fMinAngle; // angle
		MV_MatrixSet(pMatRes, i, 6, fxtmp);
		MV_MatrixSet(pMatRes, i, 7, fxang);
	}

	*pNum = iLineMax;
	return TRUE;
}

// buffer size assure outside
// width must be at least 4
VOID MV_ComputeAccumGraph(LPMV_MATRIX_S pMatSrc, INT *piarrDst, INT iOffX, INT iOffY, 
						  INT iWidth, INT iHeight)
{
#ifndef MV_USE_NEON //Simulator
	//=========Data definition=========//
	INT iCount = 0;
	INT iCount2 = 0;
	INT iRow = 0;
	INT iCol = 0;
	INT iLength = iWidth * iHeight;

	if ((NULL == pMatSrc) || (NULL == piarrDst)) {
		return;
	}

	if ((iOffX + iWidth > pMatSrc->iCols) ||
		(iOffY + iHeight > pMatSrc->iRows)) {
		return;
	}

	//==========Implementation==========//
	//Copy data
	iCount = 0;
	for (iRow = iOffY; iRow < iOffY + iHeight; iRow++)
	{
		for (iCol = iOffX; iCol < iOffX + iWidth; iCol++)
		{
			piarrDst[iCount] = MV_MatrixGet_S(pMatSrc, iRow, iCol);
			iCount++;
		}
	}

	//Accumulate row data
	for (iCount = 0; iCount < iLength - iWidth; iCount++)
	{
		piarrDst[iCount + iWidth] += piarrDst[iCount];
	}

	//Accumulate column data
	for (iCount2 = 0; iCount2 < iWidth - 1; iCount2++)
	{
		for (iCount = iCount2; iCount < iLength - 1; iCount+=iWidth)
		{
			piarrDst[iCount + 1] += piarrDst[iCount];
		}
	}
#else
	INT i = 0, j = 0;
	INT iLength = iWidth * iHeight;

	// ASSERT(iOffX >= 0 && iOffY >= 0);
	// ASSERT(iWidth + iOffX <= pMatSrc->iCols);
	// ASSERT(iHeight + iOffY <= pMatSrc->iRow);

	if ((iWidth & 3) == 0)
	{
		BYTE *pLineSrc = (BYTE*)pMatSrc->pDataAlign + iOffY * pMatSrc->iPitch;
		INT *pLineDst1 = piarrDst;
		INT *pLineDst2 = piarrDst + iWidth;
		int32x4_t vec32_1, vec32_2;
		int16x4_t vec16_1, vec16_2;
		INT iSum = 0;

		for (j = 0; j < iWidth; j += 4)
		{
			vec16_1 = vld1_s16((MV_FIXED_S*)pLineSrc + iOffX + j);
			vec32_1 = vmovl_s16(vec16_1);
			vst1q_s32(pLineDst1 + j, vec32_1);
		}
		pLineSrc += pMatSrc->iPitch;

		for (i = 0; i < iHeight - 1; i++)
		{
			for (j = 0; j < iWidth; j += 4)
			{
				vec16_1 = vld1_s16((MV_FIXED_S*)pLineSrc + iOffX + j);
				vec32_1 = vld1q_s32(pLineDst1 + j);
				vst1q_s32(pLineDst2 + j, vaddw_s16(vec32_1, vec16_1));
			}

			pLineSrc += pMatSrc->iPitch;
			pLineDst1 += iWidth;
			pLineDst2 += iWidth;
		}

		pLineDst1 = piarrDst;
		for (i = 0; i < iHeight; i++)
		{
			iSum = pLineDst1[0] + pLineDst1[1];
			pLineDst1[1] = iSum;
			iSum += pLineDst1[2];
			pLineDst1[2] = iSum;
			iSum += pLineDst1[3];
			pLineDst1[3] = iSum;
			for (j = 4; j < iWidth; j += 4)
			{
				iSum += pLineDst1[j];
				pLineDst1[j] = iSum;
				iSum += pLineDst1[j + 1];
				pLineDst1[j + 1] = iSum;
				iSum += pLineDst1[j + 2];
				pLineDst1[j + 2] = iSum;
				iSum += pLineDst1[j + 3];
				pLineDst1[j + 3] = iSum;
			}

			pLineDst1 += iWidth;
		}
	}
	else
	{
		INT iCount = 0;
		for (i = iOffY; i < iOffY + iHeight; i++)
		{
			for (j = iOffX; j < iOffX + iWidth; j++)
			{
				piarrDst[iCount++] = MV_MatrixGet_S(pMatSrc, i, j);
			}
		}

		for (i = 0; i < iLength - iWidth; i++)
		{
			piarrDst[i + iWidth] += piarrDst[i];
		}

		for (i = 0; i < iWidth - 1; i++)
		{
			for (j = i; j < iLength - 1; j += iWidth)
			{
				piarrDst[j + 1] += piarrDst[j];
			}
		}
	}
#endif
}

VOID IMG_mad_16x16_hand(CONST UCHAR *refImg, CONST UCHAR *srcImg, INT pitch, INT sx, INT sy, UINT *match)
{
	INT i = 0, j = 0, x = 0, y = 0, matx = 0, maty = 0;
	UINT matpos = 0, matval = 0;
	UINT acc = 0;

	matval = ~0U;
	matx = maty = 0;

	if (!refImg || !srcImg || !match) {
		return;
	}

	for (x = 0; x < sx; x++)
	{
		for (y = 0; y < sy; y++)
		{
			for (i = 0; i < 16; i++)
			{
				for (j = 0; j < 16; j++)
				{
					acc += abs(srcImg[i*16 + j] - refImg[(i+y)*pitch + x + j]);
				}
			}

			if (acc < matval)
			{
				matval = acc;
				matx = x;
				maty = y;
			}
			acc = 0;
		}

		matpos = (0xffff0000 & (matx << 16)) | (0x0000ffff & maty);
		match[0] = matpos;
		match[1] = matval;
	}
}

VOID MV_MAD_16X16(MV_MATRIX_B *pMatSrc, MV_MATRIX_B *pMatMask, INT iX, INT iY, 
				  INT iWidth, INT iHeight, UINT *piMatch)
{
	MV_FIXED_B *pLineSrc = MV_MatrixGetLinePtr_B(pMatSrc, iY);
	MV_FIXED_B *pLineMask = MV_MatrixGetLinePtr_B(pMatMask, 0);
	UINT match[2];
	INT iEndWidth = 0;
	INT iEndHeight = 0;
	
	if ((NULL == pMatSrc) || (NULL == pMatMask) || (NULL == piMatch)) {
		return;
	}

	if ((iX + iWidth > pMatSrc->iCols)  || (iY + iHeight > pMatSrc->iRows)
		|| (pMatMask->iCols != pMatMask->iRows) || (pMatMask->iCols != 16)) {
		return;
	}

	iEndWidth = MV_MIN((iX + iWidth), (pMatSrc->iCols - pMatMask->iCols + 1));
	iEndWidth -= iX;
	iEndHeight = MV_MIN((iY + iHeight), (pMatSrc->iRows - pMatMask->iCols + 1));
	iEndHeight -= iY;

#ifdef C64X
	IMG_mad_16x16(pLineSrc + iX, pLineMask, pMatSrc->iPitch, iEndWidth, iEndHeight, match);
#else
	IMG_mad_16x16_hand(pLineSrc + iX, pLineMask, pMatSrc->iPitch, iEndWidth, iEndHeight, match);
#endif
	piMatch[0] = ((match[0] >> 16) & 0xffff);
	piMatch[1] = (match[0] & 0xffff);
	piMatch[2] = match[1];
}
