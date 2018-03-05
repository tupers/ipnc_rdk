#ifndef FILE_OPS_H
#define FILE_OPS_H

int OSA_fileExist(char *pathName);

int OSA_mkdir(char *dirName);

long OSA_readFile(char *pathName,long offset,unsigned size,char *buf);

long OSA_writeFile(char *pathName,long offset,unsigned size,char *buf);

int OSA_getFile_count(char *dirName);

int OSA_getDir_count(char *dirName);

char *OSA_getcurDir(char *pathname,unsigned size);

int OSA_chdir(char *pathname);

#endif
