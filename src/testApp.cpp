#include "testApp.h"





//--------------------------------------------------------------
void testApp::setup() {
	ofSetFrameRate(60);
	input.setup(panel);  // < -- could pass in useKinnect here?
	
	panel.setup("Control Panel", 1024-310, 305, 300, 600);
	
	panel.addPanel("Threshold and Scale");
	panel.addSlider("near threshold", "nearThreshold", 255, 0, 255, true);
	panel.addSlider("far threshold", "farThreshold", 0, 0, 255, true);
	
	panel.addPanel("Control");
	panel.addSlider("scale", "scale", 7, 1, 20);	
	panel.addSlider("rotate y axis", "rotateY", 0, -360, 360, false);	
	panel.addToggle("auto rotate", "autoRotate", false);
	panel.addToggle("draw debug", "drawDebug", false);
	
	panel.addToggle("draw scene bounding frustrum", "drawSceneBox", false);
		
	if (input.usingKinect() == false){
		panel.addSlider("playback speed", "playSpeed", 0.5, -1, 1, false);
		panel.addToggle("playbackPause", "playbackPause", false);
	}
	

	FP.setup();
	FP.setupPanel(&panel);
	
	scaleSmoothed			= 7.0;
	rotateYSmoothed			= 0;
	
	TM.setup(320,240);
	
	TM.setGrayscaleDataPtr(&FP.grayImgFull);
	
	
}

//--------------------------------------------------------------
void testApp::update() {
	input.update();
	
	// sorry the order here is weird, will change
	// data goes input -> LDP -> TM 

	
	// get the gray image in LDP right: 
	for (int i = 0; i < 640*480; i++){
		FP.colorPixelsFullSize[i*3 + 0] = input.colorPixels[i*4 + 0];		//+ app->input.colorPixels[i*4+1] + app->input.colorPixels[i*4+2])/3;
		FP.colorPixelsFullSize[i*3 + 1] = input.colorPixels[i*4 + 1];
		FP.colorPixelsFullSize[i*3 + 2] = input.colorPixels[i*4 + 2];
	}
	FP.colorFull.setFromPixels(FP.colorPixelsFullSize, 640, 480);
	FP.grayImgFull = FP.colorFull;
	FP.grayImg.scaleIntoMe(FP.grayImgFull);
	
	TM.update();
	FP.update();
	
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofBackground(0, 0, 0);
	
	ofSetColor(255,255,255);
	
	FP.draw();
	FP.grayImgFull.draw(0, 0);
	
	ofNoFill();
	ofSetColor(255,0,100);
	ofBeginShape();
	for (int i = 0; i <60; i++){
		//ofVertex(TM.asmPtsSmoothed[i].x, TM.asmPtsSmoothed[i].y);
	}
	ofEndShape();
	
	ofNoFill();
	
	
	
	
	if (ofGetFrameNum() > 10){
	
		Delaunay::Point tempP;
		vector< Delaunay::Point > v;
		for (int i = 0; i < 60; i++){
			tempP[0] = TM.asmPtsSmoothed[i].x;
			tempP[1] = TM.asmPtsSmoothed[i].y;
			v.push_back(tempP);
		}
		Delaunay delobject(v);
		delobject.Triangulate();
		
		for(Delaunay::fIterator fit  = delobject.fbegin();
			fit != delobject.fend();
			++fit){
			
			int pta = delobject.Org(fit);
			int ptb = delobject.Dest(fit);
			int ptc = delobject.Apex(fit);
			
			ofLine(TM.asmPtsSmoothed[pta].x, TM.asmPtsSmoothed[pta].y, TM.asmPtsSmoothed[ptb].x, TM.asmPtsSmoothed[ptb].y);
			ofLine(TM.asmPtsSmoothed[ptc].x, TM.asmPtsSmoothed[ptc].y, TM.asmPtsSmoothed[ptb].x, TM.asmPtsSmoothed[ptb].y);
			ofLine(TM.asmPtsSmoothed[pta].x, TM.asmPtsSmoothed[pta].y, TM.asmPtsSmoothed[ptc].x, TM.asmPtsSmoothed[ptc].y);

			
			/*for(int i =0; i < 3; ++i){
				
				int pta = delobject.Org(fit);
				int ptb = delobject.Sym(fit,i);
				ofLine(TM.asmPtsSmoothed[pta].x, TM.asmPtsSmoothed[pta].y, TM.asmPtsSmoothed[ptb].x, TM.asmPtsSmoothed[ptb].y);
				
			}*/
		}
	}
	
}

//--------------------------------------------------------------
void testApp::exit() {
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	
	if(key == 'f') {
		ofToggleFullscreen();
	}

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
}

