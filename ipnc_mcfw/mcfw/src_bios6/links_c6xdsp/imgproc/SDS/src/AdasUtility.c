/*
 * @(#)$Id: AdasUtility.cpp 685 2015-11-25 04:13:07Z LvGangXiao $
 * @(#)AdasUtility
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
//#include "stdafx.h"
#include "pcvMathFixed32.h"
#include "SDS_Detection.h"

#ifndef ADAS_UTILITY_H
#   include "AdasUtility.h"
#endif

VOID UTI_ForOverFlowChecking_Matrix(BOOL iIsIncreasingOrder_X, BOOL iIsIncreasingOrder_Y,
		                           BOOL* piValidFlag_X, BOOL* piValidFlag_Y,
								   INT* piStartX, INT* piEndX,
		                           INT* piStartY, INT* piEndY,
								   INT iDataLengthX, INT iDataLengthY)
	{
		//X----------------------------------
		*piValidFlag_X = TRUE;

		if (iIsIncreasingOrder_X == TRUE)
		{
			if (*piEndX < *piStartX)
			{
				*piValidFlag_X = FALSE;
			}
		}
		else
		{
			if (*piStartX < *piEndX)
			{
				*piValidFlag_X = FALSE;
			}
		}

		if (*piStartX < 0)
			*piStartX = 0;

		if (*piStartX >= iDataLengthX)
			*piStartX = iDataLengthX - 1;

		if (*piEndX < 0)
			*piEndX = 0;

		if (*piEndX >= iDataLengthX)
			*piEndX = iDataLengthX - 1;


		//Y----------------------------------
		*piValidFlag_Y = TRUE;

		if (iIsIncreasingOrder_Y == TRUE)
		{
			if (*piEndY < *piStartY)
			{
				*piValidFlag_Y = FALSE;
			}
		}
		else
		{
			if (*piStartY < *piEndY)
			{
				*piValidFlag_Y = FALSE;
			}
		}

		if (*piStartY < 0)
			*piStartY = 0;

		if (*piStartY >= iDataLengthY)
			*piStartY = iDataLengthY - 1;

		if (*piEndY < 0)
			*piEndY = 0;

		if (*piEndY >= iDataLengthY)
			*piEndY = iDataLengthY - 1;



	}

	BOOL UTI_ForOverFlowChecking_Array(BOOL iIsIncreasingOrder, INT* piStart, INT* piEnd,
		                                        INT iDataLength)
	{
		if (iIsIncreasingOrder == TRUE)
		{
			if (*piEnd < *piStart)
			{
				return FALSE;
			}
		}
		else
		{
			if (*piStart < *piEnd)
			{
				return FALSE;
			}
		}


		if (*piStart < 0)
			*piStart = 0;

		if (*piStart >= iDataLength)
			*piStart = iDataLength - 1;

		if (*piEnd < 0)
			*piEnd = 0;

		if (*piEnd >= iDataLength)
			*piEnd = iDataLength - 1;

		return TRUE;

	}


	VOID UTI_ForOverFlowChecking_Unit(INT* piIdx, INT iDataLength)
	{
		if (*piIdx < 0)
			*piIdx = 0;

		if (*piIdx >= iDataLength)
			*piIdx = iDataLength - 1;

	}


/*Get Time Counter */
LONGLONG UTI_GetTimeCounter()
{
	//=========Data definition=========//
	//LARGE_INTEGER iClockFrequency;

	//==========Implementation==========//
	//QueryPerformanceCounter(&iClockFrequency);
	//return iClockFrequency.QuadPart;
	return 0;
}


/* pow operation of the small value */
MV_FIXED UTI_MV_PowSmallValue(MV_FIXED iValue, MV_FIXED idx)
{
	return MV_Div(MV_Pow(MV_Mul(iValue, MV_FixedFromInt(100)), idx), MV_FixedFromInt(10000));
}


/* Compute Accumulation Graph */
VOID UTI_ComputeAccumGraph(LPMV_MATRIX_S psImageData, INT* pchAGData,
									INT iWidth, INT iHeight)
{
	//=========Data definition=========//
	INT iCount = 0;
	INT iCount2 = 0;
	INT iRow = 0;
	INT iCol = 0;
	INT iLength = iWidth * iHeight;

	//==========Implementation==========//
	//Copy data
	iCount = 0;
	for (iRow = 0; iRow < psImageData->iRows; iRow++)
	{
		MV_FIXED_S	*pImgStartLineAddr = (MV_FIXED_S	*)MV_MatrixGetLinePtr_S(psImageData, iRow, 0);

		for (iCol = 0; iCol < psImageData->iCols; iCol++)
		{
			//pchAGData[iCount] = MV_MatrixGet_S(psImageData, iRow, iCol);
			pchAGData[iCount] = pImgStartLineAddr[iCol];
			iCount++;
		}
	}

	//Accumulate row data
	for (iCount = 0; iCount < iLength - iWidth; iCount++)
	{
		pchAGData[iCount + iWidth] += pchAGData[iCount];
	}

	//Accumulate column data
	for (iCount2 = 0; iCount2 < iWidth - 1; iCount2++)
	{
		for (iCount = iCount2; iCount < iLength - 1; iCount+=iWidth)
		{
			pchAGData[iCount + 1] += pchAGData[iCount];
		}
	}
	
}//UTI_ComputeAccumGraph


/* Print Info To File */
 VOID UTI_PrintInfoToFile(MV_FILE *fp, CONST CHAR *format, ...)
{
	//=========Data definition=========//
	static CHAR buffer[PRNT_BUFF_SIZE];
	va_list	ap;

	//==========Implementation==========//
	va_start(ap, format);
	vsnprintf(buffer, PRNT_BUFF_SIZE, format, ap);
	va_end(ap);

	MV_WriteFile(buffer, strlen(buffer), fp);

}

 /* Print Info To Screen */
 VOID UTI_PrintInfoToScreen(CONST CHAR *format, ...)
 {
	 //=========Data definition=========//
	 static CHAR buffer[PRNT_BUFF_SIZE];
	 va_list	ap;

	 //==========Implementation==========//
	 va_start(ap, format);
	 vsnprintf(buffer, PRNT_BUFF_SIZE, format, ap);
	 va_end(ap);

	 MV_Printf1(buffer);

 }


 /* Print Info */
CHAR* UTI_PrintInfo(CONST CHAR *format, ...)
 {
	 //=========Data definition=========//
	 static CHAR buffer[PRNT_BUFF_SIZE];
	 va_list	ap;

	 //==========Implementation==========//
	 va_start(ap, format);
	 vsnprintf(buffer, PRNT_BUFF_SIZE, format, ap);
	 va_end(ap);

	return buffer;

 }

 /* GetROIMatrix */
BOOL UTI_GetROIMatrix(MV_MATRIX_S *pMatSrc, AdasImageROI *sRoi, INT iRoiCount, MV_MATRIX_S *pMatDst)
{
#if 1
	MV_FIXED_S	*pSrcAddr;
	MV_FIXED_S	*pDstAddr;
	unsigned width,height,src_offset,dst_offset;
	width = sRoi[0].iWidth;
	height = sRoi[0].iHeight;
	src_offset = pMatSrc->iPitch;
	dst_offset = pMatDst->iPitch;

	pSrcAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatSrc, sRoi[0].iY, sRoi[0].iX);
	pDstAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(&pMatDst[0], 0, 0);

	ImgLink_cacheWb((unsigned int)pDstAddr,pMatDst->iPitch*height);
	ImgLink_cacheWb((unsigned int)pSrcAddr,pMatSrc->iPitch*height);
	ImgAlgCpy((char *)pDstAddr,(char *)pSrcAddr,width,height,src_offset,dst_offset);
	ImgLink_cacheInv((unsigned int)pDstAddr,pMatDst->iPitch*height);
#endif
#if 0
	INT i, iRow, iCol;
	LPMV_MATRIX_S pchMVFrame = NULL;

	for (i = 0; i < iRoiCount; i++)
	{
		MV_FIXED_S	*pSrcLineAddr = NULL;
		MV_FIXED_S	*pDstLineAddr = NULL;

		pchMVFrame = &pMatDst[i];
		for (iRow = sRoi[i].iY; iRow < sRoi[i].iY + sRoi[i].iHeight; iRow++)
		{
			pSrcLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatSrc, iRow, 0);
			pDstLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pchMVFrame, iRow - sRoi[i].iY, 0);

			for (iCol = sRoi[i].iX; iCol < sRoi[i].iX + sRoi[i].iWidth; iCol++)
			{
				//MV_FIXED_S kk = MV_MatrixGet_S(pMatSrc, iRow, iCol);
				//MV_MatrixSet_S(pchMVFrame, iRow - sRoi[i].iY, iCol - sRoi[i].iX, kk);
				pDstLineAddr[iCol - sRoi[i].iX] = pSrcLineAddr[iCol];
			}
		}

	}
#endif
	return TRUE;
}//UTI_GetROIMatrix


/* GetROIMatrix2 */
BOOL UTI_GetROIMatrix2(MV_MATRIX_S *pMatSrc, AdasImageROI *sRoi, MV_MATRIX_S *pMatDst, INT iXCompressStep, INT iYCompressStep)
{
	INT iRow = MVZERO; 
	INT iCol = MVZERO; 
	INT iImgRow = MVZERO; 
	INT iImgCol = MVZERO; 

	iImgRow = 0;
	for (iRow = 0; iRow < pMatDst->iRows; iRow++)
	{
		MV_FIXED_S	*pSrcLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatSrc, iImgRow + sRoi->iY, 0);
		MV_FIXED_S	*pDstLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatDst, iRow, 0);
		iImgCol = 0;

		for (iCol = 0; iCol < pMatDst->iCols; iCol++)
		{
			pDstLineAddr[iCol] = pSrcLineAddr[iImgCol + sRoi->iX];
			iImgCol += iXCompressStep;
		}//iCol

		iImgRow += iYCompressStep;

	}//iRow

	return TRUE;
}//UTI_GetROIMatrix

/* GetROIMatrix3 */
BOOL UTI_GetROIMatrix3(MV_MATRIX_S *pMatSrc, AdasImageROI *sOrigRoi, AdasImageROI *sCompressRoi, MV_MATRIX_S *pMatDst, INT iXCompressStep, INT iYCompressStep)
{
	INT iRow = MVZERO; 
	INT iCol = MVZERO; 
	INT iImgRow = MVZERO; 
	INT iImgCol = MVZERO; 

	iImgRow = 0;
	for (iRow = 0; iRow < sCompressRoi->iHeight; iRow++)
	{

		MV_FIXED_S	*pSrcLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatSrc, iImgRow + sOrigRoi->iY, 0);
		MV_FIXED_S	*pDstLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatDst, iRow, 0);
		iImgCol = 0;

		for (iCol = 0; iCol < sCompressRoi->iWidth; iCol++)
		{
			pDstLineAddr[iCol] = pSrcLineAddr[iImgCol + sOrigRoi->iX];
			iImgCol += iXCompressStep;
		}//iCol

		iImgRow += iYCompressStep;

	}//iRow

	for (iRow = sCompressRoi->iHeight; iRow < pMatDst->iRows; iRow++)
	{
		MV_FIXED_S	*pDstLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatDst, iRow, 0);
		for (iCol = 0; iCol < pMatDst->iCols; iCol++)
		{
			pDstLineAddr[iCol] = 0;
		}//iCol
	}//iRow

	for (iRow = 0; iRow < sCompressRoi->iHeight; iRow++)
	{
		MV_FIXED_S	*pDstLineAddr = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(pMatDst, iRow, 0);
		for (iCol = sCompressRoi->iWidth; iCol < pMatDst->iCols; iCol++)
		{
			pDstLineAddr[iCol] = 0;
		}//iCol
	}//iRow

	return TRUE;
}//UTI_GetROIMatrix3


/* Compute Sum According To AG */
INT UTI_ComputeSumAccordingToAG(CONST INT *piAG, 
								INT iImgWidth,
								INT iImgHeight,
								AdasImageROI* psAGROI)
{
	//=========Data definition=========//
	AdasPoint sPoint = {MVZERO, MVZERO};
	INT iA = MVZERO; 
	INT iB = MVZERO; 
	INT iC = MVZERO; 
	INT iD = MVZERO; 

	//==========Implementation==========//
	//Check validity
	if (psAGROI->iX < 0
		|| psAGROI->iX + psAGROI->iWidth - 1 >= iImgWidth
		|| psAGROI->iY < 0
		|| psAGROI->iY + psAGROI->iHeight - 1 >= iImgHeight
		|| psAGROI->iWidth <= 0
		|| psAGROI->iHeight <= 0)
	{
		return 0;
	}

	//Compute A
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX < 0
		|| sPoint.iY < 0)
	{
		iA = 0;
	}
	else
	{
		iA = piAG[sPoint.iY * iImgWidth + sPoint.iX];
	}

	//Compute B
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX >= iImgWidth
		|| sPoint.iY < 0)
	{
		iB = 0;
	}
	else
	{
		iB = piAG[sPoint.iY * iImgWidth + sPoint.iX];
	}

	//Compute C
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX >= iImgWidth
		|| sPoint.iY >= iImgHeight)
	{
		iC = 0;
	}
	else
	{
		iC = piAG[sPoint.iY * iImgWidth + sPoint.iX];
	}

	//Compute D
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX < 0
		|| sPoint.iY >= iImgHeight)
	{
		iD = 0;
	}
	else
	{
		iD = piAG[sPoint.iY * iImgWidth + sPoint.iX];
	}

	return iA + iC - iB - iD;

}//UTI_ComputeSumAccordingToAG


/* Compute Sum According To AG */
INT UTI_ComputeSumAccordingToAG2(LPMV_MATRIX piAG,
								 AdasImageROI* psAGROI)
{
	//=========Data definition=========//
	AdasPoint sPoint = {MVZERO, MVZERO};
	INT iA = MVZERO; 
	INT iB = MVZERO; 
	INT iC = MVZERO; 
	INT iD = MVZERO; 

	//==========Implementation==========//
	//Check validity
	if (psAGROI->iX < 0
		|| psAGROI->iX + psAGROI->iWidth - 1 >= piAG->iCols
		|| psAGROI->iY < 0
		|| psAGROI->iY + psAGROI->iHeight - 1 >= piAG->iRows
		|| psAGROI->iWidth <= 0
		|| psAGROI->iHeight <= 0)
	{
		return 0;
	}

	//Compute A
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX < 0
		|| sPoint.iY < 0)
	{
		iA = 0;
	}
	else
	{
		iA = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	//Compute B
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX >= piAG->iCols
		|| sPoint.iY < 0)
	{
		iB = 0;
	}
	else
	{
		iB = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	//Compute C
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX >= piAG->iCols
		|| sPoint.iY >= piAG->iRows)
	{
		iC = 0;
	}
	else
	{
		iC = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	//Compute D
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX < 0
		|| sPoint.iY >= piAG->iRows)
	{
		iD = 0;
	}
	else
	{
		iD = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	return iA + iC - iB - iD;

}//UTI_ComputeSumAccordingToAG2

/* Compute Sum According To AG */
INT UTI_ComputeSumAccordingToAG4(LPMV_MATRIX piAG,
								 AdasImageROI* psAGROI)
{
	//=========Data definition=========//
	AdasPoint sPoint = {MVZERO, MVZERO};
	INT iA = MVZERO; 
	INT iB = MVZERO; 
	INT iC = MVZERO; 
	INT iD = MVZERO; 

	//==========Implementation==========//
	//Check validity
	if (psAGROI->iX < 0
		|| psAGROI->iX + psAGROI->iWidth - 1 >= piAG->iCols
		|| psAGROI->iY < 0
		|| psAGROI->iY + psAGROI->iHeight - 1 >= piAG->iRows
		|| psAGROI->iWidth <= 0
		|| psAGROI->iHeight <= 0)
	{
		return 0;
	}

	//Compute A
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX < 0
		|| sPoint.iY < 0)
	{
		iA = 0;
	}
	else
	{
		iA = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	//Compute B
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX >= piAG->iCols
		|| sPoint.iY < 0)
	{
		iB = 0;
	}
	else
	{
		iB = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	//Compute C
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX >= piAG->iCols
		|| sPoint.iY >= piAG->iRows)
	{
		iC = 0;
	}
	else
	{
		iC = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}


	//Compute D
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX < 0
		|| sPoint.iY >= piAG->iRows)
	{
		iD = 0;
	}
	else
	{
		iD = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	return iA + iC - iB - iD;

}//UTI_ComputeSumAccordingToAG4


/* Compute Sum According To AG for multi resolution */
INT UTI_ComputeSumAccordingToAG_MR(LPMV_MATRIX piAG,
								   AdasImageROI* psDetROI,
								   AdasImageROI* psAGROI)
{
	//=========Data definition=========//
	AdasPoint sPoint = {MVZERO, MVZERO};
	INT iA = MVZERO; 
	INT iB = MVZERO; 
	INT iC = MVZERO; 
	INT iD = MVZERO; 
	INT iDetROIWidth = MVZERO;
	INT iDetROIHeight = MVZERO;

	//==========Implementation==========//
	//Check validity
	iDetROIWidth = psDetROI->iX + psDetROI->iWidth;
	iDetROIHeight = psDetROI->iY + psDetROI->iHeight;

	if (psAGROI->iX < 0
		|| psAGROI->iX + psAGROI->iWidth - 1 >= iDetROIWidth
		|| psAGROI->iY < 0
		|| psAGROI->iY + psAGROI->iHeight - 1 >= iDetROIHeight
		|| psAGROI->iWidth <= 0
		|| psAGROI->iHeight <= 0)
	{
		return 0;
	}

	//Compute A
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX < 0
		|| sPoint.iY < 0)
	{
		iA = 0;
	}
	else
	{
		iA = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	//Compute B
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY - 1;
	if (sPoint.iX >= iDetROIWidth
		|| sPoint.iY < 0)
	{
		iB = 0;
	}
	else
	{
		iB = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	//Compute C
	sPoint.iX = psAGROI->iX + psAGROI->iWidth - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX >= iDetROIWidth
		|| sPoint.iY >= iDetROIHeight)
	{
		iC = 0;
	}
	else
	{
		iC = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}


	//Compute D
	sPoint.iX = psAGROI->iX - 1;
	sPoint.iY = psAGROI->iY + psAGROI->iHeight - 1;
	if (sPoint.iX < 0
		|| sPoint.iY >= iDetROIHeight)
	{
		iD = 0;
	}
	else
	{
		iD = *((MV_FIXED*)(((BYTE*)piAG->pDataAlign) + (sPoint.iY) * piAG->iPitch) + (sPoint.iX));
	}

	return iA + iC - iB - iD;

}//UTI_ComputeSumAccordingToAG_MR



/* Copy AdasImageROI */
VOID UTI_CopyAdasImageROI(AdasImageROI* psOrig, 
						  AdasImageROI* psDest
						  )
{
	//=========Data definition=========//

	//==========Implementation==========//
	psDest->iX = psOrig->iX;
	psDest->iY = psOrig->iY;
	psDest->iWidth = psOrig->iWidth;
	psDest->iHeight = psOrig->iHeight;

}//UTI_ComputeSumAccordingToAG


/* Compute Mean Value */
VOID UTI_ComputeMean_Int(BufferParams_Int* psDataList, INT iDatalength, INT* piMean, 
						 INT* piValidElementNum, BYTE* pWorkBuffer)
{
	//=========Data definition=========//
	INT iCount = MVZERO;
	INT iMean = MVZERO;
	INT iMean_BackUp = MVZERO;
	INT iIndex = MVZERO;
	INT iMeanDiff = MVZERO;
	BufferParams_Int sDiffList = {MVNULL, MVZERO};
	MV_FIXED fVar = MVZERO;

	//==========Implementation==========//
	//Initial some params

	//Allocate memory
	sDiffList.iDataNum = iDatalength;
	sDiffList.piBuffer = (INT*)(pWorkBuffer);

	//Compute mean value
	iMean = 0;
	for (iIndex = 0; iIndex < iDatalength; iIndex++)
	{
		iMean += psDataList->piBuffer[iIndex];
	}//for iIndex
	fVar = MV_Div(iMean, iDatalength);
	iMean = MV_IntFromFixed(fVar + MV_FixedFromFloat(0.5));
	iMean_BackUp = iMean;

	//Compute mean diff value
	iMeanDiff = 0;
	for (iIndex = 0; iIndex < iDatalength; iIndex++)
	{
		sDiffList.piBuffer[iIndex] = MV_Abs(psDataList->piBuffer[iIndex] - iMean);
		iMeanDiff += sDiffList.piBuffer[iIndex];
		
	}//for iIndex
	fVar = MV_Div(iMeanDiff, iDatalength);
	iMeanDiff = MV_IntFromFixed(fVar + MV_FixedFromFloat(0.5));

	//Compute final mean value
	iMean = 0;
	iCount = 0;
	for (iIndex = 0; iIndex < iDatalength; iIndex++)
	{
		if (sDiffList.piBuffer[iIndex] <= iMeanDiff)
		{
			iMean += psDataList->piBuffer[iIndex];
			iCount++;
		}
	}//for iIndex
	if (iCount > 0)
	{
		iMean /= iCount;
	}
	else
	{
		iMean = iMean_BackUp;
	}

	//Result result
	*piMean = iMean;
	*piValidElementNum = iCount;


}//UTI_ComputeMean_Int


/* Compute Mean Value */
VOID UTI_ComputeMean_Fixed(BufferParams_Fixed* psDataList, INT iDatalength, MV_FIXED* piMean, 
					  INT* piValidElementNum, BYTE* pWorkBuffer)
{
	//=========Data definition=========//
	INT iCount = MVZERO;
	MV_FIXED iMean = MVZERO;
	MV_FIXED iMean_BackUp = MVZERO;
	INT iIndex = MVZERO;
	MV_FIXED iMeanDiff = MVZERO;
	BufferParams_Fixed sDiffList = {MVNULL, MVZERO};

	//==========Implementation==========//
	//Initial some params
	if (iDatalength <= 0)
	{
		*piMean = 0;
		*piValidElementNum = 0;
		return;
	}

	//Allocate memory
	sDiffList.iDataNum = iDatalength;
	sDiffList.piBuffer = (MV_FIXED*)(pWorkBuffer);

	//Compute mean value
	iMean = 0;
	for (iIndex = 0; iIndex < iDatalength; iIndex++)
	{
		iMean += psDataList->piBuffer[iIndex];
	}//for iIndex
	iMean = MV_Div(iMean, MV_FixedFromInt(iDatalength));
	iMean_BackUp = iMean;

	//Compute mean diff value
	iMeanDiff = 0;
	for (iIndex = 0; iIndex < iDatalength; iIndex++)
	{
		sDiffList.piBuffer[iIndex] = MV_Abs(psDataList->piBuffer[iIndex] - iMean);
		iMeanDiff += sDiffList.piBuffer[iIndex];

	}//for iIndex
	iMeanDiff = MV_Div(iMeanDiff, MV_FixedFromInt(iDatalength));

	//Compute final mean value
	iMean = 0;
	iCount = 0;
	for (iIndex = 0; iIndex < iDatalength; iIndex++)
	{
		if (sDiffList.piBuffer[iIndex] <= iMeanDiff)
		{
			iMean += psDataList->piBuffer[iIndex];
			iCount++;
		}
	}//for iIndex
	if (iCount > 0)
	{
		iMean = MV_Div(iMean, MV_FixedFromInt(iCount));
	}
	else
	{
		iMean = iMean_BackUp;
	}

	//Result result
	*piMean = iMean;
	*piValidElementNum = iCount;


}//UTI_ComputeMean_Fixed

/*
 *
 * $Log$
 */
/* EOF */





