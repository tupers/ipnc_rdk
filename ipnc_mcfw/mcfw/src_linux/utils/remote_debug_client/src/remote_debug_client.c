/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <remote_debug_client.h>
#include <remote_debug_socket.h>
#include <remote_debug_msg.h>
#include <remote_debug_shmem.h>
#include <Msg_Def.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


STATIC char *gRemoteDebug_corePrefix[] =
{
  REMOTE_DEBUG_CORE_PREFIX_C6XDSP,
  REMOTE_DEBUG_CORE_PREFIX_M3VIDEO,
  REMOTE_DEBUG_CORE_PREFIX_M3VPSS,
  REMOTE_DEBUG_CORE_PREFIX_HOST
};

STATIC RemoteDebug_ClientObj gRemoteDebug_clientObj;

STATIC int RemoteDebug_mapMem()
{
  gRemoteDebug_clientObj.memDevFd = open("/dev/mem",O_RDWR|O_SYNC);
  if(gRemoteDebug_clientObj.memDevFd < 0)
  {
    printf(" ERROR: /dev/mem open failed !!!\n");
    return -1;
  }

	gRemoteDebug_clientObj.coreObjVirtBaseAddr = mmap(
	        (void	*)gRemoteDebug_clientObj.coreObjPhysBaseAddr,
	        gRemoteDebug_clientObj.coreObjTotalMemSize,
					PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,
					gRemoteDebug_clientObj.memDevFd,
					gRemoteDebug_clientObj.coreObjPhysBaseAddr
					);

	if (gRemoteDebug_clientObj.coreObjVirtBaseAddr==NULL)
	{
		printf(" ERROR: mmap() failed !!!\n");
		return -1;
	}

  return 0;
}

STATIC int RemoteDebug_unmapMem()
{
  if(gRemoteDebug_clientObj.coreObjVirtBaseAddr)
    munmap((void*)gRemoteDebug_clientObj.coreObjVirtBaseAddr, gRemoteDebug_clientObj.coreObjTotalMemSize);

  if(gRemoteDebug_clientObj.memDevFd >= 0)
    close(gRemoteDebug_clientObj.memDevFd);

  return 0;
}

STATIC int RemoteDebug_clientInit(unsigned int coreObjPhysBaseAddr, int resetShm)
{
  unsigned int coreId;
  int status;
  volatile RemoteDebug_CoreObj *pCoreObj;

  memset(&gRemoteDebug_clientObj, 0, sizeof(gRemoteDebug_clientObj));

  gRemoteDebug_clientObj.coreObjPhysBaseAddr = coreObjPhysBaseAddr;
  gRemoteDebug_clientObj.coreObjTotalMemSize = sizeof(RemoteDebug_CoreObj)*REMOTE_DEBUG_CORE_ID_MAX;

  status = RemoteDebug_mapMem();
  if(status!=0)
    return -1;

  for(coreId=0; coreId<REMOTE_DEBUG_CORE_ID_MAX; coreId++)
  {
    gRemoteDebug_clientObj.pCoreObj[coreId] = (RemoteDebug_CoreObj*)(gRemoteDebug_clientObj.coreObjVirtBaseAddr + sizeof(RemoteDebug_CoreObj)*coreId);

    if(resetShm)
    {
      pCoreObj = gRemoteDebug_clientObj.pCoreObj[coreId];

      pCoreObj->serverIdx = 0;
      pCoreObj->clientIdx = 0;
      pCoreObj->serverFlags[0] = pCoreObj->serverFlags[1] = 0;
      pCoreObj->clientFlags[0] = pCoreObj->clientFlags[1] = 0;
      pCoreObj->reserved = 0;
      pCoreObj->headerTag = REMOTE_DEBUG_HEADER_TAG;
    }

    printf(" %s Remote Debug Shared Memory @ 0x%08x\n", gRemoteDebug_corePrefix[coreId], (unsigned int)gRemoteDebug_clientObj.coreObjPhysBaseAddr+sizeof(RemoteDebug_CoreObj)*coreId);
  }

  return 0;
}

STATIC int RemoteDebug_clientDeInit()
{
  RemoteDebug_unmapMem();

  return 0;
}

STATIC int RemoteDebug_putChar(unsigned int coreObjPhysBaseAddr, unsigned int coreId, char ch)
{
    volatile RemoteDebug_CoreObj *pCoreObj;
    int status;

    status = RemoteDebug_clientInit(coreObjPhysBaseAddr, 0);
    if(status!=0)
      return status;

    if(coreId>=REMOTE_DEBUG_CORE_ID_MAX)
        return -1;

    pCoreObj = gRemoteDebug_clientObj.pCoreObj[coreId];

    if(pCoreObj->headerTag == REMOTE_DEBUG_HEADER_TAG)
    {
      pCoreObj->serverFlags[0] = (REMOTE_DEBUG_FLAG_TYPE_CHAR | ch);
    }

    RemoteDebug_clientDeInit();

    return 0;
}

STATIC int RemoteDebug_putStr(unsigned int coreObjPhysBaseAddr, unsigned int coreId, char *str)
{
    volatile RemoteDebug_CoreObj *pCoreObj;
    int status,length;

    status = RemoteDebug_clientInit(coreObjPhysBaseAddr, 0);
    if(status!=0)
        return status;

    if(coreId>=REMOTE_DEBUG_CORE_ID_MAX)
        return -1;

    pCoreObj = gRemoteDebug_clientObj.pCoreObj[coreId];

    length = strlen(str);
    if(pCoreObj->headerTag == REMOTE_DEBUG_HEADER_TAG)
    {
        if(length>0 && length<REMOTE_DEBUG_PARAM_BUF_SIZE)
        {
            strcpy((char*)pCoreObj->serverParamBuf , str );
            pCoreObj->serverFlags[1] = length;
            pCoreObj->serverFlags[0] = (REMOTE_DEBUG_FLAG_TYPE_STRING);
        }
    }
    RemoteDebug_clientDeInit();

    return 0;
}


STATIC int RemoteDebug_getChar(unsigned int coreObjPhysBaseAddr, unsigned int coreId, char expectedChar, unsigned int timeout)
{
    volatile RemoteDebug_CoreObj *pCoreObj;
    volatile int value;
    char receivedChar;

    int status;

    status = RemoteDebug_clientInit(coreObjPhysBaseAddr, 0);
    if(status!=0)
      return status;

    if(coreId>=REMOTE_DEBUG_CORE_ID_MAX)
        return -1;

    pCoreObj = gRemoteDebug_clientObj.pCoreObj[coreId];

    while(1)
    {
        if(pCoreObj->headerTag != REMOTE_DEBUG_HEADER_TAG)
            return -1;

        value = pCoreObj->clientFlags[0];
        if(value & REMOTE_DEBUG_FLAG_TYPE_CHAR)
        {
            receivedChar = value & 0xFF;
            pCoreObj->clientFlags[0] = 0;
            if(receivedChar==expectedChar)
            {
              printf(" %s Received characteOSA_Qr '%c'\n", gRemoteDebug_corePrefix[coreId], receivedChar );
              break;
            }
        }
        if(timeout==(unsigned int)-1)
            OSA_waitMsecs(10);
        else
        	return -1;
    }

    RemoteDebug_clientDeInit();

    return 0;
}

STATIC int RemoteDebug_clientGetLine(unsigned int coreId, char * pString)
{
    unsigned int numBytes, copyBytes=0, serverIdx, clientIdx;
    volatile unsigned char *pSrc;
    volatile unsigned char curChar;

    volatile RemoteDebug_CoreObj *pCoreObj = gRemoteDebug_clientObj.pCoreObj[coreId];

    if(pCoreObj->headerTag != REMOTE_DEBUG_HEADER_TAG)
        return -1;

    serverIdx = pCoreObj->serverIdx;
    clientIdx = pCoreObj->clientIdx;

    if(clientIdx>serverIdx)
        numBytes = (REMOTE_DEBUG_LOG_BUF_SIZE - clientIdx) + serverIdx;
    else
        numBytes = serverIdx - clientIdx;

    if(numBytes>0)
    {
        pSrc = &pCoreObj->serverLogBuf[0];

        for(copyBytes=0; copyBytes<numBytes; copyBytes++)
        {
          if(clientIdx>=REMOTE_DEBUG_LOG_BUF_SIZE)
            clientIdx = 0;

          curChar = pSrc[clientIdx];

          clientIdx++;

          if(curChar==0xA0||curChar=='\r'||curChar=='\n'||curChar==0||copyBytes>=REMOTE_DEBUG_LINE_BUF_SIZE)
            break;
          else
            *pString++ = curChar;
        }

        pCoreObj->clientIdx = clientIdx;
    }

    *pString = 0;

    return copyBytes;
}
static unsigned char gRemoteDebugQuit = 0;
#define PROC_MEM_SIZE 4096*3
#define MAX_SHARE_PROC	9
static void *RemoteMsgThread(void *arg)
{
	unsigned char tQuit = 0;
	int msg_size;
	int status;
	RemoteDebugSt *pFreebuf;
	
	REMOTEDEBUG_SOCKMSG_BUF *msgbuf = (REMOTEDEBUG_SOCKMSG_BUF *)malloc(sizeof(REMOTEDEBUG_SOCKMSG_BUF));
	if(msgbuf == NULL)
	{
		printf("%s in %d:malloc fail\n",__func__,__LINE__);
		return NULL;
	}

	gRemoteDebug_clientObj.sd = RemoteDebug_create_server_socket_udp(REMOTEDEBUG_PORT);
	if(gRemoteDebug_clientObj.sd < 0)
	{
		printf("%s in %d:RemoteDebug_create_server_socket_udp fail\n",__func__,__LINE__);

		return NULL;
	}
		
	while(tQuit == 0)
	{
		msg_size = Remote_debug_recv_data(gRemoteDebug_clientObj.sd,(char *)msgbuf,sizeof(REMOTEDEBUG_SOCKMSG_BUF));
		if( msg_size < 0 )
		{
			perror("Remote_debug_recv_data");
			tQuit = 1;
		} 
		else 
		{
			switch(msgbuf->cmd)
			{
				case REMOTEDEBUG_MSG_QUIT:
				{
					tQuit = 1;
					gRemoteDebugQuit = 1;
					break;
				}	
				case REMOTEDEBUG_MSG_NONE:
				{
					gRemoteDebug_clientObj.DebugStatus = DEBUG_NONE;
					break;
				}	
				case REMOTEDEBUG_MSG_SERIAL:
				{
					gRemoteDebug_clientObj.DebugStatus = DEBUG_SERIAL;
					break;
				}
				case REMOTEDEBUG_MSG_SOCKET:
				{
					REMOTEDEBUG_SOCKINFO_BUF *sockinfo = (REMOTEDEBUG_SOCKINFO_BUF *)(msgbuf->data);
					if(sockinfo->port<0) 
					{
						printf("error param\n");
						break;
					}

					gRemoteDebug_clientObj.sClient.remote_addr.sin_family = AF_INET;
					gRemoteDebug_clientObj.sClient.remote_addr.sin_port = htons(sockinfo->port);
					inet_aton(sockinfo->ip, &(gRemoteDebug_clientObj.sClient.remote_addr.sin_addr));
					bzero(&(gRemoteDebug_clientObj.sClient.remote_addr.sin_zero), 8);
	
					gRemoteDebug_clientObj.DebugStatus = DEBUG_SCOKET;
					break;
				}				
				case REMOTEDEBUG_MSG_DEBUGINFO:
				{
					if((msgbuf->dataBytes > REMOTE_DEBUG_BUF_SIZE) || (msgbuf->dataBytes) > msg_size)
						break;
						
					status = OSA_queGet(&(gRemoteDebug_clientObj.freeQuehdl), (Int32 *)&pFreebuf,OSA_TIMEOUT_NONE);                  
    				if (status != OSA_SOK)
						break;	

					pFreebuf->size = msgbuf->dataBytes;

					memcpy(pFreebuf->data,msgbuf->data,msgbuf->dataBytes);

					status = OSA_quePut(&(gRemoteDebug_clientObj.fullQuehdl),(Int32)pFreebuf, OSA_TIMEOUT_NONE);
					if(status != OSA_SOK)
					{
						OSA_quePut(&(gRemoteDebug_clientObj.freeQuehdl),(Int32)pFreebuf, OSA_TIMEOUT_NONE);
						break;
					}	
									
					break;
				}								
				default:
				{
					break;
				}
			}
		}		
	}

	free(msgbuf);
	close(gRemoteDebug_clientObj.sd);
	return NULL;
}

static char *gRemoteBuf = NULL;
static int RemoteDebug_socketCleintInit()
{
	int ret;
	int status;
	int idx;
	
	gRemoteDebug_clientObj.DebugStatus = DEBUG_SERIAL;
	
	gRemoteDebug_clientObj.sClient.sd = RemoteDebug_create_server_socket_udp(0);
	if(gRemoteDebug_clientObj.sClient.sd < 0)
	{
		printf("%s in %d:RemoteDebug_create_server_socket_udp fail\n",__func__,__LINE__);

		return -1;
	}
      
	ret = RemoteDebug_thrCreate(&(gRemoteDebug_clientObj.hndl), RemoteMsgThread, REMOTEDEBUG_THR_PRI_DEFAULT, REMOTEDEBUG_THR_STACK_SIZE_DEFAULT, NULL);
	if(ret<0)
	{
		printf("%s in %d:RemoteDebug_thrCreate fail\n",__func__,__LINE__);

		return -1;		
	}
	status = OSA_queCreate(&gRemoteDebug_clientObj.freeQuehdl,REMOTE_DEBUG_MAXMSG);
    if(status != OSA_SOK)
    {
    	printf("%s in %d:OSA_queCreate fail\n",__func__,__LINE__);
		return -1;
    }
	
  	status = OSA_queCreate(&gRemoteDebug_clientObj.fullQuehdl,REMOTE_DEBUG_MAXMSG);
    if(status != OSA_SOK)
    {
    	printf("%s in %d:OSA_queCreate fail\n",__func__,__LINE__);
		return -1;
    }

	gRemoteBuf = (char *)malloc(REMOTE_DEBUG_LINE_BUF_SIZE*REMOTE_DEBUG_MAXMSG);
	if(gRemoteBuf == NULL)
	{
		printf("%s in %d:malloc fail\n",__func__,__LINE__);
		return -1;
	}
 	
    for(idx=0;idx<REMOTE_DEBUG_MAXMSG;idx++)
    {
		gRemoteDebug_clientObj.remoteBuf[idx].size = 0;
		gRemoteDebug_clientObj.remoteBuf[idx].data = gRemoteBuf + idx * REMOTE_DEBUG_LINE_BUF_SIZE;

		status = OSA_quePut(&gRemoteDebug_clientObj.freeQuehdl,(Int32)(&(gRemoteDebug_clientObj.remoteBuf[idx])), OSA_TIMEOUT_NONE);
		if(status != OSA_SOK)
		{
			printf("%s in %d:OSA_quePut fail\n",__func__,__LINE__);
			return -1;
		}
    }
	return 0;
}

static void RemoteDebug_socketCleintDeInit()
{
	if(gRemoteDebug_clientObj.sClient.sd>0)
		close(gRemoteDebug_clientObj.sClient.sd);
		
	RemoteDebug_thrDelete(&(gRemoteDebug_clientObj.hndl));

	OSA_queDelete(&gRemoteDebug_clientObj.fullQuehdl);
		
	OSA_queDelete(&gRemoteDebug_clientObj.freeQuehdl);
		
	if(gRemoteBuf)
		free(gRemoteBuf);
}

static char sSockString[REMOTE_DEBUG_LOG_BUF_SIZE*2] = {0};

int RemoteDebug_clientRun(unsigned int coreObjPhysBaseAddr)
{
  unsigned int coreId;
  int numBytes, status,ret;
  unsigned int doWait;
  char *tmp;
  RemoteDebugSt *pFullBuf = NULL;
  gRemoteDebugQuit = 0;
 
  status = RemoteDebug_clientInit(coreObjPhysBaseAddr, 1);
  if(status!=0)
    return status;

  status = RemoteDebug_socketCleintInit();
  if(status!=0)
    return status;
      
  while(gRemoteDebugQuit == 0)
  {
    doWait = 1;

    for(coreId=0; coreId<REMOTE_DEBUG_CORE_ID_MAX; coreId++)
    {            	
        do {   
        	if(coreId == REMOTE_DEBUG_CORE_ID_HOST)
       		{
       			pFullBuf = NULL;
       			numBytes = 0;
       			
				status = OSA_queGet(&(gRemoteDebug_clientObj.fullQuehdl), (Int32 *)&pFullBuf,OSA_TIMEOUT_NONE);                  
    			if (status != OSA_SOK)
					break;
					
				numBytes = pFullBuf->size;
				tmp = pFullBuf->data;							
        	}
        	else
        	{
				numBytes = RemoteDebug_clientGetLine( coreId, gRemoteDebug_clientObj.lineBuf );
            	tmp = gRemoteDebug_clientObj.lineBuf;		
        	}       

            
            if(numBytes>0)
            {
            	if(gRemoteDebug_clientObj.DebugStatus == DEBUG_SCOKET)
            	{
            		if((coreId == REMOTE_DEBUG_CORE_ID_M3VIDEO) || (coreId == REMOTE_DEBUG_CORE_ID_M3VPSS))
            			continue;
            			
            		sprintf(sSockString,"%s %s\r\n",gRemoteDebug_corePrefix[coreId],tmp);
            		
            		ret = sendto(gRemoteDebug_clientObj.sClient.sd, sSockString, strlen(sSockString), 0,(const struct sockaddr *)&(gRemoteDebug_clientObj.sClient.remote_addr), sizeof(gRemoteDebug_clientObj.sClient.remote_addr));
            		if(ret <0)
            		{
						printf("socket sendto fail\n");
						
            			gRemoteDebug_clientObj.DebugStatus = DEBUG_SERIAL;
					}
					sSockString[0] = '\0';
            	}
            	
            	if(gRemoteDebug_clientObj.DebugStatus == DEBUG_SERIAL)
            	{
					printf(" %s %s\n", gRemoteDebug_corePrefix[coreId], tmp );
            	}

                if(gRemoteDebug_clientObj.DebugStatus == DEBUG_NONE)
                	;
                doWait = 0;
            }

            if((coreId == REMOTE_DEBUG_CORE_ID_HOST) && pFullBuf)
        	{
        		OSA_quePut(&(gRemoteDebug_clientObj.freeQuehdl),(Int32)pFullBuf, OSA_TIMEOUT_FOREVER);					
        	}
        }while(numBytes);
       
    }
    if(doWait)
      OSA_waitMsecs(10);
  }

  RemoteDebug_clientDeInit();

  RemoteDebug_socketCleintDeInit();
  return 0;
}

STATIC int RemoteDebug_printCmdLineArgs(char *str)
{
  printf(" \n");
  printf(" Remote Debug Client Utility, (c) Texas Instruments 2010\n");
  printf(" \n");
  printf(" %s <remote debug shared memory physical address in Hex> \n", str);
  printf(" \n");
  printf(" The shared memory physical address MUST be 4KB aligned. \n");
  printf(" \n");

  return 0;
}

unsigned int RemoteDebug_getCoreId(char *coreName)
{
    if(strcmp(coreName,"c6xdsp")==0)
        return REMOTE_DEBUG_CORE_ID_C6XDSP;
    if(strcmp(coreName,"m3vpss")==0)
        return REMOTE_DEBUG_CORE_ID_M3VPSS;
    if(strcmp(coreName,"m3video")==0)
        return REMOTE_DEBUG_CORE_ID_M3VIDEO;

    return REMOTE_DEBUG_CORE_ID_M3VPSS;
}

int main(int argc, char **argv)
{
  unsigned int remoteDebugShmPhysAddr, coreId;

  if(argc<2)
  {
    RemoteDebug_printCmdLineArgs(argv[0]);
    exit(0);
  }

  remoteDebugShmPhysAddr = xstrtoi(argv[1]);
  if(remoteDebugShmPhysAddr==0 || (remoteDebugShmPhysAddr & 0xFFF))
  {
    RemoteDebug_printCmdLineArgs(argv[0]);
    exit(0);
  }

  if(argc==5 || argc==4)
  {
    if(argc==5)
        coreId = RemoteDebug_getCoreId(argv[4]);
    else
        coreId = REMOTE_DEBUG_CORE_ID_M3VPSS;

    if(strcmp(argv[2], "--putch")==0)
    {
      RemoteDebug_putChar(remoteDebugShmPhysAddr, coreId, argv[3][0]);
    }
    if(strcmp(argv[2], "--putstr")==0)
    {
      RemoteDebug_putStr(remoteDebugShmPhysAddr, coreId, argv[3]);
    }
    if(strcmp(argv[2], "--waitch")==0)
    {
      RemoteDebug_getChar(remoteDebugShmPhysAddr, coreId, argv[3][0], (unsigned int)-1);
    }
  }
  else
  {
    RemoteDebug_clientRun(remoteDebugShmPhysAddr);
  }

  return 0;
}

STATIC void   OSA_waitMsecs(unsigned int msecs)
{
  struct timespec delayTime, elaspedTime;

  delayTime.tv_sec  = msecs/1000;
  delayTime.tv_nsec = (msecs%1000)*1000000;

  nanosleep(&delayTime, &elaspedTime);
}

static char xtod(char c) {
  if (c>='0' && c<='9') return c-'0';
  if (c>='A' && c<='F') return c-'A'+10;
  if (c>='a' && c<='f') return c-'a'+10;
  return c=0;        // not Hex digit
}

static int HextoDec(char *hex, int l)
{
  if (*hex==0)
    return(l);

  return HextoDec(hex+1, l*16+xtod(*hex)); // hex+1?
}

STATIC int xstrtoi(char *hex)      // hex string to integer
{
  return HextoDec(hex,0);
}


