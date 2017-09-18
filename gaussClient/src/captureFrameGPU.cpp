#pragma warning(disable: 4996)

#include "captureFrameGPU.h"
#include "CL/cl.h"
#include "logging.h"
//#include <condition_variable>
void captureFrameGPU::init(ParamCaptGPU *ParamGPU)
{
	
	context = (*ParamGPU).getContext();
	kernel = (*ParamGPU).getKernel();
	queue = (*ParamGPU).getQueue();
}

void captureFrameGPU::checkErr(int err, const char* err_code)
{
	if (err)
	{
		LOG(ERROR) << "Error: " << err_code << "(" << err << ")";
		std::cout << "press enter to continue..." << std::endl;
		getchar();
	}
}

void captureFrameGPU::captureFrame(
	CaptureFrameParams &Param,
	std::mutex &captureFrameMutex,
	char* pathToInputClip,
	std::condition_variable &captureFrameCondition,
	FILE *outputFile)
{
	cl_int errCode = 0;
	int filterWidth = 2 * Param.radius + 1;
	int sizeY = Param.width*Param.heigth;
	int sizeOfImage = Param.width*Param.heigth*sizeof(unsigned char);
	int deviceDataSize = Param.width * Param.heigth* filterWidth * sizeof(unsigned char);
	int offset = sizeOfImage / 2;

	FILE *f = fopen(pathToInputClip, "rb");
	if (f == NULL)
	{
		printf("Error: Can't open input clip.\n");
		return;
	}
	unsigned char* frameBuffer = new unsigned char[sizeOfImage + offset];
	for (int i = 0; i < Param.nFrames; i++)
	{
		fread(frameBuffer, sizeOfImage + offset, 1, f);
		fwrite(frameBuffer, sizeOfImage + offset, 1, outputFile);
	}
	delete[] frameBuffer;
	fseek(outputFile, 0, SEEK_SET);
	fseek(f, 0, SEEK_SET);

	inputImage = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
		Param.width * Param.heigth * filterWidth * sizeof(unsigned char), NULL, &errCode);
	checkErr(errCode, "clCreateBuffer in captureFrameGPU");

	int CurrentFrame = 0;
	while (CurrentFrame < Param.nFrames)
	{
		std::unique_lock<std::mutex> lock(captureFrameMutex);
		errCode = clSetKernelArg(kernel, 9, sizeof(int), &bufferCounter);
		checkErr(errCode, "clSetKernelArg in captureFrameGPU");
		Param.buffer.setFrameInBuffer(Param.setFramePointer);

		bufferCounter++;
		if (bufferCounter == filterWidth)
			bufferCounter = 0;
		uiInput = clEnqueueMapBuffer(queue, inputImage, CL_TRUE, CL_MAP_WRITE, bufferCounter * sizeY, sizeY * sizeof(unsigned char), 0, NULL, NULL, &errCode);
		checkErr(errCode, "clEnqueueMapBuffer in captureFrameGPU");
		fread(uiInput, sizeOfImage, 1, f);
		fseek(f, offset, SEEK_CUR);

		clEnqueueUnmapMemObject(queue, inputImage, uiInput, 0, NULL, NULL);
		errCode = clSetKernelArg(kernel, 0, sizeof(inputImage), &inputImage);
		checkErr(errCode, "clSetKernelArg in captureFrameGPU");
		CurrentFrame++;
		captureFrameCondition.wait(lock);
	}
	
	fclose(f);
	Param.stop = true;
}
