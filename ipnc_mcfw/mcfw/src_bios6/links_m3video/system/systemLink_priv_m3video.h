/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SYSTEM_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3video/system/system_priv_m3video.h>
#include <mcfw/interfaces/link_api/systemLink_m3video.h>

Int Utils_encdecSetCh2IvahdMap(SystemVideo_Ivahd2ChMap_Tbl* Tbl);

typedef struct {
    UInt32 tskId;

    Utils_TskHndl tsk;

} SystemLink_Obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
