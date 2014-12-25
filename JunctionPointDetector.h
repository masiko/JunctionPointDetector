#include "iostream"
#include "math.h"
#include "stdio.h"
#include "vector"
#include "lsd.h"
#include "cv.h"
#include "highgui.h"

class JunctionPointDetector {
private:
	double *input;			//LSD output
	double map[307200];		//LSD input image / Line-ID map
	int width;
	int height;
	int wsize;				//Window size
	int n;
	std::vector<int> dst;	//Output(x,y,type)
public:
	JunctionPointDetector();
	~JunctionPointDetector();
	int Ipl2Double(IplImage* , double []);
	int Double2Ipl(IplImage* , double []);
	int setImage(double* out, int X, int Y, int num, int w);
	int setDst(int x, int y, int type);
	std::vector<int> getDst();
	double* getLineSegment(int* num);
	int process(int x, int y);
	int detectJunction(int x,int y,int num, int list[]);
	std::vector<int> JPD(IplImage* img, int w);
};

