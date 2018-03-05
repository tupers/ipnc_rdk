
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys_env_type.h>
#include <sysctrl.h>
#include <para_list.h>
#include <sys_env_type.h>
#include <web_translate.h>
#include <system_default.h>
#include <file_list.h>
#include <onvif.h>
#include <file_msg_drv.h>
#include <net_config.h>


extern int tempdhcp_flag;
char *_TZ_name[25] ={
	"GMT-12",// Eniwetok, Kwajalein", 
	"GMT-11",// Midway Island, Samoa",
	"GMT-10",// Hawaii", // HAW10
	"AKST9AKDT,M3.2.0,M11.1.0",// Alaska", // AKST9AKDT,M3.2.0,M11.1.0
	"PST8PDT,M3.2.0,M11.1.0",// Pacific Time (US & Canada), Tijuana", // PST8PDT,M3.2.0,M11.1.0
	"GMT-07",// Mountain Time (US & Canada), Arizona", // MST7
	"CST6CDT,M3.2.0,M11.1.0",// Central Time (US & Canada), Mexico City, Tegucigalpa, Saskatchewan", // CST6CDT,M3.2.0,M11.1.0
	"EST5EDT,M3.2.0,M11.1.0",// Eastern Time (US & Canada), Indiana(East), Bogota, Lima", // EST5EDT,M3.2.0,M11.1.0
	"GMT-04",// Atlantic Time (Canada), Caracas, La Paz", // AST4ADT,M4.1.0/00:01:00,M10.5.0/00:01:00
	"GMT-03",// Brasilia, Buenos Aires, Georgetown",
	"GMT-02",// Mid-Atlantic",
	"GMT-01",// Azores, Cape Verdes Is.",
	"GMT+00",// GMT, Dublin, Edinburgh, London, Lisbon, Monrovia, Casablanca", // GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00
	"GMT+01",// Berlin, Stockholm, Rome, Bern, Brussels, Vienna, Paris, Madrid, Amsterdam, Prague, Warsaw, Budapest", // CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
	"GMT+02",// Athens, Helsinki, Istanbul, Cairo, Eastern Europe, Harare, Pretoria, Israel", // EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00
	"GMT+03",// Baghdad, Kuwait, Nairobi, Riyadh, Moscow, St. Petersburg, Kazan, Volgograd", // MSK-3MSD,M3.5.0/2,M10.5.0/3
	"GMT+04",// Abu Dhabi, Muscat, Tbilisi",
	"GMT+05",// Islamabad, Karachi, Ekaterinburg, Tashkent",
	"GMT+06",// Alma Ata, Dhaka",
	"GMT+07",// Bangkok, Jakarta, Hanoi",
	"GMT+08",// Taipei, Beijing, Chongqing, Urumqi, Hong Kong, Perth, Singapore",
	"GMT+09",// Tokyo, Osaka, Sapporo, Seoul, Yakutsk",
	"GMT+10",// Brisbane, Melbourne, Sydney, Guam, Port Moresby, Vladivostok, Hobart",
	"GMT+11",// Magadan, Solomon Is., New Caledonia",
	"GMT+12",// Fiji, Kamchatka, Marshall Is., Wellington, Auckland"
}; 


int ocheckusername(struct soap *soap)
{
	int i;
	SysInfo* oSysInfo = GetSysInfo();

	for(i = 0; i < ACOUNT_NUM; i++) 
	{			
		if (strcmp(oSysInfo->acounts[i].user,soap->header->wsse__Security->UsernameToken->Username)==0) 
		{
			//Check Password include here
			return oSysInfo->acounts[i].authority;
			// 1: admin , 2: operator, 3: viewer, 4: supervisor
		}
	}
	return 0;	
}

char* oget_ipaddress(void)
{
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	char *_IPv4Address = (char*)malloc(20*sizeof(char)); 
	sprintf(_IPv4Address, "http://%03d.%03d.%03d.%03d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	return _IPv4Address;
}

int oget_encoding(int streamId)
{
	SysInfo* oSysInfo = GetSysInfo();
	int encode=oSysInfo->lan_config.nVideocombo;
	if(oSysInfo->lan_config.nStreamType == 0) { //single
		if(oSysInfo->lan_config.nVideocombo == 0) {  // H264
			return 2;
		} else if(oSysInfo->lan_config.nVideocombo == 1) { //MPEG4
			return 1;
		} else if(oSysInfo->lan_config.nVideocombo == 2) { //Mjpeg
			if(oSysInfo->lan_config.nVideocodecres == 0 ||
					oSysInfo->lan_config.nVideocodecres == 2 ||
					oSysInfo->lan_config.nVideocodecres == 4) {
				return 2;
			} else return 0;
		} else return 2;
	} else if(oSysInfo->lan_config.nStreamType == 1) {
		if(oSysInfo->lan_config.nVideocombo == 0) { //  H.264+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 2;
				else if(streamId == 1) return 0;
				else return 2;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 1) {//MPEG4+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 1;
				else if(streamId == 1) return 0;
				else return 2;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 2) {//H264+H264;
			if(oSysInfo->lan_config.nVideocodecres == 0) { 
				if(streamId == 0) return 2;
				else if(streamId == 1) return 2;
				else return 2;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 3) {//MPEG4+MPEG4;
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 1;
				else if(streamId == 1) return 1;
				else return 2;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 4) {//H264+MPEG4
			if(oSysInfo->lan_config.nVideocodecres == 0) { 
				if(streamId == 0) return 2;
				else if(streamId == 1) return 1;
				return 2;
			}
		}
	} else if(oSysInfo->lan_config.nStreamType == 2) {//triple
		if(oSysInfo->lan_config.nVideocombo == 0) {//H264+H264+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				//H264:720,JPEG:VGA,H264:QVGA@
				if(streamId == 0) return 2;
				else if(streamId == 1) return 2;
				else if(streamId == 2) return 0;
			} 
		} else if (oSysInfo->lan_config.nVideocombo == 1) {// MPEG4+MPEG4+MJPEG
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 1;
				else if(streamId == 1) return 1;
				else if(streamId == 2) return 0;
				return 1;
			}
		}
	}
	return 2; 
}
int oget_bitrate(int streamId)
{
	SysInfo* oSysInfo = GetSysInfo();
	if(streamId == 0)
		return oSysInfo->lan_config.nMpeg41bitrate/1000;
	else if(streamId == 1)
		return oSysInfo->lan_config.nMpeg42bitrate/1000;
	else
		return oSysInfo->lan_config.nMpeg41bitrate/1000;
}

int oget_numberofstream(void)
{
	SysInfo* oSysInfo = GetSysInfo();
	return (oSysInfo->lan_config.nStreamType + 1);
}

int oget_framerate(int streamId)
{
	SysInfo* oSysInfo = GetSysInfo();
	if(oSysInfo->lan_config.nStreamType == 0){ //single
		if(oSysInfo->lan_config.nVideocombo == 0){  // H264
			if(oSysInfo->lan_config.nVideocodecres >= 0 &&
					oSysInfo->lan_config.nVideocodecres <= 3){
				//H264:720;H264:D1;H264:SXVGA;H264:1080;H264:720MAX60@
				if(streamId == 0) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				}
			} 
		} else if(oSysInfo->lan_config.nVideocombo == 1) { //MPEG4
			if(oSysInfo->lan_config.nVideocodecres >= 0 &&
					oSysInfo->lan_config.nVideocodecres <= 3) {  
				//MPEG4:720;MPEG4:D1;MPEG4:SXVGA;MPEG4:1080;MPEG4:720MAX60@
				if(streamId == 0) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				} 
			}
		} else if(oSysInfo->lan_config.nVideocombo == 2) { //Mjpeg
			if(oSysInfo->lan_config.nVideocodecres >= 0 &&
					oSysInfo->lan_config.nVideocodecres <= 1) { 
				//H264:2MP;JPG:2MP;
				if(streamId == 0) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				} 
			} else if(oSysInfo->lan_config.nVideocodecres >= 2 &&
					oSysInfo->lan_config.nVideocodecres <= 3) {
				//H264:3MP;JPG:3MP
				if(streamId == 0) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 20;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 8;
				} 
			}
		}
	} else if(oSysInfo->lan_config.nStreamType == 1)
	{
		if(oSysInfo->lan_config.nVideocombo == 0) { //  H.264+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres <= 4) { 
				//H264:720,JPEG:VGA;H264:D1,JPEG:D1;H264:720,JPEG:720;H264:1080,JPEG:QVGA;H264:1080,JPEG:D1@
				if(streamId <= 1) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				} else return 30;
			}
		}
		else if(oSysInfo->lan_config.nVideocombo == 1) {//MPEG4+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres <= 4) {
				//MPEG4:720,JPEG:VGA;MPEG4:D1,JPEG:D1;MPEG4:720,JPEG:720;MPEG4:1080,JPEG:QVGA;MPEG4:1080,JPEG:D1@
				if(streamId <= 1) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				}else return 30;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 2) {//H264+H264;
			if(oSysInfo->lan_config.nVideocodecres <= 4) {
				//H264:720,H264:QVGA;H264:D1,H264:D1;H264:D1,H264:QVGA;H264:1080,H264:QVGA;H264:1080,H264:D1@
				if(streamId <= 1) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				} else return 30;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 3) {//MPEG4+MPEG4;
			if(oSysInfo->lan_config.nVideocodecres <= 4) { //MPEG4:1080P
				//MPEG4:720,MPEG4:QVGA;MPEG4:D1,MPEG4:D1;MPEG4:D1,MPEG4:QVGA;MPEG4:1080,MPEG4:QVGA;MPEG4:1080,MPEG4:D1@
				if(streamId <= 1) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				} else return 30;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 4) {//H264+MPEG4
			if(oSysInfo->lan_config.nVideocodecres <= 1) { //MPEG4:1080P
				//H264:D1,MPEG4:D1;H264:1080,MPEG4:D1@
				if(streamId <= 1) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				} else return 30;
			}
		}

	} else if(oSysInfo->lan_config.nStreamType == 2) {//triple
		if(oSysInfo->lan_config.nVideocombo == 0) {//H264+H264+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				//H264:720,JPEG:VGA,H264:QVGA@
				if(streamId <= 2) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 8;
				} else return 30;
			}
		} else if (oSysInfo->lan_config.nVideocombo == 1) {// MPEG4+MPEG4+MJPEG
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				//MPEG4:720,JPEG:VGA,MPEG4:QVGA
				if(streamId <= 2) {
					if(oSysInfo->lan_config.nFrameRate1 == 0)   return 30;
					else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
					else if(oSysInfo->lan_config.nFrameRate1 == 2)  return 15;
					else if(oSysInfo->lan_config.nFrameRate1 == 3)  return 5;
				} else return 30;
			}
		} 
	}
	return 0;
}

int oget_ratecontrol(int streamId)
{
	SysInfo* oSysInfo = GetSysInfo();
	if(streamId == 0)
		return oSysInfo->lan_config.nRateControl1;
	else if(streamId == 1)
		return oSysInfo->lan_config.nRateControl2;
	else
		return 0;
}

int oget_ipratio(int streamId)
{
	SysInfo* oSysInfo = GetSysInfo();
	if(streamId == 0)
		return oSysInfo->codec_advconfig[0].ipRatio;
	else if(streamId == 1)
		return oSysInfo->codec_advconfig[1].ipRatio;
	else
		return oSysInfo->codec_advconfig[0].ipRatio;
}

int oget_resolutionwidth(int streamId)
{
	SysInfo* oSysInfo = GetSysInfo();

	if(oSysInfo->lan_config.nStreamType == 0) { //single
		if(oSysInfo->lan_config.nVideocombo <= 1) {  // H264  //MPEG4
			//H264:720;H264:D1;H264:SXVGA;H264:1080;H264:720MAX60@
			//MPEG4:720;MPEG4:D1;MPEG4:SXVGA;MPEG4:1080;MPEG4:720MAX60@
			if(oSysInfo->lan_config.nVideocodecres == 0) { 
				return 1280;
			} else if(oSysInfo->lan_config.nVideocodecres == 1) {
				return 720;
			}else if(oSysInfo->lan_config.nVideocodecres == 2) {
				return 1280;
			}else if(oSysInfo->lan_config.nVideocodecres == 3) {
				return 1920;
			}else if(oSysInfo->lan_config.nVideocodecres == 4) {
				return 720;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 2) { //Mjpeg
			//H264:2MP;JPG:2MP;H264:3MP;JPG:3MP;H264:5MP;JPG:5MP@
			if(oSysInfo->lan_config.nVideocodecres <= 1) { //MPEG4:1080P
				return 1600;
			} else if (oSysInfo->lan_config.nVideocodecres <= 3) {
				return 2048;//1536
			} else if (oSysInfo->lan_config.nVideocodecres <= 5) {
				return 2592;//1920
			}
		} else return 1920;
	} else if(oSysInfo->lan_config.nStreamType == 1) {
		if(oSysInfo->lan_config.nVideocombo <= 1) { //  H.264+MJPEG; //MPEG4+MJPEG;
			// H264:720,JPEG:VGA; H264:D1,JPEG:D1; H264:720,JPEG:720; H264:1080,JPEG:QVGA; H264:1080,JPEG:D1@
			if(oSysInfo->lan_config.nVideocodecres == 0) { 
				if(streamId == 0) return 1280;
				else if(streamId == 1) return 640;
			} else  if(oSysInfo->lan_config.nVideocodecres == 1) {
				if(streamId == 0) return 720;
				else if(streamId == 1) return 720;
			} else  if(oSysInfo->lan_config.nVideocodecres == 2) {
				if(streamId == 0) return 1280;
				else if(streamId == 1) return 1280;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1920;
				else if(streamId == 1) return 320;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1920;
				else if(streamId == 1) return 720;
			}
		} else if(oSysInfo->lan_config.nVideocombo <= 3) {//H264+H264;
			//H264:720,H264:QVGA;H264:D1,H264:D1;H264:D1,H264:QVGA;H264:1080,H264:QVGA;H264:1080,H264:D1@
			//MPEG4:720,MPEG4:QVGA;MPEG4:D1,MPEG4:D1;MPEG4:D1,MPEG4:QVGA;MPEG4:1080,MPEG4:QVGA;MPEG4:1080,MPEG4:D1@
			if(oSysInfo->lan_config.nVideocodecres == 0) { //MPEG4:1080P
				if(streamId == 0) return 1280;
				else if(streamId == 1) return 320;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 1) {
				if(streamId == 0) return 720;
				else if(streamId == 1) return 720;
			} else  if(oSysInfo->lan_config.nVideocodecres == 2) {
				if(streamId == 0) return 720;
				else if(streamId == 1) return 320;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1920;
				else if(streamId == 1) return 320;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1920;
				else if(streamId == 1) return 720;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 4) {//H264+MPEG4
			//H264:D1,MPEG4:D1;H264:1080,MPEG4:D1@
			if(oSysInfo->lan_config.nVideocodecres == 0) { 
				if(streamId == 0) return 720;
				else if(streamId == 1) return 720;
			} else if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 1920;
				else if(streamId == 1) return 720;
			}
		}
	}  else if(oSysInfo->lan_config.nStreamType == 2) {//triple
		if(oSysInfo->lan_config.nVideocombo == 0) {//H264+H264+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres == 0) { //H264:1080P,H264:D1,MJPEG:1080P;
				//H264:720,JPEG:VGA,H264:QVGA@
				if(streamId == 0) return 1280;
				else if(streamId == 1) return 640;
				else if(streamId == 2) return 320;
			} 
		} else if (oSysInfo->lan_config.nVideocombo == 1) {// MPEG4+MPEG4+MJPEG
			if(oSysInfo->lan_config.nVideocodecres == 0) { //H264:1080P,H264:D1,MJPEG:1080P;
				//MPEG4:720,JPEG:VGA,MPEG4:QVGA
				if(streamId == 0) return 1280;
				else if(streamId == 1) return 640;
				else if(streamId == 2) return 320;
			}
		}
	}
	return 1920;
}
int oget_resolutionheight(int streamId)
{
	SysInfo* oSysInfo = GetSysInfo();
	if(oSysInfo->lan_config.nStreamType == 0) { //single
		if(oSysInfo->lan_config.nVideocombo <= 1) {  // H264  //MPEG4
			//H264:720;H264:D1;H264:SXVGA;H264:1080;H264:720MAX60@
			//MPEG4:720;MPEG4:D1;MPEG4:SXVGA;MPEG4:1080;MPEG4:720MAX60@
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				return 720;
			} else if(oSysInfo->lan_config.nVideocodecres == 1) {
				return 480;
			}else if(oSysInfo->lan_config.nVideocodecres == 2) {
				return 1024;
			}else if(oSysInfo->lan_config.nVideocodecres == 3) {
				return 1080;
			}else if(oSysInfo->lan_config.nVideocodecres == 4) {
				return 720;
			}
		} else if(oSysInfo->lan_config.nVideocombo == 2) { //Mjpeg
			//H264:2MP;JPG:2MP;H264:3MP;JPG:3MP;H264:5MP;JPG:5MP@
			if(oSysInfo->lan_config.nVideocodecres <= 1) { //MPEG4:1080P
				return 1200;
			} else if (oSysInfo->lan_config.nVideocodecres <= 3) {
				return 1536;//1536
			} else if (oSysInfo->lan_config.nVideocodecres <= 5) {
				return 1920;//1920
			}
		} 
	}  else if(oSysInfo->lan_config.nStreamType == 1) {
		if(oSysInfo->lan_config.nVideocombo <= 1) { //  H.264+MJPEG; //MPEG4+MJPEG;
			// H264:720,JPEG:VGA; H264:D1,JPEG:D1; H264:720,JPEG:720; H264:1080,JPEG:QVGA; H264:1080,JPEG:D1@
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 720;
				else if(streamId == 1) return 480;
			} else  if(oSysInfo->lan_config.nVideocodecres == 1) {
				if(streamId == 0) return 480;
				else if(streamId == 1) return 480;
			} else  if(oSysInfo->lan_config.nVideocodecres == 2) {
				if(streamId == 0) return 720;
				else if(streamId == 1) return 720;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1080;
				else if(streamId == 1) return 240;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1080;
				else if(streamId == 1) return 480;
			}
		} else if(oSysInfo->lan_config.nVideocombo <= 3) {//H264+H264;
			//H264:720,H264:QVGA;H264:D1,H264:D1;H264:D1,H264:QVGA;H264:1080,H264:QVGA;H264:1080,H264:D1@
			//MPEG4:720,MPEG4:QVGA;MPEG4:D1,MPEG4:D1;MPEG4:D1,MPEG4:QVGA;MPEG4:1080,MPEG4:QVGA;MPEG4:1080,MPEG4:D1@
			if(oSysInfo->lan_config.nVideocodecres == 0) { //MPEG4:1080P
				if(streamId == 0) return 720;
				else if(streamId == 1) return 240;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 1) {
				if(streamId == 0) return 480;
				else if(streamId == 1) return 480;
			} else  if(oSysInfo->lan_config.nVideocodecres == 2) {
				if(streamId == 0) return 480;
				else if(streamId == 1) return 240;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1080;
				else if(streamId == 1) return 240;
			}  else  if(oSysInfo->lan_config.nVideocodecres == 3) {
				if(streamId == 0) return 1080;
				else if(streamId == 1) return 480;
			}
		}else if(oSysInfo->lan_config.nVideocombo == 4) {//H264+MPEG4
			//H264:D1,MPEG4:D1;H264:1080,MPEG4:D1@
			if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 480;
				else if(streamId == 1) return 480;
			} else if(oSysInfo->lan_config.nVideocodecres == 0) {
				if(streamId == 0) return 1080;
				else if(streamId == 1) return 480;
			}
		}
	}  else if(oSysInfo->lan_config.nStreamType == 2) {//triple
		if(oSysInfo->lan_config.nVideocombo == 0) {//H264+H264+MJPEG;
			if(oSysInfo->lan_config.nVideocodecres == 0) { //H264:1080P,H264:D1,MJPEG:1080P;
				//H264:720,JPEG:VGA,H264:QVGA@
				if(streamId == 0) return 720;
				else if(streamId == 1) return 480;
				else if(streamId == 2) return 240;
			}
		} else if (oSysInfo->lan_config.nVideocombo == 1) {// MPEG4+MPEG4+MJPEG
			if(oSysInfo->lan_config.nVideocodecres == 0) { //H264:1080P,H264:D1,MJPEG:1080P;
				//MPEG4:720,JPEG:VGA,MPEG4:QVGA
				if(streamId == 0) return 720;
				else if(streamId == 1) return 480;
				else if(streamId == 2) return 240;
			}
		}
	}


	if(oSysInfo->lan_config.nVideocombo == 0) //--------------- 264 --------------//
	{
		if(oSysInfo->lan_config.nVideocodecres == 0)		// 720
		{
			//return 1280; //
			return 720;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 1) // D1
		{
			//return 720;	//
			return 480;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 2) // H264:SXVGA
		{
			//return 1280; //
			return 960;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 3) // H264:1080
		{
			//return 1920; //
			return 1080;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 4) // H264:720MAX60
		{
			//return 1280; //
			return 720;
		}
	}

	else if (oSysInfo->lan_config.nVideocombo == 1) //--------------- MPEG4 --------------//
	{
		if(oSysInfo->lan_config.nVideocodecres == 0)		// MPEG4:720
		{
			//return 1280;	//
			return 720;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 1) // MPEG4:D1
		{
			//return 720;	//
			return 480;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 2) // MPEG4:SXVGA
		{
			//return 1280;	//
			return 960;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 3) // MPEG4:1080
		{
			//return 1920;	//
			return 1080;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 4) // MPEG4:720MAX60
		{
			//return 1280; //
			return 720;
		}
	}

	else if (oSysInfo->lan_config.nVideocombo == 2) //--------------- MegaPixel --------------// 
	{
		if((oSysInfo->lan_config.nVideocodecres == 0) || (oSysInfo->lan_config.nVideocodecres == 1))	// JPG:1600X1200
		{
			//return 1600;	//
			return 1200;
		}
		else if((oSysInfo->lan_config.nVideocodecres == 2) || (oSysInfo->lan_config.nVideocodecres == 3))// JPG:2048X1536
		{
			//return 2048;	//
			return 1536;
		}
		else if((oSysInfo->lan_config.nVideocodecres == 4) || (oSysInfo->lan_config.nVideocodecres == 5))// JPG:2048X1536
		{
			//return 2592;	//
			return 1920;
		}
	}
	return 0;
}



int oget_dhcpstatus(void)
{
	SysInfo* oSysInfo = GetSysInfo();
	return oSysInfo->lan_config.net.dhcp_enable;
}

char *oget_timezone(int no)
{
	return _TZ_name[no];
}

/***************************************************************************/
/****                                SET                                ****/
/***************************************************************************/
int oset_encoding(int width,int height, int encode)
{
	return 0;
}

int oset_bitrate(int bitrate)
{
	//SysInfo* oSysInfo = GetSysInfo();
	ControlSystemData(SFIELD_SET_MPEG41_BITRATE, (void *)&bitrate, sizeof(bitrate));
	return 0;
}

int oset_framerate(int encoding,int resolution,int framerate)
{
	__u8 _Framerate; 
	SysInfo* oSysInfo = GetSysInfo();
	if((encoding==0) || (encoding==1)) // H264 || MPEG4
	{
		if((resolution==0) || (resolution==1) || (resolution==2)) 
		{
			if(framerate>=30)  _Framerate = 0;
			else if(framerate>=24) _Framerate = 1;
			else if(framerate>=15) _Framerate = 2;
			else _Framerate = 3;//if(framerate==8)
		}
		else if(resolution==3)
		{
			if(framerate>=23) _Framerate = 0;
			else if(framerate>=15) _Framerate = 1;
			else _Framerate = 2; //if(framerate==8) 
		}
		else _Framerate = 0;
	}
	else if(encoding==2) // MegaPixel
	{
		if((resolution==0) || (resolution==2) || (resolution==4)) 
		{
			if(framerate>=30)  _Framerate = 0;
			else if(framerate>=24) _Framerate = 1;
			else if(framerate>=15) _Framerate = 2;
			else _Framerate = 3;//if(framerate==8) 
		}
		else if((resolution==1) || (resolution==3)) 
		{
			if(framerate>=15)  _Framerate = 0;
			else _Framerate = 1; //if(framerate==8) 
		}
		else if(resolution==5) _Framerate = 0;
	}
	if(oSysInfo->lan_config.nFrameRate1 != _Framerate)
		ControlSystemData(SFIELD_SET_FRAMERATE1, (void *)&_Framerate, sizeof(_Framerate));
	return 0;
}

int oset_ratecontrol(int ratecontrol)
{
	//SysInfo* oSysInfo = GetSysInfo();
	return 0;
}

__u8 oset_resolution(__u8 encoding,int width, int height)
{
	//SysInfo* oSysInfo = GetSysInfo();
	//onvif {JPEG = 0, MPEG4 = 1, H264 = 2};
	//ipnc  {H264 = 0, MPEG4 = 1, MegaPixel = 2};		
	// Onvif -> IPNC
	if(encoding==0) encoding = 2; // jpeg  -> Megapixel
	else if(encoding==1) encoding = 1; // MPEG4 -> MPEG4
	else if(encoding==2) encoding = 0; // H264  -> H264

	if(encoding == 0) // H264
	{
	}
	else if(encoding==1) // MPEG4
	{
	}
	else if(encoding==2) // MegaPixel
	{
	}
	return 0;
}

int oset_dhcpstatus(int status)
{
	tempdhcp_flag = status;
	return 1;
}

int oset_timezone(char *TZ)
{
	if (strncmp(TZ, "IDLW", 4)==0)return 0;//-12
	else if (strncmp(TZ, "NT", 2)==0)return 1;//-11
	else if ((strncmp(TZ, "AHST", 4)==0)||(strncmp(TZ, "CAT", 3)==0)||(strncmp(TZ, "HST", 3)==0)||(strncmp(TZ, "HDT", 3)==0))return 2;//-10
	else if ((strncmp(TZ, "YST", 3)==0)||(strncmp(TZ, "YDT", 3)==0))return 3;//-9
	else if ((strncmp(TZ, "PST", 3)==0)||(strncmp(TZ, "PDT", 3)==0))return 4;//-8
	else if ((strncmp(TZ, "MST", 3)==0)||(strncmp(TZ, "MDT", 3)==0))return 5;//-7
	else if ((strncmp(TZ, "CST", 3)==0)||(strncmp(TZ, "CDT", 3)==0))return 6;//-6
	else if ((strncmp(TZ, "EST", 3)==0)||(strncmp(TZ, "EDT", 3)==0))return 7;//-5
	else if ((strncmp(TZ, "AST", 3)==0)||(strncmp(TZ, "ADT", 3)==0))return 8;//-4
	else if (strncmp(TZ, "GMT-03", 6)==0)return 9;//-3
	else if (strncmp(TZ, "AT", 2)==0)return 10;//-2
	else if (strncmp(TZ, "WAT", 3)==0)return 11;//-1
	else if ((strncmp(TZ, "GMT", 3)==0)||(strncmp(TZ, "UT", 2)==0)||(strncmp(TZ, "UTC", 3)==0)||(strncmp(TZ, "BST", 3)==0))return 12;//-0
	else if ((strncmp(TZ, "CET", 3)==0)||(strncmp(TZ, "FWT", 3)==0)||(strncmp(TZ, "MET", 3)==0)\
			||(strncmp(TZ, "MEWT", 4)==0)||(strncmp(TZ, "SWT", 3)==0)||(strncmp(TZ, "MEST", 4)==0)\
			||(strncmp(TZ, "MESZ", 4)==0)||(strncmp(TZ, "SST", 3)==0)||(strncmp(TZ, "FST", 3)==0))return 13;//1
	else if (strncmp(TZ, "EET", 3)==0)return 14;//2
	else if (strncmp(TZ, "BT", 2)==0)return 15;//3
	else if (strncmp(TZ, "ZP4", 3)==0)return 16;//4
	else if (strncmp(TZ, "ZP5", 3)==0)return 17;//5
	else if (strncmp(TZ, "ZP6", 3)==0)return 18;//6
	else if (strncmp(TZ, "ZP7", 3)==0)return 19;//7
	else if (strncmp(TZ, "WAST", 4)==0)return 20;//8
	else if (strncmp(TZ, "JST", 3)==0)return 21;//9
	else if (strncmp(TZ, "ACT", 3)==0)return 22;//10
	else if (strncmp(TZ, "EAST", 4)==0)return 23;//11
	else if (strncmp(TZ, "IDLE", 4)==0)return 24;//12
	else return 100;//ERROR


}

int onvif_fault(struct soap *soap,int flag, char *value1,char *value2, char *reason)
{
	soap->fault = (struct SOAP_ENV__Fault*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Fault)));
	soap->fault->SOAP_ENV__Code = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));
	if(flag == 1)
		soap->fault->SOAP_ENV__Code->SOAP_ENV__Value = "SOAP-ENV:Sender";
	else
		soap->fault->SOAP_ENV__Code->SOAP_ENV__Value = "SOAP-ENV:Receiver";

	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Value = value1;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode->SOAP_ENV__Value = value2;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode->SOAP_ENV__Subcode = NULL;
	soap->fault->faultcode = NULL;
	soap->fault->faultstring = NULL;
	soap->fault->faultactor = NULL;
	soap->fault->detail = NULL;
	soap->fault->SOAP_ENV__Reason = (struct SOAP_ENV__Reason*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Reason)));
	soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text = reason;
	soap->fault->SOAP_ENV__Node = NULL;
	soap->fault->SOAP_ENV__Role = NULL;
	soap->fault->SOAP_ENV__Detail = NULL;
}
int onvif_fault_probe(struct soap *soap, char *value1, char *reason)
{
	soap->fault = (struct SOAP_ENV__Fault*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Fault)));
	soap->fault->SOAP_ENV__Code = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Value = "SOAP-ENV:Sender";

	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Value = value1;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode = NULL;
	soap->fault->faultcode = NULL;
	soap->fault->faultstring = NULL;
	soap->fault->faultactor = NULL;
	soap->fault->detail = NULL;
	soap->fault->SOAP_ENV__Reason = (struct SOAP_ENV__Reason*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Reason)));
	soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text = reason;
	soap->fault->SOAP_ENV__Node = NULL;
	soap->fault->SOAP_ENV__Role = NULL;
	soap->fault->SOAP_ENV__Detail = NULL;
}


int checkhostname(char *hostname)
{
	while(*hostname != '\0')
	{
		if(*hostname=='_')return 1;
		hostname++;
	}
	return 0; //No error
}


