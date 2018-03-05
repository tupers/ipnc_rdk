/* 
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) XDAS 2.51.00 11-29-2003 (xdas-2.50.00.9)
 */
/* 
 *  ======== alg_create.c ========
 *  This file contains a simple implementation of the ALG_create API operation.
 */

// #include <GenericComDataType.h>
// #include <H264VEncDataTypes.h>
#include <xdc/std.h>

#include <ti/sdo/fc/dskt2/dskt2.h>
#include <ti/xdais/ialg.h>
#include <stdlib.h>


// extern Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
// extern Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);

/* 
 *  ======== ALG_create ========
 */
IALG_Handle ALG_create(Int scratchMutexId,IALG_Fxns * fxns, IALG_Handle p, IALG_Params * params)
{
    return DSKT2_createAlgExt(scratchMutexId,fxns,p,params);
}

/* 
 *  ======== ALG_delete ========
 */
Void ALG_delete(Int scratchMutexId,IALG_Handle alg)
{
    DSKT2_freeAlg(scratchMutexId,alg);
}
