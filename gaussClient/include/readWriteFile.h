#ifndef S_READ_WRITE_H
#define S_READ_WRITE_H
#include <iostream>

/*Write frame in some .yuv file, using format is 4:2:0
@param  Y, Cb and Cr are the components of frame
@param  w is width of frame
@param h is height of frame
@param f is file
*/
void writeFrame(unsigned char *Y, unsigned char *Cb, unsigned char *Cr, int w, int h, FILE *f);

/*Read frame from some .yuv file, using format is 4:2:0
@param  Y, Cb and Cr are the components of frame
@param  w is width of frame
@param h is height of frame
@param f is file*/
void readFrame(unsigned char *frame, int w, int h, FILE *f);

/*Read the kernel file from some source*/
char* readSource(const char* kernelPath);

#endif
