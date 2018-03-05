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
    \file avsync_m3vpss.c
    \brief m3 side function defenitions.
*/

#include <xdc/std.h>
#include <mcfw/src_bios6/links_m3vpss/avsync/avsync_m3vpss.h>

/* For Wall Timer */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Timer.h>

/* IPC */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/GateMP.h>


/* link stack */
#pragma DATA_ALIGN(gAvsyncLog_tskStack, 32)
#pragma DATA_SECTION(gAvsyncLog_tskStack, ".bss:taskStackSection")
UInt8 gAvsyncLog_tskStack[2024];


AVSYNC_AvSyncInfo * 	pAvSyncInfo_obj;				// AvSync_App_IF Instance
Clock_Handle	clk_Handle_walltime;
Utils_TskHndl	tskHandle_log;
GateMP_Handle 	avsyncMPGate ;
FVID2_Frame* Frame_Dequeued[AVSYNC_MAX_DISPLAYS][AVSYNC_MAX_STREAMS_SUPPORTED];		// Frame render control - supporting objects.

/* Wall timer */
#define	AVSYNC_USE_TIMESTAMP_MODULE

#ifdef	AVSYNC_USE_TIMESTAMP_MODULE
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

#define UTILS_FREQPERMILLISEC_DIV_FACTOR                       (1 * 1000)

UInt_64	freqInMillisec;
#endif

UInt_64	wallTimeOrgArr[AVSYNC_MAX_STORAGE];
UInt_64	wallTimeResetArr[AVSYNC_MAX_STORAGE];
UInt_64	mediaTimeIncOrgArr[AVSYNC_MAX_STORAGE];
UInt_64	mediaTimeIncResetArr[AVSYNC_MAX_STORAGE];
Uint8	wallTimeResetIndex = 0;

UInt_64	videoISRTime[AVSYNC_MAX_STORAGE];
Uint8	videoISRIndex = 0;

void AVSYNC_logWallTimeResetInfo(UInt_64 wallTimeOrg, UInt_64 wallTimeReset, UInt_64 mediaTimeIncOrg, UInt_64 mediaTimeIncReset)
{
	wallTimeOrgArr[wallTimeResetIndex] = wallTimeOrg;
	wallTimeResetArr[wallTimeResetIndex] = wallTimeReset;
	mediaTimeIncOrgArr[wallTimeResetIndex] = mediaTimeIncOrg;
	mediaTimeIncResetArr[wallTimeResetIndex] = mediaTimeIncReset;

	wallTimeResetIndex++;

	if (wallTimeResetIndex >= AVSYNC_MAX_STORAGE)
		wallTimeResetIndex = 0;
}


/* Log */
/* If syncing to a video or audio, just send those stream's frames to display without PTS check */
// #define		NO_PTS_CHECK_FOR_REFCLK_STREAM

// #define		LOG_AVSYNC

#ifdef	LOG_AVSYNC
#define	MAX_STRINGS				200

static UInt8 log_string [MAX_STRINGS][400];
static UInt16 log_index = -1;
#endif

AVSYNC_wall_info	debug_info;
AVSYNC_sync_info  	debug_sync_info[AVSYNC_MAX_STREAMS_SUPPORTED];

Uint8 trigger_log = 1;

void AVSYNC_logSyncInfo(Uint8 strmId, AVSYNC_FRAME_RENDER_FLAG flag)
{
	debug_sync_info[strmId].flag[debug_sync_info[strmId].index] = flag;

	if (trigger_log)
	{
		debug_sync_info[strmId].index ++;
		if (debug_sync_info[strmId].index >= AVSYNC_MAX_STORAGE)
			debug_sync_info[strmId].index = 0;
}
}


Void AVSYNCPrintDataStructure ();

static UInt_64  AVSYNC_GetMediaTime (Uint8 strmId)
{
    /* AVSync logic to be applied only when a valid timestamp is available */
	if (pAvSyncInfo_obj->timeInfo.mediaTimeBase[strmId] != AVSYNC_INVALID_TIMESTAMP)
		return (pAvSyncInfo_obj->timeInfo.mediaTimeBase[strmId] +
                     (pAvSyncInfo_obj->timeInfo.mediaTimeRunning[strmId] *
                                 (pAvSyncInfo_obj->playRate[strmId].scaleM/pAvSyncInfo_obj->playRate[strmId].scaleD ) ));
    else
        return (AVSYNC_INVALID_TIMESTAMP);
}


/* implementations */
Int32 InitSharedDataStructure()
{
    Int32 status = 0, i;

	pAvSyncInfo_obj->streamMode = AVSYNC_NStreamMode;
	pAvSyncInfo_obj->execMode = AVSYNC_ExecutionModeTimeSynced; //AVSYNC_ExecutionModeFreeRun;
	clk_Handle_walltime = NULL;
	pAvSyncInfo_obj->playAudio = FALSE;
	pAvSyncInfo_obj->avSyncCompEnable = FALSE;

    for (i = 0; i < AVSYNC_MAX_STREAMS_SUPPORTED; i++) {
		pAvSyncInfo_obj->playRate[ i ].scaleM = 1;
		pAvSyncInfo_obj->playRate[ i ].scaleD = 1;
		pAvSyncInfo_obj->playRate[ i ].startFrameSeqNum = 0;
		pAvSyncInfo_obj->playRate[ i ].strmId = i;

		pAvSyncInfo_obj->clientVideoStartTimes[ i ] = 0;
		pAvSyncInfo_obj->clientAudioStartTimes[ i ] = 0;
		pAvSyncInfo_obj->firstVidFrameRcvd[i] = FALSE;
	}

	pAvSyncInfo_obj->clkState.eState = AVSYNC_TIME_ClockStateRunning; //TIME_ClockStateStopped;
	pAvSyncInfo_obj->clkState.nAudioWaitStrmIdMask = 0;
	pAvSyncInfo_obj->clkState.nVideoWaitStrmIdMask = 0;

	pAvSyncInfo_obj->timeInfo.activeRefClk.strmId = 0;
	pAvSyncInfo_obj->timeInfo.activeRefClk.clkType = AVSYNC_TIME_ClkTypeVideo; //TIME_ClkTypeAudio;
	pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp = 0;
	pAvSyncInfo_obj->timeInfo.activeRefClk.nOffsetToMediaTime = 0;

	pAvSyncInfo_obj->StrmInfo.nAudioStrmActiveMask = 0xFFFFFFFF;
	pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask = 0xFFFFFFFF;

    for (i = 0; i < AVSYNC_MAX_DISPLAYS; i++) {
		pAvSyncInfo_obj->DisplayEnable.nActiveDisplayFlag[i] = 0xFFFFFFFF; // for 32ch avsync support // 0x00030000;
    }

	pAvSyncInfo_obj->videoBackendDelay = 0;//(1000/30);
	pAvSyncInfo_obj->audioBackendDelay = 0;

	pAvSyncInfo_obj->LeadLagRange.audioLag = AVSYNC_MAX_AUDIO_LAG_TIME;
	pAvSyncInfo_obj->LeadLagRange.audioLead= AVSYNC_MAX_AUDIO_LEAD_TIME;
	pAvSyncInfo_obj->LeadLagRange.videoLag = AVSYNC_MAX_VIDEO_LAG_TIME;
	pAvSyncInfo_obj->LeadLagRange.videoLead= AVSYNC_MAX_VIDEO_LEAD_TIME;

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
	memset(Frame_Dequeued, 0, sizeof(FVID2_Frame*) * AVSYNC_MAX_DISPLAYS * AVSYNC_MAX_STREAMS_SUPPORTED);

	avsyncMPGate = NULL;

    return status;
}

/*<    Shared data control functions    >*/
Int32 AVSYNC_AllocSharedDataStructure()
{
    Int32           status = 0;
    IHeap_Handle    srHeap;
    GateMP_Params   gateMPParams;
    UInt32          gateMPSize;

    srHeap = SharedRegion_getHeap(SYSTEM_IPC_SR_NON_CACHED_DEFAULT);
    UTILS_assert(srHeap != NULL);

	pAvSyncInfo_obj = Memory_alloc(srHeap, sizeof(AVSYNC_AvSyncInfo), 0, NULL);
	UTILS_assert(pAvSyncInfo_obj != NULL);

    InitSharedDataStructure();

    GateMP_Params_init (&gateMPParams);
    gateMPParams.remoteProtect  = GateMP_RemoteProtect_SYSTEM;
    gateMPParams.localProtect   = GateMP_LocalProtect_INTERRUPT;
    gateMPSize                  = GateMP_sharedMemReq(&gateMPParams);
    gateMPParams.sharedAddr     = Memory_alloc( SharedRegion_getHeap(SYSTEM_IPC_SR_NON_CACHED_DEFAULT),
                                    gateMPSize, 128,  NULL);
    if (gateMPParams.sharedAddr == NULL)
    {
        Vps_printf("%s - %d AVSync GateMP struct allocation failed.\n", __FUNCTION__ , __LINE__);
			pAvSyncInfo_obj = NULL;

        return FALSE;
    }
    gateMPParams.name = AVSYNC_GATE_MP_NAME;

	avsyncMPGate = GateMP_create (&gateMPParams);

	if (avsyncMPGate == NULL)
    {
        Vps_printf("%s - %d AVSync GateMP creation failed\n", __FUNCTION__ , __LINE__);
        return FALSE;
    }

    return status;
}

Int32 AVSYNC_DeallocSharedDataStructure()
{
    Int32 status= 0;
    IHeap_Handle srHeap;

    srHeap = SharedRegion_getHeap(SYSTEM_IPC_SR_NON_CACHED_DEFAULT);
    UTILS_assert(srHeap != NULL);

    Memory_free(srHeap, pAvSyncInfo_obj, sizeof(AVSYNC_AvSyncInfo));
	GateMP_delete (&avsyncMPGate);

    return status;
}

Int32 AVSYNC_GetSharedDataStructure(SystemVpss_AvSyncSharedObjPtr *avsyncSharedObjPtr )
{
    Int32 status = 0;
	avsyncSharedObjPtr->pAvSyncSharedObjPtr = (void*) pAvSyncInfo_obj;

    return status;
}
Int32 AVSYNC_LockSharedDataStructure (  IArg key)
{
	if (avsyncMPGate && key)
		key = GateMP_enter (avsyncMPGate);

    return 0;
}

Int32 AVSYNC_ReleaseSharedDataStructure (  IArg key)
{
	if (avsyncMPGate && key)
	    GateMP_leave (avsyncMPGate, key);

    return 0;
}

/*<    END - Shared data control functions  - END>*/


/*<     Wall time functions  >*/
Int32 AVSYNC_Walltime_update()
{
    Int32 status = 0;
    UInt_64                 ticks;
	IArg					key = NULL;
    static  UInt_64         last_ticks = 0;

	if ( pAvSyncInfo_obj->avSyncCompEnable == TRUE) {

#ifdef  AVSYNC_USE_TIMESTAMP_MODULE
    Types_Timestamp64   ts;
    Timestamp_get64(&ts);
    ticks = ts.hi;
    ticks <<= 32;
    ticks |= ts.lo;

		ticks /= freqInMillisec;
		status = AVSYNC_LockSharedDataStructure( key);
		pAvSyncInfo_obj->timeInfo.wallTimeInTicks += (ticks - last_ticks);
		pAvSyncInfo_obj->timeInfo.currentTimeStamp = ticks;	// For use by audio recording
		last_ticks = ticks;
#else
    ticks = Clock_getTicks();
		status = AVSYNC_LockSharedDataStructure( key);
		pAvSyncInfo_obj->timeInfo.wallTimeInTicks += (ticks - last_ticks);
		//pAvSyncInfo_obj->timeInfo.currentTimeStamp = Clock_getTicks();	// For use by audio recording
		last_ticks = ticks;
#endif
		status &= AVSYNC_ReleaseSharedDataStructure( key);

		debug_info.wallTime[debug_info.index] = pAvSyncInfo_obj->timeInfo.wallTimeInTicks;		// For Debug purpose
		debug_info.index ++;
		if (debug_info.index >= AVSYNC_MAX_STORAGE)
				debug_info.index = 0;
    }

    return status;
}

Int32 AVSYNC_Init_WallTimer()
{
    Int32 status = 0;
    Clock_Params clockParams;

    Vps_printf(" %d : AVsync AVSYNC_Init_WallTimer********************** !!! \r\n", Clock_getTicks());

#ifdef  AVSYNC_USE_TIMESTAMP_MODULE
    Types_FreqHz freq;
#endif
	memset(debug_sync_info, 0, sizeof(debug_sync_info));
	debug_info.index = 0;

#ifdef  AVSYNC_USE_TIMESTAMP_MODULE
    Timestamp_getFreq(&freq);
	freqInMillisec = freq.hi;
	freqInMillisec <<= 32;
	freqInMillisec |= freq.lo;
	freqInMillisec /= UTILS_FREQPERMILLISEC_DIV_FACTOR;
#endif

    Clock_Params_init(&clockParams);
    clockParams.period = 1;             /* Each tick corresponds to 1 millisecond - refer tickPeriod in bios cfg file */
    clockParams.startFlag = TRUE;       /* start immediately */
    clockParams.arg = 0;

	clk_Handle_walltime = Clock_create((Clock_FuncPtr)AVSYNC_Walltime_update, 4, &clockParams, NULL);

    return status;
}

/*<    END - Wall time functions  - END>*/

Int32 AVSYNC_Create_LogTask()
{
	Int32 status = 0;
	Vps_printf("AVSYNC_Create_LogTask********************** !!! \r\n");

    status = Utils_tskCreate(&tskHandle_log,
                             AVSYNCPrintDataStructure,
                             8,
                             gAvsyncLog_tskStack,
                             2024, 0, "AVSYNC_LOG ");
    UTILS_assert(status == 0);

	return status;
}

/*<   AvSync M3 main functions  >*/

Int32 AVSYNC_M3_Init()
{
    Int32 status = 0;
	Vps_printf(" AVsync AVSYNC_M3_Init********************** !!! \r\n");

    AVSYNC_AllocSharedDataStructure();
	GateMP_open (AVSYNC_GATE_MP_NAME, &avsyncMPGate);
    AVSYNC_Init_WallTimer();
	//AVSYNC_Create_LogTask();

    return status;
}

Int32 AVSYNC_M3_DeInit()
{
    Int32 status = 0;
	Vps_printf(" AVsync AVSYNC_M3_DeInit********************** !!! \r\n");

	Clock_delete(&clk_Handle_walltime);

    /* Delete link task */
 //   Utils_tskDelete( &tskHandle_log);

	GateMP_close(&avsyncMPGate);
    AVSYNC_DeallocSharedDataStructure();

    return status;
}

/*<  END - AvSync M3 functions - END  >*/


/*<  AvSync Logic functions   >*/

Int32 Video_ISR()
{
	Int32 	status = 0;
    IArg    key = NULL;

	Uint8	i;
	Uint8	mediaTimeUpdated = 0;

    status = AVSYNC_LockSharedDataStructure( key);


	if (pAvSyncInfo_obj->clkState.eState == AVSYNC_TIME_ClockStateRunning)
    {
		Int_64 	mediaTimeInc;

        /* timeInfo.mediaTime & RefClk might have started with different start times
         * For skew correction to timeInfo.mediaTime <if required>, compute the difference between RefClk & timeInfo.mediaTime
         * and add that to timeInfo.mediaTime
         */
		if (pAvSyncInfo_obj->timeInfo.lastWallTimeInTicks > pAvSyncInfo_obj->timeInfo.wallTimeInTicks)
			pAvSyncInfo_obj->StreamStats.wallClkRollOverCnt++;

		mediaTimeInc = (pAvSyncInfo_obj->timeInfo.wallTimeInTicks - pAvSyncInfo_obj->timeInfo.lastWallTimeInTicks);
		pAvSyncInfo_obj->timeInfo.mediaTimeInTicks += mediaTimeInc;
		mediaTimeInc = (mediaTimeInc / AVSYNC_WALL_TIMER_FREQ_MS);

#ifdef	ENABLE_VIDEO_REFCLK_CORRECTION
		UInt_64 	oldMediaTime, videoRefTime;
		Int32	delta;

		if (pAvSyncInfo_obj->timeInfo.mediaTimeBase[pAvSyncInfo_obj->timeInfo.activeRefClk.strmId] != AVSYNC_INVALID_TIMESTAMP
			&& pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp != AVSYNC_INVALID_TIMESTAMP)
		{
			/* Check Ref Video stream has started really & do skew correction */
			if (pAvSyncInfo_obj->timeInfo.activeRefClk.clkType == AVSYNC_TIME_ClkTypeVideo)
			{
				if ( (pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask & (1 << pAvSyncInfo_obj->timeInfo.activeRefClk.strmId))
						&& (pAvSyncInfo_obj->StreamStats.StreamChStats[pAvSyncInfo_obj->timeInfo.activeRefClk.strmId].nVidFramesRendered > 0))
				{
					oldMediaTime = pAvSyncInfo_obj->timeInfo.mediaTime;

					videoRefTime = pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp + pAvSyncInfo_obj->timeInfo.activeRefClk.nOffsetToMediaTime;
					delta = (pAvSyncInfo_obj->timeInfo.mediaTime + mediaTimeInc) - (videoRefTime + pAvSyncInfo_obj->videoBackendDelay);
					if (delta > pAvSyncInfo_obj->LeadLagRange.videoLag)
                    {
                        // Update WallClock
						AVSYNC_logWallTimeResetInfo(pAvSyncInfo_obj->timeInfo.wallTimeInTicks,
										videoRefTime * (AVSYNC_WALL_TIMER_FREQ_MS),
                                        mediaTimeInc,
										(pAvSyncInfo_obj->timeInfo.mediaTime - oldMediaTime));
						pAvSyncInfo_obj->timeInfo.wallTimeInTicks	= videoRefTime * (AVSYNC_WALL_TIMER_FREQ_MS);
						pAvSyncInfo_obj->timeInfo.mediaTime 		= videoRefTime;
						mediaTimeInc						= (pAvSyncInfo_obj->timeInfo.mediaTime - oldMediaTime);
						pAvSyncInfo_obj->StreamStats.wallClkResetCnt++;
						mediaTimeUpdated = 1;
					}
					if ((delta < 0) && (delta < -(pAvSyncInfo_obj->LeadLagRange.videoLead)))
					{
						// Update WallClock
						logWallTimeResetInfo(pAvSyncInfo_obj->timeInfo.wallTimeInTicks,
										videoRefTime * (AVSYNC_WALL_TIMER_FREQ_MS),
										mediaTimeInc,
										(pAvSyncInfo_obj->timeInfo.mediaTime - oldMediaTime));
						pAvSyncInfo_obj->timeInfo.wallTimeInTicks	= videoRefTime * (AVSYNC_WALL_TIMER_FREQ_MS);
						pAvSyncInfo_obj->timeInfo.mediaTime 			= videoRefTime;
						mediaTimeInc						= (pAvSyncInfo_obj->timeInfo.mediaTime - oldMediaTime);
						pAvSyncInfo_obj->StreamStats.wallClkResetCnt++;
						mediaTimeUpdated = 1;
					}
                    }
            }
        }
#endif
		if (mediaTimeUpdated == 0)
			pAvSyncInfo_obj->timeInfo.mediaTime += mediaTimeInc;

        /* Update all running counters */
        for (i=0; i<AVSYNC_MAX_STREAMS_SUPPORTED; i++)
        {
			if (pAvSyncInfo_obj->timeInfo.mediaTimeBase[i] != AVSYNC_INVALID_TIMESTAMP)
            {
				if ( pAvSyncInfo_obj->timeInfo.mediaTimeRunning[i] == 0)
					 pAvSyncInfo_obj->timeInfo.wallTimeBase[i] = pAvSyncInfo_obj->timeInfo.wallTimeInTicks;

				pAvSyncInfo_obj->timeInfo.mediaTimeRunning[i] += mediaTimeInc;
            }
        }
		pAvSyncInfo_obj->timeInfo.lastWallTimeInTicks = pAvSyncInfo_obj->timeInfo.wallTimeInTicks;
    }

    status &= AVSYNC_ReleaseSharedDataStructure( key);

    return status;
}


AVSYNC_FRAME_RENDER_FLAG VideoSyncFn (UInt_64 videoPTS, UInt32 strmId, UInt32  nBufFlags)
{
    UInt_64     absoluteMediaTime;
    Int32       delta;

#ifdef	LOG_AVSYNC
	Uint8	*str;
	Uint16 	idx;

	log_index++;
	if (log_index >= MAX_STRINGS)
		log_index = 0;
	str = log_string[log_index];

	sprintf (str, "%lld] VideoSyncFn ", Clock_getTicks() );
	idx = strlen (str);
	str =  log_string[log_index] + (idx - 1);
#endif
	if(pAvSyncInfo_obj == NULL)
	return AVSYNC_DropFrame;
	if (pAvSyncInfo_obj->streamMode == AVSYNC_OneStreamMode)
        strmId = 0;

	if (pAvSyncInfo_obj)
    {

        /* Stream not active anymore - flush remaining frames */
		if ((pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask & (1 << strmId)) == 0)
        {
            AVSYNC_logSyncInfo(strmId, AVSYNC_DropFrame);
            return AVSYNC_DropFrame;
        }

		if (pAvSyncInfo_obj->execMode == AVSYNC_ExecutionModeTimeSynced
				&& pAvSyncInfo_obj->clkState.eState == AVSYNC_TIME_ClockStateRunning)
        {
            /* Check for first frame to arrive for a new sequence when play mode changes */
			if (pAvSyncInfo_obj->firstVidFrameRcvd[strmId] == FALSE)
            {
                if(nBufFlags & AVSYNC_BUFFERFLAG_STARTTIME)
                {
					pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].nVidLastStartSeqNum = AVSYNC_SEQ_NUM(nBufFlags);
					//if (AVSYNC_SEQ_NUM(nBufFlags) == pAvSyncInfo_obj->playRate[strmId].startFrameSeqNum)
                    {
						pAvSyncInfo_obj->firstVidFrameRcvd[strmId] = TRUE;

						pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].firstVidPTS = videoPTS;
                        /* Non synced start for a stream joining later - IL Client would have set this earlier, still take care here also */
						if (pAvSyncInfo_obj->timeInfo.mediaTimeBase[strmId] == AVSYNC_INVALID_TIMESTAMP)
                        {
							pAvSyncInfo_obj->timeInfo.mediaTimeBase[strmId] = videoPTS;
                        }
                    }
                }   /* The valid & correct first frame after a play state transition has not come yet */
                else {
					pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].nVidFramesInvalid++;
                    AVSYNC_logSyncInfo(strmId, AVSYNC_DropFrame);
                    return AVSYNC_DropFrame;
                }
            }

            absoluteMediaTime = AVSYNC_GetMediaTime( strmId);

			delta = absoluteMediaTime - videoPTS + pAvSyncInfo_obj->videoBackendDelay;

			debug_sync_info[strmId].mediaTime[debug_sync_info[strmId].index] = absoluteMediaTime;
			debug_sync_info[strmId].PTS[debug_sync_info[strmId].index] = videoPTS;
			debug_sync_info[strmId].delta[debug_sync_info[strmId].index] = delta;

#ifdef	LOG_AVSYNC
			sprintf (str, "vpts %lld mediaTime %lld delta %lld", videoPTS, absoluteMediaTime, delta);
			idx = strlen (str);
			str =  log_string[log_index] + (idx - 1);
#endif

            /* If RefClk stream; just send to display without wait -- Is this the correct logic ??? */
#ifdef	NO_PTS_CHECK_FOR_REFCLK_STREAM

			if (pAvSyncInfo_obj->timeInfo.activeRefClk.clkType == AVSYNC_TIME_ClkTypeVideo
					&& pAvSyncInfo_obj->timeInfo.activeRefClk.strmId == strmId)
            {
                        /* Initialize refClk for the first time */
				if ( pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp == AVSYNC_INVALID_TIMESTAMP )
                {
					pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp = pAvSyncInfo_obj->timeInfo.mediaTimeRunning[strmId];

                    /* Since refClk is starting now, calculate the difference between timeInfo.mediaTime & RefClk start */
					pAvSyncInfo_obj->timeInfo.activeRefClk.nOffsetToMediaTime = pAvSyncInfo_obj->timeInfo.mediaTime - pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp;
                }

				//pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].nVidFramesRendered++;
				//pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].lastVidPTS = videoPTS;

				pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp = pAvSyncInfo_obj->timeInfo.mediaTimeRunning[strmId];
				AVSYNC_logSyncInfo(strmId, AVSYNC_RenderFrame);
#ifdef	LOG_AVSYNC
				sprintf (str, " refclk freerun - RENDER\n");
				idx = strlen (str);
				str =  log_string[log_index] + (idx - 1);
#endif

				return AVSYNC_RenderFrame;
            }
#endif

			if (((delta <= pAvSyncInfo_obj->LeadLagRange.videoLag) && (delta > -pAvSyncInfo_obj->LeadLagRange.videoLead))
                    /*  || numEntries == 1 */ )
            {
                /* Video within displayable limit, hence render */
				//pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].nVidFramesRendered++;
				//pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].lastVidPTS = videoPTS;

#ifndef	NO_PTS_CHECK_FOR_REFCLK_STREAM
				/* Initialize refClk for the first time */
				if (pAvSyncInfo_obj->timeInfo.activeRefClk.clkType == AVSYNC_TIME_ClkTypeVideo
						&& pAvSyncInfo_obj->timeInfo.activeRefClk.strmId == strmId
						&& pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp == AVSYNC_INVALID_TIMESTAMP)
				{
					pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp = pAvSyncInfo_obj->timeInfo.mediaTimeRunning[strmId];

					/* Since refClk is starting now, calculate the difference between timeInfo.mediaTime & RefClk start */
					pAvSyncInfo_obj->timeInfo.activeRefClk.nOffsetToMediaTime = pAvSyncInfo_obj->timeInfo.mediaTime - pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp;
				}

				if (pAvSyncInfo_obj->timeInfo.activeRefClk.strmId == strmId && pAvSyncInfo_obj->timeInfo.activeRefClk.clkType == AVSYNC_TIME_ClkTypeVideo)
					pAvSyncInfo_obj->timeInfo.activeRefClk.nTimeStamp = pAvSyncInfo_obj->timeInfo.mediaTimeRunning[strmId];
#endif
                AVSYNC_logSyncInfo(strmId, AVSYNC_RenderFrame);
#ifdef	LOG_AVSYNC
				sprintf (str,  " - RENDER\n");
				idx = strlen (str);
				str =  log_string[log_index] + (idx - 1);
#endif

                return AVSYNC_RenderFrame;
            }
			else if ((delta < 0) && (delta > -pAvSyncInfo_obj->LeadLagRange.videoLead))
            {           // say delta is -1 ~ -4999 (where leadTime is 5000), still repeat and wait for displaying this frame in future

                /* Future frame, repeat frame to delay video & render this frame in next VSYNC */
				//pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].nVidFramesRepeated++;
                AVSYNC_logSyncInfo(strmId, AVSYNC_RepeatFrame);
#ifdef	LOG_AVSYNC
				sprintf (str, " - REPEAT\n");
				idx = strlen (str);
				str =  log_string[log_index] + (idx - 1);
#endif

                return AVSYNC_RepeatFrame;
            } // if not a repeat & say delta is (2*33) ~ -4999, then display
            else    // if delta is not between -4999 ~ (2*33), then video is either lagging well behind and leading far ahead. skip in both cases
            {
                /* Any frame beyond lead / lag limits should be discarded */
				//pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].nVidFramesSkipped++;
                AVSYNC_logSyncInfo(strmId, AVSYNC_DropFrame);
#ifdef	LOG_AVSYNC
				sprintf (str,  " - DROP\n");
				idx = strlen (str);
				str =  log_string[log_index] + (idx - 1);
#endif

                return AVSYNC_DropFrame;
            }
        }
		else if (pAvSyncInfo_obj->execMode == AVSYNC_ExecutionModeFreeRun)
        {
			pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].lastVidPTS = videoPTS;
            AVSYNC_logSyncInfo(strmId, AVSYNC_RenderFrame);
#ifdef	LOG_AVSYNC
			sprintf (str,  " - RENDER\n");
			idx = strlen (str);
			str =  log_string[log_index] + (idx - 1);
#endif

            return AVSYNC_RenderFrame;
        }
		pAvSyncInfo_obj->StreamStats.StreamChStats[strmId].nVidFramesInvalid++;	
    }
    AVSYNC_logSyncInfo(strmId, AVSYNC_DropFrame);
#ifdef	LOG_AVSYNC
	sprintf (str,  " - INVALID\n");
	idx = strlen (str);
	str =  log_string[log_index] + (idx - 1);
#endif

    return AVSYNC_DropFrame;
}

    
/*<  END -  AvSync Logic functions - END  >*/

/*<  AvSync Logs >*/

void printStats (AVSYNC_StreamChStats_t *pStat)
{
	Vps_printf ("VIDEO ==> Dq [%6d] Ren [%6d] Rep [%6d], Skip [%6d], Inv [%6d], LstPTS %10d\n",
									pStat->nVidFramesDequeued,
									pStat->nVidFramesRendered,
									pStat->nVidFramesRepeated,
									pStat->nVidFramesSkipped,
									pStat->nVidFramesInvalid,
									pStat->lastVidPTS
									);
	Vps_printf ("AUDIO ==> Dq [%6d] Ren [%6d] Rep [%6d], Skip [%6d], Inv [%6d], LstPTS %10d\n",
									pStat->nAudFramesDequeued,
									pStat->nAudFramesRendered,
									pStat->nAudFramesRepeated,
									pStat->nAudFramesSkipped,
									pStat->nAudFramesInvalid,
									pStat->lastAudPTS
									);

        }

void AVSYNCPrintDataStructure()
{
#if 1
	Int8 i;
	Vps_printf ("=============== AVSYNC INFO ==================\n");
	Vps_printf ("ExecMode %d streamMode %d\n", pAvSyncInfo_obj->execMode, pAvSyncInfo_obj->streamMode);
	Vps_printf ("Clock- State %d Masks - Aud %X, Vid %X \n", pAvSyncInfo_obj->clkState.eState, pAvSyncInfo_obj->clkState.nVideoWaitStrmIdMask, pAvSyncInfo_obj->clkState.nAudioWaitStrmIdMask);
	Vps_printf ("WallTime - Ticks %d\n", pAvSyncInfo_obj->timeInfo.wallTimeInTicks);
	Vps_printf ("timeInfo.mediaTime - Ticks %d, msecs %d\n", pAvSyncInfo_obj->timeInfo.mediaTimeInTicks, pAvSyncInfo_obj->timeInfo.mediaTime);
	Vps_printf ("Backend Delays - Vid %d Aud %d\n",  pAvSyncInfo_obj->videoBackendDelay, pAvSyncInfo_obj->audioBackendDelay);
	Vps_printf ("Lead times - Vid %d Aud %d\n", pAvSyncInfo_obj->LeadLagRange.videoLead, pAvSyncInfo_obj->LeadLagRange.audioLead);
	Vps_printf ("Lag times - Vid %d Aud %d\n", pAvSyncInfo_obj->LeadLagRange.videoLag, pAvSyncInfo_obj->LeadLagRange.audioLag);

	Vps_printf ("WALL resetCnt - %d, rollOverCnt - %d\n", pAvSyncInfo_obj->StreamStats.wallClkResetCnt, pAvSyncInfo_obj->StreamStats.wallClkRollOverCnt);
	Vps_printf ("Active Masks - Vid %X Aud %X\n", pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask, pAvSyncInfo_obj->StrmInfo.nAudioStrmActiveMask);

	for (i=0; i<AVSYNC_MAX_STREAMS_SUPPORTED; i++)
	{
		if (pAvSyncInfo_obj->StrmInfo.nVideoStrmActiveMask & (1 << i))
		{
			Vps_printf ("------------ ch %d----------\n", i);
			Vps_printf ("StartTimes - Vid %lld, Aud %lld\n", pAvSyncInfo_obj->clientVideoStartTimes[i], pAvSyncInfo_obj->clientAudioStartTimes[i]);
			Vps_printf ("VidFrmRcvd %s AudFrmRcvd %s\n",
							pAvSyncInfo_obj->firstVidFrameRcvd[i] != 0 ? "TRUE" : "FALSE",
							pAvSyncInfo_obj->firstAudFrameRcvd[i] != 0 ? "TRUE" : "FALSE"
						 );
			Vps_printf ("timeInfo.mediaTime - Base %lld, Running %lld\n", pAvSyncInfo_obj->timeInfo.mediaTimeBase[i], pAvSyncInfo_obj->timeInfo.mediaTimeRunning[i]);
			Vps_printf ("Rate %d Needed startSeqNum %d, Rcvd startSeqNum %d\n",
					(pAvSyncInfo_obj->playRate[i].scaleM/pAvSyncInfo_obj->playRate[i].scaleD),
                         pAvSyncInfo_obj->playRate[i].startFrameSeqNum,
                            pAvSyncInfo_obj->StreamStats.StreamChStats[i].nVidLastStartSeqNum);
			printStats(&(pAvSyncInfo_obj->StreamStats.StreamChStats[i]));
        }
        }
#endif
    }
    
/*<  END -  AvSync Logs - END  >*/


