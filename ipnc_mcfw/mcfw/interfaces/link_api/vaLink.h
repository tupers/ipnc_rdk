/** ==================================================================
 *  @file   vaLink.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/interfaces/link_api/                                                 
 *                                                                    
 *  @desc   This  File contains Va Link API.        
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
    \defgroup VA_LINK_API Va Link API

    VA link runs on DSP and applies Video/Smart Analytics algorithms onto video stream.
    VA link creates the VA metadata and sends a message to encoder link to embed the metadata 
    into the encoder bitstream.
    VA link also sends a message to SWOSD link to imprint the VA event details.  	
	
	In the current release only the following VA algorithms are supported:
	1.Camera Tamper Detection
	2.Trip Zone
	3.Object Counting

    @{
*/

/**
    \file vaLink.h
    \brief VA Link API
*/

#ifndef _VA_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _VA_H_

#include <mcfw/interfaces/link_api/system.h>

/** \brief Max size of VA metadata */
#define VALINK_METADATASIZE					(128)									

/** \brief Commands handled by the VA link */
#define VALINK_CMD_GUISETUP					(0xA000)
#define VALINK_CMD_GUISETUPALGOFRAMERATE	(0xA001)
#define VALINK_CMD_GUISETUPALGODETECTFREQ	(0xA002)
#define VALINK_CMD_GUISETUPEVTRECORDENABLE	(0xA003)
#define VALINK_CMD_GUISETUPSMETATRACKENABLE	(0xA004)
#define VALINK_CMD_GUISETUPTZPRM			(0xA005)
#define VALINK_CMD_GUISETUPMAINPRM			(0xA006)
#define VALINK_CMD_GUISETUPCTDPRM			(0xA007)
#define VALINK_CMD_GUISETUPOCPRM			(0xA008)
#define VALINK_CMD_GUISETUPSMETAPRM			(0xA009)
#define VALINK_CMD_GUISETUPIMDPRM			(0xA00A)
#define VALINK_CMD_GUISETUPENCROIENABLE		(0xA00B)
#define VALINK_CMD_CHANGE_FRAMERATE			(0xA00C)

/**
    \brief Va Link Event type
*/
typedef enum {
    VALINK_EVT_IMD,
	VALINK_EVT_TZ,
	VALINK_EVT_OC,
	VALINK_EVT_CTD,
	VALINK_EVT_SMETA,
	VALINK_EVT_STOP
	
} VaLink_EventType;

/**
    \brief Va Link Main screen GUI parameters
*/
typedef struct {
  UInt32 dmvaCamID;
  /**< Cam ID */
  
  UInt32 dmvaGetSchedule;
  /**< Get Schedule */
  
  UInt32 dmvaCfgFD;
  /**< FD config */
  
  UInt32 dmvaCfgCTDImdSmetaTzOc; 
  /**< VA algorithm selection - Bit 4: OC, Bit 3: TZ, Bit 2: SMETA, Bit 1: IMD, Bit 0: CTD */
  
  UInt32 dmvaDisplayOptions; 
  /**< Display options - Bit 3: Object Bounding Box, Bit 2: Zones, Bit 1: TimeStamp, Bit 0: MetaData Overlay */
  
  UInt32 dmvaEventRecordingVAME;
  /**< Event record options - Bit 3: Video Stream, Bit 2: Audio Stream, Bit 1: MetaData Stream, Bit 0: Event Log */
  
  UInt32 dmvaEventListActionPlaySendSearchTrash;
  /**< Event action options - Bit 3: Play, Bit 2: Send Email, Bit 1: Search, Bit 0: Trash */
  
  UInt32 dmvaEventListSelectEvent;
  /**< Select an event from the VA event list */
  
  UInt32 dmvaEventListArchiveFlag;
  /**< Event list archieve flag */
  
  UInt32 dmvaEventListEventType;
  /**< Event type */
  
  UInt32 dmvaEventListStartMonth;
  /**< Start month for event search */
  
  UInt32 dmvaEventListStartDay;
  /**< Start day for event search */
  
  UInt32 dmvaEventListStartTime;
  /**< Start time for event search */
  
  UInt32 dmvaEventListEndMonth;
  /**< End month for event search */
  
  UInt32 dmvaEventListEndDay;
  /**< End day for event search */
  
  UInt32 dmvaEventListEndTime;
  /**< End time for event search */
  
} VaLink_GuiMainParams;

/**
    \brief Va Link TZ screen GUI parameters
*/
typedef struct {
  UInt32 dmvaTZSensitivity; 
  /**< Sensitivity - 1 to 5: Low to High*/
  
  UInt32 dmvaTZDirection; 
  /**< Direction - 4: Bi-direction, 2: Zone 1 to Zone 2, 1: Zone 2 to Zone 1 */  
  
  UInt32 dmvaTZPersonMinWidth;
  /**< Person min width */
  
  UInt32 dmvaTZPersonMinHeight;
  /**< Person min height */
  
  UInt32 dmvaTZVehicleMinWidth;
  /**< Vehicle min width */
  
  UInt32 dmvaTZVehicleMinHeight;
  /**< Vehicle min height */
  
  UInt32 dmvaTZPersonMaxWidth;
  /**< Person max width */
  
  UInt32 dmvaTZPersonMaxHeight;
  /**< Person max height */
  
  UInt32 dmvaTZVehicleMaxWidth;
  /**< Vehicle max width */
  
  UInt32 dmvaTZVehicleMaxHeight;
  /**< Vehicle max height */
    
  UInt32 dmvaTZEnable;
  /**< Enable flag */
  
  UInt32 dmvaTZZone1_ROI_numSides;
  /**< Num of sides of first Zone */
  
  UInt32 dmvaTZZone2_ROI_numSides;
  /**< Num of sides of second Zone */
  
  UInt32 dmvaTZZone1_x[16];
  /**< First Zone X co-ordinates */
  
  UInt32 dmvaTZZone1_y[16];
  /**< First Zone Y co-ordinates */
  
  UInt32 dmvaTZZone2_x[16];
  /**< Second Zone X co-ordinates */
  
  UInt32 dmvaTZZone2_y[16];
  /**< Second Zone Y co-ordinates */
  
  UInt32 dmvaTZPresentAdjust; 
  /**< People/Vehicle present/adjust flag - 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle */
  
} VaLink_GuiTZParams;

/**
    \brief Va Link IMD GUI parameters
*/
typedef struct {
  UInt32 dmvaIMDSensitivity; 
  /**< Sensitivity - 1 to 5: Low to High */
  
  UInt32 dmvaIMDPresentAdjust; 
  /**< People/Vehicle present/adjust flag - 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle */
  
  UInt32 dmvaIMDPersonMinWidth;
  /**< Person min width */
  
  UInt32 dmvaIMDPersonMinHeight;
  /**< Person min height */
  
  UInt32 dmvaIMDVehicleMinWidth;
  /**< Vehicle min width */
  
  UInt32 dmvaIMDVehicleMinHeight;
  /**< Vehicle min height */
  
  UInt32 dmvaIMDPersonMaxWidth;
  /**< Person max width */
  
  UInt32 dmvaIMDPersonMaxHeight;
  /**< Person max height */
  
  UInt32 dmvaIMDVehicleMaxWidth;
  /**< Vehicle max width */
  
  UInt32 dmvaIMDVehicleMaxHeight;
  /**< Vehicle max height */
  
  UInt32 dmvaIMDNumZones; 
  /**< Num of Zones - 1 to 4 */
  
  UInt32 dmvaIMDZoneEnable; 
  /**< Zone Enable - Bit 3: Zone 3 enable, Bit 2: Zone 2 enable, Bit 1: Zone 1 enable, Bit 0: Zone 0 enable */
  
  UInt32 dmvaIMDZone1_ROI_numSides;
  /**< Num of sides of first Zone */
  
  UInt32 dmvaIMDZone2_ROI_numSides;
  /**< Num of sides of second Zone */
  
  UInt32 dmvaIMDZone3_ROI_numSides;
  /**< Num of sides of third Zone */
  
  UInt32 dmvaIMDZone4_ROI_numSides;
  /**< Num of sided of fourth Zone */
  
  UInt32 dmvaIMDZone1_x[16];
  /**< First Zone X co-ordinates */
  
  UInt32 dmvaIMDZone1_y[16];
  /**< First Zone Y co-ordinates */
  
  UInt32 dmvaIMDZone2_x[16];
  /**< Second Zone X co-ordinates */
  
  UInt32 dmvaIMDZone2_y[16];
  /**< Second Zone Y co-ordinates */
  
  UInt32 dmvaIMDZone3_x[16];
  /**< Third Zone X co-ordinates */
  
  UInt32 dmvaIMDZone3_y[16];
  /**< Third Zone Y co-ordinates */
  
  UInt32 dmvaIMDZone4_x[16];
  /**< Fourth Zone X co-ordinates */
  
  UInt32 dmvaIMDZone4_y[16];
  /**< Fourth Zone Y co-ordinates */
  
} VaLink_GuiIMDParams;

/**
    \brief Va Link OC GUI parameters
*/
typedef struct {
  UInt32 dmvaOCSensitivity; 
  /**< Sensitivity - 1 to 5: Low to High */

  UInt32 dmvaOCDirection;
  /**< Direction - 4: Bi-direction, 2: Zone 1 to Zone 2, 1: Zone 2 to Zone 1 */
  
  UInt32 dmvaOCObjectWidth;
  /**< Min object width */
  
  UInt32 dmvaOCObjectHeight;
  /**< Min object height */
    
  UInt32 dmvaOCEnable;
  /**< Enable flag */
  
  UInt32 dmvaOCLeftRight;
  /**< Orientation - Left to Right */
  
  UInt32 dmvaOCTopBottom;
  /**< Orientation - Top to Bottom */
  
} VaLink_GuiOCParams;

/**
    \brief Va Link SMETA GUI parameters
*/
typedef struct {
  UInt32 dmvaSMETASensitivity; 
  /**< Sensitivity - 1 to 5: Low to High */
  
  UInt32 dmvaSMETAEnableStreamingData;
  /**< Enable flag */
  
  UInt32 dmvaSMETAStreamBB;
  /**< Enable Bounding box streaming */
  
  UInt32 dmvaSMETAStreamVelocity;
  /**< Enable Velocity streaming */
  
  UInt32 dmvaSMETAStreamHistogram;
  /**< Enable Histogram streaming */
  
  UInt32 dmvaSMETAStreamMoments;
  /**< Enable Moment streaming */

  UInt32 dmvaSMETAPresentAdjust; 
  /**< People/Vehicle present/adjust flag - 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle */
  
  UInt32 dmvaSMETAPersonMinWidth;
  /**< Person min width */
  
  UInt32 dmvaSMETAPersonMinHeight;
  /**< Person min height */
  
  UInt32 dmvaSMETAVehicleMinWidth;
  /**< Vehicle min width */
  
  UInt32 dmvaSMETAVehicleMinHeight;
  /**< Vehicle min height */
  
  UInt32 dmvaSMETAPersonMaxWidth;
  /**< Person max width */
  
  UInt32 dmvaSMETAPersonMaxHeight;
  /**< Person max height */
  
  UInt32 dmvaSMETAVehicleMaxWidth;
  /**< Vehicle max width */
  
  UInt32 dmvaSMETAVehicleMaxHeight;
  /**< Vehicle max height */
  
  UInt32 dmvaSMETANumZones; 
  /**< Num of Zones - 1 */
  
  UInt32 dmvaSMETAZone1_ROI_numSides;
  /**< Num of sides of first Zone */
  
  UInt32 dmvaSMETAZone1_x[16];
  /**< First Zone X co-ordinates */
  
  UInt32 dmvaSMETAZone1_y[16];
  /**< First Zone Y co-ordinates */

} VaLink_GuiSMETAParams;

/**
    \brief Va Link CTD GUI parameters
*/
typedef struct {
  UInt32 dmvaCfgTDEnv1;
  /**< Detect Lights On to Off */
  
  UInt32 dmvaCfgTDEnv2;
  /**< Detect Lights On to Off */
  
  UInt32 dmvaCfgTDSensitivity;
  /**< Sensitivity - 1 to 5: Low to High */
  
  UInt32 dmvaCfgTDResetTime; 
  /**< Reset time in num of seconds */
  
} VaLink_GuiCTDParams;

/**
    \brief Va Link GUI parameters
*/
typedef struct {
    UInt32 dmvaenable;
	/**< DMVA enable ON/OFF */
	
	UInt32 dmvaexptalgoframerate;
	/**< video (fps/N): 3 means 30/3 = 10 fps */
	
	UInt32 dmvaexptalgodetectfreq;
	/**< 10 means once in 10 frames that go to DMVA */
	
	UInt32 dmvaexptevtrecordenable;
	/**< 0: Off, 1: On */

	UInt32 dmvaexptsmetatrackerenable;
	/**< 0: Off, 1: On */	
	
	UInt32 dmvaEncRoiEnable;
	/**< 0: Off, 1: On */	
	
	VaLink_GuiMainParams dmvaMainPrm;
	/**< VA main page params */	
	
	VaLink_GuiTZParams dmvaTZPrm;
	/**< VA TZ params */	
	
	VaLink_GuiIMDParams dmvaIMDPrm;
	/**< VA IMD params */	
	
	VaLink_GuiOCParams dmvaOCPrm;
	/**< VA OC params */	
	
	VaLink_GuiSMETAParams dmvaSMETAPrm;
	/**< VA SMETA params */	
	
	VaLink_GuiCTDParams dmvaCTDPrm;
	/**< VA CTD params */	
	
} VaLink_GuiParams;

/**
    \brief Va Link create parameters
*/
typedef struct {
    System_LinkInQueParams inQueParams;
    /**< Input queue information */
	
	UInt32 encLinkId;
	/**< Encoder Link ID */
	
	UInt32 swosdLinkId;
	/**< SWOSD Link ID */	
	
	UInt32 cameraLinkId;
	/**< Camera Link ID */

	UInt32 vaFrameRate;
	/**< VA Frame Rate */	
	
} VaLink_CreateParams;


/**
 *  @func     VaLink_init                                               
 *                                                                    
 *  @desc     Function does the Va link register and init
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
Int32 VaLink_init();

/**
 *  @func     VaLink_deInit                                               
 *                                                                    
 *  @desc     Function does the Va link de-register and de-init                           
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
Int32 VaLink_deInit();

/**
 *  @func     VaLink_CreateParams_Init                                               
 *                                                                    
 *  @desc     Function configures the default creation parameters for VA link                           
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
static inline void VaLink_CreateParams_Init(VaLink_CreateParams * pPrm)
{
    memset(pPrm, 0, sizeof(*pPrm));

	pPrm->encLinkId    = SYSTEM_LINK_ID_INVALID;   
	pPrm->swosdLinkId  = SYSTEM_LINK_ID_INVALID;   
	pPrm->cameraLinkId = SYSTEM_LINK_ID_INVALID;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
