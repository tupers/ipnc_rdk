/*
 * SDS_CommStruct.h
 *
 *  Created on: 2016-5-6
 *      Author: Fisher
 */

#ifndef SDS_COMMSTRUCT_H_
#define SDS_COMMSTRUCT_H_
#include "AdasCommonDataStructure.h"

typedef struct _SDSSendData
{
	unsigned char nStatusCode;
	unsigned char nWarningCode;

	//圆心用于计算
	float   Cir1_PixelX;//将下面6个变量类型Uint改为float cjs20120911
	float   Cir1_PixelY;
	float   Cir2_PixelX;
	float   Cir2_PixelY;
	float   Cir3_PixelX;
	float   Cir3_PixelY;
	float   Cir4_PixelX;
	float   Cir4_PixelY;
	float   Cir5_PixelX;
	float   Cir5_PixelY;

	//重心用于显示
	float   GCir1_PixelX;
	float   GCir1_PixelY;
	float   GCir2_PixelX;
	float   GCir2_PixelY;
	float   GCir3_PixelX;
	float   GCir3_PixelY;
	float   GCir4_PixelX;
	float   GCir4_PixelY;
	float   GCir5_PixelX;
	float   GCir5_PixelY;

	//圆心用于显示
	float   CCir1_PixelX;
	float   CCir1_PixelY;
	float   CCir2_PixelX;
	float   CCir2_PixelY;
	float   CCir3_PixelX;
	float   CCir3_PixelY;
	float   CCir4_PixelX;
	float   CCir4_PixelY;
	float   CCir5_PixelX;
	float   CCir5_PixelY;
	//半径用于显示
	float   Cir1_r;
	float   Cir2_r;
	float   Cir3_r;
	float   Cir4_r;
	float   Cir5_r;
	//半径误差用于显示
	float   Cir1_f;
	float   Cir2_f;
	float   Cir3_f;
	float   Cir4_f;
	float   Cir5_f;
	//Block整个区域的像素数用于显示
//	int Cir1_area;
//	int Cir2_area;
//	int Cir3_area;
//	int Cir4_area;
//	int Cir5_area;
	//Block整个区域的255像素数用于显示
//	int Cir1_Total255;
//	int Cir2_Total255;
//	int Cir3_Total255;
//	int Cir4_Total255;
//	int Cir5_Total255;


	//Block搜多框大小
	AdasBlobT Cir1_bt;
	AdasBlobT Cir2_bt;
	AdasBlobT Cir3_bt;
	AdasBlobT Cir4_bt;
	AdasBlobT Cir5_bt;

	//Uint    nTimeStamp;
	//FILETIME nTimeStamp;
	unsigned int nTimeStamp;
	int     ErrorIndex;
	unsigned char	threshold[5];
}SDSSendData;




typedef struct _Range
{
	int  iMin;	///< minimal value
	int  iMax;	///< maximal value
}Range;

typedef struct _ColorRange
{
	Range sHSV_H;		//HSV: Hue
	Range sHSV_S;		//HSV: Saturation

}ColorRange;

typedef struct {
	float fOpticalCenterX;
	float fOpticalCenterY;
	int      iFocalLengthX;
	int      iFocalLengthY;
	int      iFrameRate;
	int      iImageWidth;
	int      iImageHeight;
} MV_S_IntrinsicCamPara;

typedef struct {
	float fPitch;
	float fRoll;
	float fYaw;
	float fCameraHeight;
	int      iROIX;
	int      iROIY;
	int      iROIWidth;
	int      iROIHeight;
	float fLateralOffset;
	float fVehicleWidth;
} MV_S_ExtrinsicCamPara;


/**
* Camera color params
* Hue range: [0, 360]
* Saturation range: [0, 255], i.e. the standard value range [0, 1] * 255
*/
typedef struct {
	//Road sign detection
	ColorRange sRoadSign_Blue;
	ColorRange sRoadSign_Green;
	int iMinRoadSignSatu;
} MV_S_RSGDColorPara;


typedef struct {
	int     iOffsetX;
	int     iOffsetY;
	float   fZoomRateX;
	float   fZoomRateY;
} MV_S_CamCtrlPara;

typedef struct {
	float   fNightExposure;
} MV_S_CamControlPara;

typedef struct {
	MV_S_IntrinsicCamPara    sIntrinsicCamPara;
	MV_S_ExtrinsicCamPara    sExtrinsicCamPara;
	MV_S_CamCtrlPara         sCtrlPara;
	MV_S_RSGDColorPara		 sRSGDColorPara;
	MV_S_CamControlPara	     sControlPara;
} MV_S_CamPara;

extern MV_S_CamPara  g_sCamPara[10];




#endif /* SDS_COMMSTRUCT_H_ */
