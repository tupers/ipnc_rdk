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
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys_env_type.h>
#include <system_default.h>
#include "net_config.h"

#define  PROC_TIMEOUT	20

#define BOOT_NFS	0
//#define DEBUG

#ifdef DEBUG
#define printf(fmt, args...)	printf("Debug " fmt, ##args)
#else
#define printf(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("Error " fmt, ##args)

int ReadLocalIp(struct in_addr* pLocalIp, FILE* pFile);
int ReadNetMask(struct in_addr* pNetMask, FILE* pFile);
int ReadGateway(struct in_addr* pGateway, FILE* pFile);
int ReadDNS(struct in_addr* pDNS, FILE* pFile);

#if BOOT_NFS
/**
 * @brief	check magic number
 * @param	fp [I ]file pointer
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
static int check_magic_num(FILE *fp)
{
	int ret;
	unsigned long MagicNum;
	if(fread(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
		ret = FAIL;
	} else {
		if(MagicNum == MAGIC_NUM){
			ret = SUCCESS;
		} else {
			ret = FAIL;
		}
	}
	return ret;
}

/**
 * @brief	read SysInfo from system file
 * @param	"void *Buffer" : [OUT]buffer to store SysInfo
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
static int ReadGlobal(void *Buffer)
{
	FILE *fp;
	int ret;
	if((fp = fopen(SYS_FILE, "rb")) == NULL){
		/* System file not exist */
		ret = FAIL;
	} else {
		if(check_magic_num(fp) == SUCCESS){
			if(fread(Buffer, 1, SYS_ENV_SIZE,fp) != SYS_ENV_SIZE){
				ret = FAIL;
			} else {
				ret = SUCCESS;
			}
		} else {
			ret = FAIL;
		}
		fclose(fp);
	}
	return ret;
}

static int SetNetwork(SysInfo *pSysInfo)
{
	if(pSysInfo == NULL){
		return -1;
	}
	
	if(net_get_hwaddr(ETH_NAME, pSysInfo->net.net_config.MAC) != 0){
		printf("Error on get MAC address\n");
		return -1;
	}
		
	system("ifconfig eth0 up\n");
	if(net_set_ifaddr(ETH_NAME,inet_addr(pSysInfo->net.net_config.ip) < 0))
	{
		printf("Error on Set ip\n");
		return -1;
	}
			
	/* set net mask */
	if(net_set_netmask(ETH_NAME, inet_addr(pSysInfo->net.net_config.netmask) < 0)){
		printf("Fail on set netmask\n");
		return -1;
	}
		
	/* set gateway */
	if(net_set_gateway(inet_addr(pSysInfo->net.net_config.gateway) < 0)){
		printf("Fail on set gateway\n");
		return -1;
	}	
	/* set DNS */
	if(net_set_dns(pSysInfo->net.net_config.dns) < 0){
		printf("Fail on set dns\n");
		return -1;
	}

	/* set http port will be done by BOA */
	return 0;
}
#endif
/**
* @brief Get DNS from file
* @param pDNS [O] where DNS will return
* @param pFile [I] pointer to a opened file
* @retval 0:SUCCESS
* @retval -1:FAIL
*/
int ReadDNS(struct in_addr* pDNS, FILE* pFile)
{
	const char sTarget[] = "DNS=\"";
	int i;
	char strIp[16], ch;
	unsigned long ip;
	for(i = 0;i < strlen(sTarget);i++)
		if(getc(pFile) != sTarget[i]){
			printf("File format error\n");
			return -1;
		}
	for(i = 0;i < sizeof(strIp);i++){
		if((ch = getc(pFile)) != '"')
			strIp[i] = ch;
		else {
			ch = getc(pFile);
			if(ch == '\n'){
				break;
			}else if(ch == '\r'){
				getc(pFile);
				getc(pFile);
				break;
			}else
				return -1;
		}
	}
	if(i == sizeof(strIp))
		return -1;
	/* parse ip */
	strIp[i] = '\0';
	ip = inet_addr(strIp);
	memcpy(pDNS, &ip, 4);
	return 0;
}

/**
* @brief Get Gateway from file
*
* @param pGateway [O] where Gateway will return
* @param pFile [I] pointer to a opened file
* @retval 0:SUCCESS
* @retval -1:FAIL
*/
int ReadGateway(struct in_addr* pGateway, FILE* pFile)
{
	const char sTarget[] = "Gateway=\"";
	int i;
	char strIp[16], ch;
	unsigned long ip;
	for(i = 0;i < strlen(sTarget);i++)
		if(getc(pFile) != sTarget[i]){
			ERR("File format error\n");
			return -1;
		}
	for(i = 0;i < sizeof(strIp);i++){
		if((ch = getc(pFile)) != '"')
			strIp[i] = ch;
		else {
			ch = getc(pFile);
			if(ch == '\n'){
				break;
			}else if(ch == '\r'){
				getc(pFile);
				getc(pFile);
				break;
			}else
				return -1;
		}
	}
	if(i == sizeof(strIp))
		return -1;
	/* parse ip */
	strIp[i] = '\0';
	ip = inet_addr(strIp);
	memcpy(pGateway, &ip, 4);
	return 0;
}

/**
* @brief Get NetMask from file
*
* @param pNetMask [O] where NetMask will return
* @param pFile [I] pointer to a opened file
* @retval 0:SUCCESS
* @retval -1:FAIL
*/
int ReadNetMask(struct in_addr* pNetMask, FILE* pFile)
{
	const char sTarget[] = "NetMask=\"";
	int i;
	char strIp[16], ch;
	unsigned long ip;
	for(i = 0;i < strlen(sTarget);i++)
		if(getc(pFile) != sTarget[i]){
			ERR("File format error\n");
			return -1;
		}
	for(i = 0;i < sizeof(strIp);i++){
		if((ch = getc(pFile)) != '"')
			strIp[i] = ch;
		else {
			ch = getc(pFile);
			if(ch == '\n'){
				break;
			}else if(ch == '\r'){
				getc(pFile);
				getc(pFile);
				break;
			}else
				return -1;
		}
	}
	if(i == sizeof(strIp))
		return -1;
	/* parse ip */
	strIp[i] = '\0';
	ip = inet_addr(strIp);
	memcpy(pNetMask, &ip, 4);
	return 0;
}

/**
* @brief Get new config from USB file and set it to system.
* @param pSysInfo [I] pointer to current system infomation
* @retval 0:SUCCESS
* @retval -1:FAIL
*/
int UpdateEnvFromFile(unsigned int dhcp_enable)
{
	int ret = 0;
	FILE *fp;
	char filename[] = USB_CFG_FILE;
	struct in_addr ip;
	struct in_addr netmask;
	struct in_addr gateway;

	in_addr_t Localip;
	in_addr_t Localnetmask;
	in_addr_t Localgateway;
	Localip = net_get_ifaddr(ETH_NAME);
	Localnetmask = net_get_netmask(ETH_NAME);
	Localgateway = net_get_gateway();

	if((Localip == -1) || (Localnetmask == -1) ||(Localgateway == -1))
	{
		ERR("get local ip failed\n");
		return -1;
	}	
	if(dhcp_enable){
		printf("DHCP enable\n");
		return ret;
	}
	if ((fp = fopen(filename, "rb")) == NULL) {
		printf("Can't open config file\n");
		ret = -1;
	}
	 else 
	{		
		if((ReadLocalIp(&ip, fp) == 0) && (ReadNetMask(&netmask, fp) == 0) &&(ReadGateway(&gateway, fp) == 0))
		{
			printf("LocalIP=\"%s\"\n",inet_ntoa(ip));
			printf("NetMask=\"%s\"\n",inet_ntoa(netmask));
			printf("Gateway=\"%s\"\n",inet_ntoa(gateway));
			
			if(ip.s_addr != Localip){
				printf("IP change\n");
				net_set_ifaddr(ETH_NAME, ip.s_addr);
			}
			if(netmask.s_addr != Localnetmask){
				printf("NetMask change\n");
				net_set_netmask(ETH_NAME, netmask.s_addr);
			}
			if(gateway.s_addr != Localgateway){
				printf("Gateway change\n");
				net_set_gateway(gateway.s_addr);
			}
		} else {
			ERR("Unexcept error\n");
			ret = -1;
		}
		fclose(fp);
	}
	return ret;
}

/**
* @brief Get IP from file
*
* @param pLocalIp[OUT] where IP will return
* @param pFile[IN]	 Pointer to a opened file
* @return 0:SUCCESS	-1:FAIL
*/
int ReadLocalIp(struct in_addr* pLocalIp, FILE* pFile)
{
	const char sTarget[] = "LocalIP=\"";
	int i;
	char strIp[16], ch;
	unsigned long ip;
	for(i = 0;i < strlen(sTarget);i++)
		if(getc(pFile) != sTarget[i]){
			ERR("File format error\n");
			return -1;
		}
	for(i = 0;i < sizeof(strIp);i++){
		if((ch = getc(pFile)) != '"')
			strIp[i] = ch;
		else {
			ch = getc(pFile);
			if(ch == '\n'){
				break;
			}else if(ch == '\r'){
				getc(pFile);
				getc(pFile);
				break;
			}else
				return -1;
		}
	}
	if(i == sizeof(strIp))
		return -1;
	/* parse ip */
	strIp[i] = '\0';
	ip = inet_addr(strIp);
	memcpy(pLocalIp, &ip, 4);
	return 0;
}

/**
 * @brief	main entry
 * @param	command line argument
 * @return	error code
 */
int main(int argc,char *argv[]) 
{
#if BOOT_NFS
	int ret;
	Network_Config_Data *net;
	char mac[MAC_LENGTH];
	
	SysInfo *pSysInfo = (SysInfo *)malloc(SYS_ENV_SIZE);
	if(NULL == pSysInfo)
	{
		printf("malloc error\n");
		return -1;
	}
	
	ret = ReadGlobal(pSysInfo);
	if(ret == FAIL){
		printf("ReadGlobal Fail\n");

		return -1;
	}
	else
	{
		memcpy(mac,net->MAC,MAC_LENGTH);

		printf("dhcp=%d,ip=%s,netmask=%s,gateway=%s,dns=%s\n",net->dhcp_enable,net->ip,net->netmask,net->gateway,net->dns);
		int i;
		printf("\nMAC:");
		for(i=0;i<6;i++)
			printf("0x%x:",mac[i]);
		printf("\n");	

		SetNetwork(pSysInfo);
	}	

	return 0;
#else
	printf("before UpdateEnvFromFile\n");
	UpdateEnvFromFile(0);

	return 0;	
#endif	
}
