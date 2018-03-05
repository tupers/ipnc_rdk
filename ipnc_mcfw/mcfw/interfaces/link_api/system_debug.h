/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup SYSTEM_LINK_API

    @{
*/

/**
    \file system_debug.h
    \brief System debug API

    Modify this file to enable/disable prints from different links.

    When SYSTEM_DEBUG_xxx_RT  is enabled a print is done for every frame.
    In real-time system such prints may slow down the system and hence
    are intended to used only for debug purposes.
*/

#ifndef _SYSTEM_DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_DEBUG_H_



#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#ifdef ENABLE_TILER
#define SYSTEM_USE_TILER
#else
#undef SYSTEM_USE_TILER
#endif
#else
#define SYSTEM_USE_TILER
#endif
/* Uncomment to Disable tiler at build time */
//#undef SYSTEM_USE_TILER

#define SYSTEM_USE_SENSOR

/* define this to use I2C from M3 side  */

/* Define SYSTEM_DISABLE_AUDIO set by build options */
#ifndef SYSTEM_DISABLE_AUDIO
#define SYSTEM_ENABLE_AUDIO
#endif

//#define SYSTEM_DEBUG_VIP_RES_ALLOC
//#define SYSTEM_DEBUG_TILER
//#define SYSTEM_DEBUG_CMD_ERROR

#define SYSTEM_DEBUG
#define SYSTEM_DEBUG_CAPTURE
#define SYSTEM_DEBUG_CAMERA
#define SYSTEM_DEBUG_NSF
#define SYSTEM_DEBUG_DEI
#define SYSTEM_DEBUG_DISPLAY
#define SYSTEM_DEBUG_DISPLAY_HWMS
#define SYSTEM_DEBUG_NULL
#define SYSTEM_DEBUG_DUP
#define SYSTEM_DEBUG_SWMS
#define SYSTEM_DEBUG_SCLR
#define SYSTEM_DEBUG_IPC
#define SYSTEM_DEBUG_MERGE
#define SYSTEM_DEBUG_ENC
#define SYSTEM_DEBUG_MJPEG
#define SYSTEM_DEBUG_DEC
#define SYSTEM_DEBUG_SCD
#define SYSTEM_DEBUG_SELECT
#define SYSTEM_DEBUG_FD
#define SYSTEM_DEBUG_SWOSD
#define SYSTEM_DEBUG_VNF
#define SYSTEM_DEBUG_ISP
#define SYSTEM_DEBUG_VA
#define SYSTEM_DEBUG_IMG
#define SYSTEM_DEBUG_VSTAB
#define SYSTEM_DEBUG_MUX
#define SYSTEM_DEBUG_GLBCE
#define SYSTEM_DEBUG_RVM

#define SYSTEM_DEBUG_MEM_PRINT
//#define SYSTEM_VERBOSE_PRINTS
//#define SYSTEM_DEBUG_RT

#ifdef SYSTEM_DEBUG_RT
    	#define SYSTEM_DEBUG_CAPTURE_RT
    	#define SYSTEM_DEBUG_CAMERA_RT
    	#define SYSTEM_DEBUG_NSF_RT
    	#define SYSTEM_DEBUG_DEI_RT
    	#define SYSTEM_DEBUG_DISPLAY_RT
    	#define SYSTEM_DEBUG_NULL_RT
  	#define SYSTEM_DEBUG_SWMS_RT
   	#define SYSTEM_DEBUG_SCLR_RT
    	#define SYSTEM_DEBUG_IPC_RT
    	#define SYSTEM_DEBUG_ENC_RT
    	#define SYSTEM_DEBUG_MJPEG_RT
    	#define SYSTEM_DEBUG_DEC_RT
 	#define SYSTEM_DEBUG_ISP_RT
	#define SYSTEM_DEBUG_VA_RT
	#define SYSTEM_DEBUG_VSTAB_RT
	#define SYSTEM_DEBUG_GLBCE_RT
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
