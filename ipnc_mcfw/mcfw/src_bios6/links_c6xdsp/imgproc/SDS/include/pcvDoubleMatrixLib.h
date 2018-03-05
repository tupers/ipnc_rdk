
/*
* @(#)$Id: pcvDoubleMatrixLib.h 1007 2012-12-19 03:13:32Z LvGangXiao $ 
* @(#) Declaration file of  Double Matrix Lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/

#ifndef PCVDOUBLEMATRIXLIB_H
#define PCVDOUBLEMATRIXLIB_H


#define PCV_DM_EPSILON (0.0000001f)
#define PCV_ITERATION_NUMBER 60

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcvDoubleVectorLib.h"

typedef struct _MV_MATRIX_D_
{
	INT iRows;
	INT iCols;
	INT iLength;
	DOUBLE* pdData;
}MV_MATRIX_D,*LPMV_MATRIX_D;

typedef enum _MATRIX_DIM
{
	MATRIX_DIM_COLUMN = 1,
	MATRIX_DIM_ROW,
} MATRIX_DIM;

#ifdef __cplusplus
extern "C" {
#endif

//矩阵求和
INT MV_MatrixSum_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//矩阵求最大值
INT MV_MatrixMax_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//矩阵求最小值
INT MV_MatrixMin_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//矩阵排序
INT MV_MatrixSort_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//矩阵翻转
INT MV_MatrixFliplr_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD);

//矩阵求平均值
INT MV_MatrixMean_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//矩阵求中位数
INT MV_MatrixMedian_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//矩阵的标准差
DOUBLE MV_MatrixSTD_D(LPMV_MATRIX_D psMat,INT iflag);

//矩阵相减
INT MV_MatrixSub_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//矩阵相加
INT MV_MatrixAdd_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//矩阵点除
INT MV_MatrixDiv_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//矩阵点乘
INT MV_MatrixDotMul_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//矩阵乘法
INT MV_MatrixMul_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//矩阵转置
INT MV_MatrixTrans_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD);

//矩阵求平方根
INT MV_MatrixSqrt_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D pMatDr,LPMV_MATRIX_D pMatDi);

//获取矩阵某一行或一列作为一个向量
INT MV_GetMatricesRoworCol_D(LPMV_MATRIX_D psMat,LPMV_VECTOR_D psOutput,INT iIndex,MATRIX_DIM iDim);

//矩阵某行或列转为vector
INT MV_MatrixToVector_D(LPMV_MATRIX_D psMat,INT iIndex,MATRIX_DIM iDim,LPMV_VECTOR_D psOutput);

//向量转为矩阵
INT MV_VectorToMatrix_D(LPMV_VECTOR_D psVec,MATRIX_DIM iDim,LPMV_MATRIX_D psOutput);

//矩阵按行或按列求范数
DOUBLE MV_MatrixNorm2_D(LPMV_MATRIX_D psMat,INT iIndex,MATRIX_DIM iDim);

//求实对称矩阵的特征值和特征向量
//input:psMat
//output:psMat:对角线为特征值;psEigenVector:特征向量
INT MV_MatrixEigen_D(LPMV_MATRIX_D psMat,LPMV_MATRIX_D psEigenVector);

//矩阵求范数
DOUBLE MV_MatrixNorm_D(LPMV_MATRIX_D psMat);

//求逆矩阵
INT MV_MatrixInv_D(LPMV_MATRIX_D psMat);

//奇异值分解
//input：
//   LPMV_MATRIX_D pmat(mxn)
//output：
//   LPMV_MATRIX_D pmat(mxn):对角线给出奇异值，其余元素为0
//   LPMV_MATRIX_D psMatL(mxm):存放左奇异向量
//   LPMV_MATRIX_D psMatR(nxn):存放右奇异向量
INT MV_MatrixSVD_D(LPMV_MATRIX_D psMat,LPMV_MATRIX_D psMatL,LPMV_MATRIX_D psMatR);

//广义逆矩阵
INT MV_MatrixGeneralInv_D(LPMV_MATRIX_D psMatI,LPMV_MATRIX_D psMatO);


//求矩阵行列式的值
DOUBLE MV_Matrixdet_D(LPMV_MATRIX_D psMat);

//生成零矩阵（值全为0）
LPMV_MATRIX_D MV_MatricesZeros_D(INT iRows,INT iCols,INT iNum);

//生成单位矩阵（值全为1）
LPMV_MATRIX_D MV_MatricesOnes_D(INT iRows,INT iCols,INT iNum);

//释放矩阵
void MV_FreeMatrices_D(LPMV_MATRIX_D psMatrics,INT iNum);

DOUBLE MV_MatrixGet_D(const LPMV_MATRIX_D psMatrix, INT iRow, INT iCol);

void MV_MatrixSet_D(LPMV_MATRIX_D psMatrix, INT iRow, INT iCol, DOUBLE dData);

DOUBLE* MV_MatrixGetLinePtr_D(LPMV_MATRIX_D psMatrix, INT iRow);

LPMV_MATRIX_D MV_MatrixCopy_D(LPMV_MATRIX_D psSrc);

void MV_MatrixCopy2_D(LPMV_MATRIX_D psSrc, LPMV_MATRIX_D psDst);

void MV_PrintMatrix_D(LPMV_MATRIX_D psMat);

INT lsqnonlin(void (*func)(DOUBLE *pdP, DOUBLE *pdF, INT iM, INT iN, DOUBLE *pdXdata1, DOUBLE *pdXdata2),
			  LPMV_MATRIX_D psMatH, LPMV_VECTOR_D psVecydata,LPMV_MATRIX_D psMatx, LPMV_MATRIX_D psMatxp);

#ifdef __cplusplus
}
#endif


#endif /* PCVDOUBLEMATRIXLIB_H*/
