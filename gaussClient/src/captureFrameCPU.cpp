#include "captureFrameCPU.h"
#include "readWriteFile.h"
#include <condition_variable>
#pragma warning(disable: 4996)
void captureFrameCPU::init(ParamCaptGPU *ParamGPU)
{
	return;
}

void captureFrameCPU::captureFrame(
	CaptureFrameParams &Param, 
	std::mutex &captureFrameMutex, 
	char* pathToInputClip, 
	std::condition_variable &captureFrameCondition, 
	FILE *outputFile)
{
	FILE *f = fopen(pathToInputClip, "rb");
	if (f == NULL)
	{
		printf("Error: Can't open input clip.\n");
		return;
	}

	int sizeOfImage = Param.heigth*Param.width*1.5*sizeof(unsigned char);
	unsigned char* frameBuffer = new unsigned char[sizeOfImage];
	for (int i = 0; i < Param.nFrames; i++)
	{
		fread(frameBuffer, sizeOfImage, 1, f);
		fwrite(frameBuffer, sizeOfImage, 1, outputFile);
	}
	delete[] frameBuffer;
	fseek(outputFile, 0, SEEK_SET);
	fseek(f, 0, SEEK_SET);

	int CurrentFrame = 0;
	while (CurrentFrame<Param.nFrames)
	{
		std::unique_lock<std::mutex> lock(captureFrameMutex);
		Param.buffer.setFrameInBuffer(Param.setFramePointer);
		readFrame(Param.setFramePointer, Param.width, Param.heigth, f);
		CurrentFrame++;
		captureFrameCondition.wait(lock);
	}
	fclose(f);
	Param.stop = true;
};



CaptureFrameParams::~CaptureFrameParams()
{
};