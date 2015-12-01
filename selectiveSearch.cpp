#include <iostream>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <sys/stat.h>

#include "grouping.h"
#include "matImage.h"
#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;

int main(int argc, char **argv) {
	string imgName,Imjpg,s;
	string line;
	Mat imMat;
	image<rgb>* im;
	region* combinedRegions ;
	int colorSpaceIdx=0;
	int num_combinedRegions;
 	int gtBox[4];
    string token;
    int imgIdx;

	int k = 200; 
	int minSize = k;
	int num_region;
	double sigma = 0.8;

	double maxOverlap=0;
	int maxOverlapIdx=0;
	double unionor,unionand;

    ifstream imgNameFile ( "gtIms2.csv" ); 
    ifstream gtBoxFile ( "gtBoxes2.csv" ); 


	if (stoi(argv[1])<0) {
		// vector<string> gtIms1;
		imgIdx=0;
		double sumOverlap=0;
		if ( imgNameFile.good() ) {
			while (getline(imgNameFile,line)) {
				line.erase(remove(line.begin(), line.end(), ','), line.end());
				// gtIms1.push_back(line);
				imgName=line;
				Imjpg="../VOCdevkittest/VOC2007/JPEGImages/"+imgName+".jpg";

				imMat=imread(Imjpg,CV_LOAD_IMAGE_COLOR);

				im = matToImage(imMat);
			    combinedRegions = hierarchicalGrouping(im, sigma, k, minSize, colorSpaceIdx,num_region,imgName,1); 
			   
			    // cout << "numRegion: " << num_region << endl;
			 	num_combinedRegions = 2*num_region -1;

			 	if ( gtBoxFile.good() ) {
					// for (int i=0; i<imgIdx; i++)
					getline(gtBoxFile,line);
					stringstream ss(line);
					int k=0;
					while (getline(ss,token,',')) {
						gtBox[k++]=stoi(token);
						cout << gtBox[k-1] << " ";
					}
					cout << endl;
				}
			    for(int i = 0; i < num_combinedRegions; i++) {
			    	unionor = (max(combinedRegions[i].box[3],gtBox[3])-min(combinedRegions[i].box[1],gtBox[1]))*(max(combinedRegions[i].box[2],gtBox[2])-min(combinedRegions[i].box[0],gtBox[0]));
			    	unionand = (min(combinedRegions[i].box[3],gtBox[3])-max(combinedRegions[i].box[1],gtBox[1]))*(min(combinedRegions[i].box[2],gtBox[2])-max(combinedRegions[i].box[0],gtBox[0]));
			    	if (double(unionand/unionor) > maxOverlap) {
			    		maxOverlap=unionand/unionor;
			    		maxOverlapIdx=i;
			    	}
			    }
			    cout << "imgIdx: " <<imgIdx << " maxOverlap: " << maxOverlap << endl;
			    sumOverlap += maxOverlap;
			    maxOverlap = 0;

				imgIdx++;
			}
		}
		double ABO= sumOverlap/imgIdx;
		cout << "ABO: " << ABO << endl;
	}
	else {
		imgIdx=stoi(argv[1]);
	    if (imgNameFile.good()) {
	    	for (int i=0; i<imgIdx; i++)
	    		getline(imgNameFile,line);
			line.erase(remove(line.begin(), line.end(), ','), line.end());
	    }
		// imgName=argv[1];
		imgName=line;
		cout << "imgName: " << imgName << endl;		
		Imjpg="../VOCdevkittest/VOC2007/JPEGImages/"+imgName+".jpg";
		struct stat st = {0};
		s="results/"+imgName;
		if (stat((const char*)s.c_str(), &st) == -1) {
		    mkdir((const char*)s.c_str(), 0700);
		}
	
		imMat=imread(Imjpg,CV_LOAD_IMAGE_COLOR);

		im = matToImage(imMat);
	    combinedRegions = hierarchicalGrouping(im, sigma, k, minSize, colorSpaceIdx,num_region,imgName,0); 
	   
	    cout << "numRegion: " << num_region << endl;
	 	num_combinedRegions = 2*num_region -1;

		if ( gtBoxFile.good() ) {
			for (int i=0; i<imgIdx; i++)
				getline(gtBoxFile,line);
			stringstream ss(line);
			int k=0;
			while (getline(ss,token,',')) {
				gtBox[k++]=stoi(token);
				cout << gtBox[k-1] << " ";
			}
			cout << endl;
		}
		
	    // save all windows 
	    for(int i = 0; i < num_combinedRegions; i++) {
	    	rgb color = random_rgb();
	    	for(int j = combinedRegions[i].box[0]; j < combinedRegions[i].box[2]; j++){
	    		imRef(im,combinedRegions[i].box[1],j) = color;
	    		imRef(im,combinedRegions[i].box[3],j) = color;
	    	}
	    	for(int j =combinedRegions[i].box[1]; j < combinedRegions[i].box[3]; j++){
	    	    imRef(im,j,combinedRegions[i].box[0]) = color;
	    	    imRef(im,j,combinedRegions[i].box[2]) = color;
	    	}
	    	unionor = (max(combinedRegions[i].box[3],gtBox[3])-min(combinedRegions[i].box[1],gtBox[1]))*(max(combinedRegions[i].box[2],gtBox[2])-min(combinedRegions[i].box[0],gtBox[0]));
	    	unionand = (min(combinedRegions[i].box[3],gtBox[3])-max(combinedRegions[i].box[1],gtBox[1]))*(min(combinedRegions[i].box[2],gtBox[2])-max(combinedRegions[i].box[0],gtBox[0]));
	    	if (double(unionand/unionor) > maxOverlap) {
	    		maxOverlap=unionand/unionor;
	    		maxOverlapIdx=i;
	    	}
	    }
	    Mat img = *imageToMat(im);
	    s="results/"+imgName+"/boxes.jpg";
	    imwrite(s,img);

	    //plot maxoverlap
		rgb color;//=random_rgb();
		color.r=(uchar)255;
		color.g=(uchar)0;
		color.b=(uchar)0;

	    im = matToImage(imMat);
	    for(int j = combinedRegions[maxOverlapIdx].box[0]; j < combinedRegions[maxOverlapIdx].box[2]; j++){
			imRef(im,combinedRegions[maxOverlapIdx].box[1],j) = color;
			imRef(im,combinedRegions[maxOverlapIdx].box[3],j) = color;
		}
		for(int j =combinedRegions[maxOverlapIdx].box[1]; j < combinedRegions[maxOverlapIdx].box[3]; j++){
		    imRef(im,j,combinedRegions[maxOverlapIdx].box[0]) = color;
		    imRef(im,j,combinedRegions[maxOverlapIdx].box[2]) = color;
		}
		color.r=(uchar)0;
		color.g=(uchar)255;
		color.b=(uchar)0;
	    for(int j = gtBox[0]-1; j < gtBox[2]; j++){
			imRef(im,gtBox[1]-1,j) = color;
			imRef(im,gtBox[3]-1,j) = color;
		}
		for(int j = gtBox[1]-1; j < gtBox[3]; j++){
		    imRef(im,j,gtBox[0]-1) = color;
		    imRef(im,j,gtBox[2]-1) = color;
		}
		img = *imageToMat(im);
		s="results/"+imgName+"/gt.jpg";
	    imwrite(s,img);
		namedWindow( "window", WINDOW_AUTOSIZE );
		imshow("window",img);
		waitKey(10);





		cout << "Total Num of region: " << num_region << endl;
		cout << "Please enter window number or 0 to quit, -1 to demo hiercarchical Grouping: " ;

	    int boxNum;
	    while (cin>>boxNum) {
			im = matToImage(imMat);

	    	if (boxNum==0)
	    		break;
	    	else if (boxNum==-1) {
	    		vector<String> files;
		        s="results/"+imgName+"/region_*.PPM";

				glob(s, files);
				for (int i=0; i<files.size(); i++) {
					Mat imMovie = imread(files[i]);
					namedWindow( "window", WINDOW_AUTOSIZE );
					imshow("window",imMovie);
					waitKey(100);	
				}
				cout << "Total Num of region: " << num_region << endl;
				cout << "Please enter window number or 0 to quit, -1 to demo hiercarchical Grouping: " ;
				continue;
	    	}
		    for(int i = 0; i < num_combinedRegions; i++){
		    	if (combinedRegions[i].combinedNum>boxNum)
		    		continue;
		    	for(int j = combinedRegions[i].box[0]; j <= combinedRegions[i].box[2]; j++){
		    		imRef(im,j,combinedRegions[i].box[1]) = color;
		    		imRef(im,j,combinedRegions[i].box[3]) = color;
		    	}
		    	for(int j =combinedRegions[i].box[1]; j <= combinedRegions[i].box[3]; j++){
		    	    imRef(im,combinedRegions[i].box[0],j) = color;
		    	    imRef(im,combinedRegions[i].box[2],j) = color;
		    	}
		    }

			img = *imageToMat(im);
			namedWindow( "window", WINDOW_AUTOSIZE );
			imshow("window",img);
			cout << "Total Num of region: " << num_region << endl;
			cout << "Please enter window number or 0 to quit, -1 to demo hiercarchical Grouping: " ;
			waitKey(10);
		}
	}
	return 0;
}
  
