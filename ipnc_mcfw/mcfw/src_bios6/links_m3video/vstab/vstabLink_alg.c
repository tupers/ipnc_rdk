/** ==================================================================
 *  @file   vstabLink_drv.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/vstab/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "vstabLink_priv.h"

// VS ALG create
/* ===================================================================
 *  @func     VS_ALG_create
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
IALG_Handle VS_ALG_create(IALG_Fxns * fxns, IALG_Handle p, IALG_Params * params)
{
    IALG_MemRec memTab[2];

    IALG_Handle alg;

    IALG_Fxns *fxnsPtr;

    int n;

    n = fxns->algAlloc(params, &fxnsPtr, memTab);

    if (n <= 0)
    {
        return (NULL);
    }

    // Allocate memory requested by VS algorithm
    memTab[0].base = memalign(32, memTab[0].size);
    memTab[1].base = memalign(32, memTab[1].size);

    alg = (IALG_Handle) memTab[0].base;
    alg->fxns = fxns;

    if (fxns->algInit(alg, memTab, p, params) == IALG_EOK)
    {
        return (alg);
    }
    else
    {
        VS_ALG_delete(alg);
        return (NULL);
    }
}

/* Vstab alg create */

/* ===================================================================
 *  @func     VstabLink_algCreate                                               
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
Int32 VstabLink_algCreate(VstabLink_Obj *pObj)
{
    VstabLink_algObj *pAlgObj = &pObj->algObj;
	IVS1_Params vsParams;

    vsParams.vertImgSizeBeforeBSC = pObj->createArgs.totalFrameHeight;
    vsParams.horzImgSizeBeforeBSC = pObj->createArgs.totalFrameWidth;
    vsParams.vertDownsampleRatioInImgSensor = 1;
    vsParams.horzDownSampleRatioInImgSensor = 1;
    vsParams.minPanCoeffHorz = 29846;                      // should be
                                                           // between 0 - 1
                                                           // in Q15 format
                                                           // 0.91 * 2^15 =
                                                           // 29846
    vsParams.maxPanCoeffHorz = (1 * (1 << 15));            // should be
                                                           // between 0 - 1
                                                           // in Q15 format
    vsParams.minPanCoeffVert = 29846;                      // should be
                                                           // between 0 - 1
                                                           // in Q15 format
                                                           // 0.91 * 2^15 =
                                                           // 29846
    vsParams.maxPanCoeffVert = (1 * (1 << 15));            // should be
                                                           // between 0 - 1
                                                           // in Q15 format  
                                                           
    vsParams.panningCaseMotionThreshold = 0;
    vsParams.bounPels = vsParams.horzImgSizeBeforeBSC / 10;
    vsParams.bounLines = vsParams.vertImgSizeBeforeBSC / 10;


	pAlgObj->vsHandle = (IALG_Handle) VS_ALG_create((IALG_Fxns *) & VS_TI_IVS,
                                                    (IALG_Handle) NULL,
                                                    (IALG_Params *) & vsParams);
    UTILS_assert(pAlgObj->vsHandle != NULL);													

#ifdef SYSTEM_DEBUG_VSTAB
    Vps_printf(" %d: VSTAB   : Alg Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Vstab link process */

/* ===================================================================
 *  @func     VstabLink_algProcess                                               
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
Int32 VstabLink_algProcess(VstabLink_Obj * pObj,
                           VstabLink_processPrm *pProcessPrm)
{	
    Int32 retVal;
	VstabLink_algObj *pAlgObj = &pObj->algObj;
    IVS1_InArgs inArgs;
    IVS1_OutArgs outArgs;
    IVS1_Fxns *ivsFxns;	
	
#ifdef SYSTEM_DEBUG_VSTAB_RT					
	UInt32 start,end;
#endif	

    ivsFxns = (IVS1_Fxns *) (pAlgObj->vsHandle)->fxns;
 					   
#ifdef SYSTEM_DEBUG_VSTAB_RT								
	start = Utils_getCurTimeInMsec(); 
#endif				

    // Activate the alg
    (pAlgObj->vsHandle)->fxns->algActivate(pAlgObj->vsHandle);
				
    inArgs.bscBufferCurrV = (XDAS_UInt32) pProcessPrm->bscBufferCurrV;
    inArgs.bscBufferCurrH = (XDAS_UInt32) pProcessPrm->bscBufferCurrH;
    inArgs.bscBufferPrevV = (XDAS_UInt32) pProcessPrm->bscBufferPrevV;
    inArgs.bscBufferPrevH = (XDAS_UInt32) pProcessPrm->bscBufferPrevH;

	inArgs.enableOverwriteWindowLocation = 0;

	retVal = ivsFxns->process((IVS1_Handle) pAlgObj->vsHandle, &inArgs, &outArgs);
	
	// Deactivate the alg
    (pAlgObj->vsHandle)->fxns->algDeactivate(pAlgObj->vsHandle);
	
	if(retVal != IVS1_EOK)
	{
	    pProcessPrm->startX = 0;
		pProcessPrm->startY = 0;
	}
	else
	{
	    pProcessPrm->startX = outArgs.startX;
		pProcessPrm->startY = outArgs.startY;	
	}

#ifdef SYSTEM_DEBUG_VSTAB_RT								
	end = Utils_getCurTimeInMsec();
	Vps_printf(" %d: VSTAB   : Process time = %d msec !!!\n",
			   Clock_getTicks(), (end - start));				
#endif				
				
    return FVID2_SOK;
}

// VS ALG delete
/* ===================================================================
 *  @func     VS_ALG_delete
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
Void VS_ALG_delete(IALG_Handle alg)
{
    IALG_MemRec memTab[2];

    IALG_Fxns *fxns;

    fxns = alg->fxns;

    memTab[0].base = alg;

    fxns->algFree(alg, memTab);

    free(memTab[0].base);
    free(memTab[1].base);
}

/* Delete the alg instance */

/* ===================================================================
 *  @func     VstabLink_algDelete                                               
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
Int32 VstabLink_algDelete(VstabLink_Obj * pObj)
{
	VstabLink_algObj *pAlgObj = &pObj->algObj;

    if (pAlgObj->vsHandle == NULL)
    {
        return FVID2_EFAIL;
    }	
	
    // delete the VS alg
    VS_ALG_delete(pAlgObj->vsHandle);	
	
#ifdef SYSTEM_DEBUG_VSTAB
    Vps_printf(" %d: VSTAB   : Alg Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}
