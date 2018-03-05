/** ==================================================================
 *  @file   ti_vcam.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/                                                 
 *                                                                    
 *  @desc   This  File contains McFW Video Camera (VCAM) API.    
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
    \ingroup MCFW_API
    \defgroup MCFW_VCAM_API McFW Video Camera (VCAM) API

    @{
*/

/**
    \file ti_vcam.h
    \brief McFW Video Camera (VCAM) API
*/

#ifndef __TI_VALG_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_VALG_H__

#include "ti_vsys.h"

/**==========
 * Defines
 * ============================================================================= */

/** Maximum Video Camera channels */
#define VALG_CHN_MAX		(1)
#define VALG_STRM_MAX		(2)


/**==========
 * Enums
 * ============================================================================= */


/**
    \brief Camera Param Set/Get ID
*/
typedef enum {
	VALG_ENABLE,
	
	VALG_DISABLE,

	VALG_STATIC_IMG,

	VALG_SENSOR_IMG,

    VALG_SET_PARAM,
    /**< Set Alg Param */

    VALG_GET_PARAM,
    /**< Get Alg Param */

    VALG_ENABLE_DBGMODE,

    VALG_DISABLE_DBGMODE,
    
    VALG_IMAGE_SOURCE,
    VALG_UPDATE_DBGIMG
} VALG_PARAMS_E;

/**==========
 * Structures
 * ============================================================================= */
/**
    \brief Camera Channel Specific Dynamic Parameters
*/
typedef struct {
	unsigned int prmSize;
    Alg_Params *pPrm;
    Alg_DbgImg *pImg;
} VALG_PARAMS_S;




/**==========
 * APIs
 * ============================================================================= */

/**
 *  @func     Vcam_params_init                                               
 *                                                                    
 *  @desc     Function does the Set defaults for VCAM_PARAMS_S         
 *            By default camera parameters are setup for 16CH D1 camera mode
 *                                                                    
 *  @modif             
 *                                                                    
 *  @inputs   None                                    
 *                                                                    
 *  @outputs  pContext    [OUT] VCAM_PARAMS_S filled with default parameters
 *                                                                    
 *  @return   None
 *
 */                                                                   
Void Valg_params_init(VALG_PARAMS_S * pContext);

/**
 *  @func     Vcam_init                                               
 *                                                                    
 *  @desc     Function does the Initialize camera sub-system                             
 *                                                                    
 *  @modif          
 *                                                                    
 *  @inputs   pContext [IN] params for creation / Camera configuration                                   
 *                                                                    
 *  @outputs  None                                   
 *                                                                    
 *  @return   ERROR_NONE on success                  
 *
 */                                                                   
Int32 Valg_init(VALG_PARAMS_S * pContext);

/**
 *  @func     Vcam_exit                                               
 *                                                                    
 *  @desc     Function does the De-Initialize camera sub-system      
 *            MUST be called after Vcam_stop() and Vsys_delete()
 *                                                                    
 *  @modif           
 *                                                                    
 *  @inputs   None                                    
 *                                                                    
 *  @outputs  None                                    
 *                            s                                        
 *  @return  None                  
 *
 */                                                                   
Void Vcam_exit();

Int32 Valg_setDynamicParamChn(VALG_PARAMS_S * pDynaParam,VALG_PARAMS_E paramId);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
