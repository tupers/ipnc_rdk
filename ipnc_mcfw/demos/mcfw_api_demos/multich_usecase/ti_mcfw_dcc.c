/** ==================================================================
 *  @file   ti_mcfw_dcc.c
 *
 *  @path    ipnc_mcfw/demos/mcfw_api_demos/multich_usecase/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <osa.h>

#include <mcfw/src_linux/mcfw_api/ti_vsys_priv.h>

#if 0
Vsys_AllocBufInfo dccBufInfo;
Vsys_dccPrm dccParams;

#define MCFW_DCC_PARAM_SIZE 	(7509)

/* ===================================================================
 *  @func     dccParamInit
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
Void dccParamInit(Void)
{
    FILE *dccPtr;

    char dccFilename[64];

    strcpy(dccFilename, "/opt/ipnc/dcc/dcc_default.bin");

    dccPtr = fopen(dccFilename, "rb");

    if (dccPtr == NULL)
    {
        OSA_printf("DCC Default Intialization File Not Present!\n");
    }
    else
    {
		fseek(dccPtr,0,SEEK_END);

		dccParams.dccSize = ftell(dccPtr);

		fseek(dccPtr,0,SEEK_SET);

		Vsys_allocBuf(VSYS_SR1_SHAREMEM, dccParams.dccSize, 32, &dccBufInfo);

        fread(dccBufInfo.virtAddr, sizeof(UInt32), dccParams.dccSize, dccPtr);

        fclose(dccPtr);

		OSA_printf("DCC buffer allocated for size %d\n", dccParams.dccSize);
        OSA_printf("DCC Default File Intialization Done\n");
    }
}

/* ===================================================================
 *  @func     dccParamUpdate
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
Void dccParamUpdate(Void)
{
	VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
    params.dccBufAddr = (Int32) dccBufInfo.physAddr;
    params.dccSize = dccParams.dccSize;
    Vcam_setDynamicParamChn(0, &params, VCAM_DCCBUFADDR);
}

/* ===================================================================
 *  @func     dccParamDelete
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
Void dccParamDelete(Void)
{
    Vsys_freeBuf(VSYS_SR1_SHAREMEM, dccBufInfo.virtAddr,
                 MCFW_DCC_PARAM_SIZE * sizeof(UInt32));
}
#endif
