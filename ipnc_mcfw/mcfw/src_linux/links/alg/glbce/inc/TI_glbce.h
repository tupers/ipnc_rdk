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
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//This is the interface to the GLBCE library
#define MAX_NUM_HORZ  20
#define MAX_NUM_VERT  11
 
#define MAX_NUM_BLOCKS (MAX_NUM_HORZ*MAX_NUM_VERT)

//GLBCE input parameters during creation
typedef struct {
	Word16   imgVertSize;
	Word16   imgHorzSize;
	Word16   numBits;
	Word16   downsampleRatio;
	Byte*    PersistentMemory;
    Word32   PersistentMemorySize; 
	Byte*    externalScratchMemory;
	Word32   externalScratchMemorySize;
	Byte     ExternalGamma;  
	Byte     PreviewMode;
	Byte	 CaptureMode;
	Byte*    Gamma;
	Word16   NumGainLimitPoints;
	Word16*  GainLimitXPoints;
	Word16*  GainLimitYPoints;
	Byte*    LimitTable;     
	Byte     temporalAverWeightLog; /* temporal averaging factor n used in 1/(2^n) */
	Byte     FrameCount;   

} GLBCE_CreationInputParamsStruct;


//GLBCE output parameters during creation
typedef struct {
	Word16   errorCode;
	Word32   externalScratchMemoryUsed;
	Word32   PersistentMemoryUsed;
} GLBCE_CreationOutputParamsStruct;


//GLBCE input parameters during a run for one image
typedef struct {
	uWord16*  boxcarMemory;
	uWord16*  bayerMemory;
	uWord16*  bayerImage;
	Byte*    gainTable;
	float    maxGain;
	Byte	numBits;
	Word16	WB_RGain;
	Word16	WB_GrGain;
	Word16	WB_GbGain;
	Word16	WB_BGain;
	Word16	RRCoef;
	Word16	RGCoef;
	Word16	RBCoef;
	Word16	GRCoef;
	Word16	GGCoef;
	Word16	GBCoef;
	Word16	BRCoef;
	Word16	BGCoef;
	Word16	BBCoef;
	Byte	SensorType;
	Word16  GBEStrength;
	Word16  GCEStrength;
	Word16  LBEStrength; //Q4.12 format
	Word16  LCEStrength; //Q4.12 format
	
// Capture Params
	Word32	ExposureTime;
	Word16	AnalogGain;
	Word16	DigitalGain;
	Word16	AnalogGainDivider; 
	Word16	DigitalGainDivider; 
	Word16	Aperture;
	Word16	Aperturedivider; 
	Word16  ExpositionValue;
  Byte    toneCurveIndex;
	
} GLBCE_ImageInputParamsStruct;


//GLBCE output parameters during a run for one image
typedef struct {
	Word16   glbceStatus;
	Word16*  ToneTable;
	Word16*  CombinedToneTable;
	//float*   GainMap;
	Byte	 SToneValue;
	Byte	 MToneValue;
	Byte	 HToneValue;
	Byte     LowerCutoffValue;
        //Byte*    toneCurves[2];
        Byte*    toneCurves;
} GLBCE_ImageOutputParamsStruct;


//GLBCE CONTEXT
typedef struct {
	uWord16                           glbceHandle[200];
	GLBCE_CreationInputParamsStruct   creationInputParams;
	GLBCE_CreationOutputParamsStruct  creationOutputParams;
	GLBCE_ImageInputParamsStruct      imageInputParams;
	GLBCE_ImageOutputParamsStruct     imageOutputParams;
} GLBCE_CONTEXT;


void  GLBCE_algConfig  (GLBCE_CONTEXT* glbceCon);  //First, call this function and make sure it executes without error.
void  GLBCE_init       (GLBCE_CONTEXT* glbceCon);  //Second, call this function to initialize memory. 
void  GLBCE_run        (GLBCE_CONTEXT* glbceCon);  //Third, call this function for each image.

#ifdef __cplusplus
}
#endif /* __cplusplus */
