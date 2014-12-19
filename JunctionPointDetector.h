#include "iostream"
#include "math.h"
#include "stdio.h"
#include "vector"
#include "lsd.h"
#include "cv.h"
#include "highgui.h"

class JunctionPointDetector {
private:
	double *input;
	double *ls;
	int map[307200];
	int width;
	int height;
	int tmpw;
	int n;
	std::vector<int> dst;
public:
	JunctionPointDetector();
	~JunctionPointDetector();
	int Ipl2Double(IplImage* , double []);
	int Double2Ipl(IplImage* , double []);
	int setImage(double* out, int X, int Y, int num, int w);
	int setDst(int x, int y, int type);
	std::vector<int> getDst();
	double* getLineSegment(int*);
	int getDirection(int x, int y);
	int detectJunction(int,int,int,int[]);
	std::vector<int> JPD(IplImage*, int);
};

