#ifndef FACTORY
#define FACTORY
#include "filter.h"

class Filter
{
public:
	static IFilter* createFilter(FilterID id);
	virtual ~Filter(){};
};

#endif