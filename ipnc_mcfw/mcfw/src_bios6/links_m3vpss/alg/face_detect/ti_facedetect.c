/** ==================================================================
 *  @file   ti_facedetect.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/face_detect/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "facedetect_priv.h"
#include <mcfw/interfaces/link_api/fdLink.h>
#include <ih264enc.h>
#include <mcfw/src_bios6/links_m3vpss/alg/common/ti_draw.h>

FD_ALG_Hndl_t *gPfdAlgHndl = NULL;

/* FD Set params */

/* ===================================================================
 *  @func     DM812X_FD_setPrm
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
Void DM812X_FD_setPrm(FD_ALG_Hndl_t * pAlgHndl, UInt32 * pPrm)
{
    /* FD enable/disable */
    pAlgHndl->fdetect = pPrm[0];

    /* FD mask enable option */
    pAlgHndl->maskEnable = pPrm[7];

    if (pAlgHndl->maskEnable)
        pAlgHndl->fdetect = 1;

    /* ROI update */
    pAlgHndl->fdRoi.ulInputImageStartX = (pPrm[1] * pAlgHndl->fdWidth) / 100;
    pAlgHndl->fdRoi.ulInputImageStartY = (pPrm[2] * pAlgHndl->fdHeight) / 100;
    pAlgHndl->fdRoi.ulSizeX = (pPrm[3] * pAlgHndl->fdWidth) / 100;
    pAlgHndl->fdRoi.ulSizeY = (pPrm[4] * pAlgHndl->fdHeight) / 100;

    if ((pAlgHndl->fdRoi.ulSizeX == 0) || (pAlgHndl->fdRoi.ulSizeY == 0))
    {
        /* ROI size is 0 so force FD disable */
        pAlgHndl->fdetect = 0;
    }

    fd_config_ROI_update(&pAlgHndl->fdRoi);

    /* FD threshold */
    /* fd takes values between 0 - 9,0 is lower threshold */
    /* converting confidence value to threshold value */
    pAlgHndl->fdCreatePrm.ulThresholdValue = pPrm[5] / 11;
    fd_config_threshold_update(pAlgHndl->fdCreatePrm.ulThresholdValue);

    /* FD direction */
    pAlgHndl->fdCreatePrm.ulDirection = (MSP_FD_DIR_PARAM) pPrm[6];
    fd_config_direction_update(pAlgHndl->fdCreatePrm.ulDirection);

}

/* FD alg create */

/* ===================================================================
 *  @func     DM812X_FD_AlgCreate
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
Int32 DM812X_FD_AlgCreate(FD_ALG_Hndl_t * pAlgHndl)
{
    Int32 status;

    Semaphore_Params semParams;

    MSP_FD_CREATE_PARAMS *pFdCreatePrm;

    gPfdAlgHndl = pAlgHndl;

    /* create semaphore for FD draw */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;

    pAlgHndl->fdDrawSem = Semaphore_create(1u, &semParams, NULL);

    UTILS_assert(pAlgHndl->fdDrawSem != NULL);

    /* initially fd is disabled */
    pAlgHndl->fdetect = 0;
    pAlgHndl->fdFrameCnt = 0;
    pAlgHndl->stabilizeCnt = 0;
    pAlgHndl->maskEnable = 0;

    /* fd alg init */
    pFdCreatePrm = &pAlgHndl->fdCreatePrm;
    pFdCreatePrm->ulThresholdValue = 4;
    pFdCreatePrm->ulInputWidth = pAlgHndl->fdWidth;
    pFdCreatePrm->ulInputHeight = pAlgHndl->fdHeight;
    pFdCreatePrm->ulMinFaceSize = MSP_FD_MIN_SIZE_20;
    pFdCreatePrm->ulDirection = MSP_FD_DIR_UP;

    pFdCreatePrm->ptFdRoi = &pAlgHndl->fdRoi;
    pFdCreatePrm->ptFdRoi->ulInputImageStartX = 0;
    pFdCreatePrm->ptFdRoi->ulInputImageStartY = 0;
    pFdCreatePrm->ptFdRoi->ulSizeX = pAlgHndl->fdWidth;
    pFdCreatePrm->ptFdRoi->ulSizeY = pAlgHndl->fdHeight;

    status = app_fd_init(pFdCreatePrm);

    UTILS_assert(status == FVID2_SOK);

    /* alloc work are buffer */
    pAlgHndl->workBufAddr = Utils_memAlloc(FD_WORK_BUF_SIZE, 32);

    memset(&pAlgHndl->fdResult, 0x00, sizeof(MSP_FD_RESULT_PARAMS));
    memset(&pAlgHndl->prevFdResult, 0x00, sizeof(MSP_FD_RESULT_PARAMS));

#ifdef SYSTEM_DEBUG_FD
    Vps_printf(" %d: FD   : Alg Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Stabilize the FD results */

/* ===================================================================
 *  @func     DM812X_FD_Stabilize
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
Int32 DM812X_FD_Stabilize(FD_ALG_Hndl_t * pAlgHndl)
{
    pAlgHndl->pFdDrawResult = &pAlgHndl->fdResult;

    if (pAlgHndl->fdResult.ulFaceCount < pAlgHndl->prevFdResult.ulFaceCount)
    {
        if (pAlgHndl->stabilizeCnt < FD_STABILIZE_CNT)
        {
            pAlgHndl->pFdDrawResult = &pAlgHndl->prevFdResult;
            pAlgHndl->stabilizeCnt++;
        }
        else
        {
            pAlgHndl->stabilizeCnt = 0;
            memcpy(&pAlgHndl->prevFdResult, &pAlgHndl->fdResult,
                   sizeof(MSP_FD_RESULT_PARAMS));
        }
    }
    else
    {
        pAlgHndl->stabilizeCnt = 0;
        memcpy(&pAlgHndl->prevFdResult, &pAlgHndl->fdResult,
               sizeof(MSP_FD_RESULT_PARAMS));
    }

    return FVID2_SOK;
}

/* FD algorithm process */

/* ===================================================================
 *  @func     DM812X_FD_AlgProcess
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
Int32 DM812X_FD_AlgProcess(FD_ALG_Hndl_t * pAlgHndl, Ptr inFrameAddr)
{
    MSP_FD_CREATE_PARAMS *pFdCreatePrm;

    MSP_BUFHEADER_TYPE inBufHdr;

    pFdCreatePrm = &pAlgHndl->fdCreatePrm;

    if ((pAlgHndl->fdetect == 1) &&
        (pAlgHndl->fdFrameCnt >= FD_START_FRAME_CNT))
    {
        /* set the input buf addr */
        inBufHdr.unNumOfCompBufs = 2;
        inBufHdr.nPortIndex = 0;
        inBufHdr.unBufSize[0] =
            (pFdCreatePrm->ulInputWidth * pFdCreatePrm->ulInputHeight);
        inBufHdr.pBuf[0] = inFrameAddr;
        inBufHdr.unBufSize[1] = FD_WORK_BUF_SIZE;
        inBufHdr.pBuf[1] = pAlgHndl->workBufAddr;

        /* init input buffer addr */
        app_fd_Addr_init(&inBufHdr);

        /* Execute FD */
        fd_execute();

        /* Get the face info into out buf */
        Semaphore_pend(pAlgHndl->fdDrawSem, BIOS_WAIT_FOREVER);
        memset(&pAlgHndl->fdResult, 0x00, sizeof(MSP_FD_RESULT_PARAMS));
        FD_get_num_faces(&pAlgHndl->fdResult);
        FD_get_all_face_detect_positions(&pAlgHndl->fdResult);
        DM812X_FD_Stabilize(pAlgHndl);
        Semaphore_post(pAlgHndl->fdDrawSem);
    }
    else
    {
        pAlgHndl->fdFrameCnt++;
    }

    return FVID2_SOK;
}

/* FD algorithm delete */

/* ===================================================================
 *  @func     DM812X_FD_AlgDelete
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
Int32 DM812X_FD_AlgDelete(FD_ALG_Hndl_t * pAlgHndl)
{
    /* delete FD draw sem */
    Semaphore_delete(&pAlgHndl->fdDrawSem);

    /* Free the work buffer */
    Utils_memFree(pAlgHndl->workBufAddr, FD_WORK_BUF_SIZE);

    /* FD alg deinit */
    app_fd_deinit();

    gPfdAlgHndl = NULL;

#ifdef SYSTEM_DEBUG_FD
    Vps_printf(" %d: FD   : Alg Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Draw the FD rectangles */

/* ===================================================================
 *  @func     DM812X_FD_Draw
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
Int32 DM812X_FD_Draw(int streamId, Ptr frameAddrY, Ptr frameAddrUV,
                     UInt32 frameWidth, UInt32 frameHeight,
                     UInt32 frameDataFormat, UInt32 framePitch,
                     UInt32 codingFormat)
{
    UInt32 numFace;

    FD_ALG_Hndl_t *pAlgHndl = gPfdAlgHndl;

    DRAW_InFrame_Info_t inFrameInfo;

    DRAW_Rect_Info_t rectInfo;

    UInt32 fdWidth, fdHeight, centreX, centreY;

    FdLink_result fdResult;

    UInt32 fdResultOffset, fdResultAddr;

    UInt32 blankingDataSize;

    UInt32 codecPrivacyMask;

    UInt32 fdRoiX,fdRoiY,fdRoiW,fdRoiH;

    UInt32 faceCnt;

    if (pAlgHndl == NULL)
    {
        return FVID2_SOK;
    }

    fdWidth = pAlgHndl->fdWidth;
    fdHeight = pAlgHndl->fdHeight;

#ifdef CODEC_PRIV_MASK
    if ((codingFormat == IVIDEO_H264BP) || (codingFormat == IVIDEO_H264MP) ||
        (codingFormat == IVIDEO_H264HP))
    {
        codecPrivacyMask = 1;
    }
    else
    {
        codecPrivacyMask = 0;
    }
    codecPrivacyMask = 1;
#else
    codecPrivacyMask = 0;
#endif

    if (codecPrivacyMask == 1)
    {
        if (streamId == 0)
        {
            blankingDataSize = BLANKING_DATA_SIZE;
        }
        else
        {
            blankingDataSize = 0;
        }

        if ((frameDataFormat == FVID2_DF_YUV420SP_VU) ||
            (frameDataFormat == FVID2_DF_YUV420SP_UV) ||
            (frameDataFormat == FVID2_DF_YUV420P))
        {
            //fdResultOffset =
            //   ((framePitch * frameHeight * 3) >> 1) + blankingDataSize;
            fdResultOffset = ((unsigned int)frameAddrUV - (unsigned int)frameAddrY) +
                ((framePitch * frameHeight) >> 1)    + blankingDataSize;
        }
        else
        {
            fdResultOffset =
                ((framePitch * frameHeight) << 1) + blankingDataSize;
        }

        fdResultAddr = (UInt32) frameAddrY + fdResultOffset;
    }                                                      // if(codecPrivacyMask
                                                           //
    // == 1)

    if ((pAlgHndl->fdetect == 0) ||
        (pAlgHndl->pFdDrawResult->ulFaceCount >= FD_MAX_DRAW_CNT))
    {
        if (codecPrivacyMask == 1)
        {
            fdResult.numFaces = 0;

            /* write the FD result at the end of the buffer */
            memcpy((void *) fdResultAddr, &fdResult, sizeof(FdLink_result));
        }

        return FVID2_SOK;
    }

    /* input frame info */
    inFrameInfo.videoInOutAddrY = (UInt8 *) frameAddrY;
    inFrameInfo.videoInOutAddrUV = (UInt8 *) frameAddrUV;
    inFrameInfo.videoWidth = frameWidth;
    inFrameInfo.videoHeight = frameHeight;
    inFrameInfo.videoOffsetH = framePitch;
    inFrameInfo.videoOffsetV = frameHeight;
    inFrameInfo.videoDataFormat = frameDataFormat;
    inFrameInfo.maskEnable = pAlgHndl->maskEnable;

    rectInfo.colorY = 0xFF;
    rectInfo.colorC = 0x80;
    rectInfo.thickness = 4;

    Semaphore_pend(pAlgHndl->fdDrawSem, BIOS_WAIT_FOREVER);

    faceCnt = 0;

    for (numFace = 0; numFace < pAlgHndl->pFdDrawResult->ulFaceCount; numFace++)
    {
        /* reactangle info */
        centreX =
            (pAlgHndl->pFdDrawResult->tFacePosition[numFace].ulResultX *
             inFrameInfo.videoWidth) / fdWidth;
        centreY =
            (pAlgHndl->pFdDrawResult->tFacePosition[numFace].ulResultY *
             inFrameInfo.videoHeight) / fdHeight;

        rectInfo.width =
            (pAlgHndl->pFdDrawResult->tFacePosition[numFace].ulResultSize *
             inFrameInfo.videoWidth) / fdWidth;
        rectInfo.height =
            (pAlgHndl->pFdDrawResult->tFacePosition[numFace].ulResultSize *
             inFrameInfo.videoHeight) / fdHeight;
        rectInfo.startX = centreX - rectInfo.width / 2;
        rectInfo.startY = centreY - rectInfo.height / 2;

        fdRoiX = (pAlgHndl->fdRoi.ulInputImageStartX * inFrameInfo.videoWidth) / fdWidth;
        fdRoiY = (pAlgHndl->fdRoi.ulInputImageStartY * inFrameInfo.videoHeight) / fdHeight;
        fdRoiW = (pAlgHndl->fdRoi.ulSizeX * inFrameInfo.videoWidth) / fdWidth;
        fdRoiH = (pAlgHndl->fdRoi.ulSizeY * inFrameInfo.videoHeight) / fdHeight;

        if((rectInfo.startX < fdRoiX) ||
           (rectInfo.startY < fdRoiY))
        {
		    continue;
		}

        if(((rectInfo.startX + rectInfo.width) > (fdRoiX + fdRoiW)) ||
           ((rectInfo.startY + rectInfo.height) > (fdRoiY + fdRoiH)))
        {
		    continue;
		}

        if ((codecPrivacyMask == 1) && (pAlgHndl->maskEnable == 1))
        {
            fdResult.faceInfo[faceCnt].startX = rectInfo.startX;
            fdResult.faceInfo[faceCnt].startY = rectInfo.startY;
            fdResult.faceInfo[faceCnt].width = rectInfo.width;
            fdResult.faceInfo[faceCnt].height = rectInfo.height;
            faceCnt ++;
        }
        else
        {
            /* Draw the rectangles */
            DM812X_DRAW_rectangle(&inFrameInfo, &rectInfo, framePitch);
        }
    }

    if ((codecPrivacyMask == 1) && (pAlgHndl->maskEnable == 1))
    {
        fdResult.numFaces = faceCnt;
    }
    else
    {
        fdResult.numFaces = 0;
    }

    Semaphore_post(pAlgHndl->fdDrawSem);

    if (codecPrivacyMask == 1)
    {
        /* write the FD result at the end of the buffer */
        memcpy((void *) fdResultAddr, &fdResult, sizeof(FdLink_result));
    }

    return FVID2_SOK;
}
