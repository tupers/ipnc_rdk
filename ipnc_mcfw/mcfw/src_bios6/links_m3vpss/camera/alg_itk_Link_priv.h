/** ==================================================================
 *  @file   alg_itk_Link_priv.h
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/camera/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ALG_ITK_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALG_ITK_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
#include <ti/psp/iss/alg/aewb/appro2a/inc/alg_appro_aewb.h>

#include <ti/psp/iss/hal/iss/isp/ipipe/inc/ipipe.h>

// #include <ti/psp/devices/mt9j003/issdrv_mt9j003_config.h>

#define ALG_ITK_LINK_TSK_STACK_SIZE	(16*KB)
#define ALG_ITK_LINK_TSK_PRI 		(9)
#define MAX_DCC_BUF_SIZE (1024*1024)

#define ITT_ASSERT( COND )                                 // COND
#define NUM_CAPTURE_BUFS (3)

#define ITT_SERVER_MSGQ_NAME      "ITT_SERVERQ"
#define DCC_SERVER_MSGQ_NAME      "DCC_SERVERQ"

/**
 *  Handle to the ITT Server Message Queue
 */
MessageQ_Handle hDccMsgQ = NULL;

Int8 dcc_buf[MAX_DCC_BUF_SIZE];

Uint32 dcc_buf_size = 0;

typedef struct ittServer_Msg {
    MessageQ_MsgHeader header;
    Int32 response;
    Int32 arg1;
    Int32 arg2;

} ittServer_Msg;

extern volatile int yuv_data_ready;

extern volatile int request_yuv_data;

extern volatile int YUVDumpSize;

extern volatile int new_dcc_data_available;

extern volatile int ti_2a_run;

extern volatile int dcc_parser_run;

extern CameraLink_Obj gCameraLink_obj;

extern ipipe_regs_ovly ipipe_reg;

#define CMD_CKH_CONNECTION   (1)
#define CMD_SEND_DCC         (2)
#define CMD_MEM_READ_WORD    (3)
#define CMD_SAVE_RAW         (4)
#define CMD_REGS_READ        (5)
#define CMD_MEM_WRITE_WORD   (6)
#define CMD_RECV_DCC         (7)
#define CMD_CAMERA_CNTL      (8)
#define CMD_SAVE_YUV         (9)
#define CMD_MEM_READ_BUFF    (10)
#define CMD_SENSOR_REGS_READ (11)
#define CMD_SENSOR_REG_WRITE (12)
#define CMD_ITT_STOP         (13)

/**
  These variables are used for the communication between the
  itt server thread and isif isr rotine to trasfer the
  raw data from isif to PC
  */
volatile int request_raw_data;

volatile int raw_data_ready;

#ifdef MEMORY_256MB
	#define ITT_SHAREDREGION_ID (1)	
#else
	#define ITT_SHAREDREGION_ID (0)
#endif

UInt16 *captureBuff_Y[NUM_CAPTURE_BUFS];

Uint16 *captureBuff_C[NUM_CAPTURE_BUFS];

#define _SENSOR_CONTROL_H_

/* Definitions for the sensor conrol parameters */

/* typedef enum { FLICKER_DISABLE, FLICKER_50HZ , FLICKER_60HZ
 * }flickerSelect; */

typedef enum {
    CLEAR_COMMAND = 0,
    FLICKER_CONTROL = 1,
    AEWB_ENABLE = 2,
    AEWB_DISABLE = 3,
    INCREASE_EXPOSURE = 4,
    DECREASE_EXPOSURE = 5,
    DCC_ENABLE = 6,
    DCC_DISABLE = 7,
    AWB_TOGGLE = 8,
    CONTROL_PARAMS_2A = 9
} ControlCommand;

typedef struct {
    int update;
    int flicker_sel;                                       /* <TBR: Uday>Sel
                                                            * between 50/60Hz
                                                            * * flicker */
    int flickerFreq;                                       /* Select custum
                                                            * flicker */
    int minExposure;                                       /* Mininum Sensor
                                                            * exposure */
    int maxExposure;                                       /* Maximum Sensor
                                                            * exposure */
    int stepSize;                                          /* step size for
                                                            * the exposure
                                                            * variation */
    int aGainMin;                                          /* Minimum analog
                                                            * Gain */
    int aGainMax;                                          /* Maximum Analog
                                                            * gain */
    int dGainMin;                                          /* Minimum digital
                                                            * gain */
    int dGainMax;                                          /* Maximum Digital
                                                            * gain */
    int targetBrightnessMin;                               /* Minimum target
                                                            * bright ness */
    int targetBrightnessMax;                               /* Maximum target
                                                            * bright ness */
    int targetBrightness;                                  /* target
                                                            * brightness to
                                                            * be achieved */

} SensorControlParams_t;

/* ===================================================================
 *  @func     Alg_ITK_Link_init
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
//Int32 Alg_ITK_Link_init();

/* ===================================================================
 *  @func     Alg_ITK_Link_deInit
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
//Int32 Alg_ITK_Link_deInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
