#ifndef CAPTURE_FRAME_GPU
#define CAPTURE_FRAME_GPU

#include "captureFrameInterface.h"
#include "paramCaptGPU.h"
#include "CL/cl.h"

class captureFrameGPU : public ICaptureFrame
{
private:
	cl_context context;
	cl_command_queue queue;
	cl_kernel kernel;
	cl_mem inputImage;
	void* uiInput;
	int frameCounter = 0;
	int bufferCounter = 0;
	void checkErr(int err, const char* err_code);
public:
	/*Run the thread, that simulates the transmission frame main stream in
	real time in accordance with pre-set parameters.
	These parameters are stored in the structure SF_Param*/

	void captureFrame(
		CaptureFrameParams &Param,
		std::mutex &captureFrameMutex,
		char* pathToInputClip,
		std::condition_variable &captureFrameCondition,
		FILE *outputFile);
	void init(ParamCaptGPU *ParamGPU);
};
#endif
