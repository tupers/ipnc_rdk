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

int SWOSD_TI_algMemcpy2D(SWOSD_TI_Obj *swOsdObj, UInt8 *dest, UInt8 *src, UInt16 width, UInt16 height, UInt16 lineOffsetSrc, UInt16 lineOffsetDest)
{
  ECPY_Params params;

  if(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_MEMCPY]==NULL)
    return SWOSD_EFAIL;

  ECPY_activate(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_MEMCPY]);

  /* If src or dst is internal memory, offset the address by 0x30000000 */
  /* as all internal memory via EDMA is accessed using L3 address space */
  if(((UInt32)src >= 0x10800000u) && ((UInt32)src < 0x10840000u))
  {
	  src += 0x30000000;
  }
  if(((UInt32)dest >= 0x10800000u) && ((UInt32)dest < 0x10840000u))
  {
	  dest += 0x30000000;
  }

  params.transferType = ECPY_2D2D;
  params.srcAddr = src;
  params.dstAddr = dest;
  params.elementSize = width;
  params.numElements = height;
  params.numFrames = 1;
  params.srcElementIndex = lineOffsetSrc;
  params.dstElementIndex = lineOffsetDest;
  params.srcFrameIndex = 0;
  params.dstFrameIndex = 0;

  ECPY_directConfigure(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_MEMCPY], &params, 1);

  /* submit as no linked transfers */
  ECPY_directSetFinal(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_MEMCPY], 1);

  ECPY_directStartEdma(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_MEMCPY]);
  ECPY_directWait(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_MEMCPY]);

  ECPY_deactivate(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_MEMCPY]);

  return SWOSD_SOK;
}

void SWOSD_TI_dmaActivate(SWOSD_TI_Obj *swOsdObj)
{
  ECPY_activate(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA]);
}

void SWOSD_TI_dmaDeactivate(SWOSD_TI_Obj *swOsdObj)
{
  ECPY_deactivate(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA]);
}


void SWOSD_TI_dmaStart(SWOSD_TI_Obj *swOsdObj,
                       UInt8 *pInA, UInt8 *pInB,
                       UInt8 *pInAlp, UInt8 *pOut,
                       UInt64 *pLineBufA, UInt64 *pLineBufB,
                       Int32 *pLineBufAlp, UInt64 *pLineBufOut,
					   UInt16 width
                      )
{
	ECPY_Params params;

	params.transferType = ECPY_1D1D;
	params.srcAddr = (Ptr)pInA;
	params.dstAddr = (Ptr)((Uns)pLineBufA + 0x30000000);
	params.numElements = 1;
	params.numFrames = 1;
	params.srcElementIndex = 0;
	params.dstElementIndex = 0;
	params.srcFrameIndex = 0;
	params.dstFrameIndex = 0;
    params.elementSize = width;

    ECPY_directConfigure(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], &params, SWOSD_DMA_CH_IN_A + 1);


	ECPY_directConfigure32(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_DSTADDR, (Uns)pLineBufB + 0x30000000, SWOSD_DMA_CH_IN_B + 1);
	ECPY_directConfigure32(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_SRCADDR, (Uns)pInB, SWOSD_DMA_CH_IN_B + 1);

	ECPY_directConfigure32(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_DSTADDR, (Uns)pLineBufAlp + 0x30000000, SWOSD_DMA_CH_ALP + 1);
	ECPY_directConfigure32(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_SRCADDR, (Uns)pInAlp, SWOSD_DMA_CH_ALP + 1);

	ECPY_directConfigure32(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_DSTADDR, (Uns)pOut, SWOSD_DMA_CH_OUT + 1);
	ECPY_directConfigure32(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_SRCADDR, (Uns)pLineBufOut + 0x30000000, SWOSD_DMA_CH_OUT + 1);

      /* submit as a four linked 1D transfers */
	ECPY_directSetFinal(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], SWOSD_DMA_CH_MAX);

	ECPY_directStartEdma(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA]);
}

void SWOSD_TI_dmaWait(SWOSD_TI_Obj *swOsdObj)
{
  ECPY_directWait(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA]);
}

void SWOSD_TI_dmaSetup(SWOSD_TI_Obj *swOsdObj, UInt16 width, Bool dmaAlpha)
{
  ECPY_Params params;
  int i;

  if(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA]==NULL)
    return;

  params.transferType = ECPY_1D1D;
  params.srcAddr = NULL;
  params.dstAddr = NULL;
  params.numElements = 1;
  params.numFrames = 1;
  params.srcElementIndex = 0;
  params.dstElementIndex = 0;
  params.srcFrameIndex = 0;
  params.dstFrameIndex = 0;

  ECPY_setEarlyCompletionMode(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], 0);

  for(i=0; i<SWOSD_DMA_CH_MAX; i++) {

    if(i==SWOSD_DMA_CH_ALP) {
      // alpha
      if(dmaAlpha)
        params.elementSize = width;
      else
        params.elementSize = 0;
    } else {
      params.elementSize = width;
    }

    ECPY_directConfigure(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], &params, i+1);
  }

      /* submit as a four linked 1D transfers */
    ECPY_directSetFinal(swOsdObj->ecpyHandle[SWOSD_DMA_HNDL_LINKED_DMA], SWOSD_DMA_CH_MAX);
}
