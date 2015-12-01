
#ifndef SEGMENT_H
#define SEGMENT_H

#include "Dependencies/FelZenSegment/segment-image.h"
#include "Dependencies/FelZenSegment/pnmfile.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
#define UInt8 char

class segment{
public:
	segment(image<rgb> *im, float sigma, float c, int min_size);
	~segment();
	void init(image<rgb> *im,float sigma, float c, int min_size);
	int* get_initial_box();
	int* get_neighbors();
	int* get_regionSize();
	int* getIndexMap() {
		return indexMap;
	}
	int get_numRegion() {
		return num_Region;
	}

private:
	int width;
	int height;
	int num_Region;
	int* indexMap;
};

segment::segment(image<rgb> *im, float sigma, float c, int min_size) {
	width = im->width();
	height = im->height();
	num_Region=0;
	// cout << num_Region << endl;
	indexMap = new int[width * height];

	init(im, sigma, c, min_size);
}

segment::~segment() {
	delete [] indexMap;
}
void segment::init(image<rgb> *im,float sigma, float c, int min_size) {
	int* num_RegionPtr = &num_Region;
	// cout << "1" << endl;
		// cout << num_Region << endl;

	image<float> *r = new image<float>(width, height);
	image<float> *g = new image<float>(width, height);
	image<float> *b = new image<float>(width, height);

  // smooth each color chaneighborsel  
  	for (int y = 0; y < height; y++) {
    	for (int x = 0; x < width; x++) {
      		imRef(r, x, y) = imRef(im, x, y).r;
      		imRef(g, x, y) = imRef(im, x, y).g;
      		imRef(b, x, y) = imRef(im, x, y).b;
    	}
  	}
  	image<float> *smooth_r = smooth(r, sigma);
  	image<float> *smooth_g = smooth(g, sigma);
  	image<float> *smooth_b = smooth(b, sigma);
  	delete r;
  	delete g;
  	delete b;
	// cout << "2" << endl;
		// cout << num_Region << endl;

 
  // build graph
  	edge *edges = new edge[width*height*4];
  	int num = 0;
  	for (int y = 0; y < height; y++) {
  	  for (int x = 0; x < width; x++) {
  	    if (x < width-1) {
		edges[num].a = y * width + x;
		edges[num].b = y * width + (x+1);
		edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y);
		num++;
  	    }

  	    if (y < height-1) {
		edges[num].a = y * width + x;
		edges[num].b = (y+1) * width + x;
		edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x, y+1);
		num++;
  	    }

  	    if ((x < width-1) && (y < height-1)) {
		edges[num].a = y * width + x;
		edges[num].b = (y+1) * width + (x+1);
		edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y+1);
		num++;
  	    }

    	  if ((x < width-1) && (y > 0)) {
		edges[num].a = y * width + x;
		edges[num].b = (y-1) * width + (x+1);
		edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y-1);
		num++;
    	  }
    	}
  	}
  	delete smooth_r;
  	delete smooth_g;
  	delete smooth_b;
	// cout << "3" << endl;
		// cout << num_Region << endl;

  	// segment
  	universe *u = segment_graph(width*height, num, edges, c);
		// cout << num_Region << endl;
  	
  	// post process small components
  	for (int i = 0; i < num; i++) {
  	  int a = u->find(edges[i].a);
  	  int b = u->find(edges[i].b);
  	  if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
  	    u->join(a, b);
  	}
	// cout << num_Region <<endl;

  	delete [] edges;
	// cout << "4" << endl;
	// cout << num_Region;

  	*num_RegionPtr = u->num_sets();

	// cout << num_Region;
  	//image<rgb> *output = new image<rgb>(width, height);

  	// pick random colors for each component
  	int *colors = new int[width*height];
  	for (int i = 0; i < width*height; i++)
  		colors[i] = 0;
	// cout << "5" << endl;
  	
  	int idx = 1;
	// indexMap = new int[width * height];

  	for (int y = 0; y < height; y++) {
  		for (int x = 0; x < width; x++) {
  	    	int comp = u->find(y * width + x);
  	    	if (!(colors[comp])){
  	    	    colors[comp] = idx;
  	    	    idx = idx + 1;
  	    	}
			// cout<< "6" << endl;
  	    	// cout << colors[comp]<<endl;
  	    	// cout << indexMap[x * height + y] << endl;
	
  	    	//imRef(output, x, y) = colors[comp];
  	    	indexMap[y * width + x] = colors[comp];
  	    	// cout << colors[comp]<<endl;
  		}
  	}  
	// cout << "7" << endl;

  	//mexPrintf("indexMap 0: %f\n", indexMap[0]);
  	//mexPrintf("indexMap 1: %f\n", indexMap[1]);

  	delete [] colors;
  	delete u;
	// cout << "8" << endl;
}
int* segment::get_initial_box() {
	int* box= new int[num_Region*4];

	for(int i = 0; i < num_Region; i++){
		box[4*i+0] = width;     // min x
		box[4*i+1] = height;    // min y
		box[4*i+2] = -1;         // max x
		box[4*i+3] = -1;         // max y
	}
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
  			int mcurr = indexMap[y*width+x] - 1;
  			if(box[4*mcurr+0] > x){
  				box[4*mcurr+0] = x;
  			}
  			if(box[4*mcurr+1] > y){
  			 	box[4*mcurr+1] = y;
  			}
  			if(box[4*mcurr+2] < x){
  			  	box[4*mcurr+2] = x;
  			}
  			if(box[4*mcurr+3] < y){
  			  	box[4*mcurr+3] = y;
  			}
		}
	}
	return box;
}
int* segment::get_neighbors(){
    int* neighbors = new int[num_Region*num_Region];		
	for(int i = 0; i < num_Region*num_Region; i++){
	neighbors[i] = 0;
	}
	int vcurr, hcurr, curr;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			curr = indexMap[y * width + x];       
	      	if(x > 0){
	        	hcurr = indexMap[y * width + x-1];  
	        	neighbors[(hcurr-1)*num_Region+curr-1] = 1;
	        	neighbors[(curr-1)*num_Region+hcurr-1] = 1;
	      	}

	      	if(y > 0){
	        	vcurr = indexMap[(y-1)*width+x];  
	        	neighbors[(vcurr-1)*num_Region+curr-1] = 1;
	        	neighbors[(curr-1)*num_Region+vcurr-1] = 1;
	      	}
	  	}
	}

	for(int i = 0; i < num_Region; i++){
	  	neighbors[i*num_Region+i] = 0;
	}
	return neighbors;	
}

int* segment::get_regionSize(){
	int* sizes = new int[num_Region];
	for (int i=0; i<num_Region; i++) {
		sizes[i]=0;
	}
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++) {
			sizes[indexMap[y*width+x]-1]++;
		}
	}
	return sizes;
}
#endif