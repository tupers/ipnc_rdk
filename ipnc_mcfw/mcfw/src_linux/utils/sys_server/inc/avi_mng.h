/* ===========================================================================
* @path $(IPNCPATH)\sys_server\inc\
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file avi_mng.h
* @brief AVI manager.
* @defgroup AVI_MNG AVI manager

* This module takes care of recording AVI file and ask file dispatcher to send
*  the AVI file when recording is done.
* @{
*/
#ifndef __AVI_MNG_H__
#define __AVI_MNG_H__
#include <Msg_Def.h>

#define AVI_PATH "/tmp"

#define AVI_MODE1		(0) 	///< Save AVI files to /tmp and separate by duration.
#define AVI_MODE2		(1) 	///< Save AVI files to SD card until full.
#define AVI_MODE3		(2) 	///< Save AVI files to SD card and separate by duration.

#define AVI_RUN			(1)
#define AVI_THR_EXIT 	(1<<1)
#define AVI_NO_STOP 	(1<<2)
#define TMP_IN_SD		(1<<3)
#define AVI_TO_SD		(1<<4) 	///< Save AVI files to SD card.
#define AVI_TO_SMTP		(1<<5) 	///< Save AVI files to SMTP server.
#define AVI_TO_FTP		(1<<6) 	///< Save AVI files to FTP server.
#define AVI_FOR_DMVA	(1<<7) 

#define AVI_INT_STATE	(0)
#define AVI_DST_MASK 	(AVI_TO_SD | AVI_TO_SMTP | AVI_TO_FTP)
#define AVI_CFG_MASK 	(TMP_IN_SD | AVI_DST_MASK | AVI_FOR_DMVA)
#define AVI_EXT_SEC 	(10)
#define AVI_RUN_AGAIN	(0x6300)

int AviMngInit();
int AviMngExit();
int AviRun(int nMode, int nDstFlg);
int AviStop();
#endif
/**@}
 */
