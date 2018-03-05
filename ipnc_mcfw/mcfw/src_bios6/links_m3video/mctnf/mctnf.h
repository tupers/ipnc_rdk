/*
********************************************************************************
 * HDVICP2.0 Based Motion Compensated Temporal Noise Filter(MCTNF)
 *
 * "HDVICP2.0 Based MCTNF" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of filtering noise from a
 *  4:2:0 semi planar Raw data.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/

#ifndef MCTNF_
#define MCTNF_

#include <ti/xdais/ialg.h>
#include <ividnf1.h>
#include <ti/sdo/fc/ires/hdvicp/ires_hdvicp2.h>
#include <imctnf.h>



/*
 *  ======== MCTNF_Handle ========
 *  This pointer is used to reference all MCTNF instance objects
 */
typedef struct IMCTNF_Obj *MCTNF_Handle;
extern IRES_Fxns MCTNF_TI_IRES;
/*
 *  ======== MCTNF_Params ========
 *  This structure defines the creation parameters for all MCTNF objects
 */
typedef IMCTNF_Params MCTNF_Params;

/*
 *  ======== MCTNF_PARAMS ========
 *  This structure defines the default creation parameters for MCTNF objects
 */
#define MCTNF_PARAMS   IMCTNF_PARAMS

/*
// ===========================================================================
// MCTNF_DynamicParams
//
// This structure defines the Dynamic parameters for all MCTNF objects
*/
typedef IMCTNF_DynamicParams MCTNF_DynamicParams;

/*
 *  ======== MCTNF_DYNAMICPARAMS ========
 *  This structure defines the default creation parameters for MCTNF objects
 */
#define MCTNF_DYNAMICPARAMS   IMCTNF_DYNAMICPARAMS


/*
// ===========================================================================
// MCTNF_Status
//
// This structure defines the real-time parameters for MCTNF objects
*/
typedef IMCTNF_Status MCTNF_Status;

/*
 *  ======== IMCTNF_InArgs ========
 *  This structure defines the runtime input arguments for IMCTNF::process
 */

typedef IMCTNF_InArgs MCTNF_InArgs;

/*
 *  ======== IMCTNF_OutArgs ========
 *  This structure defines the run time output arguments for IMCTNF::process
 *  function.
 */

typedef IMCTNF_OutArgs MCTNF_OutArgs;

typedef IMCTNF_Cmd MCTNF_Cmd;
typedef IMCTNF_Fxns MCTNF_Fxns;

/*
// ===========================================================================
// control method commands
*/
#define MCTNF_GETSTATUS    IMCTNF_GETSTATUS
#define MCTNF_SETPARAMS    IMCTNF_SETPARAMS
#define MCTNF_PREPROCESS   IMCTNF_PREPROCESS
#define MCTNF_RESET        IMCTNF_RESET
#define MCTNF_FLUSH        IMCTNF_FLUSH
#define MCTNF_SETDEFAULT   IMCTNF_SETDEFAULT
#define MCTNF_GETBUFINFO   IMCTNF_GETBUFINFO

/*
 *  ======== MCTNF_create ========
 *  Create an MCTNF instance object (using parameters specified by prms)
 */
extern MCTNF_Handle MCTNF_create
(
	const MCTNF_Fxns   *fxns,
    const MCTNF_Params *prms,
    Int scratchGroupID
);

/*
 *  ======== MCTNF_delete ========
 *  Delete the MCTNF instance object specified by handle
 */
extern Void MCTNF_delete(Int scratchGroupID, MCTNF_Handle handle);

/*
 *  ======== MCTNF_init ========
 */
extern Void MCTNF_init(void);

/*
 *  ======== MCTNF_exit ========
 */
extern Void MCTNF_exit(void);

/*
 *  ======== MCTNF_enodeFrame ========
 */
extern XDAS_Int32 MCTNF_processFrame
(
	MCTNF_Handle     handle,
    IVIDEO2_BufDesc *InBufs,
    IVIDEO2_BufDesc *OutBufs,
    MCTNF_InArgs    *Inargs,
    MCTNF_OutArgs   *Outargs
);

/*
 *  ======== MCTNF_control ========
 * Get, set, and change the parameters of the MCTNF function (using parameters specified by status).
 */
extern XDAS_Int32 MCTNF_control
(
	MCTNF_Handle         handle,
	MCTNF_Cmd            cmd,
    MCTNF_DynamicParams *params,
    MCTNF_Status        *status
);

#endif   /* MCTNF_ */
