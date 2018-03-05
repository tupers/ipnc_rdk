/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup AvSync library
*/

/**
    \file avsync.c
    \brief A8 side function defenitions. Interface functions for Audio-Video sync component.
*/

#include <mcfw/src_linux/links/avsync/avsync_priv.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/systemLink_m3vpss.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/utils/Memory.h>
#include <mcfw/src_linux/osa/inc/osa_debug.h>
#include <pthread.h>
#include <sys/time.h>

#define ENABLE_AVSYNC_STATS_LOG

AVSYNC_AvSyncInfo*          pAvSyncInfo_obj; //AVSYNC_AvSyncInfo  instance ptr;
GateMP_Handle*              pAvsyncMPGate ;

pthread_t AvsynLog_ThrHandle;
static int avsyncLogThr_exit;

Int32 AVSYNC_LogThrCreate();
Int32 AVSYNC_LogThrDelete ();
Int32 AVSYNC_ReInit();


/*<   AvSync Shared Data Control functions  >*/


Int32 AVSYNC_GetSharedDataStructure()
{
    Int32 status = 0;
    SystemVpss_AvSyncSharedObjPtr avsyncSharedObjPtr;

    status = System_linkControl( SYSTEM_LINK_ID_M3VPSS,
                        SYSTEM_M3VPSS_CMD_GET_AVSYNC_SHAREDOBJ_PTR,
                        &avsyncSharedObjPtr,  sizeof(avsyncSharedObjPtr), TRUE );

    pAvSyncInfo_obj = Memory_translate((Ptr)avsyncSharedObjPtr.pAvSyncSharedObjPtr, Memory_XltFlags_Phys2Virt);

    return status;
}

Int32 AVSYNC_LockSharedDataStructure (  Int32* key)
{
    if (pAvsyncMPGate && key)
        key = GateMP_enter (*pAvsyncMPGate);

    return OSA_SOK;
}

Int32 AVSYNC_ReleaseSharedDataStructure (  Int32* key)
{
    if (pAvsyncMPGate && key)
        GateMP_leave (*pAvsyncMPGate, key);

    return OSA_SOK;
}

/*<    END - AvSync Shared Data Control functions  - END>*/

static Int64 AVSYNC_GetCurrentTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (((Int64)tv.tv_sec*1000000 + tv.tv_usec)/1000);
}

/*<   AvSync A8 main functions  >*/

Int32 AVSYNC_Init()
{
    Int32  status = 0;
    Int64  curTime;
    Int32* key = NULL;

    AVSYNC_AvSyncEnable_t AVSYNC_AvSyncEnable;

    status = AVSYNC_GetSharedDataStructure();

    status = GateMP_open (AVSYNC_GATE_MP_NAME, pAvsyncMPGate);

#ifdef  ENABLE_AVSYNC_STATS_LOG

    avsyncLogThr_exit = 0;
    status = AVSYNC_LogThrCreate();
        OSA_assert(status == OSA_SOK);
#endif

    AVSYNC_AvSyncEnable.avSyncCompEnable = TRUE;
    AVSYNC_Control(AVSync_ComponentEnable, &AVSYNC_AvSyncEnable );

    curTime = AVSYNC_GetCurrentTime();
    OSA_printf ("\n\n=============== AVSYNC_curTime = %lld ==================\n", curTime);

    AVSYNC_LockSharedDataStructure(key);

    pAvSyncInfo_obj->timeInfo.wallTimeInTicks = curTime;

    AVSYNC_ReleaseSharedDataStructure(key);

    return OSA_SOK;
}

Int32 AVSYNC_DeInit()
{
    Int32 status = 0;
    AVSYNC_AvSyncEnable_t AVSYNC_AvSyncEnable;

        AVSYNC_AvSyncEnable.avSyncCompEnable = FALSE;
    AVSYNC_Control(AVSync_ComponentEnable, &AVSYNC_AvSyncEnable );

#ifdef  ENABLE_AVSYNC_STATS_LOG
    avsyncLogThr_exit = 1;
            AVSYNC_LogThrDelete ();
#endif

    if(pAvSyncInfo_obj != NULL)
        {
        status = GateMP_close ( pAvsyncMPGate);
        pAvSyncInfo_obj = NULL;
    }

    return OSA_SOK;
}

Int32 AVSYNC_Control(UInt8 cmd,  Void *pPrm)
{
    Int32 i = 0;
    Int32* key = NULL;

    AVSYNC_LockSharedDataStructure(key);

    switch (cmd)
    {
        case    AVSync_StreamMode:
        {
            AVSYNC_STREAM_MODE  *tempPtr = (AVSYNC_STREAM_MODE *)pPrm;
            pAvSyncInfo_obj->streamMode = *tempPtr;

            break;
        }

        case    AVSync_ExecutionMode:
        {
            AVSYNC_EXECUTION_MODE   *tempPtr = (AVSYNC_EXECUTION_MODE *)pPrm;
            pAvSyncInfo_obj->execMode = *tempPtr;

            break;
        }

        case    AVSync_PlayRate:
        {
            AVSYNC_PlayRate_t*  tempPtr = (AVSYNC_PlayRate_t*)pPrm;
            pAvSyncInfo_obj->playRate[ tempPtr->strmId ].scaleM = tempPtr->scaleM;
            pAvSyncInfo_obj->playRate[ tempPtr->strmId ].scaleD = tempPtr->scaleD;
            pAvSyncInfo_obj->playRate[ tempPtr->strmId ].startFrameSeqNum = tempPtr->startFrameSeqNum;
            pAvSyncInfo_obj->playRate[ tempPtr->strmId ].strmId = tempPtr->strmId;

            break;
        }

        case    AVSync_TimeClockState:
        {
            AVSYNC_ClkState_t*  tempPtr = (AVSYNC_ClkState_t*)pPrm;
            pAvSyncInfo_obj->clkState.eState = tempPtr->eState;
            pAvSyncInfo_obj->clkState.nAudioWaitStrmIdMask = tempPtr->nAudioWaitStrmIdMask;
            pAvSyncInfo_obj->clkState.nVideoWaitStrmIdMask = tempPtr->nVideoWaitStrmIdMask;

            break;
        }

        case    AVSync_TimeActiveRefClock:
            {
            AVSYNC_RefClk_t*    tempPtr = (AVSYNC_RefClk_t*)pPrm;
            pAvSyncInfo_obj->timeInfo.activeRefClk.strmId = tempPtr->strmId;
            pAvSyncInfo_obj->timeInfo.activeRefClk.clkType = tempPtr->clkType;
            pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp = tempPtr->nTimeStamp;
            pAvSyncInfo_obj->timeInfo.activeRefClk.nOffsetToMediaTime = tempPtr->nOffsetToMediaTime;

            break;
        }

        case    AVSync_StreamActive:
        {
            AVSYNC_StrmInfo_t*  tempPtr = (AVSYNC_StrmInfo_t*)pPrm;
            pAvSyncInfo_obj->StrmInfo.nAudioStrmActiveMask = tempPtr->nAudioStrmActiveMask;
            pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask = tempPtr->nVideoStrmActiveMask;

            break;
        }
        break;

        case    AVSync_ClientStartTime:
        {
            AVSYNC_StartPTS_t*  tempPtr = (AVSYNC_StartPTS_t*)pPrm;
            if (tempPtr->clkType == AVSYNC_TIME_ClkTypeVideo)
                pAvSyncInfo_obj->clientVideoStartTimes[ tempPtr->strmId ] = tempPtr->nTimeStamp;
            else if (tempPtr->clkType == AVSYNC_TIME_ClkTypeAudio)
                pAvSyncInfo_obj->clientAudioStartTimes[ tempPtr->strmId ] = tempPtr->nTimeStamp;

            break;
        }

        case    AVSync_BackEndDelay:
        {
            AVSYNC_BackendDelay_t*  tempPtr = (AVSYNC_BackendDelay_t*)pPrm;
            if (tempPtr->clkType == AVSYNC_TIME_ClkTypeVideo)
                pAvSyncInfo_obj->videoBackendDelay = tempPtr->nDelay;
            else if (tempPtr->clkType == AVSYNC_TIME_ClkTypeAudio)
                pAvSyncInfo_obj->audioBackendDelay = tempPtr->nDelay;

            break;
        }

        case    AVSync_MaxLeadLagTime:
        {
            AVSYNC_LeadLag_t*   tempPtr = (AVSYNC_LeadLag_t*)pPrm;
            pAvSyncInfo_obj->LeadLagRange.audioLag = tempPtr->audioLag;
            pAvSyncInfo_obj->LeadLagRange.audioLead= tempPtr->audioLead;
            pAvSyncInfo_obj->LeadLagRange.videoLag = tempPtr->videoLag;
            pAvSyncInfo_obj->LeadLagRange.videoLead= tempPtr->videoLead;

            break;
        }

        case    AVSync_ChannelEnable:
        {
            AVSYNC_Enable_t*    tempPtr = (AVSYNC_Enable_t*)pPrm;

            pAvSyncInfo_obj->DisplayEnable.nActiveDisplayFlag[tempPtr->Display]
                                                    |= (1 <<  tempPtr->channel);

            break;
        }

        case    AVSync_ChannelDisable:
        {
            AVSYNC_Enable_t*    tempPtr = (AVSYNC_Enable_t*)pPrm;

            pAvSyncInfo_obj->DisplayEnable.nActiveDisplayFlag[tempPtr->Display]
                                                    &= ~( 1 <<  tempPtr->channel);

            break;
        }

        case    AVSync_ComponentEnable:
        {
            AVSYNC_AvSyncEnable_t*  tempPtr = (AVSYNC_AvSyncEnable_t*)pPrm;
            pAvSyncInfo_obj->avSyncCompEnable = tempPtr->avSyncCompEnable; // use 0 or 1 as possible

            AVSYNC_ReInit();

            if (tempPtr->avSyncCompEnable != 0) {
                for (i = 0; i < AVSYNC_MAX_DISPLAYS; i++) {
                    pAvSyncInfo_obj->DisplayEnable.nActiveDisplayFlag[i] = 0xFFFFFFFF;
                }
            }
            else {
                for (i = 0; i < AVSYNC_MAX_DISPLAYS; i++) {
                    pAvSyncInfo_obj->DisplayEnable.nActiveDisplayFlag[i] = 0x00000000;
                }
            }

            break;
        }
        case    AVSync_PrintStats:
        {
            AVSYNCPrintInfo();
            break;
        }

        default:
            OSA_printf(" *** Invalid command was called *** \n");
    }
    AVSYNC_ReleaseSharedDataStructure(key);

    return OSA_SOK;
}

/*<    END - AvSync A8 main functions  - END>*/

Int32 AVSYNC_ReInit()
{
    Int32 i;

    for (i = 0; i < AVSYNC_MAX_STREAMS_SUPPORTED; i++) {
        pAvSyncInfo_obj->playRate[ i ].scaleM = 1;
        pAvSyncInfo_obj->playRate[ i ].scaleD = 1;
        pAvSyncInfo_obj->playRate[ i ].startFrameSeqNum = 0;
        pAvSyncInfo_obj->playRate[ i ].strmId = i;

        pAvSyncInfo_obj->clientVideoStartTimes[ i ] = 0;
        pAvSyncInfo_obj->clientAudioStartTimes[ i ] = 0;
        pAvSyncInfo_obj->firstVidFrameRcvd[i] = FALSE;
    }

    pAvSyncInfo_obj->clkState.nAudioWaitStrmIdMask = 0;
    pAvSyncInfo_obj->clkState.nVideoWaitStrmIdMask = 0;

    pAvSyncInfo_obj->timeInfo.activeRefClk.strmId = 0;
    pAvSyncInfo_obj->timeInfo.activeRefClk.clkType = AVSYNC_TIME_ClkTypeVideo; //TIME_ClkTypeAudio;
    pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp = 0;
    pAvSyncInfo_obj->timeInfo.activeRefClk.nOffsetToMediaTime = 0;

    pAvSyncInfo_obj->StrmInfo.nAudioStrmActiveMask = 0xFFFFFFFF;
    pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask = 0xFFFFFFFF;

    for (i = 0; i < AVSYNC_MAX_STREAMS_SUPPORTED; i++) {
        pAvSyncInfo_obj->timeInfo.mediaTimeBase[i] = AVSYNC_INVALID_TIMESTAMP; //AVSYNC_INVALID_TIMESTAMP;
        pAvSyncInfo_obj->timeInfo.mediaTimeRunning[i] = 0;
        pAvSyncInfo_obj->timeInfo.wallTimeBase[i] = 0;
    }

    pAvSyncInfo_obj->timeInfo.mediaTimeInTicks = 0;
    pAvSyncInfo_obj->timeInfo.mediaTime = 0;
    pAvSyncInfo_obj->timeInfo.lastMediaTime = 0;
    pAvSyncInfo_obj->timeInfo.wallTimeInTicks = 0;
    pAvSyncInfo_obj->timeInfo.lastWallTimeInTicks = 0;

    memset(&pAvSyncInfo_obj->StreamStats, 0, sizeof(AVSYNC_StreamStats_t));

    return OSA_SOK;

}


/*<  AvSync Logic functions   >*/

Int32  AVSYNC_Audio_ISR()
{
#if 0
    void AVSYNC_AudioISR (AVSYNC_INFO           *pAVSyncInfo)
    {
        U32                 delta;
        U8                  i;
        U8                  mediaTimeUpdated = 0;
#ifdef  LOG_AVSYNC
        U8  str;
        U16 idx;
#endif
        static int log_cnt = 0;

#ifdef  LOG_AVSYNC
        log_index++;
        if (log_index >= MAX_STRINGS)
            log_index = 0;

        str = log_string[log_index];
        sprintf (str, "%lld] AudioISR ", Clock_getTicks() );
        idx = strlen (str);
        str =  log_string[log_index] + (idx - 1);
#endif

        if (pAVSyncInfo->clockState.eState == TIME_ClockStateRunning)
        {
            S64     mediaTimeInc;
            U64     oldMediaTime, audioRefTime;

            /* timeInfo.mediaTime & RefClk might have started with different start times
             * For skew correction to timeInfo.mediaTime <if required>, compute the difference between RefClk & timeInfo.mediaTime
             * and add that to timeInfo.mediaTime
             */
            if (pAVSyncInfo->timeInfo.lastWallTimeInTicks > pAVSyncInfo->timeInfo.wallTimeInTicks)
                pAVSyncInfo->stat.wallClkRollOverCnt++;



            mediaTimeInc = (pAVSyncInfo->timeInfo.wallTimeInTicks - pAVSyncInfo->timeInfo.lastWallTimeInTicks);
            pAVSyncInfo->timeInfo.mediaTimeInTicks += mediaTimeInc;
            mediaTimeInc = (mediaTimeInc / WALL_TIMER_FREQ_MS);


#ifdef  LOG_AVSYNC
            sprintf (str,  "OrgWall %lld OrgLastWall %lld OrgMedia %lld OrgInc %lld ",
                                pAVSyncInfo->timeInfo.wallTimeInTicks,
                                pAVSyncInfo->timeInfo.lastWallTimeInTicks,
                                pAVSyncInfo->timeInfo.mediaTime,
                                mediaTimeInc);
            idx = strlen (str);
            str =  log_string[log_index] + (idx - 1);
#endif

            // TODO:  Add some intelligence to check this audio PTS is valid  */

#ifdef  ENABLE_AUDIO_REFCLK_CORRECTION
            if (pAVSyncInfo->timeInfo.mediaTimeBase[pAVSyncInfo->timeInfo.activeRefClk.strmId] != AVSYNC_INVALID_TIMESTAMP
                && pAVSyncInfo->timeInfo.activeRefClk.nTimeStamp != AVSYNC_INVALID_TIMESTAMP)
            {
                log_cnt++;

                /* Check Ref Audio stream has started really & do skew correction */
                if (pAVSyncInfo->timeInfo.activeRefClk.clkType == TIME_ClkTypeAudio)
                {
                    if ( (pAVSyncInfo->streamInfo.nAudioStrmActiveMask & (1 << pAVSyncInfo->timeInfo.activeRefClk.strmId))
                            && (pAVSyncInfo->stat.streamStats[pAVSyncInfo->timeInfo.activeRefClk.strmId].nAudFramesRendered > 0))
                    {

                        trigger_log = 1;
    //                wait__();

    //                wait__();

                        ref_clk_var[ref_clk_debug_idx].orgWallTime = pAVSyncInfo->timeInfo.wallTimeInTicks;
                        ref_clk_var[ref_clk_debug_idx].orgLastWallTime = pAVSyncInfo->timeInfo.lastWallTimeInTicks;
                        ref_clk_var[ref_clk_debug_idx].orgMediaTime = pAVSyncInfo->timeInfo.mediaTime;
                        ref_clk_var[ref_clk_debug_idx].orgMediaInc = mediaTimeInc;
                        ref_clk_var[ref_clk_debug_idx].newMediaInc = 0;
                        ref_clk_var[ref_clk_debug_idx].newMediaTime = 0;
                        ref_clk_var[ref_clk_debug_idx].newWallTime = 0;
                        ref_clk_var[ref_clk_debug_idx].refClk = pAVSyncInfo->timeInfo.activeRefClk.nTimeStamp;
                        ref_clk_var[ref_clk_debug_idx].refClkOffset = pAVSyncInfo->timeInfo.activeRefClk.nOffsetToMediaTime;
#ifdef __ti__
                        ref_clk_var[ref_clk_debug_idx].currentTimeStamp = Clock_getTicks();
#else
                        ref_clk_var[ref_clk_debug_idx].currentTimeStamp = pAVSyncInfo->timeInfo.currentTimeStamp;
#endif

                        oldMediaTime = pAVSyncInfo->timeInfo.mediaTime;

                        audioRefTime = pAVSyncInfo->timeInfo.activeRefClk.nTimeStamp + pAVSyncInfo->timeInfo.activeRefClk.nOffsetToMediaTime;

                        delta = (pAVSyncInfo->timeInfo.mediaTime + mediaTimeInc) - audioRefTime + pAVSyncInfo->audioBackendDelay;
                        if (delta > pAVSyncInfo->maxAudioLagTime)
                        {
                            // Update WallClock
                            logWallTimeResetInfo(pAVSyncInfo->timeInfo.wallTimeInTicks,
                                            audioRefTime * (WALL_TIMER_FREQ_MS),
                                            mediaTimeInc,
                                            (pAVSyncInfo->timeInfo.mediaTime - oldMediaTime));
                            pAVSyncInfo->timeInfo.wallTimeInTicks   = audioRefTime * (WALL_TIMER_FREQ_MS);
                            pAVSyncInfo->timeInfo.mediaTime             = audioRefTime;
                            mediaTimeInc                        = (pAVSyncInfo->timeInfo.mediaTime - oldMediaTime);

                            // Important. After refclk correction, reset refclk to INVALID_TIMESTAMP.
                            // Required - If refclk <audio> update at audio rendering end doesnt at regular intervals, mediastamp will get reset to a older
                            // refClk value several times within the next refclk update period at audio renderer.
                            pAVSyncInfo->timeInfo.activeRefClk.nTimeStamp = AVSYNC_INVALID_TIMESTAMP;

                            ref_clk_var[ref_clk_debug_idx].newMediaInc = mediaTimeInc;
                            ref_clk_var[ref_clk_debug_idx].newMediaTime = pAVSyncInfo->timeInfo.mediaTime;
                            ref_clk_var[ref_clk_debug_idx].newWallTime = pAVSyncInfo->timeInfo.wallTimeInTicks;
#ifdef  LOG_AVSYNC
                            sprintf (str, "delta %lld newInc %lld refClk %lld refClkOffset %lld newMedia %lld newWallTime %lld\n",
                                delta,
                                mediaTimeInc,
                                pAVSyncInfo->timeInfo.activeRefClk.nTimeStamp,
                                pAVSyncInfo->timeInfo.activeRefClk.nOffsetToMediaTime,
                                pAVSyncInfo->timeInfo.mediaTime,
                                pAVSyncInfo->timeInfo.wallTimeInTicks
                                 );
                            idx = strlen (str);
                            str =  log_string[log_index] + (idx - 1);
#endif
                            pAVSyncInfo->stat.wallClkResetCnt++;
                            mediaTimeUpdated = 1;
                        }
                        if ((delta < 0) && (delta < -(pAVSyncInfo->maxAudioLeadTime)))
                        {
                            // Update WallClock
                            logWallTimeResetInfo(pAVSyncInfo->timeInfo.wallTimeInTicks,
                                            audioRefTime * (WALL_TIMER_FREQ_MS),
                                            mediaTimeInc,
                                            (pAVSyncInfo->timeInfo.mediaTime - oldMediaTime));
                            pAVSyncInfo->timeInfo.wallTimeInTicks   = audioRefTime * (WALL_TIMER_FREQ_MS);
                            pAVSyncInfo->timeInfo.mediaTime             = audioRefTime;
                            mediaTimeInc                        = (pAVSyncInfo->timeInfo.mediaTime - oldMediaTime);

                            // Important. After refclk correction, reset refclk to INVALID_TIMESTAMP.
                            // Required - If refclk <audio> update at audio rendering end doesnt at regular intervals, mediastamp will get reset to a older
                            // refClk value several times within the next refclk update period at audio renderer.
                            pAVSyncInfo->timeInfo.activeRefClk.nTimeStamp = AVSYNC_INVALID_TIMESTAMP;

                            ref_clk_var[ref_clk_debug_idx].newMediaInc = mediaTimeInc;
                            ref_clk_var[ref_clk_debug_idx].newMediaTime = pAVSyncInfo->timeInfo.mediaTime;
                            ref_clk_var[ref_clk_debug_idx].newWallTime = pAVSyncInfo->timeInfo.wallTimeInTicks;
                            pAVSyncInfo->stat.wallClkResetCnt++;
                            mediaTimeUpdated = 1;

#ifdef  LOG_AVSYNC
                            sprintf (str, "delta %lld newInc %lld refClk %lld newMedia %lld newWallTime %lld\n",
                                delta,
                                mediaTimeInc,
                                audioRefTime,
                                pAVSyncInfo->timeInfo.mediaTime,
                                pAVSyncInfo->timeInfo.wallTimeInTicks
                                 );
                            idx = strlen (str);
                            str =  log_string[log_index] + (idx - 1);
#endif

                        }
                    }
                }
            }
#endif
            if (mediaTimeUpdated == 0)
                    pAVSyncInfo->timeInfo.mediaTime += mediaTimeInc;

            /* Update all running counters */
            for (i=0; i<AVSYNC_MAX_STREAMS_SUPPORTED; i++)
            {
                if (pAVSyncInfo->timeInfo.mediaTimeBase[i] != AVSYNC_INVALID_TIMESTAMP)
                {
                    if ( pAVSyncInfo->timeInfo.mediaTimeRunning[i] == 0)
                         pAVSyncInfo->timeInfo.wallTimeBase[i] = pAVSyncInfo->timeInfo.wallTimeInTicks;

                    pAVSyncInfo->timeInfo.mediaTimeRunning[i] += mediaTimeInc;
                }
                }
            pAVSyncInfo->timeInfo.lastWallTimeInTicks = pAVSyncInfo->timeInfo.wallTimeInTicks;
            }

        if (log_cnt % 30)
        {
            ref_clk_debug_idx++;
            if (ref_clk_debug_idx >= MAX_STORAGE)
                ref_clk_debug_idx = 0;
        }
    }
#endif

    return OSA_SOK;
}
/*<    END - AvSync Logic functions - END>*/




/*<  AvSync Logs >*/

void printStats (AVSYNC_StreamChStats_t *pStat)
{
    OSA_printf ("VIDEO ==> Dq [%6d] Ren [%6d] Rep [%6d], Skip [%6d], Inv [%6d], LstPTS %10lld\n",
                                    pStat->nVidFramesDequeued,
                                    pStat->nVidFramesRendered,
                                    pStat->nVidFramesRepeated,
                                    pStat->nVidFramesSkipped,
                                    pStat->nVidFramesInvalid,
                                    pStat->lastVidPTS
                                    );
    /*OSA_printf ("AUDIO ==> Dq [%6d] Ren [%6d] Rep [%6d], Skip [%6d], Inv [%6d], LstPTS %10d\n",
                                    pStat->nAudFramesDequeued,
                                    pStat->nAudFramesRendered,
                                    pStat->nAudFramesRepeated,
                                    pStat->nAudFramesSkipped,
                                    pStat->nAudFramesInvalid,
                                    pStat->lastAudPTS
                                    );*/

}

void AVSYNCPrintInfo()
{
    Int16 i;
    OSA_printf ("=============== AVSYNC INFO ==================\n");
    OSA_printf ("ExecMode %d streamMode %d\n", pAvSyncInfo_obj->execMode, pAvSyncInfo_obj->streamMode);
    OSA_printf ("Clock- State %d Masks - Aud %X, Vid %X \n", pAvSyncInfo_obj->clkState.eState, pAvSyncInfo_obj->clkState.nVideoWaitStrmIdMask, pAvSyncInfo_obj->clkState.nAudioWaitStrmIdMask);
    OSA_printf ("WallTime - Ticks %lld\n", pAvSyncInfo_obj->timeInfo.wallTimeInTicks);
    OSA_printf ("timeInfo.mediaTime - Ticks %lld, msecs %lld\n", pAvSyncInfo_obj->timeInfo.mediaTimeInTicks, pAvSyncInfo_obj->timeInfo.mediaTime);
    OSA_printf ("Backend Delays - Vid %d Aud %d\n",  pAvSyncInfo_obj->videoBackendDelay, pAvSyncInfo_obj->audioBackendDelay);
    OSA_printf ("Lead times - Vid %d Aud %d\n", pAvSyncInfo_obj->LeadLagRange.videoLead, pAvSyncInfo_obj->LeadLagRange.audioLead);
    OSA_printf ("Lag times - Vid %d Aud %d\n", pAvSyncInfo_obj->LeadLagRange.videoLag, pAvSyncInfo_obj->LeadLagRange.audioLag);

    OSA_printf ("WALL resetCnt - %d, rollOverCnt - %d\n", pAvSyncInfo_obj->StreamStats.wallClkResetCnt, pAvSyncInfo_obj->StreamStats.wallClkRollOverCnt);
    OSA_printf ("Active Masks - Vid %X Aud %X\n", pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask, pAvSyncInfo_obj->StrmInfo.nAudioStrmActiveMask);

    for (i=0; i<AVSYNC_MAX_STREAMS_SUPPORTED; i++)
    {
        if (pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask & (1 << i))
        {
            OSA_printf ("------------ ch %d----------\n", i);
            OSA_printf ("StartTimes - Vid %d, Aud %d\n", pAvSyncInfo_obj->clientVideoStartTimes[i], pAvSyncInfo_obj->clientAudioStartTimes[i]);
            OSA_printf ("VidFrmRcvd %s AudFrmRcvd %s\n",
                            pAvSyncInfo_obj->firstVidFrameRcvd[i] != 0 ? "TRUE" : "FALSE",
                            pAvSyncInfo_obj->firstAudFrameRcvd[i] != 0 ? "TRUE" : "FALSE"
                         );
            OSA_printf ("timeInfo.mediaTime - Base %lld, Running %lld\n", pAvSyncInfo_obj->timeInfo.mediaTimeBase[i], pAvSyncInfo_obj->timeInfo.mediaTimeRunning[i]);
            OSA_printf ("Rate %d Needed startSeqNum %d, Rcvd startSeqNum %d\n",
                    pAvSyncInfo_obj->playRate[i].scaleM /pAvSyncInfo_obj->playRate[i].scaleD,
                         pAvSyncInfo_obj->playRate[i].startFrameSeqNum, pAvSyncInfo_obj->StreamStats.StreamChStats[i].nVidLastStartSeqNum);
            printStats(&(pAvSyncInfo_obj->StreamStats.StreamChStats[i]));
        }
    }
    OSA_printf ("=============== AVSYNCPrintInfo exit ==================\n");
}


void AVSYNC_LogPerFrame()
{
    UInt32 i;

    OSA_printf ("\n\n=============== AVSYNC_LogPerFrame ==================\n");

    for (i=0; i<AVSYNC_MAX_STREAMS_SUPPORTED; i++)
    {
        if (pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask & (1 << i))
        {
            OSA_printf ("ch [ %d ] =>",i);
            printStats(&(pAvSyncInfo_obj->StreamStats.StreamChStats[i]));
        }
    }
}

void* AVSYNC_LogThrRoutine(void* tmp)
{
    while(avsyncLogThr_exit == 0) {
 //       AVSYNC_LogPerFrame();
        // AVSYNC_Control(AVSync_PrintStats, 0);        // testing API
        sleep(5);
    }

    return 0;
}


/*<  END -  AvSync Logs - END  >*/


Int32 AVSYNC_LogThrCreate()
{
    Int32 status = OSA_SOK;
    Uint32 pri = 18;
    Uint32 stackSize = 2048;
    pthread_attr_t thread_attr;
    struct sched_param schedprm;

    // initialize thread attributes structure
    status = pthread_attr_init(&thread_attr);

    if(status!=OSA_SOK)
    {
        OSA_ERROR("avsyncLog_thrCreate() - Could not initialize thread attributes\n");
        return status;
    }

    if(stackSize != 0)
      pthread_attr_setstacksize(&thread_attr, stackSize);

    status |= pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
    status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);

    if(pri>sched_get_priority_max(SCHED_FIFO))
        pri=sched_get_priority_max(SCHED_FIFO);
    else if(pri<sched_get_priority_min(SCHED_FIFO))
        pri=sched_get_priority_min(SCHED_FIFO);

    schedprm.sched_priority = pri;
    status |= pthread_attr_setschedparam(&thread_attr, &schedprm);

    if(status!=OSA_SOK)
    {
        OSA_ERROR("avsyncLog_thrCreate() - Could not initialize thread attributes\n");
        goto error_exit;
    }

    status = pthread_create(&AvsynLog_ThrHandle, &thread_attr, AVSYNC_LogThrRoutine, 0);

    if(status!=OSA_SOK)
    {
        OSA_ERROR("avsyncLog_thrCreate() - Could not create thread [%d]\n", status);
    }

error_exit:
   pthread_attr_destroy(&thread_attr);

   return status;
}

Int32 AVSYNC_LogThrDelete ()
{
    Int32 status=OSA_SOK;
    void* retVal;

    if (pthread_join(AvsynLog_ThrHandle, &retVal))
    {
        OSA_ERROR("avsyncLog_thrDelete() - handle is NULL\n");
    }
    return status;
}

