#ifndef TEST_VERSION_CPU_GPU_H
#define TEST_VERSION_CPU_GPU_H

/** @brief . The function to test the differences of the two filtered frames computing în the GPU and CPU.
@param _CPU_frame The filtered frame on CPU.
@param _GPU_frame The filtered frame on GPU.
@param w Width of the frame.
@param h Height of the frame.
*/
void testVersionCPUGPU(FILE *f1, FILE *f2, int w, int h, int nFrames);

/*@brief . The function return error code (OpenCL)*/
void checkErr(int err, char* err_code);

#endif
