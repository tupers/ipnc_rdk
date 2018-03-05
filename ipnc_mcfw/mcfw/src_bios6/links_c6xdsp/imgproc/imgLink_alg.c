/** ==================================================================
 *  @file   vaLink_alg.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_c6xdsp/va/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "imgLink_priv.h"
#include "srcimg.h"
#include "detectRect.h"


ImgAlg_Obj *gImgAlgLink_obj = NULL;	 

Int32 ImgLink_cacheInv(UInt32 addr,UInt32 size)
{
	Cache_inv((Ptr)addr,size,Cache_Type_ALL,TRUE);	
	return FVID2_SOK;
}

Int32 ImgLink_cacheWb(UInt32 addr,UInt32 size)
{
	Cache_wb((Ptr)addr,size,Cache_Type_ALL,TRUE);	
	return FVID2_SOK;
}

Int32 ImgAlgLink_create(ImgLink_Obj * Obj)
{
	ImgAlg_Obj *pAlgObj = &Obj->algObj;
	gImgAlgLink_obj = pAlgObj;

	pAlgObj->pPrm = (Alg_Params *)malloc(ALGPARAM_MAX_SIZE);

	Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
	semParams.mode = Semaphore_Mode_BINARY;
    pAlgObj->semSync = Semaphore_create(1,&semParams,NULL);
    UTILS_assert(pAlgObj->semSync != NULL);
     
    System_getEdmaCh(&pAlgObj->algEdmaHndl);	

	pAlgObj->width = 0;
	pAlgObj->height = 0;
	pAlgObj->algCount = 0;
	pAlgObj->needRotate = FALSE;
	
    pAlgObj->imgData = (unsigned char *)malloc(IMG_WIDTH_MAX*IMG_HEIGHT_MAX);
    if(NULL==pAlgObj->imgData)
    {
		Vps_printf("malloc fail@ %s\n",__FUNCTION__);

		return -1;
    }

	int i;
	for(i=0;i<SRCIMG_HEIGHT;i++)
		memcpy(pAlgObj->imgData + i*SRCIMG_WIDTH,(char *)srcimg_gvss + i*SRCIMG_WIDTH,SRCIMG_WIDTH);
	pAlgObj->width = SRCIMG_WIDTH;
	pAlgObj->height = SRCIMG_HEIGHT;
	pAlgObj->pitch = SRCIMG_WIDTH;
				
    Semaphore_Params_init(&semParams);
	semParams.mode = Semaphore_Mode_BINARY;
    pAlgObj->semRd = Semaphore_create(0,&semParams,NULL);
    UTILS_assert(pAlgObj->semRd != NULL);
        
	Alg_DrvInit(ImgAlgCpy);
	
	pAlgObj->algDrvHandle = Alg_DrvCreate(ALG_MODULE_GVSS);
	
	return 0;
}

void ImgAlgLink_algDelete(ImgLink_Obj *Obj)
{
	ImgAlg_Obj *pAlgObj = &Obj->algObj;
	
    System_putEdmaCh(&pAlgObj->algEdmaHndl);	

	pAlgObj->width = 0;
	pAlgObj->height = 0;
	pAlgObj->algCount = 0;
	
    if(pAlgObj->imgData)
    	free(pAlgObj->imgData); 

    if(pAlgObj->pPrm)	
    	free(pAlgObj->pPrm);

	Alg_DrvDelete(pAlgObj->algDrvHandle);
	
    Alg_DrvdeInit(NULL);		
}

void ImgAlgCpy(char * dst,char *  src,unsigned int width,unsigned int height,unsigned src_pitch,unsigned dst_pitch,unsigned int cacheValid)
{
	Semaphore_pend(gImgAlgLink_obj->semSync, BIOS_WAIT_FOREVER);

	if(cacheValid == 1)
	{
		ImgLink_cacheWb((UInt32)src,src_pitch*height);
	}
		
    DM81XX_EDMA3_setParams(gImgAlgLink_obj->algEdmaHndl.dmaHndl.chId, 			// chId
			         IMG_LINK_EDMA3_QUEUE_ID,  					// dmaQueue
				 (UInt32)src, 		// srcAddr
				 (UInt32)dst,		// dstAddr
				 width,          								// edmaWidth
				 height,         								// edmaHeight
				 src_pitch,          								// srcLineOffset
				 dst_pitch);         								// dstLineOffset
     /* Trigger the edma transfer */
    DM81XX_EDMA3_triggerTransfer(gImgAlgLink_obj->algEdmaHndl.dmaHndl.chId);	

    ImgLink_cacheInv((unsigned int)dst,dst_pitch*height);
    
    Semaphore_post(gImgAlgLink_obj->semSync);     
}
/* ===================================================================
 *  @func     ImgLink_processFrames
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

Int32 ImgLink_algProcessFrames(ImgLink_Obj *pObj)
{
    FVID2_FrameList frameList;
    int frameId;
    Int32 status;
    FVID2_Frame *pFullFrame = NULL;
    System_FrameInfo *pfrmInfo; 
	Alg_Result *pResult;
	AlgPrm_MsgBuf *msgBuf;
	AlgProcessArgs InArgs;
	unsigned t1,t2;	
	
    status = Utils_bufGetFull(&pObj->outFrameQue, &frameList, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);
                             
    if(frameList.numFrames)
    {
    	t1 = Utils_getCurTimeInMsec();
    		
		for(frameId=0;frameId<frameList.numFrames;frameId++)
		{
			pFullFrame = frameList.frames[frameId];
			pfrmInfo  = (System_FrameInfo *)pFullFrame->appData;
			msgBuf = pFullFrame->pImgAlg;
			pResult = (Alg_Result *)(msgBuf->offset + (char *)pObj->pAlgShBuf);
			
			if(TRUE==pObj->useStaticImg)
			{	
				InArgs.data = pObj->algObj.imgData;
				InArgs.width = pObj->algObj.width;
				InArgs.height = pObj->algObj.height;
				InArgs.pitch = pObj->algObj.pitch;
				ImgAlgCpy(pFullFrame->addr[0][0],(Ptr)pObj->algObj.imgData,pObj->algObj.width,pObj->algObj.height,pObj->algObj.pitch,pfrmInfo->rtChInfo.pitch[0],1);
			}
			else
			{	
				InArgs.data = pFullFrame->addr[0][0];
				InArgs.width = pfrmInfo->rtChInfo.width;
				InArgs.height = pfrmInfo->rtChInfo.height;				
				InArgs.pitch = pfrmInfo->rtChInfo.pitch[0];
			}	


			if(pObj->algObj.algDrvHandle == NULL)
				Vps_printf("Not init\n");
			else	
				status = Alg_DrvProcess(pObj->algObj.algDrvHandle,&InArgs,pResult);
	
			t2 = Utils_getCurTimeInMsec();	

			if(pResult->resultSize > ALGRESULT_MAX_SIZE)
				msgBuf->size = 0;
			else	
				msgBuf->size = pResult->resultSize + sizeof(Alg_Result);		
			pResult->weightValid = !(pResult->weightValid);
			Vps_printf("status = %d,Alg_DrvProcess elapse :%dms\n",status,t2-t1);

			/* Put the full buffer into full queue */
            status = Utils_bufPutFullFrame(&pObj->outFrameQue, pFullFrame);
            UTILS_assert(status == FVID2_SOK);
		}
    }	

   return FVID2_SOK;
}

void ImgAlg_PrmUpdate(ImgAlg_Obj *pAlgObj,char *pPrm,int prmSize)
{	
	memcpy(pAlgObj->pPrm,pPrm,prmSize);
}

void ImgAlg_PrmGet(ImgAlg_Obj *pAlgObj,char *pPrm)
{
	Semaphore_pend(pAlgObj->semRd, BIOS_WAIT_FOREVER);

	Alg_Params *pAlgPrm = (Alg_Params *)(pAlgObj->pPrm);
	
	memcpy(pPrm,pAlgPrm,pAlgPrm->prmSize+sizeof(Alg_Params));
}


int ImgAlg_isParamValid(char *pPrm)
{
	int ret;
	
	ret = Alg_DrvControl(gImgAlgLink_obj->algDrvHandle,ALG_PARAM_ISVALID,pPrm,NULL);	

	return ret; 	
}
Void ImgLink_processTskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg,done;
    Int32 status = FVID2_SOK;
	ImgLink_Obj *pObj = (ImgLink_Obj *) pTsk->appData;
		
	if(cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg,FVID2_EFAIL);
        return;
    }
	
    /* Create the va alg instance */
    status = ImgAlgLink_create(pObj);
    UTILS_assert(status == FVID2_SOK);	
	
	Utils_tskAckOrFreeMsg(pMsg,status);
	
    done   = FALSE;
    ackMsg = FALSE;
	
    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk,&pMsg,BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
		{
            case SYSTEM_CMD_DELETE:
            {
                done   = TRUE;
                ackMsg = TRUE;
				Utils_tskAckOrFreeMsg(pMsg, status);
                break;
             }   		
            case SYSTEM_CMD_NEW_DATA:
            {
                Utils_tskAckOrFreeMsg(pMsg, status);							
				ImgLink_algProcessFrames(pObj);			
				
				Utils_tskSendCmd(&pObj->tsk,SYSTEM_IMG_ALG_DONE);
                break;
			}
            case SYSTEM_IMG_ALG_UPDATE_PARAM:
            {
            	Alg_Params *pAlgPrm = (Alg_Params *)pObj->algObj.pPrm;
             	Alg_DrvControl(pObj->algObj.algDrvHandle,ALG_SET_PARAM,pAlgPrm,NULL);	
             	
				Utils_tskAckOrFreeMsg(pMsg, status);
             	break;   
			}
            case SYSTEM_IMG_ALG_GET_PARAM:
			{
				Alg_Params *pAlgPrm = (Alg_Params *)pObj->algObj.pPrm;
				pAlgPrm->prmSize = ALGPARAM_MAX_SIZE;
				Alg_DrvControl(pObj->algObj.algDrvHandle,ALG_GET_PARAM,NULL,pAlgPrm);
				Semaphore_post(pObj->algObj.semRd);
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
			}	 
			case SYSTEM_IMG_OUTPUT_SOURCE:
			{
				Alg_DrvControl(pObj->algObj.algDrvHandle,ALG_SET_IMG_SOURCE,NULL,NULL);
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;				
			}		
			case SYSTEM_IMG_UPDATE_DBGIMG:
			{
				Alg_DbgImg *pDbgImg = (Alg_DbgImg *)(pObj->pAlgPrmShbuf);
	
				if((pDbgImg->header.width > IMG_WIDTH_MAX)||(pDbgImg->header.height > IMG_HEIGHT_MAX))
				{
					//printf("invalid image\n");
				}
				else
				{
					ImgAlgCpy((Ptr)pObj->algObj.imgData,pDbgImg->data + pDbgImg->header.infoSize,pDbgImg->header.width,pDbgImg->header.height,pDbgImg->header.pitch,pDbgImg->header.pitch,1);
					pObj->algObj.width = pDbgImg->header.width;
					pObj->algObj.height = pDbgImg->header.height;
					pObj->algObj.pitch = pDbgImg->header.pitch;					
				}

				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
			}			
            default:
            {
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
            }    
        }
    }
	
    /* Delete the va alg instance */
    ImgAlgLink_algDelete(pObj);	
	
    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg,status);
    }
		
    return;		
}
