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

#ifndef _DEV_DMA_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DEV_DMA_H_

#include <linux/ioctl.h>
#include <osa_dma.h>

#define DMA_DRV_NAME  "dev_dma"
//#define DMA_DEBUG 

#define DMA_DRV_MAGICNUM       (0xDA)

#define DMA_DEV_MAX_CH         (64) 

#define DMA_CMD_CH_OPEN        (0x01)
#define DMA_CMD_CH_CLOSE       (0x02)
#define DMA_CMD_COPY_2D        (0x03)
#define DMA_CMD_COPY_1D        (0x04)
#define DMA_CMD_FILL_2D        (0x05)
#define DMA_CMD_FILL_1D        (0x06)

#define DMA_IOCTL_CMD_MAKE(cmd)     ( _IO( DMA_DRV_MAGICNUM, cmd) )
#define DMA_IOCTL_CMD_GET(cmd)      ( _IOC_NR(cmd) )
#define DMA_IOCTL_CMD_IS_VALID(cmd) ( (_IOC_TYPE(cmd) == DMA_DRV_MAGICNUM ) ? 1 : 0)

typedef struct {

  int chId;         // [ O]
  int mode;
  int maxTransfers;

} DMA_OpenClosePrm;

typedef struct {

  int chId;
  int count;
  int copyFillType;
  
  void *prm;
  
} DMA_CopyFillPrm;

int  DMA_devInit(void);
void DMA_devExit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

