/** ==================================================================
 *  @file   mctnfLink_mctnf.h                                                  
 *                                                                     
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/vnf/                                                 
 *                                                                    
 *  @desc   This  File contains.                                       
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _MCTNFLINK_VNF_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MCTNFLINK_VNF_H_

#include <ti/psp/vps/fvid2.h>
#include <imctnf.h>
#include <mctnf_ti.h>
#include "mctnf.h"
#include <ti/psp/iss/iss_capture.h>
#include <ti/psp/iss/alg/dcc/inc/idcc.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

/****Base Error Codes****/
    /* No error */
#define MSP_ERROR_NONE 0x00
    /* Component warning / Notification. Could be recovered by retrying */
#define MSP_ERROR_WARN 0x01
    /* General Component failure */
#define MSP_ERROR_FAIL 0x02





typedef enum{
   MCTNF_YUV_FORMAT_YCBCR422 = 0,
   MCTNF_YUV_FORMAT_YCBCR420 = 1  
}MCTNF_DATA_FORMAT;

/* ========================================================================== 
 */
/**
* @struct MctnfCreateParams
* This structure is used for providing create parameters to the framework of the MSP VNF component.
*
* @param ulInputWidth          : Input width of the input to LDC in pixels
* @param ulInputHeight         : Input height of the input to LDC in pixels
* @param ulComputeWidth        : Compute width used for processing data
* @param ulComputeHeight       : Compute height used for processing data
* @param ulInputStride         : Input stride of the input to LDC in bytes for YUV422 Interleaved or luma in case of YUV420 NV12 input
* @param ulInputStrideChroma  : Input stride of the input to LDC in bytes for chroma in case of YUV420 NV12 input
* @param ulOutputStrideLuma    : Output stride for the luma component in pixels
* @param ulOutputStrideChroma  : Output stride for the chroma component in pixels
* @param eOperateMode          : Operate mode for VNF component
* @param eInputFormat          : Input format
* @param eOutputFormat         : Output format
* @param eInterpolationLuma    : Interpolation method to be used for Y data by LDC module
* @param bLumaEn               : Enable luma filtering within NSF module
* @param bChromaEn             : Enable chroma filtering within NSF module
* @param eSmoothVal            : Smoothness parameter for NSF
* @param bSmoothLumaEn         : Enable smoothing operation of luma component
* @param bSmoothChromaEn       : Enable smoothing operation of chroma component
* 
*/
/* ========================================================================== 
 */
typedef struct {
    MSP_U32 ulInputWidth;
    MSP_U32 ulInputHeight;
    MSP_U32 ulComputeWidth;
    MSP_U32 ulComputeHeight;
    MSP_U32 ulInputStride;
    MSP_U32 ulInputStrideChroma;
    MSP_U32 ulOutputStrideLuma;
    MSP_U32 ulOutputStrideChroma;
    MCTNF_DATA_FORMAT eInputFormat;
    MCTNF_DATA_FORMAT eOutputFormat;
    MSP_U16 unPixelPad;
    MSP_BOOL btnfUseDefault;
    MSP_BOOL bTnfEn;
    MSP_BOOL bSnfEn;
	MSP_BOOL bLdcEn;
	MSP_BOOL tilerEn;
} MctnfCreateParams;


void MCTNF_SetInitParams(MCTNF_Params *params,
                                           MCTNF_DynamicParams *dynamicParams, int width, int height, int pitch);

#define GOTO_EXIT(E) \																									\
		Vps_printf("Error %d :: %s : %s : %d\n", E, __FILE__, __FUNCTION__, __LINE__);						  \
		return E;																				\

#define GOTO_EXIT_IF(C,E)                                                                                   \
        if (C) {                                                                                            \
			Vps_printf("Error %d :: %s : %s : %d\n", E, __FILE__, __FUNCTION__, __LINE__);                        \
            return E;                                                                                  \
        }                                                                                                   \

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     // _MCTNFLINK_VNF_H_
