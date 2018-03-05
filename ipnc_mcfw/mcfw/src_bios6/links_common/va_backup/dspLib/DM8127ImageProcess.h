/*
 * DM8127ImageProcess.h
 *
 *  Created on: 2015-9-25
 *      Author: Administrator
 */

#ifndef DM8127IMAGEPROCESS_H_
#define DM8127IMAGEPROCESS_H_

#include "ImageLib_osa.h"

void ImageProcess(unsigned char* outData, unsigned char* src,const unsigned int row, const unsigned int col);

void BinaryImage(unsigned char * dst, const unsigned char* src,const unsigned int cols, const unsigned int rows,unsigned int threshold);

void Sobel_3x3(const unsigned char* inData, unsigned char* outData,unsigned int cols, unsigned int rows);

#endif /* DM8127IMAGEPROCESS_H_ */
