#pragma warning(disable: 4996)


#include <iostream>
#include <thread>
#include <mutex>
#include <time.h>
#include "gauss.h"
#include "captureFrameCPU.h"
#include "captureFrameGPU.h"
#include "readWriteFile.h"
#include "propertiesCPUGPU.h"
#include "threadPool.h"
#include "filter.h"
#include "logging.h"
#include <condition_variable>
#include "macro.h"
#include "validationCPUGPU.h"
#include "factory.h"
#include "captureFrameInterface.h"



INITIALIZE_EASYLOGGINGPP

int main()
{
	EasyLogging logger;
	logger.initLogger();
	LOG(INFO) << "Start programm";

	CPUGPUProperties cpuGpuParams;
	cpuGpuParams.initProperties();
	
#ifdef USE_CPU
	FILE *f1 = fopen(cpuGpuParams.pathToOtputClipCPU, "wb");
#elif defined(USE_GPU)
	FILE *f2 = fopen(cpuGpuParams.pathToOtputClipGPU, "wb");
#elif defined(TEST_CPU_GPU)
	FILE *f1 = fopen(cpuGpuParams.pathToOtputClipCPU, "wb");
	FILE *f2 = fopen(cpuGpuParams.pathToOtputClipGPU, "wb");
#endif
	CaptureFrameParams captureFrameParams;
	captureFrameParams.initCaptureFrameParams(cpuGpuParams.nFrames,
		cpuGpuParams.width,
		cpuGpuParams.heigth,
		cpuGpuParams.radius);
	ParamCaptGPU *ParamGPU = new ParamCaptGPU();
	
	
	Filter filter;
	IFilter *modelCPU = filter.createFilter(CPUFilterID);
	IFilter *modelGPU = filter.createFilter(GPUFilterID);
	
	bool testGPU = false;

#ifdef USE_CPU
	ICaptureFrame *captFrame = new captureFrameCPU();
	modelCPU->makeInit(cpuGpuParams.weights,
		cpuGpuParams.choosenPlatform,
		cpuGpuParams.pathToKernel,
		cpuGpuParams.radius,
		cpuGpuParams.width,
		cpuGpuParams.heigth,
		cpuGpuParams.widthOfProcBlock,
		cpuGpuParams.heightOfProcBlock,
		ParamGPU);
#endif	
	
#ifdef USE_GPU 
	ICaptureFrame *captFrame = new captureFrameGPU();
	
		LOG(INFO) << "Init GPU";
		modelGPU->makeInit(
			cpuGpuParams.weights,
			cpuGpuParams.choosenPlatform,
			cpuGpuParams.pathToKernel,
			cpuGpuParams.radius,
			cpuGpuParams.width,																   
			cpuGpuParams.heigth,
			cpuGpuParams.widthOfProcBlock,
			cpuGpuParams.heightOfProcBlock,
			ParamGPU);
		
		captFrame->init(ParamGPU);
#endif


#ifdef USE_CPU

		LOG(INFO) << "Start multithreading CPU";
		std::mutex captureFrameMutex;
		std::condition_variable captureFrameCondition;
		std::thread getFrame(callCaptureFrame,
			captFrame,
			std::ref(captureFrameParams), 
			std::ref(captureFrameMutex),
			cpuGpuParams.pathToInputClip,
			std::ref(captureFrameCondition),
			f1);
#endif
#ifdef USE_GPU
		LOG(INFO) << "Start multithreading CPU";
		std::mutex captureFrameMutex;
		std::condition_variable captureFrameCondition;
		std::thread getFrame(callCaptureFrame,
			captFrame,
			std::ref(captureFrameParams),
			std::ref(captureFrameMutex),
			cpuGpuParams.pathToInputClip,
			std::ref(captureFrameCondition),
			f2);

#endif

#ifdef TEST_CPU_GPU
		
		LOG(INFO) << "Test_version_CPU_GPU";
		ICaptureFrame *captFrameCPU = new captureFrameCPU();
		
		modelCPU->makeInit(
			cpuGpuParams.weights,
			cpuGpuParams.choosenPlatform,
			cpuGpuParams.pathToKernel,
			cpuGpuParams.radius,
			cpuGpuParams.width,
			cpuGpuParams.heigth,
			cpuGpuParams.widthOfProcBlock,
			cpuGpuParams.heightOfProcBlock,
			ParamGPU);

		LOG(INFO) << "Start multithreading CPU";
		std::mutex captureFrameMutexCPU;
		std::condition_variable captureFrameConditionCPU;
		std::thread getFrameCPU(callCaptureFrame,
			captFrameCPU,
			std::ref(captureFrameParams),
			std::ref(captureFrameMutexCPU),
			cpuGpuParams.pathToInputClip,
			std::ref(captureFrameConditionCPU),
			f1);

		std::thread makeFilteringCPU(makeGaussianFiltering,
			std::ref(captureFrameParams),
			std::ref(cpuGpuParams),
			std::ref(modelCPU),
			f1, f1, std::ref(modelGPU),
			std::ref(captureFrameMutexCPU),
			std::ref(captureFrameConditionCPU),
			testGPU);

		makeFilteringCPU.join();
		getFrameCPU.join();
		testGPU = true;

		LOG(INFO) << "Init GPU";
		ICaptureFrame *captFrameGPU = new captureFrameGPU();
		modelGPU->makeInit(
			cpuGpuParams.weights,
			cpuGpuParams.choosenPlatform,
			cpuGpuParams.pathToKernel,
			cpuGpuParams.radius,
			cpuGpuParams.width,
			cpuGpuParams.heigth,
			cpuGpuParams.widthOfProcBlock,
			cpuGpuParams.heightOfProcBlock,
			ParamGPU);
		captFrameGPU->init(ParamGPU);

		LOG(INFO) << "Start multithreading GPU";
		std::mutex captureFrameMutexGPU;
		std::condition_variable captureFrameConditionGPU;
		std::thread getFrameGPU(callCaptureFrame,
			captFrameGPU,
			std::ref(captureFrameParams),
			std::ref(captureFrameMutexGPU),
			cpuGpuParams.pathToInputClip,
			std::ref(captureFrameConditionGPU),
			f2);

		captureFrameParams.stop = false;
		captureFrameParams.buffer.framesInBuffer = 0;

		std::thread makeFilteringGPU(makeGaussianFiltering,
			std::ref(captureFrameParams),
			std::ref(cpuGpuParams),
			std::ref(modelCPU),
			f2, f2, std::ref(modelGPU),
			std::ref(captureFrameMutexGPU),
			std::ref(captureFrameConditionGPU),
			testGPU);
		makeFilteringGPU.join();
		getFrameGPU.join();

#endif

#ifdef USE_CPU
		
		std::thread makeFiltering(makeGaussianFiltering, 
			std::ref(captureFrameParams),
			std::ref(cpuGpuParams),
			std::ref(modelCPU),
			f1, f1, std::ref(modelGPU),
			std::ref(captureFrameMutex),
			std::ref(captureFrameCondition),
			testGPU);
		makeFiltering.join();
		getFrame.join();
#endif
#ifdef USE_GPU
		
		std::thread makeFiltering(makeGaussianFiltering, 
			std::ref(captureFrameParams),
			std::ref(cpuGpuParams), 
			std::ref(modelCPU),
			f2, f2, std::ref(modelGPU),
			std::ref(captureFrameMutex),
			std::ref(captureFrameCondition),
			testGPU);
		makeFiltering.join();
		getFrame.join();
#endif

	
#ifdef USE_CPU
		delete modelCPU;
		delete modelGPU;
		fclose(f1);
#elif defined(USE_GPU)
		delete modelCPU;
		delete modelGPU;
		fclose(f2);
#elif defined(TEST_CPU_GPU)
		
		testVersionCPUGPU(f1, f2, cpuGpuParams.width, cpuGpuParams.heigth, cpuGpuParams.nFrames);
		
		delete modelCPU;
		delete modelGPU;
		fclose(f1);
		fclose(f2);
#endif
	system("pause");
	return 0;
}
