/** ==================================================================
 *  @file   swosdLink_tsk.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/swosd/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "swosdLink_priv.h"

#pragma DATA_ALIGN(gSwosdLink_tskStack, 32)
#pragma DATA_SECTION(gSwosdLink_tskStack, ".bss:taskStackSection")
UInt8 gSwosdLink_tskStack[SWOSD_LINK_OBJ_MAX][SWOSD_LINK_TSK_STACK_SIZE];

SwosdLink_Obj gSwosdLink_obj[SWOSD_LINK_OBJ_MAX];

extern Int32 isTiled[];
extern Int32 lineOffsets[];

int isIMXbasedSWOSDtoBeCalled = 0;
int isTransparencyEnabled[3] = { 0, 0, 0};
extern Semaphore_Handle cpisSync;
extern int isCPIS_GLBCE_deleted;

Int32 Swosd_drvPrintRtStatus(SwosdLink_Obj * pObj)
{

    Vps_rprintf(" %d: SWOSD:  Latency (Min / Max) = ( %d / %d ) !!! \r\n",
         Utils_getCurTimeInMsec(),
        pObj->minLatency,
        pObj->maxLatency
        );

    /* reset max time */

    pObj->maxLatency = 0;
    pObj->minLatency = 0xFF;

    return 0;
}

/* Swosd link create */

/* ===================================================================
 *  @func     SwosdLink_create
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_create(SwosdLink_Obj * pObj, SwosdLink_CreateParams * pPrm)
{
    Int32 status;

    UInt32 strId,j,swOsdEnable;

    UInt32 chId;

    SWOSD_config_t swOsdPrm;

    System_LinkChInfo *pChInfo;

    System_LinkQueInfo *pOutQueInfo;

    System_LinkChInfo *pInChInfo, *pOutChInfo;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    /* Get the link info from prev link */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue,           // pHndl
                             FALSE,                        // blockOnGet
                             FALSE);                       // blockOnPut

    UTILS_assert(status == FVID2_SOK);

    isIMXbasedSWOSDtoBeCalled = 0;
    /* SWOSD alg create */
    memset(&swOsdPrm, 0x00, sizeof(SWOSD_config_t));

    /* NOTE:Assuming numCh is same as no of streams */
    swOsdPrm.numCaptureStream = pObj->inQueInfo.numCh;
    swOsdPrm.autoFocusVal = 0;

    if ((pObj->inQueInfo.chInfo[0].dataFormat == FVID2_DF_YUV420SP_VU) ||
        (pObj->inQueInfo.chInfo[0].dataFormat == FVID2_DF_YUV420SP_UV))
    {
        swOsdPrm.captureYuvFormat = SWOSD_FORMATYUV420p;
    }
    else
    {
        swOsdPrm.captureYuvFormat = SWOSD_FORMATYUV422i;
    }

    for (strId = 0; strId < swOsdPrm.numCaptureStream; strId++)
    {
        pChInfo = &pObj->inQueInfo.chInfo[strId];

        swOsdEnable = 1;

        /*
           SWOSD should be applied on the no of streams generated and not on the
           no of stream encoded
        */
        for(j = 0;j < strId;j ++)
        {
		    if((swOsdPrm.swosdConfig[j].cropWidth == pChInfo->width) &&
		       (swOsdPrm.swosdConfig[j].cropHeight == pChInfo->height))
		    {
			    swOsdEnable = 0;
			}
		}

        swOsdPrm.swosdConfig[strId].swosdEnable = swOsdEnable;
        swOsdPrm.swosdConfig[strId].osd_count = 0;
        swOsdPrm.swosdConfig[strId].capLineOffsetY = pChInfo->pitch[0];
		swOsdPrm.swosdConfig[strId].capLineOffsetUV = pChInfo->pitch[1];
        swOsdPrm.swosdConfig[strId].capWidth = pChInfo->width;
        swOsdPrm.swosdConfig[strId].capHeight = pChInfo->height;
        swOsdPrm.swosdConfig[strId].cropWidth = pChInfo->width;
        swOsdPrm.swosdConfig[strId].cropHeight = pChInfo->height;
        swOsdPrm.swosdConfig[strId].codecType = ALG_VID_CODEC_NONE;
        swOsdPrm.swosdConfig[strId].rateControlType =
            ALG_VID_ENC_RATE_CONTROL_VBR;
        swOsdPrm.swosdConfig[strId].frameRateBase = 0;
        swOsdPrm.swosdConfig[strId].codecBitrate = 0;
        swOsdPrm.swosdConfig[strId].swosdType = SWOSD_BASIC;
        swOsdPrm.swosdConfig[strId].swosdTextPos = SWOSD_FMT_TOP_RIGHT;
        swOsdPrm.swosdConfig[strId].swosdLogoPos = SWOSD_FMT_TOP_LEFT;
        swOsdPrm.swosdConfig[strId].swosdDatePos = SWOSD_FMT_BOTTOM_LEFT;
        swOsdPrm.swosdConfig[strId].swosdTimePos = SWOSD_FMT_BOTTOM_RIGHT;
        swOsdPrm.swosdConfig[strId].swosdTimeFormat = SWOSD_FMT_24HR;
        swOsdPrm.swosdConfig[strId].swosdDateFormat = SWOSD_FMT_DMY;
        swOsdPrm.swosdConfig[strId].swosdDateEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdTransparencyEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdTimeEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdLogoEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdTextEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdEvalEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdLogoStringUpdate = OSD_YES;
        swOsdPrm.swosdConfig[strId].swosdDateTimeUpdate = OSD_YES;

		isTiled[strId] = FALSE;
		
		if(pChInfo->memType == SYSTEM_MT_TILEDMEM)
            isTiled[strId] = TRUE;
			
        swOsdPrm.swosdConfig[strId].memType = isTiled[strId];
		lineOffsets[strId] = pChInfo->pitch[0];
	    UTILS_assert(SWOSD_imxCreate(strId, isTiled[strId]/*isTiled*/,lineOffsets[strId]/*Video Buffer Pitch*/) == 0); 
    }

    status = DM81XX_SWOSD_create(&swOsdPrm);
    UTILS_assert(status == SWOSD_SOK);

    /* Populate the link's info structure */
    pObj->info.numQue = 1;

    pOutQueInfo = &pObj->info.queInfo[0];
    pObj->info.queInfo[0].numCh = pObj->inQueInfo.numCh;

    for (chId = 0; chId < pObj->info.queInfo[0].numCh; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];
        pOutChInfo = &pOutQueInfo->chInfo[chId];

        memcpy(pOutChInfo, pInChInfo, sizeof(System_LinkChInfo));
    }
	
    pObj->minLatency = 0xFF;
    pObj->maxLatency = 0;

#ifdef SYSTEM_DEBUG_SWOSD
    Vps_printf(" %d: SWOSD   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Swosd process frames */

/* ===================================================================
 *  @func     SwosdLink_processFrames
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_processFrames(SwosdLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

    UInt32 frameId ,latency;

	Int32 Width = 0,Height = 0,Pitch0 =0,Pitch1 = 0;

    FVID2_FrameList frameList;

    FVID2_Frame *pFullFrame;

    System_LinkInQueParams *pInQueParams;

    System_LinkQueInfo *pInQueInfo;

    System_LinkChInfo *pChInfo;

	System_FrameInfo *pInFrameInfo;

    Bool rtParamUpdatePerFrame = FALSE;
	
	UInt32 histData;
	
	Ptr fullFrameAddrY,fullFrameAddrUV;
	
	Ptr swosdApplyAddrY,swosdApplyAddrUV;

    pInQueParams = &pObj->createArgs.inQueParams;
    pInQueInfo = &pObj->inTskInfo.queInfo[pInQueParams->prevLinkQueId];

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        /* Apply SWOSD on the full frames */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFullFrame = frameList.frames[frameId];

            pChInfo 	 = &pInQueInfo->chInfo[pFullFrame->channelNum];
			pInFrameInfo = (System_FrameInfo *) pFullFrame->appData;

			if ((pInFrameInfo != NULL) && (pInFrameInfo->rtChInfoUpdate == TRUE))
            {				
				DM81XX_SWOSD_getResolution(pFullFrame->channelNum,&Pitch0,&Pitch1,&Width,&Height);
				
				if (pInFrameInfo->rtChInfo.height != Height)
                {
                    Height = pInFrameInfo->rtChInfo.height;
					rtParamUpdatePerFrame = TRUE;					
                }

                if (pInFrameInfo->rtChInfo.width != Width)
                {
                    Width = pInFrameInfo->rtChInfo.width;
					rtParamUpdatePerFrame = TRUE;
                }

                if (pInFrameInfo->rtChInfo.pitch[0] != Pitch0)
                {
                    Pitch0 = pInFrameInfo->rtChInfo.pitch[0];
					rtParamUpdatePerFrame = TRUE;
                }

                if (pInFrameInfo->rtChInfo.pitch[1] != Pitch1)
                {
                    Pitch1 = pInFrameInfo->rtChInfo.pitch[1];
					rtParamUpdatePerFrame = TRUE;
                }
				
				if(rtParamUpdatePerFrame == TRUE)
				{
					DM81XX_SWOSD_Reset(pFullFrame->channelNum,Pitch0,Pitch1,Width,Height);
					SWOSD_imxUpdateLineOffset(pFullFrame->channelNum, Pitch0);
				}	
            }
			if ((pInFrameInfo != NULL) && (pInFrameInfo->rtChInfoUpdate == FALSE))
            {				
				Height = pChInfo->height;
				Width  = pChInfo->width;
				Pitch0 = pChInfo->pitch[0];
				Pitch1 = pChInfo->pitch[1];
			}
			
			swosdApplyAddrY  = fullFrameAddrY  = (Ptr)((UInt32)pFullFrame->addr[0][0] + 	
													   (pChInfo->startX + (pChInfo->startY * pChInfo->pitch[0])));
			swosdApplyAddrUV = fullFrameAddrUV = (Ptr)((UInt32)pFullFrame->addr[0][1] + 
			                                           (pChInfo->startX + (pChInfo->startY/2 * pChInfo->pitch[1])));
						
			if(pChInfo->memType == SYSTEM_MT_TILEDMEM)
			{
				fullFrameAddrY  = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)fullFrameAddrY);
				fullFrameAddrUV = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)fullFrameAddrUV);
				
				if(DM81XX_SWOSD_isTransparencyEnable(pFullFrame->channelNum) == 0)
				{
					/*
					 * If transperency is OFF then it is going to be EDMA cpy based SWOSD apply.
					 * For EDMA we need to supply Tiler CPU address.
                     */					 
					swosdApplyAddrY  = fullFrameAddrY;
					swosdApplyAddrUV = fullFrameAddrUV;
				}
			}
			
            /* Draw the faces for FD */
            status = DM812X_FD_Draw(pFullFrame->channelNum,
                                    fullFrameAddrY, 	
                                    fullFrameAddrUV,	
                                    Width,
                                    Height,
                                    pChInfo->dataFormat,
                                    Pitch0, pChInfo->codingformat);

            UTILS_assert(status == FVID2_SOK);

			DM81XX_SWOSD_apply(pFullFrame->channelNum,     	// stream Id
							   (Int32)swosdApplyAddrY,	  	// Y plane address
							   (Int32)swosdApplyAddrUV); 	// UV plane address
			
			/* Draw the histogram */						
			if(pFullFrame->channelNum == 0)
			{
                CameraLink_getHistData(&histData);
							
				DM81XX_SWOSD_drawHistogram(pFullFrame->channelNum,     			// stream Id
										   (Int32) fullFrameAddrY,	  			// Y plane
										   (Int32) fullFrameAddrUV, 			// UV plane
										   (Int32*) histData);
            }									   
			
			latency = Utils_getCurTimeInMsec() - pFullFrame->timeStamp;
			if(latency>pObj->maxLatency)
				pObj->maxLatency = latency;
			if(latency<pObj->minLatency)
				pObj->minLatency = latency;
				
            /* Put the full buffer into full queue */
            status = Utils_bufPutFullFrame(&pObj->outFrameQue, pFullFrame);

            UTILS_assert(status == FVID2_SOK);
        }

        /* send SYSTEM_CMD_NEW_DATA to next link */
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
    }

    return FVID2_SOK;
}

/* Swosd link drv delete */

/* ===================================================================
 *  @func     SwosdLink_delete
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_delete(SwosdLink_Obj * pObj)
{
    Int32 status;	
	Int32 i = 0;


    /* Delete the buffers */
    status = Utils_bufDelete(&pObj->outFrameQue);

    UTILS_assert(status == FVID2_SOK);

    /* SW OSD Alg Delete */
    DM81XX_SWOSD_delete();
	
	for(i = 0; i < pObj->inQueInfo.numCh; i++)
	  SWOSD_imxDelete(i);

#ifdef SYSTEM_DEBUG_SWOSD
    Vps_printf(" %d: SWOSD   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}
void synchronizeTheCPIS(SwosdLink_GuiParams *pSwOsdGuiPrm, UInt32 streamId)
{
	isTransparencyEnabled[streamId] = pSwOsdGuiPrm->transparencyEnable;				
    Semaphore_pend(cpisSync, BIOS_WAIT_FOREVER ); 
	isIMXbasedSWOSDtoBeCalled = isTransparencyEnabled[0] || isTransparencyEnabled[1] || isTransparencyEnabled[2];  
    Semaphore_post(cpisSync); 
	
	if(isIMXbasedSWOSDtoBeCalled)
	{
	    do
		{
		}while(!isCPIS_GLBCE_deleted);
	}
	
	return;

}
 
/* Swosd link task main function */

/* ===================================================================
 *  @func     SwosdLink_tskMain
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Void SwosdLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status;

    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    SwosdLink_GuiParams *pSwOsdGuiPrm;

    SwosdLink_dateTime *pDateTimePrm;
	
	SwosdLink_vaStrPrm *pVaStrPrm;

    UInt32 streamId;

    UInt32 dateTimePrm[6];

    UInt32 flushCmds[2];

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the swosd driver */
    status = SwosdLink_create(pObj, Utils_msgGetPrm(pMsg));

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
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);

                SwosdLink_processFrames(pObj);
                break;

            case SWOSDLINK_CMD_GUIPRM:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                synchronizeTheCPIS(pSwOsdGuiPrm, streamId);
               

                DM81XX_SWOSD_setDateEnable(streamId, pSwOsdGuiPrm->dateEnable);
                DM81XX_SWOSD_setTransparencyEnable(streamId, pSwOsdGuiPrm->transparencyEnable);
                DM81XX_SWOSD_setTimeEnable(streamId, pSwOsdGuiPrm->timeEnable);
                DM81XX_SWOSD_setLogoEnable(streamId, pSwOsdGuiPrm->logoEnable);
                DM81XX_SWOSD_setLogoPosn(streamId, pSwOsdGuiPrm->logoPos);
                DM81XX_SWOSD_setTextEnable(streamId, pSwOsdGuiPrm->textEnable);
                DM81XX_SWOSD_setTextPosn(streamId, pSwOsdGuiPrm->textPos);
                DM81XX_SWOSD_setDtailedInfo(streamId,
                                            pSwOsdGuiPrm->detailedInfo);
                DM81XX_SWOSD_setUserString(streamId,
                                           (char *) pSwOsdGuiPrm->usrString);
                DM81XX_SWOSD_setCodecType(streamId, pSwOsdGuiPrm->encFormat);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_BR:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                DM81XX_SWOSD_setCodecBitRate(streamId, pSwOsdGuiPrm->bitRate);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_RC:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                DM81XX_SWOSD_setRateControl(streamId,
                                            pSwOsdGuiPrm->rateControl);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_FR:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                DM81XX_SWOSD_setCodecFrameRate(streamId,
                                               pSwOsdGuiPrm->frameRate);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_DATETIMEPOSN:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                dateTimePrm[0] = pSwOsdGuiPrm->dateFormat;
                dateTimePrm[1] = pSwOsdGuiPrm->datePos;
                dateTimePrm[2] = pSwOsdGuiPrm->timeFormat;
                dateTimePrm[3] = pSwOsdGuiPrm->timePos;

                DM81XX_SWOSD_setDateTimePosn(streamId, (Int32 *) dateTimePrm);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_DATETIME:
                pDateTimePrm = (SwosdLink_dateTime *) Utils_msgGetPrm(pMsg);

                dateTimePrm[0] = pDateTimePrm->year;
                dateTimePrm[1] = pDateTimePrm->month;
                dateTimePrm[2] = pDateTimePrm->day;
                dateTimePrm[3] = pDateTimePrm->hour;
                dateTimePrm[4] = pDateTimePrm->min;
                dateTimePrm[5] = pDateTimePrm->sec;

				for (streamId = 0; streamId <  pObj->inQueInfo.numCh; streamId++)
				{
                	DM81XX_SWOSD_setDateTime(streamId, (Int32 *) dateTimePrm);
				}

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

			case SWOSDLINK_CMD_VASTRING:
				pVaStrPrm = (SwosdLink_vaStrPrm *) Utils_msgGetPrm(pMsg);	
				
				for (streamId = 0; streamId <  pObj->inQueInfo.numCh; streamId++)
				{
				    DM81XX_SWOSD_setVaString(streamId,(char*)pVaStrPrm->vaString,pVaStrPrm->disFlag);
				}
				
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
				
			case SWOSDLINK_CMD_HISTENABLE:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
				
				for (streamId = 0; streamId <  pObj->inQueInfo.numCh; streamId++)
				{
					DM81XX_SWOSD_setHistEnable(streamId,pSwOsdGuiPrm->histEnable);
				}
				
				Utils_tskAckOrFreeMsg(pMsg, status);
                break;			
				
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the swosd driver */
    SwosdLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* Swosd link get info */

/* ===================================================================
 *  @func     SwosdLink_getInfo
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* Swosd get full frames */

/* ===================================================================
 *  @func     SwosdLink_getFullFrames
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                              FVID2_FrameList * pFrameList)
{
    Int32 status;

    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);

    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Swosd put empty frames */

/* ===================================================================
 *  @func     SwosdLink_putEmptyFrames
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                               FVID2_FrameList * pFrameList)
{
    System_LinkInQueParams *pInQueParams;

    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    pInQueParams = &pObj->createArgs.inQueParams;

    /* Put the empty buffers back to previous link */
    System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                               pInQueParams->prevLinkQueId, pFrameList);

    return FVID2_SOK;
}

/* Swosd Link init */

/* ===================================================================
 *  @func     SwosdLink_init
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 swosdId;

    SwosdLink_Obj *pObj;

    char tskName[32];

    for (swosdId = 0; swosdId < SWOSD_LINK_OBJ_MAX; swosdId++)
    {
        pObj = &gSwosdLink_obj[swosdId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = SwosdLink_getFullFrames;
        linkObj.linkPutEmptyFrames = SwosdLink_putEmptyFrames;
        linkObj.getLinkInfo = SwosdLink_getInfo;

        System_registerLink(SYSTEM_LINK_ID_SWOSD_0 + swosdId, &linkObj);

        sprintf(tskName, "SWOSD%d", swosdId);

        status = Utils_tskCreate(&pObj->tsk,
                                 SwosdLink_tskMain,
                                 SWOSD_LINK_TSK_PRI,
                                 gSwosdLink_tskStack[swosdId],
                                 SWOSD_LINK_TSK_STACK_SIZE, pObj, tskName);

        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* Swosd link de-init */

/* ===================================================================
 *  @func     SwosdLink_deInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 SwosdLink_deInit()
{
    UInt32 swosdId;

    for (swosdId = 0; swosdId < SWOSD_LINK_OBJ_MAX; swosdId++)
    {
        Utils_tskDelete(&gSwosdLink_obj[swosdId].tsk);
    }

    return FVID2_SOK;
}
