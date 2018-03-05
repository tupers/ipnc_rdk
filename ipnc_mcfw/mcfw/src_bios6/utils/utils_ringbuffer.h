/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _UTILS_RINGBUFFER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _UTILS_RINGBUFFER_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/psp/vps/vps.h>
#include <mcfw/src_bios6/utils/utils_trace.h>

#include <xdc/runtime/IHeap.h>
//#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/ipc/SharedRegion.h>
#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/utils/utils_mem.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/system_tiler.h>
#include <mcfw/interfaces/link_api/vidbitstream.h>
#include <mcfw/src_bios6/links_common/system/system_priv_ipc.h>


#define MINBUFSIZE  2048
#define ALIGNSIZE     2048
#define DATA_ALIGN(val, alignment) ((val + alignment -1)/alignment*alignment)
#define MIN_TERMINATION_SIZE(x,y) ((x)*(y))

typedef struct _BitStreamRingBuffer
{
	void 	*baseAddr;
	UInt32	totalSize;
	UInt32 	wrOfst;
	UInt32 	rdOfst;
	Int32	encodedStreamCount;
	GateMP_Handle ringBufferGateHandle;
} BitStreamRingBuffer ;

typedef BitStreamRingBuffer* RingBufferHandle_t ;

Int32 Utils_ringBuffer_init(RingBufferHandle_t ringBufferHnl, UInt32 size);
Bool Utils_ringBuffer_isFull(RingBufferHandle_t ringBufferHnl);
Bool Utils_ringBuffer_isEmpty(RingBufferHandle_t ringBufferHnl);
Bool Utils_ringBuffer_Size_Avail_for_MIN(RingBufferHandle_t ringBufferHnl, UInt32 minSize);
Bool Utils_ringBuffer_print_info(RingBufferHandle_t ringBufferHnl);
Int32 Utils_ringBuffer_updateRdOfst(RingBufferHandle_t ringBufferHnl, UInt32 length);
Int32 Utils_ringBuffer_updateWrOfst(RingBufferHandle_t ringBufferHnl, UInt32 length);
Int32 Utils_ringBuffer_updateEncodedStreamCount(RingBufferHandle_t ringBufferHnl, Bool increment);
Int32 Utils_ringBuffer_deinit(RingBufferHandle_t ringBufferHnl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  
