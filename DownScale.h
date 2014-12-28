#include "stdio.h"
#include "vector"
#include "cv.h"
#include "highgui.h"

class DownScale{
private:
	IplImage* img;
	std::vector<int> originPos;
	int scale;
public:
	DownScale();
	~DownScale();
	int setImage(IplImage* in, int scale);
	int downscale(IplImage* out, unsigned char color);
	int downscale(IplImage* out, int position[], unsigned char color);
	int downscale(IplImage* out, IplImage* colormap, unsigned short color);
	int downscale(IplImage* out, int position[], IplImage* colormap, unsigned short color);
};
