/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <mcfw/src_bios6/utils/utils_tiler.h>
#include <mcfw/interfaces/link_api/system_tiler.h>
#include <mcfw/interfaces/link_api/system_debug.h>

UInt32 Utils_tilerAddr2CpuAddr(UInt32 tilerAddr)
{
    UInt32 cpuAddr, cntMode;

    cntMode = UTILS_TILER_GET_CNT_MODE(tilerAddr);

    cpuAddr =
        UTILS_TILER_CPU_VIRT_ADDR + UTILS_TILER_CONTAINER_MAXSIZE * cntMode;
    cpuAddr += (tilerAddr & 0x07FFFFFF);

#ifdef SYSTEM_DEBUG_TILER
    {
        Vps_printf(" [TILER] Tiler Addr = 0x%08x, CPU Addr = 0x%08x\n",
                   tilerAddr, cpuAddr);
    }
#endif

    return cpuAddr;
}

Int32 Utils_tilerGetMaxPitch(UInt32 cntMode, UInt32 * maxPitch)
{
    *maxPitch = 0;

    switch (cntMode)
    {
        case UTILS_TILER_CNT_8BIT:
            *maxPitch = VPSUTILS_TILER_CNT_8BIT_PITCH;
            break;
        case UTILS_TILER_CNT_16BIT:
            *maxPitch = VPSUTILS_TILER_CNT_16BIT_PITCH;
            break;
        case UTILS_TILER_CNT_32BIT:
            *maxPitch = VPSUTILS_TILER_CNT_32BIT_PITCH;
            break;
        default:
            return -1;
    }

    return 0;
}

UInt32 Utils_tilerGetOriAddr(UInt32 tilerAddr, UInt32 cntMode, UInt32 oriFlag, UInt32 width, UInt32 height)
{
	UInt32      oriAddr;
    UInt32      hOffset, vOffset;
    UInt32      hStride, vStride;

    /* Get the base address without orientation and container modes */
    oriAddr = tilerAddr;
    oriAddr &= ~(0x1Fu << 27u);
    /*oriFlag &= (UTILS_TILER_ORI_X_FLIP |
                UTILS_TILER_ORI_Y_FLIP |
                UTILS_TILER_ORI_XY_SWAP);
	*/
    /* Figure out horizontal stride and max lines as per container mode and
     * orientation flag */
    if (oriFlag & UTILS_TILER_ORI_XY_SWAP)
    {
        /* Figure out horizontal stride and max lines as per container mode */
        if (UTILS_TILER_CNT_8BIT == cntMode)
        {
            hStride = VPSUTILS_TILER_CNT_8BIT_SWAP_PITCH;
            vStride = UTILS_TILER_CNT_8BIT_SWAP_MAX_LINES;
        }
        else if (UTILS_TILER_CNT_16BIT == cntMode)
        {
            hStride = VPSUTILS_TILER_CNT_16BIT_SWAP_PITCH;
            vStride = UTILS_TILER_CNT_16BIT_SWAP_MAX_LINES;
        }
        else
        {
            hStride = VPSUTILS_TILER_CNT_32BIT_SWAP_PITCH;
            vStride = UTILS_TILER_CNT_32BIT_SWAP_MAX_LINES;
        }
    }
    else
    {
        if (UTILS_TILER_CNT_8BIT == cntMode)
        {
            hStride = VPSUTILS_TILER_CNT_8BIT_PITCH;
            vStride = UTILS_TILER_CNT_8BIT_MAX_LINES;
        }
        else if (UTILS_TILER_CNT_16BIT == cntMode)
        {
            hStride = VPSUTILS_TILER_CNT_16BIT_PITCH;
            vStride = UTILS_TILER_CNT_16BIT_MAX_LINES;
        }
        else
        {
            hStride = VPSUTILS_TILER_CNT_32BIT_PITCH;
            vStride = UTILS_TILER_CNT_32BIT_MAX_LINES;
        }
    }

    if (oriFlag & UTILS_TILER_ORI_XY_SWAP)
    {
    	if (UTILS_TILER_CNT_8BIT == cntMode)
    	{
			vOffset = oriAddr & (vStride - 1u);
        	hOffset = oriAddr / vStride;
			if (oriFlag == 4)
			{
				oriAddr = vOffset * hStride + hOffset;
			}
			else if (oriFlag == 5)
			{
				oriAddr = hStride * (vStride - vOffset - height) + hOffset;
			}
			else if (oriFlag == 6)
			{
				oriAddr = vOffset * hStride + hStride - hOffset - width;
    		}
    	}
		
        if (UTILS_TILER_CNT_16BIT == cntMode)
        {
        	vOffset = oriAddr & (vStride * 2 - 1u);
        	vOffset >>= 1u;
			hOffset = oriAddr / (vStride * 2);	
			if (oriFlag == 4)
			{
				oriAddr = vOffset * hStride + hOffset*2;
			}
			else if (oriFlag == 5)
			{
				oriAddr = hStride * (vStride - vOffset - height) + hOffset * 2;
			}
			else if (oriFlag == 6)
			{
				oriAddr = vOffset * hStride + hStride - hOffset * 2 - width;
			}
        }
				
#if 0
        /* Calculate vertical offset in terms of lines */
        vOffset = oriAddr & (vStride - 1u);
        /* Since the above gives the offsets in terms of bytes with respect to
         * 0 degree address, divide the offset by number of bytes in an
         * element - 2 for 16-bit and 4 for 32-bit container. */
        if (UTILS_TILER_CNT_16BIT == cntMode)
        {
            vOffset >>= 1u;
        }
        else if (UTILS_TILER_CNT_32BIT == cntMode)
        {
            vOffset >>= 2u;
        }

        /* Calculate horizontal line offset from start of line */
        hOffset = 2*(oriAddr / hStride);
        //UTILS_assert(hStride > (hOffset + height));

        /* Calculate the new address for X and Y axis swap */
        oriAddr = vOffset * hStride + hOffset;

        /* Calculate X' address */
        if (oriFlag & UTILS_TILER_ORI_X_FLIP)
        {
            /* Calculate new address from reverse X (X') axis */
            oriAddr = (vStride - (vOffset + width)) * hStride;
            oriAddr += hOffset;
        }

        /* Calculate Y' address */
        if (oriFlag & UTILS_TILER_ORI_Y_FLIP)
        {
            /* Clear line offset - Get the start of line address */
            oriAddr &= ~(hStride - 1u);
            UTILS_assert(hStride > (hOffset + height));

            /* Calculate new address from reverse Y (Y') axis */
            oriAddr += hStride - (hOffset + height);
        }
#endif		
    }
    else
    {
        /* Calculate horizontal line offset from start of line */
        hOffset = oriAddr & (hStride - 1u);

        if (oriFlag & UTILS_TILER_ORI_Y_FLIP)
        {
            /* Calculate vertical offset in terms of lines */
            vOffset = (oriAddr / hStride);
            UTILS_assert(vStride > (vOffset + height));

            /* Calculate new address from reverse Y (Y') axis */
            oriAddr = (vStride - (vOffset + height)) * hStride;
            oriAddr += hOffset;
        }
		
        if (oriFlag & UTILS_TILER_ORI_X_FLIP)
        {
            /* Clear line offset - Get the start of line address */
            oriAddr &= ~(hStride - 1u);
            UTILS_assert(hStride > (hOffset + width));

            /* Calculate new address from reverse X (X') axis */
            oriAddr += hStride - (hOffset + width);
     	}
     	
    }

    /* Set the orientation modes */
    oriAddr &= ~UTILS_TILER_ORI_MODE_MASK;
    oriAddr |= (oriFlag << UTILS_TILER_ORI_MODE_SHIFT);

    /* Set the container mode */
    oriAddr = UTILS_TILER_PUT_CNT_MODE(oriAddr, cntMode);

    return (oriAddr);
}

Int32 Utils_tilerFrameAlloc(FVID2_Format * pFormat,
                            FVID2_Frame * pFrame, UInt16 numFrames)
{
    UInt32 frameId;
	UInt32 height;
	
    /* align height to multiple of 2 */
    pFormat->height = VpsUtils_align(pFormat->height, 2);

    for (frameId = 0; frameId < numFrames; frameId++)
    {
        /* init FVID2_Frame to 0's */
        memset(pFrame, 0, sizeof(*pFrame));

        /* copy channelNum to FVID2_Frame from FVID2_Format */
        pFrame->channelNum = pFormat->channelNum;

        switch (pFormat->dataFormat)
        {
            case FVID2_DF_YUV422SP_UV:
            case FVID2_DF_YUV422I_VYUY:
			case FVID2_DF_YUV422I_UYVY:
                /* Y plane */
              
                pFrame->addr[0][0] =
                    (Ptr) SystemTiler_alloc(SYSTEM_TILER_CNT_8BIT,
                                            pFormat->width, pFormat->height);
                pFrame->addr[1][0] = pFrame->addr[0][0];
                UTILS_assert((UInt32)(pFrame->addr[0][0]) != SYSTEM_TILER_INVALID_ADDR);
                /* C plane */
                
                pFrame->addr[0][1] =
                    (Ptr) SystemTiler_alloc(SYSTEM_TILER_CNT_16BIT,
                                            pFormat->width, pFormat->height);
                pFrame->addr[1][1] = pFrame->addr[0][1];
                UTILS_assert((UInt32)(pFrame->addr[0][1]) != SYSTEM_TILER_INVALID_ADDR);
                break;
            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:

				/* 1024 = maxmum frame height with 96MB TILER */
				if ((pFormat->width/2) > 1024)
				{
					height = pFormat->height;
				}
				else
				{
					height = pFormat->width;
				}
				
                /* Y plane */				
                pFrame->addr[0][0] =
                    (Ptr) SystemTiler_alloc(SYSTEM_TILER_CNT_8BIT,
                                            pFormat->width, height/*pFormat->height*/);
				pFrame->addr[1][0] = pFrame->addr[0][0];                                            
                UTILS_assert((UInt32)(pFrame->addr[0][0]) != SYSTEM_TILER_INVALID_ADDR);
                /* C plane */
             
                pFrame->addr[0][1] =
                    (Ptr) SystemTiler_alloc(SYSTEM_TILER_CNT_16BIT,
                                            pFormat->width,
                                            height/2/*pFormat->height / 2*/);
                pFrame->addr[1][1] = pFrame->addr[0][1];                                            
                UTILS_assert((UInt32)(pFrame->addr[0][1]) != SYSTEM_TILER_INVALID_ADDR);
                break;
            default:
                UTILS_assert(0);
                break;

        }

        pFrame++;
    }

    return 0;
}

Int32 Utils_tilerCopy(UInt32 dir, UInt32 tilerAddr, UInt32 dataWidth,
                      UInt32 dataHeight, UInt8 * ddrAddr, UInt32 ddrPitch)
{
    UInt32 cntMode;
    UInt32 tilerCpuAddr, tilerPitch;
    UInt32 inc;
    UInt32 dstAddrBase, dstAddr, dstPitch;
    UInt32 srcAddrBase, srcAddr, srcPitch;
    UInt32 h, w;

    cntMode = UTILS_TILER_GET_CNT_MODE(tilerAddr);
    tilerCpuAddr = Utils_tilerAddr2CpuAddr(tilerAddr);

    Utils_tilerGetMaxPitch(cntMode, &tilerPitch);

    inc = 1 << cntMode;

    if (dir == UTILS_TILER_COPY_TO_DDR)
    {
        dstAddrBase = (UInt32) ddrAddr;
        dstPitch = ddrPitch;
        srcAddrBase = tilerCpuAddr;
        srcPitch = tilerPitch;
    }
    else
    {
        srcAddrBase = (UInt32) ddrAddr;
        srcPitch = ddrPitch;
        dstAddrBase = tilerCpuAddr;
        dstPitch = tilerPitch;
    }

    for (h = 0; h < dataHeight; h++)
    {
        dstAddr = dstAddrBase;
        srcAddr = srcAddrBase;

        switch (cntMode)
        {
            case UTILS_TILER_CNT_8BIT:
                for (w = 0; w < dataWidth;
                     w += inc, dstAddr += inc, srcAddr += inc)
                    *(volatile UInt8 *) dstAddr = *(volatile UInt8 *) srcAddr;
                break;
            case UTILS_TILER_CNT_16BIT:
                for (w = 0; w < dataWidth;
                     w += inc, dstAddr += inc, srcAddr += inc)
                    *(volatile UInt16 *) dstAddr = *(volatile UInt16 *) srcAddr;
                break;
            case UTILS_TILER_CNT_32BIT:
                for (w = 0; w < dataWidth;
                     w += inc, dstAddr += inc, srcAddr += inc)
                    *(volatile UInt32 *) dstAddr = *(volatile UInt32 *) srcAddr;
                break;
        }

        dstAddrBase += dstPitch;
        srcAddrBase += srcPitch;
    }

    return 0;
}
