/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2011 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \file multich_ipcbits.h
    \brief Ipcbits helped functions
*/


#ifndef MULTICH_IPCBITS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define MULTICH_IPCBITS_H_

Void MultiCh_ipcBitsInitCreateParams_BitsInHLOS(IpcBitsInLinkHLOS_CreateParams *
                                                cp);
Void
MultiCh_ipcBitsInitCreateParams_SimcopBitsInHLOS(IpcBitsInLinkHLOS_CreateParams
                                                 * cp);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MULTICH_IPCBITS_H_ */
