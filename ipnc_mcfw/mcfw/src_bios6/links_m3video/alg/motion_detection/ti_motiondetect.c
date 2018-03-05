/** ==================================================================
 *  @file   ti_motiondetect.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/alg/motion_detection/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* DM81XX Motion Detection Algorithm */

#include <xdc/std.h>
#include "motiondetect_priv.h"
#include <mcfw/src_bios6/links_common/system/system_priv_common.h>
#include <mcfw/src_bios6/links_m3video/iva_enc/encLink_priv.h>

    /* Motion Detection Task stack */
#pragma DATA_ALIGN(gMD_tskStack, 32)
#pragma DATA_SECTION(gMD_tskStack, ".bss:taskStackSection")
UInt8 gMD_tskStack[MD_TSK_STACK_SIZE];

    /* Motion Detection Algorithm handle */
MD_Hndl_t gMDHndl;

/* ===================================================================
 *  @func     DM81XX_MD_Run
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
int DM81XX_MD_Run(MD_Hndl_t * hndl, MD_RunPrm_t * prm, MD_RunStatus_t * status);

    /* Motion Detection task */
/* ===================================================================
 *  @func     MD_tskMain
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
Void MD_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status, mdStatus;

    MD_RunStatus_t mdRunStatus;

    if (cmd != SYSTEM_CMD_CREATE)

    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* Allocate buffers to hold the SAD data */
    gMDHndl.curSadBuf = 0;
    gMDHndl.prevSadBuf = -1;
    gMDHndl.sadBuf[0] = (UInt32) Utils_memAlloc(ENC_LINK_MV_DATA_SIZE, 32);
    gMDHndl.sadBuf[1] = (UInt32) Utils_memAlloc(ENC_LINK_MV_DATA_SIZE, 32);
    Utils_tskAckOrFreeMsg(pMsg, FVID2_SOK);
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

                /* ack or free msg */
                Utils_tskAckOrFreeMsg(pMsg, status);

                /* apply Motion Detection */
                mdStatus =
                    DM81XX_MD_Run(&gMDHndl,
                                  &gMDHndl.mdRunPrm[gMDHndl.curSadBuf],
                                  &mdRunStatus);
                if (mdStatus == ALG_MOTION_S_DETECT)

                {
                    if (gMDHndl.detectCnt > ALG_MOTION_DETECT_MAX_DETECT_CNT)

                    {
                        gMDHndl.detectCnt = 0;
                    }
                    if (gMDHndl.detectCnt == 0)

                    {

                        /* Notify A8 about the Motion Detected */
                        status = Notify_sendEvent(System_getSyslinkProcId(SYSTEM_PROC_HOSTA8),   // proc
                                                                        //
                                                  //
                                                  //
                                                  // id
                                                  SYSTEM_IPC_NOTIFY_LINE_ID,    // line
                                                                                //
                                                  //
                                                  //
                                                  // id
												  SYSTEM_IPC_NOTIFY_EVENT_ID_APP, 	// event id
                                                                                    //
                                                  //
                                                  //
                                                  // id
                                                  SYSTEM_IPC_EVTTYPE_MD,    // payload
                                                  TRUE);   // waitclear
                    }
                    gMDHndl.detectCnt++;
                }

                /* Toggle the cur sad buffer */
                if (gMDHndl.curSadBuf == 0)
                    gMDHndl.curSadBuf = 1;

                else
                    gMDHndl.curSadBuf = 0;
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
    if (ackMsg && pMsg != NULL)

    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    /* Free the SAD buffers */
    Utils_memFree((Ptr)gMDHndl.sadBuf[0], ENC_LINK_MV_DATA_SIZE);
    Utils_memFree((Ptr)gMDHndl.sadBuf[1], ENC_LINK_MV_DATA_SIZE);
    Vps_printf("Motion Detect Task Exit Completed\n");
}

    /* DM81XX Motion Detection Algorithm create */
/* ===================================================================
 *  @func     DM81XX_MD_Create
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
int DM81XX_MD_Create()
{
    Int32 status;

    gMDHndl.frameCnt = 0;
    gMDHndl.startCnt = 200;
    gMDHndl.detectCnt = 0;
    gMDHndl.motionenable = 0;
    gMDHndl.motioncenable = 0;
    gMDHndl.motioncvalue = 0;
    gMDHndl.motionlevel = ALG_MOTION_DETECT_SENSITIVITY_LOW;
    gMDHndl.motionsens = 0;
    gMDHndl.blockNum = 0;

    /* Create Motion Detection Task */
    status =
        Utils_tskCreate(&gMDHndl.tskHndl, MD_tskMain, MD_TSK_PRI,
                        gMD_tskStack, MD_TSK_STACK_SIZE, NULL, "MD_TASK");
    UTILS_assert(status == FVID2_SOK);

    /* Send SYSTEM_CMD_CREATE message to the MD task */
    Utils_tskSendCmd(&gMDHndl.tskHndl, SYSTEM_CMD_CREATE);
    return 0;
}

    /* DM81XX Motion Detection Algorithm delete */
/* ===================================================================
 *  @func     DM81XX_MD_Delete
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
int DM81XX_MD_Delete()
{
    gMDHndl.frameCnt = 0;
    gMDHndl.startCnt = 200;
    gMDHndl.detectCnt = 0;

    /* Send SYSTEM_CMD_DELETE message to the MD task */
    Utils_tskSendCmd(&gMDHndl.tskHndl, SYSTEM_CMD_DELETE);

    /* Delete Motion Detection task */
    Utils_tskDelete(&gMDHndl.tskHndl);
    return 0;
}

    /* GUI params set functions */
/* ===================================================================
 *  @func     DM81XX_MD_SetPrm
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
int DM81XX_MD_SetPrm(int streamId, int *pPrm)
{
    gMDHndl.motionenable = pPrm[0];
    gMDHndl.motioncenable = pPrm[1];
    gMDHndl.motionlevel = pPrm[2];
    gMDHndl.motioncvalue = pPrm[3];
    gMDHndl.blockNum = pPrm[4];
    return 0;
}

    /* Motion Detection Calculate */
/* ===================================================================
 *  @func     DM81XX_MD_Calc
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
int DM81XX_MD_Calc(MD_Hndl_t * hndl)
{
    unsigned int mbMV_data;

    int i, j, status;

    unsigned short mbSAD;

    mbMV_data = hndl->runPrm.mbMvInfo + hndl->MvDataOffset;
    hndl->warning_count = 0;
    for (i = 0; i < hndl->win_height; i++)

    {
        for (j = 0; j < hndl->win_width; j++)

        {
            mbSAD = *((unsigned short *) mbMV_data);
            if (mbSAD > hndl->SAD_THRESHOLD)

            {
                hndl->warning_count++;
            }
            mbMV_data += (hndl->runPrm.mvJump);
        }
        mbMV_data =
            mbMV_data +
            ((hndl->frame_width - hndl->win_width) * hndl->runPrm.mvJump);
    }

    /* If the hndl->warning_count is bigger than hndl->threshold, the
     * function will return a alarm signal */
    status =
        (hndl->warning_count >=
         hndl->threshold) ? ALG_MOTION_S_DETECT : ALG_MOTION_S_NO_DETECT;
    return status;
}

    /* DM81XX Motion Detection start */
/* ===================================================================
 *  @func     DM81XX_MD_Start
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
int DM81XX_MD_Start(MD_Hndl_t * hndl)
{
    int detect_cnt = 0;

    int i = 0, j = 0;

    hndl->runPrm.windowEnableMap[0][0] = (hndl->blockNum & 0x100) ? 1 : 0;
    hndl->runPrm.windowEnableMap[1][0] = (hndl->blockNum & 0x200) ? 1 : 0;
    hndl->runPrm.windowEnableMap[2][0] = (hndl->blockNum & 0x400) ? 1 : 0;
    hndl->runPrm.windowEnableMap[3][0] = (hndl->blockNum & 0x800) ? 1 : 0;
    hndl->runPrm.windowEnableMap[0][1] = (hndl->blockNum & 0x010) ? 1 : 0;
    hndl->runPrm.windowEnableMap[1][1] = (hndl->blockNum & 0x020) ? 1 : 0;
    hndl->runPrm.windowEnableMap[2][1] = (hndl->blockNum & 0x040) ? 1 : 0;
    hndl->runPrm.windowEnableMap[3][1] = (hndl->blockNum & 0x080) ? 1 : 0;
    hndl->runPrm.windowEnableMap[0][2] = (hndl->blockNum & 0x001) ? 1 : 0;
    hndl->runPrm.windowEnableMap[1][2] = (hndl->blockNum & 0x002) ? 1 : 0;
    hndl->runPrm.windowEnableMap[2][2] = (hndl->blockNum & 0x004) ? 1 : 0;
    hndl->runPrm.windowEnableMap[3][2] = (hndl->blockNum & 0x008) ? 1 : 0;
    hndl->MvDataOffset = 0;
    for (i = 0; ((hndl->win_height * i) < hndl->frame_height); i++)

    {
        if (i >= ALG_MOTION_DETECT_VERT_REGIONS)
            continue;
        for (j = 0; ((hndl->win_width * j) < hndl->frame_width); j++)

        {
            if (j >= ALG_MOTION_DETECT_HORZ_REGIONS)
                continue;
            if (hndl->runPrm.windowEnableMap[j][i] == 1)   // 1- True , 0 -
                // False
            {
                hndl->MvDataOffset =
                    ((hndl->win_width * j) +
                     (hndl->frame_width * hndl->win_height * i));
                hndl->MvDataOffset *= (hndl->runPrm.mvJump);
                if (DM81XX_MD_Calc(hndl) == ALG_MOTION_S_DETECT)

                {
                    hndl->runStatus.windowMotionDetected[j][i] = 1; // Motion
                                                                    //
                    //
                    //
                    // Detected
                    detect_cnt++;
                }

                else

                {
                    hndl->runStatus.windowMotionDetected[j][i] = 0; // Motion
                                                                    //
                    //
                    //
                    // Not
                    // Detected
                }
            }
        }
    }
    return ((detect_cnt > 0) ? ALG_MOTION_S_DETECT : ALG_MOTION_S_NO_DETECT);
}

    /* DM81XX Get Threshold */
/* ===================================================================
 *  @func     DM81XX_MD_GetThres
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
int DM81XX_MD_GetThres(MD_Hndl_t * hndl)
{
    int block_size;

    if (hndl == NULL)

    {
        return ALG_MOTION_S_FAIL;
    }
    hndl->frame_width = (hndl->runPrm.ImageWidth >> 4);    // Number of
    // macroblocks in
    // frame width
    hndl->frame_height = (hndl->runPrm.ImageHeight >> 4);  // Number of
    // macroblocks in
    // frame height

    /* Set the motion block size base on image size */
    hndl->win_width = (hndl->runPrm.windowWidth >> 4);     // Window width in
                                                           //
    //
    //
    // macroblocks
    hndl->win_height = (hndl->runPrm.windowHeight >> 4);   // Window height
    // in macroblocks
    block_size = hndl->win_width * hndl->win_height;
    if (block_size >= 600)                                 /* for 1080P */

    {
        hndl->SAD_THRESHOLD = 2500;
    }

    else if (block_size < 600 && block_size >= 300)        /* for 720P */

    {
        hndl->SAD_THRESHOLD = 3000;
    }

    else if (block_size < 300 && block_size >= 100)        /* for D1 */

    {
        hndl->SAD_THRESHOLD = 3500;
    }

    else if (block_size < 100 && block_size >= 20)         /* for CIF */

    {
        hndl->SAD_THRESHOLD = 4000;
    }

    else
        /* less than CIF */
    {
        hndl->SAD_THRESHOLD = 4500;
    }
    return ALG_MOTION_S_OK;
}

    /* DM81XX Get Sensitivity */
/* ===================================================================
 *  @func     DM81XX_MD_GetSensitivity
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
int DM81XX_MD_GetSensitivity(MD_Hndl_t * hndl)
{
    if (hndl == NULL)

    {
        return ALG_MOTION_S_FAIL;
    }
    hndl->motionsens = 1;                                  /* Intialize to *
                                                            * avoid non-zero
                                                            * * value */

    /* Get motion sensitivity from webpage according to user input */
    if (hndl->motioncenable == 0)

    {
        if (hndl->frame_width >= 120)                      /* 1080P = *
                                                            * 675MB/zone */

        {
            if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_LOW) // low
                                                                        //
                //
                //
                // level
            {
                hndl->motionsens = 35;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_MEDIUM) // medium
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 55;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_HIGH)   // high
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 95;
            }
        }

        else if (hndl->frame_width < 120 && hndl->frame_width >= 80)    /* 720P
                                                                         * =
                                                                         * 300MB/zone
                                                                         */

        {
            if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_LOW) // low
                                                                        //
                //
                //
                // level
            {
                hndl->motionsens = 15;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_MEDIUM) // medium
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 25;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_HIGH)   // high
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 50;
            }
        }

        else if (hndl->frame_width < 80 && hndl->frame_width >= 40) /* D1 =
                                                                     * 112.5MB/zone
                                                                     */

        {
            if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_LOW) // low
                                                                        //
                //
                //
                // level
            {
                hndl->motionsens = 7;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_MEDIUM) // medium
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 11;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_HIGH)   // high
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 20;
            }
        }

        else
            /* CIF = 20MB/zone */
        {
            if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_LOW) // low
                                                                        //
                //
                //
                // level
            {
                hndl->motionsens = 3;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_MEDIUM) // medium
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 5;
            }

            else if (hndl->motionlevel == ALG_MOTION_DETECT_SENSITIVITY_HIGH)   // high
                                                                                //
                //
                //
                // level
            {
                hndl->motionsens = 10;
            }
        }
    }                                                      // if(hndl->motioncenable
                                                           //
    //
    //
    // == 0)
    else

    {
        if (hndl->motioncvalue == 0)

        {
            hndl->motionsens = 20;
        }

        else

        {
            if (hndl->frame_width > 45)

            {
                if (hndl->motioncvalue >= 20)

                {
                    hndl->motionsens = hndl->motioncvalue;
                }

                else

                {
                    hndl->motionsens = 20;
                }
            }

            else

            {
                hndl->motionsens = (hndl->motioncvalue / 5);
                if (hndl->motionsens < 10)

                {
                    hndl->motionsens = 10;
                }
            }
        }
    }
    if (hndl->motionsens == 0)

    {
        hndl->motionsens = 1;
    }

    /* Calculate the threshold value base on the motion sensitivity */
    hndl->threshold = (hndl->win_width * hndl->win_height) / hndl->motionsens;
    return ALG_MOTION_S_OK;
}

    /* DM81XX Motion Detection Run */
/* ===================================================================
 *  @func     DM81XX_MD_Run
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
int DM81XX_MD_Run(MD_Hndl_t * hndl, MD_RunPrm_t * prm, MD_RunStatus_t * status)
{
    if (hndl == NULL)

    {
        return ALG_MOTION_S_FAIL;
    }

    /* Parm tranfer */
    hndl->runPrm = *prm;
    hndl->runStatus = *status;
    hndl->frameCnt++;
    if ((hndl->runPrm.isKeyFrame == TRUE) || (hndl->frameCnt < hndl->startCnt))

    {
        return ALG_MOTION_S_NO_DETECT;
    }
    DM81XX_MD_GetThres(hndl);
    DM81XX_MD_GetSensitivity(hndl);
    return DM81XX_MD_Start(hndl);
}

    /* DM81XX Motion Detection Apply */
/* ===================================================================
 *  @func     DM81XX_MD_Apply
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
int DM81XX_MD_Apply(Uint32 mvBuf, Uint32 frameWidth, Uint32 frameHeight,
                    Uint32 isKeyFrame)
{
    int copySize;

    AnalyticHeaderInfo *pAnalyticHeaderInfo = (AnalyticHeaderInfo *) mvBuf;

    int numElements = pAnalyticHeaderInfo->NumElements;

    if ((numElements == 0) || (isKeyFrame == 1) || (gMDHndl.motionenable == 0)
        || (gMDHndl.curSadBuf == gMDHndl.prevSadBuf))

    {
        return 0;
    }
    copySize =
        (MD_align(frameWidth, 16) / 16) * (MD_align(frameHeight, 16) / 16) *
        pAnalyticHeaderInfo->elementInfoField0SAD.Jump;

    /* copy SAD data to sad buf */
    memcpy((void *) gMDHndl.sadBuf[gMDHndl.curSadBuf],
           (void *) (mvBuf +
                     (pAnalyticHeaderInfo->elementInfoField0SAD.StartPos)),
           copySize);
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].mbMvInfo =
        gMDHndl.sadBuf[gMDHndl.curSadBuf];
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].mvJump =
        pAnalyticHeaderInfo->elementInfoField0SAD.Jump;
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].ImageWidth = frameWidth;
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].ImageHeight = frameHeight;
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].isKeyFrame = isKeyFrame;
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].codecType = 0;
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].isDateTimeDraw = 0;
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].windowWidth =
        (gMDHndl.mdRunPrm[gMDHndl.curSadBuf].ImageWidth /
         ALG_MOTION_DETECT_HORZ_REGIONS);
    gMDHndl.mdRunPrm[gMDHndl.curSadBuf].windowHeight =
        (gMDHndl.mdRunPrm[gMDHndl.curSadBuf].ImageHeight /
         ALG_MOTION_DETECT_VERT_REGIONS);
    gMDHndl.prevSadBuf = gMDHndl.curSadBuf;

    /* Send cmd to the Motion Detection task */
    Utils_tskSendCmd(&gMDHndl.tskHndl, SYSTEM_CMD_NEW_DATA);
    return 0;
}
