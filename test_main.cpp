#include "stdio.h"
#include "cv.h"
#include "highgui.h"
#include "JunctionPointDetector.h"
#include "DownScale.h"

int main() {
	const int scale = 4;
	int n;
	std::vector<int> v;
	IplImage* img = cvLoadImage("image00014.png");
	IplImage* img2 = cvCreateImage(cvSize(img->width/scale, img->height/scale), 8, 1);
//	IplImage* img3 = cvCreateImage(cvSize(img->width/scale, img->height/scale), 8, 1);
	cv::Mat result;
	result.create(img->height/scale, img->width/scale, CV_8UC3);
	JunctionPointDetector jpd;
	DownScale ds;

	ds.setImage(img, scale);
	ds.downscale(img2, 180);

	cvNamedWindow("dst");
	cvShowImage("dst", img2);
	cv::waitKey();

	v = jpd.JPD(img2, 5);

	printf("num: %d, ",(int)(v.size()/3));
	for (int i=0; i<v.size()/3; i++)	if (v[3*i+2]>0)	printf("%d: %d,%d,%d\n", i, v[3*i]*scale, v[3*i+1]*scale, v[3*i+2]);

	int num;
	double* out;
	out = jpd.getLineSegment(&num);
	for (int i=0; i<num; i++) {
		cv::line(result, cv::Point( (int)*(out +7*i+0) , (int)*(out +7*i+1) ),cv::Point((int)*(out +7*i+2),(int)*(out +7*i+3)),cv::Scalar( 0, 0, 255 ),1,0);
//		img3->imageData[v[3*i]+img3->width*v[3*i+1]] = 255;
	}
	cv::namedWindow("result");
	cv::imshow("result", result);
//	cvNamedWindow("dst2");
//	cvShowImage("dst2", img3);
	cv::waitKey();
	return 0;
}

