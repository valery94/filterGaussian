#include <stdlib.h>
#include "bufferManager.h"

void BufferManager::initBuffer(int iR, int iw, int ih, int iIncrementSize)
{
	frameSize = iw*ih*1.5;
	size = 2 * iR + 2;
	incrementSize = iIncrementSize;
	base = new unsigned char[size*frameSize];
};

BufferManager::~BufferManager()
{
	free(base);
};

void BufferManager::setFrameInBuffer(unsigned char *&pointer)
{
	pointer = base + set*frameSize;
	incrementSetPointer();
	framesInBuffer++;
};

void BufferManager::getFrameSet(unsigned char **&setOfPointers, int n)
{
	if (framesInBuffer == size - 1)
	{
		for (int i = 0; i < n; i++)
		{
			setOfPointers[i] = base + ((get + i) % size)*frameSize;
		}
		incrementGetPointer();
		framesInBuffer--;
	}
};

void BufferManager::incrementGetPointer()
{
	get = (get + incrementSize) % size;
};


void BufferManager::incrementSetPointer()
{
	set = (set + incrementSize) % size;
};