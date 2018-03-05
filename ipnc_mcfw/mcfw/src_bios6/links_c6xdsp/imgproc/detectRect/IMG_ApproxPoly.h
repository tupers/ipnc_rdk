/*
 * IMG_ApproxPoly.h
 *
 *  Created on: 2017年5月24日
 *      Author: tupers
 */

#ifndef DETECTRECT_IMG_APPROXPOLY_H_
#define DETECTRECT_IMG_APPROXPOLY_H_

#include "IMG_Common.h"

#define IMG_APPROXPOLY_CONTOURPOINT     300
#define IMG_APPROXPOLY_COMPRESSPOINT    100
#define IMG_APPROXPOLY_APPROXPOINT      50
#define IMG_APPROXPOLY_RESULTPOINT      15

#define IMG_APPROXPOLY_OK                   0
#define IMG_APPROXPOLY_FINDCONTOUREXCEED    1
#define IMG_APPROXPOLY_SIMPLIFYCHAINEXCEED  2
#define IMG_APPROXPOLY_APPROXEXCEED         3
#define IMG_APPROXPOLY_RESULTEXCEED         4

typedef struct
{
    int cnt;
    IMG_Point firstPoint;
    unsigned char* code;
}IMG_ApproxPoly_ContourChain;

typedef struct
{
	char directTable[8][2];
	IMG_ApproxPoly_ContourChain chain;
	IMG_mapPointGroup compress;
	IMG_mapPointGroup_shadow convexhull;
	void* workBuf;
	int workBufSize;
}IMG_ApproxPoly_Handle;


int IMG_ApproxPoly_WorkBufSize();
void IMG_ApproxPoly_Init(IMG_ApproxPoly_Handle*);
int IMG_ApproxPoly(IMG_ApproxPoly_Handle*,IMG_FrameData*,int );
int IMG_ApproxPoly_FindContour(IMG_ApproxPoly_Handle*,IMG_FrameData*,int );
int IMG_ApproxPoly_FindContourSingle(IMG_ApproxPoly_Handle*,IMG_FrameData*,int,int,int);
int IMG_ApproxPoly_SimplifyChain(IMG_ApproxPoly_Handle*);
int IMG_ApproxPoly_ConvexHull(IMG_ApproxPoly_Handle*);
int IMG_ApproxPoly_PointCross(IMG_Point*A,IMG_Point*B,IMG_Point*C);
//void IMG_ApproxPoly_Single(int,int,IMG_PointGroup*,IMG_PointGroup*);
//float IMG_ApproxPoly_DPMax(int,int,IMG_Point*,int*);
int IMG_ApproxPoly_MER(IMG_ApproxPoly_Handle*);

IMG_Point IMG_BlobCentroid_rect(IMG_FrameData* frame,VLIB_CC cc);
IMG_Point IMG_BlobCentroid_map(IMG_FrameData* map,VLIB_CC cc,int index);

#endif /* DETECTRECT_IMG_APPROXPOLY_H_ */
