/*
 * Copyright (C) 2005-2006 WIS Technologies International Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and the associated README documentation file (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// A class that encapsulates a 'FramedSource' object that
// returns JPEG video frames.
// Implementation

#include "WISJPEGStreamSource.hh"
#include <stdio.h>
#include <stdlib.h>

// NOTE: Change the following if the size of the encoder's JPEG hdr changes
#define  JPEG_HEADER_SIZE (0xC00 - 2416)

WISJPEGStreamSource*
WISJPEGStreamSource::createNew(FramedSource* inputSource) {
  return new WISJPEGStreamSource(inputSource);
}

WISJPEGStreamSource::WISJPEGStreamSource(FramedSource* inputSource)
  : JPEGVideoSource(inputSource->envir()),
    fLastWidth(0), fLastHeight(0), fLastQuantizationTableSize(128),fLastrestartIntervalTableSize(4) {
  fSource = inputSource;
}

WISJPEGStreamSource::~WISJPEGStreamSource() {
  Medium::close(fSource);
}

void WISJPEGStreamSource::doGetNextFrame() {
  fSource->getNextFrame(fBuffer, sizeof fBuffer,
			WISJPEGStreamSource::afterGettingFrame, this,
			FramedSource::handleClosure, this);
}

u_int8_t WISJPEGStreamSource::type() {
  return 65;
};

u_int8_t WISJPEGStreamSource::qFactor() {
  return 255; // indicates that quantization tables are dynamic
};

u_int8_t WISJPEGStreamSource::width() {
  return fLastWidth;
}

u_int8_t WISJPEGStreamSource::height() {
  return fLastHeight;
}

u_int8_t const* WISJPEGStreamSource::quantizationTables(u_int8_t& precision,
							   u_int16_t& length) {
  precision = 0;
  length = fLastQuantizationTableSize;
  return fLastQuantizationTable;
}

u_int8_t const* WISJPEGStreamSource::restartIntervalTables(u_int8_t& precision,
							   u_int16_t& length) {
  precision = 0;
  length = fLastrestartIntervalTableSize;
  return fLastrestartIntervalTable;
}

 
void WISJPEGStreamSource
::afterGettingFrame(void* clientData, unsigned frameSize,
		    unsigned numTruncatedBytes,
		    struct timeval presentationTime,
		    unsigned durationInMicroseconds) {
  WISJPEGStreamSource* source = (WISJPEGStreamSource*)clientData;
  source->afterGettingFrame1(frameSize, numTruncatedBytes,
			     presentationTime, durationInMicroseconds);
}

int findNumberOfEOIMarkers(unsigned char * data, unsigned int size)
{
  int numEOI = 0;
  int i;
  for (i = 0; i < size; i++)
  {
	  if(data[i] == 0xFF && data[i+1] == 0xD9)
	  numEOI++;
  }
  return numEOI;
}
void dumpJPEGImageToFile(unsigned char * data, unsigned int size)
{
	FILE *fp;
	static char number[4] = "000";
	char fileName[100] = "MJPEG_File_";
	char *number2String;

	number2String = number;

	strcat(fileName, number2String);
	strcat(fileName,".jpg");

	fp =fopen(fileName, "wb");
	fwrite(data, size, 1, fp);
	fclose(fp);

	number[2]++;
}

void WISJPEGStreamSource
::afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
		     struct timeval presentationTime,
		     unsigned durationInMicroseconds) {
  fFrameSize = frameSize < fMaxSize ? frameSize : fMaxSize;
  // NOTE: Change the following if the size of the encoder's JPEG hdr changes
  unsigned int JPEGHeaderSize = JPEG_HEADER_SIZE;
  unsigned i = 0;
  unsigned char isQuantTableFound = 0;
  
  // Look for the "SOF0" marker (0xFF 0xC0) in the header, to get the frame
  // width and height.  Also, look for the "DQT" marker(s) (0xFF 0xDB), to
  // get the quantization table(s):
  Boolean foundSOF0 = False;
  fLastQuantizationTableSize = 0;
  
  for(i = 0, JPEGHeaderSize = 0; i < JPEG_HEADER_SIZE; i++)
	{
	   if(fBuffer[i] == 0xFF )
	   if(fBuffer[i+1] == 0xDA )  //SOS Marker
	   if(fBuffer[i+12] == 0x3f )
	   if(fBuffer[i+13] == 0x00 )
		{
		    JPEGHeaderSize += 14;
	        break;
		}
		 
	 JPEGHeaderSize++;
	}
  
  //envir() << "JPEGHeaderSize is " << JPEGHeaderSize <<"\n"; 
  
  
  for (i = 0; i < JPEGHeaderSize; ++i) {
  
    if (fBuffer[i] == 0xFF) {
		if (fBuffer[i+1] == 0xDB && isQuantTableFound == 0) 
		{ // DQT
			u_int16_t length = (fBuffer[i+2]<<8) | fBuffer[i+3];
			//envir() << "i is " << i <<"\n"; 
			//envir() << "length is " << length <<"\n"; 
			if (i+2 + length <= JPEGHeaderSize) 
			{ // sanity check
			    isQuantTableFound = 1;
				u_int16_t tableSize = length - 3;
				//envir() << "tableSize is " << tableSize <<"\n"; 
				if (fLastQuantizationTableSize + tableSize > 128) { // sanity check
				tableSize = 128 - fLastQuantizationTableSize;
				}
				memmove(&fLastQuantizationTable[fLastQuantizationTableSize],
				  &fBuffer[i+5], 64);
				memmove(&fLastQuantizationTable[fLastQuantizationTableSize+64],
				  &fBuffer[i+5+1+64], 64);

				
				fLastQuantizationTableSize += tableSize;
				
				if (fLastQuantizationTableSize == 128 && foundSOF0) break;
				
				  // we've found everything that we want
				i += length; // skip over table
			}
      	} 
     	else if (fBuffer[i+1] == 0xC0) 
		{ // SOF0

			fLastHeight = (fBuffer[i+5]<<5)|(fBuffer[i+6]>>3);
			fLastWidth = (fBuffer[i+7]<<5)|(fBuffer[i+8]>>3);
			foundSOF0 = True;
			if (fLastQuantizationTableSize == 128) break;


			// we've found everything that we want
			i += 8;
		}
    }
  }
  	
  //envir() << "Found SOFO!!!! with Height " << fLastHeight << "with Width " << fLastWidth << "and QuantTable Size is " << fLastQuantizationTableSize <<"\n";
  	
  for (i = 0; i < JPEGHeaderSize; i++) 
  {
  	//printf("2 %X %X \n",fBuffer[i],fBuffer[i+1]);
    if (fBuffer[i] == 0xFF) 
    {
		if (fBuffer[i+1] == 0xDD) 
		{
			fLastrestartIntervalTable[0] = fBuffer[i+4];
			fLastrestartIntervalTable[1] = fBuffer[i+5];
			fLastrestartIntervalTable[2] = 0xFF;
			fLastrestartIntervalTable[3] = 0xFF;
			break;
		}
	}
  }

  //envir() << "Number of EOI markers found is " << findNumberOfEOIMarkers(&fBuffer[JPEGHeaderSize], fFrameSize - JPEGHeaderSize) << " in size of " <<(fFrameSize - JPEGHeaderSize) << " bytes  \n";
  //dumpJPEGImageToFile(&fBuffer[0], fFrameSize);
  
  if (fLastQuantizationTableSize == 64) {
    // Hack: We apparently saw only one quantization table.  Unfortunately,
    // media players seem to be unhappy if we don't send two (luma+chroma).
    // So, duplicate the existing table data:
    memmove(&fLastQuantizationTable[64], fLastQuantizationTable, 64);
    fLastQuantizationTableSize = 128;
  }

  if (!foundSOF0) envir() << "Failed to find SOF0 marker in JPEG header!\n";


  // Complete delivery to the client:
  fFrameSize = fFrameSize > JPEGHeaderSize ? fFrameSize - JPEGHeaderSize : 0;
  memmove(fTo, &fBuffer[JPEGHeaderSize], fFrameSize);
  fNumTruncatedBytes = numTruncatedBytes;
  fPresentationTime = presentationTime;
  fDurationInMicroseconds = fDurationInMicroseconds;
  FramedSource::afterGetting(this);

}
