#include <iostream>
#include <math.h>
#include "gaussCPU.h"

ModelCPU::ModelCPU()
{
	
};


ModelCPU::~ModelCPU()
{
};

void ModelCPU::makeInit(
	float *fWeight, 
	int choosenPlatform, 
	char* passToKernel, 
	int iRadius, 
	int iWidth, 
	int iHeight, 
	int sizeOfProcesedBlockX,
	int sizeOfProcesedBlockY,
	ParamCaptGPU *ParamGPU)
{
	height = iHeight;
	width = iWidth;
	filtrRadius = iRadius;
	weights = fWeight;
};

void ModelCPU::filtering(unsigned char **YFrameSet, unsigned char *YProcCPU, unsigned char *YProcGPU)
{
	float sum; int num;
	int sizeY = height*width;
	for(register int i = 0; i < height; i++)
	{
		for(register int j = 0; j < width; j++)
		{
			if(i >= filtrRadius && j >= filtrRadius && i < height - filtrRadius && j < width - filtrRadius)
			{
				num = 0; sum = 0;
				for (register int k = -filtrRadius; k <= filtrRadius; k++)
				{
					for (register int x = i - filtrRadius; x <= i + filtrRadius; x++)
					{
						for (register int y = j - filtrRadius; y <= j + filtrRadius; y++)
						{
							sum += weights[num++] * YFrameSet[k + filtrRadius][x*width + y];
						}
					}
				}
				sum = sum > 255 ? 255 : sum;
				sum = sum < 0 ? 0 : sum;
				YProcCPU[i*width + j] = unsigned char(sum);
			}
			else
			{
				YProcCPU[i*width + j] = 0;
			}
		}
	}
}

/*void ModelCPU::releaseObj()
{

}*/