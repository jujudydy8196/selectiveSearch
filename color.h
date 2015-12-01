#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

//rgb struct defined in misc.h
class colorHistaa{
public:
	colorHistaa() {
		for (int i=0; i<25; i++) {
			c1[i]=0.0;
			c2[i]=0.0;
			c3[i]=0.0;
		}
	}
	void set(int colorspaceidx){
		channel = (colorspaceidx>3) ? 1:3;
		colorSpaceIdx=colorspaceidx;	
	}
	double sim_color(colorHist CH) {
		double sim=0.0;
		for (int i=0; i<25; i++) {
			sim += min(CH.c1[i],this->c1[i]);
			sim += min(CH.c2[i],this->c2[i]);
			sim += min(CH.c3[i],this->c3[i]);
		}
		return sim;
	}


	int channel;
	int colorSpaceIdx;
	// vector<double> c;
	double c1[25];
	double c2[25];
	double c3[25];
};

#endif