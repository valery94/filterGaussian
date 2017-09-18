#ifndef PARAM_CAPT_GPU
#define PARAM_CAPT_GPU
#include "CL\cl.h"
class ParamCaptGPU
{
private:
	cl_context context;
	cl_command_queue queue;
	cl_kernel kernel;
public:
	cl_context getContext();
	cl_command_queue getQueue();
	cl_kernel getKernel();
	void setContext(cl_context inputContext);
	void setQueue(cl_command_queue inputQueue);
	void setKernel(cl_kernel inputKernel);
};

#endif