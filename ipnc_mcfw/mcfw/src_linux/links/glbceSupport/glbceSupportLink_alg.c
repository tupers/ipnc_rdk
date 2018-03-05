/** ==================================================================
 *  @file   glbceSupportLink_drv.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3video/glbceSupport/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <osa_file.h>
#include "glbceSupportLink_priv.h"
#include <cmem.h>

#define GAMMATABLESIZE 4096
#ifdef USE_ISS_BOXCAR
static int isSeasoningDone = 0;
#endif

extern Uint8 Gamma12To8[GAMMATABLESIZE];

#define MMAP_MEM_PAGEALIGN (0x1000)
static int memDevFd;
static char * virtBoxCarAddr = NULL;
static char * virtTonalCurveAddr = NULL;

static Int16 rgb2rgbMat[9]={
  434, -191, 13,
 -112, 358,  10,
  -138, -451,845
};

static void *MM_mmap(unsigned long physAddr,unsigned int size)
{
    Uint8 *virtAddr;
	Uint32 memOffset, mmapMemAddr, mmapMemSize;

    memOffset   = physAddr & MMAP_MEM_PAGEALIGN;

    mmapMemAddr = physAddr - memOffset;

    mmapMemSize = size + memOffset;


	virtAddr = mmap(
		  (void	*)mmapMemAddr,
		  mmapMemSize,
	   PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,
	   memDevFd,
	   mmapMemAddr
	   );


	if (virtAddr==NULL)
	{
	  OSA_printf(" ERROR: mmap() failed !!!\n");
	  return NULL;
	}

	virtAddr = (Uint8 *)((UInt32)virtAddr + memOffset);

	return virtAddr;
}

static int MM_ummap(void *virtAddr,unsigned int size)
{
   if(virtAddr)
     munmap((void*)virtAddr, size);

   return 0;

}


// VS ALG create
/* ===================================================================
 *  @func     Glbce_Support_create
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
int  Glbce_Support_create(void)
{
    return GLBCE_SUPPORT_LINK_S_SUCCESS;
}

/* GlbceSupport alg create */

/* ===================================================================
 *  @func     GlbceSupportLink_algCreate
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
Int32 GlbceSupportLink_algCreate(GlbceSupportLink_Obj *pObj)
{

    memDevFd = open("/dev/mem",O_RDWR|O_SYNC);
    if(memDevFd < 0)
    {
      OSA_printf(" ERROR: /dev/mem open failed !!!\n");
      return -1;
    }


	//OSA_printf("Got the mm_fd as %d ", mm_fd);

	pObj->WIDTH = pObj->createArgs.totalFrameWidth;
	pObj->HEIGHT = pObj->createArgs.totalFrameHeight;

    pObj->glbceCIP = &(pObj->glbceHandle.creationInputParams);
	pObj->glbceIIP = &(pObj->glbceHandle.imageInputParams);
    pObj->glbce = (GLBCEstruct*)pObj->glbceHandle.glbceHandle;

	pObj->glbceCIP->externalScratchMemorySize = MAX_NUM_BLOCKS*1024*sizeof(Word16);
    pObj->glbceCIP->PersistentMemorySize = 1210*sizeof(Word16);

    pObj->glbceCIP->externalScratchMemory = (Byte*)OSA_memAlloc(pObj->glbceCIP->externalScratchMemorySize);
	OSA_assert(pObj->glbceCIP->externalScratchMemory != 0);
	pObj->glbceCIP->PersistentMemory = (Byte*)OSA_memAlloc(pObj->glbceCIP->PersistentMemorySize);
	OSA_assert(pObj->glbceCIP->PersistentMemory != 0);

       pObj->glbceIIP->RRCoef =  rgb2rgbMat[0];
	   pObj->glbceIIP->RGCoef =  rgb2rgbMat[1];
	   pObj->glbceIIP->RBCoef =  rgb2rgbMat[2];
	   pObj->glbceIIP->GRCoef =  rgb2rgbMat[3];
	   pObj->glbceIIP->GGCoef =  rgb2rgbMat[4];
	   pObj->glbceIIP->GBCoef =  rgb2rgbMat[5];
	   pObj->glbceIIP->BRCoef =  rgb2rgbMat[6];
	   pObj->glbceIIP->BGCoef =  rgb2rgbMat[7];
	   pObj->glbceIIP->BBCoef =  rgb2rgbMat[8];

    return GLBCE_SUPPORT_LINK_S_SUCCESS;
}

#ifdef USE_ISS_BOXCAR
static void v_rgb_2_rgb(Uint16 hsize, Uint16 vsize, Int32 *red, Int32 *green, Int32 *blue, Int16 *rgb2rgb)
{

  Int32 tmp_r, tmp_g, tmp_b, tmp_y, tmp_cb, tmp_cr;

  tmp_r = *red;
  tmp_g = *green;
  tmp_b = *blue;
  /* use JFIF spec */
  //if ((rrgain + rggain + rbgain) != 256) OSA_printf("Error, RGB Gains do not sum to 256");
  //if ((grgain + gggain + gbgain) != 256) OSA_printf("Error, RGB Gains do not sum to 256");
  //if ((brgain + bggain + bbgain) != 256) OSA_printf("Error, RGB Gains do not sum to 256");

  tmp_y  =  ((rgb2rgb[0] *tmp_r + rgb2rgb[1] *tmp_g + rgb2rgb[2] *tmp_b + 128)  >> 8);
  tmp_cb =  ((rgb2rgb[3] *tmp_r + rgb2rgb[4] *tmp_g + rgb2rgb[5] *tmp_b + 128)  >> 8);
  tmp_cr =  ((rgb2rgb[6] *tmp_r + rgb2rgb[7] *tmp_g + rgb2rgb[8] *tmp_b + 128)  >> 8);

  *red = tmp_y;
  *green = tmp_cb;
  *blue = tmp_cr;

}

static void seasonBoxCarData(short * virtBoxCarAddr, int imgHorzSize, int imgVertSize, int downsampleRatio, Int16 *vrgb2rgbMat)
{

 int width = imgHorzSize / downsampleRatio;
 int height = imgVertSize / downsampleRatio;
 int h, w;
 unsigned short * ptr = (unsigned short *) virtBoxCarAddr;
 int * outptr = ( int *) virtBoxCarAddr;
 Int32 RSum/*,GrSum,GbSum*/,BSum,GSum,  ZeroSum;
  Int32 LHist,R,G,B;

  Int32 finalShift;
  Uint16 wR,wG,wB;
  char numOutBits = 8;

  finalShift = 8 - numOutBits;



  wR = 77; //77/256
  wG = 150; //150/256
  wB = 29; //29/256

  //OSA_printf("Entered the function\n");
  //OSA_printf("height is %d \n", height);
  //OSA_printf("width is %d \n", width);


 for(h = 0; h < height; h++)
  for(w = 0; w < width; w++)
  {
#if 1
     BSum = (Int32)   *ptr++;
     GSum = (Int32)   *ptr++;
     RSum = (Int32)   *ptr++;
     ZeroSum = (Int32)   *ptr++;

	 RSum = RSum << 8;
	 GSum = GSum << 8;
	 BSum = BSum << 8;

     R= RSum; G= GSum; B= BSum;


     v_rgb_2_rgb(1, 1, &RSum, &GSum, &BSum, vrgb2rgbMat);
     //OSA_printf("Called v_rgb_2_rgb()\n");
     /* convert 16 bits to 12 bits */
     RSum = ((RSum + (1 << 3)) >> 4);
		GSum = ((GSum + (1 << 3)) >> 4);
		BSum = ((BSum + (1 << 3)) >> 4);

		RSum = MIN(MAX(RSum, 0), GAMMATABLESIZE-1);
		GSum = MIN(MAX(GSum, 0), GAMMATABLESIZE-1);
		BSum = MIN(MAX(BSum, 0), GAMMATABLESIZE-1);

		RSum = Gamma12To8[RSum];
		GSum = Gamma12To8[GSum];
		BSum = Gamma12To8[BSum];

     LHist = ( ( (wR*RSum + (1 << 7)) >> 8) + ( (wG*GSum + (1 << 7)) >> 8) + ( (wB*BSum + (1 << 7)) >> 8) );
     if (finalShift)
        LHist = (LHist + (1<<(finalShift-1))) >> finalShift;
     LHist= MIN(MAX(LHist, 0), (1<<numOutBits)-1);


	 *outptr++ = LHist;

#endif

  }

 return;
}
#endif



/* GlbceSupport link process */

/* ===================================================================
 *  @func     GlbceSupportLink_algProcess
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
Int32 GlbceSupportLink_algProcess(GlbceSupportLink_Obj * pObj,
                           GlbceSupportLink_processPrm *pProcessPrm)
{
#ifdef USE_ISS_BOXCAR
	Int16 * rgb2rgbMatPtr;
#endif
        		//OSA_printf("Received the buffer %x ", (unsigned int) pProcessPrm);
				//OSA_printf("Received the Message with struct Index#%d ",pProcessPrm->index);
				//OSA_printf("Manipulated the Index to be %d", ++(pProcessPrm->index));


	   //GBCE
	   pObj->glbceIIP->GBEStrength = pProcessPrm->GBEStrength;
	   pObj->glbceIIP->GCEStrength = pProcessPrm->GCEStrength;
	   //LBCE
	   pObj->glbceIIP->LBEStrength = pProcessPrm->LBEStrength;
	   pObj->glbceIIP->LCEStrength = pProcessPrm->LCEStrength;

     //if (IssAlg_capt2AGetAEWBVendor() == AEWB_ID_TI)
	 {
      pObj->glbceIIP->WB_RGain =  pProcessPrm->WB_RGain;
      pObj->glbceIIP->WB_GrGain = pProcessPrm->WB_GrGain;
      pObj->glbceIIP->WB_GbGain = pProcessPrm->WB_GbGain;
      pObj->glbceIIP->WB_BGain =  pProcessPrm->WB_BGain;
	  }
	  //else
	  //{
	  // pObj->glbceIIP->WB_RGain = 614;
	  // pObj->glbceIIP->WB_GrGain = 512;
	  // pObj->glbceIIP->WB_GbGain = 512;
	  // pObj->glbceIIP->WB_BGain = 570;
	  //}

	   pObj->glbceIIP->SensorType = pProcessPrm->sensorType;

     //pObj->gGain= (pObj->glbceIIP->WB_GrGain + pObj->glbceIIP->WB_GbGain + 1)>>1;
     pObj->glbceIIP->RRCoef =  rgb2rgbMat[0];
	   pObj->glbceIIP->RGCoef =  rgb2rgbMat[1];
	   pObj->glbceIIP->RBCoef =  rgb2rgbMat[2];
	   pObj->glbceIIP->GRCoef =  rgb2rgbMat[3];
	   pObj->glbceIIP->GGCoef =  rgb2rgbMat[4];
	   pObj->glbceIIP->GBCoef =  rgb2rgbMat[5];
	   pObj->glbceIIP->BRCoef =  rgb2rgbMat[6];
	   pObj->glbceIIP->BGCoef =  rgb2rgbMat[7];
	   pObj->glbceIIP->BBCoef =  rgb2rgbMat[8];

     pObj->glbceIIP->toneCurveIndex= pProcessPrm->toneCurveIndex;
	 pObj->glbceCIP->imgVertSize= pProcessPrm->imgVertSize;
	 pObj->glbceCIP->imgHorzSize= pProcessPrm->imgHorzSize;
	 pObj->glbceCIP->downsampleRatio= pProcessPrm->downsampleRatio;
     pObj->glbceCIP->temporalAverWeightLog= pProcessPrm->temporalAverWeightLog;

	 //OSA_printf("Got the Physical Address 0x%x", (unsigned int) pProcessPrm->boxCarPtr);
	 //OSA_printf("Size of the Buffer is %d", pProcessPrm->boxCarSize);

#ifdef USE_ISS_BOXCAR
     rgb2rgbMatPtr = (Int16 *) MM_mmap ((unsigned long) pProcessPrm->rgbMatPtr,
                      pProcessPrm->rgbMatSize);
	 Cache_inv(rgb2rgbMatPtr, pProcessPrm->rgbMatSize, Cache_Type_ALL, TRUE);
#endif
	 virtBoxCarAddr = (char  *)MM_mmap ((unsigned long) pProcessPrm->boxCarPtr,
                      pProcessPrm->boxCarSize);
	 //OSA_printf("Hence map the Virtual Address to 0x%x", (unsigned int) virtBoxCarAddr);
	 Cache_inv(virtBoxCarAddr, pProcessPrm->boxCarSize, Cache_Type_ALL, TRUE);

	 //OSA_printf("Got the Physical Address 0x%x", (unsigned int) pProcessPrm->tonalCurvePtr);
	 //OSA_printf("Size of the Buffer is %d", pProcessPrm->tonalCurveSize);

     virtTonalCurveAddr = (char  *)MM_mmap ((unsigned long) pProcessPrm->tonalCurvePtr,
                      pProcessPrm->tonalCurveSize);
	 //OSA_printf("Hence map the Virtual Address to 0x%x", (unsigned int) virtTonalCurveAddr);

	 pObj->glbceIIP->boxcarMemory = (Uint16*) virtBoxCarAddr;
	 pObj->glbce->toneCurves[0]  = (Byte *) virtTonalCurveAddr;


#ifdef USE_ISS_BOXCAR
	if(isSeasoningDone)
	  seasonBoxCarData((short *) pObj->glbceIIP->boxcarMemory, (int) pObj->glbceCIP->imgHorzSize, (int)  pObj->glbceCIP->imgVertSize,(int)  pObj->glbceCIP->downsampleRatio, rgb2rgbMatPtr);
#endif

       GLBCE_algConfig(&pObj->glbceHandle);

	   GLBCE_init(&pObj->glbceHandle);

		pObj->glbce->toneCurves[0] = (Byte *) virtTonalCurveAddr;
         pObj->glbce->toneCurves[1] = (Byte *)(virtTonalCurveAddr)+sizeof(unsigned char)*MAX_NUM_BLOCKS*NUM_OF_BINS;


       GLBCE_run(&pObj->glbceHandle);

	Cache_wb(virtTonalCurveAddr, pProcessPrm->tonalCurveSize, Cache_Type_ALL, TRUE);

#if 1
     MM_ummap(virtBoxCarAddr, pProcessPrm->boxCarSize);
     MM_ummap(virtTonalCurveAddr, pProcessPrm->tonalCurveSize);
#ifdef USE_ISS_BOXCAR
     MM_ummap(rgb2rgbMatPtr, pProcessPrm->rgbMatSize);
#endif
#endif

	 System_linkControl(pObj->createArgs.glbceLinkID,
                     GLBCE_LINK_CMD_TC_READY,
                     pProcessPrm,
                             sizeof(GlbceSupportLink_processPrm),FALSE);


    return GLBCE_SUPPORT_LINK_S_SUCCESS;
}
/* Delete the alg instance */

/* ===================================================================
 *  @func     GlbceSupportLink_algDelete
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
Int32 GlbceSupportLink_algDelete(GlbceSupportLink_Obj * pObj)
{
	OSA_memFree(pObj->glbceCIP->PersistentMemory);
	OSA_memFree(pObj->glbceCIP->externalScratchMemory);

  //if(mm_fd >= 0)
  //  close(mm_fd);
    close(memDevFd);

    return GLBCE_SUPPORT_LINK_S_SUCCESS;
}

// VS ALG delete
/* ===================================================================
 *  @func     Glbce_Support_delete
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
Void Glbce_Support_delete(int alg)
{
}

