/** ==================================================================
 *  @file   glbceLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/glbce/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include "glbceLink_movAvg.h"
 
void movAvgInit(movAvgClass * srt)
{
    srt->readIndex = 0;
	srt->writeIndex = 0;
	srt->counter = 0;
	srt->accumulator = 0;

    return;	
}

int  movAvgTakeNewGiveAverage(movAvgClass * srt, int newData)
{
    if( srt->counter <  LENGTH_OF_AVERAGE)
	{
	    srt->array[srt->writeIndex++] = newData;
		if(srt->writeIndex == LENGTH_OF_AVERAGE)
		  srt->writeIndex = 0;
		
		srt->newsetData = newData;
		srt->accumulator += newData;
		srt->counter++;
		
		return(srt->accumulator/srt->counter);
		
	}
	else
	{
	    srt->oldestData = srt->array[srt->readIndex++];
		if(srt->readIndex == LENGTH_OF_AVERAGE)
		  srt->readIndex = 0;
	    srt->array[srt->writeIndex++] = newData;
		if(srt->writeIndex == LENGTH_OF_AVERAGE)
		  srt->writeIndex = 0;
		
		srt->newsetData = newData;
		srt->accumulator += newData;
	    srt->accumulator -=  srt->oldestData;
		return(srt->accumulator/LENGTH_OF_AVERAGE);
	}
    
}


