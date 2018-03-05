/** ==================================================================
 *  @file   vaLink_alg.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_c6xdsp/va/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "vaLink_priv.h"
#include <mcfw/interfaces/link_api/encLink.h>
#include <mcfw/interfaces/link_api/swosdLink.h>
#include <mcfw/interfaces/link_api/cameraLink.h>

#ifdef LPR_DEMO
	#include <mcfw/src_bios6/links_c6xdsp/alg/VI_LPRAlg/inc/LPRAlg.h>
	extern int nLPRNotInitialized;
	#define MAXCHANNEL 1
	extern unsigned char *pucRecogResult[MAXCHANNEL];
	extern unsigned char isLPRResult[MAXCHANNEL];
	unsigned char ucRecogResult[8];
	int siLPRShowCount;	
#endif

#define VA_PROFILE

#define UTILS_PRINTF_MAX_ARG1_LEN (500)
char utils_strArg1[UTILS_PRINTF_MAX_ARG1_LEN];

Int32 utils_printf(const char *format, ...)
{
    va_list va;
    int len;
    va_start(va, format);
    len = vsnprintf(utils_strArg1, UTILS_PRINTF_MAX_ARG1_LEN, format, va);
    utils_strArg1[len] = '\0';
    Vps_printf("DMVAL:%s",utils_strArg1);
    va_end(va);
    return 0 ;
}


/* ===================================================================
 *  @func     VaLink_algGetGuiPrm
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
Int32 VaLink_algGetGuiPrm(VaLink_Obj *pObj)
{
    UInt32 i;
	VaLink_algObj *pAlgObj = &pObj->algObj;

    pAlgObj->detectMode = (DMVALdetectMode)pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc;

    if(pAlgObj->detectMode & DMVAL_DETECTMODE_TAMPER)
	{
	    pAlgObj->tamperSensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaCTDPrm.dmvaCfgTDSensitivity;
		pAlgObj->resetCounter        = pObj->guiPrm.dmvaCTDPrm.dmvaCfgTDResetTime;
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_TRIP)
	{
	    pAlgObj->direction     = (DMVALdirection)pObj->guiPrm.dmvaTZPrm.dmvaTZDirection;
		pAlgObj->presentAdjust = pObj->guiPrm.dmvaTZPrm.dmvaTZPresentAdjust;
		pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaTZPrm.dmvaTZSensitivity;

		pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMinWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMinHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->maxPersonSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMaxWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->maxPersonSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMaxHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->minVehicleSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMinWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->minVehicleSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMinHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->maxVehicleSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMaxWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->maxVehicleSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMaxHeight * pAlgObj->params.maxHeight)/100;

        pAlgObj->numPolygons = 2;

		/* First Polygon */
		pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
		pAlgObj->polygon[0].polygonID = 1;
		pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;
		pAlgObj->polygon[0].valid     = 1;
		pAlgObj->polygon[0].type      = DMVAL_TZ_1A;
		pAlgObj->polygon[0].numPoints = pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides;

		for(i = 0;i < pAlgObj->polygon[0].numPoints;i ++)
		{
		    pAlgObj->polygon[0].pt[i].x = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_x[i] * pAlgObj->params.maxWidth)/100;
			pAlgObj->polygon[0].pt[i].y = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_y[i] * pAlgObj->params.maxHeight)/100;
		}

		/* Closing ROI by making last and first vertx same */
		pAlgObj->polygon[0].numPoints ++;
		pAlgObj->polygon[0].pt[i].x = pAlgObj->polygon[0].pt[0].x;
		pAlgObj->polygon[0].pt[i].y = pAlgObj->polygon[0].pt[0].y;

		/* Second Polygon */
		pAlgObj->polygon[1].size      = sizeof(DMVALpolygon);
		pAlgObj->polygon[1].polygonID = 2;
		pAlgObj->polygon[1].dir       = DMVAL_DIRECTION_UNSPECIFIED;
		pAlgObj->polygon[1].valid     = 1;
		pAlgObj->polygon[1].type      = DMVAL_TZ_1B;
		pAlgObj->polygon[1].numPoints = pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_ROI_numSides;

		for(i = 0;i < pAlgObj->polygon[1].numPoints;i ++)
		{
		    pAlgObj->polygon[1].pt[i].x = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_x[i] * pAlgObj->params.maxWidth)/100;
			pAlgObj->polygon[1].pt[i].y = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_y[i] * pAlgObj->params.maxHeight)/100;
		}

		/* Closing ROI by making last and first vertx same */
		pAlgObj->polygon[1].numPoints ++;
		pAlgObj->polygon[1].pt[i].x = pAlgObj->polygon[1].pt[0].x;
		pAlgObj->polygon[1].pt[i].y = pAlgObj->polygon[1].pt[0].y;
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_COUNT)
	{
	    pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaOCPrm.dmvaOCSensitivity;

		pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaOCPrm.dmvaOCObjectWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaOCPrm.dmvaOCObjectHeight * pAlgObj->params.maxHeight)/100;

        if(pObj->guiPrm.dmvaOCPrm.dmvaOCDirection == 1)
		{
            pAlgObj->direction = DMVAL_DIRECTION_ZONEB2A;
		}
		else if(pObj->guiPrm.dmvaOCPrm.dmvaOCDirection == 2)
			 {
                 pAlgObj->direction = DMVAL_DIRECTION_ZONEA2B;
		     }
			 else
			 {
                 pAlgObj->direction = DMVAL_DIRECTION_UNSPECIFIED;
             }

		pAlgObj->orientation = pObj->guiPrm.dmvaOCPrm.dmvaOCLeftRight;
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_IMD)
	{
	    pAlgObj->presentAdjust = pObj->guiPrm.dmvaIMDPrm.dmvaIMDPresentAdjust;
		pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaIMDPrm.dmvaIMDSensitivity;

		pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMinWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMinHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->maxPersonSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMaxWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->maxPersonSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMaxHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->minVehicleSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMinWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->minVehicleSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMinHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->maxVehicleSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->maxVehicleSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->numPolygons = pObj->guiPrm.dmvaIMDPrm.dmvaIMDNumZones;

		/* IMD ZONE 1 */
		pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
		pAlgObj->polygon[0].polygonID = 1;
		if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x1)
		{
	        pAlgObj->polygon[0].valid = 1;
		}
		else
		{
		    pAlgObj->polygon[0].valid = 0;
		}
		pAlgObj->polygon[0].type      = DMVAL_IMD;
		pAlgObj->polygon[0].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_ROI_numSides;
		pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;

		for(i = 0;i < pAlgObj->polygon[0].numPoints;i ++)
		{
		    pAlgObj->polygon[0].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_x[i] * pAlgObj->params.maxWidth)/100;
			pAlgObj->polygon[0].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_y[i] * pAlgObj->params.maxHeight)/100;
		}

		/* Closing ROI by making last and first vertx same */
		pAlgObj->polygon[0].numPoints ++;
		pAlgObj->polygon[0].pt[i].x = pAlgObj->polygon[0].pt[0].x;
		pAlgObj->polygon[0].pt[i].y = pAlgObj->polygon[0].pt[0].y;

		/* IMD ZONE 2 */
		pAlgObj->polygon[1].size      = sizeof(DMVALpolygon);
		pAlgObj->polygon[1].polygonID = 2;
		if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x2)
		{
	        pAlgObj->polygon[1].valid = 1;
		}
		else
		{
		    pAlgObj->polygon[1].valid = 0;
		}
		pAlgObj->polygon[1].type      = DMVAL_IMD;
		pAlgObj->polygon[1].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_ROI_numSides;
		pAlgObj->polygon[1].dir       = DMVAL_DIRECTION_UNSPECIFIED;

		for(i = 0;i < pAlgObj->polygon[1].numPoints;i ++)
		{
		    pAlgObj->polygon[1].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_x[i] * pAlgObj->params.maxWidth)/100;
			pAlgObj->polygon[1].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_y[i] * pAlgObj->params.maxHeight)/100;
		}

		/* Closing ROI by making last and first vertx same */
		pAlgObj->polygon[1].numPoints ++;
		pAlgObj->polygon[1].pt[i].x = pAlgObj->polygon[1].pt[0].x;
		pAlgObj->polygon[1].pt[i].y = pAlgObj->polygon[1].pt[0].y;

		/* IMD ZONE 3 */
		pAlgObj->polygon[2].size      = sizeof(DMVALpolygon);
		pAlgObj->polygon[2].polygonID = 3;
		if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x4)
		{
	        pAlgObj->polygon[2].valid = 1;
		}
		else
		{
		    pAlgObj->polygon[2].valid = 0;
		}
		pAlgObj->polygon[2].type      = DMVAL_IMD;
		pAlgObj->polygon[2].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_ROI_numSides;
		pAlgObj->polygon[2].dir       = DMVAL_DIRECTION_UNSPECIFIED;

		for(i = 0;i < pAlgObj->polygon[2].numPoints;i ++)
		{
		    pAlgObj->polygon[2].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_x[i] * pAlgObj->params.maxWidth)/100;
			pAlgObj->polygon[2].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_y[i] * pAlgObj->params.maxHeight)/100;
		}

		/* Closing ROI by making last and first vertx same */
		pAlgObj->polygon[2].numPoints ++;
		pAlgObj->polygon[2].pt[i].x = pAlgObj->polygon[2].pt[0].x;
		pAlgObj->polygon[2].pt[i].y = pAlgObj->polygon[2].pt[0].y;

		/* IMD ZONE 4 */
		pAlgObj->polygon[3].size      = sizeof(DMVALpolygon);
		pAlgObj->polygon[3].polygonID = 4;
		if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x8)
		{
	        pAlgObj->polygon[3].valid = 1;
		}
		else
		{
		    pAlgObj->polygon[3].valid = 0;
		}
		pAlgObj->polygon[3].type      = DMVAL_IMD;
		pAlgObj->polygon[3].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_ROI_numSides;
		pAlgObj->polygon[3].dir       = DMVAL_DIRECTION_UNSPECIFIED;

		for(i = 0;i < pAlgObj->polygon[3].numPoints;i ++)
		{
		    pAlgObj->polygon[3].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_x[i] * pAlgObj->params.maxWidth)/100;
			pAlgObj->polygon[3].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_y[i] * pAlgObj->params.maxHeight)/100;
		}

		/* Closing ROI by making last and first vertx same */
		pAlgObj->polygon[3].numPoints ++;
		pAlgObj->polygon[3].pt[i].x = pAlgObj->polygon[3].pt[0].x;
		pAlgObj->polygon[3].pt[i].y = pAlgObj->polygon[3].pt[0].y;
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_SMETA)
	{
	    pAlgObj->presentAdjust = pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPresentAdjust;
		pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaSMETAPrm.dmvaSMETASensitivity;

		pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->maxPersonSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->maxPersonSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->minVehicleSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->minVehicleSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->maxVehicleSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxWidth * pAlgObj->params.maxWidth)/100;
		pAlgObj->maxVehicleSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxHeight * pAlgObj->params.maxHeight)/100;

		pAlgObj->numPolygons = pObj->guiPrm.dmvaSMETAPrm.dmvaSMETANumZones;

        if(pAlgObj->numPolygons > 1)
		{
            pAlgObj->numPolygons = 1;
		}

		pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
		pAlgObj->polygon[0].polygonID = 1;
		pAlgObj->polygon[0].valid     = 1;
		pAlgObj->polygon[0].type      = DMVAL_META;
		pAlgObj->polygon[0].numPoints = pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_ROI_numSides;
		pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;

		for(i = 0;i < pAlgObj->polygon[0].numPoints;i ++)
		{
		    pAlgObj->polygon[0].pt[i].x = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_x[i] * pAlgObj->params.maxWidth)/100;
			pAlgObj->polygon[0].pt[i].y = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_y[i] * pAlgObj->params.maxHeight)/100;
		}

		/* Closing ROI by making last and first vertx same */
		pAlgObj->polygon[0].numPoints ++;
		pAlgObj->polygon[0].pt[i].x = pAlgObj->polygon[0].pt[0].x;
		pAlgObj->polygon[0].pt[i].y = pAlgObj->polygon[0].pt[0].y;
	}

	return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algSetDmvaPrm
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
Int32 VaLink_algSetDmvaPrm(VaLink_Obj *pObj)
{
    UInt32 i,j;
	Int32 libStatus;
	Int16 vehiclesInFOV = 0,peopleInFOV = 0;
	Int16 detectionRate,tzMaxSpatialOverlap,tzMinTemporalOverlap;
	Int16 tamper_blockup_th,tamper_blockdown_th,tamper_bkg_frm_interval;
	Int16 enableIgnoreMask;
	Int16 ocMaxSpatialOverlap,ocMinTemporalOverlap;
	Int16 smetaTrackerEnable;
	Int16 verboseLevel;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;

	if(pObj->guiPrm.dmvaexptsmetatrackerenable == 1)
	{
		// velocity and obj id are also present
		pAlgObj->maxNoObj = (VALINK_METADATASIZE - 28)/12;
	}
	else
	{
		// no velocity and obj id
		pAlgObj->maxNoObj = (VALINK_METADATASIZE - 28)/8;
	}

    verboseLevel = 0;
	libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_DBG_VERBOSE_LEVEL,&(verboseLevel),1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_DBG_VERBOSE_LEVEL\n",__func__,libStatus);
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_TAMPER)
	{
	    tamper_blockup_th       = 32 * 0.5; 	// Default value from VA test bench
		tamper_blockdown_th     = 32 * 0.5; 	// Default value from VA test bench
		tamper_bkg_frm_interval = 10;       	// Default value from VA test bench

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting CTD Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : ---------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : Tamper Sensitiveness    = %d !!!\n", Clock_getTicks(),pAlgObj->tamperSensitiveness);
		Vps_printf(" %d: VA   : Reset Counter           = %d frames !!!\n", Clock_getTicks(),pAlgObj->resetCounter);
		Vps_printf(" %d: VA   : tamper_blockup_th       = %d  !!!\n", Clock_getTicks(),tamper_blockup_th);
		Vps_printf(" %d: VA   : tamper_blockdown_th     = %d  !!!\n", Clock_getTicks(),tamper_blockdown_th);
		Vps_printf(" %d: VA   : tamper_bkg_frm_interval = %d  !!!\n", Clock_getTicks(),tamper_bkg_frm_interval);
#endif

	    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TAMPERSENSITIVITY,(Int16*)&(pAlgObj->tamperSensitiveness), 1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TAMPERSENSITIVITY\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TAMPER_BCK_RESET_INTERVAL,&(pAlgObj->resetCounter), 1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TAMPER_BCK_RESET_INTERVAL\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TAMPER_BLOCKUP_TH,&(tamper_blockup_th), 1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TAMPER_BLOCKUP_TH\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TAMPER_BLOCKDOWN_TH,&(tamper_blockdown_th), 1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TAMPER_BLOCKDOWN_TH\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TAMPER_BACKGROUND_INTERVAL,&(tamper_bkg_frm_interval), 1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TAMPER_BACKGROUND_INTERVAL\n",__func__,libStatus);
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting CTD Parameters DONE !!!\n", Clock_getTicks());
#endif
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_TRIP)
	{
		for(i = 0; i < pAlgObj->numPolygons; i++)
		{
			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[i]));
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting First ROI\n",__func__,libStatus);
			}
		}

		vehiclesInFOV = 0;
		if ((pAlgObj->presentAdjust & 0x4) == 0x4)
		{
			vehiclesInFOV = 1;
		}

		peopleInFOV = 0;
		if ((pAlgObj->presentAdjust & 0x8) == 0x8)
		{
			peopleInFOV = 1;
		}

		detectionRate = VA_LINK_DETECTIONRATE;

		tzMaxSpatialOverlap  = 10;    	// Default value from VA test bench
		tzMinTemporalOverlap = 1000;   	// Default value from VA test bench

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting TZ Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness        = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : Min Person Size      = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Max Person Size      = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxPersonSize[0],pAlgObj->maxPersonSize[1]);
		Vps_printf(" %d: VA   : Min Vehicle Size     = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minVehicleSize[0],pAlgObj->minVehicleSize[1]);
		Vps_printf(" %d: VA   : Max Vehicle Size     = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxVehicleSize[0],pAlgObj->maxVehicleSize[1]);
		Vps_printf(" %d: VA   : VehiclesInFOV        = %d !!!\n", Clock_getTicks(),vehiclesInFOV);
		Vps_printf(" %d: VA   : PeopleInFOV          = %d !!!\n", Clock_getTicks(),peopleInFOV);
		Vps_printf(" %d: VA   : Direction            = %d !!!\n", Clock_getTicks(),pAlgObj->direction);
		Vps_printf(" %d: VA   : Detection Interval   = %d !!!\n", Clock_getTicks(),detectionRate);
		Vps_printf(" %d: VA   : tzMaxSpatialOverlap  = %d !!!\n", Clock_getTicks(),tzMaxSpatialOverlap);
		Vps_printf(" %d: VA   : tzMinTemporalOverlap = %d !!!\n", Clock_getTicks(),tzMinTemporalOverlap);
		Vps_printf(" %d: VA   : ZONE A params      - Id = %d,valid = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
		           pAlgObj->polygon[0].polygonID,pAlgObj->polygon[0].valid,pAlgObj->polygon[0].type,
				   pAlgObj->polygon[0].numPoints);
		for(i = 0;i < pAlgObj->polygon[0].numPoints;i++)
		{
            Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
			           pAlgObj->polygon[0].pt[i].x,pAlgObj->polygon[0].pt[i].y);
		}

		Vps_printf(" %d: VA   : ZONE B params      - Id = %d,valid = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
		           pAlgObj->polygon[1].polygonID,pAlgObj->polygon[1].valid,pAlgObj->polygon[1].type,
				   pAlgObj->polygon[1].numPoints);
		for(i = 0;i < pAlgObj->polygon[1].numPoints;i++)
		{
            Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
			           pAlgObj->polygon[1].pt[i].x,pAlgObj->polygon[1].pt[i].y);
		}
#endif

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXPERSONSIZE,pAlgObj->maxPersonSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXPERSONSIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINVEHICLESIZE,pAlgObj->minVehicleSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINVEHICLESIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXVEHICLESIZE,pAlgObj->maxVehicleSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXVEHICLESIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_VEHICLES,&vehiclesInFOV,1);	// Not there in VA test bench
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_VEHICLES\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_PEOPLE,&peopleInFOV,1);		// Not there in VA test bench
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_PEOPLE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TZDIRECTION,(Int16*)&(pAlgObj->direction),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TZDIRECTION\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_DETECTIONRATE,&(detectionRate),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_DETECTIONRATE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO,&(tzMaxSpatialOverlap),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OVERLAPPED_EVENTS_INTERVAL,&(tzMinTemporalOverlap),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OVERLAPPED_EVENTS_INTERVAL\n",__func__,libStatus);
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting TZ Parameters DONE !!!\n", Clock_getTicks());
#endif
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_COUNT)
	{
	    detectionRate = VA_LINK_DETECTIONRATE;

		ocMaxSpatialOverlap  = 25;			// Default value from VA test bench
		ocMinTemporalOverlap = 1000;		// Default value from VA test bench

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting OC Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness        = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : Min Person Size      = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Orientation          = %d !!!\n", Clock_getTicks(),pAlgObj->orientation);
		Vps_printf(" %d: VA   : Direction            = %d !!!\n", Clock_getTicks(),pAlgObj->direction);
		Vps_printf(" %d: VA   : Detection Interval   = %d !!!\n", Clock_getTicks(),detectionRate);
		Vps_printf(" %d: VA   : ocMaxSpatialOverlap  = %d !!!\n", Clock_getTicks(),ocMaxSpatialOverlap);
		Vps_printf(" %d: VA   : ocMinTemporalOverlap = %d !!!\n", Clock_getTicks(),ocMinTemporalOverlap);
#endif

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OCORIENTATION,&(pAlgObj->orientation),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OCORIENTATION\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OCDIRECTION, (Int16*)&(pAlgObj->direction),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OCDIRECTION\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_DETECTIONRATE,&(detectionRate),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_DETECTIONRATE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO,&(ocMaxSpatialOverlap),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OVERLAPPED_EVENTS_INTERVAL,&(ocMinTemporalOverlap),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OVERLAPPED_EVENTS_INTERVAL\n",__func__,libStatus);
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting OC Parameters DONE !!!\n", Clock_getTicks());
#endif
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_IMD)
	{
		for(i = 0; i < pAlgObj->numPolygons; i++)
		{
			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[i]));
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting %d ROI\n",__func__,libStatus,i);
			}
		}

		vehiclesInFOV = 0;
		if ((pAlgObj->presentAdjust & 0x4) == 0x4)
		{
			vehiclesInFOV = 1;
		}

		peopleInFOV = 0;
		if ((pAlgObj->presentAdjust & 0x8) == 0x8)
		{
			peopleInFOV = 1;
		}

		enableIgnoreMask = 0;   // Default value from VA test bench

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting IMD Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness      = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : Min Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Max Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxPersonSize[0],pAlgObj->maxPersonSize[1]);
		Vps_printf(" %d: VA   : Min Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minVehicleSize[0],pAlgObj->minVehicleSize[1]);
		Vps_printf(" %d: VA   : Max Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxVehicleSize[0],pAlgObj->maxVehicleSize[1]);
		Vps_printf(" %d: VA   : VehiclesInFOV      = %d !!!\n", Clock_getTicks(),vehiclesInFOV);
		Vps_printf(" %d: VA   : PeopleInFOV        = %d !!!\n", Clock_getTicks(),peopleInFOV);
		Vps_printf(" %d: VA   : enableIgnoreMask   = %d !!!\n", Clock_getTicks(),enableIgnoreMask);
		Vps_printf(" %d: VA   : Num of Zones       = %d !!!\n", Clock_getTicks(),pAlgObj->numPolygons);
        for(j = 0;j < pAlgObj->numPolygons;j++)
        {
			Vps_printf(" %d: VA   : ZONE %d params      - Id = %d,valid = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
					   j,pAlgObj->polygon[j].polygonID,pAlgObj->polygon[j].valid,pAlgObj->polygon[j].type,
					   pAlgObj->polygon[j].numPoints);

			for(i = 0;i < pAlgObj->polygon[j].numPoints;i++)
			{
				Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
						   pAlgObj->polygon[j].pt[i].x,pAlgObj->polygon[j].pt[i].y);
			}
        }
#endif

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXPERSONSIZE,pAlgObj->maxPersonSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXPERSONSIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINVEHICLESIZE,pAlgObj->minVehicleSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINVEHICLESIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXVEHICLESIZE,pAlgObj->maxVehicleSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXVEHICLESIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl, DMVAL_PARAM_INVIEW_VEHICLES,&vehiclesInFOV,1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_VEHICLES\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl, DMVAL_PARAM_INVIEW_PEOPLE,&peopleInFOV,1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_PEOPLE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl, DMVAL_PARAM_IGNOREZONE,&enableIgnoreMask,1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_IGNOREZONE\n",__func__,libStatus);
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting IMD Parameters DONE !!!\n", Clock_getTicks());
#endif
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_SMETA)
	{
	    detectionRate      = VA_LINK_DETECTIONRATE;
		smetaTrackerEnable = pObj->guiPrm.dmvaexptsmetatrackerenable;

		for(i = 0; i < pAlgObj->numPolygons; i++)
		{
			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[i]));
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting %d ROI\n",__func__,libStatus,i);
			}
		}

		vehiclesInFOV = 0;
		if ((pAlgObj->presentAdjust & 0x4) == 0x4)
		{
			vehiclesInFOV = 1;
		}

		peopleInFOV = 0;
		if ((pAlgObj->presentAdjust & 0x8) == 0x8)
		{
			peopleInFOV = 1;
		}

	    enableIgnoreMask = 0;   // Default value from VA test bench

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting SMETA Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness      = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : SMETA tracker      = %d !!!\n", Clock_getTicks(),smetaTrackerEnable);
		Vps_printf(" %d: VA   : Detection Interval = %d !!!\n", Clock_getTicks(),detectionRate);
		Vps_printf(" %d: VA   : Min Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Max Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxPersonSize[0],pAlgObj->maxPersonSize[1]);
		Vps_printf(" %d: VA   : Min Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minVehicleSize[0],pAlgObj->minVehicleSize[1]);
		Vps_printf(" %d: VA   : Max Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxVehicleSize[0],pAlgObj->maxVehicleSize[1]);
		Vps_printf(" %d: VA   : VehiclesInFOV      = %d !!!\n", Clock_getTicks(),vehiclesInFOV);
		Vps_printf(" %d: VA   : PeopleInFOV        = %d !!!\n", Clock_getTicks(),peopleInFOV);
		Vps_printf(" %d: VA   : enableIgnoreMask   = %d !!!\n", Clock_getTicks(),enableIgnoreMask);
		Vps_printf(" %d: VA   : Num of Zones       = %d !!!\n", Clock_getTicks(),pAlgObj->numPolygons);
        for(j = 0;j < pAlgObj->numPolygons;j++)
        {
			Vps_printf(" %d: VA   : ZONE %d params      - Id = %d,valid = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
					   j,pAlgObj->polygon[j].polygonID,pAlgObj->polygon[j].valid,pAlgObj->polygon[j].type,
					   pAlgObj->polygon[j].numPoints);

			for(i = 0;i < pAlgObj->polygon[j].numPoints;i++)
			{
				Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
						   pAlgObj->polygon[j].pt[i].x,pAlgObj->polygon[j].pt[i].y);
			}
        }
#endif

	    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SMD_TRACKER,&smetaTrackerEnable,1);
				if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SMD_TRACKER\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_DETECTIONRATE,&(detectionRate),1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_DETECTIONRATE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXPERSONSIZE,pAlgObj->maxPersonSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXPERSONSIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINVEHICLESIZE,pAlgObj->minVehicleSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINVEHICLESIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXVEHICLESIZE,pAlgObj->maxVehicleSize,2);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXVEHICLESIZE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_VEHICLES,&vehiclesInFOV,1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_VEHICLES\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_PEOPLE,&peopleInFOV,1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_PEOPLE\n",__func__,libStatus);
		}

		libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_IGNOREZONE,&enableIgnoreMask,1);
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_IGNOREZONE\n",__func__,libStatus);
		}


#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting SMETA Parameters DONE !!!\n", Clock_getTicks());
#endif
	}

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algSetTZPrm
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
Int32 VaLink_algSetTZPrm(VaLink_Obj *pObj)
{
	Int32 libStatus;
	Int16 vehiclesInFOV = 0,peopleInFOV = 0;
	UInt32 i,setRoi;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;

    if(pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_TRIP)
	{
        // sensitiveness
		if(pObj->prevGuiPrm.dmvaTZPrm.dmvaTZSensitivity != pObj->guiPrm.dmvaTZPrm.dmvaTZSensitivity)
		{
		    pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaTZPrm.dmvaTZSensitivity;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
			}
		}

		// Direction
		if(pObj->prevGuiPrm.dmvaTZPrm.dmvaTZDirection != pObj->guiPrm.dmvaTZPrm.dmvaTZDirection)
		{
		    pAlgObj->direction = (DMVALdirection)pObj->guiPrm.dmvaTZPrm.dmvaTZDirection;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TZDIRECTION,(Int16*)&(pAlgObj->direction),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TZDIRECTION\n",__func__,libStatus);
			}
		}

		// Person Min Size
        if((pObj->prevGuiPrm.dmvaTZPrm.dmvaTZPersonMinWidth  != pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMinWidth) ||
 		   (pObj->prevGuiPrm.dmvaTZPrm.dmvaTZPersonMinHeight != pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMinHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMinWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMinHeight * pAlgObj->params.maxHeight)/100;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
			}
		}

		// Vehicle Min Size
        if((pObj->prevGuiPrm.dmvaTZPrm.dmvaTZVehicleMinWidth  != pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMinWidth) ||
 		   (pObj->prevGuiPrm.dmvaTZPrm.dmvaTZVehicleMinHeight != pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMinHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMinWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMinHeight * pAlgObj->params.maxHeight)/100;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINVEHICLESIZE,pAlgObj->minVehicleSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINVEHICLESIZE\n",__func__,libStatus);
			}
		}

		// Person Max Size
        if((pObj->prevGuiPrm.dmvaTZPrm.dmvaTZPersonMaxWidth  != pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMaxWidth) ||
 		   (pObj->prevGuiPrm.dmvaTZPrm.dmvaTZPersonMaxHeight != pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMaxHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMaxWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZPersonMaxHeight * pAlgObj->params.maxHeight)/100;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXPERSONSIZE,pAlgObj->maxPersonSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXPERSONSIZE\n",__func__,libStatus);
			}
		}

		// Vehicle Max Size
        if((pObj->prevGuiPrm.dmvaTZPrm.dmvaTZVehicleMaxWidth  != pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMaxWidth) ||
 		   (pObj->prevGuiPrm.dmvaTZPrm.dmvaTZVehicleMaxHeight != pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMaxHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMaxWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaTZPrm.dmvaTZVehicleMaxHeight * pAlgObj->params.maxHeight)/100;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXVEHICLESIZE,pAlgObj->maxVehicleSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXVEHICLESIZE\n",__func__,libStatus);
			}
		}

		// Present Adjust
		if(pObj->prevGuiPrm.dmvaTZPrm.dmvaTZPresentAdjust != pObj->guiPrm.dmvaTZPrm.dmvaTZPresentAdjust)
		{
		    pAlgObj->presentAdjust = pObj->guiPrm.dmvaTZPrm.dmvaTZPresentAdjust;

			vehiclesInFOV = 0;
			if ((pAlgObj->presentAdjust & 0x4) == 0x4)
			{
				vehiclesInFOV = 1;
			}

			peopleInFOV = 0;
			if ((pAlgObj->presentAdjust & 0x8) == 0x8)
			{
				peopleInFOV = 1;
			}

			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_VEHICLES,&vehiclesInFOV,1);	// Not there in VA test bench
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_VEHICLES\n",__func__,libStatus);
			}

			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_PEOPLE,&peopleInFOV,1);		// Not there in VA test bench
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_PEOPLE\n",__func__,libStatus);
			}
		}

        // First ROI
		setRoi = 0;
        if(pObj->prevGuiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides != pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides)
		{
		    setRoi = 1;
		}

		if(setRoi == 0)
		{
			for(i = 0;i < pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides;i ++)
			{
			    if((pObj->prevGuiPrm.dmvaTZPrm.dmvaTZZone1_x[i] != pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_x[i]) ||
				   (pObj->prevGuiPrm.dmvaTZPrm.dmvaTZZone1_y[i] != pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_y[i]))
				{
                    setRoi = 1;
				}
			}
		}

		if(setRoi == 1)
		{
			pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
			pAlgObj->polygon[0].polygonID = 1;
			pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;
			pAlgObj->polygon[0].valid     = 1;
			pAlgObj->polygon[0].type      = DMVAL_TZ_1A;
			pAlgObj->polygon[0].numPoints = pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides;

			for(i = 0;i < pAlgObj->polygon[0].numPoints;i ++)
			{
				pAlgObj->polygon[0].pt[i].x = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_x[i] * pAlgObj->params.maxWidth)/100;
				pAlgObj->polygon[0].pt[i].y = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_y[i] * pAlgObj->params.maxHeight)/100;
			}

			/* Closing ROI by making last and first vertx same */
			pAlgObj->polygon[0].numPoints ++;
			pAlgObj->polygon[0].pt[i].x = pAlgObj->polygon[0].pt[0].x;
			pAlgObj->polygon[0].pt[i].y = pAlgObj->polygon[0].pt[0].y;

			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[0]));
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting First ROI\n",__func__,libStatus);
			}
		}

        // Second ROI
		setRoi = 0;
        if(pObj->prevGuiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides != pObj->guiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides)
		{
		    setRoi = 1;
		}

		if(setRoi == 0)
		{
			for(i = 0;i < pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_ROI_numSides;i ++)
			{
			    if((pObj->prevGuiPrm.dmvaTZPrm.dmvaTZZone2_x[i] != pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_x[i]) ||
				   (pObj->prevGuiPrm.dmvaTZPrm.dmvaTZZone2_y[i] != pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_y[i]))
				{
                    setRoi = 1;
				}
			}
		}

		if(setRoi == 1)
		{
			pAlgObj->polygon[1].size      = sizeof(DMVALpolygon);
			pAlgObj->polygon[1].polygonID = 2;
			pAlgObj->polygon[1].dir       = DMVAL_DIRECTION_UNSPECIFIED;
			pAlgObj->polygon[1].valid     = 1;
			pAlgObj->polygon[1].type      = DMVAL_TZ_1B;
			pAlgObj->polygon[1].numPoints = pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_ROI_numSides;

			for(i = 0;i < pAlgObj->polygon[1].numPoints;i ++)
			{
				pAlgObj->polygon[1].pt[i].x = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_x[i] * pAlgObj->params.maxWidth)/100;
				pAlgObj->polygon[1].pt[i].y = (pObj->guiPrm.dmvaTZPrm.dmvaTZZone2_y[i] * pAlgObj->params.maxHeight)/100;
			}

			/* Closing ROI by making last and first vertx same */
			pAlgObj->polygon[1].numPoints ++;
			pAlgObj->polygon[1].pt[i].x = pAlgObj->polygon[1].pt[0].x;
			pAlgObj->polygon[1].pt[i].y = pAlgObj->polygon[1].pt[0].y;

			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[1]));
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting Second ROI\n",__func__,libStatus);
			}
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting NEW TZ Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness        = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : Min Person Size      = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Max Person Size      = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxPersonSize[0],pAlgObj->maxPersonSize[1]);
		Vps_printf(" %d: VA   : Min Vehicle Size     = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minVehicleSize[0],pAlgObj->minVehicleSize[1]);
		Vps_printf(" %d: VA   : Max Vehicle Size     = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxVehicleSize[0],pAlgObj->maxVehicleSize[1]);
		Vps_printf(" %d: VA   : VehiclesInFOV        = %d !!!\n", Clock_getTicks(),vehiclesInFOV);
		Vps_printf(" %d: VA   : PeopleInFOV          = %d !!!\n", Clock_getTicks(),peopleInFOV);
		Vps_printf(" %d: VA   : Direction            = %d !!!\n", Clock_getTicks(),pAlgObj->direction);
		Vps_printf(" %d: VA   : ZONE A params      - Id = %d,valid = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
		           pAlgObj->polygon[0].polygonID,pAlgObj->polygon[0].valid,pAlgObj->polygon[0].type,
				   pAlgObj->polygon[0].numPoints);
		for(i = 0;i < pAlgObj->polygon[0].numPoints;i++)
		{
            Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
			           pAlgObj->polygon[0].pt[i].x,pAlgObj->polygon[0].pt[i].y);
		}

		Vps_printf(" %d: VA   : ZONE B params      - Id = %d,dir = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
		           pAlgObj->polygon[1].polygonID,pAlgObj->polygon[1].dir,pAlgObj->polygon[1].type,
				   pAlgObj->polygon[1].numPoints);
		for(i = 0;i < pAlgObj->polygon[1].numPoints;i++)
		{
            Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
			           pAlgObj->polygon[1].pt[i].x,pAlgObj->polygon[1].pt[i].y);
		}
#endif
	}

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algSetMainPrm
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
Int32 VaLink_algSetMainPrm(VaLink_Obj *pObj)
{
    Int32 status;
	Int32 libStatus;
    VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;
	SwosdLink_vaStrPrm vaStrPrm;

	if(pObj->prevGuiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc != pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc)
	{
		status = VaLink_algGetGuiPrm(pObj);
		UTILS_assert(status == FVID2_SOK);

		/* DMVAL init with new detect mode */
		libStatus = DMVAL_initModule(pDmvaHndl,pAlgObj->detectMode);
		UTILS_assert(libStatus == DMVAL_NO_ERROR);

		status = VaLink_algSetDmvaPrm(pObj);
		UTILS_assert(status == FVID2_SOK);

		if(!(pObj->prevGuiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_COUNT) ||
		   !(pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_COUNT))
		{
			pAlgObj->objectCount  = 0;
		}

		/* If previous alg is OC and current alg is not OC then we need to turn off SWOSD OC count value */
		if((pObj->prevGuiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_COUNT) &&
		   !(pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_COUNT))
		{
			vaStrPrm.disFlag = 0;
			vaStrPrm.vaString[0] = '\0';			

			System_linkControl(pObj->createArgs.swosdLinkId,
							   SWOSDLINK_CMD_VASTRING,
							   &vaStrPrm,
							   sizeof(SwosdLink_vaStrPrm),TRUE);
		}	
		
		pAlgObj->vaAlgChanged = 1;
		pAlgObj->maxProcessTime = 0;
	}

	return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algSetCTDPrm
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
Int32 VaLink_algSetCTDPrm(VaLink_Obj *pObj)
{
    Int32 libStatus;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;

    if(pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_TAMPER)
	{
        // Sensitiveness
		if(pObj->prevGuiPrm.dmvaCTDPrm.dmvaCfgTDSensitivity != pObj->guiPrm.dmvaCTDPrm.dmvaCfgTDSensitivity)
		{
		    pAlgObj->tamperSensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaCTDPrm.dmvaCfgTDSensitivity;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->tamperSensitiveness),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
			}
		}

		// Reset Time
        if(pObj->prevGuiPrm.dmvaCTDPrm.dmvaCfgTDResetTime != pObj->guiPrm.dmvaCTDPrm.dmvaCfgTDResetTime)
		{
		    pAlgObj->resetCounter  = pObj->guiPrm.dmvaCTDPrm.dmvaCfgTDResetTime;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TAMPER_BCK_RESET_INTERVAL,(Int16*)&(pAlgObj->resetCounter),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TAMPER_BCK_RESET_INTERVAL\n",__func__,libStatus);
			}

		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting NEW CTD Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : ---------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : Tamper Sensitiveness    = %d !!!\n", Clock_getTicks(),pAlgObj->tamperSensitiveness);
		Vps_printf(" %d: VA   : Reset Counter           = %d frames !!!\n", Clock_getTicks(),pAlgObj->resetCounter);
#endif
	}

    return FVID2_SOK;
}

 /* ===================================================================
 *  @func     VaLink_algSetOCPrm
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
Int32 VaLink_algSetOCPrm(VaLink_Obj *pObj)
{
	Int32 libStatus;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;

	if(pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_COUNT)
	{
        // Sensitiveness
		if(pObj->prevGuiPrm.dmvaOCPrm.dmvaOCSensitivity != pObj->guiPrm.dmvaOCPrm.dmvaOCSensitivity)
		{
		    pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaOCPrm.dmvaOCSensitivity;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
			}
		}

		// Direction
        if(pObj->prevGuiPrm.dmvaOCPrm.dmvaOCDirection != pObj->guiPrm.dmvaOCPrm.dmvaOCDirection)
		{
			if(pObj->guiPrm.dmvaOCPrm.dmvaOCDirection == 1)
			{
				pAlgObj->direction = DMVAL_DIRECTION_ZONEB2A;
			}
			else if(pObj->guiPrm.dmvaOCPrm.dmvaOCDirection == 2)
				 {
					 pAlgObj->direction = DMVAL_DIRECTION_ZONEA2B;
				 }
				 else
				 {
					 pAlgObj->direction = DMVAL_DIRECTION_UNSPECIFIED;
				 }

			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OCDIRECTION,(Int16*)&(pAlgObj->direction),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OCDIRECTION\n",__func__,libStatus);
			}
		}

        // Object Size
        if((pObj->prevGuiPrm.dmvaOCPrm.dmvaOCObjectWidth  != pObj->guiPrm.dmvaOCPrm.dmvaOCObjectWidth) ||
		   (pObj->prevGuiPrm.dmvaOCPrm.dmvaOCObjectHeight != pObj->guiPrm.dmvaOCPrm.dmvaOCObjectHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaOCPrm.dmvaOCObjectWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaOCPrm.dmvaOCObjectHeight * pAlgObj->params.maxHeight)/100;
            libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
			}
		}

        // Orientation
        if(pObj->prevGuiPrm.dmvaOCPrm.dmvaOCDirection != pObj->guiPrm.dmvaOCPrm.dmvaOCDirection)
		{
            pAlgObj->orientation = pObj->guiPrm.dmvaOCPrm.dmvaOCLeftRight;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OCORIENTATION,&(pAlgObj->orientation),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OCORIENTATION\n",__func__,libStatus);
			}
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting NEW OC Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness        = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : Min Person Size      = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Orientation          = %d !!!\n", Clock_getTicks(),pAlgObj->orientation);
		Vps_printf(" %d: VA   : Direction            = %d !!!\n", Clock_getTicks(),pAlgObj->direction);
#endif
	}

    return FVID2_SOK;
}

 /* ===================================================================
 *  @func     VaLink_algSetSMETAPrm
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
Int32 VaLink_algSetSMETAPrm(VaLink_Obj *pObj)
{
	Int32 libStatus;
	Int16 vehiclesInFOV = 0,peopleInFOV = 0;
	UInt32 i,j,setRoi;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;

	if(pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_SMETA)
	{
        // Sensitiveness
		if(pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETASensitivity != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETASensitivity)
		{
		    pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaSMETAPrm.dmvaSMETASensitivity;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
			}
		}

        // Min Person Size
		if((pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinWidth  != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinWidth) ||
		   (pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinHeight != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
			}
		}

        // Min Vehicle Size
		if((pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinWidth  != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinWidth) ||
		   (pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinHeight != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinHeight))
		{
			pAlgObj->minVehicleSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minVehicleSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINVEHICLESIZE,pAlgObj->minVehicleSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINVEHICLESIZE\n",__func__,libStatus);
			}
		}

        // Max Person Size
		if((pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxWidth  != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxWidth) ||
		   (pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxHeight != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxHeight))
		{
			pAlgObj->maxPersonSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->maxPersonSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXPERSONSIZE,pAlgObj->maxPersonSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXPERSONSIZE\n",__func__,libStatus);
			}
		}

        // Max Vehicle Size
		if((pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxWidth  != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxWidth) ||
		   (pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxHeight != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxHeight))
		{
			pAlgObj->maxVehicleSize[0] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->maxVehicleSize[1] = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXVEHICLESIZE,pAlgObj->maxVehicleSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXVEHICLESIZE\n",__func__,libStatus);
			}
		}

        // Present Adjust
		if(pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAPresentAdjust != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPresentAdjust)
		{
		    pAlgObj->presentAdjust = pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAPresentAdjust;

			vehiclesInFOV = 0;
			if ((pAlgObj->presentAdjust & 0x4) == 0x4)
			{
				vehiclesInFOV = 1;
			}

			peopleInFOV = 0;
			if ((pAlgObj->presentAdjust & 0x8) == 0x8)
			{
				peopleInFOV = 1;
			}

			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_VEHICLES,&vehiclesInFOV,1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_VEHICLES\n",__func__,libStatus);
			}

			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_PEOPLE,&peopleInFOV,1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_PEOPLE\n",__func__,libStatus);
			}
		}

		// ROI
		setRoi = 0;
        if(pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAZone1_ROI_numSides != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_ROI_numSides)
		{
		    setRoi = 1;
		}

		if(setRoi == 0)
		{
			for(i = 0;i < pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_ROI_numSides;i ++)
			{
			    if((pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAZone1_x[i] != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_x[i]) ||
				   (pObj->prevGuiPrm.dmvaSMETAPrm.dmvaSMETAZone1_y[i] != pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_y[i]))
				{
                    setRoi = 1;
				}
			}
		}

		if(setRoi == 1)
		{
			pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
			pAlgObj->polygon[0].polygonID = 1;
			pAlgObj->polygon[0].valid     = 1;
			pAlgObj->polygon[0].type      = DMVAL_META;
			pAlgObj->polygon[0].numPoints = pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_ROI_numSides;
			pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;

			for(i = 0;i < pAlgObj->polygon[0].numPoints;i ++)
			{
				pAlgObj->polygon[0].pt[i].x = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_x[i] * pAlgObj->params.maxWidth)/100;
				pAlgObj->polygon[0].pt[i].y = (pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAZone1_y[i] * pAlgObj->params.maxHeight)/100;
			}

			/* Closing ROI by making last and first vertx same */
			pAlgObj->polygon[0].numPoints ++;
			pAlgObj->polygon[0].pt[i].x = pAlgObj->polygon[0].pt[0].x;
			pAlgObj->polygon[0].pt[i].y = pAlgObj->polygon[0].pt[0].y;

			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[0]));
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
			}
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting NEW SMETA Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness      = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : Min Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Max Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxPersonSize[0],pAlgObj->maxPersonSize[1]);
		Vps_printf(" %d: VA   : Min Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minVehicleSize[0],pAlgObj->minVehicleSize[1]);
		Vps_printf(" %d: VA   : Max Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxVehicleSize[0],pAlgObj->maxVehicleSize[1]);
		Vps_printf(" %d: VA   : VehiclesInFOV      = %d !!!\n", Clock_getTicks(),vehiclesInFOV);
		Vps_printf(" %d: VA   : PeopleInFOV        = %d !!!\n", Clock_getTicks(),peopleInFOV);
		Vps_printf(" %d: VA   : Num of Zones       = %d !!!\n", Clock_getTicks(),pAlgObj->numPolygons);
        for(j = 0;j < pAlgObj->numPolygons;j++)
        {
			Vps_printf(" %d: VA   : ZONE %d params      - Id = %d,valid = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
					   j,pAlgObj->polygon[j].polygonID,pAlgObj->polygon[j].valid,pAlgObj->polygon[j].type,
					   pAlgObj->polygon[j].numPoints);

			for(i = 0;i < pAlgObj->polygon[j].numPoints;i++)
			{
				Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
						   pAlgObj->polygon[j].pt[i].x,pAlgObj->polygon[j].pt[i].y);
			}
        }
#endif
	}

    return FVID2_SOK;
}

  /* ===================================================================
 *  @func     VaLink_algSetIMDPrm
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
Int32 VaLink_algSetIMDPrm(VaLink_Obj *pObj)
{
	Int32 libStatus;
	Int16 vehiclesInFOV = 0,peopleInFOV = 0;
	UInt32 i,j,setRoi,polygonCnt;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;

	if(pObj->guiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc & DMVAL_DETECTMODE_IMD)
	{
        // Sensitiveness
		if(pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDSensitivity != pObj->guiPrm.dmvaIMDPrm.dmvaIMDSensitivity)
		{
		    pAlgObj->sensitiveness = (DMVALsensitivity)pObj->guiPrm.dmvaIMDPrm.dmvaIMDSensitivity;
			libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
			}
		}

        // Min Person Size
		if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDPersonMinWidth  != pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMinWidth) ||
		   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDPersonMinHeight != pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMinHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMinWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMinHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
			}
		}

        // Min Vehicle Size
		if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMinWidth  != pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMinWidth) ||
		   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMinHeight != pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMinHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMinWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMinHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINVEHICLESIZE,pAlgObj->minVehicleSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINVEHICLESIZE\n",__func__,libStatus);
			}
		}

        // Max Person Size
		if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDPersonMaxWidth  != pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMaxWidth) ||
		   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDPersonMaxHeight != pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMaxHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMaxWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDPersonMaxHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXPERSONSIZE,pAlgObj->maxPersonSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXPERSONSIZE\n",__func__,libStatus);
			}
		}

        // Max Vehicle Size
		if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxWidth  != pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxWidth) ||
		   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxHeight != pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxHeight))
		{
			pAlgObj->minPersonSize[0] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxWidth * pAlgObj->params.maxWidth)/100;
			pAlgObj->minPersonSize[1] = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxHeight * pAlgObj->params.maxHeight)/100;
		    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXVEHICLESIZE,pAlgObj->maxVehicleSize,2);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXVEHICLESIZE\n",__func__,libStatus);
			}
		}

		// Present Adjust
		if(pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDPresentAdjust != pObj->guiPrm.dmvaIMDPrm.dmvaIMDPresentAdjust)
		{
		    pAlgObj->presentAdjust = pObj->guiPrm.dmvaIMDPrm.dmvaIMDPresentAdjust;

			vehiclesInFOV = 0;
			if ((pAlgObj->presentAdjust & 0x4) == 0x4)
			{
				vehiclesInFOV = 1;
			}

			peopleInFOV = 0;
			if ((pAlgObj->presentAdjust & 0x8) == 0x8)
			{
				peopleInFOV = 1;
			}

            libStatus = DMVAL_setParameter(pDmvaHndl, DMVAL_PARAM_INVIEW_VEHICLES,&vehiclesInFOV,1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_VEHICLES\n",__func__,libStatus);
			}

            libStatus = DMVAL_setParameter(pDmvaHndl, DMVAL_PARAM_INVIEW_PEOPLE,&peopleInFOV,1);
			if(libStatus != DMVAL_NO_ERROR)
			{
			    Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_PEOPLE\n",__func__,libStatus);
			}
		}

		pAlgObj->numPolygons = pObj->guiPrm.dmvaIMDPrm.dmvaIMDNumZones;
		polygonCnt = 0;

		// First ROI
		if(polygonCnt < pAlgObj->numPolygons)
		{
			setRoi = 0;
			if(pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone1_ROI_numSides != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_ROI_numSides)
			{
				setRoi = 1;
			}

			if(setRoi == 0)
			{
				for(i = 0;i < pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_ROI_numSides;i ++)
				{
					if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone1_x[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_x[i]) ||
					   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone1_y[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_y[i]))
					{
						setRoi = 1;
					}
				}
			}

            if(setRoi == 0)
			{
				if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x1) != (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x1))
				{
					setRoi = 1;
				}
			}

			if(setRoi == 1)
			{
				pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
				pAlgObj->polygon[0].polygonID = 1;
				if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x1)
				{
					pAlgObj->polygon[0].valid = 1;
				}
				else
				{
					pAlgObj->polygon[0].valid = 0;
				}
				pAlgObj->polygon[0].type      = DMVAL_IMD;
				pAlgObj->polygon[0].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_ROI_numSides;
				pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;

				for(i = 0;i < pAlgObj->polygon[0].numPoints;i ++)
				{
					pAlgObj->polygon[0].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_x[i] * pAlgObj->params.maxWidth)/100;
					pAlgObj->polygon[0].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone1_y[i] * pAlgObj->params.maxHeight)/100;
				}

				/* Closing ROI by making last and first vertx same */
				pAlgObj->polygon[0].numPoints ++;
				pAlgObj->polygon[0].pt[i].x = pAlgObj->polygon[0].pt[0].x;
				pAlgObj->polygon[0].pt[i].y = pAlgObj->polygon[0].pt[0].y;

				libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[0]));
				if(libStatus != DMVAL_NO_ERROR)
				{
					Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
				}
			}
		}
		else
		{
			pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
			pAlgObj->polygon[0].polygonID = 1;
			pAlgObj->polygon[0].valid     = 0;
			pAlgObj->polygon[0].type      = DMVAL_IMD;
			pAlgObj->polygon[0].numPoints = 0;
			pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;

			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[0]));
			if(libStatus != DMVAL_NO_ERROR)
			{
				Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
			}
		}

		polygonCnt ++;

		// Second ROI
		if(polygonCnt < pAlgObj->numPolygons)
		{
			setRoi = 0;
			if(pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone2_ROI_numSides != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_ROI_numSides)
			{
				setRoi = 1;
			}

			if(setRoi == 0)
			{
				for(i = 0;i < pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_ROI_numSides;i ++)
				{
					if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone2_x[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_x[i]) ||
					   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone2_y[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_y[i]))
					{
						setRoi = 1;
					}
				}
			}

            if(setRoi == 0)
			{
				if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x2) != (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x2))
				{
					setRoi = 1;
				}
			}

			if(setRoi == 1)
			{
				pAlgObj->polygon[1].size      = sizeof(DMVALpolygon);
				pAlgObj->polygon[1].polygonID = 2;
				if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x2)
				{
					pAlgObj->polygon[1].valid = 1;
				}
				else
				{
					pAlgObj->polygon[1].valid = 0;
				}
				pAlgObj->polygon[1].type      = DMVAL_IMD;
				pAlgObj->polygon[1].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_ROI_numSides;
				pAlgObj->polygon[1].dir       = DMVAL_DIRECTION_UNSPECIFIED;

				for(i = 0;i < pAlgObj->polygon[1].numPoints;i ++)
				{
					pAlgObj->polygon[1].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_x[i] * pAlgObj->params.maxWidth)/100;
					pAlgObj->polygon[1].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone2_y[i] * pAlgObj->params.maxHeight)/100;
				}

				/* Closing ROI by making last and first vertx same */
				pAlgObj->polygon[1].numPoints ++;
				pAlgObj->polygon[1].pt[i].x = pAlgObj->polygon[1].pt[0].x;
				pAlgObj->polygon[1].pt[i].y = pAlgObj->polygon[1].pt[0].y;

				libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[1]));
				if(libStatus != DMVAL_NO_ERROR)
				{
					Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
				}
			}
		}
        else
		{
			pAlgObj->polygon[1].size      = sizeof(DMVALpolygon);
			pAlgObj->polygon[1].polygonID = 2;
			pAlgObj->polygon[1].valid     = 0;
			pAlgObj->polygon[1].type      = DMVAL_IMD;
			pAlgObj->polygon[1].numPoints = 0;
			pAlgObj->polygon[1].dir       = DMVAL_DIRECTION_UNSPECIFIED;

			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[1]));
			if(libStatus != DMVAL_NO_ERROR)
			{
				Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
			}
		}

		polygonCnt ++;

		// Third ROI
		if(polygonCnt < pAlgObj->numPolygons)
		{
			setRoi = 0;
			if(pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone3_ROI_numSides != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_ROI_numSides)
			{
				setRoi = 1;
			}

			if(setRoi == 0)
			{
				for(i = 0;i < pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_ROI_numSides;i ++)
				{
					if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone3_x[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_x[i]) ||
					   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone3_y[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_y[i]))
					{
						setRoi = 1;
					}
				}
			}

            if(setRoi == 0)
			{
				if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x4) != (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x4))
				{
					setRoi = 1;
				}
			}

			if(setRoi == 1)
			{
				pAlgObj->polygon[2].size      = sizeof(DMVALpolygon);
				pAlgObj->polygon[2].polygonID = 3;
				if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x4)
				{
					pAlgObj->polygon[2].valid = 1;
				}
				else
				{
					pAlgObj->polygon[2].valid = 0;
				}
				pAlgObj->polygon[2].type      = DMVAL_IMD;
				pAlgObj->polygon[2].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_ROI_numSides;
				pAlgObj->polygon[2].dir       = DMVAL_DIRECTION_UNSPECIFIED;

				for(i = 0;i < pAlgObj->polygon[2].numPoints;i ++)
				{
					pAlgObj->polygon[2].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_x[i] * pAlgObj->params.maxWidth)/100;
					pAlgObj->polygon[2].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone3_y[i] * pAlgObj->params.maxHeight)/100;
				}

				/* Closing ROI by making last and first vertx same */
				pAlgObj->polygon[2].numPoints ++;
				pAlgObj->polygon[2].pt[i].x = pAlgObj->polygon[2].pt[0].x;
				pAlgObj->polygon[2].pt[i].y = pAlgObj->polygon[2].pt[0].y;

				libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[2]));
				if(libStatus != DMVAL_NO_ERROR)
				{
					Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
				}
			}
		}
		else
		{
			pAlgObj->polygon[2].size      = sizeof(DMVALpolygon);
			pAlgObj->polygon[2].polygonID = 3;
			pAlgObj->polygon[2].valid     = 0;
			pAlgObj->polygon[2].type      = DMVAL_IMD;
			pAlgObj->polygon[2].numPoints = 0;
			pAlgObj->polygon[2].dir       = DMVAL_DIRECTION_UNSPECIFIED;

			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[2]));
			if(libStatus != DMVAL_NO_ERROR)
			{
				Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
			}
		}

		polygonCnt ++;

		// Fourth ROI
		if(polygonCnt < pAlgObj->numPolygons)
		{
			setRoi = 0;
			if(pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone4_ROI_numSides != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_ROI_numSides)
			{
				setRoi = 1;
			}

			if(setRoi == 0)
			{
				for(i = 0;i < pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_ROI_numSides;i ++)
				{
					if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone4_x[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_x[i]) ||
					   (pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZone4_y[i] != pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_y[i]))
					{
						setRoi = 1;
					}
				}
			}

            if(setRoi == 0)
			{
				if((pObj->prevGuiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x8) != (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x8))
				{
					setRoi = 1;
				}
			}

			if(setRoi == 1)
			{
				pAlgObj->polygon[3].size      = sizeof(DMVALpolygon);
				pAlgObj->polygon[3].polygonID = 4;
				if(pObj->guiPrm.dmvaIMDPrm.dmvaIMDZoneEnable & 0x8)
				{
					pAlgObj->polygon[3].valid = 1;
				}
				else
				{
					pAlgObj->polygon[3].valid = 0;
				}
				pAlgObj->polygon[3].type      = DMVAL_IMD;
				pAlgObj->polygon[3].numPoints = pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_ROI_numSides;
				pAlgObj->polygon[3].dir       = DMVAL_DIRECTION_UNSPECIFIED;

				for(i = 0;i < pAlgObj->polygon[3].numPoints;i ++)
				{
					pAlgObj->polygon[3].pt[i].x = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_x[i] * pAlgObj->params.maxWidth)/100;
					pAlgObj->polygon[3].pt[i].y = (pObj->guiPrm.dmvaIMDPrm.dmvaIMDZone4_y[i] * pAlgObj->params.maxHeight)/100;
				}

				/* Closing ROI by making last and first vertx same */
				pAlgObj->polygon[3].numPoints ++;
				pAlgObj->polygon[3].pt[i].x = pAlgObj->polygon[3].pt[0].x;
				pAlgObj->polygon[3].pt[i].y = pAlgObj->polygon[3].pt[0].y;

				libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[3]));
				if(libStatus != DMVAL_NO_ERROR)
				{
					Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
				}
			}
		}
		else
		{
			pAlgObj->polygon[3].size      = sizeof(DMVALpolygon);
			pAlgObj->polygon[3].polygonID = 4;
			pAlgObj->polygon[3].valid     = 0;
			pAlgObj->polygon[3].type      = DMVAL_IMD;
			pAlgObj->polygon[3].numPoints = 0;
			pAlgObj->polygon[3].dir       = DMVAL_DIRECTION_UNSPECIFIED;

			libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[3]));
			if(libStatus != DMVAL_NO_ERROR)
			{
				Vps_printf("WARNING:%s:Error %d while setting ROI\n",__func__,libStatus);
			}
		}

#ifdef SYSTEM_DEBUG_VA
		Vps_printf(" %d: VA   : Setting NEW IMD Parameters !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : --------------------- !!!\n", Clock_getTicks());
		Vps_printf(" %d: VA   : sensitiveness      = %d !!!\n", Clock_getTicks(),pAlgObj->sensitiveness);
		Vps_printf(" %d: VA   : Min Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minPersonSize[0],pAlgObj->minPersonSize[1]);
		Vps_printf(" %d: VA   : Max Person Size    = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxPersonSize[0],pAlgObj->maxPersonSize[1]);
		Vps_printf(" %d: VA   : Min Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->minVehicleSize[0],pAlgObj->minVehicleSize[1]);
		Vps_printf(" %d: VA   : Max Vehicle Size   = (%d,%d) !!!\n", Clock_getTicks(),
		           pAlgObj->maxVehicleSize[0],pAlgObj->maxVehicleSize[1]);
		Vps_printf(" %d: VA   : VehiclesInFOV      = %d !!!\n", Clock_getTicks(),vehiclesInFOV);
		Vps_printf(" %d: VA   : PeopleInFOV        = %d !!!\n", Clock_getTicks(),peopleInFOV);
		Vps_printf(" %d: VA   : Num of Zones       = %d !!!\n", Clock_getTicks(),pAlgObj->numPolygons);
        for(j = 0;j < pAlgObj->numPolygons;j++)
        {
			Vps_printf(" %d: VA   : ZONE %d params      - Id = %d,valid = %d,type = %d,numPoints = %d !!!\n", Clock_getTicks(),
					   j,pAlgObj->polygon[j].polygonID,pAlgObj->polygon[j].valid,pAlgObj->polygon[j].type,
					   pAlgObj->polygon[j].numPoints);

			for(i = 0;i < pAlgObj->polygon[j].numPoints;i++)
			{
				Vps_printf(" %d: VA   : 	(%d,%d) !!!\n", Clock_getTicks(),
						   pAlgObj->polygon[j].pt[i].x,pAlgObj->polygon[j].pt[i].y);
			}
        }
#endif
	}

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algSetFrameRate
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
Int32 VaLink_algSetFrameRate(VaLink_Obj *pObj)
{
	VaLink_algObj *pAlgObj = &pObj->algObj;

	if(pAlgObj->inFrameRate != pObj->inFrameRate)
	{
	    pAlgObj->inFrameRate = pObj->inFrameRate;

		if(pAlgObj->inFrameRate < pObj->createArgs.vaFrameRate)
		{
		    pAlgObj->vaFrameRate   = pAlgObj->inFrameRate;
		}
		else
		{
		    pAlgObj->vaFrameRate   = pObj->createArgs.vaFrameRate;
		}

		pAlgObj->frameSkipContext.firstTime       = TRUE;
		pAlgObj->frameSkipContext.inputFrameRate  = pAlgObj->inFrameRate;
		pAlgObj->frameSkipContext.outputFrameRate = pAlgObj->vaFrameRate;
	}

	return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algInitParams
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
Int32 VaLink_algInitParams(VaLink_Obj *pObj)
{
    VaLink_algObj *pAlgObj = &pObj->algObj;

	/* By default the VA algorithm initilialized is TZ */
	memset(&pAlgObj->dmvalHndl,0x00,sizeof(DMVALhandle));

	pAlgObj->dmvalHndl.size = sizeof(DMVALhandle);
	pAlgObj->output.size    = sizeof(DMVALout);
	pAlgObj->params.size    = sizeof(DMVALparams);

	pAlgObj->inFrameRate   = VA_LINK_INFRAMERATE;
	pAlgObj->vaFrameRate   = pObj->createArgs.vaFrameRate;

	pAlgObj->frameSkipContext.firstTime       = TRUE;
	pAlgObj->frameSkipContext.inputFrameRate  = pAlgObj->inFrameRate;
	pAlgObj->frameSkipContext.outputFrameRate = pAlgObj->vaFrameRate;

	pAlgObj->params.detectMode = DMVAL_DETECTMODE_TAMPER | DMVAL_DETECTMODE_IMD  |
	                             DMVAL_DETECTMODE_SMETA  | DMVAL_DETECTMODE_TRIP |
								 DMVAL_DETECTMODE_COUNT;
	pAlgObj->params.maxWidth      = pObj->inQueInfo.chInfo[0].width;
	pAlgObj->params.maxHeight     = pObj->inQueInfo.chInfo[0].height;
	pAlgObj->params.imgType       = DMVAL_IMG_YUV420_PLANARINTERLEAVED;
	pAlgObj->params.edmaChId	  = pObj->algEdmaHndl.dmaHndl.chId;	
	pAlgObj->params.edmaQId       = VA_LINK_EDMA3_QUEUE_ID;	
	pAlgObj->params.edmaBaseAddr  = VA_LINK_EDMA_BASE_ADDR;                       

    pAlgObj->inStride      = pObj->inQueInfo.chInfo[0].pitch[0];
	pAlgObj->direction     = DMVAL_DIRECTION_UNSPECIFIED;
	pAlgObj->sensitiveness = DMVAL_SENSITIVITY_LEVEL_3;

	pAlgObj->minPersonSize[0]  = (VA_LINK_MINPERSONWIDTH * pAlgObj->params.maxWidth)/100;
	pAlgObj->minVehicleSize[0] = (VA_LINK_MINVEHICLEWIDTH * pAlgObj->params.maxWidth)/100;
	pAlgObj->maxPersonSize[0]  = (VA_LINK_MAXPERSONWIDTH * pAlgObj->params.maxWidth)/100;
	pAlgObj->maxVehicleSize[0] = (VA_LINK_MAXVEHICLEWIDTH * pAlgObj->params.maxWidth)/100;
	pAlgObj->minPersonSize[1]  = (VA_LINK_MINPERSONHEIGHT * pAlgObj->params.maxHeight)/100;
	pAlgObj->minVehicleSize[1] = (VA_LINK_MINVEHICLEHEIGHT * pAlgObj->params.maxHeight)/100;
	pAlgObj->maxPersonSize[1]  = (VA_LINK_MAXPERSONHEIGHT * pAlgObj->params.maxHeight)/100;
	pAlgObj->maxVehicleSize[1] = (VA_LINK_MAXVEHICLEHEIGHT * pAlgObj->params.maxHeight)/100;

	pAlgObj->numPolygons = 2;

	/* First Polygon */
	pAlgObj->polygon[0].size      = sizeof(DMVALpolygon);
	pAlgObj->polygon[0].polygonID = 1;
	pAlgObj->polygon[0].valid     = 1;
	pAlgObj->polygon[0].type      = DMVAL_TZ_1A;
	pAlgObj->polygon[0].numPoints = 5;
	pAlgObj->polygon[0].dir       = DMVAL_DIRECTION_UNSPECIFIED;

	pAlgObj->polygon[0].pt[0].x = (15 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[0].pt[0].y = (15 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[0].pt[1].x = (50 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[0].pt[1].y = (15 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[0].pt[2].x = (50 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[0].pt[2].y = (85 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[0].pt[3].x = (15 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[0].pt[3].y = (85 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[0].pt[4].x = pAlgObj->polygon[0].pt[0].x;
	pAlgObj->polygon[0].pt[4].y = pAlgObj->polygon[0].pt[0].y;

	/* Second Polygon */
	pAlgObj->polygon[1].size      = sizeof(DMVALpolygon);
	pAlgObj->polygon[1].polygonID = 2;
	pAlgObj->polygon[1].valid     = 1;
	pAlgObj->polygon[1].type      = DMVAL_TZ_1B;
	pAlgObj->polygon[1].numPoints = 5;
	pAlgObj->polygon[1].dir       = DMVAL_DIRECTION_UNSPECIFIED;

	pAlgObj->polygon[1].pt[0].x = (50 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[1].pt[0].y = (15 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[1].pt[1].x = (85 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[1].pt[1].y = (15 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[1].pt[2].x = (85 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[1].pt[2].y = (85 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[1].pt[3].x = (50 * pAlgObj->params.maxWidth)/100;
	pAlgObj->polygon[1].pt[3].y = (85 * pAlgObj->params.maxHeight)/100;
	pAlgObj->polygon[1].pt[4].x = pAlgObj->polygon[1].pt[0].x;
	pAlgObj->polygon[1].pt[4].y = pAlgObj->polygon[1].pt[0].y;

	pAlgObj->totalProcessTime = 0;
	pAlgObj->processFrCnt     = 0;
	pAlgObj->maxProcessTime   = 0;

	memset(pAlgObj->metaData,0x00,VALINK_METADATASIZE);
	memset(pAlgObj->prevMetaData,0x00,VALINK_METADATASIZE);

	pAlgObj->objectCount  = 0;
	pAlgObj->vaAlgChanged = 1;
	pAlgObj->tamperEventDone = 0;

	pAlgObj->frameTS.clock.tv_sec  = 0;
	pAlgObj->frameTS.clock.tv_usec = 0;

	pAlgObj->eventRecStatus = 0;
	pAlgObj->eventStartTime = 0;
	pAlgObj->startCnt       = 0;

	return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algDmvalCreate
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
Int32 VaLink_algDmvalCreate(VaLink_Obj *pObj)
{
    UInt32 i;
	Int32 libStatus;
	Int16 vehiclesInFOV = 0,peopleInFOV = 0;
	Int16 detectionRate,tzMaxSpatialOverlap,tzMinTemporalOverlap;
	Int16 verboseLevel;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;

#ifndef IPNC_DSP_ON
	RM_SIMCOP_CLIENT_PARAMS stRMClientParams;
    RM_SIMCOP_STATUS eRMStatus = RM_SIMCOP_STATUS_SUCCESS;
#endif	
	
    libStatus = DMVAL_getMemReq(pDmvaHndl,&pAlgObj->params);
	UTILS_assert(libStatus == DMVAL_NO_ERROR);

	pAlgObj->size = pDmvaHndl->memTab[0].size;
	pDmvaHndl->memTab[0].base = Utils_memAlloc_cached(pAlgObj->size,64);
	pAlgObj->ptr = pDmvaHndl->memTab[0].base;
	UTILS_assert(pAlgObj->ptr != NULL);

#ifdef SYSTEM_DEBUG_VA
    Vps_printf(" %d: VA   : MEMALLOC %d bytes for DMVAL memory at addr 0x%X!!!\n",
	           Clock_getTicks(),pAlgObj->size,pAlgObj->ptr);
#endif

    libStatus = DMVAL_create(pDmvaHndl,&pAlgObj->params);
	UTILS_assert(libStatus == DMVAL_NO_ERROR);

	/* DMVAL init is done only for one algo */
	pAlgObj->detectMode = DMVAL_DETECTMODE_TRIP;

	libStatus = DMVAL_initModule(pDmvaHndl,pAlgObj->detectMode);
	UTILS_assert(libStatus == DMVAL_NO_ERROR);

	/* DMVAL set params */
	vehiclesInFOV = 1;
	peopleInFOV   = 1;
	detectionRate = VA_LINK_DETECTIONRATE;

    for(i = 0; i < pAlgObj->numPolygons; i++)
    {
        libStatus = DMVAL_setROI(pDmvaHndl,&(pAlgObj->polygon[i]));
		if(libStatus != DMVAL_NO_ERROR)
		{
			Vps_printf("WARNING:%s:Error %d while setting First ROI\n",__func__,libStatus);
		}
    }

	tzMaxSpatialOverlap  = 10;     // Default value from VA test bench
	tzMinTemporalOverlap = 1000;   // Default value from VA test bench

    verboseLevel = 0;
	libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_DBG_VERBOSE_LEVEL,&(verboseLevel),1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_DBG_VERBOSE_LEVEL\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_SENSITIVITY,(Int16*)&(pAlgObj->sensitiveness),1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_SENSITIVITY\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINPERSONSIZE,pAlgObj->minPersonSize,2);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINPERSONSIZE\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXPERSONSIZE,pAlgObj->maxPersonSize,2);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXPERSONSIZE\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MINVEHICLESIZE,pAlgObj->minVehicleSize,2);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MINVEHICLESIZE\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_MAXVEHICLESIZE,pAlgObj->maxVehicleSize,2);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_MAXVEHICLESIZE\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_VEHICLES,&vehiclesInFOV,1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_VEHICLES\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_INVIEW_PEOPLE,&peopleInFOV,1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_INVIEW_PEOPLE\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_TZDIRECTION,(Int16*)&(pAlgObj->direction),1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_TZDIRECTION\n",__func__,libStatus);
	}

    libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_DETECTIONRATE,&(detectionRate),1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_DETECTIONRATE\n",__func__,libStatus);
	}

	libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO,&(tzMaxSpatialOverlap),1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OVERLAPPED_EVENTS_AREA_RATIO\n",__func__,libStatus);
	}

	libStatus = DMVAL_setParameter(pDmvaHndl,DMVAL_PARAM_OVERLAPPED_EVENTS_INTERVAL,&(tzMinTemporalOverlap),1);
	if(libStatus != DMVAL_NO_ERROR)
	{
		Vps_printf("WARNING:%s:Error %d while setting DMVAL_PARAM_OVERLAPPED_EVENTS_INTERVAL\n",__func__,libStatus);
	}
	
#ifndef IPNC_DSP_ON
    stRMClientParams.nSize         = sizeof(RM_SIMCOP_CLIENT_PARAMS);
    stRMClientParams.pClientHandle = (RM_SIMCOP_CLIENT_HANDLE)pObj;
		 
    /* Get Resource Manager Handle independent of resources And register Client */
    eRMStatus = RM_SIMCOP_RegisterClient(stRMClientParams,(RM_SIMCOP_CLIENT_HANDLE *)&pObj->pRMHandle);
    UTILS_assert(eRMStatus == RM_SIMCOP_STATUS_SUCCESS);	
#endif	
	
	return FVID2_SOK;
}

/* VA ALG create */

/* ===================================================================
 *  @func     VaLink_algCreate
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
Int32 VaLink_algCreate(VaLink_Obj *pObj)
{
    Int32 status;

	System_getEdmaCh(&pObj->algEdmaHndl);	
	System_getEdmaCh(&pObj->copyEdmaHndl);
	
    /***************************
	     VA ALG CREATE HERE
	***************************/
	status = VaLink_algInitParams(pObj);
	UTILS_assert(status == FVID2_SOK);

#ifdef SYSTEM_DEBUG_VA
    Vps_printf(" %d: VA   : ALG Init Done !!!\n", Clock_getTicks());
#endif

    status = VaLink_algDmvalCreate(pObj);
	UTILS_assert(status == FVID2_SOK);
			
#ifdef SYSTEM_DEBUG_VA
    Vps_printf(" %d: VA   : ALG Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Va link copy frames */

/* ===================================================================
 *  @func     VaLink_algCopyFrames
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
Int32 VaLink_algCopyFrames(VaLink_Obj *pObj)
{
    Int32 status;
	Int32 ret = FVID2_EFAIL;
    Int32 frameId;
    UInt32 edmaWidth, edmaHeight;
    FVID2_Frame *pEmptyFrame;
    FVID2_Frame *pFullFrame;
    FVID2_FrameList frameList;
    System_LinkInQueParams *pInQueParams;
	VaLink_algObj *pAlgObj = &pObj->algObj;

    /* Get the full frames from the previous link */
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            if(Utils_doSkipFrame(&pAlgObj->frameSkipContext) == FALSE)
			{
				/* Get the VA empty bufs if any */
				status = Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame,BIOS_NO_WAIT);

				if ((status == FVID2_SOK) && (pEmptyFrame != NULL))
				{
					pFullFrame = frameList.frames[frameId];

					/* copy the frame */
					edmaWidth  = pObj->inQueInfo.chInfo[0].width;
					edmaHeight = pObj->inQueInfo.chInfo[0].height;

					/* copy Y plane */
					DM81XX_EDMA3_setParams(pObj->copyEdmaHndl.dmaHndl.chId, 			// chId
										   VA_LINK_EDMA3_QUEUE_ID,  					// dmaQueue
										   (UInt32)pFullFrame->addr[0][0], 				// srcAddr
										   (UInt32)pEmptyFrame->addr[0][0],				// dstAddr
										   edmaWidth,          							// edmaWidth
										   edmaHeight,         							// edmaHeight
										   edmaWidth,          							// srcLineOffset
										   edmaWidth);         							// dstLineOffset

					/* Trigger the edma transfer */
					DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndl.dmaHndl.chId);

					pEmptyFrame->timeStamp = pFullFrame->timeStamp;

					/* put the buffer into full queue */
					status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
					UTILS_assert(status == FVID2_SOK);

					ret = FVID2_SOK;
				}
			}
        }

        /* Put the full buffers bacl to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }

    return ret;
}

/* ===================================================================
 *  @func     VaLink_algProcessFrames
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
HPTimeStamp VaLink_algGetFrameTS(VaLink_Obj *pObj,UInt32 timeStampMsec)
{
	VaLink_algObj *pAlgObj = &pObj->algObj;
#if 1
    UInt32 interFrameInterval;

	interFrameInterval = (1000000/pAlgObj->vaFrameRate);

	pAlgObj->frameTS.clock.tv_usec += interFrameInterval;

	if(pAlgObj->frameTS.clock.tv_usec >= (interFrameInterval * pAlgObj->vaFrameRate))
	{
	    pAlgObj->frameTS.clock.tv_sec ++;
		pAlgObj->frameTS.clock.tv_usec = 0;
	}
#else
	pAlgObj->frameTS.clock.tv_sec  = (timeStampMsec/1000);
	pAlgObj->frameTS.clock.tv_usec = (timeStampMsec % 1000) * 1000;
#endif

	return (pAlgObj->frameTS);
}

/* ===================================================================
 *  @func     VaLink_algCreateMetaData
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
Int32 VaLink_algCreateMetaData(VaLink_Obj *pObj)
{
    Int32 status;
	UInt32 curTime;
	VaLink_algObj *pAlgObj = &pObj->algObj;
	UInt8 *pMetaBufPtr = pAlgObj->metaData;
	DMVALout *pOutput = &pAlgObj->output;
	UInt8 zone[4];
	UInt16 Vx,Vy;
	UInt32 numObj,i;
	SwosdLink_vaStrPrm vaStrPrm;
	VaLink_EventType evtType = VALINK_EVT_CTD;

    memcpy(pAlgObj->prevMetaData,pAlgObj->metaData,VALINK_METADATASIZE);
	memset(pMetaBufPtr,0x00,VALINK_METADATASIZE);

	// Write the METS fourcc
	*pMetaBufPtr++ = 'M';
	*pMetaBufPtr++ = 'E';
	*pMetaBufPtr++ = 'T';
	*pMetaBufPtr++ = 'S';

	// Write the OBJT fourcc
	*pMetaBufPtr++ = 'O';
	*pMetaBufPtr++ = 'B';
	*pMetaBufPtr++ = 'J';
	*pMetaBufPtr++ = 'T';

	if(pOutput->numObjects > pAlgObj->maxNoObj)
	{
	    numObj = pAlgObj->maxNoObj;
	}
	else
	{
	    numObj = pOutput->numObjects;
	}

	if(((pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData & 0x10) != 0))
	{
		// insert no of objects
		*pMetaBufPtr++ = numObj;

		memset(zone,0x00,4);

		// insert the OBJT meta data
		for(i = 0;i < numObj;i ++)
		{

			*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objBox.xmin * 100)/pAlgObj->params.maxWidth);    // X
			*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objBox.ymin * 100)/pAlgObj->params.maxHeight);    // Y
			*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objBox.width * 100)/pAlgObj->params.maxWidth);   // width
			*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objBox.height * 100)/pAlgObj->params.maxHeight);  // height

			switch(pOutput->objMetadata[i].roiID)
			{
				// roiID 1,2,3 and 4 are used for the IMD zones
				case 1:
					zone[0] = 1;
					break;
				case 2:
					zone[1] = 1;
					break;
				case 3:
					zone[2] = 1;
					break;
				case 4:
					zone[3] = 1;
					break;
			}//switch
		}//for
	}
	else
	{
		// insert no of objects
		*pMetaBufPtr++ = 0;

		memset(zone,0x00,4);

		// insert the OBJT meta data
		for(i = 0;i < numObj;i ++)
		{
			switch(pOutput->objMetadata[i].roiID)
			{
				// roiID 1,2,3 and 4 are used for the IMD zones
				case 1:
					zone[0] = 1;
					break;
				case 2:
					zone[1] = 1;
					break;
				case 3:
					zone[2] = 1;
					break;
				case 4:
					zone[3] = 1;
					break;
			}//switch
		}//for
	}

	vaStrPrm.disFlag = 0;
	vaStrPrm.vaString[0] = '\0';

	// Write the DMVA event type for zone highlighting
	if(pOutput->modeResult & DMVAL_DETECTOR_TRIPZONE)
	{
		// TRIP
		*pMetaBufPtr++ = 'T';
		*pMetaBufPtr++ = 'R';
		*pMetaBufPtr++ = 'I';
		*pMetaBufPtr++ = 'P';

		*pMetaBufPtr++ = 1;

		strcpy((char*)vaStrPrm.vaString,"TRIP ZONE");
		evtType = VALINK_EVT_TZ;
	}

	if(pOutput->modeResult & DMVAL_DETECTOR_IMD)
	{
		// IMD
		*pMetaBufPtr++ = 'I';
		*pMetaBufPtr++ = 'M';
		*pMetaBufPtr++ = 'D';
		*pMetaBufPtr++ = 'T';

		// Write the zone highhlight status
		*pMetaBufPtr++ = zone[0];
		*pMetaBufPtr++ = zone[1];
		*pMetaBufPtr++ = zone[2];
		*pMetaBufPtr++ = zone[3];

		strcpy((char*)vaStrPrm.vaString,"IMD");
		evtType = VALINK_EVT_IMD;
	}

	if(pOutput->modeResult & DMVAL_DETECTOR_COUNTER)
	{
		// CTD
		*pMetaBufPtr++ = 'O';
		*pMetaBufPtr++ = 'B';
		*pMetaBufPtr++ = 'C';
		*pMetaBufPtr++ = 'T';

		*pMetaBufPtr++ = 1;

		pAlgObj->objectCount ++;

		evtType = VALINK_EVT_OC;
	}

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_COUNT)
	{
		vaStrPrm.disFlag = 1;
		sprintf((char*)vaStrPrm.vaString,"OBJECT COUNT : %4d",pAlgObj->objectCount);

		if(pAlgObj->tamperEventDone == 1)
		{
		    pAlgObj->vaAlgChanged    = 1;
			pAlgObj->tamperEventDone = 0;
		}
	}

	if(pOutput->modeResult & DMVAL_DETECTOR_TAMPER)
	{
		// CTD
		*pMetaBufPtr++ = 'C';
		*pMetaBufPtr++ = 'M';
		*pMetaBufPtr++ = 'T';
		*pMetaBufPtr++ = 'D';

		*pMetaBufPtr++ = 1;

		if(pOutput->modeResult & DMVAL_TAMPER_SCENECHANGE)
		{
		    strcpy((char*)vaStrPrm.vaString,"TAMPER (SCENE MODIFIED)");
		}
		else if(pOutput->modeResult & DMVAL_TAMPER_MOVEMENT)
		     {
			     strcpy((char*)vaStrPrm.vaString,"TAMPER (CAMERA MOVE)");
			 }
			 else if(pOutput->modeResult & DMVAL_TAMPER_DARK)
			      {
                      strcpy((char*)vaStrPrm.vaString,"TAMPER (SCENE DARKEN)");
                  }
				  else if(pOutput->modeResult & DMVAL_TAMPER_BRIGHT)
                       {
                           strcpy((char*)vaStrPrm.vaString,"TAMPER (SCENE BRIGHTEN)");
                       }
                       else if(pOutput->modeResult & DMVAL_TAMPER_BLUR)
                            {
                                strcpy((char*)vaStrPrm.vaString,"TAMPER (CAMERA DEFOCUS)");
                            }

		pAlgObj->tamperEventDone = 1;
		evtType = VALINK_EVT_CTD;
	}

	// insert centroid and velocity
	// Write the CDVT fourcc
	*pMetaBufPtr++ = 'C';
	*pMetaBufPtr++ = 'D';
	*pMetaBufPtr++ = 'V';
	*pMetaBufPtr++ = 'T';

	if(((pOutput->modeResult & DMVAL_DETECTOR_SMETA) || (pOutput->modeResult & DMVAL_DETECTOR_IMD)) &&
		(((pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData & 0x4) != 0) || ((pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData & 0x1) != 0)))
	{
		// insert no of objects
		*pMetaBufPtr++ = numObj;

		for(i = 0;i < numObj;i ++)
		{
			// insert centroid
			if(((pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData & 0x4) != 0) ||
			   ((pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData & 0x1) != 0))
			{
				*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objCentroid.x * 100)/pAlgObj->params.maxWidth);    // X
				*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objCentroid.y * 100)/pAlgObj->params.maxHeight);    // Y
			}
			else
			{
				*pMetaBufPtr++ = 0;    // X
				*pMetaBufPtr++ = 0;    // Y
			}

			// insert velocity
			if(((pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData & 0x1) != 0) &&
				(pObj->guiPrm.dmvaexptsmetatrackerenable == 1))
			{
				Vx = pOutput->objMetadata[i].objCentroid.x + (5 * (pOutput->objMetadata[i].objVelocity.x >> 5)); // Velocity value given out by DMVAL is in SQ10.5 format
				Vy = pOutput->objMetadata[i].objCentroid.y + (5 * (pOutput->objMetadata[i].objVelocity.y >> 5)); // Velocity value given out by DMVAL is in SQ10.5 format

				*pMetaBufPtr++ = (UInt8)((Vx * 100)/pAlgObj->params.maxWidth);
				*pMetaBufPtr++ = (UInt8)((Vy * 100)/pAlgObj->params.maxHeight);
			}
			else
			{
				*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objCentroid.x * 100)/pAlgObj->params.maxWidth);   // X
				*pMetaBufPtr++ = (UInt8)((pOutput->objMetadata[i].objCentroid.y * 100)/pAlgObj->params.maxHeight);   // Y
			}
		}
	}
	else
	{
		// insert no of objects
		*pMetaBufPtr++ = 0;
	}

	// insert objectr id
	// Write the OBID fourcc
	*pMetaBufPtr++ = 'O';
	*pMetaBufPtr++ = 'B';
	*pMetaBufPtr++ = 'I';
	*pMetaBufPtr++ = 'D';

	if(((pOutput->modeResult & DMVAL_DETECTOR_SMETA) || (pOutput->modeResult & DMVAL_DETECTOR_IMD)) &&
		(pObj->guiPrm.dmvaexptsmetatrackerenable == 1) &&
		((pObj->guiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData & 0x2) != 0))
	{
		// insert no of objects
		*pMetaBufPtr++ = numObj;

		for(i = 0;i < numObj;i ++)
		{
			*pMetaBufPtr++ = (UInt8)(pOutput->objMetadata[i].objID.data2 >> 24);
			*pMetaBufPtr++ = (UInt8)(pOutput->objMetadata[i].objID.data2 >> 16);
			*pMetaBufPtr++ = (UInt8)(pOutput->objMetadata[i].objID.data2 >> 8);
			*pMetaBufPtr++ = (UInt8)(pOutput->objMetadata[i].objID.data2);
		}
	}
	else
	{
		// insert no of objects
		*pMetaBufPtr++ = 0;
	}

	// Write METE fourCC at the end
	*pMetaBufPtr++ = 'M';
	*pMetaBufPtr++ = 'E';
	*pMetaBufPtr++ = 'T';
	*pMetaBufPtr++ = 'E';

	if(memcmp(pAlgObj->metaData,pAlgObj->prevMetaData,VALINK_METADATASIZE) != 0)
	{
		if(pObj->createArgs.encLinkId != SYSTEM_LINK_ID_INVALID)
		{
			System_linkControl(pObj->createArgs.encLinkId,
							   ENC_LINK_CMD_SET_VA_METADATA,
							   pAlgObj->metaData,
							   VALINK_METADATASIZE,TRUE);
		}
	}

	/* SWOSD VA text */
	if(((pOutput->modeResult != DMVAL_DETECTOR_NONE) && (pOutput->modeResult != DMVAL_DETECTOR_SMETA)) ||
	   (pAlgObj->vaAlgChanged == 1))
	{
		if(pAlgObj->vaAlgChanged == 1)
		{
			pAlgObj->vaAlgChanged = 0;
		}

		if((pObj->createArgs.swosdLinkId != SYSTEM_LINK_ID_INVALID) &&
		   (pOutput->modeResult != DMVAL_DETECTOR_NONE))
		{
			System_linkControl(pObj->createArgs.swosdLinkId,
							   SWOSDLINK_CMD_VASTRING,
							   &vaStrPrm,
							   sizeof(SwosdLink_vaStrPrm),TRUE);
		}
	}

	/* Notify HOST about the event */
	if(((pOutput->modeResult != DMVAL_DETECTOR_NONE) && (pOutput->modeResult != DMVAL_DETECTOR_SMETA)) &&
	   (pAlgObj->eventRecStatus == 0))
	{
		status = Notify_sendEvent(System_getSyslinkProcId(SYSTEM_PROC_HOSTA8), 	// proc id
								  SYSTEM_IPC_NOTIFY_LINE_ID,    				// line id
								  SYSTEM_IPC_NOTIFY_EVENT_ID_APP, 				// event id
								  evtType,    									// payload
								  TRUE);   										// waitclear

		if(status != Notify_S_SUCCESS)
		{
			Vps_printf(" %d: VA   : Send Notify FAILED !!!\n", Clock_getTicks());
		}

		pAlgObj->eventRecStatus = 1;						// Event Recording started
		pAlgObj->eventStartTime = Utils_getCurTimeInMsec();
	}
	else
	{
	    if(pAlgObj->eventRecStatus == 1)
		{
		    curTime = Utils_getCurTimeInMsec();

			if((curTime >= (pAlgObj->eventStartTime + VA_LINK_EVT_RECTIME_MSEC)) ||
               (curTime < pAlgObj->eventStartTime)) // incase msec var wraps around
			{
			    /* Stop the recording */
				status = Notify_sendEvent(System_getSyslinkProcId(SYSTEM_PROC_HOSTA8),  	// proc id
										  SYSTEM_IPC_NOTIFY_LINE_ID,    					// line id
										  SYSTEM_IPC_NOTIFY_EVENT_ID_APP, 					// event id
										  VALINK_EVT_STOP,    								// payload
										  TRUE);   											// waitclear

				if(status != Notify_S_SUCCESS)
				{
					Vps_printf(" %d: VA   : Send Notify FAILED !!!\n", Clock_getTicks());
				}

                pAlgObj->eventRecStatus = 0;
			}
		}
	}

	return FVID2_SOK;
}

/* Va link get focus value */

/* ===================================================================
 *  @func     VaLink_algProcessFrames
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
UInt32 VaLink_algGetFocusValue(VaLink_Obj *pObj)
{
    VaLink_algObj *pAlgObj = &pObj->algObj;
	Iss_CaptFocusValue focusValueSt;
	UInt32 focusValue = VA_LINK_DEFAULT_FOCUSVALUE;

	if(pAlgObj->detectMode & DMVAL_DETECTMODE_TAMPER)
	{
		if(pObj->createArgs.cameraLinkId != SYSTEM_LINK_ID_INVALID)
		{
			focusValueSt.resetFlag = pAlgObj->output.focusValueReset;

			System_linkControl(pObj->createArgs.cameraLinkId,
							   CAMERA_LINK_CMD_GET_FOCUSVALUE,
							   &focusValueSt,
							   sizeof(Iss_CaptFocusValue),TRUE);

            focusValue = focusValueSt.focusValue;
		}
	}

	return focusValue;
 }

/* Va link process frames */

/* ===================================================================
 *  @func     VaLink_algProcessFrames
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
Int32 VaLink_algProcessFrames(VaLink_Obj *pObj)
{
    Int32 status;
    VaLink_algObj *pAlgObj = &pObj->algObj;	
#ifdef LPR_DEMO	
	SwosdLink_vaStrPrm vaStrPrm;
#else
	Int32 libStatus;
	DMVALhandle *pDmvaHndl = &pAlgObj->dmvalHndl;
	UInt32 frameId;
	FVID2_Frame *pEmptyFrame;		
#endif	
	DMVALimage inImage;
    FVID2_Frame *pFullFrame;	
#ifndef IPNC_DSP_ON
	RM_SIMCOP_STATUS eRMStatus = RM_SIMCOP_STATUS_SUCCESS;
	UInt32 bitFieldOfResources;
    RM_SIMCOP_RESOURCE_STATUS resStatus;	
#endif	
	
#ifdef VA_PROFILE
	UInt32 start,end;
#endif

    do
    {
        /* Get the frame from the full queue */
        status = Utils_bufGetFullFrame(&pObj->outFrameQue, &pFullFrame,BIOS_NO_WAIT);

        if ((status == FVID2_SOK) && (pFullFrame != NULL))
        {
			/***************************
				 VA ALG APPLY HERE
			***************************/
            inImage.size		= sizeof(DMVALimage);
			inImage.timeStamp   = VaLink_algGetFrameTS(pObj,pFullFrame->timeStamp);
			inImage.type        = DMVAL_IMG_YUV420_PLANARINTERLEAVED;
			inImage.pixelDepth  = DMVAL_PIXEL_U08;
			inImage.width       = pAlgObj->params.maxWidth;
			inImage.height      = pAlgObj->params.maxHeight;
			inImage.imageStride = pAlgObj->inStride;
			inImage.horzOffset  = 0;
			inImage.vertOffset  = 0;
			inImage.imageData   = pFullFrame->addr[0][0];
            inImage.inputId     = ((VaLink_perFrameConfig*)pFullFrame->perFrameCfg)->bufId;
			inImage.focusValue  = VaLink_algGetFocusValue(pObj);
			inImage.type        = DMVAL_IMG_YUV420_PLANARINTERLEAVED;

#ifdef VA_PROFILE
			start = Utils_getCurTimeInUsec();
#endif

#ifndef IPNC_DSP_ON
			bitFieldOfResources              = RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_HWSEQ | RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DMA;
			resStatus.bitFieldDirtyResources = 0;
			eRMStatus = RM_SIMCOP_AcquireMultiResources((RM_SIMCOP_CLIENT_HANDLE)pObj->pRMHandle,
														 bitFieldOfResources,
														 RM_SIMCOP_ACQUIRE_WAIT,
														 &resStatus);
			UTILS_assert(eRMStatus == RM_SIMCOP_STATUS_SUCCESS);
#endif

			Cache_inv((Ptr)inImage.imageData,(inImage.width * inImage.height),Cache_Type_ALL,TRUE);

#ifdef LPR_DEMO			
			LPRAlg(inImage.imageData,0);
#else
			if(pAlgObj->startCnt >= VA_LINK_START_CNT)
			{
				libStatus = DMVAL_process(pDmvaHndl,
										  &inImage,
										  &pAlgObj->output);
				UTILS_assert(libStatus == DMVAL_NO_ERROR);
			}
			else
			{
				pAlgObj->output.freeBufId[0] = inImage.inputId;	
				pAlgObj->output.freeBufId[1] = 0;
				pAlgObj->startCnt++;
			}
#endif			

#ifndef IPNC_DSP_ON
			bitFieldOfResources = RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_HWSEQ | RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DMA;
			eRMStatus = RM_SIMCOP_ReleaseMultiResources((RM_SIMCOP_CLIENT_HANDLE)pObj->pRMHandle,
									                     bitFieldOfResources);
			UTILS_assert(eRMStatus == RM_SIMCOP_STATUS_SUCCESS);
#endif			

#ifdef LPR_DEMO		
			vaStrPrm.disFlag     = 0;
			vaStrPrm.vaString[0] = '\0';
	
			if(isLPRResult[0]) 
			{
				isLPRResult[0] = 0;
				siLPRShowCount = 150;	
				strcpy((char*)ucRecogResult,(char*)pucRecogResult[0]);				
			}
			
			if(siLPRShowCount) 
			{
				strcpy((char*)vaStrPrm.vaString,(char*)ucRecogResult);
				siLPRShowCount--;
				
				if(pObj->createArgs.swosdLinkId != SYSTEM_LINK_ID_INVALID)	
				{
					System_linkControl(pObj->createArgs.swosdLinkId,
									   SWOSDLINK_CMD_VASTRING,
									   &vaStrPrm,
									   sizeof(SwosdLink_vaStrPrm),TRUE);				
				}				   
			}			
#else
			status = VaLink_algCreateMetaData(pObj);
			UTILS_assert(status == FVID2_SOK);
#endif			

#ifdef VA_PROFILE
			end = Utils_getCurTimeInUsec();
			if((end - start) > pAlgObj->maxProcessTime)
			{
				pAlgObj->maxProcessTime = (end - start);
			}
			pAlgObj->totalProcessTime += (end - start);
#endif

			pAlgObj->processFrCnt ++;

			if(pAlgObj->processFrCnt >= 900)
			{
			    Vps_printf(" %d: VA   : ######################################### !!!\n",
				           Clock_getTicks());
#ifdef VA_PROFILE
				Vps_printf(" %d: VA   : Peroidic Avg Process time = %d usec/frame !!!\n",
						   Clock_getTicks(),pAlgObj->totalProcessTime/pAlgObj->processFrCnt);
				Vps_printf(" %d: VA   : Maximum Process time      = %d usec/frame !!!\n",
						   Clock_getTicks(),pAlgObj->maxProcessTime);						   
#else
				Vps_printf(" %d: VA   : Peroidic print !!!\n", Clock_getTicks());
#endif
			    Vps_printf(" %d: VA   : ######################################### !!!\n",
				           Clock_getTicks());
                pAlgObj->processFrCnt     = 0;
				pAlgObj->totalProcessTime = 0;
            }

#ifdef LPR_DEMO
			status = Utils_bufPutEmptyFrame(&pObj->outFrameQue,pFullFrame);
			UTILS_assert(status == FVID2_SOK);
#else			
            for(frameId = 0;frameId < DMVAL_MAX_BUFFER_LOCK;frameId ++)
			{
			    /* The array ends with Id equal to 0 */
				if(pAlgObj->output.freeBufId[frameId] == 0)
				{
				    break;
				}

			    pEmptyFrame = &pObj->outFrames[pAlgObj->output.freeBufId[frameId] - 1];

				/* Put the frames back which are marked as free by DMVAL_process */
				status = Utils_bufPutEmptyFrame(&pObj->outFrameQue,pEmptyFrame);
				UTILS_assert(status == FVID2_SOK);
			}
#endif			
        }
    } while ((status == FVID2_SOK) && (pFullFrame != NULL));

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     VaLink_algDelete
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
 Int32 VaLink_algDmvalDelete(VaLink_Obj *pObj)
 {
 	VaLink_algObj *pAlgObj = &pObj->algObj;

	/* Free ALG mem */
	Utils_memFree_cached(pAlgObj->ptr,pAlgObj->size);

#ifndef IPNC_DSP_ON
	RM_SIMCOP_DeRegisterClient(pObj->pRMHandle);
#endif
	
	return FVID2_SOK;
 }

/* Delete the driver instance */

/* ===================================================================
 *  @func     VaLink_algDelete
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
Int32 VaLink_algDelete(VaLink_Obj *pObj)
{
    Int32 status;

	/***************************
		VA ALG DELETE HERE
	***************************/
	status = VaLink_algDmvalDelete(pObj);
	UTILS_assert(status == FVID2_SOK);
	
	System_putEdmaCh(&pObj->copyEdmaHndl);
	System_putEdmaCh(&pObj->algEdmaHndl);		

#ifdef SYSTEM_DEBUG_VA
    Vps_printf(" %d: VA   : ALG Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}
