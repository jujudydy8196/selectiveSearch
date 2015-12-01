#include <iostream>

// #include "color.h"
#include "hist.h"
#include "Dependencies/anigaussm/anigauss.c"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
#define UInt8 char

colorHist* get_colorHists(int* indexmap, int num_Region, int colorIdx, image<rgb> *im) {
    colorHist* colorHists = new colorHist[num_Region];
    for (int i=0; i<num_Region; i++) {
        colorHists[i].set(colorIdx);
    }
    // cout << colorHists[0].c1[2] << endl;

    double step= 255.0/25.0 + 0.005;
    int regionIdx,bin;
    int width = im->width();
    int height = im->height();
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            regionIdx=indexmap[y*width+x]-1;
            bin=int(imRef(im,x,y).r/step);
            colorHists[regionIdx].c1[bin]++;
            bin=int(imRef(im,x,y).g/step);
            colorHists[regionIdx].c2[bin]++;
            bin=int(imRef(im,x,y).b/step);
            colorHists[regionIdx].c3[bin]++;
        }
    }
    // cout << colorHists[4].c1[23] << endl;

    return colorHists;

}

textureHist* get_textureHists(int* indexmap, int num_Region, int colorIdx, image<rgb> *im) {
    textureHist* textureHists = new textureHist[num_Region];
    for (int i=0; i<num_Region; i++) {
        textureHists[i].set(colorIdx);
    }
    int width = im->width();
    int height = im->height();

    image<float> *r = new image<float>(width,height);
    image<float> *g = new image<float>(width,height);
    image<float> *b = new image<float>(width,height);

    image<float> *gaussian_r = new image<float>(width,height);
    image<float> *gaussian_g = new image<float>(width,height);
    image<float> *gaussian_b = new image<float>(width,height);

    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            imRef(r,x,y)=imRef(im,x,y).r;
            imRef(g,x,y)=imRef(im,x,y).g;
            imRef(b,x,y)=imRef(im,x,y).b;
        }
    }
    //eight orientations:360/8
    double angle = 360/8;
    for (int i=0; i<8; i++) {
        anigauss(r->data, gaussian_r->data, width, height , 1, 1, angle*i, 1, 1);
        anigauss(g->data, gaussian_g->data, width, height , 1, 1, angle*i, 1, 1);
        anigauss(b->data, gaussian_b->data, width, height , 1, 1, angle*i, 1, 1);
        
        double Rmax = 0 ;
        double Rmin = 0 ;
        double Gmax = 0 ;
        double Gmin = 0 ;        
        double Bmax = 0 ;
        double Bmin = 0 ;

        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                // cout << imRef(gaussian_r,x,y) << " ";
                if (imRef(gaussian_r,x,y)>Rmax)
                    Rmax=imRef(gaussian_r,x,y);
                else if (imRef(gaussian_r,x,y)<Rmin)
                    Rmin=imRef(gaussian_r,x,y);
                if (imRef(gaussian_g,x,y)>Gmax)
                    Gmax=imRef(gaussian_g,x,y);
                else if (imRef(gaussian_g,x,y)<Gmin)
                    Gmin=imRef(gaussian_g,x,y);
                if (imRef(gaussian_b,x,y)>Rmax)
                    Bmax=imRef(gaussian_b,x,y);
                else if (imRef(gaussian_b,x,y)<Bmin)
                    Bmin=imRef(gaussian_b,x,y);
            }
        }
        double r_step = (Rmax-Rmin)/(10-0.05);
        double g_step = (Gmax-Gmin)/(10-0.05);
        double b_step = (Bmax-Bmin)/(10-0.05);

        int regionIdx,bin;
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                regionIdx=indexmap[y*width+x]-1;
                bin=int((imRef(gaussian_r,x,y)-Rmin)/r_step);
                textureHists[regionIdx].c1[i*8+bin]++;
                bin=int((imRef(gaussian_g,x,y)-Gmin)/g_step);
                textureHists[regionIdx].c2[i*8+bin]++;
                bin=int((imRef(gaussian_b,x,y)-Bmin)/b_step);
                textureHists[regionIdx].c3[i*8+bin]++;
            }
        }
    }

    delete gaussian_r;
    delete gaussian_g;
    delete gaussian_b;

    delete r;
    delete g;
    delete b;

    // cout << colorHists[4].c1[23] << endl;

    return textureHists;



}