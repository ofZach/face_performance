#pragma once

#include "ofMain.h"

#include "ofxAutoControlPanel.h"
#include "DepthVisualizerInput.h"
#include "ofShader.h"
#include "ofxFBOTexture.h"

#include "faceProcessor.h"
#include "trackingManager.h"
#include "ofxTriangle.h"



class testApp : public ofBaseApp {
	
	public:

		void setup();
		void update();
		void draw();
		void exit();
	
		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		ofxAutoControlPanel panel;
		faceProcessor  FP;
		DepthVisualizerInput input;
	
		float scaleSmoothed, rotateYSmoothed, focusDistanceSmoothed, apertureSmoothed, pointBrightSmoothed, rgbBrightSmoothed, maxPointSmoothed;
		float volumeSmoothed, redScaleSmoothed, greenScaleSmoothed, blueScaleSmoothed;
	
		trackingManager TM;
	
		
	
	
};
