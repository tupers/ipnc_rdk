#ifndef _REMOTE_DEBUG_SHARE_MEM_H
#define _REMOTE_DEBUG_SHARE_MEM_H



int OSA_ShareMemInit(int key,unsigned int size);

void *OSA_ShaaeMemGetPtr(int mid);

void OSA_ShareMemWrite(int mid,int offset,void *buf, unsigned int length);

void OSA_ShareMemRead(int mid,int offset,void *buf, int length);


#endif
