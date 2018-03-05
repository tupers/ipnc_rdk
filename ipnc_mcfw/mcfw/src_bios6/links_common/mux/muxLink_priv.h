/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2013 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _MUX_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MUX_LINK_PRIV_H_

#include <mcfw/src_bios6/links_common/system/system_priv_common.h>
#include <mcfw/interfaces/link_api/muxLink.h>

/** \brief Max number of instnaces of mux link supported.
           When this count is increased, ensure to update the support for more
           instances in  \ipnc_mcfw\mcfw\interfaces\link_api\system_linkId.h */
#define MUX_LINK_OBJ_MAX (2)


/** \brief Maximum number of duplicate frames that is supported, per instance
            of this link. */
#define MUX_LINK_MAX_DUP_FRAMES (SYSTEM_LINK_FRAMES_PER_CH * \
                                    SYSTEM_MAX_CH_PER_OUT_QUE)

/** \brief Enable gathering of stats per channel. When disabled, instance 
            specific stats are tracked. */
#define MUX_LINK_ENABLE_CHANNEL_STATS

/**
    \brief Processed frame statistics
*/
typedef struct {

#ifdef MUX_LINK_ENABLE_CHANNEL_STATS
    UInt32 processedCount[MUX_LINK_MAX_IN_QUE][SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Frames processed count */
    UInt32 retunedCount[MUX_LINK_MAX_IN_QUE][SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Frames returned to originator count */
    UInt32 rejectCount[MUX_LINK_MAX_IN_QUE][SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Frame rejected count */
    UInt32 outCount[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Frames forwarded count - includes duped frames */
    UInt32 rcvCount[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Frames returned by next link to this link */
#else
    UInt32 processedCount;
    /**< Frames processed count */
    UInt32 retunedCount;
    /**< Frames returned to originator count */
    UInt32 rejectCount;
    /**< Frame rejected count */
    UInt32 outCount;
    /**< Frames forwarded count - includes duped frames */
    UInt32 rcvCount;
    /**< Frames returned by next link to this link */
#endif /* MUX_LINK_ENABLE_CHANNEL_STATS */

}  MuxLink_StatsInfo;


/**
    \brief A place holder to define output channel specific configuration.
*/
typedef struct {

    Bool    isValid;
    /**< Flag to indicate if this instance has a valid entry */
    UInt32  numOutChan;
    /**< Number of output channels that is associated for this input channel */
    UInt32  outChNum[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Holds the output channel (s) number that a input channel should be sent
            on.
            e.g. numOutChan = 1; then outChNum[0] will hold the output channel 
                number that the given input channel should be sent on.
                In this case a given input frame is not duplicated.
            e.g. numOutChan = 3; then outChNum[0], outChNum[1] outChNum[2] will
                hold the output channel number that the given input should be
                sent on.
                In this case a given input frame is duplicated 2
    */
    Bool    opChRtInfoUpdate;
    /**< Defines if RT param requires an update. Could be set when output / 
            input channel map/queue id changes. */

} MuxLink_OutChInfo;

/**
    \brief Defines per-instance configurations/parameters of multiplexer link
*/
typedef struct {
    UInt32 tskId;
    /**< Multiplexer task id */
    char name[32];
    /**< Mux Link instance name */
    UInt32 instId;
    /**< Instance identifier */
    Utils_TskHndl tsk;
    /**< Multiplexer task handle */
    MuxLink_CreateParams createArgs;
    /**< Configurations created with / updated on remap of channels */
    System_LinkInfo inTskInfo[MUX_LINK_MAX_IN_QUE];
    /**< Input queue / links configurations. Also used to maintain the last
            know channel rtParams. */
    System_LinkInfo muxLinkInfo;
    /**< Multiplexer link info, used by next link */

    Utils_BufHndl outFrameQue;
    /**< Queue that holds frames that are to be sent to next link */

    Semaphore_Handle lock;
    /**< To ensure mutual exclusive access, required when frames are being 
            returned by the next link */
    FVID2_FrameList rtnFrameList[MUX_LINK_MAX_IN_QUE];
    /**< Frame list that would be used to return frames to previous link */

    Utils_QueHandle freeFrameQue;
    /**< Pool of FVID2 frame (containers only) and its associated sysFrameInfo
            (appData)*/
    FVID2_Frame *freeFrameQueMemory[MUX_LINK_MAX_DUP_FRAMES];
    /**< Memory required by utils to implement free queue */

    FVID2_Frame frames[MUX_LINK_MAX_DUP_FRAMES];
    /**< FVID2 Frames that would be used while duplicating */
    System_FrameInfo frameInfo[MUX_LINK_MAX_DUP_FRAMES];
    /**< Associated frame info */

    MuxLink_OutChInfo outChInfo[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Maintains output configurations for a given input channel of a given
            input queue. 
            Please go through below to understand better. */
    UInt32 inQueChToOutChMap[MUX_LINK_MAX_IN_QUE][SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Association between input channel number ,input queue ID) and its
            output channel(s) information. 
        
        e.g. if input queue id 1 channel 8 is mapped to output channel 2, 3, 4
                input queue id 0 channel 7 is mapped to output channel 0, 1
                input queue id 2 channel 8 is mapped to output channel 5
        then 
            inQueChToOutChMap[1][8] = 0 - A
            inQueChToOutChMap[0][7] = 1 - B
            inQueChToOutChMap[2][8] = 2 - C
            
            These values 0, 1, 2 is used as an index into outChInfo to detemine
            configurations / map for this channel.\

            A
            outChInfo[0].numOutChan = 3;
            outChInfo[0].outChNum[0] = 2;
            outChInfo[0].outChNum[1] = 3;
            outChInfo[0].outChNum[2] = 4;
            In this case, the input channel 8 of queue 1 is sent on output
            channels 2, 3, 4

            B
            outChInfo[1].numOutChan = 2;
            outChInfo[1].outChNum[0] = 0;
            outChInfo[1].outChNum[1] = 1;
            In this case, the input channel 7 of queue 0 is sent on output
            channels 0, 1

            C
            outChInfo[2].numOutChan = 1;
            outChInfo[2].outChNum[0] = 5;
            In this case, the input channel 8 of queue 2 is sent to output
            channel 5
        */

    MuxLink_StatsInfo stats;
    /**< Stats */

} MuxLink_Obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _MUX_LINK_PRIV_H_ */
