/** ==================================================================
 *  @file   ti_vcam_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_linux/mcfw_api/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef __TI_VALG_PRIV_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_VALG_PRIV_H__

#include "ti_valg.h"

#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/imgLink.h>

/* =============================================================================
 * Structure
 * ============================================================================= */
typedef struct {
    unsigned int algId;
    unsigned char *pPrm;
    unsigned int PrmSize;
} VALG_MODULE_CONTEXT_S;

extern VALG_MODULE_CONTEXT_S gValgModuleContext;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
