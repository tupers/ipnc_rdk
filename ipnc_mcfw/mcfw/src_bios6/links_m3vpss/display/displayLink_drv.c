/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "displayLink_priv.h"
#include <mcfw/src_bios6/links_m3vpss/avsync/avsync_m3vpss.h>
#include "mcfw/interfaces/common_def/ti_vsys_common_def.h"

static inline UInt32 DisplayLink_getSc1DispId()
{
    UInt32 displayId;

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    displayId = VPS_DISP_INST_MAIN_DEI_SC1;
#else
    displayId = VPS_DISP_INST_MAIN_DEIH_SC1;
#endif

    return (displayId);
}

static inline UInt32 DisplayLink_getSc2DispId()
{
    UInt32 displayId;

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    displayId = VPS_DISP_INST_AUX_SC2;
#else
    displayId = VPS_DISP_INST_AUX_DEI_SC2;
#endif

    return (displayId);
}

static inline Bool DisplayLink_drvIsDeiDisplayDrv(DisplayLink_Obj * pObj)
{
    Bool isDeiDisplay = FALSE;

    if ((VPS_DISP_INST_MAIN_DEIH_SC1 == pObj->displayInstId) ||
        (VPS_DISP_INST_AUX_DEI_SC2 == pObj->displayInstId) ||
        (VPS_DISP_INST_MAIN_DEI_SC1 == pObj->displayInstId) ||
        (VPS_DISP_INST_AUX_SC2 == pObj->displayInstId))
    {
        isDeiDisplay = TRUE;
    }

    return (isDeiDisplay);
}

static
Bool  DisplayLink_drvIsSDDef(DisplayLink_Obj * pObj)
{
    Bool isSdDef = FALSE;

    if ((VSYS_STD_NTSC == pObj->createArgs.displayRes)
        ||
        (VSYS_STD_PAL == pObj->createArgs.displayRes))
    {
        isSdDef = TRUE;
    }
    return isSdDef;
}

static
Bool DisplayLink_drvDoInputPitchDoubling(DisplayLink_Obj * pObj)
{
	Bool doPitchDouble = FALSE;

    if (DisplayLink_drvIsSDDef(pObj)
        &&
        pObj->displayFormat.fieldMerged[0] == FALSE
        &&
            (
                (FALSE == pObj->fieldInput[pObj->curActiveQueue])
                ||
                pObj->createArgs.forceFieldSeparatedInputMode
            )
        )
    {
    	doPitchDouble = TRUE;
    }
    return doPitchDouble;
}

static
Void  DisplayLink_drvCreateFrameInfo(DisplayLink_Obj * pObj)
{
    Int32 status,i;
    struct DisplayLink_FrameInfoObj *frameInfo =
        &pObj->frameInfo;

    UTILS_COMPILETIME_ASSERT(UTILS_ARRAYSIZE(frameInfo->infoQMem) ==
                             UTILS_ARRAYSIZE(frameInfo->infoMem));
    status =
    Utils_queCreate(&frameInfo->infoQ,
                    UTILS_ARRAYSIZE(frameInfo->infoQMem),
                    &frameInfo->infoQMem[0],
                    UTILS_QUE_FLAG_NO_BLOCK_QUE);
    UTILS_assert(status == 0);
    for (i = 0; i < UTILS_ARRAYSIZE(frameInfo->infoMem);i++)
    {
        DisplayLink_FrameInfo *frameInfoElem;

        frameInfoElem = &frameInfo->infoMem[i];
        status =
        Utils_quePut(&frameInfo->infoQ,frameInfoElem,
                     BIOS_NO_WAIT);
        UTILS_assert(status == 0);
    }
    return;
}

static
Void  DisplayLink_drvDeleteFrameInfo(DisplayLink_Obj * pObj)
{
    Int32 status;

    status =
    Utils_queDelete(&pObj->frameInfo.infoQ);
    UTILS_assert(status == 0);
}




static
Void  DisplayLink_drvFreeFrameInfo(DisplayLink_Obj * pObj,
                                   FVID2_FrameList *frameList)
{
    Int i;
    FVID2_Frame *frame;
    DisplayLink_FrameInfo *frameInfo;
    Int32 status;

    for (i = 0; i < frameList->numFrames; i++)
    {
        frame = frameList->frames[i];
        if (DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM != frame->channelNum)
        {
            frameInfo = frame->appData;
            UTILS_assert(UTILS_ARRAYISVALIDENTRY(frameInfo,
                                    pObj->frameInfo.infoMem));
            frame->appData = frameInfo->origFrameInfo;
            status =
              Utils_quePut(&pObj->frameInfo.infoQ,
                           frameInfo,
                           BIOS_NO_WAIT);
            UTILS_assert(status == 0);
        }
    }
}

static
Void DisplayLink_drvCreateRtParams(DisplayLink_Obj * pObj)
{
    Int32 status;
    Int i;

    UTILS_COMPILETIME_ASSERT(UTILS_ARRAYSIZE(pObj->rtParams.freeQMem)
                             ==
                             UTILS_ARRAYSIZE(pObj->rtParams.paramsMem));
    status =
    Utils_queCreate(&pObj->rtParams.freeQ,
                    UTILS_ARRAYSIZE(pObj->rtParams.freeQMem),
                    pObj->rtParams.freeQMem,
                    UTILS_QUE_FLAG_NO_BLOCK_QUE);
    UTILS_assert(0 == status);
    for (i = 0; i < UTILS_ARRAYSIZE(pObj->rtParams.paramsMem); i++)
    {
        DisplayLink_drvRtParams *rtParams = &pObj->rtParams.paramsMem[i];
        rtParams->dispRtPrms.inFrmPrms = &rtParams->inFrmPrms;
        status = Utils_quePut(&pObj->rtParams.freeQ,
                              rtParams,
                              BIOS_NO_WAIT);
        UTILS_assert(0 == status);
    }
}

static
Void DisplayLink_drvDeleteRtParams(DisplayLink_Obj * pObj)
{
    Int32 status;

    status = Utils_queDelete(&pObj->rtParams.freeQ);
    UTILS_assert(0 == status);
}


static
Vps_DispRtParams * DisplayLink_drvSetFramePitchRtParams(DisplayLink_Obj * pObj)
{
    Int32 status;
    DisplayLink_drvRtParams *rtParams;
    Int i;

    status = Utils_queGet(&pObj->rtParams.freeQ,
                          (Ptr *)&rtParams,
                          1,
                          BIOS_NO_WAIT);
    UTILS_assert(status == 0);
    rtParams->dispRtPrms.inFrmPrms = &rtParams->inFrmPrms;
    rtParams->inFrmPrms.width      = pObj->displayFormat.width;
    rtParams->inFrmPrms.height     = pObj->displayFormat.height;
	rtParams->inFrmPrms.memType    = pObj->displayCreateArgs.memType;
	rtParams->inFrmPrms.dataFormat = pObj->displayFormat.dataFormat;	
    for (i = 0; i < pObj->numDisplayBufPlanes; i++)
    {
        rtParams->inFrmPrms.pitch[i] = pObj->displayFormat.pitch[i];
    }
    return (&rtParams->dispRtPrms);
}

static
Void DisplayLink_drvFreeRtParams(DisplayLink_Obj * pObj,
                                 FVID2_FrameList *freeFrameList)
{
    Int32 status;
    DisplayLink_drvRtParams *rtParams;
	UInt32 rtParamsAddr;

    rtParams = freeFrameList->perListCfg;
    if (rtParams)
    {
		if(DisplayLink_drvIsDeiDisplayDrv(pObj))
		{
			rtParamsAddr = (UInt32)rtParams;
			rtParamsAddr = rtParamsAddr - (sizeof(Vps_DispRtParams) + sizeof(Vps_FrameParams));
			rtParams     = (DisplayLink_drvRtParams*)rtParamsAddr;
		}	
		
        UTILS_COMPILETIME_ASSERT(
          offsetof(DisplayLink_drvRtParams,dispRtPrms) == 0);
        UTILS_assert(UTILS_ARRAYISVALIDENTRY(rtParams,pObj->rtParams.paramsMem));
        status = Utils_quePut(&pObj->rtParams.freeQ,
                              rtParams,
                              BIOS_NO_WAIT);
        UTILS_assert(status == 0);
    }

}

static
Void  DisplayLink_drvSetPitchInfo(DisplayLink_Obj * pObj)
{
    Int i;
    UInt32 curInQueId;
    UInt32 prevLinkQueId;
    System_LinkChInfo *chInfo;

    curInQueId = pObj->curActiveQueue;
    UTILS_assert(curInQueId < pObj->createArgs.numInputQueues);
    prevLinkQueId =
        pObj->createArgs.inQueParams[curInQueId].prevLinkQueId;
    UTILS_assert(prevLinkQueId <
                 pObj->inTskInfo[curInQueId].numQue);
    pObj->numDisplayBufPlanes =
    Utils_getNumPlanes((FVID2_DataFormat)pObj->displayFormat.dataFormat);
    UTILS_assert(pObj->numDisplayBufPlanes  != 0);
    if (pObj->curDisplayChannelNum <
        pObj->inTskInfo[curInQueId].queInfo[prevLinkQueId].numCh)
    {
        chInfo =
          &pObj->inTskInfo[curInQueId].queInfo[prevLinkQueId].chInfo[pObj->curDisplayChannelNum];
        UTILS_assert(chInfo->dataFormat == pObj->displayFormat.dataFormat);
        for (i = 0; i < pObj->numDisplayBufPlanes; i++)
        {
            pObj->displayFormat.pitch[i] = chInfo->pitch[i];
            if (DisplayLink_drvDoInputPitchDoubling(pObj))
            {
                pObj->displayFormat.pitch[i] *= 2;
            }
        }
    }
}

Vps_DeiDispRtParams * DisplayLink_drvSetDeiFramePitchRtParams(DisplayLink_Obj * pObj)
{
    Int32 status;
    DisplayLink_drvRtParams *rtParams;
    Int i;

    status = Utils_queGet(&pObj->rtParams.freeQ,
                          (Ptr *)&rtParams,
                          1,
                          BIOS_NO_WAIT);
    UTILS_assert(status == 0);

	rtParams->deiDispRtPrms.layoutId     = NULL;
	rtParams->deiDispRtPrms.vcompCropCfg = NULL;
	rtParams->deiDispRtPrms.posCfg       = NULL;
	rtParams->deiDispRtPrms.deiScCropCfg = NULL;
	rtParams->deiDispRtPrms.outFrmPrms   = NULL;
	rtParams->deiDispRtPrms.deiRtCfg     = NULL;
	rtParams->deiDispRtPrms.inFrmPrms    = &rtParams->inFrmPrms;
	
    rtParams->inFrmPrms.width      = pObj->displayFormat.width;
    rtParams->inFrmPrms.height     = pObj->displayFormat.height;
	rtParams->inFrmPrms.memType    = pObj->displayCreateArgs.memType;
	rtParams->inFrmPrms.dataFormat = pObj->displayFormat.dataFormat;
    for (i = 0; i < pObj->numDisplayBufPlanes; i++)
    {
        rtParams->inFrmPrms.pitch[i] = pObj->displayFormat.pitch[i];
    }
    return (&rtParams->deiDispRtPrms);
}


static
Void  DisplayLink_drvHandleDynamicPitchChange(DisplayLink_Obj * pObj,
                                              FVID2_FrameList *frameList)
{
    Bool updateDrvPitch = FALSE;
    UInt32 curInQueId;
    UInt32 prevLinkQueId;
    FVID2_Frame *frame;
    System_LinkChInfo *chInfo;
    DisplayLink_FrameInfo *frameInfo;

    curInQueId = pObj->curActiveQueue;
    UTILS_assert(curInQueId < pObj->createArgs.numInputQueues);
    prevLinkQueId =
        pObj->createArgs.inQueParams[curInQueId].prevLinkQueId;
    UTILS_assert(prevLinkQueId <
                 pObj->inTskInfo[curInQueId].numQue);

    UTILS_assert(frameList->numFrames == 1);
    frame = frameList->frames[0];
    if (frame->channelNum == pObj->curDisplayChannelNum)
    {
        Int i;

        chInfo =
          &pObj->inTskInfo[curInQueId].queInfo[prevLinkQueId].chInfo[pObj->curDisplayChannelNum];

        for (i = 0; i < pObj->numDisplayBufPlanes; i++)
        {
            UInt32 pitch;

            frameInfo = frame->appData;
            UTILS_assert((frameInfo != NULL)
                         &&
                         UTILS_ARRAYISVALIDENTRY(frameInfo,
                         pObj->frameInfo.infoMem));

            if ((frameInfo->origFrameInfo)
                &&
                (frameInfo->origFrameInfo->rtChInfoUpdate))
            {
                chInfo->pitch[i] = frameInfo->origFrameInfo->rtChInfo.pitch[i];
            }

            pitch = chInfo->pitch[i];

            if (DisplayLink_drvDoInputPitchDoubling(pObj))
            {
                pitch *= 2;
            }
            if (pObj->displayFormat.pitch[i] != pitch)
            {
                updateDrvPitch = TRUE;
                Vps_printf("DISPLAY:Runtime Pitch change:[%d->%d]",
                           pObj->displayFormat.pitch[i],pitch);
                break;
            }
        }
    }

    if (updateDrvPitch)
    {
		DisplayLink_drvSetPitchInfo(pObj);
		
		if(frameInfo->origFrameInfo->rtChInfoUpdate)
		{
			/* Current display channel params changed */
			pObj->displayFormat.pitch[0] = frameInfo->origFrameInfo->rtChInfo.pitch[0];
			pObj->displayFormat.pitch[1] = frameInfo->origFrameInfo->rtChInfo.pitch[1];
			pObj->displayFormat.width    = frameInfo->origFrameInfo->rtChInfo.width;
			pObj->displayFormat.height   = frameInfo->origFrameInfo->rtChInfo.height;
		}	
		else
		{
			/* Current display Channel changed */
			pObj->displayFormat.pitch[0] = chInfo->pitch[0];
			pObj->displayFormat.pitch[1] = chInfo->pitch[1];
			pObj->displayFormat.width    = chInfo->width;
			pObj->displayFormat.height   = chInfo->height;		
		}
		
        if (DisplayLink_drvIsDeiDisplayDrv(pObj))
        {
            frameList->perListCfg  =
                DisplayLink_drvSetDeiFramePitchRtParams(pObj);
        }
        else
        {
            frameList->perListCfg  =
                DisplayLink_drvSetFramePitchRtParams(pObj);
        }
    }
}

static
Void  DisplayLink_validateFrameList(DisplayLink_Obj * pObj,
                                    FVID2_FrameList *frameList)
{
    static Bool enableValidate = FALSE;
    FVID2_Frame *frame;
    DisplayLink_FrameInfo *frameInfo;
    Int i;

    if (enableValidate)
    {
        for (i = 0; i < frameList->numFrames; i++)
        {
            frame = frameList->frames[i];
            if (DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM !=
                frame->channelNum)
            {
                frameInfo = frame->appData;
                UTILS_assert(UTILS_ARRAYISVALIDENTRY(frameInfo,
                                                     pObj->frameInfo.infoMem));
            }
        }
    }
}

static
Void  DisplayLink_drvCreateFieldMergedFrame(DisplayLink_Obj * pObj, FVID2_Frame *top_field,
                                            FVID2_Frame *bot_field,
                                            DisplayLink_FrameInfo *frameInfo)
{
    FVID2_Frame *fieldMergedFrame = &frameInfo->frame;

    fieldMergedFrame->addr[0][0] = top_field->addr[0][0];
    fieldMergedFrame->addr[0][1] = top_field->addr[0][1];
    fieldMergedFrame->addr[0][2] = top_field->addr[0][2];
    fieldMergedFrame->addr[1][0] = bot_field->addr[0][0];
    fieldMergedFrame->addr[1][1] = bot_field->addr[0][1];
    fieldMergedFrame->addr[1][2] = bot_field->addr[0][2];

    if(pObj->createArgs.forceFieldSeparatedInputMode)
    {
        /* go to next line in frame for bottom field */
        fieldMergedFrame->addr[1][0] =
         (Ptr)((UInt32)fieldMergedFrame->addr[1][0] + pObj->displayFormat.pitch[0]/2);

        fieldMergedFrame->addr[1][1] =
         (Ptr)((UInt32)fieldMergedFrame->addr[1][1] + pObj->displayFormat.pitch[1]/2);

        fieldMergedFrame->addr[1][2] =
         (Ptr)((UInt32)fieldMergedFrame->addr[1][2] + pObj->displayFormat.pitch[2]/2);

    }

    fieldMergedFrame->blankData  = top_field->blankData;
    fieldMergedFrame->channelNum = top_field->channelNum;
    fieldMergedFrame->drvData    = top_field->drvData;
    fieldMergedFrame->fid        = FVID2_FID_TOP;
    fieldMergedFrame->perFrameCfg = top_field->perFrameCfg;
    fieldMergedFrame->reserved    = top_field->reserved;
    fieldMergedFrame->subFrameInfo = top_field->subFrameInfo;
    fieldMergedFrame->timeStamp    = top_field->timeStamp;
    fieldMergedFrame->appData      = frameInfo;

    frameInfo->isFieldPair         = TRUE;
    frameInfo->origFrame[0]        = top_field;
    frameInfo->origFrame[1]        = bot_field;
    frameInfo->origFrameInfo       = NULL;
}


static
Void  DisplayLink_drvSetFrameInfo(DisplayLink_Obj * pObj,
                                  FVID2_Frame **pFramePtr,
                                  Bool        *frameReject)
{
    DisplayLink_FrameInfo *frameInfo;
    FVID2_Frame *frame;
    Int32 status;
    Bool fieldSeparatedInputMode;

    *frameReject = FALSE;
    frame = *pFramePtr;


    fieldSeparatedInputMode = FALSE;

    if (pObj->fieldInput[pObj->curActiveQueue]
            &&
        pObj->createArgs.forceFieldSeparatedInputMode == FALSE
       )
    {
        fieldSeparatedInputMode = TRUE;
    }
    if (pObj->fieldInput[pObj->curActiveQueue]
            &&
        pObj->createArgs.forceFieldSeparatedInputMode == TRUE
            &&
        pObj->enableFieldSeparatedInputMode
       )
    {
        fieldSeparatedInputMode = TRUE;
    }

    if (DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM !=
        frame->channelNum)
    {
        if(fieldSeparatedInputMode)
        {
            if(pObj->createArgs.forceFieldSeparatedInputMode)
                frame->fid = pObj->nextFid;

            if (frame->fid == pObj->nextFid)
            {
                pObj->nextFid = !(pObj->nextFid);
                if (frame->fid == FVID2_FID_BOTTOM)
                {
                    status = Utils_queGet(&pObj->frameInfo.infoQ,
                                          (Ptr *)&frameInfo,
                                          1,
                                          BIOS_NO_WAIT);
                    UTILS_assert(status == 0);
                    frameInfo->activeQueueId = pObj->curActiveQueue;
                    /* Received bot fields of frame.
                     * Create a field merged frame
                     */
                    UTILS_assert(pObj->topField != NULL);
                    UTILS_assert(pObj->topField->fid == FVID2_FID_TOP);
                    DisplayLink_drvCreateFieldMergedFrame(pObj, pObj->topField,
                                                         frame,
                                                         frameInfo);
                    pObj->topField = NULL;
                    *pFramePtr = &frameInfo->frame;
                }
                else
                {
                    /* if top field. Store top field */
                    pObj->topField = frame;
                    frame->timeStamp = Utils_getCurTimeInMsec();
                    *pFramePtr = NULL;
                }
            }
            else
            {
                /* FID out of order. Reject */
                *frameReject = TRUE;
            }
        }
        else
        {
            status = Utils_queGet(&pObj->frameInfo.infoQ,
                                  (Ptr *)&frameInfo,
                                  1,
                                  BIOS_NO_WAIT);
            UTILS_assert(status == 0);
            frameInfo->activeQueueId = pObj->curActiveQueue;
            frameInfo->isFieldPair = FALSE;
            frameInfo->origFrameInfo = frame->appData;
            frame->appData = frameInfo;
        }
    }
}



static
Int32 DisplayLink_drvDeQueue(DisplayLink_Obj * pObj,
                             FVID2_FrameList *freeFrameList,
                             UInt32 *freeQueId)
{
    Int32 status,i;
    FVID2_Frame *frame;
    FVID2_FrameList tmpFreeFrameList;
    DisplayLink_FrameInfo *frameInfo;

    freeFrameList->numFrames = 0;
    tmpFreeFrameList.numFrames = 0;
    *freeQueId = DISPLAY_LINK_INVALID_INQUEID;
    status =
        FVID2_dequeue(pObj->displayHndl,&tmpFreeFrameList,0,
                      BIOS_NO_WAIT);
    if (FVID2_SOK == status)
    {
        pObj->dequeCount += tmpFreeFrameList.numFrames;

        if (tmpFreeFrameList.numFrames)
        {
            pObj->maxQueueCount++;
#ifdef SYSTEM_DEBUG_DISPLAY_RT
            Vps_printf(" %d: DISPLAY: Deque %d frames\n",
                       Utils_getCurTimeInMsec(),
                       tmpFreeFrameList.numFrames);
#endif
            DisplayLink_drvFreeRtParams(pObj,&tmpFreeFrameList);
        }
        for (i = 0; i < tmpFreeFrameList.numFrames;i++)
        {
            frame = tmpFreeFrameList.frames[i];
            if (DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM !=
                frame->channelNum)
            {
                frameInfo = frame->appData;
                UTILS_assert(UTILS_ARRAYISVALIDENTRY(frameInfo,pObj->frameInfo.infoMem));
                *freeQueId = frameInfo->activeQueueId;
                if (frameInfo->isFieldPair)
                {
                    UInt32 curTime;

                    curTime = Utils_getCurTimeInMsec();
                    UTILS_assert(freeFrameList->numFrames <
                                 UTILS_ARRAYSIZE(freeFrameList->frames));
                    freeFrameList->frames[freeFrameList->numFrames] =
                                 frameInfo->origFrame[0];
                    pObj->drvTopFieldLatency +=
                    (curTime - frameInfo->origFrame[0]->timeStamp);
                    pObj->numTopFields++;
                    freeFrameList->numFrames++;
                    UTILS_assert(freeFrameList->numFrames <
                                 UTILS_ARRAYSIZE(freeFrameList->frames));
                    freeFrameList->frames[freeFrameList->numFrames] =
                                 frameInfo->origFrame[1];
                    freeFrameList->numFrames++;
                    pObj->numBufsInDriver--;
                }
                else
                {
                    /* If not field pair, restore original appData */
                    frame->appData = frameInfo->origFrameInfo;
                    UTILS_assert(freeFrameList->numFrames <
                                 UTILS_ARRAYSIZE(freeFrameList->frames));
                    freeFrameList->frames[freeFrameList->numFrames] =
                                 frame;
                    freeFrameList->numFrames++;
                    pObj->numBufsInDriver--;
                }
                status =
                  Utils_quePut(&pObj->frameInfo.infoQ,
                               frameInfo,
                               BIOS_NO_WAIT);
                UTILS_assert(status == 0);
            }
        }
    }
    return status;
}

Int32 DisplayLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    DisplayLink_Obj *pObj = (DisplayLink_Obj *) appData;
    UInt32 elaspedTime, curTime;

    Utils_tskSendCmd(&pObj->tsk, DISPLAY_LINK_CMD_DO_DEQUE);

    pObj->cbCount++;

    curTime = Utils_getCurTimeInMsec();

    if (pObj->cbCount > 10)
    {
        elaspedTime = curTime - pObj->lastCbTime;

        if (elaspedTime > pObj->maxCbTime)
            pObj->maxCbTime = elaspedTime;

        if (elaspedTime < pObj->minCbTime)
            pObj->minCbTime = elaspedTime;
    }

    pObj->lastCbTime = curTime;

#if AVSYNC_COMP_ENABLE
    if ( pAvSyncInfo_obj->avSyncCompEnable )
    {
        if(pAvSyncInfo_obj->timeInfo.activeRefClk.clkType == AVSYNC_TIME_ClkTypeVideo) {
            Video_ISR(); // media time update
        }
    }
#endif

    return FVID2_SOK;
}

Int32 DisplayLink_drvSwitchCh(DisplayLink_Obj * pObj, DisplayLink_SwitchChannelParams *prm)
{
	UInt32 curInQueId;
	UInt32 prevLinkQueId;

    DisplayLink_drvLock(pObj);

	curInQueId    = pObj->curActiveQueue;
	prevLinkQueId = pObj->createArgs.inQueParams[curInQueId].prevLinkQueId;	
	
    if(prm->activeChId < pObj->inTskInfo[curInQueId].queInfo[prevLinkQueId].numCh)
	{
		pObj->curDisplayChannelNum = prm->activeChId;
	}
	else
	{
		Vps_printf("DISPLAY :curActiveQueue %d is greater than numCh %d\n",
				   prm->activeChId,pObj->inTskInfo[curInQueId].queInfo[prevLinkQueId].numCh);
	}	

    DisplayLink_drvUnlock(pObj);

    return FVID2_SOK;
}


Int32 DisplayLink_drvSwitchActiveQueue(DisplayLink_Obj * pObj,
                                       DisplayLink_SwitchActiveQueueParams *prm)
{
    Int32 status = FVID2_SOK;

    DisplayLink_drvLock(pObj);

    if (prm->activeQueueId < pObj->createArgs.numInputQueues)
    {
        if (NULL != pObj->topField)
        {
            System_LinkInQueParams *pInQueParams;
            UInt32 freeQueueId = pObj->curActiveQueue;
            FVID2_FrameList freeFrameList;
            Int32 status;

            UTILS_assert(freeQueueId <
                         pObj->createArgs.numInputQueues);
            pInQueParams = &pObj->createArgs.inQueParams[freeQueueId];
            freeFrameList.numFrames = 1;
            freeFrameList.frames[0] = pObj->topField;
            status =
                System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId,
                                       &freeFrameList);
            UTILS_assert(status == 0);
            pObj->topField = NULL;
        }
        pObj->nextFid        = FVID2_FID_TOP;
        pObj->curActiveQueue = prm->activeQueueId;
    }
    else
    {
        status = -1;
    }

    DisplayLink_drvUnlock(pObj);

    return status;
}

Int32 DisplayLink_drvDisplayAllocAndQueBlankFrame(DisplayLink_Obj * pObj)
{
    FVID2_FrameList frameList;
    UInt32 numQue;
    Int32 status;
    
    for (numQue = 0; numQue < DISPLAY_LINK_BLANK_FRAME_INIT_QUE; numQue++)
    {
		if(pObj->displayCreateArgs.memType != VPS_VPDMA_MT_TILEDMEM)
		{
			status = System_getBlankFrame(&pObj->blankFrame,0);
		}
		else
		{
			status = System_getBlankFrame(&pObj->blankFrame,1);
		}
		
		UTILS_assert(status == FVID2_SOK);	
		
		pObj->blankFrame.channelNum = DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM;
	
        frameList.numFrames = 1;
        frameList.frames[0] = &pObj->blankFrame;
        frameList.perListCfg = NULL;

        if(pObj->displayCreateArgs.memType != VPS_VPDMA_MT_TILEDMEM)
        {
            pObj->blankFrame.addr[1][0] =
            (UInt8 *) pObj->blankFrame.addr[0][0] +
            pObj->displayFormat.pitch[0];
        }
        else
        {
            pObj->blankFrame.addr[1][0] = pObj->blankFrame.addr[0][0];
            pObj->blankFrame.addr[1][1] = pObj->blankFrame.addr[0][1];
        }
		
        pObj->numBufsInDriver++;
        status = FVID2_queue(pObj->displayHndl,&frameList,0);
        UTILS_assert(status == FVID2_SOK);
        pObj->maxQueueCount--;
    }

    return status;
}

static
Int32 DisplayLink_drvSetDeiDispPrms(DisplayLink_Obj *pObj, FVID2_Format *pFormat)
{
    Int32              status;
    Vps_DeiDispParams *deiDispPrms = &pObj->deiDispPrms;

    deiDispPrms->fmt = *pFormat;

    deiDispPrms->comprEnable = FALSE;
    deiDispPrms->drnEnable = FALSE;
    deiDispPrms->scCfg.bypass = FALSE;
    deiDispPrms->scCfg.nonLinear = FALSE;
    deiDispPrms->scCfg.stripSize = 0;
    deiDispPrms->scCfg.vsType = VPS_SC_VST_POLYPHASE;
    deiDispPrms->scCfg.hsType = VPS_SC_HST_AUTO;
    deiDispPrms->scCfg.enablePeaking = FALSE;
    deiDispPrms->scCfg.enableEdgeDetect = FALSE;

    deiDispPrms->deiHqCfg = NULL;
    deiDispPrms->deiCfg = NULL;

    deiDispPrms->scTarWidth = pFormat->width;
    deiDispPrms->scTarHeight = pFormat->height;


    status = FVID2_control(
                 pObj->displayHndl,
                 IOCTL_VPS_DEI_DISP_SET_PARAMS,
                 deiDispPrms,
                 NULL);

    return (status);
}

Int32 DisplayLink_drvSetFmt(DisplayLink_Obj * pObj, FVID2_Format *pFormat)
{

    Int32 status;

    /* For the DEI Display instances, set the DEI params */
    if (DisplayLink_drvIsDeiDisplayDrv(pObj))
    {
        status = DisplayLink_drvSetDeiDispPrms(pObj, pFormat);
    }
    else
    {
        status = FVID2_setFormat(pObj->displayHndl, pFormat);
    }

    UTILS_assert(status == FVID2_SOK);
    return(status);
}

Int32 DisplayLink_drvSetCoeff(DisplayLink_Obj *pObj)
{
    Int32                   status;
    Vps_ScCoeffParams       coeffPrms;

    /* Set the scaling coefficients,
       it is assumed here that the scalar is used for upscaling, so setting
       upscaling coefficients */
    coeffPrms.hScalingSet = VPS_SC_US_SET;
    coeffPrms.vScalingSet = VPS_SC_US_SET;

    coeffPrms.coeffPtr = NULL;
    coeffPrms.scalarId = VPS_M2M_DEI_SCALAR_ID_DEI_SC;

    status = FVID2_control(
                 pObj->displayHndl,
                 IOCTL_VPS_SET_COEFFS,
                 &coeffPrms,
                 NULL);

    return (status);
}


static Void DisplayLink_prdCalloutFcn(UArg arg)
{
    DisplayLink_Obj *pObj = (DisplayLink_Obj *) arg;

    UTILS_assert(pObj->isDisplayRunning == FALSE);

    Utils_tskSendCmd(&pObj->tsk, DISPLAY_LINK_CMD_DO_DEQUE);
}

static Int32 DisplayLink_drvCreatePrdObj(DisplayLink_Obj * pObj)
{
    Clock_Params clockParams;

    Clock_Params_init(&clockParams);
    clockParams.arg = (UArg) pObj;
    UTILS_assert(pObj->prd.clkHandle == NULL);

    Clock_construct(&(pObj->prd.clkStruct),
                    DisplayLink_prdCalloutFcn, 1, &clockParams);

    pObj->prd.clkHandle = Clock_handle(&pObj->prd.clkStruct);
    pObj->prd.clkStarted = FALSE;

    Clock_setPeriod(pObj->prd.clkHandle, DISPLAY_LINK_DONE_PERIOD_MS);
    Clock_setTimeout(pObj->prd.clkHandle, DISPLAY_LINK_DONE_PERIOD_MS);

    return FVID2_SOK;
}

static Int32 DisplayLink_drvDeletePrdObj(DisplayLink_Obj * pObj)
{

    if(pObj->prd.clkStarted)
    {
        Clock_stop(pObj->prd.clkHandle);
    }

    Clock_destruct(&(pObj->prd.clkStruct));
    pObj->prd.clkHandle = NULL;
    pObj->prd.clkStarted = FALSE;

    return FVID2_SOK;
}


Int32 DisplayLink_drvDisplayCreate(DisplayLink_Obj * pObj)
{
    Int32 status;
    UInt32 dispId[3] = {VPS_DISP_INST_BP0, VPS_DISP_INST_BP1, VPS_DISP_INST_SEC1};
    FVID2_CbParams cbParams;
    System_LinkChInfo *pInChInfo;
    FVID2_Format *pFormat;

    pInChInfo = &pObj->inQueInfo.chInfo[0];

    memset(&cbParams, 0, sizeof(cbParams));
    cbParams.cbFxn = DisplayLink_drvFvidCb;
    cbParams.appData = pObj;

    pObj->displayCreateArgs.memType = pInChInfo->memType;
    pObj->displayCreateArgs.periodicCallbackEnable = TRUE;

    pFormat = &pObj->displayFormat;

	if(pInChInfo->height > 1080)
		pInChInfo->height = 1080;
	
    pFormat->channelNum = 0;
    pFormat->width      = pInChInfo->width;
    pFormat->height     = pInChInfo->height;
    pFormat->pitch[0]   = pInChInfo->pitch[0];
    pFormat->pitch[1]   = pInChInfo->pitch[1];
    pFormat->pitch[2]   = pInChInfo->pitch[2];
    pFormat->dataFormat = pInChInfo->dataFormat;

	Vps_rprintf("DisplayId = %d,DataFormat = %d\n",pObj->createArgs.displayId,pInChInfo->dataFormat);
			
	/*
	 * DISPLAY_LINK_DISPLAY_BP0/BP1 supports FVID2_DF_YUV422I_UYVY and FVID2_DF_YUV422I_YUYV formats.
	 * DISPLAY_LINK_DISPLAY_SC1/SC2 supports FVID2_DF_YUV422I_YUYV and FVID2_DF_YUV420SP_UV formats.
	 * Over riding the displayId based on the data format for HD resolutions
	 */
	if(pObj->createArgs.displayId != DISPLAY_LINK_DISPLAY_SD)
	{		
		if((pFormat->dataFormat == FVID2_DF_YUV422I_UYVY) || 
		   (pFormat->dataFormat == FVID2_DF_YUV422I_YUYV))
		{
			if((pObj->createArgs.displayId != DISPLAY_LINK_DISPLAY_BP0) &&
			   (pObj->createArgs.displayId != DISPLAY_LINK_DISPLAY_BP1))
			{
				pObj->createArgs.displayId = DISPLAY_LINK_DISPLAY_BP0;    
			}	
		}	
		else if((pFormat->dataFormat == FVID2_DF_YUV422I_YUYV) || 
		        (pFormat->dataFormat == FVID2_DF_YUV420SP_UV))
		     {
			     if((pObj->createArgs.displayId != DISPLAY_LINK_DISPLAY_SC1) &&
					(pObj->createArgs.displayId != DISPLAY_LINK_DISPLAY_SC2))
				 {		
				     pObj->createArgs.displayId = DISPLAY_LINK_DISPLAY_SC2;    
				 }	
		     }
	}	
	
    switch (pObj->createArgs.displayId)
    {
        default:
        case DISPLAY_LINK_DISPLAY_AUTO_SELECT:
            pObj->displayInstId = dispId[pObj->tskId -
                                            SYSTEM_LINK_ID_DISPLAY_FIRST];
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
			Vps_rprintf("Auto displayInstId = %d\n",pObj->displayInstId);
            break;
        case DISPLAY_LINK_DISPLAY_BP0:
            pObj->displayInstId = VPS_DISP_INST_BP0;
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
			Vps_rprintf("BP0 displayInstId = %d\n",pObj->displayInstId);
            break;
        case DISPLAY_LINK_DISPLAY_BP1:
            pObj->displayInstId = VPS_DISP_INST_BP1;
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
			Vps_rprintf("BP1 displayInstId = %d\n",pObj->displayInstId);
            break;
        case DISPLAY_LINK_DISPLAY_SC1:
            pObj->displayInstId = DisplayLink_getSc1DispId();
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
			Vps_rprintf("SC1 displayInstId = %d\n",pObj->displayInstId);
            break;
        case DISPLAY_LINK_DISPLAY_SC2:
            pObj->displayInstId = DisplayLink_getSc2DispId();
            pFormat->scanFormat = FVID2_SF_PROGRESSIVE;
			Vps_rprintf("SC2 displayInstId = %d\n",pObj->displayInstId);
            break;
        case DISPLAY_LINK_DISPLAY_SD:
            pObj->displayInstId = VPS_DISP_INST_SEC1;
            pFormat->scanFormat = FVID2_SF_INTERLACED;
			Vps_rprintf("SD displayInstId = %d\n",pObj->displayInstId);
            break;
    }

    pFormat->fieldMerged[0] = FALSE;

    if (pObj->createArgs.displayRes == VSYS_STD_1080I_60
        ||
        pObj->createArgs.displayRes == VSYS_STD_NTSC
        ||
        pObj->createArgs.displayRes == VSYS_STD_PAL)
    {
        Int i;

        pFormat->scanFormat = FVID2_SF_INTERLACED;
        pFormat->fieldMerged[0] = TRUE;
        if (DisplayLink_drvIsSDDef(pObj))
        {
            for (i = 0; i < pObj->createArgs.numInputQueues; i++)
            {
                /* if we are going to get field input we should
                 * not enable field merged mode in the driver
                 * since we need to seamlessly switch from
                 * one input queue to another and changing
                 * field merged mode is not supported at
                 * runtime.
                 */
                if (pObj->fieldInput[i] == TRUE)
                {
                    pFormat->fieldMerged[0] = FALSE;
                }
            }
            if (DisplayLink_drvDoInputPitchDoubling(pObj))
            {
                pFormat->pitch[0] *= 2;
                pFormat->pitch[1] *= 2;
                pFormat->pitch[2] *= 2;
            }
        }
    }
    if (pObj->createArgs.displayRes == VSYS_STD_720P_60)
    {
        pFormat->width = 1280;
        pFormat->height = 720;
    }
    if (pObj->createArgs.displayRes == VSYS_STD_720P_60)
    {
        pFormat->width = 1280;
        pFormat->height = 720;
    }
    if (pObj->createArgs.displayRes == VSYS_STD_NTSC)
    {
        pFormat->width = 720;
        pFormat->height = 480;
    }
    if (pObj->createArgs.displayRes == VSYS_STD_PAL)
    {
        pFormat->width = 720;
        pFormat->height = 576;
    }

    pFormat->fieldMerged[1] = pFormat->fieldMerged[0];
    pFormat->fieldMerged[2] = pFormat->fieldMerged[0];
    pFormat->bpp = FVID2_BPP_BITS16;
    pFormat->reserved = NULL;

#ifdef SYSTEM_VERBOSE_PRINTS
    Vps_printf(" %d: DISPLAY: %d: Window size %dx%d, %dB",
               Utils_getCurTimeInMsec(),
               pObj->displayInstId, pFormat->width, pFormat->height,
               pFormat->pitch[0]);
#endif

    pObj->displayHndl = FVID2_create(FVID2_VPS_DISP_DRV,
                                     pObj->displayInstId,
                                     &pObj->displayCreateArgs,
                                     &pObj->displayCreateStatus, &cbParams);
    UTILS_assert(pObj->displayHndl != NULL);
    pObj->maxQueueCount = pObj->displayCreateStatus.maxReqInQueue;

    /* Set the Scalar Coefficients */
    /* For the DEI Display instances, set the DEI params */
    if (DisplayLink_drvIsDeiDisplayDrv(pObj))
    {
        status = DisplayLink_drvSetCoeff(pObj);
        UTILS_assert(status == FVID2_SOK);
    }

    status = DisplayLink_drvSetFmt(pObj, pFormat);
    UTILS_assert(status == FVID2_SOK);

    pObj->queueCount = 0;

    return FVID2_SOK;
}


static
Bool DisplayLink_drvIsFieldSeparatedInput(DisplayLink_Obj * pObj,
                                          System_LinkQueInfo *queInfo)
{
    Bool isFieldSeparatedInput = FALSE;
    UInt32 fieldMergedHeightNTSC;
    UInt32 fieldMergedHeightPAL;

    UTILS_assert(DisplayLink_drvIsSDDef(pObj) == TRUE);
    fieldMergedHeightNTSC = 240;
    fieldMergedHeightPAL  = 288;

    pObj->enableFieldSeparatedInputMode = FALSE;

    if (queInfo->numCh > 0)
    {
        if(SYSTEM_SF_INTERLACED == queInfo->chInfo[0].scanFormat
        )
        {
            if ((queInfo->chInfo[0].height ==  fieldMergedHeightNTSC)
                ||
                (queInfo->chInfo[0].height ==  fieldMergedHeightPAL))
            {
                isFieldSeparatedInput = TRUE;
            }
        }
        if(pObj->createArgs.forceFieldSeparatedInputMode)
        {
            isFieldSeparatedInput = TRUE;
            pObj->enableFieldSeparatedInputMode = TRUE;
        }

    }
    return isFieldSeparatedInput;
}

static
Void DisplayLink_drvSetFieldSeparatedInfo(DisplayLink_Obj * pObj)
{
    Int i,status;
    DisplayLink_CreateParams * pPrm;

    pPrm = &pObj->createArgs;
    for (i = 0; i < pObj->createArgs.numInputQueues;i++)
    {
        status = System_linkGetInfo(pPrm->inQueParams[i].prevLinkId,
                                    &pObj->inTskInfo[i]);
        UTILS_assert(status == FVID2_SOK);
        if (DisplayLink_drvIsSDDef(pObj))
        {
            pObj->fieldInput[i] =
            DisplayLink_drvIsFieldSeparatedInput (pObj,
              &pObj->inTskInfo[i].queInfo[pPrm->inQueParams[i].prevLinkQueId]);

            if(pObj->fieldInput[i])
            {
                Vps_printf(" %d: DISPLAY: %d: Field Separate Mode ENABLED for Que %d !!!\n",
                    Utils_getCurTimeInMsec(),
                    SYSTEM_GET_LINK_ID(pObj->tskId),
                    i
                  );
            }
        }
        else
        {
            pObj->fieldInput[i] = FALSE;
        }
    }
}

Int32 DisplayLink_drvCreate(DisplayLink_Obj * pObj,
                            DisplayLink_CreateParams * pPrm)
{
    Semaphore_Params semParams;
    Int32 status = FVID2_SOK;
    DisplayLink_SwitchChannelParams switchParams;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Create in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    pObj->cbCount = 0;
    pObj->dequeCount = 0;
    pObj->totalTime = 0;
    pObj->minCbTime = 0xFF;
    pObj->maxCbTime = 0;
    pObj->lastCbTime = 0;
    pObj->maxLatency = 0;
    pObj->minLatency = 0xFF;
    pObj->drvTopFieldLatency = 0;
    pObj->numTopFields = 0;

    pObj->inFrameGetCount = 0;
    pObj->inFramePutCount = 0;
    pObj->isDisplayRunning = FALSE;

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    UTILS_assertError((pPrm->activeQueue < UTILS_ARRAYSIZE(pPrm->inQueParams)),
                      status,
                      -1,
                      pObj->tskId,
                      -1);
    if (status != FVID2_SOK)
    {
        pPrm->activeQueue = 0;
    }
    DisplayLink_drvSetFieldSeparatedInfo(pObj);
    status = System_linkGetInfo(pPrm->inQueParams[pPrm->activeQueue].prevLinkId,
                                &pObj->inTskInfo[pPrm->activeQueue]);
    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams[pPrm->activeQueue].prevLinkQueId <
                 pObj->inTskInfo[pPrm->activeQueue].numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo[pPrm->activeQueue].queInfo[pPrm->inQueParams[pPrm->activeQueue].prevLinkQueId],
           sizeof(pObj->inQueInfo));
    pObj->curActiveQueue = pPrm->activeQueue;
    pObj->numBufsInDriver = 0;
    pObj->topField = NULL;
    pObj->nextFid  = FVID2_FID_TOP;
    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->lock = Semaphore_create(1u, &semParams, NULL);
    UTILS_assert(pObj->lock != NULL);

    Semaphore_Params_init(&semParams);

    semParams.mode = Semaphore_Mode_BINARY;

    pObj->complete = Semaphore_create(0u, &semParams, NULL);
    UTILS_assert(pObj->complete != NULL);

    DisplayLink_drvDisplayCreate(pObj);
    DisplayLink_drvCreateFrameInfo(pObj);
    DisplayLink_drvCreateRtParams(pObj);
    DisplayLink_drvDisplayAllocAndQueBlankFrame(pObj);
    switchParams.activeChId = 0;
    DisplayLink_drvSwitchCh(pObj, &switchParams);
    DisplayLink_drvSetPitchInfo(pObj);

    DisplayLink_drvCreatePrdObj(pObj);

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Create Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}

Int32 DisplayLink_drvPrintRtStatus(DisplayLink_Obj * pObj, UInt32 elaspedTime)
{
    UInt32 displayId;
    char *displayName[] = { "HDDAC(BP0) ", "DVO2(BP1)  ", "SDDAC(SEC1)", "PRIMARY PATH", "AUX PATH" };

    displayId = pObj->displayInstId;
    if ((displayId == VPS_DISP_INST_MAIN_DEIH_SC1) ||
        (displayId == VPS_DISP_INST_MAIN_DEI_SC1))
    {
        displayId = 3;
    }
    if ((displayId == VPS_DISP_INST_AUX_DEI_SC2) ||
        (displayId == VPS_DISP_INST_AUX_SC2))
    {
        displayId = 4;
    }

    Vps_rprintf(" %d: DISPLAY: %s: %d fps, Latency (Min / Max) = ( %d / %d ), Callback Interval (Min / Max) = ( %d / %d ) !!! \r\n",
         Utils_getCurTimeInMsec(),
         displayName[displayId],
        pObj->cbCount*1000/elaspedTime,
        pObj->minLatency,
        pObj->maxLatency,
        pObj->minCbTime,
        pObj->maxCbTime
        );

    /* reset max time */
    pObj->maxCbTime = 0;
    pObj->minCbTime = 0xFF;
    pObj->maxLatency = 0;
    pObj->minLatency = 0xFF;

    if(pObj->displayInstId==0)
    {
        System_displayUnderflowPrint(TRUE, TRUE);
        System_memPrintHeapStatus();
    }

    return 0;
}

Int32 DisplayLink_drvPrintStatistics(DisplayLink_Obj * pObj)
{
    UInt32 elaspedTime;

    elaspedTime = Utils_getCurTimeInMsec() - pObj->startTime;

    DisplayLink_drvPrintRtStatus(pObj, elaspedTime);

    return 0;
}

Int32 DisplayLink_drvProcessData(DisplayLink_Obj * pObj)
{
    FVID2_FrameList frameList;
    FVID2_FrameList freeFrameList;
    FVID2_FrameList displayFrameList;
    UInt32 freeFrameNum, elaspedTime;
    System_LinkInQueParams *pInQueParams;
    FVID2_Frame *pFrame;
    Int32 status;
    UInt32 latency;
    Int i;
    UInt32 frameIdx;
    System_LinkChInfo *pInChInfo = &pObj->inQueInfo.chInfo[0];

    if(pObj->isDisplayRunning)
    {
        elaspedTime = Utils_getCurTimeInMsec() - pObj->startTime;

        if ((elaspedTime - pObj->prevTime) > 24 * 60 * 60 * 1000)
        {
            DisplayLink_drvPrintRtStatus(pObj, elaspedTime);

            pObj->prevTime = elaspedTime;
        }

        if(pObj->displayInstId==0)
        {
            System_displayUnderflowCheck(FALSE);
        }

        do
        {
            UInt32 freeQueueId;
            /* dequeue all completed frames */
            status = DisplayLink_drvDeQueue(pObj,&freeFrameList,
                                            &freeQueueId);

            if (freeFrameList.numFrames)
            {
                pObj->inFramePutCount += freeFrameList.numFrames;

                UTILS_assert(freeQueueId <
                             pObj->createArgs.numInputQueues);
                pInQueParams = &pObj->createArgs.inQueParams[freeQueueId];
                System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                           pInQueParams->prevLinkQueId, &freeFrameList);
            }

        } while (status == FVID2_SOK);


        /* Free frames queued in inactive queues immediately */
        for (i = 0; i < pObj->createArgs.numInputQueues;i++)
        {
            if (i != pObj->curActiveQueue)
            {
                frameList.numFrames = 0;
                pInQueParams =
                  &pObj->createArgs.inQueParams[i];
                System_getLinksFullFrames(pInQueParams->prevLinkId,
                                          pInQueParams->prevLinkQueId,
                                          &frameList);
                if (frameList.numFrames)
                {
                    pObj->inFrameGetCount += frameList.numFrames;
                    pObj->inFramePutCount += frameList.numFrames;
                    System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                               pInQueParams->prevLinkQueId,
                                               &frameList);
                }
            }
        }
        UTILS_assert(pObj->curActiveQueue < pObj->createArgs.numInputQueues);
        pInQueParams = &pObj->createArgs.inQueParams[pObj->curActiveQueue];
        /* que frames if any */

        System_getLinksFullFrames(pInQueParams->prevLinkId,
                                  pInQueParams->prevLinkQueId, &frameList);
        pObj->inFrameGetCount += frameList.numFrames;

        #ifdef ENABLE_FXN_PROFILE
            DisplayLink_drvFxnProfileControl(pObj);
        #endif /* #ifdef ENABLE_FXN_PROFILE */
        freeFrameNum = 0;

        DisplayLink_drvLock(pObj);
        frameIdx = 0;
        while((pObj->maxQueueCount)
              &&
              (frameIdx < frameList.numFrames))
        {
            pFrame = frameList.frames[frameIdx];
            frameIdx++;
            UTILS_assert(pFrame != NULL);
            if (pFrame->channelNum == pObj->curDisplayChannelNum)
            {
                Bool frameReject;
                UInt32 pitch0,pitch1;

                pitch0 = pObj->displayFormat.pitch[0];
                pitch1 = pObj->displayFormat.pitch[1];
                if (DisplayLink_drvDoInputPitchDoubling(pObj))
                {
                    pitch0 /= 2;
                    pitch1 /= 2;
                }

                UTILS_assert(DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM !=
                             pFrame->channelNum);
				pFrame->addr[0][0] = (Ptr)((UInt32)pFrame->addr[0][0] + pInChInfo->startY * pitch0 + pInChInfo->startX);
				pFrame->addr[0][1] = (Ptr)((UInt32)pFrame->addr[0][1] + pInChInfo->startY * pitch1/2 + pInChInfo->startX);
                pFrame->addr[1][0] =
                    (UInt8 *) pFrame->addr[0][0] + pitch0;
                pFrame->addr[1][1] =
                    (UInt8 *) pFrame->addr[0][1] + pitch1;
    #ifdef SYSTEM_DEBUG_DISPLAY_RT
                Vps_printf(" %d: DISPLAY: Queue %d frames\n", Utils_getCurTimeInMsec(),
                           displayFrameList.numFrames);
    #endif
                latency = Utils_getCurTimeInMsec() - pFrame->timeStamp;

                if(latency>pObj->maxLatency)
                    pObj->maxLatency = latency;
                if(latency<pObj->minLatency)
                    pObj->minLatency = latency;
                /* queue frame for display */
                displayFrameList.numFrames = 0;
                DisplayLink_drvSetFrameInfo(pObj,
                                            &pFrame,
                                            &frameReject);
                if (pFrame)
                {
                    if (frameReject)
                    {
                        UTILS_assert(freeFrameNum <
                                     UTILS_ARRAYSIZE(freeFrameList.frames));
                        /* error in queing to display, instead of asserting
                            release the frame and continue
                        */
                        freeFrameList.frames[freeFrameNum] = pFrame;
                        freeFrameNum++;
                    }
                    else
                    {
                        displayFrameList.frames[displayFrameList.numFrames] = pFrame;
                        displayFrameList.numFrames++;
                        displayFrameList.perListCfg = NULL;
                        DisplayLink_drvHandleDynamicPitchChange(pObj,&displayFrameList);
                        DisplayLink_validateFrameList(pObj,
                                                      &displayFrameList);
                        pObj->queueCount += displayFrameList.numFrames;
                        pObj->numBufsInDriver++;

                        status = FVID2_queue(pObj->displayHndl,
                                             &displayFrameList,
                                             0);
                        pObj->maxQueueCount--;

                        if(status!=FVID2_SOK)
                        {

                             Vps_rprintf(" %d: DISPLAY (%d): Queue to driver failed !!!\n", Utils_getCurTimeInMsec(),
                                   pObj->tskId);

                            UTILS_assert(freeFrameNum <
                                         UTILS_ARRAYSIZE(freeFrameList.frames));
                            /* error in queing to display, instead of asserting
                                release the frame and continue
                            */
                            freeFrameList.frames[freeFrameNum] = pFrame;
                            freeFrameNum++;
                            DisplayLink_drvFreeFrameInfo(pObj,
                                                         &displayFrameList);
                        }
                    }
               }
            }
            else
            {
                UTILS_assert(freeFrameNum <
                             UTILS_ARRAYSIZE(freeFrameList.frames));
                freeFrameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }
        DisplayLink_drvUnlock(pObj);

        for (i = frameIdx; i < frameList.numFrames;i++)
        {
            UTILS_assert(freeFrameNum < UTILS_ARRAYSIZE(freeFrameList.frames));
            freeFrameList.frames[freeFrameNum] = frameList.frames[i];
            freeFrameNum++;
        }
        if (freeFrameNum)
        {
            freeFrameList.numFrames = freeFrameNum;
            pObj->inFramePutCount += freeFrameList.numFrames;

            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                       pInQueParams->prevLinkQueId, &freeFrameList);
        }

    }
    else {

        /* Free frames queued in inactive queues immediately */
        for (i = 0; i < pObj->createArgs.numInputQueues;i++)
        {
            frameList.numFrames = 0;
            pInQueParams =
              &pObj->createArgs.inQueParams[i];
            System_getLinksFullFrames(pInQueParams->prevLinkId,
                                      pInQueParams->prevLinkQueId,
                                      &frameList);
            if (frameList.numFrames)
            {
                pObj->inFrameGetCount += frameList.numFrames;
                pObj->inFramePutCount += frameList.numFrames;
                System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                           pInQueParams->prevLinkQueId,
                                           &frameList);
            }
        }
    }


    return FVID2_SOK;
}

Int32 DisplayLink_drvDelete(DisplayLink_Obj * pObj)
{
    FVID2_FrameList frameList;
    Int32 status;

#ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DISPLAY: Frames = %d (fps = %d) !!!\n",
               Utils_getCurTimeInMsec(),
               pObj->dequeCount,
               pObj->dequeCount * 100 / (pObj->totalTime / 10));
#endif

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Delete in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    do
    {
        // dequeue queued buffer's
        status = FVID2_dequeue(pObj->displayHndl, &frameList, 0, BIOS_NO_WAIT);
    } while (status == FVID2_SOK);

    status = FVID2_delete(pObj->displayHndl, NULL);
    if (FVID2_SOK != status)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, status, __LINE__);
        return (status);
    }

    Semaphore_delete(&pObj->lock);
    Semaphore_delete(&pObj->complete);
    DisplayLink_drvDeleteFrameInfo(pObj);
    DisplayLink_drvDeleteRtParams(pObj);

    DisplayLink_drvDeletePrdObj(pObj);

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Delete Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}

Int32 DisplayLink_drvStart(DisplayLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Start in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    pObj->lastCbTime = Utils_getCurTimeInMsec();
    pObj->startTime = Utils_getCurTimeInMsec();
    pObj->prevTime = pObj->startTime;

    if(pObj->prd.clkStarted)
    {
        Clock_stop(pObj->prd.clkHandle);
        pObj->prd.clkStarted = FALSE;
    }

    status = FVID2_start(pObj->displayHndl, NULL);
    UTILS_assert(status == FVID2_SOK);

    pObj->isDisplayRunning = TRUE;

    pObj->totalTime = Utils_getCurTimeInMsec();

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Start Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return status;
}

Int32 DisplayLink_drvStop(DisplayLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Stop in progress !!!\n", Utils_getCurTimeInMsec());
#endif

    pObj->totalTime = Utils_getCurTimeInMsec() - pObj->totalTime;

    pObj->isDisplayRunning = FALSE;

    status = FVID2_stop(pObj->displayHndl, NULL);
    UTILS_assert(status == FVID2_SOK);

    if(pObj->prd.clkStarted == FALSE)
    {
        Clock_start(pObj->prd.clkHandle);
        pObj->prd.clkStarted = TRUE;
    }

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Stop Done !!!\n", Utils_getCurTimeInMsec());
#endif

    return status;
}

Int32 DisplayLink_drvLock(DisplayLink_Obj * pObj)
{
    return Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);
}

Int32 DisplayLink_drvUnlock(DisplayLink_Obj * pObj)
{
    Semaphore_post(pObj->lock);

    return FVID2_SOK;
}

Int32 DisplayLink_drvSwitchInputMode(DisplayLink_Obj * pObj, DisplayLink_SwitchInputMode *pPrm)
{
    if(pObj->createArgs.forceFieldSeparatedInputMode)
        pObj->enableFieldSeparatedInputMode = pPrm->enableFieldSeparatedInputMode;
    else
        pObj->enableFieldSeparatedInputMode = FALSE;

    return FVID2_SOK;
}

Int32 DisplayLink_printBufferStatus(DisplayLink_Obj * pObj)
{
    Vps_rprintf
        (
        " \n"
        " *** Display Statistics *** \n"
        "%d: DISPLAY: Rcvd from prev = %d, Returned to prev = %d, BufsInDriver = %d\r\n",
        Utils_getCurTimeInMsec(), pObj->inFrameGetCount, pObj->inFramePutCount, pObj->numBufsInDriver);

    return 0;
}

Int32 DisplayLink_devStop(DisplayLink_Obj *pObj)
{
	UInt32 i,queId;
	Int32 status = FVID2_SOK;
	DisplayLink_FrameInfo *pFrameInfo;
	System_LinkInQueParams *pInQueParams;
	FVID2_Frame *pFrame;
	FVID2_FrameList tmpFrameList,frameList;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Device Stop in progress !!!\n", Utils_getCurTimeInMsec());
#endif	
	
	pObj->isDisplayRunning = FALSE;
	
    if(pObj->prd.clkStarted == FALSE)
    {
        Clock_start(pObj->prd.clkHandle);
        pObj->prd.clkStarted = TRUE;
    }	

    status = FVID2_stop(pObj->displayHndl, NULL);
    UTILS_assert(status == FVID2_SOK);		
	
    do
    {
        status = FVID2_dequeue(pObj->displayHndl,&tmpFrameList,0,BIOS_NO_WAIT);		
		if(status == FVID2_SOK)
		{
			frameList.numFrames = 0;
			
			for(i = 0;i < tmpFrameList.numFrames;i++)
			{
				pFrame = tmpFrameList.frames[i];
				
				if(pFrame->channelNum != DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM)
				{
					pFrameInfo = pFrame->appData;
					queId = pFrameInfo->activeQueueId;	

					/* Restore original appData */
					pFrame->appData = pFrameInfo->origFrameInfo;	
					frameList.frames[frameList.numFrames] = pFrame;		
					frameList.numFrames ++;				
				}	
			}				
			
			pInQueParams = &pObj->createArgs.inQueParams[queId];		
			System_putLinksEmptyFrames(pInQueParams->prevLinkId,
									   pInQueParams->prevLinkQueId,
									   &frameList);		
		}						   
		
    } while(status == FVID2_SOK);
	
	status = FVID2_delete(pObj->displayHndl, NULL);
    if (FVID2_SOK != status)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, status, __LINE__);
        return status;
    }		
	
	DisplayLink_drvDeleteFrameInfo(pObj);
	
#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Device Stop DONE !!!\n", Utils_getCurTimeInMsec());
#endif
	
	return status;
}

Int32 DisplayLink_devStart(DisplayLink_Obj *pObj)
{
	Int32 status = FVID2_SOK;

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Device Start in progress !!!\n", Utils_getCurTimeInMsec());
#endif
	
	DisplayLink_drvDisplayCreate(pObj);
	DisplayLink_drvCreateFrameInfo(pObj);
	DisplayLink_drvDisplayAllocAndQueBlankFrame(pObj);	
	
    if(pObj->prd.clkStarted)
    {
        Clock_stop(pObj->prd.clkHandle);
        pObj->prd.clkStarted = FALSE;
    }

    status = FVID2_start(pObj->displayHndl, NULL);
    UTILS_assert(status == FVID2_SOK);

    pObj->isDisplayRunning = TRUE;	

#ifdef SYSTEM_DEBUG_DISPLAY
    Vps_printf(" %d: DISPLAY: Device Start DONE !!!\n", Utils_getCurTimeInMsec());
#endif
	
	return status;
}
