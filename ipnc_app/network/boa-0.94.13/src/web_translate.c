
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <file_list.h>
#include <web_translate.h>
#include <sys_env_type.h>
#include <file_msg_drv.h>
#include <net_config.h>
#include <ipnc_ver.h>
#include <Frame_rate_ctrl.h>

static char *nicname = "eth0";

ARG_HASH_TABLE *arg_hash;

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_ftpip(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->ftp_config.servier_ip);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_ftpipport(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->ftp_config.port);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_ftpuser(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->ftp_config.username);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_ftppassword(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->ftp_config.password);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_ftppapath(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->ftp_config.foldername);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_maxftpuserlen(char *data, char *arg)
{
	return sprintf(data, "%d", USER_LEN - 1);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_maxftppwdlen(char *data, char *arg)
{
	return sprintf(data, "%d", PASSWORD_LEN - 1);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_maxftppathlen(char *data, char *arg)
{
	return sprintf(data, "%d", MAX_FILE_NAME - 1);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_smtpip(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->smtp_config.servier_ip);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_smtpport(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->smtp_config.server_port);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_smtpauth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->smtp_config.authentication);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_smtpuser(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->smtp_config.username);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_maxsmtpuser(char *data, char *arg)
{
	return sprintf(data, "%d", USER_LEN - 1);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_smtppwd(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->smtp_config.password);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_maxsmtppwd(char *data, char *arg)
{
	return sprintf(data, "%d", PASSWORD_LEN - 1);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_smtpsender(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->smtp_config.sender_email);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_maxsmtpsender(char *data, char *arg)
{
	return sprintf(data, "%d", MAX_NAME_LENGTH - 1);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_emailuser(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->smtp_config.receiver_email);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_maxemailuserlen(char *data, char *arg)
{
	return sprintf(data, "%d", MAX_NAME_LENGTH - 1);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 * NEW FEATURE                                                             *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_ftppath(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->ftp_config.foldername);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_authorityadmin(char *data, char *arg)
{
	return sprintf(data, "%d", AUTHORITY_ADMIN);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_authorityoperator(char *data, char *arg)
{
	return sprintf(data, "%d", AUTHORITY_OPERATOR);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_authorityviewer(char *data, char *arg)
{
	return sprintf(data, "%d", AUTHORITY_VIEWER);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_user(char *data, char *arg)
{
	int i, count = 0;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	if ( *arg >= '0' && *arg <= '9' )
		return sprintf(data, "%s", pSysInfo->acounts[*arg - '0'].user);
	else if ( *arg == 'a' || *arg == '\0' ){
		for(i = 0; i< ACOUNT_NUM;i++)
			count += sprintf(data + count, "user%d:%s\n", i + 1, pSysInfo->acounts[i].user);
		return count;
	}
	return -1;
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_authority(char *data, char *arg)
{
	int i, count = 0;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	if ( *arg >= '0' && *arg <= '9' )
		return sprintf(data, "%d", pSysInfo->acounts[*arg - '0'].authority);
	else if ( *arg == 'a' || *arg == '\0' ){
		for(i = 0; i< ACOUNT_NUM;i++)
			count += sprintf(data + count, "authority%d:%d\n", i+1,
							pSysInfo->acounts[i].authority);
		return count;
	}
	return -1;
}
  /***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_smtpminattach(char *data, char *arg)
{
  return sprintf(data, "1");
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
 int para_smtpmaxattach(char *data, char *arg)
{
  return sprintf(data, "20");
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_motionxlimit(char *data, char *arg)
{
	return sprintf(data, "%d", 1280);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_motionxblock(char *data, char *arg)
{
	return sprintf(data, "%d", 4);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_motionyblock(char *data, char *arg)
{
	return sprintf(data, "%d", 3);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_dmva_event_location(char *data, char *arg)
{
	extern unsigned int dmva_event_location;

	return sprintf(data, "%d", dmva_event_location);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_get_dmva_event(char *data, char *arg)
{
	extern unsigned int dmva_event_location;
	DmvaLogEntry_t tDmvaLog;
	int i = atoi(arg) - 1 + dmva_event_location;
	struct tm* ptm;
	if(GetDmvaSysLog(0, i, &tDmvaLog)){
		/* No event */
		return sprintf(data, "");
	}
	ptm = &tDmvaLog.time;
	return sprintf(data, "%s:%d-%02d-%02d %02d:%02d:%02d %s",
	                tDmvaLog.eventListFileName,
					ptm->tm_year, ptm->tm_mon, ptm->tm_mday,
					ptm->tm_hour, ptm->tm_min, ptm->tm_sec, tDmvaLog.eventListEventDetails_sys);
}

/***************** Live Video Page ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_democfg(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->lan_config.democfg);
}

int para_democfgname(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if ( *arg == '0' )
	{
		return sprintf(data, "NONE");
	}
	else if ( *arg == 'a' || *arg == '\0' )
	{
		if(pSysInfo->lan_config.powerMode==IPNC_FULL_FEATURE_MODE)
		{
			if(pSysInfo->advanceMode.vnfMode>2)
			{
				return sprintf(data, "NONE;SMART ANALYTICS;ROI CENTER DEMO;VS DEMO;VNF DEMO;LDC DEMO");
			}
			else
			{
				return sprintf(data, "NONE;SMART ANALYTICS;ROI CENTER DEMO;VS DEMO;VNF DEMO");
			}
		}
		else
		{
			return sprintf(data, "NONE");
		}

	}

	return -1;
}

 int para_clicksnapfilename(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->lan_config.nClickSnapFilename);
}
 int para_clicksnapstorage(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nClickSnapStorage);
}
int para_streamname1(char *data, char *arg)
{
	NET_IPV4 ip;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	ip.int32 = net_get_ifaddr(nicname);

	return sprintf(data, "%s(%dx%d)@rtsp://%d.%d.%d.%d:%d/%s", pSysInfo->stream_config[0].name,
		pSysInfo->stream_config[0].width,pSysInfo->stream_config[0].height,
		ip.str[0], ip.str[1], ip.str[2], ip.str[3],
		pSysInfo->stream_config[0].portnum, pSysInfo->stream_config[0].portname);
}
int para_streamname2(char *data, char *arg)
{
	NET_IPV4 ip;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	ip.int32 = net_get_ifaddr(nicname);

	return sprintf(data, "%s(%dx%d)@rtsp://%d.%d.%d.%d:%d/%s", pSysInfo->stream_config[1].name,
		pSysInfo->stream_config[1].width,pSysInfo->stream_config[1].height,
		ip.str[0], ip.str[1], ip.str[2], ip.str[3],
		pSysInfo->stream_config[1].portnum, pSysInfo->stream_config[1].portname);
}
int para_streamname3(char *data, char *arg)
{
	NET_IPV4 ip;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	ip.int32 = net_get_ifaddr(nicname);

	return sprintf(data, "%s(%dx%d)@rtsp://%d.%d.%d.%d:%d/%s", pSysInfo->stream_config[2].name,
		pSysInfo->stream_config[2].width,pSysInfo->stream_config[2].height,
		ip.str[0], ip.str[1], ip.str[2], ip.str[3],
		pSysInfo->stream_config[2].portnum, pSysInfo->stream_config[2].portname);
}
int para_streamwidth1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->stream_config[0].width);
}
int para_streamwidth2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->stream_config[1].width);
}
int para_streamwidth3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->stream_config[2].width);
}

int para_streamheight1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->stream_config[0].height);
}
int para_streamheight2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->stream_config[1].height);
}
int para_streamheight3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->stream_config[2].height);
}


/***************** VIDEO / IMAGE PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_title(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->lan_config.title);
}
int para_videocodec(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nStreamType);
}

int para_videocodecname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Single");
	else if ( *arg == '1' )
		return sprintf(data, "Dual");
	else if ( *arg == '2' )
		return sprintf(data, "Tri-Stream");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Single;Dual;Tri-Stream");
	return -1;
}
int para_videocodeccombo(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nVideocombo);
}
int para_videocodeccomboname(char *data, char *arg)
{
	char *modename_list[] =
	{
		"H.264;MPEG4;MJPEG",
		"H.264+MJPEG;MPEG4+MJPEG;H264+H264;MPEG4+MPEG4;H264+MPEG4",
		"H264+H264+MJPEG;MPEG4+MPEG4+MJPEG"
	};
	int tblsize = sizeof(modename_list)/sizeof(modename_list[0]);
	int i = 0;
	char rtn_msg[1024] = "\0";
	if ( *arg == 'a' || *arg == '\0' )
	{
		for (i = 0; i < tblsize; i++)
		{
			if (i > 0)
				strcat(rtn_msg, "@");
			strcat(rtn_msg, modename_list[i]);
		}
		return sprintf(data, "%s", rtn_msg);
	}
	else if (*arg >= '0' && *arg <= '9')
	{
		i = atoi(arg);
		if (i < tblsize)
			return sprintf(data, "%s", modename_list[i]);
	}

	return -1;
}

int para_videocodecres(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nVideocodecres);
}

int para_videocodecresname(char *data, char *arg)
{
	char *resname_list[] =
	{
		"H264:1080P",
		"MPEG4:1080P",
		"MJPEG:1080P",
		"H264:1080P,MJPEG:D1",
		"MPEG4:1080P,MJPEG:D1",
		"H264:1080P,H264:D1;H264:1080P,H264:1080P",
		"MPEG4:1080P,MPEG4:D1",
		"H264:1080P,MPEG4:D1",
#ifdef IPNC_DM385
		"H264:1080P,H264:D1,MJPEG:1080P",
#else
#ifdef IPNC_LOW_POWER
#ifdef ROTATE_ON
		"H264:1080P,H264:D1,MJPEG:1080P;H264:5MP,H264:D1,MJPEG:5MP;H264:8MP,H264:D1,MJPEG:8MP",
#else
		"H264:1080P,H264:D1,MJPEG:1080P;H264:5MP,H264:D1,MJPEG:5MP;H264:8MP,H264:D1,MJPEG:8MP;H264:10MP,H264:D1,MJPEG:10MP",
#endif
#else
		"H264:1080P,H264:D1,MJPEG:1080P",
#endif
#endif
		"MPEG4:1080P,MPEG4:D1,MJPEG:1080P"
	};
	int tblsize = sizeof(resname_list)/sizeof(resname_list[0]);
	int i = 0;
	char rtn_msg[1024] = "\0";
	if ( *arg == 'a' || *arg == '\0' )
	{
		for (i = 0; i < tblsize; i++)
		{
			if (i > 0)
				strcat(rtn_msg, "@");
			strcat(rtn_msg, resname_list[i]);
		}
		return sprintf(data, "%s", rtn_msg);
	}
	else if (*arg >= '0' && *arg <= '9')
	{
		i = atoi(arg);
		if (i < tblsize)
			return sprintf(data, "%s", resname_list[i]);
	}

	return -1;
}

int para_mpeg4cvalue(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nMpeg41bitrate/1000);
}
int para_mpeg42cvalue(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nMpeg42bitrate/1000);
}
int para_livequality(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.njpegquality);
}
int para_framerate1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->lan_config.nFrameRate1);
}
int para_framerate2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nFrameRate2);
}
int para_framerate3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nFrameRate3);
}

#define MAX_FRAMERATE_SIZE	(10)
const char *dayfrmrate_list1_tbl[MAX_FRAMERATE_SIZE] =
{
	"60,30,15",
	"50,30,15",
	"5",
	"60,30,15",
	"50,30,15",
	"60,30,15;60,30,15",
	"50,30,15",
	"60,30,15",
#ifdef IPNC_DM385
	"60,30,15",
#else
#ifdef IPNC_LOW_POWER
	"60,30,15;Auto;Auto;Auto",
#else
	"60,30,15",
#endif
#endif
	"50,30,15"
};
const char *dayfrmrate_list2_tbl[MAX_FRAMERATE_SIZE] =
{
	"NA", "NA", "NA",
	"30,15",
	"25,15",
	"30,15;30,15",
	"25,15",
	"30,15",
#ifdef IPNC_DM385
	"30,15",
#else
#ifdef IPNC_LOW_POWER
	"30,15;Auto;Auto;Auto",
#else
	"30,15",
#endif
#endif
	"25,15"
};
const char *dayfrmrate_list3_tbl[MAX_FRAMERATE_SIZE] =
{
	"NA", "NA", "NA", "NA", "NA", "NA;NA",	"NA", "NA",
#ifdef IPNC_DM385
	"5",
#else
#ifdef IPNC_LOW_POWER
	"5;Auto;Auto;Auto",
#else
	"5",
#endif
#endif
	"5"
};
const char *nightfrmrate_list1_tbl[MAX_FRAMERATE_SIZE] =
{
	"60,30,15",
	"50,30,15",
	"5",
	"60,30,15",
	"50,30,15",
	"60,30,15;60,30,15",
	"50,30,15",
	"60,30,15",
#ifdef IPNC_DM385
	"60,30,15",
#else
#ifdef IPNC_LOW_POWER
	"60,30,15;Auto;Auto;Auto",
#else
	"60,30,15",
#endif
#endif
	"50,30,15"
};
const char *nightfrmrate_list2_tbl[MAX_FRAMERATE_SIZE] =
{
	"NA", "NA", "NA",
	"30,15",
	"25,15",
	"30,15;30,15",
	"25,15",
	"30,15",
#ifdef IPNC_DM385
	"30,15",
#else
#ifdef IPNC_LOW_POWER
	"30,15;Auto;Auto;Auto",
#else
	"30,15",
#endif
#endif
	"25,15"
};
const char *nightfrmrate_list3_tbl[MAX_FRAMERATE_SIZE] =
{
	"NA", "NA", "NA", "NA", "NA", "NA;NA",	"NA", "NA",
#ifdef IPNC_DM385
	"5",
#else
#ifdef IPNC_LOW_POWER
	"5;Auto;Auto;Auto",
#else
	"5",
#endif
#endif
	"5"
};

int para_frameratenameall1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	int i = 0;
	int tblsize;
	char rtn_msg[1024] = "\0";
	const char **nightfrmrate_list1 = nightfrmrate_list1_tbl;
	const char **dayfrmrate_list1 = dayfrmrate_list1_tbl;

	if (pSysInfo->lan_config.nDayNight == 0) {
		tblsize = MAX_FRAMERATE_SIZE;
		if ( *arg == 'a' || *arg == '\0' )
		{
			for (i = 0; i < tblsize; i++)
			{
				if (i > 0)
					strcat(rtn_msg, "@");
				strcat(rtn_msg, nightfrmrate_list1[i]);
			}
			return sprintf(data, "%s", rtn_msg);
		}
		else if (*arg >= '0' && *arg <= '9')
		{
			i = atoi(arg);
			if (i < tblsize)
				return sprintf(data, "%s", nightfrmrate_list1[i]);
		}
	}
	else {
		tblsize = MAX_FRAMERATE_SIZE;
		if ( *arg == 'a' || *arg == '\0' )
		{
			for (i = 0; i < tblsize; i++)
			{
				if (i > 0)
					strcat(rtn_msg, "@");
				strcat(rtn_msg, dayfrmrate_list1[i]);
			}
			return sprintf(data, "%s", rtn_msg);
		}
		else if (*arg >= '0' && *arg <= '9')
		{
			i = atoi(arg);
			if (i < tblsize)
				return sprintf(data, "%s", dayfrmrate_list1[i]);
		}
	}

	return -1;
}

int para_frameratenameall2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	int i = 0;
	int tblsize;
	char rtn_msg[1024] = "\0";
	const char **nightfrmrate_list2 = nightfrmrate_list2_tbl;
	const char **dayfrmrate_list2 = dayfrmrate_list2_tbl;

	if (pSysInfo->lan_config.nDayNight == 0) {
		tblsize = MAX_FRAMERATE_SIZE;
		if ( *arg == 'a' || *arg == '\0' )
		{
			for (i = 0; i < tblsize; i++)
			{
				if (i > 0)
					strcat(rtn_msg, "@");
				strcat(rtn_msg, nightfrmrate_list2[i]);
			}
			return sprintf(data, "%s", rtn_msg);
		}
		else if (*arg >= '0' && *arg <= '9')
		{
			i = atoi(arg);
			if (i < tblsize)
				return sprintf(data, "%s", nightfrmrate_list2[i]);
		}
	}
	else {
		tblsize = MAX_FRAMERATE_SIZE;
		if ( *arg == 'a' || *arg == '\0' )
		{
			for (i = 0; i < tblsize; i++)
			{
				if (i > 0)
					strcat(rtn_msg, "@");
				strcat(rtn_msg, dayfrmrate_list2[i]);
			}
			return sprintf(data, "%s", rtn_msg);
		}
		else if (*arg >= '0' && *arg <= '9')
		{
			i = atoi(arg);
			if (i < tblsize)
				return sprintf(data, "%s", dayfrmrate_list2[i]);
		}
	}


	return -1;
}

int para_frameratenameall3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	int i = 0;
	int tblsize;
	char rtn_msg[1024] = "\0";
	const char **nightfrmrate_list3 = nightfrmrate_list3_tbl;
	const char **dayfrmrate_list3 = dayfrmrate_list3_tbl;

	if (pSysInfo->lan_config.nDayNight == 0) {
		tblsize = MAX_FRAMERATE_SIZE;
		if ( *arg == 'a' || *arg == '\0' )
		{
			for (i = 0; i < tblsize; i++)
			{
				if (i > 0)
					strcat(rtn_msg, "@");
				strcat(rtn_msg, nightfrmrate_list3[i]);
			}
			return sprintf(data, "%s", rtn_msg);
		}
		else if (*arg >= '0' && *arg <= '9')
		{
			i = atoi(arg);
			if (i < tblsize)
				return sprintf(data, "%s", nightfrmrate_list3[i]);
		}
	}
	else {
		tblsize = MAX_FRAMERATE_SIZE;
		if ( *arg == 'a' || *arg == '\0' )
		{
			for (i = 0; i < tblsize; i++)
			{
				if (i > 0)
					strcat(rtn_msg, "@");
				strcat(rtn_msg, dayfrmrate_list3[i]);
			}
			return sprintf(data, "%s", rtn_msg);
		}
		else if (*arg >= '0' && *arg <= '9')
		{
			i = atoi(arg);
			if (i < tblsize)
				return sprintf(data, "%s", dayfrmrate_list3[i]);
		}
	}

	return -1;
}

int para_ratecontrol1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nRateControl1);
}
int para_ratecontrol2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nRateControl2);
}
int para_ratecontrolname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "VBR");
	else if ( *arg == '1' )
		return sprintf(data, "CBR");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "VBR;CBR");
	return -1;
}

 int para_datestampenable1(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[0].dstampenable);
}
 int para_datestampenable2(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[1].dstampenable);
}
 int para_datestampenable3(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[2].dstampenable);
}
 int para_timestampenable1(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[0].tstampenable);
}
 int para_timestampenable2(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[1].tstampenable);
}
 int para_timestampenable3(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[2].tstampenable);
}
  int para_logoenable1(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[0].nLogoEnable);
}
 int para_logoenable2(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[1].nLogoEnable);
}
 int para_logoenable3(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[2].nLogoEnable);
}
  int para_logoposition1(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[0].nLogoPosition);
}
 int para_logoposition2(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[1].nLogoPosition);
}
 int para_logoposition3(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[2].nLogoPosition);
}
 int para_logopositionname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Top-Left");
	else if ( *arg == '1' )
		return sprintf(data, "Top-Right");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Top-Left;Top-Right");
	return -1;
}
 int para_textenable1(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[0].nTextEnable);
}
 int para_textenable2(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[1].nTextEnable);
}
 int para_textenable3(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[2].nTextEnable);
}
  int para_textposition1(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[0].nTextPosition);
}
 int para_textposition2(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[1].nTextPosition);
}
 int para_textposition3(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[2].nTextPosition);
}
 int para_textpositionname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Top-Left");
	else if ( *arg == '1' )
		return sprintf(data, "Top-Right");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Top-Left;Top-Right");
	return -1;
}
 int para_overlaytext1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->osd_config[0].overlaytext);
}
  int para_overlaytext2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->osd_config[1].overlaytext);
}
int para_overlaytext3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->osd_config[2].overlaytext);
}
 int para_detailinfo1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[0].nDetailInfo);
}
  int para_detailinfo2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[1].nDetailInfo);
}
  int para_detailinfo3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->osd_config[2].nDetailInfo);
}
int para_mirctrl(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->lan_config.mirror);
}

int para_mirctrlname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == '1' )
		return sprintf(data, "HORIZONTAL");
	else if ( *arg == '2' )
		return sprintf(data, "VERTICAL");
	else if ( *arg == '3')
		return sprintf(data, "BOTH");
#ifdef ROTATE_ON
	else if ( *arg == '4')
		return sprintf(data, "270ROTATION");
	else if ( *arg == '5')
		return sprintf(data, "90ROTATION");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF;HORIZONTAL;VERTICAL;BOTH;270ROTATION;90ROTATION");
#else
		return sprintf(data, "OFF;HORIZONTAL;VERTICAL;BOTH");
#endif
	return -1;
}
 int para_encryption(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.encryptEnable);
}
 int para_localdisplay(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nLocalDisplay);
}
int para_localdisplayname(char *data, char *arg)
{
#ifdef IPNC_LOW_POWER
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF");
#else
	if ( *arg == '0' )
		return sprintf(data, "HDMI + NTSC");
	else if ( *arg == '1' )
		return sprintf(data, "HDMI + PAL");
	else if ( *arg == '2' )
		return sprintf(data, "OFF");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "HDMI + NTSC;HDMI + PAL;OFF");
#endif
	return -1;
}

 int para_aviformat(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.aviformat);
}
 int para_aviformatname(char *data, char *arg)
{
  	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	int supportMpeg4 = pSysInfo->lan_config.Supportstream2;
	int supportMpeg4cif = pSysInfo->lan_config.Supportstream3;
	int supportH264 = pSysInfo->lan_config.Supportstream5;
	int supportH264cif = pSysInfo->lan_config.Supportstream6;
	int mpeg41Xsize= pSysInfo->lan_config.Mpeg41Xsize;
	int mpeg41Ysize= pSysInfo->lan_config.Mpeg41Ysize;
	int mpeg42Xsize=pSysInfo->lan_config.Mpeg42Xsize;
	int mpeg42Ysize=pSysInfo->lan_config.Mpeg42Ysize;
	int avc1Xsize=pSysInfo->lan_config.Avc1Xsize;
	int avc1Ysize=pSysInfo->lan_config.Avc1Ysize;
	int avc2Xsize=pSysInfo->lan_config.Avc2Xsize;
	int avc2Ysize=pSysInfo->lan_config.Avc2Ysize;

	if ( supportH264 ) {
		if (supportH264cif){
			return sprintf(data,"H.264(%d x %d);H.264(%d x %d)", avc1Xsize,avc1Ysize,avc2Xsize,avc2Ysize);
		}
		else if(supportMpeg4cif){
			return sprintf(data,"H.264(%d x %d);MPEG4(%d x %d)",avc1Xsize,avc1Ysize,mpeg42Xsize,mpeg42Ysize);
		}
		else {
			return sprintf(data,"H.264(%d x %d)",avc1Xsize,avc1Ysize);
		}
	}
	else if (supportMpeg4){
		if (supportH264cif){
			return sprintf(data,"MPEG4(%d x %d);H.264(%d x %d)",mpeg41Xsize,mpeg41Ysize,avc2Xsize,avc2Ysize);
		}
		else if(supportMpeg4cif){
			return sprintf(data,"MPEG4(%d x %d);MPEG4(%d x %d)",mpeg41Xsize,mpeg41Ysize,mpeg42Xsize,mpeg42Ysize);
		}
		else {
			return sprintf(data,"MPEG4(%d x %d)",mpeg41Xsize,mpeg41Ysize);
		}
	}
	else{
		return sprintf(data,"NA");
	}

	return -1;
}

 int para_aviduration(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.aviduration);
}
 int para_avidurationname(char *data, char *arg)
{
  	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->sdcard_config.sdinsert==3){
		if( *arg == '0' )
			return sprintf(data, "AUTO");
		else if( *arg == 'a' || *arg == '\0')
			return sprintf(data, "AUTO");
	}
	else {
		if ( *arg == '0' )
			return sprintf(data, "5 sec");
		else if ( *arg == '1' )
			return sprintf(data, "10 sec");
		else if ( *arg == 'a' || *arg == '\0' )
			return sprintf(data, "5 sec;10 sec");
	}

	return -1;
}

/***************** VIDEO ADVANCED PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/

 int para_ipratio1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[0].ipRatio);
}
  int para_ipratio2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[1].ipRatio);
}
  int para_ipratio3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[2].ipRatio);
}
  int para_forceIframe1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[0].fIframe);
}
  int para_forceIframe2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[1].fIframe);
}
  int para_forceIframe3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[2].fIframe);
}
  int para_qpinit1(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[0].qpInit;
	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_qpinit2(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[1].qpInit;
	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_qpinit3(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[2].qpInit;
	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_qpmin1(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[0].qpMin;
	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_qpmin2(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[1].qpMin;
	if(pSysInfo->lan_config.codectype2==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype2==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_qpmin3(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[2].qpMin;
	if(pSysInfo->lan_config.codectype3==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype3==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
 int para_qpmax1(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[0].qpMax;
	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_qpmax2(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[1].qpMax;
	if(pSysInfo->lan_config.codectype2==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype2==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_qpmax3(char *data, char *arg)
{
	unsigned char value;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = pSysInfo->codec_advconfig[2].qpMax;
	if(pSysInfo->lan_config.codectype3==H264_CODEC)
		value = (value>51) ? 51:value;
	else if(pSysInfo->lan_config.codectype3==MPEG4_CODEC)
		value = (value>31) ? 31:value;

	value = (value==0) ? 1: value;

	return sprintf(data, "%d", value);
}
  int para_meconfig1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[0].meConfig);
}
  int para_meconfig2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[1].meConfig);
}
  int para_meconfig3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[2].meConfig);
}
 int para_meconfigname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "AUTO");
	else if ( *arg == '1' )
		return sprintf(data, "HIGH SPEED");
	else if ( *arg == '2' )
		return sprintf(data, "HIGH QUALITY");
	else if ( *arg == '3' )
		return sprintf(data, "SVC AUTO");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "AUTO;HIGH SPEED;HIGH QUALITY;SVC AUTO");
	return -1;
}
 int para_packetsize1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[0].packetSize);
}
  int para_packetsize2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[1].packetSize);
}
  int para_packetsize3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_advconfig[2].packetSize);
}
 int para_regionofinterestenable1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].numROI);
}
  int para_regionofinterestenable2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].numROI);
}
  int para_regionofinterestenable3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].numROI);
}
 int para_str1x1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[0].startx);
}
 int para_str1y1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[0].starty);
}
 int para_str1w1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[0].width);
}
 int para_str1h1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[0].height);
}
 int para_str1x2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[1].startx);
}
 int para_str1y2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[1].starty);
}
 int para_str1w2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[1].width);
}
 int para_str1h2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[1].height);
}
 int para_str1x3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[2].startx);
}
 int para_str1y3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[2].starty);
}
 int para_str1w3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[2].width);
}
 int para_str1h3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[0].roi[2].height);
}
 int para_str2x1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[0].startx);
}
 int para_str2y1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[0].starty);
}
 int para_str2w1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[0].width);
}
 int para_str2h1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[0].height);
}
 int para_str2x2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[1].startx);
}
 int para_str2y2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[1].starty);
}
 int para_str2w2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[1].width);
}
 int para_str2h2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[1].height);
}
 int para_str2x3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[2].startx);
}
 int para_str2y3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[2].starty);
}
 int para_str2w3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[2].width);
}
 int para_str2h3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[1].roi[2].height);
}
 int para_str3x1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[0].startx);
}
 int para_str3y1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[0].starty);
}
 int para_str3w1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[0].width);
}
 int para_str3h1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[0].height);
}
 int para_str3x2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[1].startx);
}
 int para_str3y2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[1].starty);
}
 int para_str3w2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[1].width);
}
 int para_str3h2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[1].height);
}
 int para_str3x3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[2].startx);
}
 int para_str3y3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[2].starty);
}
 int para_str3w3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[2].width);
}
 int para_str3h3(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->codec_roiconfig[2].roi[2].height);
}


/***************** VIDEO ANALYTICS PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/

int para_motionblock(char *data, char *arg)
{
  	SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->motion_config.motionblock);
}
int para_motion_blk(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;

	strncpy(data, (char*)pSysInfo->motion_config.motionblock, MOTION_BLK_LEN);
	return strlen(data);
}
int para_motionsensitivity(char *data, char *arg)
{
  SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->motion_config.motionlevel);

}
int para_motionname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "LOW");
	else if ( *arg == '1' )
		return sprintf(data, "MEDIUM");
	else if ( *arg == '2' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;MEDIUM;HIGH");

	return -1;
}
int para_motioncenable(char *data, char *arg)
{
  SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->motion_config.motioncenable);
}
int para_motioncvalue(char *data, char *arg)
{
  SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->motion_config.motioncvalue);
}
int para_fdetect(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->face_config.fdetect);
}
int para_fdetectname(char *data, char *arg)
{
#ifdef IPNC_LOW_POWER
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF");
#else
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == '1' )
		return sprintf(data, "DETECT");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF;DETECT");
#endif

	return -1;
}
int para_fdx(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.startX);
}
 int para_fdy(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.startY);
}
 int para_fdw(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.width);
}
 int para_fdh(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.height);
}
 int para_fdconflevel(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.fdconflevel);
}
 int para_fddirection(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.fddirection);
}
int para_fddirectionname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "UP");
	else if ( *arg == '1' )
		return sprintf(data, "LEFT");
	else if ( *arg == '2' )
		return sprintf(data, "RIGHT");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "UP;RIGHT;LEFT");
	return -1;
}
int para_frecognition(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->face_config.frecog);
}
int para_frecognitionname(char *data, char *arg)
{
#if 0
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == '1' )
		return sprintf(data, "RECOGNIZE USER");
	else if ( *arg == '2' )
		return sprintf(data, "REGISTER USER");
	else if ( *arg == '3' )
		return sprintf(data, "CLEAR ALL USERS");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF;RECOGNIZE USER;REGISTER USER;CLEAR ALL USERS");
#else
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF");
#endif

	return -1;
}
int para_frconflevel(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.frconflevel);
}
int para_frdatabase(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->face_config.frdatabase);
}
int para_privacymask(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->face_config.pmask);
}
int para_maskoptions(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

#ifdef IPNC_LOW_POWER
	return sprintf(data, "%d", 0);
#else
	return sprintf(data, "%d", pSysInfo->face_config.maskoption);
#endif
}
int para_maskoptionsname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "BLACK BOX");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "BLACK BOX");
	return -1;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

/***************** CAMERA PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_brightness(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nBrightness);
}
int para_contrast(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nContrast);
}
int para_saturation(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nSaturation);
}
int para_sharpness(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nSharpness);
}
int para_blc(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nBacklightControl);
}
int para_backlight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nBackLight);
}
int  para_backlightname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Min");
	else if ( *arg == '1' )
		return sprintf(data, "Mid");
	else if ( *arg == '2' )
		return sprintf(data, "Max");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Min;Mid;Max");
	return -1;
}
int para_dynrange(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->advanceMode.dynRange);
}
int para_dynrangename(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == '1' )
		return sprintf(data, "HIGH SPEED");
	else if ( *arg == '2' )
		return sprintf(data, "HIGH QUALITY");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF;HIGH SPEED;HIGH QUALITY");

	return -1;
}
  int para_dynrangestrength(char *data, char *arg)
 {
 	SysInfo* pSysInfo = GetSysInfo();
 	if(pSysInfo == NULL)
 		return -1;
 	return sprintf(data, "%d", pSysInfo->advanceMode.dynRangeStrength);
 }
   int para_dynrangestrengthname(char *data, char *arg)
 {
	if ( *arg == '0' )
		return sprintf(data, "LOW");
	else if ( *arg == '1' )
		return sprintf(data, "MEDIUM");
	else if ( *arg == '2' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' ) {
		return sprintf(data, "LOW;MEDIUM;HIGH");
	}
 	return -1;
 }

int para_awb(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nWhiteBalance);
}

int para_awbname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Auto");
	else if ( *arg == '1' )
		return sprintf(data, "Day_D65");
	else if ( *arg == '2' )
		return sprintf(data, "Day_D55");
	else if ( *arg == '3' )
		return sprintf(data, "Florescent");
	else if ( *arg == '4' )
		return sprintf(data, "Incandescent");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Auto;Day_D55;Day_D65;Florescent;Incandescent");

	return -1;
}

int para_daynight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nDayNight);
}
int para_daynightname(char *data, char *arg)
{
	char *namelist[] = {"night","day"};
	if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "%s;%s", namelist[0], namelist[1]);
	else if (*arg >= '0' && *arg <= '1') {
		int idx = atoi(arg);
		return sprintf(data, "%s", namelist[idx]);
	}
	return -1;
}
int para_histogram(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.histogram);
}
int para_binning(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nBinning);
}
int para_binningname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "%s", "BINNING");
#if 0
	else if ( *arg == '1' )
		return sprintf(data, "%s", "SKIPPING");
	else if ( *arg == '2' )
		return sprintf(data, "%s", "WINDOW");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "BINNING;SKIPPING;WINDOW");
#else
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "BINNING");
#endif
	return -1;
}
int para_image2a(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->lan_config.nAEWswitch);
}

int para_img2aname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "NONE");
	else if ( *arg == '1' )
		return sprintf(data, "APPRO");
	else if ( *arg == '2' )
		return sprintf(data, "TI");
	else if ( *arg == 'a' || *arg == '\0' ) {
		return sprintf(data, "NONE;APPRO;TI");
	}
	return -1;
}
int para_image2atype(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->lan_config.nAEWtype);
}

int para_img2atypename(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == '1' )
		return sprintf(data, "Auto Exposure");
	else if ( *arg == '2' )
		return sprintf(data, "Auto White Balance");
	else if ( *arg == '3' )
		return sprintf(data, "Auto Exposure + Auto White Balance");
	else if ( *arg == 'a' || *arg == '\0' ) {
		return sprintf(data, "OFF;Auto Exposure;Auto White Balance;Auto Exposure + Auto White Balance");
	}
	return -1;
}
int para_imagesource(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.net.imagesource);
}
 int para_exposurename(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "60hz flicker");
	else if ( *arg == '1' )
		return sprintf(data, "50hz flicker");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "60hz flicker;50hz flicker");

	return -1;
}
int para_priority(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.expPriority);
}

 int para_priorityname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Priortize FrameRate");
	else if ( *arg == '1' )
		return sprintf(data, "Priortize Exposure");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Priortize FrameRate;Priortize Exposure");
	return -1;
}
 int para_vidstb(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->advanceMode.vsEnable);
}
 int para_ldc(char *data, char *arg)
 {
 	SysInfo* pSysInfo = GetSysInfo();
 	if(pSysInfo == NULL)
 		return -1;
 	return sprintf(data, "%d", pSysInfo->advanceMode.ldcEnable);
 }
    int para_vnf(char *data, char *arg)
 {
     SysInfo* pSysInfo = GetSysInfo();

 	if(pSysInfo == NULL)
 		return -1;

 	return sprintf(data, "%d", pSysInfo->advanceMode.vnfEnable);
 }
 int para_vnfmode(char *data, char *arg)
 {
     SysInfo* pSysInfo = GetSysInfo();

 	if(pSysInfo == NULL)
 		return -1;
 	return sprintf(data, "%d", pSysInfo->advanceMode.vnfMode);
 }
    int para_vnfmodename(char *data, char *arg)
 {
	if ( *arg == '0' )
 		return sprintf(data, "HIGH SPEED: TNF ONLY");
	else if ( *arg == '1' )
 		return sprintf(data, "HIGH SPEED: SNF ONLY");
	else if ( *arg == '2' )
 		return sprintf(data, "HIGH SPEED: SNF+TNF");
 	else if ( *arg == '3' )
 		return sprintf(data, "HIGH QUALITY: TNF ONLY");
 	else if ( *arg == '4' )
 		return sprintf(data, "HIGH QUALITY: SNF ONLY");
 	else if ( *arg == '5' )
 		return sprintf(data, "HIGH QUALITY: SNF+TNF");
 	else if ( *arg == 'a' || *arg == '\0' )
 		return sprintf(data, "HIGH SPEED: TNF ONLY;HIGH SPEED: SNF ONLY;HIGH SPEED: SNF+TNF;HIGH QUALITY: TNF ONLY;HIGH QUALITY: SNF ONLY;HIGH QUALITY: SNF+TNF");

 	return -1;
 }
  int para_vnfstrength(char *data, char *arg)
 {
 	SysInfo* pSysInfo = GetSysInfo();
 	if(pSysInfo == NULL)
 		return -1;
 	return sprintf(data, "%d", pSysInfo->advanceMode.vnfStrength);
 }
   int para_vnfstrengthname(char *data, char *arg)
 {
	if ( *arg == '0' )
		return sprintf(data, "AUTO");
	else if ( *arg == '1' )
		return sprintf(data, "LOW");
	else if ( *arg == '2' )
		return sprintf(data, "MEDIUM");
	else if ( *arg == '3' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' ) {
		return sprintf(data, "AUTO;LOW;MEDIUM;HIGH");
	}

 	return -1;
 }
int para_framectrl(char *data, char *arg)
 {
     SysInfo* pSysInfo = GetSysInfo();

 	if(pSysInfo == NULL)
 		return -1;

 	return sprintf(data, "%d", pSysInfo->lan_config.framectrl);
 }
int para_framectrlname(char *data, char *arg)
 {
	if ( *arg == '0' )
 		return sprintf(data, "30");
 	else if ( *arg == '1' )
 		return sprintf(data, "60");
 	else if ( *arg == 'a' || *arg == '\0' )
 		return sprintf(data, "30;60");
 	return 0;
 }

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

/***************** AUDIO PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_audioenable(char *data, char *arg)
{
  SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.audioON);
}
 int para_audiomode(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.audiomode);
}
 int para_audiomodename(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Only Mic");
	else if ( *arg == '1' )
		return sprintf(data, "Only Speaker");
	else if ( *arg == '2' )
		return sprintf(data, "Both");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Only Mic;Only Speaker;Both Mic & Speaker");
	return -1;
}
  int para_audioinvolume(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.audioinvolume);
}
 int para_encoding(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.codectype);
}
  int para_encodingname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "G711");
#if 0
	if ( *arg == '1' )
		return sprintf(data, "AAC-LC");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "G711;AAC-LC");
#else
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "G711");
#endif
	return -1;
}
 int para_samplerate(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.samplerate);
}
  int para_sampleratename(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "8Khz");
	else if ( *arg == '1' )
		return sprintf(data, "16Khz");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "8Khz;16Khz");

	return -1;
}
 int para_audiobitrate(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.bitrate);
}
  int para_audiobitratename(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->audio_config.codectype==0) {
		if(pSysInfo->audio_config.samplerate==0) {
			if ( *arg == '0' )
				return sprintf(data, "AUTO: 64Kbps");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "AUTO: 64Kbps");
		}
		else if(pSysInfo->audio_config.samplerate==1) {
			if ( *arg == '0' )
				return sprintf(data, "AUTO: 128Kbps");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "AUTO: 128Kbps");
		}
	}
	else if(pSysInfo->audio_config.codectype==1) {
		if(pSysInfo->audio_config.samplerate==0) {
			if ( *arg == '0' )
				return sprintf(data, "24Kbps");
			else if ( *arg == '1' )
				return sprintf(data, "36Kbps");
			else if ( *arg == '2' )
				return sprintf(data, "48Kbps");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "24Kbps;36Kbps;48Kbps");
		}
		else if(pSysInfo->audio_config.samplerate==1) {
			if ( *arg == '0' )
				return sprintf(data, "32Kbps");
			else if ( *arg == '1' )
				return sprintf(data, "48Kbps");
			else if ( *arg == '2' )
				return sprintf(data, "64Kbps");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "32Kbps;48Kbps;64Kbps");
		}
	}

	return -1;
}
int para_audiobitratenameall(char *data, char *arg)
{
	char *resname_list[] =
	{
		"AUTO: 64Kbps",
		"24Kbps;48Kbps;64Kbps",
		"AUTO: 128Kbps",
		"40Kbps;64Kbps;96Kbps",
	};
	int tblsize = sizeof(resname_list)/sizeof(resname_list[0]);
	int i = 0;
	char rtn_msg[512] = "\0";
	if ( *arg == 'a' || *arg == '\0' )
	{
		for (i = 0; i < tblsize; i++)
		{
			if (i > 0)
				strcat(rtn_msg, "@");
			strcat(rtn_msg, resname_list[i]);
		}
		return sprintf(data, "%s", rtn_msg);
	}
	else if (*arg >= '0' && *arg <= '1')
	{
		i = atoi(arg);
		if (i < tblsize)
			return sprintf(data, "%s", resname_list[i]);
	}

	return -1;
}

 int para_alarmlevel(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.alarmlevel);
}
  int para_audiooutvolume(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.audiooutvolume);
}

  int para_audioreceiverenable(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.audiorecvenable);
}

  int para_audioserverip(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->audio_config.audioServerIp);
}

/***************** DATE TIME PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_date(char *data, char *arg)
{
	time_t tnow;
	struct tm *tmnow;

	tzset();
	time(&tnow);
	tmnow = localtime(&tnow);

	if ( *arg == '0' ) {
		return sprintf(data, "%d", tmnow->tm_year+1900);
	}
	else if ( *arg == '1' ) {
		return sprintf(data, "%d", tmnow->tm_mon+1);
	}
	else if ( *arg == '2' ) {
		return sprintf(data, "%d", tmnow->tm_mday);
	}
	else if ( *arg == '\0' )
		return sprintf(data, "%04d/%02d/%02d", tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday);
	return -1;
}
int para_time(char *data, char *arg)
{
	time_t tnow;
	struct tm *tmnow;

	tzset();
	time(&tnow);
	tmnow = localtime(&tnow);

	if ( *arg == '0' ) {
		return sprintf(data, "%d", tmnow->tm_hour);
	}
	else if ( *arg == '1' ) {
		return sprintf(data, "%d", tmnow->tm_min);
	}
	else if ( *arg == '2' ) {
		return sprintf(data, "%d", tmnow->tm_sec);
	}
	else if ( *arg == '\0' )
		return sprintf(data, "%02d:%02d:%02d", tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec);

	return -1;
}

int para_sntptimezone(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->lan_config.net.ntp_timezone);
}

char *TZname[] = {
	"GMT-12 Eniwetok, Kwajalein",
	"GMT-11 Midway Island, Samoa",
	"GMT-10 Hawaii",
	"GMT-09 Alaska",
	"GMT-08 Pacific Time (US & Canada), Tijuana",
	"GMT-07 Mountain Time (US & Canada), Arizona",
	"GMT-06 Central Time (US & Canada), Mexico City, Tegucigalpa, Saskatchewan",
	"GMT-05 Eastern Time (US & Canada), Indiana(East), Bogota, Lima",
	"GMT-04 Atlantic Time (Canada), Caracas, La Paz",
	"GMT-03 Brasilia, Buenos Aires, Georgetown",
	"GMT-02 Mid-Atlantic",
	"GMT-01 Azores, Cape Verdes Is.",
	"GMT+00 GMT, Dublin, Edinburgh, London, Lisbon, Monrovia, Casablanca",
	"GMT+01 Berlin, Stockholm, Rome, Bern, Brussels, Vienna, Paris, Madrid, Amsterdam, Prague, Warsaw, Budapest",
	"GMT+02 Athens, Helsinki, Istanbul, Cairo, Eastern Europe, Harare, Pretoria, Israel",
	"GMT+03 Baghdad, Kuwait, Nairobi, Riyadh, Moscow, St. Petersburg, Kazan, Volgograd",
	"GMT+04 Abu Dhabi, Muscat, Tbilisi",
	"GMT+05 Islamabad, Karachi, Ekaterinburg, Tashkent",
	"GMT+06 Alma Ata, Dhaka",
	"GMT+07 Bangkok, Jakarta, Hanoi",
	"GMT+08 Taipei, Beijing, Chongqing, Urumqi, Hong Kong, Perth, Singapore",
	"GMT+09 Tokyo, Osaka, Sapporo, Seoul, Yakutsk",
	"GMT+10 Brisbane, Melbourne, Sydney, Guam, Port Moresby, Vladivostok, Hobart",
	"GMT+11 Magadan, Solomon Is., New Caledonia",
	"GMT+12 Fiji, Kamchatka, Marshall Is., Wellington, Auckland"
};
char *DMVAEnvProfileDetails[] = {
	"The size of moving objects varies substantially within the camera’s field of view",
	"Glare from sunlight and other bright light sources may appear in the camera’s field of view",
	"Camera’s automatic gain control is ON",
	"Camera is prone to substantial vibration",
	"The ground plane is visible in the camera’s field of view",
	"Foliage appears in the scene",
	"The sky appears in the scene"
};


int para_timezonename(char *data, char *arg)
{
#if 1
	if ( *arg == '\0' ) {
		SysInfo* pSysInfo = GetSysInfo();
		if(pSysInfo == NULL)
			return -1;
		if (pSysInfo->lan_config.net.ntp_timezone <= 24)
			return sprintf(data, "%s", TZname[pSysInfo->lan_config.net.ntp_timezone]);
		return -1;
	}
	else {
		int tz = atoi(arg);
		if ((tz >= 0) && (tz <= 24))
			return sprintf(data, "%s", TZname[tz]);
		return -1;
	}
#else

	int i, tblsize = sizeof(TZname)/sizeof(TZname[0]);
	char timezone_msg[2048] = "\0";

	if ( *arg == 'a' || *arg == '\0' )
	{
		for (i = 0; i < tblsize; i++)
		{
			if (i > 0)
				strcat(timezone_msg, "@");

			strcat(timezone_msg, TZname[i]);

		}

		return sprintf(data, "%s", timezone_msg);
	}

	return -1;

#endif
}

int para_daylight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.net.daylight_time);
}
int para_dateformat(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.dateformat);
}
int para_dateformatname(char *data, char *arg)
{
  if ( *arg == '0' )
		return sprintf(data, "YYYY/MM/DD");
	else if ( *arg == '1' )
		return sprintf(data, "MM/DD/YYYY");
	else if ( *arg == '2' )
		return sprintf(data, "DD/MM/YYYY");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "YYYY/MM/DD;MM/DD/YYYY;DD/MM/YYYY");
	return -1;
}
int para_tstampformat(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.tstampformat);
}
int para_tstampformatname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "12Hrs");
	else if ( *arg == '1' )
		return sprintf(data, "24Hrs");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "12Hrs;24Hrs");
	return -1;
}
int para_dateposition(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.dateposition);
}
 int para_timeposition(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.timeposition);
}
 int para_datetimepositionname(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "Bottom-Left");
	else if ( *arg == '1' )
		return sprintf(data, "Bottom-Right");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "Bottom-Left;Bottom-Right");
	return -1;
}
/***************** NETWORK & PORT ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
 int para_dhcpenable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.net.dhcp_enable);
}

int para_netip(char *data, char *arg)
{
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(nicname);

	if ( *arg == '0' )
		return sprintf(data, "%d", ip.str[0]);
	else if ( *arg == '1' )
		return sprintf(data, "%d", ip.str[1]);
	else if ( *arg == '2' )
		return sprintf(data, "%d", ip.str[2]);
	else if ( *arg == '3' )
		return sprintf(data, "%d", ip.str[3]);
	else if ( *arg == '\0' )
		return sprintf(data, "%03d.%03d.%03d.%03d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	return -1;
}

int para_netmask(char *data, char *arg)
{
	NET_IPV4 mask;
	mask.int32 = net_get_netmask(nicname);

	if ( *arg == '0' )
		return sprintf(data, "%d", mask.str[0]);
	else if ( *arg == '1' )
		return sprintf(data, "%d", mask.str[1]);
	else if ( *arg == '2' )
		return sprintf(data, "%d", mask.str[2]);
	else if ( *arg == '3' )
		return sprintf(data, "%d", mask.str[3]);
	else if ( *arg == '\0' )
		return sprintf(data, "%03d.%03d.%03d.%03d", mask.str[0], mask.str[1], mask.str[2], mask.str[3]);
	return -1;
}

int para_gateway(char *data, char *arg)
{
	NET_IPV4 gateway;
	gateway.int32 = net_get_gateway();

	if ( *arg == '0' )
		return sprintf(data, "%d", gateway.str[0]);
	else if ( *arg == '1' )
		return sprintf(data, "%d", gateway.str[1]);
	else if ( *arg == '2' )
		return sprintf(data, "%d", gateway.str[2]);
	else if ( *arg == '3' )
		return sprintf(data, "%d", gateway.str[3]);
	else if ( *arg == '\0' )
		return sprintf(data, "%03d.%03d.%03d.%03d", gateway.str[0], gateway.str[1], gateway.str[2], gateway.str[3]);
	return -1;
}

int para_dnsip(char *data, char *arg)
{
	NET_IPV4 dns;
	dns.int32 = net_get_dns();

	if ( *arg == '0' )
		return sprintf(data, "%d", dns.str[0]);
	else if ( *arg == '1' )
		return sprintf(data, "%d", dns.str[1]);
	else if ( *arg == '2' )
		return sprintf(data, "%d", dns.str[2]);
	else if ( *arg == '3' )
		return sprintf(data, "%d", dns.str[3]);
	else if ( *arg == '\0' )
		return sprintf(data, "%03d.%03d.%03d.%03d", dns.str[0], dns.str[1], dns.str[2], dns.str[3]);
	return -1;
}

int para_multicast(char *data, char *arg)
{
  SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.net.multicast_enable);
}

int para_sntpip(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%s", pSysInfo->lan_config.net.ntp_server);
}

int para_httpport(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.net.http_port);
}

int para_httpsport(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.net.https_port);
}
int para_giointype(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.giointype);
}
 int para_gioinname(char *data, char *arg)
{
  if ( *arg == '0' )
		return sprintf(data, "LOW");
	else if ( *arg == '1' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;HIGH");
	return -1;
}
int para_gioouttype(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.gioouttype);
}
 int para_giooutname(char *data, char *arg)
{
  if ( *arg == '0' )
		return sprintf(data, "LOW");
	else if ( *arg == '1' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;HIGH");
	return -1;
}
 int para_rs485(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.rs485config);
}
 int para_rs485name(char *data, char *arg)
{
#if 0
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == '1' )
		return sprintf(data, "PTZ");
	else if ( *arg == '1' )
		return sprintf(data, "ALARM");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF;PTZ;ALARM");
#else
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF");
#endif
	return -1;
}

/*************************** ALARM PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
 int para_alarmenable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nAlarmEnable);
}
int para_alarmduration(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nAlarmDuration);
}
int para_recordduration(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "10 seconds");
	else if ( *arg == '1' )
		return sprintf(data, "30 seconds");
	else if ( *arg == '2' )
		return sprintf(data, "1 minute");
	else if ( *arg == '3' )
		return sprintf(data, "5 minutes");
	else if ( *arg == '4' )
		return sprintf(data, "10 minutes");
	else if ( *arg == '5' )
		return sprintf(data, "Non-Stop");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "10 seconds;30 seconds;1 minute;5 minutes;10 minutes;Non-Stop");
	return -1;
}
int para_motionenable(char *data, char *arg)
{
  SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->motion_config.motionenable);
}
int para_lostalarm(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.lostalarm);
}
 int para_audioalarm(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->audio_config.alarmON);
}

int para_extalarm(char *data, char *arg)
{
  SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nExtAlarm);
}
int para_gioinenable(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.gioinenable);
}
 int para_giooutenable(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.giooutenable);
}

  int para_exttriggername(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "OFF");
	else if ( *arg == '1' )
		return sprintf(data, "ON");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "OFF;ON");
	return -1;
}
int para_aftpenable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.bAFtpEnable);
}
 int para_ftpfileformat(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->ftp_config.ftpfileformat);
}

int para_asmtpenable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.bASmtpEnable);
}
  int para_attfileformat(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->smtp_config.attfileformat);
}
int para_asmtpattach(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->smtp_config.attachments);
}
 int para_formatName(char *data, char *arg)
{
	int supportMpeg4=0,supportMpeg4cif=0 ,supportH264=0, supportH264cif=0, supportJPG=0;
	int supportAVI=0;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	supportJPG 		= pSysInfo->lan_config.Supportstream1;
	supportMpeg4 	= pSysInfo->lan_config.Supportstream2;
	supportMpeg4cif = pSysInfo->lan_config.Supportstream3;
	supportH264 	= pSysInfo->lan_config.Supportstream5;
	supportH264cif 	= pSysInfo->lan_config.Supportstream6;

	supportAVI = supportH264|supportH264cif|supportMpeg4|supportMpeg4cif;

	if (pSysInfo->sdcard_config.sdinsert==3) {
		if(supportJPG) {
			if ( *arg == '0' )
				return sprintf(data, "JPEG");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "JPEG");
		}
		else {
			return sprintf(data, "NA");
		}
	}
	else {
		if ((supportAVI&&supportJPG)){
			if ( *arg == '0' )
				return sprintf(data, "AVI");
			else if(*arg =='1')
				return sprintf(data,"JPEG");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "AVI;JPEG");
		}
		else if (supportAVI){
			if ( *arg == '0' )
				return sprintf(data, "AVI");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "AVI");
		}
		else if (supportJPG){
			if ( *arg == '0' )
				return sprintf(data, "JPEG");
			else if ( *arg == 'a' || *arg == '\0' )
				return sprintf(data, "JPEG");
		}
	}

	return -1;
}

int para_sdaenable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.bSdaEnable);
}
 int para_sdfileformat(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->sdcard_config.sdfileformat);
}
 int para_sdformatName(char *data, char *arg)
{
	int supportMpeg4=0,supportMpeg4cif=0 ,supportH264=0, supportH264cif=0, supportJPG=0;
	int supportAVI=0;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	supportJPG 		= pSysInfo->lan_config.Supportstream1;
	supportMpeg4 	= pSysInfo->lan_config.Supportstream2;
	supportMpeg4cif = pSysInfo->lan_config.Supportstream3;
	supportH264 	= pSysInfo->lan_config.Supportstream5;
	supportH264cif 	= pSysInfo->lan_config.Supportstream6;

	supportAVI = supportH264|supportH264cif|supportMpeg4|supportMpeg4cif;

    if ((supportAVI&&supportJPG)){
		if ( *arg == '0' )
			return sprintf(data, "AVI");
		else if(*arg =='1')
			return sprintf(data,"JPEG");
		else if ( *arg == 'a' || *arg == '\0' )
			return sprintf(data, "AVI;JPEG");
	}
	else if (supportAVI){
		if ( *arg == '0' )
			return sprintf(data, "AVI");
		else if ( *arg == 'a' || *arg == '\0' )
			return sprintf(data, "AVI");
	}
	else if (supportJPG){
		if ( *arg == '0' )
			return sprintf(data, "JPEG");
		else if ( *arg == 'a' || *arg == '\0' )
			return sprintf(data, "JPEG");
	}

	return -1;
}
 int para_alarmlocalstorage(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.alarmlocal);
}
 int para_alarmaudioplay(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nAlarmAudioPlay);
}
 int para_alarmaudiofile(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nAlarmAudioFile);
}
  int para_alarmaudiofilename(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "alarm_1.wav");
	if ( *arg == '1' )
		return sprintf(data, "alarm_2.wav");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "alarm_1.wav;alarm_2.wav");
	return -1;
}

/***************** STORAGE PAGE ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/

int para_rftpenable(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->ftp_config.rftpenable);
}
 int para_sdrenable(char *data, char *arg)
{
    SysInfo* pSysInfo = GetSysInfo();

	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->sdcard_config.sdrenable);
}
int para_schedule(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	Schedule_t *pSchedule;
	int ret = 0, i, arg_i = atoi(arg);

	if(pSysInfo == NULL)
		return -1;
	 if ( *arg == 'a' || *arg == '\0' ){
		for(i = 0; i < SCHDULE_NUM; i ++){
			pSchedule = &(pSysInfo->lan_config.aSchedules[i]);
			ret += sprintf(data + ret,
				"%02d%d%02d%02d%02d%02d%02d%02d%02d\n", i,
				pSchedule -> bStatus, pSchedule -> nDay,
				pSchedule -> tStart.nHour, pSchedule -> tStart.nMin,
				pSchedule -> tStart.nSec, pSchedule -> tDuration.nHour,
				pSchedule -> tDuration.nMin, pSchedule -> tDuration.nSec);
		}
		return ret;
	} else if (arg_i >= 0 && arg_i < SCHDULE_NUM){
		pSchedule = &(pSysInfo->lan_config.aSchedules[arg_i]);
		return sprintf(data, "%d%02d%02d%02d%02d%02d%02d%02d",
				pSchedule -> bStatus, pSchedule -> nDay,
				pSchedule -> tStart.nHour, pSchedule -> tStart.nMin,
				pSchedule -> tStart.nSec, pSchedule -> tDuration.nHour,
				pSchedule -> tDuration.nMin, pSchedule -> tDuration.nSec);
	}
	return -1;
}
 int para_recordlocalstorage(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.recordlocal);
}

 int para_schedulerepeatenable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nScheduleRepeatEnable);
}
 int para_schedulenumweeks(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nScheduleNumWeeks);
}
 int para_scheduleinfiniteenable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->lan_config.nScheduleInfiniteEnable);
}

/***************** Support Page ***********************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_kernelversion(char *data, char *arg)
{
	FILE *fptr;
	char TempBuff[80];
	char *pStr[3];
	int buffsize = sizeof(TempBuff);
	int cnt = 0;

	fptr=fopen("/proc/version","r");
	if (fptr==NULL){
	fprintf(stderr,"\n Can't get Version information ");
	return -1;
	}
	fread( TempBuff, buffsize-1, 1,fptr);
	TempBuff[buffsize-1] = '\0';

	pStr[0] = strtok(TempBuff, " ");

	for( cnt = 1;cnt < 3;cnt++ )
	{
		pStr[cnt] = strtok(NULL, " ");
	}
	fclose(fptr);
	return sprintf(data, "%s %s %s ",pStr[0],pStr[1],pStr[2]);

}

int para_biosversion(char *data, char *arg)
{
	int fd0;
	size_t size, cnt = 0, length;
	char readbuff[2048], *strptr, *target;
	fd0 = open("/dev/mtd2", O_RDONLY);
	if (fd0 == -1) {
		dbg("=======Open mtd2 failed========\n");
		return sprintf(data, "Open mtd2 failed");
	}

	size = read(fd0, readbuff, 2048);
	dbg("#######size = %d\n", size);
	readbuff[1023] = '\0';
	strptr = readbuff;
	while (cnt < size) {
		length = strlen(strptr) + 1;
		if (strncmp(strptr, "ver=", 4) == 0)
			break;
		strptr += length;
		cnt += length;
	}

	target = strtok(strptr, "=");
	target = strtok(NULL, "=");

	close(fd0);
	return sprintf(data, "%s", target);
}

int para_softwareversion(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", IPNC_APP_VERSION);
}

/************************** SDCARD EXPLORER PAGE **************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_sdinsert(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	return sprintf(data, "%d", pSysInfo->sdcard_config.sdinsert);
}
 int para_sdleft(char *data, char *arg)
{
  long long freespace;
  freespace=GetDiskfreeSpace("/mnt/mmc/");

  return sprintf(data, "%lld",freespace*1024);
}
 int para_sdused(char *data, char *arg)
{
  long long usedspace;
  usedspace=GetDiskusedSpace("/mnt/mmc/");
  return sprintf(data,"%lld",usedspace*1024);
}
/************************** Others ****************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_reloadflag(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->lan_config.reloadFlag);
}

int para_reloadtime(char *data, char *arg)
{
	return sprintf(data, "%d", 28);
}


int para_maxaccount(char *data, char *arg)
{
	return sprintf(data, "%d", ACOUNT_NUM);
}

int para_minnamelen(char *data, char *arg)
{
	return sprintf(data, "%d", MIN_USER_LEN);
}

int para_maxnamelen(char *data, char *arg)
{
	return sprintf(data, "%d", USER_LEN);
}

int para_minpwdlen(char *data, char *arg)
{
	return sprintf(data, "%d", MIN_PASSWORD_LEN);
}

int para_maxpwdlen(char *data, char *arg)
{
	return sprintf(data, "%d", PASSWORD_LEN);
}

int para_bkupfirmware(char *data, char *arg)
{
	return sprintf(data, "%d", 0);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

int para_dmvaEnable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaenable_sys);
}

int para_dmvaExptAlgoFrameRate(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaexptalgoframerate_sys);
}

int para_dmvaExptAlgoDetectFreq(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaexptalgodetectfreq_sys);
}

int para_dmvaExptEvtRecordEnable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaexptevtrecordenable_sys);
}

/************************** Set DMVA Trip Zone specific Parameters *********/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int para_dmvaTZSensitivity(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZSensitivity_sys);
}
int para_dmvaTZPersonMinWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZPersonMinWidth_sys);
}

int para_dmvaTZPersonMinHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZPersonMinHeight_sys);
}
int para_dmvaTZVehicleMinWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZVehicleMinWidth_sys);
}
int para_dmvaTZVehicleMinHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZVehicleMinHeight_sys);
}

int para_dmvaTZPersonMaxWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZPersonMaxWidth_sys);
}
int para_dmvaTZPersonMaxHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZPersonMaxHeight_sys);
}

int para_dmvaTZVehicleMaxWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZVehicleMaxWidth_sys);
}
int para_dmvaTZVehicleMaxHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZVehicleMaxHeight_sys);
}
int para_dmvaTZDirection(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZDirection_sys);
}
int para_dmvaTZPresentAdjust(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZPresentAdjust_sys);
}

int para_dmvaTZEnable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZEnable_sys);
}

int para_dmvaTZZone1_ROI_numSides(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_ROI_numSides_sys);
}

int para_dmvaTZZone2_ROI_numSides(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_ROI_numSides_sys);
}





int para_dmvaTZZone1_x01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[1]);
}
int para_dmvaTZZone1_x02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[2]);
}
int para_dmvaTZZone1_x03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[3]);
}
int para_dmvaTZZone1_x04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[4]);
}
int para_dmvaTZZone1_x05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[5]);
}
int para_dmvaTZZone1_x06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[6]);
}
int para_dmvaTZZone1_x07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[7]);
}
int para_dmvaTZZone1_x08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[8]);
}
int para_dmvaTZZone1_x09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[9]);
}
int para_dmvaTZZone1_x10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[10]);
}
int para_dmvaTZZone1_x11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[11]);
}
int para_dmvaTZZone1_x12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[12]);
}
int para_dmvaTZZone1_x13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[13]);
}
int para_dmvaTZZone1_x14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[14]);
}
int para_dmvaTZZone1_x15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[15]);
}
int para_dmvaTZZone1_x16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_x[16]);
}

int para_dmvaTZZone1_y01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[1]);
}
int para_dmvaTZZone1_y02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[2]);
}
int para_dmvaTZZone1_y03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[3]);
}
int para_dmvaTZZone1_y04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[4]);
}
int para_dmvaTZZone1_y05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[5]);
}
int para_dmvaTZZone1_y06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[6]);
}
int para_dmvaTZZone1_y07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[7]);
}
int para_dmvaTZZone1_y08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[8]);
}
int para_dmvaTZZone1_y09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[9]);
}
int para_dmvaTZZone1_y10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[10]);
}
int para_dmvaTZZone1_y11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[11]);
}
int para_dmvaTZZone1_y12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[12]);
}
int para_dmvaTZZone1_y13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[13]);
}
int para_dmvaTZZone1_y14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[14]);
}
int para_dmvaTZZone1_y15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[15]);
}
int para_dmvaTZZone1_y16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone1_y[16]);
}


int para_dmvaTZZone2_x01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[1]);
}
int para_dmvaTZZone2_x02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[2]);
}
int para_dmvaTZZone2_x03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[3]);
}
int para_dmvaTZZone2_x04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[4]);
}
int para_dmvaTZZone2_x05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[5]);
}
int para_dmvaTZZone2_x06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[6]);
}
int para_dmvaTZZone2_x07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[7]);
}
int para_dmvaTZZone2_x08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[8]);
}
int para_dmvaTZZone2_x09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[9]);
}
int para_dmvaTZZone2_x10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[10]);
}
int para_dmvaTZZone2_x11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[11]);
}
int para_dmvaTZZone2_x12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[12]);
}
int para_dmvaTZZone2_x13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[13]);
}
int para_dmvaTZZone2_x14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[14]);
}
int para_dmvaTZZone2_x15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[15]);
}
int para_dmvaTZZone2_x16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_x[16]);
}

int para_dmvaTZZone2_y01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[1]);
}
int para_dmvaTZZone2_y02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[2]);
}
int para_dmvaTZZone2_y03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[3]);
}
int para_dmvaTZZone2_y04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[4]);
}
int para_dmvaTZZone2_y05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[5]);
}
int para_dmvaTZZone2_y06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[6]);
}
int para_dmvaTZZone2_y07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[7]);
}
int para_dmvaTZZone2_y08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[8]);
}
int para_dmvaTZZone2_y09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[9]);
}
int para_dmvaTZZone2_y10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[10]);
}
int para_dmvaTZZone2_y11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[11]);
}
int para_dmvaTZZone2_y12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[12]);
}
int para_dmvaTZZone2_y13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[13]);
}
int para_dmvaTZZone2_y14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[14]);
}
int para_dmvaTZZone2_y15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[15]);
}
int para_dmvaTZZone2_y16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZZone2_y[16]);
}

/* DMVA TZ save/load params */
int para_dmvaTZSave(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaAppTZSave_sys);
}

int para_dmvaTZLoad(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppTZLoad_sys);
}

int para_dmvaTZLoadNames(char *data, char *arg)
{
	int i;
	char *tmpData = data;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

    if(pSysInfo->dmva_config.dmvaSaveLoadTZ.numRules == 0)
    {
	    sprintf(tmpData,"");
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadTZ.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadTZ.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadTZ.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadTZ.ruleName[i]);
		}
    }

	return 0;
}

  /* DMVA MAIN ANALYTICS PAGE */
int para_dmvaCamID(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppCamID_sys);
}
int para_dmvaGetSchedule(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppGetSchedule_sys);
}
int para_dmvaCfgFD(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppCfgFD_sys);
}
int para_dmvaCfgCTDImdSmetaTzOc(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppCfgCTDImdSmetaTzOc_sys);
}
int para_dmvaDisplayOptions(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppDisplayOptions_sys);
}
int para_dmvaEventRecordingVAME(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventRecordingVAME_sys);
}
int para_dmvaEventListActionPlaySendSearchTrash(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys);
}
int para_dmvaEventListSelectEvent(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListSelectEvent_sys);
}
int para_dmvaEventListArchiveFlag(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->tCurDmvaLog.eventListArchiveFlag_sys);
}
int para_dmvaEventListEventType(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->tCurDmvaLog.eventListEventType_sys);
}
  /* DMVA CTD PAGE */
int para_dmvaCfgTDEnv1(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppCfgTDEnv1_sys);
}
int para_dmvaCfgTDEnv2(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppCfgTDEnv2_sys);
}
int para_dmvaCfgTDSensitivity(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppCfgTDSensitivity_sys);
}

int para_dmvaCfgTDSensitivityName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "LOW");
	else if ( *arg == '2' )
		return sprintf(data, "LOW_TO_MODERATE");
	else if ( *arg == '3' )
		return sprintf(data, "MODERATE");
	else if ( *arg == '4' )
		return sprintf(data, "MODERATE_TO_HIGH");
	else if ( *arg == '5' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;LOW_TO_MODERATE;MODERATE;MODERATE_TO_HIGH;HIGH");
	return -1;
}


int para_dmvaCfgTDResetTime(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppCfgTDResetTime_sys);
}

  /* DMVA OC PAGE */
int para_dmvaOCSensitivity(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCSensitivity_sys);
}


int para_dmvaOCSensitivityName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "LOW");
	else if ( *arg == '2' )
		return sprintf(data, "LOW_TO_MODERATE");
	else if ( *arg == '3' )
		return sprintf(data, "MODERATE");
	else if ( *arg == '4' )
		return sprintf(data, "MODERATE_TO_HIGH");
	else if ( *arg == '5' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;LOW_TO_MODERATE;MODERATE;MODERATE_TO_HIGH;HIGH");
	return -1;
}


int para_dmvaOCObjectWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCObjectWidth_sys);
}
int para_dmvaOCObjectHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCObjectHeight_sys);
}
int para_dmvaOCDirection(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCDirection_sys);
}
int para_dmvaOCEnable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCEnable_sys);
}
int para_dmvaOCLeftRight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCLeftRight_sys);
}
int para_dmvaOCTopBottom(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCTopBottom_sys);
}

/* DMVA OC save/load params */
int para_dmvaOCSave(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaAppOCSave_sys);
}

int para_dmvaOCLoad(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppOCLoad_sys);
}

int para_dmvaOCLoadNames(char *data, char *arg)
{
	int i;
	char *tmpData = data;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

    if(pSysInfo->dmva_config.dmvaSaveLoadOC.numRules == 0)
    {
	    sprintf(tmpData,"");
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadOC.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadOC.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadOC.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadOC.ruleName[i]);
		}
    }

	return 0;
}

  /* DMVA SMETA PAGE */
int para_dmvaSMETASensitivity(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETASensitivity_sys);
}

int para_dmvaSMETASensitivityName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "LOW");
	else if ( *arg == '2' )
		return sprintf(data, "LOW_TO_MODERATE");
	else if ( *arg == '3' )
		return sprintf(data, "MODERATE");
	else if ( *arg == '4' )
		return sprintf(data, "MODERATE_TO_HIGH");
	else if ( *arg == '5' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;LOW_TO_MODERATE;MODERATE;MODERATE_TO_HIGH;HIGH");
	return -1;
}


int para_dmvaSMETAEnableStreamingData(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAEnableStreamingData_sys);
}
int para_dmvaSMETAStreamBB(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAStreamBB_sys);
}
int para_dmvaSMETAStreamVelocity(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAStreamVelocity_sys);
}
int para_dmvaSMETAStreamHistogram(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAStreamHistogram_sys);
}
int para_dmvaSMETAStreamMoments(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAStreamMoments_sys);
}
int para_dmvaSMETAPresentAdjust(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAPresentAdjust_sys);
}
int para_dmvaSMETAPersonMinWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAPersonMinWidth_sys);
}
int para_dmvaSMETAPersonMinHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAPersonMinHeight_sys);
}
int para_dmvaSMETAVehicleMinWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAVehicleMinWidth_sys);
}
int para_dmvaSMETAVehicleMinHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAVehicleMinHeight_sys);
}
int para_dmvaSMETAPersonMaxWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAPersonMaxWidth_sys);
}
int para_dmvaSMETAPersonMaxHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAPersonMaxHeight_sys);
}
int para_dmvaSMETAVehicleMaxWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxWidth_sys);
}
int para_dmvaSMETAVehicleMaxHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxHeight_sys);
}
int para_dmvaSMETANumZones(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETANumZones_sys);
}

int para_dmvaSMETAZone1_ROI_numSides(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_ROI_numSides_sys);
}



int para_dmvaSMETAZone1_x01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[1]);
}
int para_dmvaSMETAZone1_x02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[2]);
}
int para_dmvaSMETAZone1_x03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[3]);
}
int para_dmvaSMETAZone1_x04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[4]);
}
int para_dmvaSMETAZone1_x05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[5]);
}
int para_dmvaSMETAZone1_x06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[6]);
}
int para_dmvaSMETAZone1_x07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[7]);
}
int para_dmvaSMETAZone1_x08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[8]);
}
int para_dmvaSMETAZone1_x09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[9]);
}
int para_dmvaSMETAZone1_x10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[10]);
}
int para_dmvaSMETAZone1_x11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[11]);
}
int para_dmvaSMETAZone1_x12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[12]);
}
int para_dmvaSMETAZone1_x13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[13]);
}
int para_dmvaSMETAZone1_x14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[14]);
}
int para_dmvaSMETAZone1_x15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[15]);
}
int para_dmvaSMETAZone1_x16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_x[16]);
}

int para_dmvaSMETAZone1_y01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[1]);
}
int para_dmvaSMETAZone1_y02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[2]);
}
int para_dmvaSMETAZone1_y03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[3]);
}
int para_dmvaSMETAZone1_y04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[4]);
}
int para_dmvaSMETAZone1_y05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[5]);
}
int para_dmvaSMETAZone1_y06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[6]);
}
int para_dmvaSMETAZone1_y07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[7]);
}
int para_dmvaSMETAZone1_y08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[8]);
}
int para_dmvaSMETAZone1_y09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[9]);
}
int para_dmvaSMETAZone1_y10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[10]);
}
int para_dmvaSMETAZone1_y11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[11]);
}
int para_dmvaSMETAZone1_y12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[12]);
}
int para_dmvaSMETAZone1_y13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[13]);
}
int para_dmvaSMETAZone1_y14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[14]);
}
int para_dmvaSMETAZone1_y15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[15]);
}
int para_dmvaSMETAZone1_y16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAZone1_y[16]);
}

 int para_dmvaSMETAZone_1_Label(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaSMETAZone_1_Label_sys);
}

int para_dmvaSMETAEnableVelocityObjId(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETAEnableVelocityObjId_sys);
}

/* DMVA SMETA save/load params */
int para_dmvaSMETASave(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaAppSMETASave_sys);
}

int para_dmvaSMETALoad(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppSMETALoad_sys);
}

int para_dmvaSMETALoadNames(char *data, char *arg)
{
	int i;
	char *tmpData = data;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

    if(pSysInfo->dmva_config.dmvaSaveLoadSMETA.numRules == 0)
    {
	    sprintf(tmpData,"");
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadSMETA.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadSMETA.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadSMETA.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadSMETA.ruleName[i]);
		}
    }

	return 0;
}

  /* DMVA IMD PAGE */
int para_dmvaIMDSensitivity(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDSensitivity_sys);
}

int para_dmvaIMDSensitivityName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "LOW");
	else if ( *arg == '2' )
		return sprintf(data, "LOW_TO_MODERATE");
	else if ( *arg == '3' )
		return sprintf(data, "MODERATE");
	else if ( *arg == '4' )
		return sprintf(data, "MODERATE_TO_HIGH");
	else if ( *arg == '5' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;LOW_TO_MODERATE;MODERATE;MODERATE_TO_HIGH;HIGH");
	return -1;
}

int para_dmvaIMDPresentAdjust(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDPresentAdjust_sys);
}
int para_dmvaIMDPersonMinWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDPersonMinWidth_sys);
}
int para_dmvaIMDPersonMinHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDPersonMinHeight_sys);
}
int para_dmvaIMDVehicleMinWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDVehicleMinWidth_sys);
}
int para_dmvaIMDVehicleMinHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDVehicleMinHeight_sys);
}
int para_dmvaIMDPersonMaxWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDPersonMaxWidth_sys);
}
int para_dmvaIMDPersonMaxHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDPersonMaxHeight_sys);
}
int para_dmvaIMDVehicleMaxWidth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDVehicleMaxWidth_sys);
}
int para_dmvaIMDVehicleMaxHeight(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDVehicleMaxHeight_sys);
}
int para_dmvaIMDNumZones(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDNumZones_sys);
}
int para_dmvaIMDZoneEnable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZoneEnable_sys);
}


  /* DMVA IMD PAGE ZONE Parameters */



int para_dmvaIMDZone1_ROI_numSides(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_ROI_numSides_sys);
}

int para_dmvaIMDZone2_ROI_numSides(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_ROI_numSides_sys);
}



int para_dmvaIMDZone3_ROI_numSides(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_ROI_numSides_sys);
}

int para_dmvaIMDZone4_ROI_numSides(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_ROI_numSides_sys);
}




int para_dmvaIMDZone1_x01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[1]);
}
int para_dmvaIMDZone1_x02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[2]);
}
int para_dmvaIMDZone1_x03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[3]);
}
int para_dmvaIMDZone1_x04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[4]);
}
int para_dmvaIMDZone1_x05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[5]);
}
int para_dmvaIMDZone1_x06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[6]);
}
int para_dmvaIMDZone1_x07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[7]);
}
int para_dmvaIMDZone1_x08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[8]);
}
int para_dmvaIMDZone1_x09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[9]);
}
int para_dmvaIMDZone1_x10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[10]);
}
int para_dmvaIMDZone1_x11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[11]);
}
int para_dmvaIMDZone1_x12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[12]);
}
int para_dmvaIMDZone1_x13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[13]);
}
int para_dmvaIMDZone1_x14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[14]);
}
int para_dmvaIMDZone1_x15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[15]);
}
int para_dmvaIMDZone1_x16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_x[16]);
}

int para_dmvaIMDZone1_y01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[1]);
}
int para_dmvaIMDZone1_y02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[2]);
}
int para_dmvaIMDZone1_y03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[3]);
}
int para_dmvaIMDZone1_y04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[4]);
}
int para_dmvaIMDZone1_y05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[5]);
}
int para_dmvaIMDZone1_y06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[6]);
}
int para_dmvaIMDZone1_y07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[7]);
}
int para_dmvaIMDZone1_y08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[8]);
}
int para_dmvaIMDZone1_y09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[9]);
}
int para_dmvaIMDZone1_y10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[10]);
}
int para_dmvaIMDZone1_y11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[11]);
}
int para_dmvaIMDZone1_y12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[12]);
}
int para_dmvaIMDZone1_y13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[13]);
}
int para_dmvaIMDZone1_y14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[14]);
}
int para_dmvaIMDZone1_y15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[15]);
}
int para_dmvaIMDZone1_y16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone1_y[16]);
}


int para_dmvaIMDZone2_x01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[1]);
}
int para_dmvaIMDZone2_x02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[2]);
}
int para_dmvaIMDZone2_x03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[3]);
}
int para_dmvaIMDZone2_x04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[4]);
}
int para_dmvaIMDZone2_x05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[5]);
}
int para_dmvaIMDZone2_x06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[6]);
}
int para_dmvaIMDZone2_x07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[7]);
}
int para_dmvaIMDZone2_x08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[8]);
}
int para_dmvaIMDZone2_x09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[9]);
}
int para_dmvaIMDZone2_x10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[10]);
}
int para_dmvaIMDZone2_x11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[11]);
}
int para_dmvaIMDZone2_x12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[12]);
}
int para_dmvaIMDZone2_x13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[13]);
}
int para_dmvaIMDZone2_x14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[14]);
}
int para_dmvaIMDZone2_x15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[15]);
}
int para_dmvaIMDZone2_x16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_x[16]);
}

int para_dmvaIMDZone2_y01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[1]);
}
int para_dmvaIMDZone2_y02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[2]);
}
int para_dmvaIMDZone2_y03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[3]);
}
int para_dmvaIMDZone2_y04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[4]);
}
int para_dmvaIMDZone2_y05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[5]);
}
int para_dmvaIMDZone2_y06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[6]);
}
int para_dmvaIMDZone2_y07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[7]);
}
int para_dmvaIMDZone2_y08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[8]);
}
int para_dmvaIMDZone2_y09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[9]);
}
int para_dmvaIMDZone2_y10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[10]);
}
int para_dmvaIMDZone2_y11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[11]);
}
int para_dmvaIMDZone2_y12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[12]);
}
int para_dmvaIMDZone2_y13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[13]);
}
int para_dmvaIMDZone2_y14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[14]);
}
int para_dmvaIMDZone2_y15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[15]);
}
int para_dmvaIMDZone2_y16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone2_y[16]);
}





int para_dmvaIMDZone3_x01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[1]);
}
int para_dmvaIMDZone3_x02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[2]);
}
int para_dmvaIMDZone3_x03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[3]);
}
int para_dmvaIMDZone3_x04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[4]);
}
int para_dmvaIMDZone3_x05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[5]);
}
int para_dmvaIMDZone3_x06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[6]);
}
int para_dmvaIMDZone3_x07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[7]);
}
int para_dmvaIMDZone3_x08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[8]);
}
int para_dmvaIMDZone3_x09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[9]);
}
int para_dmvaIMDZone3_x10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[10]);
}
int para_dmvaIMDZone3_x11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[11]);
}
int para_dmvaIMDZone3_x12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[12]);
}
int para_dmvaIMDZone3_x13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[13]);
}
int para_dmvaIMDZone3_x14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[14]);
}
int para_dmvaIMDZone3_x15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[15]);
}
int para_dmvaIMDZone3_x16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_x[16]);
}

int para_dmvaIMDZone3_y01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[1]);
}
int para_dmvaIMDZone3_y02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[2]);
}
int para_dmvaIMDZone3_y03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[3]);
}
int para_dmvaIMDZone3_y04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[4]);
}
int para_dmvaIMDZone3_y05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[5]);
}
int para_dmvaIMDZone3_y06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[6]);
}
int para_dmvaIMDZone3_y07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[7]);
}
int para_dmvaIMDZone3_y08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[8]);
}
int para_dmvaIMDZone3_y09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[9]);
}
int para_dmvaIMDZone3_y10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[10]);
}
int para_dmvaIMDZone3_y11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[11]);
}
int para_dmvaIMDZone3_y12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[12]);
}
int para_dmvaIMDZone3_y13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[13]);
}
int para_dmvaIMDZone3_y14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[14]);
}
int para_dmvaIMDZone3_y15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[15]);
}
int para_dmvaIMDZone3_y16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone3_y[16]);
}


int para_dmvaIMDZone4_x01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[1]);
}
int para_dmvaIMDZone4_x02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[2]);
}
int para_dmvaIMDZone4_x03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[3]);
}
int para_dmvaIMDZone4_x04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[4]);
}
int para_dmvaIMDZone4_x05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[5]);
}
int para_dmvaIMDZone4_x06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[6]);
}
int para_dmvaIMDZone4_x07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[7]);
}
int para_dmvaIMDZone4_x08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[8]);
}
int para_dmvaIMDZone4_x09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[9]);
}
int para_dmvaIMDZone4_x10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[10]);
}
int para_dmvaIMDZone4_x11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[11]);
}
int para_dmvaIMDZone4_x12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[12]);
}
int para_dmvaIMDZone4_x13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[13]);
}
int para_dmvaIMDZone4_x14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[14]);
}
int para_dmvaIMDZone4_x15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[15]);
}
int para_dmvaIMDZone4_x16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_x[16]);
}

int para_dmvaIMDZone4_y01(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[1]);
}
int para_dmvaIMDZone4_y02(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[2]);
}
int para_dmvaIMDZone4_y03(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[3]);
}
int para_dmvaIMDZone4_y04(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[4]);
}
int para_dmvaIMDZone4_y05(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[5]);
}
int para_dmvaIMDZone4_y06(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[6]);
}
int para_dmvaIMDZone4_y07(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[7]);
}
int para_dmvaIMDZone4_y08(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[8]);
}
int para_dmvaIMDZone4_y09(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[9]);
}
int para_dmvaIMDZone4_y10(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[10]);
}
int para_dmvaIMDZone4_y11(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[11]);
}
int para_dmvaIMDZone4_y12(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[12]);
}
int para_dmvaIMDZone4_y13(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[13]);
}
int para_dmvaIMDZone4_y14(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[14]);
}
int para_dmvaIMDZone4_y15(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[15]);
}
int para_dmvaIMDZone4_y16(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDZone4_y[16]);
}

 int para_dmvaIMDZone_1_Label(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaIMDZone_1_Label_sys);
}

 int para_dmvaIMDZone_2_Label(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaIMDZone_2_Label_sys);
}

  int para_dmvaIMDZone_3_Label(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaIMDZone_3_Label_sys);
}

   int para_dmvaIMDZone_4_Label(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaIMDZone_4_Label_sys);
}

/* DMVA IMD save/load params */
int para_dmvaIMDSave(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaAppIMDSave_sys);
}

int para_dmvaIMDLoad(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppIMDLoad_sys);
}

int para_dmvaIMDLoadNames(char *data, char *arg)
{
	int i;
	char *tmpData = data;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

    if(pSysInfo->dmva_config.dmvaSaveLoadIMD.numRules == 0)
    {
	    sprintf(tmpData,"");
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadIMD.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadIMD.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadIMD.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadIMD.ruleName[i]);
		}
    }

	return 0;
}


int para_dmvaCamIDName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "CAM_1");
	else if ( *arg == '2' )
		return sprintf(data, "CAM_2");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "CAM_1;CAM_2");
	return -1;
}


   int para_dmvaEventListStartMonth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListStartMonth_sys);

}

   int para_dmvaEventListStartDay(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListStartDay_sys);
}

   int para_dmvaEventListStartTime(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListStartTime_sys);
}

   int para_dmvaEventListEndMonth(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListEndMonth_sys);
}

   int para_dmvaEventListEndDay(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListEndDay_sys);

}

   int para_dmvaEventListEndTime(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEventListEndTime_sys);
}

int para_dmvaEventListMonthName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "JAN");
	else if ( *arg == '2' )
		return sprintf(data, "FEB");
	else if ( *arg == '3' )
		return sprintf(data, "MAR");
	else if ( *arg == '4' )
		return sprintf(data, "APR");
	else if ( *arg == '5' )
		return sprintf(data, "MAY");
	else if ( *arg == '6' )
		return sprintf(data, "JUN");
	else if ( *arg == '7' )
		return sprintf(data, "JUL");
	else if ( *arg == '8' )
		return sprintf(data, "AUG");
	else if ( *arg == '9' )
		return sprintf(data, "SEP");
	else if ( *arg == 'b' )
		return sprintf(data, "OCT");
	else if ( *arg == 'c' )
		return sprintf(data, "NOV");
	else if ( *arg == 'd' )
		return sprintf(data, "DEC");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "JAN;FEB;MAR;APR;MAY;JUN;JUL;AUG;SEP;OCT;NOV;DEC");
	return -1;
}

int para_dmvaEventListDayName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "01");
	else if ( *arg == '2' )
		return sprintf(data, "02");
	else if ( *arg == '3' )
		return sprintf(data, "03");
	else if ( *arg == '4' )
		return sprintf(data, "04");
	else if ( *arg == '5' )
		return sprintf(data, "05");
	else if ( *arg == '6' )
		return sprintf(data, "06");
	else if ( *arg == '7' )
		return sprintf(data, "07");
	else if ( *arg == '8' )
		return sprintf(data, "08");
	else if ( *arg == '9' )
		return sprintf(data, "09");
	else if ( *arg == 'b' )
		return sprintf(data, "10");
	else if ( *arg == 'c' )
		return sprintf(data, "11");
	else if ( *arg == 'd' )
		return sprintf(data, "12");
	else if ( *arg == 'e' )
		return sprintf(data, "13");
	else if ( *arg == 'f' )
		return sprintf(data, "14");
	else if ( *arg == 'g' )
		return sprintf(data, "15");
	else if ( *arg == 'h' )
		return sprintf(data, "16");
	else if ( *arg == 'i' )
		return sprintf(data, "17");
	else if ( *arg == 'j' )
		return sprintf(data, "18");
	else if ( *arg == 'k' )
		return sprintf(data, "19");
	else if ( *arg == 'l' )
		return sprintf(data, "20");
	else if ( *arg == 'm' )
		return sprintf(data, "21");
	else if ( *arg == 'n' )
		return sprintf(data, "22");
	else if ( *arg == 'o' )
		return sprintf(data, "23");
	else if ( *arg == 'p' )
		return sprintf(data, "24");
	else if ( *arg == 'q' )
		return sprintf(data, "25");
	else if ( *arg == 'r' )
		return sprintf(data, "26");
	else if ( *arg == 's' )
		return sprintf(data, "27");
	else if ( *arg == 't' )
		return sprintf(data, "28");
	else if ( *arg == 'u' )
		return sprintf(data, "29");
	else if ( *arg == 'v' )
		return sprintf(data, "30");
	else if ( *arg == 'w' )
		return sprintf(data, "31");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "01;02;03;04;05;06;07;08;09;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;25;26;27;28;29;30;31");
	return -1;
}

int para_dmvaEventListTimeName(char *data, char *arg)
{
	if ( *arg == '0' )
		return sprintf(data, "00:00");
	else if ( *arg == '1' )
		return sprintf(data, "01:00");
	else if ( *arg == '2' )
		return sprintf(data, "02:00");
	else if ( *arg == '3' )
		return sprintf(data, "03:00");
	else if ( *arg == '4' )
		return sprintf(data, "04:00");
	else if ( *arg == '5' )
		return sprintf(data, "05:00");
	else if ( *arg == '6' )
		return sprintf(data, "06:00");
	else if ( *arg == '7' )
		return sprintf(data, "07:00");
	else if ( *arg == '8' )
		return sprintf(data, "08:00");
	else if ( *arg == '9' )
		return sprintf(data, "09:00");
	else if ( *arg == 'b' )
		return sprintf(data, "10:00");
	else if ( *arg == 'c' )
		return sprintf(data, "11:00");
	else if ( *arg == 'd' )
		return sprintf(data, "12:00");
	else if ( *arg == 'e' )
		return sprintf(data, "13:00");
	else if ( *arg == 'f' )
		return sprintf(data, "14:00");
	else if ( *arg == 'g' )
		return sprintf(data, "15:00");
	else if ( *arg == 'h' )
		return sprintf(data, "16:00");
	else if ( *arg == 'i' )
		return sprintf(data, "17:00");
	else if ( *arg == 'j' )
		return sprintf(data, "18:00");
	else if ( *arg == 'k' )
		return sprintf(data, "19:00");
	else if ( *arg == 'l' )
		return sprintf(data, "20:00");
	else if ( *arg == 'm' )
		return sprintf(data, "21:00");
	else if ( *arg == 'n' )
		return sprintf(data, "22:00");
	else if ( *arg == 'o' )
		return sprintf(data, "23:00");

	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "00:00;01:00;02:00;03:00;04:00;05:00;06:00;07:00;08:00;09:00;10:00;11:00;12:00;13:00;14:00;15:00;16:00;17:00;18:00;19:00;20:00;21:00;22:00;23:00");
	return -1;
}

   int para_dmvaEventListTimeStamp(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->tCurDmvaLog.eventListTimeStamp_sys);
}

   int para_dmvaEventListEventDetails(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->tCurDmvaLog.eventListEventDetails_sys);
}

/* Set DMVA Advanced Settings Page specific Parameters */
#if 1

int para_dmvaObjectMetaData(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaObjectMetaData_sys);
}
int para_dmvaOverlayPropertiesFontType(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesFontType_sys);
}
int para_dmvaOverlayPropertiesFontSize(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesFontSize_sys);
}
int para_dmvaOverlayPropertiesFontColor(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesFontColor_sys);
}
int para_dmvaOverlayPropertiesBoxColor(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesBoxColor_sys);
}
int para_dmvaOverlayPropertiesCentroidColor(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesCentroidColor_sys);
}
int para_dmvaOverlayPropertiesActiveZoneColor(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesActiveZoneColor_sys);
}

int para_dmvaOverlayPropertiesInactiveZoneColor(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesInactiveZoneColor_sys);
}

int para_dmvaOverlayPropertiesVelocityColor(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaOverlayPropertiesVelocityColor_sys);
}
int para_dmvaOverlayPropertiesFontTypeList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "ARIAL");
	else if ( *arg == '2' )
		return sprintf(data, "ARIAL_BLACK");
	else if ( *arg == '3' )
		return sprintf(data, "TIMES_NEW_ROMAN");
	else if ( *arg == '4' )
		return sprintf(data, "VERDANA");
	else if ( *arg == '5' )
		return sprintf(data, "BOOK_ANTIQUA");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "ARIAL;ARIAL_BLACK;TIMES_NEW_ROMAN;VERDANA;BOOK_ANTIQUA");
	return -1;
}

int para_dmvaOverlayPropertiesFontSizeList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "6");
	else if ( *arg == '2' )
		return sprintf(data, "8");
	else if ( *arg == '3' )
		return sprintf(data, "10");
	else if ( *arg == '4' )
		return sprintf(data, "12");
	else if ( *arg == '5' )
		return sprintf(data, "14");
	else if ( *arg == '6' )
		return sprintf(data, "16");
	else if ( *arg == '7' )
		return sprintf(data, "18");
	else if ( *arg == '8' )
		return sprintf(data, "20");
	else if ( *arg == '9' )
		return sprintf(data, "22");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "6;8;10;12;14;16;18;20;22");
	return -1;
}

int para_dmvaOverlayPropertiesFontColorList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "C1");
	else if ( *arg == '2' )
		return sprintf(data, "C2");
	else if ( *arg == '3' )
		return sprintf(data, "C3");
	else if ( *arg == '4' )
		return sprintf(data, "C4");
	else if ( *arg == '5' )
		return sprintf(data, "C5");
	else if ( *arg == '6' )
		return sprintf(data, "C6");
	else if ( *arg == '7' )
		return sprintf(data, "C7");
	else if ( *arg == '8' )
		return sprintf(data, "C8");
	else if ( *arg == '9' )
		return sprintf(data, "C9");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "C1;C2;C3;C4;C5;C6;C7;C8;C9");
	return -1;
}

int para_dmvaOverlayPropertiesBoxColorList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "C1");
	else if ( *arg == '2' )
		return sprintf(data, "C2");
	else if ( *arg == '3' )
		return sprintf(data, "C3");
	else if ( *arg == '4' )
		return sprintf(data, "C4");
	else if ( *arg == '5' )
		return sprintf(data, "C5");
	else if ( *arg == '6' )
		return sprintf(data, "C6");
	else if ( *arg == '7' )
		return sprintf(data, "C7");
	else if ( *arg == '8' )
		return sprintf(data, "C8");
	else if ( *arg == '9' )
		return sprintf(data, "C9");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "C1;C2;C3;C4;C5;C6;C7;C8;C9");
	return -1;
}

int para_dmvaOverlayPropertiesCentroidColorList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "C1");
	else if ( *arg == '2' )
		return sprintf(data, "C2");
	else if ( *arg == '3' )
		return sprintf(data, "C3");
	else if ( *arg == '4' )
		return sprintf(data, "C4");
	else if ( *arg == '5' )
		return sprintf(data, "C5");
	else if ( *arg == '6' )
		return sprintf(data, "C6");
	else if ( *arg == '7' )
		return sprintf(data, "C7");
	else if ( *arg == '8' )
		return sprintf(data, "C8");
	else if ( *arg == '9' )
		return sprintf(data, "C9");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "C1;C2;C3;C4;C5;C6;C7;C8;C9");
	return -1;
}

int para_dmvaOverlayPropertiesActiveZoneColorList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "C1");
	else if ( *arg == '2' )
		return sprintf(data, "C2");
	else if ( *arg == '3' )
		return sprintf(data, "C3");
	else if ( *arg == '4' )
		return sprintf(data, "C4");
	else if ( *arg == '5' )
		return sprintf(data, "C5");
	else if ( *arg == '6' )
		return sprintf(data, "C6");
	else if ( *arg == '7' )
		return sprintf(data, "C7");
	else if ( *arg == '8' )
		return sprintf(data, "C8");
	else if ( *arg == '9' )
		return sprintf(data, "C9");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "C1;C2;C3;C4;C5;C6;C7;C8;C9");
	return -1;
}

int para_dmvaOverlayPropertiesInactiveZoneColorList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "C1");
	else if ( *arg == '2' )
		return sprintf(data, "C2");
	else if ( *arg == '3' )
		return sprintf(data, "C3");
	else if ( *arg == '4' )
		return sprintf(data, "C4");
	else if ( *arg == '5' )
		return sprintf(data, "C5");
	else if ( *arg == '6' )
		return sprintf(data, "C6");
	else if ( *arg == '7' )
		return sprintf(data, "C7");
	else if ( *arg == '8' )
		return sprintf(data, "C8");
	else if ( *arg == '9' )
		return sprintf(data, "C9");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "C1;C2;C3;C4;C5;C6;C7;C8;C9");
	return -1;
}

int para_dmvaOverlayPropertiesVelocityColorList(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "C1");
	else if ( *arg == '2' )
		return sprintf(data, "C2");
	else if ( *arg == '3' )
		return sprintf(data, "C3");
	else if ( *arg == '4' )
		return sprintf(data, "C4");
	else if ( *arg == '5' )
		return sprintf(data, "C5");
	else if ( *arg == '6' )
		return sprintf(data, "C6");
	else if ( *arg == '7' )
		return sprintf(data, "C7");
	else if ( *arg == '8' )
		return sprintf(data, "C8");
	else if ( *arg == '9' )
		return sprintf(data, "C9");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "C1;C2;C3;C4;C5;C6;C7;C8;C9");
	return -1;
}


int para_dmvaEnvironmentProfileLoad(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEnvironmentProfileLoad_sys);
}
int para_dmvaEnvironmentProfileStore(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEnvironmentProfileStore_sys);
}
int para_dmvaEnvironmentProfileClear(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEnvironmentProfileClear_sys);
}
int para_dmvaEnvironmentProfileState(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEnvironmentProfileState_sys);
}
int para_dmvaEnvironmentProfileDesc(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaEnvironmentProfileDesc_sys);
}

#endif

/*DMVA event delete */
int para_dmvaEventDeleteIndex(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEventDeleteIndex);
}

int para_dmvaEventDeleteStartIndex(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEventDeleteStartIndex);
}

int para_dmvaEventDeleteEndIndex(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEventDeleteEndIndex);
}

int para_dmvaEventDeleteAll(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaEventDeleteAll);
}

/* DMVA Scheduler */
int para_dmvaScheduleEnable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.enable);
}

int para_dmvaScheduleRuleType(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule.ruleType);
}

int para_dmvaScheduleLoadSetting(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule.loadSetting);
}

int para_dmvaScheduleRuleLabel(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%s", pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule.ruleLabel);
}

int para_dmvaScheduleStartDay(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule.startDay - 1);
}

int para_dmvaScheduleStartTime(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule.startTimeInMin);
}

int para_dmvaScheduleEndDay(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule.endDay - 1);
}

int para_dmvaScheduleEndTime(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.curDmvaSchedule.endTimeInMin);
}

int para_dmvaScheduleResult(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaScheduleMgr.result);
}

int para_dmvaScheduleRuleTypeNames(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "TRIP;IMD;OC;SMETA");
}

int para_dmvaScheduleDayNames(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "Monday;Tuesday;Wednesday;Thursday;Friday;Saturday;Sunday;Everyday;Weekdays;Weekend");
}

int para_dmvaScheduleLoadSettingNames(char *data, char *arg)
{
	int i;
	char *tmpData = data;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

    // TZ
    if(pSysInfo->dmva_config.dmvaSaveLoadTZ.numRules == 0)
    {
	    sprintf(tmpData,"NA");
	    tmpData += 2;
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadTZ.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadTZ.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadTZ.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadTZ.ruleName[i]);
		}
    }

    sprintf(tmpData++,"@");

    // IMD
    if(pSysInfo->dmva_config.dmvaSaveLoadIMD.numRules == 0)
    {
	    sprintf(tmpData,"NA");
	    tmpData += 2;
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadIMD.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadIMD.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadIMD.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadIMD.ruleName[i]);
		}
    }

    sprintf(tmpData++,"@");

    // OC
    if(pSysInfo->dmva_config.dmvaSaveLoadOC.numRules == 0)
    {
	    sprintf(tmpData,"NA");
	    tmpData += 2;
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadOC.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadOC.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadOC.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadOC.ruleName[i]);
		}
    }

    sprintf(tmpData++,"@");

    // SMETA
    if(pSysInfo->dmva_config.dmvaSaveLoadSMETA.numRules == 0)
    {
	    sprintf(tmpData,"NA");
	    tmpData += 2;
	}
    else
    {
		for(i = 0;i < pSysInfo->dmva_config.dmvaSaveLoadSMETA.numRules;i ++)
		{
			if(i == 0)
				sprintf(tmpData,"%s",pSysInfo->dmva_config.dmvaSaveLoadSMETA.ruleName[i]);
			else
			{
				sprintf(tmpData,";%s",pSysInfo->dmva_config.dmvaSaveLoadSMETA.ruleName[i]);
				tmpData ++;
			}

			tmpData += strlen(pSysInfo->dmva_config.dmvaSaveLoadSMETA.ruleName[i]);
		}
    }

    return 0;
}

/* DMVA Enc Roi Enable */
int para_dmvaEncRoiEnable(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaAppEncRoiEnable_sys);
}


/* TZ/OC box min/max size limits */
int para_dmvaTZOCMinWidthLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaTZOCMinWidthLimit_sys);
}

int para_dmvaTZOCMinHeightLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaTZOCMinHeightLimit_sys);
}

int para_dmvaTZOCMaxWidthLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaTZOCMaxWidthLimit_sys);
}

int para_dmvaTZOCMaxHeightLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaTZOCMaxHeightLimit_sys);
}

/* IMD/SEMTA box min/max size limits */
int para_dmvaIMDSMETAPersonMinWidthLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAPeopleMinWidthLimit_sys);
}

int para_dmvaIMDSMETAPersonMinHeightLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAPeopleMinHeightLimit_sys);
}

int para_dmvaIMDSMETAPersonMaxWidthLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAPeopleMaxWidthLimit_sys);
}

int para_dmvaIMDSMETAPersonMaxHeightLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAPeopleMaxHeightLimit_sys);
}


int para_dmvaIMDSMETAVehicleMinWidthLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAVehicleMinWidthLimit_sys);
}

int para_dmvaIMDSMETAVehicleMinHeightLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAVehicleMinHeightLimit_sys);
}

int para_dmvaIMDSMETAVehicleMaxWidthLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAVehicleMaxWidthLimit_sys);
}

int para_dmvaIMDSMETAVehicleMaxHeightLimit(char *data, char *arg)
{
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	return sprintf(data, "%d", pSysInfo->dmva_config.dmvaIMDSMETAVehicleMaxHeightLimit_sys);
}


/* DMVA DROPDOWN MENUS */

int para_dmvaTZSensitivityName(char *data, char *arg)
{
	if ( *arg == '1' )
		return sprintf(data, "LOW");
	else if ( *arg == '2' )
		return sprintf(data, "LOW_TO_MODERATE");
	else if ( *arg == '3' )
		return sprintf(data, "MODERATE");
	else if ( *arg == '4' )
		return sprintf(data, "MODERATE_TO_HIGH");
	else if ( *arg == '5' )
		return sprintf(data, "HIGH");
	else if ( *arg == 'a' || *arg == '\0' )
		return sprintf(data, "LOW;LOW_TO_MODERATE;MODERATE;MODERATE_TO_HIGH;HIGH");
	return -1;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
#define HASH_TABLE_SIZE	(sizeof(HttpArgument)/sizeof(HTML_ARGUMENT))

HTML_ARGUMENT HttpArgument [] =
{

	{ "motionxlimit"			, para_motionxlimit	    	, AUTHORITY_VIEWER , NULL },
	{ "motionxblock"			, para_motionxblock		    , AUTHORITY_VIEWER , NULL },
	{ "motionyblock"			, para_motionyblock		    , AUTHORITY_VIEWER , NULL },

	{ "authorityadmin"		, para_authorityadmin		, AUTHORITY_VIEWER , NULL },
	{ "authorityoperator"	, para_authorityoperator	, AUTHORITY_VIEWER , NULL },
	{ "authorityviewer"		, para_authorityviewer		, AUTHORITY_VIEWER , NULL },
	{ "user"			    , para_user				    , AUTHORITY_VIEWER , NULL },
	{ "authority"		    , para_authority	   	    , AUTHORITY_VIEWER , NULL },
    { "dmvaeventlocation"	    , para_dmva_event_location  		, AUTHORITY_VIEWER , NULL },
    { "dmvaevent"	    		, para_get_dmva_event  		, AUTHORITY_VIEWER , NULL },

	/*		LIVE VIDEO SCREEN 	*/
	{ "democfg"				, para_democfg 				, AUTHORITY_VIEWER , NULL },
	{ "democfgname"			, para_democfgname			, AUTHORITY_VIEWER , NULL },
    { "clicksnapfilename"   , para_clicksnapfilename 	, AUTHORITY_VIEWER , NULL },
    { "clicksnapstorage"    , para_clicksnapstorage  	, AUTHORITY_VIEWER , NULL },
	{ "streamname1"			, para_streamname1			, AUTHORITY_VIEWER , NULL },
	{ "streamname2"			, para_streamname2			, AUTHORITY_VIEWER , NULL },
	{ "streamname3"			, para_streamname3			, AUTHORITY_VIEWER , NULL },
	{ "streamwidth1"		, para_streamwidth1			, AUTHORITY_VIEWER , NULL },
	{ "streamwidth2"		, para_streamwidth2			, AUTHORITY_VIEWER , NULL },
	{ "streamwidth3"		, para_streamwidth3			, AUTHORITY_VIEWER , NULL },
	{ "streamheight1"		, para_streamheight1		, AUTHORITY_VIEWER , NULL },
	{ "streamheight2"		, para_streamheight2		, AUTHORITY_VIEWER , NULL },
	{ "streamheight3"		, para_streamheight3		, AUTHORITY_VIEWER , NULL },

	/*		VIDEO / IMAGE SCREEN 	*/
	{ "title"           	, para_title                , AUTHORITY_VIEWER , NULL },
    { "videocodec"		    , para_videocodec		    , AUTHORITY_VIEWER , NULL },
    { "videocodecname"		, para_videocodecname		, AUTHORITY_VIEWER , NULL },
    { "videocodeccombo"		, para_videocodeccombo		, AUTHORITY_VIEWER , NULL },
	{ "videocodeccomboname"	, para_videocodeccomboname	, AUTHORITY_VIEWER , NULL },
	{ "videocodecres"		, para_videocodecres		, AUTHORITY_VIEWER , NULL },
	{ "videocodecresname"	, para_videocodecresname	, AUTHORITY_VIEWER , NULL },
	{ "bitrate1"			, para_mpeg4cvalue			, AUTHORITY_VIEWER , NULL },
	{ "bitrate2"			, para_mpeg42cvalue			, AUTHORITY_VIEWER , NULL },
	{ "livequality"     	, para_livequality          , AUTHORITY_VIEWER , NULL },
	{ "framerate1"			, para_framerate1			, AUTHORITY_VIEWER , NULL },
	{ "framerate2"			, para_framerate2			, AUTHORITY_VIEWER , NULL },
	{ "framerate3"			, para_framerate3			, AUTHORITY_VIEWER , NULL },
	{ "frameratenameall1"	, para_frameratenameall1	, AUTHORITY_VIEWER , NULL },
	{ "frameratenameall2"	, para_frameratenameall2	, AUTHORITY_VIEWER , NULL },
	{ "frameratenameall3"	, para_frameratenameall3	, AUTHORITY_VIEWER , NULL },
    { "ratecontrol1"        , para_ratecontrol1   		, AUTHORITY_VIEWER , NULL },
    { "ratecontrol2"        , para_ratecontrol2   		, AUTHORITY_VIEWER , NULL },
	{ "ratecontrolname"		, para_ratecontrolname		, AUTHORITY_VIEWER , NULL },
	{ "datestampenable1"    , para_datestampenable1    	, AUTHORITY_VIEWER , NULL },
	{ "datestampenable2"    , para_datestampenable2    	, AUTHORITY_VIEWER , NULL },
	{ "datestampenable3"    , para_datestampenable3    	, AUTHORITY_VIEWER , NULL },
	{ "timestampenable1"    , para_timestampenable1    	, AUTHORITY_VIEWER , NULL },
	{ "timestampenable2"    , para_timestampenable2    	, AUTHORITY_VIEWER , NULL },
	{ "timestampenable3"    , para_timestampenable3    	, AUTHORITY_VIEWER , NULL },
	{ "logoenable1"         , para_logoenable1      	, AUTHORITY_VIEWER , NULL },
	{ "logoenable2"         , para_logoenable2      	, AUTHORITY_VIEWER , NULL },
	{ "logoenable3"         , para_logoenable3      	, AUTHORITY_VIEWER , NULL },
	{ "logoposition1"       , para_logoposition1    	, AUTHORITY_VIEWER , NULL },
	{ "logoposition2"       , para_logoposition2    	, AUTHORITY_VIEWER , NULL },
	{ "logoposition3"       , para_logoposition3    	, AUTHORITY_VIEWER , NULL },
	{ "logopositionname"    , para_logopositionname 	, AUTHORITY_VIEWER , NULL },
	{ "textenable1"         , para_textenable1      	, AUTHORITY_VIEWER , NULL },
	{ "textenable2"         , para_textenable2      	, AUTHORITY_VIEWER , NULL },
	{ "textenable3"         , para_textenable3      	, AUTHORITY_VIEWER , NULL },
    { "textposition1"       , para_textposition1    	, AUTHORITY_VIEWER , NULL },
    { "textposition2"       , para_textposition2    	, AUTHORITY_VIEWER , NULL },
    { "textposition3"       , para_textposition3    	, AUTHORITY_VIEWER , NULL },
	{ "textpositionname"    , para_textpositionname 	, AUTHORITY_VIEWER , NULL },
	{ "overlaytext1"    	, para_overlaytext1     	, AUTHORITY_VIEWER , NULL },
	{ "overlaytext2"    	, para_overlaytext2     	, AUTHORITY_VIEWER , NULL },
	{ "overlaytext3"    	, para_overlaytext3     	, AUTHORITY_VIEWER , NULL },
	{ "detailinfo1"         , para_detailinfo1      	, AUTHORITY_VIEWER , NULL },
	{ "detailinfo2"         , para_detailinfo2      	, AUTHORITY_VIEWER , NULL },
	{ "detailinfo3"         , para_detailinfo3      	, AUTHORITY_VIEWER , NULL },
	{ "encryptvideo"        , para_encryption 	    	, AUTHORITY_VIEWER , NULL },
	{ "mirctrl"				, para_mirctrl 				, AUTHORITY_VIEWER , NULL },
	{ "mirctrlname"			, para_mirctrlname			, AUTHORITY_VIEWER , NULL },
	{ "localdisplay"        , para_localdisplay     	, AUTHORITY_VIEWER , NULL },
	{ "localdisplayname"    , para_localdisplayname		, AUTHORITY_VIEWER , NULL },
 	{ "aviformat"	        , para_aviformat			, AUTHORITY_VIEWER , NULL },
	{ "aviformatname"	    , para_aviformatname		, AUTHORITY_VIEWER , NULL },
    { "aviduration"	        , para_aviduration			, AUTHORITY_VIEWER , NULL },
	{ "avidurationname"	    , para_avidurationname		, AUTHORITY_VIEWER , NULL },

	/*	Video Advanced Setting Page    */
	{ "ipratio1"                 , para_ipratio1               , AUTHORITY_VIEWER , NULL },
	{ "ipratio2"                 , para_ipratio2               , AUTHORITY_VIEWER , NULL },
	{ "ipratio3"                 , para_ipratio3               , AUTHORITY_VIEWER , NULL },
	{ "forceiframe1"             , para_forceIframe1           , AUTHORITY_VIEWER , NULL },
	{ "forceiframe2"             , para_forceIframe2           , AUTHORITY_VIEWER , NULL },
	{ "forceiframe3"             , para_forceIframe3           , AUTHORITY_VIEWER , NULL },
	{ "qpinit1"          , para_qpinit1          , AUTHORITY_VIEWER , NULL },
	{ "qpinit2"          , para_qpinit2          , AUTHORITY_VIEWER , NULL },
	{ "qpinit3"          , para_qpinit3          , AUTHORITY_VIEWER , NULL },
	{ "qpmin1"           , para_qpmin1           , AUTHORITY_VIEWER , NULL },
	{ "qpmin2"           , para_qpmin2           , AUTHORITY_VIEWER , NULL },
	{ "qpmin3"           , para_qpmin3           , AUTHORITY_VIEWER , NULL },
	{ "qpmax1"           , para_qpmax1           , AUTHORITY_VIEWER , NULL },
	{ "qpmax2"           , para_qpmax2           , AUTHORITY_VIEWER , NULL },
	{ "qpmax3"           , para_qpmax3           , AUTHORITY_VIEWER , NULL },
	{ "meconfig1"        , para_meconfig1        , AUTHORITY_VIEWER , NULL },
	{ "meconfig2"        , para_meconfig2        , AUTHORITY_VIEWER , NULL },
	{ "meconfig3"        , para_meconfig3        , AUTHORITY_VIEWER , NULL },
	{ "meconfigname"     , para_meconfigname     , AUTHORITY_VIEWER , NULL },
	{ "packetsize1"      , para_packetsize1      , AUTHORITY_VIEWER , NULL },
	{ "packetsize2"      , para_packetsize2      , AUTHORITY_VIEWER , NULL },
	{ "packetsize3"      , para_packetsize3      , AUTHORITY_VIEWER , NULL },
	{ "regionofinterestenable1"    , para_regionofinterestenable1     , AUTHORITY_VIEWER , NULL },
	{ "regionofinterestenable2"    , para_regionofinterestenable2     , AUTHORITY_VIEWER , NULL },
	{ "regionofinterestenable3"    , para_regionofinterestenable3     , AUTHORITY_VIEWER , NULL },
	{ "str1x1"            , para_str1x1          , AUTHORITY_VIEWER , NULL },
      { "str1y1"            , para_str1y1          , AUTHORITY_VIEWER , NULL },
	{ "str1w1"            , para_str1w1          , AUTHORITY_VIEWER , NULL },
      { "str1h1"            , para_str1h1          , AUTHORITY_VIEWER , NULL },
	{ "str1x2"            , para_str1x2          , AUTHORITY_VIEWER , NULL },
      { "str1y2"            , para_str1y2          , AUTHORITY_VIEWER , NULL },
	{ "str1w2"            , para_str1w2          , AUTHORITY_VIEWER , NULL },
      { "str1h2"            , para_str1h2          , AUTHORITY_VIEWER , NULL },
	{ "str1x3"            , para_str1x3          , AUTHORITY_VIEWER , NULL },
      { "str1y3"            , para_str1y3          , AUTHORITY_VIEWER , NULL },
	{ "str1w3"            , para_str1w3          , AUTHORITY_VIEWER , NULL },
      { "str1h3"            , para_str1h3          , AUTHORITY_VIEWER , NULL },
	{ "str2x1"            , para_str2x1          , AUTHORITY_VIEWER , NULL },
      { "str2y1"            , para_str2y1          , AUTHORITY_VIEWER , NULL },
	{ "str2w1"            , para_str2w1          , AUTHORITY_VIEWER , NULL },
      { "str2h1"            , para_str2h1          , AUTHORITY_VIEWER , NULL },
	{ "str2x2"            , para_str2x2          , AUTHORITY_VIEWER , NULL },
      { "str2y2"            , para_str2y2          , AUTHORITY_VIEWER , NULL },
	{ "str2w2"            , para_str2w2          , AUTHORITY_VIEWER , NULL },
      { "str2h2"            , para_str2h2          , AUTHORITY_VIEWER , NULL },
	{ "str2x3"            , para_str2x3          , AUTHORITY_VIEWER , NULL },
      { "str2y3"            , para_str2y3          , AUTHORITY_VIEWER , NULL },
	{ "str2w3"            , para_str2w3          , AUTHORITY_VIEWER , NULL },
      { "str2h3"            , para_str2h3          , AUTHORITY_VIEWER , NULL },
	{ "str3x1"            , para_str3x1          , AUTHORITY_VIEWER , NULL },
      { "str3y1"            , para_str3y1          , AUTHORITY_VIEWER , NULL },
	{ "str3w1"            , para_str3w1          , AUTHORITY_VIEWER , NULL },
      { "str3h1"            , para_str3h1          , AUTHORITY_VIEWER , NULL },
	{ "str3x2"            , para_str3x2          , AUTHORITY_VIEWER , NULL },
      { "str3y2"            , para_str3y2          , AUTHORITY_VIEWER , NULL },
	{ "str3w2"            , para_str3w2          , AUTHORITY_VIEWER , NULL },
      { "str3h2"            , para_str3h2          , AUTHORITY_VIEWER , NULL },
	{ "str3x3"            , para_str3x3          , AUTHORITY_VIEWER , NULL },
      { "str3y3"            , para_str3y3          , AUTHORITY_VIEWER , NULL },
	{ "str3w3"            , para_str3w3          , AUTHORITY_VIEWER , NULL },
      { "str3h3"            , para_str3h3          , AUTHORITY_VIEWER , NULL },

	/*      DMVA SCREENS 				*/
	{ "dmvaenable"							, para_dmvaEnable 						, AUTHORITY_VIEWER 		, NULL },
	{ "dmvaexptalgoframerate"				, para_dmvaExptAlgoFrameRate 			, AUTHORITY_VIEWER 		, NULL },
	{ "dmvaexptalgodetectfreq"				, para_dmvaExptAlgoDetectFreq 			, AUTHORITY_VIEWER 		, NULL },
	{ "dmvaexptevtrecordenable"				, para_dmvaExptEvtRecordEnable			, AUTHORITY_VIEWER 		, NULL },

	/* DMVA MAIN  PAGE */
	{ "dmvacamid"      							, para_dmvaCamID									, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvagetschedule"      					, para_dmvaGetSchedule							, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvacfgfd"      							, para_dmvaCfgFD									, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvacfgctdimdsmostzoc"      				, para_dmvaCfgCTDImdSmetaTzOc						, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvadisplayoptions"      					, para_dmvaDisplayOptions						, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventrecordingvame"      				, para_dmvaEventRecordingVAME					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistactionplaysendsearchtrash"   , para_dmvaEventListActionPlaySendSearchTrash	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistselectevent"      			, para_dmvaEventListSelectEvent					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistarchiveflag"      			, para_dmvaEventListArchiveFlag					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlisteventtype"      				, para_dmvaEventListEventType					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventliststartmonth"      	, para_dmvaEventListStartMonth	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventliststartday"      	    , para_dmvaEventListStartDay		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventliststarttime"      	    , para_dmvaEventListStartTime	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistendmonth"      	    , para_dmvaEventListEndMonth		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistendday"      	    , para_dmvaEventListEndDay		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistendtime"      	    , para_dmvaEventListEndTime		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlisttimestamp"      	    , para_dmvaEventListTimeStamp	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlisteventdetails"      	, para_dmvaEventListEventDetails	, AUTHORITY_OPERATOR 	, NULL },

	/* DMVA ADVANCED SETTINGS  PAGE */
	{ "dmvaobjectmetadata"      				, para_dmvaObjectMetaData			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesfonttype"      		, para_dmvaOverlayPropertiesFontType	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesfontsize"			, para_dmvaOverlayPropertiesFontSize	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesfontcolor"			, para_dmvaOverlayPropertiesFontColor, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesboxcolor"  			, para_dmvaOverlayPropertiesBoxColor, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiescentriodcolor"  	, para_dmvaOverlayPropertiesCentroidColor, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesactivezonecolor"  	, para_dmvaOverlayPropertiesActiveZoneColor, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesinactivezonecolor"  	, para_dmvaOverlayPropertiesInactiveZoneColor, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesvelocitycolor"  	, para_dmvaOverlayPropertiesVelocityColor, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaenvironmentprofileload"      			, para_dmvaEnvironmentProfileLoad		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaenvironmentprofilestore"     			, para_dmvaEnvironmentProfileStore		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaenvironmentprofileclear"     			, para_dmvaEnvironmentProfileClear		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaenvironmentprofilestate"     			, para_dmvaEnvironmentProfileState		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaenvironmentprofiledesc"      			, para_dmvaEnvironmentProfileDesc		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesfonttypelist"      		, para_dmvaOverlayPropertiesFontTypeList	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesfontsizelist"			, para_dmvaOverlayPropertiesFontSizeList	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesfontcolorlist"			, para_dmvaOverlayPropertiesFontColorList, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesboxcolorlist"  			, para_dmvaOverlayPropertiesBoxColorList, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiescentriodcolorlist"  	, para_dmvaOverlayPropertiesCentroidColorList, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesactivezonecolorlist"  	, para_dmvaOverlayPropertiesActiveZoneColorList, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesinactivezonecolorlist"  	, para_dmvaOverlayPropertiesInactiveZoneColorList, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoverlaypropertiesvelocitycolorlist"  	, para_dmvaOverlayPropertiesVelocityColorList, AUTHORITY_OPERATOR 	, NULL },

	/*DMVA event delete */
	{ "dmvaeventdeleteindex"      	, para_dmvaEventDeleteIndex	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventdeletestartindex"      	, para_dmvaEventDeleteStartIndex	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventdeleteendindex"      	, para_dmvaEventDeleteEndIndex	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventdeleteall"      	, para_dmvaEventDeleteAll	, AUTHORITY_OPERATOR 	, NULL },

	/* Camera Tamper Detect  PAGE */
	{ "dmvacfgtdenv1"      			, para_dmvaCfgTDEnv1					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvacfgtdenv2"      			, para_dmvaCfgTDEnv2					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvacfgtdsensitivity"         , para_dmvaCfgTDSensitivity			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvacfgtdsensitivityname"  	    , para_dmvaCfgTDSensitivityName				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvacfgtdresettime"      	    , para_dmvaCfgTDResetTime			, AUTHORITY_OPERATOR 	, NULL },

	/* Trip Zone PAGE */
	{ "dmvatzsensitivity"      	    , para_dmvaTZSensitivity				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzsensitivityname"  	    , para_dmvaTZSensitivityName				, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvatzpersonminwidth"      	, para_dmvaTZPersonMinWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzpersonminheight"      	, para_dmvaTZPersonMinHeight			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzvehicleminwidth"      	, para_dmvaTZVehicleMinWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzvehicleminheight"      	, para_dmvaTZVehicleMinHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzpersonmaxwidth"      	, para_dmvaTZPersonMaxWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzpersonmaxheight"      	, para_dmvaTZPersonMaxHeight			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzvehiclemaxwidth"      	, para_dmvaTZVehicleMaxWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzvehiclemaxheight"      	, para_dmvaTZVehicleMaxHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzdirection"      	    , para_dmvaTZDirection				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzpresentadjust"    	    , para_dmvaTZPresentAdjust				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzenable"      	        , para_dmvaTZEnable					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvatzzone1_roi_numsides"      	        , para_dmvaTZZone1_ROI_numSides					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_roi_numsides"      	        , para_dmvaTZZone2_ROI_numSides					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvatzzone1_x01"      	        , para_dmvaTZZone1_x01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x02"      	        , para_dmvaTZZone1_x02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x03"      	        , para_dmvaTZZone1_x03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x04"      	        , para_dmvaTZZone1_x04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x05"      	        , para_dmvaTZZone1_x05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x06"      	        , para_dmvaTZZone1_x06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x07"      	        , para_dmvaTZZone1_x07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x08"      	        , para_dmvaTZZone1_x08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x09"      	        , para_dmvaTZZone1_x09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x10"      	        , para_dmvaTZZone1_x10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x11"      	        , para_dmvaTZZone1_x11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x12"      	        , para_dmvaTZZone1_x12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x13"      	        , para_dmvaTZZone1_x13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x14"      	        , para_dmvaTZZone1_x14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x15"      	        , para_dmvaTZZone1_x15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_x16"      	        , para_dmvaTZZone1_x16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvatzzone1_y01"      	        , para_dmvaTZZone1_y01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y02"      	        , para_dmvaTZZone1_y02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y03"      	        , para_dmvaTZZone1_y03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y04"      	        , para_dmvaTZZone1_y04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y05"      	        , para_dmvaTZZone1_y05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y06"      	        , para_dmvaTZZone1_y06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y07"      	        , para_dmvaTZZone1_y07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y08"      	        , para_dmvaTZZone1_y08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y09"      	        , para_dmvaTZZone1_y09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y10"      	        , para_dmvaTZZone1_y10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y11"      	        , para_dmvaTZZone1_y11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y12"      	        , para_dmvaTZZone1_y12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y13"      	        , para_dmvaTZZone1_y13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y14"      	        , para_dmvaTZZone1_y14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y15"      	        , para_dmvaTZZone1_y15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone1_y16"      	        , para_dmvaTZZone1_y16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvatzzone2_x01"      	        , para_dmvaTZZone2_x01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x02"      	        , para_dmvaTZZone2_x02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x03"      	        , para_dmvaTZZone2_x03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x04"      	        , para_dmvaTZZone2_x04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x05"      	        , para_dmvaTZZone2_x05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x06"      	        , para_dmvaTZZone2_x06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x07"      	        , para_dmvaTZZone2_x07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x08"      	        , para_dmvaTZZone2_x08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x09"      	        , para_dmvaTZZone2_x09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x10"      	        , para_dmvaTZZone2_x10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x11"      	        , para_dmvaTZZone2_x11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x12"      	        , para_dmvaTZZone2_x12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x13"      	        , para_dmvaTZZone2_x13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x14"      	        , para_dmvaTZZone2_x14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x15"      	        , para_dmvaTZZone2_x15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_x16"      	        , para_dmvaTZZone2_x16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvatzzone2_y01"      	        , para_dmvaTZZone2_y01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y02"      	        , para_dmvaTZZone2_y02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y03"      	        , para_dmvaTZZone2_y03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y04"      	        , para_dmvaTZZone2_y04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y05"      	        , para_dmvaTZZone2_y05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y06"      	        , para_dmvaTZZone2_y06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y07"      	        , para_dmvaTZZone2_y07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y08"      	        , para_dmvaTZZone2_y08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y09"      	        , para_dmvaTZZone2_y09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y10"      	        , para_dmvaTZZone2_y10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y11"      	        , para_dmvaTZZone2_y11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y12"      	        , para_dmvaTZZone2_y12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y13"      	        , para_dmvaTZZone2_y13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y14"      	        , para_dmvaTZZone2_y14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y15"      	        , para_dmvaTZZone2_y15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzzone2_y16"      	        , para_dmvaTZZone2_y16					, AUTHORITY_OPERATOR 	, NULL },

	/* Save/Load DMVA TZ params */
	{ "dmvatzsave"      	        , para_dmvaTZSave					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzload"      	        , para_dmvaTZLoad					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzloadnames"      	    , para_dmvaTZLoadNames					, AUTHORITY_OPERATOR 	, NULL },

	/* Object Counting  PAGE */
	{ "dmvaocsensitivity"      		, para_dmvaOCSensitivity				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocsensitivityname"  	    , para_dmvaOCSensitivityName				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocobjectwidth"      		, para_dmvaOCObjectWidth				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocobjectheight"      		, para_dmvaOCObjectHeight			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocdirection"      		, para_dmvaOCDirection				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocenable"      			, para_dmvaOCEnable					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocleftright"      		, para_dmvaOCLeftRight				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaoctopbottom"      		, para_dmvaOCTopBottom				, AUTHORITY_OPERATOR 	, NULL },

	/* Save/Load DMVA OC params */
	{ "dmvaocsave"      	        , para_dmvaOCSave					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocload"      	        , para_dmvaOCLoad					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaocloadnames"      	    , para_dmvaOCLoadNames					, AUTHORITY_OPERATOR 	, NULL },

	/* Streaming MOS  PAGE */
	{ "dmvasmetasensitivity"      	, para_dmvaSMETASensitivity			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetasensitivityname"  	    , para_dmvaSMETASensitivityName				, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvasmetaenablestreamingdata"  , para_dmvaSMETAEnableStreamingData	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetastreambb"      		, para_dmvaSMETAStreamBB				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetastreamvelocity"      	, para_dmvaSMETAStreamVelocity		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetastreamhistogram"      , para_dmvaSMETAStreamHistogram		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetastreammoments"      	, para_dmvaSMETAStreamMoments			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetapresentadjust"      	, para_dmvaSMETAPresentAdjust			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetapersonminwidth"     	, para_dmvaSMETAPersonMinWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetapersonminheight"    	, para_dmvaSMETAPersonMinHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetavehicleminwidth"    	, para_dmvaSMETAVehicleMinWidth		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetavehicleminheight"   	, para_dmvaSMETAVehicleMinHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetapersonmaxwidth"		, para_dmvaSMETAPersonMaxWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetapersonmaxheight"      	, para_dmvaSMETAPersonMaxHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetavehiclemaxwidth"      	, para_dmvaSMETAVehicleMaxWidth		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetavehiclemaxheight"      , para_dmvaSMETAVehicleMaxHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetanumzones"      		, para_dmvaSMETANumZones				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_roi_numsides"      	        , para_dmvaSMETAZone1_ROI_numSides					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x01"      	        , para_dmvaSMETAZone1_x01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x02"      	        , para_dmvaSMETAZone1_x02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x03"      	        , para_dmvaSMETAZone1_x03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x04"      	        , para_dmvaSMETAZone1_x04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x05"      	        , para_dmvaSMETAZone1_x05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x06"      	        , para_dmvaSMETAZone1_x06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x07"      	        , para_dmvaSMETAZone1_x07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x08"      	        , para_dmvaSMETAZone1_x08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x09"      	        , para_dmvaSMETAZone1_x09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x10"      	        , para_dmvaSMETAZone1_x10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x11"      	        , para_dmvaSMETAZone1_x11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x12"      	        , para_dmvaSMETAZone1_x12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x13"      	        , para_dmvaSMETAZone1_x13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x14"      	        , para_dmvaSMETAZone1_x14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x15"      	        , para_dmvaSMETAZone1_x15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_x16"      	        , para_dmvaSMETAZone1_x16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvasmetazone1_y01"      	        , para_dmvaSMETAZone1_y01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y02"      	        , para_dmvaSMETAZone1_y02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y03"      	        , para_dmvaSMETAZone1_y03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y04"      	        , para_dmvaSMETAZone1_y04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y05"      	        , para_dmvaSMETAZone1_y05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y06"      	        , para_dmvaSMETAZone1_y06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y07"      	        , para_dmvaSMETAZone1_y07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y08"      	        , para_dmvaSMETAZone1_y08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y09"      	        , para_dmvaSMETAZone1_y09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y10"      	        , para_dmvaSMETAZone1_y10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y11"      	        , para_dmvaSMETAZone1_y11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y12"      	        , para_dmvaSMETAZone1_y12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y13"      	        , para_dmvaSMETAZone1_y13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y14"      	        , para_dmvaSMETAZone1_y14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y15"      	        , para_dmvaSMETAZone1_y15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetazone1_y16"      	        , para_dmvaSMETAZone1_y16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvasmetazone_1_label"      	    , para_dmvaSMETAZone_1_Label				, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvasmetaenablevelocityobjid"      	, para_dmvaSMETAEnableVelocityObjId		, AUTHORITY_OPERATOR 	, NULL },
	/* Save/Load DMVA SM params */
	{ "dmvasmetasave"      	        , para_dmvaSMETASave					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetaload"      	        , para_dmvaSMETALoad					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvasmetaloadnames"      	, para_dmvaSMETALoadNames				, AUTHORITY_OPERATOR 	, NULL },

	/* IMD  PAGE */
	{ "dmvaimdsensitivity"      		, para_dmvaIMDSensitivity			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdsensitivityname"  	    , para_dmvaIMDSensitivityName				, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdpresentadjust"      	, para_dmvaIMDPresentAdjust			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdpersonminwidth"     	, para_dmvaIMDPersonMinWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdpersonminheight"    	, para_dmvaIMDPersonMinHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdvehicleminwidth"    	, para_dmvaIMDVehicleMinWidth		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdvehicleminheight"   	, para_dmvaIMDVehicleMinHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdpersonmaxwidth"		, para_dmvaIMDPersonMaxWidth			, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdpersonmaxheight"      	, para_dmvaIMDPersonMaxHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdvehiclemaxwidth"      	, para_dmvaIMDVehicleMaxWidth		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdvehiclemaxheight"      , para_dmvaIMDVehicleMaxHeight		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdnumzones"      		, para_dmvaIMDNumZones				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzoneenable"      		, para_dmvaIMDZoneEnable				, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone1_roi_numsides"      	        , para_dmvaIMDZone1_ROI_numSides					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_roi_numsides"      	        , para_dmvaIMDZone2_ROI_numSides					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_roi_numsides"      	        , para_dmvaIMDZone3_ROI_numSides					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_roi_numsides"      	        , para_dmvaIMDZone4_ROI_numSides					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone1_x01"      	        , para_dmvaIMDZone1_x01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x02"      	        , para_dmvaIMDZone1_x02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x03"      	        , para_dmvaIMDZone1_x03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x04"      	        , para_dmvaIMDZone1_x04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x05"      	        , para_dmvaIMDZone1_x05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x06"      	        , para_dmvaIMDZone1_x06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x07"      	        , para_dmvaIMDZone1_x07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x08"      	        , para_dmvaIMDZone1_x08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x09"      	        , para_dmvaIMDZone1_x09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x10"      	        , para_dmvaIMDZone1_x10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x11"      	        , para_dmvaIMDZone1_x11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x12"      	        , para_dmvaIMDZone1_x12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x13"      	        , para_dmvaIMDZone1_x13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x14"      	        , para_dmvaIMDZone1_x14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x15"      	        , para_dmvaIMDZone1_x15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_x16"      	        , para_dmvaIMDZone1_x16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone1_y01"      	        , para_dmvaIMDZone1_y01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y02"      	        , para_dmvaIMDZone1_y02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y03"      	        , para_dmvaIMDZone1_y03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y04"      	        , para_dmvaIMDZone1_y04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y05"      	        , para_dmvaIMDZone1_y05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y06"      	        , para_dmvaIMDZone1_y06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y07"      	        , para_dmvaIMDZone1_y07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y08"      	        , para_dmvaIMDZone1_y08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y09"      	        , para_dmvaIMDZone1_y09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y10"      	        , para_dmvaIMDZone1_y10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y11"      	        , para_dmvaIMDZone1_y11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y12"      	        , para_dmvaIMDZone1_y12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y13"      	        , para_dmvaIMDZone1_y13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y14"      	        , para_dmvaIMDZone1_y14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y15"      	        , para_dmvaIMDZone1_y15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone1_y16"      	        , para_dmvaIMDZone1_y16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone2_x01"      	        , para_dmvaIMDZone2_x01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x02"      	        , para_dmvaIMDZone2_x02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x03"      	        , para_dmvaIMDZone2_x03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x04"      	        , para_dmvaIMDZone2_x04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x05"      	        , para_dmvaIMDZone2_x05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x06"      	        , para_dmvaIMDZone2_x06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x07"      	        , para_dmvaIMDZone2_x07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x08"      	        , para_dmvaIMDZone2_x08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x09"      	        , para_dmvaIMDZone2_x09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x10"      	        , para_dmvaIMDZone2_x10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x11"      	        , para_dmvaIMDZone2_x11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x12"      	        , para_dmvaIMDZone2_x12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x13"      	        , para_dmvaIMDZone2_x13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x14"      	        , para_dmvaIMDZone2_x14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x15"      	        , para_dmvaIMDZone2_x15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_x16"      	        , para_dmvaIMDZone2_x16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone2_y01"      	        , para_dmvaIMDZone2_y01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y02"      	        , para_dmvaIMDZone2_y02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y03"      	        , para_dmvaIMDZone2_y03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y04"      	        , para_dmvaIMDZone2_y04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y05"      	        , para_dmvaIMDZone2_y05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y06"      	        , para_dmvaIMDZone2_y06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y07"      	        , para_dmvaIMDZone2_y07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y08"      	        , para_dmvaIMDZone2_y08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y09"      	        , para_dmvaIMDZone2_y09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y10"      	        , para_dmvaIMDZone2_y10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y11"      	        , para_dmvaIMDZone2_y11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y12"      	        , para_dmvaIMDZone2_y12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y13"      	        , para_dmvaIMDZone2_y13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y14"      	        , para_dmvaIMDZone2_y14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y15"      	        , para_dmvaIMDZone2_y15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone2_y16"      	        , para_dmvaIMDZone2_y16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone3_x01"      	        , para_dmvaIMDZone3_x01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x02"      	        , para_dmvaIMDZone3_x02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x03"      	        , para_dmvaIMDZone3_x03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x04"      	        , para_dmvaIMDZone3_x04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x05"      	        , para_dmvaIMDZone3_x05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x06"      	        , para_dmvaIMDZone3_x06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x07"      	        , para_dmvaIMDZone3_x07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x08"      	        , para_dmvaIMDZone3_x08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x09"      	        , para_dmvaIMDZone3_x09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x10"      	        , para_dmvaIMDZone3_x10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x11"      	        , para_dmvaIMDZone3_x11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x12"      	        , para_dmvaIMDZone3_x12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x13"      	        , para_dmvaIMDZone3_x13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x14"      	        , para_dmvaIMDZone3_x14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x15"      	        , para_dmvaIMDZone3_x15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_x16"      	        , para_dmvaIMDZone3_x16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone3_y01"      	        , para_dmvaIMDZone3_y01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y02"      	        , para_dmvaIMDZone3_y02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y03"      	        , para_dmvaIMDZone3_y03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y04"      	        , para_dmvaIMDZone3_y04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y05"      	        , para_dmvaIMDZone3_y05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y06"      	        , para_dmvaIMDZone3_y06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y07"      	        , para_dmvaIMDZone3_y07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y08"      	        , para_dmvaIMDZone3_y08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y09"      	        , para_dmvaIMDZone3_y09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y10"      	        , para_dmvaIMDZone3_y10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y11"      	        , para_dmvaIMDZone3_y11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y12"      	        , para_dmvaIMDZone3_y12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y13"      	        , para_dmvaIMDZone3_y13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y14"      	        , para_dmvaIMDZone3_y14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y15"      	        , para_dmvaIMDZone3_y15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone3_y16"      	        , para_dmvaIMDZone3_y16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone4_x01"      	        , para_dmvaIMDZone4_x01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x02"      	        , para_dmvaIMDZone4_x02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x03"      	        , para_dmvaIMDZone4_x03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x04"      	        , para_dmvaIMDZone4_x04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x05"      	        , para_dmvaIMDZone4_x05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x06"      	        , para_dmvaIMDZone4_x06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x07"      	        , para_dmvaIMDZone4_x07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x08"      	        , para_dmvaIMDZone4_x08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x09"      	        , para_dmvaIMDZone4_x09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x10"      	        , para_dmvaIMDZone4_x10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x11"      	        , para_dmvaIMDZone4_x11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x12"      	        , para_dmvaIMDZone4_x12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x13"      	        , para_dmvaIMDZone4_x13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x14"      	        , para_dmvaIMDZone4_x14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x15"      	        , para_dmvaIMDZone4_x15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_x16"      	        , para_dmvaIMDZone4_x16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone4_y01"      	        , para_dmvaIMDZone4_y01					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y02"      	        , para_dmvaIMDZone4_y02					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y03"      	        , para_dmvaIMDZone4_y03					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y04"      	        , para_dmvaIMDZone4_y04					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y05"      	        , para_dmvaIMDZone4_y05					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y06"      	        , para_dmvaIMDZone4_y06					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y07"      	        , para_dmvaIMDZone4_y07					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y08"      	        , para_dmvaIMDZone4_y08					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y09"      	        , para_dmvaIMDZone4_y09					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y10"      	        , para_dmvaIMDZone4_y10					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y11"      	        , para_dmvaIMDZone4_y11					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y12"      	        , para_dmvaIMDZone4_y12					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y13"      	        , para_dmvaIMDZone4_y13					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y14"      	        , para_dmvaIMDZone4_y14					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y15"      	        , para_dmvaIMDZone4_y15					, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone4_y16"      	        , para_dmvaIMDZone4_y16					, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdzone_1_label"      	    , para_dmvaIMDZone_1_Label				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone_2_label"      	    , para_dmvaIMDZone_2_Label				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone_3_label"      	    , para_dmvaIMDZone_3_Label				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdzone_4_label"      	    , para_dmvaIMDZone_4_Label				, AUTHORITY_OPERATOR 	, NULL },

	/* DMVA IMD save/load params */
	{ "dmvaimdsave"      	    , para_dmvaIMDSave				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdload"      	    , para_dmvaIMDLoad				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdloadnames"      	, para_dmvaIMDLoadNames				, AUTHORITY_OPERATOR 	, NULL },

	{ "dmvacamidname"      				, para_dmvaCamIDName				, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistmonthname"      	, para_dmvaEventListMonthName	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlistdayname"      		, para_dmvaEventListDayName	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaeventlisttimename"      		, para_dmvaEventListTimeName	, AUTHORITY_OPERATOR 	, NULL },

	/* DMVA Scheduler */
	{ "dmvascheduleenable"      		, para_dmvaScheduleEnable		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvascheduleruletype"      		, para_dmvaScheduleRuleType		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvascheduleloadsetting"      	, para_dmvaScheduleLoadSetting	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaschedulerulelabel"      		, para_dmvaScheduleRuleLabel	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaschedulestartday"      		, para_dmvaScheduleStartDay		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaschedulestarttime"      		, para_dmvaScheduleStartTime	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvascheduleendday"      		, para_dmvaScheduleEndDay		, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvascheduleendtime"      		, para_dmvaScheduleEndTime		, AUTHORITY_OPERATOR 	, NULL },

    { "dmvascheduleresult"      		, para_dmvaScheduleResult 	        , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvascheduleruletypenames"      	, para_dmvaScheduleRuleTypeNames 	, AUTHORITY_OPERATOR 	, NULL },
	{ "dmvascheduledaynames"      	    , para_dmvaScheduleDayNames 	    , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvascheduleloadsettingnames"    , para_dmvaScheduleLoadSettingNames , AUTHORITY_OPERATOR 	, NULL },

	/* DMVA Enc ROI */
	{ "dmvaencroienable"                , para_dmvaEncRoiEnable , AUTHORITY_OPERATOR 	, NULL },

	/* TZ/OC box min/max size limit */
	{ "dmvatzocminwidthlimit"           , para_dmvaTZOCMinWidthLimit  , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzocminheightlimit"          , para_dmvaTZOCMinHeightLimit , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzocmaxwidthlimit"           , para_dmvaTZOCMaxWidthLimit  , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvatzocmaxheightlimit"          , para_dmvaTZOCMaxHeightLimit , AUTHORITY_OPERATOR 	, NULL },

	/* IMD/SMETA box min/max size limit */
	{ "dmvaimdsmetapersonminwidthlimit"  , para_dmvaIMDSMETAPersonMinWidthLimit  , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdsmetapersonminheightlimit" , para_dmvaIMDSMETAPersonMinHeightLimit , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdsmetapersonmaxwidthlimit"  , para_dmvaIMDSMETAPersonMaxWidthLimit  , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdsmetapersonmaxheightlimit" , para_dmvaIMDSMETAPersonMaxHeightLimit , AUTHORITY_OPERATOR 	, NULL },

	{ "dmvaimdsmetavehicleminwidthlimit"  , para_dmvaIMDSMETAVehicleMinWidthLimit  , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdsmetavehicleminheightlimit" , para_dmvaIMDSMETAVehicleMinHeightLimit , AUTHORITY_OPERATOR     , NULL },
	{ "dmvaimdsmetavehiclemaxwidthlimit"  , para_dmvaIMDSMETAVehicleMaxWidthLimit  , AUTHORITY_OPERATOR 	, NULL },
	{ "dmvaimdsmetavehiclemaxheightlimit" , para_dmvaIMDSMETAVehicleMaxHeightLimit , AUTHORITY_OPERATOR 	, NULL },

	/*      VIDEO ANALYTICS SCREEN 				*/
	{ "motionblock"		    , para_motionblock			, AUTHORITY_VIEWER 	, NULL },
	{ "motionsensitivity"	, para_motionsensitivity	, AUTHORITY_VIEWER 	, NULL },
	{ "motionname"		    , para_motionname			, AUTHORITY_VIEWER 	, NULL },
	{ "motioncenable"		, para_motioncenable		, AUTHORITY_VIEWER 	, NULL },
	{ "motioncvalue"      	, para_motioncvalue			, AUTHORITY_VIEWER 	, NULL },
	{ "fdetect"				, para_fdetect 				, AUTHORITY_VIEWER	, NULL },
	{ "fdetectname"			, para_fdetectname			, AUTHORITY_VIEWER	, NULL },
	{ "frecognition"        , para_frecognition       	, AUTHORITY_VIEWER 	, NULL },
	{ "frecognitionname"    , para_frecognitionname   	, AUTHORITY_VIEWER 	, NULL },
	{ "privacymask"         , para_privacymask       	, AUTHORITY_VIEWER 	, NULL },
	{ "maskoptions"         , para_maskoptions       	, AUTHORITY_VIEWER 	, NULL },
	{ "maskoptionsname"     , para_maskoptionsname   	, AUTHORITY_VIEWER 	, NULL },
	{ "fdconflevel"         , para_fdconflevel  		, AUTHORITY_VIEWER 	, NULL },
	{ "fdx"                 , para_fdx          		, AUTHORITY_VIEWER 	, NULL },
	{ "fdy"                 , para_fdy          		, AUTHORITY_VIEWER 	, NULL },
	{ "fdw"                 , para_fdw          		, AUTHORITY_VIEWER 	, NULL },
	{ "fdh"                 , para_fdh          		, AUTHORITY_VIEWER 	, NULL },
	{ "frconflevel"         , para_frconflevel  		, AUTHORITY_VIEWER 	, NULL },
	{ "fddirection"         , para_fddirection      	, AUTHORITY_VIEWER 	, NULL },
	{ "fddirectionname"     , para_fddirectionname  	, AUTHORITY_VIEWER 	, NULL },
	{ "frdatabase"          , para_frdatabase       	, AUTHORITY_VIEWER 	, NULL },

	/*      CAMERA SCREEN 				*/
    { "brightness"				, para_brightness			, AUTHORITY_VIEWER 		, NULL },
    { "contrast"				, para_contrast				, AUTHORITY_VIEWER 		, NULL },
 	{ "sharpness"				, para_sharpness			, AUTHORITY_VIEWER 		, NULL },
    { "saturation"				, para_saturation			, AUTHORITY_VIEWER 		, NULL },
  	{ "blc"						, para_blc					, AUTHORITY_VIEWER 		, NULL },
 	{ "backlight"				, para_backlight			, AUTHORITY_VIEWER 		, NULL },
 	{ "backlightname"			, para_backlightname		, AUTHORITY_VIEWER 		, NULL },
	{ "dynrange"    			, para_dynrange        		, AUTHORITY_VIEWER   	, NULL },
	{ "dynrangename"   			, para_dynrangename    		, AUTHORITY_VIEWER   	, NULL },
	{ "maxgain"                 , para_dynrangestrength     , AUTHORITY_VIEWER 	, NULL },
    { "maxgainname"             , para_dynrangestrengthname , AUTHORITY_VIEWER 	, NULL },
 	{ "awb"				    	, para_awb					, AUTHORITY_VIEWER 		, NULL },
    { "awbname"					, para_awbname				, AUTHORITY_VIEWER 		, NULL },
	{ "colorkiller"				, para_daynight				, AUTHORITY_VIEWER 		, NULL },
    { "daynightname"			, para_daynightname			, AUTHORITY_VIEWER 		, NULL },
    { "histogram"     			, para_histogram          	, AUTHORITY_VIEWER   	, NULL },
    { "binning"					, para_binning				, AUTHORITY_VIEWER 		, NULL },
    { "binningname"				, para_binningname			, AUTHORITY_VIEWER 		, NULL },
	{ "img2a"					, para_image2a				, AUTHORITY_VIEWER 		, NULL },
	{ "img2aname"				, para_img2aname			, AUTHORITY_VIEWER 		, NULL },
	{ "img2atype"				, para_image2atype			, AUTHORITY_VIEWER 		, NULL },
	{ "img2atypename"			, para_img2atypename		, AUTHORITY_VIEWER 		, NULL },
	{ "exposurectrl"		    , para_imagesource	    	, AUTHORITY_VIEWER 		, NULL },
	{ "exposurename"		    , para_exposurename		    , AUTHORITY_VIEWER 		, NULL },
	{ "priority"		    	, para_priority		    	, AUTHORITY_VIEWER 		, NULL },
	{ "priorityname"		    , para_priorityname	    	, AUTHORITY_VIEWER 		, NULL },
	{ "vidstb1"  				, para_vidstb				, AUTHORITY_VIEWER 		, NULL },
	{ "lensdistortcorrection"  	, para_ldc					, AUTHORITY_VIEWER 		, NULL },
	{ "tnfltctrl"               , para_vnf           		, AUTHORITY_VIEWER 	, NULL },
   	{ "nfltctrl"                , para_vnfmode            	, AUTHORITY_VIEWER 	, NULL },
   	{ "nfltctrlname"            , para_vnfmodename        	, AUTHORITY_VIEWER 	, NULL },
	{ "maxexposuretime"         , para_vnfstrength      	, AUTHORITY_VIEWER 	, NULL },
    { "maxexposuretimename"     , para_vnfstrengthname		, AUTHORITY_VIEWER 	, NULL },
	{ "framectrl"               , para_framectrl            , AUTHORITY_VIEWER 	, NULL },
	{ "framectrlname"       	, para_framectrlname        , AUTHORITY_VIEWER 	, NULL },

	/*      AUDIO SCREEN 				*/
	{ "audioenable"				, para_audioenable			, AUTHORITY_VIEWER   , NULL },
	{ "audiomode"              	, para_audiomode            , AUTHORITY_VIEWER , NULL },
	{ "audiomodename"          	, para_audiomodename        , AUTHORITY_VIEWER , NULL },
	{ "audioinvolume"          	, para_audioinvolume        , AUTHORITY_VIEWER , NULL },
	{ "encoding"               	, para_encoding             , AUTHORITY_VIEWER , NULL },
	{ "encodingname"           	, para_encodingname         , AUTHORITY_VIEWER , NULL },
	{ "samplerate"             	, para_samplerate           , AUTHORITY_VIEWER , NULL },
	{ "sampleratename"         	, para_sampleratename       , AUTHORITY_VIEWER , NULL },
	{ "audiobitrate"           	, para_audiobitrate         , AUTHORITY_VIEWER , NULL },
	{ "audiobitratename"       	, para_audiobitratename     , AUTHORITY_VIEWER , NULL },
	{ "audiobitratenameall"    	, para_audiobitratenameall  , AUTHORITY_VIEWER , NULL },
	{ "alarmlevel"             	, para_alarmlevel           , AUTHORITY_VIEWER , NULL },
	{ "audiooutvolume"       	, para_audiooutvolume     	, AUTHORITY_VIEWER , NULL },
	{ "audioreceiverenable"     , para_audioreceiverenable  , AUTHORITY_VIEWER , NULL },
	{ "audioserverip"           , para_audioserverip        , AUTHORITY_VIEWER , NULL },

	/*      DATE TIME SCREEN				*/
	{ "date"            		, para_date                 , AUTHORITY_VIEWER  , NULL },
	{ "time"            		, para_time                 , AUTHORITY_VIEWER  , NULL },
	{ "timezone"    			, para_sntptimezone         , AUTHORITY_VIEWER 	, NULL },
	{ "timezonename"			, para_timezonename			, AUTHORITY_VIEWER 	, NULL },
	{ "daylight"				, para_daylight				, AUTHORITY_VIEWER  , NULL },
    { "dateformat"				, para_dateformat			, AUTHORITY_VIEWER  , NULL },
    { "dateformatname"         	, para_dateformatname       , AUTHORITY_VIEWER 	, NULL },
    { "tstampformat"        	, para_tstampformat      	, AUTHORITY_VIEWER 	, NULL },
	{ "tstampformatname"    	, para_tstampformatname  	, AUTHORITY_VIEWER 	, NULL },
	{ "dateposition"           	, para_dateposition         , AUTHORITY_VIEWER 	, NULL },
	{ "timeposition"           	, para_timeposition         , AUTHORITY_VIEWER 	, NULL },
	{ "datetimepositionname"   	, para_datetimepositionname , AUTHORITY_VIEWER 	, NULL },

	/*      NETWORK PORT				*/
	{ "dhcpenable"      , para_dhcpenable           , AUTHORITY_VIEWER , NULL },
	{ "netip"           , para_netip                , AUTHORITY_VIEWER , NULL },
	{ "netmask"         , para_netmask              , AUTHORITY_VIEWER , NULL },
	{ "gateway"         , para_gateway              , AUTHORITY_VIEWER , NULL },
	{ "dnsip"           , para_dnsip                , AUTHORITY_VIEWER , NULL },

	{ "ftpip"           , para_ftpip                , AUTHORITY_VIEWER , NULL },
	{ "ftpipport"       , para_ftpipport            , AUTHORITY_VIEWER , NULL },
	{ "ftpuser"         , para_ftpuser              , AUTHORITY_VIEWER , NULL },
	{ "ftppassword"     , para_ftppassword          , AUTHORITY_VIEWER , NULL },
	{ "ftppath"         , para_ftppath              , AUTHORITY_VIEWER , NULL },
	{ "maxftpuserlen"   , para_maxftpuserlen        , AUTHORITY_VIEWER , NULL },
	{ "maxftppwdlen"    , para_maxftppwdlen         , AUTHORITY_VIEWER , NULL },
	{ "maxftppathlen"   , para_maxftppathlen        , AUTHORITY_VIEWER , NULL },

	{ "smtpauth"        , para_smtpauth             , AUTHORITY_VIEWER , NULL },
	{ "smtpuser"        , para_smtpuser             , AUTHORITY_VIEWER , NULL },
	{ "maxsmtpuser"     , para_maxsmtpuser          , AUTHORITY_VIEWER , NULL },
	{ "smtppwd"         , para_smtppwd              , AUTHORITY_VIEWER , NULL },
	{ "maxsmtppwd"      , para_maxsmtppwd           , AUTHORITY_VIEWER , NULL },
	{ "smtpsender"      , para_smtpsender           , AUTHORITY_VIEWER , NULL },
	{ "maxsmtpsender"   , para_maxsmtpsender        , AUTHORITY_VIEWER , NULL },
	{ "smtpip"          , para_smtpip               , AUTHORITY_VIEWER , NULL },
	{ "smtpport"        , para_smtpport             , AUTHORITY_VIEWER , NULL },
	{ "emailuser"       , para_emailuser            , AUTHORITY_VIEWER , NULL },
	{ "maxemailuserlen" , para_maxemailuserlen      , AUTHORITY_VIEWER , NULL },

	{ "multicast"		, para_multicast  			, AUTHORITY_VIEWER , NULL },

	{ "sntpip"          , para_sntpip               , AUTHORITY_VIEWER , NULL },

	{ "httpport"        , para_httpport             , AUTHORITY_VIEWER , NULL },
	{ "httpsport"       , para_httpsport         	, AUTHORITY_VIEWER , NULL },
    { "portinput"		, para_giointype  		 	, AUTHORITY_VIEWER , NULL },
    { "portinputname"	, para_gioinname  		 	, AUTHORITY_VIEWER , NULL },
    { "portoutput"		, para_gioouttype  		 	, AUTHORITY_VIEWER , NULL },
    { "portoutputname"	, para_giooutname  		 	, AUTHORITY_VIEWER , NULL },
	{ "rs485"           , para_rs485             	, AUTHORITY_VIEWER , NULL },
	{ "rs485name"       , para_rs485name         	, AUTHORITY_VIEWER , NULL },

	/*      ALARM PAGE				*/
	{ "alarmenable"         , para_alarmenable      		, AUTHORITY_VIEWER , NULL },
    { "alarmduration"		, para_alarmduration			, AUTHORITY_VIEWER , NULL },
    { "recordduration"		, para_recordduration			, AUTHORITY_VIEWER , NULL },
	{ "motionenable"		, para_motionenable				, AUTHORITY_VIEWER , NULL },
    { "lostalarm"		    , para_lostalarm				, AUTHORITY_VIEWER , NULL },
    { "darkblankalarm"      , para_audioalarm   			, AUTHORITY_VIEWER , NULL },
    { "audioalarm"      	, para_audioalarm   			, AUTHORITY_VIEWER , NULL },
	{ "extalarm"            , para_extalarm         		, AUTHORITY_VIEWER , NULL },
	{ "exttriggerinput"     , para_gioinenable  			, AUTHORITY_VIEWER , NULL },
    { "exttriggeroutput"    , para_giooutenable 			, AUTHORITY_VIEWER , NULL },
 	{ "exttriggername"      , para_exttriggername   		, AUTHORITY_VIEWER , NULL },
    { "aftpenable"			, para_aftpenable				, AUTHORITY_VIEWER , NULL },
 	{ "ftpfileformat"	    , para_ftpfileformat			, AUTHORITY_VIEWER , NULL },
	{ "ftpfileformatname"   , para_formatName				, AUTHORITY_VIEWER , NULL },
    { "asmtpenable"			, para_asmtpenable				, AUTHORITY_VIEWER , NULL },
    { "attfileformat"	    , para_attfileformat			, AUTHORITY_VIEWER , NULL },
	{ "attfileformatname"   , para_formatName				, AUTHORITY_VIEWER , NULL },
	{ "asmtpattach"		    , para_asmtpattach				, AUTHORITY_VIEWER , NULL },
	{ "smtpminattach"       , para_smtpminattach			, AUTHORITY_VIEWER , NULL },
	{ "smtpmaxattach"       , para_smtpmaxattach			, AUTHORITY_VIEWER , NULL },
    { "sdaenable"		    , para_sdaenable				, AUTHORITY_VIEWER , NULL },
    { "sdfileformat"	    , para_sdfileformat				, AUTHORITY_VIEWER , NULL },
	{ "sdfileformatname"    , para_sdformatName				, AUTHORITY_VIEWER , NULL },
	{ "alarmlocalstorage"   , para_alarmlocalstorage        , AUTHORITY_VIEWER , NULL },
	{ "alarmaudioplay"      , para_alarmaudioplay           , AUTHORITY_VIEWER , NULL },
	{ "alarmaudiofile"      , para_alarmaudiofile           , AUTHORITY_VIEWER , NULL },
	{ "alarmaudiofilename"  , para_alarmaudiofilename       , AUTHORITY_VIEWER , NULL },

	/*      SCHEDULE PAGE				*/
	{ "rftpenable"          , para_rftpenable		, AUTHORITY_VIEWER , NULL },
	{ "sdrenable"		    , para_sdrenable		, AUTHORITY_VIEWER , NULL },
	{ "schedule"			, para_schedule			, AUTHORITY_VIEWER , NULL },
	{ "recordlocalstorage"		 , para_recordlocalstorage		 , AUTHORITY_VIEWER , NULL },
    { "schedulerepeatenable"     , para_schedulerepeatenable     , AUTHORITY_VIEWER , NULL },
	{ "schedulenumweeks"         , para_schedulenumweeks         , AUTHORITY_VIEWER , NULL },
    { "scheduleinfiniteenable"   , para_scheduleinfiniteenable   , AUTHORITY_VIEWER , NULL },

	/*       SUPPORT PAGE       */
	{ "kernelversion"		, para_kernelversion		, AUTHORITY_VIEWER 	, NULL },
	{ "biosversion"			, para_biosversion			, AUTHORITY_VIEWER 	, NULL },
    { "softwareversion"	    , para_softwareversion  	, AUTHORITY_VIEWER 	, NULL },

	/*       SDCARD EXPLORER PAGE       */
	{ "sdinsert"        	, para_sdinsert         , AUTHORITY_VIEWER   , NULL },
	{ "sdleft"		        , para_sdleft			, AUTHORITY_VIEWER , NULL },
	{ "sdused"		        , para_sdused			, AUTHORITY_VIEWER , NULL },

	/*       Others       */
	{ "reloadflag"			, para_reloadflag			, AUTHORITY_VIEWER 		, NULL },
	{ "reloadtime"			, para_reloadtime			, AUTHORITY_VIEWER 		, NULL },
    { "maxaccount"		    , para_maxaccount			, AUTHORITY_VIEWER 		, NULL },
    { "minnamelen"		    , para_minnamelen			, AUTHORITY_VIEWER 		, NULL },
    { "maxnamelen"	    	, para_maxnamelen			, AUTHORITY_VIEWER 		, NULL },
    { "minpwdlen"		    , para_minpwdlen			, AUTHORITY_VIEWER 		, NULL },
    { "maxpwdlen"		    , para_maxpwdlen			, AUTHORITY_VIEWER 		, NULL },
    { "bkupfirmware"		, para_bkupfirmware			, AUTHORITY_VIEWER 		, NULL },
};

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
unsigned int arg_hash_cal_value(char *name)
{
	unsigned int value = 0;

	while (*name)
		value = value * 37 + (unsigned int)(*name++);
	return value;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void arg_hash_insert_entry(ARG_HASH_TABLE *table, HTML_ARGUMENT *arg)
{
	if (table->harg) {
		arg->next = table->harg;
	}
	table->harg = arg;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int arg_hash_table_init(void)
{
	int i;

	if ( (arg_hash = (ARG_HASH_TABLE *)calloc(sizeof(ARG_HASH_TABLE), MAX_ARG_HASH_SIZE)) == NULL) {
		return -1;
	}
	for (i=0; i<HASH_TABLE_SIZE; i++) {
		arg_hash_insert_entry(arg_hash+(arg_hash_cal_value(HttpArgument[i].name)%MAX_ARG_HASH_SIZE), HttpArgument+i);
	}
	return 0;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int TranslateWebPara(AUTHORITY authority, char *target, char *para, char *subpara)
{
	HTML_ARGUMENT *htmp;
	htmp = arg_hash[arg_hash_cal_value(para)%MAX_ARG_HASH_SIZE].harg;

	while (htmp) {
		if ( strcasecmp(htmp->name, para) == 0 ) {
			if (authority > htmp->authority) {
				dbg("[%s.%s] permission denied!!!\n", para, subpara);
				return -1;
			}
			return (*htmp->handler) (target, subpara);
		}
		htmp = htmp->next;
	}
	dbg("[%s.%s] not found\n", para, subpara);
	return -1;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

void arg_hash_table_cleanup()
{
	free(arg_hash);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

int ShowAllWebValue(char *data, int max_size, AUTHORITY authority)
{
	HTML_ARGUMENT *htmp;
	int i, total_size = 0, size;
	char buf[1024];
	for(i = 0;i < HASH_TABLE_SIZE; i++){
		htmp = &HttpArgument[i];
		if (authority > htmp->authority)
			continue;
		if(htmp ->handler == NULL)
			continue;
		size = sprintf(buf, "%s=", htmp->name);
		if(total_size + size + 1 > max_size){
			total_size = sprintf(data, "Not enogh size to show");
			break;
		}
		total_size += sprintf(data+total_size, "%s", buf);
		size = (*htmp->handler) (buf, "");
		if(size < 0){
			size = sprintf(buf, "Error return");
		}
		if(total_size + size + 1 > max_size){
			total_size = sprintf(data, "Not enogh size to show");
			break;
		}
		total_size += sprintf(data+total_size, "%s<br>", buf);
	}
	return total_size;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

static int ShowPara(char* buf, int index, char* name, AUTHORITY authority)
{
	char strAuthority[5][20] = {"ADMINISTRATOR","OPERATOR","VIEWER","NONE","ERROR"};
	int a_index;
	switch(authority){
		case AUTHORITY_ADMIN:
			a_index = 0;
			break;
		case AUTHORITY_OPERATOR:
			a_index = 1;
			break;
		case AUTHORITY_VIEWER:
			a_index = 2;
			break;
		case AUTHORITY_NONE:
			a_index = 3;
			break;
		default:
			a_index = 4;
			break;
	}
	return sprintf(buf, "%3d.%-25s%s\n", index, name, strAuthority[a_index]);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

int ShowAllPara(char *data, int max_size, AUTHORITY authority)
{
	HTML_ARGUMENT *htmp;
	int i, total_size = 0, size,count = 0;
	char buf[80];
	for(i = 0;i < HASH_TABLE_SIZE; i++){
		htmp = &HttpArgument[i];
		if(htmp ->handler == NULL)
			continue;
		size = ShowPara(buf, ++count, htmp->name, htmp->authority);
		if(total_size + size + 1 > max_size){
			total_size = sprintf(data, "Not enogh size to show");
			break;
		}
		total_size += sprintf(data+total_size, "%s", buf);
	}
	return total_size;
}

