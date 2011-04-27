/*
 *  faceProcessor.h
 *  DepthVisualizer
 *
 *  Created by zachary lieberman on 3/27/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxAutoControlPanel.h"
#include "ofxOpenCv.h"

class testApp;

class faceProcessor {

public: 

	
	faceProcessor(){}
	~faceProcessor(){}
	
	void setup();
	
	void setupPanel(ofxAutoControlPanel * _panel);
	
	ofxAutoControlPanel * panel;
	
	
	void update();
	void draw();
	
	ofxCvColorImage colorFull;
	unsigned char * colorPixelsFullSize;
		
	unsigned char * previousDepthImage;
	ofxCvGrayscaleImage depthImageSmoothed;
	ofxCvGrayscaleImage prevDepthImageSmoothed;
	
	
	ofxCvGrayscaleImage grayImgFull;
	ofxCvGrayscaleImage grayImg;
	
	ofxCvGrayscaleImage faceSmall;
	ofxCvGrayscaleImage faceDepthSmall;
	
	
	testApp * app;
	
	int w, h;
	
	
	
	
};