#include "osa_fileops.h"
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int OSA_fileExist(char *pathName)
{
	int ret;
	ret = access(pathName, F_OK);
	if(ret == 0)
		return 1;
	else
		return 0;	
}
int OSA_mkdir(char *dirName)
{
	int ret;
	
	if(dirName == NULL)
		return -1;
		
	ret = access(dirName, F_OK);
	if(ret==0)
		return 0;

	ret = mkdir(dirName, S_IRWXU|S_IRWXG|S_IRWXO);
	if(ret != 0)
		return -1;

	return 0;			
}

long OSA_readFile(char *pathName,long offset,unsigned size,char *buf)
{
	FILE *fp = fopen(pathName, "r");
	int ret;
	int len;
	
	if(fp == NULL)
		return -1;
		
	ret = fseek(fp, offset, SEEK_SET);
	if(ret < 0)
	{
		fclose(fp);
		return ret;
	}
	
	len = fread(buf, 1, size, fp);

	fclose(fp);

	return len;	
}

long OSA_writeFile(char *pathName,long offset,unsigned size,char *buf)
{
	FILE *fp; 
	int ret;
	int len;

	if(offset < 0)
		return -1;
		
	fp = fopen(pathName, "a");	
	if(fp == NULL)
		return -1;
		
	ret = fseek(fp, offset, SEEK_SET);
	if(ret < 0)
	{
		fclose(fp);
		return ret;
	}
	
	len = fwrite(buf, 1, size, fp);

	fclose(fp);

	return len;		
}

int OSA_getFile_count(char *dirName)
{
	DIR *dir;
	struct dirent *ptr;
	int num = 0;
	dir = opendir(dirName);
	if(dir == NULL)
	{
		printf("%s in %d:opendir fail\n",__func__,__LINE__);

		return num;
	}

	while((ptr=readdir(dir))!=NULL)
	{
		if(ptr->d_type == DT_REG)
		{
			num++;
		}			
	}

	closedir(dir);
	return num;
}

int OSA_getDir_count(char *dirName)
{
	DIR *dir;
	struct dirent *ptr;
	int num = 0;
	dir = opendir(dirName);
	if(dir == NULL)
	{
		printf("%s in %d:opendir fail\n",__func__,__LINE__);

		return num;
	}

	while((ptr=readdir(dir))!=NULL)
	{
		if(ptr->d_type == DT_DIR)
		{
			printf("dirname=%s,type:%d\n",ptr->d_name,ptr->d_type);
			num++;
		}			
	}

	closedir(dir);
	return num;
}

char *OSA_getcurDir(char *pathname,unsigned size)
{
	return getcwd(pathname, size);
}

int OSA_chdir(char *pathname)
{
	return chdir(pathname);
}
