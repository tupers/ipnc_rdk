#ifndef _FTP_THR_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _FTP_THR_H_
#include <sched.h>
#include <pthread.h>

#define FTP_THR_PRI_MAX                 sched_get_priority_max(SCHED_FIFO)
#define FTP_THR_PRI_MIN                 sched_get_priority_min(SCHED_FIFO)

#define FTP_THR_PRI_DEFAULT             ( FTP_THR_PRI_MIN + (FTP_THR_PRI_MAX-FTP_THR_PRI_MIN)/2 )

#define FTP_THR_STACK_SIZE_DEFAULT      0

typedef void * (*Ftp_ThrEntryFunc)(void *);

typedef struct {

  pthread_t      hndl;
  
} Ftp_ThrHndl;

int Ftp_thrCreate(Ftp_ThrHndl *hndl, Ftp_ThrEntryFunc entryFunc, unsigned pri, unsigned stackSize, void *prm);
int Ftp_thrDelete(Ftp_ThrHndl *hndl);
int Ftp_thrJoin(Ftp_ThrHndl *hndl);
int Ftp_thrChangePri(Ftp_ThrHndl *hndl, unsigned pri);
int Ftp_thrExit(void *returnVal);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _FTP_THR_H_ */



