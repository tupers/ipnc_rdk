/*
 * SDS_Dynamic_Params.h
 *
 *  Created on: 2016Äê11ÔÂ22ÈÕ
 *      Author: tupers
 */

#ifndef SDS_DYNAMIC_PARAMS_H_
#define SDS_DYNAMIC_PARAMS_H_

typedef struct SDS_Dynamic_Params
{
	float ROI_startX;
	float ROI_endX;
	float ROI_startY;
	float ROI_endY;
	float f_RDifSideCir;
	float fCirRadius_1;
	float fCirRadius_2;
	float fCirGood_1;
	float fCirGood_2;
	float fCirGood_3;
	float fCirWarning_1;
	float fCirWarning_2;
	float fCirWarning_3;
	float fCenterCirGoodOffset;
	float fCenterCirWarningOffset;
	unsigned int nBiggestRaduis;
	unsigned int nSmallestRaduis;
	unsigned int Blot_BiggestArea;
	unsigned int Blot_SmallestArea;
	unsigned int Blot_xyOffset;
	unsigned int Cir_SmallestArea;
	unsigned int Cir_xyOffset;
	float Cir_errf;
	unsigned int Judge_xyOffset;
	float Speed_k1;
	float Speed_k2;
	unsigned int Speed_ExpandPixel1;
	unsigned int Speed_ExpandPixel2;
}SDS_DParams;

VOID SDS_SetDynamicParam(SDS_DParams *pPrm);


#endif /* SDS_DYNAMIC_PARAMS_H_ */
