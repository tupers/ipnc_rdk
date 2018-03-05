/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _UTILS_ENCDEC_PRF_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _UTILS_ENCDEC_PRF_H_

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/psp/vps/vps.h>
#include <mcfw/src_bios6/utils/utils.h>

#define UTILS_ENCDEC_HDVICP_PROFILE

#define UTILS_ENCDEC_MAXNUMOFHDVICP2_RESOUCES  (1)

typedef struct HDVICP_logTbl {
    UInt32 totalAcquire2wait;
    UInt32 totalWait2Isr;
    UInt32 totalIsr2Done;
    UInt32 totalWait2Done;
    UInt32 totalDone2Release;
    UInt32 totalAcquire2Release;
    UInt32 totalAcq2acqDelay;
    UInt32 tempPrevTime;
    UInt32 tempAcquireTime;
    UInt32 tempWaitTime;
    UInt32 startTime;
    UInt32 endTime;
    UInt32 numAccessCnt;
    UInt32 currentBatchSize;
    UInt32 totalIVAHDActivateTime;
} HDVICP_logTbl;

typedef struct HDVICP_tskEnv
{
    UInt32 size;
    UInt32 ivaID;
    Int32 batchSize;
} HDVICP_tskEnv;

extern HDVICP_logTbl g_HDVICP_logTbl[UTILS_ENCDEC_MAXNUMOFHDVICP2_RESOUCES];

static inline UInt32 Utils_encdecGetTime(Void)
{
    return (Utils_getCurTimeInMsec());
}

static Void Utils_encdecHdvicpPrfInit(Void)
{
    memset(&g_HDVICP_logTbl, 0, sizeof(g_HDVICP_logTbl));
    return;
}

static Void Utils_encdecHdvicpPrfPrint(void)
{
    int ivaId;
    UInt32 totalElapsedTime;

#ifdef UTILS_ENCDEC_HDVICP_PROFILE
    for (ivaId = 0; ivaId < UTILS_ENCDEC_MAXNUMOFHDVICP2_RESOUCES; ivaId++)
    {

        Vps_printf("\n\t%d: HDVICP-ID:%d\n", Utils_encdecGetTime(), ivaId);

        totalElapsedTime = (g_HDVICP_logTbl[ivaId].endTime -
                            g_HDVICP_logTbl[ivaId].startTime);

        Vps_printf("\n\t\ttotalAcquire2wait in msec:%10d"
                   "\n\t\ttotalWait2Isr in msec:%10d"
                   "\n\t\ttotalIsr2Done in msec:%10d"
                   "\n\t\ttotalWait2Done in msec:%10d"
                   "\n\t\ttotalDone2Release in msec:%10d"
                   "\n\t\ttotalAcquire2Release in msec:%10d"
                   "\n\t\ttotalAcq2acqDelay in msec:%10d"
                   "\n\t\ttotalElapsedTime in msec:%10d"
                   "\n\t\tnumAccessCnt:%10d\n",
                   g_HDVICP_logTbl[ivaId].totalAcquire2wait,
                   g_HDVICP_logTbl[ivaId].totalWait2Isr,
                   g_HDVICP_logTbl[ivaId].totalIsr2Done,
                   g_HDVICP_logTbl[ivaId].totalWait2Done,
                   g_HDVICP_logTbl[ivaId].totalDone2Release,
                   g_HDVICP_logTbl[ivaId].totalAcquire2Release,
                   g_HDVICP_logTbl[ivaId].totalAcq2acqDelay,
                   totalElapsedTime, g_HDVICP_logTbl[ivaId].numAccessCnt);
        Vps_printf("\n\t\tIVA-FPS :%10d\n",
                   ((g_HDVICP_logTbl[ivaId].numAccessCnt) /
                    (totalElapsedTime/1000)));
    }
#endif
    return;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
