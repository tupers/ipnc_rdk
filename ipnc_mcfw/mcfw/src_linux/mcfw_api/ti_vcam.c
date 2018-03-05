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
#include "ti_vcam_priv.h"
#include <ti/ipc/SharedRegion.h>

/* =============================================================================
 * Globals
 * ============================================================================= */

VCAM_MODULE_CONTEXT_S gVcamModuleContext;

/* =============================================================================
 * Vcam module APIs
 * ============================================================================= */

/* ===================================================================
 *  @func     Vcam_params_init
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
Void Vcam_params_init(VCAM_PARAMS_S * pContext)
{
    UInt16 devId, chnId, strmId;

    for (devId = 0; devId < VCAM_DEV_MAX; devId++)
    {
        pContext->deviceParams[devId].portEnable = FALSE;
        pContext->deviceParams[devId].portMode = VCAM_MODE_YUV420;
        pContext->deviceParams[devId].dataFormat =
            VCAM_MULTICHN_OPTION_SINGLE_CHN;
        pContext->deviceParams[devId].signalType = VS_AUTO_DETECT;
    }
    for (chnId = 0; chnId < VCAM_CHN_MAX; chnId++)
    {
        for (strmId = 0; strmId < VCAM_STRM_MAX; strmId++)
        {
            pContext->channelParams[chnId].strmEnable[strmId] = FALSE;
            pContext->channelParams[chnId].strmFormat[strmId] = VF_YUV422I_UYVY;
            pContext->channelParams[chnId].strmResolution[strmId].start_X = 0;
            pContext->channelParams[chnId].strmResolution[strmId].start_Y = 0;
            pContext->channelParams[chnId].strmResolution[strmId].width = 0;
            pContext->channelParams[chnId].strmResolution[strmId].height = 0;
        }
        pContext->channelParams[chnId].deviceId = 0;
        pContext->channelParams[chnId].dynamicParams.contrast = 0;
        pContext->channelParams[chnId].dynamicParams.saturation = 0;
        pContext->channelParams[chnId].dynamicParams.brightness = 0;
        pContext->channelParams[chnId].dynamicParams.hue = 0;
    }
}

/* ===================================================================
 *  @func     Vcam_init
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
Int32 Vcam_init(VCAM_PARAMS_S * pContext)
{
    UInt16 devId, chnId, strmId, deiId;

    /* Mark all links related to camera as invalid by default, they will be
     * setup with valid IDs later */
    gVcamModuleContext.cameraId           = SYSTEM_LINK_ID_INVALID;
	gVcamModuleContext.ispId           	  = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.osdId              = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.nsfId              = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.vnfId              = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.nullSrcId          = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.ipcFramesOutVpssId = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.ipcFramesInDspId   = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.glbceId            = SYSTEM_LINK_ID_INVALID;
#ifdef USE_MCTNF
	gVcamModuleContext.mctnfId            = SYSTEM_LINK_ID_INVALID;
#endif	

    for (deiId = 0; deiId < MAX_DEI_LINK; deiId++)
    {
        gVcamModuleContext.deiId[deiId] = SYSTEM_LINK_ID_INVALID;
    }
    gVcamModuleContext.ipcFramesOutVpssToHostId = SYSTEM_LINK_ID_INVALID;
    gVcamModuleContext.ipcFramesInHostId = SYSTEM_LINK_ID_INVALID;

    /* initialize counters */
    gVcamModuleContext.numDevices = 0;
    gVcamModuleContext.numChannels = 0;

    if (pContext == NULL)
    {
        Vcam_params_init(&gVcamModuleContext.vcamConfig);
    }
    else
    {
        for (devId = 0; devId < VCAM_DEV_MAX; devId++)
        {
            if (pContext->deviceParams[devId].portEnable)
            {
                gVcamModuleContext.numDevices++;
            }
        }

        for (chnId = 0; chnId < VCAM_CHN_MAX; chnId++)
        {
            for (strmId = 0; strmId < VCAM_STRM_MAX; strmId++)
            {
                if (pContext->channelParams[chnId].strmEnable[strmId])
                {
                    gVcamModuleContext.numChannels++;
                }
            }
        }

        memcpy(&gVcamModuleContext.vcamConfig, pContext, sizeof(VCAM_PARAMS_S));
    }
    return 0;
}

/* ===================================================================
 *  @func     Vcam_exit
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
Void Vcam_exit()
{
    /* Empty for now */
}

/* ===================================================================
 *  @func     Vcam_start
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
Int32 Vcam_start()
{
    UInt32 deiId;

    /* start can happen in any order, except its recommended to start camera
     * Link the last */
    if (gVcamModuleContext.ipcFramesOutVpssToHostId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStart(gVcamModuleContext.ipcFramesOutVpssToHostId);
    }

    if (gVcamModuleContext.ipcFramesInHostId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStart(gVcamModuleContext.ipcFramesInHostId);
    }

    /* start can happen in any order, except its recommended to start camera
     * Link the last */
    if (gVcamModuleContext.nsfId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStart(gVcamModuleContext.nsfId);
    }

    /* start can happen in any order, except its recommended to start camera
     * Link the last */
    if (gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStart(gVcamModuleContext.vnfId);
    }

    /* start can happen in any order, except its recommended to start camera
     * Link the last */
    if (gVcamModuleContext.osdId != SYSTEM_LINK_ID_INVALID)
    {
        if (gVcamModuleContext.ipcFramesOutVpssId != SYSTEM_LINK_ID_INVALID)
        {
            System_linkStart(gVcamModuleContext.ipcFramesOutVpssId);
        }
        if (gVcamModuleContext.ipcFramesInDspId != SYSTEM_LINK_ID_INVALID)
        {
            System_linkStart(gVcamModuleContext.ipcFramesInDspId);
        }
        System_linkStart(gVcamModuleContext.osdId);
    }

    for (deiId = 0; deiId < MAX_DEI_LINK; deiId++)
    {
        if (gVcamModuleContext.deiId[deiId] != SYSTEM_LINK_ID_INVALID)
        {
            System_linkStart(gVcamModuleContext.deiId[deiId]);
        }
    }
    if (gVcamModuleContext.nullSrcId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStart(gVcamModuleContext.nullSrcId);
    }

    if (gVcamModuleContext.glbceId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStart(gVcamModuleContext.glbceId);
    }
	
    /* Start taking CPU load just before starting of camera link */
    MultiCh_prfLoadCalcEnable(TRUE, FALSE, FALSE);

    if (gVcamModuleContext.cameraId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStart(gVcamModuleContext.cameraId);
    }

#if defined(CAPTURE_DISPLAY_MODE) || defined(ENC_A8_DEC_USE_CASE) || defined(CAPTURE_MCTNF_DISP_MODE) || defined(RVM_DEMO_MODE)
    VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
    params.brightness = 128;
    Vcam_setDynamicParamChn(0, &params, VCAM_BRIGHTNESS);
    params.contrast = 128;
    Vcam_setDynamicParamChn(0, &params, VCAM_CONTRAST);
    params.saturation = 128;
    Vcam_setDynamicParamChn(0, &params, VCAM_SATURATION);
    params.sharpness = 128;
    Vcam_setDynamicParamChn(0, &params, VCAM_SHARPNESS);
#endif

    return 0;
}

/* ===================================================================
 *  @func     Vcam_stop
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
Int32 Vcam_stop()
{
    UInt32 deiId;

    /* stop needs to be in the reseverse order of create */
	if (gVcamModuleContext.ispId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.ispId);
    }
    if (gVcamModuleContext.cameraId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.cameraId);
    }
    if (gVcamModuleContext.glbceId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.glbceId);
    }

    if (gVcamModuleContext.nullSrcId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.nullSrcId);
    }

    if (gVcamModuleContext.nsfId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.nsfId);
    }

    if (gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.vnfId);
    }

    if (gVcamModuleContext.ipcFramesOutVpssId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.ipcFramesOutVpssId);
    }

    if (gVcamModuleContext.ipcFramesInDspId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.ipcFramesInDspId);
    }

    if (gVcamModuleContext.osdId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.osdId);
    }

    for (deiId = 0; deiId < gVsysModuleContext.vsysConfig.numDeis; deiId++)
    {
        if (gVcamModuleContext.deiId[deiId] != SYSTEM_LINK_ID_INVALID)
        {
            System_linkStop(gVcamModuleContext.deiId[deiId]);
        }
    }

    if (gVcamModuleContext.ipcFramesOutVpssToHostId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.ipcFramesOutVpssToHostId);
    }

    if (gVcamModuleContext.ipcFramesInHostId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkStop(gVcamModuleContext.ipcFramesInHostId);
    }

    return 0;
}

/* ===================================================================
 *  @func     Vcam_setParamDevice
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
Int32 Vcam_setParamDevice(VCAM_DEV vcDevId, VCAM_DEV_PARAM_S * psvcDevParam,
                          VCAM_PARAMS_E paramId)
{
    memcpy(&gVcamModuleContext.vcamConfig.deviceParams[vcDevId], psvcDevParam,
           sizeof(VCAM_DEV_PARAM_S));
    return 0;
}

/* ===================================================================
 *  @func     Vcam_getParamDevice
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
Int32 Vcam_getParamDevice(VCAM_DEV vcDevId, VCAM_DEV_PARAM_S * psvcDevParam,
                          VCAM_PARAMS_E paramId)
{
    memcpy(psvcDevParam, &gVcamModuleContext.vcamConfig.deviceParams[vcDevId],
           sizeof(VCAM_DEV_PARAM_S));
    return 0;
}

/* ===================================================================
 *  @func     Vcam_enableDevice
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
Int32 Vcam_enableDevice(VCAM_DEV vcDevId)
{
    return 0;
}

/* ===================================================================
 *  @func     Vcam_disableDevice
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
Int32 Vcam_disableDevice(VCAM_DEV vcDevId)
{
    return 0;
}

/* ===================================================================
 *  @func     Vcam_setParamChn
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
Int32 Vcam_setParamChn(VCAM_CHN vcChnId, VCAM_CHN_PARAM_S * psCamChnParam,
                       VCAM_PARAMS_E paramId)
{
    memcpy(&gVcamModuleContext.vcamConfig.channelParams[vcChnId], psCamChnParam,
           sizeof(VCAM_CHN_PARAM_S));
    return 0;
}

/* ===================================================================
 *  @func     Vcam_getParamChn
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
Int32 Vcam_getParamChn(VCAM_CHN vcChnId, VCAM_CHN_PARAM_S * psCamChnParam,
                       VCAM_PARAMS_E paramId)
{
    memcpy(psCamChnParam, &gVcamModuleContext.vcamConfig.channelParams[vcChnId],
           sizeof(VCAM_CHN_PARAM_S));
    return 0;
}

/* ===================================================================
 *  @func     Vcam_setDynamicParamChn
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
Int32 Vcam_setDynamicParamChn(VCAM_CHN vcChnId,
                              VCAM_CHN_DYNAMIC_PARAM_S * psCamChnDynaParam,
                              VCAM_PARAMS_E paramId)
{
    Int32 status = ERROR_NONE;
	
#ifdef USE_MCTNF
	MctnfLink_ChDynamicParams mcTnfDynPrm;
#endif	

    if (gVcamModuleContext.cameraId == SYSTEM_LINK_ID_INVALID)
        return ERROR_FAIL;

    switch (paramId)
    {
        case VCAM_FORMAT:
            break;

        case VCAM_GLBCE:
#ifdef WDR_ON
			psCamChnDynaParam->enableGlbce = psCamChnDynaParam->enableGlbce <= 3 ? 0 : psCamChnDynaParam->enableGlbce - 3;
			if (gVcamModuleContext.glbceId != SYSTEM_LINK_ID_INVALID)
			 status = System_linkControl(gVcamModuleContext.glbceId,
										 WDR_LINK_CMD_CHANGE_MODE,
										 &(psCamChnDynaParam->enableGlbce),
										 sizeof(psCamChnDynaParam->enableGlbce),
										 TRUE);
#else
            if (gVcamModuleContext.glbceId != SYSTEM_LINK_ID_INVALID)
             status = System_linkControl(gVcamModuleContext.glbceId,
                                         GLBCE_LINK_CMD_SET_DYNAMIC_PARAMS,
                                         &(psCamChnDynaParam->enableGlbce),
                                         sizeof(psCamChnDynaParam->enableGlbce),
                                         TRUE);
#endif
            break;

        case VCAM_RESOLUTION:
        {
            CameraLink_SetDynParams params;
            params.chId = vcChnId;
            params.InputWidth = psCamChnDynaParam->InputWidth;
            params.InputHeight = psCamChnDynaParam->InputHeight;
            status = System_linkControl(gVcamModuleContext.cameraId,
                            CAMERA_LINK_CMD_CHANGE_RESOLUTION,
                            &(params), sizeof(params), TRUE);
            break;
        }
        case VCAM_DCCBUFADDR:
        {
            CameraLink_DccParams params;
            params.dccBufAddr = psCamChnDynaParam->dccBufAddr;
            params.dccSize = psCamChnDynaParam->dccSize;
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_DCC_BUFFER,
                                        &(params), sizeof(params), TRUE);
            break;
        }
        case VCAM_AEWB_VENDOR:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_AEWB_VENDOR,
                                        &(psCamChnDynaParam->aewbVendor),
                                        sizeof(psCamChnDynaParam->aewbVendor),
                                        TRUE);
            break;

        case VCAM_MIRROR_MODE:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_MIRROR_MODE,
                                        &(psCamChnDynaParam->mirrorMode),
                                        sizeof(psCamChnDynaParam->mirrorMode),
                                        TRUE);
            break;
        case VCAM_AEWB_MODE:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_AEWB_MODE,
                                        &(psCamChnDynaParam->aewbMode),
                                        sizeof(psCamChnDynaParam->aewbMode),
                                        TRUE);
            break;
        case VCAM_AEWB_PRIORITY:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_AEWB_PRIORITY,
                                        &(psCamChnDynaParam->aewbPriority),
                                        sizeof(psCamChnDynaParam->aewbPriority),
                                        TRUE);
            break;
        case VCAM_IRIS:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_IRIS,
                                        &(psCamChnDynaParam->IRIS),
                                        sizeof(psCamChnDynaParam->IRIS), TRUE);
            break;
        case VCAM_FRAMECTRL:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_FRAMECTRL,
                                        &(psCamChnDynaParam->frameCtrl),
                                        sizeof(psCamChnDynaParam->frameCtrl),
                                        TRUE);
            break;
        case VCAM_CONTRAST:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_CONTRAST,
                                        &(psCamChnDynaParam->contrast),
                                        sizeof(psCamChnDynaParam->contrast),
                                        TRUE);
            break;
        case VCAM_SATURATION:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_SATURATION,
                                        &(psCamChnDynaParam->saturation),
                                        sizeof(psCamChnDynaParam->saturation),
                                        TRUE);
            break;
        case VCAM_BRIGHTNESS:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_BRIGHTNESS,
                                        &(psCamChnDynaParam->brightness),
                                        sizeof(psCamChnDynaParam->brightness),
                                        TRUE);
            break;
        case VCAM_SHARPNESS:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_SHARPNESS,
                                        &(psCamChnDynaParam->sharpness),
                                        sizeof(psCamChnDynaParam->sharpness),
                                        TRUE);
            break;
       case  VCAM_INCREASE_EXPOSURE:      
       		 status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_INCREASE_EXPOSURE,
                                        &(psCamChnDynaParam->exposure),
                                        sizeof(psCamChnDynaParam->exposure),
                                        TRUE);
            break;    
       case  VCAM_DECREASE_EXPOSURE:
       		 status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_DECREASE_EXPOSURE,
                                        &(psCamChnDynaParam->exposure),
                                        sizeof(psCamChnDynaParam->exposure),
                                        TRUE);
            break;              
        case VCAM_SET_EXPOSURE:
       		 status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_SET_EXPOSURE,
                                        &(psCamChnDynaParam->exposure),
                                        sizeof(psCamChnDynaParam->exposure),
                                        TRUE);
            break;   
        case VCAM_GET_EXPOSURE:
       		 status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_EXPOSURE,
                                        &(psCamChnDynaParam->exposure),
                                        sizeof(psCamChnDynaParam->exposure),
                                        TRUE);                          
            break;   
        case VCAM_GET_MAX_EXPOSURE:
       		 status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_MAX_EXPOSURE,
                                        &(psCamChnDynaParam->maxexposure),
                                        sizeof(psCamChnDynaParam->maxexposure),
                                        TRUE);                            
            break;
        case VCAM_GET_MIN_EXPOSURE:
       		 status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_MIN_EXPOSURE,
                                        &(psCamChnDynaParam->minexposure),
                                        sizeof(psCamChnDynaParam->minexposure),
                                        TRUE);                             
            break;  
        case VCAM_SET_GAIN:
        {
        	CameraLink_GainPrm gainPrm;
        	gainPrm.sensorGain = psCamChnDynaParam->sensorGain;
        	gainPrm.pipeGain = psCamChnDynaParam->pipeGain;
        	
       		status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_SET_GAIN,
                                        &gainPrm,
                                        sizeof(CameraLink_GainPrm),
                                        TRUE);
            break;   
		}
        case VCAM_GET_GAIN:
        {
        	CameraLink_GainPrm gainPrm;
       		status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_GAIN,
                                        &gainPrm,
                                        sizeof(CameraLink_GainPrm),
                                        TRUE);
            psCamChnDynaParam->sensorGain = gainPrm.sensorGain;
        	psCamChnDynaParam->pipeGain = gainPrm.pipeGain;                            
            break;
         }            
        case VCAM_GET_MAX_GAIN:
        {
        	CameraLink_GainPrm gainPrm;
       		status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_MAX_GAIN,
                                        &gainPrm,
                                        sizeof(CameraLink_GainPrm),
                                        TRUE);
            psCamChnDynaParam->maxsensorGain = gainPrm.sensorGain;
        	psCamChnDynaParam->maxpipeGain = gainPrm.pipeGain;                         
            break;
         }                      
        case VCAM_GET_MIN_GAIN:
        {
         	CameraLink_GainPrm gainPrm;
       		status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_MIN_GAIN,
                                        &gainPrm,
                                        sizeof(CameraLink_GainPrm),
                                        TRUE);
            psCamChnDynaParam->minsensorGain = gainPrm.sensorGain;
        	psCamChnDynaParam->minpipeGain = gainPrm.pipeGain;                         
            break;
        }                                      
       case VCAM_BLC:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_BLC,
                                        &(psCamChnDynaParam->blc),
                                        sizeof(psCamChnDynaParam->blc), TRUE);
            break;                         	    
        case VCAM_AWBMODE:
        {
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_AWBMODE,
                                        &(psCamChnDynaParam->awbMode),
                                        sizeof(psCamChnDynaParam->awbMode),
                                        TRUE);
            break;
        }        
        case VCAM_GET_2A_PARAM:
        {
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_2A_PARAM,
                                        psCamChnDynaParam->H3aParams.pData,
                                        psCamChnDynaParam->H3aParams.size,
                                        TRUE);                             
                                             	
			break;
        }   
        case VCAM_SET_2A_PARAM:
        {
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_SET_2A_PARAM,
                                        psCamChnDynaParam->H3aParams.pData,
                                        psCamChnDynaParam->H3aParams.size,
                                        TRUE);        	
			break;
        }  
        case VCAM_GET_2A_WEIGHT:
        {      
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_GET_2A_WEIGHT,
                                        psCamChnDynaParam->H3aParams.pData,
                                        psCamChnDynaParam->H3aParams.size,
                                        TRUE);                                                                          	
			break;
        }   
        case VCAM_SET_2A_WEIGHT:
        {
        	EzCamH3AParam H3aParams;
        	
        	memcpy(&H3aParams,&psCamChnDynaParam->H3aParams,sizeof(EzCamH3AParam));
        	
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_SET_2A_WEIGHT,
                                        psCamChnDynaParam->H3aParams.pData,
                                        psCamChnDynaParam->H3aParams.size,
                                        TRUE);        	
			break;
        }               
        case VCAM_FRAMERATE:
        {
            CameraLink_FpsParams params;

            params.StreamId = vcChnId;
            params.targetFps = psCamChnDynaParam->frameRate;
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_FRAMERATE,
                                        &(params), sizeof(params), TRUE);
            break;
        }
        case VCAM_AEMODE:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_AEMODE,
                                        &(psCamChnDynaParam->AEMode),
                                        sizeof(psCamChnDynaParam->AEMode),
                                        TRUE);
            break;
        case VCAM_ENV_50_60HZ:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_ENV_50_60HZ,
                                        &(psCamChnDynaParam->env50_60hz),
                                        sizeof(psCamChnDynaParam->env50_60hz),
                                        TRUE);
            break;
        case VCAM_BINNING_SKIP:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_BINNING,
                                        &(psCamChnDynaParam->binningMode),
                                        sizeof(psCamChnDynaParam->binningMode),
                                        TRUE);
            break;
        case VCAM_HUE:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_HUE,
                                        &(psCamChnDynaParam->hue),
                                        sizeof(psCamChnDynaParam->hue), TRUE);
            break;
        case VCAM_LDC:
            if (gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
                status = System_linkControl(gVcamModuleContext.vnfId,
                                            VNF_LINK_CMD_ENABLE_LDC,
                                            &(psCamChnDynaParam->enableLDC),
                                            sizeof(psCamChnDynaParam->
                                                   enableLDC), TRUE);
            break;
        case VCAM_TNF:
            if (gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
                status = System_linkControl(gVcamModuleContext.vnfId,
                                            VNF_LINK_CMD_ENABLE_TNF,
                                            &(psCamChnDynaParam->enableTNF),
                                            sizeof(psCamChnDynaParam->
                                                   enableTNF), TRUE);
            if (gVcamModuleContext.nsfId != SYSTEM_LINK_ID_INVALID)
                status = System_linkControl(gVcamModuleContext.nsfId,
                                            NSF_LINK_CMD_ENABLE_TNF,
                                            &(psCamChnDynaParam->enableTNF),
                                            sizeof(psCamChnDynaParam->
                                                   enableTNF), TRUE);
#ifdef USE_MCTNF			
			if(gVcamModuleContext.mctnfId != SYSTEM_LINK_ID_INVALID)
			{
				/* Get the McTNF dynamic params */	
				mcTnfDynPrm.chId = vcChnId;	
                status = System_linkControl(gVcamModuleContext.mctnfId,
                                            MCTNF_LINK_CMD_GET_DYNAMIC_PARAMS,
                                            &mcTnfDynPrm,
                                            sizeof(MctnfLink_ChDynamicParams), 
											TRUE);				

				if(mcTnfDynPrm.noiseFilterParams.minWtForCurQ8 != 255)
					gUI_mcfw_config.prevMinWtForCurQ8 = mcTnfDynPrm.noiseFilterParams.minWtForCurQ8;
											
				if((gUI_mcfw_config.snfEnable == 0) && (gUI_mcfw_config.tnfEnable == 0))
				{
					/* Disable McTNF by setting minWtForCurQ8 parameter to 255 */
					mcTnfDynPrm.noiseFilterParams.minWtForCurQ8 = 255;	
				}
				else
				{
					mcTnfDynPrm.noiseFilterParams.minWtForCurQ8 = 
						gUI_mcfw_config.prevMinWtForCurQ8;
				}
				
				/* Set the McTNF Dynamic params */
                status = System_linkControl(gVcamModuleContext.mctnfId,
                                            MCTNF_LINK_CMD_SET_DYNAMIC_PARAMS,
                                            &mcTnfDynPrm,
                                            sizeof(MctnfLink_ChDynamicParams), 
											TRUE);								
			}
#endif												   
            break;
        case VCAM_SNF:
            if (gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
                status = System_linkControl(gVcamModuleContext.vnfId,
                                            VNF_LINK_CMD_ENABLE_SNF,
                                            &(psCamChnDynaParam->enableSNF),
                                            sizeof(psCamChnDynaParam->
                                                   enableSNF), TRUE);
            if (gVcamModuleContext.nsfId != SYSTEM_LINK_ID_INVALID)
                status = System_linkControl(gVcamModuleContext.nsfId,
                                            NSF_LINK_CMD_ENABLE_SNF,
                                            &(psCamChnDynaParam->enableSNF),
                                            sizeof(psCamChnDynaParam->
                                                   enableTNF), TRUE);
#ifdef USE_MCTNF			
			if(gVcamModuleContext.mctnfId != SYSTEM_LINK_ID_INVALID)
			{
				/* Get the McTNF dynamic params */	
				mcTnfDynPrm.chId = vcChnId;		
                status = System_linkControl(gVcamModuleContext.mctnfId,
                                            MCTNF_LINK_CMD_GET_DYNAMIC_PARAMS,
                                            &mcTnfDynPrm,
                                            sizeof(MctnfLink_ChDynamicParams), 
											TRUE);				

				if(mcTnfDynPrm.noiseFilterParams.minWtForCurQ8 != 255)
					gUI_mcfw_config.prevMinWtForCurQ8 = mcTnfDynPrm.noiseFilterParams.minWtForCurQ8;
											
				if((gUI_mcfw_config.snfEnable == 0) && (gUI_mcfw_config.tnfEnable == 0))
				{
					/* Disable McTNF by setting minWtForCurQ8 parameter to 255 */
					mcTnfDynPrm.noiseFilterParams.minWtForCurQ8 = 255;	
				}
				else
				{
					mcTnfDynPrm.noiseFilterParams.minWtForCurQ8 = 
						gUI_mcfw_config.prevMinWtForCurQ8;
				}
				
				/* Set the McTNF Dynamic params */
                status = System_linkControl(gVcamModuleContext.mctnfId,
                                            MCTNF_LINK_CMD_SET_DYNAMIC_PARAMS,
                                            &mcTnfDynPrm,
                                            sizeof(MctnfLink_ChDynamicParams), 
											TRUE);								
			}
#endif												   
            break;

        case VCAM_NF_STRENGTH:
            if (gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
                status = System_linkControl(gVcamModuleContext.vnfId,
                                            VNF_LINK_CMD_VNF_STRENGTH,
                                            &(psCamChnDynaParam->nfStrength),
                                            sizeof(psCamChnDynaParam->
                                                   nfStrength), TRUE);
            if (gVcamModuleContext.nsfId != SYSTEM_LINK_ID_INVALID)
                status = System_linkControl(gVcamModuleContext.nsfId,
                                            NSF_LINK_CMD_NSF_STRENGTH,
                                            &(psCamChnDynaParam->nfStrength),
                                            sizeof(psCamChnDynaParam->
                                                   nfStrength), TRUE);
            break;

        case VCAM_PORTMODE:
            break;
        case VCAM_SIGNALTYPE:
            break;
        case VCAM_SD_FORMAT:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_SET_FORMAT,
                                        &(psCamChnDynaParam->sdFormat),
                                        sizeof(psCamChnDynaParam->
                                               sdFormat), TRUE);
            break;
        case VCAM_ALL:
            status = System_linkControl(gVcamModuleContext.cameraId,
                                        CAMERA_LINK_CMD_CHANGE_CONTRAST,
                                        &(psCamChnDynaParam->contrast),
                                        sizeof(psCamChnDynaParam->contrast),
                                        TRUE);
            if (status >= 0)
            {
                status = System_linkControl(gVcamModuleContext.cameraId,
                                            CAMERA_LINK_CMD_CHANGE_SATURATION,
                                            &(psCamChnDynaParam->saturation),
                                            sizeof(psCamChnDynaParam->
                                                   saturation), TRUE);
            }
            if (status >= 0)
            {
                status = System_linkControl(gVcamModuleContext.cameraId,
                                            CAMERA_LINK_CMD_CHANGE_BRIGHTNESS,
                                            &(psCamChnDynaParam->brightness),
                                            sizeof(psCamChnDynaParam->
                                                   brightness), TRUE);
            }
            if (status >= 0)
            {
                status = System_linkControl(gVcamModuleContext.cameraId,
                                            CAMERA_LINK_CMD_CHANGE_HUE,
                                            &(psCamChnDynaParam->hue),
                                            sizeof(psCamChnDynaParam->hue),
                                            TRUE);
            }
            break;

    }

    return status;
}

/**
 * \brief:
 *      Set camera frame rate. This is an additional control in camera side itself; is really useful for secondary stream <CIF>.
 *      Stream 0 is D1 & Stream 1 is CIF.
 * \input:
 *      vcChnId             -- camera Channel id
 *      vStrmId             -- Stream Id
 *      frameRate          -- Frame Rate
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS    --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Vcam_setFrameRate
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
Int32 Vcam_setFrameRate(VCAM_CHN vcChnId, VCAM_STRM vStrmId, Int32 frameRate)
{
    DeiLink_ChFpsParams params;

    Int32 status = ERROR_NONE;

    params.chId = vcChnId;
    params.streamId = vStrmId;

    params.inputFrameRate = (1000 * frameRate);
    status =
        System_linkControl(gVcamModuleContext.deiId[0],
                           DEI_LINK_CMD_SET_FRAME_RATE, &params, sizeof(params),
                           TRUE);
#if !defined(TI_814X_BUILD) && !defined(TI_8107_BUILD)
    status =
        System_linkControl(gVcamModuleContext.deiId[1],
                           DEI_LINK_CMD_SET_FRAME_RATE, &params, sizeof(params),
                           TRUE);
#endif
    return status;
}

/**
 * \brief:
 *      Get camera frame rate. Not available now
 * \input:
 *      vcChnId             -- camera Channel id
 *      vStrmId             -- Stream Id
 *      frameRate          -- Frame Rate
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS    --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Vcam_getFrameRate
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
Int32 Vcam_getFrameRate(VCAM_CHN vcChnId, VCAM_STRM vStrmId)
{
    return 0;
}

/**
 * \brief:
 *      Get camera Channel dynamic parameters
 * \input:
 *      vcChnId             -- camera Channel id
 *      psCamChnDynaParam   -- Device dynamic parameter structure
 * \output:
 *      NA
 * \return
*       TI_MEDIA_SUCCESS    --  while success
*       ERROR_CODE          --  refer for err defination
*/
/* ===================================================================
 *  @func     Vcam_getDynamicParamChn
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
Int32 Vcam_getDynamicParamChn(VCAM_CHN vcChnId,
                              VCAM_CHN_DYNAMIC_PARAM_S * psCamChnParam,
                              VCAM_PARAMS_E paramId)
{
    return 0;
}

/* ===================================================================
 *  @func     Vcam_enableDisableChn
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
Int32 Vcam_enableDisableChn(VCAM_CHN vcChnId, VCAM_STRM vcStrmId,
                            Bool enableChn)
{
    char *onOffName[] = { "ON ", "OFF" };
    Int32 status = ERROR_FAIL;

    DeiLink_ChannelInfo channelInfo;

    UInt32 cmdId, deiId;

    channelInfo.channelId = vcChnId % NUM_CH_PER_DEI;
    channelInfo.streamId = vcStrmId;
    channelInfo.enable = enableChn;

    deiId = vcChnId / NUM_CH_PER_DEI;

    if (enableChn)
        cmdId = DEI_LINK_CMD_ENABLE_CHANNEL;
    else
        cmdId = DEI_LINK_CMD_DISABLE_CHANNEL;

    OSA_printf(" VCAM: CH%d STRM%d = [%s]\n", vcChnId, vcChnId,
           onOffName[enableChn]);

    if (deiId < MAX_DEI_LINK)
    {
        if (gVcamModuleContext.deiId[deiId] != SYSTEM_LINK_ID_INVALID)
        {
            status = System_linkControl(gVcamModuleContext.deiId[deiId],
                                        cmdId,
                                        &(channelInfo),
                                        sizeof(DeiLink_ChannelInfo), TRUE);
        }
    }

    return status;

}

/* ===================================================================
 *  @func     Vcam_enableChn
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
Int32 Vcam_enableChn(VCAM_CHN vcChnId, VCAM_STRM vcStrmId)
{
    return Vcam_enableDisableChn(vcChnId, vcStrmId, TRUE);
}

/* ===================================================================
 *  @func     Vcam_disableChn
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
Int32 Vcam_disableChn(VCAM_CHN vcChnId, VCAM_STRM vcStrmId)
{
    return Vcam_enableDisableChn(vcChnId, vcStrmId, FALSE);
}

/* ===================================================================
 *  @func     Vcam_getChnBufferBlocking
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
Int32 Vcam_getChnBufferBlocking(VCAM_CHN vcChnId, UInt8 * pChnBuffer, UInt32 uiTimeoutMs)   // consider
                                                                                            // later
{
    return 0;
}

/* ===================================================================
 *  @func     Vcam_getChnBufferNonBlocking
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
Int32 Vcam_getChnBufferNonBlocking(VCAM_CHN vcChnId, UInt8 * pChnBuffer)    // consider
                                                                            // later
{
    return 0;
}

/* ===================================================================
 *  @func     Vcam_releaseChnBuffer
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
Int32 Vcam_releaseChnBuffer(VCAM_CHN vcChnId, UInt8 * pChnBuffer)   // consider
                                                                    // later
{
    return 0;
}

/* ===================================================================
 *  @func     Vcam_delete
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
Int32 Vcam_delete()
{
    UInt32 i;

    /* delete can be done in any order */

    if (gVcamModuleContext.cameraId != SYSTEM_LINK_ID_INVALID)
        System_linkDelete(gVcamModuleContext.cameraId);
    if (gVcamModuleContext.ipcFramesOutVpssId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gVcamModuleContext.ipcFramesOutVpssId);
    }

    if (gVcamModuleContext.ipcFramesInDspId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gVcamModuleContext.ipcFramesInDspId);
    }

    if (gVcamModuleContext.osdId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gVcamModuleContext.osdId);
    }

    for (i = 0; i < MAX_DEI_LINK; i++)
    {
        if (gVcamModuleContext.deiId[i] != SYSTEM_LINK_ID_INVALID)
            System_linkDelete(gVcamModuleContext.deiId[i]);
    }

    if (gVcamModuleContext.nsfId != SYSTEM_LINK_ID_INVALID)
        System_linkDelete(gVcamModuleContext.nsfId);
    if (gVcamModuleContext.vnfId != SYSTEM_LINK_ID_INVALID)
        System_linkDelete(gVcamModuleContext.vnfId);

    if (gVcamModuleContext.ipcFramesOutVpssToHostId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gVcamModuleContext.ipcFramesOutVpssToHostId);
    }

    if (gVcamModuleContext.ipcFramesInHostId != SYSTEM_LINK_ID_INVALID)
    {
        System_linkDelete(gVcamModuleContext.ipcFramesInHostId);
    }

    return 0;
}

static Void Vcam_copyVidFrameInfoLink2McFw(VIDEO_FRAMEBUF_S *dstBuf,
                                           VIDFrame_Buf    *srcBuf)
{
    Int i,j;
    OSA_assert(VIDEO_MAX_FIELDS == VIDFRAME_MAX_FIELDS);
    OSA_assert(VIDEO_MAX_PLANES == VIDFRAME_MAX_PLANES);

    for (i = 0; i < VIDEO_MAX_FIELDS; i++)
    {
        for (j = 0; j < VIDEO_MAX_PLANES; j++)
        {
            dstBuf->addr[i][j] = srcBuf->addr[i][j];
            dstBuf->phyAddr[i][j] = srcBuf->phyAddr[i][j];
        }
    }
    dstBuf->channelNum  = srcBuf->channelNum;
    dstBuf->fid         = srcBuf->fid;
    dstBuf->frameWidth  = srcBuf->frameWidth;
    dstBuf->frameHeight = srcBuf->frameHeight;
    dstBuf->linkPrivate = srcBuf->linkPrivate;
    dstBuf->timeStamp   = srcBuf->timeStamp;
    dstBuf->framePitch[0] = srcBuf->framePitch[0];
    dstBuf->framePitch[1] = srcBuf->framePitch[1];
}

static Void Vcam_copyVidFrameInfoMcFw2Link(VIDFrame_Buf *dstBuf,
                                           VIDEO_FRAMEBUF_S    *srcBuf)
{
    Int i,j;
    OSA_assert(VIDEO_MAX_FIELDS == VIDFRAME_MAX_FIELDS);
    OSA_assert(VIDEO_MAX_PLANES == VIDFRAME_MAX_PLANES);

    for (i = 0; i < VIDEO_MAX_FIELDS; i++)
    {
        for (j = 0; j < VIDEO_MAX_PLANES; j++)
        {
            dstBuf->addr[i][j] = srcBuf->addr[i][j];
            dstBuf->phyAddr[i][j] = srcBuf->phyAddr[i][j];
        }
    }
    dstBuf->channelNum  = srcBuf->channelNum;
    dstBuf->fid         = srcBuf->fid;
    dstBuf->frameWidth  = srcBuf->frameWidth;
    dstBuf->frameHeight = srcBuf->frameHeight;
    dstBuf->linkPrivate = srcBuf->linkPrivate;
    dstBuf->timeStamp   = srcBuf->timeStamp;
    dstBuf->framePitch[0] = srcBuf->framePitch[0];
    dstBuf->framePitch[1] = srcBuf->framePitch[1];
}

/**
    \brief Request filled video buffers from framework

    User calls this API to get full video frames from the framework.
    After getting the video frames, user will
    - consume the video frames
    - and then call Vcam_putEmptyVideoFrames() to free the video frames back to the framework

    \param pFrameBufList    [OUT]  List of video frames returned by the framework
    \param timeout          [IN]   TIMEOUT_WAIT_FOREVER or TIMEOUT_NO_WAIT or timeout in msecs

    \return ERROR_NONE on success
*/
Int32 Vcam_getFullVideoFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList, UInt32 timeout)
{
    VIDFrame_BufList  vidBufList;
    VIDFrame_Buf     *pInBuf;
    VIDEO_FRAMEBUF_S *pOutBuf;
    UInt32 i;

    pFrameBufList->numFrames = 0;
    vidBufList.numFrames = 0;

    IpcFramesInLink_getFullVideoFrames(gVcamModuleContext.ipcFramesInHostId,
                                       &vidBufList);

    pFrameBufList->numFrames = vidBufList.numFrames;
    for (i = 0; i < vidBufList.numFrames; i++)
    {
        pOutBuf = &pFrameBufList->frames[i];
        pInBuf = &vidBufList.frames[i];

        Vcam_copyVidFrameInfoLink2McFw(pOutBuf,pInBuf);
    }

    return 0;
}
/**
    \brief Give consumed video frames back to the application to be freed

    Buffers that are were previously got from Vcam_getFullVideoFrames can be
    freed back to the framework by invoking this API.

    \param pFrameBufList [IN]   List of video frames

    \return ERROR_NONE on success
*/
Int32 Vcam_putEmptyVideoFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList)
{
    VIDEO_FRAMEBUF_S *pSrcBuf;
    VIDFrame_Buf     *pDstBuf;
    VIDFrame_BufList  vidBufList;
    UInt32 i;
    Int status = 0;

    vidBufList.numFrames = pFrameBufList->numFrames;
    for (i = 0; i < vidBufList.numFrames; i++)
    {
        pSrcBuf = &pFrameBufList->frames[i];
        pDstBuf = &vidBufList.frames[i];
        Vcam_copyVidFrameInfoMcFw2Link(pDstBuf,pSrcBuf);
    }
    if (vidBufList.numFrames)
    {
        status =
        IpcFramesInLink_putEmptyVideoFrames(gVcamModuleContext.ipcFramesInHostId,
                                            &vidBufList);
    }
    return 0;
}

Int32 Vcam_SaveFrame(char *filename,unsigned size)
{
	Int32 status;

	if(size > 20)
		return -1;
		
    if (gVcamModuleContext.frameProcessId != SYSTEM_LINK_ID_INVALID)
    {
		status = System_linkControl(gVcamModuleContext.frameProcessId,
                                        SAVE_FRAME_IMG,
                                        filename,
                                        size, TRUE);
    	return 0;		                            
    }
	
	return -1;
}

Int32 Vcam_EnableEncodeVideo()
{
	Int32 status;

    if (gVcamModuleContext.frameProcessId != SYSTEM_LINK_ID_INVALID)
    {
		status = System_linkControl(gVcamModuleContext.frameProcessId,
                                        ENABLE_ENCODE_IMG,
                                        NULL,
                                        0, TRUE);   
    	return 0;		                            
    }
	
	return -1;
}

Int32 Vcam_DisableEncodeVideo()
{
	Int32 status;

    if (gVcamModuleContext.frameProcessId != SYSTEM_LINK_ID_INVALID)
    {
		status = System_linkControl(gVcamModuleContext.frameProcessId,
                                        DISABLE_ENCODE_IMG,
                                        NULL,
                                        0, TRUE);
    	return 0;		                            
    }
	
	return -1;
}

Int32 Vcam_EnableSaveErrImg()
{
	Int32 status;

    if (gVcamModuleContext.frameProcessId != SYSTEM_LINK_ID_INVALID)
    {
		status = System_linkControl(gVcamModuleContext.frameProcessId,
                                        ENABLE_SAVE_ERRIMG,
                                        NULL,
                                        0, TRUE);   
    	return 0;		                            
    }
	
	return -1;
}

Int32 Vcam_DisableSaveErrImg()
{
	Int32 status;

    if (gVcamModuleContext.frameProcessId != SYSTEM_LINK_ID_INVALID)
    {
		status = System_linkControl(gVcamModuleContext.frameProcessId,
                                        DISABLE_SAVE_ERRIMG,
                                        NULL,
                                        0, TRUE);
    	return 0;		                            
    }
	
	return -1;
}
/**
 * \brief:
 *              register call back which will post the message
 * \input:
 *              callback                -- callback function
 * \output:
 *              NA
 * \return
*               TI_MEDIA_SUCCESS        --      while success
*               ERROR_CODE                      --      refer for err defination
*/
Int32 Vcam_registerCallback(VCAM_CALLBACK_S * callback, Ptr arg)
{
    gVcamModuleContext.callbackFxn = *callback;
    gVcamModuleContext.callbackArg = arg;

    return 0;
}

/**
 * \brief:
 *              MCFW layer call back which will invoke the app registered callback
 * \input:
 *              callback                -- callback function
 * \output:
 *              NA
 * \return
*               TI_MEDIA_SUCCESS        --      while success
*               ERROR_CODE                      --      refer for err defination
*/
Void Vcam_ipcFramesInCbFxn(Ptr cbCtx)
{
    OSA_assert(cbCtx == &gVcamModuleContext);

	OSA_printf("Vcam Get Frame\n");
	
    if (gVcamModuleContext.callbackFxn.newDataAvailableCb)
    {
        gVcamModuleContext.callbackFxn.newDataAvailableCb(gVcamModuleContext.callbackArg);
    }
}


/**
    \brief Change the capture mode/resolution

    \param vcChnId             [IN] channel Id, 0 always

    \param chPrms              [IN] Channel Parameters containing
                                    Resolution information

    \return ERROR_NONE on success
*/
Int32 Vcam_changeCaptMode(VCAM_CHN vcChnId, VCAM_CHN_PARAM_S *chPrms)
{
    UInt32 cnt;
    CameraLink_UpdateResParams resPrms;

    OSA_assert(NULL != chPrms);
    OSA_assert(0 == vcChnId);

    resPrms.sensorOutWidth = chPrms->inputWidth;
    resPrms.sensorOutHeight = chPrms->inputHeight;

    resPrms.numOutput = 0;
    /* Note that startX/Y are ignored here */
    for (cnt = 0u; cnt < VCAM_STRM_MAX; cnt ++)
    {
        if (TRUE == chPrms->strmEnable[cnt])
        {
            resPrms.outParams[resPrms.numOutput].scOutWidth = chPrms->strmResolution[cnt].width;
            resPrms.outParams[resPrms.numOutput].scOutHeight = chPrms->strmResolution[cnt].height;
            resPrms.numOutput ++;
        }
    }

    System_linkControl(gVcamModuleContext.cameraId,
                       CAMERA_LINK_CMD_CHANGE_MODE,
                       &resPrms,
                       sizeof(resPrms),
                       TRUE);

    return (0);
}

/**
    \brief Enable/Disable channels for Scaling

    \param linkId [IN]   Link Id of the Scaler Link
	
	\param chId [IN]   Channel Id
	
	\param enable [IN]   Enable/Dsiable flag

    \return ERROR_NONE on success
*/
Int32 Vcam_enableDisableScalerCh(UInt32 linkId,UInt32 chId,Bool enable)
{
	Int32 status = ERROR_NONE;
	SclrLink_ChannelInfo channelInfo;
	UInt32 cmdId;

	if(linkId != SYSTEM_LINK_ID_INVALID)
	{
		channelInfo.channelId = chId;
		channelInfo.enable    = enable;
		
		if(enable)
			cmdId = SCLR_LINK_CMD_ENABLE_CHANNEL;
		else
			cmdId = SCLR_LINK_CMD_DISABLE_CHANNEL;
					
		status = System_linkControl(linkId,
								    cmdId,
									&channelInfo,
									sizeof(SclrLink_ChannelInfo),
									TRUE);
	}
	
	return status;
}

/**
    \brief Enable/Disable channels for MP Scaling

    \param linkId [IN]   Link Id of the MP Scaler Link
	
	\param chId [IN]   Channel Id
	
	\param enable [IN]   Enable/Dsiable flag

    \return ERROR_NONE on success
*/
Int32 Vcam_enableDisableMpScalerCh(UInt32 linkId,UInt32 chId,Bool enable)
{
	Int32 status = ERROR_NONE;
	MpSclrLink_ChannelInfo channelInfo;
	UInt32 cmdId;

	if(linkId != SYSTEM_LINK_ID_INVALID)
	{
		channelInfo.channelId = chId;
		channelInfo.enable    = enable;
		
		if(enable)
			cmdId = MP_SCLR_LINK_CMD_ENABLE_CHANNEL;
		else
			cmdId = MP_SCLR_LINK_CMD_DISABLE_CHANNEL;
					
		status = System_linkControl(linkId,
								    cmdId,
									&channelInfo,
									sizeof(MpSclrLink_ChannelInfo),
									TRUE);
	}
	
	return status;
}

/**
    \brief Enable/Disable channels for Deinterlacing

    \param linkId [IN]   Link Id of the DEI Link
	
	\param chId [IN]   Channel Id
	
	\param streamId [IN]   Stream Id
	
	\param enable [IN]   Enable/Dsiable flag

    \return ERROR_NONE on success
*/
Int32 Vcam_enableDisableDeiCh(UInt32 linkId,UInt32 chId,UInt32 streamId,Bool enable)
{
	Int32 status = ERROR_NONE;
	DeiLink_ChannelInfo channelInfo;
	UInt32 cmdId;

	if(linkId != SYSTEM_LINK_ID_INVALID)
	{
		channelInfo.channelId = chId;
		channelInfo.streamId  = streamId;
		channelInfo.enable    = enable;
		
		if(enable)
			cmdId = DEI_LINK_CMD_ENABLE_CHANNEL;
		else
			cmdId = DEI_LINK_CMD_DISABLE_CHANNEL;
					
		status = System_linkControl(linkId,
								    cmdId,
									&channelInfo,
									sizeof(DeiLink_ChannelInfo),
									TRUE);
	}
	
	return status;
}

/**
    \brief Set Scalar Output Resolution

    \param linkId [IN]   Link Id of the Scaler Link
	
	\param chId [IN]   Channel Id
	
	\param width [IN]   Output Width
	
	\param height [IN]   Output Height

	\param pitch0 [IN]   Output Pitch0

	\param pitch1 [IN]   Output Pitch1	

    \return ERROR_NONE on success
*/
Int32 Vcam_setScalarOutResolution(UInt32 linkId,UInt32 chId,UInt32 width,
							      UInt32 height,UInt32 pitch0,UInt32 pitch1)
{
	Int32 status = ERROR_NONE;
	SclrLink_chDynamicSetOutRes sclrOutRes;
	
	if(linkId != SYSTEM_LINK_ID_INVALID)
	{
		sclrOutRes.chId     = chId;
		sclrOutRes.width    = width;
		sclrOutRes.height   = height;
		sclrOutRes.pitch[0] = pitch0;
		sclrOutRes.pitch[1] = pitch1;

		status = System_linkControl(linkId,
								    SCLR_LINK_CMD_SET_OUTPUTRESOLUTION,
									&sclrOutRes,
									sizeof(SclrLink_chDynamicSetOutRes),
									TRUE);	
	}
	
	return status;
}
