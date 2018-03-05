#ifndef FILEOPS_H
#define FILEOPS_H
#ifdef __cplusplus
extern "C"{
#endif

#define DIR_FILE_MAX_NUM		30
#define PATH_NOT_EXIST			-2
#define PATH_UNKOWN				-1
#define PATH_IS_DIR				0
#define PATH_IS_FILE			1

/** FTP FILE STRUCT */
#define DIR_TYPE			0
#define FILE_TYPE			1

#define FILE_NAME_MAX_SIZE		32
typedef struct __FILE_INFO
{
	unsigned int type;
	char name[FILE_NAME_MAX_SIZE];
	unsigned int size;
}FileInfo;

typedef struct __FILE_LIST
{
	unsigned int dirCount;
	FileInfo finfo[0];
}FileList;


#define LIST_SOK		0
#define LIST_SFAIL		-1


int FileType(char *pathname,int *ByteSize);

int Listdir(char *pathname,FileList *list);

int getFile(char *pathname,char *buf,int size);

int getFileSize(char *pathname);

int putFile(char *pathname,char *buf,int size);
#ifdef __cpusplus
}
#endif  /*__cplusplus*/
#endif /* FILEOPS_H */
