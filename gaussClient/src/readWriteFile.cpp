#include <iostream>

#include "logging.h"
#pragma warning(disable: 4996)
void writeFrame(unsigned char *Y, unsigned char *Cb, unsigned char *Cr, int w, int h, FILE *f)
{
	int offset = w*h*1.5 - w*h;
	fwrite(Y, sizeof(unsigned char)*w*h, 1, f);
	fseek(f, offset, SEEK_CUR);
	//fwrite(Cb, sizeof(unsigned char)*w*h / 4, 1, f);
	//fwrite(Cr, sizeof(unsigned char)*w*h / 4, 1, f);
}

void readFrame(unsigned char *frame, int w, int h, FILE *f)
{
	fread(frame, sizeof(unsigned char)*w*h*1.5, 1, f);
}


char* readSource(const char* kernelPath)
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