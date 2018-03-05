#ifndef ALG_DRV_MGR_H_
#define ALG_DRV_MGR_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define 	ALG_DRVSOK					0
#define		ALG_DRVSFAIL				-1
#define 	ALG_DRVEDRIVER_INUSE		-2
#define 	ALG_DRVEDEVICE_INUSE		-3
#define		ALG_DRVEALLOC				-4

typedef enum
{
	ALG_MODULE_SDS,
	ALG_MODULE_GVSS,
	ALG_MODULE_MAX
}ALG_MODULE_ENUM; 

typedef enum
{
	ALG_SET_PARAM,
	ALG_GET_PARAM,
	ALG_PARAM_ISVALID,
	ALG_SET_IMG_SOURCE,
}ALG_CMD;
typedef struct _AlgProcessArgs
{
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
	unsigned char *data;
}AlgProcessArgs;

typedef int (*pDmaActionFun)(char * dst,char *  src,unsigned int width,unsigned int height,unsigned src_pitch,unsigned dst_pitch,unsigned int cacheIValid);
/**
 *  struct Fdm_Driver
 *  \brief Structure to store driver information.
 */
typedef struct 
{
	unsigned int         drvId;
    unsigned int         numOpens;
    /**< Number of times the driver is opened using create API. */
    unsigned int         isUsed;
    /**< Flag indicating whether the object is used or not. */
	pDmaActionFun		algEdma_action;
	
    void *         		algHandle;
}Algm_Driver;

typedef Algm_Driver *	Alg_DrvHandle;

typedef void (*Alg_ModuleInitHandle_t)(void);
typedef void (*Alg_ModuleExitHandle_t)(void);

int Alg_DrvInit(void * args);

Alg_DrvHandle Alg_DrvCreate(unsigned int drvId);

int Alg_DrvControl(Alg_DrvHandle drvHandle,unsigned int Cmd,void * InArgs,void * OutArgs);

int Alg_DrvProcess(Alg_DrvHandle drvHandle,AlgProcessArgs *InArgs,void * Result);

int Alg_DrvregisterDriver(unsigned int drvId,void* handle);

int Alg_DrvunRegisterDriver(unsigned int drvId);

int Alg_DrvDelete(Alg_DrvHandle drvHandle);

int Alg_DrvdeInit(void * args);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
