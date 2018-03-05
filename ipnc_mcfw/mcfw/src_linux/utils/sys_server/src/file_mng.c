/* ===========================================================================
* @file file_mng.c
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
#include <stdlib.h>
#include <string.h>
#include <file_mng.h>
#include <sys_env_type.h>
#include <system_default.h>
#include <ipnc_ver.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Global variables
char *gLogBuffer = NULL;


#ifdef __FILE_DEBUG
#define DBG(fmt, args...) fprintf(stdout, "FileMng: Debug " fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERROR(fmt,args...)	printf("FileMng: Error " fmt, ##args)

typedef struct LogData_t{
	LogEntry_t tLogData;
	struct LogData_t* pNext;
}LogData_t;

static LogData_t* gLogHead = NULL;

#define _DEFALUT_CODEC_ADVCONFIG	
static SysInfo SysInfoDefault =
{
	REL_UNIQUE_ID,
	ACOUNT_DEFAULT,	
	{
		{
			{IP_DEFAULT},
			{NETMASK_DEFAULT},
			{GATEWAY_DEAFULT},
			{DNS_DEFAULT},
			HTTP_PORT_DEFAULT,
			HTTPS_PORT_DEFAULT,
			DHCP_ENABLE_DEFAULT,
			DHCP_CONFIG_DEFAULT,
			NTP_SERVER_DEFAULT,
			NTP_TIMEZONE_DEFAULT,
			MAC_DEFAULT
      	},
		{
			FTP_SERVER_IP_DEFAULT,
			FTP_USERNAME_DEFAULT,
			FTP_PASSWORD_DEFAULT,
			FTP_FOLDERNAME_DEFAULT,
			FTP_PID_DEFAULT,
			FTP_PORT_DEFAULT,
        	RFTPENABLE_DEFAULT,
	   		FTPFILEFORMAT_DEFAULT
		},
		RTSPSERVERPORT,
	},	
	{
		SENSORGAIN_DEFAULT,
		PIPEGAIN_DEFAULT,
		EXPOSURE_DEFAULT,
		WHITEBALANCE_DEFAULT,
		DAY_NIGHT_DEFAULT,
		BINNING_DEFAULT,
		BACKLIGHT_CONTROL_DEFAULT,
		BACKLIGHT_VALUE_DEFAULT,
		BRIGHTNESS_DEFAULT,
		CONTRAST_DEFAULT,
		SATURATION_DEFAULT,
		SHARPNESS_DEFAULT,
		MIRROR_DEF,
		AEW_SWITCH_DEFAULT,      	
		AEW_TYPE_DEFAULT,
		AEW_EXPRIORITY_DEFAULT,
		HISTOGRAM_DEF,
		FRAMECTRL_DEFAULT,
     	MAX_FRAMERATE_DEFAULT,
     	MAX_FRAMERATE_DEFAULT,
     	MAX_FRAMERATE_DEFAULT,
     	/** H3A Param ***/
		{
			100,				/** minExposure **/
			33333,				/** maxExposure **/
			200,				/** stepSize **/
			1000,				/** aGainMin **/
			230000,				/** aGainMax **/
			1024,				/** dGainMin **/
			8092,				/** dGainMax **/
			30,					/** targetBrightnessMin **/
			50,					/** targetBrightnessMax **/
			40,					/** targetBrightness **/
			10,					/** threshold **/

			80,					/** width **/
			24,					/** height **/
			16,					/** HCount **/
			30,					/** VCount **/
			0,					/** HStart **/
			30,					/** VStart **/
			8,					/** horzIncr **/
			8,					/** vertIncr **/
		},
		/**AEWB WEIGHT **/
		{
			16,				/** width1 **/
			30,				/** height1 **/
			4,				/** h_start2 **/
			7,				/** v_start2 **/
			8,				/** width2 **/
			15,				/** height2 **/
			50,				/** weight **/
		},
		{
			VS_DEFAULT,
			LDC_DEFAULT,
			VNF_DEFAULT,
			VNF_MODE_DEFAULT,
			VNF_STRENGTH_DEFAULT,
			DYNRANGE_DEF,
			DYNRANGE_STRENGTH
		},	    		
	},  	
	{
		STREAMTYPE_DEFAULT,
		VCODECMODE_DEFAULT,	
		VCODECCOMBO_DEFAULT,
		VCODECRES_DEFAULT,
		FRAMERATE1_DEFAULT,
		FRAMERATE2_DEFAULT,
		FRAMERATE3_DEFAULT,
		JPEGQUALITY_DEF,
		MPEG41BITRATE_DEFAULT,
		MPEG42BITRATE_DEFAULT,
		RATECONTROL_DEFAULT,
		RATECONTROL_DEFAULT,
		H264_CODEC,
		H264_CODEC,
		H264_CODEC,  
		CODECADVPARAM,
	},
	/** CamSystemInfo ***/
	{
		1,
		TOTAL_MEM,
		FREE_MEM,
		SHARE_MEM,
		CORE_MEM,
		STORAGE_MEM,
		STORAGE_FREE_MEM,
		CORE_FREQ
	},
#if 1	
	/* CamLight_Config */
	{
		0,50,
		0,50,
	},
#else
	/* CamLight_Config */
	{
		{0},
	},
#endif	
	/*CamState */
	{
		ALG_GVSS,
		ALG_TRIGGER_DEF,
		ALG_IMG_SRC_DEF,
		IMG_ENCODE_DEF,
	},	
	CUR_LOG_DEFAULT,	
	/*** algparam ***/
	{
		0,
	},
};

typedef struct DetectRect_DynamicParams_Struture
{
	int MaxArraySize;//MAX_ARRAY_SIZE
	int MinHBlockRatio;//***height/ratio***
	int MaxHBlockRatio;//***height/ratio***
	float HBlockVarianceThreshold;
	float MinCodeUnitRatio;//***length*ratio***
	float MaxCodeUnitRatio;//***length*ratio***
	float MinCodeFlagRatio;//***length*ratio***
	float MaxCodeFlagRatio;//***length*ratio***
}DetectRect_DParams_Str;

static DetectRect_DParams_Str algDefaultParam = 
{
	50,
	70,
	10,
	5.0,
	0.8,
	1.2,
	0.8,
	1.2
};

/**
 * @brief	check magic number
 * @param	fp [I ]file pointer
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int check_magic_num(FILE *fp)
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
 * @brief	Add new log event to tail
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int AddLogToTail(LogEntry_t *pLog)
{
	LogData_t *pLogData, *pTail = NULL;
	DBG("pLog->tLogData.event=%s\n", pLog->event);
	DBG("pLog->tLogData.time=%s\n", asctime(& pLog->time));
	pLogData = (LogData_t*) malloc(sizeof(LogData_t));
	if(pLogData == NULL){
		DBG("No enough memory\n");
		if(gLogHead == NULL)
			return FAIL;
		/* Do Nothing if no more memory */
	} else {
		memcpy(&pLogData->tLogData, pLog, sizeof(LogEntry_t));
		pLogData->pNext = NULL;
		if(gLogHead == NULL){
			gLogHead = pLogData;
		} else {
			for(pTail = gLogHead;pTail->pNext != NULL;)
				pTail = pTail->pNext;
			pTail->pNext = pLogData;
		}
	}
	return SUCCESS;
}

/**
 * @brief	Add new log event
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int AddLog(LogEntry_t *pLog)
{
	LogData_t *pLogData, *pFrontData = NULL;
	DBG("pLog->tLogData.event=%s\n", pLog->event);
	DBG("pLog->tLogData.time=%s\n", asctime(& pLog->time));
	pLogData = (LogData_t*) malloc(sizeof(LogData_t));
	if(pLogData == NULL){
		DBG("No enough memory\n");
		if(gLogHead == NULL)
			return FAIL;
		/* If no more memory, replace the oldest one with current. */
		pLogData = gLogHead;
		while(pLogData->pNext != NULL){
			pFrontData = pLogData;
			pLogData = pLogData->pNext;
		}
		memcpy(&pLogData->tLogData, pLog, sizeof(LogEntry_t));
		if(pFrontData != NULL){
			pFrontData->pNext = NULL;
			pLogData->pNext = gLogHead;
			gLogHead = pLogData;
		}
	} else {
		memcpy(&pLogData->tLogData, pLog, sizeof(LogEntry_t));
		pLogData->pNext = gLogHead;
		gLogHead = pLogData;
	}
	DBG("gLogHead->tLogData.event=%s\n", gLogHead->tLogData.event);
	DBG("gLogHead->tLogData.time=%s\n", asctime(& gLogHead->tLogData.time));
	return SUCCESS;
}

/**
 * @brief	Show all system log
 * @return
 */
void ShowAllLog()
{
	LogData_t *ptr;
	for(ptr = gLogHead;ptr != NULL;ptr = ptr->pNext){
		fprintf(stderr, "Event:%s\n", ptr->tLogData.event);
		fprintf(stderr, "Time:%s\n", asctime(&ptr->tLogData.time));
	}
}
/**
 * @brief	Clean system log
 * @return
 */
void CleanLog()
{
	LogData_t *ptr;
	while(gLogHead != NULL){
		ptr = gLogHead;
		gLogHead = gLogHead->pNext;
		free(ptr);
	}
}

/**
 * @brief	read log from log file
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int ReadLog()
{
	FILE *fp;
	char Buffer[LOG_ENTRY_SIZE];
	int ret = SUCCESS, count = 0;
	if((fp = fopen(LOG_FILE, "rb")) == NULL){
		/* log file not exist */
		ret = FAIL;
	} else {
		if(check_magic_num(fp) == SUCCESS){
			while(count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM)
				if(fread(Buffer, 1, LOG_ENTRY_SIZE,fp) != LOG_ENTRY_SIZE){
					break;
				} else {
					if(AddLogToTail((LogEntry_t *)Buffer) != SUCCESS){
						ret = FAIL;
						break;
					}
					count++;
				}
		} else {
			ret = FAIL;
		}
		fclose(fp);
	}
	return ret;
}

/**
 * @brief	create log file
 * @param	"int nPageNum" : [IN]log page number
 * @param	"int nItemIndex" : [IN]log index in that page
 * @return	LogEntry_t*: log data
 */
LogEntry_t* GetLog(int nPageNum, int nItemIndex)
{
	LogData_t* pLog;
	int count, index = nPageNum * NUM_LOG_PER_PAGE + nItemIndex;
	if(nPageNum < 0 || nItemIndex < 0)
		return NULL;
	for(count = 0, pLog = gLogHead;(count < index) && (pLog != NULL);count ++)
		pLog = pLog->pNext;
	return &pLog->tLogData;
}

/**
 * @brief	create log file
 * @param	name [I ]File name to create in nand.
 * @param	pLogData  [I ]pointer to log list
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int create_log_file(char *name, LogData_t*pLogData){
	FILE *fp;
	int ret, count = 0;
	unsigned long MagicNum = MAGIC_NUM;

	char *logBufAddr = gLogBuffer;
	int writeSize;

#if 1
    // write the magic number into log buffer
	memcpy(logBufAddr,&MagicNum,sizeof(MagicNum));
	logBufAddr += sizeof(MagicNum);

    // write the log details into the log buffer
	while((pLogData != NULL) && (count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM))
	{
	    memcpy(logBufAddr,&pLogData->tLogData,LOG_ENTRY_SIZE);
	    logBufAddr += LOG_ENTRY_SIZE;
	    count ++;
	    pLogData = pLogData->pNext; // next log
	}

    writeSize = (count * LOG_ENTRY_SIZE) + sizeof(MagicNum);

    // write the contents of the log buffer to the file
	if((fp = fopen(name, "wb")) == NULL)
	{
	    ERROR("Can't create log file\n");
		ret = FAIL;
	}
	else
	{
        if(fwrite(gLogBuffer,1,writeSize,fp) != writeSize)
        {
		    ERROR("Writing log fail\n");
		    ret = FAIL;
		}
        else
        {
		    ret = SUCCESS;
		}

        fclose(fp);
	}

    return ret;
#else
	if((fp = fopen(name, "wb")) == NULL){
		ERROR("Can't create log file\n");
		ret = FAIL;
	} else {
		if(fwrite(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
			ERROR("Writing Magic Number fail\n");
			ret = FAIL;
		} else {
			ret = SUCCESS;
			while(pLogData != NULL &&
					count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM)
				if(fwrite(&pLogData->tLogData, 1, LOG_ENTRY_SIZE, fp) !=
						LOG_ENTRY_SIZE){
					ERROR("Writing log fail\n");
					ret = FAIL;
					break;
				} else {
					count++;
					pLogData = pLogData->pNext;
				}
		}
		fclose(fp);
	}
	return ret;
#endif
}

/**
 * @brief	create system file
 * @param	name [I ]File name to create in nand.
 * @param	Global [I ]Pointer to System information
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int create_sys_file(char *name, void *Global){
	FILE *fp;
	int ret;
	unsigned long MagicNum = MAGIC_NUM;
	if((fp = fopen(name, "wb")) == NULL){
		ERROR("Can't create system file\n");
		ret = FAIL;
	} else {
		if(fwrite(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
			ERROR("Writing Magic Number fail\n");
			ret = FAIL;
		} else {
			if(fwrite(Global, 1, SYS_ENV_SIZE, fp) != SYS_ENV_SIZE){
				ERROR("Writing global fail\n");
				ret = FAIL;
			} else {
				ret = SUCCESS;
			}
		}
		fclose(fp);

		/* must sync twice !? */
		system("sync");
		system("sync");
	}
	return ret;
}

int create_algParam_file(char *name, Alg_Params *Global)
{
	FILE *fp;
	int ret;
	unsigned long MagicNum = MAGIC_NUM;
	if((fp = fopen(name, "wb")) == NULL){
		ERROR("Can't create system file\n");
		ret = FAIL;
	} else {
		if(fwrite(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
			ERROR("Writing Magic Number fail\n");
			ret = FAIL;
		} 
		else 
		{
			if(fwrite((char *)Global, 1, Global->prmSize + sizeof(Alg_Params), fp) != (Global->prmSize + sizeof(Alg_Params)))
			{
					ERROR("Writing global fail\n");
					ret = FAIL;
			} 
			else 
			{
				ret = SUCCESS;
			}				
		}
		fclose(fp);

		/* must sync twice !? */
		system("sync");
		system("sync");
	}
	return ret;
}

/**
 * @brief	read SysInfo from system file
 * @param	"void *Buffer" : [OUT]buffer to store SysInfo
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int ReadGlobal(void *Buffer)
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

int ReadAlgParam(Alg_Params *Buffer)
{
	FILE *fp;
	int ret = SUCCESS;
	if((fp = fopen(ALG_PARAM_FILE, "rb")) == NULL){
		/* System file not exist */
		ret = FAIL;
	} else {
		if(check_magic_num(fp) == SUCCESS){
			fread(Buffer, 1, ALGPARAM_MAX_SIZE,fp);
		} else {
			ret = FAIL;
		}
		fclose(fp);
	}
	return ret;	
}

#define CONFIG_FILE "/mnt/config/NET.TXT"
/**
* @brief Write config data to file
*/
int WriteNetConfigFile(SysInfo *pSysInfo)
{
	int ret = 0;
	FILE *fp;
	char *filename = CONFIG_FILE;
	if ((fp = fopen(filename, "wb")) == NULL) {
		printf("Can't create config file\n");
		ret = -1;
	} else {
		fprintf(fp,"LocalIP=\"%s\"\n",pSysInfo->net.net_config.ip);
		fprintf(fp,"NetMask=\"%s\"\n",pSysInfo->net.net_config.netmask);
		fprintf(fp,"Gateway=\"%s\"\n",pSysInfo->net.net_config.gateway);
		fprintf(fp,"DNS=\"%s\"\n",pSysInfo->net.net_config.dns);
		fprintf(fp,"DHCPEnable=%d\n",pSysInfo->net.net_config.dhcp_enable);
		fprintf(fp,"MAC=\"%02x:%02x:%02x:%02x:%02x:%02x\"\n", pSysInfo->net.net_config.MAC[0], pSysInfo->net.net_config.MAC[1], pSysInfo->net.net_config.MAC[2], pSysInfo->net.net_config.MAC[3], pSysInfo->net.net_config.MAC[4], pSysInfo->net.net_config.MAC[5]);
		fclose(fp);
	}
	return ret;
}

int UpdateNetwork(void *Buffer)
{
	int ret = 0;
	char cmd[200];
	
	SysInfo *pSysInfo = (SysInfo *)Buffer;
#if 0	
	SysInfo *pInfo = (SysInfo *)malloc(sizeof(SysInfo));
	SysInfo *pData = (SysInfo *)Buffer;
	if(NULL == pData)
		return -1;
		
	if(NULL == pInfo)
		return -1;

	ret = ReadGlobal(pInfo);
	if(ret <0)
	{
		free(pInfo);
		return -1;
	}

	memcpy(&(pInfo->net),&(pData->net),sizeof(pData->net));

	ret |= create_sys_file(SYS_FILE, pInfo);
#endif		
	ret |= create_sys_file(SYS_FILE, pSysInfo);
	ret |= WriteNetConfigFile(pSysInfo);
	if(ret == 0)
	{
		system("sync");
		usleep(500*1000);
		sprintf(cmd,"cp %s %s",CONFIG_FILE,USB_CFG_FILE);
		printf("cmd=%s\n",cmd);
		system(cmd);
		system("sync");
		usleep(500*1000);	
	}
	
	printf("UpdateNetwork ret=%d\n",ret);

	return ret | system("sync");
}
int WriteAlgParam(Alg_Params *Buffer)
{
	int ret = 0;
	ret |= create_algParam_file(ALG_PARAM_FILE, Buffer);
    usleep(500*1000);
	return ret | system("sync");
}
/**
 * @brief	write SysInfo to system file
 * @param	"void *Buffer" : [IN]buffer of SysInfo
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int WriteGlobal(void *Buffer)
{
	int ret;
	ret = create_log_file(LOG_FILE, gLogHead);
	ret |= create_sys_file(SYS_FILE, Buffer);
    usleep(500*1000);
	return ret | system("sync");
}

int WriteGlobalLog(void *Buffer)
{
    return 	create_log_file(LOG_FILE, gLogHead);
}


int WriteGlobalSys(void *Buffer)
{
    return 	create_sys_file(SYS_FILE, Buffer);
}

static void itoa(unsigned short num,char *str)
{
	int power,j;
	j = num;
	for(power=1;j>=10;j/=10)
		power *= 10;
	for(;power>0;power/=10)
	{
		*str++ = '0' + num/power;
		num%=power;
	}	

	*str = '\0';
}
static void genrtspurl(SysInfo *pSysInfo,char *url)
{
	char port[10] = {0};
	
	strcat(url, RTSP_HEADER);
	strcat(url, pSysInfo->net.net_config.ip);
	strcat(url, ":");
	itoa(pSysInfo->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[0],port);
	strcat(url, port);
	strcat(url, RTSP_URL);
}
/**
 * @brief	file manager initialization
 * @param	ShareMem [O ]Pointer to share memory where system information will
 be stored.
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int FileMngInit(void *ShareMem)
{
	int ret;
	SysInfo *pSysInfo = ShareMem;
	DBG("Global value size:%d\n", SYS_ENV_SIZE);

	// Allocate log and Dmva log buffers
	gLogBuffer     = (char *)malloc(MAX_LOG_PAGE_NUM * (NUM_LOG_PER_PAGE + 1) * LOG_ENTRY_SIZE);
	if(gLogBuffer == NULL)
	{
	    ERROR("\n*** cannot allocate %d memory\n",MAX_LOG_PAGE_NUM * (NUM_LOG_PER_PAGE + 1) * LOG_ENTRY_SIZE);
	}

	ret = ReadGlobal(ShareMem);
	if(ret == FAIL){
		ret = create_sys_file(SYS_FILE, &SysInfoDefault);
		if(ret == SUCCESS)
			memcpy(ShareMem, &SysInfoDefault, SYS_ENV_SIZE);
		else
			ERROR("Initialize fail (FileMgnInit)\n");
	}

	ret = ReadAlgParam(&(pSysInfo->algPrm));
	if(ret == FAIL){
		pSysInfo->algPrm.prmSize = sizeof(algDefaultParam);
		memcpy(&(pSysInfo->algPrm.prm),&algDefaultParam,sizeof(algDefaultParam));
		ret = create_algParam_file(ALG_PARAM_FILE, &(pSysInfo->algPrm));
		if(ret != SUCCESS)
			ERROR("Initialize fail (FileMgnInit)\n");
	}

	/********* set rtsp url ****************/
	char rtspurl[120] = {0};
	genrtspurl(pSysInfo,rtspurl);
	memcpy(pSysInfo->net.rtsp_config.rtspUrl,rtspurl,RTSPURL_MAX_LEN);
	
	if(ret == SUCCESS){

		ret = ReadLog();
		if(ret == FAIL)
		{
			ret = create_log_file(LOG_FILE, NULL);
			if(ret == FAIL)
			    ERROR("create_log_file failed\n");
		}

#ifdef __FILE_DEBUG
		else
			ShowAllLog();
#endif
	}

	return ret;
}

/**
 * @brief	file manager exit
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int FileMngExit()
{
	CleanLog();

	// Free the log and Dmva log buffers
	free(gLogBuffer);

	return 0;
}

/**
 * @brief	file manager reset
 * @param	"void *ShareMem" : [IN]pointer to share memory
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int FileMngReset(void *ShareMem)
{
	int ret;
	SysInfo *pSysInfo = ShareMem;
	ret = ReadGlobal(ShareMem);
	if(ret == SUCCESS)
		ret = ReadAlgParam(&(pSysInfo->algPrm));
		
	return ret;
}


/* clear system and access logs */
int ClearSystemLog()
{
    LogData_t *pLog = gLogHead;
    LogData_t *pNextLog,*pPrevLog = NULL;
    int logListCount = 0;

    while(pLog != NULL)
    {
		if(strstr(pLog->tLogData.event,"login") != NULL)
		{
			if(logListCount == 0)
			    gLogHead = pLog;

            pNextLog    = pLog->pNext;
            pLog->pNext = NULL;
            if(pPrevLog != NULL)
            {
			    pPrevLog->pNext = pLog;
			}
            pPrevLog = pLog;

			pLog = pNextLog; // goto next entry
			logListCount ++;
		    continue;
		}

	    pNextLog = pLog->pNext;
	    free(pLog);
	    pLog = pNextLog;
	}

	if(logListCount == 0)
	    gLogHead = NULL;

    return 0;
}

int ClearAccessLog()
{
    LogData_t *pLog = gLogHead;
    LogData_t *pNextLog,*pPrevLog = NULL;
    int logListCount = 0;

    while(pLog != NULL)
    {
		if(strstr(pLog->tLogData.event,"login") == NULL)
		{
			if(logListCount == 0)
			    gLogHead = pLog;

            pNextLog    = pLog->pNext;
            pLog->pNext = NULL;
            if(pPrevLog != NULL)
            {
			    pPrevLog->pNext = pLog;
			}
            pPrevLog = pLog;

			pLog = pNextLog; // goto next entry
			logListCount ++;
		    continue;
		}

	    pNextLog = pLog->pNext;
	    free(pLog);
	    pLog = pNextLog;
	}

	if(logListCount == 0)
	    gLogHead = NULL;

    return 0;
}
