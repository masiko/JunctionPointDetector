#include "DownScale.h"

DownScale::DownScale(){};

DownScale::~DownScale(){};

int DownScale::setImage(IplImage* in, int s) {
	int col, row;
	scale = s;
	col = in->width;
	row = in->height;

	if (col==0 && row==0)	return 1;
	img = cvCreateImage(cvSize(col, row), 8, 1);
	
	cvCvtColor(in, img, CV_BGR2GRAY);
	
	return 0;
}

int DownScale::downscale(IplImage* out, unsigned char color) {
	int col, row;
	int x,y;
	int flag=0;
	int pos=0;
	col = out->width;;
	row = out->height;

	if (col != img->width/scale || row != img->height/scale)	return 1;

	for (int i=0; i<row; i++) {
		y = i*scale*img->width;
		for (int j=0; j<col; j++) {
			x = j*scale;
			flag=0;
			for(int k=0; k<scale; k++) {
				for (int l=0; l<scale; l++) {
					if ( (unsigned char)img->imageData[x+y+l+k*img->width] > color) {
						flag=1;
						pos =x+y+l+k*img->width;
						break;
					}
				}
				if (flag)	break;
			}
			out->imageData[j+i*col] = flag ? (unsigned char)img->imageData[pos] : 0;
		}
	}
	return 0;
}

int DownScale::downscale(IplImage* out, int position[], unsigned char color) {
	int col, row;
	int x,y;
	int flag=0;
	int posx=0;
	int posy=0;
	col = out->width;;
	row = out->height;

	if (col != img->width/scale || row != img->height/scale)	return 1;

	for (int i=0; i<row; i++) {
		y = i*scale*img->width;
		for (int j=0; j<col; j++) {
			x = j*scale;
			flag=0;
			for(int k=0; k<scale; k++) {
				for (int l=0; l<scale; l++) {
					if ( (unsigned char)img->imageData[x+y+l+k*img->width] > color) {
						flag=1;
						posx = x+l;
						posy = y+k*img->width;
						break;
					}
				}
				if (flag)	break;
			}

			if (flag) {
				out->imageData[j+i*col] = (unsigned char)img->imageData[posx+posy];
				position[2*(j+i*col)] = posx;
				position[2*(j+i*col)+1] = posy;				
			}else{
				out->imageData[j+i*col] = 0;
				position[2*(j+i*col)] = -1;
				position[2*(j+i*col)+1] = -1;
			}
		}
	}
	return 0;
}

int DownScale::downscale(IplImage* out, IplImage* colormap, unsigned short color) {
	unsigned short* p = (unsigned short*)colormap->imageData;
	int col, row;
	int x,y;
	int flag=0;
	int pos=0;
	col = out->width;;
	row = out->height;

	if (col != img->width/scale || row != img->height/scale)	return 1;

	for (int i=0; i<row; i++) {
		y = i*scale*colormap->width;
		for (int j=0; j<col; j++) {
			x = j*scale;
			flag=0;
			for(int k=0; k<scale; k++) {
				for (int l=0; l<scale; l++) {
					if ( *(p + x+y+l+k*colormap->width) == color) {
						flag=1;
						pos =x+y+l+k*img->width;
						break;
					}
				}
				if (flag)	break;
			}
			out->imageData[j+i*col] = flag ? (unsigned char)img->imageData[pos] : 0;
		}
	}
	return 0;
}

int DownScale::downscale(IplImage* out, int position[], IplImage* colormap, unsigned short color) {
	unsigned short* p = (unsigned short*)colormap->imageData;
	int col, row;
	int x,y;
	int flag=0;
	int posx=0;
	int posy=0;
	col = out->width;;
	row = out->height;

	if (col != img->width/scale || row != img->height/scale)	return 1;

	for (int i=0; i<row; i++) {
		y = i*scale*colormap->width;
		for (int j=0; j<col; j++) {
			x = j*scale;
			flag=0;
			for(int k=0; k<scale; k++) {
				for (int l=0; l<scale; l++) {
					if ( *(p + x+y+l+k*colormap->width) == color) {
						flag=1;
						posx = x+l;
						posy = y+k*img->width;
						break;
					}
				}
				if (flag)	break;
			}
			if (flag) {
				out->imageData[j+i*col] = (unsigned char)img->imageData[posx+posy];
				position[2*(j+i*col)] = posx;
				position[2*(j+i*col)] = posy;				
			}else{
				out->imageData[j+i*col] = 0;
				position[2*(j+i*col)] = -1;
				position[2*(j+i*col)] = -1;
			}
		}
	}
	return 0;
}


