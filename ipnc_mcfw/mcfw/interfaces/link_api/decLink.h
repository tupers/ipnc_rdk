/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup DEC_LINK_API Video Decoder Link API

    Video Decode Link can be used to take input from a link and after doing
    video decode like H264 on it output the frames to output queue.

    @{
*/

/**
    \file decLink.h
    \brief Video Decoder Link API
*/

#ifndef _DEC_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DEC_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief Max DEC outputs/output queues */
#define DEC_LINK_MAX_OUT_QUE     (1)

/** \brief Max DEC channels per link */
#define DEC_LINK_MAX_CH          (4)

/** \brief Max number of buffer polls in DEC link */
#define DEC_LINK_MAX_BUF_ALLOC_POOLS         (4)

/** \brief Default value for DPB size in frames. */
#define DEC_LINK_DPB_SIZE_IN_FRAMES_DEFAULT  (-1)

/** \brief Default value for Number of buffers per channel request */
#define DEC_LINK_NUM_BUFFERS_PER_CHANNEL  (0)

/**
    \brief Link CMD: Configure Video Decoder params
*/
#define DEC_LINK_CMD_PRINT_IVAHD_STATISTICS  (0x2001)
/** \Dec command to enable channel */
#define DEC_LINK_CMD_ENABLE_CHANNEL          (0x2002)
/** \Dec command to disable channel */
#define DEC_LINK_CMD_DISABLE_CHANNEL         (0x2003)
/** \Dec command to print channel level statistics */
#define DEC_LINK_CMD_PRINT_STATISTICS        (0x2004)
/** \Dec command to set trick play configuration */
#define DEC_LINK_CMD_SET_TRICKPLAYCONFIG     (0x2005)
#define DEC_LINK_CMD_PRINT_BUFFER_STATISTICS     (0x2006)

/**
    \brief Dec link channel dynamic params

    Defines those parameters that can be changed dynamically on a per channel
    basis for the decode link
*/
typedef struct DecLink_ChDynamicParams
{
    Int32 targetFrameRate;
    /**< Target frame rate of the decoder */
    Int32 targetBitRate;
    /**< Target bitrate of the decoder */
} DecLink_ChDynamicParams;

typedef struct DecLink_ChCreateParams
{
    UInt32 format;
    /**< Codec profile */
    Int32 profile;
    /**< Video coding format */
    Int32 targetMaxWidth;
    /**< Target frame width of the decoder */
    Int32 targetMaxHeight;
    /**< Target frame height of the decoder */
    Int32 displayDelay;
    /**< Max number of frames delayed by decoder */
    DecLink_ChDynamicParams defaultDynamicParams;
    /**< default dynamic params for decoder */
    UInt32 fieldMergeDecodeEnable;
    /**< Enable this option to decode 2 fields in same Decode link call
     * ie. both Top & Bottom field in same ouput buffer with Top filed
     * followed by bottom field data in field seperated mode */
    UInt32 processCallLevel;
    /**< Specifies if process call is done frame level or field level */
    UInt32 numBufPerCh;
    /**< Number of buffer to allocate per channel */
    Int32 dpbBufSizeInFrames;
    /**< Size of the decoder picture buffer.If application
     *   knows the max number of reference frames in the
     *   stream to be fed to the decoder, it can set this
     *   value to enable reduced memory consumption.
     *   Application should set this value to default
     *   if it doesn't care about this parameter
     */
} DecLink_ChCreateParams;

/**
    \brief Dec link create parameters
*/
typedef struct DecLink_CreateParams
{
    System_LinkInQueParams      inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams     outQueParams;
    /**< Output queue information. */
    DecLink_ChCreateParams      chCreateParams[DEC_LINK_MAX_CH];
    /**< Decoder link channel create params */
    UInt32                      tilerEnable;
    /**< Enable tiler for decode ouput buffers */
    UInt32                      numBufPerPool[DEC_LINK_MAX_BUF_ALLOC_POOLS];
    /**< Number of buffer to allocate per channel per buffr pool */

} DecLink_CreateParams;

/**
    \brief Dec link channel info
*/
typedef struct
{
	UInt32 chId;
    /**< Decoder channel number */
} DecLink_ChannelInfo;


/**
    \brief Dec link trickplay configure
*/

typedef struct
{
	UInt32 chId;
    /**< Decoder channel number */
    UInt32 inputFps;
    /**< FrameRate at which Decoder is getting the data> */
    UInt32 targetFps;
    /**< Target FrameRate for TrickPlay. TrickPlay will generate target frame rate from the input framerate> */
} DecLink_TPlayConfig;

/**
    \brief Dec link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 DecLink_init();

/**
    \brief Dec link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 DecLink_deInit();


static inline void DecLink_CreateParams_Init(DecLink_CreateParams *pPrm)
{
    UInt32 i;

    memset(pPrm, 0, sizeof(*pPrm));

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;

    /* when set 0, decoder will take default value based on system
       defined default on BIOS side */
    for (i=0; i<DEC_LINK_MAX_BUF_ALLOC_POOLS; i++)
    {
        pPrm->numBufPerPool[i] = 0;
    }
    for (i=0; i<DEC_LINK_MAX_CH;i++)
    {
        pPrm->chCreateParams[i].dpbBufSizeInFrames =
                               DEC_LINK_DPB_SIZE_IN_FRAMES_DEFAULT;
        pPrm->chCreateParams[i].numBufPerCh = 0;
        pPrm->chCreateParams[i].displayDelay = 0;
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/*@}*/

