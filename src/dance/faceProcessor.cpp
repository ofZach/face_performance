/*
 *  faceProcessor.cpp
 *  DepthVisualizer
 *
 *  Created by zachary lieberman on 3/27/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "faceProcessor.h"
#include "testApp.h"


//
//
//	//unsigned char * depthPixels = app->input.depthImage.getPixels();
//	depthImageSmoothed.setFromPixels(app->input.depthImage.getPixels(), 640,480);
//	depthImageSmoothed.blur(13);
//	
//	unsigned char * curPix = depthImageSmoothed.getPixels();
//	unsigned char * prePix = prevDepthImageSmoothed.getPixels();


void faceProcessor::setupPanel(ofxAutoControlPanel * _panel){
	
	/*panel = _panel;
	 
	 panel->addPanel("animation");
	 
	 panel->addToggle("freeze particles", "freeze", false);
	 panel->addToggle("use vector field", "useVec", false);
	 
	 panel->addToggle("use noise", "useNoise", false);
	 
	 panel->addToggle("fade field", "fadeField", false);
	 panel->addToggle("randomize field once", "randomField", false);
	 
	 panel->addSlider("damping", "damp", 0.14f, 0.0, 1.0);	
	 panel->addSlider("colorDropOffRate", "colorDropOff", 0.99f, 0.01, 1.0);	
	 
	 panel->addSlider("noiseScale", "noiseScale", 10, 1, 100.0, true);	
	 panel->addSlider("timeScale", "timeScale", 10, 1, 400.0, true);	
	 panel->addSlider("fieldScale", "fieldScale", 0, 1, 5.0, true);	
	 
	 panel->addToggle("drop all in", "allDropIn", false);
	 
	 */
	
}





void faceProcessor::setup(){
	
	w = 320;
	h = 240;
	previousDepthImage = new unsigned char[640*480];
	//grayPixelsFullSize = new unsigned char[640*480];
	colorPixelsFullSize = new unsigned char[640*480*3];
	grayImgFull.allocate(640,480);
	colorFull.allocate(640,480);
	grayImg.allocate(w,h);
	depthImageSmoothed.setUseTexture(true);
	prevDepthImageSmoothed.setUseTexture(false);
	depthImageSmoothed.allocate(640,480);
	prevDepthImageSmoothed.allocate(640,480);
	app = (testApp*)ofGetAppPtr();
	
	
	faceSmall.allocate(100,100);
	faceDepthSmall.allocate(100,100);
	
}


void faceProcessor::update(){
	
	depthImageSmoothed.setFromPixels(app->input.depthImage.getPixels(), 640,480);
	depthImageSmoothed.blur(13);
	
	//find the bounding box of the pts: 
	
	testApp * appPtr = (testApp * ) ofGetAppPtr();
	
	float minx = 640;
	float miny = 480;
	float maxx = 0;
	float maxy = 0;
	
	for (int i = 0; i < 60; i++){
		
		float x = appPtr->TM.asmPtsSmoothed[i].x;
		float y = appPtr->TM.asmPtsSmoothed[i].y;
		
		minx = MIN(minx, x);
		miny = MIN(miny, y);
		maxx = MAX(maxx, x);
		maxy = MAX(maxy, y);
		
	}
	
	
	ofPoint src[4];
	ofPoint dst[4];
	
	int margin = 8;
	minx -= margin;
	maxx += margin;
	miny -= margin;
	maxy += margin;
	
	src[0].set(minx, miny);
	src[1].set(maxx, miny);
	src[2].set(maxx, maxy);
	src[3].set(minx, maxy);
	
	dst[0].set(0,0);
	dst[1].set(100,0);
	dst[2].set(100,100);
	dst[3].set(0,100);
	
	//cout << src[0] << " " << src[1] << " " << src[2] << " " << src[3] << endl;
	
	faceSmall.warpIntoMe(grayImgFull, src, dst);
	faceDepthSmall.warpIntoMe(depthImageSmoothed, src, dst);
	
}



void faceProcessor::draw(){
	
	glPushMatrix();
	//grayImg.draw(0,0,320,240);
	depthImageSmoothed.draw(640,0,320,240);
	faceSmall.draw(0,480);
	faceDepthSmall.draw(0+100, 480);
	glPopMatrix();
	//haarFinder.draw(320, 0);
	
}