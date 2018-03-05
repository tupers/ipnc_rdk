/*
 * @(#)$Id: pcvBasicStruct.h 1874 2012-03-01 04:29:28Z LvGangXiao $ 
 * @(#) Declaration file of class pcvBasicStruct
 *
 * (c)  EZ CORPORATION  2015
 * All Rights Reserved.
 */
/** @file   
 * Declaration of pcvBasicStruct
 * 
 * TO DESC : FILE DETAIL DESCRIPTION, BUT DON'T DESCRIBE CLASS DETAIL HERE.
 */
#ifndef CXX_PCVBASICSTRUCT_H
#define CXX_PCVBASICSTRUCT_H


#include "pcvPlatformdef.h"


enum PCVIMAGE_TYPE_SUB{
	PCVIMAGE_TYPE_SUB_Y,
	PCVIMAGE_TYPE_SUB_U,
	PCVIMAGE_TYPE_SUB_V,
	//PCVIMAGE_TYPE_SUB_H,
	//PCVIMAGE_TYPE_SUB_S,
	PCVIMAGE_TYPE_SUB_MAX
};


enum PCVIMAGE_FORMAT{
	PCVIMAGE_FORMAT_NV21,
	PCVIMAGE_FORMAT_PLYUV,
	PCVIMAGE_FORMAT_YUYV,
	PCVIMAGE_FORMAT_HSV,
	PCVIMAGE_FORMAT_MAX,
};


typedef struct pcvPoint{
	INT x;
	INT	y;
}pcvPoint;


typedef struct pcvPoint2D32f{
	FLOAT x;
	FLOAT y;
}pcvPoint2D32f;


typedef struct pcvSize{
	INT width;
	INT height;
}pcvSize;


typedef struct pcvSize2D32f{
	FLOAT width;
	FLOAT height;

}pcvSize2D32f; 


typedef struct pcvRect{
	INT x;
	INT y;
	INT width;
	INT height;
}pcvRect; 


typedef struct pcvScalar{
	FLOAT val[4];
}pcvScalar;


typedef struct pcvMat{

	INT type;
	INT step;

	/* for internal use only*/
	INT* refcount;
	INT hdr_refcount;

	union
	{
		UCHAR*	ptr;
		SHORT*	s;
		INT*	i;
		FLOAT*	fl;
		DOUBLE* db;
	}data;

	INT	rows;
	INT cols;

}pcvMat; 


typedef struct _pcv3DVECTOR {
    FLOAT x;
    FLOAT y;
    FLOAT z;
}pcv3DVECTOR;

typedef struct _pcvVECTOR4 {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;
}pcvVECTOR4;


typedef struct _pcv3DMATRIX {
    union {

        struct
        {
            float   _11, _12, _13, _14;
            float   _21, _22, _23, _24;
            float   _31, _32, _33, _34;
            float   _41, _42, _43, _44;
        };

        float m[4][4];
    };

} pcv3DMATRIX;

typedef struct _PG3DVIEWPORT {
    DWORD       X;			  /**< Viewport Up left */
    DWORD       Y;            /**< Viewport Up left */
    DWORD       Width;		  /**< Viewport Dimensions */
    DWORD       Height;       /**< Viewport Dimensions */
    float       MinZ;         /**< Min of clip Volume */
    float       MaxZ;		  /**< Max of clip Volume */
} pcv3DVIEWPORT;

typedef struct _MV_MATRIX
{
	/* config */
	INT iStartAddrAlign;
	INT iLineAddrAlign;

	/* param */
	INT iRows;
	INT iCols;
	INT iPitch;
	INT iLength;
	BYTE *pData;
	MV_FIXED *pDataAlign;
} MV_MATRIX, *LPMV_MATRIX;

typedef struct _MV_MATRIX_S
{
	/* config */
	INT iStartAddrAlign;
	INT iLineAddrAlign;

	/* param */
	INT iRows;
	INT iCols;
	INT iPitch;
	INT iLength;
	BYTE *pData;
	MV_FIXED_S *pDataAlign;
} MV_MATRIX_S, *LPMV_MATRIX_S;

typedef struct _MV_MATRIX_B
{
	/* config */
	INT iStartAddrAlign;
	INT iLineAddrAlign;

	/* param */
	INT iRows;
	INT iCols;
	INT iPitch;
	INT iLength;
	BYTE *pData;
	MV_FIXED_B *pDataAlign;
} MV_MATRIX_B, *LPMV_MATRIX_B;

typedef struct _MV_MATRIX_F
{
	/* param */
	INT iRows;
	INT iCols;
	INT iPitch;
	INT iLength;
	BYTE *pData;
	FLOAT *pDataAlign;
} MV_MATRIX_F, *LPMV_MATRIX_F;


typedef struct _MV_MATRIX_L
{
	/* config */
	INT iStartAddrAlign;
	INT iLineAddrAlign;

	/* param */
	INT iRows;
	INT iCols;
	INT iPitch;
	INT iLength;
	BYTE *pData;
	MV_FIXED_L *pDataAlign;
} MV_MATRIX_L, *LPMV_MATRIX_L;


typedef struct pcvRawData{
	INT iWidth;
	INT iHeight;
	INT iPitch;
	INT iFormat;
	BYTE *pData;
}pcvRawData;


typedef struct pcvImage{
	pcvRawData cRawData;
	LPMV_MATRIX_S pSubData[PCVIMAGE_TYPE_SUB_MAX];
	BOOL bSubDataValid[PCVIMAGE_TYPE_SUB_MAX];

}pcvImage;


typedef struct _HOUGH_REGION {
    INT iX;
    INT iY;
    INT iWidth;
    INT iHeight;

} HOUGH_REGION;

typedef struct pcvDTParam{
	INT iNearVal;
	INT iFarVal;
	INT iWidth;
	INT iHeight;
} pcvDTParam;


typedef struct pcvColorf{
	FLOAT r;
	FLOAT g;
	FLOAT b;
	FLOAT a;
}pcvColorf;

typedef struct pcvListCell
{
	VOID*					data;	
	struct pcvListCell*		prev;		
	struct pcvListCell*		next;
	//static HANDLE	heap_mem;	//here to attention
}pcvListCell;


typedef struct pcvList{
	pcvListCell *pFirstCell;
	pcvListCell *pLastCell;
	LONG lItem;
}pcvList; 


BOOL	pcvListInit(pcvList* pList);
VOID	pcvListDeInit(pcvList* pList);

/*Add data to the last*/
BOOL	pcvListAdd(pcvList* pList, VOID *pData);
BOOL	pcvListInsert(pcvList* pList, VOID* pData);
BOOL	pcvListInsertPos(pcvList* pList, VOID* pData, DWORD count);

VOID	pcvListRemove(pcvList* pList, VOID* pData);

VOID*	pcvListPop(pcvList* pList);			
VOID*	pcvListNth(pcvList* pList, LONG lcnt);	
VOID*	pcvListGetLast(pcvList* pList);	
VOID*	pcvListRemoveLast(pcvList* pList);	

LONG	pcvListLength(pcvList* pList);
VOID	pcvListClear(pcvList* pList);	

typedef struct pcvQueue{
	pcvList list;
}pcvQueue; 

BOOL	pcvQueueInit(pcvQueue* pQueue);
VOID	pcvQueueDeInit(pcvQueue* pQueue);
BOOL	pcvQueuePush(pcvQueue* pQueue,  VOID *pData);	
VOID*	pcvQueuePop(pcvQueue* pQueue);	
LONG	pcvQueueLength(pcvQueue* pQueue);
VOID	pcvQueueClear(pcvQueue* pQueue);	

#endif // CXX_PCVBASICSTRUCT_H
/* EOF */
