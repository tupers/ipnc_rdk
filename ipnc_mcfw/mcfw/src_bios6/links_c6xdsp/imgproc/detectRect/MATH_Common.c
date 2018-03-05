/*
 * MATH_Common.c
 *
 *  Created on: 2017Äê1ÔÂ4ÈÕ
 *      Author: tupers
 */

#ifdef _C674DSP_
#include <ti/mathlib/mathlib.h>
#endif

float MATH_Variance(unsigned int*data,int start,int end)
{
	int i;
	int sum=0;
	for(i=start;i<end+1;i++)
		sum+=data[i];
	float avg = sum/(end-start+1);
	float var=0.0;
	for(i=start;i<end+1;i++)
	{
#ifdef _C674DSP_
		var += powsp(data[i]-avg,2);
#else
		var += (data[i]-avg)*(data[i]-avg);
#endif

	}
	var = var/(end-start+1);
	return var;
}


