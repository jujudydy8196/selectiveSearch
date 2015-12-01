#ifndef GROUPING_H
#define GROUPING_H

// #include "segment_noclass.h"
#include "segment.h"
#include "region.h"
#include "computeHistogram.h"
// #include "hist.h"

#include <iostream>

using namespace std;

region* hierarchicalGrouping(image<rgb> *im, double sigma, double k, double min_size, int colorSpaceIdx, int &num_region, string imgName, int classEvaluate){
	int width = im->width();
    int height = im->height();
    // cout <<"width: " <<width << " height: " << height << endl; 

    // cout << "start segment" << endl;
    segment* Seg= new segment(im, sigma, k, min_size);
    // cout << "init segemnt" << endl;
    int* indexmap = Seg->getIndexMap(); //size: width * height
    // cout << "get Indexmap!" << endl;
    int* box = Seg->get_initial_box();
    // neighbouring blobs:
    int* neighbors = Seg->get_neighbors();
    num_region = Seg->get_numRegion();
    int* regionSizes = Seg->get_regionSize();
    // cout << " num region : " << num_region << endl;
    colorHist* colorHists= get_colorHists(indexmap,num_region,colorSpaceIdx,im);
    // cout << colorHists[4].c1[23] <<" " << colorHists[9].c1[25] << endl;
    textureHist* textureHists = get_textureHists(indexmap,num_region,colorSpaceIdx,im);
    // cout << textureHists[100].c1[50] <<" " << colorHists[50].c3[20] << endl;
    int a[4]={1,1,1,1};
    string s;
    const char * ch ;



 	// int num_combinedRegions = 2*num_region -1;
 	int num_combinedRegions = 2*num_region -1;
	region* combinedRegions= new region[num_combinedRegions];
	int* combinedNeighbors = new int[num_combinedRegions*num_combinedRegions];
    double* region_sim = new double[num_combinedRegions*num_combinedRegions];

    rgb* colors = new rgb[num_combinedRegions];
    image<rgb> *output = new image<rgb>(width, height);
    
	int imgSize=height*width;

 //    cout <<  "Save the segments ........." << endl;
    // vector<image<rgb>*> Out(num_region);
    if (classEvaluate==0) {

    	for(int i = 0; i < num_region; i++){
    		colors[i]= random_rgb();
    	}

    	// Save the coloured image regions
    	for(int y = 0; y < height; y++) {
    		for(int x = 0; x < width; x++) {
    			imRef(output, x, y) = colors[indexmap[y*width+x]];
    	    }
    	}
        s="results/"+imgName+"/regions.PPM";
        ch = s.c_str();

    	savePPM(output,ch);
    }




	for (int i=0; i<num_combinedRegions; i++) {
		combinedRegions[i].im_size=imgSize;
		if (i<num_region) {
			combinedRegions[i].box[0]=box[4*i+0];
			combinedRegions[i].box[1]=box[4*i+1];
			combinedRegions[i].box[2]=box[4*i+2];
			combinedRegions[i].box[3]=box[4*i+3];
			combinedRegions[i].region_size=regionSizes[i];

			combinedRegions[i].c = colorHists[i]/regionSizes[i];
			combinedRegions[i].t = textureHists[i]/regionSizes[i];
			combinedRegions[i].combinedNum = num_region;
		}

	}

    for(int i = 0; i < num_combinedRegions; i++) {
        for(int j = i; j < num_combinedRegions; j++){
        	if( i < num_region && j < num_region){
        		combinedNeighbors[i*num_combinedRegions+j] = neighbors[i*num_region+j];
        		combinedNeighbors[j*num_combinedRegions+i] = neighbors[j*num_region+i];
        	}
        	else{
        		combinedNeighbors[i*num_combinedRegions+j] = 0;
        		combinedNeighbors[j*num_combinedRegions+i] = 0;
        	}
        }
    }
    for(int i = 0; i < num_combinedRegions; i++) {
        for(int j = i; j < num_combinedRegions; j++){
            if(combinedNeighbors[i*num_combinedRegions+j] == 1){
                region_sim[i*num_combinedRegions+j] = combinedRegions[i].sim(&combinedRegions[j],a);
                region_sim[j*num_combinedRegions+i] = region_sim[i*num_combinedRegions+j];
            }
            else{
                region_sim[i*num_combinedRegions+j] = 0.0;
                region_sim[j*num_combinedRegions+i] = 0.0;
            }
        }
    }

    // delete[] neighbors;
    // delete[] 

    // cout << "start merging" << endl;
    int c=num_region-1;
    for (int r=num_region; r<num_combinedRegions; r++) {
    	double max_sim=0.0;
    	int regionIdx1, regionIdx2;
    	for (int i=0; i<r; i++) {
    		for (int j=i+1; j<r; j++) {
    			int idx= i*num_combinedRegions+j;
    			if (region_sim[idx]>max_sim) {
    				max_sim=region_sim[idx];
    				regionIdx1=i;
    				regionIdx2=j;
    			}
    		}
    	}
    	// cout << "combine: " << regionIdx1 << " & " << regionIdx2  << " " << max_sim << endl;
		combinedRegions[r].combinedNum=c;
		for (int i=0; i<r; i++) {
			if ( i != regionIdx1 && i != regionIdx2 && combinedRegions[i].combinedNum==c+1)
				combinedRegions[i].combinedNum=c;
		}
		c--;

    	combinedRegions[r].mergeRegion(&combinedRegions[regionIdx1],&combinedRegions[regionIdx2],regionIdx1,regionIdx2);

    	colors[regionIdx2]=colors[regionIdx1];
    	colors[r]=colors[regionIdx1];

        if (classEvaluate==0) {
    		for(int y = 0; y < height; y++) {
    			for(int x = 0; x < width; x++) {
    				if((indexmap[y*width+x] == regionIdx1+1) ||(indexmap[y*width+x] == regionIdx2+1)){
    	      	  	    indexmap[y*width+x] = r + 1;
    	      	    }				
    				imRef(output, x, y) = colors[indexmap[y*width+x]-1];
    		    }
    		}	
    		// Out.push_back(output);
            s="results/"+imgName+"/region_"+to_string(r)+".PPM";
            ch = s.c_str();
    	    // sprintf(s,"results/region%s_%d.PPM",imgName.c_str(),r);
    	    savePPM(output,ch);
        }


	    

    	for (int i=0; i<r; i++) {
    		if (i != regionIdx1 && i != regionIdx2 && (combinedNeighbors[regionIdx1*num_combinedRegions+i]==1 || combinedNeighbors[regionIdx2*num_combinedRegions+i]==1 )) {
    			combinedNeighbors[r*num_combinedRegions+i]=1;
    			combinedNeighbors[i*num_combinedRegions+r]=1;
    			region_sim[i*num_combinedRegions+r]=combinedRegions[r].sim(&combinedRegions[i],a);
    			region_sim[r*num_combinedRegions+i]=region_sim[i*num_combinedRegions+r];
    		}
    	}
    	for (int i=0; i<r; i++) {
	        combinedNeighbors[regionIdx1*num_combinedRegions+i] = 0;
            combinedNeighbors[i*num_combinedRegions+regionIdx1] = 0;
            region_sim[regionIdx1*num_combinedRegions+i] = 0;
            region_sim[i*num_combinedRegions+regionIdx1] = 0;

            combinedNeighbors[regionIdx2*num_combinedRegions+i] = 0;
            combinedNeighbors[i*num_combinedRegions+regionIdx2] = 0;
            region_sim[regionIdx2*num_combinedRegions+i] = 0;
            region_sim[i*num_combinedRegions+regionIdx2] = 0;
    	}



    }


	// delete[] combinedRegions;
	delete[] combinedNeighbors;
	delete[] region_sim;
	delete output;


	return combinedRegions;
	// return boundingBoxes;


}

#endif