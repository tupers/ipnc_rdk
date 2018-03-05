/* ===========================================================================
* @file file_msg_drv.c
*
* @path $(IPNCPATH)\util
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
#include <share_mem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sem_util.h>
#include <file_msg_drv.h>
#include <system_default.h>

#ifdef DEBUG
#define DBG(fmt, args...)	printf("FileMsgDrv: Debug\n" fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("FileMsgDrv: Error\n" fmt, ##args)
#define SWAP(a, b)  {a ^= b; b ^= a; a ^= b;}
#define ONVIF_SCOPE "www.onvif.org"

static int qid,mid,gProcId, eCount = 0, gKey;
static void *pShareMem;
static SemHandl_t hndlDrvSem = NULL;

int SaveDmvaTZParams();
int LoadDmvaTZParams();
int SaveDmvaIMDParams();
int LoadDmvaIMDParams();
int SaveDmvaSMETAParams();
int LoadDmvaSMETAParams();
int SaveDmvaOCParams();
int LoadDmvaOCParams();
/**
* @brief Save message ID in file message driver

* Save a message ID to file message driver. This ID will be used to identify
*  who is communicating with file manager. This function is old method, please
*  use InitFileMsgDrv(int key,int iProcId) instead.
* @param iqid [I ] message ID
*/
void SaveQid(int iqid)
{
	qid = iqid;
}
/**
* @brief Send quit command to file manager.

* This command will make file manager stop running. After you called this, all
*  the other process can't get system information because file manager is down.
*/
void SendFileQuitCmd()
{
	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = 0;
	msgbuf.cmd = FILE_MSG_QUIT;
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
}

#if 0
static int GetShareMemoryId()
{
	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_GET_MID;
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	return msgbuf.shmid;
}
#endif
/**
* @brief Initialize file message driver.

* Initialize file message driver. Please see \ref FILE_MSG_DRV_HOW to learn more.
* @note This API must be used before use any other file message driver API.
* @param key [I ] Key number for message queue and share memory.
* @param iProcId [I ] Message ID(Which was define in \ref File_Msg_Def.h) to initialize file message driver.
* @retval 0 Success.
* @retval -1 Fail.
*/
int InitFileMsgDrv(int key,int iProcId)
{
	if(hndlDrvSem == NULL)
		hndlDrvSem = MakeSem();
	if(hndlDrvSem == NULL){
		ERR("Semaphore create fail!!!\n");
		return -1;
	}
	DBG("Semaphore Addr: %p\n", hndlDrvSem);
	if((qid = Msg_Init(key)) < 0){
		ERR("Message init fail\n");
		DestroySem(hndlDrvSem);
		return -1;
	}
	DBG("Qid: %d\n",qid);
	gKey = key;
	mid = pShareMemInit(key,PROC_MEM_SIZE * MAX_SHARE_PROC);
	if(mid < 0){
		ERR("Share memory id error\n");
		DestroySem(hndlDrvSem);
		return -1;
	}
	DBG("Share Memory id : %d\n", mid);
	pShareMem = shmat(mid,0,0);
	DBG("Share Memory Address:%p\n", pShareMem);
	gProcId = iProcId;
	return 0;
}
/**
* @brief Cleanup file message driver.

* This API shoud be called at the end of process.
*/
void CleanupFileMsgDrv()
{
	shmdt(pShareMem);
	DestroySem(hndlDrvSem);
}
/**
* @brief Get system information.

* Get system information from file manager. In order to make every change can
*  update quickly, this function return the pointer to share memory.
* @warning Do NOT modify where this pointer point to.
* @return Pointer to where system information was stored.
* @retval NULL Can't get system information.
*/
SysInfo *GetSysInfo()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return NULL;
	}
	return (SysInfo *) pShareMem;
}

Alg_Params *GetAlgPrmInfo()
{
	SysInfo *pSysInfo;
	pSysInfo = (SysInfo *)pShareMem;
	return &(pSysInfo->algPrm);
}
#if 0
/**
* @brief Get system log information.

* @param nPageNum [I ] Page number
* @param nItemNum [I ] Item number
* @param pBuf [O ] pointer to where log information stored.
* @retval 0 Success.
* @retval -1 Fail
*/
int GetSysLog(int nPageNum, int nItemNum, LogEntry_t * pBuf)
{
	FILE_MSG_BUF msgbuf;
	SysInfo *pSysInfo = (SysInfo *) pShareMem;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ;
	msgbuf.shmid = mid;
	msgbuf.shmidx = 1;
	msgbuf.nPageNum = nPageNum;
	msgbuf.nItemNum = nItemNum;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	memcpy(pBuf, &pSysInfo->tCurLog, sizeof(LogEntry_t));
	return 0;
}
#endif
/**
* @brief Save system information in file manager.
* @param nShmIndex [I ] Share memory index (0 or 1). 0 is used for system settings;
 and 1 is used for system log.
* @retval 0 Success.
* @retval -1 Fail
*/
static int SetSysInfo(int nShmIndex)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE;
	msgbuf.shmid = mid;
	msgbuf.shmidx = nShmIndex;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}
/**
* @brief Save system information in file manager immediately
* @retval 0 Success.
* @retval -1 Fail
*/
static int SetSysInfoImmediately(int nShmIndex)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_I;
	msgbuf.shmid = mid;
	msgbuf.shmidx = nShmIndex;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

static int SetSysNetwork(int nShmIndex)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_NETWORK;
	msgbuf.shmid = mid;
	msgbuf.shmidx = nShmIndex;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fUpdateAlgPrmFile()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_UPDATE_ALGFILE;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}
int fSetNetwork()
{
	return SetSysNetwork(0);

}
int fUpdateDefaultCfg()
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	return SetSysInfoImmediately(0);	
}
#if 0
/**
* @brief Save netmask in file manager

* The netmask here will be used in static IP mode.
* @param netmask [I ] New netmask in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetNetMask(struct in_addr netmask)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.netmask, &netmask, sizeof(netmask));
	return SetSysInfo(0);
}
/**
* @brief Save IP in file manager

* The IP here will be used in static IP mode.
* @param ip [I ] New IP in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetIp(struct in_addr ip)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.ip, &ip, sizeof(ip));
	return SetSysInfoImmediately(0);
}
/**
* @brief Save gateway in file manager
* @param gateway [I ] New gateway in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGateway(struct in_addr gateway)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.gateway, &gateway, sizeof(gateway));
	return SetSysInfo(0);
}
/**
* @brief Save DNS ip in file manager.

* DNS IP here will be used under static IP mode.
* @param ip [I ] DNS ip.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDns(struct in_addr ip)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.dns, &ip, sizeof(ip));
	return SetSysInfo(0);
}
#else
/**
* @brief Save netmask in file manager

* The netmask here will be used in static IP mode.
* @param netmask [I ] New netmask in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetNetMask(char *netmask)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(pSysInfo->net.net_config.netmask, netmask, strlen(netmask) + 1);
	return SetSysInfo(0);
}
/**
* @brief Save IP in file manager

* The IP here will be used in static IP mode.
* @param ip [I ] New IP in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetIp(char* ip)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(pSysInfo->net.net_config.ip, ip, strlen(ip) + 1);
	return SetSysInfoImmediately(0);
}
/**
* @brief Save gateway in file manager
* @param gateway [I ] New gateway in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGateway(char* gateway)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(pSysInfo->net.net_config.gateway, gateway, strlen(gateway)+1);
	return SetSysInfo(0);
}
/**
* @brief Save DNS ip in file manager.

* DNS IP here will be used under static IP mode.
* @param ip [I ] DNS ip.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDns(char* ip)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(pSysInfo->net.net_config.dns, ip, strlen(ip)+1);
	return SetSysInfo(0);
}
#endif
/**
* @brief Set http port in file manager

* Http port of web site will be change when boa restart.
* @param port [I ] New http port.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetHttpPort(unsigned short port)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.http_port, &port, sizeof(port));
	return SetSysInfo(0);
}
#if 0
/**
* @brief Save IPCAM title in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetTitle(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->lan_config.title) < len+1)
		return -1;
	memcpy(pSysInfo->lan_config.title, buf, len);
	pSysInfo->lan_config.title[len] = '\0';
	return SetSysInfo(0);
}
#endif

/**
* @brief Save DHCP status in file manager
* @param enable [I ] 0 meets DHCP mode, otherwise static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDhcpEnable(int enable)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.dhcp_enable, &enable, sizeof(enable));
	return SetSysInfo(0);
}
/**
* @brief Save FTP FQDN in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpFqdn(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->net.ftp_config.servier_ip) < len + 1)
		return -1;
	memcpy(&pSysInfo->net.ftp_config.servier_ip, buf, len);
	pSysInfo->net.ftp_config.servier_ip[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save FTP user name in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpUsername(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->net.ftp_config.username) < len + 1)
		return -1;
	memcpy(pSysInfo->net.ftp_config.username, buf, len);
	pSysInfo->net.ftp_config.username[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save FTP user password in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpPassword(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->net.ftp_config.password) < len + 1)
		return -1;
	memcpy(pSysInfo->net.ftp_config.password, buf, len);
	pSysInfo->net.ftp_config.password[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save FTP folder in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpFoldname(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->net.ftp_config.foldername) < len + 1)
		return -1;
	memcpy(pSysInfo->net.ftp_config.foldername, buf, len);
	pSysInfo->net.ftp_config.foldername[len] = '\0';
	return SetSysInfo(0);
}
#if 0
/**
* @brief Save FTP image acount in file manager

* This value controls that how many JPEG image will be sent in one FTP
*  connection.
* @param imageacount [I ] JPEG image count in one FTP connection.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpImageacount(int imageacount)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.ftp_config.image_acount, &imageacount, sizeof(imageacount));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save FTP port number in file manager
* @param port [I ] New FTP port.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpPort(unsigned short port)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.ftp_config.port, &port, sizeof(port));
	return SetSysInfo(0);
}
/**
* @brief Save FTP client process ID in file manager
* @param pid [I ] FTP client process ID.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpPid(int pid)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.ftp_config.pid, &pid, sizeof(pid));
	return SetSysInfo(0);
}

#if 0
/**
* @brief SaveSMTP file format in file manager.
* @param value [I ] 0:MPEG4 , 1:JPEG.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAttFileFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.attfileformat, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetASmtpAttach(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.asmtpattach, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save SMTP FQDN in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpServerIp(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.servier_ip) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.servier_ip, buf, len);
	pSysInfo->smtp_config.servier_ip[len] = '\0';
	return SetSysInfo(0);
}

/**
* @brief Save SMTP port number in file manager
* @param port [I ] New SMTP port.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpServerPort(unsigned short port)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.server_port, &port, sizeof(port));
	return SetSysInfo(0);
}

/**
* @brief Save SMTP Authentication in file manager.

* Determind whether use SMTP authentication to send e-mail or not.
* @param authentication [I ] SMTP authentication.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpAuthentication(unsigned char authentication)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.authentication, &authentication,
		sizeof(authentication));
	return SetSysInfo(0);
}
/**
* @brief Save SMTP user name in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpUsername(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.username) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.username, buf, len);
	pSysInfo->smtp_config.username[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save SMTP user password in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpPassword(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.password) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.password, buf, len);
	pSysInfo->smtp_config.password[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save SMTP sender e-mail address in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpSenderEmail(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.sender_email) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.sender_email, buf, len);
	pSysInfo->smtp_config.sender_email[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save SMTP reveiver e-mail address in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpReceiverEmail(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.receiver_email) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.receiver_email, buf, len);
	pSysInfo->smtp_config.receiver_email[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save e-mail CC in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpCC(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.CC) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.CC, buf, len);
	pSysInfo->smtp_config.CC[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save e-mail subject in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpSubject(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.subject) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.subject, buf, len);
	pSysInfo->smtp_config.subject[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save e-mail contant text in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpText(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.text) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.text, buf, len);
	pSysInfo->smtp_config.text[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save e-mail attachments in file manager.
* @param attachments [I ] Count of JPEG images in one e-mail.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpAttachments(unsigned char attachments)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.attachments, &attachments,
		sizeof(attachments));
	return SetSysInfo(0);
}
/**
* @brief Save e-mail view in file manager.
* @param view [I ] 0 meets text style, otherwise html style.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpView(unsigned char view)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.view, &view, sizeof(view));
	return SetSysInfo(0);
}
#endif

/**
* @brief Save SNTP FQDN in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSntpServer(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->net.net_config.ntp_server) < len + 1)
		return -1;
	memcpy(pSysInfo->net.net_config.ntp_server, buf, len);
	pSysInfo->net.net_config.ntp_server[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save camera day/night mode in file manager.
* @param value [I ] Day/night mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamDayNight(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nDayNight, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera white balance mode in file manager.
* @param value [I ] White balance mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamWhiteBalance(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nWhiteBalance, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera backlight mode in file manager.
* @param value [I ] Backlight mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamBacklight(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nBackLight, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera brightness in file manager.
* @param value [I ] Brightness.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamBrightness(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nBrightness, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera contrast in file manager.
* @param value [I ] Contrast.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamContrast(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nContrast, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera saturation in file manager.
* @param value [I ] Saturation.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamSaturation(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nSaturation, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera sharpness in file manager.
* @param value [I ] Sharpness.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamSharpness(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nSharpness, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save JPEG encode quality in file manager.
* @param value [I ] JPEG quality.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetJpegQuality(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.njpegquality, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMirror(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.mirror, &value, sizeof(value));
	return SetSysInfo(0);
}
#if 0
int fSetFaceDetect(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.fdetect, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDX(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.startX, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDY(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.startY, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDW(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDH(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDConfLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.fdconflevel, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDDirection(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.fddirection, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetFRecognition(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.frecog, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFRConfLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.frconflevel, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFRDatabase(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.frdatabase, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetPrivacyMask(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.pmask, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMaskOptions(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.maskoption, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetPowerMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.powerMode, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDemoCfg(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.democfg, &value, sizeof(value));
	return SetSysInfo(0);
}

void fResetFDROIParam(int stream, int width, int height)
{
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	if(stream == 0) {
		pSysInfo->face_config.startX 		= (width*pSysInfo->face_config.startX)/pSysInfo->face_config.prevwidth;
		pSysInfo->face_config.startY 		= (height*pSysInfo->face_config.startY)/pSysInfo->face_config.prevheight;
		pSysInfo->face_config.width  		= (width*pSysInfo->face_config.width)/pSysInfo->face_config.prevwidth;
		pSysInfo->face_config.height 		= (height*pSysInfo->face_config.height)/pSysInfo->face_config.prevheight;
		pSysInfo->face_config.prevwidth  	= (width<=0)?1:width;
		pSysInfo->face_config.prevheight 	= (height<=0)?1:height;
	}

	return;
}
#endif
int fSetHistogram(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.histogram, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVstabValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.advanceMode.vsEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetLdcValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.advanceMode.ldcEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVnfValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.advanceMode.vnfEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVnfMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.advanceMode.vnfMode, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVnfStrength(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.advanceMode.vnfStrength, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDynRange(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.advanceMode.dynRange, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDynRangeStrength(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.advanceMode.dynRangeStrength, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save video codec mode in file manager.

* Video codec mode can be single codec, dual codec, triple codec.
* @param value [I ] Video codec mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetVideoMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nStreamType, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVideoCodecMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nVideocodecmode, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVideoCodecCombo(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nVideocombo, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVideoCodecRes(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nVideocodecres, &value, sizeof(value));
	return SetSysInfo(0);
}
#if 0
/**
* @brief Save daylight time setting in file manager.
* @param value [I ] Use daylight or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDaylightTime(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.daylight_time, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save time zone in file manager.
* @param value [I ] Time zone.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetTimeZone(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.ntp_timezone, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save stream active status in file manager.
* @param stream1 [I ] Active or not.
* @param stream2 [I ] Active or not.
* @param stream3 [I ] Active or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetStreamActive(unsigned char stream1, unsigned char stream2, unsigned char stream3,
                     unsigned char stream4, unsigned char stream5, unsigned char stream6)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.Supportstream1, &stream1, sizeof(stream1));
	memcpy(&pSysInfo->codec_config.Supportstream2, &stream2, sizeof(stream2));
	memcpy(&pSysInfo->codec_config.Supportstream3, &stream3, sizeof(stream3));
	memcpy(&pSysInfo->codec_config.Supportstream4, &stream4, sizeof(stream4));
	memcpy(&pSysInfo->codec_config.Supportstream5, &stream5, sizeof(stream5));
	memcpy(&pSysInfo->codec_config.Supportstream6, &stream6, sizeof(stream6));
	return SetSysInfo(0);
}
/**
* @brief Save image size in file manager.
* @param stream [I ] 0: JPEG stream. 1: MPEG1 stream. 2: MPEG2 stream.
* @param Xvalue [I ] Image width.
* @param Yvalue [I ] Image height.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetVideoSize(unsigned char stream, int Xvalue, int Yvalue)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if (stream == 1 || stream == 4) {
		memcpy(&pSysInfo->codec_config.JpegXsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->codec_config.JpegYsize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 2) {
		memcpy(&pSysInfo->codec_config.Mpeg41Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->codec_config.Mpeg41Ysize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 3) {
		memcpy(&pSysInfo->codec_config.Mpeg42Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->codec_config.Mpeg42Ysize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 5) {
		memcpy(&pSysInfo->codec_config.Avc1Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->codec_config.Avc1Ysize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 6) {
		memcpy(&pSysInfo->codec_config.Avc2Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->codec_config.Avc2Ysize, &Yvalue, sizeof(Yvalue));
	}

	return SetSysInfo(0);
}

int fSetStreamConfig(unsigned char stream, int width, int height, char *name, int portnum)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	fResetFDROIParam(stream, width, height);
	fResetCodecROIParam(stream, width, height);

	pSysInfo->stream_config[stream].width	= width;
	pSysInfo->stream_config[stream].height  = height;

	strcpy((char *)pSysInfo->stream_config[stream].name, name);

	pSysInfo->stream_config[stream].portnum  = portnum;

//	if((portnum==H264_1_PORTNUM) || (portnum==H264_2_PORTNUM))
	 if((portnum==pSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[0]) || (portnum==pSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[1]))
		strcpy((char *)pSysInfo->stream_config[stream].portname, "PSIA/Streaming/channels/2?videoCodecType=H.264");  //h264   PSIA/Streaming/channels/2?videoCodecType=H.264
//	else if((portnum==MPEG4_1_PORTNUM) || (portnum==MPEG4_2_PORTNUM))
	else if((portnum==pSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[2]) || (portnum==pSysInfo->RtspServerPort.rtspServerPort_H264_MPEG4_MJPEG[3]))
		strcpy((char *)pSysInfo->stream_config[stream].portname, "PSIA/Streaming/channels/1?videoCodecType=MPEG4");  //mpeg4   PSIA/Streaming/channels/1?videoCodecType=MPEG4
	else
		strcpy((char *)pSysInfo->stream_config[stream].portname, "PSIA/Streaming/channels/0?videoCodecType=MJPEG");  //mjpeg   PSIA/Streaming/channels/0?videoCodecType=MJPEG

	return SetSysInfo(0);
}

void fResetCodecROIParam(int stream, int width, int height)
{
	int i=0;

    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	for(i=0;i<pSysInfo->codec_roiconfig[stream].numROI;i++) {
		pSysInfo->codec_roiconfig[stream].roi[i].startx = (width*pSysInfo->codec_roiconfig[stream].roi[i].startx)/pSysInfo->codec_roiconfig[stream].prevwidth;
		pSysInfo->codec_roiconfig[stream].roi[i].starty = (height*pSysInfo->codec_roiconfig[stream].roi[i].starty)/pSysInfo->codec_roiconfig[stream].prevheight;
		pSysInfo->codec_roiconfig[stream].roi[i].width 	= (width*pSysInfo->codec_roiconfig[stream].roi[i].width)/pSysInfo->codec_roiconfig[stream].prevwidth;
		pSysInfo->codec_roiconfig[stream].roi[i].height = (height*pSysInfo->codec_roiconfig[stream].roi[i].height)/pSysInfo->codec_roiconfig[stream].prevheight;
	}

	pSysInfo->codec_roiconfig[stream].prevwidth 	= (width<=0)?1:width;
	pSysInfo->codec_roiconfig[stream].prevheight 	= (height<=0)?1:height;

	return;
}

/**
* @brief Save TV out clock config in file manager
* @param value [I] NTSC or PAL mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetImageSource(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.imagesource, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save MPEG1 bitrate in file manager.
* @param value [I ] Bitrate.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMP41bitrate(int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nMpeg41bitrate, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save MPEG2 bitrate in file manager.
* @param value [I ] Bitrate.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMP42bitrate(int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nMpeg42bitrate, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save encode frame rate in file manager.
* @param stream [I ] Which stream.
* @param value [I ] Frame rate.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFramerate1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->codec_config.nFrameRate1, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerate2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->codec_config.nFrameRate2, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerate3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->codec_config.nFrameRate3, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerateVal1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.frameRateVal1, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerateVal2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.frameRateVal2, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerateVal3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.frameRateVal3, &value, sizeof(value));

	return SetSysInfo(0);
}
#if 0
/**
* @brief Save user schedule in file manager.

* Number of schedules was define as \ref SCHDULE_NUM . User can modify the
	front 7 schedules by web site.
* @param index [I ] User schedule index.
* @param pSchedule [I ] User schedule.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSchedule(int index,Schedule_t* pSchedule, int day, int year)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.aSchedules[index], pSchedule, sizeof(Schedule_t));
	memcpy(&pSysInfo->lan_config.schedCurDay, &day, sizeof(day));
	memcpy(&pSysInfo->lan_config.schedCurYear, &year, sizeof(year));

	return SetSysInfo(0);
}

/**
* @brief Save Ethernet lost status in file manager.
* @param value [I ] Ethernet lost or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetLostAlarm(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.lostalarm, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save SD card alarm enable status in file manager.
* @param value [I ] Enable alarm save to SD card or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.bSdaEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save FTP alarm enable status in file manager.
* @param value [I ] Enable alarm save to FTP or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFTPAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.bAFtpEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save SMTP alarm enable status in file manager.
* @param value [I ] Enable alarm save to SMTP or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSMTPAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.bASmtpEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save alarm duration in file manager.
* @param value [I ] New alarm duration.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAlarmDuration(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmDuration, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save AVI duration in file manager.
* @param value [I ] New AVI duration.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAVIDuration(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.aviduration, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save AVI format in file manager.
* @param value [I ] New AVI format.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAVIFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.aviformat, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save FTP file format in file manager.
* @param value [I ] 0:MPEG4 , 1:JPEG.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFTPFileFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.ftp_config.ftpfileformat, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save record to FTP status in file manager.
* @param value [I ] Enable record to FTP or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetRftpenable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.ftp_config.rftpenable, &value, sizeof(value));
	return SetSysInfo(0);
}

#if 0
/**
* @brief Save SD file format in file manager.
* @param value [I ] 0:MPEG4 , 1:JPEG.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDFileFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdfileformat, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save SD insert status in file manager.
* @param value [I ] SD insert status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDInsert(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdinsert, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save record to SD card status in file manager.
* @param value [I ] Enable record to SD card or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSdReEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdrenable, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save 2A mode in file manager.
* @param value [I ] New 2A mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetImageAEW(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAEWswitch, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save 2A mode in file manager.
* @param value [I ] New 2A mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetImageAEWType(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAEWtype, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save binning mode in file manager.
* @param value [I ] New binning mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetBinning(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(pSysInfo->lan_config.nBinning == value)
		return 0;
	memcpy(&pSysInfo->lan_config.nBinning, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save backlight control status in file manager.
* @param value [I ] New backlight control mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetBlc(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nBacklightControl, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save frame control status in file manager.
* @param value [I ] New frame control mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFrameCtrl(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.framectrl, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save user acount in file manager.
* @param index [I ] User acount index.
* @param acout [I ] User acount data.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAcount(int index, Acount_t *acout)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->acounts[index], acout, sizeof(Acount_t));
	return SetSysInfoImmediately(0);
}
#if 0
/**
* @brief Save motion detect enable status in file manager.
* @param value [I ] Enable motion detection or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motionenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save motion detect choice status in file manager.
* @param value [I ] New motion detect choice status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionCEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motioncenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save motion detect level status in file manager.
* @param value [I ] New motion detect level status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motionlevel, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save motion detect user define value in file manager.
* @param value [I ] New motion detect user define value.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionCValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motioncvalue, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save motion block in file manager.
* @param pValue [I ] New motion block value.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionBlock(char *pValue,int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(MOTION_BLK_LEN < len)
		return -1;
	memcpy(&pSysInfo->motion_config.motionblock, pValue, len);
	return SetSysInfo(0);
}

/**
* @brief Save GIO input alarm status in file manager.
* @param value [I ] Enable GIO input alarm or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOInEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.gioinenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save GIO input triger status in file manager.
* @param value [I ] High or low triger.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOInType(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.giointype, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save GIO output alarm status in file manager.
* @param value [I ] Enable alarm GIO output or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOOutEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.giooutenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save GIO outputput status in file manager.
* @param value [I ] output high or low.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOOutType(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.gioouttype, &value, sizeof(value));
	return SetSysInfo(0);
}


/**
* @brief Save time stamp format in file manager.
* @param value [I ] Time stamp format.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDateFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.dateformat, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetTStampFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.tstampformat, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDatePosition(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.dateposition, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetTimePosition(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.timeposition, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif

/**
* @brief Reset system to default.
* @retval 0 Success.
* @retval -1 Fail
*/
int fResetSysInfo()
{
	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_RESET;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("FILE_MSG_RESET fail\n");
		return -1;
	}
	return 0;
}
/**
* @brief Save a system log in file manager.
* @param value [I ] System log.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSysLog(LogEntry_t* value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL || value == NULL)
		return -1;
	memcpy(&pSysInfo->tCurLog, value, sizeof(LogEntry_t));
	DBG("LOG DEBUG MSG: event: %s\n", pSysInfo->tCurLog.event);
	DBG("LOG DEBUG MSG: time: %s\n", asctime(&pSysInfo->tCurLog.time));
	return SetSysInfo(1);
}
#if 0
/**
* @brief Update alarm status in file manager.

* This function will change alarm status in file manager.
* @param value [I ] New alarm status
* @retval -1 Fail to update.
* @retval 0 Update successfully.
*/
int fSetAlarmStatus(unsigned short value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.alarmstatus, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save .
* @param value [I ] .
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetNetMulticast(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if (value == pSysInfo->net.rtsp_config.multicast_enable)
		return 0;
	memcpy(&pSysInfo->net.rtsp_config.multicast_enable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetHttpsPort(unsigned short value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.https_port, &value, sizeof(value));
	return SetSysInfo(0);
}
#if 0
int fSetRS485Port(unsigned short value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.rs485config, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
int fSetRateControl1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nRateControl1, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetRateControl2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.nRateControl2, &value, sizeof(value));
	return SetSysInfo(0);
}

#if 0
int fSetDateStampEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].dstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDateStampEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].dstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDateStampEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].dstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTimeStampEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].tstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTimeStampEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].tstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTimeStampEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].tstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nLogoEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nLogoEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nLogoEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoPosition1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nLogoPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoPosition2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nLogoPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoPosition3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nLogoPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextPosition1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nTextPosition, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetTextPosition2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nTextPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextPosition3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nTextPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nTextEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nTextEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nTextEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetOverlayText1(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->osd_config[0].overlaytext, buf, len);
	pSysInfo->osd_config[0].overlaytext[len] = '\0';

	return SetSysInfo(0);
}

int fSetOverlayText2(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->osd_config[1].overlaytext, buf, len);
	pSysInfo->osd_config[1].overlaytext[len] = '\0';

	return SetSysInfo(0);
}

int fSetOverlayText3(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->osd_config[2].overlaytext, buf, len);
	pSysInfo->osd_config[2].overlaytext[len] = '\0';

	return SetSysInfo(0);
}
int fSetDetailInfo1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nDetailInfo, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDetailInfo2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nDetailInfo, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDetailInfo3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nDetailInfo, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetEncryption(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.encryptEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
 int fSetIpratio1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[0].ipRatio, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetIpratio2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[1].ipRatio, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetIpratio3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[2].ipRatio, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetForceIframe1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[0].fIframe, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetForceIframe2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[1].fIframe, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetForceIframe3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[2].fIframe, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPInit1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[0].qpInit, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPInit2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[1].qpInit, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPInit3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[2].qpInit, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPMin1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[0].qpMin, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPMin2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[1].qpMin, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPMin3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[2].qpMin, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetQPMax1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[0].qpMax, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetQPMax2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[1].qpMax, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetQPMax3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[2].qpMax, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMEConfig1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[0].meConfig, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMEConfig2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[1].meConfig, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMEConfig3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[2].meConfig, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetPacketSize1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[0].packetSize, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetPacketSize2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[1].packetSize, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetPacketSize3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_config.codec_advconfig[2].packetSize, &value, sizeof(value));
	return SetSysInfo(0);
}
#if 0
int fSetROIEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].numROI, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetROIEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].numROI, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetROIEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].numROI, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetStr1X1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1Y1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1W1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1H1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1X2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1Y2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1W2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1H2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1X3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1Y3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1W3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1H3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2X1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2Y1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2W1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2H1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2X2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2Y2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2W2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2H2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2X3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2Y3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2W3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2H3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3X1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3Y1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3W1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3H1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3X2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3Y2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3W2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3H2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3X3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3Y3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3W3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3H3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].height, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
int fSetExpPriority(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.expPriority, &value, sizeof(value));
	return SetSysInfo(0);
}
#if 0
int fSetClickSnapFilename(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->lan_config.nClickSnapFilename, buf, len);
	pSysInfo->lan_config.nClickSnapFilename[len] = '\0';

	return SetSysInfo(0);
}

int fSetClickSnapStorage(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nClickSnapStorage, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetExtAlarm(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nExtAlarm, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAlarmAudioPlay(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmAudioPlay, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAlarmAudioFile(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmAudioFile, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetScheduleRepeatEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nScheduleRepeatEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetScheduleNumWeeks(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nScheduleNumWeeks, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetScheduleInfiniteEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nScheduleInfiniteEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetAlarmStorage(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.alarmlocal, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetRecordStorage(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.recordlocal, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetChipConfig(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.chipConfig, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/* System and Access log clear */
int fSetClearSysLog(unsigned char value)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_CLEAR_SYSLOG;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fSetClearAccessLog(unsigned char value)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_CLEAR_ACCESSLOG;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}
#if 0
/**
* @brief Save the duty value of PWM1
*/
int fSetPWM1_DutyVal(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nPWM1_DutyVal, &value, sizeof(value));
	return SetSysInfo(0);
}


/* Onvif */
int fsetimaging(Imaging_Conf_tmp *imaging)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->imaging_conf[imaging->position], &imaging->imaging_conf_in, sizeof(imaging->imaging_conf_in));

	return SetSysInfo(0);
}

int fsetmediaprofile(int index,media_profile *profile)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[index], profile, sizeof(media_profile));
	pSysInfo->nprofile++;

	return SetSysInfo(0);
}
int faddvideosourceconfiguration(int index,video_source_conf *Vsource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
	      return -1;
	memcpy(&pSysInfo->Oprofile[Vsource->position].AVSC, &Vsource->video_source_conf_in_t, sizeof(Vsource->video_source_conf_in_t));
	return SetSysInfo(0);

}
int fsetvideosourceconfiguration(video_source_conf *SetVsource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[SetVsource->position].AVSC, &SetVsource->video_source_conf_in_t, sizeof(SetVsource->video_source_conf_in_t));
	return SetSysInfo(0);
}
int faddaudiosourceconfiguration(Add_AudioSource_Configuration_tmp *addaudiosource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[addaudiosource->position].AASC.Atoken, &addaudiosource->Add_AudioSource_in.Atoken_t, sizeof(addaudiosource->Add_AudioSource_in.Atoken_t));
	return SetSysInfo(0);
}
int fsetaudiosourceconfiguration(Add_AudioSource_Configuration_tmp *addaudiosource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[addaudiosource->position].AASC, &addaudiosource->Add_AudioSource_in, sizeof(addaudiosource->Add_AudioSource_in));
	return SetSysInfo(0);
}
int faddmetadataconfiguration(Metadata_configuration_tmp *metadata)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
	      return -1;
	memcpy(&pSysInfo->Oprofile[metadata->position].MDC, &metadata->MDC_in, sizeof(metadata->MDC_in));
	return SetSysInfo(0);

}
int fsetmetadata(Metadata_configuration_tmp *metadata)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[metadata->position].MDC, &metadata->MDC_in, sizeof(metadata->MDC_in));
	return SetSysInfo(0);
}
int fdelmetadataconfiguration(Metadata_configuration_tmp *metadata)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
	      return -1;
	memset(&pSysInfo->Oprofile[metadata->position].MDC, 0, sizeof(pSysInfo->Oprofile[metadata->position].MDC));
	return SetSysInfo(0);
}
int faddaudioencoderconfiguration(Add_AudioEncoder_Configuration_tmp *addaudioencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
    	int i;
	if(pSysInfo == NULL)
		return -1;
	for(i = 0; i < addaudioencoder->position; i++)
	{
		if(strcmp((char *)(addaudioencoder->Add_AudioEncoder_in.AEtoken_t), (char *)(pSysInfo->Oprofile[i].AAEC.AEtoken)) == 0)
		{
			if(pSysInfo->Oprofile[addaudioencoder->position].AAEC.AEusercount == 0)
				pSysInfo->Oprofile[i].AAEC.AEusercount += 1;
			break;
		}
	}
	memcpy(&pSysInfo->Oprofile[addaudioencoder->position].AAEC, &addaudioencoder->Add_AudioEncoder_in, sizeof(addaudioencoder->Add_AudioEncoder_in));
	return SetSysInfo(0);
}
int fsetaudioencoderconfiguration(Add_AudioEncoder_Configuration_tmp *addaudioencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[addaudioencoder->position].AAEC, &addaudioencoder->Add_AudioEncoder_in, sizeof(addaudioencoder->Add_AudioEncoder_in));
	return SetSysInfo(0);
}
int faddvideoencoderconfiguration(video_encoder_conf *Vencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
	      return -1;
	memcpy(&pSysInfo->Oprofile[Vencoder->position].AESC, &Vencoder->video_encoder_conf_in_t, sizeof(Vencoder->video_encoder_conf_in_t));
	return SetSysInfo(0);
}

int fsetvideoencoderconfiguration(video_encoder_conf *SetVencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[SetVencoder->position].AESC, &SetVencoder->video_encoder_conf_in_t, sizeof(SetVencoder->video_encoder_conf_in_t));
	return SetSysInfo(0);
}

int fdelvideoencoderconfiguration(video_encoder_conf *delVencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memset(&pSysInfo->Oprofile[delVencoder->position].AESC, 0, sizeof(pSysInfo->Oprofile[delVencoder->position].AESC));
	return SetSysInfo(0);
}
int fdelprofile(int index)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char Vtoken[MAX_CONF_TOKEN], Atoken[MAX_LENGTH], VEtoken[MAX_CONF_TOKEN], AEtoken[MAX_LENGTH],
		VAtoken[MAX_CONF_TOKEN], VOtoken[MAX_CONF_TOKEN], MDtoken[MAX_LENGTH];

	if(pSysInfo == NULL)
	   return -1;

	strcpy(Vtoken, pSysInfo->Oprofile[index].AVSC.Vtoken);
	strcpy(Atoken, pSysInfo->Oprofile[index].AASC.Atoken);
	strcpy(VEtoken, pSysInfo->Oprofile[index].AESC.VEtoken);
	strcpy(AEtoken, pSysInfo->Oprofile[index].AAEC.AEtoken);
	strcpy(VAtoken, pSysInfo->Oprofile[index].AVAC.VAtoken);
	strcpy(VOtoken, pSysInfo->Oprofile[index].AVOC.token);
	strcpy(MDtoken, pSysInfo->Oprofile[index].MDC.MDtoken);

	int i = 0;
	if(strcmp(AEtoken,pSysInfo->Oprofile[i].AAEC.AEtoken) == 0) {
		for( i = 0; i < index; i++) {
			if(pSysInfo->Oprofile[index].AAEC.AEusercount > 0) {
				pSysInfo->Oprofile[index].AAEC.AEusercount = 0;
				pSysInfo->Oprofile[i].AAEC.AEusercount -= 1;
				break;
			}
		}
	}
	if(index == pSysInfo->nprofile)
	{
		memset(&pSysInfo->Oprofile[index], 0, sizeof(pSysInfo->Oprofile[index]));
	}
	else if(index != 0)
	{
		memcpy(&pSysInfo->Oprofile[index], &pSysInfo->Oprofile[pSysInfo->nprofile - 1], sizeof(pSysInfo->Oprofile[index]));
		memset(&pSysInfo->Oprofile[pSysInfo->nprofile - 1], 0, sizeof(pSysInfo->Oprofile[index]));
	}

	pSysInfo->nprofile--;
	for(i = 0; i < pSysInfo->nprofile; i ++)
	{
		if(strcmp(Vtoken,"") !=0 && strcmp(Vtoken,pSysInfo->Oprofile[i].AVSC.Vtoken) == 0) {
			pSysInfo->Oprofile[i].AVSC.Vcount -= 1;
		}
		if(strcmp(Atoken,"") !=0 && strcmp(Vtoken,pSysInfo->Oprofile[i].AASC.Atoken) == 0) {
			pSysInfo->Oprofile[i].AASC.Ausecount -= 1;
		}
		if(strcmp(VEtoken,"") !=0 && strcmp(Vtoken,pSysInfo->Oprofile[i].AESC.VEtoken) == 0) {
			pSysInfo->Oprofile[i].AESC.VEusercount -= 1;
		}
		/*if(strcmp(AEtoken,"") !=0 && strcmp(Vtoken,pSysInfo->Oprofile[i].AAEC.AEtoken) == 0) {
			pSysInfo->Oprofile[i].AAEC.AEusercount -= 1;
		}*/
		if(strcmp(VAtoken,"") !=0 && strcmp(Vtoken,pSysInfo->Oprofile[i].AVAC.VAtoken) == 0) {
			pSysInfo->Oprofile[i].AVAC.VAusercount -= 1;
		}
		if(strcmp(VOtoken,"") !=0 && strcmp(Vtoken,pSysInfo->Oprofile[i].AVOC.token) == 0) {
			pSysInfo->Oprofile[i].AVOC.usecount -= 1;
		}
		if(strcmp(MDtoken,"") !=0 && strcmp(Vtoken,pSysInfo->Oprofile[i].MDC.MDtoken) == 0) {
			pSysInfo->Oprofile[i].MDC.MDusecount -= 1;
		}
	}
	return SetSysInfo(0);
}

int fremoveaudiosource(int index)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
	   return -1;
	memset(&pSysInfo->Oprofile[index].AASC, 0, sizeof(pSysInfo->Oprofile[index].AASC));

	return SetSysInfo(0);
}

int fremoveaudioencoder(int index)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	int i;
	if(pSysInfo == NULL)
		return -1;
	if(pSysInfo->Oprofile[index].AAEC.AEusercount > 0)
	{
		for(i = 0; i < index; i++)
		{
			if(strcmp(pSysInfo->Oprofile[index].AAEC.AEtoken, pSysInfo->Oprofile[i].AAEC.AEtoken) == 0)
			{
				pSysInfo->Oprofile[index].AAEC.AEusercount = 0;
				if(pSysInfo->Oprofile[i].AAEC.AEusercount > 1)
					pSysInfo->Oprofile[i].AAEC.AEusercount -= 1;
				break;
			}
		}
	}

         memset(&pSysInfo->Oprofile[index].AAEC, 0, sizeof(pSysInfo->Oprofile[index].AAEC));

	return SetSysInfo(0);
}

int fsetvideoanalytics(video_analytics_conf video_analytics_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[video_analytics_tmp.position].AVAC, &video_analytics_tmp.video_analytics_conf_t, sizeof(video_analytics_tmp.video_analytics_conf_t));
	return SetSysInfo(0);
}
int fsetvideooutput(video_output_conf video_output_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Oprofile[video_output_tmp.position].AVOC, &video_output_tmp.video_output_conf_t, sizeof(video_output_tmp.video_output_conf_t));
	return SetSysInfo(0);
}

int fsetdiscoverymode(unsigned char mode)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->net.net_config.discovery_mode, &mode, sizeof(mode));
	return SetSysInfo(0);
}

int faddscopes(Onvif_Scopes_t onvif_scopes_t)
{
	SysInfo *oSysInfo = (SysInfo*)pShareMem;
	if(oSysInfo == NULL)
		return -1;
	int i =0;
	int NEW_TYPE_COUNT =0;
	int NEW_LOC_COUNT =0;
	int NEW_JUNK_COUNT = 0;
	int TYPE_EMPTY_COUNT =0;
	int LOC_EMPTY_COUNT =0;
	int JUNK_EMPTY_COUNT =0;
	int INDEX_TYPE_EMP[3] = {0,0,0};
	int INDEX_LOC_EMP[3] = {0,0,0};
	int INDEX_JUNK_EMP[3] = {0,0,0};
	for(i =0;i<3;i++)
	{
		if(strstr(onvif_scopes_t.type_t[i],"/type")!=NULL)
		{
			NEW_TYPE_COUNT++;
		}
		if(strstr(onvif_scopes_t.location_t[i],"/location")!=NULL)
		{
			NEW_LOC_COUNT++;
		}
		if(strstr(onvif_scopes_t.improper_input_t[i],"onvif://www.onvif.org")!= NULL)
		{
			NEW_JUNK_COUNT++;
		}
	}
	for(i =0;i<3;i++)
	{
		if(strstr(oSysInfo->onvif_scopes.type[i],"/type")==NULL)
		{
			memset(oSysInfo->onvif_scopes.type[i],0,sizeof(oSysInfo->onvif_scopes.type[i]));
			TYPE_EMPTY_COUNT++;
			INDEX_TYPE_EMP[i] = 1;

		}
		if(strstr(oSysInfo->onvif_scopes.location[i],"/location")==NULL)
		{
			memset(oSysInfo->onvif_scopes.location[i],0,sizeof(oSysInfo->onvif_scopes.location[i]));
			LOC_EMPTY_COUNT++;
			INDEX_LOC_EMP[i] = 1;
		}

		if(strstr(oSysInfo->onvif_scopes.improper_input[i],"onvif://www.onvif.org") == NULL)
		{
			memset(oSysInfo->onvif_scopes.improper_input[i],0,sizeof(oSysInfo->onvif_scopes.improper_input[i]));
			JUNK_EMPTY_COUNT++;
			INDEX_JUNK_EMP[i] = 1;
		}

	}
	if(NEW_TYPE_COUNT <=TYPE_EMPTY_COUNT)
	{
		for(i=0;i<NEW_TYPE_COUNT;)
		{
			if(INDEX_TYPE_EMP[0]==1)
			{
				strcpy(oSysInfo->onvif_scopes.type[0],onvif_scopes_t.type_t[i]);
				INDEX_TYPE_EMP[0]=0;
				i++;
			}
			else if(INDEX_TYPE_EMP[1]==1)
			{
				strcpy(oSysInfo->onvif_scopes.type[1],onvif_scopes_t.type_t[i]);
				INDEX_TYPE_EMP[1]=0;i++;
			}
			else if(INDEX_TYPE_EMP[2]==1)
			{
				strcpy(oSysInfo->onvif_scopes.type[2],onvif_scopes_t.type_t[i]);
				INDEX_TYPE_EMP[2]=0;i++;
			}

		}
	}
	if(NEW_LOC_COUNT <=LOC_EMPTY_COUNT)
	{
		for(i=0;i<NEW_LOC_COUNT;)
		{
			if(INDEX_LOC_EMP[0]==1)
			{
				strcpy(oSysInfo->onvif_scopes.location[0],onvif_scopes_t.location_t[i]);
				INDEX_LOC_EMP[0]=0;
				i++;
			}
			else if(INDEX_LOC_EMP[1]==1)
			{
				strcpy(oSysInfo->onvif_scopes.location[1],onvif_scopes_t.location_t[i]);
				INDEX_LOC_EMP[1]=0;i++;
			}
			else if(INDEX_LOC_EMP[2]==1)
			{
				strcpy(oSysInfo->onvif_scopes.location[2],onvif_scopes_t.location_t[i]);
				INDEX_LOC_EMP[2]=0;i++;
			}
		}
	}
	if(NEW_JUNK_COUNT <=JUNK_EMPTY_COUNT)
	{
		for(i=0;i<NEW_JUNK_COUNT;)
		{
			if(INDEX_JUNK_EMP[0]==1)
			{
				strcpy(oSysInfo->onvif_scopes.improper_input[0],onvif_scopes_t.improper_input_t[i]);
				INDEX_JUNK_EMP[0]=0;
				i++;
			}
			else if(INDEX_JUNK_EMP[1]==1)
			{
				strcpy(oSysInfo->onvif_scopes.improper_input[1],onvif_scopes_t.improper_input_t[i]);
				INDEX_JUNK_EMP[1]=0;i++;
			}
			else if(INDEX_JUNK_EMP[2]==1)
			{
				strcpy(oSysInfo->onvif_scopes.improper_input[2],onvif_scopes_t.improper_input_t[i]);
				INDEX_JUNK_EMP[2]=0;i++;
			}
		}
	}
	return SetSysInfo(0);
}

int fsetscopes(Onvif_Scopes_t onvif_scopes_t)
{
    int i = 0;
    int TYPE_MAX = 0;
    int LOC_MAX = 0;
    int JUNK_MAX = 0;
    int num_tp = 0;
    int num_loc = 0;
    int num_junk = 0;
    int TYPE_OMAX = 0;
    int LOC_OMAX = 0;
    int JUNK_OMAX = 0;

    SysInfo *oSysInfo = (SysInfo*)pShareMem;
    if(oSysInfo == NULL)
        return -1;


    for(i = 0; i < 3; i++)
    {
        if(strstr(onvif_scopes_t.type_t[i],"/type")!= NULL)
            ++TYPE_MAX;
        if(strstr(onvif_scopes_t.location_t[i],"/location")!= NULL)
            ++LOC_MAX;
        if(strstr(onvif_scopes_t.improper_input_t[i],"onvif://www.onvif.org")!= NULL)
            ++JUNK_MAX;
    }

    for(i = 0; i < 3; i++)
    {
        if(strstr(oSysInfo->onvif_scopes.type[i],"/type")!= NULL)
            ++TYPE_OMAX;
        if(strstr(oSysInfo->onvif_scopes.location[i],"/location")!= NULL)
            ++LOC_OMAX;
        if(strstr(oSysInfo->onvif_scopes.improper_input[i],"onvif://www.onvif.org")!= NULL)
            ++JUNK_OMAX;

}

    for(i = 0; i < TYPE_MAX; i++)
    {
        while(num_tp < TYPE_OMAX)
        {
            if(strstr(oSysInfo->onvif_scopes.type[num_tp], "/type") != NULL)
                break;
            else
                num_tp++;
        }
        memcpy(&oSysInfo->onvif_scopes.type[num_tp], &onvif_scopes_t.type_t[i], sizeof(onvif_scopes_t.type_t[i]));
    }


    for(i = 0; i < LOC_MAX; i++)
    {
        while(num_loc < LOC_OMAX)
        {
            if(strstr(oSysInfo->onvif_scopes.location[num_loc], "/location") != NULL)
                break;
            else
                num_loc++;
        }
        memcpy(&oSysInfo->onvif_scopes.location[num_loc], &onvif_scopes_t.location_t[i], sizeof(onvif_scopes_t.location_t[i]));
    }
    for(i = 0; i < JUNK_MAX; i++)
    {
        while(num_junk < JUNK_OMAX)
        {
            if(strstr(oSysInfo->onvif_scopes.improper_input[num_junk],"onvif://www.onvif.org") != NULL)
                break;
            else
                num_junk++;
        }
        memcpy(&oSysInfo->onvif_scopes.improper_input[num_junk], &onvif_scopes_t.improper_input_t[i], sizeof(onvif_scopes_t.improper_input_t[i]));
    }

  return SetSysInfo(0);
}

int fdelscopes(Onvif_Scopes_t onvif_scopes_t)
{
	SysInfo *oSysInfo = (SysInfo*)pShareMem;
	if(oSysInfo == NULL)
		return -1;
	int i = 0;
	int num_tp = 0;
	int num_loc = 0;
	int num_junk = 0;
	for(i=0;i<3;i++)
	{
		num_tp =0;
		if(strstr(onvif_scopes_t.type_t[i],"type")!=NULL)
		{
			while(num_tp<3)
			{
				if(strcmp(oSysInfo->onvif_scopes.type[num_tp],onvif_scopes_t.type_t[i])==0)
				{
					memset(&oSysInfo->onvif_scopes.type[num_tp],0,sizeof(oSysInfo->onvif_scopes.type[num_tp]));
				}
				num_tp++;
			}
		}
	}
	for(i=0;i<3;i++)
	{
		num_loc =0;
		if(strstr(onvif_scopes_t.location_t[i],"location")!=NULL)
		{
			while(num_loc<3)
			{
				if(strcmp(oSysInfo->onvif_scopes.location[num_loc],onvif_scopes_t.location_t[i])==0)
				{
					memset(&oSysInfo->onvif_scopes.location[num_loc],0,sizeof(oSysInfo->onvif_scopes.location[num_loc]));
				}
				num_loc++;
			}
		}
	}

	for(i=0;i<3;i++)
	{
		num_junk =0;
		if(strstr(onvif_scopes_t.improper_input_t[i],"onvif://www.onvif.org")!= NULL)
		{
			while(num_junk<3)
			{
				if(strcmp(oSysInfo->onvif_scopes.improper_input[num_junk],onvif_scopes_t.improper_input_t[i])==0)
				{
					memset(&oSysInfo->onvif_scopes.improper_input[num_junk],0,sizeof(oSysInfo->onvif_scopes.improper_input[num_junk]));
				}
				num_junk++;
			}
		}
	}
	return SetSysInfo(0);
}


int fsetrelay(relay_conf relay_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->onvif_relay[relay_tmp.position].onvif_relay_in, &relay_tmp.relay_conf_in_t, sizeof(relay_tmp.relay_conf_in_t));
	return SetSysInfo(0);
}
int fsetrelaylogicalstate(relay_conf relay_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->onvif_relay[relay_tmp.position].relaylogicalstate, &relay_tmp.logicalstate_t, sizeof(relay_tmp.logicalstate_t));
	return SetSysInfo(0);
}
/* End-Onvif */

int fsethostname(hostname_tmp *host)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.hostname, &host->hostname_t, sizeof(host->hostname_t));
	pSysInfo->nrelays++ ;
	return SetSysInfo(0);
}

/**
* @brief Get system log information.

* @param nPageNum [I ] Page number
* @param nItemNum [I ] Item number
* @param pBuf [O ] pointer to where log information stored.
* @retval 0 Success.
* @retval -1 Fail
*/
int GetDmvaSysLog(int nPageNum, int nItemNum, DmvaLogEntry_t * pBuf)
{
	FILE_MSG_BUF msgbuf;
	SysInfo *pSysInfo = (SysInfo *) pShareMem;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ_DMVA_LOG;
	msgbuf.shmid = mid;
	msgbuf.shmidx = 1;
	msgbuf.nPageNum = nPageNum;
	msgbuf.nItemNum = nItemNum;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	memcpy(pBuf, &pSysInfo->tCurDmvaLog, sizeof(DmvaLogEntry_t));
	return 0;
}
/* DMVA specific Fset */
/**
* @brief Save a system log in file manager.
* @param value [I ] System log.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDmvaSysLog(DmvaLogEntry_t* value)
{
	FILE_MSG_BUF msgbuf;
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL || value == NULL)
		return -1;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_DMVA_LOG;
	msgbuf.shmid = mid;
	msgbuf.shmidx = 0;
	msgbuf.nPageNum = (int)value;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}


int fSetDmvaEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaenable_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaenable_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaExptAlgoFrameRate(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaexptalgoframerate_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaexptalgoframerate_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaExptAlgoDetectFreq(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaexptalgodetectfreq_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaexptalgodetectfreq_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaExptEvtRecordEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaexptevtrecordenable_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaexptevtrecordenable_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaExptSmetaTrackerEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaexptSmetaTrackerEnable_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaexptSmetaTrackerEnable_sys, &value, sizeof(value));
	pSysInfo->dmva_config.dmvaAppSMETAEnableVelocityObjId_sys = pSysInfo->dmva_config.dmvaexptSmetaTrackerEnable_sys;
	return SetSysInfo(0);
}

/* DMVA TZ specific Fset */
int fSetDmvaTZSensitivity(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZSensitivity_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZSensitivity_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZPersonMinWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZPersonMinWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZPersonMinWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZPersonMinHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZPersonMinHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZPersonMinHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZVehicleMinWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZVehicleMinWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZVehicleMinWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZVehicleMinHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZVehicleMinHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZVehicleMinHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZPersonMaxWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZPersonMaxWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZPersonMaxWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZPersonMaxHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZPersonMaxHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZPersonMaxHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZVehicleMaxWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZVehicleMaxWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZVehicleMaxWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZVehicleMaxHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZVehicleMaxHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZVehicleMaxHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZDirection(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZDirection_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZDirection_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZPresentAdjust(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZPresentAdjust_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZPresentAdjust_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZEnable_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZEnable_sys, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaTZZone1Numsides(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_ROI_numSides_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_ROI_numSides_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2Numsides(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_ROI_numSides_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_ROI_numSides_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[1], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[2], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[3], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[4], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[5], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[6], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[7], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[8], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[9], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[10], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[11], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[12], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[13], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[14], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[15], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_x16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_x[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_x[16], &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaTZZone1_y01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[1], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[2], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[3], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[4], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[5], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[6], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[7], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[8], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[9], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[10], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[11], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[12], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[13], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[14], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[15], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone1_y16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone1_y[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone1_y[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[1], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[2], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[3], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[4], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[5], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[6], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[7], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[8], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[9], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[10], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[11], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[12], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[13], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[14], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[15], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_x16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_x[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_x[16], &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaTZZone2_y01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[1], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[2], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[3], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[4], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[5], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[6], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[7], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[8], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[9], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[10], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[11], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[12], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[13], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[14], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[15], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaTZZone2_y16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppTZZone2_y[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZZone2_y[16], &value, sizeof(value));
	return SetSysInfo(0);
}

/* DMVA TZ save/load params */
int fSetDmvaTZSave(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->dmva_config.dmvaAppTZSave_sys,buf,len);
	pSysInfo->dmva_config.dmvaAppTZSave_sys[len] = '\0';

	return SaveDmvaTZParams();
}

int fSetDmvaTZLoad(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->dmva_config.dmvaAppTZLoad_sys, &value, sizeof(value));
	return LoadDmvaTZParams();
}


/* Set DMVA Main Page specific Fset */
int fSetDmvaCamID(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppCamID_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppCamID_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaGetSchedule(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppGetSchedule_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppGetSchedule_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaCfgFD(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppCfgFD_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppCfgFD_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaCfgCTDImdSmetaTzOc(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppCfgCTDImdSmetaTzOc_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppCfgCTDImdSmetaTzOc_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaDisplayOptions(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppDisplayOptions_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppDisplayOptions_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventRecordingVAME(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventRecordingVAME_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventRecordingVAME_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int SearchDmvaEvent()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_SEARCH_DMVA_EVT;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fSetDmvaEventListActionPlaySendSearchTrash(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
#if 0
	if(value == pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys)
	    return -2;
#endif
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys, &value, sizeof(value));

	return SearchDmvaEvent();
}

int fSetDmvaEventListSelectEvent(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventListSelectEvent_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListSelectEvent_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventListArchiveFlag(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->tCurDmvaLog.eventListArchiveFlag_sys)
	    return -2;
	memcpy(&pSysInfo->tCurDmvaLog.eventListArchiveFlag_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventListEventType(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->tCurDmvaLog.eventListEventType_sys)
	    return -2;
	memcpy(&pSysInfo->tCurDmvaLog.eventListEventType_sys, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaEventListStartMonth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventListStartMonth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListStartMonth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventListStartDay(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventListStartDay_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListStartDay_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventListStartTime(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventListStartTime_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListStartTime_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventListEndMonth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventListEndMonth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListEndMonth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventListEndDay(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventListEndDay_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListEndDay_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEventListEndTime(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEventListEndTime_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEventListEndTime_sys, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaEventListTimeStamp(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->tCurDmvaLog.eventListTimeStamp_sys))==0)
	    return -2;
	memcpy(pSysInfo->tCurDmvaLog.eventListTimeStamp_sys, buf, len);
	pSysInfo->tCurDmvaLog.eventListTimeStamp_sys[len] = '\0';

	return SetSysInfo(0);
}

int fSetDmvaEventListEventDetails(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->tCurDmvaLog.eventListEventDetails_sys))==0)
	    return -2;
	memcpy(pSysInfo->tCurDmvaLog.eventListEventDetails_sys, buf, len);
	pSysInfo->tCurDmvaLog.eventListEventDetails_sys[len] = '\0';

	return SetSysInfo(0);
}

/* Set DMVA Advanced Settings Page specific Parameters */

#if 1
int fSetDmvaObjectMetaData(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaObjectMetaData_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaObjectMetaData_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesFontType(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesFontType_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesFontType_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesFontSize(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesFontSize_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesFontSize_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesFontColor(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesFontColor_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesFontColor_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesBoxColor(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesBoxColor_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesBoxColor_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesCentroidColor(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesCentroidColor_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesCentroidColor_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesActiveZoneColor(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesActiveZoneColor_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesActiveZoneColor_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesInactiveZoneColor(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesInactiveZoneColor_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesInactiveZoneColor_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOverlayPropertiesVelocityColor(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaOverlayPropertiesVelocityColor_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaOverlayPropertiesVelocityColor_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaEnvironmentProfileLoad(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaEnvironmentProfileLoad_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaEnvironmentProfileLoad_sys, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaEnvironmentProfileStore(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaEnvironmentProfileStore_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaEnvironmentProfileStore_sys, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaEnvironmentProfileClear(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaEnvironmentProfileClear_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaEnvironmentProfileClear_sys, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaEnvironmentProfileState(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaEnvironmentProfileState_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaEnvironmentProfileState_sys, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaEnvironmentProfileDesc(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->dmva_config.dmvaEnvironmentProfileDesc_sys))==0)
	    return -2;
	memcpy(pSysInfo->dmva_config.dmvaEnvironmentProfileDesc_sys, buf, len);
	pSysInfo->dmva_config.dmvaEnvironmentProfileDesc_sys[len] = '\0';

	return SetSysInfo(0);
}
#endif
/*Set DMVA event delete specific fset */
int DmvaEventDeleteIndex()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_DMVA_DELETE_INDEX;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fSetDmvaEventDeleteIndex(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value != pSysInfo->dmva_config.dmvaEventDeleteIndex)
	{
	    memcpy(&pSysInfo->dmva_config.dmvaEventDeleteIndex, &value, sizeof(value));
	    SetSysInfo(0);
	}

	return DmvaEventDeleteIndex();
}

int DmvaEventDeleteIndexStartEnd()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_DMVA_DELETE_STARTEND_INDEX;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fSetDmvaEventDeleteStartIndex(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value != pSysInfo->dmva_config.dmvaEventDeleteStartIndex)
	{
	    memcpy(&pSysInfo->dmva_config.dmvaEventDeleteStartIndex, &value, sizeof(value));
	    SetSysInfo(0);
	}

	return 0;
}

int fSetDmvaEventDeleteEndIndex(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value != pSysInfo->dmva_config.dmvaEventDeleteEndIndex)
	{
	    memcpy(&pSysInfo->dmva_config.dmvaEventDeleteEndIndex, &value, sizeof(value));
	    SetSysInfo(0);
	}

	return DmvaEventDeleteIndexStartEnd();
}

int DmvaEventDeleteIndexAll()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_DMVA_DELETE_ALL;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fSetDmvaEventDeleteAll(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value != pSysInfo->dmva_config.dmvaEventDeleteAll)
	{
	    memcpy(&pSysInfo->dmva_config.dmvaEventDeleteAll, &value, sizeof(value));
	    SetSysInfo(0);
	}

	return DmvaEventDeleteIndexAll();
}

/* Set DMVA CTD Page specific Fset */
int fSetDmvaCfgTDEnv1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppCfgTDEnv1_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppCfgTDEnv1_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaCfgTDEnv2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppCfgTDEnv2_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppCfgTDEnv2_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaCfgTDSensitivity(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppCfgTDSensitivity_sys)
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppCfgTDSensitivity_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaCfgTDResetTime(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppCfgTDResetTime_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppCfgTDResetTime_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

/* Set DMVA OC Page specific Fset */
int fSetDmvaOCSensitivity(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppOCSensitivity_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCSensitivity_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOCObjectWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppOCObjectWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCObjectWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOCObjectHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppOCObjectHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCObjectHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOCDirection(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppOCDirection_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCDirection_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOCEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppOCEnable_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCEnable_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOCLeftRight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppOCLeftRight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCLeftRight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaOCTopBottom(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppOCTopBottom_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCTopBottom_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

/* DMVA OC save/load params */
int fSetDmvaOCSave(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->dmva_config.dmvaAppOCSave_sys,buf,len);
	pSysInfo->dmva_config.dmvaAppOCSave_sys[len] = '\0';

	return SaveDmvaOCParams();
}

int fSetDmvaOCLoad(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->dmva_config.dmvaAppOCLoad_sys, &value, sizeof(value));
	return LoadDmvaOCParams();
}


/* Set DMVA SMETA Page specific Fset */
int fSetDmvaSMETASensitivity(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETASensitivity_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETASensitivity_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAEnableStreamingData(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAEnableStreamingData_sys)
	    return -2;
	if((value & 0x4) == 0)
	{
	    value &= ~0x1;
	}
	if((value & 0x10) == 0)
	{
	    value &= ~0x2;
	}
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAEnableStreamingData_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAStreamBB(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAStreamBB_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAStreamBB_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAStreamVelocity(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAStreamVelocity_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAStreamVelocity_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAStreamHistogram(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAStreamHistogram_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAStreamHistogram_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAStreamMoments(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAStreamMoments_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAStreamMoments_sys, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaSMETAPresentAdjust(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAPresentAdjust_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAPresentAdjust_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAPersonMinWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAPersonMinWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAPersonMinWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAPersonMinHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAPersonMinHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAPersonMinHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAVehicleMinWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAVehicleMinWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAVehicleMinWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAVehicleMinHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAVehicleMinHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAVehicleMinHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAPersonMaxWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAPersonMaxWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAPersonMaxWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAPersonMaxHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAPersonMaxHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAPersonMaxHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAVehicleMaxWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAVehicleMaxHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETANumZones(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETANumZones_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETANumZones_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAZone1Numsides(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_ROI_numSides_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_ROI_numSides_sys, &value, sizeof(value));
	return SetSysInfo(0);
}



int fSetDmvaSMETAZone1_x01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_x16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_x[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_x[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAZone1_y01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaSMETAZone1_y16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppSMETAZone1_y[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETAZone1_y[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaSMETAZone1_Label(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->dmva_config.dmvaSMETAZone_1_Label_sys))==0)
	    return -2;
	memcpy(pSysInfo->dmva_config.dmvaSMETAZone_1_Label_sys, buf, len);
	pSysInfo->dmva_config.dmvaSMETAZone_1_Label_sys[len] = '\0';

	return SetSysInfo(0);
}

/* DMVA SMETA save/load params */
int fSetDmvaSMETASave(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->dmva_config.dmvaAppSMETASave_sys,buf,len);
	pSysInfo->dmva_config.dmvaAppSMETASave_sys[len] = '\0';

	return SaveDmvaSMETAParams();
}

int fSetDmvaSMETALoad(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->dmva_config.dmvaAppSMETALoad_sys, &value, sizeof(value));
	return LoadDmvaSMETAParams();
}


/* Set DMVA IMD Page specific Fset */
int fSetDmvaIMDSensitivity(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDSensitivity_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDSensitivity_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDPresentAdjust(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDPresentAdjust_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDPresentAdjust_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDPersonMinWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDPersonMinWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDPersonMinWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDPersonMinHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDPersonMinHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDPersonMinHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDVehicleMinWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDVehicleMinWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDVehicleMinWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDVehicleMinHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDVehicleMinHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDVehicleMinHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDPersonMaxWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDPersonMaxWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDPersonMaxWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDPersonMaxHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDPersonMaxHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDPersonMaxHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDVehicleMaxWidth(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDVehicleMaxWidth_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDVehicleMaxWidth_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDVehicleMaxHeight(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDVehicleMaxHeight_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDVehicleMaxHeight_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDNumZones(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDNumZones_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDNumZones_sys, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDZoneEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZoneEnable_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZoneEnable_sys, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaIMDZone1Numsides(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_ROI_numSides_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_ROI_numSides_sys, &value, sizeof(value));
	return SetSysInfo(0);
}



int fSetDmvaIMDZone2Numsides(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_ROI_numSides_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_ROI_numSides_sys, &value, sizeof(value));
	return SetSysInfo(0);
}



int fSetDmvaIMDZone3Numsides(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_ROI_numSides_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_ROI_numSides_sys, &value, sizeof(value));
	return SetSysInfo(0);
}



int fSetDmvaIMDZone4Numsides(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_ROI_numSides_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_ROI_numSides_sys, &value, sizeof(value));
	return SetSysInfo(0);
}




int fSetDmvaIMDZone1_x01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_x16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_x[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_x[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDZone1_y01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone1_y16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone1_y[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone1_y[16], &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetDmvaIMDZone2_x01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[1])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[2])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[3])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[4])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[5])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[6])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[7])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[8])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[9])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[10])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[11])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[12])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[13])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[14])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[15])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_x16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
    if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_x[16])
        return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_x[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDZone2_y01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone2_y16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone2_y[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone2_y[16], &value, sizeof(value));
	return SetSysInfo(0);
}





int fSetDmvaIMDZone3_x01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_x16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_x[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_x[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDZone3_y01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone3_y16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone3_y[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone3_y[16], &value, sizeof(value));
	return SetSysInfo(0);
}





int fSetDmvaIMDZone4_x01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_x16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_x[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_x[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDZone4_y01(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[1])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[1], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y02(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[2])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[2], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y03(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[3])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[3], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y04(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[4])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[4], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y05(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[5])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[5], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y06(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[6])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[6], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y07(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[7])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[7], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y08(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[8])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[8], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y09(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[9])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[9], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y10(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[10])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[10], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y11(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[11])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[11], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y12(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[12])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[12], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y13(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[13])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[13], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y14(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[14])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[14], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y15(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[15])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[15], &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDmvaIMDZone4_y16(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppIMDZone4_y[16])
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDZone4_y[16], &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDmvaIMDZone1_Label(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->dmva_config.dmvaIMDZone_1_Label_sys))==0)
	    return -2;
	memcpy(pSysInfo->dmva_config.dmvaIMDZone_1_Label_sys, buf, len);
	pSysInfo->dmva_config.dmvaIMDZone_1_Label_sys[len] = '\0';

	return SetSysInfo(0);
}

int fSetDmvaIMDZone2_Label(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->dmva_config.dmvaIMDZone_2_Label_sys))==0)
	    return -2;
	memcpy(pSysInfo->dmva_config.dmvaIMDZone_2_Label_sys, buf, len);
	pSysInfo->dmva_config.dmvaIMDZone_2_Label_sys[len] = '\0';

	return SetSysInfo(0);
}

int fSetDmvaIMDZone3_Label(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->dmva_config.dmvaIMDZone_3_Label_sys))==0)
	    return -2;
	memcpy(pSysInfo->dmva_config.dmvaIMDZone_3_Label_sys, buf, len);
	pSysInfo->dmva_config.dmvaIMDZone_3_Label_sys[len] = '\0';

	return SetSysInfo(0);
}

int fSetDmvaIMDZone4_Label(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if(strcmp(buf,(char *)(pSysInfo->dmva_config.dmvaIMDZone_4_Label_sys))==0)
	    return -2;
	memcpy(pSysInfo->dmva_config.dmvaIMDZone_4_Label_sys, buf, len);
	pSysInfo->dmva_config.dmvaIMDZone_4_Label_sys[len] = '\0';

	return SetSysInfo(0);
}

/* DMVA IMD save/load params */
int fSetDmvaIMDSave(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->dmva_config.dmvaAppIMDSave_sys,buf,len);
	pSysInfo->dmva_config.dmvaAppIMDSave_sys[len] = '\0';

	return SaveDmvaIMDParams();
}

int fSetDmvaIMDLoad(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->dmva_config.dmvaAppIMDLoad_sys, &value, sizeof(value));
	return LoadDmvaIMDParams();
}
#endif
// AYK - 0310
// Save the DMVA TZ params in sysenv_tz.cgf file
int SaveDmvaTZParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_DMVATZ;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

// Load the DMVA TZ params into shared memory

int LoadDmvaTZParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ_DMVATZ;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}

// Save the DMVA IMD params in sysenv_imd.cgf file
int SaveDmvaIMDParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_DMVAIMD;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

// Load the DMVA IMD params into shared memory
int LoadDmvaIMDParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ_DMVAIMD;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}

// Save the DMVA OC params in sysenv_oc.cgf file
int SaveDmvaOCParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_DMVAOC;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

// Load the DMVA OC params into shared memory

int LoadDmvaOCParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ_DMVAOC;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}

// Save the DMVA SMETA params in sysenv_smeta.cgf file
int SaveDmvaSMETAParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_DMVASMETA;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}

// Load the DMVA SMETA params into shared memory

int LoadDmvaSMETAParams()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ_DMVASMETA;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}

#if 0
/* DMVA Scheduler */

int fSetDmvaSchEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;

	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->dmva_config.dmvaScheduleMgr.enable)
	{
	    memcpy(&pSysInfo->dmva_config.dmvaScheduleMgr.enable, &value, sizeof(value));
	    return SetSysInfo(0);
	}

	return 0;
}

int fSetDmvaSchStart(unsigned int value)
{
	int i;
	SysInfo *pSysInfo = (SysInfo *)pShareMem;

	if(pSysInfo == NULL)
		return -1;

    for(i = 0;i < DMVA_SCH_MAX_NUM_SCHEDULE;i ++)
    {
        if(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].indexNo == value)
        {
		    memcpy(&pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule,
		           &pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i],sizeof(DMVA_schedule_t));
		    return SetSysInfo(0);
		}
	}

	memset(&pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule,0x00,sizeof(DMVA_schedule_t));
	return SetSysInfo(0);
}

static int dmvaCheckOverlap(SysInfo *pSysInfo,DMVA_schedule_t *pDmvaSchedule)
{
    int i;
    int ret = 0;
    int checkTime = 0;

    for(i = 0;i < DMVA_SCH_MAX_NUM_SCHEDULE;i ++)
    {
        if((strcmp((char *)(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].ruleLabel),(char *)(pDmvaSchedule->ruleLabel)) != 0) &&
		   (pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].isActive == 1))
        {
            switch(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay)
            {
                case DMVA_SCH_DAY_ALL:
                    checkTime = 1;
                    break;

                case DMVA_SCH_DAY_WEEKDAYS:
                    if((pDmvaSchedule->startDay == DMVA_SCH_DAY_ALL)      ||
                       (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKDAYS) ||
                       ((pDmvaSchedule->startDay >= DMVA_SCH_DAY_MON) && (pDmvaSchedule->startDay <= DMVA_SCH_DAY_FRI)) ||
                       ((pDmvaSchedule->endDay >= DMVA_SCH_DAY_MON) && (pDmvaSchedule->endDay <= DMVA_SCH_DAY_FRI)))
                    {
                        checkTime = 1;
                    }

                    break;

                case DMVA_SCH_DAY_WEEKEND:
                    if((pDmvaSchedule->startDay == DMVA_SCH_DAY_ALL)     ||
                       (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKEND) ||
                       ((pDmvaSchedule->startDay == DMVA_SCH_DAY_SAT) || (pDmvaSchedule->endDay == DMVA_SCH_DAY_SAT))     ||
                       ((pDmvaSchedule->startDay == DMVA_SCH_DAY_SUN) || (pDmvaSchedule->endDay == DMVA_SCH_DAY_SUN)))
                    {
                        checkTime = 1;
                    }

                    break;

                case DMVA_SCH_DAY_MON:
                case DMVA_SCH_DAY_TUE:
                case DMVA_SCH_DAY_WED:
                case DMVA_SCH_DAY_THU:
                case DMVA_SCH_DAY_FRI:
                    if((pDmvaSchedule->startDay == DMVA_SCH_DAY_ALL)      ||
                       (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKDAYS) ||
                       ((pDmvaSchedule->startDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
                        (pDmvaSchedule->startDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)) ||
                       ((pDmvaSchedule->endDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
                        (pDmvaSchedule->endDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)))
                    {
					    checkTime = 1;
                    }

                    break;

                case DMVA_SCH_DAY_SAT:
                case DMVA_SCH_DAY_SUN:
                    if((pDmvaSchedule->startDay == DMVA_SCH_DAY_ALL)     ||
                       (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKEND) ||
                       ((pDmvaSchedule->startDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
                        (pDmvaSchedule->startDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)) ||
                       ((pDmvaSchedule->endDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
                        (pDmvaSchedule->endDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)))
                    {
					    checkTime = 1;
                    }

                    break;
            }// switch(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay)

            if(checkTime == 0)
            {
			    switch(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)
			    {
					case DMVA_SCH_DAY_MON:
					case DMVA_SCH_DAY_TUE:
					case DMVA_SCH_DAY_WED:
					case DMVA_SCH_DAY_THU:
					case DMVA_SCH_DAY_FRI:
						if((pDmvaSchedule->startDay == DMVA_SCH_DAY_ALL)      ||
						   (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKDAYS) ||
						   ((pDmvaSchedule->startDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
							(pDmvaSchedule->startDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)) ||
						   ((pDmvaSchedule->endDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
							(pDmvaSchedule->endDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)))
						{
							checkTime = 1;
						}

						break;

					case DMVA_SCH_DAY_SAT:
					case DMVA_SCH_DAY_SUN:
						if((pDmvaSchedule->startDay == DMVA_SCH_DAY_ALL)     ||
						   (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKEND) ||
						   ((pDmvaSchedule->startDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
							(pDmvaSchedule->startDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)) ||
						   ((pDmvaSchedule->endDay >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) &&
							(pDmvaSchedule->endDay <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)))
						{
							checkTime = 1;
						}

						break;
				}// switch(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay)
			}

            if(checkTime == 1)
            {
				checkTime = 0;

				if(((pDmvaSchedule->startTimeInMin <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startTimeInMin) &&
					(pDmvaSchedule->endTimeInMin <= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startTimeInMin)) ||
				   ((pDmvaSchedule->startTimeInMin >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endTimeInMin) &&
					(pDmvaSchedule->endTimeInMin >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endTimeInMin)))
				{
					ret = 0;
				}
				else
				{
					#ifdef DMVA_SCH_DBG
					fprintf(stderr," DMVA SCH > CONFLICT with rule %d\n",i);
					fprintf(stderr," DMVA SCH > rule start day = %d,new rule start day = %d\n",
							pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay,pDmvaSchedule->startDay);
					fprintf(stderr," DMVA SCH > rule end day   = %d,new rule end day   = %d\n",
							pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay,pDmvaSchedule->endDay);
					fprintf(stderr," DMVA SCH > rule startTime = %d,new rule startTime = %d\n",
							pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startTimeInMin,
							pDmvaSchedule->startTimeInMin);
					fprintf(stderr," DMVA SCH > rule endTime   = %d,new rule endTime   = %d\n",
							pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endTimeInMin,
							pDmvaSchedule->endTimeInMin);
					#endif

					return -1;
				}
			}
        }
    }// for(i = 0;i < DMVA_SCH_MAX_NUM_SCHEDULE;i ++)

    return ret;
}

static void dmvaIndexScheduleList(SysInfo *pSysInfo)
{
    int i;
    int schIndex = 1;

    for(i = 0;i < DMVA_SCH_MAX_NUM_SCHEDULE;i ++)
    {
        if(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].isActive == 1)
        {
            pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].indexNo = schIndex;
            schIndex ++;
        }
        else
        {
            pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].indexNo = -1;
        }
    }
}

static int dmvaCheckLoadSettings(SysInfo *pSysInfo,DMVA_schedule_t *pDmvaSchedule)
{
    Dmva_save_load_rules_t *pLoadSave = NULL;

    switch(pDmvaSchedule->ruleType)
    {
        case DMVA_SCH_RULETYPE_TZ:
            pLoadSave = &pSysInfo->dmva_config.dmvaSaveLoadTZ;
            break;

        case DMVA_SCH_RULETYPE_IMD:
            pLoadSave = &pSysInfo->dmva_config.dmvaSaveLoadIMD;
            break;

        case DMVA_SCH_RULETYPE_OC:
            pLoadSave = &pSysInfo->dmva_config.dmvaSaveLoadOC;
            break;

        case DMVA_SCH_RULETYPE_SMETA:
            pLoadSave = &pSysInfo->dmva_config.dmvaSaveLoadSMETA;
            break;
    }

    if(pDmvaSchedule->loadSetting >= pLoadSave->numRules)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int fSetDmvaSchCheckAddRule(DMVA_schedule_t *pDmvaSchedule)
{
    int i;
    int schIndex = -1;
    SysInfo *pSysInfo = (SysInfo *)pShareMem;

    // check for the start and end day
	if(dmvaCheckStartEndDay(pDmvaSchedule) != 0)
	{
	    pSysInfo->dmva_config.dmvaScheduleMgr.result = DMVA_SCH_RESULT_INCORRECTDAY;
		#ifdef DMVA_SCH_DBG
		fprintf(stderr," DMVA SCH > start day(%d) and end day(%d) are incorrect\n",
		        pDmvaSchedule->startDay,pDmvaSchedule->endDay);
		#endif
	    goto exit;
	}

    // check for the start and end time
    if(pDmvaSchedule->startTimeInMin >= pDmvaSchedule->endTimeInMin)
    {
	    pSysInfo->dmva_config.dmvaScheduleMgr.result = DMVA_SCH_RESULT_INCORRECTTIME;
		#ifdef DMVA_SCH_DBG
		fprintf(stderr," DMVA SCH > start time(%d) is greater than or equal to end time(%d)\n",
		        pDmvaSchedule->startTimeInMin,pDmvaSchedule->endTimeInMin);
		#endif
	    goto exit;
	}

    // check if the schedule can be added
    for(i = 0;i < DMVA_SCH_MAX_NUM_SCHEDULE;i ++)
    {
        if((strcmp((char *)(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].ruleLabel),(char *)(pDmvaSchedule->ruleLabel)) == 0) &&
           (pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].isActive == 1))
        {
            schIndex = i;
            break;
        }
        else if((pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].isActive == 0) &&
                (schIndex == -1))
             {
                 schIndex = i;
             }
    }

    if(schIndex != -1)
    {
        // check for overlapping
        if(dmvaCheckOverlap(pSysInfo,pDmvaSchedule) == 0)
        {
            // check if the load settings value is valid
            if(dmvaCheckLoadSettings(pSysInfo,pDmvaSchedule) != 0)
            {
                #ifdef DMVA_SCH_DBG
                fprintf(stderr," DMVA SCH > Invalid Load Settings(%d)\n",pDmvaSchedule->loadSetting);
                #endif

                pSysInfo->dmva_config.dmvaScheduleMgr.result = DMVA_SCH_RESULT_INVALIDLOADSETTINGS;
            }
            else
            {
                // schedule rule doesn't overlap with the other schedule rules
                memcpy(&pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[schIndex],pDmvaSchedule,sizeof(DMVA_schedule_t));

                #ifdef DMVA_SCH_DBG
                fprintf(stderr,"\n DMVA SCH > Adding schedule at index %d \n\n",schIndex);
                fprintf(stderr," DMVA SCH > load setting      = %d\n",pDmvaSchedule->loadSetting);
                fprintf(stderr," DMVA SCH > rule type         = %d\n",pDmvaSchedule->ruleType);
                fprintf(stderr," DMVA SCH > rule label        = %s\n",pDmvaSchedule->ruleLabel);
                fprintf(stderr," DMVA SCH > start day         = %d\n",pDmvaSchedule->startDay);
                fprintf(stderr," DMVA SCH > end day           = %d\n",pDmvaSchedule->endDay);
                fprintf(stderr," DMVA SCH > start time in min = %d\n",pDmvaSchedule->startTimeInMin);
                fprintf(stderr," DMVA SCH > end time in min   = %d\n",pDmvaSchedule->endTimeInMin);
                #endif

                pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[schIndex].isActive = 1;
                pSysInfo->dmva_config.dmvaScheduleMgr.result                          = DMVA_SCH_RESULT_ADDDONE;

                dmvaIndexScheduleList(pSysInfo);
            }
        }
        else
        {
            #ifdef DMVA_SCH_DBG
            fprintf(stderr," DMVA SCH > schedule %s overlapps \n",pDmvaSchedule->ruleLabel);
            #endif

            pSysInfo->dmva_config.dmvaScheduleMgr.result = DMVA_SCH_RESULT_CONFLICT;
        }
    }
    else
    {
        #ifdef DMVA_SCH_DBG
        fprintf(stderr," DMVA SCH > schedule list is FULL \n");
        #endif

        pSysInfo->dmva_config.dmvaScheduleMgr.result = DMVA_SCH_RESULT_LISTFULL;
    }

exit:
    // write back the shared memory contents to the nand
    return SetSysInfo(0);
}


int fSetDmvaSchDelRule(void *buf, int len)
{
    int i;
    int ret = 0;
    char ruleLabel[DMVA_SCH_MAX_STRING_LENGTH];
    SysInfo *pSysInfo = (SysInfo *)pShareMem;

    memcpy(ruleLabel,buf,len);
    ruleLabel[len] = '\0';

    for(i = 0;i < DMVA_SCH_MAX_NUM_SCHEDULE;i ++)
    {
        if((strcmp(ruleLabel,(char *)(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].ruleLabel)) == 0) &&
           (pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].isActive == 1))
        {
			// If the rule to be deleted is currently running then stop it
			if(pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule == i)
			{
				ret = -2;

				pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule    = -1;
				pSysInfo->dmva_config.dmvaScheduleMgr.curRuleType    = -1;
				pSysInfo->dmva_config.dmvaScheduleMgr.curLoadSetting = -1;
			}

            // make the schedule rule inactive
            pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].isActive = 0;
            pSysInfo->dmva_config.dmvaScheduleMgr.result                   = DMVA_SCH_RESULT_DELDONE;

            #ifdef DMVA_SCH_DBG
            fprintf(stderr," DMVA SCH > deleting schedule rule with the index %d and label %s \n",
                    i,pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].ruleLabel);
            #endif

            dmvaIndexScheduleList(pSysInfo);
            SetSysInfo(0);
            return ret;
        }
    }

    #ifdef DMVA_SCH_DBG
    fprintf(stderr," DMVA SCH > cannot find the rule label %s for deletion\n",ruleLabel);
    #endif

    pSysInfo->dmva_config.dmvaScheduleMgr.result = DMVA_SCH_RESULT_DELFAIL;
    SetSysInfo(0);
    return ret;
}

/* DMVA Enc Roi Enable */
int fSetDmvaEncRoiEnable(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(value == pSysInfo->dmva_config.dmvaAppEncRoiEnable_sys)
	    return -2;
	memcpy(&pSysInfo->dmva_config.dmvaAppEncRoiEnable_sys, &value, sizeof(value));
	return SetSysInfo(0);
}


static int dmvaCheckStartEndDay(DMVA_schedule_t *pDmvaSchedule)
{
	int ret = 0;

    if((pDmvaSchedule->startDay == DMVA_SCH_DAY_ALL)      ||
       (pDmvaSchedule->endDay   == DMVA_SCH_DAY_ALL)      ||
       (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKDAYS) ||
       (pDmvaSchedule->endDay   == DMVA_SCH_DAY_WEEKDAYS) ||
       (pDmvaSchedule->startDay == DMVA_SCH_DAY_WEEKEND)  ||
       (pDmvaSchedule->endDay   == DMVA_SCH_DAY_WEEKEND))
    {
	    if(pDmvaSchedule->startDay != pDmvaSchedule->endDay)
	    {
		    ret = -1;
		}
	}
	else if(pDmvaSchedule->endDay < pDmvaSchedule->startDay)
	     {
		     ret = -1;
         }

	return ret;
}
#endif

#if 0
/**
* @brief Save alarm audio volume in file manager.
* @param value [I ] Alarm audio volume.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAudioInVolume(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audioinvolume, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioAlarm(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.alarmON, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioReceiverEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audiorecvenable, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetAudioSeverIp(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->audio_config.audioServerIp) < len+1)
		return -1;
	memcpy(pSysInfo->audio_config.audioServerIp, buf, len);
	pSysInfo->audio_config.audioServerIp[len] = '\0';

	return SetSysInfo(0);
}

/**
* @brief Save web site audio enable status in file manager.
* @param value [I ] Enable audio on web site or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAudioON(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audioON, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audioenable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetAudioMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audiomode, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioEncode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.codectype, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioSampleRate(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.samplerate, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioBitrateValue(int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.bitrateValue, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioBitrate(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.bitrate, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioAlarmLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.alarmlevel, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioOutVolume(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audiooutvolume, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
