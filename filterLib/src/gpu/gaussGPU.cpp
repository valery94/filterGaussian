#pragma warning(disable: 4996)
#include "gaussGPU.h"


//private
void ModelGPU::checkErr(int err, const char* err_code)
{
	if (err)
	{
		LOG(ERROR) << "Error: " << err_code << "(" << err << ")";
		std::cout << "press enter to continue..." << std::endl;
		getchar();
	}
}
unsigned int ModelGPU::roundUp(	unsigned int value,	unsigned int multiple)
{
	// Determine how far past the nearest multiple the value is
	unsigned int remainder = value % multiple;
	// Add the difference to make the value a multiple
	if (remainder != 0) {
		value += (multiple - remainder);
	}
	return value;
}

char* ModelGPU::readKernel(const char* kernelPath)
{

	int status;
	FILE *fp;
	char *source;
	long int size;

	fp = fopen(kernelPath, "rb");
	if (!fp) {
		LOG(ERROR) << "Could not open kernel file";
		printf("Could not open kernel file\n");
		printf("press Enter to continue...");
		getchar();
		exit(-1);
	}
	status = fseek(fp, 0, SEEK_END);
	if (status != 0) {
		LOG(ERROR) << "Error seeking to end of file";
		printf("Error seeking to end of file\n");
		printf("press Enter to continue...");
		getchar();
		exit(-1);
	}
	size = ftell(fp);
	if (size < 0) {
		LOG(ERROR) << "Error getting file position";
		printf("Error getting file position\n");
		printf("press Enter to continue...");
		getchar();
		exit(-1);
	}

	rewind(fp);

	source = (char *)malloc(size + 1);

	int i;
	for (i = 0; i < size + 1; i++) {
		source[i] = '\0';
	}

	if (source == NULL) {
		printf("Error allocating space for the kernel source\n");
		printf("press Enter to continue...");
		getchar();
		exit(-1);
	}

	fread(source, 1, size, fp);
	source[size] = '\0';

	return source;
}
//constructor and destructor
ModelGPU::ModelGPU()
{
}
ModelGPU::~ModelGPU()
{
}

//public
void ModelGPU::makeInit(
	float *fWeight, 
	int	choosenPlatform, 
	char* passToKernel,
	int iRadius,
	int iWidth, 
	int iHeight, 
	int sizeOfProcesedBlockX, 
	int sizeOfProcesedBlockY,
	ParamCaptGPU *ParamGPU)
{
	height = iHeight;
	width = iWidth;
	filtrRadius = iRadius;
	sizeOfBlockX = sizeOfProcesedBlockX;
	sizeOfBlockY = sizeOfProcesedBlockY;

	LOG(INFO) << "Start OpenCL init";

	wgWidth = sizeOfBlockX;
	wgHeight = sizeOfBlockY;

	int iSizeOfFilterBufer = (2 * filtrRadius + 1)*(2 * filtrRadius + 1)*(2 * filtrRadius + 1);
	filterWidth = (2 * filtrRadius + 1);
	paddingPixels = (int)(filterWidth / 2) * 2;
	errCode = clGetPlatformIDs(NULL, NULL, &iNumOfPlatforms);
	checkErr(errCode, "clGetPlatformIDs");

	// выделяем память под каждую платформу
	platform = new cl_platform_id[iNumOfPlatforms];
	// считываем ID всех платформ
	errCode = clGetPlatformIDs(iNumOfPlatforms, platform, NULL);
	checkErr(errCode, "clGetPlatformIDs");

	errCode = clGetDeviceIDs(platform[choosenPlatform], CL_DEVICE_TYPE_DEFAULT, 1, &deviceId, &numDevice);
	checkErr(errCode, "clGetDeviceIDs");

	char *deviceName = new char [1024];
	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, sizeof(char)*1024, deviceName, NULL);
	LOG(INFO) << "CL_DEVICE_NAME:" << deviceName;
	printf("CL_DEVICE_NAME: %s\n", deviceName);
	delete[] deviceName;
	
	size_t buf_int;
	errCode = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_int), &buf_int, NULL);
	//printf("CL_DEVICE_MAX_WORK_GROUP_SIZE: %d\n", buf_int);

	cl_uint CompUnits;
	errCode = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(CompUnits), &CompUnits, NULL);
	//printf("CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", buf_int);

	props[0] = CL_CONTEXT_PLATFORM;
	props[1] = (cl_context_properties)(platform[choosenPlatform]);
	props[2] = 0;

	context = clCreateContext(props, 1, &deviceId, NULL, NULL, &errCode);

	checkErr(errCode, "clCreateContext");
	ParamGPU->setContext(context);
	// Create command queue
	queue = clCreateCommandQueue(context, deviceId, 0, &errCode);
	checkErr(errCode, "clCreateCommandQueue");
	ParamGPU->setQueue(queue);
	// Create memory buffers
	LOG(INFO) << "Creating buffers for input and output image";
	filter = clCreateBuffer(context, CL_MEM_READ_ONLY, iSizeOfFilterBufer * sizeof(float), NULL, &errCode);
	
	outputImage = clCreateBuffer(context, CL_MEM_READ_WRITE, width * height * sizeof(unsigned char), NULL, &errCode);
	unsigned char fiiling = 0;
	errCode = clEnqueueFillBuffer(queue, outputImage, &fiiling, sizeof(unsigned char), 0, width * height * sizeof(unsigned char), 0, NULL, NULL);
	checkErr(errCode, "clCreateBuffer");

	errCode = clEnqueueWriteBuffer(queue, filter, CL_TRUE, 0, iSizeOfFilterBufer*sizeof(float), fWeight, 0, NULL, NULL);
	checkErr(errCode, "clEnqueueWriteBuffer");
	
	source = readKernel(passToKernel);
	program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &errCode);
	checkErr(errCode, "clCreateProgramWithSource");
	LOG(INFO) << "Start build program";
	errCode = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);

	checkErr(errCode, "clBuildProgram");
	if (errCode != 0)
	{
		printf("clBulidProgram Fail....\n");

		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *)malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s\n", log);
	}

	kernel = clCreateKernel(program, "Gaus_3D", &errCode);
	checkErr(errCode, "clCreateKernel");
	ParamGPU->setKernel(kernel);

	int totalWorkItemsX = roundUp(width - paddingPixels, wgWidth);
	int totalWorkItemsY = roundUp(height - paddingPixels, wgHeight);

	localSize[0] = wgWidth;
	localSize[1] = wgHeight;
	
	//localSize[0] = 16;
	//localSize[1] = 16;

	localWidth = int(localSize[0] + paddingPixels);
	localHeight = int(localSize[1] + paddingPixels);

	// Size of the NDRange
	globalSize[0] = totalWorkItemsX;
	globalSize[1] = totalWorkItemsY;

	localMemSize = (localWidth * localHeight * filterWidth *sizeof(float));

	errCode = clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputImage);
	errCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), &filter);
	errCode = clSetKernelArg(kernel, 3, sizeof(int), &height);
	errCode = clSetKernelArg(kernel, 4, sizeof(int), &width);
	errCode = clSetKernelArg(kernel, 5, sizeof(int), &filterWidth);
	errCode = clSetKernelArg(kernel, 6, localMemSize, NULL);
	errCode = clSetKernelArg(kernel, 7, sizeof(int), &localHeight);
	errCode = clSetKernelArg(kernel, 8, sizeof(int), &localWidth);
	checkErr(errCode, "clSetKernelArg");
	LOG(INFO) << "End of GPU init";
}

void ModelGPU::filtering(unsigned char **videoSequenceY, unsigned char *YProcCPU, unsigned char *filterFrameY)
{
	int sizeY = width*height;
	int filterSideSize = 2 * filtrRadius + 1;
	errCode = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
	checkErr(errCode, "clEnqueueNDRangeKernel");

	errCode = clEnqueueReadBuffer(
		queue,
		outputImage,
		CL_TRUE,
		0,
		sizeY*sizeof(unsigned char),
		filterFrameY, 0, NULL, NULL);
	checkErr(errCode, "clEnqueueReadBuffer");

	frameCounter++;
}

/*void ModelGPU::releaseObj()
{
	
	delete[] platform;
	delete[] source;
	clReleaseMemObject(inputImage);
	clReleaseMemObject(outputImage);
	clReleaseMemObject(filter);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
}*/
