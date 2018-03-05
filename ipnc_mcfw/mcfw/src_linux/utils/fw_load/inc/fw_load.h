
#ifndef _FW_LOAD_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _FW_LOAD_H_

#include <osa_debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ti/syslink/Std.h>
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>

Int FirmwareLoad_startup (UInt16 procId, String filePath);
Int FirmwareLoad_shutdown (UInt16 procId);
Void FirmwareLoad_printStatus (Void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


