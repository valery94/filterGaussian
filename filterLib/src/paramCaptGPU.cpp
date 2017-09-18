#include "paramCaptGPU.h"

cl_context ParamCaptGPU::getContext()
{
	return context;
}

cl_command_queue ParamCaptGPU::getQueue()
{
	return queue;
}

cl_kernel ParamCaptGPU::getKernel()
{
	return kernel;
}
void ParamCaptGPU::setContext(cl_context inputContext)
{
	context = inputContext;
}

void ParamCaptGPU::setQueue(cl_command_queue inputQueue)
{
	queue = inputQueue;
}
void ParamCaptGPU::setKernel(cl_kernel inputKernel)
{
	kernel = inputKernel;
}