/** ==================================================================
 *  @file   vnfLink_vnf.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/vnf/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <mcfw/src_bios6/utils/utils_mem.h>

#include <ti/psp/iss/alg/jpeg_enc/inc/msp.h>
#include <ti/psp/iss/alg/ip_run/inc/ip_run.h>
#include <ti/psp/iss/alg/vnf/msp_vnf.h>
#include <ti/psp/vps/fvid2.h>

#include "vnfLink_vnf.h"
#include "vnfLink_priv.h"

#define VNF_USE_DEFAULT_PARAMS
#define VNF_EXTERNAL_RESOURCES_HANDLE

/* Pre-defined temporal noise filter parameters structures including default */
static MSP_VNF_TNF_PARAMS tnf_weak_params = { 16, 16, 16, 200, 14, 1, 0, MSP_VNF_PARAM_USER,
  13, MSP_VNF_PARAM_USER, 0, 13};
static MSP_VNF_TNF_PARAMS tnf_strong_params = { 16, 16, 16, 200, 22, 1, 0, MSP_VNF_PARAM_USER,
  13, MSP_VNF_PARAM_USER, 0, 13};
static MSP_VNF_TNF_PARAMS tnf_default_params = { 16, 16, 16, 200, 18, 1, 0, MSP_VNF_PARAM_USER,
  13, MSP_VNF_PARAM_USER, 0, 13};


/*unYComponentWt,unUComponentWt,unVComponentWt*/
/*unMaxBlendingFactor,unMotionThreshold(TM),unDiffScaleFactor(Q),unDiffShiftFactor*/
/*eTnfAlphaLUTParam,unStrengthOfTNF3(A),eTnf3Preset*/
static MSP_VNF_TNF_PARAMS tnf3_weak_params = { 16, 16, 16,
    200, 12, 2, 0,
    MSP_VNF_PARAM_USER, 9, MSP_VNF_PARAM_USER, 0, 9};
static MSP_VNF_TNF_PARAMS tnf3_strong_params = { 16, 16, 16,
    200, 36, 2, 0,
    MSP_VNF_PARAM_USER, 13, MSP_VNF_PARAM_USER, 0, 13};
static MSP_VNF_TNF_PARAMS tnf3_default_params = { 16, 16, 16,
    200, 24, 2, 0,
    MSP_VNF_PARAM_USER, 11, MSP_VNF_PARAM_USER, 0, 11};

static MSP_S16 nsf_weak_params[] = {32, 0, 16, 0, 8, 0, 64, 0, 32, 0, 16, 0, 64, 0, 32, 0, 16, 0};

static MSP_S16 nsf_default_params[] = {64, 0, 32, 0, 16, 0, 64, 0, 32, 0, 16, 0, 64, 0, 32, 0, 16, 0};

static MSP_S16 nsf_strong_params[] = {128, 0, 64, 0, 32, 0, 256, 0, 128, 0, 64, 0, 256, 0, 128, 0, 64, 0};

extern ti2a_output ti2a_output_params;

// static MSP_VNF_TNF_PARAMS tnf_0_params = {16, 16, 16, 256, 13};
// static MSP_VNF_TNF_PARAMS tnf_1_params = {16, 10, 10, 256, 15};


Iss_CaptDccPrm gDccPrm;

#ifdef VNF_USE_DEFAULT_PARAMS
/* Pre-defined affine parameters structures including default */
static MSP_VNF_AFFINE_PARAMS vnf_affine_default_params =
    { 4096, 0, 0, 0, 4096, 0 };
/* static MSP_VNF_AFFINE_PARAMS vnf_affine_1_params = {4218, 0, 0, 0, 4218,
 * 0}; static MSP_VNF_AFFINE_PARAMS vnf_affine_2_params = {3978, 0, 0, 0,
 * 3978, 0}; static MSP_VNF_AFFINE_PARAMS vnf_affine_3_params = {3921, 0, 0,
 * 0, 3921, 0}; static MSP_VNF_AFFINE_PARAMS vnf_affine_4_params = {3840, 0,
 * 0, 0, 3840, 0}; static MSP_VNF_AFFINE_PARAMS
 * vnf_affine_horizontal_shift_params = {4096, 0, 100, 0, 4096, 0}; static
 * MSP_VNF_AFFINE_PARAMS vnf_affine_vertical_shift_params = {4096, 0, 0, 0,
 * 4096, 100}; static MSP_VNF_AFFINE_PARAMS vnf_affine_rotation_1_params =
 * {0, 4096, 0, 4096, 0, 0}; static MSP_VNF_AFFINE_PARAMS
 * vnf_affine_rotation_2_params = {4096, 0, 0, 0, 4096, 0}; static
 * MSP_VNF_AFFINE_PARAMS vnf_affine_rotation_3_params = {4096, 0, 0, 0, 4096,
 * * 0}; */

extern const MSP_U16 vnf_ldc_lut_0[];
extern const MSP_U16 vnf_ldc_lut_tamron[];

extern MSP_U8 vnf_tamron_ldckhl;

extern MSP_U8 vnf_tamron_ldckhr;

extern MSP_U8 vnf_tamron_ldckvl;

extern MSP_U8 vnf_tamron_ldckvu;

extern MSP_U16 vnf_tamron_Rth;

extern MSP_U8 vnf_tamron_rightShiftBits;
#endif

static int frameCnt;
static int firstFrame;

typedef struct {
    MSP_APPCBPARAM_TYPE tVnf_appcbptr;
    ti_sysbios_knl_Semaphore_Handle pProcEndSem;
    MSP_VNF_CREATE_PARAMS vnfQueryParam;
    MSP_HANDLE hVnfMSP;
    MSP_BUFHEADER_TYPE inBuffVnf;
    MSP_BUFHEADER_TYPE outBuffVnf;
    MSP_BUFHEADER_TYPE prevInBuffVnf;
} MSP_APPDATA_VNF;

// ##################################################################################################
// ######
// ##################################################################################################

/* ================================================================ */
/**
 * MSP_VNF_Callback()
 * This is MSP VNF component callback implementation
 *
 * @param  hMSP          : Handle to MSP VNF component
 * @param  pAppData      : Handle to MSP VNF application data
 * @param  tEvent        : Handle to event structure
 * @param  nEventData1   : Event data type 1
 * @param  nEventData2   : Event data type 2
 *
 * @return MSP_ERROR_NONE    = success \n
 *         Other             = error
 */
/* ================================================================ */
/* ===================================================================
 *  @func     MSP_VNF_Callback
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
static MSP_ERROR_TYPE MSP_VNF_Callback(MSP_PTR hMSP, MSP_PTR pAppData,
                                       MSP_EVENT_TYPE tEvent,
                                       MSP_OPAQUE nEventData1,
                                       MSP_OPAQUE nEventData2)
{
    MSP_COMPONENT_TYPE *jComponent = (MSP_COMPONENT_TYPE *) pAppData;

    MSP_APPDATA_VNF *pCompPrv = jComponent->pCompPrivate;

    switch (tEvent)
    {
        case MSP_OPEN_EVENT:
        case MSP_CLOSE_EVENT:
        case MSP_PROFILE_EVENT:
        case MSP_CTRLCMD_EVENT:
        case MSP_ERROR_EVENT:
            break;

        case MSP_DATA_EVENT:
            if (nEventData1 == MSP_VNF_INPUT_PORT)
            {
                /* Input buffer processed */
            }
            if (nEventData1 == MSP_VNF_OUTPUT_PORT)
            {
                /* Post the semaphore */
                Semaphore_post(pCompPrv->pProcEndSem);
            }
            break;
    }

    return MSP_ERROR_NONE;
}

Int32 MSP_VNF_Engine_Reset(MSP_COMPONENT_TYPE * jComponent)
{
    MSP_APPDATA_VNF *pCompPrv;
    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    GOTO_EXIT_IF(NULL == jComponent, MSP_ERROR_NULLPTR);
    pCompPrv = jComponent->pCompPrivate;

    /* De-initialize the MSP VNF component */
    GOTO_EXIT_IF((MSP_deInit(pCompPrv->hVnfMSP)) != MSP_ERROR_NONE,
                 MSP_ERROR_FAIL);

    /* Initialize the MSP VNF component */
    status =
        MSP_init(&pCompPrv->hVnfMSP, "MSP.VNF", MSP_PROFILE_REMOTE,
                 &pCompPrv->tVnf_appcbptr);
    GOTO_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     MSP_VNF_Engine_Init
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
Int32 MSP_VNF_Engine_Init(MSP_COMPONENT_TYPE ** jComponent)
{
    MSP_APPDATA_VNF *pCompPrv;

    Semaphore_Params semParams;

    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    MSP_VNF_CREATE_PARAMS *pVnfQueryParam;

    MSP_VNF_LDC_PARAMS *ldc_params;

    MSP_VNF_LENS_DISTORTION_PARAMS *lens_distortion_params;

    GOTO_EXIT_IF(NULL == jComponent, MSP_ERROR_NULLPTR);

    frameCnt = 1;
    firstFrame = 1;

    *jComponent = MSP_MEMALLOC(sizeof(MSP_COMPONENT_TYPE));
    GOTO_EXIT_IF(NULL == (*jComponent), MSP_ERROR_NOTENOUGHRESOURCES);

    (*jComponent)->pCompPrivate = MSP_MEMALLOC(sizeof(MSP_APPDATA_VNF));
    GOTO_EXIT_IF(NULL == (*jComponent)->pCompPrivate,
                 MSP_ERROR_NOTENOUGHRESOURCES);
    pCompPrv = (*jComponent)->pCompPrivate;

    pCompPrv->tVnf_appcbptr.pAppData = (*jComponent);
    pCompPrv->tVnf_appcbptr.MSP_callback = MSP_VNF_Callback;

    pVnfQueryParam = &(pCompPrv->vnfQueryParam);

    pVnfQueryParam->ptLdcParams =
        (MSP_VNF_LDC_PARAMS *) MSP_MEMALLOC(sizeof(MSP_VNF_LDC_PARAMS));
    GOTO_EXIT_IF(pVnfQueryParam->ptLdcParams == NULL,
                 MSP_ERROR_NOTENOUGHRESOURCES);
    ldc_params = pVnfQueryParam->ptLdcParams;

    pVnfQueryParam->ptNsfParams =
        (MSP_VNF_NSF_PARAMS *) MSP_MEMALLOC(sizeof(MSP_VNF_NSF_PARAMS));
    GOTO_EXIT_IF(pVnfQueryParam->ptNsfParams == NULL,
                 MSP_ERROR_NOTENOUGHRESOURCES);

    pVnfQueryParam->ptTnfParams =
        (MSP_VNF_TNF_PARAMS *) MSP_MEMALLOC(sizeof(MSP_VNF_TNF_PARAMS));
    GOTO_EXIT_IF(pVnfQueryParam->ptTnfParams == NULL,
                 MSP_ERROR_NOTENOUGHRESOURCES);

    ldc_params->ptLensDistortionParams =
        (MSP_VNF_LENS_DISTORTION_PARAMS *)
        MSP_MEMALLOC(sizeof(MSP_VNF_LENS_DISTORTION_PARAMS));
    GOTO_EXIT_IF(ldc_params->ptLensDistortionParams == NULL,
                 MSP_ERROR_NOTENOUGHRESOURCES);
    lens_distortion_params = ldc_params->ptLensDistortionParams;

    ldc_params->ptAffineParams =
        (MSP_VNF_AFFINE_PARAMS *) MSP_MEMALLOC(sizeof(MSP_VNF_AFFINE_PARAMS));
    GOTO_EXIT_IF(ldc_params->ptAffineParams == NULL,
                 MSP_ERROR_NOTENOUGHRESOURCES);

    lens_distortion_params->ptLutParams =
        (MSP_VNF_LUT_PARAMS *) MSP_MEMALLOC(sizeof(MSP_VNF_LUT_PARAMS));
    GOTO_EXIT_IF(lens_distortion_params->ptLutParams == NULL,
                 MSP_ERROR_NOTENOUGHRESOURCES);

    Semaphore_Params_init(&semParams);
    pCompPrv->pProcEndSem = Semaphore_create(0, &semParams, NULL);

    /* Initialize the IP_RUN by creating the task and semaphores */
    IP_Run_Init();

    /* Initialize the MSP VNF component */
    status =
        MSP_init(&pCompPrv->hVnfMSP, "MSP.VNF", MSP_PROFILE_REMOTE,
                 &pCompPrv->tVnf_appcbptr);
    GOTO_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     MSP_VNF_Engine_Open
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
Int32 MSP_VNF_Engine_Open(MSP_COMPONENT_TYPE * jComponent,
                          VnfCreateParams * params)
{
    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    MSP_APPDATA_VNF *pCompPrv;

    MSP_VNF_CREATE_PARAMS *pVnfQueryParam;

    MSP_VNF_LDC_PARAMS *ldc_params;

    MSP_VNF_NSF_PARAMS *nsf_params;

    MSP_VNF_TNF_PARAMS *tnf_params;

    MSP_VNF_LENS_DISTORTION_PARAMS *lens_distortion_params;

    MSP_VNF_AFFINE_PARAMS *affine_params;

    MSP_VNF_LUT_PARAMS *lut_params;

	static VnfRunTimeParams pVnfParam;

    GOTO_EXIT_IF(NULL == jComponent, MSP_ERROR_NULLPTR);
    GOTO_EXIT_IF(NULL == params, MSP_ERROR_NULLPTR);
    GOTO_EXIT_IF(NULL == jComponent->pCompPrivate,
                 MSP_ERROR_NOTENOUGHRESOURCES);
    pCompPrv = jComponent->pCompPrivate;

    pVnfQueryParam = &(pCompPrv->vnfQueryParam);

    ldc_params = pVnfQueryParam->ptLdcParams;
    nsf_params = pVnfQueryParam->ptNsfParams;
    tnf_params = pVnfQueryParam->ptTnfParams;
    lens_distortion_params = ldc_params->ptLensDistortionParams;
    affine_params = ldc_params->ptAffineParams;
    lut_params = lens_distortion_params->ptLutParams;

    pVnfQueryParam->ulComputeWidth = params->ulComputeWidth;
    pVnfQueryParam->ulComputeHeight = params->ulComputeHeight;

    pVnfQueryParam->eOperateMode = params->eOperateMode;

    pVnfQueryParam->eInputFormat = params->eInputFormat;
    pVnfQueryParam->eOutputFormat = params->eOutputFormat;

    switch (pVnfQueryParam->eInputFormat)
    {
		case MSP_VNF_YUV_FORMAT_YCBCR420 :
			pVnfQueryParam->ulInputStrideChroma = params->ulInputStrideChroma;
		case MSP_VNF_YUV_FORMAT_YCBCR422 :
			pVnfQueryParam->ulInputStride = params->ulInputStride;
            break;
        default:
            // GOTO_EXIT(MSP_ERROR_NOTSUPPORTED);
            break;
    }

    pVnfQueryParam->ulOutputStrideLuma = params->ulOutputStrideLuma;
    pVnfQueryParam->ulOutputStrideChroma = params->ulOutputStrideChroma;
    ldc_params->eYInterpolationMethod = params->eInterpolationLuma;

    if(firstFrame)
    {
        pVnfParam.vnf_dcc_params_exist = 0;
        firstFrame = 0;
    }

    if((frameCnt == 300)||firstFrame)
    {
        extern MSP_S16  vnf_user_alpha_lookup[];

        //int i;
        pVnfParam.pVnfQueryParam = pVnfQueryParam;
        pVnfParam.shift_factor_for_3dnf_diff = (MSP_S16 *) &(tnf_params->unDiffScaleFactor);
        pVnfParam.scale_factor_for_3dnf_diff = (MSP_S16 *) &(tnf_params->unDiffScaleFactor);
        pVnfParam.vnf_user_alpha_lookup      = vnf_user_alpha_lookup;

		VnfLink_drvUpdateDCC(&pVnfParam);

	    nsf_params->eHalfFilterParam	= nsf_params->eFilterParam;
	    nsf_params->eQuadFilterParam	= nsf_params->eFilterParam;

#ifdef PRINT_TNF3_PARAM
		Vps_printf("===================VNF Parameters :%s============ \n",pVnfParam.vnf_dcc_params_exist==1?"true":"false");
        if(pVnfParam.vnf_dcc_params_exist)
        {
            Vps_printf("[VNF] tnf_params->unDiffShiftFactor      = %d \n",tnf_params->unDiffShiftFactor);
            Vps_printf("[VNF] eFilterParam = %d \n",nsf_params->eFilterParam);
            Vps_printf("[VNF] eShdParam = %d \n",nsf_params->eShdParam);
            Vps_printf("[VNF] eEdgeParam = %d \n",nsf_params->eEdgeParam);
            Vps_printf("[VNF] eDesatParam = %d \n",nsf_params->eDesatParam);
            Vps_printf("[VNF] tnf_params->unMaxBlendingFactor    = %d \n",tnf_params->unMaxBlendingFactor);
			Vps_printf("[VNF] params->bTnfEn                     = %d \n", params->bTnfEn);
            Vps_printf("[VNF] pVnfQueryParam->eOperateMode       = %d [6-TNF3, 7-NSF2]\n",pVnfQueryParam->eOperateMode);
            Vps_printf("[VNF] tnf_params->eTnf3Preset            = %d \n",tnf_params->eTnf3Preset);
            Vps_printf("[VNF] tnf_params->unMotionThreshold      = %d \n",tnf_params->unMotionThreshold);
            Vps_printf("[VNF] tnf_params->unStrengthOfTNF3       = %d \n",tnf_params->unStrengthOfTNF3);
            Vps_printf("[VNF] nsf_params->tnf3TS                 = %d \n",nsf_params->tnf3TS);
			Vps_printf("[VNF] tnf_params->brightnessOffset       = %d \n",tnf_params->brightnessOffset);
            Vps_printf("[VNF] aGain = %4d, dGain = %4d\n",(ti2a_output_params.sensorGain),(ti2a_output_params.ipipe_awb_gain.dGain * 1000 /256));
        }
#endif
        frameCnt = 0;
    	}
    frameCnt++;

    if(pVnfQueryParam->eOperateMode != params->eOperateMode)
    {
        pVnfQueryParam->eOperateMode = params->eOperateMode;
    }

    if(pVnfParam.vnf_dcc_params_exist == 0)
    {
        ldc_params->unPixelPad = params->unPixelPad;
        ldc_params->eYInterpolationMethod = params->eInterpolationLuma;
        memcpy(nsf_params, &params->nsfParams, sizeof(*nsf_params));
        memcpy(tnf_params, &params->tnfParams, sizeof(*tnf_params));

        nsf_params->eHalfFilterParam  = nsf_params->eFilterParam;
        nsf_params->eQuadFilterParam  = nsf_params->eFilterParam;

        if (params->btnfUseDefault)
        {
                if (IssAlg_capt2AGetAEWBVendor() == AEWB_ID_TI)
                {
                    if (ti2a_output_params.sensorGain < 16000)
                        MSP_Memcpy(tnf_params, &tnf_weak_params, sizeof(*tnf_params));
                    else
                        MSP_Memcpy(tnf_params, &tnf_strong_params, sizeof(*tnf_params));
                }
                else
                    MSP_Memcpy(tnf_params, &tnf_default_params, sizeof(*tnf_params));
        }

		if ((pVnfQueryParam->eOperateMode == MODE_TNF3) || (pVnfQueryParam->eOperateMode == MODE_NSF2))
        {
        	int gain;

			nsf_params->eFilterParam = MSP_VNF_PARAM_USER;
			nsf_params->eHalfFilterParam = MSP_VNF_PARAM_USER;

			if (IssAlg_capt2AGetAEWBVendor() == AEWB_ID_TI)
				gain = ti2a_output_params.sensorGain * ti2a_output_params.ipipe_awb_gain.dGain / 256;
			else
				gain = 48000;

        	switch (params->strength)
			{
				default:
				case MSP_VNF_STRENGTH_AUTO:
	                MSP_Memcpy(tnf_params, &tnf3_default_params, sizeof(*tnf_params));
					nsf_params->tnf3TS = 11;
					nsf_params->rgnFilterVal = nsf_default_params;

	                if (gain < 32000)
	                {
	                    MSP_Memcpy(tnf_params, &tnf3_weak_params, sizeof(*tnf_params));
						nsf_params->tnf3TS = 7;
						nsf_params->rgnFilterVal = nsf_weak_params;
	                }
	                else if (gain > 64000)
	                {
	                    MSP_Memcpy(tnf_params, &tnf3_strong_params, sizeof(*tnf_params));
						nsf_params->tnf3TS = 15;
	                    nsf_params->rgnFilterVal = nsf_strong_params;
	                }
					break;

				case MSP_VNF_STRENGTH_LOW:
					MSP_Memcpy(tnf_params, &tnf3_weak_params, sizeof(*tnf_params));
					nsf_params->tnf3TS = 7;
					nsf_params->rgnFilterVal = nsf_weak_params;
					break;

				case MSP_VNF_STRENGTH_MIDDLE:
	                MSP_Memcpy(tnf_params, &tnf3_default_params, sizeof(*tnf_params));
					nsf_params->tnf3TS = 11;
					nsf_params->rgnFilterVal = nsf_default_params;
					break;

				case MSP_VNF_STRENGTH_HIGH:
					MSP_Memcpy(tnf_params, &tnf3_strong_params, sizeof(*tnf_params));
					nsf_params->tnf3TS = 15;
					nsf_params->rgnFilterVal = nsf_strong_params;
					break;
            }
        }

#ifdef VNF_USE_DEFAULT_PARAMS
        pVnfQueryParam->bDisablePipeDown = MSP_FALSE;
        pVnfQueryParam->eMspRotation = MSP_CONFIG_ROT_0;

        ldc_params->tStartXY.unStartX = 0;
        ldc_params->tStartXY.unStartY = 0;

        lens_distortion_params->unLensCentreX = 959;
        lens_distortion_params->unLensCentreY = 538;

        MSP_Memcpy(affine_params, &vnf_affine_default_params,
               sizeof(MSP_VNF_AFFINE_PARAMS));

        lut_params->unLdcKhl = vnf_tamron_ldckhl;
        lut_params->unLdcKhr = vnf_tamron_ldckhr;
        lut_params->unLdcKvl = vnf_tamron_ldckvl;
        lut_params->unLdcKvu = vnf_tamron_ldckvu;
        lut_params->unLdcRth = vnf_tamron_Rth;
        lut_params->ucRightShiftBits = vnf_tamron_rightShiftBits;
        lut_params->punLdcLutTable = (MSP_U16 *) vnf_ldc_lut_tamron;
#endif
    }

    if (!params->bTnfEn)
    {
        tnf_params->unMotionThreshold = 0;
        tnf_params->eTnfAlphaLUTParam = MSP_VNF_PARAM_DISABLE;
    }
    else
    {
        tnf_params->eTnfAlphaLUTParam = MSP_VNF_PARAM_USER;
    }

    if (!params->bSnfEn)
    {
        nsf_params->eFilterParam = MSP_VNF_PARAM_DISABLE;
        nsf_params->eEdgeParam = MSP_VNF_PARAM_DISABLE;
        nsf_params->eShdParam = MSP_VNF_PARAM_DISABLE;
        nsf_params->eDesatParam = MSP_VNF_PARAM_DISABLE;
    }

	if (!params->bLdcEn)
		lut_params->punLdcLutTable = (MSP_U16 *) vnf_ldc_lut_0;
	else
		lut_params->punLdcLutTable = (MSP_U16 *) vnf_ldc_lut_tamron;

    pCompPrv->inBuffVnf.nPortIndex = MSP_VNF_INPUT_PORT;
    pCompPrv->inBuffVnf.unBufSize[0] =
        pVnfQueryParam->ulInputStride * params->ulInputHeight;
    pCompPrv->inBuffVnf.unBufStride[0] = pVnfQueryParam->ulInputStride;

    pCompPrv->outBuffVnf.nPortIndex = MSP_VNF_OUTPUT_PORT;
    pCompPrv->outBuffVnf.unNumOfCompBufs = 2;
    pCompPrv->outBuffVnf.unBufStride[0] = pVnfQueryParam->ulOutputStrideLuma;
    pCompPrv->outBuffVnf.unBufStride[1] = pVnfQueryParam->ulOutputStrideChroma;

    pCompPrv->prevInBuffVnf.nPortIndex = MSP_VNF_INPUT_PORT;
    pCompPrv->prevInBuffVnf.unNumOfCompBufs = 2;
    pCompPrv->prevInBuffVnf.unBufStride[0] = pVnfQueryParam->ulOutputStrideLuma;
    pCompPrv->prevInBuffVnf.unBufStride[1] =
        pVnfQueryParam->ulOutputStrideChroma;


    //Testing --Venu
    //nsf_params->eNsfSet =
      //MSP_VNF_NSF_DISABLE;//
      //MSP_VNF_NSF_CHROMA_ONLY;
      //MSP_VNF_NSF_LUMA_CHROMA;
    //Vps_rprintf("  Mode of Operation is   %d \n", nsf_params->eNsfSet);


    switch (pVnfQueryParam->eInputFormat)
    {
        case MSP_VNF_YUV_FORMAT_YCBCR422:
            pCompPrv->inBuffVnf.unNumOfCompBufs = 1;
            break;
        case MSP_VNF_YUV_FORMAT_YCBCR420:
            pCompPrv->inBuffVnf.unNumOfCompBufs = 2;
            pCompPrv->inBuffVnf.unBufSize[1] =
                pVnfQueryParam->ulInputStrideChroma *
                (params->ulInputHeight / 2);
            pCompPrv->inBuffVnf.unBufStride[1] =
                pVnfQueryParam->ulInputStrideChroma;
            break;
    }

    pCompPrv->outBuffVnf.unBufSize[0] =
        pVnfQueryParam->ulOutputStrideLuma *
        (params->ulInputHeight);
    pCompPrv->outBuffVnf.unBufSize[1] =
        pVnfQueryParam->ulOutputStrideChroma *
        ((params->ulInputHeight) / 2);

    pCompPrv->prevInBuffVnf.unBufSize[0] = pCompPrv->outBuffVnf.unBufSize[0];
    pCompPrv->prevInBuffVnf.unBufSize[1] = pCompPrv->outBuffVnf.unBufSize[1];

    pCompPrv->prevInBuffVnf.unBufOffset[0] =
        pVnfQueryParam->ulOutputStrideLuma * 32;
    pCompPrv->prevInBuffVnf.unBufOffset[1] =
        pVnfQueryParam->ulOutputStrideChroma * 32;

#ifdef VNF_EXTERNAL_RESOURCES_HANDLE
    status = MSP_VNF_acquireResources(pCompPrv->hVnfMSP);
    GOTO_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
#endif

    /* Open the MSP VNF component */
    status = MSP_open(pCompPrv->hVnfMSP, (MSP_PTR *) pVnfQueryParam);
    GOTO_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    VNFLINK_INFO_LOG_RT
        ("VNF: MSP_VNF_Engine_Open LDC %d TNF %d SNF %d!!!\n",
         params->bLdcEn, params->bTnfEn, params->bSnfEn);

    return status;
}

/* ===================================================================
 *  @func     MSP_VNF_Engine_Close
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
Int32 MSP_VNF_Engine_Close(MSP_COMPONENT_TYPE * jComponent)
{
    MSP_APPDATA_VNF *pCompPrv;

    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    GOTO_EXIT_IF(NULL == jComponent, MSP_ERROR_NULLPTR);
    pCompPrv = jComponent->pCompPrivate;

    /* Close the MSP VNF component */
    GOTO_EXIT_IF((MSP_close(pCompPrv->hVnfMSP)) != MSP_ERROR_NONE,
                 MSP_ERROR_FAIL);

#ifdef VNF_EXTERNAL_RESOURCES_HANDLE
    status = MSP_VNF_releaseResources(pCompPrv->hVnfMSP);
    GOTO_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
#endif

    return status;
}

/* ===================================================================
 *  @func     MSP_VNF_Engine_DeInit
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
Int32 MSP_VNF_Engine_DeInit(MSP_COMPONENT_TYPE * jComponent)
{
    MSP_APPDATA_VNF *pCompPrv;

    GOTO_EXIT_IF(NULL == jComponent, MSP_ERROR_NULLPTR);
    pCompPrv = jComponent->pCompPrivate;

    /* De-initialize the MSP VNF component */
    GOTO_EXIT_IF((MSP_deInit(pCompPrv->hVnfMSP)) != MSP_ERROR_NONE,
                 MSP_ERROR_FAIL);

    /* De-initialize the IP Run */
    IP_Run_Deinit();

    MSP_MEMFREE(pCompPrv->vnfQueryParam.ptLdcParams->
                ptLensDistortionParams->ptLutParams);
    MSP_MEMFREE(pCompPrv->vnfQueryParam.ptLdcParams->ptLensDistortionParams);
    MSP_MEMFREE(pCompPrv->vnfQueryParam.ptLdcParams->ptAffineParams);
    MSP_MEMFREE(pCompPrv->vnfQueryParam.ptLdcParams);
    MSP_MEMFREE(pCompPrv->vnfQueryParam.ptNsfParams);
    MSP_MEMFREE(pCompPrv->vnfQueryParam.ptTnfParams);

    Semaphore_delete(&pCompPrv->pProcEndSem);

    MSP_MEMFREE(jComponent->pCompPrivate);
    MSP_MEMFREE(jComponent);

    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     MSP_VNF_ProcessFrame
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
Int32 MSP_VNF_ProcessFrame(MSP_COMPONENT_TYPE * jComponent,
                           FVID2_Frame * prevImageBuffer,
                           FVID2_Frame * currentImageBuffer,
                           FVID2_Frame * outputImageBuffer, unsigned int startX, unsigned int startY, unsigned int lumaPitch, unsigned int chromaPitch)
{
    MSP_APPDATA_VNF *pCompPrv;
	Int32 sStartX = startX,sStartY = startY,lumaOffset,chromaOffset;

    /* prevImageBuffer could be NULL for the first frame */
    GOTO_EXIT_IF(NULL == currentImageBuffer, MSP_ERROR_NULLPTR);
    GOTO_EXIT_IF(NULL == outputImageBuffer, MSP_ERROR_NULLPTR);
    GOTO_EXIT_IF(NULL == jComponent, MSP_ERROR_NULLPTR);
    pCompPrv = jComponent->pCompPrivate;

    if (prevImageBuffer)
    {
        pCompPrv->prevInBuffVnf.pBuf[0] = prevImageBuffer->addr[1][0];
        pCompPrv->prevInBuffVnf.pBuf[1] = prevImageBuffer->addr[1][1];
    }
    else
    {
        /* No previous image for the first frame */
        pCompPrv->prevInBuffVnf.pBuf[0] = NULL;
        pCompPrv->prevInBuffVnf.pBuf[1] = NULL;
    }
	//Vps_printf("[VNF]  startX = %d startY = %d lumaPitch = %d chromaPitch = %d \n", startX, startY, lumaPitch, chromaPitch);
	lumaOffset = (sStartY * lumaPitch) + sStartX;
    pCompPrv->inBuffVnf.pBuf[0] = currentImageBuffer->addr[0][0];
    pCompPrv->inBuffVnf.pBuf[0] = (unsigned char *) ((unsigned int) currentImageBuffer->addr[0][0] + lumaOffset);
		
    switch (pCompPrv->vnfQueryParam.eInputFormat)
    {
        case MSP_VNF_YUV_FORMAT_YCBCR422:
            pCompPrv->inBuffVnf.pBuf[1] = NULL;
            break;
        case MSP_VNF_YUV_FORMAT_YCBCR420:
			chromaOffset = (sStartY/2 * chromaPitch) + sStartX;
            pCompPrv->inBuffVnf.pBuf[1] = currentImageBuffer->addr[0][1];
            pCompPrv->inBuffVnf.pBuf[1] = (unsigned char *) ((unsigned int) currentImageBuffer->addr[0][1] + chromaOffset);
            break;
    }
	
    pCompPrv->outBuffVnf.pBuf[0] = outputImageBuffer->addr[1][0];
    pCompPrv->outBuffVnf.pBuf[1] = outputImageBuffer->addr[1][1];

    GOTO_EXIT_IF(MSP_ERROR_NONE !=
                 MSP_process((MSP_HANDLE) pCompPrv->hVnfMSP,
                             &pCompPrv->prevInBuffVnf, &pCompPrv->inBuffVnf),
                 MSP_ERROR_FAIL);

    GOTO_EXIT_IF(MSP_ERROR_NONE !=
                 MSP_process((MSP_HANDLE) pCompPrv->hVnfMSP, NULL,
                             &pCompPrv->outBuffVnf), MSP_ERROR_FAIL);

    GOTO_EXIT_IF(Semaphore_pend(pCompPrv->pProcEndSem, 1000) == FALSE,
                 MSP_ERROR_FAIL);

    return MSP_ERROR_NONE;
}
