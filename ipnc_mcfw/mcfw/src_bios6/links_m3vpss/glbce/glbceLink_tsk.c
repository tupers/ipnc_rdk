/** ==================================================================
 *  @file   glbceLink_tsk.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/glbce/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "glbceLink_priv.h"

#pragma DATA_ALIGN(gGlbceLink_tskStack, 32)
#pragma DATA_SECTION(gGlbceLink_tskStack, ".bss:taskStackSection")
UInt8 gGlbceLink_tskStack[GLBCE_LINK_OBJ_MAX][GLBCE_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gGlbceLink_obj, 32)
#pragma DATA_SECTION(gGlbceLink_obj, ".bss:gGlbceLink_objSection")
GlbceLink_Obj gGlbceLink_obj[GLBCE_LINK_OBJ_MAX];

#ifdef USE_A8_FOR_TC_CALC
extern int totalNumberOfTcRcvd;
#endif

extern Iss_CaptDccPrm ggDccPrm;

/* ===================================================================
 *  @func     GlbceLink_tskMain                                               
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
Void GlbceLink_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    Bool ackMsg, done;

    Int32 status;

    GlbceLink_Obj *pObj;

    UInt32 flushCmds[2];

    UInt32 cmd = Utils_msgGetCmd(pMsg);

    pObj = (GlbceLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }
	

    status = GlbceLink_drvCreate(pObj, Utils_msgGetPrm(pMsg));

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
                GlbceLink_drvStart(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);
                //Vps_rprintf("Calling ProcessData \n");
                GlbceLink_drvProcessData(pObj);
                //Vps_rprintf("Done and out of Call ProcessData \n");
                break;

            case GLBCE_LINK_CMD_GET_STATIC_PARAMS:
                GlbceLink_drvGetStaticParams(pObj,
                                           (GlbceLink_ChStaticParams *)
                                           Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case GLBCE_LINK_CMD_GET_DYNAMIC_PARAMS:
                GlbceLink_drvGetDynamicParams(pObj,
                                            (GlbceLink_ChDynamicParams *)
                                            Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case GLBCE_LINK_CMD_SET_DYNAMIC_PARAMS:
                GlbceLink_drvSetDynamicParams(pObj,
                                            (GlbceLink_PresetDynamicParams *)
                                            Utils_msgGetPrm(pMsg));
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
				
			case GLBCE_LINK_CMD_TC_READY:
#ifdef USE_A8_FOR_TC_CALC
			     totalNumberOfTcRcvd++;
#endif
				 Utils_tskAckOrFreeMsg(pMsg, status);
                 break;

            case SYSTEM_CMD_STOP:
                GlbceLink_drvStop(pObj);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
			case GLBCE_LINK_CMD_UPDATE_DCC:
			    memcpy(&ggDccPrm, (Iss_CaptDccPrm *) Utils_msgGetPrm(pMsg), sizeof(Iss_CaptDccPrm));
                Utils_tskAckOrFreeMsg(pMsg, status);
				break;

            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

	Vps_printf("GLBCE Task has come out of the do while loop\n");
    GlbceLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/* ===================================================================
 *  @func     GlbceLink_init                                               
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
Int32 GlbceLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    GlbceLink_Obj *pObj;

    char name[32];

    UInt32 objId;


    for (objId = 0; objId < GLBCE_LINK_OBJ_MAX; objId++)
    {
        pObj = &gGlbceLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));
        pObj->linkId = SYSTEM_LINK_ID_GLBCE + objId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = GlbceLink_getFullFrames;
        linkObj.linkPutEmptyFrames = GlbceLink_putEmptyFrames;
        linkObj.linkGetFullBitBufs = NULL;
        linkObj.linkPutEmptyBitBufs = NULL;
        linkObj.getLinkInfo = GlbceLink_getInfo;

        sprintf(name, "GLBCE%d   ", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = Utils_tskCreate(&pObj->tsk,
                                 GlbceLink_tskMain,
                                 GLBCE_LINK_TSK_PRI,
                                 gGlbceLink_tskStack[objId],
                                 GLBCE_LINK_TSK_STACK_SIZE, pObj, name);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     GlbceLink_deInit                                               
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
Int32 GlbceLink_deInit()
{
    UInt32 objId;

    GlbceLink_Obj *pObj;



    for (objId = 0; objId < GLBCE_LINK_OBJ_MAX; objId++)
    {
        pObj = &gGlbceLink_obj[objId];

        Utils_tskDelete(&pObj->tsk);
    }



    return FVID2_SOK;
}

/* ===================================================================
 *  @func     GlbceLink_getInfo                                               
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
Int32 GlbceLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    GlbceLink_Obj *pObj = (GlbceLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     GlbceLink_getFullFrames                                               
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
Int32 GlbceLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    Int32 status;

    GlbceLink_Obj *pObj = (GlbceLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->bufOutQue, pFrameList, BIOS_NO_WAIT);
    GLBCELINK_INFO_LOG_RT("GLBCE: output %d frames to the next link!!!\n",
                        pFrameList->numFrames);

    return status;
}

/* ===================================================================
 *  @func     GlbceLink_putEmptyFrames                                               
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
Int32 GlbceLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
    GlbceLink_Obj *pObj = (GlbceLink_Obj *) pTsk->appData;

    return GlbceLink_drvReleaseFrames(pObj, pFrameList);
}
