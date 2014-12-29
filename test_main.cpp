#include "stdio.h"
#include "cv.h"
#include "highgui.h"
#include "JunctionPointDetector.h"
#include "DownScale.h"

int main() {
	const int scale = 4;
	int n;
	int position[307200];
	std::vector<int> v;
	IplImage* img = cvLoadImage("image00012.png");
	IplImage* img2 = cvCreateImage(cvSize(img->width/scale, img->height/scale), 8, 1);
	IplImage* img3 = cvCreateImage(cvSize(img->width/scale, img->height/scale), 8, 1);
	JunctionPointDetector jpd;
	DownScale ds;

	ds.setImage(img, scale);
	ds.downscale(img2, position, 180);

	cvNamedWindow("dst");
	cvShowImage("dst", img2);
	cv::waitKey();

	v = jpd.JPD(img2, 5);

	for (int i=0; i<v.size()/3; i++) {
		if ( position[2*(v[3*i]+img2->width*v[3*i+1])]<0 || position[2*(v[3*i]+img2->width*v[3*i+1])+1]<0)	continue;
		printf("%d,%d,%d\n", v[3*i], v[3*i+1], v[3*i+2]);
		printf("%d,%d,%d\n", position[2*(v[3*i]+img2->width*v[3*i+1])], position[2*(v[3*i]+img2->width*v[3*i+1])+1], v[3*i+2]);
	}

	for (int i=0; i<v.size()/3; i++)	img3->imageData[v[3*i]+img3->width*v[3*i+1]] = 255;

	cvNamedWindow("dst2");
	cvShowImage("dst2", img3);
	cv::waitKey();
	return 0;
}

