#include "propertiesCPUGPU.h"
#include <math.h>
//#include <stdlib.h>
#pragma warning(disable: 4996)

void CPUGPUProperties::initProperties()
{
	char* configFile = readSource("..//clip//config.txt");
	rapidjson::Document conf;
	conf.Parse(configFile);
	
	nFrames = conf["nFrames"].GetInt();
	width = conf["width"].GetInt();
	heigth = conf["heigth"].GetInt();
	radius = conf["radius"].GetInt();
	sigma = conf["sigma"].GetDouble();
	heightOfProcBlock = conf["heightOfProcBlock"].GetInt();
	widthOfProcBlock = conf["widthOfProcBlock"].GetInt();
	choosenPlatform = conf["choosenPlatform"].GetInt();

	pathToKernel = new char[conf["pathToKernel"].GetStringLength()+1];
	std::strcpy(pathToKernel, conf["pathToKernel"].GetString());

	pathToInputClip = new char[conf["pathToInputClip"].GetStringLength() + 1];
	std::strcpy(pathToInputClip, conf["pathToInputClip"].GetString());

	pathToOtputClipCPU = new char[conf["pathToOtputClipCPU"].GetStringLength() + 1];
	std::strcpy(pathToOtputClipCPU, conf["pathToOtputClipCPU"].GetString());

	pathToOtputClipGPU = new char[conf["pathToOtputClipGPU"].GetStringLength() + 1];
	std::strcpy(pathToOtputClipGPU, conf["pathToOtputClipGPU"].GetString());

	z = calculateZ(radius, sigma);
	weights = new float[(2 * radius + 1)*(2 * radius + 1)*(2 * radius + 1)];
	calculateW(weights, z, radius, sigma);
	frameSet = new unsigned char*[2 * radius + 1];
	YProcCPU = new unsigned char[width*heigth];
	YProcGPU = new unsigned char[width*heigth];
	
};
float CPUGPUProperties::calculateZ(int filtrRadius, float sigma)
{
	float z = 0;
	for (register int l = -filtrRadius; l <= filtrRadius; l++)
	{
		for (register int k = -filtrRadius; k <= filtrRadius; k++)
		{
			for (register int m = -filtrRadius; m <= filtrRadius; m++)
			{
				z += exp(-float(k*k + m*m + l*l) / (2 * sigma * sigma));
			}
		}
	}
	return z;
}

void CPUGPUProperties::calculateW(float *weights, float z, int filtrRadius, float sigma)
{
	int count = 2 * filtrRadius + 1;
	for (register int l = -filtrRadius; l <= filtrRadius; l++)
	{
		for (register int k = -filtrRadius; k <= filtrRadius; k++)
		{
			for (register int m = -filtrRadius; m <= filtrRadius; m++)
			{
				weights[((l + filtrRadius)*count*count) + ((k + filtrRadius)*count + (m + filtrRadius))] = (float(1) / z)*exp(-float(k*k + m*m + l*l) / (2 * sigma * sigma));
			}
		}
	}
}

CPUGPUProperties::~CPUGPUProperties()
{
	delete[] frameSet;
	delete[] YProcCPU;
	delete[] YProcGPU;
};

