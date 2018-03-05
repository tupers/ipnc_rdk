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
    \file avsync_m3vpss.h
    \brief m3 side structures & functions
*/

#ifndef _AVSYNC_M3VPSS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _AVSYNC_M3VPSS_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>      // for  FVID2_Frame
#include <mcfw/interfaces/link_api/avsync.h>

#define AVSYNC_SR_OBJ_HEAP_SIZE 	(2*1024*1024)

/* Temp - Hack !!!!*/
#define	AVSYNC_WALL_TIMER_FREQ_MS			(1)		/* Each tick corresponds to 1 millisecond */

/* ==================== BUFFERHEADERTYPE.nFlags interpretation ==========
    =============*/
/* Bit0 ~ Bit15 ==> BUFFERFLAG_STARTTIME / BUFFERFLAG_DECODEONLY /
BUFFERFLAG_DATACORRUPT / BUFFERFLAG_ENDOFFRAME /
						BUFFERFLAG_SYNCFRAME / BUFFERFLAG_EXTRADATA /
BUFFERFLAG_CODECCONFIG */
/* Bit16 ~ Bit31 ==> startFrameSeqNum which gets incremented for every first
frame after a play state transition. Initial value of 0 is invalid */

#define AVSYNC_BUFFERFLAG_STARTTIME 0x00000002

#define	AVSYNC_SEQ_NUM(flag)		(flag >> 16)

/* Enable this to perform video refclk correction */
// #define	ENABLE_VIDEO_REFCLK_CORRECTION

#define AVSYNC_MAX_STORAGE 100

//#define NO_PTS_CHECK_FOR_REFCLK_STREAM

// mapping display number from actual display id


#define AVSYNC_MAP_SC_INS_ID_2_SC_NUM(ID) ( ID - SYSTEM_LINK_ID_SW_MS_MULTI_INST_0 )


// debug structure for walltime clock
typedef	struct	AVSYNC_wall_info
{
    UInt_64                     wallTime[AVSYNC_MAX_STORAGE];
    UInt_64                     count[AVSYNC_MAX_STORAGE];
    Uint32                      period;
    Uint16                      index;
}AVSYNC_wall_info;

typedef struct  AVSYNC_sync_info
{
    UInt_64                     mediaTime[AVSYNC_MAX_STORAGE];
    UInt_64                     PTS[AVSYNC_MAX_STORAGE];
    Int_64                      delta[AVSYNC_MAX_STORAGE];
    AVSYNC_FRAME_RENDER_FLAG    flag[AVSYNC_MAX_STORAGE];
    Uint16                      index;
}AVSYNC_sync_info;

Int32   AVSYNC_GetSharedDataStructure(SystemVpss_AvSyncSharedObjPtr *avsyncSharedObjPtr );
Int32   AVSYNC_LockSharedDataStructure();
Int32   AVSYNC_ReleaseSharedDataStructure();

Int32	Video_ISR();
AVSYNC_FRAME_RENDER_FLAG VideoSyncFn (UInt_64 videoPTS, UInt32 strmId, UInt32  nBufFlags);

// AvSyncInfo Instance
extern AVSYNC_AvSyncInfo* pAvSyncInfo_obj; //AvSyncInfo  instance ptr;

// Frame render control - supporting objects.
extern FVID2_Frame* Frame_Dequeued[AVSYNC_MAX_DISPLAYS][AVSYNC_MAX_STREAMS_SUPPORTED];

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
