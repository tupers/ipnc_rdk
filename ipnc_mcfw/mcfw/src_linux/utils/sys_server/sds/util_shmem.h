#ifndef EZ_UTILS_SHARE_MEM_H
#define EZ_UTILS_SHARE_MEM_H



int Utils_ShareMemInit(int key,unsigned int size);

void *Utils_ShaaeMemGetPtr(int mid);

void Utils_ShareMemWrite(int mid,int offset,void *buf, unsigned int length);

void Utils_ShareMemRead(int mid,int offset,void *buf, int length);

#endif
