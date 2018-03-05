/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ALG_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALG_LINK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algLink.h>
#include <mcfw/src_bios6/links_c6xdsp/alg_link/scd/scdLink_priv.h>
#include <mcfw/src_bios6/links_c6xdsp/alg_link/swosd/osdLink_priv.h>

#define ALG_LINK_OBJ_MAX                     (SYSTEM_LINK_ID_ALG_COUNT)

typedef struct AlgLink_OutObj {
    Utils_BitBufHndl bufOutQue;
    UInt32           numAllocPools;
    Bitstream_Buf    outBufs[ALG_LINK_SCD_MAX_OUT_FRAMES];
    UInt32           outNumBufs[UTILS_BITBUF_MAX_ALLOC_POOLS];
    UInt32           buf_size[UTILS_BITBUF_MAX_ALLOC_POOLS];
    UInt32           ch2poolMap[ALG_LINK_SCD_MAX_CH];
} AlgLink_OutObj;

typedef struct AlgLink_Obj {
    UInt32 linkId;
    
    char name[32];
    
    Utils_TskHndl tsk;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;

    AlgLink_CreateParams createArgs;

    System_LinkInfo info;

    AlgLink_OutObj outObj[ALG_LINK_MAX_OUT_QUE];
    
    AlgLink_OsdObj osdAlg;
    /**< handle to OSD algorithm */

    AlgLink_ScdObj scdAlg;
    /**< handle to SCD algorithm */

    UInt32 memUsed[UTILS_MEM_MAXHEAPS];
} AlgLink_Obj;


Int32 AlgLink_algCreate(AlgLink_Obj * pObj, AlgLink_CreateParams * pPrm);
Int32 AlgLink_algProcessData(AlgLink_Obj * pObj);
Int32 AlgLink_algDelete(AlgLink_Obj * pObj);
Int32 AlgLink_getFullBufs(Utils_TskHndl * pTsk, UInt16 queId,
                           Bitstream_BufList * pBufList);
Int32 AlgLink_putEmptyBufs(Utils_TskHndl * pTsk, UInt16 queId,
                           Bitstream_BufList * pBufList);
Int32 AlgLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

