/** ===========================================================================
* @file main.c
*
* @path $(IPNCPATH)\sys_adm\boot_proc
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2010
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys_env_type.h>
#include <system_default.h>
#include <ipnc_gio_util.h>


/**
 * @brief	main entry
 * @param	command line argument
 * @return	error code
 */
int main(int argc,char *argv[]) 
{
	unsigned int gpio_num,gpio_value;
	
	if(argc < 3)
	{
		printf("Usage ./gpio_test gpio_num gpio_value\n");

		return 0;
	}

	gpio_num = atoi(argv[1]);
	gpio_value = atoi(argv[2]);

	printf("num=%d,value=%d\n",gpio_num,gpio_value);

	ipnc_gio_write(gpio_num,gpio_value);

	return 0;
}
