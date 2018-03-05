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
    \file avsync.h
    \brief main data structures
*/

#ifndef _AVSYNC_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _AVSYNC_H_

#define OSA_SOK      0  ///< Status : OK
#define OSA_EFAIL   -1  ///< Status : Generic error

/* AVSync specific defines known to all modules */
#define AVSYNC_MAX_STREAMS_SUPPORTED            32      /**<  Max streams supported by a single avsync component */
#define AVSYNC_MAX_DISPLAYS 2
#define AVSYNC_GATE_MP_NAME "AVSYNC_INFO_GATE_MP"

#define AVSYNC_INVALID_TIMESTAMP    0xFFFFFFFFUL        /* Invalid timestamp -  no operations will be done, used during state changes */

#define AVSYNC_MAX_VIDEO_LEAD_TIME          5000
#define AVSYNC_MAX_VIDEO_LAG_TIME           (3 * 33)
#define AVSYNC_MAX_AUDIO_LEAD_TIME          (2 * 33)
#define AVSYNC_MAX_AUDIO_LAG_TIME           125

#define AVSYNC_COMP_ENABLE  FALSE


/* signed quantities */
typedef long long int Int_64;               ///< Signed 64-bit integer
typedef unsigned long long int UInt_64;      ///< Unsigned 64-bit integer

typedef enum
{
     AVSync_StreamMode = 101,           /**< reference: AVSYNC_STREAM_MODE */
     AVSync_ExecutionMode,          /**< reference: AVSYNC_EXECUTION_MODE */
     AVSync_PlayRate,               /**< reference: AVSYNC_PLAY_RATE */
     AVSync_GetNextFrameInfo,       /**< reference: AVSYNC_NEXT_FRAME_INFO */
     AVSync_TimeClockState,         /**< reference: AVSYNC_CLOCK_STATE */
     AVSync_StreamActive,           /**< reference: AVSYNC_STREAM_INFO */
     AVSync_TimeActiveRefClock,     /**< reference: AVSYNC_REF_CLOCK */
     AVSync_RefMediaTime,           /**< reference: AVSYNC_TIME */
     AVSync_StreamMediaTime,            /**< reference: AVSYNC_TIME */
     AVSync_CurrentWallTime,            /**< reference: AVSYNC_TIME */
     AVSync_ClientStartTime,            /**< reference: AVSYNC_START_TIME */
     AVSync_BackEndDelay,           /**< reference: AVSYNC_BACKEND_TIME */
     AVSync_MaxLeadLagTime,             /**< reference: AVSYNC_MaxLeadLagTime */
     AVSync_ComponentEnable,                /**< reference: AVSYNC_component enable */
     AVSync_ChannelEnable,              /**< reference: AVSYNC_ChannelEnable */
     AVSync_ChannelDisable,             /**< reference: AVSYNC_ChannelDisable*/
     AVSync_PrintStats,                     /**< reference: AVSYNC_PRINTSTATS */
     AVSYNC_IndexParamAVSYNCEnabledPort,                /**< reference: AVSYNC_PARAM_U32TYPE */
}AVSYNC_CONFIG_TYPES;

typedef enum
{
    AVSYNC_OneStreamMode,                   /**< Set this flag if the usecase is just for 1 stream */
    AVSYNC_NStreamMode,             /**<  Set this flag to operate in multi Stream Mode <limited to MAX_STREAMS_SUPPORTED> */
    /* Assign a value using 4 bytes to have common enum size in all processors */
    AVSYNC_StreamMode_LAST = 0x7FFFFFFF
}AVSYNC_STREAM_MODE;

typedef enum
{
    AVSYNC_ExecutionModeTimeSynced,          /**<  Used to enable scheduling of frames according to their PTS */
    AVSYNC_ExecutionModeFreeRun,             /**<  Used to switch off avsync logic */
    AVSYNC_ExecutionMode_LAST = 0x7FFFFFFF
}AVSYNC_EXECUTION_MODE;

typedef enum
{
    AVSYNC_TIME_ClkTypeAudio = 0,
    AVSYNC_TIME_ClkTypeVideo = 1,            /**<  limitation --> MAX_STREAM_TYPES_USED_IN_AVSYNC */
    AVSYNC_ClkType_LAST = 0x7FFFFFFF
}AVSYNC_REFCLK_TYPE;

/** Enumeration of clock states. */
typedef enum
{
      AVSYNC_TIME_ClockStateRunning,              /**< Clock running. */
      AVSYNC_TIME_ClockStateWaitingForStartTime, /**< Clock waiting until the
                                               *   prescribed clients emit their  start time.
                                               * AVSYNC_TIME_ClockStateStopped,  Clock stopped.*/
      AVSYNC_TIME_ClockStateKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
      AVSYNC_TIME_ClockStateVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
      AVSYNC_TIME_ClockStateMax = 0x7FFFFFFF
} AVSYNC_TIME_CLOCKSTATE;


/**<  === AVSync Video / Audio Frame Logic return values. AVSync Frame Q / RingIO should interpret & act as per these flags  === */
typedef enum
{
    AVSYNC_DropFrame,               /**<  lag scenario or flush scenario during state transitions. frame buffer should be recycled */
    AVSYNC_RepeatFrame,         /**<  lead scenario */
    AVSYNC_RenderFrame,         /**<  pts & media time match within limits */
    AVSYNC_RenderFrame_LAST = 0x7FFFFFFF
} AVSYNC_FRAME_RENDER_FLAG;


//< AvSync App IF structures ;  >//


typedef struct  AVSYNC_AvSyncEnable_t
{
    UInt32 avSyncCompEnable;                /* to enable / disable the avsync component  */
}AVSYNC_AvSyncEnable_t ;

typedef struct AVSYNC_Enable_t
{
     UInt Display;       /* limit : 0 to Max_Displays*/
     UInt channel;       /* limit : 0 to Max_Channelse for one display*/
}AVSYNC_Enable_t ;


 typedef struct AVSYNC_ClkState_t
{
    AVSYNC_TIME_CLOCKSTATE  eState;                         /**<  AVSync clock can be in Stopped or Running or WaitingForStartTime state */
    UInt32                  nAudioWaitStrmIdMask;       /**<  A bit set to 1 says that clock will wait till that strmId's first audio PTS is reported to AVSync */
    UInt32                  nVideoWaitStrmIdMask;       /**<  A bit set to 1 says that clock will wait till that strmId's first video PTS is reported to AVSync */
}AVSYNC_ClkState_t ;

 typedef struct AVSYNC_StrmInfo_t
{
     UInt32                 nAudioStrmActiveMask;       /**<  A bit set to 1 says that audio stream is available for this strmId */
     UInt32                 nVideoStrmActiveMask;       /**<  A bit set to 1 says that video stream is available for this strmId */
}AVSYNC_StrmInfo_t ;

 typedef struct AVSYNC_RefClk_t
{
    UInt8                   strmId;                         /**<  stream identifier - Input parameter */
    AVSYNC_REFCLK_TYPE  clkType;            /**<  Specifies if this timestamp is for audio or video - Input parameter */
    UInt_64             nTimeStamp;         /**<  Ref clock - running counter */
    Int_64              nOffsetToMediaTime; /**<  This is the start offset of RefClk wrt mediaTime when RefClk got changed */
}AVSYNC_RefClk_t ;

/* Config parameter set via AVSyncTimeScale */
 typedef struct AVSYNC_PlayRate_t
{
    Int32               scaleM;             /**<  Scaling factor applicable to each stream. Multiplier part >*/
    Int32               scaleD;             /**<  Scaling factor applicable to each stream. Divider part >*/
    UInt16                  startFrameSeqNum;               /**<  Seq Num scheme used to discard wrong frames during fast play rate transitions */
    UInt16                  strmId;                         /**<  stream identifier */
}AVSYNC_PlayRate_t ;

/* Config parameter got via AVSyncGeNextFrameInfo */
 typedef struct AVSYNC_NextFrameInfo_t
{
    UInt16                  startFrameSeqNum;               /**<  AVSync generates this seq num on TimeScale & sends for use by other components */
    UInt8                   strmId;                         /**<  Stream Identifier */
    UInt_64                 lastVidPTS;                     /**<  pts of last rendered video frame by avsync */
    UInt_64                 lastAudPTS;                     /**<  pts of last rendered audio frame by avsync */
}AVSYNC_NextFrameInfo_t ;

/* Config parameter for AVSyncClientStartTime */
 typedef struct AVSYNC_StartPTS_t
{
    UInt_64                 nTimeStamp;                     /**<  TimeStamp */
    UInt8                   strmId;                         /**<  Stream Identifier */
    AVSYNC_REFCLK_TYPE  clkType;                            /**<  Specifies if this timestamp is for audio or video */
}AVSYNC_StartPTS_t ;

/* Config parameter for AVSyncBackEndDelay, AVSyncMaxLeadTime, AVSyncMaxLagTime */
 typedef struct AVSYNC_BackendDelay_t
{
    Int16                   nDelay;                         /**<  Delay or lag / lead time in millisecs */
    AVSYNC_REFCLK_TYPE  clkType;            /**<  Specifies if this timestamp is for audio or video */
}AVSYNC_BackendDelay_t ;

typedef struct AVSYNC_LeadLag_t
{
   Int16                        audioLead;                   /**<  Delay or lag / lead time in millisecs */
   Int16                        audioLag;
   Int16                        videoLead;
   Int16                        videoLag;
}AVSYNC_LeadLag_t ;

//< End - AvSync App IF structures  - End  >//


 typedef struct AVSYNC_DisplayEnable_t
{
    UInt32                  nActiveDisplayFlag[AVSYNC_MAX_DISPLAYS];    /* to enable / disable the particular display */
}AVSYNC_DisplayEnable_t ;


typedef struct  AVSYNC_StreamChStats_t
{

        UInt32      nVidFramesRendered;                                     /*  Rendered after applying avsync logic */
        UInt32      nAudFramesRendered;

        UInt32      nVidFramesSkipped;                                       /* Skipped as per avsync logic */
        UInt32      nAudFramesSkipped;

        UInt32      nVidFramesRepeated;
        UInt32      nAudFramesRepeated;                                     /* Repeated as per avsync logic */

        UInt32      nVidFramesInvalid;                                       /* Dropped without applying avsync logic during state transitions - invalid frames */
        UInt32      nAudFramesInvalid;

        UInt_64     lastVidPTSDequeued;                                     /* PTS values / mediaTime of frames recently dequeued */

        UInt_64     lastVidPTS;                                                 /* PTS values / mediaTime of frames recently sent to renderer */
        UInt_64     lastAudPTS;
        UInt_64     firstVidPTS;
        UInt_64     firstAudPTS;
        UInt_64     lastmediaTimeRunning;

        UInt32      nVidFramesDequeued;                                     /* Actual frames dequeued */
        UInt32      nAudFramesDequeued;

        UInt16      nVidLastStartSeqNum;                                    /* Last startSeqNum received */
        UInt16      nAudLastStartSeqNum;

}AVSYNC_StreamChStats_t;

typedef struct  AVSYNC_StreamStats_t
{
    AVSYNC_StreamChStats_t StreamChStats[AVSYNC_MAX_STREAMS_SUPPORTED];
    UInt32                                      wallClkRollOverCnt;
    UInt32                                      wallClkResetCnt;

}AVSYNC_StreamStats_t;

 typedef struct AVSYNC_TimeInfo_t
{
/* Word 0 */        UInt_64                         magicNum;                   /**< Magic number to indicate initialization done */

        /* mediaTimeBase of each stream which is AVSYNC_START_TIME.nTimeStamp reported via AVSyncClientStartTime */
        /* Stream mediaTime = mediaTimeBase + mediaTime * scaleM / scaleD */

/* Word 2 */        UInt_64                         mediaTimeBase[AVSYNC_MAX_STREAMS_SUPPORTED];    /* Base time - starting time */
/* Word 34 */   UInt_64                         mediaTimeRunning[AVSYNC_MAX_STREAMS_SUPPORTED]; /* Running Time */
/* Word 66 */   UInt_64                         wallTimeBase[AVSYNC_MAX_STREAMS_SUPPORTED];         /* wallTime when stream mediaTime was started - for debug */

/* Word 98 */   UInt_64                         mediaTimeInTicks;           /* mediaTime getting incremented from 0 */
/* Word 100 */  UInt_64                         mediaTime;                  /* mediaTime in milliseconds  */
/* Word 102 */  UInt_64                         lastMediaTime;              /* Previous as seen by Audio or Video ISR */

/* Internal info */
/* Word 104 */  UInt_64                         wallTimeInTicks;                /* Ticks as per H/W Timer */
/* Word 106 */  UInt_64                         lastWallTimeInTicks;             /* Previous as seen by Audio or Video ISR */

                /* For use by audio recording possible running in other processor. Video & audio stores in timestamps in same units */
                /* This is updated with same mechanism as FVID2 timestamp <which is the actual video timestamp */
                /* This requires avsync to be active even during recording */
/* Word 108 */  UInt_64                         currentTimeStamp;

/* Word 110 */  AVSYNC_RefClk_t                 activeRefClk;
/* Word 116 */  UInt32                          reserved[12];

}                               AVSYNC_TimeInfo_t;

 //< AvSync Info Global structures ;  >//

typedef struct  AVSYNC_AvSyncInfo_t
{
    AVSYNC_TimeInfo_t                   timeInfo;

    AVSYNC_PlayRate_t                   playRate[AVSYNC_MAX_STREAMS_SUPPORTED];         /* Streams play rate & expected seq number */
    AVSYNC_ClkState_t                   clkState ;
    AVSYNC_StrmInfo_t                   StrmInfo ;
    AVSYNC_DisplayEnable_t              DisplayEnable;

    UInt32                              playAudio;
    AVSYNC_STREAM_MODE                  streamMode;                                 /**< 1 or N stream mode */
    AVSYNC_EXECUTION_MODE               execMode;                                   /* Time Synced or free run mode */

    Int16                               audioBackendDelay;                          /* Possible post process delays before rendering */
    Int16                               videoBackendDelay;                          /* Possible post process delays before rendering */
    AVSYNC_LeadLag_t                    LeadLagRange;                               /* Max audio, video lead & lag  time Range*/

    UInt32                              clientVideoStartTimes[AVSYNC_MAX_STREAMS_SUPPORTED];    /* Start times <first PTS> of the streams */
    UInt32                              clientAudioStartTimes[AVSYNC_MAX_STREAMS_SUPPORTED];

    UInt32                              firstVidFrameRcvd   [AVSYNC_MAX_STREAMS_SUPPORTED];     /* Flag to signal that first frame after a play state transition is rcvd */
    UInt32                              firstAudFrameRcvd   [AVSYNC_MAX_STREAMS_SUPPORTED];     /* Flag to signal that first frame after a play state transition is rcvd */

    UInt8                               avSyncCompEnable;                       /* enable /disable avsync component  */
    AVSYNC_StreamStats_t                StreamStats;                                    /* Statistical info of rendering, frame numbers,...*/
} AVSYNC_AvSyncInfo;

 //< End - AvSync Info Global structures - End  >//


Int32 AVSYNC_Init();
Int32 AVSYNC_DeInit();
Int32 AVSYNC_Control(UInt8 cmd,  Void *pPrm);

Int32 AVSYNC_M3_Init();
Int32 AVSYNC_M3_DeInit();


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
