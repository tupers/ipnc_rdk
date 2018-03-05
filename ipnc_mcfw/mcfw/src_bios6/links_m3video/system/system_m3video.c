/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "system_priv_m3video.h"
#include <mcfw/interfaces/link_api/encLink.h>
#include <mcfw/interfaces/link_api/decLink.h>
#include <mcfw/src_bios6/links_m3video/codec_utils/utils_encdec.h>
#include <mcfw/interfaces/link_api/vstabLink.h>


System_VideoObj gSystem_objVideo;

Int32 System_init()
{
    Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System Video Init in progress !!!\n",
               Utils_getCurTimeInMsec());
#endif

    IpcOutM3Link_init();
    IpcInM3Link_init();
    IpcBitsInLink_init();
    IpcBitsOutLink_init();

    Utils_encdecInit();

    System_initLinks();

#ifdef SYSTEM_DEBUG
		Vps_printf(" %d: SYSTEM  : System Video Init Done !!!\n", Utils_getCurTimeInMsec());
#endif
    return status;
}

Int32 System_deInit()
{
#ifdef SYSTEM_DEBUG
		Vps_printf(" %d: SYSTEM  : System Video De-Init in progress !!!\n",
				   Utils_getCurTimeInMsec());
#endif

    System_deInitLinks();

#ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM  : System Video De-Init Done !!!\n",
               Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}

Void System_initLinks()
{
    Vps_printf(" %d: SYSTEM  : Initializing Links !!! \r\n", Utils_getCurTimeInMsec());
    System_memPrintHeapStatus();

    MctnfLink_init();
    EncLink_init();
    DecLink_init();
    NullLink_init();
    NullSrcLink_init();
    DupLink_init();
    MergeLink_init();
	VstabLink_init();

    Vps_printf(" %d: SYSTEM  : Initializing Links ... DONE !!! \r\n",
               Utils_getCurTimeInMsec());
}

Void System_deInitLinks()
{
    Vps_printf(" %d: SYSTEM  : De-Initializing Links !!! \r\n",
               Utils_getCurTimeInMsec());

    IpcOutM3Link_deInit();
    IpcInM3Link_deInit();
    IpcBitsInLink_deInit();
    IpcBitsOutLink_deInit();

    MctnfLink_deInit();
    DecLink_deInit();
    EncLink_deInit();
    MergeLink_deInit();
    DupLink_deInit();
    NullLink_deInit();
    NullSrcLink_deInit();
	VstabLink_deInit();

    System_memPrintHeapStatus();

    Vps_printf(" %d: SYSTEM  : De-Initializing Links ... DONE !!! \r\n",
               Utils_getCurTimeInMsec());
}
