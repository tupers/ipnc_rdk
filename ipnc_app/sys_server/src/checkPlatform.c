/** ===========================================================================
* @file checkPlatform.c
*
* @path $(IPNCPATH)\sys_server\src\
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @brief	CheckCpuSpeed
 * @param	none
 * @return	0 : DM365 \n
 *			1 : DM368 \n
 *			-1: error
 */
int CheckCpuSpeed( void )
{
	static int CurrentStatus = -255;
	int fd_cpu = -1;
	int ret = -1;
	char StrBuffer[300];
	char TmpBuf[50];
	char *pStr = NULL;
	float cpu_mips = 0;
	char delima_buf[] = ":\r\n ";

	if( CurrentStatus >= -1 )
	{
		fprintf(stderr,"CPU CurrentStatus is = %d \n", CurrentStatus);
		return CurrentStatus;
	}

	fd_cpu = open("/proc/cpuinfo", O_RDONLY);
	if( fd_cpu < 0 )
	{
		fprintf(stderr,"CheckCpuSpeed open file fail \n");
		ret = -1;
		goto CHECK_CPU_END;

	}

	ret = read(fd_cpu,  StrBuffer, sizeof(StrBuffer)-1);
	if( ret <= 0 )
	{
		fprintf(stderr,"read device error !!");
		ret = -1;
		goto CHECK_CPU_END;

	}

	ret = -1;
	StrBuffer[sizeof(StrBuffer)-1] = '\0';
	pStr = strtok(StrBuffer,delima_buf );

	while( pStr != NULL )
	{
		sscanf( pStr,"%s",TmpBuf);

		if( strncmp(TmpBuf, "BogoMIPS", sizeof("BogoMIPS")) == 0 )
		{

			pStr = strtok(NULL, delima_buf);

			sscanf( pStr,"%f",&cpu_mips);

			//fprintf(stderr,"CPU MIPS = %f \n", cpu_mips);

			if( cpu_mips > 100 && cpu_mips <= 1000 )
			{
				fprintf(stderr,"\nCPU is TI812x\n");
				ret = 0;
				goto CHECK_CPU_END;
			}
		}

		pStr = strtok(NULL, delima_buf);

	}

CHECK_CPU_END:

	if( fd_cpu >= 0 )
		close(fd_cpu);


	CurrentStatus = ret;

	return ret;
}

/**
* @brief Check the running platform
*
* @return Platfprm check result
* @retval -1  Error
* @retval 0  EVM
* @retval 1  IPNC
*/
int Platform_check( void )
{
	FILE *pfile = NULL;
	char tempbuff[100];
	char filename[]="/proc/version";
	int	 ret = 0;
	char *pStr = NULL;

	pfile = fopen(filename,"r");
	if( pfile == NULL )
	{
		ret = -1;
		goto CHECK_END;
	}

	if( fread(tempbuff, sizeof(tempbuff),1,pfile) <= 0 )
	{
		ret = -1;
		goto CHECK_END;
	}

	tempbuff[sizeof(tempbuff)-1]='\0';

	pStr = strstr(tempbuff,"IPNC");
	if( pStr != NULL )
	{
		ret = 1;
	}
	else {
		ret = 0;
	}

CHECK_END:

	if( pStr )
	{
		fprintf(stderr,"%s \n",pStr);
	}
	if( pfile )
	{
		fclose(pfile);
	}
	return ret;
}

/**
* @brief check device version
*
* @param void
* @return device version
* @retval 0  old version
* @reval 1   new version
* @retval -1  Error
*/
int Check_device_version( void )
{
	int 	fd_proc = 0;
	char	data[5];
	int		result = 0;
	char 	dev_name[] = "/proc/device_version";

	fd_proc = open(dev_name, O_RDONLY);

	if( !fd_proc )
	{
		printf("Check_device_version : open device error !! \n");
		return -1;
	}
	result = read(fd_proc,  data, 1);
	if( result <= 0 )
	{
		printf("Check_device_version : read device error !!\n");
		close(fd_proc);
		return -1;
	}
	close(fd_proc);
	if( data[0] == '1')
	{
		printf("Check_device_version : DEVICE VER = 1 !!\n");
		return 1;
	}else{
		printf("Check_device_version : DEVICE VER = 0 !!\n");
		return 0;
	}

	return -1;
}
