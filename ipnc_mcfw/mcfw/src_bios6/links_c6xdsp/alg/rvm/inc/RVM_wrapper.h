/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2007-2013 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#define RVM_MAX_PLANES 2

#if (RVM_REFC)
    #define restrict 
#endif

typedef struct _rvm_image_t {
    uint8_t *restrict pData[RVM_MAX_PLANES];  /**< The byte pointer to the 2D data. Each plane has a separate pointer. Up to RVM_MAX_PLANES are supported */
    uint32_t          stride[RVM_MAX_PLANES]; /**< Bytes per line of the buffers */
} Rvm_Image_t;

typedef enum _rvm_colorformat_e {
    RVM_COLORFORMAT_YUV422,              /**< YUV422 */
    RVM_COLORFORMAT_NV12,                /**< NV12 */
    RVM_COLORFORMAT_LUMA8                /**< LUMA 8-bit */
} RVM_colorformat_e;

int RVM_wrapper(
    Rvm_Image_t       *input,            /** Input  image (YUV422, NV12, or luma 8-bit) */
    Rvm_Image_t       *output,           /** Output image (YUV422, NV12, or luma 8-bit) */
    RVM_colorformat_e  color,            /** Color format of input and output images */
    uint8_t           *lut,              /** Pointer to RVM lookup table */
    uint8_t           *scratchMem,       /** Scratch Memory Pointer */
    uint32_t           scratchSize,      /** Scratch Memory size (bytes) */
    uint8_t            useDma,            /** 0: use memcpy, 1: use dma */
	int32_t (*pCacheInv)(uint32_t addr,uint32_t size),
	int32_t (*pCacheWb)(uint32_t addr,uint32_t size),
	uint32_t *paramEntry,
	uint32_t *tccVal		
    );

int RVM_exit(void);	
	
int RVM_getScratchSize(
    uint8_t *lut,                        /** Pointer to RVM lookup table */
    RVM_colorformat_e  color             /** Color format of input and output images */
    );

