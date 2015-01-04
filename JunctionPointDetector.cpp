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
	n = num;
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
	sx = x-wsize;// - (int)(y*0.1);
	sy = y-wsize;// - (int)(y*0.1);
	ex = x+wsize;// + (int)(y*0.1);
	ey = y+wsize;// + (int)(y*0.1);

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
	int nolist[20];
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
		if (flag) {
			nolist[count++] = i;
		}
	}

	flag = 1;
	if (count==2) {
	//-PI < normalaize_diffa < +PI ==> straight line
		diffa = *(input + 7*list[ nolist[0] ]+4) - *(input + 7*list[ nolist[1] ]+4);
		if (diffa<-1.0 || 1.0<diffa)	flag=0;
		if (flag)	return 0;
	}
/*
	else if (count==3) {
		int para=0;
		for (int i=0; i<3; i++) {
			for (int j=0; j<3; j++) {
				diffa = *(input + 7*list[ nolist[i] ]+4) - *(input + 7*list[ nolist[j] ]+4);
				if (diffa<-0.5 || 0.5<diffa)		para++;
			}
		}
		if (para<4)	return 0;
	}
*/
	return count;
}

int JunctionPointDetector::mergeJunction(){
	std::vector<int> merge;
	int num = dst.size()/3;
	int pos0, pos1;
	int x0,y0,x1,y1;
	int x, y, type, c;

	for (int i=0; i<num; i++) {
		pos0 = 3*i;
		x = dst[pos0];
		y = dst[pos0+1];
		type = dst[pos0+2];
		if(type<0)	continue;
		c = 0;
		x0 = dst[pos0] - wsize;
		y0 = dst[pos0+1] - wsize;
		x1 = dst[pos0] + wsize;
		y1 = dst[pos0+1] + wsize;
		for (int j=i+1; j<num; j++) {
			pos1 = 3*j;
			if (x0 < dst[pos1] && dst[pos1] < x1 && y0 < dst[pos1+1] && dst[pos1+1] < y1 && dst[pos1+2] > 0) {
				x += dst[pos1];
				y += dst[pos1+1];
				type = std::max(type, dst[pos1+2]);
				dst[pos1+2] = -1;
				c++;
			}
		}
//		if (!c)	continue;
		std::cout<<"x"<<x<<",c"<<c<<"\n";
		x /= (c+1);
		y /= (c+1);
		merge.push_back(x);
		merge.push_back(y);
		merge.push_back(type);
	}
	dst.swap(merge);
	return 0;
}
/*
int JunctionPointDetector::mergeJunction(){
	int num = dst.size()/3;
	int pos0, pos1;
	int x0,y0,x1,y1;

	for (int i=0; i<num; i++) {
		pos0 = 3*i;
		x0 = dst[pos0] - wsize;
		y0 = dst[pos0+1] - wsize;
		x1 = dst[pos0] + wsize;
		y1 = dst[pos0+1] + wsize;

		for (int j=i+1; j<num; j++) {
			pos1 = 3*j;
			if (x0 < dst[pos1] && dst[pos1] < x1 && y0 < dst[pos1+1] && dst[pos1+1] < y1) {
				if (dst[pos0+2] < dst[pos1+2])	dst[pos0+2] = dst[pos1+2];
				else							dst[pos1+2] = dst[pos0+2];
			}
		}
	}
	return 0;
}
*/
std::vector<int> JunctionPointDetector::JPD(IplImage* img, int w) {
	int x,y,type,num;
	double *ls;
	dst.clear();
	
	if (Ipl2Double(img, map))	return getDst();
	ls = lsd_scale(&num, map, img->width, img->height, 0.8);
	
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
	mergeJunction();
	return getDst();
}
