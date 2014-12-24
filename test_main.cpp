#include "stdio.h"
#include "cv.h"
#include "highgui.h"
#include "JunctionPointDetector.h"
#include "DownScale.h"

int main() {
	const int scale = 2;
	int n;
	std::vector<int> v;
	IplImage* img = cvLoadImage("image00005.png",0);
	IplImage* img2 = cvCreateImage(cvSize(img->width/scale, img->height/scale), 8, 1);
	JunctionPointDetector jpd;
	DownScale ds;

	ds.setImage(img, scale);
	ds.downscale(img2, 180);

	cvNamedWindow("dst");
	cvShowImage("dst", img2);
	cv::waitKey();

	v = jpd.JPD(img, 5);

	for (int i=0; i<v.size()/3; i++)	printf("%d, %d, %d\n", v[3*i], v[3*i+1], v[3*i+2]);

	cv::waitKey();
	return 0;
}

