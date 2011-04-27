#include "DepthVisualizerInput.h"
#include "testApp.h"

const int useKinect = false;

bool DepthVisualizerInput::usingKinect(){
	return useKinect;
}

void DepthVisualizerInput::setup(ofxControlPanel& panel){
	this->panel = &panel;
	
	camWidth = 640;
	camHeight = 480;
	
	
	depthImage.allocate(camWidth, camHeight);
	colorPixels = new unsigned char[camWidth*camHeight*4];
	
	/*if(useKinect) {
		//kinect.init(true);  //shows infrared image
		kinect.init();
		kinect.setVerbose(true);
		kinect.open();
		// this is setting the raw data conversion range
		// we assume that 100 to 300 CM (1 - 3 meters) is good for tracking a person in space
		rawNearThreshold = 100;
		rawFarThreshold = 300;
		kinect.getCalibration().setClippingInCentimeters(rawNearThreshold, rawFarThreshold);
	} else */{
		rawNearThreshold = 100;
		rawFarThreshold = 140;
		
		//ofSetLogLevel(OF_LOG_VERBOSE);
		
		animation.load("/Users/zach/Desktop/FACE__/face output/");
		cout << "Animation length is " << animation.size() << " frames." << endl;
	}
	
	for(int y = 0; y < camHeight; y++) {
		for(int x = 0; x < camWidth; x++) {
			pointGrid[y][x].set(0, 0, 0);
		}
	}
}



bool DepthVisualizerInput::update(){
	bool isFrameNew = false;

	/*if(useKinect) {
		kinect.update();
		if(kinect.isFrameNew()) {
			isFrameNew = true;
			depthImage.setFromPixels(kinect.getDepthPixels(), camWidth, camHeight);
			depthImage.flagImageChanged();
		}
	} else */ {
		
		if(panel->getValueB("playbackPause") ) {
			animation.setPlaySpeed(0);
		} else {
			animation.setPlaySpeed(panel->getValueF("playSpeed"));
		}
		
		if(animation.isFrameNew()) {
			isFrameNew = true;
			ofImage& cur = animation.getAlpha();
			depthImage.setFromPixels(cur.getPixels(), cur.getWidth(), cur.getHeight());
			depthImage.flagImageChanged();
			colorPixels = animation.get().getPixels();
		}
	}
	
	if(isFrameNew) {
		thresholdDepthImage();
		buildPointCloud();
	}
	
	return isFrameNew;
}

void DepthVisualizerInput::thresholdDepthImage() {
	// do processing here on depthImage
	// getting rid of (setting depth = 0) on pixels we don't care about
	// for example, background, etc. 
	//
	unsigned char* depthPixels = depthImage.getPixels();
	int nearThresh = panel->getValueF("nearThreshold");
	int farThresh = panel->getValueF("farThreshold");
	
	for(int y = 0; y < camHeight; y++) {
		for(int x = 0; x < camWidth; x++) {
			int i = y * camWidth + x;
			if (depthPixels[i] <  farThresh  || depthPixels[i] >= nearThresh){
				depthPixels[i] = 0;
			}
		}
	}
}

void DepthVisualizerInput::buildPointCloud() {	
	unsigned char* depthPixels = depthImage.getPixels();
	
	// generate the point cloud
	/*if(useKinect) {
		pointCloud.clear();
		// this offset will center the data on the center of the scene
		float offset = -(rawNearThreshold + rawFarThreshold) / 2;
		for(int y = 0; y < camHeight; y++) {
			for(int x = 0; x < camWidth; x++) {
				int i = y * camWidth + x;
				if(depthPixels[i] != 0) {
					
					pointcloudPoint pp; 
					ofxVec3f cur = kinect.getWorldCoordinateFor(x, y);
					cur *= 100; // convert from meters to cm
					cur.z += offset;
					pp.point = cur;
					pp.color.set(255,0,0);
					
					pointCloud.push_back(pp);
					
					pointGrid[y][x].set(cur);
				} else {
					pointGrid[y][x].set(0, 0, 0);
				}
			}
		}
	} else */{
		double fx_d = 1.0 / 5.9421434211923247e+02;
		double fy_d = 1.0 / 5.9104053696870778e+02;
		float cx_d = 3.3930780975300314e+02;
		float cy_d = 2.4273913761751615e+02;
		
		pointCloud.clear();
		// this offset will center the data on the center of the scene
		float offset = -(rawNearThreshold + rawFarThreshold) / 2;
		for(int y = 0; y < camHeight; y++) {
			for(int x = 0; x < camWidth; x++) {
				int i = y * camWidth + x;
				if(depthPixels[i] != 0) {
					// ok where are we: 
					float pct = depthPixels[i] / 255.0;
					
					float zval = rawNearThreshold + (1-pct) * (rawFarThreshold - rawNearThreshold);
					
					float z = zval / 100.0f;
					ofxVec3f cur;
					cur.x = float((x - cx_d) * z * fx_d);
					cur.y = float((y - cy_d) * z * fy_d);
					cur.z = z;
					
					//ofxVec3f cur = kinect.getWorldCoordinateFor((float)x, (float)y, zval/100.0f);
					cur *= 100; // convert from meters to cm
					cur.z += offset;
					
					float rr = colorPixels[i*4]/255.0;
					float gg = colorPixels[i*4+1]/255.0;
					float bb = colorPixels[i*4+2]/255.0;
					
					pointcloudPoint pp; 
					pp.oldx = x;
					pp.oldy = y;
					
					
					
					pp.point.set(cur.x, cur.y, cur.z);
					pp.color.set(rr,gg,bb);
					pointCloud.push_back(pp);
					
					
					pointGrid[y][x].set(cur);
				} else {
					pointGrid[y][x].set(0, 0, 0);
				}
			}
		}
	}
}

void DepthVisualizerInput::drawOrthographic() {
	;  // this needs to be done, from the depth image. 
}

void DepthVisualizerInput::drawPerspective() {
	
	
}

void DepthVisualizerInput::drawDebug(){
	
	//ofTranslate((ofGetWidth() - camWidth) / 2, 0, 0);
	ofSetColor(255, 255, 255);
	depthImage.draw(0, 0);

	ofPushMatrix();
		
	ofTranslate(camWidth/2.0, ofGetHeight()*0.85, -500);
		ofRotateY(panel->getValueF("rotateY"));
		if (panel->getValueB("autoRotate")){
			ofRotateY(ofGetElapsedTimef()*5);
		}
		ofScale(2,2,2);
	
		
		// need something here....
		drawPerspective();

	ofPopMatrix();
	
}


//--------------------------------------------------------------
void DepthVisualizerInput::exit() {
	/*if(useKinect) {
		kinect.close();
	}*/
}