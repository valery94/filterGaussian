#include<stdlib.h>
#include <iostream>
#include "logging.h"
void testVersionCPUGPU(FILE *f1, FILE *f2, int w, int h, int nFrames){
	
	fseek(f1, 0L, SEEK_SET);
	fseek(f2, 0L, SEEK_SET);

	float ssd = 0.0, sad = 0.0, difference = 0.0, avrBadPixels = 0.0;
	int valueCPU, valueGPU;
	int count = 0;
	int dif = 0;
	int maxDif = 0;
	unsigned char *frameCPU = new unsigned char[w*h*3/2];
	unsigned char *frameGPU = new unsigned char[w*h*3/2];
	for (int z = 0; z < nFrames; z++){
		fread(frameCPU, sizeof(unsigned char)*w*h*1.5, 1, f1);
		fread(frameGPU, sizeof(unsigned char)*w*h*1.5, 1, f2);
		for (int i = 0; i < h; i++){
			for (int j = 0; j < w; j++){
				valueCPU = frameCPU[i*w + j]; valueGPU = frameGPU[i*w + j];
				if (valueCPU != valueGPU)
				{
					count++;
					difference += valueCPU - valueGPU;
					dif = valueCPU - valueGPU;
					if (dif > maxDif)
						maxDif = dif;

					sad += abs(valueCPU - valueGPU);
					ssd += pow(float(valueCPU - valueGPU), 2);

				}
			}
		}
		avrBadPixels = difference / count;
		LOG(INFO) << "================================================================================";
		LOG(INFO) << "The maximum difference\t" << maxDif;
		LOG(INFO) << "Difference\t" << difference;
		LOG(INFO) << "Bad pixels\t" << count;
		LOG(INFO) << "Avr bad pixels\t" << avrBadPixels;
		LOG(INFO) << "SAD\t" << sad;
		LOG(INFO) << "SSD\t" << ssd;
		LOG(INFO) << "Try dont round " << 0.00333;
		LOG(INFO) << "================================================================================";
	}
	delete[] frameCPU;
	delete[] frameGPU;
}
