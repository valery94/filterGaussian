#ifndef CAPTURE_FRAME_H
#define CAPTURE_FRAME_H


#include "captureFrameInterface.h"
#include "paramCaptGPU.h"

/*Run the thread, that simulates the transmission frame main stream in
real time in accordance with pre-set parameters.
These parameters are stored in the structure SF_Param*/
class captureFrameCPU : public ICaptureFrame
{
public:
	void captureFrame(
		CaptureFrameParams &Param, 
		std::mutex &captureFrameMutex, 
		char* pathToInputClip, 
		std::condition_variable &captureFrameCondition,
		FILE *outputFile);
	void init(ParamCaptGPU *ParamGPU);
};

#endif




