/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SYSTEM_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_COMMON_H_  

/**
    \ingroup SYSTEM_LINK_API

    @{
*/

/**
    \file system_common.h
    \brief Common system data structure's and constants

    Common defines, data structure's shared across processor's
    Typically user does not use these in his application directly
*/

#include <stdio.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/HeapMemMP.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/MessageQ.h>



/* Frequency values of various cores set explicitly during system init */
#define SYSTEM_M3CORE_FREQ         (200*1000*1000)  

#if defined(TI_814X_BUILD)
#define SYSTEM_M3VPSS_FREQ         (SYSTEM_M3CORE_FREQ)
#define SYSTEM_M3VIDEO_FREQ        (SYSTEM_M3CORE_FREQ)
#define SYSTEM_DSP_FREQ            (750*1000*1000)
#endif
#if defined(TI_8107_BUILD)
#define SYSTEM_M3VPSS_FREQ         (SYSTEM_M3CORE_FREQ)
#define SYSTEM_M3VIDEO_FREQ        (SYSTEM_M3CORE_FREQ)
#define SYSTEM_DSP_FREQ            (0)
#endif

#define SYSTEM_CMD_CREATE          (0x00000000)
#define SYSTEM_CMD_START           (0x00000001)
#define SYSTEM_CMD_STOP            (0x00000002)
#define SYSTEM_CMD_DELETE          (0x00000003)
#define SYSTEM_CMD_NEW_DATA        (0x00000004)
#define SYSTEM_CMD_GET_INFO        (0x00000005)

#define SYSTEM_IPC_M3_MAX_LIST_ELEM    (1000)

#define SYSTEM_IPC_BITS_MAX_LIST_ELEM  (SYSTEM_IPC_M3_MAX_LIST_ELEM)

#define SYSTEM_IPC_FRAMES_MAX_LIST_ELEM  (SYSTEM_IPC_M3_MAX_LIST_ELEM)
#define SYSTEM_IPC_CMD_RELEASE_FRAMES  (0x2000)
#define SYSTEM_IPC_CMD_SLAVE_CORE_EXCEPTION (0x2001)

#define SYSTEM_IPC_NOTIFY_LINE_ID   (0)
#define SYSTEM_IPC_NOTIFY_EVENT_ID  (12)

/* Motion Detection */
#define SYSTEM_IPC_NOTIFY_EVENT_ID_APP  (SYSTEM_IPC_NOTIFY_EVENT_ID + 1)
#define SYSTEM_IPC_EVTTYPE_MD			(0)

#define SYSTEM_IPC_MSGQ_HEAP_NAME               "IPC_MSGQ_MSG_HEAP"
#define SYSTEM_IPC_MSGQ_HEAP_SIZE               (512*1024)
#define SYSTEM_IPC_MSGQ_MSG_SIZE_MAX            (64*1024)
#define SYSTEM_IPC_MSGQ_HEAP                    (0)
#define SYSTEM_IPC_MSGQ_MSG_PAYLOAD_PTR(msg)    (Void*)((UInt32)(msg)+sizeof(SystemIpcMsgQ_Msg))
#define SYSTEM_IPC_MSGQ_OWNER_PROC_ID           SYSTEM_PROC_M3VPSS

#define SYSTEM_IPC_SR_NON_CACHED_DEFAULT     (0)
#define SYSTEM_IPC_SR_M3_LIST_MP             (0)
#define SYSTEM_IPC_SR_CACHED                 (1)
#define SYSTEM_IPC_SR_VIDEO_FRAME            (2)

#define SYSTEM_IPC_PROC_NAME_DSP             "DSP"
#define SYSTEM_IPC_PROC_NAME_M3VIDEO         "VIDEO-M3"
#define SYSTEM_IPC_PROC_NAME_M3VPSS          "VPSS-M3"
#define SYSTEM_IPC_PROC_NAME_HOST            "HOST"
#define SYSTEM_IPC_PROC_NAME_INVALID         "INVALID PROC"

/* Dual mode timer to be used in the system */
#define SYSTEM_DMTIMER_ID                    (2)

#ifdef TI816X_2G_DDR
#define SYSTEM_DDR_BASE_ADDR (0x80000000)
#endif

#if defined(TI_8107_BUILD) || !defined(IPNC_DSP_ON)
#define SYSTEM_PROC_DSP_ID         SYSTEM_PROC_INVALID
#else                                   
#define SYSTEM_PROC_DSP_ID         SYSTEM_PROC_DSP
#endif                                  

#define SYSTEM_IPC_ENABLE_PROC_ID   {\
            SYSTEM_PROC_HOSTA8,         \
            SYSTEM_PROC_DSP_ID,            \
            SYSTEM_PROC_M3VIDEO,        \
            SYSTEM_PROC_M3VPSS,         \
            SYSTEM_PROC_MAX /* Last entry */    \
            }


#define SYSTEM_SCAN_FORMAT_STRINGS  \
        { "INTERLACED ",    \
          "PROGRESSIVE"     \
        }

#define SYSTEM_MEMORY_TYPE_STRINGS  \
        { "NON-TILED  ",    \
          "TILED      "     \
        }

#define SYSTEM_ON_OFF_STRINGS  \
        { "OFF",    \
          "ON"     \
        }

typedef struct {

    MessageQ_MsgHeader msgHead;
    UInt32 linkId;
    UInt32 cmd;
    UInt32 prmSize;
    UInt32 waitAck;
    UInt32 status;

} SystemIpcMsgQ_Msg;


extern const UInt32 gSystem_ipcEnableProcId[];

static inline char *System_getProcName(UInt32 procId)
{
    if(procId==SYSTEM_PROC_DSP)
        return SYSTEM_IPC_PROC_NAME_DSP;

    if(procId==SYSTEM_PROC_M3VIDEO)
        return SYSTEM_IPC_PROC_NAME_M3VIDEO;

    if(procId==SYSTEM_PROC_M3VPSS)
        return SYSTEM_IPC_PROC_NAME_M3VPSS;

    if(procId==SYSTEM_PROC_HOSTA8)
        return SYSTEM_IPC_PROC_NAME_HOST;

    return SYSTEM_IPC_PROC_NAME_INVALID;
}

/*
    Convert from Syslink Proc ID to McFW Proc ID

    Syslink/IPC Proc ID could be different from McFW Proc ID
    and framework does not rely on
    syslink/IPC Proc ID and McFW Proc ID being same.
*/
static inline UInt32 System_getSyslinkProcId(UInt32 procId)
{
    char *procName = System_getProcName(procId);

    if(strcmp(procName, SYSTEM_IPC_PROC_NAME_INVALID)!=0)
    {
        return MultiProc_getId(procName);
    }

    return MultiProc_INVALIDID;
}

/*
    Return McFW Proc ID

    Note,
    Syslink/IPC Proc ID could be different from McFW Proc ID
    and framework does not rely on
    syslink/IPC Proc ID and McFW Proc ID being same.
*/
UInt32 System_getSelfProcId();


static inline Int32 System_ipcGetMsgQName(UInt32 procId, char *msgQName, char *ackMsgQName)
{
    UTILS_assert(procId< SYSTEM_PROC_MAX);

    sprintf(msgQName, "%s_MSGQ", System_getProcName(procId));
    sprintf(ackMsgQName, "%s_ACK_MSGQ", System_getProcName(procId));

    return 0;
}

static inline Int32 System_ipcGetListMPName(UInt32 linkId, char *listMPOutName, char *listMPInName)
{
    UInt32 procId;

    procId = SYSTEM_GET_PROC_ID(linkId);
    linkId = SYSTEM_GET_LINK_ID(linkId);

    UTILS_assert(procId< SYSTEM_PROC_MAX);
    UTILS_assert(linkId< SYSTEM_LINK_ID_MAX);

    sprintf(listMPOutName, "%s_IPC_OUT_%d", System_getProcName(procId), linkId);
    sprintf(listMPInName , "%s_IPC_IN_%d", System_getProcName(procId), linkId);

    return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
