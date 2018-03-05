/*
 * IMG_ApproxPoly.c
 *
 *  Created on: 2017年5月24日
 *      Author: tupers
 */

#include "IMG_ApproxPoly.h"

extern int Vps_printf(char *format, ...);
/**
 * @brief IMG_ApproxPoly_WorkBufSize
 * @return
 */
int IMG_ApproxPoly_WorkBufSize()
{
    int chainSize = sizeof(unsigned char)*IMG_APPROXPOLY_CONTOURPOINT;
    int compressSize = sizeof(IMG_mapPoint)*IMG_APPROXPOLY_COMPRESSPOINT;
    int convexSize = sizeof(IMG_mapPoint*)*IMG_APPROXPOLY_COMPRESSPOINT;
    //int resultSize = sizeof(IMG_Point)*IMG_APPROXPOLY_RESULTPOINT;

    return chainSize+compressSize+convexSize;
}

/**
 * @brief IMG_ApproxPoly_Init
 * @param handle
 */
void IMG_ApproxPoly_Init(IMG_ApproxPoly_Handle *handle)
{
	//init handle buf
	unsigned char* pbuf = (unsigned char*)handle->workBuf;
	handle->chain.code=pbuf;
	handle->compress.mapPoint=(IMG_mapPoint*)(pbuf+sizeof(unsigned char)*IMG_APPROXPOLY_CONTOURPOINT);
	handle->convexhull.pointPtrArr=(IMG_mapPoint**)(pbuf+sizeof(unsigned char)*IMG_APPROXPOLY_CONTOURPOINT+sizeof(IMG_mapPoint)*IMG_APPROXPOLY_COMPRESSPOINT);
	//handle->compressDown.point=(IMG_Point*)(pbuf+sizeof(unsigned char)*IMG_APPROXPOLY_CONTOURPOINT+sizeof(IMG_Point)*IMG_APPROXPOLY_COMPRESSPOINT+sizeof(IMG_Point)*IMG_APPROXPOLY_APPROXPOINT);
	//handle->result.point=(IMG_Point*)(pbuf+sizeof(unsigned char)*IMG_APPROXPOLY_CONTOURPOINT+sizeof(IMG_Point)*IMG_APPROXPOLY_COMPRESSPOINT+2*sizeof(IMG_Point)*IMG_APPROXPOLY_APPROXPOINT);

    //init directTable
    handle->directTable[0][0]=1;
    handle->directTable[0][1]=0;
    handle->directTable[1][0]=1;
    handle->directTable[1][1]=-1;
    handle->directTable[2][0]=0;
    handle->directTable[2][1]=-1;
    handle->directTable[3][0]=-1;
    handle->directTable[3][1]=-1;
    handle->directTable[4][0]=-1;
    handle->directTable[4][1]=0;
    handle->directTable[5][0]=-1;
    handle->directTable[5][1]=1;
    handle->directTable[6][0]=0;
    handle->directTable[6][1]=1;
    handle->directTable[7][0]=1;
    handle->directTable[7][1]=1;
}

/**
 * @brief IMG_ApproxPoly
 * @param handle
 * @param num
 * @return
 */
int IMG_ApproxPoly(IMG_ApproxPoly_Handle *handle, IMG_FrameData* src, int index)
{
    //find contour,suppose there is one label only
    int ret = IMG_ApproxPoly_FindContour(handle,src,index);
    if(ret!=IMG_APPROXPOLY_OK)
        return ret;
    //simplify chain code
    ret = IMG_ApproxPoly_SimplifyChain(handle);
    if(ret!=IMG_APPROXPOLY_OK)
        return ret;

	//find convexHull
	ret = IMG_ApproxPoly_ConvexHull(handle);
	if(ret!=IMG_APPROXPOLY_OK)
		return ret;
	return IMG_APPROXPOLY_OK;
}

/**
 * @brief IMG_ApproxPoly_FindContour
 * @param handle
 * @param src
 * @return
 */
int IMG_ApproxPoly_FindContour(IMG_ApproxPoly_Handle *handle, IMG_FrameData *src, int index)
{
    IMG_ApproxPoly_ContourChain* chain = &handle->chain;

    int rows=0;
    int cols=0;
    chain->cnt=0;
    int curx=0;
    int cury=0;
    unsigned char isfind=0;
    for(rows=1;rows<src->rows-1;rows++)
    {
        for(cols=1;cols<src->cols-1;cols++)
        {
            if(IMG_PixVal(src,cols,rows)==index)
            {
                chain->firstPoint.x=cols;
                chain->firstPoint.y=rows;
                chain->cnt=0;
                curx=cols;
                cury=rows;
                isfind=1;
            }
            if(isfind==1)
                break;
        }
        if(isfind==1)
            break;
    }

    int pos=0;
    do
    {
        pos = IMG_ApproxPoly_FindContourSingle(handle,src,curx,cury,pos);
        if(pos==9)
            break;
        curx = curx+handle->directTable[pos][0];
        cury = cury+handle->directTable[pos][1];
        chain->code[chain->cnt]=pos;
        chain->cnt++;
        if(curx==cols&&cury==rows)
        {
            int lastPos = IMG_ApproxPoly_FindContourSingle(handle,src,curx,cury,pos);
            if(lastPos==chain->code[0])
                break;
        }
    }while(chain->cnt<IMG_APPROXPOLY_CONTOURPOINT);
    if(chain->cnt>=IMG_APPROXPOLY_CONTOURPOINT)
        return IMG_APPROXPOLY_FINDCONTOUREXCEED;
    else
        return IMG_APPROXPOLY_OK;
}

/**
 * @brief IMG_ApproxPoly_FindContourSingle
 * @param table
 * @param src
 * @param curCols
 * @param curRows
 * @param lastPos
 * @return
 */
int IMG_ApproxPoly_FindContourSingle(IMG_ApproxPoly_Handle *handle, IMG_FrameData* src, int curCols, int curRows, int lastPos)
{
    unsigned int start = lastPos+3+8;
    unsigned int i;
    
    for(i=0;i<=7;i++)
    {
        unsigned int curPos = (start-i)%8;
        int x,y;
        x = curCols+handle->directTable[curPos][0];
        y=curRows+handle->directTable[curPos][1];
//        Vps_printf("the next point (%d,%d) (%d,%d)",x,y,handle->directTable[curPos][0],handle->directTable[curPos][1]);
        if(x>=0&&x<src->cols&&y>=0&&y<src->rows)
            if(IMG_PixVal(src,x,y)>0)
                return curPos;
    }
    return 9;
}

/**
 * @brief IMG_ApproxPoly_SimplifyChain
 * @param handle
 * @return
 */
int IMG_ApproxPoly_SimplifyChain(IMG_ApproxPoly_Handle *handle)
{
    IMG_ApproxPoly_ContourChain* chain = &handle->chain;
    IMG_mapPointGroup* compress = &handle->compress;
    IMG_Point curPoint;

    IMG_PointCpy(&compress->mapPoint[0].point,&chain->firstPoint);
    IMG_PointCpy(&curPoint,&compress->mapPoint[0].point);
    curPoint.x+=handle->directTable[chain->code[0]][0];
    curPoint.y+=handle->directTable[chain->code[0]][1];
    IMG_PointCpy(&compress->mapPoint[1].point,&curPoint);
    compress->cnt=2;

    int i;
    for(i=1;i<chain->cnt;i++)
    {
        unsigned char code = chain->code[i];
        curPoint.x+=handle->directTable[code][0];
        curPoint.y+=handle->directTable[code][1];
        if(code!=chain->code[i-1])
        {
            IMG_PointCpy(&compress->mapPoint[compress->cnt].point,&curPoint);
            compress->cnt++;
        }
        else if(chain->code[i-1]==(code+6)%8||chain->code[i-1]==(code+10)%8)
            compress->cnt--;
        if(compress->cnt>=IMG_APPROXPOLY_COMPRESSPOINT)
            return IMG_APPROXPOLY_SIMPLIFYCHAINEXCEED;
    }
    if(compress->mapPoint[compress->cnt-1].point.x==compress->mapPoint[0].point.x&&compress->mapPoint[compress->cnt-1].point.y==compress->mapPoint[0].point.y)
        compress->cnt--;
    return IMG_APPROXPOLY_OK;
}


#if 0
/**
 * @brief IMG_ApproxPoly_Single
 * @param start
 * @param end
 * @param chain
 * @param result
 */
void IMG_ApproxPoly_Single(int start, int end, IMG_PointGroup *chain, IMG_PointGroup *result)
{
    if(result->cnt>=IMG_APPROXPOLY_RESULTPOINT)
        return;
    else if(start+1==end)
        return;
    int maxIndex = 0;
    float distance = IMG_ApproxPoly_DPMax(start,end,chain->point,&maxIndex);
	float stdDistance = (chain->point[start].x-chain->point[end].x)*(chain->point[start].x-chain->point[end].x)+(chain->point[start].y-chain->point[end].y)*(chain->point[start].y-chain->point[end].y);
	stdDistance = (stdDistance/3)<9?9:stdDistance/3;
	//Vps_printf("stdDistace :%f, distance %f\n",stdDistance,distance);
	
    if(distance>=9)
    {
        IMG_ApproxPoly_Single(start,maxIndex,chain,result);
        IMG_PointCpy(&result->point[result->cnt],&chain->point[maxIndex]);
        result->cnt++;
        IMG_ApproxPoly_Single(maxIndex,end,chain,result);
    }

}

/**
 * @brief IMG_ApproxPoly_DPMax
 * @param startIndex
 * @param endIndex
 * @param chain
 * @param maxIndex
 * @return
 */
float IMG_ApproxPoly_DPMax(int startIndex, int endIndex, IMG_Point *chain, int *maxIndex)
{
    float distance = 0.0;
    float tempL = 0.0;
    IMG_Point start;
    IMG_Point end;
    IMG_PointCpy(&start,&chain[startIndex]);
    IMG_PointCpy(&end,&chain[endIndex]);
    float A,B,C;
    A = (float)(start.y-end.y)/(start.x-end.x);
    B = -1;
    C = start.y-A*start.x;
	int i;
    for(i=startIndex+1;i<endIndex;i++)
    {
        if((start.x-end.x)==0)
        {
            tempL = abs(chain[i].x-start.x);
            tempL*=tempL;
            }
        else if((start.y-end.y)==0)
        {
            tempL = abs(chain[i].y-start.y);
            tempL*=tempL;
            }
        else
        {
            tempL = abs(A*chain[i].x+B*chain[i].y+C);
            tempL*=tempL;
            tempL/=(A*A+B*B);
            }

        if(tempL>distance)
        {
            distance=tempL;
            *maxIndex=i;
        }
    }
    return distance;
}
#endif
/**
 * @brief IMG_ApproxPoly_MER
 * @param group
 * @return
 */
int IMG_ApproxPoly_MER(IMG_ApproxPoly_Handle* handle)
{
    	IMG_mapPoint **seq = handle->convexhull.pointPtrArr;
    	int num = handle->convexhull.cnt;
	if(num<=2)
		return 0;
    	int i;
    	int area;
    	int minArea=0x7FFFFFFF;
    	for(i=0;i<num;i++)
    	{
        	int index = (i+1+num)%num;
        	int difX = seq[index]->point.x-seq[i]->point.x;
        	int difY = seq[index]->point.y-seq[i]->point.y;
        	float difXY = sqrt((float)(difX*difX+difY*difY));
        	int isRotate = 1;
        	//calculate angle need to rotate
        	if(difX==0||difY==0)
            		isRotate = 0;
        	else if(difX>0)
            		difY=0-difY;
        	else
            		difX=0-difX;
        	int j;
        	int left = seq[i]->point.x;
        	int right = seq[i]->point.x;
        	int top = seq[i]->point.y;
        	int bottom = seq[i]->point.y;
        	if(isRotate)
        	{
            		//find rotated MER and calculate square
            		for(j=0;j<num;j++)
            		{
                		if(j==i)
                    			continue;
                		float rotateX = (seq[j]->point.x-seq[i]->point.x)*difX-(seq[j]->point.y-seq[i]->point.y)*difY;
                		rotateX = (float)rotateX/difXY;
                		rotateX += seq[i]->point.x;
                		float rotateY = (seq[j]->point.x-seq[i]->point.x)*difY+(seq[j]->point.y-seq[i]->point.y)*difX;
                		rotateY = (float)rotateY/difXY;
                		rotateY += seq[i]->point.y;
                		if(left>rotateX)
                    			left=rotateX;
                		else if(right<rotateX)
                    			right=rotateX;
                		if(top>rotateY)
                    			top=rotateY;
                		else if(bottom<rotateY)
                   	 		bottom=rotateY;
            		}

            		area = (right-left) * (bottom-top);
        	}
        	else
        	{
            		//find MBR and calculate square
            		for(j=0;j<num;j++)
            		{
                		if(left>seq[j]->point.x)
                    			left=seq[j]->point.x;
               			else if(right<seq[j]->point.x)
                    			right=seq[j]->point.x;
                		if(top>seq[j]->point.y)
                    			top=seq[j]->point.y;
                		else if(bottom<seq[j]->point.y)
                    			bottom=seq[j]->point.y;
            		}
            		area = (right-left) * (bottom-top);
        	}

        	if(area<minArea)
            		minArea=area;
    	}
    	return minArea;
}



//calculate centroid with rect
IMG_Point IMG_BlobCentroid_rect(IMG_FrameData* frame,VLIB_CC cc)
{
	unsigned int i,j;
	unsigned int cols = cc.xmax-cc.xmin;
	unsigned int rows = cc.ymax-cc.ymin;
	unsigned int num=0;
	unsigned int startX = cc.xmin;
	unsigned int startY = cc.ymin;
	unsigned int totalX = 0;
	unsigned int totalY = 0;
#if 0
	for(i=0;i<rows;i++)
	{
		unsigned int index = startIdx + i * pitch;
		for(j=0;j<cols;j++)
		{
			unsigned int idx = index + j;
			unsigned char val = frame->data[idx];
			if(val==255)
			{
				//x+=
				totalIdx+=idx;
				num++;
			}
		}
	}
#endif
	for(i=0;i<rows;i++)
	{
		for(j=0;j<cols;j++)
		{
			unsigned int idx = (startY+i)*frame->cols+startX+j;
			unsigned char val = frame->data[idx];
			if(val>0)
			{
				totalX+=startX+j;
				totalY+=startY+i;
				num++;
			}
		}
	}
	
	//unsigned int centroidIdx = totalIdx/num;
	
	IMG_Point centroid;
	//centroid.x=centroidIdx%frame->cols;
	//centroid.y=centroidIdx/frame->cols;
	
	centroid.x=totalX/num;
	centroid.y=totalY/num;
	return centroid;
}


//calculate centroid with map
IMG_Point IMG_BlobCentroid_map(IMG_FrameData* map,VLIB_CC cc,int index)
{
	unsigned int i,j;
	unsigned int cols = cc.xmax-cc.xmin;
	unsigned int rows = cc.ymax-cc.ymin;
	unsigned int num=0;
	unsigned int startX = cc.xmin;
	unsigned int startY = cc.ymin;
	unsigned int totalX = 0;
	unsigned int totalY = 0;
	for(i=0;i<rows;i++)
	{
		for(j=0;j<cols;j++)
		{
			unsigned int idx = (startY+i)*map->cols+startX+j;
			unsigned char val = map->data[idx];
			if(val==index)
			{
				totalX+=startX+j;
				totalY+=startY+i;
				num++;
			}
		}
	}
	
	//unsigned int centroidIdx = totalIdx/num;
	
	IMG_Point centroid;
	//centroid.x=centroidIdx%frame->cols;
	//centroid.y=centroidIdx/frame->cols;
	
	centroid.x=totalX/num;
	centroid.y=totalY/num;
	return centroid;
}

//sort function
int ConvexHull_sortPoint(const void *a, const void *b)
{
	IMG_mapPoint *c = (IMG_mapPoint *) a;
	IMG_mapPoint *d = (IMG_mapPoint *) b;
	if(c->point.y-d->point.y==0)
		return c->point.x-d->point.x;
	else
		return d->point.y-c->point.y;
}

//Graham-Scan
int IMG_ApproxPoly_ConvexHull(IMG_ApproxPoly_Handle* handle)
{
	int i;
	//sort find left corner point
	qsort(handle->compress.mapPoint, handle->compress.cnt, sizeof(IMG_mapPoint), ConvexHull_sortPoint);
	//init point used state;
	for(i=0;i<handle->compress.cnt;i++)
		handle->compress.mapPoint[i].used=0;
	//Vps_printf("first [%d,%d],last %d [%d,%d].\n",handle->compress.mapPoint[0].point.x,handle->compress.mapPoint[0].point.y,handle->compress.cnt,handle->compress.mapPoint[handle->compress.cnt-1].point.x,handle->compress.mapPoint[handle->compress.cnt-1].point.y);
	//right chain start
	handle->convexhull.cnt=0;
	handle->convexhull.pointPtrArr[handle->convexhull.cnt]=&handle->compress.mapPoint[0];
	handle->compress.mapPoint[0].used=1;
	handle->convexhull.cnt++;

	handle->convexhull.pointPtrArr[handle->convexhull.cnt]=&handle->compress.mapPoint[1];
	handle->compress.mapPoint[1].used=1;
	handle->convexhull.cnt++;
	
	for(i=2;i<handle->compress.cnt;i++)
	{
		
		while(handle->convexhull.cnt>=2&&IMG_ApproxPoly_PointCross(&handle->convexhull.pointPtrArr[handle->convexhull.cnt-2]->point,&handle->convexhull.pointPtrArr[handle->convexhull.cnt-1]->point,&handle->compress.mapPoint[i].point)>=0)
		{
			handle->convexhull.cnt--;
			handle->convexhull.pointPtrArr[handle->convexhull.cnt]->used=0;
		}
		handle->convexhull.pointPtrArr[handle->convexhull.cnt]=&handle->compress.mapPoint[i];
		handle->compress.mapPoint[i].used=1;
		handle->convexhull.cnt++;
	}

	//Vps_printf("right chain finish cnt:%d\n",handle->convexhull.cnt);

	//reverse left chain
	for(i=handle->compress.cnt-2;i>0;i--)
	{
		if(handle->compress.mapPoint[i].used==1)
			continue;

		while(handle->convexhull.cnt>=2&&IMG_ApproxPoly_PointCross(&handle->convexhull.pointPtrArr[handle->convexhull.cnt-2]->point,&handle->convexhull.pointPtrArr[handle->convexhull.cnt-1]->point,&handle->compress.mapPoint[i].point)>=0)
		{
			handle->convexhull.cnt--;
			handle->convexhull.pointPtrArr[handle->convexhull.cnt]->used=0;
		}
		handle->convexhull.pointPtrArr[handle->convexhull.cnt]=&handle->compress.mapPoint[i];
		handle->compress.mapPoint[i].used=1;
		handle->convexhull.cnt++;
	}
	
	//Vps_printf("left chain finish cnt:%d\n",handle->convexhull.cnt);

	//check last point
	while(handle->convexhull.cnt>=3&&IMG_ApproxPoly_PointCross(&handle->convexhull.pointPtrArr[handle->convexhull.cnt-2]->point,&handle->convexhull.pointPtrArr[handle->convexhull.cnt-1]->point,&handle->convexhull.pointPtrArr[0]->point)>=0)
	{
		handle->convexhull.cnt--;
		handle->convexhull.pointPtrArr[handle->convexhull.cnt]->used=0;
	}

	//Vps_printf("last point finish, cnt :%d\n",handle->convexhull.cnt);
	return IMG_APPROXPOLY_OK;
}

//vec(AB) X vec(BC)
int IMG_ApproxPoly_PointCross(IMG_Point*A,IMG_Point*B,IMG_Point*C)
{
	IMG_Point vec1;
	IMG_Point vec2;
	vec1.x = B->x-A->x;
	vec1.y = B->y-A->y;
	
	vec2.x = C->x-B->x;
	vec2.y = C->y-B->y;
	
	int cross = vec1.x*vec2.y-vec2.x*vec1.y;

	return cross;
}
