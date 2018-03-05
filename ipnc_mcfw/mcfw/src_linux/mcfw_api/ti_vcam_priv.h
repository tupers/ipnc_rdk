/** ==================================================================
 *  @file   ti_vcam_priv.h
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
#ifndef __TI_VCAM_PRIV_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_VCAM_PRIV_H__

#include "ti_vcam.h"

#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/cameraLink.h>
#include <mcfw/interfaces/link_api/deiLink.h>
#include <mcfw/interfaces/link_api/nsfLink.h>
#include <mcfw/interfaces/link_api/osdLink.h>

#define MAX_DEI_LINK__VCAM    (2)
#define NUM_CH_PER_DEI__VCAM  (8)
#define MAX_SCLAER_LINK__VCAM (3)

/* =============================================================================
 * Structure
 * ============================================================================= */
typedef struct {
    UInt32 cameraId;
	UInt32 ispId;
    UInt32 osdId;
    UInt32 nsfId;
    UInt32 vnfId;
    UInt32 sclrId[MAX_SCLAER_LINK__VCAM];
    UInt32 deiId[MAX_DEI_LINK__VCAM];
    UInt32 ipcFramesInDspId;
    UInt32 ipcFramesOutVpssId;
    UInt32 frameProcessId;
    UInt32 nullSrcId;
    UInt32 ipcFramesOutVpssToHostId;
    UInt32 ipcFramesInHostId;
    UInt32 numDevices;
    UInt32 numChannels;
    VCAM_PARAMS_S vcamConfig;
    VCAM_CALLBACK_S callbackFxn;
    Ptr callbackArg;
    UInt32 glbceId;
#ifdef USE_MCTNF
	UInt32 mctnfId;
#endif	
} VCAM_MODULE_CONTEXT_S;

extern VCAM_MODULE_CONTEXT_S gVcamModuleContext;

Void Vcam_ipcFramesInCbFxn(Ptr cbCtx);

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
Int32 Vcam_delete();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
