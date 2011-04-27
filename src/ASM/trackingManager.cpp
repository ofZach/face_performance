/*
 *  trackingManager.cpp
 *  eyeWriterCam
 *
 *  Created by sugano on 11/01/25.
 *
 */

#include "trackingManager.h"

using namespace std;
using namespace cv;

trackingManager::trackingManager(){ 
	//sourceImage = NULL;
	//colorImage = NULL;
}

trackingManager::~trackingManager(){
	//if(sourceImage != NULL) cvReleaseImage(&sourceImage);
	//if(g != NULL) cvReleaseImage(&colorImage);
}

void trackingManager::setup(int w, int h){
	width = w;
	height = h;
	string model_path = ofToDataPath("asm/grayall_asm.model");
	string cascade_path = ofToDataPath("haarXML/haarcascade_frontalface_alt2.xml");
	readASMModel(asmModel, model_path);
	bool res = faceCascade.load(cascade_path);
}

void trackingManager::setGrayscaleDataPtr(ofxCvGrayscaleImage * gray){
	grayPtr = gray;
}

void trackingManager::update(){
	//--- update video/camera input
		
	//--- eye tracking (on new frames)	
	asmFit();
	
	static float prevTime = ofGetElapsedTimef();
	float currTime = ofGetElapsedTimef();
	prevTime = ofGetElapsedTimef();
}

void trackingManager::updateDispImage(){
	//Mat dispMat(dispImage.getCvImage());
//	Mat greyMat(dispMat.size(), CV_8UC1);
//	cvtColor(dispMat, greyMat, CV_BGR2GRAY);
//	Mat roi;
}

void trackingManager::draw(){
	//dispImage.draw((ofGetWidth()-IM.width)/2, (ofGetHeight()-IM.height)/2, IM.width, IM.height);
}


void trackingManager::readASMModel(ASMModel &asmModel, string modelPath){
    ModelFileAscii mf;
    mf.openFile(modelPath.c_str(),"rb");
    asmModel.loadFromFile(mf);
    mf.closeFile();
}

bool trackingManager::asmFit(){
	
	grayPtr->contrastStretch();
	// this seems slow?  allocate once?
	Mat img(grayPtr->getCvImage());
	vector<FitResult> fitResult;
	
	double scale = 0.8;
	Mat small_img;
	resize(img, small_img, cv::Size(0,0), scale, scale);
	
	asmModel.fit(small_img, fitResult, faceCascade, true);
	
	// if detected
	if(fitResult.size() > 0)
	{
		asmModel.resultToPointList(fitResult[0], ptslist);
		
		for (int i = 0; i < ptslist.size(); i++){
			asmPts[i] = ofPoint(ptslist[i].x/scale, ptslist[i].y/scale);
			asmPtsSmoothed[i] = 0.7f * asmPtsSmoothed[i] + 0.3f * asmPts[i];
		}
		return true;
	}
	else return false;
}


