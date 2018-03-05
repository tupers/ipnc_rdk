/*
* @(#)$Id: pcvDoubleVectorLib.c 1007 2012-12-19 03:13:32Z LvGangXiao $ 
* @(#) Implementation file of  Double Vector Lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/
#include "pcvDoubleVectorLib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>


DOUBLE MV_VectorSum_D(LPMV_VECTOR_D psVec)
{
	INT iIVal = 0;
	DOUBLE dSum = 0;
	DOUBLE *pdData = NULL;
	
	if(!psVec)
	{
		return(-1);
	}

	pdData = psVec->pdData;

	for (iIVal = 0;iIVal < psVec->iLength;iIVal ++)
	{
		dSum += (*pdData);
		pdData++;
	}

	return dSum;
}

//向量求最大值
DOUBLE MV_VectorMax_D(LPMV_VECTOR_D psVec,INT *piIndex)
{
	INT iIVarl = 0;
	DOUBLE dMax = 0;
	DOUBLE *pdData = NULL;

	if(!psVec)
	{
		return(-1);
	}

	pdData = psVec->pdData;
	dMax = *pdData;
	pdData ++;
	*piIndex = 0;

	for (iIVarl = 1;iIVarl < psVec->iLength;iIVarl ++)
	{
		if (dMax < *pdData)
		{
			dMax= *pdData;
			*piIndex = iIVarl;
		}
		pdData++;
	}

	return dMax;

}

//向量最小值
DOUBLE MV_VectorMin_D(LPMV_VECTOR_D psVec,INT *piIndex)
{
	INT iIVarl = 0;
	DOUBLE* pdData = NULL;
	DOUBLE dMin = 0;

	if(!psVec)
	{
		return(-1);
	}

	pdData = psVec->pdData;
	dMin = *pdData;
	pdData ++;

	for (iIVarl = 1;iIVarl < psVec->iLength;iIVarl ++)
	{
		if (dMin > (*pdData))
		{
			dMin = *pdData;
			*piIndex = iIVarl;
		}
		pdData++;
	}

	return dMin;
}
//冒泡排序
static INT MV_BubbleSort_D(DOUBLE *pdInput,DOUBLE *pdOutput,INT iN)
{
	INT iIVarl = 0;
	INT iJVarl = 0;
	INT iFlag = 0;
	DOUBLE dTemp = 0;

	if (!pdInput||!pdOutput||iN <= 0)
	{
		return(-1);
	}

	memcpy(pdOutput,pdInput,iN*sizeof(DOUBLE));

	for(iIVarl = 0;iIVarl < iN - 1 ;iIVarl++)
	{
		iFlag = 0;
		for(iJVarl = iN-1;iJVarl > iIVarl;iJVarl--)
		{
			if(pdOutput[iJVarl] < pdOutput[iJVarl-1])
			{
				dTemp = pdOutput[iJVarl];
				pdOutput[iJVarl] = pdOutput[iJVarl-1];
				pdOutput[iJVarl-1] = dTemp;

				iFlag = 1;
			}

		}
		if (iFlag == 0)
		{
			break;
		}
	}
	return(0);
}

//向量排序
INT MV_VectorSort_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecD)
{
	if(!psVecS||!psVecD)
	{
		return(-1);
	}
	
	if (psVecS->iLength != psVecD->iLength)
	{
		return(-1);
	}

	MV_BubbleSort_D(psVecS->pdData,psVecD->pdData,psVecS->iLength);

	return(0);

}

//矩阵翻转
INT MV_VectorFliplr_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecD)
{
	INT iIVarl = 0;
	DOUBLE *pdDataSrc = NULL;
	DOUBLE *pdDataDst = NULL;
	INT iMaxIndex = 0;

	if(!psVecS||!psVecD)
	{
		return(-1);
	}

	if(psVecS->iLength != psVecD->iLength)
	{
		return(-1);
	}

	pdDataSrc = psVecS->pdData;
	pdDataDst = psVecD->pdData;
	iMaxIndex = psVecS->iLength - 1;

	for (iIVarl = 0;iIVarl < psVecS->iLength;iIVarl ++)
	{
		pdDataSrc[iIVarl] = pdDataDst[iMaxIndex - iIVarl];
	}

	return(0);

}

//向量求平均值
DOUBLE MV_VectorMean_D(LPMV_VECTOR_D psVec)
{
	INT iIVarl = 0;
	DOUBLE *pdData = NULL;
	DOUBLE dMean = 0;

	if(!psVec)
	{
		return(-1);
	}

	pdData = psVec->pdData;

	for (iIVarl = 0;iIVarl < psVec->iLength;iIVarl++)
	{
				dMean += (*pdData);
				pdData++;
	}
	dMean = dMean/psVec->iLength;

	return dMean;
}

//矩阵求中位数
DOUBLE MV_VectorMedian_D(LPMV_VECTOR_D psVec)
{
	LPMV_VECTOR_D psSortVec = NULL;
	INT iMean = 0;
	DOUBLE dMedian = 0;
	DOUBLE *pdData = NULL;

	if(!psVec)
	{
		return(-1);
	}

	psSortVec = MV_VectorsZeros_D(psVec->iLength,1);

	if (!psSortVec)
	{
		return(-1);
	}

	if(MV_VectorSort_D(psVec,psSortVec) < 0)
	{
		MV_FreeVectors_D(psSortVec,1);
		return(-1);
	}
		
	iMean = psVec->iLength/2;
	pdData = psSortVec->pdData;

	if (psVec->iLength%2 == 0)
	{
		dMedian = (pdData[iMean - 1] + pdData[iMean])/2;
	}
	else
	{
		dMedian = pdData[iMean];
	}

	MV_FreeVectors_D(psSortVec,1);

	return dMedian;
}

//向量的标准差
DOUBLE MV_VectorSTD_D(LPMV_VECTOR_D psVec,INT iFlag)
{
	DOUBLE *pdA = NULL;
	INT iLength = 0;
	DOUBLE dSum = 0.0;
	DOUBLE dMean = 0.0;
	DOUBLE dVal = 0.0;
	INT iIVarl = 0;

	if (!psVec)
	{
		return 0;
	}
	iLength = psVec->iLength;
	pdA = psVec->pdData;

	if (0 == iLength)
	{
		return 0;
	}

	for (iIVarl = 0;iIVarl < iLength;iIVarl++)
	{
		dSum += pdA[iIVarl];
	}
	dMean = dSum/iLength;
	dSum = 0.0;
	for (iIVarl = 0;iIVarl < iLength;iIVarl++)
	{
		dVal = pdA[iIVarl] - dMean;
		dSum += dVal*dVal;
	}
	if (0 == iFlag&&iLength != 1)
	{
		return sqrt(dSum/(iLength-1));
	}		
	else
	{
		return sqrt(dSum/(iLength));
	}
}


//矩阵相减
INT MV_VectorSub_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC)
{
	INT iIVarl = 0;
	DOUBLE *pdDataA = NULL;
	DOUBLE *pdDataB = NULL;
	DOUBLE *pdDataC = NULL;

	if (!psVecA||!psVecB||!psVecC)
	{
		return(-1);
	}

	if (psVecA->iLength != psVecB->iLength||psVecA->iLength != psVecC->iLength)
	{
		return(-1);
	}

	pdDataA = psVecA->pdData;
	pdDataB = psVecB->pdData;
	pdDataC = psVecC->pdData;

	for( iIVarl = 0; iIVarl < psVecA->iLength; iIVarl++)
	{
		(*pdDataC) = (*pdDataA) - (*pdDataB);
		pdDataA++;
		pdDataB++;
		pdDataC++;
	}

	return(0);

}

//向量相加
INT MV_VectorAdd_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC)
{
	INT iIVarl = 0;
	DOUBLE *pdDataA = NULL;
	DOUBLE *pdDataB = NULL;
	DOUBLE *pdDataC = NULL;

	if (!psVecA||!psVecB||!psVecC)
	{
		return(-1);
	}

	if (psVecA->iLength != psVecB->iLength||psVecA->iLength != psVecC->iLength)
	{
		return(-1);
	}

	pdDataA = psVecA->pdData;
	pdDataB = psVecB->pdData;
	pdDataC = psVecC->pdData;

	for( iIVarl = 0; iIVarl < psVecA->iLength; iIVarl++)
	{
		(*pdDataC) = (*pdDataA) + (*pdDataB);
		pdDataA++;
		pdDataB++;
		pdDataC++;
	}

	return(0);


}

//向量点除
INT MV_VectorDiv_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC)
{
	INT iIVarl = 0;
	DOUBLE *pdDataA = NULL;
	DOUBLE *pdDataB = NULL;
	DOUBLE *pdDataC = NULL;

	if (!psVecA||!psVecB||!psVecC)
	{
		return(-1);
	}

	if (psVecA->iLength != psVecB->iLength||psVecA->iLength != psVecC->iLength)
	{
		return(-1);
	}

	pdDataA = psVecA->pdData;
	pdDataB = psVecB->pdData;
	pdDataC = psVecC->pdData;


	for( iIVarl = 0; iIVarl < psVecA->iLength; iIVarl++)
	{
		if (0 == *pdDataB)
		{
			if (*pdDataA > 0)
			{
				(*pdDataC) = exp(1000);
			}
			else if (*pdDataA < 0)
			{
				(*pdDataC) = -exp(1000);
			}
			else
			{
				return(-1);
			}
		}
		else
		{
			(*pdDataC) = (*pdDataA)/(*pdDataB);
		}
			
		pdDataA++;
		pdDataB++;
		pdDataC++;
	}

	return(0);
}

//向量点乘
INT MV_VectorDotMul_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC)
{ 
	INT iIVarl = 0;
	DOUBLE *pdDataA = NULL;
	DOUBLE *pdDataB = NULL;
	DOUBLE *pdDataC = NULL;

	if (!psVecA||!psVecB||!psVecC)
	{
		return(-1);
	}

	if (psVecA->iLength != psVecB->iLength||psVecA->iLength != psVecC->iLength)
	{
		return(-1);
	}

	pdDataA = psVecA->pdData;
	pdDataB = psVecB->pdData;
	pdDataC = psVecC->pdData;


	for( iIVarl = 0; iIVarl < psVecA->iLength; iIVarl++)
	{
		(*pdDataC) = (*pdDataA)*(*pdDataB);
		pdDataA++;
		pdDataB++;
		pdDataC++;
	}

	return(0);
}

//向量求平方根
INT MV_VectorSqrt_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecDr,LPMV_VECTOR_D psVecDi)
{
	INT iIndex = 0;
	DOUBLE *pdDatas = NULL;
	DOUBLE *pdDatadr = NULL;
	DOUBLE *pdDatadi = NULL;

	if (!psVecS||!psVecDr||!psVecDi)
	{
		return(-1);
	}
	if (psVecS->iLength != psVecDr->iLength||psVecS->iLength != psVecDi->iLength)
	{
		return(-1);
	}

	pdDatas = psVecS->pdData;
	pdDatadr = psVecDr->pdData;
	pdDatadi = psVecDi->pdData;

	for (iIndex = 0;iIndex < psVecS->iLength;iIndex ++)
	{
		DOUBLE fVar = *pdDatas;
		if(*pdDatas < 0)
		{
			fVar = -fVar;
			*pdDatadi = sqrt(fVar);
		}
		else
		{
			*pdDatadr = sqrt(fVar);
		}
		pdDatas++;
		pdDatadr++;
		pdDatadi++;
	}
	return(0);
}

//QR分解
static INT QRDecomposition(DOUBLE* pdA,INT iN,DOUBLE* pdU,DOUBLE* pdV,DOUBLE dEps,INT iJt)
{ 
	INT iM,iIt,iIVarl,iJVarl,iKVarl,iLVarl,iIi,iJj,iKk,iLl;
	DOUBLE iB,iC,iW,iG,iXy,iP,iQ,iR,iX,iS,iE,iFVarl,iZ,iY;
	iIt=0;
	iM=iN;
	while (iM!=0)
	{ 
		iLVarl=iM-1;
		while ((iLVarl>0)&&(fabs(pdA[iLVarl*iN+iLVarl-1])>dEps*(fabs(pdA[(iLVarl-1)*iN+iLVarl-1])+fabs(pdA[iLVarl*iN+iLVarl])))) iLVarl=iLVarl-1;
		iIi=(iM-1)*iN+iM-1;
		iJj=(iM-1)*iN+iM-2;
		iKk=(iM-2)*iN+iM-1;
		iLl=(iM-2)*iN+iM-2;
		if (iLVarl==iM-1)
		{
			pdU[iM-1]=pdA[(iM-1)*iN+iM-1];
			pdV[iM-1]=0.0;
			iM=iM-1; 
			iIt=0;
		}
		else if (iLVarl==iM-2)
		{ 
			iB=-(pdA[iIi]+pdA[iLl]);
			iC=pdA[iIi]*pdA[iLl]-pdA[iJj]*pdA[iKk];
			iW=iB*iB-4.0*iC;
			iY=sqrt(fabs(iW));
			if (iW>0.0)
			{ 
				iXy=1.0;
				if (iB<0.0)
				{
					iXy=-1.0;
				}
				pdU[iM-1]=(-iB-iXy*iY)/2.0;
				pdU[iM-2]=iC/pdU[iM-1];
				pdV[iM-1]=0.0; pdV[iM-2]=0.0;
			}
			else
			{
				pdU[iM-1]=-iB/2.0; 
				pdU[iM-2]=pdU[iM-1];
				pdV[iM-1]=iY/2.0; 
				pdV[iM-2]=-pdV[iM-1];
			}
			iM=iM-2; 
			iIt=0;
		}
		else
		{
			if (iIt>=iJt)
			{ 
				//printf("fail\iN");
				return(-1);
			}
			iIt=iIt+1;
			for (iJVarl=iLVarl+2; iJVarl<=iM-1; iJVarl++)
			{
				pdA[iJVarl*iN+iJVarl-2]=0.0;
			}
			for (iJVarl=iLVarl+3; iJVarl<=iM-1; iJVarl++)
			{
				pdA[iJVarl*iN+iJVarl-3]=0.0;
			}
			for (iKVarl=iLVarl; iKVarl<=iM-2; iKVarl++)
			{ 
				if (iKVarl!=iLVarl)
				{
					iP=pdA[iKVarl*iN+iKVarl-1];
					iQ=pdA[(iKVarl+1)*iN+iKVarl-1];
					iR=0.0;
					if (iKVarl!=iM-2)
					{
						iR=pdA[(iKVarl+2)*iN+iKVarl-1];
					}
				}
				else
				{
					iX=pdA[iIi]+pdA[iLl];
					iY=pdA[iLl]*pdA[iIi]-pdA[iKk]*pdA[iJj];
					iIi=iLVarl*iN+iLVarl; 
					iJj=iLVarl*iN+iLVarl+1;
					iKk=(iLVarl+1)*iN+iLVarl;
					iLl=(iLVarl+1)*iN+iLVarl+1;
					iP=pdA[iIi]*(pdA[iIi]-iX)+pdA[iJj]*pdA[iKk]+iY;
					iQ=pdA[iKk]*(pdA[iIi]+pdA[iLl]-iX);
					iR=pdA[iKk]*pdA[(iLVarl+2)*iN+iLVarl+1];
				}
				if ((fabs(iP)+fabs(iQ)+fabs(iR))!=0.0)
				{ 
					iXy=1.0;
					if (iP<0.0)
					{
						iXy=-1.0;
					}
					iS=iXy*sqrt(iP*iP+iQ*iQ+iR*iR);
					if (iKVarl!=iLVarl)
					{
						pdA[iKVarl*iN+iKVarl-1]=-iS;
					}
					iE=-iQ/iS; 
					iFVarl=-iR/iS; 
					iX=-iP/iS;
					iY=-iX-iFVarl*iR/(iP+iS);
					iG=iE*iR/(iP+iS);
					iZ=-iX-iE*iQ/(iP+iS);
					for (iJVarl=iKVarl; iJVarl<=iM-1; iJVarl++)
					{ 
						iIi=iKVarl*iN+iJVarl;
						iJj=(iKVarl+1)*iN+iJVarl;
						iP=iX*pdA[iIi]+iE*pdA[iJj];
						iQ=iE*pdA[iIi]+iY*pdA[iJj];
						iR=iFVarl*pdA[iIi]+iG*pdA[iJj];
						if (iKVarl!=iM-2)
						{
							iKk=(iKVarl+2)*iN+iJVarl;
							iP=iP+iFVarl*pdA[iKk];
							iQ=iQ+iG*pdA[iKk];
							iR=iR+iZ*pdA[iKk];
							pdA[iKk]=iR;
						}
						pdA[iJj]=iQ; 
						pdA[iIi]=iP;
					}
					iJVarl=iKVarl+3;
					if (iJVarl>=iM-1)
					{
						iJVarl=iM-1;
					}
					for (iIVarl=iLVarl; iIVarl<=iJVarl; iIVarl++)
					{
						iIi=iIVarl*iN+iKVarl;
						iJj=iIVarl*iN+iKVarl+1;
						iP=iX*pdA[iIi]+iE*pdA[iJj];
						iQ=iE*pdA[iIi]+iY*pdA[iJj];
						iR=iFVarl*pdA[iIi]+iG*pdA[iJj];
						if (iKVarl!=iM-2)
						{
							iKk=iIVarl*iN+iKVarl+2;
							iP=iP+iFVarl*pdA[iKk];
							iQ=iQ+iG*pdA[iKk];
							iR=iR+iZ*pdA[iKk];
							pdA[iKk]=iR;
						}
						pdA[iJj]=iQ;
						pdA[iIi]=iP;
					}
				}
			}
		}
	}
	return(0);
}

INT MV_VectorQRRoots_D(LPMV_VECTOR_D psVector,LPMV_VECTOR_D psRRoots,LPMV_VECTOR_D psIRoots)
{
	DOUBLE *pdA = NULL;
	INT iN = 0;
	DOUBLE *pdXr = NULL;
	DOUBLE *pdXi = NULL;
	const DOUBLE eps = PCV_DM_EPSILON;
	const INT jt = PCV_ITERATION_NUMBER;
	INT iIVarl = 0,iJVarl = 0;
	DOUBLE *pdQ = NULL;
	DOUBLE *pdA1 = NULL;

	if (!psVector||!psRRoots||!psIRoots)
	{
		return(-1);
	}
	
	iN = psVector->iLength - 1;

	if (iN <= 0||psRRoots->iLength < iN||psIRoots->iLength < iN)
	{
		return(-1);
	}
	pdA = psVector->pdData;
	pdXr = psRRoots->pdData;
	pdXi = psIRoots->pdData;

	pdA1 = (DOUBLE*)malloc((iN+1)*sizeof(DOUBLE));

	if (!pdA1)
	{
		return(-1);
	}

	for (iIVarl = 0;iIVarl <= iN/2;iIVarl++)
	{
		pdA1[iIVarl] = pdA[iN - iIVarl];
		pdA1[iN - iIVarl] = pdA[iIVarl];
	}
	
	pdQ = (DOUBLE*)malloc(iN*iN*sizeof(DOUBLE));

	if (!pdQ)
	{
		free(pdA1);
		return(-1);
	}
	
	for (iJVarl=0; iJVarl<=iN-1; iJVarl++)
	{
		pdQ[iJVarl]=-pdA1[iN-iJVarl-1]/pdA1[iN];
	}

	for (iJVarl=iN; iJVarl<=iN*iN-1; iJVarl++)
	{
		pdQ[iJVarl]=0.0;
	}

	for (iIVarl=0; iIVarl<=iN-2; iIVarl++)
	{
		pdQ[(iIVarl+1)*iN+iIVarl]=1.0;
	}

	iIVarl = QRDecomposition(pdQ,iN,pdXr,pdXi,eps,jt);

	free(pdQ);
	free(pdA1);
	
	return(iIVarl);
}

/// <summary>
/// 生成单位矩阵（对角线上为1，其余为0）
/// 高级功能：指定tran后，将生成的单位矩阵上移或下移tran行
/// 例如：Diag(data, 2, -1)生成矩阵{{0,0,0},{1,0,0},{0,1,0}}
/// </summary>
/// <param name="data">【出参】生成的对角阵，大小为N*N,其中N=n+abs(tran)</param>
/// <param name="n">基本对角阵的行、列数</param>
/// <param name="tran">平移量，正数为向上平移，负数为向下平移</param>
static void Diag(DOUBLE* pdData, INT iNVarl, INT iTran)
{
	INT iTranAbs = abs(iTran);
	INT iNn = abs(iNVarl) + iTranAbs;
	INT iIVarl = 0;
	memset(pdData,0,iNn*iNn*sizeof(DOUBLE));
	if (iTran == 0)
	{
		for (iIVarl = 0; iIVarl < iNVarl; iIVarl++)
		{
			pdData[iIVarl*iNn + iIVarl] = 1;
		}
	}
	else if (iTran < 0)
	{
		for (iIVarl = 0; iIVarl < iNVarl; iIVarl++)
		{
			pdData[(iIVarl + iTranAbs)*iNn + iIVarl] = 1;
		}
	}
	else
	{
		for (iIVarl = 0; iIVarl < iNVarl; iIVarl++)
		{
			pdData[iIVarl*iNn + (iIVarl + iTranAbs)] = 1;
		}
	}

	return;
}

/// <summary>
/// 求多项式方程的根
/// 例如求 s^3 - 6* s^2 -72* s - 27 = 0 的根，则data = [1 -6 -72 -27]
/// </summary>
/// <param name="data">多项式的系数，个数为n</param>
/// <param name="n">多项式的系数个数</param>
/// <param name="roots">【出参】求得的根，用复数结构体表示，个数为rn</param>
/// <param name="rn">【出参】根的个数</param>
/// <returns></returns>
INT MV_VectorQRRoots2_D(DOUBLE* pdData, INT iNVarl, ComplexD* roots, INT* piRn)
{
	const DOUBLE eps = 0.000000000001;
	const INT jt = 100;
	INT* piNz = NULL;
	INT iNzCount = 0;
	INT iIVarl = 0;
	DOUBLE *pdCc = NULL;
	INT iCcCount = 0;
	DOUBLE *pdD = 0;
	INT iRn = 0;

	*piRn = 0;

	piNz = (INT*)malloc(iNVarl*sizeof(INT));
	iNzCount = 0;
	for (iIVarl = 0; iIVarl < iNVarl; iIVarl++)
	{
		if (pdData[iIVarl] != 0) //如果元素不为0，则入队
		{
			piNz[iNzCount]=iIVarl;
			iNzCount++;
		}
	}
	if (iNzCount == 0) //所有元素为0
	{
		free(piNz);
		return(-1);
	}

	//头部的0无用，剔除之
	//尾部的0也去掉，但是记住它们为在0点的根
	pdCc = (DOUBLE*)malloc(iNVarl*sizeof(DOUBLE));
	iCcCount = 0;
	for (iIVarl = piNz[0]; iIVarl <= piNz[iNzCount - 1]; iIVarl++)
	{
		pdCc[iCcCount] = pdData[iIVarl];
		iCcCount++;
	}
	for (iIVarl = 0; iIVarl < iNVarl - piNz[iNzCount - 1] - 1; iIVarl++)
	{
		ComplexD complex;
		complex.Real = 0.0;
		complex.Imag = 0.0;
		roots[iRn] = complex;
		iRn++;
	}

	pdD = (DOUBLE*)malloc((iCcCount - 1)*sizeof(DOUBLE));
	for (iIVarl = 0; iIVarl < iCcCount - 1; iIVarl++)
	{
		pdD[iIVarl] = pdCc[iIVarl + 1]/pdCc[0];
	}

	//通过伴随矩阵求多项式的根
	iNVarl = iCcCount;
	if (iNVarl > 1)
	{
		DOUBLE* pdA = NULL;
		DOUBLE* pdU = NULL;
		DOUBLE* pdV = NULL;

		pdA = (DOUBLE*)malloc((abs(iNVarl - 2) + abs(-1))*(abs(iNVarl - 2) + abs(-1))*sizeof(DOUBLE));
		Diag(pdA, iNVarl - 2, -1);
		for (iIVarl = 0; iIVarl < iNVarl - 1; iIVarl++)
		{
			pdA[iIVarl] = -pdD[iIVarl];
		}
		pdU = (DOUBLE*)malloc((iNVarl - 1)*sizeof(DOUBLE));
		pdV = (DOUBLE*)malloc((iNVarl - 1)*sizeof(DOUBLE));
		QRDecomposition(pdA, iNVarl - 1, pdU, pdV,eps,jt);
		for (iIVarl = 0; iIVarl < iNVarl - 1; iIVarl++)
		{
			ComplexD complex;
			complex.Real = pdU[iIVarl];
			complex.Imag = pdV[iIVarl];
			roots[iRn] = complex;
			iRn++;
		}
		free(pdA);
		free(pdU);
		free(pdV);
	}

	free(piNz);
	free(pdCc);
	free(pdD);

	*piRn = iRn;

	return(0);
}

//多项式估计运算
INT MV_VectorPolyval_D(LPMV_VECTOR_D psLPVector,LPMV_VECTOR_D psLPInput,LPMV_VECTOR_D psLPOutput)
{
	INT iIVarl = 0;
	INT iJVarl = 0;
	DOUBLE dInputValue = 0;
	DOUBLE dOutputValue = 0;
	DOUBLE dRatio = 1.0;

	if (!psLPVector||!psLPInput||!psLPOutput)
	{
		return(-1);
	}

	if (psLPInput->iLength != psLPOutput->iLength)
	{
		return(-1);
	}

	for (iIVarl = 0;iIVarl < psLPInput->iLength;iIVarl++)
	{
		dOutputValue = 0;
		dRatio = 1.0;
		dInputValue = psLPInput->pdData[iIVarl];

		for (iJVarl = psLPVector->iLength - 1;iJVarl >=0 ;iJVarl--)
		{
			dOutputValue += psLPVector->pdData[iJVarl]*dRatio;
			dRatio *=  dInputValue;
		}

		psLPOutput->pdData[iIVarl] = dOutputValue;

	}
	return(0);
}


//多项式估计运算2
INT MV_VectorPolyval2_D(DOUBLE *pdLPVector,DOUBLE *pdLPInput,DOUBLE *pdLPOutput,INT iLength)
{
	INT iIVarl = 0;
	INT iJVarl = 0;
	DOUBLE dInputValue = 0;
	DOUBLE dOutputValue = 0;
	DOUBLE dRatio = 1.0;

	if (!pdLPVector||!pdLPInput||!pdLPOutput)
	{
		return(-1);
	}

	for (iIVarl = 0;iIVarl < iLength;iIVarl++)
	{
		dOutputValue = 0;
		dRatio = 1.0;
		dInputValue = pdLPInput[iIVarl];

		for (iJVarl = iLength - 1;iJVarl >=0 ;iJVarl--)
		{
			dOutputValue += pdLPVector[iJVarl]*dRatio;
			dRatio *=  dInputValue;
		}

		pdLPOutput[iIVarl] = dOutputValue;

	}
	return(0);

}

//向量2范数
DOUBLE MV_VectorNorm_D(LPMV_VECTOR_D psLPVectorD)
{
	INT iNVarl = psLPVectorD->iLength;
	DOUBLE* pdVectPtr =  psLPVectorD->pdData;
	DOUBLE dRes = 0.0;
	INT iIVarl;
	for (iIVarl = 0;iIVarl < iNVarl;iIVarl++)
	{
		dRes += (*pdVectPtr)*(*pdVectPtr);
		pdVectPtr++;
	}
	return sqrt(dRes);
}

LPMV_VECTOR_D MV_VectorsZeros_D(INT iLength,INT iNum)
{
	LPMV_VECTOR_D psLPVectorD = NULL;
	INT iIVarl = 0;

	if(0 == iLength)
	{
		return NULL;
	}

	psLPVectorD = (LPMV_VECTOR_D)malloc(iNum*sizeof(MV_VECTOR_D));

	if (!psLPVectorD)
	{
		return NULL;
	}

	for (iIVarl = 0;iIVarl< iNum;iIVarl++)
	{
		psLPVectorD[iIVarl].iLength = iLength;
		psLPVectorD[iIVarl].pdData = (DOUBLE*)malloc(psLPVectorD[iIVarl].iLength*sizeof(DOUBLE));
		memset(psLPVectorD[iIVarl].pdData,0,psLPVectorD[iIVarl].iLength*sizeof(DOUBLE));
	}

	return psLPVectorD;
}

LPMV_VECTOR_D MV_VectorsOnes_D(INT iLength,INT iNum)
{
	LPMV_VECTOR_D psLPVectorD = NULL;
	DOUBLE *pdDataPtr = NULL;
	INT iIVarl = 0;
	INT iJVarl = 0;

	if(0 == iLength)
	{
		return NULL;
	}

	psLPVectorD = (LPMV_VECTOR_D)malloc(iNum*sizeof(MV_VECTOR_D));

	if (!psLPVectorD)
	{
		return NULL;
	}

	for (iIVarl = 0;iIVarl< iNum;iIVarl++)
	{
		psLPVectorD[iIVarl].iLength = iLength;
		psLPVectorD[iIVarl].pdData = (DOUBLE*)malloc(psLPVectorD[iIVarl].iLength*sizeof(DOUBLE));

		pdDataPtr = psLPVectorD[iIVarl].pdData;
		for (iJVarl = 0;iJVarl < psLPVectorD[iIVarl].iLength;iJVarl++)
		{
			*(pdDataPtr++) = 1.0;
		}
	}
	return psLPVectorD;
}


//释放向量
void MV_FreeVectors_D(LPMV_VECTOR_D psLPVectorD,INT iNum)
{
	INT iIVarl = 0;

	if(!psLPVectorD)
	{
		return;
	}

	for (iIVarl = 0;iIVarl < iNum;iIVarl++)
	{
		if (psLPVectorD[iIVarl].pdData)
		{
			free(psLPVectorD[iIVarl].pdData);
			psLPVectorD[iIVarl].pdData = 0;
		}
	}

	free(psLPVectorD);

	return;
}

DOUBLE MV_VectorGet_D(const LPMV_VECTOR_D pVector, INT iIndex)
{
	if (NULL == pVector) {
		return 0;
	}

	return pVector->pdData[iIndex];
}

void MV_VectorSet_D(LPMV_VECTOR_D psVector, INT iIndex, DOUBLE dData)
{
	if (NULL == psVector) {
		return;
	}

	psVector->pdData[iIndex] = dData;

	return;
}

LPMV_VECTOR_D MV_VectorCopy_D(LPMV_VECTOR_D psSrc)
{
	LPMV_VECTOR_D psVec = NULL;

	if (NULL != psSrc) {
		psVec = MV_VectorsZeros_D(psSrc->iLength, 1);

		if (psVec == NULL) {
			return NULL;
		}

		memcpy((void*)psVec->pdData, (void*)psSrc->pdData, psSrc->iLength*sizeof(DOUBLE));
	}

	return psVec;
}

void MV_VectorCopy2_D(LPMV_VECTOR_D psSrc, LPMV_VECTOR_D psDst)
{
	if ((NULL == psSrc) || (NULL == psDst)
		|| (psDst->iLength < psSrc->iLength)) {
			return;
	}

	memcpy((void*)psDst->pdData, (void*)psSrc->pdData, psSrc->iLength*sizeof(DOUBLE));
	
	return;
}

void MV_PrintVector_D(LPMV_VECTOR_D psVec)
{
	INT iIVarl = 0;

	if (NULL != psVec) 
	{
		for (iIVarl = 0; iIVarl < psVec->iLength; iIVarl++)
		{
			printf("%13.7e\t", MV_VectorGet_D(psVec, iIVarl));
		}
		printf("\n");
	}
}
