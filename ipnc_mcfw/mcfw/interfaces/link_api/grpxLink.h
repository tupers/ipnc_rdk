/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup GRPX_LINK_API GRPX Link API

    This will start the GRPX plane displays and keeps displaying the
    TI logo on top of the video planes.

    @{
*/

/**
    \file grpxLink.h
    \brief GRPX Link API
*/

#ifndef _GRPX_LINK_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GRPX_LINK_H

#include <mcfw/interfaces/link_api/system.h>

Int32 GrpxLink_init(void);
Int32 GrpxLink_deInit(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  /* _GRPX_LINK_H */

/*@}*/
