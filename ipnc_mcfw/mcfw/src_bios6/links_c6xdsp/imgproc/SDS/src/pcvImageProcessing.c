/*
 * @(#)$Id: pcvImageProcessing.c 2841 2012-09-14 02:47:18Z LvGangXiao $ 
 * @(#) Implementation file of class pcvImageProcessing
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file   
 * Implementation of pcvImageProcessing
 * 
 * TO DESC : FILE DETAIL DESCRIPTION, BUT DON'T DESCRIBE CLASS DETAIL HERE.
 */

#ifndef CXX_PCVIMAGEPROCESSING_H
#	include "pcvImageProcessing.h"
#endif

#ifndef CXX_MATHLIB32_H
#	include "pcvMathFixed32.h"
#endif

#ifndef CXX_PCVMISC_H
#	include "pcvMisc.h"
#endif

#ifndef CXX_MATHLIB_FLOAT_H
#	include "pcvMathFloat.h"
#endif

//#ifdef _C66X_
//#include "imglib.h"
//#endif

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

extern MV_FIXED fxtabSin[NUMANGLE];
extern MV_FIXED fxtabCos[NUMANGLE];
 LPMV_MATRIX_S g_sVertFilter;
 LPMV_MATRIX_S g_sHoriFilter;


VOID pcvHoughSortDescent32s(INT *array, INT total, const INT *aux);
VOID pcvConvolution(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift);
// for unit matrix convolution, directly to pMatDst
VOID pcvConvolution_Mid(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, SHORT iMaskSize, SHORT iShift);
VOID pcvConvolution_Int(CONST MV_FIXED_S *pInData, MV_FIXED_S *pOutData, 
						SHORT iWidth, SHORT iPitch, CONST MV_FIXED_S *pMask, 
						SHORT iMaskSize, SHORT iShift);
//this function is used internal ,it's base on the condition that the size of matrix is same as the size of raw data.
BOOL pcvCopyRawData2Matrix(pcvRawData* pRawData, LPMV_MATRIX_S pMatrix, enum PCVIMAGE_TYPE_SUB eSubType)
{
	BOOL bRet = FALSE;
	MV_FIXED_S *pDstMatrix = NULL;

	if (NULL == pRawData || NULL == pMatrix){
		return FALSE;
	}

	if ((pRawData->iWidth < pMatrix->iCols) || (pRawData->iHeight < pMatrix->iRows)) {
		return FALSE;
	}

	switch(eSubType){
	case PCVIMAGE_TYPE_SUB_Y:
		{
			if ((PCVIMAGE_FORMAT_NV21 == pRawData->iFormat) 
				|| (PCVIMAGE_FORMAT_PLYUV ==  pRawData->iFormat)){

				INT iRow = 0, iCol = 0;
				INT iHeight = pMatrix->iRows; 
				INT iWidth = pMatrix->iCols;

				BYTE* pYStart = pRawData->pData;

				for (iRow = 0; iRow < iHeight; ++iRow){
					pDstMatrix = MV_MatrixGetLinePtr_S(pMatrix, iRow, 0);

					for (iCol = 0; iCol < iWidth; ++iCol){
						//MV_MatrixSet_S(pMatrix, iRow, iCol, pYStart[iCol]);
						pDstMatrix[iCol] =  pYStart[iCol];
					}

					pYStart += pRawData->iPitch;
				}

				bRet = TRUE;
			}

		}
		break;
	case PCVIMAGE_TYPE_SUB_U:
		{
			INT iRow = 0, iCol = 0;
			INT iHeight = pMatrix->iRows; 
			INT iWidth = pMatrix->iCols;

			if (PCVIMAGE_FORMAT_NV21 == pRawData->iFormat){

				BYTE* pUStart = pRawData->pData + pRawData->iHeight * pRawData->iWidth + 1;

				for (iRow = 0; iRow < iHeight; ++iRow){
					pDstMatrix = MV_MatrixGetLinePtr_S(pMatrix, iRow, 0);

					for (iCol = 0; iCol < iWidth; ++iCol){

						//MV_MatrixSet_S(pMatrix, iRow, iCol, pUStart[(iCol/4) * 2]);
						pDstMatrix[iCol] =  pUStart[(iCol/4) * 2];
					}

					pUStart += iWidth/2;
				}

				bRet = TRUE;

			}else if (PCVIMAGE_FORMAT_PLYUV == pRawData->iFormat){

				/**
				* wll: pUStart is whether more offset?
				*      (because the same buffer as the case of PCVIMAGE_TYPE_SUB_V)
				*/
				BYTE* pUStart = pRawData->pData + pRawData->iHeight * pRawData->iWidth;

				for (iRow = 0; iRow < iHeight; ++iRow){
					pDstMatrix = MV_MatrixGetLinePtr_S(pMatrix, iRow, 0);

					for (iCol = 0; iCol < iWidth; ++iCol){

						//MV_MatrixSet_S(pMatrix, iRow, iCol, pUStart[(iCol/4)]);
						pDstMatrix[iCol] =  pUStart[(iCol/4)];
					}

					pUStart += iWidth/4;
				}

				bRet = TRUE;

			}else{

			}


		}
		break;
	case PCVIMAGE_TYPE_SUB_V:
		{
			INT iRow = 0, iCol = 0;
			INT iHeight = pMatrix->iRows; 
			INT iWidth = pMatrix->iCols;

			if (PCVIMAGE_FORMAT_NV21 == pRawData->iFormat){
				BYTE* pVStart = pRawData->pData + pRawData->iHeight * pRawData->iWidth;

				for (iRow = 0; iRow < iHeight; ++iRow){

					pDstMatrix = MV_MatrixGetLinePtr_S(pMatrix, iRow, 0);

					for (iCol = 0; iCol < iWidth; ++iCol){

						//MV_MatrixSet_S(pMatrix, iRow, iCol, pVStart[(iCol/4) * 2]);
						pDstMatrix[iCol] = pVStart[(iCol/4) * 2];
					}

					pVStart += iWidth/2;
				}

				bRet = TRUE;
			}else if (PCVIMAGE_FORMAT_PLYUV == pRawData->iFormat){

				BYTE* pVStart = pRawData->pData + pRawData->iHeight * pRawData->iWidth;

				for (iRow = 0; iRow < iHeight; ++iRow){

					pDstMatrix = MV_MatrixGetLinePtr_S(pMatrix, iRow, 0);

					for (iCol = 0; iCol < iWidth; ++iCol){

						//MV_MatrixSet_S(pMatrix, iRow, iCol, pVStart[(iCol/4)]);
						pDstMatrix[iCol] =  pVStart[(iCol/4)];
					}

					pVStart += iWidth/4;
				}

				bRet = TRUE;

			}else{

			}
		}
		break;
		//case PCVIMAGE_TYPE_SUB_H:
		//case PCVIMAGE_TYPE_SUB_S:
	default:
		break;
	}

	return bRet;
}

//pcvImage operations
BOOL pcvCreateImage(pcvImage *pImage, INT iWidth, INT iHeight/*, enum PCVIMAGE_FORMAT eFormat, BYTE* pSrc  */)
{
	BOOL bRet = FALSE;

	/**
	* wll: "pcvImage *pImage" or "pcvImage **pImage"?
	*      (passed by value or reference)
	*/
	if (NULL == pImage) {
		pImage = (pcvImage*)MV_Malloc(sizeof(pcvImage));

		if (NULL != pImage){
			INT iIdx = 0;
			for( iIdx = 0; iIdx < PCVIMAGE_TYPE_SUB_MAX; ++iIdx){
				pImage->pSubData[iIdx] = NULL;
				pImage->bSubDataValid[iIdx] = FALSE;
			}


			pImage->cRawData.pData = NULL;
			pImage->cRawData.iFormat = PCVIMAGE_FORMAT_MAX;
			pImage->cRawData.iWidth = iWidth;
			pImage->cRawData.iHeight = iHeight;
			pImage->cRawData.iPitch = 0;

			bRet = TRUE;

		}
	}

	return bRet;
}

BOOL pcvDestroyImage(pcvImage *pImage)
{
	BOOL bRet = FALSE;

	/**
	* wll: "pcvImage *pImage" or "pcvImage **pImage"?
	*/
	if (NULL != pImage){

		INT iIdx = 0;
		for( iIdx = 0; iIdx < PCVIMAGE_TYPE_SUB_MAX; ++iIdx){
			if (pImage->bSubDataValid[iIdx]){
				MV_FreeMatrices_S(pImage->pSubData[iIdx], 1);
			}
		}

		pImage->cRawData.iFormat = PCVIMAGE_FORMAT_MAX;
		pImage->cRawData.iWidth  = 0;
		pImage->cRawData.iHeight = 0;
		pImage->cRawData.iPitch  = 0;
		pImage->cRawData.pData	 = NULL;

		MV_Free(pImage);

		bRet = TRUE;
	}

	return bRet;
}

BOOL pcvSetDataByImage(pcvImage *pSrc, pcvImage *pDest, pcvRect *pROI)
{
	BOOL bRet = FALSE;

	if (NULL == pSrc || NULL == pDest || NULL == pROI){

		return FALSE;
	}

	if (pROI->x + pROI->width > pSrc->cRawData.iWidth || pROI->y + pROI->height > pSrc->cRawData.iHeight){
		return FALSE;
	}

	if (pROI->height != pDest->cRawData.iHeight || pROI->width != pDest->cRawData.iWidth){
		return FALSE;
	}

	pDest->cRawData.pData = pSrc->cRawData.pData + pROI->y * pSrc->cRawData.iPitch + pROI->x;
	pDest->cRawData.iFormat = pSrc->cRawData.iFormat;
	pDest->cRawData.iPitch = pSrc->cRawData.iPitch;

	bRet = TRUE;

	return bRet;
}

BOOL pcvSetDataByData(BYTE *pSrc, INT iWidth, INT iHeight, enum PCVIMAGE_FORMAT eFormat, pcvImage *pDest)
{
	BOOL bRet = FALSE;
	if (NULL == pSrc || NULL == pDest){
		return FALSE;
	}

	if (iWidth != pDest->cRawData.iWidth || iHeight != pDest->cRawData.iHeight){
		return FALSE;
	}

	pDest->cRawData.pData = pSrc;
	pDest->cRawData.iFormat = eFormat;
	pDest->cRawData.iPitch = iWidth * 2;

	bRet = TRUE;

	return bRet;
}


LPMV_MATRIX_S pcvGetImageMatrix(pcvImage *pImage, enum PCVIMAGE_TYPE_SUB eSubType)
{
	LPMV_MATRIX_S pMatrix = NULL;

	if (NULL != pImage){
		if (pImage->bSubDataValid[eSubType]){
			return pImage->pSubData[eSubType];	
		}
	}

	return pMatrix;
}


BOOL pcvMakeSubMatrix(pcvImage *pImage, enum PCVIMAGE_TYPE_SUB eSubType)
{
	BOOL bRet = FALSE;

	if (NULL != pImage){

		if (!pImage->bSubDataValid[eSubType]){

			if (NULL != pImage->cRawData.pData){

				pImage->pSubData[eSubType] = MV_CreateMatrices_S(pImage->cRawData.iHeight, pImage->cRawData.iWidth, 1, 8, 8);
				if (NULL == pImage->pSubData[eSubType]){
					return FALSE;
				}

				if (pcvCopyRawData2Matrix(&(pImage->cRawData), pImage->pSubData[eSubType], eSubType)){
					pImage->bSubDataValid[eSubType] =TRUE;
					bRet = TRUE;
				}

			}

		}else{
			bRet = TRUE;
		}
	}

	return bRet;
}



BOOL pcvIntergral(LPMV_MATRIX_S pSrc, LPMV_MATRIX pDest)
{

#ifdef  MV_USE_NEON
	return pcvIntergral_NEON(pSrc, pDest);
#else
	return pcvIntergral_NOFUNC(pSrc, pDest);
#endif

	/*INT iSrcRow, iSrcCol;
	INT iDestRow, iDestCol;
	INT iRowIdx, iColIdx;

	if ((NULL == pSrc) || (NULL == pDest)){
		return FALSE;
	}

	iSrcRow = pSrc->iRows;
	iSrcCol = pSrc->iCols;

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	if ((iSrcRow != iDestRow) || (iSrcCol != iDestCol)){
		return FALSE;
	}

	//first row Accumulation
	for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
		MV_FIXED_S sVal = MV_MatrixGet_S(pSrc, 0, iColIdx);
		MV_MatrixSet(pDest, 0, iColIdx, (MV_FIXED)sVal);
	}

	//the left row Accumulation
	for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

		for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){

			MV_FIXED_S sValCur = MV_MatrixGet_S(pSrc, iRowIdx, iColIdx);
			MV_FIXED iValPre = MV_MatrixGet(pDest, iRowIdx -1, iColIdx);

			MV_MatrixSet(pDest, iRowIdx, iColIdx, (MV_FIXED)(iValPre + sValCur));
		}
	}


	//the column Accumulation
	for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

		for(iColIdx = 1; iColIdx < iDestCol; ++iColIdx){

			MV_FIXED iValpre = MV_MatrixGet(pDest, iRowIdx, iColIdx - 1);
			MV_FIXED iValcur = MV_MatrixGet(pDest, iRowIdx, iColIdx);

			MV_MatrixSet(pDest, iRowIdx, iColIdx, (MV_FIXED)(iValpre + iValcur));
		}
	}

	return TRUE;*/
}

BOOL pcvIntergral_NOFUNC(LPMV_MATRIX_S pSrc, LPMV_MATRIX pDest)
{
	INT iSrcRow = 0, iSrcCol = 0;
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;
	MV_FIXED_S	*pSrcLineAddr = NULL;
	MV_FIXED	*pDestLineAddr = NULL;
	MV_FIXED	*pDestPreLineAddr = NULL;

	if ((NULL == pSrc) || (NULL == pDest)){
		return FALSE;
	}

	iSrcRow = pSrc->iRows;
	iSrcCol = pSrc->iCols;

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	if ((iSrcRow != iDestRow) || (iSrcCol != iDestCol)){
		return FALSE;
	}


	pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, 0, 0);
	pDestLineAddr = MV_MatrixGetLinePtr(pDest, 0);

	//first row Accumulation
	for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
		MV_FIXED_S sVal = *(pSrcLineAddr + iColIdx);
		*(pDestLineAddr + iColIdx) = (MV_FIXED)sVal;
	}

	//the left row Accumulation
	for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

		pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iRowIdx, 0);
		pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);
		pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);

		for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){

			MV_FIXED_S sValCur = *(pSrcLineAddr + iColIdx);
			MV_FIXED iValPre   = *(pDestPreLineAddr+ iColIdx);

			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + sValCur);
		}
	}


	//the column Accumulation
	for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

		pDestLineAddr =  MV_MatrixGetLinePtr(pDest, iRowIdx);;

		for(iColIdx = 1; iColIdx < iDestCol; ++iColIdx){

			MV_FIXED iValpre = *(pDestLineAddr + iColIdx - 1);
			MV_FIXED iValcur = *(pDestLineAddr + iColIdx);

			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValpre + iValcur);

		}
	}

	return TRUE;
}
BOOL pcvIntergral_NEON(LPMV_MATRIX_S pSrc, LPMV_MATRIX pDest)
{
	INT iSrcRow = 0, iSrcCol = 0;
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;
	MV_FIXED_S	*pSrcLineAddr = NULL;
	MV_FIXED	*pDestLineAddr = NULL;
	MV_FIXED	*pDestPreLineAddr = NULL;
	INT iLeftNum = 0;
	INT iLeftStartNum = 0;

	if ((NULL == pSrc) || (NULL == pDest)){
		return FALSE;
	}

	iSrcRow = pSrc->iRows;
	iSrcCol = pSrc->iCols;

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	if ((iSrcRow != iDestRow) || (iSrcCol != iDestCol)){
		return FALSE;
	}

	pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, 0, 0);
	pDestLineAddr = MV_MatrixGetLinePtr(pDest, 0);

	//first row Accumulation
	for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
		MV_FIXED_S sVal = *(pSrcLineAddr + iColIdx);
		*(pDestLineAddr + iColIdx) = (MV_FIXED)sVal;
	}

	//the left row Accumulation
	for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

		pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iRowIdx, 0);
		pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);
		pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);

		for(iColIdx = 0; iColIdx + 3 < iDestCol; iColIdx += 4){

#ifdef MV_USE_NEON
			int16x4_t src_vec_16 = vld1_s16((pSrcLineAddr + iColIdx));
			int32x4_t src_vec_32 = vmovl_s16(src_vec_16); 
			int32x4_t dest_preVec = vld1q_s32((pDestPreLineAddr+ iColIdx));
			int32x4_t dest_vec =  vaddq_s32(src_vec_32, dest_preVec);
			vst1q_s32(pDestLineAddr + iColIdx, dest_vec);
#else
			MV_FIXED_S sValCur = *(pSrcLineAddr + iColIdx);
			MV_FIXED iValPre   = *(pDestPreLineAddr+ iColIdx);
			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + sValCur);

			sValCur = *(pSrcLineAddr + iColIdx + 1);
			iValPre   = *(pDestPreLineAddr+ iColIdx + 1);
			*(pDestLineAddr + iColIdx + 1) = (MV_FIXED)(iValPre + sValCur);

			sValCur = *(pSrcLineAddr + iColIdx + 2);
			iValPre   = *(pDestPreLineAddr+ iColIdx + 2);
			*(pDestLineAddr + iColIdx + 2) = (MV_FIXED)(iValPre + sValCur);

			sValCur = *(pSrcLineAddr + iColIdx + 3);
			iValPre   = *(pDestPreLineAddr+ iColIdx + 3);
			*(pDestLineAddr + iColIdx + 3) = (MV_FIXED)(iValPre + sValCur);
#endif

		}
	}

	iLeftNum = (iDestCol) % 4;
	iLeftStartNum = iDestCol - iLeftNum;

	if (0!= iLeftNum){

		for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){
			pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iRowIdx, 0);
			pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);
			pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);

			for(iColIdx = iLeftStartNum; iColIdx < iDestCol; ++iColIdx){

				MV_FIXED_S sValCur = *(pSrcLineAddr + iColIdx);
				MV_FIXED iValPre   = *(pDestPreLineAddr+ iColIdx);
				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + sValCur);
			}
		}
	}


	//the column Accumulation
	for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

		pDestLineAddr =  MV_MatrixGetLinePtr(pDest, iRowIdx);;

		for(iColIdx = 1; iColIdx + 4 <= iDestCol; iColIdx += 4){
/*
#ifdef PCV_USE_NEON
			int16x4_t src_vec_16 = vld1_s16((pSrcLineAddr + iColIdx));
			int32x4_t src_vec_32 = vmovl_s16(src_vec_16); 
			int32x4_t dest_preVec = vld1q_s32((pDestPreLineAddr+ iColIdx - 1));
			int32x4_t dest_CurVec = vld1q_s32((pDestPreLineAddr+ iColIdx));


			int32x4_t dest_vec =  vaddq_s32(src_vec_32, dest_preVec);
			vst1q_s32(pDestLineAddr, dest_vec);

*/

			MV_FIXED iValpre = *(pDestLineAddr + iColIdx - 1);
			MV_FIXED iValcur = *(pDestLineAddr + iColIdx);
			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValpre + iValcur);

			iValpre = *(pDestLineAddr + iColIdx );
			iValcur = *(pDestLineAddr + iColIdx + 1);
			*(pDestLineAddr + iColIdx + 1) = (MV_FIXED)(iValpre + iValcur);

			iValpre = *(pDestLineAddr + iColIdx + 1);
			iValcur = *(pDestLineAddr + iColIdx + 2);
			*(pDestLineAddr + iColIdx + 2) = (MV_FIXED)(iValpre + iValcur);

			iValpre = *(pDestLineAddr + iColIdx + 2);
			iValcur = *(pDestLineAddr + iColIdx + 3);
			*(pDestLineAddr + iColIdx + 3) = (MV_FIXED)(iValpre + iValcur);

		}

	}

	iLeftNum = (iDestCol - 1) % 4;
	iLeftStartNum = iDestCol - iLeftNum;

	if (0 != iLeftNum){
		//the column Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){
			pDestLineAddr =  MV_MatrixGetLinePtr(pDest, iRowIdx);
			for(iColIdx = iLeftStartNum ; iColIdx < iDestCol; ++iColIdx){
				MV_FIXED iValpre = *(pDestLineAddr + iColIdx - 1);
				MV_FIXED iValcur = *(pDestLineAddr + iColIdx);
				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValpre + iValcur);
			}
		}
	}

	return TRUE;
}


BOOL pcvIntergral_ROI(LPMV_MATRIX_S pSrc, pcvRect* rROI, LPMV_MATRIX pDest)
{
#ifdef  MV_USE_NEON
	return pcvIntergral_ROI_NEON(pSrc, rROI, pDest);
#else
	return pcvIntergral_ROI_NOFUNC(pSrc, rROI, pDest);
#endif
}

BOOL pcvIntergral_ROI_NOFUNC(LPMV_MATRIX_S pSrc, pcvRect* rROI, LPMV_MATRIX pDest)
{
	INT iSrcRow = 0, iSrcCol = 0;
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;
	INT iROItopX = 0, iROItopY = 0, iROIbtmX = 0, iROIbtmY = 0;

	MV_FIXED_S	*pSrcLineAddr = NULL;
	MV_FIXED	*pDestLineAddr = NULL;
	MV_FIXED	*pDestPreLineAddr = NULL;

	if ((NULL == pSrc) || (NULL == pDest) || (NULL == rROI)){
		return FALSE;
	}

	iSrcRow = pSrc->iRows;
	iSrcCol = pSrc->iCols;

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	iROItopX = rROI->x;
	iROItopY = rROI->y;

	iROIbtmX = iROItopX + rROI->width;
	iROIbtmY = iROItopY + rROI->height;

	if ((rROI->width > iDestCol) || (rROI->height > iDestRow)
		|| (iROIbtmX > pSrc->iCols) || (iROIbtmY > pSrc->iRows)) {
		return FALSE;
	}

	iDestRow =  rROI->height;
	iDestCol =  rROI->width;

	pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iROItopY, 0);
	pDestLineAddr = MV_MatrixGetLinePtr(pDest, 0);

	//first row Accumulation
	for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
		MV_FIXED_S sVal = *(pSrcLineAddr + iColIdx + iROItopX);
		*(pDestLineAddr + iColIdx) = (MV_FIXED)sVal;
	}

	//the left row Accumulation
	for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

		pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iRowIdx + iROItopY, 0);
		pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);
		pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);

		for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){

			MV_FIXED_S sValCur = *(pSrcLineAddr + iColIdx + iROItopX);
			MV_FIXED iValPre   = *(pDestPreLineAddr+ iColIdx);

			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + sValCur);
		}
	}


	//the column Accumulation
	for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

		pDestLineAddr =  MV_MatrixGetLinePtr(pDest, iRowIdx);;

		for(iColIdx = 1; iColIdx < iDestCol; ++iColIdx){

			MV_FIXED iValpre = *(pDestLineAddr + iColIdx - 1);
			MV_FIXED iValcur = *(pDestLineAddr + iColIdx);

			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValpre + iValcur);

		}
	}

	return TRUE;
}

BOOL pcvIntergral_ROI_NEON(LPMV_MATRIX_S pSrc, pcvRect* rROI, LPMV_MATRIX pDest)
{
	INT iSrcRow = 0, iSrcCol = 0;
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;
	INT iROItopX = 0, iROItopY = 0, iROIbtmX = 0, iROIbtmY = 0;
	MV_FIXED_S	*pSrcLineAddr = NULL;
	MV_FIXED	*pDestLineAddr = NULL;
	MV_FIXED	*pDestPreLineAddr = NULL;
	INT iLeftNum = 0;
	INT iLeftStartNum = 0;

	if ((NULL == pSrc) || (NULL == pDest) || (NULL == rROI)){
		return FALSE;
	}

	iSrcRow = pSrc->iRows;
	iSrcCol = pSrc->iCols;

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	iROItopX = rROI->x;
	iROItopY = rROI->y;

	iROIbtmX = iROItopX + rROI->width;
	iROIbtmY = iROItopY + rROI->height;

	if ((rROI->width > iDestCol) || (rROI->height > iDestRow)
		|| (iROIbtmX > pSrc->iCols) || (iROIbtmY > pSrc->iRows)) {
		return FALSE;
	}

	iDestRow =  rROI->height;
	iDestCol =  rROI->width;

	pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iROItopY, 0);
	pDestLineAddr = MV_MatrixGetLinePtr(pDest, 0);

	//first row Accumulation
	for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
		MV_FIXED_S sVal = *(pSrcLineAddr + iColIdx + iROItopX);
		*(pDestLineAddr + iColIdx) = (MV_FIXED)sVal;
	}

	//the left row Accumulation
	for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

		pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iRowIdx + iROItopY, 0);
		pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);
		pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);

		for(iColIdx = 0; iColIdx + 3 < iDestCol; iColIdx += 4){

#ifdef MV_USE_NEON
			int16x4_t src_vec_16 = vld1_s16((pSrcLineAddr + iColIdx + iROItopX));
			int32x4_t src_vec_32 = vmovl_s16(src_vec_16); 
			int32x4_t dest_preVec = vld1q_s32((pDestPreLineAddr+ iColIdx));
			int32x4_t dest_vec =  vaddq_s32(src_vec_32, dest_preVec);
			vst1q_s32(pDestLineAddr + iColIdx, dest_vec);
#else
			MV_FIXED_S sValCur = *(pSrcLineAddr + iColIdx + iROItopX);
			MV_FIXED iValPre   = *(pDestPreLineAddr+ iColIdx);
			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + sValCur);

			sValCur = *(pSrcLineAddr + iColIdx + 1 + iROItopX);
			iValPre   = *(pDestPreLineAddr+ iColIdx + 1);
			*(pDestLineAddr + iColIdx + 1) = (MV_FIXED)(iValPre + sValCur);

			sValCur = *(pSrcLineAddr + iColIdx + 2 + iROItopX);
			iValPre   = *(pDestPreLineAddr+ iColIdx + 2);
			*(pDestLineAddr + iColIdx + 2) = (MV_FIXED)(iValPre + sValCur);

			sValCur = *(pSrcLineAddr + iColIdx + 3 + iROItopX);
			iValPre   = *(pDestPreLineAddr+ iColIdx + 3);
			*(pDestLineAddr + iColIdx + 3) = (MV_FIXED)(iValPre + sValCur);
#endif

		}
	}

	iLeftNum = (iDestCol) % 4;
	iLeftStartNum = iDestCol - iLeftNum;

	if (0!= iLeftNum){

		for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){
			pSrcLineAddr = MV_MatrixGetLinePtr_S(pSrc, iRowIdx + iROItopY, 0);
			pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);
			pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);

			for(iColIdx = iLeftStartNum; iColIdx < iDestCol; ++iColIdx){

				MV_FIXED_S sValCur = *(pSrcLineAddr + iColIdx + iROItopX);
				MV_FIXED iValPre   = *(pDestPreLineAddr+ iColIdx);
				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + sValCur);
			}
		}
	}


	//the column Accumulation
	for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

		pDestLineAddr =  MV_MatrixGetLinePtr(pDest, iRowIdx);;

		for(iColIdx = 1; iColIdx + 4 <= iDestCol; iColIdx += 4){
			/*
			#ifdef PCV_USE_NEON
			int16x4_t src_vec_16 = vld1_s16((pSrcLineAddr + iColIdx));
			int32x4_t src_vec_32 = vmovl_s16(src_vec_16); 
			int32x4_t dest_preVec = vld1q_s32((pDestPreLineAddr+ iColIdx - 1));
			int32x4_t dest_CurVec = vld1q_s32((pDestPreLineAddr+ iColIdx));


			int32x4_t dest_vec =  vaddq_s32(src_vec_32, dest_preVec);
			vst1q_s32(pDestLineAddr, dest_vec);

			*/

			MV_FIXED iValpre = *(pDestLineAddr + iColIdx - 1);
			MV_FIXED iValcur = *(pDestLineAddr + iColIdx);
			*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValpre + iValcur);

			iValpre = *(pDestLineAddr + iColIdx );
			iValcur = *(pDestLineAddr + iColIdx + 1);
			*(pDestLineAddr + iColIdx + 1) = (MV_FIXED)(iValpre + iValcur);

			iValpre = *(pDestLineAddr + iColIdx + 1);
			iValcur = *(pDestLineAddr + iColIdx + 2);
			*(pDestLineAddr + iColIdx + 2) = (MV_FIXED)(iValpre + iValcur);

			iValpre = *(pDestLineAddr + iColIdx + 2);
			iValcur = *(pDestLineAddr + iColIdx + 3);
			*(pDestLineAddr + iColIdx + 3) = (MV_FIXED)(iValpre + iValcur);

		}

	}

	iLeftNum = (iDestCol - 1) % 4;
	iLeftStartNum = iDestCol - iLeftNum;

	if (0 != iLeftNum){
		//the column Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){
			pDestLineAddr =  MV_MatrixGetLinePtr(pDest, iRowIdx);
			for(iColIdx = iLeftStartNum ; iColIdx < iDestCol; ++iColIdx){
				MV_FIXED iValpre = *(pDestLineAddr + iColIdx - 1);
				MV_FIXED iValcur = *(pDestLineAddr + iColIdx);
				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValpre + iValcur);
			}
		}
	}

	return TRUE;
}


BOOL pcvLineIntergral(LPMV_MATRIX pSrc, INT iStep, enum PCV_DIRECTION eDirect, LPMV_MATRIX pDest)
{

#ifdef  MV_USE_NEON
	return pcvLineIntergral_NEON(pSrc, iStep, eDirect, pDest);
#else
	return pcvLineIntergral_NOFUNC(pSrc, iStep, eDirect, pDest);
#endif

	/*BOOL bRet = FALSE;

	INT iDestRow, iDestCol;
	INT iRowIdx, iColIdx;

	if ((NULL == pSrc) || (NULL == pDest)){
		return FALSE;
	}

	if (iStep <= 0){
		return FALSE;
	}

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	//PCV_INTERG_Y :column Accumulation
	if (PCV_DIRT_Y == eDirect){

		//first line Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){
			MV_FIXED iVal = MV_MatrixGet(pSrc, iRowIdx, 0);
			MV_MatrixSet(pDest, iRowIdx, 0, (MV_FIXED)iVal);
		}

		//the left row Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){
			for(iColIdx = 1; iColIdx < iDestCol; ++iColIdx){

				MV_FIXED iValCur = MV_MatrixGet(pSrc, iRowIdx, iColIdx * iStep);
				MV_FIXED iValPre = MV_MatrixGet(pDest, iRowIdx, iColIdx - 1);

				MV_MatrixSet(pDest, iRowIdx, iColIdx, (MV_FIXED)(iValPre + iValCur));
			}
		}


	}else{
		//first row Accumulation
		for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
			MV_FIXED iVal = MV_MatrixGet(pSrc, 0, iColIdx);
			MV_MatrixSet(pDest, 0, iColIdx, (MV_FIXED)iVal);
		}

		//the left row Accumulation
		for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

			for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){

				MV_FIXED iValCur = MV_MatrixGet(pSrc, iRowIdx * iStep, iColIdx);
				MV_FIXED iValPre = MV_MatrixGet(pDest, iRowIdx - 1, iColIdx);

				MV_MatrixSet(pDest, iRowIdx , iColIdx, (MV_FIXED)(iValPre + iValCur));
			}
		}

	}

	return TRUE;*/
}


BOOL pcvLineIntergral_NOFUNC(LPMV_MATRIX pSrc, INT iStep, enum PCV_DIRECTION eDirect, LPMV_MATRIX pDest)
{
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;

	MV_FIXED	*pDestLineAddr = NULL;
	MV_FIXED	*pDestPreLineAddr = NULL;
	MV_FIXED	*pSrcLineAddr = NULL;

	if ((NULL == pSrc) || (NULL == pDest) || (pDest->iRows > pSrc->iRows)
		|| (pDest->iCols > pSrc->iCols)) {
		return FALSE;
	}

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	if ((iStep <= 0) || (iStep * (iDestRow - 1) >= pSrc->iRows) 
		|| (iStep * (iDestCol - 1) >= pSrc->iCols)) {
		return FALSE;
	}

	//PCV_INTERG_Y :column Accumulation
	if (PCV_DIRT_Y == eDirect){

		//first line Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){
			MV_FIXED iVal = MV_MatrixGet(pSrc, iRowIdx, 0);
			MV_MatrixSet(pDest, iRowIdx, 0, (MV_FIXED)iVal);
		}

		//the left row Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

			pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, iRowIdx);
			pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);

			for(iColIdx = 1; iColIdx < iDestCol; ++iColIdx){

				MV_FIXED iValCur = *(pSrcLineAddr + iColIdx * iStep);
				MV_FIXED iValPre = *(pDestLineAddr + iColIdx - 1);

				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + iValCur);
			}
		}


	}else{

		pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, 0);
		pDestLineAddr = MV_MatrixGetLinePtr(pDest, 0);

		//first row Accumulation
		for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
			*(pDestLineAddr + iColIdx) = *(pSrcLineAddr + iColIdx);
		}

		//the left row Accumulation
		for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

			pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, iRowIdx * iStep);
			pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);
			pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);

			for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){

				MV_FIXED iValCur = *(pSrcLineAddr + iColIdx);
				MV_FIXED iValPre = *(pDestPreLineAddr + iColIdx);

				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + iValCur);
			}
		}

	}

	return TRUE;
}


BOOL pcvLineIntergral_NEON(LPMV_MATRIX pSrc, INT iStep, enum PCV_DIRECTION eDirect, LPMV_MATRIX pDest)
{
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;

	INT iLeftNum = 0;
	INT iLeftStartNum = 0;

	MV_FIXED	*pDestLineAddr = NULL;
	MV_FIXED	*pDestPreLineAddr = NULL;
	MV_FIXED	*pSrcLineAddr = NULL;

	if ((NULL == pSrc) || (NULL == pDest)
		|| (pDest->iRows > pSrc->iRows) || (pDest->iCols > pSrc->iCols)) {
		return FALSE;
	}

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	if ((iStep <= 0) || (iStep * (iDestRow - 1) >= pSrc->iRows) 
		|| (iStep * (iDestCol - 1) >= pSrc->iCols)){
		return FALSE;
	}

	//PCV_INTERG_Y :column Accumulation
	if (PCV_DIRT_Y == eDirect){

		//first line Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){
			MV_FIXED iVal = MV_MatrixGet(pSrc, iRowIdx, 0);
			MV_MatrixSet(pDest, iRowIdx, 0, (MV_FIXED)iVal);
		}

		//the left row Accumulation
		for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

			pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, iRowIdx);
			pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);

			for(iColIdx = 1; iColIdx + 4<= iDestCol; iColIdx += 4){

				MV_FIXED iValCur = *(pSrcLineAddr + iColIdx * iStep);
				MV_FIXED iValPre = *(pDestLineAddr + iColIdx - 1);
				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + iValCur);

				iValCur = *(pSrcLineAddr + (iColIdx + 1) * iStep);
				iValPre = *(pDestLineAddr + iColIdx);
				*(pDestLineAddr + iColIdx + 1) = (MV_FIXED)(iValPre + iValCur);

				iValCur = *(pSrcLineAddr + (iColIdx + 2) * iStep);
				iValPre = *(pDestLineAddr + iColIdx + 1);
				*(pDestLineAddr + iColIdx + 2) = (MV_FIXED)(iValPre + iValCur);

				iValCur = *(pSrcLineAddr + (iColIdx + 3 ) * iStep);
				iValPre = *(pDestLineAddr + iColIdx + 2);
				*(pDestLineAddr + iColIdx + 3) = (MV_FIXED)(iValPre + iValCur);
			}
		}

		iLeftNum = (iDestCol - 1) % 4;
		iLeftStartNum = iDestCol - iLeftNum;

		//the left column Accumulation
		if (0 != iLeftNum){			
			for(iRowIdx = 0; iRowIdx < iDestRow; ++iRowIdx){

				pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, iRowIdx);
				pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);

				for(iColIdx = iLeftStartNum ; iColIdx < iDestCol; ++iColIdx){

					MV_FIXED iValCur = *(pSrcLineAddr + iColIdx * iStep);
					MV_FIXED iValPre = *(pDestLineAddr + iColIdx - 1);
					*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + iValCur);
				}
			}
		}



	}else{

		pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, 0);
		pDestLineAddr = MV_MatrixGetLinePtr(pDest, 0);

		//first row Accumulation
		for(iColIdx = 0; iColIdx < iDestCol; ++iColIdx){
			*(pDestLineAddr + iColIdx) = *(pSrcLineAddr + iColIdx);
		}

		//the left row Accumulation
		for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

			pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, iRowIdx * iStep);
			pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);
			pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);

			for(iColIdx = 0; iColIdx + 3 < iDestCol; iColIdx += 4){
#ifdef MV_USE_NEON
				int32x4_t src_vec_32 =  vld1q_s32(pSrcLineAddr + iColIdx); 
				int32x4_t dest_preVec = vld1q_s32((pDestPreLineAddr+ iColIdx));
				int32x4_t dest_vec =  vaddq_s32(src_vec_32, dest_preVec);
				vst1q_s32(pDestLineAddr + iColIdx, dest_vec);
#else

				MV_FIXED iValCur = *(pSrcLineAddr + iColIdx);
				MV_FIXED iValPre = *(pDestPreLineAddr + iColIdx);
				*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + iValCur);

				iValCur = *(pSrcLineAddr + iColIdx + 1);
				iValPre = *(pDestPreLineAddr + iColIdx + 1);
				*(pDestLineAddr + iColIdx + 1) = (MV_FIXED)(iValPre + iValCur);

				iValCur = *(pSrcLineAddr + iColIdx + 2);
				iValPre = *(pDestPreLineAddr + iColIdx + 2);
				*(pDestLineAddr + iColIdx + 2) = (MV_FIXED)(iValPre + iValCur);

				iValCur = *(pSrcLineAddr + iColIdx + 3);
				iValPre = *(pDestPreLineAddr + iColIdx + 3);
				*(pDestLineAddr + iColIdx + 3) = (MV_FIXED)(iValPre + iValCur);
#endif

			}
		}

		iLeftNum = (iDestCol) % 4;
		iLeftStartNum = iDestCol - iLeftNum;

		if (0!= iLeftNum){

			for(iRowIdx = 1; iRowIdx < iDestRow; ++iRowIdx){

				pSrcLineAddr = MV_MatrixGetLinePtr(pSrc, iRowIdx * iStep);
				pDestPreLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx - 1);
				pDestLineAddr = MV_MatrixGetLinePtr(pDest, iRowIdx);

				for(iColIdx = iLeftStartNum; iColIdx < iDestCol; ++iColIdx){

					MV_FIXED iValCur = *(pSrcLineAddr + iColIdx);
					MV_FIXED iValPre = *(pDestPreLineAddr + iColIdx);
					*(pDestLineAddr + iColIdx) = (MV_FIXED)(iValPre + iValCur);
				}
			}
		}
	}

	return TRUE;
}


BOOL pcvSobel(LPMV_MATRIX_S pSrc, LPMV_MATRIX_S pDest, enum PCV_DIRECTION eDirect, enum PCV_SOBEL_TYPE eType)
{

	if (PCV_SOBEL_3X3 != eType){
		return FALSE;
	}

	if ((NULL == pSrc) || (NULL == pDest) 
		|| 	(NULL == g_sHoriFilter) || (NULL == g_sVertFilter)){
		return FALSE;
	}

	if (PCV_DIRT_X == eDirect){
		//sFilter = 
		pcvConvolution(pSrc, pDest, g_sHoriFilter, 3, 0);

	}else{

		pcvConvolution(pSrc, pDest, g_sVertFilter, 3, 0);
		//sFilter = 
	}

	return TRUE;
}


BOOL pcvHistogram_S(LPMV_MATRIX_S pSrc, MV_FIXED_S sMin, MV_FIXED_S sMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest)
{
	return pcvHistogram_S_NOFUNC(pSrc, sMin, sMax, iStep, rROI, pDest);
}


BOOL pcvHistogram_S_NOFUNC(LPMV_MATRIX_S pSrc, MV_FIXED_S sMin, MV_FIXED_S sMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest)
{
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;
	INT iROItopX = 0, iROItopY = 0, iROIbtmX = 0, iROIbtmY = 0;

	MV_FIXED_S* pSrcStartLineAddr = NULL;
	MV_FIXED* pDestStartLineAddr = NULL;
	if ((NULL == pSrc) || (NULL == pDest) || (NULL == rROI) || (0 == iStep)){
		return FALSE;
	}

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	iROItopX = rROI->x;
	iROItopY = rROI->y;

	iROIbtmX = iROItopX + rROI->width;
	iROIbtmY = iROItopY + rROI->height;

	if ((1 != iDestRow) || (iROItopX < 0) || (iROIbtmX > pSrc->iCols)
		|| (iROItopY < 0) || (iROIbtmY > pSrc->iRows)){
		return FALSE;
	}

	pDestStartLineAddr = MV_MatrixGetLinePtr(pDest , 0);

	for (iColIdx = 0; iColIdx < iDestCol; ++ iColIdx){
		*(pDestStartLineAddr + iColIdx) = (MV_FIXED)0;
	}

	for(iRowIdx = iROItopY; iRowIdx < iROIbtmY; ++iRowIdx){
		MV_FIXED iVal = 0;
		INT iPlaceCol = 0;

		pSrcStartLineAddr = MV_MatrixGetLinePtr_S(pSrc , iRowIdx, 0);

		for(iColIdx = iROItopX; iColIdx < iROIbtmX ; ++iColIdx){

			iVal = (MV_FIXED)(*(pSrcStartLineAddr + iColIdx));

			if ( iVal < sMin){
				iVal = sMin;
			}else if(iVal > sMax){
				iVal = sMax;
			}else{

			}

			iPlaceCol = (iVal - sMin) / iStep;
			if (iPlaceCol < iDestCol) {
				*(pDestStartLineAddr + iPlaceCol) += 1;
			}

		}
	}

	return TRUE;
}



BOOL pcvHistogram(LPMV_MATRIX pSrc, MV_FIXED iMin, MV_FIXED iMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest)
{
	return pcvHistogram_NOFUNC(pSrc, iMin, iMax, iStep, rROI, pDest);

}

BOOL pcvHistogram_NOFUNC(LPMV_MATRIX pSrc, MV_FIXED iMin, MV_FIXED iMax, INT iStep, pcvRect* rROI, LPMV_MATRIX pDest)
{
	INT iDestRow = 0, iDestCol = 0;
	INT iRowIdx = 0, iColIdx = 0;
	INT iROItopX = 0, iROItopY = 0, iROIbtmX = 0, iROIbtmY = 0;

	MV_FIXED* pSrcStartLineAddr = NULL;
	MV_FIXED* pDestStartLineAddr = NULL;
	if ((NULL == pSrc) || (NULL == pDest) || (NULL == rROI) || (0 == iStep)){
		return FALSE;
	}

	iDestRow = pDest->iRows;
	iDestCol = pDest->iCols;

	iROItopX = rROI->x;
	iROItopY = rROI->y;

	iROIbtmX = iROItopX + rROI->width;
	iROIbtmY = iROItopY + rROI->height;

	if ((1 != iDestRow) || (iROItopX < 0) || (iROIbtmX > pSrc->iCols)
		|| (iROItopY < 0) || (iROIbtmY > pSrc->iRows)){
		return FALSE;
	}

	pDestStartLineAddr = MV_MatrixGetLinePtr(pDest , 0);

	for (iColIdx = 0; iColIdx < iDestCol; ++ iColIdx){
		*(pDestStartLineAddr + iColIdx) = (MV_FIXED)0;
	}

	for(iRowIdx = iROItopY; iRowIdx < iROIbtmY; ++iRowIdx){
		MV_FIXED iVal = 0;
		INT iPlaceCol = 0;

		pSrcStartLineAddr = MV_MatrixGetLinePtr(pSrc , iRowIdx);

		for(iColIdx = iROItopX; iColIdx < iROIbtmX ; ++iColIdx){

			iVal = *(pSrcStartLineAddr + iColIdx);

			if ( iVal < iMin){
				iVal = iMin;
			}else if(iVal > iMax){
				iVal = iMax;
			}else{

			}

			iPlaceCol = (iVal - iMin) / iStep;
			if (iPlaceCol < iDestCol) {
				*(pDestStartLineAddr + iPlaceCol) += 1;
			}

		}
	}

	return TRUE;
}

BOOL pcvLineFitting(INT iPointNum, 	pcvPoint *pPoint, MV_FIXED* pfRatioInverse,	INT* piDist)
{
	BOOL bRet = FALSE;

	//=========Data definition=========//	
	LONGLONG iVar = 0, iVar2 = 0;

	LONGLONG iSum_X2 = 0;
	LONGLONG iSum_XY = 0;
	LONGLONG iSum_X = 0;
	LONGLONG iSum_Y = 0;

	INT iCount = 0;

	//==========Implementation==========// 
	iSum_X = 0;
	iSum_X2 = 0;
	iSum_XY = 0;
	iSum_Y = 0;

	if ((NULL == pPoint) || (NULL == pfRatioInverse) || (NULL == piDist)) {
		return bRet;
	}

	for (iCount = 0; iCount < iPointNum; iCount++)
	{
		iSum_X += pPoint[iCount].x;
		iSum_Y += pPoint[iCount].y;
		iSum_X2 += pPoint[iCount].x * pPoint[iCount].x;
		iSum_XY += pPoint[iCount].x * pPoint[iCount].y;
	}//for (iCount

	iVar = iPointNum * iSum_X2 - iSum_X * iSum_X;
	iVar2 = iPointNum * iSum_XY - iSum_X * iSum_Y;
	if (0 == iVar || 0 == iVar2){
		bRet = FALSE;
	}
	else
	{
		//1 / Ratio
		//*pfRatioInverse = MV_Div(iVar, (*piPointNum) * iSum_XY - iSum_X * iSum_Y);
		*pfRatioInverse = MV_FixedFromFloat(MV_Div_F((FLOAT)iVar, (FLOAT)(iVar2)));

		//Dist
		//*piDist = MV_IntFromFixed(MV_Div(iSum_X2 * iSum_Y - iSum_X * iSum_XY, iVar));
		*piDist = (iSum_X2 * iSum_Y - iSum_X * iSum_XY) / iVar;

		bRet = TRUE;

	}

	return bRet;
}

BOOL pcvThreshold(LPMV_MATRIX pHistogram, MV_FIXED coefficient, INT *piThreshold)
{
	INT iRow = 0, iCol = 0, iColIdx = 0;
	INT iT_MinBinPixelNum = 0;
	INT iHistMinPosIdx = 0;
	INT iHistMaxPosIdx = 0;
	MV_FIXED *piHistgramLineAddr = NULL;

	if ((NULL == pHistogram) || (NULL == piThreshold)){
		return FALSE;
	}

	iT_MinBinPixelNum = 10;
	iRow = pHistogram->iRows;
	iCol = pHistogram->iCols;

	if (1 != iRow){
		return FALSE;
	}
	
	//Find minimax position and maximal position of the histogram
	iHistMinPosIdx = 0;
	piHistgramLineAddr = MV_MatrixGetLinePtr(pHistogram, 0);
	for (iColIdx = 0; iColIdx < iCol - 5; iColIdx++) {
		INT iCnt = 0;
		INT iColIdx2 = 0;
		MV_FIXED iVal = 0;

		for (iColIdx2 = iColIdx; iColIdx2 < iColIdx + 5; iColIdx2++){
			//iVal = MV_MatrixGet(pHistogram, 0, iColIdx2);
			iVal = piHistgramLineAddr[iColIdx2];
			if(iVal > iT_MinBinPixelNum){
				++iCnt;
			}
		}//iColIdx2

		if (iVal < 2){
			iHistMinPosIdx += 5;
			iColIdx += 5;
		}
		else{
			break;
		}

	}//iColIdx


	iHistMaxPosIdx = 0;
	for (iColIdx = iHistMinPosIdx; iColIdx < iCol - 5; iColIdx++) 
	{
		INT iCnt = 0;
		INT iColIdx2 = 0;
		MV_FIXED iVal;

		for (iColIdx2 = iColIdx; iColIdx2 < iColIdx + 5; iColIdx2++){
			//iVal = MV_MatrixGet(pHistogram, 0, iColIdx2);
			iVal = piHistgramLineAddr[iColIdx2];
			if(iVal > iT_MinBinPixelNum){
				++iCnt;
			}
		}//iColIdx2


		if (iCnt >= 2){
			iHistMaxPosIdx += 5;
			iColIdx += 5;
		}
		else{
			break;
		}

	}//iColIdx

	//find max value and min value
	if (iHistMinPosIdx == 0)
	{
		iHistMinPosIdx = 1;
	}

	//*piThreshold = iHistMaxPosIdx * coefficient;
	*piThreshold = MV_IntFromFixed(MV_Mul(MV_FixedFromInt(iHistMaxPosIdx), coefficient));


	return TRUE;
}


BOOL pcvHoughTransform(INT* piCoordianteData, INT iDataLength, CONST HOUGH_REGION *pRoi, MV_FIXED fxrho, 
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
	INT iVar = 0;
	INT iBufferSize = 0;
	INT iAccumSize = 0;
	INT iSorBufferSize = 0;

	if ((0 == fxTheta) || (0 == fxrho)) {
		return FALSE;
	}

	if (!piCoordianteData || !pRoi || !pMatRes || !pNum || !pWorkBuffer) {
		return FALSE;
	}

	numangle = iValidAngleSize;
	iMinAng = MV_IntFromFixed(MV_Div(fMinAngle, fxTheta)) + 1;
	numrho = MV_IntFromFixed(MV_Div(MV_FixedFromInt((pRoi->iWidth + pRoi->iHeight) * 2 + 1), fxrho));

	pBuffer = (BYTE*)pWorkBuffer;
	iBufferSize = (numangle + 2) * (numrho + 2);
	iAccuBufSize = sizeof(INT) * (numangle + 2) * (numrho + 2);

	if (iAccuBufSize > iBuffSize) {
		return FALSE;
	}
	MV_Memset(pBuffer, 0, iAccuBufSize);

	accum = (INT*)pBuffer;
	iAccumSize = iBufferSize;
	sort_buf = (INT*)(pBuffer + iAccuBufSize);
	iSorBufferSize = 180 * 2;

	// fill accumulator
	for (i = 0; i < iDataLength; i++)
	{
		iX = piCoordianteData[i * 2];
		iY = piCoordianteData[i * 2 + 1];

		for (n = 0; n < numangle; n++)
		{
			iVar = n + iMinAng + 1;
			if(iVar > 0
				&& iVar < NUMANGLE)
			{
				rIdx = MV_Mul(iX, fxtabCos[n + iMinAng + 1]) + MV_Mul(iY, fxtabSin[n + iMinAng + 1]);
				rIdx += (numrho - 1) / 2;
				accum[(n + 1) * (numrho + 2) + rIdx + 1]++;
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
	pcvHoughSortDescent32s(sort_buf, total, accum);

	// create line storage
	iLineMax = MV_MIN(iLineMax, total);
	if (iLineMax > pMatRes->iRows) {
		return FALSE;
	}

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


#define hough_cmp_gt(l1, l2) (aux[l1] > aux[l2])
VOID pcvHoughSortDescent32s(INT *array, INT total, const INT *aux)
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
	if( (total <= 1) || (NULL == array) || (NULL == aux))
		return;

	stack[0].lb = array;
	stack[0].ub = array + (total - 1);

	while (sp >= 0)
	{
		INT* left = stack[sp].lb;
		INT* right = stack[sp--].ub;

		for(;;)
		{
			int i = 0, n = (int)((right - left) + 1), m = 0;
			INT *ptr = NULL;
			INT *ptr2 = NULL;

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
				INT* left0 = NULL;
				INT* left1 = NULL;
				INT* right0 = NULL;
				INT* right1 = NULL;
				INT* pivot = NULL;
				INT* a = NULL;
				INT* b = NULL;
				INT* c = NULL;
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

VOID pcvConvolution(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
					SHORT iMaskSize, SHORT iShift)
{
/*#ifdef C64X
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
#else*/
	if (3 == iMaskSize){
		pcvConvolution_Mid(pMatSrc, pMatDst, pMatMask, iMaskSize, iShift);
	}

/*#endif*/
}

// for unit matrix convolution, directly to pMatDst
VOID pcvConvolution_Mid(MV_MATRIX_S *pMatSrc, MV_MATRIX_S *pMatDst, MV_MATRIX_S *pMatMask, 
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
		//cause the filter size is 3
		IMG_conv_3x3_i16s_c16s(pLineSrc, pLineDst, pMatSrc->iCols, 
			pMatSrc->iPitch / sizeof(MV_FIXED_S), MV_MatrixGetLinePtr_S(pMatMask, 0, 0), iShift);

		/*if (iMaskSize == 3)
		{

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
		}*/

#else
		pcvConvolution_Int(pLineSrc, pLineDst, pMatSrc->iCols,
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



VOID pcvConvolution_Int(CONST MV_FIXED_S *pInData, MV_FIXED_S *pOutData, 
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


BOOL pcvDisTransform(INT* piDistanceGraph,
					 INT iDistanceGraphDataNum,
					 LPMV_MATRIX_S psEdgeDetResult,
					 pcvDTParam* pDTParam)
{
	//=========Data definition=========//
	INT * piRow = NULL;
	INT * piPreviousRow = NULL;
	INT * piNextRow = NULL;

	INT iRow = 0;
	INT iCol = 0;

	INT iDistanceGraphMaxValue = 0;

	INT iDistanceGraphFarStep = 0;
	INT iDistanceGraphNearStep = 0;

	INT iHeight = 0;
	INT iWidth = 0;

	MV_FIXED_S *pMatrixLineAddr = NULL;

	iHeight = pDTParam->iHeight;
	iWidth = pDTParam->iWidth;

	if (NULL == piDistanceGraph || NULL == psEdgeDetResult || NULL == pDTParam){
		return FALSE;
	}

	if ((psEdgeDetResult->iRows < iHeight) || (psEdgeDetResult->iCols < iWidth)
		|| (iDistanceGraphDataNum < psEdgeDetResult->iCols * iHeight)) {
		return FALSE;
	}

	//==========Implementation==========//
	//init the distance graph 
	iDistanceGraphFarStep = pDTParam->iFarVal;
	iDistanceGraphNearStep = pDTParam->iNearVal;

	iDistanceGraphMaxValue = INT_MAX / 2;

	piRow = piDistanceGraph;

	for (iRow = 0; iRow < iHeight; iRow++)
	{
		pMatrixLineAddr = MV_MatrixGetLinePtr_S(psEdgeDetResult, iRow, 0);

		for (iCol = 0; iCol < iWidth; iCol++)
		{
			if (pMatrixLineAddr[iCol] == 1)
			{
				piRow[iCol] = 0;
			}
			else
			{
				piRow[iCol] = iDistanceGraphMaxValue;
			}

		}//iCol

		piRow += psEdgeDetResult->iCols;
	}//iRow

	piRow = piDistanceGraph + psEdgeDetResult->iCols;
	piPreviousRow = piDistanceGraph;
	piNextRow = piDistanceGraph + 2 * psEdgeDetResult->iCols;

	// Create combined edge detection result
	for (iRow = 1; iRow < iHeight - 1; iRow++)
	{
		for(iCol = 1; iCol < iWidth - 1; iCol++)
		{
			if(0 == piRow[iCol])
			{
				if (piPreviousRow[iCol - 1] > iDistanceGraphFarStep)
					piPreviousRow[iCol - 1] = iDistanceGraphFarStep;

				if (piPreviousRow[iCol] > iDistanceGraphNearStep)
					piPreviousRow[iCol] = iDistanceGraphNearStep;

				if (piPreviousRow[iCol + 1] > iDistanceGraphFarStep)
					piPreviousRow[iCol + 1] = iDistanceGraphFarStep;

				if (piRow[iCol - 1] > iDistanceGraphNearStep)
					piRow[iCol - 1] = iDistanceGraphNearStep;

				if (piRow[iCol + 1] > iDistanceGraphNearStep)
					piRow[iCol + 1] = iDistanceGraphNearStep;

				if (piNextRow[iCol - 1] > iDistanceGraphFarStep)
					piNextRow[iCol - 1] = iDistanceGraphFarStep;

				if (piNextRow[iCol] > iDistanceGraphNearStep)
					piNextRow[iCol] = iDistanceGraphNearStep;

				if (piNextRow[iCol + 1] > iDistanceGraphFarStep)
					piNextRow[iCol + 1] = iDistanceGraphFarStep;
			}

		}//for iCol

		piRow += psEdgeDetResult->iCols;
		piPreviousRow += psEdgeDetResult->iCols;
		piNextRow += psEdgeDetResult->iCols;

	}//for iRow


	//Scan from top-left to bottom-right
	piRow = piDistanceGraph + psEdgeDetResult->iCols;
	piPreviousRow = piDistanceGraph;

	for (iRow = 1; iRow < iHeight; iRow++)
	{
		for(iCol = 0; iCol < iWidth; iCol++)
		{
			if (piRow[iCol] > iDistanceGraphFarStep)
			{
				if (iCol > 0)
				{
					if (piRow[iCol] > piPreviousRow[iCol - 1] + iDistanceGraphFarStep)
						piRow[iCol] = piPreviousRow[iCol - 1] + iDistanceGraphFarStep;

					if (piRow[iCol] > piRow[iCol - 1] + iDistanceGraphNearStep)
						piRow[iCol] = piRow[iCol - 1] + iDistanceGraphNearStep;
				}

				if (piRow[iCol] > piPreviousRow[iCol] + iDistanceGraphNearStep)
					piRow[iCol] = piPreviousRow[iCol] + iDistanceGraphNearStep;

				//Process the last column
				if(iCol < iWidth - 1)
				{
					if (piRow[iCol] > piPreviousRow[iCol + 1] + iDistanceGraphFarStep)
						piRow[iCol] = piPreviousRow[iCol + 1] + iDistanceGraphFarStep;
				}

			}
		}//for iCol

		piRow += psEdgeDetResult->iCols;
		piPreviousRow += psEdgeDetResult->iCols;

	}//for iRow


	//Scan from bottom-right to bot-left
	piRow = piDistanceGraph + (iHeight - 2) * psEdgeDetResult->iCols;
	piNextRow = piDistanceGraph + (iHeight - 1) * psEdgeDetResult->iCols;

	for (iRow = iHeight - 2; iRow >= 0; iRow--)
	{
		for(iCol = iWidth - 1; iCol >= 0; iCol--)
		{
			if (piRow[iCol] > iDistanceGraphFarStep)
			{
				if (piRow[iCol] > piNextRow[iCol] + iDistanceGraphNearStep)
					piRow[iCol] = piNextRow[iCol] + iDistanceGraphNearStep;

				//Process the last column
				if(iCol > 0)
				{
					if (piRow[iCol] > piNextRow[iCol - 1] + iDistanceGraphFarStep)
						piRow[iCol] = piNextRow[iCol - 1] + iDistanceGraphFarStep;
				}

				if(iCol < iWidth - 1)
				{
					if (piRow[iCol] > piNextRow[iCol + 1] + iDistanceGraphFarStep)
						piRow[iCol] = piNextRow[iCol + 1] + iDistanceGraphFarStep;

					if (piRow[iCol] > piRow[iCol + 1] + iDistanceGraphNearStep)
						piRow[iCol] = piRow[iCol + 1] + iDistanceGraphNearStep;
				}

			}
		}//for iCol


		piRow -= psEdgeDetResult->iCols;
		piNextRow -= psEdgeDetResult->iCols;

	}//for iRow


	//////////////////////////////////////////////
	//for (iRow = 0; iRow < iHeight; iRow++)
	//{
	//	for(iCol = 0; iCol < iWidth; iCol++)
	//	{
	//		UTI_PrintInfoToScreen("%d\t",piDistanceGraph[iRow * iWidth + iCol] );
	//	}//for iCol

	//	UTI_PrintInfoToScreen("\r\n");

	//}//for iRow

	//iCol = 0;
	//////////////////////////////////////////////

	return TRUE;

}


/* EOF */
