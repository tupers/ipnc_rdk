/*==============================================================================
 * @file:       ti_mcfw_multich_swms_tridisplay.c
 *
 * @brief:      Video capture mcfw function definition.
 *
 * @vers:       0.5.0.0 2011-06
 *
 *==============================================================================
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "ti_vsys.h"
#include "ti_vcam.h"
#include "ti_venc.h"
#include "ti_vdec.h"
#include "ti_vdis.h"
#include "ti_valg.h"
#include "ti_mcfw_ipcbits.h"
#include "ti_mcfw_ipcframes.h"
#include "demos/audio_sample/audio.h"
#include "demos/mcfw_api_demos/itt/itt_Capture.h"
#include "ti_mcfw_ipnc_main.h"
#include <ctype.h>
#include <stream.h>

//#define CCS_DEBUG

#define ZPMC_VISION

UI_mcfw_config gUI_mcfw_config = {.initDone = 0};
App_Ctrl gApp_ipcFramesCtrl;

#ifdef TEST_MPSCLR_ENABLE
	#include <mcfw/src_linux/mcfw_api/ti_vdis_priv.h>
#endif

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
Void App_loadDemo(Void)
{
	gUI_mcfw_config.initDone = 0;

#ifdef TEARDOWN_LOAD_UNLOAD
		system("./scripts/load_vpss.sh &");
        system("./scripts/load_video.sh&");
    #ifdef IPNC_DSP_ON
        system("./scripts/load_c6xdsp.sh&");
    #endif

        system("./scripts/wait_cmd.sh s m3vpss");
        system("./scripts/wait_cmd.sh s m3video");
    #ifdef IPNC_DSP_ON
        system("./scripts/wait_cmd.sh s c6xdsp");
    #endif
#else
        system("./scripts/send_cmd.sh T m3vpss");
        system("./scripts/send_cmd.sh T m3video");
    #ifdef IPNC_DSP_ON
        system("./scripts/send_cmd.sh T c6xdsp");
    #endif

        system("./scripts/wait_cmd.sh t m3vpss");
        system("./scripts/wait_cmd.sh t m3video");
    #ifdef IPNC_DSP_ON
        system("./scripts/wait_cmd.sh t c6xdsp");
    #endif
#endif // #ifdef TEARDOWN_LOAD_UNLOAD

    system("./scripts/osa_kermod_load.sh &");

#ifdef IPNC_DSS_ON
    /* load module fb */
    system("insmod ./kermod/vpss.ko mode=hdmi:1080p-60 sbufaddr=0xbfb00000");
    system("insmod ./kermod/ti81xxhdmi.ko 2> /dev/null");
    /* Enable range compression in HDMI 0..255 to 16..235 . This is needed
     * for consumer HDTVs */
    system("./bin/mem_rdwr.out --wr 0x46c00524 2");
#else
    system("./linux_prcm_ipcam k");                        /* switch off DSS */
#endif

#ifdef TEARDOWN_LOAD_UNLOAD
    OSA_printf("Application Load Completed\n");
#else
    	OSA_printf("Application Start Completed\n");
#endif
}

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
Void App_unloadDemo(Void)
{
#ifdef IPNC_DSS_ON
    system("rmmod ./kermod/ti81xxhdmi.ko");
    system("rmmod ./kermod/vpss.ko");
#endif

    /* should be in reverse sequence of Load function */
#ifdef TEARDOWN_LOAD_UNLOAD
    #ifdef IPNC_DSP_ON
        system("./scripts/send_cmd.sh x c6xdsp");
        system("./scripts/wait_cmd.sh e c6xdsp");
    #endif

        system("./scripts/send_cmd.sh x m3video");
        system("./scripts/wait_cmd.sh e m3video");

        system("./scripts/send_cmd.sh x m3vpss");
        system("./scripts/wait_cmd.sh e m3vpss");

    #ifdef IPNC_DSP_ON
        OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
        system("./bin/fw_load.out shutdown DSP");
    #endif

        OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
        system("./bin/fw_load.out shutdown VIDEO-M3");

        OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
        system("./bin/fw_load.out shutdown VPSS-M3");
#else
    #ifdef IPNC_DSP_ON
        system("./scripts/send_cmd.sh P c6xdsp");
    #endif
        system("./scripts/send_cmd.sh P m3video");
        system("./scripts/send_cmd.sh P m3vpss");

    #ifdef IPNC_DSP_ON
        system("./scripts/wait_cmd.sh p c6xdsp");
    #endif
        system("./scripts/wait_cmd.sh p m3video");
        system("./scripts/wait_cmd.sh p m3vpss");
#endif // #ifdef TEARDOWN_LOAD_UNLOAD

#ifdef TEARDOWN_LOAD_UNLOAD
    OSA_printf("\nApplication Unload Completed\n");
#else
    OSA_printf("\nApplication Stop Completed\n");
#endif
}

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
int App_getInitStatus()
{
	return gUI_mcfw_config.initDone;
}

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
Void App_runDemo(Void)
{
    VSYS_PARAMS_S   vsysParams;
    VCAM_PARAMS_S   vcamParams;
    VALG_PARAMS_S	valgParam;
    VENC_PARAMS_S   vencParams;
    VDEC_PARAMS_S   vdecParams;

#ifdef CCS_DEBUG
    OSA_printf("You could Connect to CCS Now:: Press any key to continue: \n");
    getchar();
#endif

    Vsys_params_init(&vsysParams);

    /* Use case selection based on compile flag */
#ifdef IPNC_DSS_ON
    vsysParams.systemUseCase = VSYS_USECASE_MULTICHN_TRISTREAM_FULLFTR;
#else
    vsysParams.systemUseCase = VSYS_USECASE_MULTICHN_TRISTREAM_LOWPWR;
#endif

#ifdef CAPTURE_DISPLAY_MODE
    vsysParams.systemUseCase = VSYS_USECASE_DUALCHN_DISPLAY;
#endif

#ifdef CAPTURE_MCTNF_DISP_MODE
	vsysParams.systemUseCase = VSYS_USECASE_MCTNF_DEMO;
#endif

#ifdef ENC_A8_DEC_USE_CASE
    vsysParams.systemUseCase = VSYS_USECASE_ENC_A8_DEC;
#endif

#ifdef QUAD_STREAM_NF_USE_CASE
    vsysParams.systemUseCase = VSYS_USECASE_MULTICHN_QUADSTREAM_NF;
#endif

#ifdef RVM_DEMO_MODE
	vsysParams.systemUseCase = VSYS_USECASE_RVM_DEMO;
#endif

#ifdef ZPMC_VISION
	vsysParams.systemUseCase = VSYS_USECASE_MACH_VISION;
#endif

    vsysParams.enableSecondaryOut   = FALSE;
    vsysParams.enableNsf            = FALSE;
    vsysParams.enableCapture        = TRUE;
    vsysParams.enableNullSrc        = FALSE;
    vsysParams.numDeis              = 0;
    vsysParams.numSwMs              = 0;
    vsysParams.numDisplays 			= 0;

    /* Override the context here as needed */
    Vsys_init(&vsysParams);

    Vcam_params_init(&vcamParams);

    /* Override the context here as needed */
    Vcam_init(&vcamParams);

	Valg_params_init(&valgParam);

	Valg_init(NULL);
	
    if((vsysParams.systemUseCase != VSYS_USECASE_DUALCHN_DISPLAY)&&
       (vsysParams.systemUseCase != VSYS_USECASE_MCTNF_DEMO))
    {
        Venc_params_init(&vencParams);
        /* Override the context here as needed */

        /* Enabling generation of motion vector for channel 0 only */
        vencParams.encChannelParams[0].enableAnalyticinfo = 1;

        /* Disable SEI WaterMark generation from codec*/
        vencParams.encChannelParams[0].enableWaterMarking = 0;
        vencParams.encChannelParams[1].enableWaterMarking = 0;

        if(vsysParams.systemUseCase == VSYS_USECASE_ENC_A8_DEC)
        {
            vencParams.encChannelParams[0].dynamicParam.targetBitRate = 10000 * 1000;
        }

        Venc_init(&vencParams);

        if(vsysParams.systemUseCase == VSYS_USECASE_ENC_A8_DEC)
        {
            Vdec_params_init(&vdecParams);
            vdecParams.decChannelParams[0].dynamicParam.targetBitRate = 10000 * 1000;
            Vdec_init(&vdecParams);
        }
    }

    /* Init the application specific module which will handle bitstream
     * exchange */
	App_streamSysInit();		
    App_ipcBitsInit();

    /* Init the application specific module which will handle video frame exchange */
    App_ipcFramesCreate(0);
    App_ipcFramesInSetCbInfo();

    /* Create Link instances and connects compoent blocks */
    Vsys_create();

    /* Start Date/Time Task for OSD */
    Vsys_datetimeCreate();

    if((vsysParams.systemUseCase != VSYS_USECASE_DUALCHN_DISPLAY)&&
       (vsysParams.systemUseCase != VSYS_USECASE_MCTNF_DEMO))
    {
        Venc_start();

        if(vsysParams.systemUseCase == VSYS_USECASE_ENC_A8_DEC)
        {
            Vdec_start();
        }
    }

    Vcam_start();

	gUI_mcfw_config.initDone = 1;
	
    OSA_printf("\nApplication Run Completed\n");
}

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
Void App_stopDemo()
{
    OSA_printf("\nENTERING TEARDOWN\n");

    if(gUI_mcfw_config.audioCfg.shutdown==TRUE)
    {
        Audio_captureDelete();
        gUI_mcfw_config.audioCfg.shutdown = FALSE;
    }

    Vsys_datetimeDelete();

    App_ipcBitsStop();
	App_streamSysExit();
	
    App_ipcFramesStop();

    Vcam_stop();

    if ((Vsys_getSystemUseCase() != VSYS_USECASE_DUALCHN_DISPLAY)&&
        (Vsys_getSystemUseCase() != VSYS_USECASE_MCTNF_DEMO))
    {
        Venc_stop();

        if(Vsys_getSystemUseCase() == VSYS_USECASE_ENC_A8_DEC)
        {
            Vdec_stop();
        }
    }

    Vsys_delete();

    /* De-initialize components */
    App_ipcBitsExit();
    App_ipcFramesDelete();

    if ((Vsys_getSystemUseCase() != VSYS_USECASE_DUALCHN_DISPLAY)&&
        (Vsys_getSystemUseCase() != VSYS_USECASE_MCTNF_DEMO))
    {
        Venc_exit();

        if(Vsys_getSystemUseCase() == VSYS_USECASE_ENC_A8_DEC)
        {
            Vdec_exit();
        }
    }

    Vcam_exit();

    Vsys_exit();
	
    OSA_printf("\nApplication Stop Completed\n");
}

/* ===================================================================
 *  @func     UI_signalHandler
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
void UI_signalHandler(int signum)
{
    gUI_mcfw_config.demoCfg.stopDemo = TRUE;
    gUI_mcfw_config.demoCfg.unloadDemo = TRUE;
    gUI_mcfw_config.demoCfg.delImgTune = TRUE;
    gUI_mcfw_config.demoCfg.exitDemo = TRUE;

    OSA_printf("MCFW EXIT COMMAND RECVD\n");
}

/* ===================================================================
 *  @func     showUsage
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
void showUsage()
{
    OSA_printf("\n");
    OSA_printf
        (" MultiChannel Framework for IPNC - (c) Texas Instruments 2012\n");
    OSA_printf("\n");
    OSA_printf(" USAGE: ./ipnc_rdk_mcfw.out <option 1> <option 2> ...\n");
    OSA_printf("\n");
    OSA_printf(" Following options are supported,\n");
    OSA_printf("\n");
    OSA_printf
        (" TRISTREAM        : Tri-streaming usecase of 1080p60H264+D130H264+1080P5MJPEG is supported\n");
    OSA_printf
        (" DUALSTREAM       : Dual-streaming usecase of 1080p60H264+D130H264 is supported\n");
    OSA_printf
        (" SINGLESTREAM     : Single-streaming usecase of 1080p60H264 is supported\n");
    OSA_printf(" <Resolution Combo>  : Details of the Resolution ID\n");
    OSA_printf
        (" 1080P_D1/1080P_1080P/5MP_D1/8MP_D1/10MP_D1 is supported\n");
    OSA_printf(" <Usecase Combo>  : Details of the Combo ID\n");
    OSA_printf
        (" SINGLE_H264/SINGLE_MPEG4/SINGLE_MJPEG/DUAL_H264/DUAL_MPEG4/H264_MJPEG/MPEG4_MJPEG/H264_MPEG4/TRIPLE_H264/TRIPLE_MPEG4 is supported\n");
    OSA_printf(" SNF        : Spatial Noise Filter is ENABLED\n");
    OSA_printf(" TNF        : Temporal Noise Filter is ENABLED\n");
    OSA_printf(" LDC        : Lens Distortion Correction is ENABLED\n");
    OSA_printf(" VS         : Video Stabilization is ENABLED\n");
    OSA_printf(" FD         : Face detection is ENABLED\n");
    OSA_printf(" VA         : Video Analytics use case is ENABLED (valid for DM8127 only)\n");
    OSA_printf(" GLBCE <Mode> <Strength>: SALDRE/GLBCE use case is ENABLED \n");
    OSA_printf(" GLBCE Mode options: HS for High Speed and HQ for High Quality \n");
    OSA_printf(" GLBCE Strength options: LOW, MED and HIGH options for strength parameters \n");
    OSA_printf(" VNF_SPEED/VNF_QUALITY: VNF_SPEED will use DSS VNF and VNF_QUALITY will use ISS VNF\n");
    OSA_printf
        (" AUDIO [sample rate] : Set the sample rate of audio. Default is 8000 (8K) if not specified\n");
    OSA_printf
        (" <2A VENDOR>  : NONE2A/APPRO2A/TI2A to be selected based on the vendor for 2A algorithm\n");
    OSA_printf(" <AEWB Mode>  : AE/AWB/AEWB is ENABLED\n");
    OSA_printf
        (" <CODEC MODE 1>  : HIGH_SPEED1/HIGH_QUALITY1/SVC1 to be selected as codec encoding preset for stream 1\n");
    OSA_printf
        (" <CODEC MODE 2>  : HIGH_SPEED2/HIGH_QUALITY2/SVC2 to be selected as codec encoding preset for stream 2\n");
}

/* ===================================================================
 *  @func     main
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
int main(int argc, char **argv)
{
    int i=0, sleepCnt=0;

    Bool done = FALSE, exitOK = FALSE;

    if (argc == 1)
    {
        showUsage();
        return 0;
    }

    memset(&gUI_mcfw_config, FALSE, sizeof(gUI_mcfw_config));

    OSA_attachSignalHandler(SIGINT, UI_signalHandler);
	
	App_msgHandlerInit(stream_get_handle());

    gUI_mcfw_config.demoCfg.usecase_id  = TRI_STREAM_USECASE;
    gUI_mcfw_config.demoCfg.loadDemo    = FALSE;
    gUI_mcfw_config.demoCfg.unloadDemo  = FALSE;
    gUI_mcfw_config.demoCfg.runDemo     = FALSE;
    gUI_mcfw_config.demoCfg.stopDemo    = FALSE;
    gUI_mcfw_config.demoCfg.exitDemo    = FALSE;
    gUI_mcfw_config.demoCfg.loadImgTune = FALSE;
    gUI_mcfw_config.demoCfg.delImgTune  = FALSE;
    gUI_mcfw_config.vsEnable            = FALSE;
    gUI_mcfw_config.glbceEnable         = FALSE;
    gUI_mcfw_config.glbceStrength       = FALSE;
    gUI_mcfw_config.ldcEnable           = TRUE;
    gUI_mcfw_config.snfEnable           = TRUE;
    gUI_mcfw_config.tnfEnable           = TRUE;
    gUI_mcfw_config.noisefilterMode     = DSS_VNF_ON;
    gUI_mcfw_config.vnfStrength         = NF_STRENGTH_AUTO;
    gUI_mcfw_config.vnfUseCase          = FALSE;
    gUI_mcfw_config.vaUseCase           = FALSE;
    gUI_mcfw_config.n2A_vendor          = UI_AEWB_ID_NONE;
    gUI_mcfw_config.n2A_mode            = UI_AEWB_OFF;

#ifdef IMGS_MICRON_MT9J003
    gUI_mcfw_config.sensorId = MT_9J003;
#endif
#ifdef IMGS_MICRON_AR0330
    gUI_mcfw_config.sensorId = MT_AR0330;
#endif
#ifdef IMGS_MICRON_AR0331
    gUI_mcfw_config.sensorId = MT_AR0331;
#endif
#ifdef IMGS_PANASONIC_MN34041
    gUI_mcfw_config.sensorId = PN_MN34041;
#endif
#ifdef IMGS_OMNIVISION_OV10630
    gUI_mcfw_config.sensorId = OM_OV10630;
#endif
#ifdef IMGS_MICRON_MT9P031
    gUI_mcfw_config.sensorId = MT_9P031;
#endif
#ifdef IMGS_SONY_IMX136
    gUI_mcfw_config.sensorId = SN_IMX136;
#endif
#ifdef IMGS_SONY_IMX104
    gUI_mcfw_config.sensorId = SN_IMX104;
#endif
#ifdef IMGS_SONY_IMX140
    gUI_mcfw_config.sensorId = SN_IMX140;
#endif
#ifdef IMGS_MICRON_MT9M034
    gUI_mcfw_config.sensorId = MT_9M034;
#endif

    gUI_mcfw_config.audioCfg.enable     = FALSE;
    gUI_mcfw_config.audioCfg.shutdown   = FALSE;
    gUI_mcfw_config.audioCfg.sampleRate = 8000;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "TRISTREAM") == 0)
            gUI_mcfw_config.demoCfg.usecase_id = TRI_STREAM_USECASE;
        else if (strcmp(argv[i], "DUALSTREAM") == 0)
            gUI_mcfw_config.demoCfg.usecase_id = DUAL_STREAM_USECASE;
        else if (strcmp(argv[i], "SINGLESTREAM") == 0)
            gUI_mcfw_config.demoCfg.usecase_id = SINGLE_STREAM_USECASE;
        else if (strcmp(argv[i], "1080P_D1") == 0)
            gUI_mcfw_config.demoCfg.resolution_combo = RES_1080P_D1;
        else if (strcmp(argv[i], "1080P_1080P") == 0) {
            gUI_mcfw_config.demoCfg.resolution_combo = RES_1080P_1080P;
        }
        else if (strcmp(argv[i], "3MP_D1") == 0)
            gUI_mcfw_config.demoCfg.resolution_combo = RES_3MP_D1;
        else if (strcmp(argv[i], "5MP_D1") == 0)
            gUI_mcfw_config.demoCfg.resolution_combo = RES_5MP_D1;
        else if (strcmp(argv[i], "8MP_D1") == 0)
            gUI_mcfw_config.demoCfg.resolution_combo = RES_8MP_D1;
        else if (strcmp(argv[i], "10MP_D1") == 0)
            gUI_mcfw_config.demoCfg.resolution_combo = RES_10MP_D1;
        else if (strcmp(argv[i], "SNF") == 0)
            gUI_mcfw_config.snfEnable = TRUE;
        else if (strcmp(argv[i], "TNF") == 0)
            gUI_mcfw_config.tnfEnable = TRUE;
        else if (strcmp(argv[i], "LDC") == 0)
            gUI_mcfw_config.ldcEnable = TRUE;
        else if (strcmp(argv[i], "VS") == 0)
            gUI_mcfw_config.vsEnable = TRUE;
        else if (strcmp(argv[i], "GLBCE") == 0){
          if(strcmp(argv[i+1], "HS")==0){
            gUI_mcfw_config.glbceEnable = 1;
            if (strcmp(argv[i+2], "LOW") == 0)
                gUI_mcfw_config.glbceStrength = 0;
            else if (strcmp(argv[i+2], "MED") == 0)
                gUI_mcfw_config.glbceStrength = 1;
            else if (strcmp(argv[i+2], "HIGH") == 0)
                gUI_mcfw_config.glbceStrength = 2;
          }
          else if(strcmp(argv[i+1], "HQ")==0) {
            gUI_mcfw_config.glbceEnable     = 2;
            if (strcmp(argv[i+2], "LOW") == 0)
                gUI_mcfw_config.glbceStrength = 0;
            else if (strcmp(argv[i+2], "MED") == 0)
                gUI_mcfw_config.glbceStrength = 1;
            else if (strcmp(argv[i+2], "HIGH") == 0)
                gUI_mcfw_config.glbceStrength = 2;
          }
          else
            gUI_mcfw_config.glbceEnable     = 0;

          i+=2;
        }
        else if (strcmp(argv[i], "VA") == 0)
            gUI_mcfw_config.vaUseCase = TRUE;
        else if (strcmp(argv[i], "AUDIO") == 0)
        {
            if (!isdigit(argv[i+1][0]))
            {
                OSA_printf("Invalid parameter for AUDIO\n");
                return 0;
            }
            gUI_mcfw_config.audioCfg.enable     = TRUE;
            gUI_mcfw_config.audioCfg.sampleRate = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "NONE2A") == 0)
            gUI_mcfw_config.n2A_vendor = UI_AEWB_ID_NONE;
        else if (strcmp(argv[i], "APPRO2A") == 0)
            gUI_mcfw_config.n2A_vendor = UI_AEWB_ID_APPRO;
        else if (strcmp(argv[i], "TI2A") == 0)
            gUI_mcfw_config.n2A_vendor = UI_AEWB_ID_TI;
        else if (strcmp(argv[i], "AE") == 0)
            gUI_mcfw_config.n2A_mode = UI_AEWB_AE;
        else if (strcmp(argv[i], "AWB") == 0)
            gUI_mcfw_config.n2A_mode = UI_AEWB_AWB;
        else if (strcmp(argv[i], "AEWB") == 0)
            gUI_mcfw_config.n2A_mode = UI_AEWB_AEWB;
        else if (strcmp(argv[i], "SVC1") == 0)
        {
            gUI_mcfw_config.StreamPreset[0] = 3;
        }
        else if (strcmp(argv[i], "SVC2") == 0)
        {
            gUI_mcfw_config.StreamPreset[1] = 3;
        }
        else if (strcmp(argv[i], "HIGH_SPEED1") == 0)
        {
            gUI_mcfw_config.StreamPreset[0] = 0;
        }
        else if (strcmp(argv[i], "HIGH_SPEED2") == 0)
        {
            gUI_mcfw_config.StreamPreset[1] = 0;
        }
        else if (strcmp(argv[i], "HIGH_QUALITY1") == 0)
        {
            gUI_mcfw_config.StreamPreset[0] = 2;
        }
        else if (strcmp(argv[i], "HIGH_QUALITY2") == 0)
        {
            gUI_mcfw_config.StreamPreset[1] = 2;
        }
        else if (strcmp(argv[i], "SINGLE_H264") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 0;
        }
        else if (strcmp(argv[i], "SINGLE_MPEG4") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 1;
        }
        else if (strcmp(argv[i], "SINGLE_JPEG") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 2;
        }
        else if (strcmp(argv[i], "H264_JPEG") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 3;
        }
        else if (strcmp(argv[i], "MPEG4_JPEG") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 4;
        }
        else if (strcmp(argv[i], "DUAL_H264") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 5;
        }
        else if (strcmp(argv[i], "DUAL_MPEG4") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 6;
        }
        else if (strcmp(argv[i], "H264_MPEG4") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 7;
        }
        else if (strcmp(argv[i], "TRIPLE_H264") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 8;
        }
        else if (strcmp(argv[i], "TRIPLE_MPEG4") == 0)
        {
            gUI_mcfw_config.demoCfg.codec_combo = 9;
        }
        else if (strcmp(argv[i], "VNF_SPEED") == 0)
        {
            gUI_mcfw_config.noisefilterMode = DSS_VNF_ON;
        }
        else if (strcmp(argv[i], "VNF_QUALITY") == 0)
        {
            gUI_mcfw_config.noisefilterMode = ISS_VNF_ON;
        }
    }

    gUI_mcfw_config.ldcEnable           = FALSE;
    
	/* Change the glbceEnable to the enum of glbcePresets defined in glbceLink_glbce.h */
    if(gUI_mcfw_config.glbceEnable==2)
        gUI_mcfw_config.glbceEnable = (4+gUI_mcfw_config.glbceStrength);
    else if(gUI_mcfw_config.glbceEnable==1)
        gUI_mcfw_config.glbceEnable = (1+gUI_mcfw_config.glbceStrength);
    else
        gUI_mcfw_config.glbceEnable = 0;
		
    while (done==FALSE)
    {
        if (gUI_mcfw_config.demoCfg.runDemo == FALSE)
        {
            if (gUI_mcfw_config.demoCfg.loadDemo == FALSE)
            {
                App_loadDemo();
                gUI_mcfw_config.demoCfg.loadDemo = TRUE;
                OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
            }
            App_runDemo();
            gUI_mcfw_config.demoCfg.runDemo = TRUE;
            OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);

            if (gUI_mcfw_config.demoCfg.loadImgTune == FALSE)
            {
                Itt_TaskCreate();
                gUI_mcfw_config.demoCfg.loadImgTune = TRUE;
                OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
            }
        }
        else if (gUI_mcfw_config.demoCfg.stopDemo == TRUE)
        {
            if (gUI_mcfw_config.demoCfg.delImgTune == TRUE)
            {
                Itt_TaskDelete();
                gUI_mcfw_config.demoCfg.delImgTune = FALSE;
                OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
            }

            App_stopDemo();
            gUI_mcfw_config.demoCfg.stopDemo = FALSE;
            OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);

            if (gUI_mcfw_config.demoCfg.unloadDemo == TRUE)
            {
                App_unloadDemo();
                gUI_mcfw_config.demoCfg.unloadDemo = FALSE;
                OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
                exitOK = TRUE;
            }
        }
        else
        {
            if(sleepCnt==MCFW_MAIN_PRINT_CNT)
            {
                sleepCnt=0;
            }
            else {
                sleepCnt++;
            }

            OSA_waitMsecs(MCFW_MAIN_SLEEP_TIME);
        }

        if (gUI_mcfw_config.demoCfg.exitDemo == TRUE)
        {
            done = TRUE; //(exitOK==TRUE)?TRUE:FALSE;
        }
    }

    if (gUI_mcfw_config.demoCfg.delImgTune == TRUE)
    {
        Itt_TaskDelete();
        gUI_mcfw_config.demoCfg.delImgTune = FALSE;
        OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
    }

    App_stopDemo();
    gUI_mcfw_config.demoCfg.stopDemo = FALSE;
    OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);

    if (gUI_mcfw_config.demoCfg.unloadDemo == TRUE)
    {
        App_unloadDemo();
        gUI_mcfw_config.demoCfg.unloadDemo = FALSE;
        OSA_waitMsecs(MCFW_MAIN_WAIT_TIME);
        exitOK = TRUE;
    }

	App_msgHandlerExit(stream_get_handle());
	
    OSA_printf("\nExiting Usecase !!! \n");

    return 0;
}
