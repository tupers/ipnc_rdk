#ifndef _EZ_FTP_H
#define _EZ_FTP_H
#include "ftp_thr.h"
#include "fileops.h"
#include <sem_util.h>

#define EZFTP_CLIENT_MAX	10

#define FTP_SERVER_PORT		8010	
#define DEFAULT_USERNAME	"root"
#define DEFAULT_PASSWD		"ftpcam"

#define FTP_ROOT			"/tmp/"
#define ROOT_NAME 			"nand"

#define MAX_USERNAME		32
#define MAX_PASSWD			32

#define MAGIC_HEADER1	0x159753
#define MAGIC_HEADER2	0x456183



#define FIRMWARE_C6DSP			"/opt/ipnc/update/ipnc_rdk_fw_c6xdsp.xe674"
#define FIRMWARE_VPSSM3			"/opt/ipnc/update/ipnc_rdk_fw_m3vpss.xem3"
#define FIRMWARE_VIDEOM3		"/opt/ipnc/update/ipnc_rdk_fw_m3video.xem3"
#define EZCAM_EZFTP_PATH		"/opt/ipnc/update/Ezftp"
#define EZCAM_APP_FILE			"/opt/ipnc/update/EzApp"
#define EZCAM_MCFW_PATH			"/opt/ipnc/update/ipnc_rdk_mcfw.out"
#define EZCAM_APP_SERVER		"/opt/ipnc/update/system_server"
#define SDS_TEST_PATH			"/opt/test"


typedef struct _EzFtp_Header
{
	unsigned int magic1;
	unsigned int magic2;
	unsigned int cmd;
	unsigned int len;
	unsigned int pad;
	unsigned int status;
}EzFtp_Header;

#define MAX_CMD_LENGTH	1024
typedef enum _Ftp_Cmd
{
	FTP_MSG_LOGIN = 0,
	FTP_MSG_CLOSE,
	FTP_MSG_LIST,
	FTP_MSG_PUT,
	FTP_MSG_GET
}Ftp_Cmd;

typedef enum _EzFtpAck_Status
{
	FTP_ACK_SOK = 0,
	FTP_ACK_SFAIL
}EzFtpAck_Status;

typedef struct _EzFtp_Ack
{
	int status;
	unsigned int len;
}EzFtp_Ack;

typedef struct _EzFtp_NetMsg
{
	unsigned char cmd;
	int			  ackCmd;
	int			  len;
	char		  buf[0]; 
}EzFtp_NetMsg;

typedef struct _EzFtp_Acount
{
	char UserName[MAX_USERNAME+1];
	char Passwd[MAX_PASSWD+1];	
}EzFtp_Acount;

#define MAX_FILE_PATH	160

typedef struct _EzFtp_File
{
	unsigned int port;
	char ip[20];
	char filePath[MAX_FILE_PATH];
	unsigned int fileSize;
}EzFtp_File;

typedef struct _FTP_MSG_BUF
{
	long 	Des; ///< message type, must be > 0 .
	unsigned int cmd; ///< data 
	unsigned int port;
	char ip[20];
	char filePath[MAX_FILE_PATH];
	int fileSize;
}FTP_MSG_BUF;


typedef struct _EzFtp
{
	int Ftpsd;
	int qid;
	int fqid;
	unsigned clentCount;
	Ftp_ThrHndl hndl;
	SemHandl_t hndlSem;
}EzFtp;

typedef struct _EzFtp_thrParam
{
	int sd;
	int msgtype;
}EzFtp_thrParam;
#endif
