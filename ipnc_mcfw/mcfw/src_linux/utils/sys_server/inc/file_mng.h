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
* @file file_mng.h
* @brief File manager.
*/
#ifndef __FILE_MNG_H__
#define __FILE_MNG_H__

#include <sys_env_type.h>
int FileMngInit(void *ShareMem);
int FileMngExit();
int ReadGlobal(void *Buffer);
int ReadAlgParam(Alg_Params *Buffer);
int ReadGlobalDmvaTZ(void *Buffer,int setNo);
int ReadGlobalDmvaIMD(void *Buffer,int setNo);
int ReadGlobalDmvaOC(void *Buffer,int setNo);
int ReadGlobalDmvaSM(void *Buffer,int setNo);
int WriteAlgParam(Alg_Params *Buffer);
int WriteGlobal(void *Buffer);
int WriteGlobalLog(void *Buffer);
int WriteGlobalDmvaLog(void *Buffer);
int WriteGlobalSys(void *Buffer);
int WriteGlobalDmvaTZ(void *Buffer,int setNo);
int WriteGlobalDmvaIMD(void *Buffer,int setNo);
int WriteGlobalDmvaOC(void *Buffer,int setNo);
int WriteGlobalDmvaSM(void *Buffer,int setNo);
int UpdateNetwork(void *Buffer);
int FileMngReset(void *ShareMem);
LogEntry_t* GetLog(int nPageNum, int nItemIndex);
DmvaLogEntry_t* GetDmvaLog(SysInfo *pSysInfo,int nPageNum, int nItemIndex);
int IsFileThreadQuit();
int AddLog(LogEntry_t *pLog);
int AddDmvaLog(DmvaLogEntry_t *pDmvaLog);

/* DMVA event delete fns */
int DmvaEventDelete(int eventIndex);
int DmvaEventDeleteStartEnd(int startIndex,int endIndex);
int DmvaEventDeleteAll();

/* DMVA event search fn */
int SearchStartEndDmvaEvents(SysInfo *pSysInfo);

unsigned int GetDmvaEventYear(char *timeStampString);
unsigned int GetDmvaEventTime(char *timeStampString);
unsigned int GetDmvaEventTimeStamp(unsigned int year,
                                   unsigned int month,
                                   unsigned int day,
                                   unsigned int time,
                                   int *pTimeFlag);
unsigned int GetDmvaEventMonth(char *timeStampString);
unsigned int GetDmvaEventDay(char *timeStampString);
int WriteGlobalDmvaSMETA(void *Buffer,int setNo);           
int ReadGlobalDmvaSMETA(void *Buffer,int setNo);
                        
/* clear system and access logs */
int ClearSystemLog();
int ClearAccessLog();
#endif   /* __FILE_MNG_H__ */
