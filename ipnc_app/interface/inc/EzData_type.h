#ifndef _EZ_DATA_TYPE_H
#define _EZ_DATA_TYPE_H

/****** CAMERA H3A PARAM ******/
typedef struct {
    unsigned int minExposure;                                       /* Mininum Sensor
                                                            * exposure */
    unsigned int maxExposure;                                       /* Maximum Sensor
                                                            * exposure */
    unsigned int stepSize;                                          /* step size for
                                                            * the exposure
                                                            * variation */
    unsigned int aGainMin;                                          /* Minimum analog
                                                            * Gain */
    unsigned int aGainMax;                                          /* Maximum Analog
                                                            * gain */
    unsigned int dGainMin;                                          /* Minimum digital
                                                            * gain */
    unsigned int dGainMax;                                          /* Maximum Digital
                                                            * gain */
    unsigned int targetBrightnessMin;                               /* Minimum target
                                                            * bright ness */
    unsigned int targetBrightnessMax;                               /* Maximum target
                                                            * bright ness */
    unsigned int targetBrightness;                                  /* target
                                                            * brightness to
                                                            * be achieved */
    unsigned int threshold;		

    /** weight **/
    unsigned int width;
	unsigned int height;
	unsigned int HCount;
	unsigned int VCount;
	unsigned int HStart;
	unsigned int VStart;
	unsigned int horzIncr;
	unsigned int vertIncr; 						   
}EzCamH3AParam;

typedef struct 
{
	int width1;
	int height1;
	int h_start2;
	int v_start2;
	int width2;
	int height2; 
	int weight;
}EzCamH3AWeight;

/*** CAMERA ALG PARAM ********/
typedef struct _RoiBlobT
{
	int id;
	int left;
	int right;
	int up;
	int low;
	int area;
	int Total255;
}RoiBlobT;

#define IMG_WIDTH_MAX		1920
#define IMG_HEIGHT_MAX		1080
#define IMG_PITCH_MAX		2000
#define ALG_SHMEM_MAXSIZE			(IMG_PITCH_MAX*IMG_HEIGHT_MAX+12)
typedef struct _ImgFrameHeader
{
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
	unsigned int infoSize;	
}ImgFrameHeader;
typedef struct
{
	ImgFrameHeader	header;
	char	 		data[0];
}Alg_DbgImg;

#define ALGPARAM_MAX_SIZE			1024
typedef struct 
{
	unsigned int prmSize;
	char prm[0];
}Alg_Params;

#define ALGRESULT_MAX_SIZE			1024
typedef struct
{
	unsigned char Alg_saveImgdirName[10];
	unsigned char Alg_saveImgIndex;
	unsigned int  errNo;
}AlgSaveImg;

typedef struct
{
	unsigned char Alg_saveImg;
	AlgSaveImg	  saveInfo;
	unsigned char weightValid;
	EzCamH3AWeight weight;
	unsigned int resultSize;
	char result[0];
}Alg_Result;

#endif
