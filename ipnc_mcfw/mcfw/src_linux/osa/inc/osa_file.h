

#ifndef _OSA_FILE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _OSA_FILE_H_

#include <osa.h>

int OSA_fileReadFile(char *fileName, Uint8 *addr, Uint32 readSize, Uint32 *actualReadSize);
int OSA_fileWriteFile(char *fileName, Uint8 *addr, Uint32 size);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _OSA_FILE_H_ */



