#ifndef SDS_EXPORT_H_
#include "SDS_Export.h"
#endif
//#include "utils_edma.h"
#ifdef MVCAMERAPARA_H_
#include "MVCameraPara.h"
#endif

#ifndef ADAS_VEHICLE_LICENSE_DETECTION_H
#include "SDS_Detection.h"
#endif

static CameraParams CAMERA_PARAM;   // 相机标定参数


void GetPara( P_CAMERA_TYPE  CameraType, CameraParams* sCamP );


BOOL SDS_GetROIMatrix(INT PixRows, INT PixCols, AdasImageROI *sRoi, MV_MATRIX_S *pMatDst);


void IMGPRO_SDS_INITIALIZE(  P_CAMERA_TYPE  CameraType )
{

	memset( &CAMERA_PARAM, 0, sizeof(CameraParams) );

	GetPara( CameraType, &CAMERA_PARAM );

	VLD_CreateInstance();
	VLD_SetInitialParams(&CAMERA_PARAM);

}

// 一维数组按照行方式排序
P_SDS_ERRORCODE IMGPRO_SDS_Analysis(MV_FIXED_S* src,INT PixRows, INT PixCols, INT iFrameIndex,  SDSSendData* pSdsOutput  )
{

	if( FALSE == SDS_GetROIMatrix(PixRows, PixCols, &(g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI), g_VLD_Params.m_piPreProcessFrame) )
	{
		return ErrCode_InvalidSize;
	}

	g_VLD_Params.m_piTransImage->pDataAlign=src;
	//SDS_GetROIMatrix( pMatSrc, PixRows, PixCols, &(g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI), g_VLD_Params.m_piCompressFrame_R);

	//SDS_GetROIMatrix( pMatSrc, PixRows, PixCols, &(g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI), g_VLD_Params.m_piCompressFrame_G);

	//SDS_GetROIMatrix( pMatSrc, PixRows, PixCols, &(g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI), g_VLD_Params.m_piCompressFrame_B);
	//Get frame index
	g_VLD_Params.m_iFrameIndex = iFrameIndex;
	//Implement image analysis

	VLD_DetectionProcess(&CAMERA_PARAM);
	*pSdsOutput = g_VLD_Params.m_pResultData;//wxf 2016-10-19

	if( g_VLD_Params.m_pResultData.nStatusCode != 0 )
	{
		return ErrCode_AlgoriFault;
	}

	return ErrCode_OK;

}

BOOL SDS_GetROIMatrix( INT PixRows, INT PixCols, AdasImageROI *sRoi, MV_MATRIX_S *pMatDst)
{
//	INT  iRow, iCol;
//	LPMV_MATRIX_S pchMVFrame = NULL;

//	MV_FIXED_S	*pSrcLineAddr = NULL;
//	MV_FIXED_S	*pDstLineAddr = NULL;

	// Check input image size

	if( ( ( sRoi->iY + sRoi->iHeight > PixRows) || ( sRoi->iX + sRoi->iWidth > PixCols ) || ( PixRows < sRoi->iY) || ( PixCols < sRoi->iX)) )
	{
		return FALSE;
	}

//	pchMVFrame = pMatDst;
//	pDstLineAddr = MV_MatrixGetLinePtr_S(pMatDst,0,0);
//	edma_copy2D(pDstLineAddr,pMatSrc,PixCols,PixRows,1);
//	for (iRow = sRoi->iY; iRow < sRoi->iY + sRoi->iHeight; iRow++)
//	{
//		pSrcLineAddr = pMatSrc + iRow * PixCols;
//		pDstLineAddr = MV_MatrixGetLinePtr_S(pchMVFrame, iRow - sRoi->iY, 0);
//
//		for (iCol = sRoi->iX; iCol < sRoi->iX + sRoi->iWidth; iCol++)
//		{
//			//MV_FIXED_S kk = MV_MatrixGet_S(pMatSrc, iRow, iCol);
//			//MV_MatrixSet_S(pchMVFrame, iRow - sRoi[i].iY, iCol - sRoi[i].iX, kk);
//			pDstLineAddr[iCol - sRoi->iX] = pSrcLineAddr[iCol];
//		}
//	}

	return TRUE;
}


void IMGPRO_LPD_Finialize()
{
	memset( &CAMERA_PARAM, 0, sizeof(CameraParams) );

	VLD_DestroyInstance();
}

//获得相机参数
void GetPara( P_CAMERA_TYPE  CameraType, CameraParams* pCamP )
{
	INT nCamIndex;

	nCamIndex = (INT)CameraType;
	MV_S_CamPara coCamPara = g_sCamPara[nCamIndex];

	pCamP->fPitch = MV_FixedFromFloat(coCamPara.sExtrinsicCamPara.fPitch  / 180.0 * ADAS_PI);
	pCamP->fRoll = MV_FixedFromFloat(coCamPara.sExtrinsicCamPara.fRoll / 180.0 * ADAS_PI);
	pCamP->fYaw = MV_FixedFromFloat(coCamPara.sExtrinsicCamPara.fYaw );
	pCamP->iOpticalCenter_X = coCamPara.sIntrinsicCamPara.fOpticalCenterX  ;
	pCamP->iOpticalCenter_Y = coCamPara.sIntrinsicCamPara.fOpticalCenterY ;
	pCamP->iFocalLength_X   = coCamPara.sIntrinsicCamPara.iFocalLengthX ;
	pCamP->iFocalLength_Y   = coCamPara.sIntrinsicCamPara.iFocalLengthY ;
	pCamP->fCameraHeight = MV_FixedFromFloat( coCamPara.sExtrinsicCamPara.fCameraHeight );
	pCamP->iFrameRate = coCamPara.sIntrinsicCamPara.iFrameRate ;
	pCamP->iImageWidth = coCamPara.sIntrinsicCamPara.iImageWidth;
	pCamP->iImageHeight = coCamPara.sIntrinsicCamPara.iImageHeight ;
	pCamP->iROI.iX = coCamPara.sExtrinsicCamPara.iROIX;
	pCamP->iROI.iY = coCamPara.sExtrinsicCamPara.iROIY;
	pCamP->iROI.iHeight = coCamPara.sExtrinsicCamPara.iROIHeight;
	pCamP->iROI.iWidth = coCamPara.sExtrinsicCamPara.iROIWidth;

	//Set down sample params
	pCamP->iDS_XOffset = coCamPara.sCtrlPara.iOffsetX;
	pCamP->iDS_YOffset = coCamPara.sCtrlPara.iOffsetY;
	pCamP->fDS_XZoomRate = MV_FixedFromFloat(coCamPara.sCtrlPara.fZoomRateX);
	pCamP->fDS_YZoomRate = MV_FixedFromFloat(coCamPara.sCtrlPara.fZoomRateY);

	//Set road sign's color params
	pCamP->sRoadSign_Blue.sHSV_H.iMin = coCamPara.sRSGDColorPara.sRoadSign_Blue.sHSV_H.iMin;
	pCamP->sRoadSign_Blue.sHSV_H.iMax = coCamPara.sRSGDColorPara.sRoadSign_Blue.sHSV_H.iMax;
	pCamP->sRoadSign_Blue.sHSV_S.iMin = coCamPara.sRSGDColorPara.sRoadSign_Blue.sHSV_S.iMin;
	pCamP->sRoadSign_Blue.sHSV_S.iMax = coCamPara.sRSGDColorPara.sRoadSign_Blue.sHSV_S.iMax;

	pCamP->sRoadSign_Green.sHSV_H.iMin = coCamPara.sRSGDColorPara.sRoadSign_Green.sHSV_H.iMin;
	pCamP->sRoadSign_Green.sHSV_H.iMax = coCamPara.sRSGDColorPara.sRoadSign_Green.sHSV_H.iMax;
	pCamP->sRoadSign_Green.sHSV_S.iMin = coCamPara.sRSGDColorPara.sRoadSign_Green.sHSV_S.iMin;
	pCamP->sRoadSign_Green.sHSV_S.iMax = coCamPara.sRSGDColorPara.sRoadSign_Green.sHSV_S.iMax;

	pCamP->iMinRoadSignSatu = coCamPara.sRSGDColorPara.iMinRoadSignSatu;

	//Set camera control params
	pCamP->fNightExposure = MV_FixedFromFloat(coCamPara.sControlPara.fNightExposure);

}






