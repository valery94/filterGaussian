#ifndef GAUSS
#define GAUSS

#include "captureFrameInterface.h"
#include "readWriteFile.h"
#include "propertiesCPUGPU.h"
#include "filter.h"
//#include "gaussGPU.h"
//@brief metod using for 3D Gaussian filtration
// param@ captureFrameParams - parameters of the input video
// param@ cpuGpuParams - parameters of the filtering
// param@ fileCpu - output file for processing by CPU
// param@ fileGpu - output file for processing by GPU
void makeGaussianFiltering(CaptureFrameParams &captureFrameParams, CPUGPUProperties &cpuGpuParams, IFilter *modelCPU, FILE *fileCpu,
	FILE *fileGpu, IFilter *modelGPU, std::mutex &captureFrameMutex, std::condition_variable &captureFrameCondition, bool testGPU);

#endif