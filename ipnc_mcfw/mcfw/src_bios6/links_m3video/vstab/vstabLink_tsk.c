/** ==================================================================
 *  @file   vstabLink_tsk.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/vstab/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "vstabLink_priv.h"

#pragma DATA_ALIGN(gVstabLink_tskStack, 32)
#pragma DATA_SECTION(gVstabLink_tskStack, ".bss:taskStackSection")
UInt8 gVstabLink_tskStack[VSTAB_LINK_OBJ_MAX][VSTAB_LINK_TSK_STACK_SIZE];

VstabLink_Obj gVstabLink_obj[VSTAB_LINK_OBJ_MAX];

/* Vstab link create */

/* ===================================================================
 *  @func     VstabLink_create                                               
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
Int32 VstabLink_create(VstabLink_Obj * pObj,
                       VstabLink_CreateParams * pPrm)
{	
    Int32 status;
    
    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    pObj->receviedFrameCount = 0;
	
	status = VstabLink_algCreate(pObj);
	UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_VSTAB
    Vps_printf(" %d: VSTAB   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Vstab link delete */

/* ===================================================================
 *  @func     VstabLink_delete                                               
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
Int32 VstabLink_delete(VstabLink_Obj * pObj)
{
    Int32 status;

    /* Delete the vstab algorithm instance */
    status = VstabLink_algDelete(pObj);
    UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_VSTAB
    Vps_printf(" %d: VSTAB   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Vstab link task main function */

/* ===================================================================
 *  @func     VstabLink_tskMain                                               
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
Void VstabLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    VstabLink_Obj *pObj = (VstabLink_Obj *) pTsk->appData;
	VstabLink_processPrm *pProcessPrm;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the vstab alg */
    status = VstabLink_create(pObj, Utils_msgGetPrm(pMsg));

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
                pProcessPrm = (VstabLink_processPrm*)Utils_msgGetPrm(pMsg);			
                VstabLink_algProcess(pObj,pProcessPrm);
				Utils_tskAckOrFreeMsg(pMsg, status);
                break;
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }
	
    /* Delete the vstab alg */
    VstabLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* Vstab Link init */

/* ===================================================================
 *  @func     VstabLink_init                                               
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
Int32 VstabLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 vstabId;

    VstabLink_Obj *pObj;

    char tskName[32];

    for (vstabId = 0; vstabId < VSTAB_LINK_OBJ_MAX; vstabId++)
    {
        pObj = &gVstabLink_obj[vstabId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = NULL;
        linkObj.linkPutEmptyFrames = NULL;
        linkObj.getLinkInfo = NULL;

        System_registerLink(SYSTEM_LINK_ID_VSTAB_0 + vstabId, &linkObj);

        sprintf(tskName, "VSTAB%d", vstabId);

        status = Utils_tskCreate(&pObj->tsk,
                                 VstabLink_tskMain,
                                 VSTAB_LINK_TSK_PRI,
                                 gVstabLink_tskStack[vstabId],
                                 VSTAB_LINK_TSK_STACK_SIZE, pObj, tskName);
        UTILS_assert(status == FVID2_SOK);		
    }

    return status;
}

/* Vstab link de-init */

/* ===================================================================
 *  @func     VstabLink_deInit                                               
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
Int32 VstabLink_deInit()
{
    UInt32 vstabId;

    for (vstabId = 0; vstabId < VSTAB_LINK_OBJ_MAX; vstabId++)
    {
        Utils_tskDelete(&gVstabLink_obj[vstabId].tsk);
    }

    return FVID2_SOK;
}
