/** ==================================================================
 *  @file   ti_mcfw_ipnc_main.h
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
  \file ipnc_main.h
  \brief  function related to ipnc mcfw main routine
  */

#ifndef IPNC_MAIN_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define IPNC_MAIN_H_

#include <osa.h>

#define MCFW_MAIN_SLEEP_TIME    (1 * 1000)
#define MCFW_MAIN_PRINT_CNT     ((30 * 1000)/MCFW_MAIN_SLEEP_TIME)
#define MCFW_MAIN_WAIT_TIME     (.1 * 1000)

typedef enum {
    SINGLE_STREAM_USECASE = 0,
    DUAL_STREAM_USECASE,
    TRI_STREAM_USECASE
} UI_USECASE_ID;

typedef enum {
    H264_1080P_H264_D1_MJPEG_1080P = 0,
    H264_5MP_H264_D1_MJPEG_1080P,
    H264_8MP_H264_D1_MJPEG_1080P,
    H264_10MP_H264_D1_MJPEG_1080P,
    RESOLUTION_COMBO_MAX
} UI_RESOLUTION_COMBO;

typedef enum {
    UI_AEWB_ID_NONE = 0,
    UI_AEWB_ID_APPRO,
    UI_AEWB_ID_TI,
    UI_AEWB_ID_MAXNUM = 3
} UI_AEWB_VENDOR;

typedef enum {
    UI_AEWB_OFF = 0,
    UI_AEWB_AE,
    UI_AEWB_AWB,
    UI_AEWB_AEWB,
    UI_AEWB_MODE_MAXNUM
} UI_AEWB_MODE;

typedef enum {
    UI_LEVEL_LOW = 0,
    UI_LEVEL_MEDIUM,
    UI_LEVEL_HIGH,
    UI_LEVEL_MAXNUM
} UI_LEVEL_MODE;

typedef enum {
    NF_STRENGTH_AUTO = 0,
    NF_STRENGTH_LOW,
    NF_STRENGTH_MEDIUM,
    NF_STRENGTH_HIGH,
    NF_STRENGTH_MAXNUM
} UI_NF_STRENGTH;

typedef enum {
    DSS_VNF_ON = 0,
    ISS_VNF_ON,
    NOISE_FILTER_OFF
} UI_NF_MODE;

typedef enum {
    RATECONTROL_VBR = 0,
    RATECONTROL_CBR,
    RATECONTROL_OFF,
    RATECONTROL_MAXIMUM
} UI_RATECONTROL_MODE;

typedef enum {
    MT_9J003 = 0,
    MT_AR0331,
    PN_MN34041,
    SN_IMX035,
    OM_OV2715,
    SN_IMX036,
    OM_OV9712,
    OM_OV10630,
    MT_9P031,
    MT_9D131,
    MT_9M034,
    TVP_514X,
    OM_OV7740,
    MT_9M034DUAL,
    SN_IMX136,
    MT_AR0330,
    SN_IMX104,
    AL_AL30210,
    OM_OV2710,
    SN_IMX122,
    SN_IMX140
} UI_SENSOR_MODE;

typedef enum {
    RES_1080P_D1 = 0,
    RES_1080P_1080P,
    RES_3MP_D1,
    RES_5MP_D1,
    RES_8MP_D1,
    RES_10MP_D1,
    RES_960P_D1
} UI_RES_MODE;

typedef struct {

    int usecase_id;
    int resolution_combo;
    int codec_combo;
    int loadDemo;
    int unloadDemo;
    int runDemo;
    int stopDemo;
    int exitDemo;
    int loadImgTune;
    int delImgTune;

} UI_mcfw_democfg;

typedef struct {
    int enable;
    int shutdown;
    int sampleRate;
    int codecType;
    int bitrate;
    int alarmEnable;
    int alarmLevel;

} UI_mcfw_audiocfg;

typedef struct {
    int vsEnable;
    int ldcEnable;
    int snfEnable;
    int tnfEnable;
    int noisefilterMode;
    int vnfStrength;
    int mctnfEnable;
    int vnfUseCase;
    int vaUseCase;
    int sensorId;
    int n2A_vendor;
    int n2A_mode;
    int StreamPreset[2];

    UI_mcfw_democfg demoCfg;
    UI_mcfw_audiocfg audioCfg;

    int glbceEnable;
    int glbceStrength;
	
	int initDone;

#ifdef USE_MCTNF
	int prevMinWtForCurQ8;
#endif	
	
} UI_mcfw_config;

extern UI_mcfw_config gUI_mcfw_config;

/* ===================================================================
 *  @func     App_getInitStatus
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
int App_getInitStatus();

/* ===================================================================
 *  @func     App_runDemo
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
Void App_runDemo(Void);

/* ===================================================================
 *  @func     App_stopDemo
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
Void App_stopDemo(Void);

/* ===================================================================
 *  @func     App_loadDemo
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
Void App_loadDemo(Void);

/* ===================================================================
 *  @func     App_unloadDemo
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
Void App_unloadDemo(Void);

/* ===================================================================
 *  @func     dccParamInit
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
Void dccParamInit(Void);
/* ===================================================================
 *  @func     dccParamUpdate
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
Void dccParamUpdate(Void);
/* ===================================================================
 *  @func     dccParamDelete
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
Void dccParamDelete(Void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* IPNC_MAIN_H_ */
