#include "Dependencies/FelZenSegment/image.h"
#include "Dependencies/FelZenSegment/misc.h"
#include "Dependencies/FelZenSegment/pnmfile.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

image<rgb>* matToImage(Mat& I){
	int channel = I.channels();
	if(channel < 3){
		fprintf(stderr,"Image type error. Please use  3 channels image(rgb)....");
		exit(1);
	}

	int width = I.cols;
	int height = I.rows;

	image<rgb>* img = new image<rgb>(width,height);

	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			uchar* p = I.ptr<uchar>(y);
			imRef(img,x,y).b = p[x*channel+0];
			imRef(img,x,y).g = p[x*channel+1];
			imRef(img,x,y).r = p[x*channel+2];
		}
	}

	return img;
}

Mat* imageToMat(image<rgb>* img){
	int width = img->width();
	int height = img->height();

	Mat* I = new Mat(Size(width,height),CV_8UC3);
	int channel = I->channels();
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			uchar* p = I->ptr<uchar>(y);
			p[x*channel+0] = imRef(img,x,y).b;
			p[x*channel+1] = imRef(img,x,y).g;
			p[x*channel+2] = imRef(img,x,y).r;
		}
	}

	return I;
}