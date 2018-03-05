/** ==================================================================
 *  @file   system_m3video_edma.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/system/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* EDMA3 programming for DM81XX */

    /* EDMA3 Channel Controller Registers */

    /* Global Registers */
#define DM81XX_EDMA3_CC_BASE        (0x49000000)
#define DM81XX_EDMA3_DCHMAP0        (DM81XX_EDMA3_CC_BASE + 0x0100)
#define DM81XX_EDMA3_DMAQNUM0       (DM81XX_EDMA3_CC_BASE + 0x0240)
#define DM81XX_EDMA3_QUEPRI         (DM81XX_EDMA3_CC_BASE + 0x0284)
#define DM81XX_EDMA3_EMCR           (DM81XX_EDMA3_CC_BASE + 0x0308)
#define DM81XX_EDMA3_EMCRH          (DM81XX_EDMA3_CC_BASE + 0x030C)
#define DM81XX_EDMA3_QEMCR          (DM81XX_EDMA3_CC_BASE + 0x0314)
#define DM81XX_EDMA3_CCERRCLR       (DM81XX_EDMA3_CC_BASE + 0x031C)
#define DM81XX_EDMA3_QWMTHRA        (DM81XX_EDMA3_CC_BASE + 0x0620)

    /* Shadow region mapping registers */
#define DM81XX_EDMA3_DRAE0          (DM81XX_EDMA3_CC_BASE + 0x0340)
#define DM81XX_EDMA3_DRAE0H         (DM81XX_EDMA3_CC_BASE + 0x0344)
#define DM81XX_EDMA3_DRAE1          (DM81XX_EDMA3_CC_BASE + 0x0348)
#define DM81XX_EDMA3_DRAE1H         (DM81XX_EDMA3_CC_BASE + 0x034C)
#define DM81XX_EDMA3_DRAE2          (DM81XX_EDMA3_CC_BASE + 0x0350)
#define DM81XX_EDMA3_DRAE2H         (DM81XX_EDMA3_CC_BASE + 0x0354)
#define DM81XX_EDMA3_DRAE3          (DM81XX_EDMA3_CC_BASE + 0x0358)
#define DM81XX_EDMA3_DRAE3H         (DM81XX_EDMA3_CC_BASE + 0x035C)
#define DM81XX_EDMA3_DRAE4          (DM81XX_EDMA3_CC_BASE + 0x0360)
#define DM81XX_EDMA3_DRAE4H         (DM81XX_EDMA3_CC_BASE + 0x0364)
#define DM81XX_EDMA3_DRAE5          (DM81XX_EDMA3_CC_BASE + 0x0368)
#define DM81XX_EDMA3_DRAE5H         (DM81XX_EDMA3_CC_BASE + 0x036C)
#define DM81XX_EDMA3_DRAE6          (DM81XX_EDMA3_CC_BASE + 0x0370)
#define DM81XX_EDMA3_DRAE6H         (DM81XX_EDMA3_CC_BASE + 0x0374)
#define DM81XX_EDMA3_DRAE7          (DM81XX_EDMA3_CC_BASE + 0x0378)
#define DM81XX_EDMA3_DRAE7H         (DM81XX_EDMA3_CC_BASE + 0x037C)

    /* EDMA Channel Registers for Shadow region 4 */
#define DM81XX_EDMA3_ESR            (DM81XX_EDMA3_CC_BASE + 0x2810)
#define DM81XX_EDMA3_ESRH           (DM81XX_EDMA3_CC_BASE + 0x2814)
#define DM81XX_EDMA3_IESR           (DM81XX_EDMA3_CC_BASE + 0x2860)
#define DM81XX_EDMA3_IESRH          (DM81XX_EDMA3_CC_BASE + 0x2864)
#define DM81XX_EDMA3_IPR            (DM81XX_EDMA3_CC_BASE + 0x2868)
#define DM81XX_EDMA3_IPRH           (DM81XX_EDMA3_CC_BASE + 0x286C)
#define DM81XX_EDMA3_ICR            (DM81XX_EDMA3_CC_BASE + 0x2870)
#define DM81XX_EDMA3_ICRH           (DM81XX_EDMA3_CC_BASE + 0x2874)

    /* EDMA3 PaRAM entry base register */
#define DM81XX_EDMA3_PARAM_BASE     (DM81XX_EDMA3_CC_BASE + 0x4000)

#define OPT                         (0x00)
#define SRC                         (0x04)
#define A_B_CNT                     (0x08)
#define DST                         (0x0C)
#define SRC_DST_BIDX                (0x10)
#define LINK_BCNTRLD                (0x14)
#define SRC_DST_CIDX                (0x18)
#define CCNT                        (0x1C)

    /* EDMA3 set params */
/* ===================================================================
 *  @func     DM81XX_EDMA3_setParams                                               
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
void DM81XX_EDMA3_setParams(int chId, int dmaQueue, unsigned int srcAddr,
                            unsigned int dstAddr, unsigned short edmaWidth,
                            unsigned short edmaHeight, short srcLineOffset,
                            short dstLineOffset)
{
    volatile unsigned int PaRAMEntryAddr =
        DM81XX_EDMA3_PARAM_BASE + (chId * 0x20);
    volatile unsigned int dchmapChId = DM81XX_EDMA3_DCHMAP0 + (chId * 4);

    volatile unsigned int dmaQnum = DM81XX_EDMA3_DMAQNUM0 + ((chId / 8) * 4);

    /* PaRAM entry setup */
    *((volatile unsigned int *) (PaRAMEntryAddr + OPT)) = ((0) |    // SAM 
                                                           // -> 
                                                           // INCR 
                                                           // mode
                                                           (0 << 1) |   // DAM 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // INCR 
                                                           // mode
                                                           (1 << 2) |   // SYNCDIM 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // AB 
                                                           // synchronized
                                                           (0 << 3) |   // STATIC 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // non-static 
                                                           // mode
                                                           (4 << 8) |   // FWID 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // FIFO 
                                                           // width 
                                                           // is 
                                                           // 128 
                                                           // bits
                                                           (0 << 11) |  // TCCMODE 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // Normal 
                                                           // completion
                                                           (chId << 12) |   // TCC 
                                                                            // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // Transfer 
                                                           // Completion 
                                                           // Code
                                                           (1 << 20) |  // TCINTEN 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // Transfer 
                                                           // Complete 
                                                           // Interrupt 
                                                           // is 
                                                           // enabled
                                                           (0 << 21) |  // ITCINTEN 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // Intermediate 
                                                           // transfer 
                                                           // complete 
                                                           // interrupt 
                                                           // is 
                                                           // disabled
                                                           (0 << 22) |  // TCCHEN 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // Transfer 
                                                           // Complete 
                                                           // Chaining 
                                                           // is 
                                                           // disabled
                                                           (0 << 23) |  // ITCCHEN 
                                                                        // 
                                                           // 
                                                           // 
                                                           // -> 
                                                           // Intermediate 
                                                           // transfer 
                                                           // complete 
                                                           // chaining 
                                                           // is 
                                                           // disabled
                                                           (0 << 24));  // PRIVID 
                                                                        // 
    // 
    // 
    // -> 
    // Privilege 
    // identification 
    // for 
    // the 
    // external 
    // host/CPU/DMA 
    // that
    *((volatile unsigned int *) (PaRAMEntryAddr + SRC)) = srcAddr;
    *((volatile unsigned int *) (PaRAMEntryAddr + A_B_CNT)) =
        (edmaHeight << 16) | (edmaWidth);
    *((volatile unsigned int *) (PaRAMEntryAddr + DST)) = dstAddr;
    *((volatile unsigned int *) (PaRAMEntryAddr + SRC_DST_BIDX)) =
        (dstLineOffset << 16) | (srcLineOffset);
    *((volatile unsigned int *) (PaRAMEntryAddr + LINK_BCNTRLD)) =
        (0 << 16) | 0xFFFF;
    *((volatile unsigned int *) (PaRAMEntryAddr + SRC_DST_CIDX)) =
        (0 << 16) | 0;
    *((volatile unsigned int *) (PaRAMEntryAddr + CCNT)) = 1;
    if (chId < 32)

    {
        /* Set Shadow region for the channel */
        *((volatile unsigned int *) DM81XX_EDMA3_DRAE4) |= (1 << chId);

        /* Enable interrupt */
        *((volatile unsigned int *) DM81XX_EDMA3_IESR) |= (1 << chId);
    }

    else

    {
        /* Set Shadow region for the channel */
        *((volatile unsigned int *) DM81XX_EDMA3_DRAE4H) |= (1 << (chId - 32));

        /* Enable interrupt */
        *((volatile unsigned int *) DM81XX_EDMA3_IESRH) |= (1 << (chId - 32));
    }
    /* channel to PaRAM entry mapping */
    *((volatile unsigned int *) dchmapChId) = (chId << 5); // maintainig 
    // one to one 
    // mapping
    // between
    // channel
    // and PaRAM
    // entry

    /* channel to queue mapping */
    *((volatile unsigned int *) dmaQnum) |= (dmaQueue << ((chId % 8) * 4));
}

    /* EDMA3 trigger transfer */
/* ===================================================================
 *  @func     DM81XX_EDMA3_triggerTransfer                                               
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
void DM81XX_EDMA3_triggerTransfer(int chId)
{
    volatile unsigned int data;

    if (chId < 32)

    {

        /* clear the interrupt */
        *((volatile unsigned int *) DM81XX_EDMA3_ICR) |= (1 << chId);

        /* Set event */
        *((volatile unsigned int *) DM81XX_EDMA3_ESR) |= (1 << chId);

        /* Wait for the interrupt */
        do

        {
            data = *((volatile unsigned int *) DM81XX_EDMA3_IPR);
        } while ((data & (1 << chId)) == 0);

        /* clear the interrupt */
        *((volatile unsigned int *) DM81XX_EDMA3_ICR) |= (1 << chId);
    }

    else

    {

        /* clear the interrupt */
        *((volatile unsigned int *) DM81XX_EDMA3_ICRH) |= (1 << (chId - 32));

        /* Set event */
        *((volatile unsigned int *) DM81XX_EDMA3_ESRH) |= (1 << (chId - 32));

        /* Wait for the interrupt */
        do

        {
            data = *((volatile unsigned int *) DM81XX_EDMA3_IPRH);
        } while ((data & (1 << (chId - 32))) == 0);

        /* clear the interrupt */
        *((volatile unsigned int *) DM81XX_EDMA3_ICRH) |= (1 << (chId - 32));
	}
}

/* memset using EDMA transfer */

/* ===================================================================
 *  @func     DM81XX_EDMA3_clrShadowRgn0                                               
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
void DM81XX_EDMA3_clrShadowRgn0(int chId)
{
    if (chId < 32)
    {
        /* Clear the channel in shadow region 0 */
        *((volatile unsigned int *) DM81XX_EDMA3_DRAE0) &= ~(1 << chId);	
    }
    else
    {
        /* Clear the channel in shadow region 0 */
        *((volatile unsigned int *) DM81XX_EDMA3_DRAE0H) &= ~(1 << (chId - 32));	
    }
}
