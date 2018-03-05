/** ==================================================================
 *  @file   ispLink.h
 *
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/
 *
 *  @desc   This  File contains Isp Link API.
 *
 * ===================================================================
 *
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 *
 * ===================================================================*/

/**
    \ingroup LINK_API
    \defgroup ISP_LINK_API Isp Link API

    ISP link does M2M (Memory To Memory) Image Signal Processing of RAW BAYER/YUV data to generate
    upto maximum of ISPLINK_MAX_OUT_QUE streams.
	If input data format is RAW BAYER then IPIPE and RSZ processings are applied.
	If input data format is YUV then only RSZ processing is applied.
	User should tune clkDivM and clkDivN values to make ISP work depending on the DDR load
	applied based on the use case.

    @{
*/

/**
    \file ispLink.h
    \brief Isp Link API
*/

#ifndef _ISP_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISP_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief Max output queues in the isp link */
#define ISPLINK_MAX_OUT_QUE          	(4)

/** \brief Commands handled by the ISP link */
#define ISPLINK_CMD_CHANGE_FRAMERATE	(0x9000)
#define ISPLINK_CMD_MIRROR_MODE			(0x9001)
#define ISPLINK_CMD_SET_OUTDATAFMT		(0x9002)

/**
    \brief Isp link Frames Per Second params

    Defines Isp FPS parameters that can be changed dynamically
    on a per stream basis for the Isp link
*/
typedef struct {
    UInt32 sensorFps;
	/**< Sensor FPS */

    UInt32 streamId;
    /**< Stream Id */

    UInt32 targetFps;
    /**< Modified Frames per second (FPS) value */

} IspLink_FpsParams;

/**
    \brief Isp link Output Data format paramters

    Defines Isp Data Format for the output stream
*/
typedef struct {
    UInt32 streamId;
    /**< Stream Id */

    System_VideoDataFormat dataFmt;
    /**< Data format*/
	
	UInt32 isVnfON;
	/**< Flag to indicate if VNF is in Data path */	
} IspLink_outputDataFormat;

/**
    \brief Isp output queue parameters
*/
typedef struct {
    UInt32 dataFormat;
	/**< output queue data format */

	UInt32 width;
	/**< output queue width */

	UInt32 height;
	/**< output queue height */

	UInt32 standard;
	/**< NTSC/PAL */

	UInt32 tilerEnable;
	/**< Flag to enable Tiled output */
} IspLink_OutQueuePrm;

/**
    \brief Isp Link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information */

	UInt32 numOutQueue;
	/**< Num of output queue */

    System_LinkOutQueParams outQueInfo[ISPLINK_MAX_OUT_QUE];
    /**< Output queue information */

	IspLink_OutQueuePrm outQueuePrm[ISPLINK_MAX_OUT_QUE];
	/**< Output queue parameters */

	UInt32 clkDivM;
	/**< CLKDIV M */

	UInt32 clkDivN;
	/**< CLKDIV N */

	UInt32 vsEnable;
	/**< VS Enable */

	UInt32 vnfFullResolution;
} IspLink_CreateParams;


/**
 *  @func     IspLink_init
 *
 *  @desc     Function does the Isp link register and init
 *               - Creates link task
 *               - Registers as a link with the system API
 *
 *  @modif
 *
 *  @inputs   This function takes the following inputs
 *
 *  @outputs
 *
 *  @return   FVID2_SOK on success
 *
 */
Int32 IspLink_init();

/**
 *  @func     IspLink_deInit
 *
 *  @desc     Function does the Isp link de-register and de-init
 *             - Deletes link task
 *             - De-registers as a link with the system API
 *
 *  @modif
 *
 *  @inputs
 *
 *  @outputs
 *
 *  @return  FVID2_SOK on success
 *
 */
Int32 IspLink_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
