/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup MCFW_API
    \defgroup MCFW_VDEC_API McFW Video Decode (VDEC) API

    @{
*/

/**
    \file ti_vdec_common_def.h
    \brief McFW Video Decode (VDEC) API- Common definitions and data structures
*/

#ifndef __TI_VDEC_COMMON_DEF_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_VDEC_COMMON_DEF_H__

/**
 ******************************************************************************
 *  @enum       VDEC_CHN_DecodeFrameType
 *  @brief      This enum is used to request decoder to decode only I, IP or ALL
 *              frame types
 ******************************************************************************
*/
typedef enum
{
  VDEC_DECODE_ALL = 0,
  /**
  * Indicates that all type of frames decoding is enabled
  * Applicable only for H264 and MPEG4 decode types
  */
  VDEC_DECODE_IP_ONLY = 1,
  /**
  * Indicates that only I/IDR and P frames decoding is enabled
  * Applicable only for H264 decode type
  */
  VDEC_DECODE_I_ONLY = 2
  /**
  * Indicates that only I/IDR frames decoding is enabled
  * Applicable only for H264 and MPEG4 decode types
  */
} VDEC_CHN_DecodeFrameType;

/** 
 ******************************************************************************
 *  @enum       VDEC_CHN_processCallLevel
 *  @brief      This enum indicates whether process call is done at a field 
 *              level or frame level
 * 
 ******************************************************************************
*/
typedef enum 
{
  VDEC_FIELDLEVELPROCESSCALL = 0,
  /** 
  * Indicates that process call should be at field level
  */
  VDEC_FRAMELEVELPROCESSCALL = 1,
  /** 
  * Indicates that process call should be at frame level
  */
} VDEC_CHN_processCallLevel;

/**
 *  \brief Dec link decoder error message
 *
 *  Defines the data structure for Reporting the decode 
 *  error message to A8/Linux Application
 */
typedef struct VDEC_CH_ERROR_MSG
{
    Bool reportA8;
    /**< Flag to notify A8 about decoder error */

    Int32 chId;
    /**< Decoder channel number */

    Int32 frameNum;
    /**< Decoder FRAME number. Reserved for future use */

    Int32 errorMsg;
    /**< Decoder error message */

    UInt32 upperTimeStamp;
    /**< Time stamp:Upper 32 bit value. Reserved for future use*/

    UInt32 lowerTimeStamp;
    /**< Time stamp: Lower 32 bit value. Reserved for future use*/
} VDEC_CH_ERROR_MSG;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */

