#ifndef __TI_VSYS_PRIV_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_VSYS_PRIV_H__

#include <mcfw/interfaces/link_api/system_const.h>

#include "ti_vsys.h"
#include "ti_vcam_priv.h"
#include "ti_mjpeg_priv.h"


#include <osa.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/captureLink.h>
#include <mcfw/interfaces/link_api/cameraLink.h>
#include <mcfw/interfaces/link_api/mjpegLink.h>
#include <mcfw/interfaces/link_api/rotateFrameLink.h>
#include <mcfw/interfaces/link_api/deiLink.h>
#include <mcfw/interfaces/link_api/nsfLink.h>
#include <mcfw/interfaces/link_api/displayLink.h>
#include <mcfw/interfaces/link_api/nullLink.h>
#include <mcfw/interfaces/link_api/grpxLink.h>
#include <mcfw/interfaces/link_api/dupLink.h>
#include <mcfw/interfaces/link_api/selectLink.h>
#include <mcfw/interfaces/link_api/swMsLink.h>
#include <mcfw/interfaces/link_api/mergeLink.h>
#include <mcfw/interfaces/link_api/nullSrcLink.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/interfaces/link_api/systemLink_m3vpss.h>
#include <mcfw/interfaces/link_api/systemLink_m3video.h>
#include <mcfw/interfaces/link_api/encLink.h>
#include <mcfw/interfaces/link_api/decLink.h>
#include <mcfw/interfaces/link_api/fdLink.h>
#include <mcfw/interfaces/link_api/swosdLink.h>
#include <mcfw/interfaces/link_api/vnfLink.h>
#include <mcfw/interfaces/link_api/mctnfLink.h>
#include <mcfw/interfaces/link_api/glbceLink.h>
#include <mcfw/interfaces/link_api/ispLink.h>
#include <mcfw/interfaces/link_api/vaLink.h>
#include <mcfw/interfaces/link_api/vstabLink.h>
#include <mcfw/interfaces/link_api/glbceSupportLink.h>
#include <mcfw/interfaces/link_api/wdrLink.h>
#include <mcfw/interfaces/link_api/muxLink.h>

#include <mcfw/interfaces/link_api/sclrLink.h>
#include <mcfw/interfaces/link_api/avsync.h>
#include <mcfw/interfaces/link_api/mpSclrLink.h>
#include <mcfw/interfaces/link_api/rvmLink.h>
#include <mcfw/interfaces/link_api/imgLink.h>
#include <mcfw/interfaces/link_api/frameProcessHost.h>
#include <ti/syslink/utils/IHeap.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/ipc/SharedRegion.h>
#include <string.h>

#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ivideo.h>
#include <ih264enc.h>
#include <ih264vdec.h>

#include <osa_thr.h>

#define VSYS_TIMER_TSK_PRI			(1)
#define VSYS_TIMER_TSK_STACK_SIZE	(0)

/* =============================================================================
 * Structure
 * =============================================================================
 */
typedef struct
{
    VSYS_PARAMS_S vsysConfig;

    Bool enableFastUsecaseSwitch;
    Bool initDone;
    UInt32 swOsdId;
    UInt32 fdId;
    UInt32 vaId;
    UInt32 muxId;
    UInt32 muxIdVnf;
    UInt32 dspImgId;

} VSYS_MODULE_CONTEXT_S;

typedef struct
{
    OSA_ThrHndl hndl;
    UInt32 startCnt;
    volatile UInt32 exitTimerTh;
    volatile UInt32 exitTimerDone;

} VSYS_TIMER_OBJ;


#define MULTICH_INIT_STRUCT(structName,structObj)  structName##_Init(&structObj)

#include "ti_vdis_priv.h"
#include "ti_venc_priv.h"
#include "ti_vdec_priv.h"
#include "ti_vcap_priv.h"


extern VSYS_MODULE_CONTEXT_S gVsysModuleContext;

Int32 MultiCh_prfLoadCalcEnable(Bool enable, Bool printStatus, Bool printTskLoad);
Int32 MultiCh_prfLoadPrint(Bool printTskLoad,Bool resetTaskLoad);

Void MultiCh_createVcapVdis();
Void MultiCh_deleteVcapVdis();

void MultiCh_createProgressiveVcapVencVdecVdis();
void MultiCh_deleteProgressiveVcapVencVdecVdis();

void MultiCh_createProgressive4D1VcapVencVdecVdis();
void MultiCh_deleteProgressive4D1VcapVencVdecVdis();

void MultiCh_createProgressive8D1VcapVencVdecVdis();
void MultiCh_deleteProgressive8D1VcapVencVdecVdis();

void MultiCh_createProgressive16ChVcapVencVdecVdis();
void MultiCh_deleteProgressive16ChVcapVencVdecVdis();
Int32 MultiCh_progressive16ChVcapVencVdecVdisSwitchLayout(VDIS_DEV vdDevId, SwMsLink_LayoutPrm *pSwMsLayout);
void MultiCh_progressive16ChVcapVencVdecVdisSwmsChReMap(VDIS_MOSAIC_S *psVdMosaicParam);
Int32 MultiCh_progressive16ChVcapVencVdecVdisSetCapFrameRate(VCAP_CHN vcChnId, VCAP_STRM vStrmId, Int32 inputFrameRate, Int32 outputFrameRate);
Int32 MultiCh_progressive16ChVcapVencVdecVdis_enableDisableCapChn(VCAP_CHN vcChnId, VCAP_STRM vcStrmId, Bool enableChn);
Int32 MultiCh_progressive16ChVcapVencVdecVdis_setCapDynamicParamChn(VCAP_CHN vcChnId, VCAP_CHN_DYNAMIC_PARAM_S *psCapChnDynaParam, VCAP_PARAMS_E paramId);
Int32 MultiCh_progressive16ChVcapVencVdecVdis_getCapDynamicParamChn(VCAP_CHN vcChnId, VCAP_CHN_DYNAMIC_PARAM_S *psCapChnDynaParam, VCAP_PARAMS_E paramId);

void MultiCh_createProgressive16ChNrtVcapVencVdecVdis();
void MultiCh_deleteProgressive16ChNrtVcapVencVdecVdis();
Int32 MultiCh_progressive16ChNrtVcapVencVdecVdisSetCapFrameRate(VCAP_CHN vcChnId, VCAP_STRM vStrmId, Int32 inputFrameRate, Int32 outputFrameRate);
Int32 MultiCh_progressive16ChNrtVcapVencVdecVdisEnableDisableCapChn(VCAP_CHN vcChnId, VCAP_STRM vStrmId, Bool enableChn);
Int32 MultiCh_progressive16ChNrtVcapVencVdecVdisSetOutRes(VCAP_CHN vcChnId, VCAP_CHN_DYNAMIC_PARAM_S *psCapChnDynaParam);
Int32 MultiCh_progressive16ChNrtVcapVencVdecVdisGetOutRes(VCAP_CHN vcChnId, VCAP_CHN_DYNAMIC_PARAM_S *psCapChnDynaParam);

Void MultiCh_createInterlacedVcapVencVdecVdis();
Void MultiCh_deleteInterlacedVcapVencVdecVdis();

Void MultiCh_createVdecVdis();
Void MultiCh_deleteVdecVdis();

Void MultiCh_createVcapVencVdis();
Void MultiCh_deleteVcapVencVdis();

Void MultiChHd_createVcapVencVdis();
Void MultiChHd_deleteVcapVencVdis();

Void MultiCh_create_8CifProgressiveVcapVencVdecVdis();
Void MultiCh_delete_8CifProgressiveVcapVencVdecVdis();

Void MultiCh_createCustomVcapVencVdecVdis();
Void MultiCh_deleteCustomVcapVencVdecVdis();

Void MultiCh_ipcBitsInitCreateParams_BitsInHLOS(IpcBitsInLinkHLOS_CreateParams *cp);
Void MultiCh_ipcBitsInitCreateParams_BitsInHLOSVcap(IpcBitsInLinkHLOS_CreateParams *cp);
Void MultiCh_ipcBitsInitCreateParams_BitsInRTOS(IpcBitsInLinkRTOS_CreateParams *cp,
                                                Bool notifyNextLink);
Void MultiCh_ipcBitsInitCreateParams_BitsOutHLOS(IpcBitsOutLinkHLOS_CreateParams *cp,
                                                 System_LinkQueInfo *inQueInfo);
Void MultiCh_ipcBitsInitCreateParams_BitsOutRTOS(IpcBitsOutLinkRTOS_CreateParams *cp,
                                                Bool notifyPrevLink);

Void MultiCh_createTriStreamLowPwr();
Void MultiCh_deleteTriStreamLowPwr();
Void MultiCh_createStreamCaptureDisplay();
Void MultiCh_deleteStreamCaptureDisplay();

Void MultiCh_createStreamDualStreamDemo();
Void MultiCh_deleteStreamDualStreamDemo();

Void MultiCh_createTriStreamFullFtr();
Void MultiCh_deleteTriStreamFullFtr();
Void MultiCh_createQuadStreamNf();
Void MultiCh_deleteQuadStreamNf();

Void MultiCh_createTriStreamDsp();
Void MultiCh_deleteTriStreamDsp();
Int32 MultiCh_getSensorId(Int32 sensorId);

Void MultiCh_createEncA8Dec();
Void MultiCh_deleteEncA8Dec();

Void MultiCh_createStreamCaptMctnfDisplay();
Void MultiCh_deleteStreamCaptMctnfDisplay();

Void MultiCh_createRvm();
Void MultiCh_deleteRvm();

Void MultiCh_createMachVision();
Void MultiCh_deleteMachVision();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


