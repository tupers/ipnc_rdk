/** ==================================================================
 *  @file   ti_vcam.c
 *
 *  @path    ipnc_mcfw/mcfw/src_linux/mcfw_api/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "ti_vsys_priv.h"
#include "ti_valg_priv.h"
#include <ti/ipc/SharedRegion.h>

/* =============================================================================
 * Globals
 * ============================================================================= */

VALG_MODULE_CONTEXT_S gValgModuleContext;

/* =============================================================================
 * Valg module APIs
 * ============================================================================= */
static Alg_Result *gResult;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int Init_AlgResult()
{
	gResult = (Alg_Result *)malloc(ALGRESULT_MAX_SIZE);
	if(NULL == gResult)
		return -1;
	memset(gResult,0,ALGRESULT_MAX_SIZE);	
		
	return 0;	
}

void DeInit_AlgResult()
{
	free(gResult);
}

void Update_AlgResult(Alg_Result *pResult)
{
	pthread_mutex_lock(&mutex);
	unsigned int size = pResult->resultSize;
	
	if((size>0) && (size < ALGRESULT_MAX_SIZE))
		memcpy(gResult,pResult,sizeof(Alg_Result) + size);

	pthread_mutex_unlock(&mutex);
}

void Get_AlgResult(Alg_Result *pResult)
{
	pthread_mutex_lock(&mutex);
	
	memcpy(pResult,gResult,sizeof(Alg_Result) + gResult->resultSize);

	pthread_mutex_unlock(&mutex);
}

/* ===================================================================
 *  @func     Valg_params_init
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
Void Valg_params_init(VALG_PARAMS_S * pContext)
{
	gValgModuleContext.pPrm = NULL;
}

/* ===================================================================
 *  @func     Valg_init
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
Int32 Valg_init(VALG_PARAMS_S * pContext)
{
   /* Mark all links related to camera as invalid by default, they will be
     * setup with valid IDs later */
    gValgModuleContext.algId           = SYSTEM_LINK_ID_INVALID;
	gValgModuleContext.pPrm = NULL;

	Init_AlgResult();	

    return 0;
}

/* ===================================================================
 *  @func     Valg_exit
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

Void Valg_exit()
{
    /* Empty for now */
    gValgModuleContext.algId           = SYSTEM_LINK_ID_INVALID;
	gValgModuleContext.pPrm = NULL;
	DeInit_AlgResult();
}

Int32 Valg_setDynamicParamChn(VALG_PARAMS_S * pDynaParam,VALG_PARAMS_E paramId)
{
    Int32 status = ERROR_NONE;		
	Alg_Params *pPrm;
	
    switch(paramId)
    {
    	case VALG_ENABLE:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gValgModuleContext.algId, 				// dest link id
						IMGLINK_CMD_ALG_ENABLE,   							// cmd
						NULL,                  								// prm
						0, 		// prm size
						TRUE);                      		// wait for ack
			}
			else
				status = ERROR_FAIL;
        break;
    	case VALG_DISABLE:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gValgModuleContext.algId, 				// dest link id
						SYS_IMGALG_DISABLE,   							// cmd
						NULL,                  								// prm
						0, 		// prm size
						TRUE);                      		// wait for ack
			}
			else
				status = ERROR_FAIL;			
        break;
    	case VALG_STATIC_IMG:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gValgModuleContext.algId, 				// dest link id
						IMGLINK_CMD_STATIC_IMG,   							// cmd
						NULL,                  								// prm
						0, 		// prm size
						TRUE);                      		// wait for ack
			}
			else
				status = ERROR_FAIL;
        break;
    	case VALG_SENSOR_IMG:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gValgModuleContext.algId, 				// dest link id
							IMGLINK_CMD_SENSOR_IMG,   							// cmd
							NULL,                  								// prm
							0, 		// prm size
							TRUE);                      		// wait for ack			
			}
			else
				status = ERROR_FAIL;			
        break; 
    	case VALG_SET_PARAM:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				AlgPrm_MsgBuf Prm;
				if(gValgModuleContext.pPrm == NULL)
				{
					System_linkControl(gValgModuleContext.algId, 				// dest link id
							IMGLINK_CMD_GET_PARAM_SPACE,   							// cmd
							&Prm,                  								// prm
							sizeof(AlgPrm_MsgBuf), 							// prm size
							TRUE);                      					// wait for ack	
					gValgModuleContext.pPrm = SharedRegion_getPtr(Prm.srPtr);	
					gValgModuleContext.PrmSize = Prm.size;					
				}	
				if(gValgModuleContext.pPrm != NULL)
				{
					Prm.offset = 0;
					Prm.size = pDynaParam->pPrm->prmSize + sizeof(Alg_Params);
					pPrm = (Alg_Params *)(gValgModuleContext.pPrm);
					pPrm->prmSize = pDynaParam->pPrm->prmSize;
					if(pPrm->prmSize > ALGPARAM_MAX_SIZE)
						return status;
					memcpy(pPrm->prm,pDynaParam->pPrm->prm,pPrm->prmSize);
					System_linkControl(gValgModuleContext.algId, 					// dest link id
						IMGLINK_CMD_SET_ALG_PARAM,   							// cmd
						&Prm,                  						// prm
						sizeof(Prm), 								// prm size
						TRUE);
				}
				else
					status = ERROR_FAIL;		                      							// wait for ack
			}
			else
				status = ERROR_FAIL;
        break; 
    	case VALG_GET_PARAM:	
    		// Get valg share memory 
    		if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				AlgPrm_MsgBuf Prm;
				if(gValgModuleContext.pPrm == NULL)
				{
					System_linkControl(gValgModuleContext.algId, 				// dest link id
							IMGLINK_CMD_GET_PARAM_SPACE,   							// cmd
							&Prm,                  								// prm
							sizeof(AlgPrm_MsgBuf), 							// prm size
							TRUE);                      					// wait for ack	
					gValgModuleContext.pPrm = SharedRegion_getPtr(Prm.srPtr);	
					gValgModuleContext.PrmSize = Prm.size;					
				}	

				//get alg param
				if(gValgModuleContext.pPrm != NULL)
				{
					System_linkControl(gValgModuleContext.algId, 					// dest link id
						IMGLINK_CMD_GET_ALG_PARAM,   							// cmd
						&Prm,                  						// prm
						sizeof(AlgPrm_MsgBuf), 								// prm size
						TRUE);
						
						memcpy(pDynaParam->pPrm,gValgModuleContext.pPrm,Prm.size);
				}
				else
					status = ERROR_FAIL;		                      							// wait for ack
			}
			else
				status = ERROR_FAIL;		
        break;
    	case VALG_ENABLE_DBGMODE:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gValgModuleContext.algId, 				// dest link id
						IMGLINK_CMD_DEBUG_MODE,   							// cmd
						NULL,                  								// prm
						0, 							// prm size
						TRUE);  											// wait for ack	                    						
			}
			else
				status = ERROR_FAIL;			
        break; 
     	case VALG_DISABLE_DBGMODE:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gValgModuleContext.algId, 				// dest link id
						IMGLINK_CMD_NORMAL_MODE,   							// cmd
						NULL,                  								// prm
						0, 							// prm size
						TRUE);  						                						
			}
			else
				status = ERROR_FAIL;			
        break;    
     	case VALG_IMAGE_SOURCE:
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				System_linkControl(gValgModuleContext.algId, 				// dest link id
						IMGLINK_CMD_PROCESS_IMG,   							// cmd
						NULL,                  								// prm
						0, 							// prm size
						TRUE);  						                						
			}
			else
				status = ERROR_FAIL;			
        break;  
     	case VALG_UPDATE_DBGIMG:
     	{
			if (gValgModuleContext.algId != SYSTEM_LINK_ID_INVALID)
			{
				AlgPrm_MsgBuf Prm;
				if(gValgModuleContext.pPrm == NULL)
				{
					System_linkControl(gValgModuleContext.algId, 				// dest link id
							IMGLINK_CMD_GET_PARAM_SPACE,   							// cmd
							&Prm,                  								// prm
							sizeof(AlgPrm_MsgBuf), 							// prm size
							TRUE);                      					// wait for ack	
					gValgModuleContext.pPrm = SharedRegion_getPtr(Prm.srPtr);	
					gValgModuleContext.PrmSize = Prm.size;					
				}	
				if(gValgModuleContext.pPrm != NULL)
				{

					Prm.offset = 0;
					Prm.size = pDynaParam->prmSize;
					memcpy(gValgModuleContext.pPrm,pDynaParam->pImg,pDynaParam->prmSize);
					System_linkControl(gValgModuleContext.algId, 					// dest link id
						IMGLINK_CMD_UPDATE_DBGIMG,   							// cmd
						&Prm,                  						// prm
						sizeof(Prm), 								// prm size
						TRUE);
				}
				else
					status = ERROR_FAIL;		                      							// wait for ack
			}
			else
				status = ERROR_FAIL;
       	 	break; 
        }	                                                                   
   	    default:
        	status = ERROR_FAIL;
        	break;
    }
    
    return status;;
}

/* ===================================================================
 *  @func     Valg_delete
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
Int32 Valg_delete()
{
    return 0;
}
