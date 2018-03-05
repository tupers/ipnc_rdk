/*
* Copyright (c) Appro Photoelectron Inc.  2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <Msg_Def.h>
/* Function error codes */
#define SUCCESS         0
#define FAILURE         -1

/* True of false enumeration */
#define TRUE            1
#define FALSE           0

#ifdef __DEBUG
#define DBG(fmt, args...) fprintf(stderr, "Sever Debug: " fmt, ## args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...) fprintf(stderr, "Server Error: " fmt, ## args)

int ProcMsg(MSG_BUF *pMsg)
{
	int ret = 0;
	if(pMsg->cmd > MSG_CMD_START && pMsg->cmd < MSG_CMD_END){
		ret = 1;
		if(pMsg->cmd == MSG_CMD_GET_MEM){
			pMsg->mem_info.addr = 1234;
			pMsg->mem_info.size = 256;
		}
	}
	return ret;
}

int main(int argc, char **argv)
{
	int qid;
	MSG_BUF msgbuf;
	int msg_size, e_count = 0;
	qid = Msg_Init(MSG_KEY);
	while(1){
		msg_size = msgrcv( qid, &msgbuf, sizeof(msgbuf) - sizeof(long),
				MSG_TYPE_MSG1, 0);
		printf("Got message in dummy server\n");
		if( msg_size < 0 ){
			if(e_count++ >= 3){
				ERR("receive msg fail \n");
				perror("ERROR:");
				break;
			}
		}else if(msgbuf.Src == MSG_TYPE_MSG1 || msgbuf.Src < 0){
			ERR("Got Error message\n");
			break;
		} else {
			e_count = 0;
			if(ProcMsg(&msgbuf) == 1)
				msgbuf.ret = 0;
			else
				msgbuf.ret = 1;
			if(msgbuf.Src != 0){
				msgbuf.Des = msgbuf.Src;
				msgbuf.Src = MSG_TYPE_MSG1;
				msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);
			}
		}
	}
	
	Msg_Kill(qid);
    return SUCCESS;
}
