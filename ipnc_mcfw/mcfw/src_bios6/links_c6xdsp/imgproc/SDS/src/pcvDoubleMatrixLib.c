/*
* @(#)$Id: pcvDoubleMatrixLib.c 1007 2012-12-19 03:13:32Z LvGangXiao $ 
* @(#) Implementation file of  Double Matrix Lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/
#include "pcvDoubleMatrixLib.h"

INT MV_MatrixSum_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim)
{
	INT iRow = 0;
	INT iCol = 0;
	
	if(!psMatS||!psMatD)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		if (psMatD->iRows != 1||psMatD->iCols != psMatS->iCols)
		{
			return(-1);
		}

		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{
			DOUBLE *pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE *pdData = psMatD->pdData;

			for (iCol = 0;iCol < psMatS->iCols;iCol ++)
			{
				(*pdData) += (*pdLineData);
				pdData++;
				pdLineData++;
			}
		}
	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		if (psMatD->iCols != 1||psMatD->iRows != psMatS->iRows)
		{
			return(-1);
		}

		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{

			DOUBLE *pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE *pdData = &(psMatD->pdData[iRow]);

			for (iCol = 0;iCol < psMatS->iCols;iCol ++)
			{
				(*pdData) += (*pdLineData);
				pdLineData++;
			}
		}
	}
	else
	{
		return(-1);
	}

	return(0);
}

//矩阵求最大值
INT MV_MatrixMax_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim)
{
	INT iRow = 0;
	INT iCol = 0;

	if(!psMatS||!psMatD)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		if (psMatD->iRows != 1||psMatD->iCols != psMatS->iCols)
		{
			return(-1);
		}

		memcpy( psMatD->pdData, psMatS->pdData, psMatS->iCols*sizeof(DOUBLE));

		for (iRow = 1;iRow < psMatS->iRows;iRow ++)
		{
			DOUBLE *pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE *pdData = psMatD->pdData;

			for (iCol = 0;iCol < psMatS->iCols;iCol ++)
			{
				if(*pdData < *pdLineData)
				{
					*pdData = *pdLineData;
				}
				pdData++;
				pdLineData++;
			}
		}
	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		if (psMatD->iCols != 1||psMatD->iRows != psMatS->iRows)
		{
			return(-1);
		}

		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{

			DOUBLE *pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE *pdData = &(psMatD->pdData[iRow]);

			*pdData = pdLineData[0];
			
			pdLineData++;

			for (iCol = 1;iCol < psMatS->iCols;iCol ++)
			{
				if (*pdData < *pdLineData)
				{
					*pdData = *pdLineData;
				}
				pdLineData++;
			}
		}
	}
	else
	{
		return(-1);
	}

	return(0);

}

//矩阵最小值
INT MV_MatrixMin_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim)
{
	INT iRow = 0;
	INT iCol = 0;

	if(!psMatS||!psMatD)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		if (psMatD->iRows != 1||psMatD->iCols != psMatS->iCols)
		{
			return(-1);
		}

		memcpy( psMatD->pdData, psMatS->pdData, psMatS->iCols*sizeof(DOUBLE));

		for (iRow = 1;iRow < psMatS->iRows;iRow ++)
		{
			DOUBLE *pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE *pdData = psMatD->pdData;

			for (iCol = 0;iCol < psMatS->iCols;iCol ++)
			{
				if((*pdData) > (*pdLineData))
				{
					(*pdData) = (*pdLineData);
				}
				pdData++;
				pdLineData++;
			}
		}
	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		if (psMatD->iCols != 1||psMatD->iRows != psMatS->iRows)
		{
			return(-1);
		}

		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{

			DOUBLE* pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE* pdData = &(psMatD->pdData[iRow]);

			 (*pdData) = pdLineData[0];
			 pdLineData++;

			for (iCol = 1;iCol < psMatS->iCols;iCol ++)
			{
				if ((*pdData) > (*pdLineData))
				{
					(*pdData) = (*pdLineData);
				}
				pdLineData++;
			}
		}
	}
	else
	{
		return(-1);
	}

	return(0);
}
//冒泡排序
static INT MV_BubbleSort_D(DOUBLE *piInput,DOUBLE *piOutput,INT iN)
{
	INT iI = 0;
	INT iJ = 0;
	INT iFlag = 0;
	DOUBLE dTemp = 0;

	if (!piInput||!piOutput||iN <= 0)
	{
		return(-1);
	}

	memcpy(piOutput,piInput,iN*sizeof(DOUBLE));

	for(iI = 0;iI < iN - 1 ;iI++)
	{
		iFlag = 0;
		for(iJ = iN-1;iJ > iI;iJ--)
		{
			if(piOutput[iJ] < piOutput[iJ-1])
			{
				dTemp = piOutput[iJ];
				piOutput[iJ] = piOutput[iJ-1];
				piOutput[iJ-1] = dTemp;

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

//矩阵排序
INT MV_MatrixSort_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim)
{
	INT iRow = 0;

	if(!psMatS||!psMatD)
	{
		return(-1);
	}
	
	if (psMatS->iRows != psMatD->iRows||psMatS->iCols != psMatD->iCols)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		LPMV_MATRIX_D pTempSrc = NULL;
		LPMV_MATRIX_D pTempDst = NULL;
		
		pTempSrc = MV_MatricesZeros_D(psMatS->iCols,psMatS->iRows,1);

		if (!pTempSrc)
		{
			return(-1);
		}

		pTempDst = MV_MatricesZeros_D(psMatS->iCols,psMatS->iRows,1);

		if (!pTempDst)
		{
			MV_FreeMatrices_D(pTempSrc,1);
			return(-1);
		}

		MV_MatrixTrans_D(psMatS,pTempSrc);

		for (iRow = 0;iRow < pTempSrc->iRows;iRow ++)
		{
			DOUBLE* pLineDataSrc = MV_MatrixGetLinePtr_D(pTempSrc,iRow);
			DOUBLE* pLineDataDst = MV_MatrixGetLinePtr_D(pTempDst,iRow);
			MV_BubbleSort_D(pLineDataSrc,pLineDataDst,pTempSrc->iCols);
		}

		MV_MatrixTrans_D(pTempDst,psMatD);

		MV_FreeMatrices_D(pTempSrc,1);
		MV_FreeMatrices_D(pTempDst,1);
	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{
			DOUBLE* pLineDataSrc = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE* pLineDataDst = MV_MatrixGetLinePtr_D(psMatD,iRow);
			MV_BubbleSort_D(pLineDataSrc,pLineDataDst,psMatS->iCols);
		}
	}
	else
	{
		return(-1);
	}

	return(0);

}

//矩阵翻转
INT MV_MatrixFliplr_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD)
{
	INT iRow = 0;
	INT iCol = 0;
	INT iMaxIndex = 0;

	if(!psMatS||!psMatD)
	{
		return(-1);
	}

	if (psMatS->iRows != psMatD->iRows||psMatS->iCols != psMatD->iCols)
	{
		return(-1);
	}

	iMaxIndex = psMatS->iCols - 1;

	for (iRow = 0;iRow < psMatS->iRows;iRow ++)
	{
		DOUBLE *pLineDataSrc = MV_MatrixGetLinePtr_D(psMatS,iRow);
		DOUBLE *pLineDataDst = MV_MatrixGetLinePtr_D(psMatD,iRow);
		for (iCol = 0;iCol < psMatS->iCols;iCol ++)
		{
			pLineDataDst[iCol] = pLineDataSrc[iMaxIndex - iCol];
		}
	}

	return(0);

}

//矩阵求平均值
INT MV_MatrixMean_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim)
{
	INT iRow = 0;
	INT iCol = 0;

	if(!psMatS)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		if (psMatD->iRows != 1||psMatD->iCols != psMatS->iCols)
		{
			return(-1);
		}

		memset(psMatD->pdData,0,sizeof(DOUBLE)*psMatD->iLength);

		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{
			DOUBLE *pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);
			DOUBLE *pdata = psMatD->pdData;

			for (iCol = 0;iCol < psMatS->iCols;iCol ++)
			{
				(*pdata) += (*pdLineData);
				pdata++;
				pdLineData++;
			}
		}

		for (iCol = 0;iCol < psMatD->iCols;iCol ++)
		{
			psMatD->pdData[iCol] = psMatD->pdData[iCol]/psMatS->iRows;
		}

	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		if (psMatD->iCols != 1||psMatD->iRows != psMatS->iRows)
		{
			return(-1);
		}

		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{

			DOUBLE* pdLineData = MV_MatrixGetLinePtr_D(psMatS,iRow);

			DOUBLE iSum = 0;

			for (iCol = 0;iCol < psMatS->iCols;iCol ++)
			{
				iSum += (*pdLineData);
				pdLineData++;
			}
			psMatD->pdData[iRow] = iSum/psMatS->iCols;
		}
	}
	else
	{
		return(-1);
	}

	return(0);
}

//矩阵求中位数
INT MV_MatrixMedian_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim)
{
	LPMV_MATRIX_D psSortMat = NULL;
	INT iRow = 0;
	INT iCol = 0;

	if(!psMatS||!psMatD)
	{
		return(-1);
	}

	psSortMat = MV_MatricesZeros_D(psMatS->iRows,psMatS->iCols,1);

	if (!psSortMat)
	{
		return(-1);
	}

	if(MV_MatrixSort_D(psMatS,psSortMat,iDim) < 0)
	{
		MV_FreeMatrices_D(psSortMat,1);
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		INT   iMean = 0;
		INT   iPos = 0;
		INT   bFlag = 0;

		if (psMatD->iRows != 1||psMatD->iCols != psMatS->iCols)
		{
			MV_FreeMatrices_D(psSortMat,1);
			return(-1);
		}

		iMean = psMatS->iRows/2;

		iPos = iMean*psMatS->iCols;

		if (psMatS->iRows%2 == 0)
		{
			bFlag = 1;
		}
		else
		{
			bFlag = 0;
		}

		if (bFlag)
		{ 
			for (iCol = 0;iCol < psMatS->iCols;iCol ++)
			{
				psMatD->pdData[iCol] = (psSortMat->pdData[iPos - psMatS->iCols + iCol] + psSortMat->pdData[iPos + iCol])/2;
			}
		}
		else
		{
			memcpy(psMatD->pdData, &(psSortMat->pdData[iPos]), psMatD->iCols*sizeof(DOUBLE));
		}

	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		INT   iMean = 0;
		INT   bFlag = 0;

		if (psMatD->iCols != 1||psMatD->iRows != psMatS->iRows)
		{
			MV_FreeMatrices_D(psSortMat,1);
			return(-1);
		}
		
		iMean = psMatS->iCols/2;

		if (psMatS->iCols%2 == 0)
		{
			bFlag = 1;
		}
		else
		{
			bFlag = 0;
		}

		for (iRow = 0;iRow < psMatS->iRows;iRow ++)
		{

			DOUBLE* pdLineData = MV_MatrixGetLinePtr_D(psSortMat,iRow);

			DOUBLE iValue = 0;

		    if (bFlag)
		    {
				iValue = (pdLineData[iMean - 1] + pdLineData[iMean])/2;
		    }
			else
			{
				iValue = pdLineData[iMean];

			}
			psMatD->pdData[iRow] = iValue;
		}
	}
	else
	{
		MV_FreeMatrices_D(psSortMat,1);
		return(-1);
	}

	MV_FreeMatrices_D(psSortMat,1);

	return(0);
}

//矩阵的标准差
DOUBLE MV_MatrixSTD_D(LPMV_MATRIX_D psMat,INT iflag)
{
	DOUBLE *pdA = NULL;
	INT iLength = 0;
	DOUBLE iSum = 0.0;
	DOUBLE iMean = 0;
	DOUBLE iVar = 0;
	INT iI = 0;

	if (!psMat)
	{
		return 0;
	}
	iLength = psMat->iLength;
	pdA = psMat->pdData;

	if (0 == iLength)
	{
		return 0;
	}

	for (iI = 0;iI < iLength;iI++)
	{
		iSum += pdA[iI];
	}
	iMean = iSum/iLength;
	iSum = 0.0;
	for (iI = 0;iI < iLength;iI++)
	{
		iVar = pdA[iI] - iMean;
		iSum += iVar*iVar;
	}
	if (0 == iflag&&iLength != 1)
	{
		return sqrt(iSum/(iLength-1));
	}		
	else
	{
		return sqrt(iSum/(iLength));
	}
}


//矩阵相减
INT MV_MatrixSub_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC)
{
	INT iRow = 0;
	INT iCol = 0;

	if (!psMatA||!psMatB||!psMatC)
	{
		return(-1);
	}

	if (psMatA->iRows != psMatB->iRows||psMatA->iCols != psMatB->iCols
		||psMatA->iRows != psMatC->iRows||psMatA->iCols != psMatC->iCols)
	{
		return(-1);
	}


	for( iRow = 0; iRow < psMatA->iRows; iRow++)
	{
		DOUBLE *pdlineA = &(psMatA->pdData[iRow*psMatA->iCols]);
		DOUBLE *pdlineB = &(psMatB->pdData[iRow*psMatA->iCols]);
		DOUBLE *pdlineC = &(psMatC->pdData[iRow*psMatA->iCols]);
		for (iCol = 0; iCol < psMatA->iCols; iCol++)
		{ 
			(*pdlineC) = (*pdlineA) - (*pdlineB);
			pdlineA++;
			pdlineB++;
			pdlineC++;
		}
	}

	return(0);

}

//矩阵相加
INT MV_MatrixAdd_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC)
{
	INT iRow = 0;
	INT iCol = 0;

	if (!psMatA||!psMatB||!psMatC)
	{
		return(-1);
	}

	if (psMatA->iRows != psMatB->iRows||psMatA->iCols != psMatB->iCols
		||psMatA->iRows != psMatC->iRows||psMatA->iCols != psMatC->iCols)
	{
		return(-1);
	}


	for( iRow = 0; iRow < psMatA->iRows; iRow++)
	{
		INT    iPos_r = iRow*psMatA->iCols;
		DOUBLE *pdlineA = &(psMatA->pdData[iPos_r]);
		DOUBLE *pdlineB = &(psMatB->pdData[iPos_r]);
		DOUBLE *pdlineC = &(psMatC->pdData[iPos_r]);
		for (iCol = 0; iCol < psMatA->iCols; iCol++)
		{ 
			(*pdlineC) = (*pdlineA) + (*pdlineB);
			pdlineA++;
			pdlineB++;
			pdlineC++;
		}
	}

	return(0);

}

//矩阵点除
INT MV_MatrixDiv_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC)
{
	INT iRow = 0;
	INT iCol = 0;
	
	if (!psMatA||!psMatB||!psMatC)
	{
		return(-1);
	}

	if (psMatA->iRows != psMatB->iRows||psMatA->iCols != psMatB->iCols
		||psMatA->iRows != psMatC->iRows||psMatA->iCols != psMatC->iCols)
	{
		return(-1);
	}


	for( iRow = 0; iRow < psMatA->iRows; iRow++)
	{
		INT    iPos_r = iRow*psMatA->iCols;
		DOUBLE *pdlineA = &(psMatA->pdData[iPos_r]);
		DOUBLE *pdlineB = &(psMatB->pdData[iPos_r]);
		DOUBLE *pdlineC = &(psMatC->pdData[iPos_r]);
		for (iCol = 0; iCol < psMatA->iCols; iCol++)
		{ 
			if (0 == *pdlineB)
			{
				if (*pdlineA > 0)
				{
					(*pdlineC) = exp(1000);
				}
				else if (*pdlineA < 0)
				{
					(*pdlineC) = -exp(1000);
				}
				else
				{
					return(-1);
				}
			}
			else
			{
				(*pdlineC) = (*pdlineA)/(*pdlineB);
			}
			
			pdlineA++;
			pdlineB++;
			pdlineC++;
		}
	}

	return(0);
}

//矩阵点乘
INT MV_MatrixDotMul_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC)
{ 
	DOUBLE *pdA = NULL;
	DOUBLE *pdB = NULL;
	DOUBLE *pdC = NULL;
	INT iM = 0, iN = 0;
	INT iI = 0,iJ = 0;

	if (!psMatA||!psMatB||!psMatC)
	{
		return(-1);
	}
	if (psMatA->iRows != psMatB->iRows||psMatA->iCols != psMatB->iCols
		||psMatA->iRows != psMatC->iRows||psMatA->iCols != psMatC->iCols)
	{
		return(-1);
	}

	pdA = psMatA->pdData;
	pdB = psMatB->pdData;
	pdC = psMatC->pdData;
	iM = psMatA->iRows;
	iN = psMatA->iCols;

	for( iI = 0; iI < iM; iI++)
	{
		INT ipos_r = iI*iN; 
		for (iJ = 0; iJ < iN; iJ++)
		{ 
			INT ipos_c = ipos_r + iJ;
			pdC[ipos_c]=pdA[ipos_c]*pdB[ipos_c];
		}
	}
	return(0);
}

//矩阵乘法
INT MV_MatrixMul_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC)
{ 
	DOUBLE *pdA = NULL;
	DOUBLE *pdB = NULL;
	DOUBLE *pdC = NULL;
	INT iM = 0,iN = 0,iK = 0;
	INT iI = 0,iJ = 0,iL = 0,iU = 0;

	if (!psMatA||!psMatB||!psMatC)
	{
		return(-1);
	}
	if (psMatA->iCols != psMatB->iRows||psMatC->iRows != psMatA->iRows||psMatC->iCols != psMatB->iCols)
	{
		return(-1);
	}

	pdA = psMatA->pdData;
	pdB = psMatB->pdData;
	pdC = psMatC->pdData;
	iM = psMatA->iRows;
	iN = psMatA->iCols;
	iK = psMatB->iCols;

	for (iI = 0; iI <= iM-1; iI++)
	{
		INT iiIndex = iI*iN;
		for (iJ = 0; iJ <= iK-1; iJ++)
		{ 
			iU = iI*iK + iJ; 
			pdC[iU]=0.0;
			for (iL=0; iL<=iN-1; iL++)
			{
				pdC[iU]=pdC[iU]+pdA[iiIndex + iL]*pdB[iL*iK + iJ];
			}
		}
	}
	return(0);
}

//矩阵转置
INT MV_MatrixTrans_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD)
{ 
	DOUBLE *pdA = NULL;
	DOUBLE *pdB = NULL;
	INT iM = 0,iN = 0;
	INT iI = 0,iJ = 0;

	if (!psMatS||!psMatD)
	{
		return(-1);
	}
    pdA = psMatS->pdData;
	pdB = psMatD->pdData;
	iM = psMatS->iRows;
	iN = psMatS->iCols;

	if (psMatD->iRows != iN||psMatD->iCols != iM)
	{
		return(-1);
	}

	for (iI = 0; iI < iM; iI++)
	{
		DOUBLE *dtmp = &(pdA[iI*iN]);
		INT iIndex = iI;
		for (iJ = 0; iJ < iN; iJ++)
		{
			pdB[iIndex]= (*dtmp);
			iIndex += iM;
			dtmp++;
		}
	}
	return(0);
}

//矩阵求平方根
INT MV_MatrixSqrt_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D pMatDr,LPMV_MATRIX_D pMatDi)
{
	INT iIndex = 0;
	DOUBLE *pdDatas = NULL;
	DOUBLE *pdDatadr = NULL;
	DOUBLE *pdDatadi = NULL;

	if (!psMatS||!pMatDr||!pMatDi)
	{
		return(-1);
	}
	if (psMatS->iCols != pMatDr->iCols||psMatS->iRows != pMatDr->iRows
		||psMatS->iCols != pMatDi->iCols||psMatS->iRows != pMatDi->iRows)
	{
		return(-1);
	}

	pdDatas = psMatS->pdData;
	pdDatadr = pMatDr->pdData;
	pdDatadi = pMatDi->pdData;

	for (iIndex = 0;iIndex < psMatS->iLength;iIndex ++)
	{
		DOUBLE dVar = *pdDatas;
		if(*pdDatas < 0)
		{
			dVar = -dVar;
			*pdDatadi = sqrt(dVar);
		}
		else
		{
			*pdDatadr = sqrt(dVar);
		}
		pdDatas++;
		pdDatadr++;
		pdDatadi++;
	}
	return(0);
}
//获取某行或某列
INT MV_GetMatricesRoworCol_D(LPMV_MATRIX_D psMat,LPMV_VECTOR_D psOutput,INT iIndex,MATRIX_DIM iDim)
{
	INT iRow = 0;

	if(!psMat||!psOutput||iIndex < 0)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		DOUBLE *pdLineData = NULL;
		DOUBLE *pdata = psOutput->pdData;

		pdLineData = psMat->pdData + iIndex;

		for (iRow = 0;iRow < psMat->iRows;iRow ++)
		{
			(*pdata) = (*pdLineData);
			pdata++;
			pdLineData += psMat->iCols;
		}

	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		memcpy(psOutput->pdData,&(psMat->pdData[iIndex*psMat->iCols]),sizeof(DOUBLE)*psOutput->iLength);
	}
	else
	{
		return(-1);
	}

	return(0);

}

//矩阵某行或列转为vector
INT MV_MatrixToVector_D(LPMV_MATRIX_D psMat,INT iIndex,MATRIX_DIM iDim,LPMV_VECTOR_D psOutput)
{
	INT iRow = 0;

	if(!psMat||!psOutput)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		DOUBLE *pdLineData = NULL;
		DOUBLE *pdata = psOutput->pdData;

		if (psOutput->iLength != psMat->iRows||iIndex >= psMat->iCols)
		{
			return(-1);
		}

		pdLineData = psMat->pdData + iIndex;

		for (iRow = 0;iRow < psMat->iRows;iRow ++)
		{
			(*pdata) = (*pdLineData);
			pdata++;
			pdLineData += psMat->iCols;
		}

	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		if (psOutput->iLength != psMat->iCols||iIndex >= psMat->iRows)
		{
			return(-1);
		}

		memcpy(psOutput->pdData,&(psMat->pdData[iIndex*psMat->iCols]),sizeof(DOUBLE)*psOutput->iLength);
	}
	else
	{
		return(-1);
	}

	return(0);
}

//向量转为矩阵
INT MV_VectorToMatrix_D(LPMV_VECTOR_D psVec,MATRIX_DIM iDim,LPMV_MATRIX_D psOutput)
{
	if(!psVec||!psOutput)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		if (psVec->iLength != psOutput->iRows)
		{
			return(-1);
		}
	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		if (psVec->iLength != psOutput->iCols)
		{
			return(-1);
		}
	}
	else
	{
		return(-1);
	}

	memcpy(psOutput->pdData,psVec->pdData,sizeof(DOUBLE)*psOutput->iLength);

	return(0);


}


//矩阵按行或按列求范数
DOUBLE MV_MatrixNorm2_D(LPMV_MATRIX_D psMat,INT iIndex,MATRIX_DIM iDim)
{
	INT iRow = 0;
	INT iCol = 0;
	DOUBLE dValue = 0;

	if(!psMat||iIndex < 0)
	{
		return(-1);
	}

	if (MATRIX_DIM_COLUMN == iDim)
	{
		DOUBLE *pdLineData = NULL;
		dValue = 0;

		if (iIndex >= psMat->iCols)
		{
			return(-1);
		}

		pdLineData = psMat->pdData + iIndex;

		for (iRow = 0;iRow < psMat->iRows;iRow ++)
		{
			dValue += (*pdLineData)*(*pdLineData);

			pdLineData += psMat->iCols;
		}
	}
	else if (MATRIX_DIM_ROW == iDim)
	{
		DOUBLE *pdLineData = NULL;

		if (iIndex >= psMat->iRows)
		{
			return(-1);
		}

		pdLineData = MV_MatrixGetLinePtr_D(psMat,iIndex);

		dValue = 0;
		for (iCol = 0;iCol < psMat->iCols;iCol ++)
		{
			dValue += (*pdLineData)*(*pdLineData);
			pdLineData++;
		}
	}
	else
	{
		return(-1);
	}

	return sqrt(dValue);

}

INT  MV_MatrixEigen_D(LPMV_MATRIX_D psMat,LPMV_MATRIX_D psEigenVector)
{
	const DOUBLE eps = PCV_DM_EPSILON;
	const INT jt = PCV_ITERATION_NUMBER;
	DOUBLE *pdA = NULL,*pdV = NULL;
	INT iN = 0;
	INT iI = 0,iJ = 0,iP = 0,iQ = 0,iU = 0,iW = 0,iT = 0,iS = 0,iL = 0;
	DOUBLE dFm = 0,dCn = 0,dSn = 0,dOmega = 0,dX = 0,dY = 0,dD = 0;

	if (!psMat||!psEigenVector)
	{
		return(-1);
	}

	if (psMat->iCols != psMat->iRows||psEigenVector->iCols !=  psEigenVector->iRows||psEigenVector->iCols != psMat->iCols)
	{
		return(-1);
	}

	pdA = psMat->pdData;
	iN = psMat->iCols;
	pdV = psEigenVector->pdData;

	iL = 1;
	for(iI = 0; iI <= iN - 1; iI++)
	{ 
		pdV[iI*iN+iI]=1.0;
		for (iJ=0; iJ<=iN-1; iJ++)
			if (iI!=iJ) pdV[iI*iN+iJ]=0.0;
	}
	while (1==1)
	{ 
		dFm=0.0;
		for (iI=1; iI<=iN-1; iI++)
			for (iJ=0; iJ<=iI-1; iJ++)
				{
					dD=fabs(pdA[iI*iN+iJ]);
					if ((iI!=iJ)&&(dD>dFm))
					{
						dFm=dD; 
						iP=iI; 
						iQ=iJ;
					}
				}
		if (dFm<eps)
			return(0);
		if (iL>jt)
			return(-1);
		iL=iL+1;
		iU=iP*iN+iQ; iW=iP*iN+iP; iT=iQ*iN+iP; iS=iQ*iN+iQ;
		dX=-pdA[iU]; dY=(pdA[iS]-pdA[iW])/2.0;
		dOmega=dX/sqrt(dX*dX+dY*dY);
		if (dY<0.0) dOmega=-dOmega;
		dSn=1.0+sqrt(1.0-dOmega*dOmega);
		dSn=dOmega/sqrt(2.0*dSn);
		dCn=sqrt(1.0-dSn*dSn);
		dFm=pdA[iW];
		pdA[iW]=dFm*dCn*dCn+pdA[iS]*dSn*dSn+pdA[iU]*dOmega;
		pdA[iS]=dFm*dSn*dSn+pdA[iS]*dCn*dCn-pdA[iU]*dOmega;
		pdA[iU]=0.0; pdA[iT]=0.0;
		for (iJ=0; iJ<=iN-1; iJ++)
			if ((iJ!=iP)&&(iJ!=iQ))
			{
				iU=iP*iN+iJ; iW=iQ*iN+iJ;
				dFm=pdA[iU];
				pdA[iU]=dFm*dCn+pdA[iW]*dSn;
				pdA[iW]=-dFm*dSn+pdA[iW]*dCn;
			}
		for (iI=0; iI<=iN-1; iI++)
			if ((iI!=iP)&&(iI!=iQ))
			{
				iU=iI*iN+iP; iW=iI*iN+iQ;
				dFm=pdA[iU];
				pdA[iU]=dFm*dCn+pdA[iW]*dSn;
				pdA[iW]=-dFm*dSn+pdA[iW]*dCn;
			}
		for (iI=0; iI<=iN-1; iI++)
		{
			iU=iI*iN+iP; iW=iI*iN+iQ;
			dFm=pdV[iU];
			pdV[iU]=dFm*dCn+pdV[iW]*dSn;
			pdV[iW]=-dFm*dSn+pdV[iW]*dCn;
		}
	}
	return(0);
}

//矩阵求范数
DOUBLE MV_MatrixNorm_D(LPMV_MATRIX_D psMat)
{
	LPMV_MATRIX_D psMatTranspos = NULL;
	LPMV_MATRIX_D psMatTMul = NULL;
	LPMV_MATRIX_D psEigenVector = NULL;
	INT iRow = 0;
	DOUBLE dMaxEigenValue = 0;

	if (!psMat)
	{
		return(-1);
	}

	psMatTranspos =  MV_MatricesZeros_D(psMat->iCols,psMat->iRows,1);

	if (!psMatTranspos)
	{
		 return(-1);
	}

	MV_MatrixTrans_D(psMat,psMatTranspos);

	psMatTMul = MV_MatricesZeros_D(psMat->iRows,psMat->iRows,1);

	if (!psMatTMul)
	{
		MV_FreeMatrices_D(psMatTranspos,1);
		return(-1);
	}

	 MV_MatrixMul_D(psMat, psMatTranspos,psMatTMul);

	 psEigenVector = MV_MatricesZeros_D(psMat->iRows,psMat->iRows,1);

	 if (!psEigenVector||-1 == MV_MatrixEigen_D(psMatTMul,psEigenVector))
	 {
		 MV_FreeMatrices_D(psMatTranspos,1);
		 MV_FreeMatrices_D(psMatTMul,1);
		 MV_FreeMatrices_D(psEigenVector,1);
		 return(-1);
	 }

	 dMaxEigenValue = psMatTMul->pdData[0];

	 for(iRow = 1;iRow < psMatTMul->iRows;iRow++)
	 {
		 DOUBLE dValue = psMatTMul->pdData[iRow*psMatTMul->iCols + iRow];
		 if (dMaxEigenValue < dValue)
		 {
			dMaxEigenValue = dValue;
		 }
	 }

	 MV_FreeMatrices_D(psMatTranspos,1);
	 MV_FreeMatrices_D(psMatTMul,1);
	 MV_FreeMatrices_D(psEigenVector,1);
	
	 return sqrt(dMaxEigenValue);
}

//求逆矩阵
INT MV_MatrixInv_D(LPMV_MATRIX_D psMat)
{ 
	DOUBLE *pdA = NULL;
	INT *piIiS = NULL;
	INT *piJjs = NULL;
	INT iI = 0,iJ = 0,iK = 0,iL = 0,iU = 0,iV = 0;
	INT iN = 0;
	DOUBLE iD = 0.0,iP = 0.0;

	if (NULL == psMat||psMat->iCols != psMat->iRows||0 == psMat->iRows)
	{
		return(-1);
	}
	pdA = psMat->pdData;
	iN = psMat->iRows;

	piIiS=(INT*)malloc(iN*sizeof(INT));
	piJjs=(INT*)malloc(iN*sizeof(INT));
	for (iK=0; iK<=iN-1; iK++)
	{ 
		iD=0.0;
		for (iI=iK; iI<=iN-1; iI++)
		for (iJ=iK; iJ<=iN-1; iJ++)
		{
			iL=iI*iN+iJ; iP=fabs(pdA[iL]);
			if (iP>iD) 
			{ 
				iD=iP;
				piIiS[iK]=iI;
				piJjs[iK]=iJ;
			}
		}
		if (iD+1.0==1.0)
		{ 
			free(piIiS); 
			free(piJjs);
			return(-1);
		}
		if (piIiS[iK]!=iK)
		for (iJ=0; iJ<=iN-1; iJ++)
		{ 
			iU=iK*iN+iJ; 
			iV=piIiS[iK]*iN+iJ;
			iP=pdA[iU]; 
			pdA[iU]=pdA[iV]; 
			pdA[iV]=iP;
		}
		if (piJjs[iK]!=iK)
		for (iI=0; iI<=iN-1; iI++)
		{ 
			iU=iI*iN+iK; 
			iV=iI*iN+piJjs[iK];
			iP=pdA[iU]; 
			pdA[iU]=pdA[iV]; 
			pdA[iV]=iP;
		}
		iL=iK*iN+iK;
		pdA[iL]=1.0/pdA[iL];
		for (iJ=0; iJ<=iN-1; iJ++)
		if (iJ!=iK)
		{ 
			iU=iK*iN+iJ; 
			pdA[iU]=pdA[iU]*pdA[iL];
		}
		for (iI=0; iI<=iN-1; iI++)
			if (iI!=iK)
				for (iJ=0; iJ<=iN-1; iJ++)
					if (iJ!=iK)
					{ 
						iU=iI*iN+iJ;
						pdA[iU]=pdA[iU]-pdA[iI*iN+iK]*pdA[iK*iN+iJ];
					}
		for (iI=0; iI<=iN-1; iI++)
			if (iI!=iK)
			{ 
				iU=iI*iN+iK; 
				pdA[iU]=-pdA[iU]*pdA[iL];
			}
	}
	for (iK=iN-1; iK>=0; iK--)
	{ 
		if (piJjs[iK]!=iK)
		for (iJ=0; iJ<=iN-1; iJ++)
		{ 
			iU=iK*iN+iJ; iV=piJjs[iK]*iN+iJ;
			iP=pdA[iU]; 
			pdA[iU]=pdA[iV]; 
			pdA[iV]=iP;
		}
		if (piIiS[iK]!=iK)
		for (iI=0; iI<=iN-1; iI++)
		{ 
			iU=iI*iN+iK; 
			iV=iI*iN+piIiS[iK];
			iP=pdA[iU]; 
			pdA[iU]=pdA[iV]; 
			pdA[iV]=iP;
		}
	}
	free(piIiS); 
	free(piJjs);
	return(0);
}

static void ppp(DOUBLE *pdA,DOUBLE *pdE,DOUBLE *pdS,DOUBLE *pdV,INT iM,INT iN)
{ 
	INT iI,iJ,iP,iQ;
	DOUBLE dD;
	if (iM>=iN) iI=iN;
	else iI=iM;
	for (iJ=1; iJ<=iI-1; iJ++)
	{
		pdA[(iJ-1)*iN+iJ-1]=pdS[iJ-1];
		pdA[(iJ-1)*iN+iJ]=pdE[iJ-1];
	}
	pdA[(iI-1)*iN+iI-1]=pdS[iI-1];
	if (iM<iN) pdA[(iI-1)*iN+iI]=pdE[iI-1];
	for (iI=1; iI<=iN-1; iI++)
	for (iJ=iI+1; iJ<=iN; iJ++)
	{
		iP=(iI-1)*iN+iJ-1; iQ=(iJ-1)*iN+iI-1;
		dD=pdV[iP]; pdV[iP]=pdV[iQ]; pdV[iQ]=dD;
	}
	return;
}

static void sss(DOUBLE *dFg,DOUBLE *dCs)
{ 
	DOUBLE dR,dD;
	if ((fabs(dFg[0])+fabs(dFg[1]))==0.0)
	{ dCs[0]=1.0; dCs[1]=0.0; dD=0.0;}
	else 
	{ 
		dD=sqrt(dFg[0]*dFg[0]+dFg[1]*dFg[1]);
		if (fabs(dFg[0])>fabs(dFg[1]))
		{ 
			dD=fabs(dD);
			if (dFg[0]<0.0) dD=-dD;
		}
		if (fabs(dFg[1])>=fabs(dFg[0]))
		{ 
			dD=fabs(dD);
			if (dFg[1]<0.0) dD=-dD;
		}
		dCs[0]=dFg[0]/dD; dCs[1]=dFg[1]/dD;
	}
	dR=1.0;
	if (fabs(dFg[0])>fabs(dFg[1])) dR=dCs[1];
	else
	if (dCs[0]!=0.0) dR=1.0/dCs[0];
	dFg[0]=dD; dFg[1]=dR;
	return;
}

//奇异值分解
INT MV_MatrixSVD_D(LPMV_MATRIX_D psMat,LPMV_MATRIX_D psMatL,LPMV_MATRIX_D psMatR)
{
	const DOUBLE eps = PCV_DM_EPSILON;
	DOUBLE *pdA = NULL;
	DOUBLE *pdU = NULL;
	DOUBLE *pdV = NULL;
	INT iM = 0,iN = 0;
	INT iKa = 0;
	INT iI = 0,iJ = 0,iK = 0,iL = 0,iIt = 0,iLl = 0,iKk = 0,iX = 0,iY = 0,iMm = 0,iNn = 0,iZ = 0,iM1 = 0,iKs = 0;
	DOUBLE dD = 0,dDd = 0,dT = 0,dSm = 0,dSm1 = 0,dEm1 = 0,dSk = 0,dEk = 0,dB = 0,dC = 0,dShh = 0;
	DOUBLE darrFg[2] = {0};
	DOUBLE darrCs[2] = {0};
	DOUBLE *pdS = NULL,*pdE = NULL,*pdW = NULL;

	if (!psMat||!psMatL||!psMatR)
	{
		return(-1);
	}

	iM = psMat->iRows;
	iN = psMat->iCols;

	if (psMatL->iRows != psMatL->iCols||psMatR->iRows != psMatR->iCols
		||psMatL->iRows != iM||psMatR->iRows != iN)
	{
		return(-1);
	}

	pdA = psMat->pdData;
	pdU = psMatL->pdData;
	pdV = psMatR->pdData;

	iKa = max(iM,iN) + 1;
	pdS=(DOUBLE*)malloc(iKa*sizeof(DOUBLE));
	pdE=(DOUBLE*)malloc(iKa*sizeof(DOUBLE));
	pdW=(DOUBLE*)malloc(iKa*sizeof(DOUBLE));
	iIt= PCV_ITERATION_NUMBER; 
	iK = iN;
	if (iM-1<iN) iK=iM-1;
	iL=iM;
	if (iN-2<iM) iL=iN-2;
	if (iL<0) iL=0;
	iLl=iK;
	if (iL>iK) iLl=iL;
	if (iLl>=1)
	{
		for (iKk=1; iKk<=iLl; iKk++)
		{ 
			if (iKk<=iK)
			{ 
				dD=0.0;
				for (iI=iKk; iI<=iM; iI++)
				{ 
					iX=(iI-1)*iN+iKk-1; dD=dD+pdA[iX]*pdA[iX];
				}
				pdS[iKk-1]=sqrt(dD);
				if (pdS[iKk-1]!=0.0)
				{ 
					iX=(iKk-1)*iN+iKk-1;
					if (pdA[iX]!=0.0)
					{ 
						pdS[iKk-1]=fabs(pdS[iKk-1]);
						if (pdA[iX]<0.0) pdS[iKk-1]=-pdS[iKk-1];
					}
					for (iI=iKk; iI<=iM; iI++)
					{ 
						iY=(iI-1)*iN+iKk-1;
						pdA[iY]=pdA[iY]/pdS[iKk-1];
					}
					pdA[iX]=1.0+pdA[iX];
				}
				pdS[iKk-1]=-pdS[iKk-1];
			}
			if (iN>=iKk+1)
			{ 
				for (iJ=iKk+1; iJ<=iN; iJ++)
				{ 
					if ((iKk<=iK)&&(pdS[iKk-1]!=0.0))
					{ 
						dD=0.0;
						for (iI=iKk; iI<=iM; iI++)
						{ 
							iX=(iI-1)*iN+iKk-1;
							iY=(iI-1)*iN+iJ-1;
							dD=dD+pdA[iX]*pdA[iY];
						}
						dD=-dD/pdA[(iKk-1)*iN+iKk-1];
						for (iI=iKk; iI<=iM; iI++)
						{ 
							iX=(iI-1)*iN+iJ-1;
							iY=(iI-1)*iN+iKk-1;
							pdA[iX]=pdA[iX]+dD*pdA[iY];
						}
					}
					pdE[iJ-1]=pdA[(iKk-1)*iN+iJ-1];
				}
			}
			if (iKk<=iK)
			{ 
				for (iI=iKk; iI<=iM; iI++)
				{ 
					iX=(iI-1)*iM+iKk-1; iY=(iI-1)*iN+iKk-1;
					pdU[iX]=pdA[iY];
				}
			}
			if (iKk<=iL)
			{ 
				dD=0.0;
				for (iI=iKk+1; iI<=iN; iI++)
				dD=dD+pdE[iI-1]*pdE[iI-1];
				pdE[iKk-1]=sqrt(dD);
				if (pdE[iKk-1]!=0.0)
				{ 
					if (pdE[iKk]!=0.0)
					{ 
						pdE[iKk-1]=fabs(pdE[iKk-1]);
						if (pdE[iKk]<0.0) pdE[iKk-1]=-pdE[iKk-1];
					}
					for (iI=iKk+1; iI<=iN; iI++)
						pdE[iI-1]=pdE[iI-1]/pdE[iKk-1];
					pdE[iKk]=1.0+pdE[iKk];
				}
				pdE[iKk-1]=-pdE[iKk-1];
				if ((iKk+1<=iM)&&(pdE[iKk-1]!=0.0))
				{ 
					for (iI=iKk+1; iI<=iM; iI++) pdW[iI-1]=0.0;
					for (iJ=iKk+1; iJ<=iN; iJ++)
					for (iI=iKk+1; iI<=iM; iI++)
						pdW[iI-1]=pdW[iI-1]+pdE[iJ-1]*pdA[(iI-1)*iN+iJ-1];
					for (iJ=iKk+1; iJ<=iN; iJ++)
					for (iI=iKk+1; iI<=iM; iI++)
					{ 
						iX=(iI-1)*iN+iJ-1;
						pdA[iX]=pdA[iX]-pdW[iI-1]*pdE[iJ-1]/pdE[iKk];
					}
				}
				for (iI=iKk+1; iI<=iN; iI++)
				pdV[(iI-1)*iN+iKk-1]=pdE[iI-1];
			}
		}
	}
	iMm=iN;
	if (iM+1<iN) iMm=iM+1;
	if (iK<iN) pdS[iK]=pdA[iK*iN+iK];
	if (iM<iMm) pdS[iMm-1]=0.0;
	if (iL+1<iMm) pdE[iL]=pdA[iL*iN+iMm-1];
	pdE[iMm-1]=0.0;
	iNn=iM;
	if (iM>iN) iNn=iN;
	if (iNn>=iK+1)
	{ 
		for (iJ=iK+1; iJ<=iNn; iJ++)
		{ 
			for (iI=1; iI<=iM; iI++)
			pdU[(iI-1)*iM+iJ-1]=0.0;
			pdU[(iJ-1)*iM+iJ-1]=1.0;
		}
	}
	if (iK>=1)
	{ 
		for (iLl=1; iLl<=iK; iLl++)
		{ 
			iKk=iK-iLl+1; iZ=(iKk-1)*iM+iKk-1;
			if (pdS[iKk-1]!=0.0)
			{ 
				if (iNn>=iKk+1)
				for (iJ=iKk+1; iJ<=iNn; iJ++)
				{ 
					dD=0.0;
					for (iI=iKk; iI<=iM; iI++)
					{ 
						iX=(iI-1)*iM+iKk-1;
						iY=(iI-1)*iM+iJ-1;
						dD=dD+pdU[iX]*pdU[iY]/pdU[iZ];
					}
					dD=-dD;
					for (iI=iKk; iI<=iM; iI++)
					{ 
						iX=(iI-1)*iM+iJ-1;
						iY=(iI-1)*iM+iKk-1;
						pdU[iX]=pdU[iX]+dD*pdU[iY];
					}
				}
				for (iI=iKk; iI<=iM; iI++)
				{ iX=(iI-1)*iM+iKk-1; pdU[iX]=-pdU[iX];}
				pdU[iZ]=1.0+pdU[iZ];
				if (iKk-1>=1)
				for (iI=1; iI<=iKk-1; iI++)
				pdU[(iI-1)*iM+iKk-1]=0.0;
			}
			else
			{ 
				for (iI=1; iI<=iM; iI++)
				pdU[(iI-1)*iM+iKk-1]=0.0;
				pdU[(iKk-1)*iM+iKk-1]=1.0;
			}
		}
	}
	for (iLl=1; iLl<=iN; iLl++)
	{ 
		iKk=iN-iLl+1; iZ=iKk*iN+iKk-1;
		if ((iKk<=iL)&&(pdE[iKk-1]!=0.0))
		{ 
			for (iJ=iKk+1; iJ<=iN; iJ++)
			{ 
				dD=0.0;
				for (iI=iKk+1; iI<=iN; iI++)
				{ 
					iX=(iI-1)*iN+iKk-1; iY=(iI-1)*iN+iJ-1;
					dD=dD+pdV[iX]*pdV[iY]/pdV[iZ];
				}
				dD=-dD;
				for (iI=iKk+1; iI<=iN; iI++)
				{ 
					iX=(iI-1)*iN+iJ-1; iY=(iI-1)*iN+iKk-1;
					pdV[iX]=pdV[iX]+dD*pdV[iY];
				}
			}
		}
		for (iI=1; iI<=iN; iI++)
		pdV[(iI-1)*iN+iKk-1]=0.0;
		pdV[iZ-iN]=1.0;
	}
	for (iI=1; iI<=iM; iI++)
		for (iJ=1; iJ<=iN; iJ++)
			pdA[(iI-1)*iN+iJ-1]=0.0;
	iM1=iMm; iIt=PCV_ITERATION_NUMBER;
	while (1==1)
	{ 
		if (iMm==0)
		{ 
			ppp(pdA,pdE,pdS,pdV,iM,iN);
			free(pdS); free(pdE); free(pdW); return(0);
		}
		if (iIt==0)
		{ 
			ppp(pdA,pdE,pdS,pdV,iM,iN);
			free(pdS); free(pdE); free(pdW); return(-1);
		}
		iKk=iMm-1;
		while ((iKk!=0)&&(fabs(pdE[iKk-1])!=0.0))
		{ 
			dD=fabs(pdS[iKk-1])+fabs(pdS[iKk]);
			dDd=fabs(pdE[iKk-1]);
			if (dDd>eps*dD) iKk=iKk-1;
			else pdE[iKk-1]=0.0;
		}
		if (iKk==iMm-1)
		{ 
			iKk=iKk+1;
			if (pdS[iKk-1]<0.0)
			{ 
				pdS[iKk-1]=-pdS[iKk-1];
				for (iI=1; iI<=iN; iI++)
				{ iX=(iI-1)*iN+iKk-1; pdV[iX]=-pdV[iX];}
			}
			while ((iKk!=iM1)&&(pdS[iKk-1]<pdS[iKk]))
			{ 
				dD=pdS[iKk-1]; pdS[iKk-1]=pdS[iKk]; pdS[iKk]=dD;
				if (iKk<iN)
				for (iI=1; iI<=iN; iI++)
				{ 
					iX=(iI-1)*iN+iKk-1; iY=(iI-1)*iN+iKk;
					dD=pdV[iX]; pdV[iX]=pdV[iY]; pdV[iY]=dD;
				}
				if (iKk<iM)
				for (iI=1; iI<=iM; iI++)
				{ 
					iX=(iI-1)*iM+iKk-1; iY=(iI-1)*iM+iKk;
					dD=pdU[iX]; pdU[iX]=pdU[iY]; pdU[iY]=dD;
				}
				iKk=iKk+1;
			}
			iIt=PCV_ITERATION_NUMBER;
			iMm=iMm-1;
		}
		else
		{ 
			iKs=iMm;
			while ((iKs>iKk)&&(fabs(pdS[iKs-1])!=0.0))
			{ 
				dD=0.0;
				if (iKs!=iMm) dD=dD+fabs(pdE[iKs-1]);
				if (iKs!=iKk+1) dD=dD+fabs(pdE[iKs-2]);
				dDd=fabs(pdS[iKs-1]);
				if (dDd>eps*dD) iKs=iKs-1;
				else pdS[iKs-1]=0.0;
			}
			if (iKs==iKk)
			{ 
				iKk=iKk+1;
				dD=fabs(pdS[iMm-1]);
				dT=fabs(pdS[iMm-2]);
				if (dT>dD) dD=dT;
				dT=fabs(pdE[iMm-2]);
				if (dT>dD) dD=dT;
				dT=fabs(pdS[iKk-1]);
				if (dT>dD) dD=dT;
				dT=fabs(pdE[iKk-1]);
				if (dT>dD) dD=dT;
				dSm=pdS[iMm-1]/dD; dSm1=pdS[iMm-2]/dD;
				dEm1=pdE[iMm-2]/dD;
				dSk=pdS[iKk-1]/dD; dEk=pdE[iKk-1]/dD;
				dB=((dSm1+dSm)*(dSm1-dSm)+dEm1*dEm1)/2.0;
				dC=dSm*dEm1; dC=dC*dC; dShh=0.0;
				if ((dB!=0.0)||(dC!=0.0))
				{ 
					dShh=sqrt(dB*dB+dC);
					if (dB<0.0) dShh=-dShh;
					dShh=dC/(dB+dShh);
				}
				darrFg[0]=(dSk+dSm)*(dSk-dSm)-dShh;
				darrFg[1]=dSk*dEk;
				for (iI=iKk; iI<=iMm-1; iI++)
				{ 
					sss(darrFg,darrCs);
					if (iI!=iKk) pdE[iI-2]=darrFg[0];
					darrFg[0]=darrCs[0]*pdS[iI-1]+darrCs[1]*pdE[iI-1];
					pdE[iI-1]=darrCs[0]*pdE[iI-1]-darrCs[1]*pdS[iI-1];
					darrFg[1]=darrCs[1]*pdS[iI];
					pdS[iI]=darrCs[0]*pdS[iI];
					if ((darrCs[0]!=1.0)||(darrCs[1]!=0.0))
					for (iJ=1; iJ<=iN; iJ++)
					{ 
						iX=(iJ-1)*iN+iI-1;
						iY=(iJ-1)*iN+iI;
						dD=darrCs[0]*pdV[iX]+darrCs[1]*pdV[iY];
						pdV[iY]=-darrCs[1]*pdV[iX]+darrCs[0]*pdV[iY];
						pdV[iX]=dD;
					}
					sss(darrFg,darrCs);
					pdS[iI-1]=darrFg[0];
					darrFg[0]=darrCs[0]*pdE[iI-1]+darrCs[1]*pdS[iI];
					pdS[iI]=-darrCs[1]*pdE[iI-1]+darrCs[0]*pdS[iI];
					darrFg[1]=darrCs[1]*pdE[iI];
					pdE[iI]=darrCs[0]*pdE[iI];
					if (iI<iM)
					if ((darrCs[0]!=1.0)||(darrCs[1]!=0.0))
					for (iJ=1; iJ<=iM; iJ++)
					{
						iX=(iJ-1)*iM+iI-1;
						iY=(iJ-1)*iM+iI;
						dD=darrCs[0]*pdU[iX]+darrCs[1]*pdU[iY];
						pdU[iY]=-darrCs[1]*pdU[iX]+darrCs[0]*pdU[iY];
						pdU[iX]=dD;
					}
				}
				pdE[iMm-2]=darrFg[0];
				iIt=iIt-1;
			}
			else
			{ 
				if (iKs==iMm)
				{
					iKk=iKk+1;
					darrFg[1]=pdE[iMm-2]; pdE[iMm-2]=0.0;
					for (iLl=iKk; iLl<=iMm-1; iLl++)
					{ 
						iI=iMm+iKk-iLl-1;
						darrFg[0]=pdS[iI-1];
						sss(darrFg,darrCs);
						pdS[iI-1]=darrFg[0];
						if (iI!=iKk)
						{ darrFg[1]=-darrCs[1]*pdE[iI-2];
						pdE[iI-2]=darrCs[0]*pdE[iI-2];
						}
						if ((darrCs[0]!=1.0)||(darrCs[1]!=0.0))
						for (iJ=1; iJ<=iN; iJ++)
						{
							iX=(iJ-1)*iN+iI-1;
							iY=(iJ-1)*iN+iMm-1;
							dD=darrCs[0]*pdV[iX]+darrCs[1]*pdV[iY];
							pdV[iY]=-darrCs[1]*pdV[iX]+darrCs[0]*pdV[iY];
							pdV[iX]=dD;
						}
					}
				}
				else
				{ 
					iKk=iKs+1;
					darrFg[1]=pdE[iKk-2];
					pdE[iKk-2]=0.0;
					for (iI=iKk; iI<=iMm; iI++)
					{
						darrFg[0]=pdS[iI-1];
						sss(darrFg,darrCs);
						pdS[iI-1]=darrFg[0];
						darrFg[1]=-darrCs[1]*pdE[iI-1];
						pdE[iI-1]=darrCs[0]*pdE[iI-1];
						if ((darrCs[0]!=1.0)||(darrCs[1]!=0.0))
						for (iJ=1; iJ<=iM; iJ++)
						{ 
							iX=(iJ-1)*iM+iI-1;
							iY=(iJ-1)*iM+iKk-2;
							dD=darrCs[0]*pdU[iX]+darrCs[1]*pdU[iY];
							pdU[iY]=-darrCs[1]*pdU[iX]+darrCs[0]*pdU[iY];
							pdU[iX]=dD;
						}
					}
				}
			}
		}
	}
	return(0);
}

//广义逆矩阵
INT MV_MatrixGeneralInv_D(LPMV_MATRIX_D psMatI,LPMV_MATRIX_D psMatO)
{
	DOUBLE *pdA = NULL;
	DOUBLE *pdAa = NULL;
	INT iM = 0,iN = 0;
	INT iI = 0,iJ = 0,iK = 0,iL = 0,iT = 0,iP = 0,iQ = 0,iF = 0;
	DOUBLE *pdU = NULL;
	DOUBLE *pdV = NULL;
	LPMV_MATRIX_D psMatL = NULL;
	LPMV_MATRIX_D psMatR = NULL;
	LPMV_MATRIX_D psMatS = NULL;

	if (!psMatI||!psMatO)
	{
		return(-1);
	}
	if (psMatI->iRows != psMatO->iCols)        //wxf
	{
		return(-1);
	}
	pdA = psMatI->pdData;
	pdAa = psMatO->pdData;
	iM = psMatI->iRows;
	iN = psMatI->iCols;

	psMatL = MV_MatricesZeros_D(iM,iM,1);
	if (!psMatL)
	{
		return(-1);
	}
	psMatR = MV_MatricesZeros_D(iN,iN,1);
	if (!psMatR)
	{
		MV_FreeMatrices_D(psMatL,1);
		return(-1);
	}

	psMatS = MV_MatricesZeros_D(iM,iN,1);
	if (!psMatS)
	{
		MV_FreeMatrices_D(psMatL,1);
		MV_FreeMatrices_D(psMatR,1);
		return(-1);
	}

	MV_MatrixCopy2_D(psMatI, psMatS);

	pdU = psMatL->pdData;
	pdV = psMatR->pdData;

	iI = MV_MatrixSVD_D(psMatS,psMatL,psMatR);

	if (iI < 0) 
	{
		MV_FreeMatrices_D(psMatL,1);
		MV_FreeMatrices_D(psMatR,1);
		MV_FreeMatrices_D(psMatS,1);
		return(-1);
	}

	pdA = psMatS->pdData;

	iJ = iN;
	if (iM < iN) {
		iJ = iM;
	}
	iJ = iJ - 1;
	iK = 0;
	while ((iK <= iJ)&&(pdA[iK*iN + iK] != 0.0)){
		iK = iK + 1;
	}
	iK = iK - 1;
	for (iI = 0; iI <= iN - 1; iI++)
		for (iJ = 0; iJ <= iM - 1; iJ++)
		{ 
			iT = iI*iM + iJ; 
			pdAa[iT] = 0.0;
			for (iL = 0; iL <= iK; iL++)
			{ 
				iF = iL*iN + iI; 
				iP =iJ*iM + iL; 
				iQ =iL*iN + iL;
				pdAa[iT] = pdAa[iT] + pdV[iF]*pdU[iP]/pdA[iQ];
			}
		}

	MV_FreeMatrices_D(psMatL,1);
	MV_FreeMatrices_D(psMatR,1);
	MV_FreeMatrices_D(psMatS,1);

	return(0);
}

//求矩阵行列式的值
DOUBLE MV_Matrixdet_D(LPMV_MATRIX_D psMat)
{ 
	DOUBLE *pdA = NULL;
	INT iN = 0;
	INT iI = 0,iJ = 0,iK = 0,iS = 0,iJs = 0,iL = 0,iU = 0,iV = 0;
	DOUBLE dF = 0,dDet = 0,dQ = 0,dD = 0;
	dF = 1.0; 
	dDet = 1.0;

	if (!psMat||psMat->iRows != psMat->iCols)
	{
		return(0);
	}
	pdA = psMat->pdData;
	iN = psMat->iRows;

	for (iK = 0; iK <= iN - 2; iK++)
	{
		dQ = 0.0;
		for (iI = iK; iI <= iN-1; iI++)
		{
			for (iJ = iK; iJ <= iN-1; iJ++)
			{
				iL = iI*iN + iJ;
				dD = fabs(pdA[iL]);
				if (dD > dQ)
				{ 
					dQ  = dD;
					iS = iI;
					iJs = iJ;
				}
			}
		}
		if (dQ + 1.0 == 1.0)
		{
			dDet = 0.0;
			return(dDet);
		}
		if (iS != iK)
		{
			dF=-dF;
			for (iJ = iK; iJ <= iN-1; iJ++)
			{
				iU = iK*iN + iJ;
				iV = iS*iN+ iJ;
				dD = pdA[iU];
				pdA[iU] = pdA[iV];
				pdA[iV] = dD;
			}
		}
		if (iJs != iK)
		{
			dF = -dF;
			for ( iI = iK; iI <= iN - 1; iI++)
			{
				iU = iI*iN + iJs;
				iV = iI*iN + iK;
				dD = pdA[iU];
				pdA[iU] = pdA[iV];
				pdA[iV] = dD;
			}
		}
		iL = iK*iN + iK;
		dDet = dDet*pdA[iL];
		for (iI = iK + 1; iI<= iN - 1; iI++)
		{
			dD=pdA[iI*iN + iK]/pdA[iL];
			for (iJ = iK + 1; iJ <= iN - 1; iJ++)
			{
				iU = iI*iN + iJ;
				pdA[iU] = pdA[iU]-dD*pdA[iK*iN+iJ];
			}
		}
	}

	dDet = dF*dDet*pdA[iN*iN-1];

	return(dDet);
}

LPMV_MATRIX_D MV_MatricesZeros_D(INT iRows,INT iCols,INT iNum)
{
	LPMV_MATRIX_D psMatrices = NULL;
	INT iI = 0;

	if(0 == iRows||0 == iCols)
	{
		return NULL;
	}

	psMatrices = (LPMV_MATRIX_D)malloc(sizeof(MV_MATRIX_D)*iNum);

	if (!psMatrices)
	{
		return NULL;
	}
	for (iI = 0;iI < iNum;iI++)
	{
		psMatrices[iI].iRows = iRows;
		psMatrices[iI].iCols = iCols;
		psMatrices[iI].iLength = iRows*iCols;
		psMatrices[iI].pdData = (DOUBLE*)malloc(psMatrices[iI].iLength*sizeof(DOUBLE));
		memset(psMatrices[iI].pdData,0,psMatrices[iI].iLength*sizeof(DOUBLE));
	}
	return psMatrices;
}

LPMV_MATRIX_D MV_MatricesOnes_D(INT iRows,INT iCols,INT iNum)
{
	LPMV_MATRIX_D psMatrices = NULL;
	DOUBLE *dataPtr = NULL;
	INT iI = 0;
	INT iJ = 0;

	if(0 == iRows||0 == iCols)
	{
		return NULL;
	}

	psMatrices = (LPMV_MATRIX_D)malloc(sizeof(MV_MATRIX_D)*iNum);

	if (!psMatrices)
	{
		return NULL;
	}

	for (iI = 0;iI < iNum;iI++)
	{
		psMatrices[iI].iRows = iRows;
		psMatrices[iI].iCols = iCols;
		psMatrices[iI].iLength = iRows*iCols;
		psMatrices[iI].pdData = (DOUBLE*)malloc(psMatrices[iI].iLength*sizeof(DOUBLE));
		dataPtr = psMatrices[iI].pdData;
		for (iJ = 0;iJ <  psMatrices[iI].iLength;iJ++)
		{
			*(dataPtr++) = 1.0;
		}
	}
	return psMatrices;
}

//释放矩阵
void MV_FreeMatrices_D(LPMV_MATRIX_D psMatrics,INT iNum)
{
	INT iI = 0;

	if(!psMatrics)
	{
		return;
	}

	for (iI = 0;iI < iNum;iI++)
	{
		if (psMatrics[iI].pdData)
		{
			free(psMatrics[iI].pdData);
			psMatrics[iI].pdData = NULL;
		}
	}

	free(psMatrics);

	return;
}

DOUBLE MV_MatrixGet_D(const LPMV_MATRIX_D psMatrix, INT iRow, INT iCol)
{
	if (NULL == psMatrix)
	{
		return 0;
	}

	return *(psMatrix->pdData + iRow * psMatrix->iCols + iCol);
}

void MV_MatrixSet_D(LPMV_MATRIX_D psMatrix, INT iRow, INT iCol, DOUBLE dData)
{
	if (NULL == psMatrix) {
		return;
	}

	*(psMatrix->pdData + iRow * psMatrix->iCols + iCol) = dData;

	return;
}

DOUBLE* MV_MatrixGetLinePtr_D(LPMV_MATRIX_D psMatrix, INT iRow)
{
	if (NULL == psMatrix) {
		return NULL;
	}

	return psMatrix->pdData + iRow * psMatrix->iCols;
}

LPMV_MATRIX_D MV_MatrixCopy_D(LPMV_MATRIX_D psSrc)
{
	LPMV_MATRIX_D psMat = NULL;

	if (NULL != psSrc) {
		psMat = MV_MatricesZeros_D(psSrc->iRows, psSrc->iCols, 1);

		if (psMat == NULL) {
			return NULL;
		}

		memcpy((void*)psMat->pdData, (void*)psSrc->pdData, psSrc->iLength*sizeof(DOUBLE));
	}

	return psMat;
}

void MV_MatrixCopy2_D(LPMV_MATRIX_D psSrc, LPMV_MATRIX_D psDst)
{
	INT iI = 0, iJ = 0;

	if ((NULL == psSrc) || (NULL == psDst)
		|| (psDst->iRows < psSrc->iRows) || (psDst->iCols < psSrc->iCols)) {
			return;
	}

	if (psSrc->iLength == psDst->iLength)
	{
		memcpy((void*)psDst->pdData, (void*)psSrc->pdData, psSrc->iLength*sizeof(DOUBLE));
	}
	else
	{
		for (iI = 0; iI < psSrc->iRows; iI++)
		{
			for (iJ = 0; iJ < psSrc->iCols; iJ++)
			{
				MV_MatrixSet_D(psDst, iI, iJ, MV_MatrixGet_D(psSrc, iI, iJ));
			}
		}
	}
}

void MV_PrintMatrix_D(LPMV_MATRIX_D psMat)
{
	INT iI = 0, iJ = 0;

	if (NULL != psMat) 
	{
		for (iI = 0; iI < psMat->iRows; iI++)
		{
			for (iJ = 0; iJ < psMat->iCols; iJ++)
			{
				printf("%13.9e\t", MV_MatrixGet_D(psMat, iI, iJ));
			}
			printf("\n");
		}
	}
}

/// <summary>
/// 求线性代数方程Ax=b的解
/// 本函数采用LU分解法：
/// 如果A=L U，其中L为下三角阵，U为上三角阵，则等价于求解L y = b, U x = y
/// </summary>
/// <param name="A">A的大小为iM*iM</param>
/// <param name="B">b的大小为iM*1</param>
/// <param name="x">【出参】x的大小为iM*1</param>
/// <param name="iM">指定A和B的大小</param>
/// <returns>成功返回0，失败返回-1</returns>
static INT LinearEquationLU(DOUBLE *A, DOUBLE *B, DOUBLE *x, INT iM)
{
	char *pchBuf = NULL;
	INT iBuf_sz = 0;
	INT iI = 0, iJ = 0, iK = 0;
	INT *piDx = NULL;
	INT iMaxi = 0, idx_sz = 0, iA_sz = 0, iWork_sz = 0, iTot_sz = 0;
	DOUBLE *pdA = NULL, *pdWork = NULL;
	DOUBLE dMax = 0, dSum = 0, dTmp = 0;

	idx_sz=iM;
	iA_sz=iM*iM;
	iWork_sz=iM;
	iTot_sz=idx_sz*sizeof(INT) + (iA_sz+iWork_sz)*sizeof(DOUBLE);

	iBuf_sz=iTot_sz;
	pchBuf=(char *)malloc(iTot_sz*sizeof(char));
	if(!pchBuf) return(-1);

	piDx=(INT *)pchBuf;
	pdA=(DOUBLE *)(piDx + idx_sz);
	pdWork=pdA + iA_sz;

	for(iI=0; iI<iM; ++iI)
	{
		pdA[iI]=A[iI];
		x[iI]=B[iI];
	}
	for(; iI<iA_sz; ++iI) pdA[iI]=A[iI];

	for(iI=0; iI<iM; ++iI)
	{
		dMax=0.0;
		for(iJ=0; iJ<iM; ++iJ)
			if((dTmp=fabs(pdA[iI*iM+iJ]))>dMax)
				dMax=dTmp;
		if(dMax==0.0)
		{
			free(pchBuf);
			return(-1);
		}
		pdWork[iI]=(1.0)/dMax;
	}

	for(iJ=0; iJ<iM; ++iJ)
	{
		for(iI=0; iI<iJ; ++iI)
		{
			dSum=pdA[iI*iM+iJ];
			for(iK=0; iK<iI; ++iK)
				dSum-=pdA[iI*iM+iK]*pdA[iK*iM+iJ];
			pdA[iI*iM+iJ]=dSum;
		}
		dMax=0.0;
		for(iI=iJ; iI<iM; ++iI)
		{
			dSum=pdA[iI*iM+iJ];
			for(iK=0; iK<iJ; ++iK)
				dSum-=pdA[iI*iM+iK]*pdA[iK*iM+iJ];
			pdA[iI*iM+iJ]=dSum;
			if((dTmp=pdWork[iI]*fabs(dSum))>=dMax)
			{
				dMax=dTmp;
				iMaxi=iI;
			}
		}
		if(iJ!=iMaxi)
		{
			for(iK=0; iK<iM; ++iK)
			{
				dTmp=pdA[iMaxi*iM+iK];
				pdA[iMaxi*iM+iK]=pdA[iJ*iM+iK];
				pdA[iJ*iM+iK]=dTmp;
			}
			pdWork[iMaxi]=pdWork[iJ];
		}
		piDx[iJ]=iMaxi;
		if(pdA[iJ*iM+iJ]==0.0)
			pdA[iJ*iM+iJ]=2.2204460492503131e-016;
		if(iJ!=iM-1)
		{
			dTmp=(1.0)/(pdA[iJ*iM+iJ]);
			for(iI=iJ+1; iI<iM; ++iI)
				pdA[iI*iM+iJ]*=dTmp;
		}
	}

	for(iI=iK=0; iI<iM; ++iI)
	{
		iJ=piDx[iI];
		dSum=x[iJ];
		x[iJ]=x[iI];
		if(iK!=0)
			for(iJ=iK-1; iJ<iI; ++iJ)
				dSum-=pdA[iI*iM+iJ]*x[iJ];
		else
			if(dSum!=0.0)
				iK=iI+1;
		x[iI]=dSum;
	}

	for(iI=iM-1; iI>=0; --iI)
	{
		dSum=x[iI];
		for(iJ=iI+1; iJ<iM; ++iJ)
			dSum-=pdA[iI*iM+iJ]*x[iJ];
		x[iI]=dSum/pdA[iI*iM+iI];
	}

	free(pchBuf);
	return(0);
}

/// <summary>
/// 有限差分近似法计算jacobian矩阵
/// </summary>
/// <param name="func">用于差分的函数</param>
/// <param name="iP">当前估计的参数，大小为iM*1</param>
/// <param name="hx">hx=func(iP)，大小为iN*1</param>
/// <param name="hxx">【入参/出参】作为缓存，用于计算函数(iP+dDelta)，大小为iN*1</param>
/// <param name="dDelta">用于计算jacobian阵的增量</param>
/// <param name="pdJac">【出参】近似得到的jacobian阵，大小iN*iM</param>
/// <param name="iM">指定矩阵的大小</param>
/// <param name="iM">指定矩阵的大小</param>
static void JacApproxDif(void (*func)(DOUBLE *pdP, DOUBLE *pdHx, INT iM, INT iN, DOUBLE *pdXdata1, DOUBLE *pdXdata2),
						 DOUBLE *pdP,DOUBLE *pdHx,DOUBLE *pdHxx,DOUBLE dDelta,DOUBLE *pdJac,INT iM,INT iN, DOUBLE *pdXdata1, DOUBLE *pdXdata2)
{
	INT iI, iJ;
	DOUBLE dTmp;
	DOUBLE dD;

	for(iJ=0; iJ<iM; ++iJ)
	{
		dD=(DOUBLE)(1E-04)*pdP[iJ];
		dD=fabs(dD);
		if(dD<dDelta)
			dD=dDelta;

		dTmp=pdP[iJ];
		pdP[iJ]+=dD;

		(*func)(pdP, pdHxx, iM, iN, pdXdata1,pdXdata2);

		pdP[iJ]=dTmp;

		dD=(1.0)/dD;
		for(iI=0; iI<iN; ++iI)
		{
			pdJac[iI*iM+iJ]=(pdHxx[iI]-pdHx[iI])*dD;
		}
	}
}

/// <summary>
/// 根据矩阵pdA（大小iN*iM），计算其转置矩阵pdA^T与自身pdA的乘积，结果为b=pdA^T pdA，b的大小为iM*iM
/// 由于pdA^Ta是对称阵，所以我们可以只计算其上三角阵，再将结果拷到下三角，用来加速
/// </summary>
/// <param name="pdA">pdA的大小为iN*iM</param>
/// <param name="b">【出参】b的大小为iM*iM</param>
/// <param name="iN">矩阵的大小</param>
/// <param name="iM">矩阵的大小</param>
/// <param name="bsize">pdA的buff大小</param>
static void MatrixTMul(DOUBLE *pdA, DOUBLE *pdB, INT iN, INT iM, INT iBsize)
{
	INT iI, iJ, iK, iJj, iKk;
	DOUBLE dSum, *pdBim, *pdAkm;

	for(iJj=0; iJj<iM; iJj+=iBsize)
	{
		for(iI=0; iI<iM; ++iI)
		{
			pdBim=pdB+iI*iM;
			for(iJ=max(iJj, iI); iJ<min(iJj+iBsize, iM); ++iJ)
				pdBim[iJ]=0.0;
		}

		for(iKk=0; iKk<iN; iKk+=iBsize)
		{
			for(iI=0; iI<iM; ++iI)
			{
				pdBim=pdB+iI*iM;
				for(iJ=max(iJj, iI); iJ<min(iJj+iBsize, iM); ++iJ)
				{
					dSum=0.0;
					for(iK=iKk; iK<min(iKk+iBsize, iN); ++iK)
					{
						pdAkm=pdA+iK*iM;
						dSum+=pdAkm[iI]*pdAkm[iJ];
					}
					pdBim[iJ]+=dSum;
				}
			}
		}
	}

	for(iI=0; iI<iM; ++iI)
		for(iJ=0; iJ<iI; ++iJ)
			pdB[iI*iM+iJ]=pdB[iJ*iM+iI];
}

/// <summary>
/// LM法求非线性最优化
/// 本函数求最适合测量向量y的参数向量iP
/// 更确切地说，给定一个向量函数func的映射：R^iM --> R^iN 其中（iN>=iM），
/// 本函数求得func(iP,xdata1,xdata2) ~= y，使e=x-func(iP,xdata1,xdata2)的平方和最小
/// </summary>
/// <param name="func">求最优值的函数</param>
/// <param name="iP">【入参】初始估计的参数，【出参】求得的解</param>
/// <param name="ydata">因变量向量</param>
/// <param name="iM">参数向量的个数</param>
/// <param name="iN">因变量的个数</param>
/// <param name="xdata1">自变量向量1</param>
/// <param name="xdata2">自变量向量2</param>
/// <returns>成功返回迭代次数(>=0)，失败返回-1</returns>
//INT lsqnonlin(void (*func)(DOUBLE *iP, DOUBLE *iF, INT iM, INT iN, DOUBLE *xdata1, DOUBLE *xdata2),
//			  DOUBLE *iP,DOUBLE *ydata,INT iM,INT iN,DOUBLE *xdata1,DOUBLE *xdata2)
INT lsqnonlin(void (*func)(DOUBLE *pdP, DOUBLE *pdF, INT iM, INT iN, DOUBLE *pdXdata1, DOUBLE *pdXdata2),
			  LPMV_MATRIX_D psMatH, LPMV_VECTOR_D psVecydata,LPMV_MATRIX_D psMatx, LPMV_MATRIX_D psMatxp)
{
	DOUBLE *pdP = NULL;
	DOUBLE *pdYdata = NULL;
	INT iM = 0, iN = 0;
	DOUBLE *pdXdata1 = NULL;
	DOUBLE *pdXdata2 = NULL;
    LPMV_VECTOR_D psVecf = NULL;

	INT ikmax = 100000;//最大迭代次数
	const DOUBLE dblMax = 1.7976931348623158e+308;
	const DOUBLE dblMin = 2.2250738585072014e-308;
	INT iI = 0, iJ = 0, iK = 0, iL = 0;
	INT iWorksz = 0, iSsolved = 0;
	DOUBLE *pdE = NULL, *pdF = NULL;
	DOUBLE *pdJacTe = NULL, *pdJac = NULL, *pdJacTjac = NULL, *pdDp = NULL, *pdDiag_jacTjac = NULL;
	DOUBLE *pDp = NULL, *pdWrk = NULL;
	DOUBLE dmu = 0, dtmp = 0;
	DOUBLE dp_eL2 = 0, dJacTe_inf = 0, dPDp_eL2 = 0;
	DOUBLE dP_L2 = 0, dDp_L2 = 0, dF = 0, dL = 0;
	DOUBLE dtau = 0, deps1 = 0, dEps2 = 0, dEps2_sq = 0, dDelta = 0;
	DOUBLE DInit_p_eL2 = 0;
	INT iNu = 0, iStop = 0, iNfev = 0, iNjap = 0, iKK = 0, iUpdjac = 0, iUpdp = 1, iNewjac = 0;
	INT iNm = 0;
	DOUBLE* pdWork = NULL;

	if(iN<iM) return -1;

	if (!psMatH||!psVecydata||!psMatx||!psMatxp)
	{
		return(-1);
	}

	if (psMatx->iRows != psMatxp->iRows||psMatx->iCols != psMatxp->iCols||psMatH->iRows != psMatH->iCols||psMatH->iRows != psMatx->iCols
		||psVecydata->iLength != psMatx->iRows)
	{
		return(-1);
	}
	pdP = psMatH->pdData;
	pdYdata = psVecydata->pdData;
	iM = psMatH->iLength;
    iN = psMatx->iRows;
	pdXdata1 = psMatx->pdData;
	pdXdata2 = psMatxp->pdData;
	psVecf = MV_VectorsZeros_D(iN,1);

	if (!psVecf)
	{
		return(-1);
	}

	pdF = psVecf->pdData;

	dDp_L2 = dblMax;
	iKK = (iM>=10)? iM: 10;
	iNm = iN*iM;


	dtau=(1E-03);
	deps1=(1E-17);
	dEps2=(1E-17);
	dEps2_sq=dEps2*dEps2;
	dDelta=(1E-06);

	iWorksz=3*iN+4*iM + iN*iM + iM*iM;
	pdWork=(DOUBLE*)malloc(iWorksz*sizeof(DOUBLE));
	if(!pdWork) {
		MV_FreeVectors_D(psVecf,1);
		return -1;
	}

	pdE=pdWork;
	//pdF=pdE + iN;
	pdJacTe=pdE + iN;
	pdJac=pdJacTe + iM;
	pdJacTjac=pdJac + iNm;
	pdDp=pdJacTjac + iM*iM;
	pdDiag_jacTjac=pdDp + iM;
	pDp=pdDiag_jacTjac + iM;
	pdWrk=pDp + iM;

	(*func)(psMatH->pdData, psVecf->pdData,iM,iN,psMatx->pdData, psMatxp->pdData);
	iNfev=1;
	for(iI=0, dp_eL2=0.0; iI<iN; ++iI)
	{
		pdE[iI]=dtmp=pdYdata[iI]-pdF[iI];
		dp_eL2+=dtmp*dtmp;
	}
	DInit_p_eL2=dp_eL2;

	iNu=20;

	for(iK=0; iK<ikmax; ++iK)
	{
		if((iUpdp && iNu>16) || iUpdjac==iKK)
		{
			JacApproxDif(func, pdP, pdF, pdWrk, dDelta, pdJac, iM, iN,pdXdata1,pdXdata2);
			++iNjap; iNfev+=iM;
			iNu=2; iUpdjac=0; iUpdp=0; iNewjac=1;
		}

		if(iNewjac)
		{
			iNewjac=0;

			if(iNm<=1024)
			{
				for(iI=0; iI<iM; ++iI)
				{
					for(iJ=iI; iJ<iM; ++iJ)
					{
						INT lm;

						for(iL=0, dtmp=0.0; iL<iN; ++iL)
						{
							lm=iL*iM;
							dtmp+=pdJac[lm+iI]*pdJac[lm+iJ];
						}

						pdJacTjac[iI*iM+iJ]=pdJacTjac[iJ*iM+iI]=dtmp;
					}

					for(iL=0, dtmp=0.0; iL<iN; ++iL)
						dtmp+=pdJac[iL*iM+iI]*pdE[iL];
					pdJacTe[iI]=dtmp;
				}
			}
			else
			{
				MatrixTMul(pdJac, pdJacTjac, iN, iM, 32);

				for(iI=0; iI<iM; ++iI)
					pdJacTe[iI]=0.0;

				for(iI=0; iI<iN; ++iI)
				{
					DOUBLE *jacrow;

					for(iL=0, jacrow=pdJac+iI*iM, dtmp=pdE[iI]; iL<iM; ++iL)
						pdJacTe[iL]+=jacrow[iL]*dtmp;
				}
			}

			for(iI=0, dP_L2=dJacTe_inf=0.0; iI<iM; ++iI)
			{
				if(dJacTe_inf < (dtmp=fabs(pdJacTe[iI]))) dJacTe_inf=dtmp;

				pdDiag_jacTjac[iI]=pdJacTjac[iI*iM+iI];
				dP_L2+=pdP[iI]*pdP[iI];
			}
		}

		if((dJacTe_inf <= deps1))
		{
			dDp_L2=0.0;
			iStop=1;
			break;
		}

		if(iK==0)
		{
			for(iI=0, dtmp=dblMin; iI<iM; ++iI)
				if(pdDiag_jacTjac[iI]>dtmp) dtmp=pdDiag_jacTjac[iI];
			dmu=dtau*dtmp;
		}
		for(iI=0; iI<iM; ++iI)
			pdJacTjac[iI*iM+iI]+=dmu;

		iSsolved=LinearEquationLU(pdJacTjac, pdJacTe, pdDp, iM);

		if(0 == iSsolved)
		{
			for(iI=0, dDp_L2=0.0; iI<iM; ++iI)
			{
				pDp[iI]=pdP[iI] + (dtmp=pdDp[iI]);
				dDp_L2+=dtmp*dtmp;
			}

			if(dDp_L2<=dEps2_sq*dP_L2)
			{
				iStop=2;
				break;
			}

			if(dDp_L2>=(dP_L2+dEps2)/((1E-12)*(1E-12)))
			{
				iStop=4;
				break;
			}

			(*func)(pDp, pdWrk, iM, iN, pdXdata1,pdXdata2); ++iNfev;
			for(iI=0, dPDp_eL2=0.0; iI<iN; ++iI)
			{
				dtmp=pdYdata[iI]-pdWrk[iI];
				dPDp_eL2+=dtmp*dtmp;
			}

			dF=dp_eL2-dPDp_eL2;
			if(iUpdp || dF>0)
			{
				for(iI=0; iI<iN; ++iI)
				{
					for(iL=0, dtmp=0.0; iL<iM; ++iL)
						dtmp+=pdJac[iI*iM+iL]*pdDp[iL];
					dtmp=(pdWrk[iI] - pdF[iI] - dtmp)/dDp_L2;
					for(iJ=0; iJ<iM; ++iJ)
						pdJac[iI*iM+iJ]+=dtmp*pdDp[iJ];
				}
				++iUpdjac;
				iNewjac=1;
			}

			for(iI=0, dL=0.0; iI<iM; ++iI)
				dL+=pdDp[iI]*(dmu*pdDp[iI]+pdJacTe[iI]);

			if(dL>0.0 && dF>0.0)
			{
				dF=((2.0)*dF/dL-(1.0));
				dtmp=dF*dF*dF;
				dtmp=(1.0)-dtmp*dtmp*dF;
				dmu=dmu*( (dtmp>=(0.3333333334))?dtmp:(0.3333333334));
				iNu=2;

				for(iI=0 ; iI<iM; ++iI)
					pdP[iI]=pDp[iI];

				for(iI=0; iI<iN; ++iI)
				{
					pdE[iI]=pdYdata[iI]-pdWrk[iI];
					pdF[iI]=pdWrk[iI];
				}
				dp_eL2=dPDp_eL2;
				iUpdp=1;
				continue;
			}
		}

		dmu*=iNu;
		iNu*=2;
		for(iI=0; iI<iM; ++iI)
			pdJacTjac[iI*iM+iI]=pdDiag_jacTjac[iI];
	}

	if(iK>=ikmax) iStop=3;
	free(pdWork);

	MV_FreeVectors_D(psVecf,1);

	return (iStop!=4)?  iK : -1;
}
