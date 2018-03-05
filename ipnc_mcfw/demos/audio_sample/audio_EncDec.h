/** ==================================================================
 *  @file   audio_EncDec.h
 *
 *  @path    ipnc_mcfw/demos/audio_sample/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef	_AUDIO_ENCDEC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define	_AUDIO_ENCDEC_H

#include <audio.h>
#include <alg_uLawCodec.h>
#include <osa.h>

#define AUDIOALARM_THRESHOLD	(1)

enum AudioCodecId { ALG_AUD_CODEC_G711 = 1 };

/* ===================================================================
 *  @func     AUDIO_audioEncode
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
Int32 AUDIO_audioEncode(Int32 AudioCodecId, short *dst, short *src,
                        Int32 bufsize);
/* ===================================================================
 *  @func     AUDIO_audioDecode
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
Int32 AUDIO_audioDecode(Int32 AudioCodecId, char *pIndata, short *pOutdata,
                        Int32 inBufSize);

/* ===================================================================
 *  @func     AUDIO_alarmCheck                                               
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
Int32 AUDIO_alarmCheck(char *inpBuf, Int32 numSamples, Int32 alarmLevel);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _AUDIO_ENCDEC_H
                                                            */
