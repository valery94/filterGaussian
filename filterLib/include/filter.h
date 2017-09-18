#ifndef FILTER
#define FILTER
#include "paramCaptGPU.h"
/** @brief Base class IFilter filtaring*/
enum FilterID { CPUFilterID = 0, GPUFilterID };

class IFilter
{
public:
	
	/* @brief . Virtual function for video filtering using CPU or GPU model.
	@param YFrameSet A video sequence in which the central block will be filtered.
	@param *YProcCPU Array to record the results of filtering CPU model.
	@param *YProcGPU Array to record the results of filtering GPU model.*/
	virtual void filtering(
		unsigned char **YFrameSet,
		unsigned char *YProcCPU,
		unsigned char *YProcGPU
		) = 0;

	/* @brief . Virtual function for initialization of context, queue, program and k
	ernel before start "Kernel", also set "Kernel" arguments. Only use for GPU model.
	param@ fWeight - weight coefficient
	param@ choosenPlatform - srial number of choosen platform (0 - Nvidia, 1 - intel)*/
	virtual void makeInit(
		float *fWeight,
		int choosenPlatform,
		char* passToKernel, 
		int iRadius, 
		int iWidth, 
		int iHeight, 
		int sizeOfProcesedBlockX,
		int sizeOfProcesedBlockY,
		ParamCaptGPU *ParamGPU) = 0;

	virtual ~IFilter() {};
};

#endif	