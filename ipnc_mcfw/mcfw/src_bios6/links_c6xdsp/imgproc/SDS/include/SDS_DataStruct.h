/*
 * SDS_DataStruct.h
 *
 *  Created on: 2016-5-5
 *      Author: Fisher
 */

#ifndef SDS_DATASTRUCT_H_
#define SDS_DATASTRUCT_H_

#include "AdasCommonDataStructure.h"


/**
 * Name the bit control flag of bLMControlFlag
 */
typedef enum _VLD_ControlFlagName
{
	VLDCF_SYSTEM_INIT_STATE_FLAG,			///< 1: memory has been initialized
	VLDCF_RESET_INIT_STATE_FLAG				///< 1: the initial params has been reseted; 0: the initial params has not been reseted

}VLD_ControlFlagName;

/**
 * The the detection roi chart
 */
typedef struct _VLD_DetRoiChart
{
	AdasImageROI sCompressGridSize;
	INT iDetRoiChartXStep;
	INT iDetRoiChartYStep;

}VLD_DetRoiChart;


/**
 * ZebraCrossingDetection detection params
 */
typedef struct _VLD_VehicleLicenseDetParams
{
	DWORD dVLDControlFlag;			///< Global control flag control, see LD_LMControlFlagName

	AdasImageROI sVehicleLicenseDetROI;		///< Zebra CrossingDetection detection ROI

}VLD_VehicleLicenseDetParams;

/**
 * The collection of recognition result
 */
typedef struct _VLD_RecognitionResult
{
	INT iTotalAnalyzeTime;
	AdasImageROI sVLDrawingArea;
	BOOL iDetectFlag;

}VLD_RecognitionResult;




#endif /* SDS_DATASTRUCT_H_ */
