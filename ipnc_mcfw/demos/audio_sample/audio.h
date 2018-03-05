/** ==================================================================
 *  @file   audio.h
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

#ifndef	_AUDIO_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define	_AUDIO_H_

#include "ti_mcfw_ipnc_main.h"
#include <osa.h>

#define	AUDIO_MAX_CHANNELS	1                      			// /< Max number
                                                           	// of audio
                                                           	// channel
                                                           	// supported for
                                                           	// recording /
                                                           	// playback

#define AUDIO_STATUS_OK      0                             	// /< Status : OK
#define AUDIO_STATUS_EFAIL   -1                            	// /< Status :
                                                           	// Generic error

/**
    \brief Create Audio capture module

    - Creates Audio capture task
    - Creates storage directories if not existing

    \return AUDIO_STATUS_OK on success
*/
/* ===================================================================
 *  @func     Audio_captureCreate
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
Int32 Audio_captureCreate(UI_mcfw_audiocfg *audioCfg);

/**
    \brief Start Audio capture

    - Start audio capture / file write

    \return AUDIO_STATUS_OK on success
*/
/* ===================================================================
 *  @func     Audio_captureStart
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
Int32 Audio_captureStart(Int8 chNum);

/**
    \brief Stop Audio capture

    - Stop audio capture / file write

    \return AUDIO_STATUS_OK on success
*/
/* ===================================================================
 *  @func     Audio_captureStop
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
Int32 Audio_captureStop(Void);

/**
    \brief Delete Audio capture

    - Deinit audio capture, delete task

    \return AUDIO_STATUS_OK on success
*/
/* ===================================================================
 *  @func     Audio_captureDelete
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
Int32 Audio_captureDelete(Void);

/**
    \brief Prints capture statistics
    \return AUDIO_STATUS_OK on success
*/
/* ===================================================================
 *  @func     Audio_capturePrintStats
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
Int32 Audio_capturePrintStats(Void);

/* ===================================================================
 *  @func     RecordAudio
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
Int32 RecordAudio(Uint8 * buffer, Int32 * numSamples);

/* ===================================================================
 *  @func     InitAudioCaptureDevice
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
Int32 InitAudioCaptureDevice(Int32 channels, Uint32 sample_rate,
                                    Int32 driver_buf_size);
/* ===================================================================
 *  @func     deInitAudioCaptureDevice
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
Int32 deInitAudioCaptureDevice(Void);

/* ===================================================================
 *  @func     Audio_recordMain
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
Void Audio_recordMain(Void * arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _AUDIO_H_ */
