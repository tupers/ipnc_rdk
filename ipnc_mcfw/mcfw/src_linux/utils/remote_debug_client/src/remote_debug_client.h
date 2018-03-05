/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _REMOTE_DEBUG_CLIENT_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _REMOTE_DEBUG_CLIENT_

#include <remote_debug_if.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <remote_debug_thr.h>
#include <remote_debug_queue.h>

#define STATIC static 

#define REMOTE_DEBUG_CORE_PREFIX_M3VPSS    "[m3vpss ]"
#define REMOTE_DEBUG_CORE_PREFIX_M3VIDEO   "[m3video]"
#define REMOTE_DEBUG_CORE_PREFIX_C6XDSP    "[c6xdsp ]"
#define REMOTE_DEBUG_CORE_PREFIX_HOST	   "[hostA8 ]"

#define REMOTE_DEBUG_LINE_BUF_SIZE         (1024)
#define REMOTE_DEBUG_MAXMSG			20	

typedef struct{
	int sd;
	struct sockaddr_in remote_addr;
}Socket_ClientObj;

#define DEBUG_NONE		0
#define DEBUG_SERIAL	(1<<0)
#define DEBUG_SCOKET	(1<<2)

typedef struct
{
	unsigned int size;
	char *data;
}RemoteDebugSt;

typedef struct {

  unsigned int coreObjPhysBaseAddr;
  unsigned char *coreObjVirtBaseAddr;
  unsigned int coreObjTotalMemSize;
  unsigned int memDevFd;
    
  volatile RemoteDebug_CoreObj *pCoreObj[REMOTE_DEBUG_CORE_ID_MAX];
  
  char lineBuf[REMOTE_DEBUG_LINE_BUF_SIZE];

  Socket_ClientObj sClient;
  RemoteDebug_ThrHndl hndl;
  OSA_QueHndl		  freeQuehdl;
  OSA_QueHndl		  fullQuehdl;
  RemoteDebugSt		  remoteBuf[REMOTE_DEBUG_MAXMSG];
  int qid;
  int mid;
  int sd;
  unsigned int DebugStatus;
} RemoteDebug_ClientObj;

STATIC int xstrtoi(char *hex);
STATIC void OSA_waitMsecs(unsigned int msecs);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _REMOTE_DEBUG_CLIENT_ */

