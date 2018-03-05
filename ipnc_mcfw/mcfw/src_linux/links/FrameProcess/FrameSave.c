#include "FrameSave.h"
#include "frameProcess_pri.h"
#include "osa_fileops.h"


Int32 FrameSave_create(FrameProcessLink_Obj * pObj)
{
	int ret;

	ret = OSA_mkdir(IMGSAVE_ROOT);
	if(ret<0)
		return -1;

	OSA_chdir(IMGSAVE_ROOT);
		
	return OSA_SOK;
}

Int32 FrameSave_delete(FrameProcessLink_Obj * pObj)
{
	return OSA_SOK;
} 

int MkErrImgSaveDir(AlgSaveImg info,char *pathname)
{
	char dirName[50] = {0};
	int num;
	int ret = 0;
	
	sprintf(dirName,"%s/%s",IMGSAVE_ROOT,info.Alg_saveImgdirName);
	OSA_mkdir(dirName);

	sprintf(dirName, "%s/%s/%s_%d",IMGSAVE_ROOT,info.Alg_saveImgdirName,ERRIMG_DIR,info.Alg_saveImgIndex);
	OSA_mkdir(dirName);	

	sprintf(dirName, "%s/%s_%d",info.Alg_saveImgdirName,ERRIMG_DIR,info.Alg_saveImgIndex);
	num = OSA_getDir_count(".");
	if(num > MAX_IMG_DIR)
	{
		if(OSA_fileExist(dirName))
			memcpy(pathname,dirName,strlen(dirName)+1);
		else
			ret = -1;
	}
	else
	{
		OSA_mkdir((char *)info.Alg_saveImgdirName);

		memcpy(pathname,dirName,strlen(dirName)+1);
	}

	return ret;
}

int MkImgSaveDir(char *pathname)
{
	char dirName[20] = {0};
	
	sprintf(dirName, "%s",IMGSAVE_DIR);
	OSA_mkdir(dirName);	

	memcpy(pathname,dirName,strlen(dirName)+1);

	return 0;	
}
int SaveImgFile(ErrFrame_Buf *pFullFrame)
{
	char curDir[50] = {0};
	char desDir[50] = {0};
	char fileName[40] = {0};
	int fileCnt = 0;
	long wrCnt = 0;
	long ssize = 0;
	unsigned wrlen = 0;
	int ret;
	
	OSA_getcurDir(curDir,50);

	if(IMG_SAVE == pFullFrame->type)
	{
		ret = MkImgSaveDir(desDir);
	}
	else
		ret = MkErrImgSaveDir(pFullFrame->info,desDir);

	OSA_chdir(desDir);

	fileCnt = OSA_getFile_count(".");
	if(fileCnt >= MAX_IMG_FILE)
	{
		OSA_chdir(curDir);
		return -1;
	}	
	
	ImgFrameHeader header;
	header.width = pFullFrame->width;
	header.height = pFullFrame->height;
	header.pitch = pFullFrame->pitch;
	
	if(IMG_SAVE == pFullFrame->type)
		header.infoSize = 0;
	else	
		header.infoSize = sizeof(AlgSaveImg);
				
	sprintf(fileName,"%s_%s",IMG_FILENAME,pFullFrame->fileName);
	
	/** check file exist **/
	if(OSA_fileExist(fileName))
	{
		OSA_chdir(curDir);
		return -1;			
	}

	/** write img header **/
	wrlen = sizeof(header); 
	ssize = OSA_writeFile(fileName,wrCnt,wrlen,(char *)&header);
	if(ssize != wrlen)
	{
		OSA_chdir(curDir);
		return -1;		
	}
	wrCnt += ssize;

	/** write img information **/
	wrlen = header.infoSize;
	ssize = OSA_writeFile(fileName,wrCnt,wrlen,(char *)&pFullFrame->info);
	if(ssize != wrlen)
	{
		OSA_chdir(curDir);
		return -1;		
	}
	wrCnt += ssize;

	/** write img **/
	wrlen = header.height * header.pitch;
	ssize = OSA_writeFile(fileName,wrCnt,wrlen,(char *)pFullFrame->pData);
	if(ssize != wrlen)
	{
		OSA_chdir(curDir);
		return -1;		
	}	
	wrCnt += ssize;

	OSA_chdir(curDir);
	
	return 0;	
}
Int32 FrameSave_Process(FrameProcessLink_Obj * pObj)
{
	int status;
	int ret;
	ErrFrame_Buf *pErrFrame = NULL;	
	while(1)
	{
		status = OSA_queGet(&pObj->outErrFrameBufQue, (Int32 *)&pErrFrame,OSA_TIMEOUT_NONE);                  
		if (status != OSA_SOK)
			break;	
		if(pErrFrame != NULL)
		{	
			ret = SaveImgFile(pErrFrame);
			if(ret < 0)
			{
				printf("%s in %d : SaveImgFile fail\n",__func__,__LINE__);
			}																
		}

		status = OSA_quePut(&pObj->freeErrFrameBufQue,(Int32)(pErrFrame), OSA_TIMEOUT_NONE);
		OSA_assert(status == OSA_SOK);	
	}	
	
	return OSA_SOK;
}

/*
 * @fucntion FrameSave_tskMain
 *
 * @param 
 *
 * @return 
 */
Int FrameSave_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg,Uint32 curState)
{
    UInt32 cmd = OSA_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status = FRAMEPROCESS_LINK_S_SUCCESS;
    FrameProcessLink_Obj *pObj = (FrameProcessLink_Obj *) pTsk->appData;

    OSA_printf("%s:Entered", __func__);
    if (cmd != SYSTEM_CMD_CREATE)
    {
        OSA_tskAckOrFreeMsg(pMsg, OSA_EFAIL);
        return status;
    }

    /* create the FrameProcessLink */
    status = FrameSave_create(pObj);

    OSA_tskAckOrFreeMsg(pMsg, status);

    if (status != OSA_SOK)
        return status;

    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = OSA_tskWaitMsg(pTsk, &pMsg);
        if (status != OSA_SOK)
            break;

        cmd = OSA_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
            {
                done = TRUE;
                ackMsg = TRUE;
                OSA_tskAckOrFreeMsg(pMsg, status);	
                break;
            }    
            case SYSTEM_CMD_NEW_DATA:	
            {
				OSA_tskAckOrFreeMsg(pMsg, status);	
                FrameSave_Process(pObj);
                break;
            }               	   
            default:
            {
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
            }    
        }
    }
	
    /* Delete the glbceSupport alg */
    FrameSave_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        OSA_tskAckOrFreeMsg(pMsg, status);
    }

    return status;
	
}
                          
