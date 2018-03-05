#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <Msg_Def.h>

#include "ftp.h"
#include "net_config.h"

#define ACKCMD_SOK      0
#define ACKCMD_SFAIL    -1

#define PAD_SIZE			1
#define DEFAULT_BYTES		540


typedef struct _EzFtpMsgType
{
	unsigned char used;
	unsigned char type;
}EzFtpMsgType;

static EzFtpMsgType gMsgtype[EZFTP_CLIENT_MAX];

static EzFtp Ftp;

int FtpEnableSaveErrImg(int qid)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = MSG_TYPE_MSG4;
    msgbuf.cmd = MSG_ENABLE_SAVE_ERRIMG;

	msgsnd(qid, &msgbuf, sizeof(MSG_BUF)-sizeof(long), 0 );
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long),MSG_TYPE_MSG4, 0);
	
    return msgbuf.ret;
}

int FtpDisableSaveErrImg(int qid)
{
    MSG_BUF msgbuf;

    memset(&msgbuf,0,sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = MSG_TYPE_MSG4;
    msgbuf.cmd = MSG_DISABLE_SAVE_ERRIMG;

	msgsnd(qid, &msgbuf, sizeof(MSG_BUF)-sizeof(long), 0 );
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long),MSG_TYPE_MSG4, 0);

    return msgbuf.ret;
}

int SetupEnv()
{	
	InitRemoteDebugMsgDrv(0);
	
	Ftp.Ftpsd = create_server_socket(FTP_SERVER_PORT,0);
	if(Ftp.Ftpsd < 0)
	{
		perror("create_server_socket");

		return -1;
	}
	
	/* listen: large number just in case your kernel is nicely tweaked */
	if (listen(Ftp.Ftpsd, 5) == -1) 
	{
		perror("listen");
		
		close(Ftp.Ftpsd);
		
		return -1;
	}

	if((Ftp.fqid=Msg_Init(MSG_KEY)) < 0){
		delete_server_socket(Ftp.Ftpsd);
		printf("%s in %d:Msg_Init fail\n",__func__,__LINE__);
        return -1;
    }
    
	Ftp.qid = Msg_Init(EZFTP_MSG_KEY);
	if(Ftp.qid < 0)
	{
		Msg_Kill(Ftp.fqid);	
		
		delete_server_socket(Ftp.Ftpsd);
		
		perror("Msg_Init");

		return -1;
	}

	Ftp.hndlSem = MakeSem();
	if(NULL == Ftp.hndlSem)
	{
		delete_server_socket(Ftp.Ftpsd);

		Msg_Kill(Ftp.fqid);
		
		Msg_Kill(Ftp.qid);	

		return -1;	
	}
	
	Ftp.clentCount = 0;

	int i;
	for(i=0;i<EZFTP_CLIENT_MAX;i++)
	{
		gMsgtype[i].used = 0;
		gMsgtype[i].type = i;
	}

	chdir(FTP_ROOT);
	
	return 0;	
}

int CleanupEnv()
{
	delete_server_socket(Ftp.Ftpsd);

	Msg_Kill(Ftp.fqid);	
	
	Msg_Kill(Ftp.qid);
	
	DestroySem(Ftp.hndlSem);

	CleanupRemoteDebugMsgDrv();
	return 0;
}

static int Ftp_Login(EzFtp_Acount *acount)
{
	if(strcmp(acount->UserName, DEFAULT_USERNAME)||strcmp(acount->Passwd, DEFAULT_PASSWD))
		return -1;
	return 0;		
}


static int Ftp_SendData(int sd,char *buf,unsigned int size)
{
	int ret;
	EzFtp_Ack ack;
	
	ret = send(sd, buf, size, 0);
	if(ret != size)
	{
		printf("%s in %d:send fail\n",__func__,__LINE__);
				
		return -1;
	}	

	ret = recv(sd, &ack, sizeof(EzFtp_Ack), 0);
	if(ret <= 0)
	{
		printf("%s in %d:recv ack fail\n",__func__,__LINE__);	
				
		return -1;			
	}
	
	if((ack.status != FTP_ACK_SOK)||(ack.len != size))
	{	
		printf("len=%d,size=%d\n",ack.len,size);
		
		return -1;				
	}	

	return 0;
}

static int Ftp_Rcvdata(int sd,char *buf,unsigned int size)
{
	int len;
	
	len = recv(sd, buf, size, 0);
	if(len <= 0)
	{
		printf("%s in %d:recv fail\n",__func__,__LINE__);			
	}

	return len;
}

void printDir(FileList *list)
{
	int i;
	FileInfo *pfinfo = list->finfo;
	for(i=0;i<list->dirCount;i++)
	{
		printf("file type:%d,file name :%s,file size:%d\n",pfinfo[i].type,pfinfo[i].name,pfinfo[i].size);
	}
}

/** ftp list */
static int Ftp_Listdir(char *pathname,int port,char *ip)
{
	int ret;
	int sd;
	struct sockaddr_in saddr;

	memset(&saddr, 0, sizeof(struct sockaddr_in));	
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(ip);
	saddr.sin_port = htons(port);	
	
	sd = create_server_socket(0,3);
	if(sd<0)
	{
		printf("%s in %d:create_server_socket fail\n",__func__,__LINE__);

		return -1;
	}
	
	printf("%s ip:%s,port:%d\n",__func__,ip,port);

	ret = connect(sd, (const struct sockaddr *)&saddr,sizeof(struct sockaddr));
    	if(ret == -1)
    	{
		printf("%s in %d:connect fail\n",__func__,__LINE__);

		close(sd);
		
		return -1;
    	}               
	else
		printf("%s connect success\n",__func__);

	FileList *list = malloc(sizeof(FileList)+sizeof(FileInfo)*DIR_FILE_MAX_NUM);
	if(NULL == list)
	{
		perror("malloc");

		close(sd);
		
		return -1;
	}

	if(strncmp(pathname, ROOT_NAME, strlen(ROOT_NAME)))
		list->dirCount = 0;
	else
	{	
		ret = Listdir(pathname,list);
		if(ret != LIST_SOK)
		{
			list->dirCount = 0;
		}
	}
	
	int size;
	int send_count;
	EzFtp_Header header;
	unsigned int last;
		
	size = sizeof(FileList) + (list->dirCount)*sizeof(FileInfo);
		
	header.magic1 = MAGIC_HEADER1;
	header.magic2 = MAGIC_HEADER2;
	header.cmd = FTP_MSG_LIST;
	header.len = size;
	header.pad = 0;

	send_count = size/DEFAULT_BYTES;
	
	if(size%DEFAULT_BYTES == sizeof(EzFtp_Header))
	{
		header.pad = PAD_SIZE;
		header.len = size + PAD_SIZE;	
	}

	char *pbuf = (char *)malloc(DEFAULT_BYTES + PAD_SIZE);
	if(NULL == pbuf)
	{
		printf("%s in %d:malloc fail\n",__func__,__LINE__);
		
		free(list);
		
		close(sd);

		return -1;
	}

	ret = Ftp_SendData(sd,(char *)&header,sizeof(EzFtp_Header));
	if(ret < 0)
	{
		printf("%s in %d:Ftp_SendData fail\n",__func__,__LINE__);
		
		free(list);
		free(pbuf);
		
		close(sd);
			
		return -1;
	}

	char *ptr = (char *)list;
	int tmp = send_count;
	while(tmp--)
	{
		ret = Ftp_SendData(sd,ptr,DEFAULT_BYTES);
		if(ret < 0)
		{
			printf("%s in %d:Ftp_SendData fail\n",__func__,__LINE__);
			free(list);
			free(pbuf);
		
			close(sd);
			
			return -1;
		}

		ptr += DEFAULT_BYTES;
	}

	last = size-send_count*DEFAULT_BYTES;
	if(last > 0)
	{
		if(header.pad)
		{
			memcpy(pbuf,ptr,last);
			ret = Ftp_SendData(sd,pbuf,last + header.pad);
			if(ret < 0)
			{
				printf("%s in %d:Ftp_SendData fail\n",__func__,__LINE__);
				
				free(list);
			
				free(pbuf);
		
				close(sd);
			
				return -1;
			}		
		}
		else
		{
			ret = Ftp_SendData(sd,ptr,last);
			if(ret < 0)
			{
				printf("%s in %d:Ftp_SendData fail\n",__func__,__LINE__);
				
				free(list);
			
				free(pbuf);
		
				close(sd);
			
				return -1;
			}
		}		
	}


	free(list);

	free(pbuf);

	close(sd);
	
	return 0;
}

static int Ftp_Getfile(char *pathname,int port,char *ip)
{
	int ret;
	int sd;
	struct sockaddr_in saddr;
			
	memset(&saddr, 0, sizeof(struct sockaddr_in));	
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(ip);
	saddr.sin_port = htons(port);	
	
	sd = create_server_socket(0,3);
	if(sd<0)
	{
		printf("%s in %d:create_server_socket fail\n",__func__,__LINE__);

		return -1;
	}

	ret = connect(sd, (const struct sockaddr *)&saddr,sizeof(struct sockaddr));
    if(ret == -1)
    {
		printf("%s in %d:connect fail\n",__func__,__LINE__);

		close(sd);
		
		return -1;
    }               

	
	int size;
	char *pData = NULL;	
	if(strncmp(pathname, ROOT_NAME, strlen(ROOT_NAME)))
		size = 0;
	else
		size = getFileSize(pathname);
		
	if(size > 0)
		pData = (char *)malloc(size+10);	

	size = getFile(pathname,pData,size+10);


	EzFtp_Header header;	
	int send_count;
		
	header.magic1 = MAGIC_HEADER1;
	header.magic2 = MAGIC_HEADER2;
	header.cmd = FTP_MSG_GET;
	header.len = size;
	header.pad = 0;


	send_count = size/DEFAULT_BYTES;
	
	if(size%DEFAULT_BYTES == sizeof(EzFtp_Header))
	{
		header.pad = PAD_SIZE;
	}

	char *pbuf = (char *)malloc(DEFAULT_BYTES + PAD_SIZE);
	if(NULL == pbuf)
	{
		printf("fail:%s\n",__func__);
		
		if(pData)
			free(pData);
		
		close(sd);
	}

	ret = Ftp_SendData(sd,(char *)&header,sizeof(EzFtp_Header));
	if(ret < 0)
	{
		printf("fail:%s\n",__func__);

		if(pData)
			free(pData);

		if(pbuf)	
			free(pbuf);
		
		close(sd);
			
		return -1;
	}

	char *ptr = pData;
	int tmp = send_count;
	while(send_count--)
	{
		ret = Ftp_SendData(sd,ptr,DEFAULT_BYTES);
		if(ret < 0)
		{
			printf("fail:%s\n",__func__);
			
			if(pData)
				free(pData);
				
			if(pbuf)	
				free(pbuf);
		
			close(sd);
			
			return -1;
		}

		ptr += DEFAULT_BYTES;
	}

	unsigned int last;
	last = size-tmp*DEFAULT_BYTES;

	if(last > 0)
	{
		if(header.pad)
		{
			memcpy(pbuf,ptr,last);
			ret = Ftp_SendData(sd,pbuf,last + header.pad);
			if(ret < 0)
			{
				if(pData)
					free(pData);
			
				if(pbuf)	
					free(pbuf);
		
				close(sd);
			
				return -1;
			}		
		}
		else
		{
			ret = Ftp_SendData(sd,ptr,last);
			if(ret < 0)
			{
				if(pData)
					free(pData);
			
				if(pbuf)	
					free(pbuf);
		
				close(sd);
			
				return -1;
			}
		}		
	}

	if(pData)
		free(pData);

	if(pbuf)	
		free(pbuf);

	close(sd);
	
	return 0;	
}

static int Ftp_Deletefile(char *pathname)
{
	char cmd[256];
	int len = strlen(DELETE_DIR);
	
	if(pathname == NULL)
		return -1;
	
	if(strncmp(DELETE_DIR, pathname, len) != 0)
	{
		return -1;
	}
	else
	{
		sprintf(cmd,"rm %s -f",pathname);
		system(cmd);
	}

	return 0;
}
static char *UpdateFile[] = 
{
	FIRMWARE_C6DSP,
	FIRMWARE_VPSSM3,
	FIRMWARE_VIDEOM3,
	EZCAM_EZFTP_PATH,
	EZCAM_APP_FILE,
	EZCAM_MCFW_PATH,
	EZCAM_APP_SERVER,
	EZCAM_ALG_DBGIMG,
	SDS_TEST_PATH
};
#define MAX_FIREWARE_SIZE		(20*1024*1024)

static int Ftp_Putfile(char *pathname,unsigned int fileSize,int port,char *ip)
{
	int ret;
	int sd;
	struct sockaddr_in saddr;
	int status = 0;
	
	char *pData = NULL;
	char *buf = NULL;
	
	int fileCount;
	int fileIndex;
	
	memset(&saddr, 0, sizeof(struct sockaddr_in));	
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr(ip);
	saddr.sin_port = htons(port);	

	if(0 == status)
	{
		do
		{
			sd = create_server_socket(0,3);
			if(sd<0)
			{
				printf("%s in %d:create_server_socket fail\n",__func__,__LINE__);

				return -1;
			}
			ret = connect(sd, (const struct sockaddr *)&saddr,sizeof(struct sockaddr));
    		if(ret == -1)
    		{
				printf("%s in %d:connect fail\n",__func__,__LINE__);

				status = -1;

				break;	
   			 } 
		}while(0);
	}

	if( 0 == status)
	{
		do
		{
			EzFtp_Header header;
			header.magic1 = MAGIC_HEADER1;
			header.magic2 = MAGIC_HEADER2;
			header.len = 0;
			header.pad = 0;	
			header.cmd = FTP_MSG_PUT;
			header.status = 0;
		
			if(fileSize > MAX_FIREWARE_SIZE)
			{
				header.status = -1;

				status = -1;
			}

			fileCount = sizeof(UpdateFile)/sizeof(UpdateFile[0]);
			for(fileIndex = 0;fileIndex<fileCount;fileIndex++)
			{
				if(strcmp(pathname,UpdateFile[fileIndex]) == 0)
				{
					break;
				}
					
			}

			if(fileIndex == fileCount)
			{
				header.status = -1;
				status = -1;
			}
			
			if((pData = (char *)malloc(DEFAULT_BYTES)) == NULL)
			{
				header.status = -1;
				status = -1;
			}
			
			if((buf =(char *)malloc(fileSize)) == NULL)
			{
				header.status = -1;
				status = -1;
			}
						
			ret = send(sd, &header, sizeof(header),0);
			if(ret != sizeof(header))
			{
				status = -1;

				break;	
			}	
		}while(0);
	}

	unsigned int imageCount = 0;
	if(0 == status)
	{
		int len;		
		do
		{
			len = Ftp_Rcvdata(sd,pData,DEFAULT_BYTES);
			if(len < 0)
			{
				printf("Ftp_Rcvdata fail\n");
				status = -1;
				break;
			}
			
			memcpy(buf+imageCount,pData,len);
			imageCount += len;

		}while(fileSize > imageCount);
	}
	
	if(imageCount != fileSize)
	{
		printf("update fail\n");
		status = -1;
	}

	
	if(0 == status)
		putFile(pathname,buf,fileSize);

	if(0 == status)
	{
		Remote_printf("update success\n");
	}
	if(buf)
		free(buf);
		
	if(pData)
		free(pData);
			
	close(sd);
	
	return 0;	
}
static void* Ftp_MsgProcThread(void *arg)
{
	FTP_MSG_BUF msgbuf;
	unsigned int type = *(unsigned int *)arg;
	unsigned int IsQuit = 0;
	int 	msg_size;

	while(IsQuit == 0)
	{
		msg_size = msgrcv( Ftp.qid, &msgbuf, sizeof(msgbuf) - sizeof(long),type, 0);
		if( msg_size < 0 )
		{
			IsQuit = 1;
		}
		else
		{
			switch(msgbuf.cmd)
			{
				case FTP_MSG_CLOSE:
				{
					IsQuit = 1;
					break;
				}	
				case FTP_MSG_LIST:
				{
					Ftp_Listdir(msgbuf.filePath,msgbuf.port,msgbuf.ip);
					break;
				}
				case FTP_MSG_GET:
				{
					FtpDisableSaveErrImg(Ftp.fqid);
					Ftp_Getfile(msgbuf.filePath,msgbuf.port,msgbuf.ip);
					FtpEnableSaveErrImg(Ftp.fqid);
					break;
				}
				case FTP_MSG_PUT:
				{
					Ftp_Putfile(msgbuf.filePath,msgbuf.fileSize,msgbuf.port,msgbuf.ip);
					break;
				}
				case FTP_MSG_DELETE:
				{
					FtpDisableSaveErrImg(Ftp.fqid);
					Ftp_Deletefile(msgbuf.filePath);
					FtpEnableSaveErrImg(Ftp.fqid);
				}
				default:
					break;			
			}
		}
	}	

	Ftp_thrExit((void *)0);

	return (void *)0;
}

static void *Ftp_CtrlThread(void *arg)
{
	Ftp_ThrHndl hndl;
	unsigned int thrQuit = 0;
	EzFtp_thrParam *pPrm = (EzFtp_thrParam *)arg;
	unsigned int msgtype = pPrm->msgtype;
	int sd = pPrm->sd;
	struct sockaddr_in peerAddr;
	socklen_t peerLen = sizeof(peerAddr);
	char *peerIp = NULL;
	ssize_t size;
	int ret;
	EzFtp_NetMsg *msg = (EzFtp_NetMsg *)malloc(sizeof(EzFtp_NetMsg) + MAX_CMD_LENGTH);
	if(NULL == msg)
	{
		perror("malloc");
		Ftp_thrExit((void *)0);
	}

	EzFtp_NetMsg *ackmsg = (EzFtp_NetMsg *)malloc(sizeof(EzFtp_NetMsg) + MAX_CMD_LENGTH);
	if(NULL == ackmsg)
	{
		free(msg);
		Ftp_thrExit((void *)0);
	}

	/** create msg proc thread */
	ret = Ftp_thrCreate(&hndl, Ftp_MsgProcThread, FTP_THR_PRI_DEFAULT, FTP_THR_STACK_SIZE_DEFAULT,&msgtype);
	if(ret != 0)
	{
		Ftp_thrExit((void *)0);
	}	

	getpeername(sd, (struct sockaddr *)&peerAddr, &peerLen);
	peerIp = inet_ntoa(peerAddr.sin_addr);
			
	unsigned int logon = 0;
	
	FTP_MSG_BUF msgbuf;
	msgbuf.Des = msgtype;
	while(thrQuit == 0)
	{
		size = recv(sd, msg, MAX_CMD_LENGTH, 0);
		if(size <= 0)
		{	/* the peer down */
			thrQuit = 1;
		}
		else
		{
			ackmsg->cmd = msg->cmd;
			ackmsg->ackCmd = ACKCMD_SOK;
			ackmsg->len = 0;
			switch(msg->cmd)
			{
				case FTP_MSG_LOGIN:
				{
					ret = Ftp_Login((EzFtp_Acount *)(msg->buf));
					if(ret != 0)
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						thrQuit = 1;
					}
					else
						logon = 1;
					break;
				}
				case FTP_MSG_CLOSE:
				{
					logon = 0;
					
					thrQuit = 1;
					break;
				}	
				case FTP_MSG_LIST:
				{
					EzFtp_File *pfile;
					if(logon == 0)
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;						
					}
					
					if(msg->len != sizeof(EzFtp_File))
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;
					}

					pfile = (EzFtp_File *)(msg->buf);
					msgbuf.cmd = msg->cmd;
					msgbuf.port = pfile->port;
					memcpy(msgbuf.ip,peerIp,strlen(peerIp)+1);
					memcpy(msgbuf.filePath,pfile->filePath,MAX_FILE_PATH);
					
					ret = msgsnd(Ftp.qid, &msgbuf, sizeof(FTP_MSG_BUF)-sizeof(long), 0 );
					if(ret != 0)
					{
						thrQuit = 1;
					}
					break;
				}
				case FTP_MSG_GET:
				{
					EzFtp_File *pfile;
					if(logon == 0)
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;						
					}
										
					if(msg->len != sizeof(EzFtp_File))
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;
					}
					
					pfile = (EzFtp_File *)(msg->buf);
					msgbuf.cmd = msg->cmd;
					msgbuf.port = pfile->port;
					memcpy(msgbuf.ip,peerIp,strlen(peerIp)+1);
					memcpy(msgbuf.filePath,pfile->filePath,MAX_FILE_PATH);

					ret = msgsnd(Ftp.qid, &msgbuf, sizeof(FTP_MSG_BUF)-sizeof(long), 0 );
					if(ret != 0)
					{
						thrQuit = 1;
					}				
					break;
				}
				case FTP_MSG_PUT:
				{
					EzFtp_File *pfile;
					if(logon == 0)
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;						
					}					
					if(msg->len != sizeof(EzFtp_File))
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;
					}

					pfile = (EzFtp_File *)(msg->buf);
					msgbuf.cmd = msg->cmd;
					msgbuf.port = pfile->port;
					memcpy(msgbuf.ip,peerIp,strlen(peerIp)+1);
					memcpy(msgbuf.filePath,pfile->filePath,MAX_FILE_PATH);
					msgbuf.fileSize = pfile->fileSize;
					
					ret = msgsnd(Ftp.qid, &msgbuf, sizeof(FTP_MSG_BUF)-sizeof(long), 0 );
					if(ret != 0)
					{
						thrQuit = 1;
					}				
					break;
				}
				case FTP_MSG_DELETE:
				{
					EzFtp_File *pfile;
					if(logon == 0)
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;						
					}					
					if(msg->len != sizeof(EzFtp_File))
					{
						ackmsg->ackCmd = ACKCMD_SFAIL;
						break;
					}

					pfile = (EzFtp_File *)(msg->buf);
					msgbuf.cmd = msg->cmd;
					memcpy(msgbuf.filePath,pfile->filePath,MAX_FILE_PATH);
					msgbuf.fileSize = pfile->fileSize;
					
					ret = msgsnd(Ftp.qid, &msgbuf, sizeof(FTP_MSG_BUF)-sizeof(long), 0 );
					if(ret != 0)
					{
						thrQuit = 1;
					}				
					break;					
				}
				default:
					ackmsg->ackCmd = ACKCMD_SFAIL;
					break;
			}	
			
			size = send(sd, ackmsg, sizeof(EzFtp_NetMsg) + ackmsg->len, 0);
			if(size != sizeof(EzFtp_NetMsg) + ackmsg->len)
				thrQuit = 1;
		}
		

	}

	msgbuf.cmd = FTP_MSG_CLOSE;
	msgsnd(Ftp.qid, &msgbuf, sizeof(FTP_MSG_BUF)-sizeof(long), 0 );	
					
	Ftp_thrJoin(&hndl);

	free(msg);
	
	free(ackmsg);

	close(sd);
	
	SemWait(Ftp.hndlSem);
	Ftp.clentCount--;
	gMsgtype[msgtype].used = 0;
	SemRelease(Ftp.hndlSem);

	Ftp_thrExit((void *)0);
		
	return (void *)0;
}


int main(int argc,char *argv[])
{
	unsigned int quit = 0;
	int ret;
	int sd;
	struct sockaddr dest_addr;
	int i;
	socklen_t addrlen = sizeof(struct sockaddr);
	EzFtp_thrParam prm;
	ret = SetupEnv();
	if(ret < 0)
	{
		return -1;
	}	
	while(quit == 0)
	{
		sd = accept(Ftp.Ftpsd, &dest_addr, &addrlen);
		if(sd<0)
		{
			quit = 1;
		}
		
		SemWait(Ftp.hndlSem);
		Ftp.clentCount++;
		if(Ftp.clentCount >= EZFTP_CLIENT_MAX)
		{
			close(sd);
			
			continue;
		}
		prm.sd = sd;
		for(i=0;i<EZFTP_CLIENT_MAX;i++)
		{
			if(gMsgtype[i].used == 0)
			{
				gMsgtype[i].used = 1;
				prm.msgtype = gMsgtype[i].type;
			}
		}
		SemRelease(Ftp.hndlSem);
			
		ret = Ftp_thrCreate(&(Ftp.hndl), Ftp_CtrlThread, FTP_THR_PRI_DEFAULT, FTP_THR_STACK_SIZE_DEFAULT, &prm);
		if(ret != 0)
		{
			quit = 1;
		}
	}
	
	CleanupEnv();	

	return 0;
}
