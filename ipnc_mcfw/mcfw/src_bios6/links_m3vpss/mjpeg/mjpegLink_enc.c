/** ==================================================================
 *  @file  mjpegLink_enc.c
 *
 *  @path   $(IPNC_PATH)/ipnc_mcfw/mcfw/src_bios6/links_m3vpss/mjpeg
 *
 *  @desc   This  File contains the implementation of wrapper function 
 *          for the SIMCOP MJPEG Enc.
 *  =====================================================================
 *  Copyright (c) Texas Instruments Inc 2012, 2011
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied    
 * ===================================================================*/
#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <mcfw/src_bios6/utils/utils_mem.h>
#include <ti/psp/iss/alg/jpeg_enc/inc/msp.h>
#include <ti/psp/iss/alg/jpeg_enc/inc/msp_component.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop_irq.h>
#include <ti/psp/iss/hal/iss/simcop/common/jpeg_tables.h>
#include <ti/psp/iss/alg/jpeg_enc/msp_jpeg_header.h>
#include <ti/psp/iss/alg/jpeg_enc/msp_jpege.h>
#include <ti/psp/iss/alg/jpeg_enc/inc/jpeg_enc.h>
#include <ti/psp/iss/alg/jpeg_enc/inc/msp_jpege_common.h>
#include <ti/psp/vps/fvid2.h>
#include "mjpegLink_enc.h"
#include <ti/psp/iss/alg/rm/inc/rm.h>
#include "mjpegLink_priv.h"

/* =========================================================================== 
 */
/**
 * @fn MSP_JPEGE_init()  Initialize the MSP component for JPEG Encoder.
 *
 * @param [out] handle    : Handle to the instance of MSP JPEGE.
 * @param [in]  eProfile    : Enumeration for profile
 *
 * @return Return         : Error type
 *
 */
/* =========================================================================== 
 */
extern MSP_ERROR_TYPE MSP_JPEGE_init(MSP_HANDLE handle,
                                     MSP_PROFILE_TYPE eProfile);

/* =========================================================================== 
 */
/**
 * @fn MSP_JPEGE_deInit()  Deinitialize the MSP component for JPEG Encoder.
 *
 * @param [in] handle    : Handle to the instance of MSP JPEGE.
 *
 * @return Return         : Error type
 *
 */
/* =========================================================================== 
 */
extern MSP_ERROR_TYPE MSP_JPEGE_deInit(MSP_HANDLE handle);

/* =========================================================================== 
 */
/**
 * @fn MSP_JPEGE_open()  Opens an instance MSP component for JPEG Encoder.
 *
 * @param [in] handle    : Handle to MSP JPEGE.
 * @param [in] pParam  : Pointer to Createtime params of JPEG Encoder instance.
 *
 * @return Return         : Error type
 *
 */
/* =========================================================================== 
 */
extern MSP_ERROR_TYPE MSP_JPEGE_open(MSP_HANDLE handle, MSP_PTR pParam);

/* =========================================================================== 
 */
/**
 * @fn MSP_JPEGE_close()  Closes an instance MSP component for JPEG Encoder.
 *
 * @param [in] handle   : Handle to MSP JPEGE.
 *
 * @return Return         : Error type
 *
 */
/* =========================================================================== 
 */
extern MSP_ERROR_TYPE MSP_JPEGE_close(MSP_HANDLE handle);

/* =========================================================================== 
 */
/**
 * @fn MSP_JPEGE_process()  Processes both input and output buffers for JPEG Encoder.
 *
 * @param [in]  handle     : Handle to MSP JPEGE.
 * @param [in]  pArg        : Argument for buffer processing
 * @param [in]  ptBufHdr  : Buffer pointer of the buffer to be processed
 *
 * @return Return             : Error type
 *
 */
/* =========================================================================== 
 */
extern MSP_ERROR_TYPE MSP_JPEGE_process(MSP_HANDLE handle,
                                        MSP_PTR pArg,
                                        MSP_BUFHEADER_TYPE * ptBufHdr);

/* =========================================================================== 
 */
/**
 * @fn MSP_JPEGE_control()  Processs various control commands given to JPEG Encoder.
 *
 * @param [in]  handle          : Handle to MSP JPEGE.
 * @param [in]  tCmd            : Type of control command for MSP_JPEGE
 * @param [in]  pCmdParam  : Process related parameters which MSP_JPEGE expects.
 *
 * @return Return         : Error type
 *
 */
/* =========================================================================== 
 */
extern MSP_ERROR_TYPE MSP_JPEGE_control(MSP_HANDLE handle,
                                        MSP_CTRLCMD_TYPE tCmd,
                                        MSP_PTR pCmdParam);

extern MSP_ERROR_TYPE MSP_JPEGE_config(MSP_HANDLE handle,
                                       MSP_JPEGE_INDEX_TYPE tConfigIndex,
                                       MSP_PTR pConfigParam);

extern MSP_ERROR_TYPE MSP_JPEGE_query(MSP_HANDLE hMSP,
                                      MSP_JPEGE_INDEX_TYPE tQueryIndex,
                                      MSP_PTR pQueryParam);

typedef struct {
    MSP_COMPONENT_TYPE *hMSP;
    MSP_APPCBPARAM_TYPE tJpege_appcbptr;
    Semaphore_Handle pProcEndSem;
    MSP_JPEGE_CREATE_PARAM stMSPJpegParams;
    MSP_HANDLE hInit;
    MSP_U32 nHeaderSize;
    MSP_HANDLE hAibhHeap;
    MSP_HANDLE hHwHeapHdl;
    MSP_U32 nAlterSeqNum;
    MSP_U32 nOldImgWidth;
    MSP_U32 nOldImgHeight;
    MSP_U32 nImgWHChg;
    MSP_ERROR_TYPE nError;
    MSP_U32 nFDOffset_SysState;
    MSP_U32 nFDOffset_SysStateFrm;
    MSP_U32 nFDOffset_MJPEGOmx;
    MSP_U32 nFDOffset_ImgPrvOmx;
    MSP_U32 nFDOffset_ImgSecOmx;
    MSP_U32 nFDOffset_MeasurementData;
    MSP_BOOL bExifTagsInitialized;
    MSP_U32 nFDOffset_EmbeddedData;
} MSP_APPDATA_JPEGE_INC;

/* ===================================================================
 *  @func     msp_jpege_callback                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE msp_jpege_callback(MSP_PTR hMSP,
                                  MSP_PTR pAppData,
                                  MSP_EVENT_TYPE tEvent,
                                  MSP_OPAQUE nEventData1,
                                  MSP_OPAQUE nEventData2)
{
    MSP_COMPONENT_TYPE *jComponent = (MSP_COMPONENT_TYPE *) pAppData;

    MSP_APPDATA_JPEGE_INC *pCompPrv = jComponent->pCompPrivate;

    switch (tEvent)
    {
        case MSP_ERROR_EVENT:
            pCompPrv->nError = nEventData2;
            break;
        case MSP_PROFILE_EVENT:
            break;
        case MSP_OPEN_EVENT:
            break;
        case MSP_CLOSE_EVENT:
            break;
        case MSP_CTRLCMD_EVENT:
            break;
        case MSP_DATA_EVENT:
            if (nEventData1 == 0)
            {
                /* Input buffer processed */
            }
            if (nEventData1 == 1)
            {
                Semaphore_post(pCompPrv->pProcEndSem);
            }
            break;
    }
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     MSP_JPEGE_INC_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 MSP_JPEGE_INC_init(MSP_COMPONENT_TYPE ** jComponent)
{
    MSP_APPDATA_JPEGE_INC *pCompPrv;

    Semaphore_Params semParams;

    *jComponent = MSP_MEMALLOC(sizeof(MSP_COMPONENT_TYPE));

    (*jComponent)->pCompPrivate = MSP_MEMALLOC(sizeof(MSP_APPDATA_JPEGE_INC));
    if (NULL == (*jComponent)->pCompPrivate)
    {
        Vps_printf("Error allocating pComponent->pCompPrivate.");
        return MSP_ERROR_NOTENOUGHRESOURCES;
    }

    pCompPrv = (*jComponent)->pCompPrivate;

    pCompPrv->tJpege_appcbptr.pAppData = (*jComponent);
    pCompPrv->tJpege_appcbptr.MSP_callback = msp_jpege_callback;
    pCompPrv->nOldImgWidth = 0;
    pCompPrv->nOldImgHeight = 0;
    pCompPrv->nImgWHChg = 0;
    pCompPrv->nAlterSeqNum = 0;
    pCompPrv->bExifTagsInitialized = MSP_FALSE;

    Semaphore_Params_init(&semParams);
    pCompPrv->pProcEndSem = Semaphore_create(0, &semParams, NULL);

    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     MSP_JPEGE_INC_deInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 MSP_JPEGE_INC_deInit(MSP_COMPONENT_TYPE * jComponent)
{
    MSP_APPDATA_JPEGE_INC *pCompPrv = jComponent->pCompPrivate;

    MSP_ERROR_TYPE result = MSP_ERROR_NONE;

    Semaphore_delete(&pCompPrv->pProcEndSem);
    MSP_MEMFREE(jComponent->pCompPrivate);
    MSP_MEMFREE(jComponent);
    return result;
}

/* ===================================================================
 *  @func     McFW_JPEG_Enc                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int McFW_JPEG_Enc(MSP_COMPONENT_TYPE * jComponent,
                  FVID2_Frame * inputImageBuffer,
                  unsigned char *outputBitstreamBuffer,
                  unsigned int uInputImageWidth,
                  unsigned int uInputImageHeight,
                  unsigned int uInputPitch,
                  int startX, int startY,
                  unsigned char qualityFactor, unsigned int outBufSize, unsigned int *paddingBytes, unsigned int isChromaFormat420)
{
    MSP_BUFHEADER_TYPE inBuffJpege;

    MSP_BUFHEADER_TYPE outBuffJpege;

    MSP_APPDATA_JPEGE_INC *pCompPrv = jComponent->pCompPrivate;

    MSP_U8 *pBuffer;

    TIMM_OSAL_PTR pInterchangeFmt = NULL;

    MSP_U32 ulImgOffs;

    MSP_U32 ulImgOffsC;

    MSP_U8 ucQuantTab[DQT_ENTRIES];

    MSP_ERROR_TYPE eError = MSP_ERROR_NONE;

    TI_JFIF_PARAMS_SUPPORTED sInterchangeFmtJfif;

    TI_JFIF_PARAMS_SUPPORTED *pInterchangeFmtJfif = &sInterchangeFmtJfif;

#ifdef SYSTEM_DEBUG_MJPEGG
    int startTime, endTime;
#endif
    int size = 0;

    unsigned int uInputImageWidth_mul8;

    unsigned int uInputImageWidth_max;

    pCompPrv->stMSPJpegParams.ulOutputChunkSize = outBufSize;

    uInputImageWidth_mul8 = (uInputImageWidth / (8 * 16)) * (8 * 16);

    uInputImageWidth_max = 0;
    if (uInputImageWidth_max < uInputImageWidth_mul8)
        uInputImageWidth_max = uInputImageWidth_mul8;

    uInputImageWidth = uInputImageWidth_max;

    memset(pInterchangeFmtJfif, 0, sizeof(*pInterchangeFmtJfif));

    /* Calc offset from the beginning of the image in pixels */
    ulImgOffs = 0;
    /* Chroma part offset has twice less lines. */
    ulImgOffsC = 0;

    if (inputImageBuffer == 0 || outputBitstreamBuffer == 0)
    {
        size = 0;
        goto EXIT_0;
    }


    /* Calc offset from the beginning of the image in pixels */
    ulImgOffs = 0;

    inBuffJpege.pBuf[0] =
        (MSP_U8 *) ((unsigned int) inputImageBuffer->addr[0][0] +
                    startY * uInputPitch);
    inBuffJpege.pBuf[0] += ulImgOffs;
    if(isChromaFormat420)
      inBuffJpege.unBufSize[0] =
        uInputImageWidth * (((uInputImageHeight + 15) / 16) * 16);
    else
        inBuffJpege.unBufSize[0] =
          2*uInputImageWidth * (((uInputImageHeight + 15) / 16) * 16);
     
    inBuffJpege.unBufOffset[0] = startX;

    inBuffJpege.pBuf[1] =
        (MSP_U8 *) ((unsigned int) inputImageBuffer->addr[0][1] +
                    (startY >> 1) * uInputPitch);
    inBuffJpege.pBuf[1] += ulImgOffsC;
    inBuffJpege.unBufSize[1] =
        (((uInputImageHeight + 15) / 16) * 16) * (uInputImageWidth >> 1);
    inBuffJpege.unBufOffset[1] = startX;

    inBuffJpege.nPortIndex = MSP_JPEGE_INPUT_PORT;

    pCompPrv->stMSPJpegParams.bDRIEnable = MSP_FALSE;
    pCompPrv->stMSPJpegParams.ulRestartInterval = 0;

    pCompPrv->nError = MSP_ERROR_NONE;


    memset(pInterchangeFmtJfif, 0, sizeof(*pInterchangeFmtJfif));

    pInterchangeFmt = pInterchangeFmtJfif;
    pCompPrv->stMSPJpegParams.eHeaderFormat = MSP_JPEGE_JFIF;

    pInterchangeFmtJfif->bThumbnailPresent = MSP_FALSE;
    pInterchangeFmtJfif->eThumbnailFormat = JFIF_THUMBNAIL_JPEG;
    pInterchangeFmtJfif->ucThumbWidth = 0;
    pInterchangeFmtJfif->ucThumbHeight = 0;
    pInterchangeFmtJfif->usThumbnailSize = 0;
    pInterchangeFmtJfif->pThumbnail = NULL;

    pInterchangeFmtJfif->ulOffsetToStartOfEncodedStream =
        TI_GetEncodeByteOffset(pCompPrv->stMSPJpegParams.eHeaderFormat,
                               pInterchangeFmtJfif->bThumbnailPresent,
                               pInterchangeFmtJfif->usThumbnailSize, MSP_FALSE,
                               MSP_FALSE);
    pInterchangeFmtJfif->usJFIFSize =
        pInterchangeFmtJfif->ulOffsetToStartOfEncodedStream;
    pCompPrv->nHeaderSize = pInterchangeFmtJfif->ulOffsetToStartOfEncodedStream;

    GOTO_EXIT_IF(pCompPrv->nHeaderSize >
                 pCompPrv->stMSPJpegParams.ulOutputChunkSize, 3);
    pCompPrv->stMSPJpegParams.ulOutputChunkSize -= pCompPrv->nHeaderSize;

    if(pCompPrv->nHeaderSize & 0xf) 
    {
        *paddingBytes = 16 - (pCompPrv->nHeaderSize & 0xf);
        memset(outputBitstreamBuffer, 0x80 , *paddingBytes);
        size += *paddingBytes;

        outputBitstreamBuffer = outputBitstreamBuffer + *paddingBytes;
        pBuffer = outputBitstreamBuffer;
    }
    else
    {
        pBuffer = outputBitstreamBuffer;
    }

    
#ifdef SYSTEM_DEBUG_MJPEGG
    startTime = Clock_getTicks();
#endif                                                     /* SYSTEM_DEBUG_MJPEG 
                                                            */

    eError =
        MSP_init((MSP_HANDLE *) & pCompPrv->hMSP, "MSP.JPEGE", MSP_PROFILE_HOST,
                 &(pCompPrv->tJpege_appcbptr));
    if (MSP_ERROR_NONE != eError)
    {
        Vps_rprintf
            (" MJPEG Encode :RM Acquire failed and Image would be skipped\n");
        size = 0;
        goto EXIT_0;
    }

    MSP_JPEGE_CLIENT_HANDLETYPE *pMspJpegHandle;

    JPEGParams *pJpegParams;

    pMspJpegHandle =
        (MSP_JPEGE_CLIENT_HANDLETYPE
         *) ((MSP_COMPONENT_TYPE *) (pCompPrv->hMSP)->pCompPrivate);
    pJpegParams = pMspJpegHandle->pJpegParams;
    pJpegParams->ulSliceHt = uInputImageHeight;

    pCompPrv->stMSPJpegParams.ulImageWidth = uInputImageWidth;
    pCompPrv->stMSPJpegParams.ulImageHeight = uInputImageHeight;
	if (isChromaFormat420)
    	pCompPrv->stMSPJpegParams.ulImageStrideHorizontal = uInputPitch;    /* Pitch */
	else
		pCompPrv->stMSPJpegParams.ulImageStrideHorizontal = uInputPitch/2;
	
    pCompPrv->stMSPJpegParams.ulInputSliceHeight = uInputImageHeight;
    pCompPrv->stMSPJpegParams.ulQualityFactor = qualityFactor;
    pCompPrv->stMSPJpegParams.ulBurstLength = 0;
    pCompPrv->stMSPJpegParams.ulOutputChunkSize = outBufSize;

    if(isChromaFormat420)
	  pCompPrv->stMSPJpegParams.eColorFormat = MSP_JPEGE_COLORFORMAT_YUV420NV12;
	else
	  pCompPrv->stMSPJpegParams.eColorFormat = MSP_JPEGE_COLORFORMAT_YUV422INTERLEAVED;

    pCompPrv->stMSPJpegParams.eUseCaseType = MSP_JPEGE_HQ;
    pCompPrv->stMSPJpegParams.eUnComprsdOpMode = MSP_JPEGE_FRAME_MODE;
    pCompPrv->stMSPJpegParams.eComprsdOpMode = MSP_JPEGE_NONCHUNK_MODE;
    pCompPrv->stMSPJpegParams.eRotationParam = MSP_JPEGE_ROTATION_UNUSED;
    pCompPrv->stMSPJpegParams.bCustomHuffTab = MSP_FALSE;
    pCompPrv->stMSPJpegParams.bCustomQuantTab = MSP_FALSE;
    pCompPrv->stMSPJpegParams.eHeaderFormat = MSP_JPEGE_JFIF;
    pCompPrv->stMSPJpegParams.bDRIEnable = MSP_FALSE;
    pCompPrv->stMSPJpegParams.ulRestartInterval = 0;
    pCompPrv->stMSPJpegParams.nSdmaChannel = 1;
    pCompPrv->stMSPJpegParams.bApp13MarkerEnable = MSP_FALSE;

    pCompPrv->stMSPJpegParams.ulInputSliceHeight = 0;
    pCompPrv->stMSPJpegParams.ulBurstLength = 0;
    pCompPrv->stMSPJpegParams.ulRestartInterval = 0;
    pCompPrv->stMSPJpegParams.bDRIEnable = MSP_FALSE;

    eError =
        MSP_JPEGE_open((MSP_HANDLE) pCompPrv->hMSP,
                       (MSP_PTR *) & (pCompPrv->stMSPJpegParams));
    GOTO_EXIT_IF((MSP_ERROR_NONE != eError), 4);

    /* get the default QuantTable used with in the Algo */
    MSP_JPEGE_query((MSP_HANDLE) pCompPrv->hMSP, MSP_JPEGE_INDEX_QUANTTABLE,
                    (MSP_PTR) ucQuantTab);

    MSP_JPEGE_query((MSP_HANDLE) pCompPrv->hMSP, MSP_JPEGE_INDEX_RESTARTITVL,
                    (MSP_PTR) & (pCompPrv->stMSPJpegParams.ulRestartInterval));
    pCompPrv->stMSPJpegParams.bDRIEnable = MSP_TRUE;

    eError =
        TI_WriteJpegHeader(pBuffer, &(pCompPrv->stMSPJpegParams),
                           pInterchangeFmt, NULL, ucQuantTab, NULL);
    GOTO_EXIT_IF(MSP_ERROR_NONE != eError, 4);

    outBuffJpege.pBuf[0] =
        (MSP_U8 *) ((MSP_U32) outputBitstreamBuffer + pCompPrv->nHeaderSize);

    outBuffJpege.unMaxBufSize[0] =
        pCompPrv->stMSPJpegParams.ulOutputChunkSize - pCompPrv->nHeaderSize;
    outBuffJpege.unBufSize[0] = 0;
    outBuffJpege.pAppBufPrivate = (MSP_PTR) & pCompPrv->nHeaderSize;
    outBuffJpege.nPortIndex = MSP_JPEGE_OUTPUT_PORT;

    outBuffJpege.pBuf[1] = (MSP_U8 *) NULL;
    outBuffJpege.unMaxBufSize[1] = 0;
    outBuffJpege.unBufSize[1] = 0;
    outBuffJpege.pBuf[2] = (MSP_U8 *) NULL;
    outBuffJpege.unBufSize[2] = 0;
    outBuffJpege.unNumOfCompBufs = 0;

    inBuffJpege.unNumOfCompBufs = 0;
    inBuffJpege.unBufStride[0] = 0;

    GOTO_EXIT_IF(MSP_ERROR_NONE !=
                 MSP_JPEGE_process((MSP_HANDLE) pCompPrv->hMSP, NULL,
                                   &inBuffJpege), 4);

    GOTO_EXIT_IF(MSP_ERROR_NONE !=
                 MSP_JPEGE_process((MSP_HANDLE) pCompPrv->hMSP, NULL,
                                   &outBuffJpege), 4);

    if (Semaphore_pend(pCompPrv->pProcEndSem, 80) == FALSE)
    {
        Semaphore_Params semParams;

        Vps_rprintf("JPEG encoder main image timeout \r\n");
        size = 0;
        MSP_JPEGE_close((MSP_HANDLE) pCompPrv->hMSP);
        MSP_deInit((MSP_HANDLE) pCompPrv->hMSP);
        Semaphore_delete(&pCompPrv->pProcEndSem);
        Semaphore_Params_init(&semParams);
        pCompPrv->pProcEndSem = Semaphore_create(0, &semParams, NULL);
        goto EXIT_0;
    }

    /* Clear flag for alternate dimensions */
    pCompPrv->nImgWHChg = 0;

#ifdef SYSTEM_DEBUG_MJPEGG
    {
        endTime = Clock_getTicks();
        eError = startTime - endTime;
        Vps_rprintf
            (" SIMCOP MJPEG: Encoding took %d ticks and bytes of %d ... \n",
             endTime - startTime,
             ((outBuffJpege).unBufSize)[0] + pCompPrv->nHeaderSize + 2);
        /* Vps_rprintf(" SIMCOP MJPEG: Encoded Bytes %d and Header Bytes %d
         * ... \n",((outBuffJpege).unBufSize)[0] , pCompPrv->nHeaderSize+2); */
    }
#endif                                                     /* SYSTEM_DEBUG_MJPEG 
                                                            */

    MSP_JPEGE_close((MSP_HANDLE) pCompPrv->hMSP);
    MSP_deInit((MSP_HANDLE) pCompPrv->hMSP);

    eError = MSP_ERROR_NONE;

    TI_WriteJpegFooter(outBuffJpege.pBuf[0] + outBuffJpege.unBufSize[0]);
    pCompPrv->nHeaderSize += 2;

    size = ((outBuffJpege).unBufSize)[0] + pCompPrv->nHeaderSize;
    if (size <= 0)
        size = 0;

  EXIT_4:
  EXIT_3:
  EXIT_0:
    return size;
}
