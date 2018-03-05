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

#ifndef _DEV_DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DEV_DEBUG_H_

#ifdef DEBUG
#define FUNCDBG(format, a...) \
        printk( KERN_INFO "DRV :%s:%d: " format, __func__, __LINE__, ## a )
#else
#define FUNCDBG(...)
#endif

#define FUNCMSG(format, a...) \
        printk( KERN_INFO "DRV :%s:%d: " format, __func__, __LINE__, ## a )

#define FUNCERR(format, a...) \
    printk( KERN_ERR " DRV ERR :%s:%d: " format, __func__, __LINE__, ## a)


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _DEV_DEBUG_H_  */

