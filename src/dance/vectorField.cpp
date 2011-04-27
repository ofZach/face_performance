#include "vectorField.h"



//------------------------------------------------------------------------------------
vectorField::vectorField(){
	
	
	
	/*fieldWidth  = 60;
    fieldHeight = 40;
	 */
}

//------------------------------------------------------------------------------------
void vectorField::setupField(int innerW, int innerH, int innerD,  ofxVec3f top, ofxVec3f bottom){
	
	fieldWidth			= innerW;
	fieldHeight			= innerH;
	fieldDepth			= innerD;
	topPoint		= top;
	bottomPoint		= bottom;
	
	field.clear();
	
	fieldSize = fieldWidth * fieldHeight * fieldDepth;
	for (int i = 0; i < fieldSize; i++){
		ofxVec3f pt;  
		pt.set(0,0, 0);
		field.push_back(pt);
	}
}


//------------------------------------------------------------------------------------
void vectorField::clear(){
    for (int i = 0; i < fieldSize; i++){
        field[i].set(0,0, 0);
    }
}


//------------------------------------------------------------------------------------
void vectorField::fadeField(float fadeAmount){
	for (int i = 0; i < fieldSize; i++){
        field[i].set(field[i].x*fadeAmount,field[i].y*fadeAmount, field[i].z * fadeAmount);
    }
}

//------------------------------------------------------------------------------------
void vectorField::randomizeField(float scale){
	for (int i = 0; i < fieldSize; i++){
        // random between -1 and 1
        float x = (float)(ofRandom(-1,1)) * scale;
        float y = (float)(ofRandom(-1,1)) * scale;
		float z = (float)(ofRandom(-1,1)) * scale;
        field[i].set(x,y, z);
    }
}


//------------------------------------------------------------------------------------
void vectorField::setFromPerlin(float scale, float time, float amount){
	int externalWidth = topPoint.x - bottomPoint.x;
	int externalHeight = topPoint.y - bottomPoint.y;
	int externalDepth = topPoint.z - bottomPoint.z;
	
	
	float scalef = scale;
	float timef = time;
    float scalex = (float)externalWidth / (float)fieldWidth;
    float scaley = (float)externalHeight / (float)fieldHeight;
	float scalez = (float)externalDepth / (float)fieldDepth;
	
    for (int i = 0; i < fieldWidth; i++){
        for (int j = 0; j < fieldHeight; j++){
			for (int k = 0; k < fieldDepth; k++){
				
				// pos in array
				int pos = (k * (fieldWidth * fieldHeight) + j * fieldWidth + i);
				// pos externally
				float px = 	bottomPoint.x + i * scalex;
				float py =  bottomPoint.y + j * scaley;
				float pz = 	bottomPoint.z + k * scalez;
				
				
				float nx = ofSignedNoise(px/scalef, py/scalef, pz/scalef, 0 + timef);
				float ny = ofSignedNoise(px/scalef, py/scalef, pz/scalef, 666767 + timef);
				float nz = ofSignedNoise(pz/scalef, py/scalef, pz/scalef, 10000000*10 + timef);
				
				field[pos].set(nx*amount, ny*amount, nz*amount);
			}
		}
	}
}


//------------------------------------------------------------------------------------
void vectorField::draw(){
	
	
	int externalWidth = topPoint.x - bottomPoint.x;
	int externalHeight = topPoint.y - bottomPoint.y;
	int externalDepth = topPoint.z - bottomPoint.z;
	
	
    float scalex = (float)externalWidth / (float)fieldWidth;
    float scaley = (float)externalHeight / (float)fieldHeight;
	float scalez = (float)externalDepth / (float)fieldDepth;
	
    for (int i = 0; i < fieldWidth; i++){
        for (int j = 0; j < fieldHeight; j++){
			 for (int k = 0; k < fieldDepth; k++){

            // pos in array
            int pos = (k * (fieldWidth * fieldHeight) + j * fieldWidth + i);
            // pos externally
            float px = 	bottomPoint.x + i * scalex;
            float py =  bottomPoint.y + j * scaley;
			float pz = 	bottomPoint.z + k * scalez;
            float px2 = px + field[pos].x * 5;
            float py2 = py + field[pos].y * 5;
			float pz2 = pz + field[pos].z * 5;

			glBegin(GL_LINES);
				 glVertex3f(px,py, pz);
				 glVertex3f(px2, py2, pz2);
			glEnd();

			
			// draw a baseline to show direction
			// get the line as vector, calculate length, then normalize. 
			// rotate and draw based on length
			
			/*ofxVec2f line;
			line.set(px2-px, py2-py);
			float length = line.length();
			line.normalize();
			line.rotate(90);  // these are angles in degrees
			ofLine(px - line.x*length*0.2, py - line.y*length*0.2, px + line.x*length*0.2, py + line.y*length*0.2);
			*/
			 }
        }
    }
}

/*
ofxVec3f vectorField::setFromPerlinNoise(float xpos, float ypos, float zpos){

	
}
*/

//------------------------------------------------------------------------------------
ofxVec3f vectorField::getForceFromPos(float xpos, float ypos, float zpos){
	
	ofxVec3f frc;
	frc.set(0,0, 0);
	
	// convert xpos and ypos into pcts = 
	float xPct = (xpos - bottomPoint.x) / (float)(topPoint.x - bottomPoint.x);
	float yPct = (ypos - bottomPoint.y) / (float)(topPoint.y - bottomPoint.y);
	float zPct = (zpos - bottomPoint.z) / (float)(topPoint.z - bottomPoint.z);
	
	// if we are less then 0 or greater then 1 in x or y, return no force.
	if ((xPct < 0 || xPct > 1) || (yPct < 0 || yPct > 1)){
		return frc;	
	} 
	
    // where are we in the array
    int fieldPosX = (int)(xPct * fieldWidth);
    int fieldPosY = (int)(yPct * fieldHeight);
	int fieldPosZ = (int)(zPct * fieldDepth);
	

    // saftey :)
    fieldPosX = MAX(0, MIN(fieldPosX, fieldWidth-1));
    fieldPosY = MAX(0, MIN(fieldPosY, fieldHeight-1));
	fieldPosZ = MAX(0, MIN(fieldPosZ, fieldDepth-1));

    // pos in array
    int pos = fieldPosZ * (fieldHeight*fieldWidth) + fieldPosY * fieldWidth + fieldPosX;
	
	frc.set(field[pos].x * 0.1, field[pos].y * 0.1, field[pos].z * 0.1);  // scale here as values are pretty large.
	return frc;
}
/*
//------------------------------------------------------------------------------------
void vectorField::addInwardCircle(float x, float y, float radius, float strength){
	
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
	// first convert to pct:
	
	float pctx			= x / (float)externalWidth;
	float pcty			= y / (float)externalHeight;
	float radiusPct		= radius / (float)externalWidth;   
	
	// then, use them here: 
    int fieldPosX		= (int)(pctx * (float)fieldWidth);
    int fieldPosY		= (int)(pcty * (float)fieldHeight);
	float fieldRadius	= (float)(radiusPct * fieldWidth);
	
	// we used to do this search through every pixel, ie: 
	//    for (int i = 0; i < fieldWidth; i++){
	//    for (int j = 0; j < fieldHeight; j++){
	// but we can be smarter :)
	// now, as we search, we can reduce the "pixels" we look at by 
	// using the x y +/- radius.
	// use min and max to make sure we don't look over the edges 
	
	int startx	= MAX(fieldPosX - fieldRadius, 0);    
	int starty	= MAX(fieldPosY - fieldRadius, 0);
	int endx	= MIN(fieldPosX + fieldRadius, fieldWidth);
	int endy	= MIN(fieldPosY + fieldRadius, fieldHeight);
	
	
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
			
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
			if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
			
			if (distance < fieldRadius){
				
				float pct = 1.0f - (distance / fieldRadius);
				
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                field[pos].x += unit_px * strongness;
                field[pos].y += unit_py * strongness;
            }
        }
    }
}


//------------------------------------------------------------------------------------
void vectorField::addOutwardCircle(float x, float y, float radius, float strength){
	
	
	// x y and radius are in external dimensions.  Let's put them into internal dimensions:
	// first convert to pct:
	
	float pctx			= x / (float)externalWidth;
	float pcty			= y / (float)externalHeight;
	float radiusPct		= radius / (float)externalWidth;   
	
	// then, use them here: 
    int fieldPosX		= (int)(pctx * (float)fieldWidth);
    int fieldPosY		= (int)(pcty * (float)fieldHeight);
	float fieldRadius	= (float)(radiusPct * fieldWidth);
	
	// we used to do this search through every pixel, ie: 
	//    for (int i = 0; i < fieldWidth; i++){
	//    for (int j = 0; j < fieldHeight; j++){
	// but we can be smarter :)
	// now, as we search, we can reduce the "pixels" we look at by 
	// using the x y +/- radius.
	// use min and max to make sure we don't look over the edges 
	
	int startx	= MAX(fieldPosX - fieldRadius, 0);    
	int starty	= MAX(fieldPosY - fieldRadius, 0);
	int endx	= MIN(fieldPosX + fieldRadius, fieldWidth);
	int endy	= MIN(fieldPosY + fieldRadius, fieldHeight);

	
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){

            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
			if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
			
			if (distance < fieldRadius){
               
				float pct = 1.0f - (distance / fieldRadius);
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                field[pos].x -= unit_px * strongness;
                field[pos].y -= unit_py * strongness;
            }
        }
    }
}

//------------------------------------------------------------------------------------
void vectorField::addClockwiseCircle(float x, float y, float radius, float strength){
	
	
	
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
	// first convert to pct:
	
	float pctx			= x / (float)externalWidth;
	float pcty			= y / (float)externalHeight;
	float radiusPct		= radius / (float)externalWidth;   
	
	// then, use them here: 
    int fieldPosX		= (int)(pctx * (float)fieldWidth);
    int fieldPosY		= (int)(pcty * (float)fieldHeight);
	float fieldRadius	= (float)(radiusPct * fieldWidth);
	
	// we used to do this search through every pixel, ie: 
	//    for (int i = 0; i < fieldWidth; i++){
	//    for (int j = 0; j < fieldHeight; j++){
	// but we can be smarter :)
	// now, as we search, we can reduce the "pixels" we look at by 
	// using the x y +/- radius.
	// use min and max to make sure we don't look over the edges 
	
	int startx	= MAX(fieldPosX - fieldRadius, 0);    
	int starty	= MAX(fieldPosY - fieldRadius, 0);
	int endx	= MIN(fieldPosX + fieldRadius, fieldWidth);
	int endy	= MIN(fieldPosY + fieldRadius, fieldHeight);
	
	
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
			
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
			if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
			
			if (distance < fieldRadius){
				
				float pct = 1.0f - (distance / fieldRadius);
				
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                field[pos].x += unit_py * strongness;   /// Note: px and py switched, for perpendicular
                field[pos].y -= unit_px * strongness;
            }
        }
    }
}



//------------------------------------------------------------------------------------
void vectorField::addCounterClockwiseCircle(float x, float y, float radius, float strength){
	
	
	
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
	// first convert to pct:
	
	float pctx			= x / (float)externalWidth;
	float pcty			= y / (float)externalHeight;
	float radiusPct		= radius / (float)externalWidth;   
	
	// then, use them here: 
    int fieldPosX		= (int)(pctx * (float)fieldWidth);
    int fieldPosY		= (int)(pcty * (float)fieldHeight);
	float fieldRadius	= (float)(radiusPct * fieldWidth);
	
	// we used to do this search through every pixel, ie: 
	//    for (int i = 0; i < fieldWidth; i++){
	//    for (int j = 0; j < fieldHeight; j++){
	// but we can be smarter :)
	// now, as we search, we can reduce the "pixels" we look at by 
	// using the x y +/- radius.
	// use min and max to make sure we don't look over the edges 
	
	int startx	= MAX(fieldPosX - fieldRadius, 0);    
	int starty	= MAX(fieldPosY - fieldRadius, 0);
	int endx	= MIN(fieldPosX + fieldRadius, fieldWidth);
	int endy	= MIN(fieldPosY + fieldRadius, fieldHeight);
	
	
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
			
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
			if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
			
			if (distance < fieldRadius){
				
				float pct = 1.0f - (distance / fieldRadius);
				
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                field[pos].x -= unit_py * strongness;   /// Note: px and py switched, for perpendicular
                field[pos].y += unit_px * strongness;
            }
        }
    }
}


//------------------------------------------------------------------------------------
void vectorField::addVectorCircle(float x, float y, float vx, float vy, float radius, float strength){
	
	
	
	// x y and radius are in external dimensions.  Let's put them into internal dimensions:
	// first convert to pct:
	
	float pctx			= x / (float)externalWidth;
	float pcty			= y / (float)externalHeight;
	float radiusPct		= radius / (float)externalWidth;   
	
	// then, use them here: 
    int fieldPosX		= (int)(pctx * (float)fieldWidth);
    int fieldPosY		= (int)(pcty * (float)fieldHeight);
	float fieldRadius	= (float)(radiusPct * fieldWidth);
	
	// we used to do this search through every pixel, ie: 
	//    for (int i = 0; i < fieldWidth; i++){
	//    for (int j = 0; j < fieldHeight; j++){
	// but we can be smarter :)
	// now, as we search, we can reduce the "pixels" we look at by 
	// using the x y +/- radius.
	// use min and max to make sure we don't look over the edges 
	
	int startx	= MAX(fieldPosX - fieldRadius, 0);    
	int starty	= MAX(fieldPosY - fieldRadius, 0);
	int endx	= MIN(fieldPosX + fieldRadius, fieldWidth);
	int endy	= MIN(fieldPosY + fieldRadius, fieldHeight);
	
	
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
			
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
			if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
			
			if (distance < fieldRadius){
				
				float pct = 1.0f - (distance / fieldRadius);
                float strongness = strength * pct;
                field[pos].x += vx * strongness;   
                field[pos].y += vy * strongness;
            }
        }
    }
}

*/

//------------------------------------------------------------------------------------
//vectorField::~vectorField(){
//
//}
