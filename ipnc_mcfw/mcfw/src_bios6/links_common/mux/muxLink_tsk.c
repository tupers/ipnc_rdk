/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2013 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "muxLink_priv.h"

#define MUX_CFG_CHECK_WIDTH_HEIGHT_ON_FRAME_RTUPDATE
/**< Config option for the MUX link - Enable this macro, when MUX link requires
        to rely on "rtChInfoUpdate" and check for change in width/height/pitch.
        In some cases, the a set "rtChInfoUpdate" might not indicate and change
        in frame config.
        When disable, MUX link rely solely on "rtChInfoUpdate" to detemine a 
        change in runtime frameconfiguration.
*/

//#define MUX_CFG_DISABLE_RT_PARAM_SET_ON_CH_MAP_RECONFIG
/**< When ever the map between input queue/channel to output channel map is 
        changed, we would require to apply the last know config for the given
        output channel.
        This requires that - previous link populate all fields of run time 
        config supplied in the frame run time configurations.
*/
#define MUX_MAP_INVALID (SYSTEM_MAX_CH_PER_OUT_QUE + 1)
/**< An identifier used to mark channel map as invalid */

Int32 muxLinkSetOutQueueInfo (MuxLink_Obj * pObj);
Int32 muxLinkUpdateChannelMap (MuxLink_Obj * pObj);
Int32 muxLinkMakeFrames(MuxLink_Obj * pObj, 
                           FVID2_Frame *pOrgFrame,
                           UInt32 queId,
                           MuxLink_OutChInfo *pOutChCfg);
Int32 MuxLink_drvSetQueChMap(MuxLink_Obj * pObj, MuxLink_InQueChNum *pPrm);
Int32 MuxLink_drvGetQueChMap(MuxLink_Obj * pObj, MuxLink_InQueChNum *pPrm);
Int32 MuxLink_drvPrintStats(MuxLink_Obj * pObj);

#ifdef SYSTEM_DEBUG_MUX_1
static void printRtInfo(System_LinkQueInfo *pInQueInfo);
#endif /* SYSTEM_DEBUG_MUX_1 */

#pragma DATA_ALIGN(gMuxLink_tskStack, 32)
#pragma DATA_SECTION(gMuxLink_tskStack, ".bss:taskStackSection")
UInt8 gMuxLink_tskStack[MUX_LINK_OBJ_MAX][MUX_LINK_TSK_STACK_SIZE];

MuxLink_Obj gMuxLink_obj[MUX_LINK_OBJ_MAX];

/**
    \brief Called when a instace of multiplexer is created.
*/
Int32 MuxLink_drvCreate(MuxLink_Obj * pObj, MuxLink_CreateParams * pPrm)
{
    Int32 status, prevLinkInQueId, i = 0;
    Semaphore_Params semParams;

#ifdef SYSTEM_DEBUG_MUX
    Vps_printf(" %d: Mux   : Creating... !!!\n", Utils_getCurTimeInMsec());
#endif
    /* 
        1. Take/maintain a local copy of create args
        2. Create local resources semaphore, outFrameQue, pool of free FVID2 
            containers, etc...
        3. Initialize stats
        4. Build the input queue/channel to output channel map
        5. Get the input queue/channels info
        6. Setup the outqueue channel info based on input queue channel info
    */
    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));
    UTILS_assert(pObj->createArgs.numInQue < MUX_LINK_MAX_IN_QUE);
    UTILS_assert(pObj->createArgs.muxNumOutChan <= SYSTEM_MAX_CH_PER_OUT_QUE);

    /* 2 */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->lock = Semaphore_create(1u, &semParams, NULL);
    UTILS_assert(pObj->lock != NULL);

    status = Utils_bufCreate(&pObj->outFrameQue, FALSE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    /* Require this queue to block on get, as a free FVID2 frame would be 
        requested only when dup is required. If we do not have free frames
        lets hold up */
    status = Utils_queCreate(&pObj->freeFrameQue, MUX_LINK_MAX_DUP_FRAMES, 
                                &pObj->freeFrameQueMemory, 
                                UTILS_QUE_FLAG_BLOCK_QUE_GET);
    UTILS_assert(status == FVID2_SOK);
    for (i = 0; i < MUX_LINK_MAX_DUP_FRAMES; i++)
    {
        /* No need to initialize FVID2 Frame container / sysFrameInfo
            when required to dup, we will copy the original frame into a free
            FVID2_Frame */
        pObj->frames[i].appData = (Ptr) &pObj->frameInfo[i];
        status = Utils_quePut(&pObj->freeFrameQue, &pObj->frames[i], 
                                BIOS_NO_WAIT);
        UTILS_assert(status == FVID2_SOK);
    }
    
    /* 3 */
    memset(&pObj->stats, 0, sizeof(pObj->stats));

    /* 4 */
    for (i = 0; i < pObj->createArgs.numInQue; i++)
    {
        status =
            System_linkGetInfo(pObj->createArgs.inQueParams[i].prevLinkId,
                                &pObj->inTskInfo[i]);
        UTILS_assert(status == FVID2_SOK);
        prevLinkInQueId = pObj->createArgs.inQueParams[i].prevLinkQueId;
        UTILS_assert(pObj->createArgs.inQueParams[i].prevLinkQueId <
                     pObj->inTskInfo[i].numQue);
        UTILS_assert(pObj->inTskInfo[i].queInfo[prevLinkInQueId].numCh <= 
            SYSTEM_MAX_CH_PER_OUT_QUE);
    
#ifdef SYSTEM_DEBUG_MUX_1
        Vps_printf(" %d: MUX_DEBUG : MuxLink_drvCreate inQue ID %d !!!\n", 
                        Utils_getCurTimeInMsec(), i);
        printRtInfo(&pObj->inTskInfo[i].queInfo[prevLinkInQueId]);
#endif /* SYSTEM_DEBUG_MUX_1 */

    }
    memset (&pObj->outChInfo, 0, sizeof(pObj->outChInfo));
    memset (&pObj->inQueChToOutChMap, MUX_MAP_INVALID, 
        sizeof(pObj->inQueChToOutChMap));
    status = muxLinkUpdateChannelMap(pObj);
    UTILS_assert(status == FVID2_SOK);

    /* 5 */
    status = muxLinkSetOutQueueInfo(pObj);
    UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_MUX
    Vps_printf(" %d: Mux   : Create Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return status;
}

/**
    \brief Called when instance of multiplexer is deleted.
*/
Int32 MuxLink_drvDelete(MuxLink_Obj * pObj)
{
    Utils_queDelete(&pObj->freeFrameQue);
    Utils_bufDelete(&pObj->outFrameQue);
    Semaphore_delete(&pObj->lock);

#ifdef SYSTEM_DEBUG_MUX
    Vps_printf(" %d: Mux   : Delete Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}


Int32 MuxLink_drvProcessFrames(MuxLink_Obj * pObj)
{
    Int32 status;
    MuxLink_CreateParams *pCreateArgs;
    FVID2_FrameList freeFrameList, inFrameList;
    System_FrameInfo *pFrameInfo;
    MuxLink_OutChInfo *pOutChCfg;
    System_LinkQueInfo *pInQueInfo;
    UInt32 queId, frameId, fwdFrame, outChInfoIndex, prevLinkQId, notifyNextLink;
    FVID2_Frame *pFrame;

    pCreateArgs = &pObj->createArgs;
    notifyNextLink = FALSE;
    /* For all input queues
        1. Get the frames that requires to be processed 
            2. Check if frame is mapped to valid output channel
                3. Duplicate frame if its mapped more than 1 output channel
            4. Return back un-processed / mapped frames
            5. Notify the next link on data availability
    */
    for (queId = 0; queId < pObj->createArgs.numInQue; queId++)
    {
        status = FVID2_EFAIL;
        freeFrameList.numFrames = 0;
        inFrameList.numFrames = 0;

        /* 1 */
        System_getLinksFullFrames(pCreateArgs->inQueParams[queId].prevLinkId,
                                  pCreateArgs->inQueParams[queId].prevLinkQueId,
                                  &inFrameList);

        for (frameId = 0; frameId < inFrameList.numFrames; frameId++)
        {
            pFrame = inFrameList.frames[frameId];
            UTILS_assert(pFrame != NULL);
            UTILS_assert(pFrame->appData != NULL);
            pFrameInfo = (System_FrameInfo *)pFrame->appData;
            UTILS_assert(pFrame->channelNum < SYSTEM_MAX_CH_PER_OUT_QUE);
            fwdFrame = TRUE;

            /* 2 */
            outChInfoIndex = 
                    pObj->inQueChToOutChMap[queId][pFrame->channelNum];
            
            if (outChInfoIndex < SYSTEM_MAX_CH_PER_OUT_QUE)
            {
                pOutChCfg = &pObj->outChInfo[outChInfoIndex];
                if (pOutChCfg->isValid == TRUE)
                {
                    /* 3 */
                    if (pOutChCfg->numOutChan >= 1)
                    {
                        #ifdef MUX_LINK_ENABLE_CHANNEL_STATS
                            pObj->stats.processedCount[queId][pFrame->channelNum]++;
                        #else
                            pObj->stats.processedCount++;
                        #endif /* MUX_LINK_ENABLE_CHANNEL_STATS */

                        status = muxLinkMakeFrames(pObj, pFrame, queId, 
                                                    pOutChCfg);
                        UTILS_assert(status == FVID2_SOK);
                        notifyNextLink = TRUE;
                    }
                    else
                    {
                        /* This should not happen */
                        UTILS_assert(pOutChCfg->numOutChan != 0x0);
                        continue;
                    }
                }
                else
                {
                    fwdFrame = FALSE;
                    /* Frame received for a un-mapped channel, check if 
                        rtParam update is set for the frame, if so, 
                        copy the config locally into inqueueInfo. 
                        If and when this channel is mapped to a valid 
                        output channel, we could use this config to 
                        described the output frame appropriately */
                    if(pFrameInfo->rtChInfoUpdate)
                    {
                        prevLinkQId = 
                            pObj->createArgs.inQueParams[queId].prevLinkQueId;
                        pInQueInfo = 
                            &pObj->inTskInfo[queId].queInfo[prevLinkQId];
                        /* Copy the frame sysFrame info into input channel 
                            info */
                        memcpy(&pInQueInfo->chInfo[pFrame->channelNum], 
                                pFrameInfo, 
                                sizeof(pInQueInfo->chInfo[pFrame->channelNum]));

#ifdef SYSTEM_DEBUG_MUX_1
                        Vps_printf(" %d: MUX_DEBUG : MuxLink_drvProcessFrame - unmapped ch inQue ID %d !!!\n", 
                        Utils_getCurTimeInMsec(), queId);
                        printRtInfo(pInQueInfo);
#endif /* SYSTEM_DEBUG_MUX_1 */

                    }
                }
            }
            else
            {
                /* Input queue id/Channel is not mapped, return the frame to
                    the originator.
                    Note that the channel number described by the frame is also
                    in-correct. Hence, we do not attempt to check for rtUpdate
                    */
                fwdFrame = FALSE;
            }

            if (fwdFrame == FALSE)
            {
                UTILS_assert(freeFrameList.numFrames < 
                    UTILS_ARRAYSIZE(freeFrameList.frames));
                freeFrameList.frames[freeFrameList.numFrames] = pFrame;
                freeFrameList.numFrames++;

                #ifdef MUX_LINK_ENABLE_CHANNEL_STATS
                    pObj->stats.rejectCount[queId][pFrame->channelNum]++;
                #else
                    pObj->stats.rejectCount++;
                #endif /* MUX_LINK_ENABLE_CHANNEL_STATS */
            }
        }

        /* 4 */
        /* Free up any rejected frames */
        if (freeFrameList.numFrames > 0)
        {
            System_putLinksEmptyFrames(pCreateArgs->inQueParams[queId].
                                       prevLinkId,
                                       pCreateArgs->inQueParams[queId].
                                       prevLinkQueId,
                                       &freeFrameList);
        }
    }
    /* 5 */
    /* Notify the next link on new data. Need not check the status, as the 
        status would be reset for every input queue iteration. */
    if (notifyNextLink == TRUE)
    {
        System_sendLinkCmd(pCreateArgs->outQueParams.nextLink,
                       SYSTEM_CMD_NEW_DATA);
    }
    return FVID2_SOK;
}

Void MuxLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    MuxLink_Obj *pObj = (MuxLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = MuxLink_drvCreate(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);

                MuxLink_drvProcessFrames(pObj);
                break;

            case MUX_LINK_CMD_SET_QUE_CH_MAP:
                MuxLink_drvSetQueChMap(pObj, 
                    (MuxLink_InQueChNum *) Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case MUX_LINK_CMD_GET_QUE_CH_MAP:
                MuxLink_drvGetQueChMap(pObj,
                    (MuxLink_InQueChNum *) Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case MUX_LINK_CMD_PRINT_STATISTICS:
                MuxLink_drvPrintStats(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    MuxLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/**
    \brief Called by the next link to get information about this links output
            queue
*/

Int32 MuxLink_getLinkInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    MuxLink_Obj *pObj = (MuxLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->muxLinkInfo, sizeof(*info));

    return FVID2_SOK;
}

/**
    \brief Called by the next link to get processed frames
*/
Int32 MuxLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                              FVID2_FrameList * pFrameList)
{
    Int32 status;

    UTILS_assert(queId == 0x0);
    MuxLink_Obj *pObj = (MuxLink_Obj *) pTsk->appData;
    /* We have only one output queue. Ignoring the arg queId */
    status =  Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);

    return status;
}

/**
    \brief Called by the next link to return processed frames.
*/

Int32 MuxLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                               FVID2_FrameList * pFrameList)
{
    MuxLink_Obj *pObj = (MuxLink_Obj *) pTsk->appData;
    UInt32 i, rtnQId, frmCnt;
    FVID2_Frame *pFrame, *pOrgFrame;
    System_FrameInfo *pFrameInfo, *pOrgFrameInfo;

    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    for (i = 0; i < MUX_LINK_MAX_IN_QUE; i++)
    {
        pObj->rtnFrameList[i].numFrames = 0;
    }

    UTILS_assert(queId == 0);
    for (i = 0; i < pFrameList->numFrames; i++)
    {
        pFrame = pFrameList->frames[i];
        if (pFrame == NULL)
            continue;

        pFrameInfo = (System_FrameInfo *) pFrame->appData;
        UTILS_assert(pFrameInfo != NULL);
        pOrgFrame = pFrameInfo->pMuxDupOrgFrame;
        UTILS_assert(pOrgFrame != NULL);
        pOrgFrameInfo = (System_FrameInfo *) pOrgFrame->appData;
        UTILS_assert(pOrgFrameInfo != NULL);

        #ifdef MUX_LINK_ENABLE_CHANNEL_STATS
		rtnQId = pFrameInfo->muxOrgQId;
            pObj->stats.rcvCount[rtnQId]++;
        #else
            pObj->stats.rcvCount++;
        #endif /* MUX_LINK_ENABLE_CHANNEL_STATS */

        /* The basic idea is, all duplicate frames should be consumed and the 
            original frame should be returned back to previous link

            We rely on the muxDupCount to ensure all frames (original + its 
            duplicate) are returned. When the count is 0, all frames have been
            returned by the next link.

            Also, we do not have control on the order in which frames would be 
            returned. Hence the following conditional checks is required.
        */
		UTILS_assert(pOrgFrameInfo->muxDupCount > 0x0);
        pOrgFrameInfo->muxDupCount--;
        if (pOrgFrameInfo->muxDupCount == 0x0)
        {
            /* Original frame that has to be returned back the previous link */
            /* Restore the original ch, get the source queue ID */
            rtnQId = pFrameInfo->muxOrgQId;
            UTILS_assert(rtnQId < MUX_LINK_MAX_IN_QUE);
            frmCnt = pObj->rtnFrameList[rtnQId].numFrames;

            pOrgFrame->channelNum = pFrameInfo->muxOrgChNum;
            pObj->rtnFrameList[rtnQId].frames[frmCnt] = pOrgFrame;
            pObj->rtnFrameList[rtnQId].numFrames++;
            #ifdef MUX_LINK_ENABLE_CHANNEL_STATS
                pObj->stats.retunedCount[rtnQId][pOrgFrame->channelNum]++;
            #else
                pObj->stats.retunedCount++;
            #endif /* MUX_LINK_ENABLE_CHANNEL_STATS */
        }

        if (pFrame != pOrgFrame)
        {
            /* Duplicate frame, consume it */
            Utils_quePut(&pObj->freeFrameQue, pFrame, BIOS_NO_WAIT);
        }
    }

    for (i = 0; i < MUX_LINK_MAX_IN_QUE; i++)
    {
        if (pObj->rtnFrameList[i].numFrames > 0)
        {
            System_putLinksEmptyFrames(
                                pObj->createArgs.inQueParams[i].prevLinkId,
                                pObj->createArgs.inQueParams[i].prevLinkQueId,
                                &pObj->rtnFrameList[i]);
        }
    }

    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}

Int32 MuxLink_drvSetQueChMap(MuxLink_Obj * pObj, 
                        MuxLink_InQueChNum *pPrm)
{
    Int32 rtnValue;
    UInt32 chIndex, qId, outChInfoIndex, prevLinkInQueId;
    MuxLink_OutChInfo *pOutChInfo;

    UTILS_assert(pPrm != NULL);
    rtnValue = FVID2_SOK;
    for (chIndex = 0; chIndex < pObj->createArgs.muxNumOutChan; chIndex++)
    {
        if (pPrm[chIndex].inQueId >= pObj->createArgs.numInQue)
        {
            Vps_printf(" %d: MUX : WARNING : Invalid input queue id (%3d) !!!\n"
					, Utils_getCurTimeInMsec(), pPrm[chIndex].inQueId);
            continue;
        }
        qId = pPrm[chIndex].inQueId;
        prevLinkInQueId = pObj->createArgs.inQueParams[qId].prevLinkQueId;
        UTILS_assert(pObj->createArgs.inQueParams[qId].prevLinkQueId <
                     pObj->inTskInfo[qId].numQue);
        UTILS_assert(pObj->inTskInfo[qId].queInfo[prevLinkInQueId].numCh <= 
            SYSTEM_MAX_CH_PER_OUT_QUE);

        if (pPrm[chIndex].inChNum >= 
            pObj->inTskInfo[qId].queInfo[prevLinkInQueId].numCh)
        {
            Vps_printf(" %d: MUX : WARNING : Invalid input channel no (%3d) "
                        " for queue-id %3d !!! \n",
                    Utils_getCurTimeInMsec(), pPrm[chIndex].inChNum, 
                    pPrm[chIndex].inQueId);
            continue;
        }
    }
    if (rtnValue == FVID2_SOK)
    {
        Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);
        
        memset(&pObj->outChInfo, 0, sizeof(pObj->outChInfo));
        memset(&pObj->inQueChToOutChMap, MUX_MAP_INVALID, 
                sizeof(pObj->inQueChToOutChMap));
        memcpy(pObj->createArgs.outChMap, pPrm, 
                sizeof(pObj->createArgs.outChMap));

        rtnValue = muxLinkUpdateChannelMap(pObj);

        /* We are not sure about the runtime params for this channel. Hence
            get the last know runtime param for this channel */
        for (qId = 0;  qId < MUX_LINK_MAX_IN_QUE; qId++)
        {
            for (chIndex = 0; chIndex < SYSTEM_MAX_CH_PER_OUT_QUE; chIndex++)
            {
                outChInfoIndex = pObj->inQueChToOutChMap[qId][chIndex];
                if (outChInfoIndex >= SYSTEM_MAX_CH_PER_OUT_QUE)
                {
                    /* Not mapped */
                    continue;
                }
                pOutChInfo = &pObj->outChInfo[outChInfoIndex];
                if (pOutChInfo->isValid == TRUE)
                {
                    /* Instruct to get the last know rtParam config */
                    pOutChInfo->opChRtInfoUpdate = TRUE;
                }
            }
        }

        Semaphore_post(pObj->lock);
    }

    return rtnValue;
}

/**
    \brief Get the current map between input queue channels and output channel
*/

Int32 MuxLink_drvGetQueChMap(MuxLink_Obj * pObj, 
                        MuxLink_InQueChNum *pPrm)
{
    UInt32 qId, chNum, outChIndex, temp, outChNum;
    MuxLink_OutChInfo *pOutChInfo;
    UTILS_assert(pPrm != NULL);
    
    for (qId = 0; qId < MUX_LINK_MAX_IN_QUE; qId++)
    {
        for (chNum = 0; chNum < SYSTEM_MAX_CH_PER_OUT_QUE; chNum++)
        {
            temp = pObj->inQueChToOutChMap[qId][chNum];
            if (temp >= SYSTEM_MAX_CH_PER_OUT_QUE)
            {
                /* Not mapped */
                continue;
            }
            pOutChInfo = &pObj->outChInfo[temp];
            if (pOutChInfo->isValid != TRUE)
            {
                /* Not mapped */
                continue;
            }
            for (outChIndex = 0; outChIndex < pOutChInfo->numOutChan; 
                    outChIndex++)
            {
                outChNum = pOutChInfo->outChNum[outChIndex];
                UTILS_assert(outChNum < SYSTEM_MAX_CH_PER_OUT_QUE);
                pPrm[outChNum].inQueId = qId;
                pPrm[outChNum].inChNum = chNum;
                #ifdef SYSTEM_DEBUG_MUXLINK
                Vps_printf(" %d: inQueueId %3d : Channel Num %3d -> %3d "
                    "outChNum\n",Utils_getCurTimeInMsec(), qId, chNum, outChNum);
                #endif
            }
        }
    }

    return FVID2_SOK;
}


/**
    \brief Prints stats of this link.
*/
Int32 MuxLink_drvPrintStats(MuxLink_Obj * pObj)
{
#ifdef MUX_LINK_ENABLE_CHANNEL_STATS
	UInt32 inQId, inChNum;
    Vps_printf("\n\n");
    Vps_printf("**** Mux Link Stats **** \n");
    Vps_printf("    Instance %3d on core %3d \n", pObj->instId, 
        SYSTEM_GET_PROC_ID(pObj->tskId));
	Vps_printf("In Q Id | In Ch Num |ProceCount| Ret Count| Rej Count| \n");
	Vps_printf("--------|-----------|----------|----------|----------| \n");
	for (inQId = 0; inQId < pObj->createArgs.numInQue; inQId++)
	{
		for (inChNum = 0; inChNum < SYSTEM_MAX_CH_PER_OUT_QUE; inChNum++)
		{
			if (pObj->stats.processedCount[inQId][inChNum] == 0)
			{
				continue;
			}
			Vps_printf("%8d|%11d|%10d|%10d|%10d|\n", inQId, inChNum,
				pObj->stats.processedCount[inQId][inChNum],
				pObj->stats.retunedCount[inQId][inChNum],
				pObj->stats.rejectCount[inQId][inChNum]);
		}
	}


#else /* MUX_LINK_ENABLE_CHANNEL_STATS */
    Vps_printf("\n\n");
    Vps_printf("**** Mux Link Stats **** \n");
    Vps_printf("    Instance %3d on core %3d \n", pObj->instId, 
        SYSTEM_GET_PROC_ID(pObj->tskId));

    Vps_printf("-------------------------------------------------- \n");
    Vps_printf("    Processed Frame Count   %d \n", pObj->stats.processedCount);
    Vps_printf("    Returned Frame Count    %d \n", pObj->stats.retunedCount);
    Vps_printf("    Rejected Frames Count   %d \n", pObj->stats.rejectCount);
    Vps_printf("    Forwarded Frames Count  %d \n", pObj->stats.outCount);
    Vps_printf("    Received Frames Count   %d \n", pObj->stats.rcvCount);
    Vps_printf("-------------------------------------------------- \n");
#endif /* MUX_LINK_ENABLE_CHANNEL_STATS */
    Vps_printf("\n\n");
    return FVID2_SOK;
}

/**
    \brief Initializes the link, creates a task for every instance of the link.
*/

Int32 MuxLink_init(void)
{
    Int32 status;
    System_LinkObj linkObj;
    UInt32 muxInstId;
    MuxLink_Obj *pObj;
    char tskName[32];
    UInt32 procId = System_getSelfProcId();

    for (muxInstId = 0; muxInstId < MUX_LINK_OBJ_MAX; muxInstId++)
    {
        pObj = &gMuxLink_obj[muxInstId];

        memset(pObj, 0, sizeof(MuxLink_Obj));

        pObj->instId = muxInstId;
        pObj->tskId =
            SYSTEM_MAKE_LINK_ID(procId, SYSTEM_LINK_ID_MUX_0) + muxInstId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = MuxLink_getFullFrames;
        linkObj.linkPutEmptyFrames = MuxLink_putEmptyFrames;
        linkObj.getLinkInfo = MuxLink_getLinkInfo;
        sprintf(pObj->name, "MUX%d", muxInstId);

        System_registerLink(pObj->tskId, &linkObj);

        sprintf(tskName, "MUX%d", muxInstId);

        status = Utils_tskCreate(&pObj->tsk,
                                 MuxLink_tskMain,
                                 MUX_LINK_TSK_PRI,
                                 gMuxLink_tskStack[muxInstId],
                                 MUX_LINK_TSK_STACK_SIZE, pObj, tskName);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/**
    \brief De-initializes the link.
*/

Int32 MuxLink_deInit()
{
    UInt32 muxInstId;

    for (muxInstId = 0; muxInstId < MUX_LINK_OBJ_MAX; muxInstId++)
    {
        Utils_tskDelete(&gMuxLink_obj[muxInstId].tsk);
    }
    return FVID2_SOK;
}

/**
    \brief Called to create output information, that is expected to be consumed
            by next link
*/

Int32 muxLinkSetOutQueueInfo (MuxLink_Obj * pObj)
{
    UInt32 outChInfoIndex, inChNum, outChNum, j, prevLinkInQueId, i;
    System_LinkQueInfo *pOutQueInfo, *pInQueInfo;

    /* Set the output q info, no of output Qs and num channels */
    pObj->muxLinkInfo.numQue = 1;
    pObj->muxLinkInfo.queInfo[0].numCh = pObj->createArgs.muxNumOutChan;

    pOutQueInfo = &pObj->muxLinkInfo.queInfo[0];
    for (i = 0; i < pObj->createArgs.numInQue; i++)
    {
        prevLinkInQueId = pObj->createArgs.inQueParams[i].prevLinkQueId;
        UTILS_assert(prevLinkInQueId < pObj->inTskInfo[i].numQue);
        UTILS_assert(pObj->inTskInfo[i].queInfo[prevLinkInQueId].numCh <= 
            SYSTEM_MAX_CH_PER_OUT_QUE);

        pInQueInfo = &pObj->inTskInfo[i].queInfo[prevLinkInQueId];
        UTILS_assert(pInQueInfo->numCh <= SYSTEM_MAX_CH_PER_OUT_QUE);
        for (inChNum = 0; inChNum < pInQueInfo->numCh; inChNum++)
        {
            outChInfoIndex = pObj->inQueChToOutChMap[i][inChNum];
            if(outChInfoIndex < pObj->muxLinkInfo.queInfo[0].numCh)
            {
                for (j = 0; j < pObj->outChInfo[outChInfoIndex].numOutChan; j++)
                {
                    outChNum = pObj->outChInfo[outChInfoIndex].outChNum[j];
                    memcpy(&pOutQueInfo->chInfo[outChNum],
                           &pInQueInfo->chInfo[inChNum],
                           sizeof(pOutQueInfo->chInfo[outChNum]));
                }
            }
            else
            {
                /* Warning */
                Vps_printf(" %d: WARNING MUX Link : Ignoring channel no %3d of "
                            "Input Q ID %3d. As this channel was not mapped "
                            "!!!\n", Utils_getCurTimeInMsec(), inChNum, i);
            }
        }
    }
    return FVID2_SOK;
}


/**
    \brief Called to create a map between, input queues channels and output
            queue channel.
            Relies on map defined by createArgs.
*/

Int32 muxLinkUpdateChannelMap (MuxLink_Obj * pObj)
{
    MuxLink_OutChInfo *pOutChCfg;
    UInt32 outChInfoIndex, outChNum, inQId, inChNum, temp, totalOutChCount;

    totalOutChCount = 0;
    outChInfoIndex = 0;
    for (outChNum = 0; ((outChNum < pObj->createArgs.muxNumOutChan) && 
                    (totalOutChCount < pObj->createArgs.muxNumOutChan)); 
            outChNum++)
    {
        inQId   = pObj->createArgs.outChMap[outChNum].inQueId;
        inChNum = pObj->createArgs.outChMap[outChNum].inChNum;
        temp = pObj->createArgs.inQueParams[inQId].prevLinkQueId;

        if ((inQId >= pObj->createArgs.numInQue) || 
            (inChNum >= pObj->inTskInfo[inQId].queInfo[temp].numCh))
        {
#ifdef SYSTEM_DEBUG_MUX
            Vps_printf(" %d: WARNING MUX Link : Ignoring channel map for"
                       " Input Queue Id & Channel No (outChMap index %d)!!!\n",
                        Utils_getCurTimeInMsec(), inQId, inChNum, outChNum);
            Vps_printf(" %d: Either incorrect Queue ID or Channel number !!!\n",
                Utils_getCurTimeInMsec());
#endif /* SYSTEM_DEBUG_MUX */

            continue;
        }

        if (pObj->inQueChToOutChMap[inQId][inChNum] < SYSTEM_MAX_CH_PER_OUT_QUE)
        {
            /* This is a duped channel */
            temp = pObj->inQueChToOutChMap[inQId][inChNum];
            pOutChCfg = &pObj->outChInfo[temp];
            UTILS_assert(pOutChCfg->isValid == TRUE);
            UTILS_assert(pOutChCfg->numOutChan < SYSTEM_MAX_CH_PER_OUT_QUE);
            pOutChCfg->outChNum[pOutChCfg->numOutChan] = outChNum;
            pOutChCfg->numOutChan++;
            totalOutChCount++;
        }
        else
        {
            /* Not mapped, mapping for this first time */
            pObj->inQueChToOutChMap[inQId][inChNum] = outChInfoIndex;
            UTILS_assert(outChInfoIndex < UTILS_ARRAYSIZE(pObj->outChInfo));
            pOutChCfg = &pObj->outChInfo[outChInfoIndex];
            pOutChCfg->isValid = TRUE;
            pOutChCfg->numOutChan = 1;
            pOutChCfg->outChNum[0] = outChNum;
            pOutChCfg->opChRtInfoUpdate = FALSE;
            outChInfoIndex++;
            totalOutChCount++;
        }
    }

#ifdef SYSTEM_DEBUG_MUX
    if (totalOutChCount != pObj->createArgs.muxNumOutChan)
    {
        Vps_printf(" %d: Mux : WARNING In Correct mapping !!!\n", 
                            Utils_getCurTimeInMsec());
        Vps_printf(" %d: Mux : WARNING Ignoring the un-mapped channels!!!\n", 
                Utils_getCurTimeInMsec());
    }
#endif /* SYSTEM_DEBUG_MUX */

    return FVID2_SOK;
}

/**
    \brief Called for every frame processed by this link. This function will 
            check if the given frame requires to be duplicated, if so, 
            duplicates and forwards the frames (original + duplicated frames).
*/

Int32 muxLinkMakeFrames(MuxLink_Obj * pObj, 
                           FVID2_Frame *pOrgFrame,
                           UInt32 queId,
                           MuxLink_OutChInfo *pOutChCfg)
{
    Int32 status;
    UInt32 dupCount, prevLinkQId, orgChanNum;
    System_FrameInfo *pFrameInfo, *pOrgFrameInfo;
    FVID2_Frame *pFrame;
    System_LinkQueInfo *pInQueInfo;

    pFrame = pOrgFrame;
    pFrameInfo = (System_FrameInfo *)pOrgFrame->appData;
    pOrgFrameInfo = pFrameInfo;
    dupCount = 0;
    status = FVID2_EFAIL;
    orgChanNum = pFrame->channelNum;

    /* Check if frames rt params are updated inqueue/channel config maintained
        locally */
    if(pOrgFrameInfo->rtChInfoUpdate)
    {
        /* We rely on input queue-channels info when channels are re-mapped. */
        prevLinkQId = pObj->createArgs.inQueParams[queId].prevLinkQueId;
        pInQueInfo = &pObj->inTskInfo[queId].queInfo[prevLinkQId];
        /* Copy the frame sysFrame info into input channel info */
        UTILS_COMPILETIME_ASSERT(sizeof(pInQueInfo->chInfo[pFrame->channelNum]) 
            == sizeof(pOrgFrameInfo->rtChInfo));
#ifdef MUX_CFG_CHECK_WIDTH_HEIGHT_ON_FRAME_RTUPDATE
        if ((pOrgFrameInfo->rtChInfo.width != 
                pInQueInfo->chInfo[pFrame->channelNum].width) || 
            (pOrgFrameInfo->rtChInfo.height != 
                pInQueInfo->chInfo[pFrame->channelNum].height) || 
            (pOrgFrameInfo->rtChInfo.pitch[0] != 
                pInQueInfo->chInfo[pFrame->channelNum].pitch[0]))
        {
            memcpy(&pInQueInfo->chInfo[pFrame->channelNum], 
                &pOrgFrameInfo->rtChInfo, 
                sizeof(pInQueInfo->chInfo[pFrame->channelNum]));
        }

#else
        memcpy(&pInQueInfo->chInfo[pFrame->channelNum], 
                &pOrgFrameInfo->rtChInfo, 
                sizeof(pInQueInfo->chInfo[pFrame->channelNum]));

#endif /* MUX_CFG_CHECK_WIDTH_HEIGHT_ON_FRAME_RTUPDATE */


        /* Need not worry about the duplicated channels, as we would copy the
            rtparam from the original frames rtParam */

#ifdef SYSTEM_DEBUG_MUX_1
        Vps_printf(" %d: MUX_DEBUG : muxLinkMakeFrames - rtFlag set inQue ID %d !!!\n", 
                        Utils_getCurTimeInMsec(), queId);
        printRtInfo(pInQueInfo);
#endif /* SYSTEM_DEBUG_MUX_1 */

    }

    while (dupCount < pOutChCfg->numOutChan)
    {
        status = FVID2_SOK;
        pFrameInfo->muxOrgQId   = queId;
        pFrameInfo->muxOrgChNum = orgChanNum;
        pFrameInfo->muxDupCount = pOutChCfg->numOutChan;
        pFrameInfo->pMuxDupOrgFrame = pOrgFrame;
        pFrame->channelNum = pOutChCfg->outChNum[dupCount];

        /* Check if channel mapping is changed dynamically 
            then copy channel (saved) rtChInfo into frame rtChInfo
            and then froce rtChInfo update for next link */

        if(pOutChCfg->opChRtInfoUpdate)
        {
            prevLinkQId = pObj->createArgs.inQueParams[queId].prevLinkQueId;
            pInQueInfo = &pObj->inTskInfo[queId].queInfo[prevLinkQId];

#ifndef MUX_CFG_DISABLE_RT_PARAM_SET_ON_CH_MAP_RECONFIG
            pFrameInfo->rtChInfoUpdate = TRUE;
            memcpy(&pFrameInfo->rtChInfo, 
                &pInQueInfo->chInfo[pFrameInfo->muxOrgChNum], 
                sizeof(System_LinkChInfo));
#endif 
            /* For all duplicated channels, we would require to set the rtInfo
                update flag. Ensure to turn off opChRtInfoUpdate flag after the
                last of output channel associated with this input que/channel is
                processed */
            if (pOutChCfg->numOutChan <= (dupCount + 1))
            {
                pOutChCfg->opChRtInfoUpdate = FALSE;
            }

#ifdef SYSTEM_DEBUG_MUX_1
            Vps_printf(" %d: MUX_DEBUG : muxLinkMakeFrames !!!\n", 
                Utils_getCurTimeInMsec());
            printRtInfo(pInQueInfo);
#endif /* SYSTEM_DEBUG_MUX_1 */

        }

        /* We can forward the frame now */
        Utils_bufPutFullFrame(&pObj->outFrameQue, pFrame);
        dupCount++;

        #ifdef MUX_LINK_ENABLE_CHANNEL_STATS
            pObj->stats.outCount[queId]++;
        #else
            pObj->stats.outCount++;
        #endif /* MUX_LINK_ENABLE_CHANNEL_STATS */

        /* Check if we require to duplicate this frame on different channels */
        if (pOutChCfg->numOutChan > dupCount)
        {
            status = Utils_queGet(&pObj->freeFrameQue, (Ptr *)&pFrame, 
                                    1, BIOS_NO_WAIT);
            if ((status != FVID2_SOK) || (pFrame == NULL))
            {
                Vps_printf(" %d: Mux : WARNING Out of buffer to dup !!!\n", 
                            Utils_getCurTimeInMsec());
                UTILS_assert(FALSE);
                break;
            }
            pFrameInfo = (System_FrameInfo *)pFrame->appData;
            /* Copy the conents of the original frame */
            memcpy(pFrame, pOrgFrame, sizeof(*pOrgFrame));
            memcpy(pFrameInfo, pOrgFrameInfo, sizeof(*pOrgFrameInfo));
            pFrame->appData = pFrameInfo;
        }
    }
    
    return (status);
}

#ifdef SYSTEM_DEBUG_MUX_1

static void printRtInfo(System_LinkQueInfo *pInQueInfo)
{
    Vps_printf(" %d: MUX_DEBUG RtParam!!!\n", 
        Utils_getCurTimeInMsec());
    Vps_printf(" codingformat : %d\n", pInQueInfo->chInfo[0].codingformat);
    Vps_printf(" dataFormat : %d\n", pInQueInfo->chInfo[0].dataFormat);
    Vps_printf(" memType : %d\n", pInQueInfo->chInfo[0].memType);
    Vps_printf(" startX : %d\n", pInQueInfo->chInfo[0].startX);
    Vps_printf(" startY : %d\n", pInQueInfo->chInfo[0].startY);
    Vps_printf(" width : %d\n", pInQueInfo->chInfo[0].width);
    Vps_printf(" pitch[0] : %d\n", pInQueInfo->chInfo[0].pitch[0]);
    Vps_printf(" pitch[1] : %d\n", pInQueInfo->chInfo[0].pitch[1]);
    Vps_printf(" pitch[2] : %d\n", pInQueInfo->chInfo[0].pitch[2]);
    Vps_printf(" scanFormat : %d\n", pInQueInfo->chInfo[0].scanFormat);
    return;
}
#endif /* SYSTEM_DEBUG_MUX_1 */
