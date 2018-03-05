/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "system_priv_common.h"
#include "system_priv_ipc.h"
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/interfaces/link_api/glbceSupportLink.h>
#include <mcfw/interfaces/link_api/frameProcessHost.h>

System_CommonObj gSystem_objCommon;

const UInt32 gSystem_ipcEnableProcId[] = SYSTEM_IPC_ENABLE_PROC_ID;

const char *gSystem_nameScanFormat[] = SYSTEM_SCAN_FORMAT_STRINGS;

const char *gSystem_nameMemoryType[] = SYSTEM_MEMORY_TYPE_STRINGS;

const char *gSystem_nameOnOff[] = SYSTEM_ON_OFF_STRINGS; 

UInt32 System_getSelfProcId()
{
    static UInt32 selfProcId = SYSTEM_PROC_INVALID; 
    UInt32 syslinkProcId;
    String procName;

    /* find self proc ID only once, since it never changes run-time
        Doing this optimization since, this API is called for every System_linkXxxx
        API
    */
    if(selfProcId!=SYSTEM_PROC_INVALID)
        return selfProcId;

    syslinkProcId = MultiProc_self();
    procName = MultiProc_getName(syslinkProcId);

    if(procName==NULL || syslinkProcId >= SYSTEM_PROC_MAX)
        selfProcId = SYSTEM_PROC_INVALID;
    else
    if(strcmp(procName, SYSTEM_IPC_PROC_NAME_DSP)==0)
        selfProcId = SYSTEM_PROC_DSP;
    else
    if(strcmp(procName, SYSTEM_IPC_PROC_NAME_M3VPSS)==0)
        selfProcId = SYSTEM_PROC_M3VPSS;
    else
    if(strcmp(procName, SYSTEM_IPC_PROC_NAME_M3VIDEO)==0)
        selfProcId = SYSTEM_PROC_M3VIDEO;
    else
    if(strcmp(procName, SYSTEM_IPC_PROC_NAME_HOST)==0)
        selfProcId = SYSTEM_PROC_HOSTA8;
    else
        selfProcId = SYSTEM_PROC_INVALID;

    return selfProcId;
}

Int32 System_init()
{
    Int32         status;

    #ifdef SYSTEM_DEBUG
    OSA_printf ( " %u: SYSTEM: System Common Init in progress !!!\n", OSA_getCurTimeInMsec());
    #endif

    memset(&gSystem_objCommon, 0, sizeof(gSystem_objCommon));

    System_ipcInit();

    IpcBitsInLink_init();
    IpcBitsOutLink_init();
    IpcFramesInLink_init();
    IpcFramesOutLink_init();
	GlbceSupportLink_init();
	FrameProcessHostLink_init();

    status = OSA_mbxCreate(&gSystem_objCommon.mbx);
    UTILS_assert(  status==OSA_SOK);

    SystemLink_init();

    #ifdef SYSTEM_DEBUG
    OSA_printf ( " %u: SYSTEM: System Common Init Done !!!\n", OSA_getCurTimeInMsec());
    #endif

    return status;

}

Int32 System_deInit()
{
    #ifdef SYSTEM_DEBUG
    OSA_printf ( " %u: SYSTEM: System Common De-Init in progress !!!\n", OSA_getCurTimeInMsec());
    #endif

    SystemLink_deInit();

    GlbceSupportLink_deInit();	
	IpcBitsInLink_deInit();
    IpcBitsOutLink_deInit();
    IpcFramesInLink_deInit();
    IpcFramesOutLink_deInit();

    System_ipcDeInit();

    OSA_mbxDelete(&gSystem_objCommon.mbx);

    #ifdef SYSTEM_DEBUG
    OSA_printf ( " %u: SYSTEM: System Common De-Init Done !!!\n", OSA_getCurTimeInMsec());
    #endif

    return OSA_SOK;
}

