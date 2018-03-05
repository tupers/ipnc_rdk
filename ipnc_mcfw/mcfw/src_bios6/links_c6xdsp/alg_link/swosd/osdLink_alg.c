/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "osdLink_priv.h"


Int32 AlgLink_OsdalgCreate(AlgLink_OsdObj * pObj)
{
	Int32 status, chId;
    SWOSD_OpenPrm algCreatePrm;
    AlgLink_OsdChObj *pChObj;
    SWOSD_Obj *pSwOsdObj;
    AlgLink_OsdChWinParams *pChWinPrm;


    algCreatePrm.maxWidth =
        pObj->osdChCreateParams[0].maxWidth;
    algCreatePrm.maxHeight =
		pObj->osdChCreateParams[0].maxHeight;

    /* Create algorithm instance and get algo handle  */
	status = SWOSD_open(&pObj->osdObj, &algCreatePrm);

	UTILS_assert(status == 0);
	
    for(chId=0; chId<pObj->inQueInfo->numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        pChWinPrm = &pObj->osdChCreateParams[chId].chDefaultParams;

        pSwOsdObj = &pChObj->osdObj;


        pChWinPrm->chId = chId;

        status = AlgLink_OsdalgSetChOsdWinPrm(pObj, pChWinPrm);

        UTILS_assert(status==0);

        pSwOsdObj->algHndl = pObj->osdObj.algHndl;

        pSwOsdObj->openPrm = pObj->osdObj.openPrm;

        pSwOsdObj->videoWindowPrm.format = SWOSD_FORMAT_YUV422i;
        pSwOsdObj->videoWindowPrm.startX = pObj->inQueInfo->chInfo[chId].startX;
        pSwOsdObj->videoWindowPrm.startY = pObj->inQueInfo->chInfo[chId].startY;
        pSwOsdObj->videoWindowPrm.width  = pObj->inQueInfo->chInfo[chId].width;
        pSwOsdObj->videoWindowPrm.height = pObj->inQueInfo->chInfo[chId].height;
        pSwOsdObj->videoWindowPrm.lineOffset = pObj->inQueInfo->chInfo[chId].pitch[0];
    }
    
	return FVID2_SOK;
}

Int32 AlgLink_OsdalgDelete(AlgLink_OsdObj * pObj)
{
    SWOSD_close(&pObj->osdObj);

	return FVID2_SOK;
}

Int32 AlgLink_OsdalgProcessFrame(AlgLink_OsdObj * pObj, FVID2_Frame *pFrame)
{
    UInt32 winId, fid, scaleX, scaleY;
    AlgLink_OsdChObj *pChObj;
    SWOSD_Obj *pSwOsdObj;
    System_FrameInfo *pFrameInfo;

    pChObj = &pObj->chObj[pFrame->channelNum];

    pSwOsdObj = &pChObj->osdObj;

    fid = 0;
    if(pObj->inQueInfo->chInfo[pFrame->channelNum].scanFormat == SYSTEM_SF_INTERLACED)
    {
        /* OSD plane is always progressive
             Input can be interlaced in this we need to skip alternate lines in OSD plane
            and feed for blending
        */
        if(pFrame->fid==1)
            fid = 1;
    }
	scaleX = scaleY = 1;
    for(winId=0; winId<pChObj->numWindows; winId++)
    {
        if(pChObj->osdWinObj[winId].enableWin)
        {
            pSwOsdObj->alphaWindowAddr = NULL;
		
            pSwOsdObj->videoWindowAddr = pFrame->addr[0][0];
            pFrameInfo = (System_FrameInfo*)pFrame->appData;

            UTILS_assert(pFrameInfo!=NULL);

            if(pFrameInfo->rtChInfoUpdate)
            {
                pSwOsdObj->videoWindowPrm.startX = pFrameInfo->rtChInfo.startX;
                pSwOsdObj->videoWindowPrm.startY = pFrameInfo->rtChInfo.startY;
                pSwOsdObj->videoWindowPrm.width  = pFrameInfo->rtChInfo.width;
                pSwOsdObj->videoWindowPrm.height = pFrameInfo->rtChInfo.height;
                pSwOsdObj->videoWindowPrm.lineOffset = pFrameInfo->rtChInfo.pitch[0];
            }

            if (SYSTEM_MT_TILEDMEM == pObj->inQueInfo->chInfo[pFrame->channelNum].memType)
            {
                pSwOsdObj->videoWindowAddr =
                (Ptr)Utils_tilerAddr2CpuAddr((UInt32)pFrame->addr[0][0]);
                pSwOsdObj->videoWindowPrm.lineOffset =  VPSUTILS_TILER_CNT_8BIT_PITCH;
            }

            pSwOsdObj->globalPrm.globalAlpha = pChObj->osdWinObj[winId].globalAlpha;
            pSwOsdObj->globalPrm.transperencyEnable = pChObj->osdWinObj[winId].transperencyEnable;

            pSwOsdObj->graphicsWindowPrm = pChObj->osdWinObj[winId].osdWinPrm;
            
            if((pSwOsdObj->graphicsWindowPrm.format == SWOSD_FORMAT_YUV420sp) &&
               (pObj->inQueInfo->chInfo[pFrame->channelNum].scanFormat == SYSTEM_SF_INTERLACED))
            {
            	scaleX = 1;
            	scaleY = 2;
            }
            if((pSwOsdObj->graphicsWindowPrm.format == SWOSD_FORMAT_YUV422i) &&
               (pObj->inQueInfo->chInfo[pFrame->channelNum].scanFormat == SYSTEM_SF_INTERLACED))
            {
            	scaleX = 2;
            	scaleY = 2;
            }
			
            pSwOsdObj->graphicsWindowAddr = (pChObj->osdWinObj[winId].addr[0][0] + fid*pSwOsdObj->graphicsWindowPrm.lineOffset*scaleX);
            pSwOsdObj->graphicsWindowPrm.startX *= scaleX;
            pSwOsdObj->graphicsWindowPrm.startY /= scaleY;    // half width
            pSwOsdObj->graphicsWindowPrm.height /= scaleY;    // half height
            pSwOsdObj->graphicsWindowPrm.width *=scaleX;
            pSwOsdObj->graphicsWindowPrm.lineOffset *= (scaleX * scaleY); // double line offset


/*
            Vps_printf("%s %d> VID %X %d %d %d %d %d, chNo %d, Gfx %d %d %d %d %d\n", 
                    __FUNCTION__, __LINE__, 
                            pSwOsdObj->videoWindowAddr,
					pSwOsdObj->videoWindowPrm.startX, pSwOsdObj->videoWindowPrm.startY, pSwOsdObj->videoWindowPrm.width, pSwOsdObj->videoWindowPrm.height, pSwOsdObj->videoWindowPrm.lineOffset,
					pFrame->channelNum, 
					pSwOsdObj->graphicsWindowPrm.startX, pSwOsdObj->graphicsWindowPrm.startY, pSwOsdObj->graphicsWindowPrm.width, pSwOsdObj->graphicsWindowPrm.height, pSwOsdObj->graphicsWindowPrm.lineOffset);
*/    
        /* for odd field, skip to 2nd line in OSD */
            SWOSD_blendWindow(pSwOsdObj);
            
            if(pSwOsdObj->graphicsWindowPrm.format == SWOSD_FORMAT_YUV420sp)
            {
            	pSwOsdObj->videoWindowAddr = pFrame->addr[0][1];
                if (SYSTEM_MT_TILEDMEM == pObj->inQueInfo->chInfo[pFrame->channelNum].memType)
                {
                    pSwOsdObj->videoWindowAddr =
                    (Ptr)Utils_tilerAddr2CpuAddr((UInt32)pFrame->addr[0][1]);
                    pSwOsdObj->videoWindowPrm.lineOffset =  VPSUTILS_TILER_CNT_16BIT_PITCH;
                }

                pSwOsdObj->graphicsWindowAddr =  pChObj->osdWinObj[winId].addr[0][1];
                pSwOsdObj->graphicsWindowPrm.startY >>= 1;    // half width
                pSwOsdObj->graphicsWindowPrm.height >>= 1;    // half height
                SWOSD_blendWindow(pSwOsdObj);                
            }
/*
            Vps_printf("%s %d> VID %X %d %d %d %d %d, chNo %d, Gfx %d %d %d %d %d\n", 
                    __FUNCTION__, __LINE__, 
                            pSwOsdObj->videoWindowAddr,
					pSwOsdObj->videoWindowPrm.startX, pSwOsdObj->videoWindowPrm.startY, pSwOsdObj->videoWindowPrm.width, pSwOsdObj->videoWindowPrm.height, pSwOsdObj->videoWindowPrm.lineOffset,
					pFrame->channelNum, 
					pSwOsdObj->graphicsWindowPrm.startX, pSwOsdObj->graphicsWindowPrm.startY, pSwOsdObj->graphicsWindowPrm.width, pSwOsdObj->graphicsWindowPrm.height, pSwOsdObj->graphicsWindowPrm.lineOffset);
*/
        }
    }

    return 0;
}


Int32 AlgLink_OsdalgSetChOsdWinPrm(AlgLink_OsdObj * pObj,
                            AlgLink_OsdChWinParams * params)
{
	Int32 status = 0;
	AlgLink_OsdChObj *pChObj;
    UInt32 i;

    if(params->chId >= pObj->inQueInfo->numCh)
        return -1;

	pChObj = &pObj->chObj[params->chId];

	pChObj->numWindows         = params->numWindows;
	for(i = 0; i < params->numWindows; i++)
	{
		AlgLink_OsdWinObj *osdWinObj = &pChObj->osdWinObj[i];

		osdWinObj->enableWin          = params->winPrm[i].enableWin;
		osdWinObj->transperencyEnable = params->winPrm[i].transperencyEnable;
		osdWinObj->globalAlpha        = params->winPrm[i].globalAlpha;


		osdWinObj->addr[0][0] = params->winPrm[i].addr[0][0];
		osdWinObj->addr[0][1] = params->winPrm[i].addr[0][1];
		
		if(params->winPrm[i].format == FVID2_DF_YUV422I_YUYV)
		{
			osdWinObj->osdWinPrm.format = SWOSD_FORMAT_YUV422i;
		}
		else if(params->winPrm[i].format == FVID2_DF_YUV420SP_UV)
		{
			osdWinObj->osdWinPrm.format = SWOSD_FORMAT_YUV420sp;
		}
		else
		{
			osdWinObj->osdWinPrm.format = -1;
		}

		osdWinObj->osdWinPrm.startX     = params->winPrm[i].startX;
		osdWinObj->osdWinPrm.startY     = params->winPrm[i].startY;
		osdWinObj->osdWinPrm.width      = params->winPrm[i].width;
		osdWinObj->osdWinPrm.height     = params->winPrm[i].height;
		osdWinObj->osdWinPrm.lineOffset = params->winPrm[i].lineOffset;
	}

	return (status);
}

                                                 


