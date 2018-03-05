/** ==================================================================
 *  @file   wdrLink_tsk.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/wdr/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "wdrLink_priv.h"

#pragma DATA_ALIGN(gWdrLink_tskStack, 32)
#pragma DATA_SECTION(gWdrLink_tskStack, ".bss:taskStackSection")
UInt8 gWdrLink_tskStack[WDR_LINK_OBJ_MAX][WDR_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gWdrLink_obj, 32)
#pragma DATA_SECTION(gWdrLink_obj, ".bss:gWdrLink_objSection")
WdrLink_Obj gWdrLink_obj[WDR_LINK_OBJ_MAX];

/* ===================================================================
 *  @func     WdrLink_tskMain                                               
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
Void WdrLink_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    Bool ackMsg, done;

    Int32 status;

    WdrLink_Obj *pObj;

    UInt32 flushCmds[2];

    UInt32 cmd = Utils_msgGetCmd(pMsg);

    pObj = (WdrLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }
	

    status = WdrLink_drvCreate(pObj, Utils_msgGetPrm(pMsg));

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
                WdrLink_drvStart(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);
                WdrLink_drvProcessData(pObj);
                break;

            case WDR_LINK_CMD_CHANGE_MODE:
				WdrLink_drvChangeMode(pObj, *(WdrLink_Mode *)Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_STOP:
                WdrLink_drvStop(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

	Vps_printf("WDR Task has come out of the do while loop\n");
    WdrLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/* ===================================================================
 *  @func     WdrLink_init                                               
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
Int32 WdrLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    WdrLink_Obj *pObj;

    char name[32];

    UInt32 objId;

    for (objId = 0; objId < WDR_LINK_OBJ_MAX; objId++)
    {
        pObj = &gWdrLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));
        pObj->linkId = SYSTEM_LINK_ID_WDR + objId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = WdrLink_getFullFrames;
        linkObj.linkPutEmptyFrames = WdrLink_putEmptyFrames;
        linkObj.linkGetFullBitBufs = NULL;
        linkObj.linkPutEmptyBitBufs = NULL;
        linkObj.getLinkInfo = WdrLink_getInfo;

        sprintf(name, "WDR%d   ", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = Utils_tskCreate(&pObj->tsk,
                                 WdrLink_tskMain,
                                 WDR_LINK_TSK_PRI,
                                 gWdrLink_tskStack[objId],
                                 WDR_LINK_TSK_STACK_SIZE, pObj, name);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     WdrLink_deInit                                               
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
Int32 WdrLink_deInit()
{
    UInt32 objId;

    WdrLink_Obj *pObj;

    for (objId = 0; objId < WDR_LINK_OBJ_MAX; objId++)
    {
        pObj = &gWdrLink_obj[objId];

        Utils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     WdrLink_getInfo                                               
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
Int32 WdrLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    WdrLink_Obj *pObj = (WdrLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     WdrLink_getFullFrames                                               
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
Int32 WdrLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;

    WdrLink_Obj *pObj = (WdrLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->bufOutQue, pFrameList, BIOS_NO_WAIT);
    WDRLINK_INFO_LOG_RT("WDR: output %d frames to the next link!!!\n",
                        pFrameList->numFrames);

    return status;
}

/* ===================================================================
 *  @func     WdrLink_putEmptyFrames                                               
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
Int32 WdrLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
    WdrLink_Obj *pObj = (WdrLink_Obj *) pTsk->appData;

    return WdrLink_drvReleaseFrames(pObj, pFrameList);
}
