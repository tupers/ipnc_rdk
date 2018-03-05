/** ==================================================================
 *  @file   vnfLink_vnf.h                                                  
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
#ifndef _VNFLINK_VNF_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _VNFLINK_VNF_H_

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/iss/core/msp_types.h>
#include <ti/psp/iss/alg/vnf/msp_vnf.h>

typedef enum {
	MSP_VNF_STRENGTH_AUTO = 0,
	MSP_VNF_STRENGTH_LOW,
	MSP_VNF_STRENGTH_MIDDLE,
	MSP_VNF_STRENGTH_HIGH,
} MSP_VNF_STRENGTH;

/* ========================================================================== 
 */
/**
* @struct VnfCreateParams
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
	MSP_U32 ulStartX;
	MSP_U32 ulStartY;
    MSP_U32 ulComputeWidth;
    MSP_U32 ulComputeHeight;
    MSP_U32 ulInputStride;
    MSP_U32 ulInputStrideChroma;
    MSP_U32 ulOutputStrideLuma;
    MSP_U32 ulOutputStrideChroma;
    MSP_VNF_OPERATION_MODE eOperateMode;
    MSP_VNF_DATA_FORMAT eInputFormat;
    MSP_VNF_DATA_FORMAT eOutputFormat;
    MSP_VNF_Y_INTERPOLATE_METHOD eInterpolationLuma;
    MSP_U16 unPixelPad;
    MSP_VNF_NSF_PARAMS nsfParams;
    MSP_VNF_TNF_PARAMS tnfParams;
    MSP_BOOL btnfUseDefault;
    MSP_BOOL bTnfEn;
    MSP_BOOL bSnfEn;
	MSP_BOOL bLdcEn;
	MSP_VNF_STRENGTH strength;
	MSP_BOOL tilerEn;
} VnfCreateParams;

/* ===================================================================
 *  @func     MSP_VNF_ProcessFrame                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 MSP_VNF_ProcessFrame(MSP_COMPONENT_TYPE * jComponent,
                           FVID2_Frame * prevImageBuffer,
                           FVID2_Frame * currentImageBuffer,
                           FVID2_Frame * outputImageBuffer, unsigned int startX, unsigned int startY, unsigned int lumaPitch, unsigned int chromaPitch);

/* ===================================================================
 *  @func     MSP_VNF_Engine_Init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 MSP_VNF_Engine_Init(MSP_COMPONENT_TYPE ** jComponent);

/* ===================================================================
 *  @func     MSP_VNF_Engine_DeInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 MSP_VNF_Engine_DeInit(MSP_COMPONENT_TYPE * jComponent);

/* ===================================================================
 *  @func     MSP_VNF_Engine_Open                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 MSP_VNF_Engine_Open(MSP_COMPONENT_TYPE * jComponent,
                          VnfCreateParams * params);

/* ===================================================================
 *  @func     MSP_VNF_Engine_Close                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 MSP_VNF_Engine_Close(MSP_COMPONENT_TYPE * jComponent);

Int32 MSP_VNF_Engine_Reset(MSP_COMPONENT_TYPE * jComponent);

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
#endif                                                     // _VNFLINK_VNF_H_
