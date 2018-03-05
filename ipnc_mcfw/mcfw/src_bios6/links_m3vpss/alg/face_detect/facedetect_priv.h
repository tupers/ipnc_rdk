/** ==================================================================
 *  @file   facedetect_priv.h
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

#ifndef _DM81XX_FD_ALG_H_
#define _DM81XX_FD_ALG_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <ti/psp/iss/hal/iss/simcop/common/cslr.h>
#include <ti/psp/iss/alg/fd/inc/alg_fd.h>

#define FD_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )
#define FD_floor(value, align)   (( (value) / (align) ) * (align) )

#define FD_WORK_BUF_SIZE		(52480)
#define FD_MAX_DRAW_CNT			(35)
#define FD_START_FRAME_CNT		(200)
#define FD_STABILIZE_CNT		(10)

    /* FD alg handle */
    typedef struct {
        UInt32 fdWidth;
        UInt32 fdHeight;

        UInt32 fdetect;
        UInt32 maskEnable;
        MSP_FD_CREATE_PARAMS fdCreatePrm;
        MSP_FD_ROI_PARAMS fdRoi;
        Ptr workBufAddr;
        MSP_FD_RESULT_PARAMS fdResult;
        MSP_FD_RESULT_PARAMS prevFdResult;
        MSP_FD_RESULT_PARAMS *pFdDrawResult;
        Semaphore_Handle fdDrawSem;
        UInt32 fdFrameCnt;
        UInt32 stabilizeCnt;

    } FD_ALG_Hndl_t;

    /* Function prototypes */
    Int32 DM812X_FD_AlgCreate(FD_ALG_Hndl_t * pAlgHndl);
    Int32 DM812X_FD_AlgProcess(FD_ALG_Hndl_t * pAlgHndl, Ptr inFrameAddr);
    Int32 DM812X_FD_AlgDelete(FD_ALG_Hndl_t * pAlgHndl);
    Void DM812X_FD_setPrm(FD_ALG_Hndl_t * pAlgHndl, UInt32 * pPrm);

    Int32 DM812X_FD_Draw(int streamId, Ptr frameAddrY, Ptr frameAddrUV,
                         UInt32 frameWidth, UInt32 frameHeight,
                         UInt32 frameDataFormat, UInt32 framePitch,
                         UInt32 codingFormat);

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     // _DM81XX_FD_ALG_H_
