/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup ALG_LINK_API ALG Link API

    ALG Link can be used to apply some algorithms <OSD, SCD,..> on channel data

    @{
*/

/**
    \file ALGLink.h
    \brief ALG Link API
*/

#ifndef _ALG_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALG_LINK_H_

#include <mcfw/interfaces/link_api/system.h>

#define ALG_LINK_OSD_MAX_WINDOWS (10)
#define ALG_LINK_OSD_MAX_CH  (48)
#define ALG_LINK_SCD_MAX_CH  (16)

/** 
  \brief Max ALG outputs/output queues 

  Currently ALG link has one output Link for SCD output. 
*/
#define ALG_LINK_MAX_OUT_QUE  (1)

#define ALG_BUFFER_ALIGNMENT (32)

/**
    \brief Link CMD: Configure Osd params
*/

#define ALG_LINK_OSD_CMD_SET_CHANNEL_WIN_PRM      (0x6001)
/**
  \brief Graphics window parameter's
*/
typedef struct {
  UInt8  *addr[2][2];    
  /**< Y/C buf Ptr/yuv422 interleaved ptr */

  UInt32 format;      
  /**< SWOSD_FORMAT_YUV422i or SWOSD_FORMAT_RGB565 */
  
  UInt32 startX;      
  /**< StartX in pixels, relative to start of video window, must be multiple of 2 */
  UInt32 startY;      
  /**< StartY in lines, relative to start of video window */
  
  UInt32 width;       
  /**< Width in pixels, must be multiple of 4  */
  UInt32 height;      
  /**< Height in lines  */
  
  UInt32 lineOffset;  
  /**< Line offset in pixels, must be >= width, must be multiple of 4, 
  recommended to be multiple of 32 for efficiency */

  UInt32 globalAlpha;         
  /**< 8-bit global Alpha Value, used only if Alpha window is not enabled, 
  set to 0 to disable alpha blend \n 0: Min Alpha, show only video, 128: Max Alpha, show only Graphics */
  UInt32 transperencyEnable;  
  /**< TRUE: enable transperency, FALSE: disable transperency */
  UInt32 enableWin;
    /**< Enable/Disable Window */

} AlgLink_OsdWindowPrm;


/**
    \brief Osd link channel dynamic set config params

    Defines osd parameters that can be changed dynamically
    on a per channel basis for the osd link
*/
typedef struct AlgLink_OsdChWinParams
{
    UInt32 chId;
    /**< OSD channel number */

    UInt32 numWindows;
    /**< Number of OSD windows per channel */

    AlgLink_OsdWindowPrm winPrm[ALG_LINK_OSD_MAX_WINDOWS];
    /**< Modified encoder bit rate value in bits/sec format */
} AlgLink_OsdChWinParams;



/**
    \brief OSD channel create time configuration parameters
*/
typedef struct
{
    UInt32 maxWidth;
    /**< Set the max width of osd graphics window */
    UInt32 maxHeight;
    /**< Set the max height of osd graphics window */
    AlgLink_OsdChWinParams chDefaultParams;
    /**< run time params for all channels. */
} AlgLink_OsdChCreateParams;

/**
    \brief Link CMD: Configure Scd params
*/
#define ALG_LINK_SCD_CMD_SET_CHANNEL_IGNORELIGHTSON    (0x7001)
#define ALG_LINK_SCD_CMD_SET_CHANNEL_IGNORELIGHTSOFF   (0x7002)
#define ALG_LINK_SCD_CMD_SET_CHANNEL_MODE              (0x7003)
#define ALG_LINK_SCD_CMD_SET_CHANNEL_SENSITIVITY       (0x7004)
#define ALG_LINK_SCD_CMD_PRINT_STATISTICS              (0x7005)
#define ALG_LINK_SCD_CMD_SET_CHANNEL_BLOCKCONFIG       (0x7006)

#define ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME			   (11 * 24)  // CIF dimensions expected, e.g. (352 / 32) * (240 / 10) Or (352 / 32) * (288 / 12)
#define ALG_LINK_SCD_THRESHOLD_NUM_SEC_POST_RESET		5

#define ALG_LINK_SCD_MAX_BUF_ALLOC_POOLS               (1)



typedef enum {
	ALG_LINK_SCD_SENSITIVITY_VERYLOW	 =  0,
	ALG_LINK_SCD_SENSITIVITY_LOW		 =  1,
	ALG_LINK_SCD_SENSITIVITY_MIDLO	 =  2,
	ALG_LINK_SCD_SENSITIVITY_MID		 =  3,
	ALG_LINK_SCD_SENSITIVITY_MIDHI	 =  4,
	ALG_LINK_SCD_SENSITIVITY_HIGH	 =  5,
	ALG_LINK_SCD_SENSITIVITY_VERYHIGH =  6
} AlgLink_ScdSensitivity;

/*
typedef enum {
    ALG_LINK_SCD_SENSITIVITY_LOW	 =  0,
    ALG_LINK_SCD_SENSITIVITY_MIDLO	 =  1,
    ALG_LINK_SCD_SENSITIVITY_MID	 =  2,
    ALG_LINK_SCD_SENSITIVITY_MIDHI	 =  3,
    ALG_LINK_SCD_SENSITIVITY_HIGH	 =  4
} AlgLink_ScdSensitivity;
*/
typedef enum
{
    ALG_LINK_SCD_DETECTMODE_DISABLE				 = 0,
    ALG_LINK_SCD_DETECTMODE_MONITOR_FULL_FRAME	 = 1,
    ALG_LINK_SCD_DETECTMODE_MONITOR_BLOCKS		 = 2,
    ALG_LINK_SCD_DETECTMODE_MONITOR_BLOCKS_AND_FRAME = 3
} AlgLink_ScdMode;

typedef enum
{
    ALG_LINK_SCD_DETECTOR_UNAVAILABLE =-1,
    ALG_LINK_SCD_DETECTOR_NO_CHANGE	  = 0,
    ALG_LINK_SCD_DETECTOR_CHANGE      = 1
} AlgLink_ScdOutput;

/**
    \brief SCD link channel block config parameters. 
    Defines SCD parameters that can be changed dynamically
    on a per channel basis for the SCD link
*/
typedef struct AlgLink_ScdChBlkConfig
{
   UInt32               blockId;           
   /**< Block ID/no. in a frame. This is a sequential block ID/no. in a frame. 
   It can take values from 0 - (SCD_LINK_MAX_BLOCKS_IN_FRAME - 1 ) */

   AlgLink_ScdSensitivity  sensitivity;       
   /**< SCD sensitivity used for detecting change within a block */
   
   UInt32               monitorBlock;
   /**< Enable/disable monitoring of change within a block */
} AlgLink_ScdChBlkConfig;

/**
    \brief Scd link channel block config parameters structure used to change
    block configuration parameter dynamically
    on a per channel basis for the SCD link
*/
typedef struct AlgLink_ScdChblkUpdate
{
    UInt32                 chId;
    /**< Number (32-bit) used to uniquely identify SCD channel */

    UInt32                 numValidBlock;                             
    /**<No. of blocks to be updated. Valid entires in the blkConfig array.*/
    
    AlgLink_ScdChBlkConfig blkConfig[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];   
    /**< Block config data specified in ScdLink_ChBlkConfig structure. 
    Specifiy numbers of block Id/no. to be updated and its configuration parameter. 
    Vaild elements: 0 - (numValidBlock - 1). */
}AlgLink_ScdChblkUpdate;

/**
    \brief Scd link channel parameters used to SET BEFORE CALLING PROCESS

    Defines SCD parameters that can be changed dynamically
    on a per channel basis for the scd link
*/
typedef struct AlgLink_ScdChFrameParams
{
    UInt32 chId;
    /**< Number (32-bit) used to uniquely identify SCD channel */
    
    UInt32 mode;
    /**< Enable flag to make scd act on selective channels <CIF> 
         Set to 0 to disable all scd processing on channel
         Set to 1 to enable change detection on entire frame (only)
         Set to 2 to enable change detection on frame tiles/blocks (only)
         Set to 3 to enable change detection on entire frame as well 
                  as individual frame tiles/blocks */
    
    UInt32 frmIgnoreLightsON;
    /**< Set to 1 to ignore sudden, significantly brighter avg. scene luminance */

    UInt32 frmIgnoreLightsOFF;
    /**< Set to 1 to ignore sudden, significantly darker avg. scene luminance */

    AlgLink_ScdSensitivity frmSensitivity;
    /**< internal threshold for scd change tolerance; globally applied to entire frame */
        
    UInt32 frmEdgeThreshold;
    /**< Set minimum number of edge pixels required to indicate non-tamper event 
         At the moment, this variable has not been completely defined
    */
    UInt32 blkNumBlksInFrame;
    /**< Set number block/tiles available in a frame.
    */

} AlgLink_ScdChFrameParams;


/**
    \brief Scd link channel metadata

    Defines block metadata that is provided after scd link
    has been processed.

    Note: Current block width is fixed at 32 pixels. Block 
    height varies between 10 and 12.
*/
typedef struct
{
    UInt32 numFrmsBlkChanged;
    /**< Number of consecutive frames with motion in block*/

    UInt32 numPixelsChanged;
    /**< Raw number of block pixels to change*/
} AlgLink_ScdblkChngMeta;


/**
    \brief Scd link channel dynamic set config params

    Defines scd parameters that can be changed dynamically
    on a per channel basis for the scd link
*/
typedef struct
{
    AlgLink_ScdSensitivity sensitivity;
    /**< Blocks's sensitivity setting for change detection */

    UInt32 monitored;
    /**< Flag indicates whether to monitor block for change detection*/
} AlgLink_ScdblkChngConfig;

/**
    \brief Scd channel control

    Defines variables to consume SCD-generated metadata to 
	determine if tamper condition exist as well as if change
	occurred within block    
*/
typedef struct
{
    UInt32 size;
    /**< Size of this struct */

    UInt32 chId;
    /**< SCD channel number */

    UInt32 frmResult;
    /**< SCD change detection result from entire frame */

    UInt32 edgeThreshold;
    /**< Minimum number of edges that must be present in a frame that has not
	     been tampered with, e.g., */

    UInt32 bsubThreshold;
    /**< Minimum number of edges that must be present in a frame that has not
	     been tampered with, e.g., */

	UInt32 numFrmsWithNoChange;
    /**< Counts the number of consecutive frames that the scene is stable, e.g.,
	     little to no change detected */

	UInt32 isSceneChanged;
    /**< Counts the number of consecutive frames that the scene appears to be
		 different than the background model*/

    UInt32 numSecs2WaitB4Init;
    /**< Set the number of seconds to wait before initializing SCD monitoring. 
         This wait time occurs when the algorithm instance first starts and 
         allows video input to stabilize.*/

    UInt32 numSecs2WaitB4FrmAlert;
    /**< Set the number of seconds to wait before signaling a frame-level scene change event.*/

    AlgLink_ScdblkChngMeta blkResult[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];
    /**< SCD change detection result from individual frame tiles/blocks */
} AlgLink_ScdChStatus;

/**
    \brief Scd link algorithm output 

    Defines variable that SCD algorithm will update.
    Generated for every frame.
*/
typedef struct AlgLink_ScdResult
{
    UInt32 chId;
    /**< SCD channel number */

	UInt32 frmAvgModelPixelVal;
    /**< SCD Metadata - average 8-bit luma pixel value in the adaptive 
	     background model */

	UInt32 frmAvgCurFrmPixelVal;
    /**< SCD Metadata - average 8-bit luma pixel value in the current frame */

	UInt32 frmPercentArea_BSUB;
    /**< SCD Metadata - percentage of luma pixels in the current frame that are
	     "different" than those in the background model. The difference is 
		 determined by subtracting the current frame from the background
		 model and thresholding the result, i.e. background subtraction (BSUB). */

	UInt32 frmPercentArea_IFD;
    /**< SCD Metadata - percentage of luma pixels in the current frame that are
	     "different" than those in the prior frame. The difference is 
		 determined by subtracting the current frame from the prior
		 luma frame and thresholding the result, i.e., inter-frame diff (IFD). */

	UInt32 frmNumEdges;
    /**< SCD Metadata - an approximation of the number of pixels with gradient 
	      magnitude (edge) larger than the edge threshold */

    AlgLink_ScdOutput       frmResult;
    /**< SCD change detection result from entire frame */

    AlgLink_ScdblkChngMeta  blkResult[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];   
    /**< SCD change detection result from individual frame tiles/blocks 
         array length = ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME */
} AlgLink_ScdResult;

/**
    \brief Scd link channel parameters used to SET BEFORE CALLING PROCESS

    Defines scd parameters that can be changed dynamically
    on a per channel basis for the scd link
*/
typedef struct AlgLink_ScdChParams
{
    UInt32 chId;
    /**< Number (32-bit) used to uniquely identify SCD channel */
    
    UInt32 mode;
    /**< Enable flag to make scd act on selective channels <CIF> 
         Set to 0 to disable all scd processing on channel
         Set to 1 to enable change detection on entire frame (only)
         Set to 2 to enable change detection on frame tiles/blocks (only)
         Set to 3 to enable change detection on entire frame as well 
                  as individual frame tiles/blocks */
    
    UInt32 frmIgnoreLightsON;
    /**< Set to 1 to ignore sudden, significantly brighter avg. scene luminance */

    UInt32 frmIgnoreLightsOFF;
    /**< Set to 1 to ignore sudden, significantly darker avg. scene luminance */

    AlgLink_ScdSensitivity frmSensitivity;
    /**< internal threshold for scd change tolerance; globally applied to entire frame */
        
    UInt32 frmEdgeThreshold;
    /**< Set minimum number of edge pixels required to indicate non-tamper event 
         At the moment, this variable has not been completely defined
    */
    UInt32 blkNumBlksInFrame;
    /**< Set number block/tiles available in a frame.
    */

    AlgLink_ScdblkChngConfig blkConfig[ALG_LINK_SCD_MAX_BLOCKS_IN_FRAME];
    /**< Array of block-wise change detection parameters for each channel. 
         Set this pointer to a valid, allocated array to set the block durin set-up;
         Otherwise, set to NULL to preserve the existing configuration arrangement, e.g.,
         you only need to set this as required to change the sensitivity or monitor state */
} AlgLink_ScdChParams;

/**
    \brief SCD link create parameters
*/
typedef struct
{
    UInt32 maxWidth;
    /**< Set the maximum width (in pixels) of video frame that scd will process */

    UInt32 maxHeight;
    /**< Set the maximum height (in pixels) of video frame that scd will process */

    UInt32 maxStride;
    /**< Set the maximum video frame pitch/stride of the images in the video buffer*/

   	UInt32 numCh;
    /**< Set the maximum number of video channels that SCD will monitor (Max is 16) */

    UInt32 numSecs2WaitB4Init;
    /**< Set the number of seconds to wait before initializing SCD monitoring. 
         This wait time occurs when the algorithm instance first starts and 
         allows video input to stabilize.*/

    UInt32 numSecs2WaitB4FrmAlert;
    /**< Set the number of seconds to wait before signaling a frame-level scene change event.*/

    UInt32 inputFrameRate;
    /**< Frames per second fed to the scd link*/

    UInt32 outputFrameRate;
    /**< Frames per second alogorithm is operated at*/

    UInt32 scdChEnableFlag[ALG_LINK_SCD_MAX_CH]; 
    /**< Set flag to install scd on selective channels <CIF resolution> */

    AlgLink_ScdChParams chDefaultParams[ALG_LINK_SCD_MAX_CH];  
    /**< Pointer to array of channel params used to configure ScdAlgLink. */

    UInt32 numBufPerCh;
    /**< Number of buffer to allocate per channel*/

    UInt32 numValidChForSCD;
    /**< Set the maximum channels on which scd will run. 
          startChNoForSCD + numValidChForSCD should be less than MAX Input channels 
          Still all parameter settings for SCD will be for Ch Num 0 ~ numValidChForSCD.
          startChNoForSCD, numValidChForSCD are used only to separate valid channels from an input queue having
          other channels which dont require SCD 
         */
} AlgLink_ScdCreateParams;


/**
    \brief OSD link create parameters
*/
typedef struct
{
    Int32   enableOSDAlg;     
    /**< Flag to enable OSD Algorithm */
    
    Int32   enableSCDAlg;     
    /**< Flag to enable SCD Algorithm */

    System_LinkInQueParams  inQueParams;     
    /**< Input queue information. */
    
    System_LinkOutQueParams outQueParams;    
    /**< Output queue information. */

    AlgLink_OsdChCreateParams  osdChCreateParams[ALG_LINK_OSD_MAX_CH];     
    /**< create time OSD params for all channels. */

    AlgLink_ScdCreateParams    scdCreateParams;     
    /**< create time SCD params for all channels. */
} AlgLink_CreateParams;


/**
    \brief Alg link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 AlgLink_init();

/**
    \brief Alg link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 AlgLink_deInit();


static inline void AlgLink_CreateParams_Init(AlgLink_CreateParams *pPrm)
{
    Int32					numBlksInFrame, x, y, i, numHorzBlks, numVertBlks, chId;

    memset(pPrm, 0, sizeof(*pPrm));

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->outQueParams.nextLink  = SYSTEM_LINK_ID_INVALID;

    pPrm->enableOSDAlg = pPrm->enableSCDAlg = FALSE;

    pPrm->scdCreateParams.maxWidth				= 352;
    pPrm->scdCreateParams.maxHeight				= 288;
    pPrm->scdCreateParams.maxStride				= 704;
    pPrm->scdCreateParams.numValidChForSCD      = 0;//1;

    pPrm->scdCreateParams.numSecs2WaitB4Init	= 3;
    pPrm->scdCreateParams.numSecs2WaitB4FrmAlert= 1;
    pPrm->scdCreateParams.inputFrameRate        = 30;
    pPrm->scdCreateParams.outputFrameRate       = 5;

    numHorzBlks		= pPrm->scdCreateParams.maxWidth / 32;
    numVertBlks		= pPrm->scdCreateParams.maxHeight / 12;
    numBlksInFrame	= numHorzBlks * numVertBlks;

    for(chId = 0; chId<ALG_LINK_SCD_MAX_CH; chId++)
    {
        pPrm->scdCreateParams.chDefaultParams[chId].chId = chId;
        pPrm->scdCreateParams.chDefaultParams[chId].mode				= ALG_LINK_SCD_DETECTMODE_MONITOR_BLOCKS_AND_FRAME;
        pPrm->scdCreateParams.chDefaultParams[chId].frmIgnoreLightsON	= FALSE;
        pPrm->scdCreateParams.chDefaultParams[chId].frmIgnoreLightsOFF	= FALSE;
        pPrm->scdCreateParams.chDefaultParams[chId].frmSensitivity		= ALG_LINK_SCD_SENSITIVITY_MID;
        pPrm->scdCreateParams.chDefaultParams[chId].frmEdgeThreshold	= 100;
        pPrm->scdCreateParams.chDefaultParams[chId].blkNumBlksInFrame	= numBlksInFrame;

        /* Configure array to monitor scene changes in all frame blocks, i.e., motion detection. 
         Each block is fixed to be 32x12 in size */ 

        i = 0;
        for(y = 0; y < numVertBlks; y++)
        {
            for(x = 0; x < numHorzBlks; x++)
            {
                pPrm->scdCreateParams.chDefaultParams[chId].blkConfig[i].sensitivity = ALG_LINK_SCD_SENSITIVITY_MID;
                pPrm->scdCreateParams.chDefaultParams[chId].blkConfig[i].monitored	 = 1;
                i++;
            }
        }
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/*@}*/

