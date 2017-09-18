#ifndef CAPTURE_INTERFACE
#define CAPTURE_INTERFACE

#include <thread>
#include <mutex>
#include <condition_variable>
#include "propertiesCPUGPU.h"
#include "bufferManager.h"
#include "paramCaptGPU.h"

/*Structure containing the parameters required for simulating the transmission frames in real time
Y, Cb and Cr are the components of frame
w is width of frame
h is height of frame
NFrames is the number of frames in current video
BuffSize is the size of buffer that contains frames
stop is the flag signalized that the all of frames are transmited
FrameNumber is number of last transmited frame
*/
class CaptureFrameParams
{
public:
	int width;
	int heigth;
	int nFrames;
	int radius;
	bool stop = false;
	unsigned char *setFramePointer;
	BufferManager buffer;

	~CaptureFrameParams();
	/* @brief . Function for initialization parametrs fo capturing frames.
	@param iNFrames number of frames.
	@param width width of image.
	@param height height of image.
	@param iRadius filter radius.*/

	void initCaptureFrameParams(int iNFrames, int width, int heigth, int iRadius);

};
/* @brief . Base class iCaptureFrame for captureFrames .*/
class ICaptureFrame
{
public:
	/* @brief . Virtual function for capturing frames for CPU or GPU model.
	@param Param Class with parametrs fo capturing frames.
	@param *pathToInputClip Path to input clip.
	@param *outputFile pointer to output file.*/
	virtual void captureFrame(
		CaptureFrameParams &Param, 
		std::mutex &captureFrameMutex, 
		char* pathToInputClip, 
		std::condition_variable &captureFrameCondition,
		FILE *outputFile) = 0;

	virtual void init(ParamCaptGPU *ParamGPU) = 0;

};

/* @brief . Function for calling method of base class wich capturing frames for CPU or GPU model.
@param Param Class with parametrs fo capturing frames.
@param *pathToInputClip Path to input clip.
@param *outputFile pointer to output file.*/
void callCaptureFrame(ICaptureFrame *captFrames, 
	CaptureFrameParams &Param,
	std::mutex &captureFrameMutex,
	char* pathToInputClip,
	std::condition_variable &captureFrameCondition,
	FILE *outputFile);

#endif