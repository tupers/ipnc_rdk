/* ===========================================================================
* @path $(IPNCPATH)\sys_adm\system_server
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file system_control.h
* @brief Commands to system server
*/
#ifndef __SYSTEM_CONTORL_H__
#define __SYSTEM_CONTORL_H__

#include <netinet/in.h>
#include <time.h>
#include <sys_env_type.h>

int SysGetSysInfo(EzSysInfo* pPrm);
int SysSetSysInfo(EzSysInfo *pPrm);
int SysUpdateSysFile(EzSysInfo *pPrm);
int InitNetWork();
void RestartSystem();
int IsRestart();
void SetReloadFlag(unsigned char value);
void SetRestart();
void CleanRestart();
int IsResetDefault();
void SetDefault();
void CleanSetDefault();
int IsUsbInserted();
int IsUsbConnect();

int SystemInit(void);
void SystemCleanup(void);
int WriteConfigFile(void);
int* GetDhcpEn();
unsigned short *GetHttpPort();
char *GetFqdn();
int RestartNetwork();
int SetNetwork(Network_Config *net);
int SetDaylightTime(unsigned char value);
char *GetTitle();
unsigned char *GetMac();
char* GetFtpServerIp();
char *GetFtpUsername();
char *GetFtpPassword();
char *GetFtpFoldername();
int *GetFtpImageAmount();
int *GetFtpPid();
unsigned short *GetFtpPort();
char* GetSmtpServerIp();
unsigned short *GetSmtpServerPort();
unsigned char *GetSmtpAuthentication();
char *GetsmtpUsername();
char *GetSmtpPassword();
char *GetSmtpSenderEmail();
char *GetSmtpReceiverEmail();
char *GetSmtpCC();
char *GetSmtpSubject();
char *GetSmtpText();
unsigned char *GetSmtpAttachments();
unsigned char *GetSmtpView();
int GetDhcpCinfig();
#if 0
int SetIp(in_addr_t ip);
int SetGateway(in_addr_t gateway);
int SetNetMask(in_addr_t net_mask);
in_addr_t* GetIp();
in_addr_t* GetGateWay();
in_addr_t* GetNetmask();
in_addr_t* GetDns();
#else
int SetIp(char * ip);
int SetGateway(char* gateway);
int SetNetMask(char* net_mask);
int SetDns(char* ip);
char* GetIp();
char* GetGateWay();
char* GetNetmask();
char* GetDns();
#endif
int SetHttpPort(unsigned short port);
int SetTitle(void * buf, int length);
int SetDhcpEnable(int enable);
int SetFtpFqdn(void * buf, int length);
int SetFtpUsername(void * buf, int length);
int SetFtpPassword(void * buf, int length);
int SetFtpFoldname(void * buf, int length);
int SetFtpImageacount(int imageacount);
int SetFtpPid(int pid);
int SetFtpPort(unsigned short port);
int SetSmtpServerIp(void *buf, int length);
int SetSmtpServerPort(unsigned short port);
int SetSmtpAuthentication(unsigned char authentication);
int SetSmtpUsername(void * buf, int length);
int SetSmtpPassword(void * buf, int length);
int SetSmtpSenderEmail(void * buf, int length);
int SetSmtpReceiverEmail(void * buf, int length);
int SetSmtpCC(void * buf, int length);
int SetSmtpSubject(void * buf, int length);
int SetSmtpText(void * buf, int length);
int SetSmtpAttachments(unsigned char attachments);
int SetSmtpView(unsigned char view);
int SetSntpServer(void * buf, int length);
void SendOSDChangeMsg(void);
int SetSchedule(void * buf, int length);
int DelSchedule(int bEnable);
int DoLogin(login_data_t* pPrm);
int EzDoLogin(Acount_t* pPrm);
int GetUserAuthority(char* user);
int GetUser(get_userdata_t* pPrm);
int AddUser(Acount_t *acout);
int DelUser(char *strUserName);
int SetLostAlarm(unsigned char value);
int SetSDAlarmEnable(unsigned char value);
int SetFTPAlarmEnable(unsigned char value);
int SetSMTPAlarmEnable(unsigned char value);
int SetAlarmDuration(unsigned char value);
int SetVideoCodecMode(unsigned char value);
int SetVideoCodecRes(unsigned char value);
int SetBlc(unsigned char value);
int SetMotionDetect(unsigned char value);
int SetMotionCEnable(unsigned char value);
int SetMotionLevel(unsigned char value);
int SetMotionCValue(unsigned char value);
int SetMotionDetectBlock(char *pValue,int len);
int SetAVIDuration(unsigned char value);
int SetAVIFormat(unsigned char value);
int SetFTPFileFormat(unsigned char value);
int SetSDFileFormat(unsigned char value);
int SetAttFileFormat(unsigned char value);
int SetAudioEnable(unsigned char value);
int SetASmtpAttach(unsigned char value);
int SetRftpenable(unsigned char value);
int SetSdReEnable(unsigned char value);
int SetImageParaDefault(unsigned char value);
int SetGIOInEnable(unsigned char value);
int SetGIOInType(unsigned char value);
int SetGIOOutEnable(unsigned char value);
int SetGIOOutType(unsigned char value);
int SetAudioInVolume(unsigned char value);
int SetAudioOutVolume(unsigned char value);
int SetSystemDefault(unsigned char value);
int SetNetMulticast(unsigned char value);
void ScheduleStop(void);
void SetRestart(void);
void SetDefault(void);
int SetSysDefaultDateTime(void);
int DoBootProc(int value);
int CheckCpuSpeed(void);
int SetVideoMode(unsigned char value);
int SetHttpsPort(unsigned char value);
int SetRS485Port(unsigned char value);
int SetVideoCodecCombo(unsigned char value);
int SetMotionDetCEnable(unsigned char value);
int SetMotionDetLevel(unsigned char value);
int SetMotionDetCValue(unsigned char value);
int SetVideoCodecType(unsigned char mode, unsigned char res);
void SetMotionDetectParam(void);
int SetAlarmEnable(unsigned char value);
int Avs_SetExtAlarm(unsigned char value);
int Avs_SetAudioAlarm(unsigned char value);
int Avs_SetAlarmAudioPlay(unsigned char value);
int Avs_SetAlarmAudioFile(unsigned char value);
int Avs_SetScheduleRepeatEnable(unsigned char value);
int Avs_SetScheduleNumWeeks(unsigned char value);
int Avs_SetScheduleInfiniteEnable(unsigned char value);
int SetAlarmStorage(unsigned char value);
int SetRecordStorage(unsigned char value);
int CheckValueBound(int value, int min, int max);
int Check_device_version(void);
void SetVideoCodecCfg(int flag);
int SetAutoIrisVol(unsigned char val);


/********* add by wuzhongbing 2016-10-17 ***********/
int SetImagingSettings(Imaging_Conf_tmp *imaging);
int set_hostname(hostname_tmp *host);
int AddMediaProfile(media_profile *profile);
int Addvideosourceconfiguration(video_source_conf *videosourceconf);
int Setvideosourceconfiguration(video_source_conf *setvideosourceconf);
int Addmetadataconfiguration(Metadata_configuration_tmp *metadata);
int Delmetadataconfiguration(Metadata_configuration_tmp *metadata);
int addaudiosourceconfiguration(Add_AudioSource_Configuration_tmp *addaudiosource);
int SetMetadataConf(Metadata_configuration_tmp *metadata);
int Setaudiosourceconfiguration(Add_AudioSource_Configuration_tmp *setaudiosource);
int addaudioencoderconfiguration(Add_AudioEncoder_Configuration_tmp *addaudioencoder);
int Setaudioencoderconfiguration(Add_AudioEncoder_Configuration_tmp *setaudioencoder);
int Addvideoencoderconfiguration(video_encoder_conf *videoencoderconf);
int Setvideoencoderconfiguration(video_encoder_conf *setvideoencoderconf);
int Delvideoencoderconfiguration(video_encoder_conf *setvideoencoderconf);
int DeleteProfile(int index);
int RemoveAudioSource(int index);
int RemoveAudioEncoder(int index);
int SetDiscovery(unsigned char mode);
int AddScope(Onvif_Scopes_t onvif_scopes_tmp);
int DelScope(Onvif_Scopes_t onvif_scopes_tmp);
int SetScope(Onvif_Scopes_t onvif_scopes_tmp);
int SetAnalytics(video_analytics_conf video_analytics_tmp);
int SetVideoOutput(video_output_conf video_output_tmp);
int SetRelay(relay_conf relay_tmp);
int SetRelaylogicalstate(relay_conf relay_tmp);
#endif
