/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SYSTEM_PRIV_M3VIDEO_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_PRIV_M3VIDEO_H_

#include <mcfw/src_bios6/links_common/system/system_priv_common.h>

#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/nullLink.h>
#include <mcfw/interfaces/link_api/dupLink.h>
#include <mcfw/interfaces/link_api/mergeLink.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/interfaces/link_api/selectLink.h>
#include <mcfw/interfaces/link_api/systemLink_c6xdsp.h>

/**
 * \brief System Task Descriptor
 *
 */
typedef struct {
    Void *rsv;

} System_DspObj;

typedef enum System_DspLinkState {
    SYSTEM_LINK_STATE_START = 0,
    SYSTEM_LINK_STATE_STOP
} System_DspLinkState;

extern System_DspObj gSystem_objDsp;

typedef struct
{
    UInt32    dstAddr;  /* MUST be 4-byte aligned */
    UInt32    srcAddr;   /* MUST be 4-byte aligned */

	UInt32 acnt;
	UInt32 bcnt;
	UInt32 ccnt;

	UInt32 srcBidx;
	UInt32 dstBidx;
	UInt32 srcCidx;
	UInt32 dstCidx;	
}System_DmaCopy3D;
void DM81XX_EDMA3_setParams_3D(int chId,int dmaQueue,System_DmaCopy3D *pInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
