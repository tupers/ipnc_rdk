/** ==================================================================
 *  @file   mctnfLink_tsk.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/vnf/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "mctnfLink_priv.h"

#pragma DATA_ALIGN(gMctnfLink_tskStack, 32)
#pragma DATA_SECTION(gMctnfLink_tskStack, ".bss:taskStackSection")
UInt8 gMctnfLink_tskStack[MCTNF_LINK_OBJ_MAX][MCTNF_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gMctnfLink_obj, 32)
#pragma DATA_SECTION(gMctnfLink_obj, ".bss:gMctnfLink_objSection")
MctnfLink_Obj gMctnfLink_obj[MCTNF_LINK_OBJ_MAX];

extern Iss_CaptDccPrm gDccPrm_for_mctnf;
extern ti2a_output ti2a_output_params_for_mctnf;
extern iss_drv_config_t *pIss_drv_config_for_mctnf;
extern iss_drv_config_t iss_drv_config_for_mctnf;

Void MctnfLink_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    Bool ackMsg, done;

    Int32 status;

    MctnfLink_Obj *pObj;

    UInt32 flushCmds[2];
	
	UInt32 channelNum;

    UInt32 cmd = Utils_msgGetCmd(pMsg);

    pObj = (MctnfLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }
	Vps_rprintf("Entered the MctnfLink_tskMain() \n");

    status = MctnfLink_drvCreate(pObj, Utils_msgGetPrm(pMsg));

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
            case SYSTEM_CMD_START:
                MctnfLink_drvStart(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);
                MctnfLink_drvProcessData(pObj);
                break;

            case MCTNF_LINK_CMD_GET_STATIC_PARAMS:
                MctnfLink_drvGetStaticParams(pObj,
                                           (MctnfLink_ChStaticParams *)
                                           Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case MCTNF_LINK_CMD_GET_DYNAMIC_PARAMS:
                MctnfLink_drvGetDynamicParams(pObj,
                                            (MctnfLink_ChDynamicParams *)
                                            Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case MCTNF_LINK_CMD_SET_DYNAMIC_PARAMS:
                MctnfLink_drvSetDynamicParams(pObj,
                                            (MctnfLink_ChDynamicParams *)
                                            Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;


            case MCTNF_LINK_CMD_VNF_STRENGTH:
                MctnfLink_drvSetStrength(pObj, *(MCTNF_LINK_VNF_STRENGTH*) Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_STOP:
                MctnfLink_drvStop(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
				
			case MCTNF_LINK_CMD_UPDATE_DCC:
			    memcpy(&gDccPrm_for_mctnf, (Iss_CaptDccPrm *) Utils_msgGetPrm(pMsg), sizeof(Iss_CaptDccPrm));
                Utils_tskAckOrFreeMsg(pMsg, status);
				break;

			case MCTNF_LINK_CMD_UPDATE_2A:
			    memcpy(&ti2a_output_params_for_mctnf, (ti2a_output *) Utils_msgGetPrm(pMsg), sizeof(ti2a_output));
                Utils_tskAckOrFreeMsg(pMsg, status);
				break;

			case MCTNF_LINK_CMD_UPDATE_ISS_CONFIG:
			    memcpy(pIss_drv_config_for_mctnf, (iss_drv_config_t *) Utils_msgGetPrm(pMsg), sizeof(iss_drv_config_t));
                Utils_tskAckOrFreeMsg(pMsg, status);
				break;
				
			case MCTNF_LINK_CMD_ENABLE_CHANNEL:
				channelNum = *((UInt32*)Utils_msgGetPrm(pMsg));
				MctnfLink_enableChannel(pObj,channelNum);	
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
				
			case MCTNF_LINK_CMD_DISABLE_CHANNEL:
				channelNum = *((UInt32*)Utils_msgGetPrm(pMsg));
				MctnfLink_disableChannel(pObj,channelNum);	
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
				
			default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    MctnfLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Int32 MctnfLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    MctnfLink_Obj *pObj;

    char name[32];

    UInt32 objId;
	
	Vps_rprintf("Entered the MctnfLink_init() \n");


    for (objId = 0; objId < MCTNF_LINK_OBJ_MAX; objId++)
    {
        pObj = &gMctnfLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));
        pObj->linkId = SYSTEM_LINK_ID_MCTNF + objId; 

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = MctnfLink_getFullFrames; 
        linkObj.linkPutEmptyFrames = MctnfLink_putEmptyFrames;
        linkObj.linkGetFullBitBufs = NULL;
        linkObj.linkPutEmptyBitBufs = NULL;
        linkObj.getLinkInfo = MctnfLink_getInfo;

        sprintf(name, "MCTNF%d   ", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = Utils_tskCreate(&pObj->tsk,
                                 MctnfLink_tskMain,
                                 MCTNF_LINK_TSK_PRI,
                                 gMctnfLink_tskStack[objId],
                                 MCTNF_LINK_TSK_STACK_SIZE, pObj, name);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

Int32 MctnfLink_deInit()
{
    UInt32 objId;

    MctnfLink_Obj *pObj;

    for (objId = 0; objId < MCTNF_LINK_OBJ_MAX; objId++)
    {
        pObj = &gMctnfLink_obj[objId];

        Utils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

Int32 MctnfLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    MctnfLink_Obj *pObj = (MctnfLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

Int32 MctnfLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;

    MctnfLink_Obj *pObj = (MctnfLink_Obj *) pTsk->appData;
    
    //Vps_printf("MctnfLink_getFullFrames() GETTING CALLED\n");

    status = Utils_bufGetFull(&pObj->bufOutQue, pFrameList, BIOS_NO_WAIT);
    MCTNFLINK_INFO_LOG_RT("MCTNF: output %d frames to the next link!!!\n",
                        pFrameList->numFrames);

    return status;
}

Int32 MctnfLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
    MctnfLink_Obj *pObj = (MctnfLink_Obj *) pTsk->appData;

    return MctnfLink_drvReleaseFrames(pObj, pFrameList);
}
