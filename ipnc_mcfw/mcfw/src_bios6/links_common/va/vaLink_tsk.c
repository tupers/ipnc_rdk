/** ==================================================================
 *  @file   vaLink_tsk.c                                                  
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

#include "vaLink_priv.h"

#pragma DATA_ALIGN(gVaLink_tskStack, 32)
#pragma DATA_SECTION(gVaLink_tskStack, ".bss:taskStackSection")
UInt8 gVaLink_tskStack[VA_LINK_OBJ_MAX][VA_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gVaLink_processTskStack, 32)
#pragma DATA_SECTION(gVaLink_processTskStack, ".bss:taskStackSection")
UInt8 gVaLink_processTskStack[VA_LINK_OBJ_MAX][VA_LINK_PROCESSTSK_STACK_SIZE];

VaLink_Obj gVaLink_obj[VA_LINK_OBJ_MAX];

/* Va link create */

/* ===================================================================
 *  @func     VaLink_create                                               
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
Int32 VaLink_create(VaLink_Obj * pObj,
                    VaLink_CreateParams * pPrm)
{
    Int32 frameId;
    Int32 status;
    FVID2_Frame *pFrame;
	
    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    pObj->receviedFrameCount = 0;

    /* Get the link info from prev link */
    status = System_linkGetInfo(pObj->createArgs.inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pObj->createArgs.inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pObj->createArgs.inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* No of channels assumed is 1 */
    UTILS_assert(pObj->inQueInfo.numCh == 1);
	
    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue,           // pHndl
                             FALSE,                        // blockOnGet
                             FALSE);                       // blockOnPut
    UTILS_assert(status == FVID2_SOK);

    /* Allocate memory for the buffers */
    pObj->bufferFrameFormat.width      = pObj->inQueInfo.chInfo[0].width;
    pObj->bufferFrameFormat.height     = pObj->inQueInfo.chInfo[0].height;
    pObj->bufferFrameFormat.dataFormat = pObj->inQueInfo.chInfo[0].dataFormat;
    pObj->bufferFrameFormat.pitch[0]   = pObj->inQueInfo.chInfo[0].pitch[0];
    pObj->bufferFrameFormat.pitch[1]   = pObj->inQueInfo.chInfo[0].pitch[1];

    status = Utils_memFrameAlloc(&pObj->bufferFrameFormat, // pFormat
                                 pObj->outFrames,          // pFrame
                                 VA_LINK_MAX_OUT_FRAMES);  // numFrames
    UTILS_assert(status == FVID2_SOK);
	
    /* Queue all the buffers to empty queue */
    for (frameId = 0; frameId < VA_LINK_MAX_OUT_FRAMES; frameId++)
    {
        pFrame = &pObj->outFrames[frameId];
		pObj->perFrameCfg[frameId].bufId = (frameId + 1);
		pFrame->perFrameCfg = (VaLink_perFrameConfig*)&pObj->perFrameCfg[frameId];
		
        status = Utils_bufPutEmptyFrame(&pObj->outFrameQue, pFrame);
        UTILS_assert(status == FVID2_SOK);
    }	

#ifdef SYSTEM_DEBUG_VA
	Vps_printf(" %d: VA   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Va link delete */

/* ===================================================================
 *  @func     VaLink_delete                                               
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
Int32 VaLink_delete(VaLink_Obj * pObj)
{
    Int32 status;

	/* Free the frame memory */
	status = Utils_memFrameFree(&pObj->bufferFrameFormat,  		// pFormat
								pObj->outFrames,        	// pFrame
								VA_LINK_MAX_OUT_FRAMES);    	// numFrames

	UTILS_assert(status == FVID2_SOK);

	/* Delete the buffers */
	status = Utils_bufDelete(&pObj->outFrameQue);
	UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_VA
    Vps_printf(" %d: VA   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Va link task main function */

/* ===================================================================
 *  @func     VaLink_tskMain                                               
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
Void VaLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
	VaLink_GuiParams *pVaGuiPrm;
	UInt32 *pFrameRate;
    VaLink_Obj *pObj = (VaLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the va driver */
    status = VaLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done   = FALSE;
    ackMsg = FALSE;
	
	Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_CREATE);

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done   = TRUE;
                ackMsg = TRUE;				
                break;
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);

                if(VaLink_algCopyFrames(pObj) == FVID2_SOK)
				{
					Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_NEW_DATA);
				}	
                break;
			case VALINK_CMD_GUISETUP:						
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW ENABLE Parameters received - %d !!!\n", 
				           Clock_getTicks(),pVaGuiPrm->dmvaenable);
#endif				
				
                pObj->guiPrm.dmvaenable = pVaGuiPrm->dmvaenable; 			
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
			case VALINK_CMD_GUISETUPALGOFRAMERATE:				
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW ALGO FRAME RATE Parameters received  - %d!!!\n", 
				           Clock_getTicks(),pVaGuiPrm->dmvaexptalgoframerate);
#endif				
				
                pObj->guiPrm.dmvaexptalgoframerate = pVaGuiPrm->dmvaexptalgoframerate; 			
				Utils_tskAckOrFreeMsg(pMsg, status);	 
				break;
			case VALINK_CMD_GUISETUPALGODETECTFREQ:			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW ALGO DETECTION FREQ Parameters received - %d !!!\n", 
				           Clock_getTicks(),pVaGuiPrm->dmvaexptalgodetectfreq);
#endif				
				
                pObj->guiPrm.dmvaexptalgodetectfreq = pVaGuiPrm->dmvaexptalgodetectfreq; 			
				Utils_tskAckOrFreeMsg(pMsg, status);			
				break;
			case VALINK_CMD_GUISETUPEVTRECORDENABLE:				
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW EVENT RECORD ENABLE Parameters received - %d!!!\n", 
				           Clock_getTicks(),pVaGuiPrm->dmvaexptevtrecordenable);
#endif				
				
                pObj->guiPrm.dmvaexptevtrecordenable = pVaGuiPrm->dmvaexptevtrecordenable; 			
				Utils_tskAckOrFreeMsg(pMsg, status);			
				break;		
            case VALINK_CMD_GUISETUPSMETATRACKENABLE:			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW SMETA TRACK ENABLE Parameters received - %d !!!\n", 
				           Clock_getTicks(),pVaGuiPrm->dmvaexptsmetatrackerenable);
#endif				
				
                pObj->guiPrm.dmvaexptsmetatrackerenable = pVaGuiPrm->dmvaexptsmetatrackerenable; 			
				Utils_tskAckOrFreeMsg(pMsg, status);			
				break;			
			case VALINK_CMD_GUISETUPTZPRM:
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW TZ Parameters received !!!\n", Clock_getTicks());
#endif			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
                memcpy(&pObj->prevGuiPrm.dmvaTZPrm,&pObj->guiPrm.dmvaTZPrm,sizeof(VaLink_GuiTZParams));
				memcpy(&pObj->guiPrm.dmvaTZPrm,&pVaGuiPrm->dmvaTZPrm,sizeof(VaLink_GuiTZParams));
				Utils_tskAckOrFreeMsg(pMsg, status);
				Utils_tskSendCmd(&pObj->processTsk,VALINK_CMD_GUISETUPTZPRM);
				break;		
            case VALINK_CMD_GUISETUPMAINPRM:		
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW MAIN Parameters received !!!\n", Clock_getTicks());
#endif			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
                memcpy(&pObj->prevGuiPrm.dmvaMainPrm,&pObj->guiPrm.dmvaMainPrm,sizeof(VaLink_GuiMainParams));
				memcpy(&pObj->guiPrm.dmvaMainPrm,&pVaGuiPrm->dmvaMainPrm,sizeof(VaLink_GuiMainParams));
				Utils_tskAckOrFreeMsg(pMsg, status);				
				Utils_tskSendCmd(&pObj->processTsk,VALINK_CMD_GUISETUPMAINPRM);
                break;			
            case VALINK_CMD_GUISETUPCTDPRM:				
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW CTD Parameters received !!!\n", Clock_getTicks());
#endif			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
                memcpy(&pObj->prevGuiPrm.dmvaCTDPrm,&pObj->guiPrm.dmvaCTDPrm,sizeof(VaLink_GuiCTDParams));
				memcpy(&pObj->guiPrm.dmvaCTDPrm,&pVaGuiPrm->dmvaCTDPrm,sizeof(VaLink_GuiCTDParams));
				Utils_tskAckOrFreeMsg(pMsg, status);
				Utils_tskSendCmd(&pObj->processTsk,VALINK_CMD_GUISETUPCTDPRM);
                break;			
			case VALINK_CMD_GUISETUPOCPRM:	
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW OC Parameters received !!!\n", Clock_getTicks());
#endif			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
                memcpy(&pObj->prevGuiPrm.dmvaOCPrm,&pObj->guiPrm.dmvaOCPrm,sizeof(VaLink_GuiOCParams));
				memcpy(&pObj->guiPrm.dmvaOCPrm,&pVaGuiPrm->dmvaOCPrm,sizeof(VaLink_GuiOCParams));
				Utils_tskAckOrFreeMsg(pMsg, status);
				Utils_tskSendCmd(&pObj->processTsk,VALINK_CMD_GUISETUPOCPRM);
                break;		
            case VALINK_CMD_GUISETUPSMETAPRM:
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW SMETA Parameters received !!!\n", Clock_getTicks());
#endif			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
                memcpy(&pObj->prevGuiPrm.dmvaSMETAPrm,&pObj->guiPrm.dmvaSMETAPrm,sizeof(VaLink_GuiSMETAParams));
				memcpy(&pObj->guiPrm.dmvaSMETAPrm,&pVaGuiPrm->dmvaSMETAPrm,sizeof(VaLink_GuiSMETAParams));
				Utils_tskAckOrFreeMsg(pMsg, status);
				Utils_tskSendCmd(&pObj->processTsk,VALINK_CMD_GUISETUPSMETAPRM);
                break;			
			case VALINK_CMD_GUISETUPIMDPRM:	
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW IMD Parameters received !!!\n", Clock_getTicks());
#endif			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
                memcpy(&pObj->prevGuiPrm.dmvaIMDPrm,&pObj->guiPrm.dmvaIMDPrm,sizeof(VaLink_GuiIMDParams));
				memcpy(&pObj->guiPrm.dmvaIMDPrm,&pVaGuiPrm->dmvaIMDPrm,sizeof(VaLink_GuiIMDParams));
				Utils_tskAckOrFreeMsg(pMsg, status);
				Utils_tskSendCmd(&pObj->processTsk,VALINK_CMD_GUISETUPIMDPRM);
                break;			
			case VALINK_CMD_GUISETUPENCROIENABLE:			
			    pVaGuiPrm = (VaLink_GuiParams*)Utils_msgGetPrm(pMsg);
				
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW ENC ROI ENABLE Parameters received - %d !!!\n", 
				           Clock_getTicks(),pVaGuiPrm->dmvaEncRoiEnable);
#endif				
			
			    pObj->guiPrm.dmvaEncRoiEnable = pVaGuiPrm->dmvaEncRoiEnable; 
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
			case VALINK_CMD_CHANGE_FRAMERATE:
			    pFrameRate = (UInt32*)Utils_msgGetPrm(pMsg);
		
#ifdef SYSTEM_DEBUG_VA
				Vps_printf(" %d: VA   : NEW Frame Rate received - %d !!!\n", 
				           Clock_getTicks(),*pFrameRate);
#endif

                pObj->inFrameRate = *pFrameRate;
				Utils_tskAckOrFreeMsg(pMsg, status);
				Utils_tskSendCmd(&pObj->processTsk,VALINK_CMD_CHANGE_FRAMERATE);
				break;				
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the va driver */
    VaLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }
	
	Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_DELETE);

    return;
}

/* Va link task main function */

/* ===================================================================
 *  @func     VaLink_tskMain                                               
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
Void VaLink_processTskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg,done;
    Int32 status = FVID2_SOK;
	VaLink_Obj *pObj = (VaLink_Obj *) pTsk->appData;
	
	if(cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg,FVID2_EFAIL);
        return;
    }
	
    /* Create the va alg instance */
    status = VaLink_algCreate(pObj);
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
                done   = TRUE;
                ackMsg = TRUE;
                break;		
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
								
				VaLink_algProcessFrames(pObj);
                break;		
			case VALINK_CMD_GUISETUPTZPRM:	
				Utils_tskAckOrFreeMsg(pMsg, status);
				
				status = VaLink_algSetTZPrm(pObj);
				UTILS_assert(status == FVID2_SOK);			
				break;
            case VALINK_CMD_GUISETUPMAINPRM:					
				Utils_tskAckOrFreeMsg(pMsg, status);
				
				status = VaLink_algSetMainPrm(pObj);
				UTILS_assert(status == FVID2_SOK);
                break;		
            case VALINK_CMD_GUISETUPCTDPRM:
				Utils_tskAckOrFreeMsg(pMsg, status);
				
				status = VaLink_algSetCTDPrm(pObj);
				UTILS_assert(status == FVID2_SOK);
                break;	
            case VALINK_CMD_GUISETUPOCPRM: 				
				Utils_tskAckOrFreeMsg(pMsg, status);
				
				status = VaLink_algSetOCPrm(pObj);
				UTILS_assert(status == FVID2_SOK);
                break;			
			case VALINK_CMD_GUISETUPSMETAPRM:
				Utils_tskAckOrFreeMsg(pMsg, status);
				
				status = VaLink_algSetSMETAPrm(pObj);
				UTILS_assert(status == FVID2_SOK);
                break;			
			case VALINK_CMD_GUISETUPIMDPRM:
				Utils_tskAckOrFreeMsg(pMsg, status);
				
				status = VaLink_algSetIMDPrm(pObj);
				UTILS_assert(status == FVID2_SOK);
                break;		
            case VALINK_CMD_CHANGE_FRAMERATE:
				Utils_tskAckOrFreeMsg(pMsg, status);
				
				status = VaLink_algSetFrameRate(pObj);
				UTILS_assert(status == FVID2_SOK);
                break;			
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the va alg instance */
    status = VaLink_algDelete(pObj);
    UTILS_assert(status == FVID2_SOK);	
	
    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg,status);
    }
		
    return;		
}

/* Va Link init */

/* ===================================================================
 *  @func     VaLink_init                                               
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
Int32 VaLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 vaId;

    VaLink_Obj *pObj;

    char tskName[32];
	
	UInt32 procId = System_getSelfProcId();

    for (vaId = 0; vaId < VA_LINK_OBJ_MAX; vaId++)
    {
        pObj = &gVaLink_obj[vaId];

        memset(pObj, 0, sizeof(*pObj));
		
		pObj->tskId = SYSTEM_MAKE_LINK_ID(procId,SYSTEM_LINK_ID_VA) + vaId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = NULL;
        linkObj.linkPutEmptyFrames = NULL;
        linkObj.getLinkInfo = NULL;

        System_registerLink(pObj->tskId, &linkObj);

        sprintf(tskName, "VA%d", vaId);

        status = Utils_tskCreate(&pObj->tsk,
                                 VaLink_tskMain,
                                 VA_LINK_TSK_PRI,
                                 gVaLink_tskStack[vaId],
                                 VA_LINK_TSK_STACK_SIZE, pObj, tskName);
        UTILS_assert(status == FVID2_SOK);		
				
		/* Create DMVAL process task */
		sprintf(tskName, "VAPROCESS%d", vaId);
        status = Utils_tskCreate(&pObj->processTsk,
                                 VaLink_processTskMain,
                                 VA_LINK_PROCESSTSK_PRI,
                                 gVaLink_processTskStack[vaId],
                                 VA_LINK_PROCESSTSK_STACK_SIZE,pObj,tskName);
        UTILS_assert(status == FVID2_SOK);						
    }

    return status;
}

/* Va link de-init */

/* ===================================================================
 *  @func     VaLink_deInit                                               
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
Int32 VaLink_deInit()
{
    UInt32 vaId;

    for (vaId = 0; vaId < VA_LINK_OBJ_MAX; vaId++)
    {
        Utils_tskDelete(&gVaLink_obj[vaId].tsk);
		Utils_tskDelete(&gVaLink_obj[vaId].processTsk);
    }

    return FVID2_SOK;
}
