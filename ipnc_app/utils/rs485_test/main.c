/*
 * devmem tools
 * Copyright (c) 2008~forever 
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 *
 * Author : Hsienwu.Huang (hsienwu.huang@appropho.com)
 *
 */
/*
 * 2011/09/27	Change to test RS485 by Y.C.C. (yong-cheh_chen@appropho.com)
 */
 
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <stdarg.h>

#define BAUDRATE        B115200
#define MODEMDEVICE     "/dev/tts/0"
#define RS485_DEV	"/dev/ttyO1"
#define _POSIX_SOURCE   1

#define SET_UART 1

#define N	1

int Uart_Open(char *dev_name)
{
	int fd;
	//int c=0, res;
#if SET_UART
    struct termios  oldtio, newtio;
#endif


    printf("Start...\n");
    fd = open(dev_name, O_RDWR|O_NOCTTY);

    if (fd < 0) {
        perror(dev_name);
        exit(1);
    }

    printf("Open...\n");
#if SET_UART	
    tcgetattr(fd, &oldtio);
    bzero(&newtio, sizeof(newtio));
	
	newtio = oldtio;

	cfsetispeed(&newtio, BAUDRATE);
	cfsetospeed(&newtio, BAUDRATE);
	cfmakeraw(&newtio);
	
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
#else
	system("stty sane 115200 raw -echo -crtscts parodd < /dev/tts/0");
#endif	
	return fd;

}

int Uart_Close(int fd)
{
	close(fd);
    //tcsetattr(fd, TCSANOW, &oldtio);
	return 0;
}

int Uart_Read(int fd, void *pData, int size)
{
	return read(fd, pData, size);
}

int Uart_Write(int fd, void *pData, int size)
{
	return write(fd, pData, size);
}

int sensor_test(int argc, char *argv[])
{
	int fd;
	int	num_base1 = 16;
	int	num_base2 = 16;
	unsigned long phyAddr 	= 0x1c70000;
	unsigned long length	= 0x2000;
	unsigned long SetVal	= 0x2000;
	//unsigned long offset	= 0;
	int	RWStatus = 0;
	char *str;
	char writeBuff[20];
	char readBuff[0x7f*4];
	
	if( argc < 4 )
	{
		printf("usage (read) : %s r <0xAddress> <0xLength> \n",argv[0] );
		printf("      (write): %s w <0xAddress> <0xValue> \n",argv[0] );
		return -1;
	}else{
		
		if( argv[2][0] == '0' && argv[2][1] == 'x' )
		{
			num_base1 = 16;
		}else{
			num_base1 = 10;
		}
		
		if( argv[3][0] == '0' && argv[3][1] == 'x' )
		{
			num_base2 = 16;
		}else{
			num_base2 = 10;
		}

		if( !strcmp(argv[1], "r") )
		{
			phyAddr = strtol(argv[2], &str, num_base1);
			length = strtol(argv[3], &str, num_base2);
			
			/*if( phyAddr%4 )
			{
				printf("\nError : <0xAddress> must aligned 4 !!!\n");
				return -1;
			}*/
			
			//length = (length + 3)/4*4;
			RWStatus = 0;
		}
		else if( !strcmp(argv[1], "w") )
		{
			phyAddr = strtol(argv[2], &str, num_base1);
			SetVal = strtol(argv[3], &str, num_base2);
			
			/*if( phyAddr%4 )
			{
				printf("\nError : <0xAddress> must aligned 4 !!!\n");
				return -1;
			}*/
			
			length = 0x4;
			RWStatus = 1;
		}
		else
		{
			printf("usage (read) : %s r <0xAddress> <0xLength> \n",argv[0] );
			printf("      (write): %s w <0xAddress> <0xValue> \n",argv[0] );
			return -1;
		}
	}
	

	fd = Uart_Open(MODEMDEVICE);
	
	if( RWStatus == 0)
	{
		/*Read*/
		printf("Read Mode phyAddr= 0x%X length = 0x%X\n",(unsigned int )phyAddr,(unsigned int )length);

		int i=0;
		int Rsize = (length & 0x7f);
		int Rget = 0;
		unsigned int rData = 0;
		int loops = 0;
		writeBuff[i] = Rsize; i++;
		
		writeBuff[i] = ((phyAddr & 0xff000000) >> 24); i++;
		writeBuff[i] = ((phyAddr & 0x00ff0000) >> 16); i++;
		writeBuff[i] = ((phyAddr & 0x0000ff00) >> 8); i++;
		writeBuff[i] = (phyAddr & 0x000000ff); i++;
		
		Uart_Write( fd, writeBuff, i);
		tcdrain( fd );
		Rget = Uart_Read(fd, readBuff, Rsize*sizeof(int));
		loops = Rsize*sizeof(int);
		if( Rget != loops )
			printf("warning read %d not match %d\n",Rget,loops);
		
		for( i = 0; i < loops; i+=4 )
		{
			rData = (unsigned int )(((readBuff[i])<<24)|((readBuff[i+1])<<16)|((readBuff[i+2])<<8)|(readBuff[i+3]));
			printf("0x%X = 0x%X \n",(unsigned int )(phyAddr+i/4),rData);
		}
		
	}else{
		/*Write*/
		printf("Write Mode phyAddr= 0x%X SetVal = 0x%X\n",(unsigned int )phyAddr, (unsigned int )SetVal);
		int i=0;
		writeBuff[i] = 0x81; i++;
		
		writeBuff[i] = ((phyAddr & 0xff000000) >> 24); i++;
		writeBuff[i] = ((phyAddr & 0x00ff0000) >> 16); i++;
		writeBuff[i] = ((phyAddr & 0x0000ff00) >> 8); i++;
		writeBuff[i] = (phyAddr & 0x000000ff); i++;
		
		writeBuff[i] = ((SetVal & 0xff000000) >> 24); i++;
		writeBuff[i] = ((SetVal & 0x00ff0000) >> 16); i++;
		writeBuff[i] = ((SetVal & 0x0000ff00) >> 8); i++;
		writeBuff[i] = (SetVal & 0x000000ff); i++;
		
		Uart_Write( fd, writeBuff, i);
		
	}
	
	Uart_Close(fd);
	return 0;
}

int fd_printf(int fd, char * format, ...)
{
  char buffer[256];
  int size, ret;
  va_list args;
  va_start (args, format);
  size = vsprintf (buffer,format, args);
  if(size > 0){
	printf("Sending %s\n", buffer);
	ret = write (fd, buffer, size);
  }
  else
	ret = -1;
  va_end (args);
  tcdrain( fd );
  return ret;
}

int RS485_test()
{
	int fd;
	system("echo 1 > /sys/class/gpio/gpio33/value");
	fd = Uart_Open(RS485_DEV);
	fd_printf(fd, "\r\n *******************************");
	fd_printf(fd, "\r\n  RS-485 TEST");
	fd_printf(fd, "\r\n ******************************* \r\n");
	Uart_Close(fd);
	printf("End of %s\n", __func__);
	return 0;
}

int RS485_read_test()
{
	int fd, i;
	char buffer[256];
	system("echo 0 > /sys/class/gpio/gpio33/value");
	fd = Uart_Open(RS485_DEV);
	printf("\r\nTry to read a charachter from RS485...");
	if(Uart_Read(fd, buffer, N) != N)
		perror("\nRS485 :");
	else{
		printf("\nGot ");
		for(i = 0;i< N;i++)
			printf("0x%02x ", buffer[i]);
		printf("\tfrom RS485\n");
	}
	Uart_Close(fd);
	printf("End of %s\n", __func__);
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc > 1){
		if(strcmp(argv[1], "r") == 0)
			RS485_read_test();
	}else
		RS485_test();
	return 0;
}
