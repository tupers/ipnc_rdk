/*
 * @(#)$Id: pcvColorConversion.c 2841 2012-09-14 02:47:18Z LvGangXiao $
 * @(#) Implementation file of class pcvColorConversion
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file
 * Implementation of pcvColorConversion
 *
 * TO DESC : FILE DETAIL DESCRIPTION, BUT DON'T DESCRIBE CLASS DETAIL HERE.
 */

#ifndef CXX_PCVCOLORCONVERSION_H
#include "pcvColorConversion.h"
#endif

#ifdef  MV_USE_NEON
#include <arm_neon.h>
#endif

#include "pcvMathFixed32.h"

//#ifdef  MV_USE_NEON
BOOL pcvCvtYUV2HSV_NEON(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                        LPMV_MATRIX_S psH, LPMV_MATRIX_S psS,  LPMV_MATRIX_S psV);
BOOL pcvCvtYUV2HSVAndCompOp_NEON(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                                 pcvHSVColorRange sColorRange, LPMV_MATRIX_S psFlag);

BOOL pcvCvtYUV2HSVAndCompOpEx_NEON(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
							   pcvHSVColorRange* psColorRange, INT iColorCount,
							   pcvRect* psFrameRect, LPMV_MATRIX_S psFlag);
//#else
/*BOOL pcvCvtYUV2HSV_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                   LPMV_MATRIX psH, LPMV_MATRIX psS,  LPMV_MATRIX psV);
BOOL pcvCvtYUV2HSVAndCompOp_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                            pcvHSVColorRange sColorRange, LPMV_MATRIX_S psFlag);*/
//#endif



BOOL pcvCvtYUV2HSV(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                   LPMV_MATRIX_S psH, LPMV_MATRIX_S psS,  LPMV_MATRIX_S psV)
{
#ifdef  MV_USE_NEON
    return pcvCvtYUV2HSV_NEON(psFrameY, psFrameU, psFrameV, psH, psS, psV);
#else
    return pcvCvtYUV2HSV_C(psFrameY, psFrameU, psFrameV, psH, psS, psV);
#endif
}

BOOL pcvCvtYUV2HSVAndCompOp(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                            pcvHSVColorRange sColorRange, LPMV_MATRIX_S psFlag)
{
#ifdef  MV_USE_NEON
    return pcvCvtYUV2HSVAndCompOp_NEON(psFrameY, psFrameU, psFrameV, sColorRange, psFlag);
#else
    return pcvCvtYUV2HSVAndCompOp_C(psFrameY, psFrameU, psFrameV, sColorRange, psFlag);
#endif
}

BOOL pcvCvtYUV2HSVAndCompOpEx(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
							   pcvHSVColorRange* psColorRange, INT iColorCount,
							   pcvRect* psFrameRect, LPMV_MATRIX_S psFlag)
{
#ifdef  MV_USE_NEON
	return pcvCvtYUV2HSVAndCompOpEx_NEON(psFrameY, psFrameU, psFrameV,
		psColorRange, iColorCount, psFrameRect, psFlag);
#else
	return pcvCvtYUV2HSVAndCompOpEx_C(psFrameY, psFrameU, psFrameV,
		   psColorRange, iColorCount, psFrameRect, psFlag);
#endif
}


#ifdef MV_USE_NEON

#define CVT_PRECISION 10
#define CVT_ONE  (1 << CVT_PRECISION)
#define CVT_HALF (1 << (CVT_PRECISION - 1))

BOOL pcvCvtYUV2HSV_NEON(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                        LPMV_MATRIX_S psH, LPMV_MATRIX_S psS,  LPMV_MATRIX_S psV)
{
    //=========Data definition=========//
    INT iRow = 0;
    INT iCol = 0;
    MV_FIXED_S *piVarY;
    MV_FIXED_S *piVarU;
    MV_FIXED_S *piVarV;
	MV_FIXED_S *piVarH;
	MV_FIXED_S *piVarS;
	INT  yuv[4] = {0};
	INT rgb[4] = {0};
    INT iTemp = 0;
    INT iR = 0;
    INT iG = 0;
    INT iB = 0;
    int32x4_t vecYUV;
    int32x4_t vecRPara;
    int32x4_t vecGPara;
    int32x4_t vecBPara;
    int32x4_t vecR;
    int32x4_t vecG;
    int32x4_t vecB;
    int32x4_t vecRGB;
    int32x4_t vecConstHalf;
    int16x4_t vecRGB16;
    int16x4_t vecRGB16Max;
    int16x4_t vecRGB16Min;
    int16x4_t vecRGB16Comp;
    int16x4_t vecRGB16CompMin;
    int16x4_t vecRGB16CompMax;
    SHORT nMax = 0;
    SHORT nMin = 0;
    INT iDelta = 0;
    INT iVarH = 0;
    INT iVarS = 0;

    if ( (psFrameY == NULL) ||  (psFrameU == NULL) || (psFrameV == NULL)
        || (psH == NULL) || (psS == NULL) )
    {
        return FALSE;
    }

	if ((psFrameU->iRows < psFrameY->iRows) || (psFrameU->iCols < psFrameY->iCols)
		|| (psFrameV->iRows < psFrameY->iRows) || (psFrameV->iCols < psFrameY->iCols)
		|| (psH->iRows < psFrameY->iRows) || (psH->iCols < psFrameY->iCols)
		|| (psS->iRows < psFrameY->iRows) || (psS->iCols < psFrameY->iCols)) {
			return FALSE;
	}

    //==========Implementation==========//
    //Initial some params
    //the last parameter is always 1
    yuv[3] = 1;
    //the last element of vecRGB is 0
    vecRGB = vsetq_lane_s32(0, vecRGB, 3);

    vecRGB16Max = vdup_n_s16(255);
    vecRGB16Min = vdup_n_s16(0);

    vecConstHalf = vdupq_n_s32(CVT_HALF);

    //iVarB = 1.164 * (iVarY - 16) + 2.018 * (iVarU - 128); = 1.164 * iVarY + 2.018 * iVarU - 276.928;
    //iVarG = 1.164 * (iVarY - 16) - 0.813 * (iVarV - 128) - 0.391 * (iVarU - 128); = 1.164 * iVarY - 0.813*iVarV - 0.391*iVarU + 135.488
    //iVarR = 1.164 * (iVarY - 16) + 1.596 * (iVarV - 128); = 1.164 * iVarY + 1.596 * iVarV - 222.912;
    iTemp = (INT) (1.164f * CVT_ONE);
    vecRPara = vsetq_lane_s32(iTemp,      vecRPara, 0);
    vecRPara = vsetq_lane_s32(0,             vecRPara, 1);
    iTemp = (INT) (1.596f * CVT_ONE);
   vecRPara = vsetq_lane_s32(iTemp,      vecRPara, 2);
   iTemp = (INT) (- 222.912f * CVT_ONE);
   vecRPara = vsetq_lane_s32(iTemp, vecRPara, 3);

   iTemp = (INT) (1.164f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,      vecGPara, 0);
   iTemp = (INT) (- 0.391f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,      vecGPara, 1);
   iTemp = (INT) (- 0.813f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,    vecGPara, 2);
   iTemp = (INT) (135.488f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,   vecGPara, 3);

   iTemp = (INT) (1.164f * CVT_ONE);
   vecBPara = vsetq_lane_s32(iTemp,       vecBPara, 0);
   iTemp = (INT) (2.018f * CVT_ONE);
   vecBPara = vsetq_lane_s32(iTemp,       vecBPara, 1);
   vecBPara = vsetq_lane_s32(0,          vecBPara, 2);
   iTemp = (INT) (- 276.928f * CVT_ONE);
   vecBPara = vsetq_lane_s32(iTemp, vecBPara, 3);

    for (iRow = 0; iRow < psFrameY->iRows; iRow++)
    {
        piVarY = MV_MatrixGetLinePtr_S(psFrameY, iRow, 0);
        piVarU = MV_MatrixGetLinePtr_S(psFrameU, iRow, 0);
        piVarV = MV_MatrixGetLinePtr_S(psFrameV, iRow, 0);
		piVarH = MV_MatrixGetLinePtr_S(psH, iRow, 0);
		piVarS = MV_MatrixGetLinePtr_S(psS, iRow, 0);

        for (iCol = 0; iCol < psFrameY->iCols; iCol++)
        {
            //Get matrix data
            yuv[0] = *(piVarY+iCol);
            yuv[1] = *(piVarU+iCol);
            yuv[2] = *(piVarV+iCol);
            //Load it from memory
            vecYUV = vld1q_s32(&yuv[0]);

            //Get R
            vecR = vmulq_s32(vecYUV, vecRPara);
            vst1q_s32(&rgb[0], vecR);
            iR = rgb[0] + rgb[1] + rgb[2] + rgb[3];
            //Get G
            vecG = vmulq_s32(vecYUV, vecGPara);
            vst1q_s32(&rgb[0], vecG);
            iG = rgb[0] + rgb[1] + rgb[2] + rgb[3];
            //Get B
            vecB = vmulq_s32(vecYUV, vecBPara);
            vst1q_s32(&rgb[0], vecB);
            iB = rgb[0] + rgb[1] + rgb[2] + rgb[3];

            vecRGB = vsetq_lane_s32(iR, vecRGB, 0);
            vecRGB = vsetq_lane_s32(iG, vecRGB, 1);
            vecRGB = vsetq_lane_s32(iB, vecRGB, 2);

            //Add 0.5f
            vecRGB = vaddq_s32(vecRGB, vecConstHalf);

            //int16x4_t vshrn_n_s32(int32x4_t a, __constrange(1,16) int b);
            vecRGB16 = vshrn_n_s32(vecRGB, CVT_PRECISION);

            //RGB Range: 0 ~ 255
            vecRGB16 = vmin_s16(vecRGB16, vecRGB16Max);
            vecRGB16 = vmax_s16(vecRGB16, vecRGB16Min);

            /* RGB -> HSV
                max=max(r, g, b);
                min=min(r, g, b);
                if r=max, H = (g-b)/(max-min)
                if g=max, H = 2 + (b-r)/(max-min)
                if b = max, H = 4 + (r-g)/(max-min)

                H = H * 60
                if H<0, H = H + 360;

                V = max(r, g, b)
                S = (max-min)/max
            */

            //Calculate max(r,g,b) , min(r, g, b)
            vecRGB16Comp = vdup_lane_s16(vecRGB16, 1);
            vecRGB16CompMax = vmax_s16(vecRGB16, vecRGB16Comp);
            vecRGB16CompMin = vmin_s16(vecRGB16, vecRGB16Comp);
            vecRGB16Comp = vdup_lane_s16(vecRGB16, 2);
            vecRGB16CompMax = vmax_s16(vecRGB16CompMax, vecRGB16Comp);
            vecRGB16CompMin = vmin_s16(vecRGB16CompMin, vecRGB16Comp);

            nMax = vget_lane_s16(vecRGB16CompMax, 0);
            nMin = vget_lane_s16(vecRGB16CompMin, 0);

            iDelta = nMax - nMin;
            iR = vget_lane_s16(vecRGB16, 0);
            iG = vget_lane_s16(vecRGB16, 1);
            iB = vget_lane_s16(vecRGB16, 2);

            if (iDelta == 0)
            {
                iVarH = 0;
            }
            else if (nMax == iR)
            {   //H = (g-b)/(max-min)
                iVarH = ( (iG - iB) * CVT_ONE)/iDelta;
            }
            else if (nMax == iG)
            {   // H = 2 + (b-r)/(max-min)
                iVarH = 2 * CVT_ONE + ((iB - iR) * CVT_ONE)/iDelta;
            }
            else if (nMax == iB)
            {   //H = 4 + (r-g)/(max-min)
                iVarH = 4 * CVT_ONE + ((iR - iG) * CVT_ONE)/iDelta;
            }

            //H = H * 60
            iVarH = (iVarH * 60) >> CVT_PRECISION;

            if(iVarH < 0)
                iVarH += 360;

            if (nMax == 0)
            {
                iVarS = 0;
            }
            else
            {   //S = (max-min)/max
                iVarS = ((iDelta * CVT_ONE * 255) /nMax) >> CVT_PRECISION;
            }

            //Set matrix data
            //MV_MatrixSet_S(psH, iRow, iCol, iVarH);
			piVarH[iCol] = iVarH;
            //MV_MatrixSet_S(psS, iRow, iCol, iVarS);
			piVarS[iCol] = iVarS;


        }//iCol

    }//iRow

    return TRUE;
}

BOOL pcvCvtYUV2HSVAndCompOp_NEON(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                                 pcvHSVColorRange sColorRange, LPMV_MATRIX_S psFlag)
{
    //=========Data definition=========//
    INT iRow = 0;
    INT iCol = 0;
    MV_FIXED_S *piVarY = NULL;
    MV_FIXED_S *piVarU = NULL;
    MV_FIXED_S *piVarV = NULL;
	MV_FIXED_S *piVarFlag = NULL;
	INT  yuv[4] = {0};
	INT rgb[4] = {0};
    INT iTemp = 0;
    INT iR = 0;
    INT iG = 0;
    INT iB = 0;
    int32x4_t vecYUV;
    int32x4_t vecRPara;
    int32x4_t vecGPara;
    int32x4_t vecBPara;
    int32x4_t vecR;
    int32x4_t vecG;
    int32x4_t vecB;
    int32x4_t vecRGB;
    int32x4_t vecConstHalf;
    int16x4_t vecRGB16;
    int16x4_t vecRGB16Max;
    int16x4_t vecRGB16Min;
    int16x4_t vecRGB16Comp;
    int16x4_t vecRGB16CompMin;
    int16x4_t vecRGB16CompMax;
    SHORT nMax = 0;
    SHORT nMin = 0;
    INT iDelta = 0;
    INT iVarH = 0;
    INT iVarS = 0;

    if ( (psFrameY == NULL) ||  (psFrameU == NULL) || (psFrameV == NULL) || (psFlag == NULL)  )
    {
        return FALSE;
    }

	if ((psFrameU->iRows < psFrameY->iRows) || (psFrameU->iCols < psFrameY->iCols)
		|| (psFrameV->iRows < psFrameY->iRows) || (psFrameV->iCols < psFrameY->iCols)
		|| (psFlag->iRows < psFrameY->iRows) || (psFlag->iCols < psFrameY->iCols)) {
		return FALSE;
	}

    //Initialize every element of flag matrix to 0
    MV_MatrixSetAll_S(psFlag, 0);

    //==========Implementation==========//
    //Initial some params
    //the last parameter is always 1
    yuv[3] = 1;
    //the last element of vecRGB is 0
    vecRGB = vsetq_lane_s32(0, vecRGB, 3);

    vecRGB16Max = vdup_n_s16(255);
    vecRGB16Min = vdup_n_s16(0);
    vecConstHalf = vdupq_n_s32(CVT_HALF);

    //iVarB = 1.164 * (iVarY - 16) + 2.018 * (iVarU - 128); = 1.164 * iVarY + 2.018 * iVarU - 276.928;
    //iVarG = 1.164 * (iVarY - 16) - 0.813 * (iVarV - 128) - 0.391 * (iVarU - 128); = 1.164 * iVarY - 0.813*iVarV - 0.391*iVarU + 135.488
    //iVarR = 1.164 * (iVarY - 16) + 1.596 * (iVarV - 128); = 1.164 * iVarY + 1.596 * iVarV - 222.912;
    iTemp = (INT) (1.164f * CVT_ONE);
    vecRPara = vsetq_lane_s32(iTemp,      vecRPara, 0);
    vecRPara = vsetq_lane_s32(0,             vecRPara, 1);
    iTemp = (INT) (1.596f * CVT_ONE);
   vecRPara = vsetq_lane_s32(iTemp,      vecRPara, 2);
   iTemp = (INT) (- 222.912f * CVT_ONE);
   vecRPara = vsetq_lane_s32(iTemp, vecRPara, 3);

   iTemp = (INT) (1.164f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,      vecGPara, 0);
   iTemp = (INT) (- 0.391f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,      vecGPara, 1);
   iTemp = (INT) (- 0.813f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,    vecGPara, 2);
   iTemp = (INT) (135.488f * CVT_ONE);
   vecGPara = vsetq_lane_s32(iTemp,   vecGPara, 3);

   iTemp = (INT) (1.164f * CVT_ONE);
   vecBPara = vsetq_lane_s32(iTemp,       vecBPara, 0);
   iTemp = (INT) (2.018f * CVT_ONE);
   vecBPara = vsetq_lane_s32(iTemp,       vecBPara, 1);
   vecBPara = vsetq_lane_s32(0,          vecBPara, 2);
   iTemp = (INT) (- 276.928f * CVT_ONE);
   vecBPara = vsetq_lane_s32(iTemp, vecBPara, 3);

    for (iRow = 0; iRow < psFrameY->iRows; iRow++)
    {
        piVarY = MV_MatrixGetLinePtr_S(psFrameY, iRow, 0);
        piVarU = MV_MatrixGetLinePtr_S(psFrameU, iRow, 0);
        piVarV = MV_MatrixGetLinePtr_S(psFrameV, iRow, 0);
		piVarFlag = MV_MatrixGetLinePtr_S(psFlag, iRow, 0);

        for (iCol = 0; iCol < psFrameY->iCols; iCol++)
        {
            //Get matrix data
            yuv[0] = *(piVarY+iCol);
            yuv[1] = *(piVarU+iCol);
            yuv[2] = *(piVarV+iCol);
            //Load it from memory
            vecYUV = vld1q_s32(&yuv[0]);

            //Get R
            vecR = vmulq_s32(vecYUV, vecRPara);
            vst1q_s32(&rgb[0], vecR);
            iR = rgb[0] + rgb[1] + rgb[2] + rgb[3];
            //Get G
            vecG = vmulq_s32(vecYUV, vecGPara);
            vst1q_s32(&rgb[0], vecG);
            iG = rgb[0] + rgb[1] + rgb[2] + rgb[3];
            //Get B
            vecB = vmulq_s32(vecYUV, vecBPara);
            vst1q_s32(&rgb[0], vecB);
            iB = rgb[0] + rgb[1] + rgb[2] + rgb[3];

            vecRGB = vsetq_lane_s32(iR, vecRGB, 0);
            vecRGB = vsetq_lane_s32(iG, vecRGB, 1);
            vecRGB = vsetq_lane_s32(iB, vecRGB, 2);

            //Add 0.5f
            vecRGB = vaddq_s32(vecRGB, vecConstHalf);

            //int16x4_t vshrn_n_s32(int32x4_t a, __constrange(1,16) int b);
            vecRGB16 = vshrn_n_s32(vecRGB, CVT_PRECISION);

            //RGB Range: 0 ~ 255
            vecRGB16 = vmin_s16(vecRGB16, vecRGB16Max);
            vecRGB16 = vmax_s16(vecRGB16, vecRGB16Min);

            /* RGB -> HSV
                max=max(r, g, b);
                min=min(r, g, b);
                if r=max, H = (g-b)/(max-min)
                if g=max, H = 2 + (b-r)/(max-min)
                if b = max, H = 4 + (r-g)/(max-min)

                H = H * 60
                if H<0, H = H + 360;

                V = max(r, g, b)
                S = (max-min)/max
            */

            //Calculate max(r,g,b) , min(r, g, b)
            vecRGB16Comp = vdup_lane_s16(vecRGB16, 1);
            vecRGB16CompMax = vmax_s16(vecRGB16, vecRGB16Comp);
            vecRGB16CompMin = vmin_s16(vecRGB16, vecRGB16Comp);
            vecRGB16Comp = vdup_lane_s16(vecRGB16, 2);
            vecRGB16CompMax = vmax_s16(vecRGB16CompMax, vecRGB16Comp);
            vecRGB16CompMin = vmin_s16(vecRGB16CompMin, vecRGB16Comp);

            nMax = vget_lane_s16(vecRGB16CompMax, 0);
            nMin = vget_lane_s16(vecRGB16CompMin, 0);

            iDelta = nMax - nMin;
            iR = vget_lane_s16(vecRGB16, 0);
            iG = vget_lane_s16(vecRGB16, 1);
            iB = vget_lane_s16(vecRGB16, 2);

            if (iDelta == 0)
            {
                iVarH = 0;
            }
            else if (nMax == iR)
            {   //H = (g-b)/(max-min)
                iVarH = ( (iG - iB) * CVT_ONE)/iDelta;
            }
            else if (nMax == iG)
            {   // H = 2 + (b-r)/(max-min)
                iVarH = 2 * CVT_ONE + ((iB - iR) * CVT_ONE)/iDelta;
            }
            else if (nMax == iB)
            {   //H = 4 + (r-g)/(max-min)
                iVarH = 4 * CVT_ONE + ((iR - iG) * CVT_ONE)/iDelta;
            }

            //H = H * 60
            iVarH = (iVarH * 60) >> CVT_PRECISION;

            if(iVarH < 0)
                iVarH += 360;

            if (nMax == 0)
            {
                iVarS = 0;
            }
            else
            {   //S = (max-min)/max
                iVarS = ((iDelta * CVT_ONE * 255) /nMax) >> CVT_PRECISION;
            }

            //Set matrix data
            //MV_MatrixSet(psH, iRow, iCol, iVarH);
            //MV_MatrixSet(psS, iRow, iCol, iVarS);
            if ( (PCV_COLOR_FILTER_OR == sColorRange.eHFilter)
                && (PCV_COLOR_FILTER_AND == sColorRange.eSFilter) )
            {
                    if ( ((iVarH <= sColorRange.iHMin) || (iVarH >= sColorRange.iHMax) )
                        && ((iVarS >= sColorRange.iSMin) && (iVarS <= sColorRange.iSMax)) )
                    {
                        //Set Flag value to 1
                        //MV_MatrixSet_S(psFlag, iRow, iCol, 1);
						piVarFlag[iCol] = 1;
                    }
            }

            else if ( (PCV_COLOR_FILTER_AND == sColorRange.eHFilter)
                && (PCV_COLOR_FILTER_AND == sColorRange.eSFilter) )
            {
                if ( (iVarH >=sColorRange.iHMin) && (iVarH <= sColorRange.iHMax)
                && (iVarS >= sColorRange.iSMin) && (iVarS <= sColorRange.iSMax) )
                {
                    //Set Flag value to 1
                    //MV_MatrixSet_S(psFlag, iRow, iCol, 1);
					piVarFlag[iCol] = 1;
                }

            }


        }//iCol

    }//iRow

    return TRUE;
}


BOOL pcvCvtYUV2HSVAndCompOpEx_NEON(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
								 pcvHSVColorRange* psColorRange, INT iColorCount,
								 pcvRect* psFrameRect, LPMV_MATRIX_S psFlag)
{
	//=========Data definition=========//
	INT iRow = 0;
	INT iCol = 0;
	INT iCount = 0;
	MV_FIXED_S *piVarY = NULL;
	MV_FIXED_S *piVarU = NULL;
	MV_FIXED_S *piVarV = NULL;
	MV_FIXED_S *piVarFlag = NULL;
	INT yuv[4] = {0};
	INT rgb[4] = {0};
	INT iTemp = 0;
	INT iR = 0;
	INT iG = 0;
	INT iB = 0;
	int32x4_t vecYUV;
	int32x4_t vecRPara;
	int32x4_t vecGPara;
	int32x4_t vecBPara;
	int32x4_t vecR;
	int32x4_t vecG;
	int32x4_t vecB;
	int32x4_t vecRGB;
	int32x4_t vecConstHalf;
	int16x4_t vecRGB16;
	int16x4_t vecRGB16Max;
	int16x4_t vecRGB16Min;
	int16x4_t vecRGB16Comp;
	int16x4_t vecRGB16CompMin;
	int16x4_t vecRGB16CompMax;
	SHORT nMax = 0;
	SHORT nMin = 0;
	INT iDelta = 0;
	INT iVarH = 0;
	INT iVarS = 0;

	INT  iStartX = 0;
	INT  iStartY = 0;
	INT  iEndX = 0;
	INT  iEndY = 0;
	INT  ipsFlagi = 0;
	INT  ipsFlagj = 0;

	if ( (psFrameY == NULL) || (psFrameU == NULL) || (psFrameV == NULL)
		|| (psFlag == NULL) || (NULL == psColorRange) || (NULL == psFrameRect))
	{
		return FALSE;
	}

	//Initialize every element of flag matrix to 0
	//MV_MatrixSetAll_S(psFlag, 0);

	//==========Implementation==========//
	//Initial some params
	//the last parameter is always 1
	yuv[3] = 1;
	//the last element of vecRGB is 0
	vecRGB = vsetq_lane_s32(0, vecRGB, 3);

	vecRGB16Max = vdup_n_s16(255);
	vecRGB16Min = vdup_n_s16(0);
	vecConstHalf = vdupq_n_s32(CVT_HALF);

	//iVarB = 1.164 * (iVarY - 16) + 2.018 * (iVarU - 128); = 1.164 * iVarY + 2.018 * iVarU - 276.928;
	//iVarG = 1.164 * (iVarY - 16) - 0.813 * (iVarV - 128) - 0.391 * (iVarU - 128); = 1.164 * iVarY - 0.813*iVarV - 0.391*iVarU + 135.488
	//iVarR = 1.164 * (iVarY - 16) + 1.596 * (iVarV - 128); = 1.164 * iVarY + 1.596 * iVarV - 222.912;
	iTemp = (INT) (1.164f * CVT_ONE);
	vecRPara = vsetq_lane_s32(iTemp,      vecRPara, 0);
	vecRPara = vsetq_lane_s32(0,             vecRPara, 1);
	iTemp = (INT) (1.596f * CVT_ONE);
	vecRPara = vsetq_lane_s32(iTemp,      vecRPara, 2);
	iTemp = (INT) (- 222.912f * CVT_ONE);
	vecRPara = vsetq_lane_s32(iTemp, vecRPara, 3);

	iTemp = (INT) (1.164f * CVT_ONE);
	vecGPara = vsetq_lane_s32(iTemp,      vecGPara, 0);
	iTemp = (INT) (- 0.391f * CVT_ONE);
	vecGPara = vsetq_lane_s32(iTemp,      vecGPara, 1);
	iTemp = (INT) (- 0.813f * CVT_ONE);
	vecGPara = vsetq_lane_s32(iTemp,    vecGPara, 2);
	iTemp = (INT) (135.488f * CVT_ONE);
	vecGPara = vsetq_lane_s32(iTemp,   vecGPara, 3);

	iTemp = (INT) (1.164f * CVT_ONE);
	vecBPara = vsetq_lane_s32(iTemp,       vecBPara, 0);
	iTemp = (INT) (2.018f * CVT_ONE);
	vecBPara = vsetq_lane_s32(iTemp,       vecBPara, 1);
	vecBPara = vsetq_lane_s32(0,          vecBPara, 2);
	iTemp = (INT) (- 276.928f * CVT_ONE);
	vecBPara = vsetq_lane_s32(iTemp, vecBPara, 3);

	iStartX = psFrameRect->x;
	iStartY = psFrameRect->y;
	iEndX   = psFrameRect->x + psFrameRect->width - 1;
	iEndY   = psFrameRect->y + psFrameRect->height - 1;
	ipsFlagi = 0;

	if ((iStartX < 0) || (iStartY < 0) || (iEndX >= psFrameY->iCols)
		|| (iEndY >= psFrameY->iRows) || (iEndX >= psFrameU->iCols)
		|| (iEndY >= psFrameU->iRows) || (iEndX >= psFrameV->iCols)
		|| (iEndY >= psFrameV->iRows) || (iEndX >= psFlag->iCols)
		|| (iEndY >= psFlag->iRows)) {
			return FALSE;
	}

	for (iRow = iStartY; iRow <= iEndY; iRow++)
	{
		piVarY = MV_MatrixGetLinePtr_S(psFrameY, iRow, 0);
		piVarU = MV_MatrixGetLinePtr_S(psFrameU, iRow, 0);
		piVarV = MV_MatrixGetLinePtr_S(psFrameV, iRow, 0);
		piVarFlag = MV_MatrixGetLinePtr_S(psFlag, ipsFlagi, 0);
		ipsFlagi ++;
		ipsFlagj = 0;

		for (iCol = iStartX; iCol <= iEndX; iCol++)
		{
			if (0 == piVarFlag[ipsFlagj])
			{
				ipsFlagj++;
				continue;
			}
			piVarFlag[ipsFlagj] = 0;


			//Get matrix data
			yuv[0] = *(piVarY+iCol);
			yuv[1] = *(piVarU+iCol);
			yuv[2] = *(piVarV+iCol);
			//Load it from memory
			vecYUV = vld1q_s32(&yuv[0]);

			//Get R
			vecR = vmulq_s32(vecYUV, vecRPara);
			vst1q_s32(&rgb[0], vecR);
			iR = rgb[0] + rgb[1] + rgb[2] + rgb[3];
			//Get G
			vecG = vmulq_s32(vecYUV, vecGPara);
			vst1q_s32(&rgb[0], vecG);
			iG = rgb[0] + rgb[1] + rgb[2] + rgb[3];
			//Get B
			vecB = vmulq_s32(vecYUV, vecBPara);
			vst1q_s32(&rgb[0], vecB);
			iB = rgb[0] + rgb[1] + rgb[2] + rgb[3];

			vecRGB = vsetq_lane_s32(iR, vecRGB, 0);
			vecRGB = vsetq_lane_s32(iG, vecRGB, 1);
			vecRGB = vsetq_lane_s32(iB, vecRGB, 2);

			//Add 0.5f
			vecRGB = vaddq_s32(vecRGB, vecConstHalf);

			//int16x4_t vshrn_n_s32(int32x4_t a, __constrange(1,16) int b);
			vecRGB16 = vshrn_n_s32(vecRGB, CVT_PRECISION);

			//RGB Range: 0 ~ 255
			vecRGB16 = vmin_s16(vecRGB16, vecRGB16Max);
			vecRGB16 = vmax_s16(vecRGB16, vecRGB16Min);

			/* RGB -> HSV
			max=max(r, g, b);
			min=min(r, g, b);
			if r=max, H = (g-b)/(max-min)
			if g=max, H = 2 + (b-r)/(max-min)
			if b = max, H = 4 + (r-g)/(max-min)

			H = H * 60
			if H<0, H = H + 360;

			V = max(r, g, b)
			S = (max-min)/max
			*/

			//Calculate max(r,g,b) , min(r, g, b)
			vecRGB16Comp = vdup_lane_s16(vecRGB16, 1);
			vecRGB16CompMax = vmax_s16(vecRGB16, vecRGB16Comp);
			vecRGB16CompMin = vmin_s16(vecRGB16, vecRGB16Comp);
			vecRGB16Comp = vdup_lane_s16(vecRGB16, 2);
			vecRGB16CompMax = vmax_s16(vecRGB16CompMax, vecRGB16Comp);
			vecRGB16CompMin = vmin_s16(vecRGB16CompMin, vecRGB16Comp);

			nMax = vget_lane_s16(vecRGB16CompMax, 0);
			nMin = vget_lane_s16(vecRGB16CompMin, 0);

			iDelta = nMax - nMin;
			iR = vget_lane_s16(vecRGB16, 0);
			iG = vget_lane_s16(vecRGB16, 1);
			iB = vget_lane_s16(vecRGB16, 2);

			if (iDelta == 0)
			{
				iVarH = 0;
			}
			else if (nMax == iR)
			{   //H = (g-b)/(max-min)
				iVarH = ( (iG - iB) * CVT_ONE)/iDelta;
			}
			else if (nMax == iG)
			{   // H = 2 + (b-r)/(max-min)
				iVarH = 2 * CVT_ONE + ((iB - iR) * CVT_ONE)/iDelta;
			}
			else if (nMax == iB)
			{   //H = 4 + (r-g)/(max-min)
				iVarH = 4 * CVT_ONE + ((iR - iG) * CVT_ONE)/iDelta;
			}

			//H = H * 60
			iVarH = (iVarH * 60) >> CVT_PRECISION;

			if(iVarH < 0)
				iVarH += 360;

			if (nMax == 0)
			{
				iVarS = 0;
			}
			else
			{   //S = (max-min)/max
				iVarS = ((iDelta * CVT_ONE * 255) /nMax) >> CVT_PRECISION;
			}

			//Set matrix data
			//MV_MatrixSet(psH, iRow, iCol, iVarH);
			//MV_MatrixSet(psS, iRow, iCol, iVarS);
			for (iCount = 0; iCount < iColorCount; iCount++)
			{
				if ( (PCV_COLOR_FILTER_OR == psColorRange[iCount].eHFilter)
					&& (PCV_COLOR_FILTER_AND == psColorRange[iCount].eSFilter) )
				{
					if ( ((iVarH <= psColorRange[iCount].iHMin) || (iVarH >= psColorRange[iCount].iHMax) )
						&& ((iVarS >= psColorRange[iCount].iSMin) && (iVarS <= psColorRange[iCount].iSMax)) )
					{
						//Set Flag value to 1
						//MV_MatrixSet_S(psFlag, iRow, iCol, 1);
						piVarFlag[ipsFlagj] = iCount + 1;
					}
				}

				else if ( (PCV_COLOR_FILTER_AND == psColorRange[iCount].eHFilter)
					&& (PCV_COLOR_FILTER_AND == psColorRange[iCount].eSFilter) )
				{
					if ( (iVarH >=psColorRange[iCount].iHMin) && (iVarH <= psColorRange[iCount].iHMax)
						&& (iVarS >= psColorRange[iCount].iSMin) && (iVarS <= psColorRange[iCount].iSMax) )
					{
						//Set Flag value to 1
						//MV_MatrixSet_S(psFlag, iRow, iCol, 1);
						piVarFlag[ipsFlagj] = iCount + 1;
					}
				}

			}

			ipsFlagj ++;


		}//iCol

	}//iRow

	return TRUE;
}

#endif

static BOOL s_bCvtYUV2RGBTableInit = FALSE;
MV_FIXED iP00[256];
MV_FIXED iP01[256];
MV_FIXED iP11[256];
MV_FIXED iP12[256];
MV_FIXED iP21[256];
VOID MakeCvtYCrCb2RGBTable()
{
    INT i = 0;

    for (i = 0; i < 256; ++i)
    {
        //Convert YCrCb(Android Camera: YCbCr_420_SP) to RGB
        //iVarB = 1.164 * (iVarY - 16) + 2.018 * (iVarU - 128); = 1.164 * iVarY + 2.018 * iVarU - 276.928;
        //iVarG = 1.164 * (iVarY - 16) - 0.813 * (iVarV - 128) - 0.391 * (iVarU - 128); = 1.164 * iVarY - 0.813*iVarV - 0.391*iVarU + 135.488
        //iVarR = 1.164 * (iVarY - 16) + 1.596 * (iVarV - 128); = 1.164 * iVarY + 1.596 * iVarV - 222.912;
        iP00[i] = MV_FixedFromFloat(i*1.164);
        iP01[i] = MV_FixedFromFloat(i*2.018);
        iP11[i] = MV_FixedFromFloat(i*0.813);
        iP12[i] = MV_FixedFromFloat(i*0.391);
        iP21[i] = MV_FixedFromFloat(i*1.596);
    }
}

BOOL pcvCvtYUV2HSV_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
              LPMV_MATRIX_S psH, LPMV_MATRIX_S psS,  LPMV_MATRIX_S psV)
{
    //=========Data definition=========//
    INT iRow = 0;
    INT iCol = 0;
    MV_FIXED_S *piVarY = NULL;
    MV_FIXED_S *piVarU = NULL;
    MV_FIXED_S *piVarV = NULL;
    MV_FIXED_S *piVarH = NULL;
	MV_FIXED_S *piVarS = NULL;
    INT iVarY = 0;
    INT iVarU = 0;
    INT iVarV = 0;
    MV_FIXED iVarB = 0;
    MV_FIXED iVarG = 0;
    MV_FIXED iVarR = 0;
    MV_FIXED iMax = 0;
    MV_FIXED iMin = 0;
    MV_FIXED iDelta = 0;
    MV_FIXED iVarH = 0;
    MV_FIXED iVarS = 0;
    MV_FIXED FixValue_255 = 0;
    MV_FIXED FixValue_60 = 0;
    MV_FIXED FixValue_120 = 0;
    MV_FIXED FixValue_240 = 0;
    MV_FIXED FixValue_360 = 0;

    MV_FIXED FixValue_276 = 0;
    MV_FIXED FixValue_135 = 0;
    MV_FIXED FixValue_222 = 0;

    if ( (psFrameY == NULL) ||  (psFrameU == NULL) || (psFrameV == NULL)
        || (psH == NULL) || (psS == NULL) )
    {
        return FALSE;
    }

	if ((psFrameU->iRows < psFrameY->iRows) || (psFrameU->iCols < psFrameY->iCols)
		|| (psFrameV->iRows < psFrameY->iRows) || (psFrameV->iCols < psFrameY->iCols)
		|| (psH->iRows < psFrameY->iRows) || (psH->iCols < psFrameY->iCols)
		|| (psS->iRows < psFrameY->iRows) || (psS->iCols < psFrameY->iCols)) {
		return FALSE;
	}

    if (FALSE == s_bCvtYUV2RGBTableInit) {
        MakeCvtYCrCb2RGBTable();
        s_bCvtYUV2RGBTableInit = TRUE;
    }

    //==========Implementation==========//
    //Initial some params
    FixValue_255 = MV_FixedFromInt(255);
    FixValue_60 = MV_FixedFromInt(60);
    FixValue_120 = MV_FixedFromInt(120);
    FixValue_240 = MV_FixedFromInt(240);
    FixValue_360 = MV_FixedFromInt(360);

    FixValue_276 = MV_FixedFromFloat(276.928);
    FixValue_135 = MV_FixedFromFloat(135.488);
    FixValue_222 = MV_FixedFromFloat(222.912);

	iVarH = 0;

    for (iRow = 0; iRow < psFrameY->iRows; iRow++)
    {
        piVarY = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameY, iRow, 0);
        piVarU = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameU, iRow, 0);
        piVarV = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameV, iRow, 0);
		piVarH = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psH, iRow, 0);
		piVarS = (MV_FIXED_S*) MV_MatrixGetLinePtr_S(psS, iRow, 0);

        for (iCol = 0; iCol < psFrameY->iCols; iCol++)
        {
            //Get matrix data
            iVarY = *(piVarY+iCol); //MV_MatrixGet_S(psFrameY, iRow, iCol);
            iVarU = *(piVarU+iCol); //MV_MatrixGet_S(psFrameU, iRow, iCol);
            iVarV = *(piVarV+iCol); //MV_MatrixGet_S(psFrameV, iRow, iCol);

            //Convert YCrCb(Android Camera: YCbCr_420_SP) to RGB
            //iVarB = 1.164 * (iVarY - 16) + 2.018 * (iVarU - 128); = 1.164 * iVarY + 2.018 * iVarU - 276.928;
            //iVarG = 1.164 * (iVarY - 16) - 0.813 * (iVarV - 128) - 0.391 * (iVarU - 128); = 1.164 * iVarY - 0.813*iVarV - 0.391*iVarU + 135.488
            //iVarR = 1.164 * (iVarY - 16) + 1.596 * (iVarV - 128); = 1.164 * iVarY + 1.596 * iVarV - 222.912;
            iVarB = iP00[iVarY] + iP01[iVarU] - FixValue_276;
            iVarG = iP00[iVarY] - iP11[iVarV] - iP12[iVarU] + FixValue_135;
            iVarR = iP00[iVarY] + iP21[iVarV] - FixValue_222;


            if (iVarB > FixValue_255)
                iVarB = FixValue_255;

            if (iVarG > FixValue_255)
                iVarG = FixValue_255;

            if (iVarR > FixValue_255)
                iVarR = FixValue_255;

            if (iVarB < 0)
                iVarB = 0;

            if (iVarG < 0)
                iVarG = 0;

            if (iVarR < 0)
                iVarR = 0;

            //Calculate HSV
            iMax = MV_MAX(iVarR, MV_MAX(iVarG, iVarB));
            iMin = MV_MIN(iVarR, MV_MIN(iVarG, iVarB));

            iDelta = iMax - iMin;

            if (iDelta == 0)
            {
                iVarH = 0;
            }
            else if (iMax == iVarR)
            {
                iVarH = MV_Div(MV_Mul(FixValue_60, iVarG - iVarB), iDelta);
            }
            else if (iMax == iVarG)
            {
                iVarH = MV_Div(MV_Mul(FixValue_60, iVarB - iVarR), iDelta) + FixValue_120;
            }
            else if (iMax == iVarB)
            {
                iVarH = MV_Div(MV_Mul(FixValue_60, iVarR - iVarG), iDelta) + FixValue_240;
            }

            if(iVarH < 0)
                iVarH += FixValue_360;

            if (iMax == 0)
            {
                iVarS = 0;
            }
            else
            {
                iVarS = MV_ONE - MV_Div(iMin, iMax);
            }

            //Set matrix data
            //MV_MatrixSet_S(psH, iRow, iCol, MV_IntFromFixed(iVarH));
			piVarH[iCol] =  MV_IntFromFixed(iVarH);
           // MV_MatrixSet_S(psS, iRow, iCol, MV_IntFromFixed(MV_Mul(iVarS, FixValue_255)));
			piVarS[iCol] = MV_IntFromFixed(MV_Mul(iVarS, FixValue_255));

        }//iCol

    }//iRow

    return TRUE;
}

BOOL pcvCvtYUV2HSVAndCompOp_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
                            pcvHSVColorRange sColorRange, LPMV_MATRIX_S psFlag)
{
    //=========Data definition=========//
    INT iRow = 0;
    INT iCol = 0;
    MV_FIXED_S *piVarY = NULL;
    MV_FIXED_S *piVarU = NULL;
    MV_FIXED_S *piVarV = NULL;
	MV_FIXED_S *piVarFlag = NULL;
    INT iVarY = 0;
    INT iVarU = 0;
    INT iVarV = 0;
    SHORT nH = 0;
    SHORT nS = 0;
    MV_FIXED iVarB = 0;
    MV_FIXED iVarG = 0;
    MV_FIXED iVarR = 0;
    MV_FIXED iMax = 0;
    MV_FIXED iMin = 0;
    MV_FIXED iDelta = 0;
    MV_FIXED iVarH = 0;
    MV_FIXED iVarS = 0;
    MV_FIXED FixValue_255 = 0;
    MV_FIXED FixValue_60 = 0;
    MV_FIXED FixValue_120 = 0;
    MV_FIXED FixValue_240 = 0;
    MV_FIXED FixValue_360 = 0;
//  MV_FIXED FixValueY = 0;

    MV_FIXED FixValue_276;
    MV_FIXED FixValue_135;
    MV_FIXED FixValue_222;

    if ( (psFrameY == NULL) ||  (psFrameU == NULL) || (psFrameV == NULL) || (psFlag == NULL) )
    {
        return FALSE;
    }

	if ((psFrameU->iRows < psFrameY->iRows) || (psFrameU->iCols < psFrameY->iCols)
		|| (psFrameV->iRows < psFrameY->iRows) || (psFrameV->iCols < psFrameY->iCols)
		|| (psFlag->iRows < psFrameY->iRows) || (psFlag->iCols < psFrameY->iCols)) {
			return FALSE;
	}

    //Initialize every element of flag matrix to 0
    MV_MatrixSetAll_S(psFlag, 0);

    if (FALSE == s_bCvtYUV2RGBTableInit) {
        MakeCvtYCrCb2RGBTable();
        s_bCvtYUV2RGBTableInit = TRUE;
    }

    //==========Implementation==========//
    //Initial some params
    FixValue_255 = MV_FixedFromInt(255);
    FixValue_60 = MV_FixedFromInt(60);
    FixValue_120 = MV_FixedFromInt(120);
    FixValue_240 = MV_FixedFromInt(240);
    FixValue_360 = MV_FixedFromInt(360);

    FixValue_276 = MV_FixedFromFloat(276.928);
    FixValue_135 = MV_FixedFromFloat(135.488);
    FixValue_222 = MV_FixedFromFloat(222.912);

	iVarH = 0;

    for (iRow = 0; iRow < psFrameY->iRows; iRow++)
    {
        piVarY = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameY, iRow, 0);
        piVarU = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameU, iRow, 0);
        piVarV = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameV, iRow, 0);
		piVarFlag = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFlag, iRow, 0);

        for (iCol = 0; iCol < psFrameY->iCols; iCol++)
        {
            //Get matrix data
            iVarY = *(piVarY+iCol); //MV_MatrixGet_S(psFrameY, iRow, iCol);
            iVarU = *(piVarU+iCol); //MV_MatrixGet_S(psFrameU, iRow, iCol);
            iVarV = *(piVarV+iCol); //MV_MatrixGet_S(psFrameV, iRow, iCol);

            //Convert YCrCb(Android Camera: YCbCr_420_SP) to RGB
            //iVarB = 1.164 * (iVarY - 16) + 2.018 * (iVarU - 128); = 1.164 * iVarY + 2.018 * iVarU - 276.928;
            //iVarG = 1.164 * (iVarY - 16) - 0.813 * (iVarV - 128) - 0.391 * (iVarU - 128); = 1.164 * iVarY - 0.813*iVarV - 0.391*iVarU + 135.488
            //iVarR = 1.164 * (iVarY - 16) + 1.596 * (iVarV - 128); = 1.164 * iVarY + 1.596 * iVarV - 222.912;
            iVarB = iP00[iVarY] + iP01[iVarU] - FixValue_276;
            iVarG = iP00[iVarY] - iP11[iVarV] - iP12[iVarU] + FixValue_135;
            iVarR = iP00[iVarY] + iP21[iVarV] - FixValue_222;


            if (iVarB > FixValue_255)
                iVarB = FixValue_255;

            if (iVarG > FixValue_255)
                iVarG = FixValue_255;

            if (iVarR > FixValue_255)
                iVarR = FixValue_255;

            if (iVarB < 0)
                iVarB = 0;

            if (iVarG < 0)
                iVarG = 0;

            if (iVarR < 0)
                iVarR = 0;

            //Calculate HSV
            iMax = MV_MAX(iVarR, MV_MAX(iVarG, iVarB));
            iMin = MV_MIN(iVarR, MV_MIN(iVarG, iVarB));

            iDelta = iMax - iMin;

            if (iDelta == 0)
            {
                iVarH = 0;
            }
            else if (iMax == iVarR)
            {
                iVarH = MV_Div(MV_Mul(FixValue_60, iVarG - iVarB), iDelta);
            }
            else if (iMax == iVarG)
            {
                iVarH = MV_Div(MV_Mul(FixValue_60, iVarB - iVarR), iDelta) + FixValue_120;
            }
            else if (iMax == iVarB)
            {
                iVarH = MV_Div(MV_Mul(FixValue_60, iVarR - iVarG), iDelta) + FixValue_240;
            }

            if(iVarH < 0)
                iVarH += FixValue_360;

            if (iMax == 0)
            {
                iVarS = 0;
            }
            else
            {
                iVarS = MV_ONE - MV_Div(iMin, iMax);
            }

            //Set matrix data
            //MV_MatrixSet(psH, iRow, iCol, MV_IntFromFixed(iVarH));
            //MV_MatrixSet(psS, iRow, iCol, MV_IntFromFixed(MV_Mul(iVarS, FixValue_255)));
            nH = (SHORT)MV_IntFromFixed(iVarH);
            nS = (SHORT)MV_IntFromFixed(MV_Mul(iVarS, FixValue_255));
            if ( (PCV_COLOR_FILTER_OR == sColorRange.eHFilter)
                && (PCV_COLOR_FILTER_AND == sColorRange.eSFilter) )
            {
                if ( ((nH <= sColorRange.iHMin) || (nH >= sColorRange.iHMax) )
                    && ((nS >= sColorRange.iSMin) && (nS <= sColorRange.iSMax)) )
                {
                    //Set Flag value to 1
                    //MV_MatrixSet_S(psFlag, iRow, iCol, 1);
					piVarFlag[iCol] = 1;
                }
            }

            else if ( (PCV_COLOR_FILTER_AND == sColorRange.eHFilter)
                && (PCV_COLOR_FILTER_AND == sColorRange.eSFilter) )
            {
                if ( (nH >=sColorRange.iHMin) && (nH <= sColorRange.iHMax)
                    && (nS >= sColorRange.iSMin) && (nS <= sColorRange.iSMax) )
                {
                    //Set Flag value to 1
                    //MV_MatrixSet_S(psFlag, iRow, iCol, 1);
					piVarFlag[iCol] = 1;
                }

            }

        }//iCol

    }//iRow

    return TRUE;
}

//#endif

BOOL pcvCvtYUV2HSVAndCompOpEx_C(LPMV_MATRIX_S psFrameY, LPMV_MATRIX_S psFrameU, LPMV_MATRIX_S psFrameV,
							  pcvHSVColorRange* psColorRange, INT iColorCount,
							  pcvRect* psFrameRect, LPMV_MATRIX_S psFlag)
{
	//=========Data definition=========//
	INT iRow = 0;
	INT iCol = 0;
	INT iCount = 0;
	MV_FIXED_S *piVarY = NULL;
	MV_FIXED_S *piVarU = NULL;
	MV_FIXED_S *piVarV = NULL;
	MV_FIXED_S *piVarFlag = NULL;
	INT iVarY = 0;
	INT iVarU = 0;
	INT iVarV = 0;
	SHORT nH = 0;
	SHORT nS = 0;
	MV_FIXED iVarB = 0;
	MV_FIXED iVarG = 0;
	MV_FIXED iVarR = 0;
	MV_FIXED iMax = 0;
	MV_FIXED iMin = 0;
	MV_FIXED iDelta = 0;
	MV_FIXED iVarH = 0;
	MV_FIXED iVarS = 0;
	MV_FIXED FixValue_255 = 0;
	MV_FIXED FixValue_60 = 0;
	MV_FIXED FixValue_120 = 0;
	MV_FIXED FixValue_240 = 0;
	MV_FIXED FixValue_360 = 0;
//	MV_FIXED FixValueY = 0;

	MV_FIXED FixValue_276 = 0;
	MV_FIXED FixValue_135 = 0;
	MV_FIXED FixValue_222 = 0;

	INT  iStartX = 0;
	INT  iStartY = 0;
	INT  iEndX = 0;
	INT  iEndY = 0;
	INT  ipsFlagi = 0;
	INT  ipsFlagj = 0;

	if ((psFrameY == NULL) || (psFrameU == NULL) || (psFrameV == NULL) 
		|| (psFlag == NULL) || (NULL == psColorRange) || (NULL == psFrameRect))
	{
		return FALSE;
	}

	//Initialize every element of flag matrix to 0
	//MV_MatrixSetAll_S(psFlag, 0);

	if (FALSE == s_bCvtYUV2RGBTableInit) {
		MakeCvtYCrCb2RGBTable();
		s_bCvtYUV2RGBTableInit = TRUE;
	}

	//==========Implementation==========//
	//Initial some params
	FixValue_255 = MV_FixedFromInt(255);
	FixValue_60 = MV_FixedFromInt(60);
	FixValue_120 = MV_FixedFromInt(120);
	FixValue_240 = MV_FixedFromInt(240);
	FixValue_360 = MV_FixedFromInt(360);

	FixValue_276 = MV_FixedFromFloat(276.928);
	FixValue_135 = MV_FixedFromFloat(135.488);
	FixValue_222 = MV_FixedFromFloat(222.912);

	iVarH = 0;

	iStartX = psFrameRect->x;
	iStartY = psFrameRect->y;
	iEndX   = psFrameRect->x + psFrameRect->width - 1;
	iEndY   = psFrameRect->y + psFrameRect->height - 1;
    ipsFlagi = 0;

	if ((iStartX < 0) || (iStartY < 0) || (iEndX >= psFrameY->iCols)
		|| (iEndY >= psFrameY->iRows) || (iEndX >= psFrameU->iCols)
		|| (iEndY >= psFrameU->iRows) || (iEndX >= psFrameV->iCols)
		|| (iEndY >= psFrameV->iRows) || (iEndX >= psFlag->iCols)
		|| (iEndY >= psFlag->iRows)) {
		return FALSE;
	}

	for (iRow = iStartY; iRow <= iEndY; iRow++)
	{
		piVarY = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameY, iRow, 0);
		piVarU = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameU, iRow, 0);
		piVarV = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFrameV, iRow, 0);
		piVarFlag = (MV_FIXED_S*)MV_MatrixGetLinePtr_S(psFlag, ipsFlagi, 0);

		ipsFlagi ++;
		ipsFlagj = 0;

		for (iCol = iStartX; iCol <= iEndX; iCol++)
		{
			if (0 == piVarFlag[ipsFlagj])
			{
				ipsFlagj++;
				continue;
			}
			piVarFlag[ipsFlagj] = 0;

			//Get matrix data
			iVarY = *(piVarY+iCol); //MV_MatrixGet_S(psFrameY, iRow, iCol);
			iVarU = *(piVarU+iCol); //MV_MatrixGet_S(psFrameU, iRow, iCol);
			iVarV = *(piVarV+iCol); //MV_MatrixGet_S(psFrameV, iRow, iCol);

			//Convert YCrCb(Android Camera: YCbCr_420_SP) to RGB
			//iVarB = 1.164 * (iVarY - 16) + 2.018 * (iVarU - 128); = 1.164 * iVarY + 2.018 * iVarU - 276.928;
			//iVarG = 1.164 * (iVarY - 16) - 0.813 * (iVarV - 128) - 0.391 * (iVarU - 128); = 1.164 * iVarY - 0.813*iVarV - 0.391*iVarU + 135.488
			//iVarR = 1.164 * (iVarY - 16) + 1.596 * (iVarV - 128); = 1.164 * iVarY + 1.596 * iVarV - 222.912;
			iVarB = iP00[iVarY] + iP01[iVarU] - FixValue_276;
			iVarG = iP00[iVarY] - iP11[iVarV] - iP12[iVarU] + FixValue_135;
			iVarR = iP00[iVarY] + iP21[iVarV] - FixValue_222;


			if (iVarB > FixValue_255)
				iVarB = FixValue_255;

			if (iVarG > FixValue_255)
				iVarG = FixValue_255;

			if (iVarR > FixValue_255)
				iVarR = FixValue_255;

			if (iVarB < 0)
				iVarB = 0;

			if (iVarG < 0)
				iVarG = 0;

			if (iVarR < 0)
				iVarR = 0;

			//Calculate HSV
			iMax = MV_MAX(iVarR, MV_MAX(iVarG, iVarB));
			iMin = MV_MIN(iVarR, MV_MIN(iVarG, iVarB));

			iDelta = iMax - iMin;

			if (iDelta == 0)
			{
				iVarH = 0;
			}
			else if (iMax == iVarR)
			{
				iVarH = MV_Div(MV_Mul(FixValue_60, iVarG - iVarB), iDelta);
			}
			else if (iMax == iVarG)
			{
				iVarH = MV_Div(MV_Mul(FixValue_60, iVarB - iVarR), iDelta) + FixValue_120;
			}
			else if (iMax == iVarB)
			{
				iVarH = MV_Div(MV_Mul(FixValue_60, iVarR - iVarG), iDelta) + FixValue_240;
			}

			if(iVarH < 0)
				iVarH += FixValue_360;

			if (iMax == 0)
			{
				iVarS = 0;
			}
			else
			{
				iVarS = MV_ONE - MV_Div(iMin, iMax);
			}

			//Set matrix data
			//MV_MatrixSet(psH, iRow, iCol, MV_IntFromFixed(iVarH));
			//MV_MatrixSet(psS, iRow, iCol, MV_IntFromFixed(MV_Mul(iVarS, FixValue_255)));
			nH = (SHORT)MV_IntFromFixed(iVarH);
			nS = (SHORT)MV_IntFromFixed(MV_Mul(iVarS, FixValue_255));
			for(iCount = 0; iCount < iColorCount;iCount++)
			{
				if ( (PCV_COLOR_FILTER_OR == psColorRange[iCount].eHFilter)
					&& (PCV_COLOR_FILTER_AND == psColorRange[iCount].eSFilter) )
				{
					if ( ((nH <= psColorRange[iCount].iHMin) || (nH >= psColorRange[iCount].iHMax) )
						&& ((nS >= psColorRange[iCount].iSMin) && (nS <= psColorRange[iCount].iSMax)) )
					{
						//Set Flag value to 1
						//MV_MatrixSet_S(psFlag, iRow, iCol, 1);
						piVarFlag[ipsFlagj] = iCount + 1;
					}
				}
				else if ( (PCV_COLOR_FILTER_AND == psColorRange[iCount].eHFilter)
					&& (PCV_COLOR_FILTER_AND == psColorRange[iCount].eSFilter) )
				{
					if ( (nH >= psColorRange[iCount].iHMin) && (nH <= psColorRange[iCount].iHMax)
						&& (nS >= psColorRange[iCount].iSMin) && (nS <= psColorRange[iCount].iSMax) )
					{
						//Set Flag value to 1
						//MV_MatrixSet_S(psFlag, iRow, iCol, 1);
						piVarFlag[ipsFlagj] = iCount + 1;
					}

				}

			}

			ipsFlagj ++;

		}//iCol

	}//iRow

	return TRUE;
}

//#endif




//For Win32
#ifdef WIN32

static BOOL s_bCvtRGB2YUVTableInit  = FALSE;

DOUBLE f00[256], f01[256], f02[256],f10[256], f11[256], f12[256],f20[256], f21[256], f22[256];
void MakeConversionTable()
{
    long i = 0;
    for (i=0; i<256; i++)
    {
        //Y
        f00[i] = (i*65738/256)/1000 + 16;
        f01[i] = (i*129057/256)/1000;
        f02[i] = (i*25064/256)/1000;
        //U
        f10[i] = (i*(-37945)/256)/1000 + 128;
        f11[i] = (i*(-74494)/256)/1000;
        f12[i] = (i*112439/256)/1000;
        //V
        f20[i] = (i*112439/256)/1000 + 128;
        f21[i] = (i*(-94154)/256)/1000;
        f22[i] = (i*(-18285)/256)/1000;
    }
}


void  pcvRGB2YUV(BYTE r, BYTE g, BYTE b, BYTE *lpY, BYTE *lpU, BYTE *lpV)
{
    double temp = 0.0; 

	if ((NULL == lpY) || (NULL == lpU) || (NULL == lpV)) {
		return;
	}

    if (FALSE == s_bCvtRGB2YUVTableInit) {
        MakeConversionTable();
        s_bCvtRGB2YUVTableInit = TRUE;
    }

    temp = f00[r] + f01[g] + f02[b];
    if(temp < 16)
    {
        *lpY = 16;
    }
    else if(temp > 235)
    {
        *lpY = 235;
    }
    else
    {
        *lpY = (BYTE)temp;
    }

    temp = f10[r] + f11[g] + f12[b];
    if(temp < 16)
    {
        *lpU = 16;
    }
    else if(temp > 240)
    {
        *lpU = 240;
    }
    else
    {
        *lpU = (BYTE)temp;
    }

    temp = f20[r] + f21[g] + f22[b];
    if(temp < 16)
    {
        *lpV = 16;
    }
    else if(temp > 240)
    {
        *lpV = 240;
    }
    else
    {
        *lpV = (BYTE)temp;
    }

}

#endif



/* EOF */
