#include "macro.h"
#include "captureFrameInterface.h"
#include "readWriteFile.h"
#include "propertiesCPUGPU.h"
#include "validationCPUGPU.h"
#include "logging.h"
#include "filter.h"
#include <condition_variable>

void makeGaussianFiltering(CaptureFrameParams &captureFrameParams, CPUGPUProperties &cpuGpuParams, IFilter *modelCPU, FILE *fileCpu,
	FILE *fileGpu, IFilter *modelGPU, std::mutex &captureFrameMutex, std::condition_variable &captureFrameCondition, bool testGPU)
{
	int currFrame = 0;
	int sizeY = cpuGpuParams.width*cpuGpuParams.heigth;
	while (!captureFrameParams.stop)
	{
		std::unique_lock<std::mutex> lock(captureFrameMutex);
		cpuGpuParams.processingTimePerFrame = clock();
		if (captureFrameParams.buffer.framesInBuffer == 2 * cpuGpuParams.radius + 1)
		{
			captureFrameParams.buffer.getFrameSet(cpuGpuParams.frameSet, 2 * cpuGpuParams.radius + 1);

#ifdef USE_CPU

			modelCPU->filtering(cpuGpuParams.frameSet,
				cpuGpuParams.YProcCPU,
				cpuGpuParams.YProcGPU
				);
			writeFrame(
				cpuGpuParams.YProcCPU,
				cpuGpuParams.frameSet[cpuGpuParams.radius] + sizeY,
				cpuGpuParams.frameSet[cpuGpuParams.radius] + 5*sizeY/4,
				cpuGpuParams.width,
				cpuGpuParams.heigth,
				fileCpu);

#elif defined(USE_GPU)

			modelGPU->filtering(cpuGpuParams.frameSet,
				cpuGpuParams.YProcCPU,
				cpuGpuParams.YProcGPU
				);

			writeFrame(
				cpuGpuParams.YProcGPU,
				cpuGpuParams.frameSet[cpuGpuParams.radius] + sizeY,
				cpuGpuParams.frameSet[cpuGpuParams.radius] + 5 * sizeY / 4,
				cpuGpuParams.width,
				cpuGpuParams.heigth,
				fileGpu);

#elif defined(TEST_CPU_GPU)
			if (!testGPU)
			{

				modelCPU->filtering(cpuGpuParams.frameSet,
					cpuGpuParams.YProcCPU,
					cpuGpuParams.YProcGPU
					);

				writeFrame(
					cpuGpuParams.YProcCPU,
					cpuGpuParams.frameSet[cpuGpuParams.radius] + sizeY,
					cpuGpuParams.frameSet[cpuGpuParams.radius] + 5 * sizeY / 4,
					cpuGpuParams.width,
					cpuGpuParams.heigth,
					fileCpu);
			}

			if (testGPU)
			{

				modelGPU->filtering(cpuGpuParams.frameSet,
					cpuGpuParams.YProcCPU,
					cpuGpuParams.YProcGPU
					);

				writeFrame(
					cpuGpuParams.YProcGPU,
					cpuGpuParams.frameSet[cpuGpuParams.radius] + sizeY,
					cpuGpuParams.frameSet[cpuGpuParams.radius] + 5 * sizeY / 4,
					cpuGpuParams.width,
					cpuGpuParams.heigth,
					fileGpu);
			}

#endif

			currFrame++;
			cpuGpuParams.averageTime += float(clock() - cpuGpuParams.processingTimePerFrame) / CLOCKS_PER_SEC;
			LOG(INFO) << "  ProcessTime=" << double(clock() - cpuGpuParams.processingTimePerFrame) / CLOCKS_PER_SEC << " s.";
			std::cout << "  ProcessTime=" << double(clock() - cpuGpuParams.processingTimePerFrame) / CLOCKS_PER_SEC << " s." << std::endl;
		}
		captureFrameCondition.notify_one();
	}
	LOG(INFO) << "End multithreading";
	LOG(INFO) << "Average Processing Time = " << cpuGpuParams.averageTime / currFrame;
	LOG(INFO) << "Processed Frames = " << currFrame;
	std::cout << "\nAverage Processing Time = " << cpuGpuParams.averageTime / currFrame << std::endl;
	std::cout << "\nProcessed Frames = " << currFrame << std::endl;
}															 