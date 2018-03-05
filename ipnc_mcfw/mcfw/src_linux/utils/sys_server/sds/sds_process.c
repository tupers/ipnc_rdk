#include <stdio.h>
#include <stdlib.h>
#include <Msg_Def.h>
#include "sds_process.h"
#include "sds_serial.h"
#include "util_shmem.h"

static SdsProcess sds;
static AlgInfo_St *gPtr;

int SdsSetupEnv()
{
	sds.qid = Msg_Init(ALGAPP_MSG_KEY);
	if(sds.qid < 0)
	{
		return -1;
	}

	sds.mid = Utils_ShareMemInit(ALGAPP_SHM_KEY,4096);
	if(sds.mid < 0)
		return -1;
		
	sds.fd = Serial_Open(SDS_RS485_DEV);
	if(sds.fd < 0)
	{
		return -1;
	}

	gPtr = (AlgInfo_St *)malloc(sizeof(AlgInfo_St) + RESULT_MAX_SIZE);
	if(gPtr == NULL)
	{
		Serial_Close(sds.fd);

		return -1;
	}
	return 0;
}

void SdsCleanupEnv()
{
	if(gPtr)
		free(gPtr);

	Serial_Close(sds.fd);
}

int SdsProcMsg(ALGAPP_MSG_BUF *msg)
{
	gPtr->size = msg->size;
	
	Utils_ShareMemRead(sds.mid,msg->offset,gPtr->result, msg->size);

	msg->ret = 0;
	
	return 0;
}

static void SdsAlg_Start(SdsProcess obj)
{
	ALGAPP_MSG_BUF msgbuf;
	msgbuf.Des = MSG_TYPE_MSG3;
	msgbuf.src = MSG_TYPE_MSG1;
	msgbuf.cmd = ALG_CMD_START;
	msgbuf.ret = 0;

	Msg_Send(obj.qid, &msgbuf , sizeof(msgbuf));

	Msg_Rsv(obj.qid, MSG_TYPE_MSG1, &msgbuf , sizeof(ALGAPP_MSG_BUF));
}
static void Sds_ProcessThread()
{
	ALGAPP_MSG_BUF msgbuf;
	int msgsize;
	unsigned int quit = 0;
	
	while(quit == 0)
	{
		msgsize = Msg_Rsv( sds.qid, MSG_TYPE_MSG1, &msgbuf , sizeof(ALGAPP_MSG_BUF));
		if(msgsize < 0)
		{
			quit = 1;
		}
		else
		{
			switch(msgbuf.cmd)
			{
				case ALG_CMD_RESULT:
				{
					SdsProcMsg(&msgbuf);
					break;
				}	
				case ALG_CMD_DELETE:
				{
					quit = 1;
					break;
				}								
				default:
				{
					msgbuf.ret = 0;
					break;
				}
			}

			if(msgbuf.src != 0){
				msgbuf.Des = msgbuf.src;
				msgbuf.src = MSG_TYPE_MSG1;
				Msg_Send( sds.qid, &msgbuf , sizeof(msgbuf) );
			}
		}
	}	
}

int main(int argc,char *argv[])
{
	int ret;
	
	ret = SdsSetupEnv();
	if(ret <0)
		return -1;

	Sds_ProcessThread();
	
	SdsCleanupEnv();
	
	return 0;	
}
