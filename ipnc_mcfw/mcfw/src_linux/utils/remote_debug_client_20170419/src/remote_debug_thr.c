#include <stdio.h>
#include <remote_debug_thr.h>

int RemoteDebug_thrCreate(RemoteDebug_ThrHndl *hndl, RemoteDebug_ThrEntryFunc entryFunc, unsigned int pri, unsigned stackSize, void *prm)
{
  int status=0;
  pthread_attr_t thread_attr;
  struct sched_param schedprm;

  // initialize thread attributes structure
  status = pthread_attr_init(&thread_attr);
  
  if(status!=0) {
    printf("RemoteDebug_thrCreate() - Could not initialize thread attributes\n");
    return status;
  }
  
  if(stackSize!=REMOTEDEBUG_THR_STACK_SIZE_DEFAULT)
    pthread_attr_setstacksize(&thread_attr, stackSize);

  status |= pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
  status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
    
  if(pri>REMOTEDEBUG_THR_PRI_MAX)   
    pri=REMOTEDEBUG_THR_PRI_MAX;
  else 
  if(pri<REMOTEDEBUG_THR_PRI_MIN)   
    pri=REMOTEDEBUG_THR_PRI_MIN;
    
  schedprm.sched_priority = pri;
  status |= pthread_attr_setschedparam(&thread_attr, &schedprm);

  if(status!=0) {
    printf("RemoteDebug_thrCreate() - Could not initialize thread attributes\n");
    goto error_exit;
  }

  status = pthread_create(&hndl->hndl, &thread_attr, entryFunc, prm);

  if(status!=0) {
    printf("RemoteDebug_thrCreate() - Could not create thread [%d]\n", status);
  }

error_exit:  
  pthread_attr_destroy(&thread_attr);

  return status;
}

int RemoteDebug_thrJoin(RemoteDebug_ThrHndl *hndl)
{
  int status=0;
  void *returnVal;
    
  status = pthread_join(hndl->hndl, &returnVal); 
  
  return status;    
}

int RemoteDebug_thrDelete(RemoteDebug_ThrHndl *hndl)
{
  int status=0;
  
  status |= pthread_cancel(hndl->hndl); 
  status |= RemoteDebug_thrJoin(hndl);
  
  return status;    
}

int RemoteDebug_thrChangePri(RemoteDebug_ThrHndl *hndl, unsigned pri)
{
  int status = 0;
  struct sched_param schedprm;  

  if(pri>REMOTEDEBUG_THR_PRI_MAX)   
    pri=REMOTEDEBUG_THR_PRI_MAX;
  else 
  if(pri<REMOTEDEBUG_THR_PRI_MIN)   
    pri=REMOTEDEBUG_THR_PRI_MIN;
  
  schedprm.sched_priority = pri;  
  status |= pthread_setschedparam(hndl->hndl, SCHED_FIFO, &schedprm);
  
  return status;
}

int RemoteDebug_thrExit(void *returnVal)
{
  pthread_exit(returnVal);
  return 0;
}

