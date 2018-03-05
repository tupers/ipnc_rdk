
/** ==================================================================
 *  @file   imgLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains Img Link API.                 
 * 
 * ===================================================================
 *
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 *
 * ===================================================================*/

/**
    \ingroup LINK_API
    \defgroup IMG_LINK_API Img Link API

	IMG Link imprints the TI logo,Date/Time,Codec details and User data onto Video streams.
	Any change in the data to be imprinted is checked at an interval of Frames Per Second.
	
    @{
*/

/**
    \file imgLink.h
    \brief Img Link API
*/

#ifndef _IMG_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IMG_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief Commands handled by the VA link */
#define IMGLINK_CMD_ALG_ENABLE			(0xA000)
#define IMGLINK_CMD_ALG_DISABLE			(0xA001)
#define IMGLINK_CMD_STATIC_IMG			(0xA002)
#define IMGLINK_CMD_SENSOR_IMG			(0xA003)
#define IMGLINK_CMD_GET_PARAM_SPACE		(0xA004)
#define IMGLINK_CMD_SET_ALG_PARAM		(0xA005)
#define IMGLINK_CMD_GET_ALG_PARAM		(0xA006)
#define IMGLINK_CMD_NORMAL_MODE			(0xA007)
#define IMGLINK_CMD_DEBUG_MODE			(0xA008)
#define IMGLINK_CMD_PROCESS_IMG			(0xA009)
#define IMGLINK_CMD_UPDATE_DBGIMG		(0xA010)

typedef struct
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
}Vsys_DspImgAlgPrm;

typedef struct
{
	unsigned int srPtr;
	unsigned int cmd;
	unsigned int offset;
	unsigned int size;
}AlgPrm_MsgBuf;

/**
    \brief Rvm link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information */

    System_LinkOutQueParams outQueParams;
    /**< Output queue information */

} ImgLink_CreateParams;

/**
 *  @func     ImgLink_init                                               
 *                                                                    
 *  @desc     Function does the Img link register and init                          
 *               - Creates link task
 *               - Registers as a link with the system API                                                             
 *                                                                    
 *  @modif             
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *                                                     
 *  @outputs  
 *                                                                    
 *  @return   FVID2_SOK on success                  
 *
 */                                                                   
Int32 ImgLink_init();

/**
 *  @func     ImgLink_deInit                                               
 *                                                                    
 *  @desc     Function does the Rvm link de-register and de-init                             
 *                                                                    
 *             - Deletes link task
 *             - De-registers as a link with the system API
 *                                                                    
 *  @modif    
 *                                                                    
 *  @inputs   
 *                                                                    
 *  @outputs                                     
 *                                                                    
 *  @return  FVID2_SOK on success                
 *
 */                                                                   
Int32 ImgLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
