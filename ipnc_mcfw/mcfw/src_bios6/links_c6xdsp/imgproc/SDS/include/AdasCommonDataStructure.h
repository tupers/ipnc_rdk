/*
 * @(#)$Id: AdasCommonDataStructure.h, v 2.0 2015/11/25  LvGangXiao $
 * @(#)Define the common data structures of ADAS. 
 *
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file
 * Define the basic data structures of ADAS. They're used for 
 * all ADAS modules, such as lane detection, vehicle detection and so on. 
 */

#ifndef ADAS_COMMON_DATASTRUCTURE_H
#define ADAS_COMMON_DATASTRUCTURE_H

#include "PFastCV.h"

#ifndef CXX_ADAS_TYPES_H
#define CXX_ADAS_TYPES_H

#ifdef WIN32
#ifdef ADAS_EXPORTS
#define ADAS_API __declspec(dllexport)
#else
#define ADAS_API __declspec(dllimport)
#endif

#else

#define ADAS_API

#endif

#endif /* CXX_ADAS_TYPES_H */


/* Define PI */
#ifndef ADAS_PI
#  define ADAS_PI 3.1415926
#endif

/* Define EXP */
#ifndef ADAS_EXP
#  define ADAS_EXP 2.718281828
#endif


#define MAX_KERNEL_SIZE	25

#define MVZERO  0
#define MVNULL NULL

#define MV_DEVELOPING

#ifdef MV_DEVELOPING
#define MVBUFFERCHECK(Val,  Size) \
    if ((Val < 0) || (Val >= Size )) \
    { \
       ADAS_LOG_ErrorInfo(ADAS_LOG_INFO_TYPE_ERROR, __FILE__, __FUNCTION__, __LINE__, ADAS_ERROR_OVER_RANGE); \
    } 
#else 
#define MVBUFFERCHECK(Val,  Size) 
#endif

/** 
 * Define model run state
 */
typedef enum _ADAS_ModelRunState
{
	ADAS_MODEL_STATE_CREATE = 0,
	ADAS_MODEL_STATE_FINALIZE,
	ADAS_MODEL_STATE_INITIALIZE,
	ADAS_MODEL_STATE_ANALYZE,
	ADAS_MODEL_STATE_PAUSE,
	ADAS_MODEL_STATE_RESTART
}ADAS_ModelRunState;


/** 
 * Define error process Method
 */
typedef enum _ADAS_ErrorProcessMethod
{
	ADAS_ERROR_PROCESS_METHOD_FINISH_DETECTION = 0,
	ADAS_ERROR_PROCESS_METHOD_RECONFIGURATION,
	ADAS_ERROR_PROCESS_METHOD_RESTART_SYSTEM,
	ADAS_ERROR_PROCESS_METHOD_CONTINUE_DETECTION,
	
}ADAS_ErrorProcessMethod;


/** 
 * ADAS model type
 */
typedef enum _AdasModelType
{
	ADAS_MODEL_ZCD = 0,
	ADAS_MODEL_NUM
}AdasModelType;

/** 
 * Image ROI (region of interest)
 */
typedef struct _AdasImageROI
{
	INT iX;			///< Top-left X coordinate
	INT iY;			///< Top-left Y coordinate
	INT iWidth;		///< ROI width
	INT iHeight;	///< ROI height
}AdasImageROI;

/** 
 * Filter Kernel Type Index
 * 
 */
typedef enum _FilterKernelTypeIndex
{
	KERNEL_SIZE_5_5,
	KERNEL_SIZE_7_7,
	KERNEL_SIZE_11_11,
	KERNEL_SIZE_15_15,
	KERNEL_SIZE_21_21,
	KERNEL_SIZE_25_25
}FilterKernelTypeIndex;


/** 
 * The params are used to control the vehicle motion state
 * Refer to LaneDetectionAlgorithmNotes[NOTE.2]
 */
typedef struct _VehicleParams
{
	INT iVelocity;					///< Velocity (Unit: mm / second)
	MV_FIXED fSteeringAngle;		///< Angle between camera optical axis and lane center axis(Unit: radian)
	MV_FIXED fLateralOffset;		///< Lateral offset of optical center relative to lane center axis (Unit: mm)
	MV_FIXED fVehicleWidth;			///< the current vehicle width (Unit: m)
}VehicleParams;


/** 
* Point Structure
 */
typedef struct _AdasPoint
{
	INT  iX;	///< X coordinate
	INT  iY;	///< Y coordinate
}AdasPoint;


/** 
* Point Structure
 */
typedef struct _MV_AdasPoint
{
	MV_FIXED  iX;	///< X coordinate
	MV_FIXED  iY;	///< Y coordinate
}MV_AdasPoint;


/** 
* Value range
 */
typedef struct _AdasRange
{
	INT  iMin;	///< minimal value
	INT  iMax;	///< maximal value
}AdasRange;

/** 
* Adas Color Range
 */
typedef struct _AdasColorRange
{
	AdasRange sHSV_H;		//HSV: Hue
	AdasRange sHSV_S;		//HSV: Saturation
	AdasRange sHSV_V;		//HSV: Value

}AdasColorRange;


	/** 
	 * YUV convert to RGB Param Look Up Table
	 */
	typedef struct _YUV2RGBParamLookUpTable
	{
		//YUV convert to RGB params look up table
		INT* piParam_BGR;		//1.164(Y - 16)
		INT* piParam_B;			//2.018(U - 128)
		INT* piParam_G;			//0.813(V - 128)
		INT* piParam_G2;		//0.391(U - 128)
		INT* piParam_R;			//1.596(V - 128)

	}YUV2RGBParamLookUpTable;


	/** 
	 * 3 * 3 sobel filter
	 */
	typedef struct _SobelFilter
	{
		LPMV_MATRIX_S psHorizontalFilter3;
		LPMV_MATRIX_S psVerticalFilter3;

	}SobelFilter;


/** 
 * Camera Params
 */
typedef struct _CameraParams
{
	//Camera angle
	MV_FIXED fPitch;			///< Pitch angle (Unit: radian)
	MV_FIXED fRoll;			///< Roll angle (Unit: radian)
	MV_FIXED fYaw;				///< Yaw angle (Unit: radian)
	
	//Camera optical center
	INT iOpticalCenter_X;	///< X coordinate of optical center (Unit: pixel)
	INT iOpticalCenter_Y;	///< Y coordinate of optical center (Unit: pixel)
	
	//Camera focal length
	INT iFocalLength_X;		///< X coordinate of focal length (Unit: pixel)
	INT iFocalLength_Y;		///< Y coordinate of focal length (Unit: pixel)
	
	//Camera height
	MV_FIXED fCameraHeight;	///< The height between optical center and ground (Unit: mm)
	
	//Image related params
	INT iFrameRate;			///< Frame rate (Unit: FPS)
	INT iImageWidth;		///< Image width (Unit: pixel)
	INT iImageHeight;		///< Image height (Unit: pixel)
	AdasImageROI iROI;		///< Image ROI
	
	//Down sample related params
	INT iDS_XOffset;		///< the offset in the X direction
	INT iDS_YOffset;		///< the offset in the Y direction
	MV_FIXED fDS_XZoomRate;	///< the zoom rate in the X direction
	MV_FIXED fDS_YZoomRate;	///< the zoom rate in the Y direction

	//Road sign color [RSGD]
	AdasColorRange sRoadSign_Blue;
	AdasColorRange sRoadSign_Green;
	INT iMinRoadSignSatu;

	//Control params
	MV_FIXED fNightExposure;

} CameraParams;

/** 
 * The histogram params
 */
typedef struct _HistParams
{
	LPMV_MATRIX piHistogram;
	MV_FIXED_S iMinValue_S;
	MV_FIXED_S iMaxValue_S;
	MV_FIXED iMinValue;
	MV_FIXED iMaxValue;
	INT iBinWidth;

}HistParams;

/** 
 * Detection ROI
 */
typedef struct _AdasDetROI
{
	INT iStartX;
	INT iEndX;
	INT iStartY;
	INT iEndY;
	BOOL iValid;
}AdasDetROI;

//用于定义初始范围
typedef struct _AdasDetROI_Float
{
	FLOAT iStartX;
	FLOAT iEndX;
	FLOAT iStartY;
	FLOAT iEndY;
	BOOL iValid;
}AdasDetROI_Float;


typedef struct _AdasBlobT
{
	int id;
	int left;
	int right;
	int up;
	int low;
	int area;
	int Total255;
}AdasBlobT;

typedef struct _AdasCircleT
{
	AdasBlobT bt;
	double Cir_x;
	double Cir_y;
	double Cir_r;
	double Cir_f;	
	double Cg_x;
	double Cg_y;
}AdasCircleT;



/** 
 * Buffer params
 */
typedef struct _BufferParams_Float
{
	FLOAT* piBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_Float;

typedef struct _BufferParams_Int
{
	INT* piBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_Int;

typedef struct _BufferParams_Fixed
{
	MV_FIXED* piBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_Fixed;

typedef struct _BufferParams_ImgROI
{
	AdasImageROI* piBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_ImgROI;

typedef struct _BufferParams_DetROI
{
	AdasDetROI* piBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_DetROI;

typedef struct _BufferParams_Point
{
	MV_AdasPoint* psBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_Point;

typedef struct _BufferParams_CharIndex
{
	LPMV_MATRIX_S piBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_CharIndex;

typedef struct _BufferParams_CodeSet
{
	LPMV_MATRIX_S piBuffer;
	INT iDataNum;
	INT iRowNum;
	INT iColNum;

}BufferParams_CodeSet;

typedef struct _BufferParamsTempl_Int
{
	INT* piBuffer;
	INT iId;
}BufferParamsTempl_Int;

extern void ImgAlgCpy(char* dst,char* src,unsigned int width,unsigned int height,unsigned src_pitch,unsigned dst_pitch,unsigned int cacheIValid);
extern int ImgLink_cacheWb(unsigned int addr,unsigned int size);
extern int ImgLink_cacheInv(unsigned int addr,unsigned int size);
extern int Vps_printf(char *format, ...);
/* 
 * 
 *
 * $Log$
 */
#endif /* ADAS_COMMON_DATASTRUCTURE_H */
/* EOF */

