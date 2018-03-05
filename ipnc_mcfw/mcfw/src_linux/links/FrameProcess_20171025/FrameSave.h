#ifndef FRAMESAVE_H
#define FRAMESAVE_H

#include <mcfw/interfaces/link_api/imgAlg_data.h>

#define ERR_FRAME_MAX_WIDTH		1920
#define ERR_FRAME_MAX_HEIGTH	1080

#define MAX_IMG_DIR			10
#define MAX_IMG_FILE		10

#define IMGSAVE_ROOT	"/tmp/nand"
#define IMGSAVE_DIR		"Image"
#define IMG_FILENAME	"img"
#define ERRIMG_ROOT	"/tmp/nand"
#define ERRIMG_DIR	"errImage"
#define ERRIMG_FILE	"image"


typedef enum 
{
	IMG_SAVE,
	IMG_ERR,
	IMG_END
}IMG_TYPE;

typedef struct _ErrFream_Buf
{
	IMG_TYPE     type;
	unsigned int pitch;
	unsigned int width;
	unsigned int height;
	AlgSaveImg	 info;
	char		 fileName[20];
	unsigned char *pData;
}ErrFrame_Buf;




#endif
