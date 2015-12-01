#ifndef REGION_H
#define REGION_H

#include "hist.h"

using namespace std;
using namespace cv;


class region{
public:
    region(){

	    box[0] = 0;
	    box[1] = 0;
	    box[2] = 0;
	    box[3] = 0;
	    im_size = 1;
	    region_size=0;
	    regionIdx1=0;
	    regionIdx2=0;
	}
	double sim(region* R, int a[4]){
		double s_color= (a[0])? this->c.sim(&(R->c))/3.0 : 0.0;
		double s_texture= (a[1])? this->t.sim(&(R->t))/(DIRECTION_NUM*3.0) : 0.0;
		double s_size= (a[2])? 1-double(this->region_size+R->region_size)/double(this->im_size) :0.0;
		int rect_minx = min(this->box[0],R->box[0]);
		int rect_miny = min(this->box[1],R->box[1]);
		int rect_maxx = max(this->box[2],R->box[2]);
		int rect_maxy = max(this->box[3],R->box[3]);
		int rect=(rect_maxx - rect_minx) * (rect_maxy-rect_miny);
		double s_fill= (a[3])? 1-double( rect - this->region_size - R->region_size )/double(this->im_size) : 0.0;
		// cout << s_color  << " " << s_texture << " "  << s_size << " " << s_fill << endl;
		return s_color + s_texture + s_size + s_fill;
	}

	void mergeRegion(region* a, region* b,int aIdx, int bIdx) {
		this->region_size = a->region_size + b->region_size;
		this->box[0] = min(a->box[0],b->box[0]);
		this->box[1] = min(a->box[1],b->box[1]);
		this->box[2] = max(a->box[2],b->box[2]);
		this->box[3] = max(a->box[3],b->box[3]);
		this->im_size = a->im_size;

		this->c = (a->c * a->region_size + b->c * b->region_size) / this->region_size;
		this->t = (a->t * a->region_size + b->t * b->region_size) / this->region_size;
		this->regionIdx1=aIdx;
		this->regionIdx2=bIdx;
	}

	int box[4];
	int im_size;
	int region_size;
    colorHist c;
    textureHist t;	
    int regionIdx1;
    int regionIdx2;
    int combinedNum;

};
#endif
