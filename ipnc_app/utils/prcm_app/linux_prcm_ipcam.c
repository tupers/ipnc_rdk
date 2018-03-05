/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Project		: Centaurus
File		: Centaurus2_EVM_DDR2_DDR3_V3.gel
Owner		: raj@ti.com
Purpose		: Centaurus System initilization function
Desciption	: Contains intilization routines for DDR2, IVAHD0, UART, TIMER, DUCATI
Version	Hystory
---------------
Version		Date			Who		 		Description
-------		-----------		------		 	-----------
Ver1.0   	23-March-2011	Rajesh		 	Consolidated init sequence for DDR,IVA,DSP,DSS,ISS etc
Ver2.0   	31-March-2011	Manoj 		 	Change the DDR PLL at 333 MHz, Add Timer initialization code, SPinLock, Mailbox and update AMMU setting.
Ver3.0   	1-APril-2011	Manoj 		 	Change the DDR3 configuration setting for EVM.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#define PRINT									printf
#define DISP_ADDRPHY_ADDRVIRT_DATA(x, y, z) 	printf ("             " "Phy Addr : 0x%.8x Data : 0x%.8x\n", x,  z);
#define DISP_ADDRPHY_ADDRVIRT_DATA_BW(x, z) 	printf ("             " "BW Phy Addr : 0x%.8x Data : 0x%.8x\n", x,  z);
#define DISP_ADDRPHY_ADDRVIRT_DATA_AW(x, z) 	printf ("             " "AW Phy Addr : 0x%.8x Data : 0x%.8x\n", x,  z);

#define MAP_SIZE 		(1024*1024)
#define MAP_MASK 		(MAP_SIZE-1)


/***************************************************************************************************************
			            CENATAURUS2  REGISTER MAP DEFINES
***************************************************************************************************************/
#define PRCM_BASE_ADDR			0x48180000
#define DMM_BASE_ADDR			0x4E000000
#define EMIF4_0_CFG_BASE_ADDR	0x4C000000
#define EMIF4_1_CFG_BASE_ADDR	0x4D000000
#define GPMC_MEM_BASE_ADDR		0x02000000
#define DDR_MEM_BASE_ADDR		0x80000000
#define DUCATI_BASE_ADDR	    0x55020000
#define DUCATI_MMU_CFG			0x55080000
#define CTRL_MODULE_BASE_ADDR	0x48140000
#define GEM_L2RAM_BASE_ADDR		0x40800000

/* Control Module*/
#define	CM_CTRL_CLKCTRL				0x481815c4
#define DSPBOOTADDR					(CTRL_MODULE_BASE_ADDR + 0x0048)
#define DSPMEM_SLEEP				(CTRL_MODULE_BASE_ADDR + 0x0650)
#define CM_ALWON_L3_SLOW_CLKSTCTRL	(PRCM_BASE_ADDR + 0x1400)
#define CM_ALWON_GPIO_1_CLKCTRL 	(PRCM_BASE_ADDR + 0x1560)

/*UART*/
#define CM_ALWON_UART_0_CLKCTRL 	(PRCM_BASE_ADDR + 0x1550)
#define CM_ALWON_UART_1_CLKCTRL 	(PRCM_BASE_ADDR + 0x1554)
#define CM_ALWON_UART_2_CLKCTRL 	(PRCM_BASE_ADDR + 0x1558)
#define CM_DEFAULT_DUCATI_CLKSTCTRL	(PRCM_BASE_ADDR + 0x0518)
#define CM_DEFAULT_DUCATI_CLKCTRL	(PRCM_BASE_ADDR + 0x0574)
#define RM_DEFAULT_RSTCTRL			(PRCM_BASE_ADDR + 0x0B10)
#define RM_DEFAULT_RSTST			(PRCM_BASE_ADDR + 0x0B14)
#define CM_ALWON_OCMC_0_CLKSTCTRL	(PRCM_BASE_ADDR + 0x1414)
#define CM_ALWON_OCMC_0_CLKCTRL		(PRCM_BASE_ADDR + 0x15B4)
#define CM_ALWON_GPMC_CLKCTRL		(PRCM_BASE_ADDR + 0x15D0)

/*IVAHD0 PRCM Definition*/
#define CM_IVAHD0_CLKSTCTRL			(PRCM_BASE_ADDR + 0x0600)
#define CM_IVAHD0_IVAHD_CLKCTRL		(PRCM_BASE_ADDR + 0x0620)
#define CM_IVAHD0_SL2_CLKCTRL		(PRCM_BASE_ADDR + 0x0624)
#define PM_IVAHD0_PWRSTST			(PRCM_BASE_ADDR + 0x0C04)
#define RM_IVAHD0_RSTCTRL			(PRCM_BASE_ADDR + 0x0C10)
#define RM_IVAHD0_RSTST		        (PRCM_BASE_ADDR + 0x0C14)
/*END OF IVAHD0 PRCM Definition*/

/*ISS PRCM Definition*/
#define CM_ISS_CLKSTCTRL	    	(PRCM_BASE_ADDR + 0x0700)
#define CM_ISS_IVAHD_CLKCTRL     	(PRCM_BASE_ADDR + 0x0720)
#define CM_ISS_SL2_CLKCTRL       	(PRCM_BASE_ADDR + 0x0724)
#define RM_ISS_RSTCTRL				(PRCM_BASE_ADDR + 0x0D10)
#define RM_ISS_RSTST		        (PRCM_BASE_ADDR + 0x0D14)
/*END OF ISS PRCM Definition*/

/*GEMSS PRCEM definition*/
#define CM_GEM_CLKSTCTRL			(PRCM_BASE_ADDR + 0x0400)
#define CM_ACTIVE_GEM_CLKCTRL		(PRCM_BASE_ADDR + 0x0420)
#define RM_ACTIVE_RSTCTRL			(PRCM_BASE_ADDR + 0x0A10)
#define RM_ACTIVE_RSTST				(PRCM_BASE_ADDR + 0x0A14)

#define CM_MMU_CLKSTCTRL			(PRCM_BASE_ADDR + 0x140C)
#define CM_ALWON_MMUDATA_CLKCTRL	(PRCM_BASE_ADDR + 0x159C)

#define CM_MMUCFG_CLKSTCTRL 		(PRCM_BASE_ADDR + 0x1410)
#define CM_ALWON_MMUCFG_CLKCTRL  	(PRCM_BASE_ADDR + 0x15A8)
/*END OF GEMSS PRCM Definition*/


//TPPSS
#define CM_DEFAULT_TPPSS_CLKSTCTRL	(PRCM_BASE_ADDR + 0x50C)
#define CM_DEFAULT_TPPSS_CLKCTRL	(PRCM_BASE_ADDR + 0x554)
#define TPPIRAM_BASE_ADDR			0x57180000

//Control Module Always on ClockControl
#define CM_ALWON_L3_SLOW_CLKSTCTRL		(PRCM_BASE_ADDR + 0x1400)
#define CM_ALWON_TIMER_0_CLKCTRL     	(PRCM_BASE_ADDR + 0x156C)
#define CM_ALWON_TIMER_1_CLKCTRL     	(PRCM_BASE_ADDR + 0x1570)
#define CM_ALWON_TIMER_2_CLKCTRL     	(PRCM_BASE_ADDR + 0x1574)
#define CM_ALWON_TIMER_3_CLKCTRL     	(PRCM_BASE_ADDR + 0x1578)
#define CM_ALWON_TIMER_4_CLKCTRL     	(PRCM_BASE_ADDR + 0x157C)
#define CM_ALWON_TIMER_5_CLKCTRL     	(PRCM_BASE_ADDR + 0x1580)
#define CM_ALWON_TIMER_6_CLKCTRL     	(PRCM_BASE_ADDR + 0x1584)
#define CM_ALWON_TIMER_7_CLKCTRL     	(PRCM_BASE_ADDR + 0x1588)
#define CM_ALWON_SYSCLK5_CLKSTCTRL		(PRCM_BASE_ADDR + 0x1424)
#define CM_ALWON_DCAN_0_1_CLKCTRL		(PRCM_BASE_ADDR + 0x1618)

#define CM_TIMER0_CLKSEL				(PRCM_BASE_ADDR + 0x038C)
#define CM_TIMER1_CLKSEL				(PRCM_BASE_ADDR + 0x0390)
#define CM_TIMER2_CLKSEL				(PRCM_BASE_ADDR + 0x0394)
#define CM_TIMER3_CLKSEL				(PRCM_BASE_ADDR + 0x0398)
#define CM_TIMER4_CLKSEL				(PRCM_BASE_ADDR + 0x039C)
#define CM_TIMER5_CLKSEL				(PRCM_BASE_ADDR + 0x03A0)
#define CM_TIMER6_CLKSEL				(PRCM_BASE_ADDR + 0x03A4)
#define CM_TIMER7_CLKSEL				(PRCM_BASE_ADDR + 0x03A8)

#define CM_ALWON_MAILBOX_CLKCTRL        (PRCM_BASE_ADDR + 0x1594)

#define DATA_SIZE 						10
#define OCMC0RAM_BASE_ADDR 				0x40300000
#define OCMC1RAM_BASE_ADDR 				0x40400000

#define DMTIMER0_BASE_ADDR 				0x4802C000
#define DMTIMER1_BASE_ADDR 				0x4802E000
#define DMTIMER2_BASE_ADDR 				0x48040000
#define DMTIMER3_BASE_ADDR 				0x48042000
#define DMTIMER4_BASE_ADDR 				0x48044000
#define DMTIMER5_BASE_ADDR 				0x48046000
#define DMTIMER6_BASE_ADDR 				0x48048000
#define DMTIMER7_BASE_ADDR 				0x4804A000

/*****************************************************************************/
/*EMIF4 PRCM Defintion*/
#define CM_DEFAULT_L3_FAST_CLKSTCTRL	(PRCM_BASE_ADDR + 0x0508)
#define CM_DEFAULT_EMIF_0_CLKCTRL		(PRCM_BASE_ADDR + 0x0520)
#define CM_DEFAULT_EMIF_1_CLKCTRL    	(PRCM_BASE_ADDR + 0x0524)
#define CM_DEFAULT_DMM_CLKCTRL 			(PRCM_BASE_ADDR + 0x0528)
#define CM_DEFAULT_FW_CLKCTRL 			(PRCM_BASE_ADDR + 0x052C)
/*end of EMIF4 PRCM definition*/
#define CM_ALWON_L4_SLOW_CLKSTCTRL  	(PRCM_BASE_ADDR + 0x1400)
#define CM_ALWON_SPINBOX_CLKCTRL    	(PRCM_BASE_ADDR + 0x1598)

#define	   DDR0_PHY_BASE_ADDR			0x47C0C400
#define	   DDR1_PHY_BASE_ADDR			0x47C0C800
#define	   DDR0_IO_CTRL	       			0x48140E04
#define	   DDR1_IO_CTRL	       			0x48140E08
#define	   VTP0_CTRL_REG       			0x48140E0C
#define	   VTP1_CTRL_REG       			0x48140E10
#define	   EMIF4_0_CFG_BASE_ADDR		0x4C000000
#define    EMIF4_1_CFG_BASE_ADDR		0x4D000000
#define	   DMM_BASE_ADDR				0x4E000000

//-DMM & EMIF4 MMR Declaration
#define DMM_LISA_MAP__0					(DMM_BASE_ADDR + 0x40)
#define DMM_LISA_MAP__1					(DMM_BASE_ADDR + 0x44)
#define DMM_LISA_MAP__2					(DMM_BASE_ADDR + 0x48)
#define DMM_LISA_MAP__3					(DMM_BASE_ADDR + 0x4C)
#define DMM_PAT_BASE_ADDR 				(DMM_BASE_ADDR + 0x460)

#define EMIF4_0_SDRAM_CONFIG			(EMIF4_0_CFG_BASE_ADDR + 0x08)
#define EMIF4_0_SDRAM_CONFIG2			(EMIF4_0_CFG_BASE_ADDR + 0x0C)
#define EMIF4_0_SDRAM_REF_CTRL			(EMIF4_0_CFG_BASE_ADDR + 0x10)
#define EMIF4_0_SDRAM_REF_CTRL_SHADOW	(EMIF4_0_CFG_BASE_ADDR + 0x14)
#define EMIF4_0_SDRAM_TIM_1				(EMIF4_0_CFG_BASE_ADDR + 0x18)
#define EMIF4_0_SDRAM_TIM_1_SHADOW		(EMIF4_0_CFG_BASE_ADDR + 0x1C)
#define EMIF4_0_SDRAM_TIM_2				(EMIF4_0_CFG_BASE_ADDR + 0x20)
#define EMIF4_0_SDRAM_TIM_2_SHADOW		(EMIF4_0_CFG_BASE_ADDR + 0x24)
#define EMIF4_0_SDRAM_TIM_3				(EMIF4_0_CFG_BASE_ADDR + 0x28)
#define EMIF4_0_SDRAM_TIM_3_SHADOW		(EMIF4_0_CFG_BASE_ADDR + 0x2C)
#define EMIF4_0_DDR_PHY_CTRL_1			(EMIF4_0_CFG_BASE_ADDR + 0xE4)
#define EMIF4_0_DDR_PHY_CTRL_1_SHADOW	(EMIF4_0_CFG_BASE_ADDR + 0xE8)
#define EMIF4_0_RDWR_LVL_RMP_CTRL		(EMIF4_0_CFG_BASE_ADDR + 0xD8)
#define EMIF4_0_RDWR_LVL_CTRL			(EMIF4_0_CFG_BASE_ADDR + 0xDC)


#define EMIF4_1_SDRAM_CONFIG			(EMIF4_1_CFG_BASE_ADDR + 0x08)
#define EMIF4_1_SDRAM_CONFIG2			(EMIF4_1_CFG_BASE_ADDR + 0x0C)
#define EMIF4_1_SDRAM_REF_CTRL			(EMIF4_1_CFG_BASE_ADDR + 0x10)
#define EMIF4_1_SDRAM_REF_CTRL_SHADOW	(EMIF4_1_CFG_BASE_ADDR + 0x14)
#define EMIF4_1_SDRAM_TIM_1				(EMIF4_1_CFG_BASE_ADDR + 0x18)
#define EMIF4_1_SDRAM_TIM_1_SHADOW		(EMIF4_1_CFG_BASE_ADDR + 0x1C)
#define EMIF4_1_SDRAM_TIM_2				(EMIF4_1_CFG_BASE_ADDR + 0x20)
#define EMIF4_1_SDRAM_TIM_2_SHADOW		(EMIF4_1_CFG_BASE_ADDR + 0x24)
#define EMIF4_1_SDRAM_TIM_3				(EMIF4_1_CFG_BASE_ADDR + 0x28)
#define EMIF4_1_SDRAM_TIM_3_SHADOW		(EMIF4_1_CFG_BASE_ADDR + 0x2C)
#define EMIF4_1_DDR_PHY_CTRL_1			(EMIF4_1_CFG_BASE_ADDR + 0xE4)
#define EMIF4_1_DDR_PHY_CTRL_1_SHADOW	(EMIF4_1_CFG_BASE_ADDR + 0xE8)
#define EMIF4_1_RDWR_LVL_RMP_CTRL		(EMIF4_1_CFG_BASE_ADDR + 0xD8)
#define EMIF4_1_RDWR_LVL_CTRL			(EMIF4_1_CFG_BASE_ADDR + 0xDC)


//- DDR0 Phy MMRs
#define	   CMD0_REG_PHY0_CTRL_SLAVE_RATIO_0	(0x01C + DDR0_PHY_BASE_ADDR)
#define	   CMD0_REG_PHY0_DLL_LOCK_DIFF_0 	(0x028 + DDR0_PHY_BASE_ADDR)
#define	   CMD0_REG_PHY0_INVERT_CLKOUT_0 	(0x02C + DDR0_PHY_BASE_ADDR)
#define	   CMD1_REG_PHY0_CTRL_SLAVE_RATIO_0	(0x050 + DDR0_PHY_BASE_ADDR)
#define	   CMD1_REG_PHY0_DLL_LOCK_DIFF_0	(0x05C + DDR0_PHY_BASE_ADDR)
#define	   CMD1_REG_PHY0_INVERT_CLKOUT_0	(0x060 + DDR0_PHY_BASE_ADDR)
#define	   CMD2_REG_PHY0_CTRL_SLAVE_RATIO_0	(0x084 + DDR0_PHY_BASE_ADDR)
#define	   CMD2_REG_PHY0_DLL_LOCK_DIFF_0	(0x090 + DDR0_PHY_BASE_ADDR)
#define	   CMD2_REG_PHY0_INVERT_CLKOUT_0	(0x094 + DDR0_PHY_BASE_ADDR)

#define	   DATA0_REG_PHY0_RD_DQS_SLAVE_RATIO_0	(0x0C8 + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_WR_DQS_SLAVE_RATIO_0	(0x0DC + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_WRLVL_INIT_RATIO_0	(0x0F0 + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_WRLVL_INIT_MODE_0 	(0x0F8 + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_GATELVL_INIT_RATIO_0 	(0x0FC + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_GATELVL_INIT_MODE_0 	(0x104 + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_FIFO_WE_SLAVE_RATIO_0 (0x108 + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_WR_DATA_SLAVE_RATIO_0 (0x120 + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_USE_RANK0_DELAYS 		(0x134 + DDR0_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY0_DLL_LOCK_DIFF_0 		(0x138 + DDR0_PHY_BASE_ADDR)

#define	   DATA1_REG_PHY0_RD_DQS_SLAVE_RATIO_0 	(0x16C + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_WR_DQS_SLAVE_RATIO_0 	(0x180 + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_WRLVL_INIT_RATIO_0 	(0x194 + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_WRLVL_INIT_MODE_0 	(0x19C + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_GATELVL_INIT_RATIO_0 	(0x1A0 + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_GATELVL_INIT_MODE_0 	(0x1A8 + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_FIFO_WE_SLAVE_RATIO_0 (0x1AC + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_WR_DATA_SLAVE_RATIO_0 (0x1C4 + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_USE_RANK0_DELAYS 		(0x1D8 + DDR0_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY0_DLL_LOCK_DIFF_0 		(0x1DC + DDR0_PHY_BASE_ADDR)

#define	   DATA2_REG_PHY0_RD_DQS_SLAVE_RATIO_0 	(0x210 + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_WR_DQS_SLAVE_RATIO_0 	(0x224 + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_WRLVL_INIT_RATIO_0 	(0x238 + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_WRLVL_INIT_MODE_0 	(0x240 + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_GATELVL_INIT_RATIO_0 	(0x244 + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_GATELVL_INIT_MODE_0 	(0x24C + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_FIFO_WE_SLAVE_RATIO_0 (0x250 + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_WR_DATA_SLAVE_RATIO_0 (0x268 + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_USE_RANK0_DELAYS 		(0x27C + DDR0_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY0_DLL_LOCK_DIFF_0 		(0x280 + DDR0_PHY_BASE_ADDR)

#define	   DATA3_REG_PHY0_RD_DQS_SLAVE_RATIO_0 	(0x2B4 + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_WR_DQS_SLAVE_RATIO_0 	(0x2C8 + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_WRLVL_INIT_RATIO_0 	(0x2DC + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_WRLVL_INIT_MODE_0 	(0x2E4 + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_GATELVL_INIT_RATIO_0 	(0x2E8 + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_GATELVL_INIT_MODE_0 	(0x2F0 + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_FIFO_WE_SLAVE_RATIO_0 (0x2F4 + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_WR_DATA_SLAVE_RATIO_0 (0x30C + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_USE_RANK0_DELAYS 		(0x320 + DDR0_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY0_DLL_LOCK_DIFF_0 		(0x324 + DDR0_PHY_BASE_ADDR)


//- DDR1 Phy MMRs
#define	   CMD0_REG_PHY1_CTRL_SLAVE_RATIO_0		(0x01C + DDR1_PHY_BASE_ADDR)
#define	   CMD0_REG_PHY1_DLL_LOCK_DIFF_0 		(0x028 + DDR1_PHY_BASE_ADDR)
#define	   CMD0_REG_PHY1_INVERT_CLKOUT_0 		(0x02C + DDR1_PHY_BASE_ADDR)
#define	   CMD1_REG_PHY1_CTRL_SLAVE_RATIO_0		(0x050 + DDR1_PHY_BASE_ADDR)
#define	   CMD1_REG_PHY1_DLL_LOCK_DIFF_0		(0x05C + DDR1_PHY_BASE_ADDR)
#define	   CMD1_REG_PHY1_INVERT_CLKOUT_0		(0x060 + DDR1_PHY_BASE_ADDR)
#define	   CMD2_REG_PHY1_CTRL_SLAVE_RATIO_0		(0x084 + DDR1_PHY_BASE_ADDR)
#define	   CMD2_REG_PHY1_DLL_LOCK_DIFF_0		(0x090 + DDR1_PHY_BASE_ADDR)
#define	   CMD2_REG_PHY1_INVERT_CLKOUT_0		(0x094 + DDR1_PHY_BASE_ADDR)

#define	   DATA0_REG_PHY1_RD_DQS_SLAVE_RATIO_0	(0x0C8 + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_WR_DQS_SLAVE_RATIO_0	(0x0DC + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_WRLVL_INIT_RATIO_0	(0x0F0 + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_WRLVL_INIT_MODE_0 	(0x0F8 + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_GATELVL_INIT_RATIO_0 	(0x0FC + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_GATELVL_INIT_MODE_0 	(0x104 + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_FIFO_WE_SLAVE_RATIO_0 (0x108 + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_WR_DATA_SLAVE_RATIO_0 (0x120 + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_USE_RANK0_DELAYS 		(0x134 + DDR1_PHY_BASE_ADDR)
#define	   DATA0_REG_PHY1_DLL_LOCK_DIFF_0 		(0x138 + DDR1_PHY_BASE_ADDR)

#define	   DATA1_REG_PHY1_RD_DQS_SLAVE_RATIO_0 	(0x16C + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_WR_DQS_SLAVE_RATIO_0 	(0x180 + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_WRLVL_INIT_RATIO_0 	(0x194 + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_WRLVL_INIT_MODE_0 	(0x19C + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_GATELVL_INIT_RATIO_0 	(0x1A0 + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_GATELVL_INIT_MODE_0 	(0x1A8 + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_FIFO_WE_SLAVE_RATIO_0 (0x1AC + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_WR_DATA_SLAVE_RATIO_0 (0x1C4 + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_USE_RANK0_DELAYS 		(0x1D8 + DDR1_PHY_BASE_ADDR)
#define	   DATA1_REG_PHY1_DLL_LOCK_DIFF_0 		(0x1DC + DDR1_PHY_BASE_ADDR)

#define	   DATA2_REG_PHY1_RD_DQS_SLAVE_RATIO_0 	(0x210 + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_WR_DQS_SLAVE_RATIO_0 	(0x224 + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_WRLVL_INIT_RATIO_0 	(0x238 + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_WRLVL_INIT_MODE_0 	(0x240 + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_GATELVL_INIT_RATIO_0 	(0x244 + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_GATELVL_INIT_MODE_0 	(0x24C + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_FIFO_WE_SLAVE_RATIO_0 (0x250 + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_WR_DATA_SLAVE_RATIO_0 (0x268 + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_USE_RANK0_DELAYS 		(0x27C + DDR1_PHY_BASE_ADDR)
#define	   DATA2_REG_PHY1_DLL_LOCK_DIFF_0 		(0x280 + DDR1_PHY_BASE_ADDR)

#define	   DATA3_REG_PHY1_RD_DQS_SLAVE_RATIO_0 	(0x2B4 + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_WR_DQS_SLAVE_RATIO_0 	(0x2C8 + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_WRLVL_INIT_RATIO_0 	(0x2DC + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_WRLVL_INIT_MODE_0 	(0x2E4 + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_GATELVL_INIT_RATIO_0 	(0x2E8 + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_GATELVL_INIT_MODE_0 	(0x2F0 + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_FIFO_WE_SLAVE_RATIO_0 (0x2F4 + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_WR_DATA_SLAVE_RATIO_0 (0x30C + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_USE_RANK0_DELAYS 		(0x320 + DDR1_PHY_BASE_ADDR)
#define	   DATA3_REG_PHY1_DLL_LOCK_DIFF_0 		(0x324 + DDR1_PHY_BASE_ADDR)

#define    DATA_MACRO_0             0
#define    DATA_MACRO_1             1
#define    DATA_MACRO_2             2
#define    DATA_MACRO_3             3
#define    DDR_PHY0		  			0
#define    DDR_PHY1		  			1

#define    DDR_FREQ		 			666 // 333MHz

//- Common DDR PHY parameters
#define	   PHY_INVERT_CLKOUT_DEFINE		 		0
#define	   DDR3_PHY_INVERT_CLKOUT_DEFINE	 	0
#define	   PHY_REG_USE_RANK0_DELAY_DEFINE	 	0
#define	   mDDR_PHY_REG_USE_RANK0_DELAY_DEFINE	1
#define	   PHY_DLL_LOCK_DIFF_DEFINE 	        0x4
#define	   PHY_CMD0_DLL_LOCK_DIFF_DEFINE		0x4

#define	   PHY_GATELVL_INIT_CS0_DEFINE          0x0
#define	   PHY_WRLVL_INIT_CS0_DEFINE		 	0x0

#define	   PHY_GATELVL_INIT_CS1_DEFINE          0x0
#define	   PHY_WRLVL_INIT_CS1_DEFINE		 	0x0
#define	   PHY_CTRL_SLAVE_RATIO_CS1_DEFINE      0x80

//- DDR2 parameters
#define    DDR2_EMIF_READ_LATENCY_DEFINE		0x09
#define    DDR2_EMIF_TIM1_DEFINE				0x0EF197EC
#define    DDR2_EMIF_TIM2_DEFINE				0x144931E2
#define    DDR2_EMIF_TIM3_DEFINE				0x00000447
#define    DDR2_EMIF_REF_CTRL_DEFINE			0x1000103D
#define    DDR2_EMIF_SDRAM_CONFIG_DEFINE		0x43801E32

#define	   DDR2_PHY_RD_DQS_CS0_DEFINE			0x35
#define	   DDR2_PHY_WR_DQS_CS0_DEFINE			0x20
#define	   DDR2_PHY_FIFO_WE_CS0_DEFINE			0x90
#define	   DDR2_PHY_WR_DATA_CS0_DEFINE			0x50
#define	   DDR2_PHY_CTRL_SLAVE_RATIO_CS0_DEFINE 0x80

//- DDR3 parameters
#define    DDR3_EMIF_READ_LATENCY_DEFINE		0xC
#define    DDR3_EMIF_TIM1_DEFINE				0x1557B9A5
#define    DDR3_EMIF_TIM2_DEFINE				0x4C5F7FEB
#define    DDR3_EMIF_TIM3_DEFINE				0x00000578
#define    DDR3_EMIF_REF_CTRL_DEFINE			0x10001860
#define    DDR3_EMIF_SDRAM_CONFIG_DEFINE		0x62833AB2

#define	   DDR3_PHY_CTRL_SLAVE_RATIO_CS0_DEFINE 0x80

#define	   DDR3_PHY_RD_DQS_CS0_DEFINE   		0x34
#define	   DDR3_PHY_WR_DQS_CS0_DEFINE			0xA
#define	   DDR3_PHY_FIFO_WE_CS0_DEFINE			0xB4
#define	   DDR3_PHY_WR_DATA_CS0_DEFINE			0x4A

//- mDDR parameters
#define    mDDR_EMIF_READ_LATENCY_DEFINE		0x5
#define    mDDR_EMIF_TIM1_DEFINE				0x0888831B
#define    mDDR_EMIF_TIM2_DEFINE				0x221A31C0
#define    mDDR_EMIF_TIM3_DEFINE				0x00000157
#define    mDDR_EMIF_REF_CTRL_DEFINE			0x10000618
#define    mDDR_EMIF_SDRAM_CONFIG_DEFINE		0x20000EA2

#define	   mDDR_PHY_RD_DQS_CS0_DEFINE	 		0x42
#define	   mDDR_PHY_WR_DQS_CS0_DEFINE	 		0x19
#define	   mDDR_PHY_FIFO_WE_CS0_DEFINE	 		0x128
#define	   mDDR_PHY_WR_DATA_CS0_DEFINE   		0x56
#define	   mDDR_PHY_CTRL_SLAVE_RATIO_CS0_DEFINE 0x80

/*************************************************************************/

//Clockout_Enable
// CLKOUT2DIV THis  field controls the external clock divison factor
//          0x0:      SYS_CLKOLUT2/1
//          0x1:      SYS_CLKOUT2/2
//          0x2:      SYS_CLKOUT2/4
//          0x3:      SYS_CLKOUT2/8
//          0x4:      SYS_CLKOUT2/16

// CLKOUT2SOURCE     This field selects the external output clock source
//          0x0:      Source clock is MAIN_PLL_CLK5
//          0x1:      Source clock is DDR_PLL_CLK1
//          0x2:      Source clock is VIDEO_PLL_CLK1
//          0x3:      Source clock is AUDIO_PLL_CLK1

#define CLKOUT2EN    1
#define CLKOUT2DIV  0
#define CLKOUT2SRC  0

/**********************************************************************/
//Pincntrl Configurations
#define PINCNTL68	(CTRL_MODULE_BASE_ADDR + 0x090C)
#define PINCNTL69	(CTRL_MODULE_BASE_ADDR + 0x0910)
#define PINCNTL72	(CTRL_MODULE_BASE_ADDR + 0x091C)
#define PINCNTL73	(CTRL_MODULE_BASE_ADDR + 0x0920)
#define PINCNTL59	(CTRL_MODULE_BASE_ADDR + 0x08e8)
#define PINCNTL60	(CTRL_MODULE_BASE_ADDR + 0x08ec)
#define PINCNTL89	(CTRL_MODULE_BASE_ADDR + 0x0960)
#define PINCNTL90	(CTRL_MODULE_BASE_ADDR + 0x0964)
#define PINCNTL91	(CTRL_MODULE_BASE_ADDR + 0x0968)
#define PINCNTL92	(CTRL_MODULE_BASE_ADDR + 0x096C)
#define PINCNTL93	(CTRL_MODULE_BASE_ADDR + 0x0970)
#define PINCNTL94	(CTRL_MODULE_BASE_ADDR + 0x0974)
#define PINCNTL95	(CTRL_MODULE_BASE_ADDR + 0x0978)
#define PINCNTL96	(CTRL_MODULE_BASE_ADDR + 0x097C)
#define PINCNTL97	(CTRL_MODULE_BASE_ADDR + 0x0980)
#define PINCNTL98	(CTRL_MODULE_BASE_ADDR + 0x0984)
#define PINCNTL99	(CTRL_MODULE_BASE_ADDR + 0x0988)
#define PINCNTL100	(CTRL_MODULE_BASE_ADDR + 0x098C)
#define PINCNTL101	(CTRL_MODULE_BASE_ADDR + 0x0990)
#define PINCNTL102	(CTRL_MODULE_BASE_ADDR + 0x0994)
#define PINCNTL103	(CTRL_MODULE_BASE_ADDR + 0x0998)
#define PINCNTL104	(CTRL_MODULE_BASE_ADDR + 0x099C)

#define PINCNTL105	(CTRL_MODULE_BASE_ADDR + 0x09A0)
#define PINCNTL106	(CTRL_MODULE_BASE_ADDR + 0x09A4)
#define PINCNTL107	(CTRL_MODULE_BASE_ADDR + 0x09A8)
#define PINCNTL108	(CTRL_MODULE_BASE_ADDR + 0x09AC)
#define PINCNTL109	(CTRL_MODULE_BASE_ADDR + 0x09B0)
#define PINCNTL110	(CTRL_MODULE_BASE_ADDR + 0x09B4)
#define PINCNTL111	(CTRL_MODULE_BASE_ADDR + 0x09B8)
#define PINCNTL112	(CTRL_MODULE_BASE_ADDR + 0x09BC)
#define PINCNTL122	(CTRL_MODULE_BASE_ADDR + 0x09E4)
#define PINCNTL123	(CTRL_MODULE_BASE_ADDR + 0x09E8)
#define PINCNTL124	(CTRL_MODULE_BASE_ADDR + 0x09EC)
#define PINCNTL125	(CTRL_MODULE_BASE_ADDR + 0x09F0)
#define PINCNTL126	(CTRL_MODULE_BASE_ADDR + 0x09F4)
#define PINCNTL127	(CTRL_MODULE_BASE_ADDR + 0x09F8)
#define PINCNTL128	(CTRL_MODULE_BASE_ADDR + 0x09FC)
#define PINCNTL129	(CTRL_MODULE_BASE_ADDR + 0x0A00)
#define	PINCNTL130	(CTRL_MODULE_BASE_ADDR + 0x0A04)
#define	PINCNTL131	(CTRL_MODULE_BASE_ADDR + 0x0A08)
#define	PINCNTL132	(CTRL_MODULE_BASE_ADDR + 0x0A0C)
#define	PINCNTL133	(CTRL_MODULE_BASE_ADDR + 0x0A10)
#define PINCNTL235	(CTRL_MODULE_BASE_ADDR + 0x0BA8)
#define PINCNTL243	(CTRL_MODULE_BASE_ADDR + 0x0BC8)
#define PINCNTL244	(CTRL_MODULE_BASE_ADDR + 0x0BCC)
#define PINCNTL245	(CTRL_MODULE_BASE_ADDR + 0x0BD0)
#define PINCNTL246	(CTRL_MODULE_BASE_ADDR + 0x0BD4)
#define PINCNTL247	(CTRL_MODULE_BASE_ADDR + 0x0BD8)
#define PINCNTL248	(CTRL_MODULE_BASE_ADDR + 0x0BDC)
#define PINCNTL249	(CTRL_MODULE_BASE_ADDR + 0x0BE0)
#define PINCNTL250	(CTRL_MODULE_BASE_ADDR + 0x0BE4)
#define PINCNTL251	(CTRL_MODULE_BASE_ADDR + 0x0BE8)
#define PINCNTL252	(CTRL_MODULE_BASE_ADDR + 0x0BEC)
#define PINCNTL253	(CTRL_MODULE_BASE_ADDR + 0x0BF0)
#define PINCNTL254	(CTRL_MODULE_BASE_ADDR + 0x0BF4)
#define PINCNTL255	(CTRL_MODULE_BASE_ADDR + 0x0BF8)
#define PINCNTL256	(CTRL_MODULE_BASE_ADDR + 0x0BFC)
#define PINCNTL257	(CTRL_MODULE_BASE_ADDR + 0x0C00)
#define PINCNTL258	(CTRL_MODULE_BASE_ADDR + 0x0C04)

/************************************************************************************/

/****************** PLL Configuration ******************************************/

/***    PLL,Control Base Adress  Base Address   ***********/
#define PLL_BASE_ADDRESS         0x481C5000
#define CONTROL_BASE_ADDRESS     0x48140000
#define OSC_SRC_CTRL            (PLL_BASE_ADDRESS+0x02c0)
#define ARM_SRC_CLK             (PLL_BASE_ADDRESS+0x02c4)

/***    Top Level ADPLLJ  Base Address   ***********/
#define MODENA_PLL_BASE         (PLL_BASE_ADDRESS+0x048)
#define L3_PLL_BASE             (PLL_BASE_ADDRESS+0x110)
#define IVA_PLL_BASE            (PLL_BASE_ADDRESS+0x0E0)
#define DSS_PLL_BASE            (PLL_BASE_ADDRESS+0x170)
#define SGX_PLL_BASE            (PLL_BASE_ADDRESS+0x0B0)
#define DSP_PLL_BASE            (PLL_BASE_ADDRESS+0x080)
#define ISS_PLL_BASE            (PLL_BASE_ADDRESS+0x140)
#define USB_PLL_BASE            (PLL_BASE_ADDRESS+0x260)
#define AUDIO_PLL_BASE          (PLL_BASE_ADDRESS+0x230)
#define VIDEO_0_PLL_BASE        (PLL_BASE_ADDRESS+0x1A0)
#define VIDEO_1_PLL_BASE        (PLL_BASE_ADDRESS+0x1D0)
#define HDMI_PLL_BASE           (PLL_BASE_ADDRESS+0x200)
#define DDR_PLL_BASE            (PLL_BASE_ADDRESS+0x290)
#define TIMER_CLKSRC				(PLL_BASE_ADDRESS + 0x02E0)

/********** ADPLL intrnal Offset Registers  ***********/
#define CLKCTRL 				0x4
#define TENABLE 				0x8
#define TENABLEDIV 				0xC
#define M2NDIV  				0x10
#define MN2DIV    			    0x14
#define STATUS 				    0x24

/*********** MODENA ADPLLS REGISTERS  *********/
#define MODENAPLL_CLKCTRL		(PLL_BASE_ADDRESS+0x04c)
#define MODENAPLL_TENABLE		(PLL_BASE_ADDRESS+0x050)
#define MODENAPLL_TENABLEDIV    (PLL_BASE_ADDRESS+0x054)
#define MODENAPLL_M2NDIV    	(PLL_BASE_ADDRESS+0x058)
#define MODENAPLL_MN2DIV    	(PLL_BASE_ADDRESS+0x05c)
#define MODENAPLL_STATUS		(PLL_BASE_ADDRESS+0x06c)

/********   SATA PLL REGISTERS   **************/
#define SATA_PLLCFG0            (CONTROL_BASE_ADDRESS+0x720)
#define SATA_PLLCFG1            (CONTROL_BASE_ADDRESS+0x724)
#define SATA_PLLCFG2            (CONTROL_BASE_ADDRESS+0x728)
#define SATA_PLLCFG3            (CONTROL_BASE_ADDRESS+0x72c)
#define SATA_PLLCFG4            (CONTROL_BASE_ADDRESS+0x730)
#define SATA_PLLSTATUS          (CONTROL_BASE_ADDRESS+0x734)
#define SATA_RXSTATUS           (CONTROL_BASE_ADDRESS+0x738)
#define SATA_TXSTATUS           (CONTROL_BASE_ADDRESS+0x73c)

/********   PCIE PLL REGISTERS   **************/
#define PCIE_PLLCFG0            (CONTROL_BASE_ADDRESS+0x6D8)
#define PCIE_PLLCFG1            (CONTROL_BASE_ADDRESS+0x6DC)
#define PCIE_PLLCFG2            (CONTROL_BASE_ADDRESS+0x6E0)
#define PCIE_PLLCFG3            (CONTROL_BASE_ADDRESS+0x6E4)
#define PCIE_PLLCFG4            (CONTROL_BASE_ADDRESS+0x6E8)
#define PCIE_PLLSTATUS          (CONTROL_BASE_ADDRESS+0x6EC)
#define PCIE_RXSTATUS           (CONTROL_BASE_ADDRESS+0x6F0)
#define PCIE_TXSTATUS           (CONTROL_BASE_ADDRESS+0x6F4)
#define SERDES_REFCLK_CTL 	    (CONTROL_BASE_ADDRESS+0xE24)


//#define WR_MEM_32(addr, data)    *(unsigned int*)(addr) =(unsigned int)(data)
//#define RD_MEM_32(addr) 	  	 *(unsigned int*)(addr)
#define UWORD32 			     unsigned int


#define CONTROL_STATUS              (CTRL_MODULE_BASE_ADDR + 0x040)
#define BANDGAP0_TRIM       		(CTRL_MODULE_BASE_ADDR + 0x44C)
#define BANDGAP1_TRIM       		(CTRL_MODULE_BASE_ADDR + 0x454)
#define PLL_SUBSYSTEM_BASE_ADDR     PLL_BASE_ADDRESS
#define DSPPLL_CLOCK_CONTROL        (PLL_SUBSYSTEM_BASE_ADDR + 0x084)
#define SGXPLL_CLOCK_CONTROL        (PLL_SUBSYSTEM_BASE_ADDR + 0x0B4)
#define IVAPLL_CLOCK_CONTROL        (PLL_SUBSYSTEM_BASE_ADDR + 0x0E4)
#define L3PLL_CLOCK_CONTROL         (PLL_SUBSYSTEM_BASE_ADDR + 0x114)
#define ISSPLL_CLOCK_CONTROL        (PLL_SUBSYSTEM_BASE_ADDR + 0x144)
#define DSSPLL_CLOCK_CONTROL        (PLL_SUBSYSTEM_BASE_ADDR + 0x174)
#define VID0PLL_CLOCK_CONTROL       (PLL_SUBSYSTEM_BASE_ADDR + 0x1A4)
#define VID1PLL_CLOCK_CONTROL       (PLL_SUBSYSTEM_BASE_ADDR + 0x1D4)
#define HDMIPLL_CLOCK_CONTROL       (PLL_SUBSYSTEM_BASE_ADDR + 0x204)
#define AUDIOPLL_CLOCK_CONTROL      (PLL_SUBSYSTEM_BASE_ADDR + 0x234)
#define USBPLL_CLOCK_CONTROL        (PLL_SUBSYSTEM_BASE_ADDR + 0x264)
#define DDRPLL_CLOCK_CONTROL        (PLL_SUBSYSTEM_BASE_ADDR + 0x294)
#define MODENAPLL_CLOCK_CONTROL     (PLL_SUBSYSTEM_BASE_ADDR + 0x04C)


/*************************************************************************************************************
							 ADPLLJ CLKCNTRL REGISTER CONFIGURATIONS

***************************************************************************************************************/
// ADPLLJ_CLKCRTL_Register Value Configurations
//add ntrim values for test device  --- Rajesh/Hemanth(bits 28:24)
#define ADPLLJ_CLKCRTL_HS2       0x00000801 //HS2 Mode,TINTZ =1  --used by all PLL's except HDMI
#define ADPLLJ_CLKCRTL_HS1       0x00001001 //HS1 Mode,TINTZ =1  --used only for HDMI
#define ADPLLJ_CLKCRTL_CLKDCO    0x200a0000 // Enable CLKDCOEN,CLKLDOEN,CLKDCOPWDNZ -- used for HDMI,USB


/****************************************************************************************************************/
// CONTROL MMMR LOCK and UNLOCK Registers

#define control_pllss_mmr_lock                     	0x481C5040
#define control_mmr_lock0                          	0x48140060
#define control_mmr_lock1                          	0x48140064
#define control_mmr_lock2                          	0x48140068
#define control_mmr_lock3                          	0x4814006c
#define control_mmr_lock4                          	0x48140070

#define control_pllss_mmr_lock_unlock_val          	0x1EDA4C3D
#define control_mmr_lock0_unlock_val               	0x2FF1AC2B
#define control_mmr_lock1_unlock_val               	0xF757FDC0
#define control_mmr_lock2_unlock_val               	0xE2BC3A6D
#define control_mmr_lock3_unlock_val               	0x1EBF131D
#define control_mmr_lock4_unlock_val               	0x6F361E05

#define EMIF_CLK_GATE 								0x48140694
#define SECSS_CLK_SRC 								0x481C52EC
/****************************************************************************************************************/
#define NETRA_TEST_DEVICE_FW_VAL 0xFFFFFFFF
#define VLYNQ_FW			0x47C06088
#define EMIF_FW 			0x47C0C088
#define GPMC_FW 			0x47C0E088
#define OCMC_RAM0_FW 		0x47C10088
#define OCMC_RAM1_FW 		0x47C12088
#define SGX530_FW			0x47C14088
#define GEM_SDMA_FW 		0x47C16088
#define DUCATI_FW 			0x47C18088
#define PCIE_FW				0x47C1A088
#define TPPSS_FW			0x47C1C088
#define IVAHD_SL2_FW 		0x47C1E088
#define IVAHD_CFG_FW 		0x47C20088
#define MCASP_FW 			0x47C28088
#define HDMI_FW 			0x47C2C088
#define MCBSP_FW			0x47C2E088
#define TPTC_FW 			0x47C30088
#define TPCC_FW 			0x47C38088
#define USB_FW 				0x47C3A088
#define DEVICE_ID  		 	0x48140040

#define CM_ALWON_L4_SLOW_CLKSTCTRL  (PRCM_BASE_ADDR + 0x1400)
#define CM_ALWON_SPINBOX_CLKCTRL    (PRCM_BASE_ADDR + 0x1598)

void WR_MEM_32(unsigned int addr, unsigned int data);
unsigned int RD_MEM_32(unsigned int addr);
#define SIZE 1

int CLKIN =	20;

volatile unsigned int 	*virt_addr;

int us_fd;

void WR_MEM_32(unsigned int addr, unsigned int data)
{
	void *map_base;
	unsigned int data_bs;
	unsigned int data_as;
	unsigned int size;
	unsigned int target;

	target = addr;
	size = 4;

	/* Map one page */
	map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, us_fd, target & ~MAP_MASK);
	if(map_base == (void *) -1){
		PRINT ("Could not open the mem file \n");
	}
//	PRINT("data %x\n", data);
	virt_addr = (unsigned int *)(map_base + (target & MAP_MASK));
	data_bs = *virt_addr;
	*virt_addr = data;
	data_as = *virt_addr;

//	DISP_ADDRPHY_ADDRVIRT_DATA_BW(target, data_bs);
//	DISP_ADDRPHY_ADDRVIRT_DATA_AW(target, data_as);

	munmap(map_base, MAP_SIZE);
}

unsigned int RD_MEM_32(unsigned int addr)
{
	void *map_base;
	unsigned int data;
	unsigned int size;
	unsigned int target;

	target = addr;
	size = 4;

	/* Map one page */
	map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, us_fd, target & ~MAP_MASK);
	if(map_base == (void *) -1){
	  PRINT ("Could not open the mem file \n");//FATAL; FATAL;
	}

	virt_addr = (unsigned int*)(map_base + (target & MAP_MASK));
	data = *virt_addr;
	DISP_ADDRPHY_ADDRVIRT_DATA(target, virt_addr, data);

	munmap(map_base,/*MAP_SIZE */ MAP_SIZE);

	return data;
}


int HSMODE,CLKOUT = 0;


void wait_delay(UWORD32 noopcount)
{
	volatile int i;

	for(i=0;i<noopcount;i++)
	{

	}
}

void ISS_A8_Enable(void)
{
     WR_MEM_32(0x48180D00, 0x2); //PM_ISP_PWRSTCTRL
     WR_MEM_32(0x48180D10, 0x3); //RM_ISP_RSTCTRL
     WR_MEM_32(0x48180700, 0x2) ; //CM_ISP_CLKSTCTRL
     WR_MEM_32(0x48180720, 0x2) ;  //CM_ISP_ISP_CLKCTRL
     WR_MEM_32(0x48180724, 0x2) ;  //CM_ISP_FDIF_CLKCTRL

     //GEL_TextOut("\tPRCM for ISS Done Successfully.....  \n","Output",1,1,1);

    /* I2c2  configuration Function 6*/
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0924), 0x60020); /* i2c2_scl_mux0 */
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0928), 0x60020); /* i2c2_sda_mux0 */

    /* I2c0 configuration function 1 TODO Remove this */
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0c18), 0x60001);   /* i2c0_scl */
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0c1c), 0x60001);   /* i2c0_sda */

    /* Set the access to the I2c registers */
    WR_MEM_32((PRCM_BASE_ADDR + 0x1564), 0x2);
    WR_MEM_32((PRCM_BASE_ADDR + 0x1568), 0x2);


    WR_MEM_32(CM_ALWON_GPIO_1_CLKCTRL, RD_MEM_32(CM_ALWON_GPIO_1_CLKCTRL) | 0x2);

    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0AAC), 0x00060002);    // CAM_HSYNC        PINCNTL172[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0AB0), 0x00060002);    // CAM_VSYNC        PINCNTL173[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0AB8), 0x00060002);    // CAM_PCLK         PINCNTL175[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A58), 0x00060020);    // CAM_WEn          PINCNTL151[5] cam_de_mux1
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A60), 0x00060080);    // gpio2[18]        PINCNTL153[7] // 0x00060080   CAM_RST          PINCNTL153[5]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A64), 0x00060020);    // CAM_STROBE       PINCNTL154[5]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A68), 0x00060020);    // CAM_SHTR         PINCNTL155[5]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0AA8), 0x00060002);    // CAM_D0           PINCNTL171[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0AA4), 0x00060002);    // CAM_D1           PINCNTL170[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0AA0), 0x00060002);    // CAM_D2           PINCNTL169[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A9C), 0x00060002);    // CAM_D3           PINCNTL168[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A98), 0x00060002);    // CAM_D4           PINCNTL167[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A94), 0x00060002);    // CAM_D5           PINCNTL166[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A90), 0x00060002);    // CAM_D6           PINCNTL165[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A8C), 0x00060002);    // CAM_D7           PINCNTL164[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A6C), 0x00060002);    // CAM_D8           PINCNTL156[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A70), 0x00060002);    // CAM_D9           PINCNTL157[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A74), 0x00060002);    // CAM_D10          PINCNTL158[1]
    WR_MEM_32((CTRL_MODULE_BASE_ADDR + 0x0A78), 0x00060002);    // CAM_D11          PINCNTL159[1]

     //GEL_TextOut("\tSensor Config Successfully.....  \n","Output",1,1,1);
}

void Emif_PRCM_Clk_Enable(void)
{
	WR_MEM_32(CM_DEFAULT_FW_CLKCTRL, 0x2); 		  /*Enable the EMIF FireWall Clocks*/
	WR_MEM_32(CM_DEFAULT_L3_FAST_CLKSTCTRL, 0x2); /*Enable the Power Domain Transition of L3 Fast Domain Peripheral*/
	WR_MEM_32(CM_DEFAULT_EMIF_0_CLKCTRL,    0x2); /*Enable EMIF0 Clock*/
	WR_MEM_32(CM_DEFAULT_EMIF_1_CLKCTRL,    0x2); /*Enable EMIF1 Clock*/
	WR_MEM_32(CM_DEFAULT_DMM_CLKCTRL,       0x2); /*Enable EMIF1 Clock*/
	while((RD_MEM_32(CM_DEFAULT_L3_FAST_CLKSTCTRL) & 0x300)!=0x300);	/*Poll for L3_FAST_GCLK  & DDR_GCLK  are active*/
	while(RD_MEM_32(CM_DEFAULT_EMIF_0_CLKCTRL)!=0x2);		/*Poll for Module is functional*/
	while(RD_MEM_32(CM_DEFAULT_EMIF_1_CLKCTRL)!=0x2);		/*Poll for Module is functional*/
	while(RD_MEM_32(CM_DEFAULT_DMM_CLKCTRL)!=0x2);			/*Poll for Module is functional*/
}

/*************************************************************************************************/

/********************************************************************************************
//Note: By default the Values of ADPLL are set using "ALL_ADPLL_CLOCKS_ENABLE" function if called.

//CONFIGURE ADPLL TO GET REQUIRED CLKOUTS  with your required values of N,M,M2

//RUN THE HOTMENU of the REQUIRED CHANGED PLL
*********************************************************************************************/

/***********************************************************************************
Change the values of N,M,M2 to get the Required ADPLL Clockout
cmdxxxxPLL(int CLKIN,int N, int M, int M2)

****   REF_CLK       = (CLKIP)/N+1  [  REF_CLK < 2.5MHz      ]
****   DCOCLK_HS2    = (REF_CLK)*M     [500  < DCOCLK < 1000MHz ]
****   DCOCLK_HS1    = (REF_CLK)*M     [1000 < DCOCLK < 2000MHz ]--used for HDMI CLKDCO
****   CLKOUT        =  DCOCLK/M2      [10   < CLKOUT < 2000MHz ]
****   N+1 			           		   [1..256]
****   M    		           		   [2..4095]
****   M2    		           		   [1..127]
****
*************************************************************************************/
void print_pll_status(UWORD32 status,UWORD32 pll_base)
{
	UWORD32 temp,mn2,m2n,m2,n,m;
	temp = status;

	if((temp & 0x00000001) == 0x1){
		PRINT("\t is in Bypass \n");
	}
	else {
		PRINT("\t is NOT in Bypass \n");
	}
	if(((temp >> 4) & 0x00000001) == 0x1)
	{
		PRINT("\t is Locked \n");
		mn2=RD_MEM_32(pll_base+MN2DIV);
		m = mn2 & 0xFFF;
		m2n=RD_MEM_32(pll_base+M2NDIV);
		if(pll_base == MODENA_PLL_BASE)
		{
			n = m2n & 0x3F;
			m2 = (m2n>>16) & 0x1F;
		}
		else {
			n = m2n & 0xFF;
			m2 = (m2n>>16) & 0x7F;
		}

		PRINT("\t Freq %d MHz \n",(m*(20/(n+1)))/m2);
	}
	else {
		PRINT("\t is NOT Locked \n");
	}

	if(((temp >> 5) & 0x00000001) == 0x1)
	{
		PRINT("\t CLKOUT is Enabled \n");
	}
	else {
		PRINT("\t CLKOUT is Disabled \n");
	}

	PRINT("\t******************\n \n");
}

void GET_PLL_Status(void)
{

    UWORD32 rval_ctrl;
    rval_ctrl = RD_MEM_32(MODENAPLL_STATUS);
    PRINT("\t MODENA ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,MODENA_PLL_BASE);

    rval_ctrl = RD_MEM_32(DSP_PLL_BASE+STATUS);
    PRINT("\t DSP ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,DSP_PLL_BASE);


    rval_ctrl = RD_MEM_32(IVA_PLL_BASE+STATUS);
    PRINT("\t IVA ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,IVA_PLL_BASE);

    rval_ctrl = RD_MEM_32(ISS_PLL_BASE+STATUS);
    PRINT("\t ISS ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl, ISS_PLL_BASE);

    rval_ctrl = RD_MEM_32(L3_PLL_BASE+STATUS);
    PRINT("\t L3 ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,L3_PLL_BASE);

    rval_ctrl = RD_MEM_32(DSS_PLL_BASE+STATUS);
    PRINT("\t DSS ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,DSS_PLL_BASE);

    rval_ctrl = RD_MEM_32(SGX_PLL_BASE+STATUS);
    PRINT("\t SGX ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,SGX_PLL_BASE);

    rval_ctrl = RD_MEM_32(USB_PLL_BASE+STATUS);
    PRINT("\t USB ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,USB_PLL_BASE);

    rval_ctrl = RD_MEM_32(VIDEO_0_PLL_BASE+STATUS);
    PRINT("\t VIDEO_0 ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,VIDEO_0_PLL_BASE);

    rval_ctrl = RD_MEM_32(VIDEO_1_PLL_BASE+STATUS);
    PRINT("\t VIDEO_1 ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,VIDEO_1_PLL_BASE);

    rval_ctrl = RD_MEM_32(HDMI_PLL_BASE+STATUS);
    PRINT("\t HDMI ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,HDMI_PLL_BASE);

    rval_ctrl = RD_MEM_32(DDR_PLL_BASE+STATUS);
    PRINT("\t DDR ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,DDR_PLL_BASE);

    rval_ctrl = RD_MEM_32(AUDIO_PLL_BASE+STATUS);
    PRINT("\t AUDIO ADPLLJ CLKSTATUS\n");
    print_pll_status(rval_ctrl,AUDIO_PLL_BASE);
}

void enable_gpio2_module(void)
{
	WR_MEM_32(CM_ALWON_GPIO_1_CLKCTRL, RD_MEM_32(CM_ALWON_GPIO_1_CLKCTRL) | 0x2);
	//PRINT("\t Enable GPIO2 module .... \n");
}

#define PM_ACTIVE_PWRSTCTRL PRCM_BASE_ADDR+0x0A00
#define PM_ACTIVE_PWRSTST   PRCM_BASE_ADDR+0x0A04
#define PM_IVA_PWRSTCTRL    PRCM_BASE_ADDR+0x0C00
#define PM_IVA_PWRSTST      PRCM_BASE_ADDR+0x0C04
#define PM_ISS_PWRSTCTRL    PRCM_BASE_ADDR+0x0D00
#define PM_ISS_PWRSTST      PRCM_BASE_ADDR+0x0D04

#define PM_SGX_PWRSTCTRL    PRCM_BASE_ADDR+0x0F00
#define RM_SGX_RSTCTRL      PRCM_BASE_ADDR+0x0F04
#define PM_SGX_PWRSTST      PRCM_BASE_ADDR+0x0F10
#define RM_SGX_RSTST        PRCM_BASE_ADDR+0x0F14
#define CM_SGX_CLKSTCTRL    PRCM_BASE_ADDR+0x0900
#define CM_SGX_SGX_CLKCTRL  PRCM_BASE_ADDR+0x0920

#define PM_DSS_PWRSTCTRL    PRCM_BASE_ADDR+0x0E00
#define PM_DSS_PWRSTST      PRCM_BASE_ADDR+0x0E04
#define RM_DSS_RSTCTRL      PRCM_BASE_ADDR+0x0E10
#define RM_DSS_RSTST        PRCM_BASE_ADDR+0x0E14
#define CM_DSS_CLKSTCTRL    PRCM_BASE_ADDR+0x0800
#define CM_DSS_DSS_CLKCTRL  PRCM_BASE_ADDR+0x0820
#define CM_DSS_HDMI_CLKCTRL PRCM_BASE_ADDR+0x0824


void print_pd_status(UWORD32 status)
{
	//PRINT("....%x\n",status);
	if((status & 0x3) == 0x3) PRINT("Domain is ON\n");
	if((status & 0x3) == 0x0) PRINT("Domain is OFF\n");
}

void GET_PD_STATUS(void)
{
	UWORD32 status;
	//ACTIVE OR GEM
	status = RD_MEM_32(PM_ACTIVE_PWRSTST);
	PRINT("ACTIVE  ");
	print_pd_status(status);
	//IVA
	status = RD_MEM_32(PM_IVA_PWRSTST);
	PRINT("IVA  ");
	print_pd_status(status);
	//ISS
	status = RD_MEM_32(PM_ISS_PWRSTST);
	PRINT("ISS  ");
	print_pd_status(status);
	//DSS
	status = RD_MEM_32(PM_DSS_PWRSTST);
	PRINT("DSS  ");
	print_pd_status(status);
	//SGX
	status = RD_MEM_32(PM_SGX_PWRSTST);
	PRINT("SGX  ");
	print_pd_status(status);
}

void DSP_Disable(void)
{
	//DSP
	WR_MEM_32(CM_GEM_CLKSTCTRL,0x1);

	WR_MEM_32(PM_ACTIVE_PWRSTCTRL,0x0);
	while(RD_MEM_32(PM_ACTIVE_PWRSTST) != 0x0);
}

void SGX_Disable(void)
{
	//SGX
	WR_MEM_32(CM_SGX_CLKSTCTRL,0x1);

	WR_MEM_32(PM_SGX_PWRSTCTRL,0x0);
	while(RD_MEM_32(PM_SGX_PWRSTST) != 0x0);
}

void DSS_Disable(void)
{
	//DSS
	RD_MEM_32(PM_DSS_PWRSTCTRL);
	RD_MEM_32(RM_DSS_RSTCTRL);
	RD_MEM_32(PM_DSS_PWRSTST);
	RD_MEM_32(RM_DSS_RSTST);
	RD_MEM_32(CM_DSS_CLKSTCTRL);
	RD_MEM_32(CM_DSS_DSS_CLKCTRL);
	RD_MEM_32(CM_DSS_HDMI_CLKCTRL);

	WR_MEM_32(CM_DSS_CLKSTCTRL,0x1);

	WR_MEM_32(PM_DSS_PWRSTCTRL,0x0);
	while(RD_MEM_32(PM_DSS_PWRSTST) != 0x0);

	RD_MEM_32(PM_DSS_PWRSTCTRL);
	RD_MEM_32(RM_DSS_RSTCTRL);
	RD_MEM_32(PM_DSS_PWRSTST);
	RD_MEM_32(RM_DSS_RSTST);
	RD_MEM_32(CM_DSS_CLKSTCTRL);
	RD_MEM_32(CM_DSS_DSS_CLKCTRL);
	RD_MEM_32(CM_DSS_HDMI_CLKCTRL);
}

#define SECSS_BASE_ADDR         0x41000000
#define SECSS_CM3_PWR_CTRL      0x411F4004 //make it 0
#define SECSS_SW_RST            0x411F4008
#define SECSS_SW_CLK_FORCE      0x411F2000
#define SECSS_SW_CLK_DISABLE    0x411F2004 //make it 0
#define SECSS_AES01_S_SYSCONFIG 0x41140084 //make it 0x1
#define SECSS_AES23_S_SYSCONFIG 0x411A0084 //make it 0x1
#define SECSS_DES_SYSCONFIG_SEC 0x41160034 //make it 0x1
#define SECSS_PKA_SYSCONFIG     0x41121FF0 //make it 0x1
#define SECSS_RNG_SYSCONFIG     0x41131FE4 //make it 0x1
#define SECSS_DMA4_SYSCONFIG    0x411E002C //make it 0x1
#define SECSS_FORCE_IDLE        0x411F2030

#define CM_ALWON_SECSS_CLKCTRL (PRCM_BASE_ADDR + 0x15C8)
#define RM_ALWON_RSTCTRL (PRCM_BASE_ADDR + 0x1810)

void secss_power_reduction(void)
{
	WR_MEM_32(SECSS_CM3_PWR_CTRL, 0x0);
	WR_MEM_32(SECSS_SW_CLK_DISABLE, 0x0);
	WR_MEM_32(SECSS_AES01_S_SYSCONFIG, 0x1);
	WR_MEM_32(SECSS_AES23_S_SYSCONFIG, 0x1);
	WR_MEM_32(SECSS_DES_SYSCONFIG_SEC, 0x1);
	WR_MEM_32(SECSS_PKA_SYSCONFIG, 0x1);
	WR_MEM_32(SECSS_RNG_SYSCONFIG, 0x1);
	WR_MEM_32(SECSS_DMA4_SYSCONFIG, 0x1);
	WR_MEM_32(SECSS_CLK_SRC,0x1);
	PRINT("secss put in low power state\n");
}

void tppss_power_reduction(void)
{
	unsigned int k=0;

	PRINT("\tPRCM for TPPSS Initialization in Progress \n");
	WR_MEM_32(RM_DEFAULT_RSTCTRL, 0xFD); /*Bring TPP ARM out of Reset*/

	/*Enable the TPPSS Clocks*/

	WR_MEM_32(CM_DEFAULT_TPPSS_CLKSTCTRL, 2);
	WR_MEM_32(CM_DEFAULT_TPPSS_CLKCTRL,   2);
	/* TO DO...when VIDEO_0_PLL is OFF, TPPSS is not able to go to fully functional mode */
	while(((RD_MEM_32(CM_DEFAULT_TPPSS_CLKSTCTRL) & 0x7F00)>>8)!=0x73);//To check for 0x7F when STC clock is ON.
	while(((RD_MEM_32(CM_DEFAULT_TPPSS_CLKCTRL)&0x30000)>>16)!=0x3);//To check for 0x0 when STC clokc is ON.
	PRINT("\tPRCM for TPPSS Initialization is Done \n");
	//Fill the ITCM RAM of TPPSS

	for(k=0; k<10; k++) {
		WR_MEM_32(TPPIRAM_BASE_ADDR+4*k, 0xEAFFFFFE);
	}

	WR_MEM_32(RM_DEFAULT_RSTCTRL,   0xFC); /*Bring TPP out of Local Reset*/
	PRINT("\tTPPSS Initialization in Done \n");
}

void DDR_IO_RX_ShutDown(void)
{
    Emif_PRCM_Clk_Enable();
    WR_MEM_32(EMIF4_0_DDR_PHY_CTRL_1,RD_MEM_32(EMIF4_0_DDR_PHY_CTRL_1)|0x00100000); //bit 20 is IO rcv pwrdn
    WR_MEM_32(EMIF4_1_DDR_PHY_CTRL_1,RD_MEM_32(EMIF4_1_DDR_PHY_CTRL_1)|0x00100000); //bit 20 is IO rcv pwrdn
	WR_MEM_32(0x4C0000E4, (RD_MEM_32(0x4C0000E4) | 0x00100000));
	WR_MEM_32(0x4D0000E4, (RD_MEM_32(0x4D0000E4) | 0x00100000));

    PRINT("DDR IOs RX is shutdown \n");
}

void GET_SERDES_STATUS(void)
{
	int temp;
	temp = RD_MEM_32(SATA_PLLCFG0);
	PRINT("test %x\n", temp);
	PRINT("SATA_PLLCFG0 %x\n",RD_MEM_32(SATA_PLLCFG0));
	PRINT("SATA_PLLCFG1 %x\n",RD_MEM_32(SATA_PLLCFG1));
	PRINT("SATA_PLLCFG2 %x\n",RD_MEM_32(SATA_PLLCFG2));
	PRINT("SATA_PLLCFG3 %x\n",RD_MEM_32(SATA_PLLCFG3));
	PRINT("SATA_PLLCFG4 %x\n",RD_MEM_32(SATA_PLLCFG4));
	PRINT("SATA_PLLSTATUS %x\n",RD_MEM_32(SATA_PLLSTATUS));
	PRINT("SATA_RXSTATUS %x\n",RD_MEM_32(SATA_RXSTATUS));
	PRINT("SATA_TXSTATUS %x\n",RD_MEM_32(SATA_TXSTATUS));

	/********   PCIE PLL REGISTERS   **************/
	PRINT("PCIE_PLLCFG0 %x\n",RD_MEM_32(PCIE_PLLCFG0));
	PRINT("PCIE_PLLCFG1 %x\n",RD_MEM_32(PCIE_PLLCFG1));
	PRINT("PCIE_PLLCFG2 %x\n",RD_MEM_32(PCIE_PLLCFG2));
	PRINT("PCIE_PLLCFG3 %x\n",RD_MEM_32(PCIE_PLLCFG3));
	PRINT("PCIE_PLLCFG4 %x\n",RD_MEM_32(PCIE_PLLCFG4));
	PRINT("PCIE_PLLSTATUS %x\n",RD_MEM_32(PCIE_PLLSTATUS));
	PRINT("PCIE_RXSTATUS %x\n",RD_MEM_32(PCIE_RXSTATUS));
	PRINT("PCIE_TXSTATUS %x\n",RD_MEM_32(PCIE_TXSTATUS));
	PRINT("SERDES_REFCLK_CTL %x\n",RD_MEM_32(SERDES_REFCLK_CTL));

}

void GET_GPMC_IO_DOMAIN_STATUS(void)
{
	int i;
	//for(i=0;i<45;i++) RD_MEM_32(0x48140960+i*4);
	//for(i=0;i<27;i++) RD_MEM_32(0x48140B9C+i*4);
	for(i=0;i<275;i++) RD_MEM_32(0x48140800+i*4);
}

int main(int argc, char **argv)
{
    unsigned int addr, data;

    if(argc < 2) {
      PRINT("Usage: %s [a|p|s|c|t]\n",  argv[0]);
      /* a - All
       * p - print
       * s - set
       * c - compare
       */
      exit(1);
    }

    if((us_fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
      PRINT ("Could not open the mem file \n");//FATAL;
    }

    PRINT("/dev/mem opened.\n");


    switch (argv[1][0]) {
		case 't':
			if (argc <4) {
				PRINT ("Please enter test address & size\n");
				exit(0);
			}
			addr = strtoul(argv[2], 0, 0);
			data = strtoul(argv[3], 0, 0);
			PRINT ("Trying to test Addr: 0x%x data :0x%x\n", addr, data);
			break;
		case 'r':
			ISS_A8_Enable();
#ifndef	DM385_CONFIG
			secss_power_reduction();
#endif
			SGX_Disable();
			DDR_IO_RX_ShutDown();
#ifdef DSP_OFF
			DSP_Disable();
#endif
#ifdef DSS_OFF
			tppss_power_reduction();
			DSS_Disable();
#endif
			break;
		case 'k':
#ifdef DSS_OFF
			DSS_Disable();
#endif
			break;
		case 'i':
			ISS_A8_Enable();
			break;
		case 's':
			PRINT ("GET PLL Status\n");
			GET_PLL_Status ();
			GET_PD_STATUS();
			GET_SERDES_STATUS();
			GET_GPMC_IO_DOMAIN_STATUS();
			break;
      	default:
			PRINT ("Choose right option\n");
			break;
    }

    close(us_fd);

    return 0;
}
