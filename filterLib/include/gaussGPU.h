#ifndef GAUSS_GPU
#define GAUSS_GPU


#include "filter.h"
#include "CL/cl.h"
#include "logging.h"

using namespace std;

class ModelGPU : public IFilter
{
protected:
	cl_int errCode = 0;
	cl_uint iNumOfPlatforms = 0;
	cl_device_id deviceId = NULL;
	cl_uint numDevice;
	cl_platform_id *platform;
	cl_context_properties props[3];
	cl_context context;
	cl_command_queue queue;
	cl_mem inputImage;
	cl_mem outputImage;
	cl_mem filter;
	cl_program program;
	char* source;
	cl_kernel kernel;

	//void* uiInput;
	//FILE *video;

	int filtrRadius;
	int sizeOfBlockX;
	int sizeOfBlockY;

	int paddingPixels;
	int wgWidth;
	int wgHeight;
	int totalWorkItemsX;
	int totalWorkItemsY;

	size_t localSize[2];
	size_t globalSize[2];

	int localWidth;
	int localHeight;
	int height;
	int width;
	int filterWidth;
	size_t localMemSize;
	int frameCounter = 0;
	int bufferCounter = 0;
	
	//EasyLogging Logger;
	/* @brief privàt method for chekking errors. Print error code 
	   and name of function in console.*/
	// param@ err - error code
	// param@ err_code - name of function with error
	void checkErr(
		int err,
		const char* err_code);
	
	// @brief privat method for computing total number of work-items
	// param@ value - number pixels in string
	// param@ multiple - size of work-group
	unsigned int roundUp(unsigned int value, unsigned int multiple);
	
	/*Read the kernel file from some source*/
	char* readKernel(const char* kernelPath);


public:
	ModelGPU();
	~ModelGPU();
	
	/* @brief method for initialization of context, queue, program and k
	ernel before start "Kernel", also set "Kernel" arguments*/
	// param@ fWeight - weight coefficient
	// param@ choosenPlatform - srial number of choosen platform (0 - Nvidia, 1 - intel)
	void makeInit(
		float *fWeight,
		int choosenPlatform,
		char* passToKernel,
		int iRadius,
		int iWidth,
		int iHeight,
		int sizeOfProcesedBlockX,
		int sizeOfProcesedBlockY,
		ParamCaptGPU *ParamGPU);

	/** @brief . Function for video filtering using a Gaussian filter GPU model.
	@param YFrameSet A video sequence in which the central block will be filtered.
	@param *YProcCPU Array to record the results of filtering CPU model.
	@param *YProcGPU Array to record the results of filtering GPU model.*/

	void filtering(unsigned char **YFrameSet, unsigned char *YProcCPU, unsigned char *YProcGPU);

	//@brief method for release "Kernel" objects
	//void releaseObj();	
};
#endif
