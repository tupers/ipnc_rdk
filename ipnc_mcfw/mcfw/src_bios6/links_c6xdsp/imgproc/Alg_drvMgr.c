#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Alg_drvMgr.h"
#include "Alg_xdais.h"


#define ALGM_NUM_DRIVER_OBJECTS			10
#define  FALSE							0
#define  TRUE							1	

static Algm_Driver gAlgmDriverObjects[ALGM_NUM_DRIVER_OBJECTS];
extern void DetectRec_ModuleInit();
extern void DetectRec_ModuleExit();

static Alg_ModuleInitHandle_t  ModuleInitFunc[ALGM_NUM_DRIVER_OBJECTS] = 
{
	DetectRec_ModuleInit,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL	
};

static Alg_ModuleExitHandle_t ModuleExitFunc[ALGM_NUM_DRIVER_OBJECTS] = 
{
	DetectRec_ModuleExit,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

void *Alg_DrvGetDRamMem(unsigned int size,unsigned int allign)
{
	return malloc(size);
//	return Utils_memAlloc_cached(size,allign);
}

void Alg_DrvReleaseDRamMem(void *Ptr,unsigned int size)
{
	free(Ptr);
//	Utils_memFree_cached(Ptr, size);
}

void *Alg_DrvGetSRamMem(unsigned int size,unsigned int allign)
{
	return malloc(size);
//	return Utils_memAlloc_cached(size, allign);
}
void Alg_DrvReleaseSRamMem(void *Ptr,unsigned int size)
{
	free(Ptr);
//	Utils_memFree_cached(Ptr, size);
}

int Alg_DrvInit(void * args)
{
	unsigned int cnt;
    int   retVal = ALG_DRVSOK;

    /* Init all global variables to zero */
    memset(gAlgmDriverObjects,0u,sizeof(gAlgmDriverObjects));
	
    /* Mark pool flags as free */
    for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
    {
        gAlgmDriverObjects[cnt].isUsed = FALSE;
        gAlgmDriverObjects[cnt].algEdma_action = (pDmaActionFun)args;
    }

	for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
	{
		if(ModuleInitFunc[cnt])
			ModuleInitFunc[cnt]();
	}
    return (retVal);
}

/**
 *  Alg_deInit
 *  \brief Alg deinit function.
 *return ALG_DRVSFAIL;
 *  Uninitializes the drivers and the hardware.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
int Alg_DrvdeInit(void * args)
{
	unsigned int cnt;
    int  retVal = ALG_DRVSOK;

    /* Mark pool flags as free */
    for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
    {
        gAlgmDriverObjects[cnt].isUsed = FALSE;
        gAlgmDriverObjects[cnt].algEdma_action = NULL;
    }

	for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
	{
		if(ModuleExitFunc[cnt])
			ModuleExitFunc[cnt]();
	}
    
    return (retVal);
}

/**
 *  FVID2_registerDriver
 *  \brief FVID2 register driver function.
 */
int Alg_DrvregisterDriver(unsigned int drvId,void* handle)
{
    unsigned int         cnt;
    int    retVal = ALG_DRVSOK;
	ALG_Handle pHandle = (ALG_Handle)handle;
    /* Check for NULL pointers */
    if ((NULL == handle)||(drvId>=ALG_MODULE_MAX))
    	return ALG_DRVSFAIL;

    /* Check whether the driver is already registered */
    for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
    {
        if (TRUE == gAlgmDriverObjects[cnt].isUsed)
        {
            /* Check for NULL pointers */
            if (drvId == gAlgmDriverObjects[cnt].drvId)
            {
                retVal = ALG_DRVEDRIVER_INUSE;
                break;
            }			
        }
    }

    if (ALG_DRVSOK == retVal)
    {
   		for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
    	{
        	if (FALSE == gAlgmDriverObjects[cnt].isUsed)
        	{
            	gAlgmDriverObjects[cnt].isUsed = TRUE;
            	gAlgmDriverObjects[cnt].numOpens = 0u;
            	gAlgmDriverObjects[cnt].algHandle = handle;
            	gAlgmDriverObjects[cnt].drvId = drvId;
            	pHandle->fxns->algdma_actcbx = gAlgmDriverObjects[cnt].algEdma_action;
            	break;
			}

        }
    }

    return (retVal);
}



/**
 *  Alg_unRegisterDriver
 *  \brief FVID2 unregister driver function.
 */
int Alg_DrvunRegisterDriver(unsigned int drvId)
{
    int       retVal = ALG_DRVEALLOC;
	unsigned int cnt;

	if(drvId>=ALG_MODULE_MAX)
		return ALG_DRVSFAIL;
		
    /* Check whether the driver is already registered */
    for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
    {
        if (TRUE == gAlgmDriverObjects[cnt].isUsed)
        {
            /* Check for NULL pointers */
            if (drvId == gAlgmDriverObjects[cnt].drvId)
            {
                gAlgmDriverObjects[cnt].algHandle = NULL;
                gAlgmDriverObjects[cnt].numOpens = 0u;
            	gAlgmDriverObjects[cnt].isUsed = FALSE;
            	gAlgmDriverObjects[cnt].algEdma_action = NULL;
                break;
            }			
        }
    }


    return (retVal);
}

/**
 *  Alg_create
 *  \brief Opens the driver identified by the driver ID.
 */
Alg_DrvHandle Alg_DrvCreate(unsigned int drvId)
{
    unsigned int        cnt;
    int					NumMemRec = 0;
    int					status;
	int					index;
	int					algStatus = 0;
	ALG_Handle			handle = NULL;
	Alg_DrvHandle       drv = NULL;
	ALG_MemRec			*pMemRec;
	ALG_MemRec			*pMemTab;

	if(drvId>=ALG_MODULE_MAX)
		return NULL;
		
    /* Get the matching driver object */
    for (cnt = 0u; cnt < ALGM_NUM_DRIVER_OBJECTS; cnt++)
    {
        if (TRUE == gAlgmDriverObjects[cnt].isUsed)
        {
            /* Check for NULL pointers */
            if (drvId == gAlgmDriverObjects[cnt].drvId)
            {
            	drv = &gAlgmDriverObjects[cnt];
				handle = gAlgmDriverObjects[cnt].algHandle;
                break;
            }
        }
    }
	
	if(handle != NULL)
	{
		if(handle->fxns->algNumAlloc)
			NumMemRec = handle->fxns->algNumAlloc();
		pMemTab = Alg_DrvGetDRamMem(sizeof(ALG_MemRec)*NumMemRec,8);
		if(pMemTab == NULL)
			return NULL;
		if(handle->fxns->algAlloc)
		{
			status = handle->fxns->algAlloc(NumMemRec,pMemTab);
			if(ALG_EOK == status)
			{
				//alloc memory
				for(index = 0;index<NumMemRec;index++)
				{
					pMemRec = pMemTab+index;
					switch(pMemRec->space)
					{
						case ALG_DARAM0:
						{
							pMemRec->base = Alg_DrvGetDRamMem(pMemRec->size,pMemRec->alignment);
							if(pMemRec->base == NULL)
								algStatus = 1;
							break;
						}
						case ALG_SARAM:
						{
							pMemRec->base = Alg_DrvGetSRamMem(pMemRec->size,pMemRec->alignment);
							if(pMemRec->base == NULL)
								algStatus = 1;
							break;
						}
						default:
						{
							pMemRec->base = Alg_DrvGetDRamMem(pMemRec->size,pMemRec->alignment);
							if(pMemRec->base == NULL)
								algStatus = 1;
							break;
						}
					}
				}
			}
			
			if(handle->fxns->algInit)
			{
				status = handle->fxns->algInit(handle,NumMemRec,pMemTab);
				if(status != ALG_EOK)
					algStatus = -1;
			}			
		}			
	}

	Alg_DrvReleaseDRamMem(pMemTab,sizeof(ALG_MemRec)*NumMemRec);
	
	if(algStatus == -1)
		drv = NULL;	
    return drv;
}

int Alg_DrvDelete(Alg_DrvHandle drvHandle)
{
    int					NumMemRec = 0;
    int					status;
	int					index;
	ALG_Handle			handle = NULL;
	ALG_MemRec			*pMemRec;
	ALG_MemRec			*pMemTab;

	if((drvHandle==NULL)||(drvHandle->algHandle==NULL))
		return ALG_DRVSFAIL;
		
	handle = (ALG_Handle)(drvHandle->algHandle);
	
	if(handle->fxns->algNumAlloc)
		NumMemRec = handle->fxns->algNumAlloc();

	pMemTab = Alg_DrvGetDRamMem(sizeof(ALG_MemRec)*NumMemRec,8);
	if(pMemTab == NULL)
		return NULL;

	if(handle->fxns->algAlloc)
	{
		status = handle->fxns->algAlloc(NumMemRec,pMemTab);
		if(ALG_EOK == status)
		{
			//alloc memory
			for(index = 0;index<NumMemRec;index++)
			{
				pMemRec = pMemTab+index;
				switch(pMemRec->space)
				{
					case ALG_DARAM0:
					{
						Alg_DrvReleaseDRamMem(pMemRec->base,pMemRec->size);
						break;
					}
					case ALG_SARAM:
					{
						Alg_DrvReleaseSRamMem(pMemRec->base,pMemRec->size);
						break;
					}
					default:
					{
						Alg_DrvReleaseDRamMem(pMemRec->base,pMemRec->size);
						break;
					}
				}
			}
		}	
	}
	
	Alg_DrvReleaseDRamMem(pMemTab,sizeof(ALG_MemRec)*NumMemRec);

	drvHandle->algHandle = NULL;
	
    return ALG_DRVSOK;
}

int Alg_DrvControl(Alg_DrvHandle drvHandle,unsigned int Cmd,void * InArgs,void * OutArgs)
{
	int ret;
	if((drvHandle == NULL)||(drvHandle->algHandle == NULL))
		return ALG_DRVSFAIL;

	ALG_Handle handle = (ALG_Handle)(drvHandle->algHandle);
	if(handle->fxns->algControl == NULL)
		return 	ALG_DRVSFAIL;

	if(handle->fxns->algActivate)
		handle->fxns->algActivate(handle);
		
	ret = handle->fxns->algControl(handle,Cmd,InArgs,OutArgs);

	if(handle->fxns->algDeactivate)
		handle->fxns->algDeactivate(handle);

	return ret;
}

int Alg_DrvProcess(Alg_DrvHandle drvHandle,AlgProcessArgs *InArgs,void * Result)
{
	int ret;
	if((drvHandle == NULL)||(drvHandle->algHandle == NULL))
		return ALG_DRVSFAIL;

	ALG_Handle handle = (ALG_Handle)(drvHandle->algHandle);
	if(handle->fxns->algProcess == NULL)
		return 	ALG_DRVSFAIL;

	if(handle->fxns->algActivate)
		handle->fxns->algActivate(handle);
		
	ret = handle->fxns->algProcess(handle,InArgs,Result);

	if(handle->fxns->algDeactivate)
		handle->fxns->algDeactivate(handle);

	return ret;
}
