/** ==================================================================
 *  @file   glbceSupportLink_tsk.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/glbceSupport/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "glbceSupportLink_priv.h"


GlbceSupportLink_Obj gGlbceSupportLink_obj[GLBCE_SUPPORT_LINK_OBJ_MAX];

/* GlbceSupport link create */

/* ===================================================================
 *  @func     GlbceSupportLink_create                                               
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
Int32 GlbceSupportLink_create(GlbceSupportLink_Obj * pObj,
                       GlbceSupportLink_CreateParams * pPrm)
{	
    Int32 status;
    
    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    pObj->receviedFrameCount = 0;
	
	status = GlbceSupportLink_algCreate(pObj);
	OSA_assert(status == GLBCE_SUPPORT_LINK_S_SUCCESS);

#ifdef SYSTEM_DEBUG_GLBCE_SUPPORT
    Vps_printf(" %d: GLBCE_SUPPORT   : Create Done !!!\n", Clock_getTicks());
#endif

    return GLBCE_SUPPORT_LINK_S_SUCCESS;
}

/* GlbceSupport link delete */

/* ===================================================================
 *  @func     GlbceSupportLink_delete                                               
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
Int32 GlbceSupportLink_delete(GlbceSupportLink_Obj * pObj)
{
    Int32 status;

    /* Delete the glbceSupport algorithm instance */
    status = GlbceSupportLink_algDelete(pObj);
    OSA_assert(status == GLBCE_SUPPORT_LINK_S_SUCCESS);

#ifdef SYSTEM_DEBUG_GLBCE_SUPPORT
    Vps_printf(" %d: GLBCE_SUPPORT   : Delete Done !!!\n", Clock_getTicks());
#endif

    return GLBCE_SUPPORT_LINK_S_SUCCESS;
}

/* GlbceSupport link task main function */

/* ===================================================================
 *  @func     GlbceSupportLink_tskMain                                               
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
Int GlbceSupportLink_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg,
                          Uint32 curState)
{
    UInt32 cmd = OSA_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status = GLBCE_SUPPORT_LINK_S_SUCCESS;
    GlbceSupportLink_Obj *pObj = (GlbceSupportLink_Obj *) pTsk->appData;
	GlbceSupportLink_processPrm *pProcessPrm;

    OSA_printf("%s:Entered", __func__);
    if (cmd != SYSTEM_CMD_CREATE)
    {
        OSA_tskAckOrFreeMsg(pMsg, OSA_EFAIL);
        return status;
    }

    /* create the glbceSupport alg */
    status = GlbceSupportLink_create(pObj, OSA_msgGetPrm(pMsg));

    OSA_tskAckOrFreeMsg(pMsg, status);

    if (status != OSA_SOK)
        return status;

    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = OSA_tskWaitMsg(pTsk, &pMsg);
        if (status != OSA_SOK)
            break;

        cmd = OSA_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
            case SYSTEM_CMD_NEW_DATA:
                pProcessPrm = (GlbceSupportLink_processPrm*)OSA_msgGetPrm(pMsg);
                //OSA_printf("Received Msg from M3 ");				
                GlbceSupportLink_algProcess(pObj,pProcessPrm);
                //OSA_printf("Processed Msg  ");				
                //OSA_printf("Sending Ack back  ");				
				OSA_tskAckOrFreeMsg(pMsg, status);
                break;
            default:
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
        }
      //OSA_printf("I am in the task loop !!!!!!!!!!! \n");		
    }
	
    /* Delete the glbceSupport alg */
    GlbceSupportLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        OSA_tskAckOrFreeMsg(pMsg, status);
    }

    return status;
}

/* GlbceSupport Link init */

/* ===================================================================
 *  @func     GlbceSupportLink_init                                               
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
Int32 GlbceSupportLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 glbceSupportId;

    GlbceSupportLink_Obj *pObj;

    char tskName[32];

    for (glbceSupportId = 0; glbceSupportId < GLBCE_SUPPORT_LINK_OBJ_MAX; glbceSupportId++)
    {
        pObj = &gGlbceSupportLink_obj[glbceSupportId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        //linkObj.linkGetFullFrames = NULL;
        //linkObj.linkPutEmptyFrames = NULL;
        //linkObj.linkGetFullBitBufs = NULL;
        //linkObj.linkPutEmptyBitBufs = NULL;
        linkObj.getLinkInfo = NULL;

        System_registerLink(SYSTEM_LINK_ID_GLBCE_SUPPORT_0 + glbceSupportId, &linkObj);

        OSA_SNPRINTF(tskName, "GLBCE_SUPPORT%d", glbceSupportId);

        status = OSA_tskCreate(&pObj->tsk,
                                 GlbceSupportLink_tskMain,
                                 GLBCE_SUPPORT_LINK_TSK_PRI,
                                 //gGlbceSupportLink_tskStack[glbceSupportId],
                                 GLBCE_SUPPORT_LINK_TSK_STACK_SIZE,0,  pObj/*, tskName*/);
        OSA_assert(status == OSA_SOK);	
    }

    return status;
}

/* GlbceSupport link de-init */

/* ===================================================================
 *  @func     GlbceSupportLink_deInit                                               
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
Int32 GlbceSupportLink_deInit()
{
    UInt32 glbceSupportId;

    for (glbceSupportId = 0; glbceSupportId < GLBCE_SUPPORT_LINK_OBJ_MAX; glbceSupportId++)
    {
        OSA_tskDelete(&gGlbceSupportLink_obj[glbceSupportId].tsk);
    }

    return GLBCE_SUPPORT_LINK_S_SUCCESS;
}
