#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "fileops.h"

static void strcpyfilename(char *dst,char *filename)
{	
	int start=0,index=0;
	char *ptr = filename;
	while(*ptr++!='\0')
	{
		if(*ptr == '/')
			start = index;
		
		index++;
	}
	
	ptr = filename + start;

	strcpy(dst,ptr);
}
int FileType(char *pathname,int *ByteSize)
{
	int ret;
	struct stat buf;
	*ByteSize = 0;
	
	ret = access(pathname, F_OK);
	if(ret!=0)
	{
		return PATH_NOT_EXIST;
	}

	ret = stat(pathname, &buf);
	if(ret!=0)
	{
		return PATH_NOT_EXIST;
	}

	*ByteSize = buf.st_size;

	if(S_ISDIR(buf.st_mode))
	{
		return PATH_IS_DIR;
	}
	else if(S_ISREG(buf.st_mode))
	{
		return PATH_IS_FILE;
	}
	else
		return PATH_UNKOWN;
}

int Listdir(char *pathname,FileList *list)
{
	int ret;
	char cur_dir[120];
	DIR *dir;
	struct dirent *ptr;
	int size;
	FileInfo *pFinfo;

	list->dirCount = 0;
	pFinfo = list->finfo;
	
	ret = FileType(pathname,&size);
	if(PATH_NOT_EXIST == ret)
	{
		return LIST_SFAIL;
	}
	else if(PATH_IS_DIR == ret)
	{
		dir = opendir(pathname);
		if(NULL == dir)
		{
			return LIST_SFAIL;
		}

		getcwd(cur_dir, 120);
		
		chdir(pathname);
		
		while((ptr = readdir(dir)) != NULL)
		{
			if(strcmp(ptr->d_name,".")&&strcmp(ptr->d_name,"..")&&strcmp(ptr->d_name,"imageCount"))
			{
				ret = FileType(ptr->d_name,&size);
				if(PATH_IS_DIR == ret)
				{
					pFinfo[list->dirCount].type = DIR_TYPE;
				}
				else if(PATH_IS_FILE == ret)
					pFinfo[list->dirCount].type = FILE_TYPE;
				else
					continue;

				/* fill file information */
				memset(pFinfo[list->dirCount].name,0,FILE_NAME_MAX_SIZE);
				strcpy(pFinfo[list->dirCount].name,ptr->d_name);
				pFinfo[list->dirCount].size = size;
				
				list->dirCount++;
				if(list->dirCount>=DIR_FILE_MAX_NUM)
					return LIST_SFAIL;
			}
		}

		chdir(cur_dir);
		
		closedir(dir);
		
		return 0;
	} /*End PATH_IS_DIR == ret */
	else if(PATH_IS_FILE == ret)
	{
		pFinfo[list->dirCount].type = FILE_TYPE;
		memset(pFinfo[list->dirCount].name,0,FILE_NAME_MAX_SIZE);
		strcpyfilename(pFinfo[list->dirCount].name,pathname);
		pFinfo[list->dirCount].size = size;
		list->dirCount++;
	}
	else
		return LIST_SFAIL;

	return LIST_SOK;
	
}


int getFile(char *pathname,char *buf,int size)
{
	int fileSize = 0;
	int ret;
	int fd;

	ret = FileType(pathname,&fileSize);
	if(ret != PATH_IS_FILE)
	{
		return 0;
	}	

	if((fileSize > size)||(fileSize == 0))
	{
		return 0;
	}

	fd = open(pathname, O_RDONLY);
	if(fd == -1)
	{
		return -1;
	}
	
	ret = read(fd, buf, fileSize);

	close(fd);

	if(ret < 0)
		return 0;
	
	return fileSize;
}


int getFileSize(char *pathname)
{
	int ret;
	struct stat buf;
	
	ret = access(pathname, F_OK);
	if(ret!=0)
	{
		perror("access");

		return 0;
	}

	ret = stat(pathname, &buf);
	if(ret!=0)
	{
		perror("stat");

		return 0;
	}

	if(S_ISREG(buf.st_mode))
	{
		return buf.st_size;
	}	
	
	return 0;
}

int putFile(char *pathname,char *buf,int size)
{
	int ret;
	int fd;
	int count = size;

	fd =  open(pathname, O_CREAT|O_WRONLY|O_TRUNC);
	if(-1 == fd)
	{
		return -1;
	}	
	
	do{
		ret = write(fd, buf, size);
		if(ret < size)
		{
			size = size - ret;
		}
		else
			break;
	}while(1);

	close(fd);
	
	system("sync");

	return count;
}
