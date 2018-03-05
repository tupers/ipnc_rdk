#include "IMG_Common.h"

#ifdef _C674DSP_
#include <xdc/runtime/System.h>
#include <ti/mathlib/mathlib.h>
#include <ti/vlib/vlib.h>
#endif

void IMG_ZoomOut_FixedRatio(IMG_FrameData* src, IMG_FrameData* dst, int ratio)
{
	if ((src->cols == 0) || (src->rows == 0) || (dst->cols == 0)
			|| (dst->rows == 0))
		return;
	if ((src->cols % ratio != 0) || (src->rows % ratio != 0)
			|| (dst->cols % ratio != 0) || (dst->rows % ratio != 0))
		return;
	if ((dst->cols * ratio != src->cols) || (dst->rows * ratio != src->rows))
		return;
	int x, y;
	int dst_cnt = -1;
	for (y = ratio - 1; y < src->rows; y += ratio)
	{
		int preindex = y * src->cols;
		for (x = ratio - 1; x < src->cols; x += ratio)
		{
			dst_cnt++;
			dst->data[dst_cnt] = src->data[preindex + x];
		}
	}

//	VLIB_image_rescale(src->data,dst->data,4096*ratio,src->cols,src->rows,3);
	return;
}

void IMG_Projection_H(IMG_FrameData*src, IMG_Pro*ph)
{
	if (src->rows != ph->length)
		return;
	int x, y;
	int hpcnt;
	for (y = 0; y < src->rows; y++)
	{
		int preindex = y * src->cols;
		hpcnt = 0;
		for (x = 0; x < src->cols; x++)
		{
			hpcnt += src->data[preindex + x];
		}
		ph->data[y] = hpcnt / 255;

	}

	return;
}

void IMG_Projeciton_V(IMG_FrameData*src, IMG_Pro*pv)
{
	if (src->cols != pv->length)
		return;
	int x, y;
	for (y = 0; y < src->rows; y++)
	{
		int preindex = y * src->cols;
		for (x = 0; x < src->cols; x++)
		{
			if (src->data[preindex + x] > 0)
				pv->data[x]++;
		}
	}
	return;
}

int IMG_getThreshold(IMG_FrameData*src, IMG_HistogramStr* restrict histparams)
{
	int i, j;
	float imgsize = src->cols * src->rows;
	unsigned short pixelCount[256];
	float pixelPro[256];
	memset(&pixelPro, 0, sizeof(float) * 256);
	memset(&pixelCount, 0, sizeof(unsigned short) * 256);

	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp;
	float deltaMax = 0;
	int threshold;
#ifdef _C674DSP_
	IMG_DSP_getHistogram(src, histparams, pixelCount);
#else
	for (i = 0; i < src->cols * src->rows; i++)
	{
		pixelCount[src->data[i]]++;
	}
#endif

	for (i = 0; i < 256; i++)
	{
#ifdef _C674DSP_
		pixelPro[i] = divsp(pixelCount[i], imgsize);
#else
		pixelPro[i] = (float) pixelCount[i] / imgsize;
#endif
	}

	for (i = 0; i < 256; i++)
	{
		w0 = 0.0f;
		w1 = 0.0f;
		u0tmp = 0.0f;
		u1tmp = 0.0f;
		u0 = 0.0f;
		u1 = 0.0f;
		deltaTmp = 0.0f;

		for (j = 0; j < 256; j++)
		{
			if (j <= i)	//背景部分
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			} else	//前景部分
			{
				w1 += pixelPro[j];
				u1tmp += j * pixelPro[j];
			}
		}

		u0 = u0tmp / w0;
		u1 = u1tmp / w1;
		//		deltaTmp =
		//		w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
#ifdef _C674DSP_
		deltaTmp = w0 * w1 * powsp(u0 - u1, 2);
#else
		deltaTmp = w0 * w1 * (u0 - u1) * (u0 - u1);
#endif

		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = i;
		}
	}

	return threshold;
}

void IMG_Threshold(IMG_FrameData*in, IMG_FrameData*out, int thres)
{
#ifdef _C674DSP_
	IMG_DSP_thrmin2max(in->data, out->data, in->cols, in->rows, thres);
#else
		int i;
		for (i = 0; i < in->cols * in->rows; i++)
		{
			if (in->data[i] > thres)
				out->data[i] = 255;
			else
				out->data[i] = 0;
		}
#endif
}

void IMG_Threshold_PH(IMG_FrameData*in, IMG_FrameData*out, IMG_Pro* ph,
		int thres)
{
	int cols, rows;
	int hpcnt;
	int preindex;
	for (rows = 0; rows < in->rows; rows++)
	{
		preindex = rows * in->cols;
		hpcnt = 0;
		for (cols = 0; cols < in->cols; cols++)
		{
			if (in->data[preindex + cols] > thres)
			{
				out->data[preindex + cols] = 255;
				hpcnt++;
				//ph->data[rows]++;
			} else
			{
				out->data[preindex + cols] = 0;
			}
		}
		ph->data[rows] = hpcnt;
	}

}

void IMG_DSP_thrmin2max(const unsigned char * restrict in_data,
		unsigned char*restrict out_data, short cols, short rows,
		unsigned char threshold)
{
	int i, pixels = rows * cols;
	unsigned thththth;

	_nassert((int) in_data % 8 == 0);
	_nassert((int) out_data % 8 == 0);
	_nassert(pixels % 16 == 0);
	_nassert(pixels >= 16);

	thththth = _pack2(threshold, threshold);
	thththth = _packl4(thththth, thththth);

	/* -------------------------------------------------------------------- */
	/*  Step through input image copying pixels to the output.  If the      */
	/*  pixels are above our threshold, set them to 255.                    */
	/* -------------------------------------------------------------------- */
#pragma MUST_ITERATE(4,,4);
#pragma UNROLL(4);

	for (i = 0; i < pixels; i += 4)
	{
		unsigned p3p2p1p0, x3x2x1x0;

		p3p2p1p0 = _amem4_const(&in_data[i]);
		x3x2x1x0 = _xpnd4(_cmpgtu4(p3p2p1p0, thththth));

		_amem4(&out_data[i]) = x3x2x1x0;
	}
}

void IMG_DSP_InitHistogram(IMG_HistogramStr*restrict histparams)
{
	int i;
	for (i = 0; i < 256; i++)
		histparams->binedge[i] = i;
	memset(histparams->histarray, 0, sizeof(unsigned char) * 257);
	memset(histparams->H1, 0, sizeof(unsigned short) * 256);
	memset(histparams->H2, 0, sizeof(unsigned short) * 256);
	memset(histparams->H3, 0, sizeof(unsigned short) * 256);
	VLIB_histogram_1D_Init_U8(histparams->binedge, 256, histparams->histarray);
}

void IMG_DSP_getHistogram(const IMG_FrameData* restrict src,
		IMG_HistogramStr* restrict histparams, unsigned short*restrict H)
{
	memset(histparams->H1, 0, sizeof(short) * 256);
	memset(histparams->H2, 0, sizeof(short) * 256);
	memset(histparams->H3, 0, sizeof(short) * 256);
	VLIB_histogram_1D_U8(src->data, src->cols * src->rows, 256, 1,
			histparams->histarray, histparams->H1, histparams->H2,
			histparams->H3, H);
}

