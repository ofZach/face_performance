#ifndef VECTORFIELD_H
#define VECTORFIELD_H


#include "ofMain.h"
#include "ofxVectorMath.h"


class vectorField {
	
    public:
	
		// the internal dimensions of the field:    (ie, 60x40, etc)
        int fieldWidth;
        int fieldHeight;
		int fieldDepth;
	
		int fieldSize;   // total number of "pixels", ie w * h * d
	
		// the external dimensions of the field:   (ie, 1024x768)
		
	
		ofxVec3f topPoint;
		ofxVec3f bottomPoint;
	
		/*int externalWidth;
		int externalHeight;
		int externalDepth;
		 */
	
        vector <ofxVec3f> field;

        vectorField();
	virtual ~vectorField(){};
		
		void setupField(int innerW, int innerH, int innderD, ofxVec3f top, ofxVec3f bottom);   // pass in internal dimensions and outer dimensions
	
        void clear();
        void fadeField(float fadeAmount);
        void randomizeField(float scale);
        void draw();
		void setFromPerlin(float scale, float time, float amount);
	
		ofxVec3f	getForceFromPos(float xpos, float ypos, float zpos);

		/*
        void addOutwardCircle(float x, float y, float radius, float strength);
        void addInwardCircle(float x, float y, float radius, float strength);
        void addClockwiseCircle(float x, float y, float radius, float strength);
        void addCounterClockwiseCircle(float x, float y, float radius, float strength);
        void addVectorCircle(float x, float y, float vx, float vy, float radius, float strength);
		 */
    protected:
    private:
};

#endif // VECTORFIELD_H
