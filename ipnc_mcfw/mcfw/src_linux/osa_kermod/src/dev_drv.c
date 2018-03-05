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

#include <dev_dma.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>           /* everything... */
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <asm/uaccess.h>        /* copy_*_user */

#include <linux/io.h>


int DRV_devInit(void)
{
  int     result;

  result = DMA_devInit();
  if(result!=0) {
    return result;
  }

  printk(KERN_INFO "DRV: Module install successful\n");
  printk(KERN_INFO "DRV: Module built on %s %s \n", __DATE__, __TIME__);

  return result;
}

void DRV_devExit(void)
{
  DMA_devExit();
}

module_init(DRV_devInit);
module_exit(DRV_devExit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
