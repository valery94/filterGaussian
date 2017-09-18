#include "filter.h"
#include "gaussCPU.h"
#include "gaussGPU.h"
#include "factory.h"

IFilter* Filter::createFilter(FilterID id)
{
	IFilter* f = NULL;
	switch (id)
	{
	case CPUFilterID:
		f = new ModelCPU();
		break;
	case GPUFilterID:
		f = new ModelGPU();
		break;
	default:
		f = NULL;
	}
	return f;
};