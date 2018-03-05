#ifndef IMG_COMMON_H_
#define IMG_COMMON_H_

#ifdef _C674DSP_
#include <xdc/std.h>
#include <ti/vlib/vlib.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define IMG_PointCpy(dst,src) {((IMG_Point*)dst)->x=((IMG_Point*)src)->x;((IMG_Point*)dst)->y=((IMG_Point*)src)->y;}
//#define IMG_PixVal(in,cols,rows) ((IMG_FrameData*)in)->data[cols+rows*((IMG_FrameData*)in)->cols]

typedef struct IMG_FRAMEDATA_STRUCT
{
	int cols;
	int rows;
//	int orgcols;
//	int orgrows;
//	int orgPitch;
//	unsigned int HStart;
//	unsigned int HEnd;
//	unsigned int VStart;
//	unsigned int VEnd;
	unsigned char* data;
} IMG_FrameData;

typedef struct IMG_PROJECTION_STRUCT
{
	int length;
	unsigned int* data;
} IMG_Pro;

typedef struct IMG_FRAME_STRUCT
{
	IMG_FrameData srcimg;
	IMG_FrameData processimg;
	IMG_FrameData scaledimg;
	IMG_FrameData morphimg; 
	int orgcols;
	int orgrows;
	int HStart;
	int HEnd;
	int VStart;
	int VEnd;
} IMG_Frame;

typedef struct IMG_DSP_Histogram_STRUCT
{
	unsigned char binedge[256];
	unsigned char histarray[257];
	unsigned short H1[256];
	unsigned short H2[256];
	unsigned short H3[256];
} IMG_HistogramStr;

typedef struct IMG_DSP_ConnectedLabel_STRUCT
{
	unsigned int* packedImg;
	unsigned int* packedImgTmp;
	void * workBuf;
	VLIB_CCHandle* handle;
	int handleSize;
	int workBufSize;
}IMG_ConnectedLabel;

typedef struct
{
	short x;
	short y;
}IMG_Point;

typedef struct
{
	IMG_Point point;
	int used;
}IMG_mapPoint;

typedef struct
{
	int cnt;
	IMG_mapPoint* mapPoint;
}IMG_mapPointGroup;

typedef struct
{
	int cnt;
	IMG_mapPoint** pointPtrArr;
}IMG_mapPointGroup_shadow;

typedef struct
{
	unsigned char* scratch1_ptr;
	//unsigned char* scratch2_ptr;
	unsigned char* se_ptr;
	unsigned char* refl_se_ptr;
}IMG_Morphology;

typedef struct
{
	int left;
	int top;
	int right;
	int bottom;
}IMG_Block;

void IMG_ZoomOut_FixedRatio(IMG_FrameData* src, IMG_FrameData* dst, int ratio);

void IMG_Projection_H(IMG_FrameData*src, IMG_Pro*ph);

void IMG_Projeciton_V(IMG_FrameData*src, IMG_Pro*pv);

int IMG_getThreshold(IMG_FrameData*src, IMG_HistogramStr* restrict histparams);

void IMG_Threshold(IMG_FrameData*in, IMG_FrameData*out, int thres);

void IMG_Threshold_PH(IMG_FrameData*in, IMG_FrameData*out, IMG_Pro* ph,
		int thres);

void IMG_DSP_thrmin2max(const unsigned char *in_data,
		unsigned char*restrict out_data, short cols, short rows,
		unsigned char threshold);

void IMG_DSP_InitHistogram(IMG_HistogramStr* restrict histparams);

void IMG_DSP_getHistogram(const IMG_FrameData* restrict src,
		IMG_HistogramStr* restrict histparams, unsigned short*restrict H);

inline unsigned char IMG_PixVal(const IMG_FrameData* in ,int cols,int rows){return in->data[cols+rows*in->cols];}


#endif
