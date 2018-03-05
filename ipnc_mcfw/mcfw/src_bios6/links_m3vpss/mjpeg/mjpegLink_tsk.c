/** ==================================================================
 *  @file  mjpegLink_task.c
 *
 *  @path   $(IPNC_PATH)/ipnc_mcfw/mcfw/src_bios6/links_m3vpss/mjpeg
 *
 *  @desc   This  File contains the implementation of link initialization,
 *          de-initialization, listening task of the mjpeg link.
 *  =====================================================================
 *  Copyright (c) Texas Instruments Inc 2012, 2011
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "mjpegLink_priv.h"
#include <ti/psp/iss/alg/rm/inc/rm.h>

#pragma DATA_ALIGN(gMjpegLink_tskStack, 32)
#pragma DATA_SECTION(gMjpegLink_tskStack, ".bss:taskStackSection")
UInt8 gMjpegLink_tskStack[MJPEG_LINK_OBJ_MAX][MJPEG_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gMjpegLink_obj, 32)
#pragma DATA_SECTION(gMjpegLink_obj, ".bss:gMjpegLink_objSection")
MjpegLink_Obj gMjpegLink_obj[MJPEG_LINK_OBJ_MAX];

#define RUN_FRAME_NUM_MJPEG_LINK   (40 * 40)

/* ===================================================================
 *  @func     MjpegLink_PrintDetails
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
void MjpegLink_PrintDetails(MjpegLink_Obj * pObj)
{
    static int strmfrmCnt = 0;

    if (strmfrmCnt == RUN_FRAME_NUM_MJPEG_LINK)
    {
        Vps_printf
            (" ==================== MjpegLink_PrintDetails ==================== ");
        MjpegLink_printStatistics(pObj, TRUE);
        Vps_printf
            (" ================================================================= ");
        strmfrmCnt = 0;
    }
    else
    {
        strmfrmCnt++;
    }
}

/* ===================================================================
 *  @func     MjpegLink_tskMain
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
Void MjpegLink_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status;

    MjpegLink_Obj *pObj;

    MSP_COMPONENT_TYPE *jpegEncComponent = NULL;

    pObj = (MjpegLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = MjpegLink_codecCreate(pObj, Utils_msgGetPrm(pMsg));

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
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);

                MjpegLink_codecProcessData(pObj);
                MjpegLink_PrintDetails(pObj);
                break;

            case MJPEG_LINK_CMD_GET_PROCESSED_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                MjpegLink_codecGetProcessedDataMsgHandler(pObj);
                break;

            case MJPEG_LINK_CMD_GET_CODEC_PARAMS:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case MJPEG_LINK_CMD_SET_CODEC_FPS:
            	{
                MjpegLink_ChFpsParams *params;

                params = (MjpegLink_ChFpsParams *) Utils_msgGetPrm(pMsg);
                MjpegLink_codecSetFrameRate(pObj, params);
                Utils_tskAckOrFreeMsg(pMsg, status);
				}
                break;

            case MJPEG_LINK_CMD_SET_CODEC_QP:
            	{
                MjpegLink_ChQpParams *params;

                params = (MjpegLink_ChQpParams *) Utils_msgGetPrm(pMsg);
                MjpegLink_codecSetqpParamP(pObj, params);
                Utils_tskAckOrFreeMsg(pMsg, status);
				}
                break;

            case MJPEG_LINK_CMD_DISABLE_CHANNEL:
            	{
                MjpegLink_ChannelInfo *params;

                params = (MjpegLink_ChannelInfo *) Utils_msgGetPrm(pMsg);
                MjpegLink_codecDisableChannel(pObj, params);
                Utils_tskAckOrFreeMsg(pMsg, status);
				}
                break;

            case MJPEG_LINK_CMD_ENABLE_CHANNEL:
            	{
                MjpegLink_ChannelInfo *params;

                params = (MjpegLink_ChannelInfo *) Utils_msgGetPrm(pMsg);
                MjpegLink_codecEnableChannel(pObj, params);
                Utils_tskAckOrFreeMsg(pMsg, status);
				}
                break;

            case SYSTEM_CMD_STOP:
                MjpegLink_codecStop(pObj);
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

    MjpegLink_codecDelete(pObj, jpegEncComponent);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/* ===================================================================
 *  @func     MjpegLink_init
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
Int32 MjpegLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    MjpegLink_Obj *pObj;

    char name[32];

    UInt32 objId;

    for (objId = 0; objId < MJPEG_LINK_OBJ_MAX; objId++)
    {
        pObj = &gMjpegLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));
        pObj->linkId = SYSTEM_LINK_ID_MJPEG + objId;
        pObj->isSomethingSubmitted = 0;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = NULL;
        linkObj.linkPutEmptyFrames = NULL;
        linkObj.linkGetFullBitBufs = MjpegLink_getFullBufs;
        linkObj.linkPutEmptyBitBufs = MjpegLink_putEmptyBufs;
        linkObj.getLinkInfo = MjpegLink_getInfo;

        sprintf(name, "SIMCOP%d   ", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = Utils_tskCreate(&pObj->tsk,
                                 MjpegLink_tskMain,
                                 MJPEG_LINK_TSK_PRI,
                                 gMjpegLink_tskStack[objId],
                                 MJPEG_LINK_TSK_STACK_SIZE, pObj, name);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     MjpegLink_deInit
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
Int32 MjpegLink_deInit()
{
    UInt32 objId;

    MjpegLink_Obj *pObj;

    for (objId = 0; objId < MJPEG_LINK_OBJ_MAX; objId++)
    {
        pObj = &gMjpegLink_obj[objId];

        Utils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_getInfo
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
Int32 MjpegLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    MjpegLink_Obj *pObj = (MjpegLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     MjpegLink_getFullBufs
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
Int32 MjpegLink_getFullBufs(Utils_TskHndl * pTsk, UInt16 queId,
                            Bitstream_BufList * pBufList)
{
    MjpegLink_Obj *pObj = (MjpegLink_Obj *) pTsk->appData;

    UTILS_assert(queId < MJPEG_LINK_MAX_OUT_QUE);

    return Utils_bitbufGetFull(&pObj->outObj.bufOutQue, pBufList, BIOS_NO_WAIT);
}

/* ===================================================================
 *  @func     MjpegLink_putEmptyBufs
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
Int32 MjpegLink_putEmptyBufs(Utils_TskHndl * pTsk, UInt16 queId,
                             Bitstream_BufList * pBufList)
{
    MjpegLink_Obj *pObj = (MjpegLink_Obj *) pTsk->appData;

    UTILS_assert(queId < MJPEG_LINK_MAX_OUT_QUE);

    return Utils_bitbufPutEmpty(&pObj->outObj.bufOutQue, pBufList);
}
