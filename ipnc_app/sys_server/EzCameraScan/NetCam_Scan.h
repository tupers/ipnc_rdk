#ifndef NETCAMERA_SCAN_H
#define NETCAMERA_SCAN_H

#define NETCAMERA_SCAN_PORT			8002

#define SCAN_MAGIC1					0x14
#define SCAN_MAGIC2					0x23


#define 	CMD_GET_CAMERA			7
#define 	CMD_PUT_CAMERA			8

typedef struct _NetCamScanSt
{
	unsigned char magic[2];
	unsigned int  cmd;
	int			  random;
	char ip[32];
}NetCamScanSt;

#endif
