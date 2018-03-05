/*
 * SDS_Export.h
 *
 *  Created on: 2016-5-5
 *      Author: Fisher
 */

#ifndef SDS_EXPORT_H_
#define SDS_EXPORT_H_

#include "pcvPlatformdef.h"
#include "SDS_CommStruct.h"

typedef enum _P_CAMERA_TYPE_
{
	CAMERA_DEFAULE = 0,
	CAMERA_USER1,
	CAMERA_USER2,
	CAMERA_TOTAL   = 10
}P_CAMERA_TYPE;

typedef enum _P_SDS_ERRORCODE_
{
	ErrCode_OK          = 0,
	ErrCode_InvalidSize = 8000,
	ErrCode_AlgoriFault = 8001

}P_SDS_ERRORCODE;

//---------------------------------
//P_IMGPRO process functions
//---------------------------------

void IMGPRO_SDS_INITIALIZE( P_CAMERA_TYPE  CameraType  );

P_SDS_ERRORCODE IMGPRO_SDS_Analysis(MV_FIXED_S* src,INT PixRows, INT PixCols, INT iFrameIndex, SDSSendData* pSdsOutput);

void IMGPRO_LPD_Finialize();


#endif /* SDS_EXPORT_H_ */
