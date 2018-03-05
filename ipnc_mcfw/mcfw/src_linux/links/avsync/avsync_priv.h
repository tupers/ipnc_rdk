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
    \file avsync_priv.h
    \brief A8 side structures & functions
*/

#ifndef _AVSYNC_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _AVSYNC_PRIV_H_

#include <mcfw/interfaces/ti_media_std.h>
#include <mcfw/interfaces/ti_media_error_def.h>
#include <mcfw/interfaces/link_api/avsync.h>

//#define	ENABLE_AUDIO_REFCLK_CORRECTION

Int32 AVSYNC_Audio_ISR();

Int32 AVSYNC_LockSharedDataStructure();
Int32 AVSYNC_ReleaseSharedDataStructure();
void  AVSYNCPrintInfo();


extern AVSYNC_AvSyncInfo* 			pAvSyncInfo_obj; //AvSyncInfo  instance ptr;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */

