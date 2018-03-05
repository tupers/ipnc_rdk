/** ==================================================================
 *  @file   ti_draw.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/common/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
 
 #include <mcfw/src_bios6/links_m3vpss/alg/common/ti_draw.h>
 
SystemCommon_edmaChPrm gDrawEdmaHndl;
 
 /* Draw Line */

/* ===================================================================
 *  @func     DM812X_DRAW_line
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
Int32 DM812X_DRAW_line(DRAW_InFrame_Info_t * pInFrameInfo,
                       DRAW_Line_Info_t * pLineInfo)
{
    UInt8 *startAddr;

    UInt32 hlen, vlen, incAddr;

    UInt32 color;

    if (pLineInfo->vertLine)
    {
        hlen = pLineInfo->thickness;
        vlen = pLineInfo->length;
    }
    else
    {
        hlen = pLineInfo->length;
        vlen = pLineInfo->thickness;
    }

    if (pLineInfo->startY >= (pInFrameInfo->videoHeight))
    {
        return FVID2_SOK;
    }

    if (pLineInfo->startX >= (pInFrameInfo->videoWidth))
    {
        return FVID2_SOK;
    }

    pLineInfo->startX = DRAW_floor(pLineInfo->startX, 4);

    if (pLineInfo->startX + hlen > (pInFrameInfo->videoWidth))
    {
        hlen = pInFrameInfo->videoWidth - pLineInfo->startX;
    }

    if (pLineInfo->startY + vlen > (pInFrameInfo->videoHeight))
    {
        vlen = pInFrameInfo->videoHeight - pLineInfo->startY;
    }

    if ((pInFrameInfo->videoDataFormat == FVID2_DF_YUV420SP_VU) ||
        (pInFrameInfo->videoDataFormat == FVID2_DF_YUV420SP_UV))
    {
        incAddr = pInFrameInfo->videoOffsetH;
        startAddr =
            pInFrameInfo->videoInOutAddrY + pLineInfo->startY * incAddr +
            pLineInfo->startX;
        color =
            (pLineInfo->colorY << 24) + (pLineInfo->colorY << 16) +
            (pLineInfo->colorY << 8) + pLineInfo->colorY;
    }
    else
    {
        incAddr = pInFrameInfo->videoOffsetH * 2;
        startAddr =
            pInFrameInfo->videoInOutAddrY + pLineInfo->startY * incAddr +
            pLineInfo->startX * 2;
        hlen *= 2;
        color =
            (pLineInfo->colorY << 24) + (pLineInfo->colorC << 16) +
            (pLineInfo->colorY << 8) + pLineInfo->colorC;
    }

    hlen = DRAW_floor(hlen, 4);

    DM81XX_EDMA3_memset(gDrawEdmaHndl.dmaHndl.chId,                 // chId
                        DRAW_EDMA_QUEUEID_DRAW,              		// dmaQueue
                        color,                             			// setValue
                        (UInt32) startAddr,                			// dstAddr
                        hlen,                              			// setWidth
                        vlen,                              			// setHeight
                        incAddr);                          			// lineOffset

    return FVID2_SOK;
}

/* Draw rectangle */

/* ===================================================================
 *  @func     DM812X_DRAW_rectangle
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
Int32 DM812X_DRAW_rectangle(DRAW_InFrame_Info_t * pInFrameInfo,
                            DRAW_Rect_Info_t * pRectInfo, Int32 pitch)
{
    UInt32 maskDstAddr;

    DRAW_Line_Info_t lineInfo;

    pRectInfo->startX = DRAW_floor(pRectInfo->startX, 4);
    pRectInfo->startY = DRAW_floor(pRectInfo->startY, 2);

    pRectInfo->width = DRAW_align(pRectInfo->width, 4);
    pRectInfo->height = DRAW_align(pRectInfo->height, 2);
    pRectInfo->thickness = DRAW_align(pRectInfo->thickness, 4);

    if (!pInFrameInfo->maskEnable)
    {
        lineInfo.thickness = pRectInfo->thickness;
        lineInfo.colorY = pRectInfo->colorY;
        lineInfo.colorC = pRectInfo->colorC;

        lineInfo.startX = pRectInfo->startX;
        lineInfo.startY = pRectInfo->startY;
        lineInfo.length = pRectInfo->width;
        lineInfo.vertLine = FALSE;

        DM812X_DRAW_line(pInFrameInfo, &lineInfo);

        lineInfo.startX = pRectInfo->startX;
        lineInfo.startY = pRectInfo->startY;
        lineInfo.length = pRectInfo->height;
        lineInfo.vertLine = TRUE;

        DM812X_DRAW_line(pInFrameInfo, &lineInfo);

        lineInfo.startX =
            (pRectInfo->startX + pRectInfo->width - pRectInfo->thickness);
        lineInfo.startY = pRectInfo->startY;
        lineInfo.length = pRectInfo->height;
        lineInfo.vertLine = TRUE;

        DM812X_DRAW_line(pInFrameInfo, &lineInfo);

        lineInfo.startX = pRectInfo->startX;
        lineInfo.startY =
            (pRectInfo->startY + pRectInfo->height - pRectInfo->thickness);
        lineInfo.length = pRectInfo->width;
        lineInfo.vertLine = FALSE;

        DM812X_DRAW_line(pInFrameInfo, &lineInfo);
    }
    else
    {
        /* Mask the face */
        maskDstAddr =
            (UInt32) (pInFrameInfo->videoInOutAddrY +
                      (pRectInfo->startX + (pRectInfo->startY * pitch)));
        DM81XX_EDMA3_memset(gDrawEdmaHndl.dmaHndl.chId,             // chId
                            DRAW_EDMA_QUEUEID_DRAW,          		// dmaQueue
                            0x00000000,                    			// setValue
                            maskDstAddr,                   			// dstAddr
                            pRectInfo->width,              			// setWidth
                            pRectInfo->height,             			// setHeight
                            pitch);                        			// lineOffset
    }

    return FVID2_SOK;
}
