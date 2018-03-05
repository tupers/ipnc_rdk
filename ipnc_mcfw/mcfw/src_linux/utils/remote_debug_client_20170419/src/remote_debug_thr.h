#ifndef _REMOTE_DEBUG_THREAD_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _REMOTE_DEBUG_THREAD

#include <sched.h>
#include <pthread.h>

#define REMOTEDEBUG_THR_PRI_MAX                 sched_get_priority_max(SCHED_FIFO)
#define REMOTEDEBUG_THR_PRI_MIN                 sched_get_priority_min(SCHED_FIFO)

#define REMOTEDEBUG_THR_PRI_DEFAULT             (REMOTEDEBUG_THR_PRI_MIN + (REMOTEDEBUG_THR_PRI_MAX-REMOTEDEBUG_THR_PRI_MIN)/2 )

#define REMOTEDEBUG_THR_STACK_SIZE_DEFAULT      0

typedef void * (*RemoteDebug_ThrEntryFunc)(void *);

typedef struct {

  pthread_t      hndl;
  
} RemoteDebug_ThrHndl;

int RemoteDebug_thrCreate(RemoteDebug_ThrHndl *hndl, RemoteDebug_ThrEntryFunc entryFunc, unsigned pri, unsigned stackSize, void *prm);
int RemoteDebug_thrDelete(RemoteDebug_ThrHndl *hndl);
int RemoteDebug_thrJoin(RemoteDebug_ThrHndl *hndl);
int RemoteDebug_thrChangePri(RemoteDebug_ThrHndl *hndl, unsigned pri);
int RemoteDebug_thrExit(void *returnVal);




#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _REMOTE_DEBUG_THREAD_ */
