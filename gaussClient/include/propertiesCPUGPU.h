#ifndef CPUGPUPROPERTIES_H
#define CPUGPUPROPERTIES_H


#include <time.h>
#include "filter.h"
#include"document.h"

#include "readWriteFile.h"
//#include <iostream>

class CPUGPUProperties
{
public:
	
	int nFrames;
	int width;
	int heigth;
	int radius;
	float sigma;
	clock_t processingTimePerFrame;
	unsigned char **frameSet;
	unsigned char *YProcCPU;
	unsigned char *YProcGPU;
	double averageTime = 0;
	float z;
	float *weights;
	int widthOfProcBlock;
	int heightOfProcBlock;
	int choosenPlatform;

	char* pathToKernel;
	char* pathToInputClip;
	char* pathToOtputClipCPU;
	char* pathToOtputClipGPU;

	void initProperties();
	
	/** @brief . Function calculationZ calcualate normalization coefficient Z.
	@param r Radius of the filter.
	@param sigma Filter parameter that defines the "smoothness".
	*/
	float calculateZ(int r, float sigma);
	
	/** @brief . Function calculationW calcualate filter coefficients W.
	@param weights An array that will store received odds on.
	@param z Normalization coefficient .
	@param r Radius of the filter.
	@param sigma Filter parameter that defines the "smoothness".
	*/
	void calculateW(float *weights, float z, int r, float sigma);

	~CPUGPUProperties();
};


#endif

