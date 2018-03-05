/** ==================================================================
 *  @file   audio_EncDec.c
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
#include <audio_EncDec.h>

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
int AUDIO_audioEncode(int AudioCodecId, short *dst, short *src, Int32 bufsize)
{
    int ret = OSA_EFAIL;

    switch (AudioCodecId == ALG_AUD_CODEC_G711)
    {
        default:
        case ALG_AUD_CODEC_G711:
            ret = ALG_ulawEncode(dst, src, bufsize);
            break;
    }
    return ret;
}

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
int AUDIO_audioDecode(int AudioCodecId, char *pIndata, short *pOutdata,
                      int inBufSize)
{
    int sizeConsumed = 0, i = 0;

    sizeConsumed = inBufSize;
    switch (AudioCodecId == ALG_AUD_CODEC_G711)
    {
        default:
        case ALG_AUD_CODEC_G711:
            for (i = 0; i < sizeConsumed; i++)
            {
                *((unsigned short *) (pOutdata) + i) =
                    ALG_ulawDecode(*((unsigned char *) (pIndata) + i));
            }
            break;
    }
    return TRUE;
}

/* ===================================================================
 *  @func     AUDIO_alarmFlag                                               
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
void AUDIO_alarmFlag(Int32 audAlarmEnable)
{
	gUI_mcfw_config.audioCfg.alarmEnable = audAlarmEnable;

}

/* ===================================================================
 *  @func     AUDIO_alarmLevel                                               
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
void AUDIO_alarmLevel(Int32 audAlarmVal)
{
	gUI_mcfw_config.audioCfg.alarmLevel = audAlarmVal;
}

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
Int32 AUDIO_alarmCheck(char *inpBuf, Int32 numSamples, Int32 alarmLevel)
{
    unsigned short i;
	int audAlarm = FALSE;
	int sampLimit = (alarmLevel*32767)/100;
	short *src = (short *)inpBuf;
    short data, alarmcount=0;
    unsigned short isNegative;

	//sampLimit = (sampLimit*50)/100;

    for (i = 0; i < numSamples; i++)
    {
        data = src[i];
        isNegative = (data < 0) ? 1 : 0;
        if (isNegative)
            data = -data;

		if(data>=sampLimit)
			alarmcount++;
	}

	//OSA_printf("AUDIO ALARM DEBUG MESSAGE: samples(%d) level(%d) samplimit(%d) count(%d)\n",
	//				numSamples, alarmLevel, sampLimit, alarmcount);

	if(alarmcount>=AUDIOALARM_THRESHOLD)
		audAlarm = TRUE;

	return audAlarm;
}

