/** ==================================================================
 *  @file   audio_capture.c
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
#include <audio.h>
#include <audio_priv.h>
#include <alarm_msg_drv.h>

#define	MAX_STR_SIZE		(256)
#define AUDIO_CMEM_ALLOC(size, align) 	calloc(1,((size)+(align - 1))/(align)*(align))
#define AUDIO_CMEM_FREE(buf) 			free(buf)

static Int32 recordAudioFlag = 0, recordChNum = 0;

static TaskCtx captureThrHandle;
static snd_pcm_t *capture_handle = NULL;
static AudioStats captureStats;

static unsigned char *gAudInBuf, *gAudOutBuf;
static int gAudSampleRate = 8000;

unsigned int AUDIO_GetTimeStamp(void)
{
    struct timeval timeval;

    gettimeofday(&timeval, NULL);

    return (timeval.tv_sec * 1000) + (timeval.tv_usec + 500) / 1000;;
}

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
Int32 Audio_captureCreate(UI_mcfw_audiocfg *audioCfg)
{
    Int32 status = AUDIO_STATUS_OK;

    gAudSampleRate = audioCfg->sampleRate;
    gAudInBuf = (unsigned char *) AUDIO_CMEM_ALLOC(INPUTBUFSIZE, 32);
    if (gAudInBuf == NULL)
    {
        OSA_ERROR("Failed to allocate audio input buffer\n");
        return AUDIO_STATUS_EFAIL;
    }

    /* Allocate output buffer */
    gAudOutBuf = (unsigned char *) AUDIO_CMEM_ALLOC(OUPUTBUFSIZE, 32);
    if (gAudOutBuf == NULL)
    {
        AUDIO_CMEM_FREE(gAudInBuf);
        OSA_ERROR("Failed to allocate audio output buffer\n");
        return AUDIO_STATUS_EFAIL;

    }

    status = Audio_thrCreate(&captureThrHandle,
                             (ThrEntryFunc) Audio_recordMain,
                             AUDIO_CAPTURE_TSK_PRI,
                             AUDIO_CAPTURE_TSK_STACK_SIZE);

    OSA_printf("\n Audio capture task created");
    Audio_captureStart(0);
    UTILS_assert(status == AUDIO_STATUS_OK);
    return status;
}

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
Int32 Audio_captureStart(Int8 chNum)
{
    InitAudioCaptureDevice(NUM_CHANNELS, gAudSampleRate, NUMSAMPLES / 2);
    sleep(1);
    recordAudioFlag = 1;
    return 0;
}

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
Int32 Audio_captureStop(Void)
{
    recordAudioFlag = 0;
    sleep(1);
    deInitAudioCaptureDevice();
    return 0;
}

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
Int32 Audio_captureDelete(Void)
{
    if (recordAudioFlag)
        Audio_captureStop();

    OSA_printf("\n Deleting Audio capture task");

    Audio_thrDelete(&captureThrHandle);

    AUDIO_CMEM_FREE(gAudInBuf);
    AUDIO_CMEM_FREE(gAudOutBuf);

    OSA_printf(" Audio capture task deleted\n");
    return 0;
}

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
Int32 Audio_capturePrintStats(Void)
{
    OSA_printf("\n============================\n");
    if (recordAudioFlag == 1)
        OSA_printf("Capture ACTIVE,  ");
    else
        OSA_printf("Capture NOT ACTIVE,  ");
    OSA_printf("Channel %d, ErrCnt [%d], lastErr [%d, %s]\n", recordChNum,
           captureStats.errorCnt, captureStats.lastError,
           snd_strerror(captureStats.lastError));
    return 0;
}

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
Void Audio_recordMain(Void * arg)
{
    int rc = 0;

    unsigned int numBytes = RAWBUFSIZE;

    int encodedBufferSize = 0;

    static unsigned int AudioLastTime = 0;

    int AudioInterval = ((NUMSAMPLES * 1000) / gAudSampleRate);

    int AudioNow = 0;

    int AudioTmp = 0;

    TaskCtx *ctx = arg;

    unsigned int timestamp;

    int encFrameSize = 0;

    void *virtAddr = NULL;

    Int32 numSamples = NUMSAMPLES;

	int audAlarm = FALSE;

    while (ctx->exitFlag == 0)
    {
        if (recordAudioFlag)
        {
            /* Read stereo data from codec */
            rc = RecordAudio(gAudInBuf, &numSamples);
            if (rc == 0)
            {
                /* EPIPE means overrun */
                continue;
            }
            else if (rc < 0)
            {
                continue;
            }
            else if (rc != (int) NUMSAMPLES)
            {
                OSA_ERROR("Audio short read, read %d frames\n", rc);
                numBytes = rc * 2;                         // as it is 16bit
                                                           // data
            }
            else
            {
                numBytes = RAWBUFSIZE;
            }

            if (AudioLastTime == 0)
            {
                AudioNow = AudioLastTime = AUDIO_GetTimeStamp();
            }
            else
            {
                AudioNow = AUDIO_GetTimeStamp();
                // OSA_printf("AudioNow - AudioLastTime = %d\n",(AudioNow -
                // AudioLastTime));
                if ((AudioNow - AudioLastTime) < AudioInterval)
                {
                    if ((AudioNow - AudioLastTime) < 10)
                    {
                        continue;
                    }
                    AudioTmp = AudioNow;
                    AudioNow = AudioLastTime + AudioInterval;
                    AudioLastTime = AudioTmp;
                }
                else
                {
                    AudioLastTime = AudioNow;
                }
            }

            timestamp = AudioNow;

			encodedBufferSize =
				AUDIO_audioEncode(ALG_AUD_CODEC_G711, (short *) gAudOutBuf,
								  (short *) gAudInBuf, numBytes);
			if (encodedBufferSize <= 0)
			{
				OSA_ERROR("Audio encode error !!\n");
				continue;
			}

			encFrameSize = encodedBufferSize;
			virtAddr = gAudOutBuf;

            stream_write(virtAddr,
                         encFrameSize,
                         AUDIO_FRAME,
                         STREAM_AUDIO, timestamp, 0, stream_get_handle());

			if(gUI_mcfw_config.audioCfg.alarmEnable==TRUE)
			{
				audAlarm = AUDIO_alarmCheck((char *) gAudInBuf, numBytes, gUI_mcfw_config.audioCfg.alarmLevel);
 				if(audAlarm == TRUE) {
					//OSA_printf("Audio Alarm ON\n");
					SendAlarmAudioTrigger();
				}
			}
        }
        else
        {
            sleep(2);
        }
    }

    // OSA_printf ("AUDIO >> Exiting audio capture task............\n");
}

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
Int32 InitAudioCaptureDevice(Int32 channels, UInt32 sample_rate,
                                    Int32 driver_buf_size)
{
    int rc = 0;

    unsigned int val = 0;

    int dir = 0;

    unsigned int buffer_time = 0;

    snd_pcm_uframes_t frames;

    snd_pcm_uframes_t buffer_frames;

    snd_pcm_hw_params_t *sound_params = NULL;

    /* Open PCM device for record. */
    rc = snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0)
    {
        OSA_ERROR("unable to open pcm device: %s\n", snd_strerror(rc));
        return -1;
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&sound_params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(capture_handle, sound_params);

    /* Set the desired hardware parameters. */

    /* Sampling rate */
    val = sample_rate;
    snd_pcm_hw_params_set_rate_near(capture_handle, sound_params, &val, &dir);
    if (val != sample_rate)
    {
        OSA_ERROR("Rate doesn't match (requested %iHz, get %iHz)\n",
                  sample_rate, val);
        snd_pcm_close(capture_handle);
        return -1;
    }

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(capture_handle, sound_params,
                                 SND_PCM_FORMAT_S16_LE);
    /* Interleaved mode */
    snd_pcm_hw_params_set_access(capture_handle, sound_params,
                                 SND_PCM_ACCESS_RW_INTERLEAVED);
    /* Num channels */
    snd_pcm_hw_params_set_channels(capture_handle, sound_params, channels);

    /* Set period size of frames. */
    frames = driver_buf_size;
    snd_pcm_hw_params_set_period_size_near(capture_handle, sound_params,
                                           &frames, &dir);

    /* set the buffer time */
    if (buffer_time)
    {
        rc = snd_pcm_hw_params_set_buffer_time_near(capture_handle,
                                                    sound_params, &buffer_time,
                                                    &dir);
        if (rc < 0)
        {
            OSA_ERROR("Unable to set buffer time %i for playback: %s\n",
                      buffer_time, snd_strerror(rc));
            snd_pcm_close(capture_handle);
            return -1;
        }
    }
    else
    {
        buffer_frames = frames * 4;
        snd_pcm_hw_params_set_buffer_size_near(capture_handle, sound_params,
                                               &buffer_frames);
    }

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(capture_handle, sound_params);
    if (rc < 0)
    {
        AUD_DEVICE_PRINT_ERROR_AND_RETURN("unable to set hw parameters: %s\n",
                                          rc, capture_handle);
    }

	rc = snd_pcm_prepare(capture_handle);
    if (rc < 0)
    {
        AUD_DEVICE_PRINT_ERROR_AND_RETURN("Prepare error: %s\n",
                                          rc, capture_handle);
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(sound_params, &frames, &dir);

    snd_pcm_hw_params_get_period_time(sound_params, &val, &dir);

    OSA_printf("AUDIO : period size = %d frames, dir = %d\n", (int) frames, dir);
    OSA_printf("AUDIO : period time = %d us, dir = %d\n", val, dir);

    return 0;
}

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
Int32 RecordAudio(Uint8 * buffer, Int32 * numSamples)
{
    int rc;

    /* Read stereo data from codec */
    rc = snd_pcm_readi(capture_handle, buffer, *numSamples);
    if (rc == -EPIPE)
    {
        /* EPIPE means overrun */
        OSA_ERROR("audio overrun occurred\n");
        snd_pcm_prepare(capture_handle);
        return 0;
    }
    else
    {
        if (rc < 0)
        {
            OSA_ERROR("audio error from read : %s\n", snd_strerror(rc));
        }

        return rc;
    }
}

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
Int32 deInitAudioCaptureDevice(Void)
{
    if (capture_handle)
    {
        snd_pcm_drain(capture_handle);
		snd_pcm_hw_free(capture_handle);
        snd_pcm_close(capture_handle);
        capture_handle = NULL;
        OSA_printf("AUDIO >> Device closed\n");
    }

    return 0;
}
