/** ==================================================================
 *  @file   alg_uLawCodec.h
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
#ifndef	_AUDIO_G711CODEC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define	_AUDIO_G711CODEC_H

#include <osa.h>

/* ===================================================================
 *  @func     ALG_ulawEncode
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
Int32 ALG_ulawEncode(short *dst, short *src, Int32 bufsize);

/* ===================================================================
 *  @func     ALG_ulawDecode
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
short ALG_ulawDecode(unsigned short input);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


