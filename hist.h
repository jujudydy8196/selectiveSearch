

#ifndef HIST_H
#define HIST_H

#include <iostream>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

#define COLOUR_BINS 25
#define TEXTURE_BINS 10
#define DIRECTION_NUM 8
#define TEXTURE_DIM TEXTURE_BINS*DIRECTION_NUM

template<const int BIN>
class abstractHist{
public:
	abstractHist() {
		for (int i=0; i<BIN; i++) {
			c1[i]=0.0;
			c2[i]=0.0;
			c3[i]=0.0;
		}
	}	
	void set(int colorspaceidx){
		channel = (colorspaceidx>3) ? 1:3;
		colorSpaceIdx=colorspaceidx;	
	}
	double sim(abstractHist<BIN>* CH) {
		double similarity=0.0;
		for (int i=0; i<BIN; i++) {
			similarity += min(CH->c1[i],this->c1[i]);
			similarity += min(CH->c2[i],this->c2[i]);
			similarity += min(CH->c3[i],this->c3[i]);
		}
		return similarity;
	}
	abstractHist<BIN>& operator/(double scale) {
		if(!scale)
			return *this;
		else
			return (*this * (1/scale));
	}
    abstractHist<BIN>& operator=(const abstractHist<BIN>* C){
        for(int i = 0; i < BIN; i++){
            this->c1[i] = C->c1[i];
            this->c2[i] = C->c2[i];
            this->c3[i] = C->c3[i];
        }

        return *this;
    }
    abstractHist<BIN>& operator*(double scale){
        for(int i = 0; i < BIN; i++){
            this->c1[i] *= scale;
            this->c2[i] *= scale;
            this->c3[i] *= scale;
        }

        return *this;
    }    

    friend abstractHist<BIN>& operator+(const abstractHist<BIN>& C1,const abstractHist<BIN>& C2){
    	abstractHist<BIN>* add = new abstractHist<BIN>;
        for(int i = 0; i < BIN; i++){
            add->c1[i] = C1.c1[i] + C2.c1[i];
            add->c2[i] = C1.c2[i] + C2.c2[i];
            add->c3[i] = C1.c3[i] + C2.c3[i];
        }

        return *add;
    }    
	double c1[BIN];
	double c2[BIN];
	double c3[BIN];

	int channel;
	int colorSpaceIdx;



};

class colorHist: public abstractHist<COLOUR_BINS> {
public:
	using abstractHist::operator*;
	// using abstractHist::operator+;
	using abstractHist::operator/;
	using abstractHist::operator=;

};

//rgb struct defined in misc.h
class textureHist: public abstractHist<TEXTURE_DIM> {
public:
	using abstractHist::operator*;
	// using abstractHist::operator+;
	using abstractHist::operator/;
	using abstractHist::operator=;

};

#endif