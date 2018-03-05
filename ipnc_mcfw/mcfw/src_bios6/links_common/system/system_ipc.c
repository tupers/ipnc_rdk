/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "system_priv_common.h"
#include "system_priv_ipc.h"

System_IpcObj gSystem_ipcObj;

Void System_ipcStart()
{
    Int32 status = Ipc_S_SUCCESS;

    do
    {
        /* Call Ipc_start() */
        status = Ipc_start();
    } while (status != Ipc_S_SUCCESS);
}

Void System_ipcStop()
{
    Int32 status = Ipc_S_SUCCESS;

    do
    {
        /* Call Ipc_start() */
        status = Ipc_stop();
    } while (status != Ipc_S_SUCCESS);
}

static Void System_ipcIntraDucatiSrOpen()
{
    SharedRegion_Entry srEntry;
    Int                srStatus = SharedRegion_S_SUCCESS;

    if ((System_getSelfProcId() == SYSTEM_PROC_M3VPSS)
        ||
        (System_getSelfProcId() == SYSTEM_PROC_M3VIDEO))
    {
        SharedRegion_entryInit(&srEntry);
        SharedRegion_getEntry(SYSTEM_IPC_SR_M3_LIST_MP, &srEntry);

        if ((FALSE == srEntry.isValid)
            &&
            (0 != srEntry.len))
        {
            srEntry.isValid     = TRUE;
            do {
                srStatus = SharedRegion_setEntry(SYSTEM_IPC_SR_M3_LIST_MP, &srEntry);

                if (srStatus != SharedRegion_S_SUCCESS) {
                    Vps_printf(" %d: MEM: ERROR: SharedRegion_setEntry (%d, 0x%08x) FAILED !!! "
                               " (status=%d) \n", Utils_getCurTimeInMsec(),
                               SYSTEM_IPC_SR_M3_LIST_MP, &srEntry, srStatus);
                    Task_sleep(10);
                }
            } while (srStatus != SharedRegion_S_SUCCESS);
        }
    }
}


Void System_ipcAttach()
{
    UInt32 i;
    UInt32 procId, syslinkProcId;
    Int32 status;

    i = 0;

    while (gSystem_ipcEnableProcId[i] != SYSTEM_PROC_MAX)
    {
        procId = gSystem_ipcEnableProcId[i];

        if ((procId != System_getSelfProcId()) && (procId != SYSTEM_PROC_INVALID))
        {
            do
            {
                Vps_printf(" %d: SYSTEM: Attaching to [%s] ... \n",
                           Utils_getCurTimeInMsec(), System_getProcName(procId));

                syslinkProcId = System_getSyslinkProcId(procId);

                status = Ipc_attach(syslinkProcId);
                if (status != 0)
                {
                    Task_sleep(50);
                }
            } while (status < 0);
            Vps_printf(" %d: SYSTEM: Attaching to [%s] ... SUCCESS !!!\n",
                       Utils_getCurTimeInMsec(), System_getProcName(procId));
        }
        i++;
    }
    System_ipcIntraDucatiSrOpen();
}

Int32 System_ipcInit()
{
    Vps_printf(" %d: SYSTEM: IPC init in progress !!!\n", Utils_getCurTimeInMsec());

    System_ipcAttach();

    System_ipcMsgQInit();

    System_ipcNotifyInit();

    Vps_printf(" %d: SYSTEM: IPC init DONE !!!\n", Utils_getCurTimeInMsec());

    return FVID2_SOK;
}

Int32 System_ipcDeInit()
{
    Vps_printf(" %d: SYSTEM: IPC de-init in progress !!!\n", Utils_getCurTimeInMsec());

    System_ipcNotifyDeInit();

    System_ipcMsgQDeInit();

    Vps_printf(" %d: SYSTEM: IPC de-init DONE !!!\n", Utils_getCurTimeInMsec());

    return FVID2_SOK;
}

