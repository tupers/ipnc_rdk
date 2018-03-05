#ifndef ADAS_VEHICLE_LICENSE_DETECTION_H
#define ADAS_VEHICLE_LICENSE_DETECTION_H

#include "SDS_DataStruct.h"
#include "pcvMathFixed32.h"
#include "SDS_CommStruct.h"
#include "SDS_errorlist.h"
#include "SDS_Dynamic_Params.h"

/** ADAS Header
 */

#ifndef ADAS_UTILITY_H
#   include "AdasUtility.h"
#   define ADAS_UTILITY_H
#endif

#ifdef _C66X_
#include <ti/mathlib/mathlib.h>
#include <ti/dsplib/dsplib.h>
#include <ti/vlib/vlib.h>
#endif

#define DATA_ALIGN (8/sizeof(MV_FIXED_S))

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _C66X_
	typedef struct DSP_Histogram_Params
	{
		unsigned char binedge[256];
		unsigned char histarray[257];
		unsigned short H1[256];
		unsigned short H2[256];
		unsigned short H3[256];
	}DSP_HistParams;
#endif

	typedef struct _VLD_VehicleLicenseDetectionParams
	{
		//***PC_ONLY_START***//
		INT m_iFrameIndex; ///< iFrameIndex is obtained in the ImageProcess.ImageAnalysis()
		//***PC_ONLY_END***//

		BYTE* m_pWorkBuffer;

		LPMV_MATRIX_S m_piPreProcessFrame;

		LPMV_MATRIX_S m_piCompressFrame;

		LPMV_MATRIX_S m_piCompressFrame_R;

		LPMV_MATRIX_S m_piCompressFrame_G;

		LPMV_MATRIX_S m_piCompressFrame_B;

		//LPMV_MATRIX m_psAGData;  //integral image

		//LPMV_MATRIX_S m_psV_ED;//vehicle edge

		//LPMV_MATRIX_S m_psSubV_ED;//sub image vehicle edge

		//LPMV_MATRIX_S m_psSub_Gray;//sub gray image

		LPMV_MATRIX_S m_piTransImage;		//转置后图像

		LPMV_MATRIX_S m_piROIProcessFrame;		//ROI图像

		LPMV_MATRIX_S m_piInterProcessFrame;		//二值化、打标签等中间过渡图像

		BufferParams_DetROI m_sValidBlotList;
		BufferParams_DetROI m_sValidBlotList2;
		BufferParams_DetROI m_sValidBlotList3;
		BufferParams_CharIndex m_sContainerCodeIndex;
		BufferParams_Int m_sLevel_Value;

		BufferParams_CodeSet m_sCodeSet;
		BufferParams_DetROI m_sCodeSetList;
		//CHAR* m_pchCodeName;
		CHAR* m_pchRecogCodeName;
		//INT   m_sLetterTempl_Num;
		//BYTE* m_pLetterTempl;
		//INT   m_sDigitTempl_Num;
		//BYTE* m_pDigitTempl;
		////////////////////////////////////
		LPMV_MATRIX_S m_psLabelMap;

		BufferParams_Int m_sQueueArr;

		BufferParams_DetROI m_sBoundingBoxList;

		BufferParams_Int m_sAreaList;
		///////////////////////////////////

		VLD_VehicleLicenseDetParams m_sVehicleLicenseDetParams;

		VLD_DetRoiChart m_sDetRoiChart;

		VLD_RecognitionResult m_sRecognitionResult;

		//ADAS_GeneralProcessParams* m_sADAS_GP_Params;

		ADAS_ModelRunState m_iRunState;

		SDSSendData m_pResultData;

		AdasDetROI_Float SDSDetROI;

		AdasBlobT *BlobT;

		AdasCircleT *Cir;

		AdasCircleT *SDSBlock;
#ifdef _C66X_
		DSP_HistParams hist;
#endif

		FLOAT f_RDifSideCir;
		FLOAT fCirGood;
		FLOAT fCirGood_R20;
		FLOAT fCirGood_R25;
		FLOAT fCirWarning;
		FLOAT fCirWarning_R20;
		FLOAT fCirWarning_R25;
		INT nBiggestRaduis;
		INT nSmallestRaduis;
		INT nCircleArea;
		INT nSaveImageMaxNum;
		BOOL bImageSaveFlag;
		//CString StrCamSN;
		INT UsedCircleNum;

		INT nAbideNum;

		BOOL IsZeroSpeed;

		BOOL b_SearchTotalImage;

		FLOAT *fSpeed_Cir_x;		//chengjs20140928 modify 3->5

		FLOAT *fSpeed_Cir_y;

		INT *CirFlag;

	} VLD_VehicleLicenseDetectionParams;

	extern VLD_VehicleLicenseDetectionParams g_VLD_Params;

	//extern SDSSendData m_pResultData;

	/** constructor
	 *
	 */
	VOID VLD_CreateInstance();

	/** destructor
	 *
	 */
	VOID VLD_DestroyInstance();

	/**
	 * Initialize Params
	 * i.e. initialize m_sCameraParam, m_sVehicleParam,
	 * @param sCameraP camera params
	 * @param sVehicleP vehicle params
	 * @param psADAS_GP_Params general process params
	 */
	VOID
	VLD_SetInitialParams(CameraParams *psCameraP);

	/**
	 * Evaluate Detection ROI
	 * @param sCameraP camera params
	 * @param sVehicleP vehicle params
	 * @param sZebraCrossingDetParams ZCD detection params
	 */
	VOID
	VLD_EvaluateDetectionROI(CameraParams *psCameraP,
			VLD_VehicleLicenseDetParams* psVehicleLicenseDetParams);

	/**
	 * Initial Detection ROI Chart
	 * @param sCameraP camera params
	 * @param sVehicleP vehicle params
	 * @param sZebraCrossingDetParams ZCD detection params
	 * @param sDetRoiChart the detection ROI chart
	 */
	VOID
	VLD_InitialDetRoiChart(CameraParams *psCameraP,
			VLD_VehicleLicenseDetParams* psVehicleLicenseDetParams,
			VLD_DetRoiChart *psDetRoiChart);
	/**
	 * ZCDetection Process
	 *
	 * @param sCameraP camera params
	 * @param sVehicleP vehicle params
	 */
	VOID
	VLD_DetectionProcess(CameraParams *psCameraP);

	//// Container code detection system---start//
	////2015.9.21 copyright lv//

	/*  Label the bw image */
	VOID VLD_Sub_C_BWLabel_SDS0(LPMV_MATRIX_S psV_ED, LPMV_MATRIX_S psLabelMap,
			LPMV_MATRIX_S pchAGData, BufferParams_DetROI* psValidBlotList,
			BufferParams_Int* psQueueArr, BufferParams_DetROI* psBoundingBox,
			BufferParams_Int* psArea, BYTE * pWorkBuffer);
	VOID VLD_Sub_C_BWLabel_SDS(const LPMV_MATRIX_S psV_ED,
			LPMV_MATRIX_S psLabelMap, LPMV_MATRIX_S pchAGData,
			BufferParams_DetROI* psValidBlotList, BufferParams_Int* psQueueArr,
			BufferParams_DetROI* psBoundingBox, BufferParams_Int* psArea,
			BYTE * pWorkBuffer);

	INT MV_ImageTrans_D(LPMV_MATRIX_S psV_EDS, LPMV_MATRIX_S psV_EDD);	//转置

	INT MV_ImageTrans_D2(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg);	//转置

	INT SDS_DetectCircleROIArea(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,
			LPMV_MATRIX_S RoiImg, LPMV_MATRIX_S psLabelMap,
			AdasDetROI_Float DetectROI, BufferParams_DetROI* psValidBlotList,
			BufferParams_Int* psQueueArr, BufferParams_DetROI* psBoundingBox,
			BufferParams_Int* psArea, INT *CirFlag, BYTE * pWorkBuffer);

	INT GetOtsuThreshold(LPMV_MATRIX_S SrcImg);

	VOID Threshold0(LPMV_MATRIX_S SrcImg, UINT threshold, UINT* coord);
	VOID Threshold(LPMV_MATRIX_S SrcImg, UINT threshold);

	VOID CloneImage(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg);

	INT RecogniseLamp(int label);

	INT GetCirlcleFromBlock(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,
			LPMV_MATRIX_S psLabelMap, AdasDetROI_Float DetectROI,
			BufferParams_DetROI* psValidBlotList, BufferParams_Int* psQueueArr,
			BufferParams_DetROI* psBoundingBox, BufferParams_Int* psArea,
			BYTE * pWorkBuffer, INT CirNum);
	INT GetOneCenterFromBlock(LPMV_MATRIX_S m_pBlockImage,
			LPMV_MATRIX_S DesImg2, LPMV_MATRIX_S psLabelMap,
			BufferParams_DetROI* psValidBlotList, BufferParams_Int* psQueueArr,
			BufferParams_DetROI* psBoundingBox, BufferParams_Int* psArea,
			BYTE * pWorkBuffer, FLOAT *xff, FLOAT *yff, FLOAT *rff, FLOAT *erff,
			FLOAT *cg_x, FLOAT *cg_y, AdasBlobT* OutBlob,int index);

	INT GetThreeCirlcleFromBlock(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,
			LPMV_MATRIX_S psLabelMap, AdasDetROI_Float DetectROI,
			BufferParams_DetROI* psValidBlotList, BufferParams_Int* psQueueArr,
			BufferParams_DetROI* psBoundingBox, BufferParams_Int* psArea,
			BYTE * pWorkBuffer);

	INT MedianSmooth(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,      //中值滤波后的图像
			INT nTemplateType,         //中值滤波模板类型(3表示3阶，5表示5阶，7表示7阶)，模板类型必须是奇数
			BYTE *pWorkBuffer);

	INT GetLableIndex(INT BlobNum);

	INT FilterCir5(INT nCirNum);

	INT FilterCir(INT nCirNum);

	INT JudgeSDSCircle5(LPMV_MATRIX_S SrcImg, INT SDSCirNum);

	INT JudgeSDSCircle(LPMV_MATRIX_S SrcImg, INT SDSCirNum, INT *CirFlag);

	BOOL GetCirclePara(LPMV_MATRIX_S EdgeImage, float *xff, float *yff,
			float *rff, float *valff, AdasBlobT* OutBlob, int cnt);
//		BOOL GetCirclePara(LPMV_MATRIX_S EdgeImage, float *xff, float *yff,
//				float *rff, float *valff);

	VOID ResetDefaultSetting();

	VOID MaopaoSortFrom7();

	INT GetAbideTime(INT ImWidth, INT ImHeight);

	BOOL AddRecog2File(const CHAR* fileName, INT, const CHAR* recogReqName);

	VOID VLD_Restart(CameraParams *pCameraP, VehicleParams *pVehicleP);

	/**
	 * Get run state
	 *
	 * @return run state
	 */
	ADAS_ModelRunState VLD_GetRunState();

	/**
	 * Set run state
	 *
	 * @param iRunState
	 */
	VOID VLD_SetRunState(ADAS_ModelRunState iRunState);

	/**
	 * Set pose state
	 *
	 */
	VOID VLD_Pause();

	VOID SortLabel(int num);
	VOID SwapLabel(AdasCircleT* A, AdasCircleT*B);

/*DSP平台新增，2016.11.14*/
#ifdef _C66X_
VOID DSP_SobelPart2(unsigned char *restrict inoutbuf, const unsigned char *restrict addbuf,
		short cols, short rows);         //补充替换SOBEL滤波第二部分。

VOID DSP_InitHistogram(DSP_HistParams* histparams);

VOID DSP_thr_min2max(const unsigned char *in_data,unsigned char*restrict out_data,
		short cols, short rows,unsigned char threshold);
VOID DSP_boundary_8(const unsigned char*restrict i_data, int rows,
		int cols, unsigned int*restrict o_coord, int* sum);
VOID DSP_boundary_8_xy(const unsigned char*restrict i_data, int rows, int cols,
		unsigned short*restrict coord_x, unsigned short* restrict coord_y);
unsigned short DSP_Max(const unsigned short*x, int nx);
unsigned short DSP_Min(const unsigned short*x, int nx);
#endif
#ifdef __cplusplus
}
#endif

/*
 *
 * $Log$
 */
#endif /* ADAS_VEHICLE_LICENSE_DETECTION_H */
/* EOF */

