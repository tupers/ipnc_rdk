/** ==================================================================
 *  @file  mjpegLink_enc.h
 *
 *  @path   $(IPNC_PATH)/ipnc_mcfw/mcfw/src_bios6/links_m3vpss/mjpeg
 *
 *  @desc   This  File contains all the definitions of  data-
 *          structure and function protoptye of functions of mjpeg enc
 *          wrapper used by the mjpeg link.
 *  =====================================================================
 *  Copyright (c) Texas Instruments Inc 2012, 2011
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied    
 * ===================================================================*/
#ifndef _MJPEGLINK_ENC_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MJPEGLINK_ENC_H_

/* ===================================================================
 *  @func     McFW_JPEG_Enc                                               
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
int McFW_JPEG_Enc(MSP_COMPONENT_TYPE * jComponent,
                  FVID2_Frame * inputImageBuffer,
                  unsigned char *outputBitstreamBuffer,
                  unsigned int uInputImageWidth,
                  unsigned int uInputImageHeight,
                  unsigned int uInputPitch,
                  int startX, int startY,
                  unsigned char qualityFactor, unsigned int outBufSize, unsigned int *paddingBytes, unsigned int isChromaFormat420);

/* ===================================================================
 *  @func     MSP_JPEGE_INC_init                                               
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
Int32 MSP_JPEGE_INC_init(MSP_COMPONENT_TYPE ** jComponent);

/* ===================================================================
 *  @func     MSP_JPEGE_INC_deInit                                               
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
Int32 MSP_JPEGE_INC_deInit(MSP_COMPONENT_TYPE * jComponent);

#define GOTO_EXIT(N)                                                                                        \
    do {                                                                                                    \
        goto EXIT_##N;                                                                                      \
    } while(0)

#define GOTO_EXIT_IF(C,N)                                                                                   \
    do {                                                                                                    \
        if (C) {                                                                                            \
            goto EXIT_##N;                                                                                  \
        }                                                                                                   \
    } while(0)

#define GOTO_EXIT_NOPRINT_IF(C,N)   \
    do {                            \
        if (C) goto EXIT_##N;       \
    } while(0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_MJPEGLINK_ENC_H_ */
