#include <stdio.h>
#include "ftp_thr.h"

int Ftp_thrCreate(Ftp_ThrHndl *hndl, Ftp_ThrEntryFunc entryFunc, unsigned pri, unsigned stackSize, void *prm)
{
  int status=0;
  pthread_attr_t thread_attr;
  struct sched_param schedprm;

  // initialize thread attributes structure
  status = pthread_attr_init(&thread_attr);
  
  if(status!=0) {
    printf("Ftp_thrCreate() - Could not initialize thread attributes\n");
    return status;
  }
  
  if(stackSize!=FTP_THR_STACK_SIZE_DEFAULT)
    pthread_attr_setstacksize(&thread_attr, stackSize);

  status |= pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
  status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
    
  if(pri>FTP_THR_PRI_MAX)   
    pri=FTP_THR_PRI_MAX;
  else 
  if(pri<FTP_THR_PRI_MIN)   
    pri=FTP_THR_PRI_MIN;
    
  schedprm.sched_priority = pri;
  status |= pthread_attr_setschedparam(&thread_attr, &schedprm);

  if(status!=0) {
    printf("Ftp_thrCreate() - Could not initialize thread attributes\n");
    goto error_exit;
  }

  status = pthread_create(&hndl->hndl, &thread_attr, entryFunc, prm);

  if(status!=0) {
    printf("Ftp_thrCreate() - Could not create thread [%d]\n", status);
  }

error_exit:  
  pthread_attr_destroy(&thread_attr);

  return status;
}

int Ftp_thrJoin(Ftp_ThrHndl *hndl)
{
  int status=0;
  void *returnVal;
    
  status = pthread_join(hndl->hndl, &returnVal); 
  
  return status;    
}

int Ftp_thrDelete(Ftp_ThrHndl *hndl)
{
  int status=0;
  
  status |= pthread_cancel(hndl->hndl); 
  status |= Ftp_thrJoin(hndl);
  
  return status;    
}

int Ftp_thrChangePri(Ftp_ThrHndl *hndl, unsigned pri)
{
  int status = 0;
  struct sched_param schedprm;  

  if(pri>FTP_THR_PRI_MAX)   
    pri=FTP_THR_PRI_MAX;
  else 
  if(pri<FTP_THR_PRI_MIN)   
    pri=FTP_THR_PRI_MIN;
  
  schedprm.sched_priority = pri;  
  status |= pthread_setschedparam(hndl->hndl, SCHED_FIFO, &schedprm);
  
  return status;
}

int Ftp_thrExit(void *returnVal)
{
  pthread_exit(returnVal);
  return 0;
}

