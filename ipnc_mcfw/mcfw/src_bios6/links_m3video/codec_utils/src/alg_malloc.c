/*
********************************************************************************  
* HDVICP2.0 Based JPEG Decoder
*
* "HDVICP2.0 Based JPEG Decoder" is software module developed on
* TI's HDVICP2 based SOCs. This module is capable of generating a raw image 
* by de-compressing/decoding a jpeg bit-stream based on ISO/IEC IS 10918-1. 
* Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
* ALL RIGHTS RESERVED 
********************************************************************************
*/

/**  
********************************************************************************
* @file      alg_malloc.c                         
*
* @brief     This module implements an algorithm memory management "policy" in
*            which no memory is shared among algorithm objects.  Memory is,
*            however reclaimed when objects are deleted.
*            This file is reused between platforms.Any change should be made
*            after considering the impact on multiple platforms
*
* @author    Chetan
*
* @version 0.0 (Dec 2008) : Created the initial version.[Odanaka]
*
* @version 0.1 (Feb 2010) : Cleaned code to adhere for the coding guidelines
*                           [Chetan]  
*
*******************************************************************************
*/

/*----------------------- data declarations ----------------------------------*/

#pragma CODE_SECTION(ALG_init, ".text:init")
#pragma CODE_SECTION(ALG_exit, ".text:exit")

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
#include <xdc/std.h>
#include <ti/xdais/ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */

/** 
********************************************************************************
*  @fn     ALG_activate(ALG_Handle alg)
*
*  @brief  This function does app specific initialization of scratch memory
*
*  @param[in,out]  alg     :  This handle type is used to reference all 
*                             ALG instance objects
*          
*  @return    none
********************************************************************************
*/
Void ALG_activate(IALG_Handle alg)
{
  /*--------------------------------------------------------------------------*/
  /* Restore all persistant shared memory                                     */
  /*--------------------------------------------------------------------------*/
  
  if (alg->fxns->algActivate != NULL) 
  {
    alg->fxns->algActivate(alg);
  }
  else
  {
    /* Nothing to execute*/
  }
}

/** 
********************************************************************************
*  @fn     ALG_deactivate(ALG_Handle alg)
*
*  @brief  This function does application specific store of persistent data
*
*  @param[in]  alg         :  This handle type is used to reference all 
*                             ALG instance objects
*          
*  @return    none
********************************************************************************
*/
Void ALG_deactivate(IALG_Handle alg)
{
  /*--------------------------------------------------------------------------*/
  /* do app specific store of persistent data                                 */
  /*--------------------------------------------------------------------------*/
  if (alg->fxns->algDeactivate != NULL) 
  {
    alg->fxns->algDeactivate(alg);
  }
  else
  {
    /* Nothing to execute*/
  }
  
}

/** 
********************************************************************************
*  @fn     ALG_exit(Void)
*
*  @brief  This function is for Module finalization
*          
*  @return    none
********************************************************************************
*/
Void ALG_exit(Void)
{
}

/** 
********************************************************************************
*  @fn     ALG_init(Void)
*
*  @brief  This function is for Module initialization
*          
*  @return    none
********************************************************************************
*/
Void ALG_init(Void)
{
}


