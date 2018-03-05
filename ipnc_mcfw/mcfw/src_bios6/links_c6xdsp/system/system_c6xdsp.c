/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <xdc/std.h>
#include "system_priv_c6xdsp.h"
#include <mcfw/interfaces/link_api/algLink.h>
#include <mcfw/interfaces/link_api/vaLink.h>
#include <mcfw/interfaces/link_api/imgLink.h>
#include <mcfw/interfaces/link_api/rvmLink.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/src_bios6/links_c6xdsp/utils/utils_dsp.h>

#define SYSTEM_DEBUG

System_DspObj gSystem_objDsp;

Int32 System_init()
{
    Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System DSP Init in progress !!!\n",
               Clock_getTicks());
#endif

    IpcFramesInLink_init();
    IpcFramesOutLink_init();
    IpcBitsOutLink_init();

    Utils_dspInit();

    System_initLinks();

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System DSP Init Done !!!\n", Clock_getTicks());
#endif
    return status;
}

Int32 System_deInit()
{
#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System Dsp De-Init in progress !!!\n",
               Clock_getTicks());
#endif

    IpcFramesInLink_deInit();
    IpcFramesOutLink_deInit();
    IpcBitsOutLink_deInit();

    System_deInitLinks();
	
	Utils_dspDeInit();

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System Dsp De-Init Done !!!\n",
               Clock_getTicks());
#endif

    return FVID2_SOK;
}

Void System_initLinks()
{
	Vps_printf(" %d: SYSTEM  : Initializing Links !!! \r\n", Clock_getTicks());

	System_memPrintHeapStatus();

	AlgLink_init();
//	VaLink_init();
	NullLink_init();
	MergeLink_init();
	SelectLink_init();
//	RvmLink_init();
	ImgLink_init();
	Vps_printf(" %d: SYSTEM  : Initializing Links ... DONE !!! \r\n",
			Clock_getTicks());					
}

Void System_deInitLinks()
{
    Vps_printf(" %d: SYSTEM  : De-Initializing Links !!! \r\n",
               Clock_getTicks());

    SelectLink_deInit();
    MergeLink_deInit();
    NullLink_deInit();
//	VaLink_deInit();
    AlgLink_deInit();
//	RvmLink_deInit();
	ImgLink_deInit();
    Vps_printf(" %d: SYSTEM  : De-Initializing Links ... DONE !!! \r\n",
               Clock_getTicks());
}
