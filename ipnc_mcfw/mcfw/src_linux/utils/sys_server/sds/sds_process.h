#ifndef SDS_PROCESS_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SDS_PROCESS_H

#define RESULT_MAX_SIZE		2048
typedef struct
{
	unsigned int 	size;
	unsigned char	result[0];
}AlgInfo_St;

typedef struct _SdsProcess
{
	int qid;
	int mid;
	int fd;
}SdsProcess;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
