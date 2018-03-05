/** ==================================================================
 *  @file   audio_utils.c
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

#include <osa.h>
#include <audio.h>
#include <audio_priv.h>

/* ===================================================================
 *  @func     Audio_thrCreate
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
Int32 Audio_thrCreate(TaskCtx * ctx, ThrEntryFunc entryFunc, Uint32 pri,
                      Uint32 stackSize)
{
    Int32 status = AUDIO_STATUS_OK;

    pthread_attr_t thread_attr;

    struct sched_param schedprm;

    if (ctx == NULL || entryFunc == NULL)
    {
        AUDIO_ERROR("Input param error\n");
        return AUDIO_STATUS_EFAIL;
    }

    // initialize thread attributes structure
    status = pthread_attr_init(&thread_attr);

    if (status != AUDIO_STATUS_OK)
    {
        AUDIO_ERROR
            ("Audio_thrCreate() - Could not initialize thread attributes\n");
        return status;
    }

    if (stackSize != 0)
        pthread_attr_setstacksize(&thread_attr, stackSize);

    status |=
        pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
    status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);

    if (pri > sched_get_priority_max(SCHED_FIFO))
        pri = sched_get_priority_max(SCHED_FIFO);
    else if (pri < sched_get_priority_min(SCHED_FIFO))
        pri = sched_get_priority_min(SCHED_FIFO);

    schedprm.sched_priority = pri;
    status |= pthread_attr_setschedparam(&thread_attr, &schedprm);

    if (status != AUDIO_STATUS_OK)
    {
        AUDIO_ERROR
            ("Audio_thrCreate() - Could not initialize thread attributes\n");
        goto error_exit;
    }

    ctx->exitFlag = 0;
    status = pthread_create(&ctx->handle, &thread_attr, entryFunc, ctx);

    if (status != AUDIO_STATUS_OK)
    {
        AUDIO_ERROR("Audio_thrCreate() - Could not create thread [%d]\n",
                    status);
    }

  error_exit:
    pthread_attr_destroy(&thread_attr);

    return status;
}

/* ===================================================================
 *  @func     Audio_thrDelete
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
Int32 Audio_thrDelete(TaskCtx * ctx)
{
    Int32 status = AUDIO_STATUS_OK;

    void *retVal;

    if (NULL == ctx)
    {
        AUDIO_ERROR("Audio_thrDelete() - handle is NULL\n");
        return AUDIO_STATUS_EFAIL;
    }
    status = pthread_cancel(ctx->handle);
    ctx->exitFlag = 1;
    if (pthread_join(ctx->handle, &retVal))
    {
        AUDIO_ERROR("Audio_thrDelete() - handle is NULL\n");
    }
    return status;
}
