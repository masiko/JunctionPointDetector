#include "JunctionPointDetector.h"


JunctionPointDetector::JunctionPointDetector(){
	for (int i=0; i<307200; i++) {
		map[i] = -1.0;
	}
}

JunctionPointDetector::~JunctionPointDetector(){
}

int JunctionPointDetector::Ipl2Double(IplImage *in, double out[]) {
	int y;
	unsigned char value;

	if (in->width*in->height > 307200)	return 1;

	for (int i=0; i<in->height; i++) {
		y = i*in->width;
		for (int j=0; j<in->width; j++) {
			value = (unsigned char)in->imageData[j+y];
			out[j+y] = (double)value;
		}
	}
	return 0;
}

int JunctionPointDetector::Double2Ipl(IplImage *out, double in[]) {
	int y;
	for (int i=0; i<out->height; i++) {
		y = i*out->width;
		for (int j=0; j<out->width; j++) {
			out->imageData[j+y] = ((unsigned char)in[j+y]);
		}
	}
	return 0;
}

int JunctionPointDetector::setImage(double* in, int X, int Y, int num, int w) {
	int diff;
	int px, py;
	double ang;
	double x1,y1,x2,y2;
	
	if(X*Y > 307200)	return 1;
	input = in;
	width = X;
	height = Y;
	wsize = w;
	for (int i=0; i<num; i++) {
		x1 =  *(input+7*i);
		y1 =  *(input+7*i+1);
		x2 =  *(input+7*i+2);
		y2 =  *(input+7*i+3);
		diff = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
		ang = atan((y2-y1)/(x2-x1));
		*(in+7*i+4) = ang;
//		std::cout<<"["<<x1<<","<<y1<<","<<x2<<","<<y2<<"]";
		for (int j=0; j<=diff; j++) {
			px = j*cos(ang) + x1;
			py = j*sin(ang) + y1;
			px = std::max(0, std::min(639, px));
			py = std::max(0, std::min(479, py));
			map[px + py*width] = i;
		}
	}

	return 0;
}

int JunctionPointDetector::setDst(int x, int y, int type) {
	if(type<=1 || 4<type)	return 1;
	dst.push_back(x);
	dst.push_back(y);
	dst.push_back(type);
	return 0;
}

std::vector<int> JunctionPointDetector::getDst() {
	return dst;
}

double* JunctionPointDetector::getLineSegment(int *num){
	*num = n;
	return input;
}

int JunctionPointDetector::process(int x, int y) {
	int sx,sy,ex,ey;
	int lines[20];
	int pos;
	int count=0;
	int flag=1;
	double diff;
	
//	std::cout<<x<<","<<y<<"/";
	sx = x-wsize;
	sy = y-wsize;
	ex = x+wsize;
	ey = y+wsize;

//	if (sx<0 || width-1<ex || sy<0 || height-1<ey)	return -1;
	sx = std::max(0, std::min(width-1, sx));
	sy = std::max(0, std::min(height-1, sy));
	ex = std::max(0, std::min(width-1, ex));
	ey = std::max(0, std::min(height-1, ey));
	for (int i=sy; i<ey; i++) {
		for (int j=sx; j<ex; j++) {
			pos = map[i*width+j];
			if (pos>=0) {
				flag=1;
				for(int k=0; k<count; k++) {
					if (lines[k]==pos)	flag=0;
				}
				if (flag){
					lines[count] = pos;
//	std::cout<<"["<<count<<"]:"<<lines[count]<<"/";
					if (++count>18){
						return detectJunction(x,y,count,lines);
					}
				}
			}
		}
	}

	return detectJunction(x,y,count,lines);
}

int JunctionPointDetector::detectJunction(int x, int y, int num, int list[20]) {
	double diffa, diff1, diff2;
	double ang[20];
	double dis[20];
	int count=0;
	int flag=1;
	
	for (int i=0; i<num; i++) {
		diff1=fabs(*(input + 7*list[i]) - x) + fabs(*(input + 7*list[i]+1) - y);
		diff2=fabs(*(input + 7*list[i]+2) - x) + fabs(*(input + 7*list[i]+3) - y);

		if (diff1<diff2)	ang[i] = atan2(*(input + 7*list[i]+3)-*(input + 7*list[i]+1), *(input + 7*list[i]+2)-*(input + 7*list[i])),
							dis[i] = diff2;
		else				ang[i] = atan2(*(input + 7*list[i]+1)-*(input + 7*list[i]+3), *(input + 7*list[i])-*(input + 7*list[i]+2)),
							dis[i] = diff1;

//		std::cout<<"|"<<diff1<<","<<diff2<<","<<ang[i]<<"|";
	}

	for (int i=0; i<num; i++) {
		flag=1;
		for (int j=i+1; j<num; j++) {
			diffa = ang[i] - ang[j];
			diffa = diffa - 2*3.1415*(int)(diffa/3.1415);
			if (-0.5<diffa && diffa<0.5)	flag=0;
		}
		if (flag)	count++;
	}

	if(count==2) {
		flag=1;
		for (int i=0; i<num; i++) {
			for (int j=i+1; j<num; j++) {
				diffa = *(input + 7*list[i]+4) - *(input + 7*list[j]+4);
				if (diffa<-0.5 || 0.5<diffa)	flag=0;
			}
		}
		if (flag)	count=0;
	}
/*	else if(count==3) {
		double dism=1;
		double disp=0;
		double p;
		for (int i=0; i<num; i++) {
			for (int j=i+1; j<num; j++) {
				diffa = *(input + 7*list[i]+4) - *(input + 7*list[j]+4);
				if (-0.5<diffa && diffa<0.5) {
					if(ang[i]>0)	disp = std::max(disp, dis[i]);
					else			dism = std::max(dism, dis[i]);
				}
			}
		}
		p = disp/dism;
	//	if (p<0.9 || 1.1 <p)	count=2;
	}
*/

//	std::cout<<num<<","<<count<<"/\n";
	return count;
	
}

std::vector<int> JunctionPointDetector::JPD(IplImage* img, int w) {
	int x,y,type,num;
	double *ls;
	dst.clear();
	
	if (Ipl2Double(img, map))	return getDst();
	ls = lsd(&num, map, img->width, img->height);
	
	for (int i=0; i<307200; i++) {
		map[i] = -1.0;
	}
	
	setImage(ls, img->width, img->height, num, w);

	for (int i=0; i<num; i++) {
		x = (int)*(input + 7*i);
		y = (int)*(input + 7*i+1);
		type = process(x,y);
		setDst(x,y,type);
		x = (int)*(input + 7*i+2);
		y = (int)*(input + 7*i+3);
		type = process(x,y);
		setDst(x,y,type);
	}
	return getDst();
}
