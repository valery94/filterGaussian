#ifndef BUFFER_MANAGER
#define BUFFER_MANAGER


using namespace std;
class BufferManager
{
private:
	unsigned char *base;
	int size;
	int frameSize;
	int set = 0;
	int get = 0;
	int incrementSize;

	//@brief Increases the index of pointer "set"
	void incrementSetPointer();

	//@brief Increases the index of pointer "get"
	void incrementGetPointer();


public:
	int framesInBuffer = 0;

	//@brief Return dinamic array of pointers,which points on ordered frames
	//@param setOfPointers - array of pointers
	//@param n - size of setOfPointers
	void getFrameSet(unsigned char **&setOfPointers, int n);

	//@brief Return pointer,which points on free space in memory,
	//where  next frame is to be written
	//@param pointer - pointer on this space
	void setFrameInBuffer(unsigned char *&pointer);

	void initBuffer(int iR, int iw, int ih, int iIncrementSize);

	~BufferManager();
};

#endif