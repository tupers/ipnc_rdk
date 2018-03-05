#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <Msg_Def.h>
#include "frameProcess_pri.h"

/*
 * @fucntion AlgApp_tskMain
 *
 * @param 
 *
 * @return 
 */
Int AlgApp_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg,Uint32 curState)
{
    UInt32 cmd = OSA_msgGetCmd(pMsg);
    Bool done;
    Int32 status = FRAMEPROCESS_LINK_S_SUCCESS;
    FrameProcessLink_Obj *pObj = (FrameProcessLink_Obj *) pTsk->appData;
	ALGAPP_MSG_BUF	msgbuf;
	int ret;
    OSA_printf("%s:Entered", __func__);
    if (cmd != SYSTEM_CMD_CREATE)
    {
        OSA_tskAckOrFreeMsg(pMsg, OSA_EFAIL);
        return status;
    }

    OSA_tskAckOrFreeMsg(pMsg, status);

    done = FALSE;
	unsigned int eCount = 0;
    while (done == FALSE)
    {
    	ret = Msg_Rsv(pObj->qSerialId, MSG_TYPE_MSG3, &msgbuf , sizeof(msgbuf));
    	if(ret < 0)
    	{
			if((pObj->qSerialId = Msg_Init(ALGAPP_MSG_KEY)) < 0)
			{
				eCount++;
				if(eCount>3)
					done = TRUE;
				continue;	
			}
    	}

		eCount = 0;
        cmd = msgbuf.cmd;
        switch (cmd)
        {
            case ALG_CMD_START:
            {
                pObj->AlgStartFlag = TRUE;
                msgbuf.ret = 0;
                break;
			}
            case ALG_CMD_STOP: 
            {
            	pObj->AlgStartFlag = FALSE;
            	msgbuf.ret = 0;
				break;
            }   
            case ALG_CMD_DELETE:	
            {
            	pObj->AlgStartFlag = FALSE;
            	msgbuf.ret = 0;
                break; 
            }              	   
            default:               
                break;
        }
    }

	while(1)
	{
		printf("%s quit\n",__func__);
		sleep(2);
	}
    return status;
	
}
                          
