/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup DUP_LINK_API Frame Duplicator (DUP) Link API

    @{
*/

/**
    \file dupLink.h
    \brief Frame Duplicator (DUP) Link API
*/

#ifndef _DUP_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DUP_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief Max output queues to which a given DUP link can connect to */
#define DUP_LINK_MAX_OUT_QUE						(6)

/**
    \brief configure one of the output queue of the dupLink to be delayed and output.

    When one the outputs of the dupLink is modified by the nextLink (for example OSD)
    the frames to that output queue should be output only after the frame has been
    freed by the other output queues. This ensures the other output queues
    receive the unmodified original frame

    \param DupLink_DelayParams * delayPrms [IN] audio parameter to set
*/
#define DUP_LINK_CMD_SET_DELAY_OUTPUT_QUE_PARAMS  	(0x1001)

/** \brief Invalid queu Id for the delay queue and sync delay queue */
#define DUP_LINK_DELAYED_OUTPUT_QUE_INVALID_ID 		(~0u)

/**
    \brief DUP link create parameters
*/
typedef struct
{
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */

    UInt32  numOutQue;
    /**< Number of output queues */

    System_LinkOutQueParams   outQueParams[DUP_LINK_MAX_OUT_QUE];
    /**< Input queue information */

	UInt32  notifyNextLink;
	/**< TRUE: send command to next link notifying that new data is ready in que */

} DupLink_CreateParams;


/**
    \brief DUP link create parameters
*/
typedef struct
{
    UInt32  outQueId;
    /**< QueueId for which frame should be dleayed and output.
     *   Should be less than DupLink_CreateParams.numOutQue
     */
	UInt32 syncOutQueId; 
    /**< QueueId on which delay is observed.
     *   Should be less than DupLink_CreateParams.numOutQue
     */	
} DupLink_DelayParams;
/**
    \brief DUP link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 DupLink_init();

/**
    \brief DUP link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 DupLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/*@}*/
