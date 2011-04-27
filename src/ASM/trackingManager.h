/*
 *  trackingManager.h
 *  eyeWriterCam
 *
 *  Created by sugano on 11/01/25.
 *
 */

#ifndef _TRACKSCENE_H
#define _TRACKSCENE_H

#include <deque>

//#include "GaussianProcess.h"
#include "cv.h"
#include "asmmodel.h"
#include "modelfile.h"
#include "util.h"
#include "ofxCvMain.h"

class trackingManager {
	
public:
	trackingManager();
	~trackingManager();
	
	void setup(int w, int h);
	void setGrayscaleDataPtr(ofxCvGrayscaleImage * gray);
	void update(); // call this on a new frame only. 
	bool asmFit();
	void draw();
	
	int width, height;
	ofxCvGrayscaleImage * grayPtr;
	
	void keyPressed(int key);
	vector< Point_<int> > ptslist;
	ofPoint asmPts[60];
	ofPoint asmPtsSmoothed[60];
	
	void grabFrame();
	//IplImage *sourceImage, *colorImage;
	ofxCvColorImage dispImage;
	void updateDispImage();

	cv::CascadeClassifier faceCascade;
	ASMModel asmModel;
	void readASMModel(ASMModel &asmModel, string modelPath);

};

#endif