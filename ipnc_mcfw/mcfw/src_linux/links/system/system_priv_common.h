/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SYSTEM_PRIV_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_PRIV_COMMON_H_

#include <osa_tsk.h>
#include <osa_mutex.h>

#include <ti/syslink/utils/IHeap.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Cache.h>

#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/vidframe.h>


#define SYSTEM_TSK_PRI                   (2)
#define IPC_LINK_TSK_PRI                 (2)
#define SYSTEM_MSGQ_TSK_PRI              (2)

#define GLBCE_SUPPORT_LINK_TSK_PRI        (8)
#define FRAMEPROSSHOST_PRI				  (2)
#define FRAMESAVE_TSK_PRI				  (3)

#define SYSTEM_DEFAULT_TSK_STACK_SIZE    (0) /* 0 means system default will be used */
#define SYSTEM_TSK_STACK_SIZE            (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define IPC_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SYSTEM_MSGQ_TSK_STACK_SIZE       (SYSTEM_DEFAULT_TSK_STACK_SIZE)

#define GLBCE_SUPPORT_LINK_TSK_STACK_SIZE               (1024)
#define FRAMEPROSSHOST_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define FRAMESAVE_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)

typedef Int32 (*System_GetLinkInfoCb)        (OSA_TskHndl *pTsk,
                                                System_LinkInfo *info);

typedef Int32(*System_LinkGetOutputFramesCb) (OSA_TskHndl * pTsk,
                                              UInt16 queId,
                                              VIDFrame_BufList * pFrameList);
typedef Int32(*System_LinkPutEmptyFramesCb) (OSA_TskHndl * pTsk,
                                             UInt16 queId,
                                             VIDFrame_BufList * pFrameList);
/**
 * \brief LINK Instance Info
 *  Each of the LINKs are expected to register with "system" with the following
 *  information.
 *  Using these links the system would form a chain with mutiple LINKs
 */
typedef struct System_LinkObj {
    OSA_TskHndl *pTsk;

    System_LinkGetOutputFramesCb linkGetFullFrames;
    /**< Function expected to be called by the LINK to get the input frames */
    System_LinkPutEmptyFramesCb linkPutEmptyFrames;
    /**< Function expected to be called by the LINK to return received frame,
         Once processed by the LINK */
    System_GetLinkInfoCb getLinkInfo;
    /**<  Function that returns the LINKs output channel configurations */
} System_LinkObj;

/**
 * \brief System Task Descriptor
 *
 */
typedef struct
{
    OSA_MbxHndl             mbx;
    System_LinkObj          linkObj[SYSTEM_LINK_ID_MAX];

} System_CommonObj;


extern System_CommonObj gSystem_objCommon;

Int32 System_sendLinkCmd(UInt32 linkId, UInt32 cmd);

Int32 System_registerLink(UInt32 linkId, System_LinkObj *pTskObj);

OSA_TskHndl *System_getLinkTskHndl(UInt32 linkId);


Int32 System_linkControl_local(UInt32 linkId, UInt32 cmd, Void *pPrm, UInt32 prmSize, Bool waitAck);
Int32 System_sendLinkCmd_local(UInt32 linkId, UInt32 cmd);
Int32 System_linkGetInfo_local(UInt32 linkId, System_LinkInfo * info);

Int32 System_linkControl_remote(UInt32 linkId, UInt32 cmd, Void *pPrm, UInt32 prmSize, Bool waitAck);

Int32 SystemLink_init();
Int32 SystemLink_deInit();

Int32 System_getLinksFullFrames(UInt32 linkId, UInt16 queId,
                                VIDFrame_BufList * pFrameList);
Int32 System_putLinksEmptyFrames(UInt32 linkId, UInt16 queId,
                                 VIDFrame_BufList * pFrameList);                                
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
