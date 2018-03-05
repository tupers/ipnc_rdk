/** ===========================================================================
* @file gop_lock.h
*
* @path $(IPNCPATH)\multimedia\encode_stream\stream
*
* @desc 
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#ifndef __GOP_LOCK_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __GOP_LOCK_H__

#include <mem_mng.h>

typedef enum {
    GOP_FAIL = -104,
    GOP_INCOMPLETE,
    GOP_INVALID_PRM,
    GOP_ERROR_OP,
    GOP_NOMEM,
    GOP_OVERWRITTEN,
    GOP_NOT_READY,
    GOP_COMPLETE = 0
} GopRet_t;

enum {
    GOP_INDEX_MP4 = 0,
    GOP_INDEX_MP4_EXT,
    GOP_INDEX_AUDIO,
    GOP_INDEX_JPEG,
    GOP_INDEX_NUM
};

/* ===================================================================
 *  @func     GopInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void GopInit(void);

/* ===================================================================
 *  @func     LockGopBySerial                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
GopRet_t LockGopBySerial(int serial, VIDEO_BLK_INFO * pVidInfo, int index);

/* ===================================================================
 *  @func     UnlockGopBySerial                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
GopRet_t UnlockGopBySerial(int serial, VIDEO_BLK_INFO * pVidInfo, int index);

/* ===================================================================
 *  @func     GopCleanup                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void GopCleanup(VIDEO_BLK_INFO * pVidInfo, int index);

/* ===================================================================
 *  @func     LockCurrentGopP                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
GopRet_t LockCurrentGopP(VIDEO_BLK_INFO * pVidInfo, int index);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
