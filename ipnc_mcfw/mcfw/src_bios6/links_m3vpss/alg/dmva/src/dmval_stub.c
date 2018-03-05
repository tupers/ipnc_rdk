/** ==================================================================
 *  @file   dmval_stub.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/dmva/src/dmval_stub.c
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
 
#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/src_bios6/links_m3vpss/alg/dmva/inc/DMVAL.h> 

XDAS_Int32 DMVAL_create(DMVALhandle *handle,
						DMVALparams *createParams)
{
	return DMVAL_NO_ERROR;
}

XDAS_Int32 DMVAL_getMemReq(DMVALhandle *handle,
                           DMVALparams *createParams)			   
{
	handle->memTab[0].size = 1024;
	return DMVAL_NO_ERROR;
}		

XDAS_Int32 DMVAL_initModule(DMVALhandle *handle,XDAS_Int32 currentDetectMode)				   
{
	return DMVAL_NO_ERROR;
}

XDAS_Int32 DMVAL_process(DMVALhandle  *handle,
						 DMVALimage   *input,
						 DMVALout     *outBuf)
{
	outBuf->modeResult = DMVAL_DETECTOR_NONE;
	return DMVAL_NO_ERROR;
}

XDAS_Int32 DMVAL_setParameter(DMVALhandle* handle,
							  XDAS_Int32 param,
							  XDAS_Int16* value,
							  XDAS_Int32 dim)						 
{
	return DMVAL_NO_ERROR;
}							

XDAS_Int32 DMVAL_setROI(DMVALhandle *handle, DMVALpolygon *roi)  
{
	return DMVAL_NO_ERROR;
}
