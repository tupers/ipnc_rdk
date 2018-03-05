#ifndef NETCAMERA_SCAN_H
#define NETCAMERA_SCAN_H

#define NETCAMERA_SCAN_PORT			8002

#define SCAN_MAGIC1					0x14
#define SCAN_MAGIC2					0x23

#define IDENTICAL_STRING			"ezcamera"

typedef struct NetCamScanData
{
	unsigned char maigc[2];
	char identical[9];
	unsigned int  CamId; 
	unsigned int crc;
}NetCamScanData;

#endif
