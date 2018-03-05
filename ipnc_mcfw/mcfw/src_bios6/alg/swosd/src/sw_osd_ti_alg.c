/*
 *  Copyright 2008
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
#include <sw_osd_ti_priv.h>
#include <sw_osd_ti_dmaOpt.h>

#define SWOSD_USE_INTRINSICS



#ifdef SWOSD_USE_INTRINSICS
  #define SWOSD_TI_algBlendLine SWOSD_TI_algBlendLine_i
#else
  #define SWOSD_TI_algBlendLine SWOSD_TI_algBlendLine_c
#endif



#define SWOSD_ALPHA_QSHIFT         (7)
#define SWOSD_ALPHA_QSHIFT_PACKED  (0x00070007)
#define SWOSD_ALPHA_MAX_PACKED     (0x80808080)
//#define SWOSD_TI_USE_ECPY

inline void SWOSD_TI_algBlendLine_i
(
 Int64 * restrict pInA,
 Int64 * restrict pInB,
 Int64 * restrict pOut,
 Int32 * restrict pAlp,
 UInt16 width,
 Bool transperencyEnable
)
{
  Int32 len;
  Int64 inB64Val, inA64Val;
  UInt16 alp16Val;
  Int32 aGain, bGain;
  Int32 sumH, sumL;
  Int32 outH, outL;
  Int64 mulA64, mulB64;
  Int32 alpH, alpL;
  UInt16 *alp16;
  UInt16 *pTmpB;
  UInt8  *pTmpAlp;

  len = (width)>>3;

  alp16 = (UInt16*)pAlp;

  pTmpB = (UInt16*)pInB;
  pTmpAlp = (UInt8*)pAlp;

  if(transperencyEnable) {
    while(width--) {
      if(! ( *pTmpB++ ) )
        *pTmpAlp = 0;
      pTmpAlp++;
    }
  }

  while(len--)
    {

    inA64Val = *pInA++;
    inB64Val = *pInB++;

    alp16Val = *(alp16+1); // 0.92 fix

    alpH  = _rotl(alp16Val, 8);
    alpL  = _swap4(alpH);
    bGain = alpL | alpH;

    aGain = _sub4(SWOSD_ALPHA_MAX_PACKED, bGain);

    mulA64 = _mpyu4ll( _hill(inA64Val), aGain);
    mulB64 = _mpyu4ll( _hill(inB64Val), bGain);

    sumH  = _add2(_hill(mulA64), _hill(mulB64));
    sumL  = _add2(_loll(mulA64), _loll(mulB64));

    sumH  = _shr2(sumH, SWOSD_ALPHA_QSHIFT_PACKED);
    sumL  = _shr2(sumL, SWOSD_ALPHA_QSHIFT_PACKED);

    outH  = _packl4(sumH, sumL);

    alp16Val = *alp16; // 0.92 fix

    alpH  = _rotl(alp16Val, 8);
    alpL  = _swap4(alpH);
    bGain = alpL | alpH;

    aGain = _sub4(SWOSD_ALPHA_MAX_PACKED, bGain);

    mulA64 = _mpyu4ll( _loll(inA64Val), aGain);
    mulB64 = _mpyu4ll( _loll(inB64Val), bGain);

    sumH  = _add2(_hill(mulA64), _hill(mulB64));
    sumL  = _add2(_loll(mulA64), _loll(mulB64));

    sumH  = _shr2(sumH, SWOSD_ALPHA_QSHIFT_PACKED);
    sumL  = _shr2(sumL, SWOSD_ALPHA_QSHIFT_PACKED);

    outL  = _packl4(sumH, sumL);

    *pOut++ = _itoll(outH, outL);

    alp16 += 2; // 0.92 fix
    }
}

inline void SWOSD_TI_algBlendLine_c
(
 Int64 * restrict pInA,
 Int64 * restrict pInB,
 Int64 * restrict pOut,
 Int32 * restrict pAlp,
 UInt16 width,
 Bool transperencyEnable
)
{
  Int32 len;
  Int64 inB64Val, inA64Val;
  Int32 alp32Val;
  UInt8  *inA8, *inB8, *out8, *gain;
  UInt16 *pTmpB;
  UInt8  *pTmpAlp;

  len = (width)>>2;

  if(transperencyEnable) {
    pTmpB = (UInt16*)pInB;
    pTmpAlp = (UInt8*)pAlp;

    while(width--) {
      if(! ( *pTmpB++ ) )
        *pTmpAlp = 0;
      pTmpAlp++;
    }
  }

  inA8 = (UInt8*)&inA64Val;
  inB8 = (UInt8*)&inB64Val;
  gain = (UInt8*)&alp32Val;
  out8 = (UInt8*)pOut;

  while(len--)
  {
    inA64Val = *pInA++;
    alp32Val = *pAlp++;
    inB64Val = *pInB++;

    out8[7] = inA8[7] + ( ((inB8[7]-inA8[7])*gain[3]) >> SWOSD_ALPHA_QSHIFT );
    out8[5] = inA8[5] + ( ((inB8[5]-inA8[5])*gain[2]) >> SWOSD_ALPHA_QSHIFT );
    out8[3] = inA8[3] + ( ((inB8[3]-inA8[3])*gain[1]) >> SWOSD_ALPHA_QSHIFT );
    out8[1] = inA8[1] + ( ((inB8[1]-inA8[1])*gain[0]) >> SWOSD_ALPHA_QSHIFT );
    out8[6] = inA8[6] + ( ((inB8[6]-inA8[6])*gain[3]) >> SWOSD_ALPHA_QSHIFT );
    out8[4] = inA8[4] + ( ((inB8[4]-inA8[4])*gain[2]) >> SWOSD_ALPHA_QSHIFT );
    out8[2] = inA8[2] + ( ((inB8[2]-inA8[2])*gain[1]) >> SWOSD_ALPHA_QSHIFT );
    out8[0] = inA8[0] + ( ((inB8[0]-inA8[0])*gain[0]) >> SWOSD_ALPHA_QSHIFT );

    out8 += 8;
  }
}

void SWOSD_TI_algBlendFrame
(
    SWOSD_TI_Obj *swOsdObj,
    UInt8 *pInA, UInt8 *pInB, UInt8 *pAlp,
    UInt8 *pOut,
    UInt16 width, UInt16 height,
    UInt16 lineOffsetA,
    UInt16 lineOffsetB,
    UInt16 lineOffsetOut,
    Bool useAlphaWindow,
    Bool useDmaManualTrigger
)
{

  UInt16 i, offset;
  Int64 *pLineBufA[2];
  Int64 *pLineBufB[2], *pLineBufOut[2];
  Int32 *pLineBufAlp;
  volatile UInt32 *srcParamPtr, *dstParamPtr;
  Int32 srcIncr, dstIncr, srcPitch, dstPitch;
  UInt32 srcAddr, dstAddr;
  Int32 dummySrc = 0, dummyDst = 0;

  Int32 dmaBuf;
  Bool   transperencyEnable;
#ifndef SWOSD_TI_USE_ECPY
  SWOSD_DMAObj *dmaHandle;
  UInt32 lowTriggerWord;
  UInt32 highTriggerWord;
  UInt32 evt_no;
#endif

#ifdef SWOSD_TI_USE_ECPY
  SWOSD_TI_dmaActivate(swOsdObj);

  SWOSD_TI_dmaSetup(swOsdObj, width, useAlphaWindow);
#endif

  transperencyEnable = swOsdObj->swOsdCtrl.globalPrm.transperencyEnable;

  offset = 0;

  pLineBufA[0]  = (Int64*)(swOsdObj->memLineBuf + offset);
  offset += width;

  pLineBufA[1]  = (Int64*)(swOsdObj->memLineBuf + offset);
  offset += width;

  pLineBufB[0]   = (Int64*)((Int32)swOsdObj->memLineBuf  + offset);
  offset += width;

  pLineBufB[1]   = (Int64*)((Int32)swOsdObj->memLineBuf  + offset);
  offset += width;

  pLineBufOut[0] = (Int64*)((Int32)swOsdObj->memLineBuf + offset);
  offset += width;

  pLineBufOut[1] = (Int64*)((Int32)swOsdObj->memLineBuf + offset);
  offset += width;

  pLineBufAlp = (Int32*)((Int32)swOsdObj->memLineBuf + offset);

  if(!useAlphaWindow) {
    UInt8 alp8  = swOsdObj->swOsdCtrl.globalPrm.globalAlpha;
    memset(pLineBufAlp, alp8, width);
  }

#ifndef SWOSD_TI_USE_ECPY


  dmaHandle = (SWOSD_DMAObj *)&swOsdObj->dmaHandle;

  lowTriggerWord = 0;
  highTriggerWord = 0;
  evt_no  = dmaHandle->edma_phy_to_lgl_map[SWOSD_DMA_CH_IN_A];
  SWOSD_TI_DMA_setTriggerWord(evt_no, lowTriggerWord, highTriggerWord);
  evt_no  = dmaHandle->edma_phy_to_lgl_map[SWOSD_DMA_CH_IN_B];
  SWOSD_TI_DMA_setTriggerWord(evt_no, lowTriggerWord, highTriggerWord);
  evt_no  = dmaHandle->edma_phy_to_lgl_map[SWOSD_DMA_CH_OUT];
  SWOSD_TI_DMA_setTriggerWord(evt_no, lowTriggerWord, highTriggerWord);

  ECPY_activate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_IN_A]);
  ECPY_activate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_IN_B]);
  ECPY_activate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_OUT]);

  /* Tempory Fix OSD Chroma offset issue with tiler mode.
    * Enabled manual pitch increment in edma copy. Flag - SWOSD_TI_MANUAL_TRIGGER.
    * Issue was due to 15+1 <sign> bit for pitch in edma registers which is not enough for pitch
    * of 32768 <tiler chroma pitch>. Auto increment will not work.
    * So enabled manual increment. Right now it does manual increment by default as SWOSD_TI_MANUAL_TRIGGER is enabled.
    * Change to do - check for lineOffset >= 32768 and take manual increment path.
    * Current change will still work - need to relook and add check for >= 32768 if performance
    * gets affected
    */

    if(useDmaManualTrigger)
    {
            srcParamPtr = (UInt32 *) dmaHandle->edma_params_array[SWOSD_DMA_CH_IN_A] + 1;
            dstParamPtr = (UInt32 *) dmaHandle->edma_params_array[SWOSD_DMA_CH_OUT] + 3;
            srcAddr  = (UInt32) pInA;
            dstAddr  = (Uint32) pOut;
            srcPitch = 0;
            dstPitch = 0;
            srcIncr  = lineOffsetA;
            dstIncr  = lineOffsetOut;
    }
    else
    {
            srcParamPtr = (UInt32 *) &dummySrc;
            dstParamPtr = (UInt32 *) &dummyDst;
            srcAddr  = (UInt32) pInA;
            dstAddr  = (Uint32) pOut;
            srcPitch = lineOffsetA;
            dstPitch = lineOffsetOut;
            srcIncr  = 0;
            dstIncr  = 0;
    }

    SWOSD_TI_DMA_Fast2D1D
    (
        dmaHandle,
        SWOSD_DMA_CH_IN_A,
        (void *)pInA,
        (void *)((UInt32)pLineBufA[0] + 0x30000000),
        width,
        2,
        srcPitch,
        width,
        srcPitch,
        (-width)
    );

    SWOSD_TI_DMA_Fast2D1D
    (
        dmaHandle,
        SWOSD_DMA_CH_IN_B,
        (void *)pInB,
        (void *)((UInt32)pLineBufB[0] + 0x30000000),
        width,
        2,
        lineOffsetB,
        width,
        lineOffsetB,
        (-width)
    );

    SWOSD_TI_DMA_Fast2D1D
    (
        dmaHandle,
        SWOSD_DMA_CH_OUT,
        (void *)((UInt32)pLineBufOut[0] + 0x30000000),
        (void *)pOut,
        width,
        2,
        width,
        dstPitch,
        (-width),
        dstPitch
    );

    SWOSD_TI_DMA_FastStart(dmaHandle, SWOSD_DMA_CH_IN_A);
    SWOSD_TI_DMA_FastStart(dmaHandle, SWOSD_DMA_CH_IN_B);

    SWOSD_TI_DMA_FastWait(dmaHandle);

    SWOSD_TI_DMA_ManualUpdate(srcParamPtr, srcAddr, srcIncr);

    SWOSD_TI_DMA_FastStart(dmaHandle, SWOSD_DMA_CH_IN_A);
    SWOSD_TI_DMA_FastStart(dmaHandle, SWOSD_DMA_CH_IN_B);

    dmaBuf = 0;
    for (i = 0; i < height; i++)
    {
        if(i>0)
        {
            SWOSD_TI_DMA_MultiStart(dmaHandle, lowTriggerWord, highTriggerWord);
        }

        SWOSD_TI_algBlendLine(
            pLineBufA[dmaBuf],
            pLineBufB[dmaBuf],
            pLineBufOut[dmaBuf],
            pLineBufAlp,
            width,
            transperencyEnable
        );

        dmaBuf ^= 1;

        SWOSD_TI_DMA_FastWait(dmaHandle);
        SWOSD_TI_DMA_ManualUpdate(srcParamPtr, srcAddr, srcIncr);
        if(i > 0) {
          SWOSD_TI_DMA_ManualUpdate(dstParamPtr, dstAddr, dstIncr);
        }
    }

    SWOSD_TI_DMA_FastStart(dmaHandle, SWOSD_DMA_CH_OUT);
    SWOSD_TI_DMA_FastWait(dmaHandle);

  ECPY_deactivate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_IN_A]);
  ECPY_deactivate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_IN_B]);
  ECPY_deactivate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_OUT]);

#else
  dmaBuf = 0;

  SWOSD_TI_dmaStart(swOsdObj,
                    pInA, pInB, pAlp, pOut,
                    pLineBufA[dmaBuf], pLineBufB[dmaBuf], pLineBufAlp, pLineBufOut[dmaBuf], width
                    );

  SWOSD_TI_dmaWait(swOsdObj);

  dmaBuf ^= 1;

  for (i = 0; i < height; i++)
  {
    if(i!=(height-1)) {
        pInA += lineOffsetA;
        pInB += lineOffsetB;
        pAlp += lineOffsetB;
    }

    SWOSD_TI_dmaStart(swOsdObj,
                      pInA, pInB, pAlp, pOut,
                      pLineBufA[dmaBuf], pLineBufB[dmaBuf], pLineBufAlp, pLineBufOut[dmaBuf], width
                      );

    if(i>0)
    {
        pOut = pOut + lineOffsetOut;
    }

    SWOSD_TI_algBlendLine( pLineBufA[dmaBuf ^ 1],
                           pLineBufB[dmaBuf ^ 1],
                           pLineBufOut[dmaBuf ^ 1],
                           pLineBufAlp,
                           width,
                           transperencyEnable
                          );

    SWOSD_TI_dmaWait(swOsdObj);

    dmaBuf ^= 1;
  }

  SWOSD_TI_dmaStart(swOsdObj,
                    pInA, pInB, pAlp, pOut,
                    pLineBufA[dmaBuf], pLineBufB[dmaBuf], pLineBufAlp, pLineBufOut[dmaBuf], width
                  );


  SWOSD_TI_dmaWait(swOsdObj);

  SWOSD_TI_dmaDeactivate(swOsdObj);
#endif
}

// blend video with graphics to give a blended output
int SWOSD_TI_algRun(SWOSD_TI_Obj *swOsdObj, SWOSD_Obj  *swOsdCtrl)
{
    UInt8 *pInA;
    UInt8 *pInB;
    UInt8 *pAlp;
    UInt16 incInA, incInB;
    Bool   useAlphaWindow;

    /* this MUST be TRUE when pitch is > 0x7FFF
        This is needed since EDMA pitch size is 16-bit in signed mode.
        So pitch greater than 0x7FFF gets treated as negative pitch
        giving wrong results
    */
    Bool   useDmaManualTrigger = FALSE;


    if(swOsdCtrl->videoWindowAddr==NULL || swOsdCtrl->graphicsWindowAddr==NULL)
        return SWOSD_EFAIL;

    if(swOsdCtrl->alphaWindowAddr==NULL)
      useAlphaWindow = FALSE;
    else
      useAlphaWindow = TRUE;

    if(!useAlphaWindow && swOsdCtrl->globalPrm.globalAlpha==0x00)
    {
        /* zero alpha, no OSD effectively, just return */
        return SWOSD_SOK;
    }

    pInA = swOsdCtrl->videoWindowAddr;
    pInB = swOsdCtrl->graphicsWindowAddr;
    pAlp = swOsdCtrl->alphaWindowAddr;

    pInA += (swOsdCtrl->graphicsWindowPrm.startY * swOsdCtrl->videoWindowPrm.lineOffset);
    pInA += (swOsdCtrl->graphicsWindowPrm.startX);

    if(swOsdCtrl->videoWindowPrm.lineOffset > 0x7FFF
        ||
        swOsdCtrl->graphicsWindowPrm.lineOffset > 0x7FFF
      )
    {
        useDmaManualTrigger = TRUE;
    }

    /** GS - Temporary Fix. If no transparency is enabled <alpha >= 0x80>, set some alpha level to 
       *  to enable DSP blending. Reason - in DSP blending mode no flicker is observed. 
       * This workaround will be removed after proper fix for flicker issue in opaque mode
       */
     if (swOsdCtrl->globalPrm.globalAlpha >= 0x80)
        swOsdCtrl->globalPrm.globalAlpha = 0x7F;

    if(!useAlphaWindow && swOsdCtrl->globalPrm.globalAlpha>=0x80)
    {
        /* unity alpha, no blending needed, just copy the buffer */
#ifdef SWOSD_TI_USE_ECPY
        SWOSD_TI_algMemcpy2D(
                swOsdObj,
                (UInt8*)pInA,
                (UInt8*)pInB,
                swOsdCtrl->graphicsWindowPrm.width,
                swOsdCtrl->graphicsWindowPrm.height,
                swOsdCtrl->graphicsWindowPrm.lineOffset,
                swOsdCtrl->videoWindowPrm.lineOffset
            );
#else
        ECPY_activate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_ALP]);
        ECPY_activate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_AUX]);

        if(useDmaManualTrigger)
        {
            volatile UInt32 *dstParamPtr;
            UInt8 *srcPtr, *dstPtr;
            Int32 height, width, ctr;
            Int32 dstPitch, srcPitch, dstAddr, dstIncr;

            srcPtr   = (UInt8 *) pInB;
            dstPtr   = (UInt8 *) pInA;
            dstAddr  = (UInt32)  pInA;
            width    = swOsdCtrl->graphicsWindowPrm.width;
            height   = swOsdCtrl->graphicsWindowPrm.height;
            srcPitch = swOsdCtrl->graphicsWindowPrm.lineOffset;
            dstPitch = 0;
            dstIncr  = swOsdCtrl->videoWindowPrm.lineOffset;
            dstParamPtr = (UInt32 *)(((UInt32 *)swOsdObj->dmaHandle.edma_params_array[SWOSD_DMA_CH_AUX]) + 3);

            SWOSD_TI_DMA_Fast2D1D
            (
                    &swOsdObj->dmaHandle,
                    SWOSD_DMA_CH_AUX,
                    (void *)srcPtr,
                    (void *)dstPtr,
                    width,
                    height,
                    srcPitch,
                    dstPitch,
                    srcPitch,
                    dstPitch
            );

            for(ctr = 0; ctr < height; ctr++)
            {
                SWOSD_TI_DMA_FastStart(&swOsdObj->dmaHandle, SWOSD_DMA_CH_AUX);
                SWOSD_TI_DMA_FastWait(&swOsdObj->dmaHandle);
                SWOSD_TI_DMA_ManualUpdate(dstParamPtr, dstAddr, dstIncr);
            }
        }
        else
        {
            SWOSD_TI_DMA_Fast2D2D(
                &swOsdObj->dmaHandle,
                SWOSD_DMA_CH_ALP,
                (void *)pInB,
                (void *)pInA,
                swOsdCtrl->graphicsWindowPrm.width,
                swOsdCtrl->graphicsWindowPrm.height,
                swOsdCtrl->graphicsWindowPrm.lineOffset,
                swOsdCtrl->videoWindowPrm.lineOffset
            );

            SWOSD_TI_DMA_FastWait(&swOsdObj->dmaHandle);
        }

        ECPY_deactivate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_AUX]);
        ECPY_deactivate(swOsdObj->ecpyHandle[SWOSD_DMA_CH_ALP]);
#endif
        return SWOSD_SOK;
    }

    incInA = swOsdCtrl->videoWindowPrm.lineOffset;
    incInB = swOsdCtrl->graphicsWindowPrm.lineOffset;

    SWOSD_TI_algBlendFrame
      (
        swOsdObj,
        pInA, pInB, pAlp, pInA,
        swOsdCtrl->graphicsWindowPrm.width,
        swOsdCtrl->graphicsWindowPrm.height,
        incInA,
        incInB,
        incInA,
        useAlphaWindow,
        useDmaManualTrigger
      );
    return SWOSD_SOK;
}

