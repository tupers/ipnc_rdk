/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _IRESMAN_HDVICP2_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IRESMAN_HDVICP2_H_

#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/ires.h>
#include <mcfw/src_bios6/links_m3video/codec_utils/iresman_hdvicp2_earlyacquire.h>

XDAS_Void IRESMAN_HDVICP2_EarlyAcquireInit();
XDAS_Void IRESMAN_HDVICP2_EarlyAcquireDeInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _IRESMAN_HDVICP2_H_
                                                            */
