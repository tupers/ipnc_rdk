/*=======================================================================
 *
 *            Texas Instruments Internal Reference Software
 *
 *                 Systems and Applications R&D Center
 *                    Video and Image Processing Lab
 *                           Imaging Branch         
 *
 *         Copyright (c) 2010 Texas Instruments, Incorporated.
 *                        All Rights Reserved.
 *      
 *
 *          FOR TI INTERNAL USE ONLY. NOT TO BE REDISTRIBUTED.
 *
 *							TI CONFIDENTIAL
 *
 *======================================================================*/


#include "dataTypeDefs.h"

//Number of Histogram for Calibration Data
#define NUM_OF_HISTOGRAM                 1000           
#define NUM_OF_BINS                      256  
#define GAMMATABLESIZE                   4096

#ifndef _GLBCE_STRUCT_INCLUDED_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GLBCE_STRUCT_INCLUDED_

typedef struct {
	Word16   imgVertSize;
	Word16   imgHorzSize;
        Byte      numHistBins;
	uWord16*  boxcarMemory;
	uWord16*  bayerMemory;
	uWord16*  bayerImage;
	Word32*   histogramMemory;
	Word16    numBits;
	Word16    downsampleRatio;
        Word16*   reconstTone;
	Byte*     ToneTable;
	Word16*   InvTable;
	Byte*     toneCurves[2];
	//Byte*     toneCurves;
    
	Byte*     tmptoneCurves;
	Word16*   CDFPoints;
	Word16*   GbceHistoryPoints;
	Word16*   ExposureState;
	
	Word16   GBEStrength;
	Word16   GCEStrength;

	// Internal GBCE Variables
	sWord32*  projectedsads;
	Word16*   tmpprojectedsads;
	Word16*   reducedprojectedsads;
	Word16*   projectedsadsindex;
	Word16*   IndexDarkLow;

	Word16*   allBlkMeans;
	Word16*   localgains;
	Byte*	  LimitTable;
	Word16*  GainLimitXPoints;
	Word16*  GainLimitYPoints;
	Byte	numHorzBlocks;
	Byte	numVertBlocks;
    uWord16 localBlockWidth;
    uWord16 localBlockHeight;
	
} GLBCEstruct;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
