/** ==================================================================
 *  @file   glbceLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/glbce/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _GLBCE_LINK_MOVAVG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _GLBCE_LINK_MOVAVG_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/glbceLink.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
#include <ti/psp/iss/iss_capture.h>
#include <ti/psp/iss/alg/dcc/inc/idcc.h>

#define LENGTH_OF_AVERAGE 10

typedef struct movAvgClass
{
    int array[LENGTH_OF_AVERAGE];
	int readIndex;
	int writeIndex;
	int accumulator;
	int oldestData;
	int newsetData;
	int counter;
} movAvgClass;

void movAvgInit(movAvgClass * srt);
int  movAvgTakeNewGiveAverage(movAvgClass * srt, int newData);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif                                                     /* _GLBCE_LINK_MOVAVG_H_ */



