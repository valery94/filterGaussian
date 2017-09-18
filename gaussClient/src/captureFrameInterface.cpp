#include "captureFrameInterface.h"
void callCaptureFrame(ICaptureFrame *captFrames,
	CaptureFrameParams &Param,
	std::mutex &captureFrameMutex,
	char* pathToInputClip,
	std::condition_variable &captureFrameCondition, 
	FILE *outputFile)
{
	(*captFrames).captureFrame(Param, captureFrameMutex, pathToInputClip, captureFrameCondition, outputFile);
}

void CaptureFrameParams::initCaptureFrameParams(int iNFrames, int iwidth, int iheigth, int iRadius)
{
	nFrames = iNFrames;
	width = iwidth;
	heigth = iheigth;
	radius = iRadius;
	buffer.initBuffer(radius, width, heigth, 1);

}