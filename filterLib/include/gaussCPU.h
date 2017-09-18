#ifndef GAUSS_H
#define GAUSS_H

#include "filter.h"
//#include ""
/** @brief Class MODEL_CPU is a 3D Gaussian filter to the video stream, where calculations are made on the CPU.
*/
class ModelCPU : public IFilter
{
private:
	int height;
	int width;
	int filtrRadius;
	float *weights;

public:
	ModelCPU();

	~ModelCPU();

	/** @brief . Function for video filtering using a Gaussian filter CPU model.
	@param YFrameSet A video sequence in which the central block will be filtered.
	@param *YProcCPU Array to record the results of filtering CPU model.
	@param *YProcGPU Array to record the results of filtering GPU model.*/
	
	void filtering(unsigned char **YFrameSet, unsigned char *YProcCPU, unsigned char *YProcGPU);

	void makeInit(float *fWeight, int choosenPlatform, char* passToKernel, int iRadius,
		int iWidth,
		int iHeight,
		int sizeOfProcesedBlockX,
		int sizeOfProcesedBlockY,
		ParamCaptGPU *ParamGPU);
};

#endif
