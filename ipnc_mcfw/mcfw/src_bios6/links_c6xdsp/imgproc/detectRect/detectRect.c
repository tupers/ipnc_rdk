/*
 * detectRect.c
 *
 *  Created on: 2017Äê1ÔÂ3ÈÕ
 *      Author: tupers
 */

#ifdef _C674DSP_
//#include <xdc/runtime/System.h>
//#include <xdc/runtime/Log.h>
//#include <ti/uia/events/UIABenchmark.h>
#endif
#include <stdlib.h>
#include <math.h>


#include "detectRect.h"
#include "Alg_drvMgr.h"
#include <mcfw/interfaces/link_api/imgAlg_data.h>
#include "alg_defParam.h"

DetectRect_Str g_detectRect;

#define OffsetOf(type,member)		((unsigned int)&((type *)0)->member)
#define Contain_Of(ptr,type,member) (type *)((unsigned int)ptr-OffsetOf(type,member))

#ifdef _C674DSP_
#include <ti/vlib/vlib.h>
#endif

void detectRect_expandSub(IMG_FrameData* dst,IMG_FrameData* src1, IMG_FrameData* src2)
{
	//dst = xpnd(src1-src2)
	//assume src1 and src 2 size is 1/4 size of dst
	_nassert((int) src1->cols % 8 == 0);
	_nassert((int) src1->rows % 8 == 0);

	int i;
	int times = src1->cols*src1->rows;

#pragma MUST_ITERATE(4,,2);
	for(i=0;i<times;i+=4)
	{	
		unsigned packedSrc1 = _amem4_const(&src1->data[i]);
		unsigned packedSrc2 = _amem4_const(&src2->data[i]);
	
		int packedDiff = _subabs4((int)packedSrc1,(int)packedSrc2);

		unsigned x1x2 = _unpklu4((unsigned)packedDiff);
		unsigned x3x4 = _unpkhu4((unsigned)packedDiff);
	
		unsigned x1x1x2x2=x1x2|(x1x2<<8);
		unsigned x3x3x4x4=x3x4|(x3x4<<8);

		unsigned rows = (i/src1->cols)*2;
		unsigned cols = (i%src1->cols)*2;
		
		unsigned idx1234_1 = rows*2*src1->cols+cols;
		unsigned idx5678_1 = idx1234_1+4;
		unsigned idx1234_2 = idx1234_1+2*src1->cols;
		unsigned idx5678_2 = idx1234_2+4;
		
		_amem4(&dst->data[idx1234_1])=x1x1x2x2;
		_amem4(&dst->data[idx1234_2])=x1x1x2x2;
		_amem4(&dst->data[idx5678_1])=x3x3x4x4;
		_amem4(&dst->data[idx5678_2])=x3x3x4x4;
	}
	
	
}


int detectShortRec(DetectRect_Block_Str *longBlock1,DetectRect_Block_Str *longBlock2,unsigned gapLongBlock,unsigned groupNum,unsigned codeNum,DetectRect_Analysis *anal,unsigned int thres,unsigned int num,unsigned shortBlockVOffset)
{

	if(groupNum>anal->code.groupNum-1||codeNum>1)
		return -1;

    int xStart = longBlock1->coord.x;
//    int xEnd = longBlock2->coord.x;
    int xEnd = gapLongBlock + longBlock1->coord.x;
    int yStart = 0;
    int yEnd = 0;
    int ret = 0;
    int shortBlockNum = 0;
    unsigned int i;

    yStart = (longBlock1->coord.y > longBlock2->coord.y)?(longBlock2->coord.y):(longBlock1->coord.y);
    yEnd = (longBlock1->coord.y > longBlock2->coord.y)?(longBlock1->coord.y):(longBlock2->coord.y);


    for (i = thres; i < num; i++)
    {
        if (anal->block[i].used == 1 || anal->block[i].coord.x > xEnd
            || anal->block[i].coord.x < xStart)
            continue;

        if ((unsigned int)anal->block[i].coord.y <= (unsigned int)(yEnd + shortBlockVOffset)
            && (unsigned int)anal->block[i].coord.y >= (unsigned int)(yStart - shortBlockVOffset))
        {
            float blockWidth = (float) (xEnd - xStart) / 10;
            float leftPosition = (float) (anal->block[i].coord.x - xStart)
                                 / blockWidth;
            float rightPosition = (float) (xEnd - anal->block[i].coord.x)
                                  / blockWidth;
            float position = (float)(((9 - leftPosition) + (rightPosition - 1)) / 2 + 0.5);
            anal->code.codeGroup[groupNum].code[codeNum] |= (0x1 << (int) (position - 1));
            anal->block[i].used = 1;
	    anal->code.codeGroup[groupNum].shortBlock[anal->code.codeGroup[groupNum].shortBlockNum] =
					&anal->block[i];
	    anal->code.codeGroup[groupNum].shortBlockNum ++;
	    shortBlockNum++;
	    if (shortBlockNum > 2)
	    {
		ret = PARSE_SHORTBLOCK_EXCEED;
		break;
	    }
        }
    }

    return ret;
}

int detectRectConvertNum(unsigned char code, int type)
{
	int result = 0;

	if (type == LEFT2RIGHT)
	{
		code >>= 1;
		if ((code & 0x10) >> 4)
			result += 8;
		if ((code & 0x8) >> 3)
			result += 5;
		if ((code & 0x4) >> 2)
			result += 3;
		if ((code & 0x2) >> 1)
			result += 2;
		if (code & 0x1)
			result += 1;
	} else if (type == RIGHT2LEFT)
	{
		if ((code & 0x2) >> 1)
			result += 8;
		if ((code & 0x4) >> 2)
			result += 5;
		if ((code & 0x8) >> 3)
			result += 3;
		if ((code & 0x10) >> 4)
			result += 2;
		if ((code & 0x20) >> 5)
			result += 1;
	}
	return result;
}

int detectRect_isLine(const IMG_Point* restrict pointA,
		const IMG_Point* restrict pointB, const IMG_Point* restrict pointC,
		const float hAngle, const float vAngle, const float slopeOffset)
{
	float ratio1 = detectRect_slope(pointA, pointB, hAngle, vAngle);
	float ratio2 = detectRect_slope(pointA, pointC, hAngle, vAngle);
	if (ratio1 <= ratio2 + slopeOffset && ratio1 >= ratio2 - slopeOffset)
		return 1;
	else
		return 0;
}

int detectRect_blockIndexOf(DetectRect_Block_Str** array,
		DetectRect_Block_Str* block, int num)
{
	int i;
	for (i = 0; i < num; i++)
	{
		if (array[i] == block)
			return i;
	}
	return -1;
}

float detectRect_slope(const IMG_Point* restrict basePoint,
		const IMG_Point* restrict tarPoint, const float hAngle,
		const float vAngle)
{
	float tempratio;
	int deltaX = tarPoint->x - basePoint->x;
	int deltaY = tarPoint->y - basePoint->y;
#if 0	
	if(deltaX == 0)
	tempratio = 1280;
	else if(deltaY == 0)
	tempratio = 0;
	else
	tempratio = (float) deltaY / deltaX;
	return tempratio;
#else	
	float hOffset = (float) fabs(deltaY) * hAngle;
	float vOffset = (float) fabs(deltaX) * vAngle;
	if (deltaX <= hOffset && deltaX >= -hOffset)
		tempratio = 1280;
	else if (deltaY == 0)
		tempratio = 0;
	else
		tempratio = (float) deltaY / deltaX;
	return tempratio;
#endif
}

int detectRect_sortmidXY_small(const void*a, const void*b)
{
	DetectRect_Block_Str *c = (DetectRect_Block_Str *) a;
	DetectRect_Block_Str *d = (DetectRect_Block_Str *) b;
	short c_midx = abs(c->coord.x - 640);
	short d_midx = abs(d->coord.x - 640);
	short c_midy = abs(c->coord.y - 360);
	short d_midy = abs(d->coord.y - 360);
	if ((c_midx - d_midx) == 0)
		return c_midy - d_midy;
	else
		return c_midx - d_midx;
}
int detectRect_sortY_small(const void*a, const void *b)
{
	DetectRect_Block_Str **c = (DetectRect_Block_Str **) a;
	DetectRect_Block_Str **d = (DetectRect_Block_Str **) b;
	return (*c)->coord.y - (*d)->coord.y;
}
int detectRect_sortY_small2(const void*a, const void *b)
{
	DetectRect_Block_Str *c = (DetectRect_Block_Str *) a;
	DetectRect_Block_Str *d = (DetectRect_Block_Str *) b;
	return c->coord.y - d->coord.y;
}
int detectRect_sortX_small(const void*a, const void *b)
{
	DetectRect_Block_Str **c = (DetectRect_Block_Str **) a;
	DetectRect_Block_Str **d = (DetectRect_Block_Str **) b;
	return (*c)->coord.x - (*d)->coord.x;
}
int detectRect_sortX_small2(const void*a, const void *b)
{
	DetectRect_Block_Str *c = (DetectRect_Block_Str *) a;
	DetectRect_Block_Str *d = (DetectRect_Block_Str *) b;
	return c->coord.x - d->coord.x;
}
int detectRect_sortarea(const void *a, const void *b)
{
	DetectRect_Block_Str *c = (DetectRect_Block_Str *) a;
	DetectRect_Block_Str *d = (DetectRect_Block_Str *) b;
	return d->area - c->area;
}

//int detectRect_sorttheta(const void *a, const void *b)
//{
//	LinePair*c = (LinePair*) a;
//	LinePair*d = (LinePair*) b;
//	return d->angle - c->angle;
//}

int detectRect_longLineBlockY_small2(Detect_LineInfo *LineInfo)
{
	unsigned int rectNum = LineInfo->rectNum;
	unsigned i,j;
	DetectRect_Block_Str tmp;

	for(i=0;i<rectNum;i++)
	{
		for(j=0;j<rectNum-i-1;j++)
		{
			if(LineInfo->block[j]->coord.y > LineInfo->block[j+1]->coord.y)
			{
				memcpy(&tmp,LineInfo->block[j],sizeof(DetectRect_Block_Str));
				memcpy(LineInfo->block[j],LineInfo->block[j+1],sizeof(DetectRect_Block_Str));
				memcpy(LineInfo->block[j+1],&tmp,sizeof(DetectRect_Block_Str));
			}
		}
	}

	return 0;
}

int detectHorizontalLongBlock(DetectRect_Block_Str *start,Detect_LineInfo *longGroup,unsigned int longBlockVOffset,DetectRect_Block_Str **detectLongBlock)
{
	DetectRect_Block_Str *pLine1Block;
	DetectRect_Block_Str *pLine2Block;
	unsigned int j;
	unsigned int absDiff;
	int ret = -1;
	
	*detectLongBlock = NULL;
	if((start == NULL) || (longGroup == NULL))
		return ret;

	pLine1Block = start;
	
	for(j=0;j<longGroup->rectNum;j++)
	{
		pLine2Block = longGroup->block[j];
		if(pLine2Block->used == 1)
			continue;
		absDiff = abs(pLine2Block->coord.y - pLine1Block->coord.y);
		if(absDiff < longBlockVOffset)
		{
			*detectLongBlock = pLine2Block;
			ret = 0;
			break;
		}	
		if(pLine2Block->coord.y > pLine1Block->coord.y)
			break;	
	}

	return ret;
}

int CompensateLongBlock(CodeGroup *group)
{
	unsigned lineNum = group->groupNum;
	unsigned lineIdx;
	unsigned comlineIdx;
	unsigned blockIdx;
	unsigned tmpIdx;
	CodePair *pLineInfo;
	CodePair *pLineComInfo;
	DetectRect_Block_Str *pLineYcoordReplaceBlock;
	DetectRect_Block_Str *pLineXcoordReplaceBlock;
	unsigned flag = 0;

	group->comLongCnt = 0;

	for(lineIdx=0;lineIdx<lineNum;lineIdx++)
	{
		pLineInfo = &(group->codeGroup[lineIdx]);

		if(pLineInfo->longBlockNum < DEF_VERTICALLINE_CNT)
		{
			pLineXcoordReplaceBlock = NULL;
			pLineYcoordReplaceBlock = NULL;
			for(blockIdx=0;blockIdx<DEF_VERTICALLINE_CNT;blockIdx++)
			{
				if(pLineInfo->longBlock[blockIdx] == NULL)
				{
					flag = 1;
					for(comlineIdx=0;comlineIdx<lineNum;comlineIdx++)
					{
						if(comlineIdx != lineIdx)
						{
							pLineComInfo = &(group->codeGroup[comlineIdx]);
							if(pLineComInfo->longBlock[blockIdx] == NULL)
							{
								flag = 0;
								break;							
							}
							else
							{
								pLineXcoordReplaceBlock = pLineComInfo->longBlock[blockIdx];
							}		
						}
					}

					
					if(flag == 1)
					{
						if(pLineYcoordReplaceBlock == NULL)
						{
							for(tmpIdx=0;tmpIdx<DEF_VERTICALLINE_CNT;tmpIdx++)
							{
								if(pLineInfo->longBlock[blockIdx] != NULL)
								{
									pLineYcoordReplaceBlock = pLineInfo->longBlock[blockIdx];
									break;
								}
							}
						}
				
						if((pLineXcoordReplaceBlock != NULL)&&(pLineYcoordReplaceBlock != NULL))
						{
							group->comlongBlock[group->comLongCnt].area = (pLineXcoordReplaceBlock->area + 	pLineYcoordReplaceBlock->area)/2;
							group->comlongBlock[group->comLongCnt].coord.x = pLineXcoordReplaceBlock->coord.x;
							group->comlongBlock[group->comLongCnt].coord.y = pLineYcoordReplaceBlock->coord.y;				
							group->comlongBlock[group->comLongCnt].used = 1;
							group->comlongBlock[group->comLongCnt].index = -1;
							pLineInfo->longBlock[blockIdx] = &(group->comlongBlock[group->comLongCnt]);
							group->comLongCnt++;
							pLineInfo->longBlockNum++;
							if(group->comLongCnt > GVSS_MAX_VERTICALLINE_CNT*GVSS_MAX_HORIZOLLINE_CNT)
								return -1;
						}
						else
							return -1;
					}
					else
						return -1;	
				}
				else
					pLineYcoordReplaceBlock = pLineInfo->longBlock[blockIdx];
			}

		}
	}

	return 0;
}

int HorizalLongBlockGroup_Detect(DetectRect_Analysis* anal,unsigned startLineNum,unsigned totalLineNum,DetectRect_DParams_Str *dParams)
{
	int status = 0;
	unsigned int i,j;

	DetectRect_Block_Str *detectLongBlock;
	unsigned autoLongBlockNum = 0;
	unsigned horizonLineCnt = 0;

	memset(anal->code.codeGroup,0,sizeof(anal->code.codeGroup));
	if(anal->longGroup[startLineNum].rectNum < anal->code.groupNum)
		return PARSE_LONGBLOCK_HCOLNOTENOUGH;

	for(i=0;i<anal->longGroup[startLineNum].rectNum;i++)
	{
		autoLongBlockNum = 0;
		anal->code.codeGroup[horizonLineCnt].longBlock[autoLongBlockNum++] = anal->longGroup[startLineNum].block[i];
		anal->code.codeGroup[horizonLineCnt].longBlockNum ++;		
		for(j=startLineNum+1;j<startLineNum+DEF_VERTICALLINE_CNT;j++)
		{
			status = detectHorizontalLongBlock(anal->longGroup[startLineNum].block[i],&(anal->longGroup[j]),dParams->longBlockVOffset,&detectLongBlock);
			if(status == 0)
			{
				anal->code.codeGroup[horizonLineCnt].longBlock[autoLongBlockNum++] = detectLongBlock;
				anal->code.codeGroup[horizonLineCnt].longBlockNum ++;
			}
			else
				anal->code.codeGroup[horizonLineCnt].longBlock[autoLongBlockNum++] = NULL;
		}
		
		if(anal->code.codeGroup[horizonLineCnt].longBlockNum < DEF_VERTICALLINE_CNT - 1)
		{
			anal->code.codeGroup[horizonLineCnt].longBlockNum = 0;
		}
		else
		{
			for(j=0;j<autoLongBlockNum;j++)
			{
				if(anal->code.codeGroup[horizonLineCnt].longBlock[j] != NULL)
					anal->code.codeGroup[horizonLineCnt].longBlock[j]->used = 1;
			}
			horizonLineCnt++;
		}

		if(horizonLineCnt >= anal->code.groupNum)
		{
			break;
		}
	}
	
	if(horizonLineCnt != anal->code.groupNum)
		return PARSE_LONGBLOCK_HCOLNOTENOUGH;

	status = CompensateLongBlock(&anal->code);
	if(status != 0)
		return PARSE_LONGBLOCK_COMSATEFAIL;

	unsigned hDisRange1 = 0;
	unsigned hDisRange2 = 0;
	float RangeRatio = 0;
	for(i=0;i<horizonLineCnt;i++)
	{
		hDisRange1 = anal->code.codeGroup[i].longBlock[1]->coord.x - anal->code.codeGroup[i].longBlock[0]->coord.x;
		hDisRange2 = anal->code.codeGroup[i].longBlock[2]->coord.x - anal->code.codeGroup[i].longBlock[1]->coord.x;

		if(hDisRange2 > hDisRange1)
			RangeRatio = (float)hDisRange2 / (float)hDisRange1;
		else
			RangeRatio = (float)hDisRange1 / (float)hDisRange2;
		if(RangeRatio >= 1.5)
			return PARSE_LONGBLOCK_TOOLONG;
		
	}

	return 	PARSE_OK;
}
int detectRect_analysis(DetectRect_Str* obj, int num)
{
	unsigned int i;
	int ret = PARSE_OK;

	DetectRect_Analysis* anal = obj->Process.blobAnal;
	DetectRect_DParams_Str* dParams = &obj->DParams;
	//sort CCL by area
	qsort(anal->block, num, sizeof(DetectRect_Block_Str), detectRect_sortarea);
	//use Otsu threshold to separate large block and small block
	int thres = 0;
	float preg = 0.0;
	for (i = 1; i < num; i++)
	{
		float w0, w1, u0, u1, g;
		w0 = w1 = u0 = u1 = g = 0.0;
		w0 = (float) i / num;
		w1 = (float) (num - i) / num;
		unsigned int j;

		for (j = 0; j < i; j++)
		{
			u0 += anal->block[j].area;
		}
		u0 /= i;
		for (j = i; j < num; j++)
		{
			u1 += anal->block[j].area;
		}
		u1 /= (num - i);
		g = w0 * w1 * (u1 - u0) * (u1 - u0);
		if (g > preg)
		{
			thres = i;
			preg = g;
		}
	}

//if(dParams->isLongBlockVvalid == 0)
	DetectRect_Block_Str *pStart;
	unsigned int verticalLineCnt = 0;
	int rectNum = 0;
	int rectCnt = 0;
	int idx;
	int jnum;
	int start = 0;
	unsigned int absDiff = 0;
//	int verticalRectMinNum = ((rectCnt>dParams->detectRowsNum - 1)>0)?(dParams->detectRowsNum - 1):0;
	int verticalRectMinNum = ((dParams->detectRowsNum - 1)>0)?(dParams->detectRowsNum - 1):0;
	memset(anal->longGroup,0,sizeof(anal->longGroup));
	
	qsort(&anal->block, thres, sizeof(DetectRect_Block_Str),detectRect_sortX_small2);

	while(rectNum<thres)
	{
		pStart = &anal->block[rectNum];
		start = rectNum;
		rectCnt = 1;
		
		for(idx=rectNum + 1;idx<thres;idx++)
		{
			absDiff = abs(anal->block[idx].coord.x - pStart->coord.x);
			if(absDiff < dParams->longBlockHOffset)
			{
				rectCnt++;
			}
			else
				break;
		}
		rectNum += rectCnt;
	
		if(rectCnt>=verticalRectMinNum)//consider dectecting 4 horizonal line 3 block is not enough
		{
//			if((verticalLineCnt==0)&&(rectCnt<dParams->detectRowsNum))
//				continue;
			rectCnt = rectCnt>9?9:rectCnt;
			for(jnum = 0;jnum < rectCnt;jnum++)
				anal->longGroup[verticalLineCnt].block[jnum] = &anal->block[start + jnum];
			anal->longGroup[verticalLineCnt].rectNum = rectCnt;	
			detectRect_longLineBlockY_small2(&(anal->longGroup[verticalLineCnt]));

			verticalLineCnt++;
		}

		
		if(verticalLineCnt >= GVSS_MAX_VERTICALLINE_CNT)
			break;
	}

	if (verticalLineCnt < DEF_VERTICALLINE_CNT)
		return PARSE_LONGBLOCK_VCOLNOTENOUGH;


	anal->code.groupNum=dParams->detectRowsNum;


	for(i=0;i<=verticalLineCnt - DEF_VERTICALLINE_CNT;i++)
	{
		ret = HorizalLongBlockGroup_Detect(anal,i,verticalLineCnt,dParams);
		if(ret == PARSE_OK)
			break;
	}
#if 0	
	unsigned int j;
	DetectRect_Block_Str *detectLongBlock;
	unsigned autoLongBlockNum = 0;
	unsigned horizonLineCnt = 0;

	for(i=0;i<anal->longGroup[0].rectNum;i++)
	{
		autoLongBlockNum = 0;
		anal->code.codeGroup[horizonLineCnt].longBlock[autoLongBlockNum++] = anal->longGroup[0].block[i];
		anal->code.codeGroup[horizonLineCnt].longBlockNum ++;		
		for(j=1;j<verticalLineCnt;j++)
		{
			status = detectHorizontalLongBlock(anal->longGroup[0].block[i],&(anal->longGroup[j]),dParams->longBlockVOffset,&detectLongBlock);
			if(status == 0)
			{
				anal->code.codeGroup[horizonLineCnt].longBlock[autoLongBlockNum++] = detectLongBlock;
				anal->code.codeGroup[horizonLineCnt].longBlockNum ++;
			}
			else
				anal->code.codeGroup[horizonLineCnt].longBlock[autoLongBlockNum++] = NULL;
		}
		
		if(anal->code.codeGroup[horizonLineCnt].longBlockNum < DEF_VERTICALLINE_CNT - 1)
		{
			anal->code.codeGroup[horizonLineCnt].longBlockNum = 0;
		}
		else
		{
			for(j=0;j<autoLongBlockNum;j++)
			{
				if(anal->code.codeGroup[horizonLineCnt].longBlock[j] != NULL)
					anal->code.codeGroup[horizonLineCnt].longBlock[j]->used = 1;
			}
			horizonLineCnt++;
		}

		if(horizonLineCnt >= anal->code.groupNum)
		{
			break;
		}
	}

	if(horizonLineCnt != anal->code.groupNum)
		return PARSE_LONGBLOCK_HCOLNOTENOUGH;

	status = CompensateLongBlock(&anal->code);
	if(status != 0)
		return PARSE_LONGBLOCK_COMSATEFAIL;

	unsigned hDisRange1 = 0;
	unsigned hDisRange2 = 0;
	float RangeRatio = 0;
	for(i=0;i<horizonLineCnt;i++)
	{
		hDisRange1 = anal->code.codeGroup[i].longBlock[1]->coord.x - anal->code.codeGroup[i].longBlock[0]->coord.x;
		hDisRange2 = anal->code.codeGroup[i].longBlock[2]->coord.x - anal->code.codeGroup[i].longBlock[1]->coord.x;
		if(hDisRange2 > hDisRange1)
			RangeRatio = (float)hDisRange2 / (float)hDisRange1;
		else
			RangeRatio = (float)hDisRange1 / (float)hDisRange2;
		if(RangeRatio >= 1.5)
			return PARSE_LONGBLOCK_TOOLONG;
		
	}
	//add shortBlock into codeGroup;
#endif
	unsigned MingapLongBlock = 0;
	unsigned gaptmp1 = 0;
	unsigned gaptmp2 = 0;

	if(ret == PARSE_OK)
	{
		for(i=0;i<anal->code.groupNum;i++)
		{

			gaptmp1 = anal->code.codeGroup[i].longBlock[1]->coord.x - anal->code.codeGroup[i].longBlock[0]->coord.x;
			gaptmp2 = anal->code.codeGroup[i].longBlock[2]->coord.x - anal->code.codeGroup[i].longBlock[1]->coord.x;
			MingapLongBlock = (gaptmp2 > gaptmp1) ? gaptmp1 : gaptmp2;


			ret = detectShortRec(anal->code.codeGroup[i].longBlock[0],anal->code.codeGroup[i].longBlock[1],MingapLongBlock,i,0,anal,thres,num,dParams->shortBlockVOffset);
			if(ret != 0)
				break;

			ret = detectShortRec(anal->code.codeGroup[i].longBlock[1],anal->code.codeGroup[i].longBlock[2],MingapLongBlock,i,1,anal,thres,num,dParams->shortBlockVOffset);
			if(ret != 0)
				break;
		}
	}

	return ret;
}

int detectRect_parse(DetectRect_Str* detectStr,DetectRect_DParams_Str* dParams)
{
	int y1, x1, mod,i;
	int code[GVSS_MAX_CODELINE*2];
	memset(code,0,sizeof(int)*GVSS_MAX_CODELINE*2);
	int status = CODE_OK;

	DetectRect_Analysis* anal = detectStr->Process.blobAnal;
	DetectRect_Result_Str *result = detectStr->Result;
	CodePair* codeGroup = anal->code.codeGroup;
	CodeInfo* codeInfo = &(anal->codeInfo);
	unsigned int codeLineNum = anal->code.groupNum;

	memset(result->code,0,GVSS_MAX_CODELINE*2);
	memset(codeInfo,0,sizeof(CodeInfo));
	
	result->position = 0.0;
	result->distance = 0.0;
	unsigned directionFlag = 0;
	//first, decode

	//xxxxxxx*******xxxxxxx*******xxxxxxx*******xxxxxxxx
	//xxxxxxx*******xcode1x*******xcode2x*******xxxxxxxx
	//xxxxxxx*******xxxxxxx*******xxxxxxx*******xxxxxxxx
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	//xxxxxxx*******xxxxxxx*******xxxxxxx*******xxxxxxxx
	//xxxxxxx*******xcode3x*******xcode4x*******xxxxxxxx
	//xxxxxxx*******xxxxxxx*******xxxxxxx*******xxxxxxxx
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	//xxxxxxx*******xxxxxxx*******xxxxxxx*******xxxxxxxx
	//xxxxxxx*******xcode5x*******xcode6x*******xxxxxxxx
	//xxxxxxx*******xxxxxxx*******xxxxxxx*******xxxxxxxx

	if (codeGroup[0].code[0] == 0 || codeGroup[0].code[1] == 0)
	{
		for (i = 0; i < codeLineNum; i++)
		{
			code[2*i]= detectRectConvertNum(codeGroup[i].code[0], RIGHT2LEFT);
			code[2*i+1]= detectRectConvertNum(codeGroup[i].code[1], RIGHT2LEFT);
		}
		directionFlag = 1;
	} else if (codeGroup[1].code[0] == 0 || codeGroup[1].code[1] == 0)
	{
		for (i = 0; i < codeLineNum; i++)
		{
			code[2 * i] = detectRectConvertNum(codeGroup[i].code[0], LEFT2RIGHT);
			code[2 * i + 1] = detectRectConvertNum(codeGroup[i].code[1], LEFT2RIGHT);
		}
	} else
		return CODE_INVALIDFORMAT;

	for(i=0;i<codeLineNum*2;i++)
		result->code[i]=code[i];

	if(status == CODE_OK)
	{
		if (code[2] == 0)
		{
			if (code[1] == 1 && code[0] == 10 && code[3] == 2)
			{
				x1 = 1;
				y1 = 120 * (11 * (code[(codeLineNum - 1) * 2 + 1] - 1) + code[(codeLineNum - 1) * 2]);
				codeInfo->index = (float)(x1 + y1 - 0.5);
			} else if (code[0] == 11)
			{
				mod = (code[1] + 1) % 11;
				if (mod == 0)
				{
					if (code[3] != 11)
						return CODE_CHECKERR;
				} else
				{
					if (mod != code[3])
						return CODE_CHECKERR;
				}
				x1 = 11 * (code[1] - 1) + 1;
				y1 = 120 * ((code[(codeLineNum - 1) * 2 + 1] - 1) * 11 + code[(codeLineNum - 1) * 2] - 1);
				codeInfo->index = (float)(x1 + y1 - 0.5);
			} else
			{	
				mod = (code[1] + code[0] + 1) % 11;
				if (mod == 0)
				{
					if (code[3] != 11)
						return CODE_CHECKERR;
				} else
				{
					if (mod != code[3])
						return CODE_CHECKERR;
				}

				x1 = (code[1] - 1) * 11 + code[0] + 1;
				y1 = 120 * (11 * (code[(codeLineNum - 1) * 2 + 1] - 1) + code[(codeLineNum - 1) * 2] - 1);
				codeInfo->index = (float)(x1 + y1 - 0.5);
			}
		} else if (code[3] == 0)
		{
			mod = (code[0] + code[1]) % 11;
			if (mod == 0)
			{
				if (code[2] != 11)
					return CODE_CHECKERR;
			} else
			{
				if (mod != code[2])
					return CODE_CHECKERR;
			}
			x1 = (code[0] - 1) * 11 + code[1];
			y1 = 120 * (11 * (code[(codeLineNum - 1) * 2] - 1) + code[(codeLineNum - 1) * 2 + 1] - 1);
			codeInfo->index = x1 + y1;
		} else
		{
			status = CODE_INVALIDFORMAT;
		}

		if(status == CODE_OK)
		{
			//get board positon
			codeInfo->position = (codeInfo->index / 5.0) * 1000;
			//get center offset
			int pixCenterOffset = 0; 
			int pixWidth = 0;
			unsigned CenterPos = detectStr->Process.frame.orgcols / 2;
			if (directionFlag == 0)   //left to right
			{
				pixCenterOffset = CenterPos - codeGroup[0].longBlock[2]->coord.x;	
				pixCenterOffset += CenterPos - codeGroup[1].longBlock[2]->coord.x;
				pixCenterOffset += CenterPos - codeGroup[2].longBlock[2]->coord.x;
				pixCenterOffset /= 3;
			}	
			else   			//right to left
			{
				pixCenterOffset = CenterPos - codeGroup[0].longBlock[0]->coord.x;	
				pixCenterOffset += CenterPos - codeGroup[1].longBlock[0]->coord.x;
				pixCenterOffset += CenterPos - codeGroup[2].longBlock[0]->coord.x;
				pixCenterOffset /= 3;			
			}
			pixCenterOffset -= detectStr->Process.frame.HStart;

			pixWidth = codeGroup[0].longBlock[2]->coord.x - codeGroup[0].longBlock[0]->coord.x;
			pixWidth += codeGroup[1].longBlock[2]->coord.x - codeGroup[1].longBlock[0]->coord.x;
			pixWidth += codeGroup[2].longBlock[2]->coord.x - codeGroup[2].longBlock[0]->coord.x;
			pixWidth /= 3;

			float offset = (float) (pixCenterOffset * 200 / pixWidth);
			codeInfo->position = (codeInfo->position + offset) * detectStr->DParams.posMconst + detectStr->DParams.posAconst;
		}
	}
	float f = dParams->focus;

	//calculate distance;
	int pixDistance = codeGroup[dParams->detectRowsNum-1].longBlock[0]->coord.y
			- codeGroup[0].longBlock[0]->coord.y;
	pixDistance += codeGroup[dParams->detectRowsNum-1].longBlock[1]->coord.y
			- codeGroup[0].longBlock[1]->coord.y;
	pixDistance += codeGroup[dParams->detectRowsNum-1].longBlock[2]->coord.y
			- codeGroup[0].longBlock[2]->coord.y;
	codeInfo->distance = (float) (100 * f * 1000 * 3.0)
			/ ((float) pixDistance * SENSOR_RADIUS);
	codeInfo->distance = dParams->mconst * codeInfo->distance + dParams->aconst;
	//calculate accurate position

	result->position = codeInfo->position;
	result->distance = codeInfo->distance;
	return CODE_OK;
}
int detectRect_trace(DetectRect_Str*obj)
{
	int i;
	CodeTrace* trace = &obj->Process.traceOpt;
	VLIB_CCHandle* CCHandle = obj->Process.CC.handle;
	CodePair* codeGroup = obj->Process.blobAnal->code.codeGroup;
	unsigned int codeLineNum = obj->Process.blobAnal->code.groupNum;
	DetectRect_DParams_Str* dParams = &(obj->DParams);
	IMG_Frame* frame = &(obj->Process.frame);
	VLIB_CC block;

	
	int top = frame->VStart;
	int bottom = frame->VEnd-1;
	for (i = 0; i < 3; i++)
	{
		if(codeGroup[0].longBlock[i]->index != -1)
		{
			VLIB_getCCFeatures(CCHandle, &block, codeGroup[0].longBlock[i]->index);
			block.ymin += frame->VStart;
			if(top == frame->VStart)
				top = block.ymin;
			else
				top = (block.ymin < top) ? block.ymin : top;
		}

		if(codeGroup[codeLineNum-1].longBlock[i]->index != -1)
		{
			VLIB_getCCFeatures(CCHandle, &block, codeGroup[codeLineNum-1].longBlock[i]->index);
			block.ymax += frame->VStart;
			if(bottom == frame->VEnd-1)
				bottom = block.ymax;
			else
				bottom = (block.ymax > bottom) ? block.ymax : bottom;
		}
	}


	int left_block = frame->HStart,right_block = frame->HEnd - 1;

	for(i=0;i<codeLineNum;i++)
	{
		if(codeGroup[i].longBlock[0]->index != -1)
		{
			VLIB_getCCFeatures(CCHandle, &block, codeGroup[i].longBlock[0]->index);
			block.xmin += frame->HStart;
			if(left_block == frame->HStart)
				left_block = block.xmin;
			else
				left_block = (block.xmin < left_block)?block.xmin:left_block;	
		}

		if(codeGroup[i].longBlock[2]->index != -1)
		{
			VLIB_getCCFeatures(CCHandle, &block, codeGroup[i].longBlock[2]->index);
			block.xmax += frame->HStart;
			if(right_block == frame->HEnd - 1)
				right_block = block.xmax;
			right_block=(block.xmax > right_block)?block.xmax:right_block;	
		}
	}

	float offset = (codeGroup[1].longBlock[2]->coord.x-codeGroup[1].longBlock[1]->coord.x)*(dParams->ROIRation)+0.5;

	trace->top = top-(int)offset;
	trace->bottom = bottom + (int)offset;
	trace->left = left_block;
	trace->right = right_block;
	//printf("trace top: %d,trace bottom: %d,trace left:%d, trace right:%d",trace->top,trace->bottom,trace->left,trace->right);


	if(trace->top < frame->VStart)
		trace->top = frame->VStart;
	if(trace->bottom > frame->VEnd-1)
		trace->bottom = frame->VEnd-1;
	if(trace->left < frame->HStart)
		trace->left = frame->HStart;
	if(trace->right > frame->HEnd - 1)
		trace->right = frame->HEnd - 1;
	//printf("trace top: %d,trace bottom: %d,trace left:%d, trace right:%d",trace->top,trace->bottom,trace->left,trace->right);

	return 0;
}
Bool isDetectRectValid(void* prm)
{
	DetectRect_DParams_Str* params = (DetectRect_DParams_Str*)prm;
	if(params->frameLeftRatio<0||params->frameLeftRatio>1)
		return FALSE;
	if(params->frameRightRatio<0||params->frameRightRatio>1)
		return FALSE;
	if(params->frameTopRatio<0||params->frameTopRatio>1)
		return FALSE;
	if(params->frameBottomRatio<0||params->frameBottomRatio>1)
		return FALSE;
	if(params->focus<0)
		return FALSE;
	if(params->RectSimilarity<0||params->RectSimilarity>1)
		return FALSE;
	if(params->ROIRation<0||params->ROIRation>1)
		return FALSE;
	if(!(params->isDetectSimilarity==1||params->isDetectSimilarity==0))
		return FALSE;
	if(!(params->isLongBlockVvalid==1||params->isLongBlockVvalid==0))
		return FALSE;
	if(params->detectRowsNum<GVSS_MIN_CODELINE||params->detectRowsNum>GVSS_MAX_CODELINE)
		return FALSE;
	return TRUE;
}
static EzDetectRecObj gDetectRec;

static int DetectRec_algNumAlloc()
{
	return 15;
}

static int DetectRec_algAlloc(int NummemTab, ALG_MemRec *memTab)
{
	int size = MAX_FRAME_WIDTH * MAX_FRAME_HEIGHT;

	memTab[0].alignment = 8;
	memTab[0].attrs = ALG_SCRATCH;
	memTab[0].size = size;
	memTab[0].space = ALG_DARAM0;

	memTab[1].alignment = 8;
	memTab[1].attrs = ALG_SCRATCH;
	memTab[1].size = size;
	memTab[1].space = ALG_DARAM0;

	memTab[2].alignment = 8;
	memTab[2].attrs = ALG_SCRATCH;
	memTab[2].size = size;
	memTab[2].space = ALG_DARAM0;

	memTab[3].alignment = 8;
	memTab[3].attrs = ALG_SCRATCH;
	memTab[3].size = VLIB_GetSizeOfCCHandle();
	memTab[3].space = ALG_DARAM0;

	int maxBytesRequired = 0;
	VLIB_calcConnectedComponentsMaxBufferSize(MAX_FRAME_WIDTH,
	MAX_FRAME_HEIGHT, 200, &maxBytesRequired);
	memTab[4].alignment = 8;
	memTab[4].attrs = ALG_SCRATCH;
	memTab[4].size = maxBytesRequired / 4;
	memTab[4].space = ALG_DARAM0;

	memTab[5].alignment = 8;
	memTab[5].attrs = ALG_SCRATCH;
	memTab[5].size = size;
	memTab[5].space = ALG_DARAM0;

	memTab[6].alignment = 8;
	memTab[6].attrs = ALG_SCRATCH;
	memTab[6].size = (256 + 257) * sizeof(unsigned char)
			+ (256 * 3) * sizeof(unsigned short);
	memTab[6].space = ALG_SARAM;

	memTab[7].alignment = 8;
	memTab[7].attrs = ALG_SCRATCH;
	memTab[7].size = sizeof(DetectRect_Result_Str);
	memTab[7].space = ALG_DARAM0;

	memTab[8].alignment = 8;
	memTab[8].attrs = ALG_SCRATCH;
	memTab[8].size = sizeof(DetectRect_Analysis);
	memTab[8].space = ALG_DARAM0;

	maxBytesRequired = IMG_ApproxPoly_WorkBufSize();
	memTab[9].alignment = 8;
	memTab[9].attrs = ALG_SCRATCH;
	memTab[9].size = maxBytesRequired;
	memTab[9].space= ALG_DARAM0;

	memTab[10].alignment = 8;
	memTab[10].attrs = ALG_SCRATCH;
	memTab[10].size = size;
	memTab[10].space = ALG_DARAM0;

	memTab[11].alignment = 8;
	memTab[11].attrs = ALG_SCRATCH;
	memTab[11].size = size;
	memTab[11].space = ALG_DARAM0;

	memTab[12].alignment = 8;
	memTab[12].attrs = ALG_SCRATCH;
	memTab[12].size = size;
	memTab[12].space = ALG_DARAM0;
	
	memTab[13].alignment = 8;
	memTab[13].attrs = ALG_SCRATCH;
	memTab[13].size = MORPH_MASK_WIDTH*MORPH_MASK_HEIGHT;
	memTab[13].space = ALG_SARAM;

	memTab[14].alignment = 8;
	memTab[14].attrs = ALG_SCRATCH;
	memTab[14].size = MORPH_MASK_WIDTH*MORPH_MASK_HEIGHT;
	memTab[14].space = ALG_SARAM;

	return 0;
}

static int DetectRec_algInit(ALG_Handle handle, int NummemRec,
		const ALG_MemRec *memTab)
{
	EzDetectRecObj *obj = Contain_Of(handle, EzDetectRecObj, algObj);

	memset(&obj->detectStr.DParams,0,sizeof(obj->detectStr.DParams));
	obj->detectStr.DParams.frameLeftRatio = DEF_LEFTRATIO;
	obj->detectStr.DParams.frameRightRatio = DEF_RIGHTRATIO;
	obj->detectStr.DParams.frameTopRatio = DEF_TOPRATIO;
	obj->detectStr.DParams.frameBottomRatio = DEF_BOTTOMRATIO;
	obj->detectStr.DParams.focus = DEF_FOCALLEN;
	obj->detectStr.DParams.mconst = DEF_MCONST;
	obj->detectStr.DParams.aconst = DEF_ACONST;
	obj->detectStr.DParams.posMconst = DEF_POSMCONST;
	obj->detectStr.DParams.posAconst = DEF_POSACONST;
	obj->detectStr.DParams.maxArea = DEF_MAXAREA;
	obj->detectStr.DParams.minArea = DEF_MINAREA;
	obj->detectStr.DParams.longBlockHOffset=DEF_LONGBLOCKOFFSET;
	obj->detectStr.DParams.longBlockVOffset=DEF_LVERTICALOFFSET;
	obj->detectStr.DParams.shortBlockVOffset=DEF_SHORTBLOCKOFFSET;
	obj->detectStr.DParams.RectSimilarity = DEF_RECTSIMILARITY;
	obj->detectStr.DParams.ROIRation = DEF_ROIRATIO;
	obj->detectStr.DParams.isTopHatValid = 0;
	obj->detectStr.DParams.viewImgSource = IMG_ORGINAL;

	//default roi
	obj->detectStr.Process.frame.orgcols = DEF_IMGWIDTH;
	obj->detectStr.Process.frame.orgrows = DEF_IMGHEIGHT;
	float HStart,HEnd,VStart,VEnd;
	HStart = DEF_IMGWIDTH * DEF_LEFTRATIO;
	HEnd = DEF_IMGWIDTH * DEF_RIGHTRATIO;
	VStart = DEF_IMGHEIGHT * DEF_TOPRATIO;
	VEnd = DEF_IMGHEIGHT * DEF_BOTTOMRATIO;
	obj->detectStr.Process.frame.HStart =  (((unsigned int)HStart) >> 5) << 5;
	obj->detectStr.Process.frame.HEnd = (((unsigned int)HEnd)>> 5) << 5;
	obj->detectStr.Process.frame.VStart = (((unsigned int)VStart) >> 5) << 5;
	obj->detectStr.Process.frame.VEnd = (((unsigned int)VEnd) >> 5) << 5;
	obj->detectStr.Process.traceOpt.left=obj->detectStr.Process.frame.HStart;
	obj->detectStr.Process.traceOpt.right=obj->detectStr.Process.frame.HEnd;
	obj->detectStr.Process.traceOpt.top = obj->detectStr.Process.frame.VStart;
	obj->detectStr.Process.traceOpt.bottom = obj->detectStr.Process.frame.VEnd;
	obj->detectStr.DParams.isDetectSimilarity = DEF_ISDETECTSIMILARITY;

	unsigned int codeRowsNum = DEF_DETECTROWNUM;
	if(codeRowsNum>5)
		codeRowsNum=GVSS_MAX_CODELINE;
	else if(codeRowsNum<3)codeRowsNum=GVSS_MIN_CODELINE;
		obj->detectStr.DParams.detectRowsNum=codeRowsNum;

	obj->detectStr.Control.sourceImg = 1;
	int i;
	for (i = 0; i < NummemRec; i++)
		memset(memTab[i].base, 0, memTab[i].size);

	obj->detectStr.Process.frame.srcimg.cols = MAX_FRAME_WIDTH;
	obj->detectStr.Process.frame.srcimg.rows = MAX_FRAME_HEIGHT;
	obj->detectStr.Process.frame.srcimg.data = memTab[0].base;

	obj->detectStr.Process.frame.processimg.cols = MAX_FRAME_WIDTH;
	obj->detectStr.Process.frame.processimg.rows = MAX_FRAME_HEIGHT;
	obj->detectStr.Process.frame.processimg.data = memTab[1].base;

	obj->detectStr.Process.frame.scaledimg.cols = MAX_FRAME_WIDTH;
	obj->detectStr.Process.frame.scaledimg.rows = MAX_FRAME_HEIGHT;
	obj->detectStr.Process.frame.scaledimg.data = memTab[2].base;

	obj->detectStr.Process.frame.morphimg.cols = MAX_FRAME_WIDTH;
	obj->detectStr.Process.frame.morphimg.rows = MAX_FRAME_HEIGHT;
	obj->detectStr.Process.frame.morphimg.data = memTab[12].base;

	obj->detectStr.Process.CC.handle = memTab[3].base;
	obj->detectStr.Process.CC.handleSize = memTab[3].size;

	obj->detectStr.Process.CC.workBuf = memTab[4].base;
	obj->detectStr.Process.CC.workBufSize = memTab[4].size;

	obj->detectStr.Process.CC.packedImg = memTab[5].base;
	obj->detectStr.Process.CC.packedImgTmp = memTab[10].base;

	VLIB_initConnectedComponentsList(obj->detectStr.Process.CC.handle,
			obj->detectStr.Process.CC.workBuf,
			obj->detectStr.Process.CC.workBufSize);

	obj->detectStr.Process.hist = memTab[6].base;

	IMG_DSP_InitHistogram(obj->detectStr.Process.hist);

	obj->detectStr.Result = memTab[7].base;

	obj->detectStr.Process.blobAnal = memTab[8].base;

	obj->detectStr.Process.approxHandle.workBuf = memTab[9].base;
	obj->detectStr.Process.approxHandle.workBufSize = memTab[9].size;
	
	IMG_ApproxPoly_Init(&obj->detectStr.Process.approxHandle);

	obj->detectStr.Process.Morph.scratch1_ptr = memTab[11].base;
	obj->detectStr.Process.Morph.se_ptr = memTab[13].base;
	for(i=0;i<MORPH_MASK_WIDTH*MORPH_MASK_HEIGHT;i++)
		obj->detectStr.Process.Morph.se_ptr[i]=1;
	obj->detectStr.Process.Morph.refl_se_ptr = memTab[14].base;
	for(i=0;i<MORPH_MASK_WIDTH*MORPH_MASK_HEIGHT;i++)
		obj->detectStr.Process.Morph.refl_se_ptr[i]=1;
	return 0;
}

static void DetectRec_algActivate(ALG_Handle handle)
{

}

static int DetectRec_algControl(ALG_Handle handle, ALG_Cmd cmd, void * InArgs,
		void * OutArgs)
{
		Alg_Params *in_arg = (Alg_Params *) InArgs;
		Alg_Params *out_arg = (Alg_Params *) OutArgs;
		EzDetectRecObj *obj = Contain_Of(handle, EzDetectRecObj, algObj);
	
		switch (cmd)
		{
			case ALG_SET_PARAM:
			{
				if (in_arg->prmSize != sizeof(DetectRect_DParams_Str))
				{
					return INVALIDPARAMS;
				}
				if(!isDetectRectValid(in_arg->prm))
				{
					return INVALIDPARAMS;
				}
				memcpy(&obj->detectStr.DParams, in_arg->prm, in_arg->prmSize);
				float HStart,HEnd,VStart,VEnd;
				HStart = DEF_IMGWIDTH * DEF_LEFTRATIO;
				HEnd = DEF_IMGWIDTH * DEF_RIGHTRATIO;
				VStart = DEF_IMGHEIGHT * DEF_TOPRATIO;
				VEnd = DEF_IMGHEIGHT * DEF_BOTTOMRATIO;
				obj->detectStr.Process.frame.HStart =  (((unsigned int)HStart) >> 5) << 5;
				obj->detectStr.Process.frame.HEnd = (((unsigned int)HEnd)>> 5) << 5;
				obj->detectStr.Process.frame.VStart = (((unsigned int)VStart) >> 5) << 5;
				obj->detectStr.Process.frame.VEnd = (((unsigned int)VEnd) >> 5) << 5;
				if(obj->detectStr.Process.traceOpt.left < obj->detectStr.Process.frame.HStart)
					obj->detectStr.Process.traceOpt.left = obj->detectStr.Process.frame.HStart;
				if(obj->detectStr.Process.traceOpt.right > obj->detectStr.Process.frame.HEnd)
					obj->detectStr.Process.traceOpt.right = obj->detectStr.Process.frame.HEnd;
				if(obj->detectStr.Process.traceOpt.top < obj->detectStr.Process.frame.VStart)
					obj->detectStr.Process.traceOpt.top = obj->detectStr.Process.frame.HStart;
				if(obj->detectStr.Process.traceOpt.bottom > obj->detectStr.Process.frame.VEnd)
					obj->detectStr.Process.traceOpt.bottom = obj->detectStr.Process.frame.VEnd;
				break;
			}
			case ALG_GET_PARAM:
			{
				if(out_arg->prmSize < sizeof(DetectRect_DParams_Str))
					return INVALIDPARAMS;
					
				memcpy(out_arg->prm, &obj->detectStr.DParams, sizeof(DetectRect_DParams_Str));
				out_arg->prmSize = sizeof(DetectRect_DParams_Str);
				break;
			}
			case ALG_PARAM_ISVALID:
			{
				if (in_arg->prmSize != sizeof(DetectRect_DParams_Str))
					return INVALIDPARAMS;
				if(!isDetectRectValid(in_arg->prm))
					return INVALIDPARAMS;
				break;	
			}
			case ALG_SET_IMG_SOURCE:
			{
				obj->detectStr.Control.sourceImg = !(obj->detectStr.Control.sourceImg);
				break;
			}		
			default:
				return INVALIDPARAMS;	
		}
	return PROCESS_OK;
}

static void DetectRec_algDeactivate(ALG_Handle handle)
{

}

static int DetectRec_algFree(ALG_Handle handle, int NummemRec,
		ALG_MemRec *memTab)
{
	int size = MAX_FRAME_WIDTH * MAX_FRAME_HEIGHT;
	EzDetectRecObj *obj = Contain_Of(handle, EzDetectRecObj, algObj);

	memTab[0].alignment = 8;
	memTab[0].attrs = ALG_SCRATCH;
	memTab[0].size = size;
	memTab[0].space = ALG_DARAM0;
	memTab[0].base = obj->detectStr.Process.frame.srcimg.data;

	memTab[1].alignment = 8;
	memTab[1].attrs = ALG_SCRATCH;
	memTab[1].size = size;
	memTab[1].space = ALG_DARAM0;
	memTab[1].base = obj->detectStr.Process.frame.processimg.data;

	memTab[2].alignment = 8;
	memTab[2].attrs = ALG_SCRATCH;
	memTab[2].size = size;
	memTab[2].space = ALG_DARAM0;
	memTab[2].base = obj->detectStr.Process.frame.scaledimg.data;

	memTab[3].alignment = 8;
	memTab[3].attrs = ALG_SCRATCH;
	memTab[3].size = obj->detectStr.Process.CC.handleSize;
	memTab[3].space = ALG_DARAM0;
	memTab[3].base = obj->detectStr.Process.CC.handle;

	memTab[4].alignment = 8;
	memTab[4].attrs = ALG_SCRATCH;
	memTab[4].size = obj->detectStr.Process.CC.workBufSize;
	memTab[4].space = ALG_DARAM0;
	memTab[4].base = obj->detectStr.Process.CC.workBuf;

	memTab[5].alignment = 8;
	memTab[5].attrs = ALG_SCRATCH;
	memTab[5].size = size;
	memTab[5].space = ALG_DARAM0;
	memTab[5].base = obj->detectStr.Process.CC.packedImg;

	memTab[6].alignment = 8;
	memTab[6].attrs = ALG_SCRATCH;
	memTab[6].size = (256 + 257) * sizeof(unsigned char)
			+ (256 * 3) * sizeof(unsigned short);
	memTab[6].space = ALG_SARAM;
	memTab[6].base = obj->detectStr.Process.hist;

	memTab[7].alignment = 8;
	memTab[7].attrs = ALG_SCRATCH;
	memTab[7].size = sizeof(DetectRect_Result_Str);
	memTab[7].space = ALG_DARAM0;
	memTab[7].base = obj->detectStr.Result;

	memTab[8].alignment = 8;
	memTab[8].attrs = ALG_SCRATCH;
	memTab[8].size = sizeof(DetectRect_Analysis);
	memTab[8].space = ALG_DARAM0;
	memTab[8].base = obj->detectStr.Process.blobAnal;
	
	memTab[9].alignment = 8;
	memTab[9].attrs = ALG_SCRATCH;
	memTab[9].size = obj->detectStr.Process.approxHandle.workBufSize;
	memTab[9].space = ALG_DARAM0;
	memTab[9].base = obj->detectStr.Process.approxHandle.workBuf;

	memTab[10].alignment = 8;
	memTab[10].attrs = ALG_SCRATCH;
	memTab[10].size = size;
	memTab[10].space = ALG_DARAM0;
	memTab[10].base = obj->detectStr.Process.CC.packedImgTmp;

	memTab[11].alignment = 8;
	memTab[11].attrs = ALG_SCRATCH;
	memTab[11].size = size;
	memTab[11].space = ALG_DARAM0;
	memTab[11].base = obj->detectStr.Process.Morph.scratch1_ptr;

	memTab[12].alignment = 8;
	memTab[12].attrs = ALG_SCRATCH;
	memTab[12].size = size;
	memTab[12].space = ALG_DARAM0;
	memTab[12].base = obj->detectStr.Process.frame.morphimg.data;

	memTab[13].alignment = 8;
	memTab[13].attrs = ALG_SCRATCH;
	memTab[13].size = MORPH_MASK_WIDTH*MORPH_MASK_HEIGHT;
	memTab[13].space = ALG_SARAM;
	memTab[13].base = obj->detectStr.Process.Morph.se_ptr;
	
	memTab[14].alignment = 8;
	memTab[14].attrs = ALG_SCRATCH;
	memTab[14].size = MORPH_MASK_WIDTH*MORPH_MASK_HEIGHT;
	memTab[14].space = ALG_SARAM;
	memTab[14].base = obj->detectStr.Process.Morph.se_ptr;
	
	return 0;
}

static void DetectRec_algMoved(ALG_Handle handle, const ALG_MemRec *memTab,
		ALG_Handle parent, const ALG_Params *params)
{

}


IMG_Point detectRect_BlobCentroid(IMG_FrameData* src,VLIB_CC cc,int blobNum,int index)
{

	IMG_Point centroid;

	if(blobNum>255)
		centroid = IMG_BlobCentroid_rect(src,cc);
	else
		centroid = IMG_BlobCentroid_map(src,cc,index);

	return centroid;
}


int detectRect_isEnclosedRect(EzDetectRecObj* obj,VLIB_CC cc,float similarity,int index)
{
	IMG_FrameData* srcFrame = &obj->detectStr.Process.frame.processimg;
	IMG_FrameData* dstFrame = &obj->detectStr.Process.frame.srcimg;
	IMG_ApproxPoly_Handle* approxHandle = &obj->detectStr.Process.approxHandle;

	int left = cc.xmin;
	int top = cc.ymin;
	int rectWidth = cc.xmax-cc.xmin;
	int rectHeight = cc.ymax-cc.ymin;

	dstFrame->cols = rectWidth+2;
	dstFrame->rows = rectHeight+2;

	
	unsigned char * dst = dstFrame->data;
	unsigned char * src = srcFrame->data+(top-1)*srcFrame->cols+left-1;

	obj->algObj.fxns->algdma_actcbx((char *)dst, (char *)src,dstFrame->cols,dstFrame->rows, srcFrame->cols, dstFrame->cols,1);

	int ret = IMG_ApproxPoly(approxHandle,dstFrame,index);


	if(ret!=IMG_APPROXPOLY_OK)
		return FALSE;

	int mer = IMG_ApproxPoly_MER(approxHandle);
	float curSimilarity = (float)cc.area/(float)mer;

	//printf("cursimilarity %f,similarity %f\n",curSimilarity,similarity);

	if(curSimilarity<similarity)
		return FALSE;
	else

		return TRUE;
}

static int Gvss_Process(EzDetectRecObj *obj,int *repeat)
{
	int ret = PROCESS_OK;
	int status = PROCESS_OK;

	int recNum = 0;
	unsigned rectWidth,rectHeight;
	status = VLIB_getNumCCs(obj->detectStr.Process.CC.handle, &recNum);

	*repeat = 0;

	obj->detectStr.Result->blockNum = 0;
	memset(obj->detectStr.Process.blobAnal, 0, sizeof(DetectRect_Analysis));
	memset(obj->detectStr.Result,0,sizeof(DetectRect_Result_Str));

	int i;
	int tempnum = 0;

	//create CCMap override processimg if necessary
	if(recNum<=255)
		VLIB_createCCMap8Bit(obj->detectStr.Process.CC.handle,obj->detectStr.Process.frame.processimg.data,obj->detectStr.Process.frame.processimg.cols,obj->detectStr.Process.frame.processimg.rows);
	
	
	for (i = 0; i < recNum; i++)
	{
		VLIB_CC cc;
		int state = VLIB_getCCFeatures(obj->detectStr.Process.CC.handle, &cc,
				i);
		if ((cc.xmin == 0)
				|| (cc.xmax >= obj->detectStr.Process.frame.scaledimg.cols - 1) 
				|| (cc.ymin <= obj->detectStr.Process.traceOpt.top-obj->detectStr.Process.frame.VStart)
				|| (cc.ymax >= obj->detectStr.Process.traceOpt.bottom-obj->detectStr.Process.frame.VStart-1)
		)
		{
			continue;
		}

		if (obj->detectStr.DParams.maxArea != 0)
			if (cc.area > obj->detectStr.DParams.maxArea)
			{
				continue;
			}
		
		if (obj->detectStr.DParams.minArea != 0)
			if (cc.area < obj->detectStr.DParams.minArea)
		{
			continue;
		}


		if(obj->detectStr.DParams.isDetectSimilarity!=0)
		{
			int idx = recNum>255?255:i+1;
			if(detectRect_isEnclosedRect(obj,cc,obj->detectStr.DParams.RectSimilarity,idx) == FALSE)
				continue;
		}

		rectWidth = cc.xmax-cc.xmin;
		rectHeight = cc.ymax-cc.ymin;

		if(rectHeight >= 2*rectWidth)
			continue;

		//get blob centoid
		IMG_Point centroid = detectRect_BlobCentroid(&obj->detectStr.Process.frame.processimg,cc,recNum,i+1);
//		printf("centroid (%d,%d) center(%d,%d)",centroid.x,centroid.y,(cc.xmin + cc.xmax) / 2,(cc.ymin + cc.ymax) / 2);
//#if 0 
		obj->detectStr.Process.blobAnal->block[tempnum].area = cc.area;
		obj->detectStr.Process.blobAnal->block[tempnum].coord.x = obj->detectStr.Process.frame.HStart + centroid.x;
		obj->detectStr.Process.blobAnal->block[tempnum].coord.y = obj->detectStr.Process.frame.VStart + centroid.y;
		obj->detectStr.Process.blobAnal->block[tempnum].index = i;
		obj->detectStr.Result->block[tempnum].area = cc.area;
		obj->detectStr.Result->block[tempnum].coord.x = obj->detectStr.Process.frame.HStart + centroid.x;
		obj->detectStr.Result->block[tempnum].coord.y = obj->detectStr.Process.frame.VStart + centroid.y;
//#endif		
#if 0
		obj->detectStr.Process.blobAnal->block[tempnum].area = cc.area;
		obj->detectStr.Process.blobAnal->block[tempnum].coord.x = obj->detectStr.Process.frame.HStart + (cc.xmin + cc.xmax) / 2;
		obj->detectStr.Process.blobAnal->block[tempnum].coord.y = obj->detectStr.Process.frame.VStart + (cc.ymin + cc.ymax) / 2;
		obj->detectStr.Process.blobAnal->block[tempnum].index = i;
		obj->detectStr.Result->block[tempnum].area = cc.area;
		obj->detectStr.Result->block[tempnum].coord.x = obj->detectStr.Process.frame.HStart + (cc.xmin + cc.xmax) / 2;
		obj->detectStr.Result->block[tempnum].coord.y = obj->detectStr.Process.frame.VStart + (cc.ymin + cc.ymax) / 2;
#endif
		tempnum++;
		if(tempnum >= MAX_LABELCOUNT)
			break;
	}
	
	obj->detectStr.Result->blockNum = tempnum;
	
	status = detectRect_analysis(&obj->detectStr, tempnum);
	if (status != 0)
		ret = status + TYPE_PARSE;
	if(status == PROCESS_OK)
	{
		status = detectRect_parse(&obj->detectStr,
				&obj->detectStr.DParams);
		if (status != 0)
			ret = status + TYPE_CODE;
	}
	if (status != PROCESS_OK)
	{
		if((obj->detectStr.Process.traceOpt.top == obj->detectStr.Process.frame.VStart)&&
				(obj->detectStr.Process.traceOpt.bottom == obj->detectStr.Process.frame.VEnd)&&
					(obj->detectStr.Process.traceOpt.left == obj->detectStr.Process.frame.HStart)&&
						(obj->detectStr.Process.traceOpt.right == obj->detectStr.Process.frame.HEnd))
		{
			*repeat = 0;
		}
		else
		{
//			*repeat = 1;	
		}		
		obj->detectStr.Process.traceOpt.top = obj->detectStr.Process.frame.VStart;
		obj->detectStr.Process.traceOpt.bottom = obj->detectStr.Process.frame.VEnd;
		obj->detectStr.Process.traceOpt.left = obj->detectStr.Process.frame.HStart;
		obj->detectStr.Process.traceOpt.right = obj->detectStr.Process.frame.HEnd;
	} 
	else
	{	
		int shortBlockNum,longBlockNum,lineIndex;
		DetectRect_Analysis* anal = obj->detectStr.Process.blobAnal;
		tempnum = 0;
		for(lineIndex=0;lineIndex<anal->code.groupNum;lineIndex++)
		{
			for(longBlockNum = 0;longBlockNum<anal->code.codeGroup[lineIndex].longBlockNum;longBlockNum++)
			{
				memcpy(&obj->detectStr.Result->block[tempnum],anal->code.codeGroup[lineIndex].longBlock[longBlockNum],sizeof(DetectRect_Result_BlockStr));
				tempnum++;
			}

			for(shortBlockNum = 0;shortBlockNum<anal->code.codeGroup[lineIndex].shortBlockNum;shortBlockNum++)
			{
				memcpy(&obj->detectStr.Result->block[tempnum],anal->code.codeGroup[lineIndex].shortBlock[shortBlockNum],sizeof(DetectRect_Result_BlockStr));
				tempnum++;
			}
	
		}

		obj->detectStr.Result->blockNum = tempnum;

		detectRect_trace(&obj->detectStr);
	}
	return ret;
}
static int DetectRec_algProcess(ALG_Handle handle, void *InArgs, void *Result)
{
	static int lastRet = PROCESS_OK;
	int ret = PROCESS_OK;
	AlgProcessArgs* inData = InArgs;
	EzDetectRecObj *obj = Contain_Of(handle, EzDetectRecObj, algObj);
	int cols = inData->width;
	int rows = inData->height;

	if (cols > MAX_FRAME_WIDTH || rows > MAX_FRAME_HEIGHT)
	{
		ret = INVALIDPARAMS;
		lastRet = ret;
		return ret;
	}

	obj->detectStr.Process.frame.srcimg.cols = cols;
	obj->detectStr.Process.frame.srcimg.rows = rows;

	//scale src data
	IMG_FrameData orignImg;
	orignImg.cols = cols;
	orignImg.rows = rows;
	orignImg.data = inData->data;

	obj->detectStr.Process.frame.orgcols = cols;
	obj->detectStr.Process.frame.orgrows = rows;

	if(obj->detectStr.DParams.isTopHatValid==1)
	{
		obj->algObj.fxns->algdma_actcbx((char *)obj->detectStr.Process.frame.srcimg.data, (char *)orignImg.data,
				cols,
				rows,
				inData->pitch, 
				cols,
				1);
		//image pyramid
		VLIB_imagePyramid8(obj->detectStr.Process.frame.srcimg.data,orignImg.cols,orignImg.rows,obj->detectStr.Process.frame.scaledimg.data);
		
		//erode and dilate with 1/4 src size img 
		VLIB_grayscale_erode_mask(cols/2,cols/2,rows/2,obj->detectStr.Process.frame.scaledimg.data,MORPH_MASK_WIDTH/2+1,MORPH_MASK_HEIGHT,obj->detectStr.Process.Morph.se_ptr,obj->detectStr.Process.Morph.scratch1_ptr,obj->detectStr.Process.frame.processimg.data);			
		VLIB_grayscale_dilate_mask(cols/2,cols/2,rows/2,obj->detectStr.Process.frame.processimg.data,MORPH_MASK_WIDTH/2+1,MORPH_MASK_HEIGHT,obj->detectStr.Process.Morph.se_ptr,obj->detectStr.Process.Morph.scratch1_ptr,obj->detectStr.Process.frame.morphimg.data);
		
		//morphology diff
		unsigned padWidth = 2*(MORPH_MASK_WIDTH - 1);
		unsigned padHeight = 2*(MORPH_MASK_HEIGHT - 1);
		unsigned leftShift = padWidth/2;
		unsigned rightShift = cols - padWidth/2;
		unsigned topShift = padHeight/2;
		unsigned bottomShift = rows - padHeight/2;

		unsigned leftShiftScaled = 2*(MORPH_MASK_WIDTH/2+1-1)/2;
		unsigned topShiftScaled = 2*(MORPH_MASK_HEIGHT/2+1-1)/2;

		unsigned k,l;
		unsigned char* pDiff = obj->detectStr.Process.frame.srcimg.data;
		unsigned char* pSrc = obj->detectStr.Process.frame.scaledimg.data;
		unsigned char* pMorph = obj->detectStr.Process.frame.morphimg.data;
		

		for(l=0;l<rows;l++)
		{
			for(k=0;k<cols;k++)
			{
				if((l<topShift)||(l>bottomShift)||(k<leftShift)||(k>rightShift))
					continue;
				else
				{
					*(pDiff+l*cols+k)=*(pSrc+(l/2)*(cols/2)+(k/2))-*(pMorph+(k/2)-leftShiftScaled+((l/2)-topShiftScaled)*(cols/2));
					//*(pDiff + l*cols + k) = *(pSrc + l*cols + k) - *(pMorph + k - (leftShift) + (l-topShift)*cols);
				}
			}
		}
//		obj->detectStr.Process.frame.scaledimg.cols = cols/2;
//		obj->detectStr.Process.frame.scaledimg.rows = rows/2;
//		detectRect_expandSub(&obj->detectStr.Process.frame.srcimg,&obj->detectStr.Process.frame.scaledimg,&obj->detectStr.Process.frame.morphimg);
		
	}
//use 4x size;
#if 0
	if(obj->detectStr.DParams.isTopHatValid==1)
	{
		obj->algObj.fxns->algdma_actcbx((char *)obj->detectStr.Process.frame.srcimg.data, (char *)orignImg.data,
				cols,
				rows,
				inData->pitch, 
				cols,
				1);
		//image pyramid
		VLIB_imagePyramid8(obj->detectStr.Process.frame.srcimg.data,orignImg.cols,orignImg.rows,obj->detectStr.Process.frame.scaledimg.data);
		
		//erode and dilate with 1/16 src size img 
		VLIB_grayscale_erode_mask(cols/4,cols/4,rows/4,obj->detectStr.Process.frame.scaledimg.data+(cols*rows)/4,MORPH_MASK_WIDTH/4+1,MORPH_MASK_HEIGHT,obj->detectStr.Process.Morph.se_ptr,obj->detectStr.Process.Morph.scratch1_ptr,obj->detectStr.Process.frame.processimg.data);			
		VLIB_grayscale_dilate_mask(cols/4,cols/4,rows/4,obj->detectStr.Process.frame.processimg.data,MORPH_MASK_WIDTH/4+1,MORPH_MASK_HEIGHT,obj->detectStr.Process.Morph.se_ptr,obj->detectStr.Process.Morph.scratch1_ptr,obj->detectStr.Process.frame.morphimg.data);
		
		//morphology diff
		unsigned padWidth = 2*(MORPH_MASK_WIDTH - 1);
		unsigned padHeight = 2*(MORPH_MASK_HEIGHT - 1);
		unsigned leftShift = padWidth/2;
		unsigned rightShift = cols - padWidth/2;
		unsigned topShift = padHeight/2;
		unsigned bottomShift = rows - padHeight/2;

		unsigned leftShiftScaled = 2*(MORPH_MASK_WIDTH/4+1-1)/2;
		unsigned topShiftScaled = 2*(MORPH_MASK_HEIGHT/4+1-1)/2;

		unsigned k,l;
		unsigned char* pDiff = obj->detectStr.Process.frame.srcimg.data;
		unsigned char* pSrc = obj->detectStr.Process.frame.scaledimg.data+(cols*rows)/4;
		unsigned char* pMorph = obj->detectStr.Process.frame.morphimg.data;
		

		for(l=0;l<rows;l++)
		{
			for(k=0;k<cols;k++)
			{
				if((l<topShift)||(l>bottomShift)||(k<leftShift)||(k>rightShift))
					continue;
				else
				{
					*(pDiff+l*cols+k)=*(pSrc+(l/4)*(cols/4)+(k/4))-*(pMorph+(k/4)-leftShiftScaled+((l/4)-topShiftScaled)*(cols/4));
					//*(pDiff + l*cols + k) = *(pSrc + l*cols + k) - *(pMorph + k - (leftShift) + (l-topShift)*cols);
				}
			}
		}
//		obj->detectStr.Process.frame.scaledimg.cols = cols/2;
//		obj->detectStr.Process.frame.scaledimg.rows = rows/2;
//		detectRect_expandSub(&obj->detectStr.Process.frame.srcimg,&obj->detectStr.Process.frame.scaledimg,&obj->detectStr.Process.frame.morphimg);
		
	}
#endif
	//get ROI
	obj->detectStr.Process.frame.HStart = cols * obj->detectStr.DParams.frameLeftRatio;
	obj->detectStr.Process.frame.HEnd = cols * obj->detectStr.DParams.frameRightRatio;
	obj->detectStr.Process.frame.VStart = rows * obj->detectStr.DParams.frameTopRatio;
	obj->detectStr.Process.frame.VEnd = rows * obj->detectStr.DParams.frameBottomRatio;
	obj->detectStr.Process.frame.HStart = ((obj->detectStr.Process.frame.HStart >> 5) << 5);
	obj->detectStr.Process.frame.HEnd = ((obj->detectStr.Process.frame.HEnd >> 5) << 5);
	obj->detectStr.Process.frame.VStart = ((obj->detectStr.Process.frame.VStart >> 5) << 5);
	obj->detectStr.Process.frame.VEnd = ((obj->detectStr.Process.frame.VEnd >> 5) << 5);
	

	obj->detectStr.Process.frame.scaledimg.cols = obj->detectStr.Process.frame.HEnd - obj->detectStr.Process.frame.HStart;
	obj->detectStr.Process.frame.scaledimg.rows = obj->detectStr.Process.frame.VEnd - obj->detectStr.Process.frame.VStart;

	unsigned char* src;
	unsigned char* dst = obj->detectStr.Process.frame.scaledimg.data;


	if(obj->detectStr.DParams.isTopHatValid == 1)
	{
		src = obj->detectStr.Process.frame.srcimg.data + obj->detectStr.Process.frame.HStart + cols*obj->detectStr.Process.frame.VStart;
		obj->algObj.fxns->algdma_actcbx((char *)dst, (char *)src,
			obj->detectStr.Process.frame.scaledimg.cols,
			obj->detectStr.Process.frame.scaledimg.rows, cols,
			obj->detectStr.Process.frame.scaledimg.cols,0);
	}
	else
	{
		src = orignImg.data + obj->detectStr.Process.frame.HStart + inData->pitch*obj->detectStr.Process.frame.VStart;
		obj->algObj.fxns->algdma_actcbx((char *)dst, (char *)src,
			obj->detectStr.Process.frame.scaledimg.cols,
			obj->detectStr.Process.frame.scaledimg.rows, inData->pitch,
			obj->detectStr.Process.frame.scaledimg.cols,0);		
	}
	


	//thresholding
	obj->detectStr.Process.frame.processimg.cols =
			obj->detectStr.Process.frame.scaledimg.cols;

	obj->detectStr.Process.frame.processimg.rows =
			obj->detectStr.Process.frame.scaledimg.rows;
	


	//use trace roi for thresholding
	IMG_FrameData thresimg;

	thresimg.cols = obj->detectStr.Process.frame.scaledimg.cols;

	thresimg.rows = obj->detectStr.Process.traceOpt.bottom-obj->detectStr.Process.traceOpt.top;

	thresimg.data = obj->detectStr.Process.frame.scaledimg.data + obj->detectStr.Process.frame.scaledimg.cols * (obj->detectStr.Process.traceOpt.top - obj->detectStr.Process.frame.VStart);

	int threshold = IMG_getThreshold(&thresimg,obj->detectStr.Process.hist);	


	IMG_Threshold(&obj->detectStr.Process.frame.scaledimg,

			&obj->detectStr.Process.frame.processimg, threshold);

	unsigned int frameSize = 0;

	frameSize = obj->detectStr.Process.frame.processimg.rows* obj->detectStr.Process.frame.processimg.cols;

	VLIB_packMask32(obj->detectStr.Process.frame.processimg.data,
			obj->detectStr.Process.CC.packedImg,frameSize);
	

	VLIB_createConnectedComponentsList(
			obj->detectStr.Process.CC.handle,
			obj->detectStr.Process.frame.processimg.cols,
			obj->detectStr.Process.frame.processimg.rows,
			obj->detectStr.Process.CC.packedImg, 30, 0);

	

	int repeatFlag = 0;

	do
	{
		ret = Gvss_Process(obj,&repeatFlag);
	}while((ret != PROCESS_OK)&&(repeatFlag == 1));


	switch(obj->detectStr.DParams.viewImgSource)
	{
		case IMG_BINARY:
		{
			unsigned int m,n;
			unsigned char *ptr = obj->detectStr.Process.frame.processimg.data;
			src = orignImg.data + obj->detectStr.Process.frame.HStart + inData->pitch*obj->detectStr.Process.frame.VStart;

			VLIB_unpackMask32(obj->detectStr.Process.CC.packedImg,obj->detectStr.Process.frame.processimg.data,obj->detectStr.Process.frame.processimg.rows* obj->detectStr.Process.frame.processimg.cols);


			for(m = 0;m<obj->detectStr.Process.frame.scaledimg.rows;m++)
			{
				for(n=0;n<obj->detectStr.Process.frame.scaledimg.cols;n++)
				{
		
					*(ptr + m * obj->detectStr.Process.frame.scaledimg.cols + n) = *(ptr + m * obj->detectStr.Process.frame.scaledimg.cols + n)*255;
				}
			}
			if(obj->algObj.fxns->algdma_actcbx != NULL)
			{
				obj->algObj.fxns->algdma_actcbx((char *)src ,(char *)obj->detectStr.Process.frame.processimg.data,
					obj->detectStr.Process.frame.scaledimg.cols,
						obj->detectStr.Process.frame.scaledimg.rows, obj->detectStr.Process.frame.scaledimg.cols,
							inData->pitch,1);
			}			
			break;
		}
		case IMG_TOPHAT:
		{
			if(obj->detectStr.DParams.isTopHatValid == 1)
			{
				obj->algObj.fxns->algdma_actcbx((char *)orignImg.data, (char *)(obj->detectStr.Process.frame.srcimg.data),
					cols,
					rows, 
					cols,
				inData->pitch,1);
			}
			break;
		}
		default:
			break;
		
	}


	Alg_Result* algResult = Result;
	if (ret != PROCESS_OK && lastRet != ret)
		algResult->Alg_saveImg = 0;
	else
		algResult->Alg_saveImg = 0;
	algResult->saveInfo.Alg_saveImgIndex = ret / 1000;
	algResult->saveInfo.errNo = ret;
	memcpy((char*) algResult->saveInfo.Alg_saveImgdirName, "GVSS",
			strlen("GVSS") + 1);
	algResult->weightValid = 0;
	algResult->resultSize = sizeof(DetectRect_Result_Str);

	obj->detectStr.Result->errIndex = ret;
	obj->detectStr.Result->top = obj->detectStr.Process.traceOpt.top;
	obj->detectStr.Result->bottom = obj->detectStr.Process.traceOpt.bottom;
	obj->detectStr.Result->left = obj->detectStr.Process.traceOpt.left;
	obj->detectStr.Result->right = obj->detectStr.Process.traceOpt.right;
	
	memcpy(algResult->result, obj->detectStr.Result,algResult->resultSize);

	lastRet = ret;		

	return ret;
}

struct ALG_Fxns Detect_fxns;

void DetectRec_ModuleInit()
{
	Detect_fxns.implementationId = NULL;
	Detect_fxns.algNumAlloc = DetectRec_algNumAlloc;
	Detect_fxns.algAlloc = DetectRec_algAlloc;
	Detect_fxns.algInit = DetectRec_algInit;
	Detect_fxns.algActivate = DetectRec_algActivate;
	Detect_fxns.algDeactivate = DetectRec_algDeactivate;
	Detect_fxns.algFree = DetectRec_algFree;
	Detect_fxns.algMoved = DetectRec_algMoved;
	Detect_fxns.algProcess = DetectRec_algProcess;
	Detect_fxns.algControl = DetectRec_algControl;
	gDetectRec.algObj.fxns = &Detect_fxns;

	Alg_DrvregisterDriver(ALG_MODULE_GVSS, &(gDetectRec.algObj));
}

void DetectRec_ModuleExit()
{
	Alg_DrvunRegisterDriver(ALG_MODULE_GVSS);
}


