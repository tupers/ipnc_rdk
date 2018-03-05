/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2013 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup MUX_LINK_API Multiplexer Link API.
                This link could be used to merge frames from multiple channels 
                of multiple input queue into a single output queue with
                mutiple channels.
                With ability to perform selective duplication of frames of 
                input queue/channels.
                WARNING - This link is currently available on M3 VPSS only.
    @{
*/

/**
    \file muxLink.h
    \brief APIs for multiplexer link.
*/

#ifndef _MUX_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MUX_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief Max input queues to which an instance of muxlink could be connected*/
#define MUX_LINK_MAX_IN_QUE         (12)

/** \brief Number output channels that should be supported by default */
#define MUX_LINK_NUM_OUT_CH_DEFAULT (0)

/**
    \brief Control to dynamically change map between input queue, channels to 
            out channel.

    NOTE: All channels of all queues that are not mapped to valid output channel
        number is not processed. Would returned to originators.

    \params MuxLink_InQueChNum  * [IN] Pointet to an array of MuxLink_InQueChNum
                                        with muxNumOutChan valid entries.
                                        muxNumOutChan is a create time config
*/
#define MUX_LINK_CMD_SET_QUE_CH_MAP     (0xA000)


/**
    \brief Get current map between input queue channel numbers to output 
        channel number.

    \params MuxLink_InQueChNum  * [IN] Pointet to an array of MuxLink_InQueChNum
                                        with muxNumOutChan valid entries.
                                        muxNumOutChan is a create time config
*/
#define MUX_LINK_CMD_GET_QUE_CH_MAP     (0xA001)

/**
    \brief Prints the stats of a given instance.

    \params none
*/
#define MUX_LINK_CMD_PRINT_STATISTICS   (0xA002)

/**
    \brief A place holder to define input queue id and its associated channel
            number.
*/
typedef struct
{
    UInt32 inQueId;
    /**< Input queue identifier. Should be less than MUX_LINK_MAX_IN_QUE.
            Also this should be less then number of input queues defined while
            creating an instance of mux link. i.e. MUX_LINK_MAX_IN_QUE */

    UInt32 inChNum;
    /**< Input queues associated channel number, should be less than 
            SYSTEM_MAX_CH_PER_OUT_QUE */

} MuxLink_InQueChNum;


/**
    \brief Mux link create parameters
*/
typedef struct
{
    UInt32 numInQue;
    /**< Number of inputs queue to be supported by this instance */

    System_LinkInQueParams inQueParams[MUX_LINK_MAX_IN_QUE];
    /**< Input queue information */

    System_LinkOutQueParams outQueParams;
    /**< Output queue information */

    MuxLink_InQueChNum outChMap[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Map between input queue, channel number to output channel number.
            e.g. if channel 5 of input queue 2 had to be mapped to output 
                channel 0.
            outChMap[0].inQueId = 2;
            outChMap[0].inChNum = 5;

            e.g. if channel 4 of input queue 1 has to be mapped to output
                channels 1, 7, 8
            outChMap[1].inQueId = 1;
            outChMap[1].inChNum = 4;

            outChMap[7].inQueId = 1;
            outChMap[7].inChNum = 4;

            outChMap[8].inQueId = 1;
            outChMap[8].inChNum = 4;
    */

    UInt32  muxNumOutChan;
    /**< Maximum number of output channels that should be supported by this
            instance of the multiplexer */
} MuxLink_CreateParams;


/**
    \brief Multiplexer link register and init

    - Creates multiplexer task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 MuxLink_init(void);

/**
    \brief Multiplexer link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 MuxLink_deInit(void);

/**
    \brief Initialize the create args required for this link

*/
static void inline MuxLink_CreateParams_Init(MuxLink_CreateParams *pPrm)
{
    memset (pPrm, 0, sizeof(*pPrm));
    memset (&(pPrm->outChMap[0]), (SYSTEM_MAX_CH_PER_OUT_QUE + 1), 
            sizeof(pPrm->outChMap));
    pPrm->muxNumOutChan = MUX_LINK_NUM_OUT_CH_DEFAULT;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/*@}*/
