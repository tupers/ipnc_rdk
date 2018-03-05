/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/



#include "system_priv_ipc.h"
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <ti/syslink/SysLink.h>
#include <ti/syslink/ProcMgr.h>

System_IpcObj gSystem_ipcObj;


Int32 System_printProcId(UInt32 procId)
{
    char *procName;
    UInt32 syslinkProcId;

    procName = System_getProcName(procId);
    syslinkProcId = System_getSyslinkProcId(procId);

    if(syslinkProcId == MultiProc_INVALIDID)
    {
        OSA_printf(" %u: SYSTEM: CPU [%s] is NOT available on this platform !!!\n",
            OSA_getCurTimeInMsec(),
            procName
            );
    }
    else
    {
        OSA_printf(" %u: SYSTEM: CPU [%s] syslink proc ID is [%d] !!!\n",
            OSA_getCurTimeInMsec(),
            procName,
            syslinkProcId
            );
    }

    return 0;
}

Int32 System_ipcInit()
{
    OSA_printf(" %u: SYSTEM: IPC init in progress !!!\n", OSA_getCurTimeInMsec());

    SysLink_setup ();

    {
        UInt32 procId;

        for(procId=0; procId<SYSTEM_PROC_MAX; procId++)
            System_printProcId(procId);
    }

    System_ipcMsgQInit();

    System_ipcNotifyInit();


    OSA_printf(" %u: SYSTEM: IPC init DONE !!!\n", OSA_getCurTimeInMsec());

    return OSA_SOK;
}

Int32 System_ipcDeInit()
{
    OSA_printf(" %u: SYSTEM: IPC de-init in progress !!!\n", OSA_getCurTimeInMsec());

    System_ipcNotifyDeInit();

    System_ipcMsgQDeInit();

    SysLink_destroy ();

    OSA_printf(" %u: SYSTEM: IPC de-init DONE !!!\n", OSA_getCurTimeInMsec());

    return OSA_SOK;
}

Int32 System_ipcGetSlaveCoreSymbolAddress(char *symbolName,UInt16 procId,
                                    UInt32 *symAddressPtr)
{
    Int32 status;
    ProcMgr_Handle procMgrHandle = NULL;
    UInt32 symbolAddress = 0;
    UInt32 fileId;
    UInt32 syslinkProcId;

    syslinkProcId = System_getSyslinkProcId(procId);

    *symAddressPtr = 0;
    status = ProcMgr_open (&procMgrHandle, syslinkProcId);

    OSA_assert(procMgrHandle != NULL);

    fileId = ProcMgr_getLoadedFileId (procMgrHandle);

    status = ProcMgr_getSymbolAddress(procMgrHandle,
                                      fileId,
                                      symbolName,
                                      &symbolAddress);

    OSA_assert(symbolAddress != 0);
    *symAddressPtr = symbolAddress;
    status = ProcMgr_close(&procMgrHandle);
    OSA_assert(status > 0);
    return OSA_SOK;
}

Int32 System_ipcCopySlaveCoreSymbolContents(char *symbolName,
                                      UInt16 procId,
                                      Ptr  dstPtr,
                                      UInt32 copySize)
{
    UInt32 symbolAddress;
    Int32 status = OSA_SOK;
    Ptr   symbolAddressMapped;

    status =
    System_ipcGetSlaveCoreSymbolAddress(symbolName,procId,&symbolAddress);
    OSA_assert(OSA_SOK  == status);

    status = OSA_mapMem(symbolAddress,copySize,&symbolAddressMapped);
    OSA_assert((status == 0) && (symbolAddressMapped != NULL));

    memcpy(dstPtr,symbolAddressMapped,copySize);

    status = OSA_unmapMem(symbolAddressMapped,copySize);

    OSA_assert(status == 0);

    return status;
}
