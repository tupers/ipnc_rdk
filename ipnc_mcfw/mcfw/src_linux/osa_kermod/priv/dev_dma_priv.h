/*
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _DEV_DMA_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DEV_DMA_PRIV_H_

#include <dev_dma.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>           /* everything... */
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <asm/uaccess.h>        /* copy_*_user */
#include <asm/hardware/edma.h>
#include <dev_debug.h>

#define DMA_PARAM_NO_LINK          (0xFFFF)
#define DMA_PARAM_OPT_SYNCDIM_AB   (1<<2)
#define DMA_PARAM_OPT_STATIC       (1<<3)
#define DMA_PARAM_OPT_TCINTEN      (1<<20)
#define DMA_PARAM_OPT_ITCCHEN      (1<<23)

#define DMA_NORMAL_EVENT_QUEUE     (EVENTQ_1)

typedef struct {
  
  unsigned long srcPhysAddr;
  unsigned long dstPhysAddr;
  
  int copyWidth;
  int copyHeight;
  
  int srcOffsetH;
  int dstOffsetH;
  
  int skipH; 

  unsigned long dstVirtAddr;  
  int fillValue;
  
  int size;  

} DRV_CopyFillPrm;

typedef struct {

  int chId;
  struct completion dma_complete;     
  struct edmacc_param dmaParam;

  int copyFillType;
  int maxCount;
  int count;
  int curCount;
  DRV_CopyFillPrm *transferPrm;

} DMA_CopyFillObj;

typedef struct {
  
  int dmaMode;
  
  DMA_CopyFillObj *copyFillObj;
  
} DMA_Obj;

typedef struct {

  struct cdev cdev;             /* Char device structure    */
  int     major;
  struct semaphore semLock;
    
  DMA_Obj *pObj[DMA_DEV_MAX_CH];
  
} DMA_Dev;

extern DMA_Dev gDMA_dev;

int DMA_copyFillRun( DMA_CopyFillPrm *pUserPrm );
int DMA_copyFillCreate(DMA_OpenClosePrm *prm);
int DMA_copyFillDelete(DMA_OpenClosePrm *prm);

void DMA_dumpPARAM(const char *message, int channel);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _DEV_DMA_PRIV_H_  */

