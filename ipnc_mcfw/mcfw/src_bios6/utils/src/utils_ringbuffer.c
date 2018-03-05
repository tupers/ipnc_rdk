/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/ipc/SharedRegion.h>
#include <mcfw/src_bios6/utils/utils_mem.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/system_tiler.h>
#include <mcfw/src_bios6/utils/utils_ringbuffer.h>


Int32 Utils_ringBuffer_init(RingBufferHandle_t ringBufferHnl, UInt32 size)
{
    Int32 status = 0;
    UInt8 *pBaseAddr;
    Error_Block ebObj;
    Error_Block *eb = &ebObj;
    GateMP_Params gparams;
    GateMP_Handle gateHandle;

    Error_init(eb);

    /* init ring buffer to 0's */
    memset(ringBufferHnl, 0, sizeof(*ringBufferHnl));

    /* align size to minimum required frame buffer alignment */
    size = VpsUtils_align(size, IVACODEC_VDMA_BUFFER_ALIGNMENT);

    /* allocate the memory for ring buffer from bitstream shared regioin 1*/
    pBaseAddr = Memory_alloc(SharedRegion_getHeap(1), size, IVACODEC_VDMA_BUFFER_ALIGNMENT, eb);

    #ifdef UTILS_MEM_DEBUG
    Vps_printf(" UTILS: MEM: BITS ALLOC, addr = 0x%08x, size = %d bytes\n", pBaseAddr, size);
    #endif

    if (!Error_check(eb) && (pBaseAddr != NULL))
    {
        memset(pBaseAddr, 0x80, size);
    }

    if (pBaseAddr == NULL)
    {
        status = -1;
    }

	//init gateMP to secure rdOfst, wrOfst, encodedStreamCount
	GateMP_Params_init(&gparams);

	gparams.regionId 		= SYSTEM_IPC_SR_NON_CACHED_DEFAULT;
	gparams.remoteProtect	= GateMP_RemoteProtect_SYSTEM;
	gateHandle 				= GateMP_create(&gparams);
	if (gateHandle == NULL)
	{
		Vps_printf("%s - %d ringBuffer GateMP creation failed\n", __FUNCTION__ , __LINE__);
		status = -1;
	}

    if (!UTILS_ISERROR(status))
    {
		ringBufferHnl->baseAddr					= pBaseAddr;
		ringBufferHnl->totalSize 				= size;
		ringBufferHnl->wrOfst					= 0;
		ringBufferHnl->rdOfst 					= 0;
		ringBufferHnl->encodedStreamCount		= 0;
		ringBufferHnl->ringBufferGateHandle		= gateHandle;
    }

    UTILS_assert(status == 0);

    return status;
}

Int32 Utils_ringBuffer_deinit(RingBufferHandle_t ringBufferHnl)
{
    UInt32 size;

    size = ringBufferHnl->totalSize;

    /* free the frame buffer memory */
    #ifdef UTILS_MEM_DEBUG
    Vps_printf(" UTILS: MEM: BITS FREE, addr = 0x%08x, size = %d bytes\n", ringBufferHnl->baseAddr, size);
    #endif

    // free the ring buffer memory
    Memory_free(SharedRegion_getHeap(1), ringBufferHnl->baseAddr, size);

	ringBufferHnl->baseAddr					= NULL;
	ringBufferHnl->totalSize 				= 0;
	ringBufferHnl->wrOfst					= 0;
	ringBufferHnl->rdOfst 					= 0;
	ringBufferHnl->encodedStreamCount		= NULL;

	//free the gateMP
	GateMP_delete(&ringBufferHnl->ringBufferGateHandle);

    return 0;
}

Bool Utils_ringBuffer_isFull(RingBufferHandle_t ringBufferHnl)
{
	if ((ringBufferHnl->wrOfst == ringBufferHnl->rdOfst) && (ringBufferHnl->encodedStreamCount !=0))
		return TRUE;
	else
		return FALSE;
}

Bool Utils_ringBuffer_isEmpty(RingBufferHandle_t ringBufferHnl)
{
	if ((ringBufferHnl->wrOfst == ringBufferHnl->rdOfst) && (ringBufferHnl->encodedStreamCount ==0))
		return TRUE;
	else
		return FALSE;
}

Bool Utils_ringBuffer_print_info(RingBufferHandle_t ringBufferHnl)
{
	 Vps_printf ("\n******************ring buffer info *********************\n");
	 Vps_printf ("ring buffer info ringBufferHnl->baseAddr:0x%x\n" ,ringBufferHnl->baseAddr);
	 Vps_printf ("ring buffer info ringBufferHnl->totalSize:0x%x\n" ,ringBufferHnl->totalSize);
	 Vps_printf ("ring buffer info ringBufferHnl->wrOfst:0x%x\n" ,ringBufferHnl->wrOfst);
	 Vps_printf ("ring buffer info ringBufferHnl->rdOfst:0x%x\n" ,ringBufferHnl->rdOfst);
	 Vps_printf ("ring buffer info ringBufferHnl->encodedStreamCount:%d\n" ,ringBufferHnl->encodedStreamCount);
	 Vps_printf ("ring buffer info ringBufferHnl->ringBufferGateHandle:0x%x\n" ,ringBufferHnl->ringBufferGateHandle);
	 return TRUE;
}

Bool Utils_ringBuffer_Size_Avail_for_MIN(RingBufferHandle_t ringBufferHnl, UInt32 minSize)
{
	IArg key;
	GateMP_Handle gateHandle = ringBufferHnl->ringBufferGateHandle;

	key = GateMP_enter(gateHandle);

	if(!Utils_ringBuffer_isFull(ringBufferHnl))
	{
		if (ringBufferHnl->wrOfst >= ringBufferHnl->rdOfst)
			{
				if((ringBufferHnl->totalSize - ringBufferHnl->wrOfst) >= minSize)
					{
						GateMP_leave(gateHandle, key);
						return TRUE;
					}
				else
					{
						if(ringBufferHnl->rdOfst >= minSize)
							{
								GateMP_leave(gateHandle, key);
								return TRUE;
							}
						else
							{
								GateMP_leave(gateHandle, key);
								return FALSE;
							}
					}
			}
		else
			{
				if((ringBufferHnl->rdOfst - ringBufferHnl->wrOfst)>=minSize)
					{
						GateMP_leave(gateHandle, key);
						return TRUE;
					}
				else
					{
						GateMP_leave(gateHandle, key);
						return FALSE;
					}
			}
	}
	else
		{
			GateMP_leave(gateHandle, key);
			return FALSE;
		}
}

Int32 Utils_ringBuffer_updateRdOfst(RingBufferHandle_t ringBufferHnl, UInt32 length)
{
	IArg key;
	GateMP_Handle gateHandle = ringBufferHnl->ringBufferGateHandle;

	key = GateMP_enter(gateHandle);

	ringBufferHnl->rdOfst = length%ringBufferHnl->totalSize;

	GateMP_leave(gateHandle, key);
	return 0;
}

Int32 Utils_ringBuffer_updateWrOfst(RingBufferHandle_t ringBufferHnl, UInt32 length)
{
	IArg key;
	GateMP_Handle gateHandle = ringBufferHnl->ringBufferGateHandle;
	key = GateMP_enter(gateHandle);

	ringBufferHnl->wrOfst = (ringBufferHnl->wrOfst + length)%ringBufferHnl->totalSize;

	GateMP_leave(gateHandle, key);
	return 0;
}

Int32 Utils_ringBuffer_updateEncodedStreamCount(RingBufferHandle_t ringBufferHnl, Bool increment)
{
	IArg key;
	GateMP_Handle gateHandle = ringBufferHnl->ringBufferGateHandle;

	key = GateMP_enter(gateHandle);

	if(increment)
		ringBufferHnl->encodedStreamCount++;
	else
		ringBufferHnl->encodedStreamCount--;

	GateMP_leave(gateHandle, key);
	return 0;
}

