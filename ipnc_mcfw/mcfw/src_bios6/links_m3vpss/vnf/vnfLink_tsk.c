/** ==================================================================
 *  @file   vnfLink_tsk.c                                                  
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

#include "vnfLink_priv.h"

#pragma DATA_ALIGN(gVnfLink_tskStack, 32)
#pragma DATA_SECTION(gVnfLink_tskStack, ".bss:taskStackSection")
UInt8 gVnfLink_tskStack[VNF_LINK_OBJ_MAX][VNF_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gVnfLink_obj, 32)
#pragma DATA_SECTION(gVnfLink_obj, ".bss:gVnfLink_objSection")
VnfLink_Obj gVnfLink_obj[VNF_LINK_OBJ_MAX];

extern Iss_CaptDccPrm gDccPrm;

/* ===================================================================
 *  @func     VnfLink_tskMain                                               
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
Void VnfLink_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    Bool ackMsg, done;

    Int32 status;

    VnfLink_Obj *pObj;

    UInt32 flushCmds[2];
	
	UInt32 channelNum;

    UInt32 cmd = Utils_msgGetCmd(pMsg);

    pObj = (VnfLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = VnfLink_drvCreate(pObj, Utils_msgGetPrm(pMsg));

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
                VnfLink_drvStart(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);
                VnfLink_drvProcessData(pObj);
                break;

            case VNF_LINK_CMD_GET_STATIC_PARAMS:
                VnfLink_drvGetStaticParams(pObj,
                                           (VnfLink_ChStaticParams *)
                                           Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case VNF_LINK_CMD_GET_DYNAMIC_PARAMS:
                VnfLink_drvGetDynamicParams(pObj,
                                            (VnfLink_ChDynamicParams *)
                                            Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case VNF_LINK_CMD_SET_DYNAMIC_PARAMS:
                VnfLink_drvSetDynamicParams(pObj,
                                            (VnfLink_ChDynamicParams *)
                                            Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case VNF_LINK_CMD_ENABLE_LDC:
                VnfLink_drvEnableLDC(pObj, *(UInt32 *) Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case VNF_LINK_CMD_ENABLE_TNF:
                VnfLink_drvEnableTNF(pObj, *(UInt32 *) Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case VNF_LINK_CMD_ENABLE_SNF:
                VnfLink_drvEnableSNF(pObj, *(UInt32 *) Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case VNF_LINK_CMD_VNF_STRENGTH:
                VnfLink_drvSetStrength(pObj, *(VNF_LINK_VNF_STRENGTH*) Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_STOP:
                VnfLink_drvStop(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
				
			case VNF_LINK_CMD_UPDATE_DCC:
			    memcpy(&gDccPrm, (Iss_CaptDccPrm *) Utils_msgGetPrm(pMsg), sizeof(Iss_CaptDccPrm));
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;

			case VNF_LINK_CMD_ENABLE_CHANNEL:
				channelNum = *((UInt32*)Utils_msgGetPrm(pMsg));
				VnfLink_enableChannel(pObj,channelNum);	
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
				
			case VNF_LINK_CMD_DISABLE_CHANNEL:
				channelNum = *((UInt32*)Utils_msgGetPrm(pMsg));
				VnfLink_disableChannel(pObj,channelNum);	
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
				
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    VnfLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/* ===================================================================
 *  @func     VnfLink_init                                               
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
Int32 VnfLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    VnfLink_Obj *pObj;

    char name[32];

    UInt32 objId;

    for (objId = 0; objId < VNF_LINK_OBJ_MAX; objId++)
    {
        pObj = &gVnfLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));
        pObj->linkId = SYSTEM_LINK_ID_VNF + objId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = VnfLink_getFullFrames;
        linkObj.linkPutEmptyFrames = VnfLink_putEmptyFrames;
        linkObj.linkGetFullBitBufs = NULL;
        linkObj.linkPutEmptyBitBufs = NULL;
        linkObj.getLinkInfo = VnfLink_getInfo;

        sprintf(name, "VNF%d   ", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = Utils_tskCreate(&pObj->tsk,
                                 VnfLink_tskMain,
                                 VNF_LINK_TSK_PRI,
                                 gVnfLink_tskStack[objId],
                                 VNF_LINK_TSK_STACK_SIZE, pObj, name);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     VnfLink_deInit                                               
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
Int32 VnfLink_deInit()
{
    UInt32 objId;

    VnfLink_Obj *pObj;

    for (objId = 0; objId < VNF_LINK_OBJ_MAX; objId++)
    {
        pObj = &gVnfLink_obj[objId];

        Utils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VnfLink_getInfo                                               
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
Int32 VnfLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    VnfLink_Obj *pObj = (VnfLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VnfLink_getFullFrames                                               
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
Int32 VnfLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;

    VnfLink_Obj *pObj = (VnfLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->bufOutQue, pFrameList, BIOS_NO_WAIT);
    VNFLINK_INFO_LOG_RT("VNF: output %d frames to the next link!!!\n",
                        pFrameList->numFrames);

    return status;
}

/* ===================================================================
 *  @func     VnfLink_putEmptyFrames                                               
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
Int32 VnfLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
    VnfLink_Obj *pObj = (VnfLink_Obj *) pTsk->appData;

    return VnfLink_drvReleaseFrames(pObj, pFrameList);
}
