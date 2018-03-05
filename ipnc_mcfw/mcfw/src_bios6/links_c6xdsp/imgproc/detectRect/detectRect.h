/*
 * detectRect.h
 *
 *  Created on: 2017Äê1ÔÂ3ÈÕ
 *      Author: tupers
 */

#ifndef DETECTRECT_H_
#define DETECTRECT_H_

#include "IMG_Common.h"
#include "MATH_Common.h"
#include "IMG_ApproxPoly.h"
#include "Alg_xdais.h"
#include "Alg_drvMgr.h"

extern int Vps_printf(char *format, ...);

#define DEBUG

#ifdef DEBUG
#define printf(fmt, args...)	Vps_printf("Debug " fmt, ##args)
#else
#define printf(fmt, args...)
#endif

#define ERR(fmt, args...)	Vps_printf("Error " fmt, ##args)
//common code
#define PROCESS_OK				0
#define INVALIDPARAMS			1

//detect error
#define TYPE_DETECT				1000
#define DETECT_OK				0
#define DETECT_EXCEED			1
#define DETECT_EDMAERR			2

//parse error code
#define TYPE_PARSE				2000
#define PARSE_OK				0
#define PARSE_LONGBLOCK_RATIOEXCEED		1
#define PARSE_LONGBLOCK_BASENOTFOUND		2
#define PARSE_LONGBLOCK_NOTENOUGH		3
#define PARSE_LONGBLOCK_SHORTSIDEERR		4
#define PARSE_LONGBLOCK_NOGROUP			5
#define PARSE_LONGBLOCK_VCOLNOTENOUGH		6
#define PARSE_LONGBLOCK_HCOLNOTENOUGH		7
#define PARSE_SHORTBLOCK_EXCEED			8
#define PARSE_LONGBLOCK_COMSATEFAIL		9
#define PARSE_LONGBLOCK_TOOLONG			10
//code error code
#define TYPE_CODE				3000
#define CODE_OK					0
#define CODE_INVALIDFORMAT		1
#define	CODE_CHECKERR			2
//block type
#define TYPE_BLOCK				3000
#define BLOCK_ERR				-1
#define BLOCK_UNDEFINED				0
#define BLOCK_FLAG				1
#define BLOCK_UNIT				2

#define MAX_FRAME_WIDTH			1920
#define MAX_FRAME_HEIGHT		1080

#define MAX_LABELCOUNT			50

#define RATIOTYPE_NONE			0
#define RATIOTYPE_H			1
#define RATIOTYPE_V			2

#define LEFT2RIGHT			0
#define RIGHT2LEFT			1

#define SHORT_REC_RATIO			1
#define LONG_REC_RATIO			3

#define GVSS_MAX_CODELINE		5
#define GVSS_MIN_CODELINE		3
#define GVSS_MAX_VERTICALLINE_CNT	7
#define GVSS_MAX_HORIZOLLINE_CNT	4
//detectRect control cmd

typedef enum
{
	UpdateParams=0,
	GetParams
}DetectRect_CtlCmd;

typedef enum
{
	IMG_ORGINAL = 0,
	IMG_BINARY,
	IMG_TOPHAT
}ViewImgSourceEnum;
typedef struct DetectRect_DynamicParams_Struture
{
	float frameLeftRatio;
	float frameRightRatio;
	float frameTopRatio;
	float frameBottomRatio;
	float focus;
	float mconst;
	float aconst;//distance = k1 * distance + k2
	float posMconst;
	float posAconst;
	float RectSimilarity;
	float ROIRation;
	unsigned int maxArea;
	unsigned int minArea;
	unsigned int longBlockHOffset;
	unsigned int longBlockVOffset;
	unsigned int shortBlockVOffset;
	unsigned int isDetectSimilarity;
	unsigned int isLongBlockVvalid;
	unsigned int isTopHatValid;
	unsigned int detectRowsNum;
	unsigned int viewImgSource;
} DetectRect_DParams_Str;

typedef struct DETECTRECT_CONTROL_STRUCTURE
{
	unsigned int sourceImg;
}DetectRect_Control_Str;

typedef struct
{
	int area;
	IMG_Point coord;
	unsigned short used; //true =1 ,false =0
	short index;
} DetectRect_Block_Str;

typedef struct
{
	float distance;
	float position;
}Detect_ResultData;

typedef struct
{
	int area;
	IMG_Point coord;
} DetectRect_Result_BlockStr;

typedef struct DETECTRECT_RESULT_STRUCTURE
{
	float position;
	float distance;
	int errIndex;
	unsigned char code[GVSS_MAX_CODELINE*2];
	unsigned char reserved[2];
	short top;
	short bottom;
	short left;
	short right;
	int blockNum;
	DetectRect_Result_BlockStr block[MAX_LABELCOUNT];
} DetectRect_Result_Str;


//typedef struct
//{
//	float ratio;
//	int blockNum;
//	DetectRect_Block_Str* block[MAX_LABELCOUNT/2];
//}HGroup;

//typedef struct
//{
//	float angle;
//	HGroup* line1;
//	HGroup* line2;
//}LinePair;


typedef struct
{
	int shortBlockNum;
	int longBlockNum;
	DetectRect_Block_Str* longBlock[GVSS_MAX_VERTICALLINE_CNT];
	DetectRect_Block_Str* shortBlock[GVSS_MAX_VERTICALLINE_CNT*2];
	unsigned char code[2];
} CodePair;

typedef struct
{
	CodePair codeGroup[GVSS_MAX_CODELINE];
	unsigned int groupNum;
	unsigned int comLongCnt;
	DetectRect_Block_Str comlongBlock[GVSS_MAX_VERTICALLINE_CNT*GVSS_MAX_HORIZOLLINE_CNT];
} CodeGroup;

typedef struct
{
	float index;
	float position;
	float distance;
}CodeInfo;

typedef struct
{
	DetectRect_Block_Str longBlock[GVSS_MAX_CODELINE][3];
	unsigned char code[GVSS_MAX_CODELINE][2];
	short top;
	short bottom;
	short left;
	short right;
//	short longBlockHOffset;
}CodeTrace;
typedef struct Detect_LineInfo_t
{
	DetectRect_Block_Str* block[10];
	unsigned int rectNum;
}Detect_LineInfo;

typedef struct
{
	//DetectRect_Block_Str* group[3][3];
	Detect_LineInfo longGroup[GVSS_MAX_VERTICALLINE_CNT];
	CodeGroup code;
	CodeInfo codeInfo;
	DetectRect_Block_Str block[MAX_LABELCOUNT];
} DetectRect_Analysis;

typedef struct DETECTRECT_PROCESS_STRUCTURE
{
	IMG_Frame frame;
	IMG_HistogramStr* hist;
	IMG_ConnectedLabel CC;
	IMG_Morphology Morph;
	DetectRect_Analysis* blobAnal;
	IMG_ApproxPoly_Handle approxHandle;
	CodeTrace traceOpt;
}DetectRect_Process_Str;

typedef struct DETECTRECT_STRUCTURE
{
	DetectRect_Process_Str Process;
	DetectRect_DParams_Str DParams;
	DetectRect_Control_Str Control;
	DetectRect_Result_Str *Result;
}DetectRect_Str;

typedef struct
{
	ALG_Obj algObj;
	DetectRect_Str detectStr;
}EzDetectRecObj;


int detectRectConvertNum(unsigned char code,int type);
void detectRect_expandSub(IMG_FrameData* dst,IMG_FrameData* src1, IMG_FrameData* src2);
int detectRect_isLine(const IMG_Point* restrict pointA,
		const IMG_Point* restrict pointB, const IMG_Point* restrict pointC,
		const float hAngle, const float vAngle, const float slopeOffset);
int detectRect_blockIndexOf(DetectRect_Block_Str** array,
		DetectRect_Block_Str* block, int num);
float detectRect_slope(const IMG_Point* restrict basePoint,
		const IMG_Point* restrict tarPoint, const float hAngle,
		const float vAngle);
int detectRect_parse(DetectRect_Str* detectStr,DetectRect_DParams_Str* dParams);
int detectRect_sortY_small(const void*a, const void *b);
int detectRect_sortX_small(const void*a, const void *b);
int detectRect_sortY_small2(const void*a, const void *b);
int detectRect_sortX_small2(const void*a, const void *b);
int detectRect_sortarea(const void *a, const void *b);
int detectRect_sortmidXY_small(const void*a,const void*b);
int detectRect_sorttheta(const void *a, const void *b);
int detectRect_analysis(DetectRect_Str*, int);
int detectRect_trace(DetectRect_Str*);
int detectRect_isEnclosedRect(EzDetectRecObj* obj,VLIB_CC cc,float similarity,int index);
void detectRectSrcBoundary(IMG_Block* block, IMG_FrameData* src,
		unsigned char mask, int threshold);
int detectRectInit(void*srccfg,int size);
int detectRectFree(void*srccfg,int size);
int detectRectProcess(void*srccfg,int size);
int detectRectControl(void*srccfg,int size,void*inArg,int inArgSize,void*outArg, int outArgSize);
int detectShortRec(DetectRect_Block_Str *longBlock1,DetectRect_Block_Str *longBlock2,unsigned gapLongBlock,unsigned groupNum,unsigned codeNum,DetectRect_Analysis *anal,unsigned int thres,unsigned int num,unsigned shortBlockVOffset);
Bool isDetectRectValid(void* prm);
IMG_Point detectRect_BlobCentroid(IMG_FrameData* src,VLIB_CC cc,int blobNum,int index);

#endif /* DETECTRECT_H_ */

