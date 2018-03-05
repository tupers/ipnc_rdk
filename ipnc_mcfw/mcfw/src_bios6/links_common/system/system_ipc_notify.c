/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "system_priv_common.h"
#include "system_priv_ipc.h"

Int32 System_ipcRegisterNotifyCb(UInt32 linkId, System_ipcNotifyCb notifyCb)
{
    linkId = SYSTEM_GET_LINK_ID(linkId);
    UTILS_assert(linkId < SYSTEM_LINK_ID_MAX);

    gSystem_ipcObj.notifyCb[linkId] = notifyCb;

    return FVID2_SOK;
}

Void System_ipcNotifyHandler(UInt16 procId, UInt16 lineId,
                             UInt32 eventId, UArg arg, UInt32 payload)
{
    UInt32 linkId, linkProcId;
    Utils_TskHndl *pTsk;

    if (lineId != SYSTEM_IPC_NOTIFY_LINE_ID)
        return;

    if (eventId != SYSTEM_IPC_NOTIFY_EVENT_ID)
        return;

    linkProcId = SYSTEM_GET_PROC_ID(payload);
    linkId = SYSTEM_GET_LINK_ID(payload);

    if (linkId >= SYSTEM_LINK_ID_MAX)
        return;

    UTILS_assert(System_getSelfProcId() == linkProcId);

    if (gSystem_ipcObj.notifyCb[linkId])
    {
        pTsk = System_getLinkTskHndl(linkId);

        gSystem_ipcObj.notifyCb[linkId] (pTsk);
    }
}

Int32 System_ipcSendNotify(UInt32 linkId)
{
    Int32 status;
    UInt32 procId = SYSTEM_GET_PROC_ID(linkId), syslinkProcId;

    UTILS_assert(procId < SYSTEM_PROC_MAX);

    syslinkProcId = System_getSyslinkProcId(procId);

    status = Notify_sendEvent(syslinkProcId,
                              SYSTEM_IPC_NOTIFY_LINE_ID,
                              SYSTEM_IPC_NOTIFY_EVENT_ID, linkId, TRUE);

    if (status != Notify_S_SUCCESS)
    {
        Vps_printf
            (" %d: NOTIFY: Send Event to [%s][%d] failed !!! (status = %d)\n",
             Utils_getCurTimeInMsec(), System_getProcName(SYSTEM_GET_PROC_ID(linkId)),
             SYSTEM_GET_LINK_ID(linkId), status);

        UTILS_assert(status == Notify_S_SUCCESS);
    }

    return FVID2_SOK;
}

Int32 System_ipcNotifyInit()
{
    UInt32 procId, i, syslinkProcId;
    Int32 status = FVID2_SOK;

    memset(gSystem_ipcObj.notifyCb, 0, sizeof(gSystem_ipcObj.notifyCb));

    i = 0;
    while (gSystem_ipcEnableProcId[i] != SYSTEM_PROC_MAX)
    {
        procId = gSystem_ipcEnableProcId[i];
        if ((procId != System_getSelfProcId()) && (procId != SYSTEM_PROC_INVALID))
        {
            Vps_printf
                (" %d: SYSTEM: Notify register to [%s] line %d, event %d ... \n",
                 Utils_getCurTimeInMsec(), System_getProcName(procId),
                 SYSTEM_IPC_NOTIFY_LINE_ID, SYSTEM_IPC_NOTIFY_EVENT_ID);

            syslinkProcId = System_getSyslinkProcId(procId);

            if (Notify_intLineRegistered(syslinkProcId, SYSTEM_IPC_NOTIFY_LINE_ID) ==
                FALSE)
            {
                UTILS_assert(0);
            }
            if (Notify_eventAvailable
                (syslinkProcId, SYSTEM_IPC_NOTIFY_LINE_ID,
                 SYSTEM_IPC_NOTIFY_EVENT_ID) == FALSE)
            {
                UTILS_assert(0);
            }

            status = Notify_registerEvent(syslinkProcId,
                                          SYSTEM_IPC_NOTIFY_LINE_ID,
                                          SYSTEM_IPC_NOTIFY_EVENT_ID,
                                          System_ipcNotifyHandler, NULL);

            UTILS_assert(status == Notify_S_SUCCESS);
        }
        i++;
    }

    return status;
}

Int32 System_ipcNotifyDeInit()
{
    Int32 status = FVID2_SOK;
    UInt32 i, procId, syslinkProcId;

    Vps_printf(" %d: SYSTEM: IPC Notify de-init in progress !!!\n",
               Utils_getCurTimeInMsec());

    i = 0;

    while (gSystem_ipcEnableProcId[i] != SYSTEM_PROC_MAX)
    {
        procId = gSystem_ipcEnableProcId[i];
        if ((procId != System_getSelfProcId()) && (procId != SYSTEM_PROC_INVALID))
        {
            syslinkProcId = System_getSyslinkProcId(procId);

            status = Notify_unregisterEvent(syslinkProcId,
                                            SYSTEM_IPC_NOTIFY_LINE_ID,
                                            SYSTEM_IPC_NOTIFY_EVENT_ID,
                                            System_ipcNotifyHandler, NULL);

            UTILS_assert(status == Notify_S_SUCCESS);
        }
        i++;
    }

    Vps_printf(" %d: SYSTEM: IPC Notify de-init DONE !!!\n", Utils_getCurTimeInMsec());

    return status;
}
