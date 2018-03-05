/*
 * @(#)$Id: AdasUtility.h v 2.0 2015/11/25  LvGangXiao $
 * @(#)AdasUtility
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file
 * AdasUtility
 *
 *
 */
#pragma once


#ifndef ADAS_UTILITY_H
#define ADAS_UTILITY_H

#ifndef ADAS_COMMON_DATASTRUCTURE_H
#include "AdasCommonDataStructure.h"
#define ADAS_COMMON_DATASTRUCTURE_H
#endif

#ifdef WIN32

#ifdef ADAS_EXPORTS
#define ADAS_API __declspec(dllexport)
#else
#define ADAS_API __declspec(dllimport)
#endif

#else

#define ADAS_API

#endif


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * AdasUtility
 *
 *
 * @author $ZhouZeng$
 * @version $2.0$
 * @date $2011/01/02$
 */

   /**
	* For over flow checking
	*/
	VOID UTI_ForOverFlowChecking_Matrix(BOOL iIsIncreasingOrder_X, BOOL iIsIncreasingOrder_Y,
		                           BOOL* piValidFlag_X, BOOL* piValidFlag_Y, 
								   INT* piStartX, INT* piEndX,
		                           INT* piStartY, INT* piEndY, 
								   INT iDataLengthX, INT iDataLengthY);

	BOOL UTI_ForOverFlowChecking_Array(BOOL iIsIncreasingOrder, INT* piStart, INT* piEnd,
		                                        INT iDataLength);


	VOID UTI_ForOverFlowChecking_Unit(INT* piIdx, INT iDataLength);



	/**
	* pow operation of the small value
	*/
	MV_FIXED UTI_MV_PowSmallValue(MV_FIXED iValue, MV_FIXED idx);


	/**
     * Compute Accumulation Graph
	 *
	 * @param pchImageData	input 8bit single channel image
	 * @param pchAGData		the evaluated accumulation graph image
	 * @param iWidth		the image width
	 * @param iHeight		the image height
     */
	 VOID UTI_ComputeAccumGraph(LPMV_MATRIX_S psImageData, INT* pchAGData,
								   INT iWidth, INT iHeight);


	/**
	* Print Info To File
	*/
	 VOID UTI_PrintInfoToFile(MV_FILE *fp, CONST CHAR *format, ...);

 	/**
	* Print Info To Screen
	*/
	 VOID UTI_PrintInfoToScreen(CONST CHAR *format, ...);

	/**
	* Print Info
	*/
	CHAR * UTI_PrintInfo(CONST CHAR *format, ...);


	/**
	* GetROIMatrix
	*/
	BOOL UTI_GetROIMatrix(MV_MATRIX_S *pMatSrc, AdasImageROI *sRoi, INT iRoiCount, MV_MATRIX_S *pMatDst);

   /**
	* GetROIMatrix2
	*/
	BOOL UTI_GetROIMatrix2(MV_MATRIX_S *pMatSrc, AdasImageROI *sRoi, MV_MATRIX_S *pMatDst, INT iXCompressStep, INT iYCompressStep);
  
	/**
	* GetROIMatrix3
	*/
BOOL UTI_GetROIMatrix3(MV_MATRIX_S *pMatSrc, AdasImageROI *sOrigRoi, AdasImageROI *sCompressRoi, MV_MATRIX_S *pMatDst, INT iXCompressStep, INT iYCompressStep);


	/**
	 * Compute Sum According To AG
	 *
	 * @param piAG						the accumulate graph
	 * @param iImgWidth
	 * @param iImgHeihgt
	 * @param psAGROI
	 */
INT UTI_ComputeSumAccordingToAG(CONST INT *piAG, 
								 INT iImgWidth,
								 INT iImgHeight,
								 AdasImageROI* psAGROI);



	/**
	 * Compute Sum According To AG
	 *
	 * @param piAG						the accumulate graph
	 * @param iImgWidth
	 * @param iImgHeihgt
	 * @param psAGROI
	 */
	 INT UTI_ComputeSumAccordingToAG2(LPMV_MATRIX piAG,
									  AdasImageROI* psAGROI);

	/**
	 * Compute Sum According To AG
	 *
	 * @param piAG						the accumulate graph
	 * @param iImgWidth
	 * @param iImgHeihgt
	 * @param psAGROI
	 */
	 INT UTI_ComputeSumAccordingToAG4(LPMV_MATRIX piAG,
									  AdasImageROI* psAGROI);


	/**
	 * Compute Sum According To AG
	 *
	 * @param piAG						the accumulate graph
	 * @param iImgWidth
	 * @param iImgHeihgt
	 * @param psAGROI
	 */
	 INT UTI_ComputeSumAccordingToAG_MR(LPMV_MATRIX piAG,
									    AdasImageROI* psDetROI,
									    AdasImageROI* psAGROI);



	/**
	 * Copy AdasImageROI
	 *
	 * @param psOrig
	 * @param psDest
	 */
	 VOID UTI_CopyAdasImageROI(AdasImageROI* psOrig, AdasImageROI* psDest);


	/**
	 * Compute Mean Value
	 *
	 * @param psDataList
	 * @param piMean
	 */
	 VOID UTI_ComputeMean_Int(BufferParams_Int* psDataList, INT iDatalength, INT* piMean, 
		                      INT* piValidElementNum, BYTE* pWorkBuffer);

	/**
	 * Compute Mean Value
	 *
	 * @param psDataList
	 * @param piMean
	 */
	 VOID UTI_ComputeMean_Fixed(BufferParams_Fixed* psDataList, INT iDatalength, MV_FIXED* piMean, 
		                        INT* piValidElementNum, BYTE* pWorkBuffer);


#ifdef __cplusplus
}
#endif

/*
 *
 * $Log$
 */
#endif /* ADAS_UTILITY_H */
/* EOF */



