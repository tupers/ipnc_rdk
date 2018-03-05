
/*
* @(#)$Id: pcvDoubleVectorLib.h 1007 2012-12-19 03:13:32Z LvGangXiao $ 
* @(#) Declaration file of  Double Vector Lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/
#ifndef PCVDOUBLEVECTORLIB_H
#define PCVDOUBLEVECTORLIB_H

#include "pcvPlatformdef.h"

#define PCV_DM_EPSILON (0.0000001f)
#define PCV_ITERATION_NUMBER 60

typedef struct _MV_VECTOR_D_
{
	INT     iLength;
	DOUBLE  *pdData;
} MV_VECTOR_D, *LPMV_VECTOR_D;

//-----------------------------------------------------------------/
// 标定运算
//-----------------------------------------------------------------/
/// <summary>
/// 复数结构体
/// </summary>
/// <param name="Real">实部</param>
/// <param name="Imag">虚部</param>
typedef struct _ComplexD
{
	DOUBLE Real;
	DOUBLE Imag;
}ComplexD;


#ifdef __cplusplus
extern "C" {
#endif

//向量求和
DOUBLE MV_VectorSum_D(LPMV_VECTOR_D psVec);

//向量求最大值
DOUBLE MV_VectorMax_D(LPMV_VECTOR_D psVec,INT *iIndex);

//向量求最小值
DOUBLE MV_VectorMin_D(LPMV_VECTOR_D psVec,INT *piIndex);

//向量排序
INT MV_VectorSort_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecD);

//向量翻转
INT MV_VectorFliplr_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecD);

//向量求平均值
DOUBLE MV_VectorMean_D(LPMV_VECTOR_D psVec);

//向量求中位数
DOUBLE MV_VectorMedian_D(LPMV_VECTOR_D psVec);

//向量标准差
DOUBLE MV_VectorSTD_D(LPMV_VECTOR_D psVec,INT iFlag);

//向量相减
INT MV_VectorSub_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//向量相加
INT MV_VectorAdd_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//向量点除
INT MV_VectorDiv_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//向量点乘
INT MV_VectorDotMul_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//向量求平方根
INT MV_VectorSqrt_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecDr,LPMV_VECTOR_D psVecDi);

//QR方程根
INT MV_VectorQRRoots_D(LPMV_VECTOR_D psVector,LPMV_VECTOR_D psRRoots,LPMV_VECTOR_D psIRoots);

////QR方程根2
INT MV_VectorQRRoots2_D(DOUBLE* pdData, INT iNVarl, ComplexD* roots, INT* piRn);

//多项式估计运算
INT MV_VectorPolyval_D(LPMV_VECTOR_D psLPVector,LPMV_VECTOR_D psLPInput,LPMV_VECTOR_D psLPOutput);

//多项式估计运算2
INT MV_VectorPolyval2_D(DOUBLE *pdLPVector,DOUBLE *pdLPInput,DOUBLE *pdLPOutput,INT iLength);

//向量2范数
DOUBLE MV_VectorNorm_D(LPMV_VECTOR_D psLPVectorD);

//生成零向量（值全为0）
LPMV_VECTOR_D MV_VectorsZeros_D(INT iLength,INT iNum);

//生成单位向量（值全为1）
LPMV_VECTOR_D MV_VectorsOnes_D(INT iLength,INT iNum);

//释放向量
void MV_FreeVectors_D(LPMV_VECTOR_D psLPVectorD,INT iNum);

DOUBLE MV_VectorGet_D(const LPMV_VECTOR_D pVector, INT iIndex);

void MV_VectorSet_D(LPMV_VECTOR_D psVector, INT iIndex, DOUBLE dData);

LPMV_VECTOR_D MV_VectorCopy_D(LPMV_VECTOR_D psSrc);

void MV_VectorCopy2_D(LPMV_VECTOR_D psSrc, LPMV_VECTOR_D psDst);

void MV_PrintVector_D(LPMV_VECTOR_D psVec);

#ifdef __cplusplus
}
#endif


#endif /* PCVDOUBLEMATRIXLIB_H*/
