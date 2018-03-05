#ifndef __TI_VENC_PRIV_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_VENC_PRIV_H__


#include "ti_venc.h"
#include "ti_vsys_priv.h"



#define VENC_FRAMERATE_LINK_MULTIPLICATION_FACTOR                         (1000)

/* =============================================================================
 * Structure
 * =============================================================================
 */

typedef struct
{
	UInt32 encId;
	UInt32 ipcBitsInHLOSId;
    UInt32 ipcBitsOutRTOSId;
	UInt32 ipcM3InId;
    UInt32 ipcM3OutId;

	VENC_CALLBACK_S callbackFxn;
	Ptr callbackArg;
	VENC_PARAMS_S vencConfig;

	UInt32 encFormat[VENC_CHN_MAX];
}VENC_MODULE_CONTEXT_S;

extern VENC_MODULE_CONTEXT_S gVencModuleContext;

Int32 Venc_create(System_LinkInQueParams *vencInQue);
Int32 Venc_delete();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


