/** ==================================================================
 *  @file   ti_mcfw_ipcbits.c
 *
 *  @path    ipnc_mcfw/demos/mcfw_api_demos/multich_usecase/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2011 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \file chains_ipcBits.c
    \brief
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>                                     // For stat().
#include <sys/stat.h>                                      // For stat().
#include <sys/statvfs.h>                                   // For statvfs()
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <osa.h>
#include <osa_que.h>
#include <osa_mutex.h>
#include <osa_thr.h>
#include <osa_sem.h>

#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ivideo.h>

#include <ti_mcfw_ipcbits.h>
#include <ti_mcfw_ipnc_main.h>
#include <ti_venc.h>
#include <ti_vdec.h>
#include <ti_mjpeg.h>

#include <mcfw/src_linux/mcfw_api/ti_vsys_priv.h>
#include <stream.h>

#include <cmem.h>

//#define IPCBITS_DEBUG_PRINT

//#define DUMP_BITSTREAM

#ifdef DUMP_BITSTREAM
	FILE *gFp;
	UInt32 gFrCnt  = 2000;
	Int32 gDumpCnt = 0;
	Int32 gCodecChanged;
	UInt32 gCodecType;
#endif

#define INVALID_CODEC_TYPE		(0xFFFF)
UInt32 gCurCodecType[3] = {INVALID_CODEC_TYPE,
						   INVALID_CODEC_TYPE,
						   INVALID_CODEC_TYPE};

#define MCFW_IPC_BITS_MAX_FILE_SIZE_INFINITY       (~(0u))
#ifdef KB
#undef KB
#endif

#ifdef MB
#undef MB
#endif

#ifdef GB
#undef GB
#endif

#define KB                                                              (1024)
#define MB                                                              (KB*KB)
#define GB                                                              (KB*KB*KB)

/* Set to TRUE to prevent doing fgets */
#define MCFW_IPC_BITS_DISABLE_USER_INPUT                              	(FALSE)

#define MCFW_IPC_BITS_MAX_FILENAME_LENGTH                               (64)
#define MCFW_IPC_BITS_MAX_PATH_LENGTH                                   (256)
#define MCFW_IPC_BITS_MAX_FULL_PATH_FILENAME_LENGTH                     (MCFW_IPC_BITS_MAX_PATH_LENGTH+MCFW_IPC_BITS_MAX_FILENAME_LENGTH)
#define MCFW_IPC_BITS_FILE_STORE_DIR                                    "/dev/shm"
#define MCFW_IPC_BITS_HDR_FILE_NAME                                     "VBITS_HDR"
#define MCFW_IPC_BITS_DATA_FILE_NAME                                    "VBITS_DATA"
#define MCFW_IPC_BITS_FILE_EXTENSION                                    "bin"

#define MCFW_IPC_BITS_MAX_NUM_CHANNELS                                  (3)
#define MCFW_IPC_BITS_NONOTIFYMODE_BITSIN                               (FALSE)
#define MCFW_IPC_BITS_NONOTIFYMODE_BITSOUT                              (FALSE)

#define MCFW_IPC_BITS_MAX_DEFAULT_SIZE                                  (4*MB)
#define MCFW_IPC_BITS_FREE_SPACE_RETRY_MS                               (8)
#define MCFW_IPC_BITS_FREE_SPACE_MAX_RETRY_CNT                          (500)

#define MCFW_IPC_BITS_MAX_BUFCONSUMEWAIT_MS                             (1000)

#define MCFW_IPC_BITS_MAX_FILE_SIZE           							(MCFW_IPC_BITS_MAX_FILE_SIZE_INFINITY)

#define MCFW_IPC_BITS_INIT_FILEHANDLE(fp)                                      \
                                                   do {                        \
                                                       if (fp != NULL)         \
                                                       {                       \
                                                           fclose(fp);         \
                                                           fp = NULL;          \
                                                       }                       \
                                                   } while (0)

#define MCFW_IPC_BITS_ENCODER_FPS                      (60)
#define MCFW_IPC_BITS_ENCODER_BITRATE                  (20 * 1024 * 1024)
#define MCFW_IPC_BITS_FILEBUF_SIZE_HDR                 (sizeof(VCODEC_BITSBUF_S) * MCFW_IPC_BITS_ENCODER_FPS)
#define MCFW_IPC_BITS_FILEBUF_SIZE_DATA                (MCFW_IPC_BITS_ENCODER_BITRATE)

#define MCFW_IPCBITS_SENDFXN_TSK_PRI                   (2)
#define MCFW_IPCBITS_RECVFXN_TSK_PRI                   (2)

#define MCFW_IPCBITS_SENDFXN_TSK_STACK_SIZE            (0) /* 0 means system
                                                            * default will be
                                                            * used */
#define MCFW_IPCBITS_RECVFXN_TSK_STACK_SIZE            (0) /* 0 means system
                                                            * default will be
                                                            * used */

#define MCFW_IPCBITS_SENDFXN_PERIOD_MS                 (8)
#define MCFW_IPCBITS_RECVFXN_PERIOD_MS                 (8)

#define MCFW_IPCBITS_INFO_PRINT_INTERVAL               (1000)

/** @enum MCFW_IPCBITS_GET_BITBUF_SIZE
 *  @brief Macro that returns max size of encoded bitbuffer for a given resolution
 */
#define MCFW_IPCBITS_DEFAULT_WIDTH                   (1920)
#define MCFW_IPCBITS_DEFAULT_HEIGHT                  (1080)

#define MCFW_IPCBITS_GET_BITBUF_SIZE(width,height)   ((width) * (height)/2)

#define MCFW_IPCBITS_MAX_PENDING_RECV_SEM_COUNT      (10)

#define MCFW_IPCBITS_MAX_NUM_FREE_BUFS_PER_CHANNEL    (4)
#define MCFW_IPCBITS_FREE_QUE_MAX_LEN                 (MCFW_IPC_BITS_MAX_NUM_CHANNELS * \
                                                         MCFW_IPCBITS_MAX_NUM_FREE_BUFS_PER_CHANNEL)
#define MCFW_IPCBITS_FULL_QUE_MAX_LEN                 (MCFW_IPCBITS_FREE_QUE_MAX_LEN)

#define MCFW_IPC_BITS_ENABLE_FILE_WRITE               (TRUE)

#define MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_CHANNEL_0     (1 << 0)
#define MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_ALLCHANNELS   ((1 << MCFW_IPC_BITS_MAX_NUM_CHANNELS) - 1)
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#define MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_CHANNEL_0_CHANNEL4   ((1 << 0) | (1<<4))
#else
#define MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_CHANNEL_0_CHANNEL16   ((1 << 0) | (1<<16))
#endif

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#define MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_DEFAULT   (MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_CHANNEL_0_CHANNEL4)
#else
#define MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_DEFAULT   (MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_CHANNEL_0_CHANNEL16)
#endif
#define MCFW_IPC_BITS_MAX_PARTITION_USAGE_PERCENT     (95)
#define MCFW_IPC_BITS_MAX_SINGLE_FILE_SIZE            (GB + (GB/2))

enum App_IpcBitsFileType {
    MCFW_IPC_BITS_FILETYPE_HDR,
    MCFW_IPC_BITS_FILETYPE_BUF
};

typedef struct App_IpcBitsCtrlFileObj {
    FILE *fpWrHdr[MCFW_IPC_BITS_MAX_NUM_CHANNELS];
    FILE *fpWrData[MCFW_IPC_BITS_MAX_NUM_CHANNELS];
    FILE *fpWrMvData[MCFW_IPC_BITS_MAX_NUM_CHANNELS];
    char fileDirPath[MCFW_IPC_BITS_MAX_PATH_LENGTH];
    Bool wrapOccuredHdr[MCFW_IPC_BITS_MAX_NUM_CHANNELS];
    Bool wrapOccuredBuf[MCFW_IPC_BITS_MAX_NUM_CHANNELS];
    UInt32 maxFileSize;
    Bool enableFWrite;
    UInt32 fwriteEnableBitMask;
} App_IpcBitsCtrlFileObj;

typedef struct App_IpcBitsCtrlThrObj {
    OSA_ThrHndl thrHandleBitsIn;
    OSA_ThrHndl thrHandleBitsOut;
    OSA_QueHndl bufQFullBufs;
    OSA_QueHndl bufQFreeBufs;
    OSA_SemHndl bitsInNotifySem;
    volatile Bool exitBitsInThread;
    volatile Bool exitBitsOutThread;
    volatile Bool exitBitsInThreadDone;
} App_IpcBitsCtrlThrObj;

typedef struct App_IpcBitsCtrl {
    Bool noNotifyBitsInHLOS;
    Bool noNotifyBitsOutHLOS;;
    App_IpcBitsCtrlFileObj fObj;
    App_IpcBitsCtrlThrObj thrObj;
} App_IpcBitsCtrl;

App_IpcBitsCtrl gApp_ipcBitsCtrl = {
    .fObj.fpWrHdr = {NULL},
    .fObj.fpWrData = {NULL},
    .fObj.fpWrMvData = {NULL},
    .fObj.maxFileSize = MCFW_IPC_BITS_MAX_FILE_SIZE,
    .fObj.enableFWrite = MCFW_IPC_BITS_ENABLE_FILE_WRITE,
    .fObj.fwriteEnableBitMask = MCFW_IPC_BITS_FWRITE_ENABLE_BITMASK_DEFAULT,
    .noNotifyBitsInHLOS = MCFW_IPC_BITS_NONOTIFYMODE_BITSIN,
    .noNotifyBitsOutHLOS = MCFW_IPC_BITS_NONOTIFYMODE_BITSOUT,
};

#define APP_IPCBITSCTRL_FREE_BITBUFINFO_TBL_SIZE                            (128)
#define APP_IPCBITSCTRL_FREE_BITBUFINFO_INVALIDID                           (~0u)

typedef struct App_FreeBitBufInfoTbl {
    OSA_MutexHndl mutex;
    UInt32 freeIndex;
    struct App_FreeBitBufInfoEntry {
        VCODEC_BITSBUF_S bitBuf;
        UInt32 nextFreeIndex;
    } tbl[APP_IPCBITSCTRL_FREE_BITBUFINFO_TBL_SIZE];
} App_FreeBitBufInfoTbl;

App_FreeBitBufInfoTbl g_FreeBitBufInfoTbl;

Void App_ipcBitsInCbFxn(Ptr cbCtx)
{
    App_IpcBitsCtrl *app_ipcBitsCtrl;

    static Int printInterval;

    OSA_assert(cbCtx = &gApp_ipcBitsCtrl);
    app_ipcBitsCtrl = cbCtx;
    OSA_semSignal(&app_ipcBitsCtrl->thrObj.bitsInNotifySem);
    if ((printInterval % MCFW_IPCBITS_INFO_PRINT_INTERVAL) == 0)
    {
#ifdef IPCBITS_DEBUG_PRINT
		OSA_printf("MCFW_IPCBITS: Callback function:%s", __func__);
#endif
    }
    printInterval++;

}


UInt32 cirBufBasePhy;

UInt32 cirBufBaseVirt;

Uint32 cirBufSize;

Vsys_AllocBufInfo strmBufInfo;

char snapFileName[32];

int snapEnable = 0;

extern unsigned int AUDIO_GetTimeStamp(void);

Int App_streamSysInit()
{
    int status;

    STREAM_SET streamSet;

    /* Set the circular buffer address */
    Vsys_allocBuf(VSYS_SR1_SHAREMEM, STREAM_BUF_MEM_SIZE, 32, &strmBufInfo);

    cirBufBasePhy  = (UInt32)strmBufInfo.physAddr;
    cirBufBaseVirt = (UInt32)strmBufInfo.virtAddr;
    cirBufSize     = STREAM_BUF_MEM_SIZE;

    if (cirBufBaseVirt == -1)
    {
#ifdef IPCBITS_DEBUG_PRINT
        OSA_printf("MCFW_IPCBITS:%s:mmap for %X failed\n", __func__, cirBufBasePhy);
#endif
		return -1;
    }
    else
    {
#ifdef IPCBITS_DEBUG_PRINT
        OSA_printf("MCFW_IPCBITS:%s:virt addr of %x cir buffer  is %x\n", __func__, cirBufBasePhy, cirBufBaseVirt);
#endif
    }

    /* stream init */
    memset(&streamSet, 0, sizeof(streamSet));

    /* primary stream params */
#ifdef MEMORY_256MB
	streamSet.Mem_layout = 	(gUI_mcfw_config.demoCfg.resolution_combo == RES_1080P_1080P) ? MEM_LAYOUT_256MB_DUALSTREAM:MEM_LAYOUT_256MB_TRISTREAM;
#else
	streamSet.Mem_layout = 	(gUI_mcfw_config.demoCfg.resolution_combo == RES_1080P_1080P) ? MEM_LAYOUT_512MB_DUALSTREAM:MEM_LAYOUT_512MB_TRISTREAM;
#endif
	streamSet.ImageWidth = 1920;
    streamSet.ImageHeight = 1080;
    streamSet.Mpeg4Quality = 100;
    streamSet.JpgQuality = 100;

    /* second stream */
    streamSet.ImageWidth_Ext[STREAM_EXT_MP4_CIF] = 720;
    streamSet.ImageHeight_Ext[STREAM_EXT_MP4_CIF] = 480;

    /* third stream */
    streamSet.ImageWidth_Ext[STREAM_EXT_JPG] = 1280;
    streamSet.ImageHeight_Ext[STREAM_EXT_JPG] = 720;
	
    status = stream_init(stream_get_handle(), &streamSet);

    if (status != 0)
    {
#ifdef IPCBITS_DEBUG_PRINT
        OSA_printf("MCFW_IPCBITS:%s:stream_init failed\n", __func__);
#endif
		return -1;
    }

    return 0;
}

Int App_streamSysVidWrite(UInt32 chId, UInt32 codecType, Void * pEncodeBuffer,
                          Int32 frameSize, Int32 frameType, UInt32 timestamp,
                          UInt32 temporalId)
{
    UInt32 streamType = STREAM_H264_1;
    Bool writeEnable = TRUE;

    if (chId > gUI_mcfw_config.demoCfg.usecase_id)
        return 0;
				
    switch (chId)
    {
        default:
            writeEnable = FALSE;
            break;
            /* primary stream */
        case 0:
            if (codecType == VCODEC_TYPE_H264)
                streamType = STREAM_H264_1;
            else if (codecType == VCODEC_TYPE_MPEG4)
                streamType = STREAM_MP4;
            else if (codecType == VCODEC_TYPE_MJPEG)
                streamType = STREAM_MJPG;
            else
                writeEnable = FALSE;
            break;
            /* ext stream 1 */
        case 1:
            if (codecType == VCODEC_TYPE_H264)
                streamType = STREAM_H264_2;
            else if (codecType == VCODEC_TYPE_MPEG4)
                streamType = STREAM_MP4_EXT;
            else if (codecType == VCODEC_TYPE_MJPEG)
                streamType = STREAM_MJPG;
            else
                writeEnable = FALSE;
            break;
            /* ext stream 2 */
        case 2:
            if (codecType == VCODEC_TYPE_MJPEG)
                streamType = STREAM_MJPG;
            else
                writeEnable = FALSE;
            break;
    }

	if((gCurCodecType[chId] != INVALID_CODEC_TYPE) &&
	   (gCurCodecType[chId] != codecType))
	{
		/* Codec Type has changed */
		stream_update_vol(stream_get_handle(),streamType);		
	}	
		
	gCurCodecType[chId] = codecType;	

    if (writeEnable == TRUE)
    {
        stream_write(pEncodeBuffer,                        // pAddr
                     frameSize,                            // size
                     frameType,                            // frame_type
                     streamType,                           // stream_type
                     AUDIO_GetTimeStamp(),                 // timestamp
                     temporalId,                           // temporalId
                     stream_get_handle());                 // pParm

        if ((snapEnable) && (streamType == STREAM_MJPG))
        {
            FILE *ofh1;

            char jpgFilename[64];

            strcpy(jpgFilename, "/mnt/mmc/ipnc/");
            strcat(jpgFilename, snapFileName);
            strcat(jpgFilename, ".jpg");
            OSA_printf("Saving JPG File: %s\n", jpgFilename);
            ofh1 = fopen(jpgFilename, "wb");
            if(ofh1 == NULL)
			return -1;
            fseek(ofh1, 0L, SEEK_SET);
            fwrite(pEncodeBuffer, 1, frameSize, ofh1);
            fclose(ofh1);
            snapEnable = 0;
        }
    }

    return 0;
}

Int App_streamSysExit()
{
    stream_end(stream_get_handle());
    Vsys_freeBuf(VSYS_SR1_SHAREMEM, strmBufInfo.virtAddr, STREAM_BUF_MEM_SIZE);

    OSA_printf("%s Exit Completed \n", __func__);

    return 0;
}

static Void *App_ipcBitsRecvStreamFxn(Void * prm)
{
    App_IpcBitsCtrl *ipcBitsCtrl = (App_IpcBitsCtrl *) prm;

    App_IpcBitsCtrlThrObj *thrObj = &ipcBitsCtrl->thrObj;

    static Int printStats;

    VCODEC_BITSBUF_LIST_S fullBufList;

    UInt32 i, codecType;

#ifdef IPCBITS_DEBUG_PRINT
	OSA_printf("MCFW_IPCBITS:%s:Entered...", __func__);
#endif

    while (FALSE == thrObj->exitBitsInThread)
    {
        OSA_semWait(&thrObj->bitsInNotifySem, OSA_TIMEOUT_FOREVER);

        Venc_getBitstreamBuffer(&fullBufList, 0);

        /* Write the buffers to circular buffer */
        for (i = 0; i < fullBufList.numBufs; i++)
        {	
            if ((fullBufList.bitsBuf[i].codecType == (VCODEC_TYPE_E) IVIDEO_H264BP) ||
                (fullBufList.bitsBuf[i].codecType == (VCODEC_TYPE_E) IVIDEO_H264MP) ||
                (fullBufList.bitsBuf[i].codecType == (VCODEC_TYPE_E) IVIDEO_H264HP))
            {
                codecType = VCODEC_TYPE_H264;
            }
            else if (fullBufList.bitsBuf[i].codecType == (VCODEC_TYPE_E) IVIDEO_MPEG4SP)
            {
                codecType = VCODEC_TYPE_MPEG4;
            }
            else
            {
                codecType = VCODEC_TYPE_MJPEG;
            }

	    	App_streamSysVidWrite(fullBufList.bitsBuf[i].chnId,
                                  codecType,
                                  (void *)((UInt32)fullBufList.bitsBuf[i].bufVirtAddr+fullBufList.bitsBuf[i].startOffset),
                                  fullBufList.bitsBuf[i].filledBufSize,
                                  (fullBufList.bitsBuf[i].frameType ==
                                   VCODEC_FRAME_TYPE_I_FRAME) ? I_FRAME :
                                  P_FRAME, fullBufList.bitsBuf[i].timestamp,
                                  fullBufList.bitsBuf[i].temporalId);
#ifdef DUMP_BITSTREAM
			{
				#define START_CNT		(0)
				#define FRAME_CNT		(100)

				UInt8 *bufAddr;
				Int8 fileName[256];
				static UInt32 chNum;
				UInt8 mpeg4Vol1[] = {0x00,0x00,0x01,0xB0,0x05,0x00,0x00,0x01,
									 0xB5,0x0E,0xCF,0x00,0x00,0x01,0x00,0x00,
									 0x00,0x01,0x20,0x00,0x86,0xe0,0x00,0x33,
									 0x88,0x60,0x1E,0xA4,0x02,0xC5,0x85,0xC8,
									 0x01,0xE5,0x16,0x84,0x3C,0x14,0x63};		// for D1	
									 //0x01,0xE5,0x3C,0x04,0x87,0x14,0x63};		// for 1080p

				if(gCodecChanged != -1)
				{
					chNum         = gCodecChanged;
					gFrCnt        = 0;
					gCodecChanged = -1;
					printf("#### chNum = %d,gCodecType = %d\n",chNum,gCodecType);
				}

				if(fullBufList.bitsBuf[i].chnId == chNum)
				{
					if(gFrCnt == (0 + START_CNT))
					{
						if(gCodecType == VCODEC_TYPE_MJPEG)
							sprintf(fileName,"/mnt/mmc/bitDump_%d.jpg",gDumpCnt++);
						else if(gCodecType == VCODEC_TYPE_MPEG4)
					             sprintf(fileName,"/mnt/mmc/bitDump_%d.mpeg4",gDumpCnt++);
						     else
								 sprintf(fileName,"/mnt/mmc/bitDump_%d.h264",gDumpCnt++);

						gFp = fopen(fileName, "wb");
						if(gFp == NULL)
						{
							OSA_printf("#### File open failed\n");
						}
						else
						{
							OSA_printf("#### File %s created !!!!\n",fileName);
						}
						
						if(gFp && (gCodecType == VCODEC_TYPE_MPEG4))
						{
							fwrite(mpeg4Vol1,1,39,gFp);
						}
					}

					if(gFrCnt < (FRAME_CNT + START_CNT))
					{
						if(gFp && (gCodecType == codecType))
						{
							bufAddr = (UInt8 *)((UInt32)fullBufList.bitsBuf[i].bufVirtAddr + fullBufList.bitsBuf[i].startOffset);
							fwrite(bufAddr,1,fullBufList.bitsBuf[i].filledBufSize,gFp);
						}
					}

					gFrCnt ++;

					if(gFrCnt == (FRAME_CNT + START_CNT))
					{
						if(gFp)
						{
							fclose(gFp);
						}
					}
				}
			}
#endif
        }

        /* relese the buffers back */
        Venc_releaseBitstreamBuffer(&fullBufList);

        Mjpeg_getBitstreamBuffer(&fullBufList, 0);

        /* Write the buffers to circular buffer */
        for (i = 0; i < fullBufList.numBufs; i++)
        {
            App_streamSysVidWrite(fullBufList.bitsBuf[i].chnId,
                                  VCODEC_TYPE_MJPEG,
                                  (void *)((UInt32)fullBufList.bitsBuf[i].bufVirtAddr+fullBufList.bitsBuf[i].startOffset),
                                  fullBufList.bitsBuf[i].filledBufSize,
                                  (fullBufList.bitsBuf[i].frameType ==
                                   VCODEC_FRAME_TYPE_I_FRAME) ? I_FRAME :
                                  P_FRAME, fullBufList.bitsBuf[i].timestamp,
                                  fullBufList.bitsBuf[i].temporalId);
        }

        /* relese the buffers back */
        Mjpeg_releaseBitstreamBuffer(&fullBufList);

        if ((printStats % MCFW_IPCBITS_INFO_PRINT_INTERVAL) == 0)
        {
#ifdef IPCBITS_DEBUG_PRINT
            OSA_printf("MCFW_IPCBITS:%s:INFO: periodic print..", __func__);
#endif
		}
        printStats++;

    }

#ifdef IPCBITS_DEBUG_PRINT
    OSA_printf("MCFW_IPCBITS:%s:Leaving...", __func__);
#endif
	
    thrObj->exitBitsInThreadDone = TRUE;

    return NULL;
}

static Void App_ipcBitsInitThrObj(App_IpcBitsCtrlThrObj * thrObj)
{

    OSA_semCreate(&thrObj->bitsInNotifySem,
                  MCFW_IPCBITS_MAX_PENDING_RECV_SEM_COUNT, 0);

    thrObj->exitBitsInThread = FALSE;
    thrObj->exitBitsOutThread = FALSE;
    thrObj->exitBitsInThreadDone = FALSE;

    OSA_thrCreate(&thrObj->thrHandleBitsIn,
                  App_ipcBitsRecvStreamFxn,
                  MCFW_IPCBITS_RECVFXN_TSK_PRI,
                  MCFW_IPCBITS_RECVFXN_TSK_STACK_SIZE, &gApp_ipcBitsCtrl);
}

static Void App_ipcBitsDeInitThrObj(App_IpcBitsCtrlThrObj * thrObj)
{
    thrObj->exitBitsInThread = TRUE;

    thrObj->exitBitsOutThread = TRUE;

    while(thrObj->exitBitsInThreadDone == FALSE)
    {
	    OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
	}

    OSA_thrDelete(&thrObj->thrHandleBitsIn);

    OSA_semDelete(&thrObj->bitsInNotifySem);
}

Void App_ipcBitsInitSetBitsInNoNotifyMode(Bool noNotifyMode)
{
    gApp_ipcBitsCtrl.noNotifyBitsInHLOS = noNotifyMode;
}

Void App_ipcBitsInitSetBitsOutNoNotifyMode(Bool noNotifyMode)
{
    gApp_ipcBitsCtrl.noNotifyBitsOutHLOS = noNotifyMode;
}

Int32 App_ipcBitsInit()
{
    VENC_CALLBACK_S callback;

    gApp_ipcBitsCtrl.fObj.enableFWrite = FALSE;

    callback.newDataAvailableCb = App_ipcBitsInCbFxn;
    /* Register call back with encoder */
    Venc_registerCallback(&callback, (Ptr) & gApp_ipcBitsCtrl);
	Mjpeg_registerCallback((MJPEG_CALLBACK_S *)&callback, (Ptr) & gApp_ipcBitsCtrl);

    App_ipcBitsInitThrObj(&gApp_ipcBitsCtrl.thrObj);

    return OSA_SOK;
}

Void App_ipcBitsStop(void)
{
    gApp_ipcBitsCtrl.thrObj.exitBitsInThread = TRUE;
    gApp_ipcBitsCtrl.thrObj.exitBitsOutThread = TRUE;
    while(gApp_ipcBitsCtrl.thrObj.exitBitsInThreadDone == FALSE)
    {
	    OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
	}
}

Int32 App_ipcBitsExit()
{
#ifdef IPCBITS_DEBUG_PRINT
    OSA_printf("Entered:%s...", __func__);
#endif

    App_ipcBitsDeInitThrObj(&gApp_ipcBitsCtrl.thrObj);

#ifdef IPCBITS_DEBUG_PRINT
    OSA_printf("Leaving:%s...", __func__);
#endif

	return OSA_SOK;
}

