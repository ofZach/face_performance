#include "lamaeParticle.h"
#include "ofMain.h"
#include "ofxVectorMath.h"

//------------------------------------------------------------
particle::particle(){
	setInitialCondition(0,0,0,0);
	damping = 0.14f;
	colorDropOffRate = 0.99f;
	
}

//------------------------------------------------------------
void particle::resetForce(){
    // we reset the forces every frame
    frc.set(0,0,0);
}

//------------------------------------------------------------
void particle::addForce(float x, float y, float z){
    // add in a force in X and Y for this frame.
    frc.x = frc.x + x;
    frc.y = frc.y + y;
	frc.z = frc.z + z;
}

//------------------------------------------------------------
void particle::addDampingForce(){
	
	// the usual way to write this is  vel *= 0.99
	// basically, subtract some part of the velocity 
	// damping is a force operating in the oposite direction of the 
	// velocity vector
	
    frc.x = frc.x - vel.x * damping;
    frc.y = frc.y - vel.y * damping;
	frc.z = frc.z - vel.z * damping;
}

//------------------------------------------------------------
void particle::setInitialCondition(float px, float py, float vx, float vy){
    pos.set(px,py,0);
	vel.set(vx,vy,0);
}

//------------------------------------------------------------
void particle::setInitialCondition(float px, float py,  float pz, float vx, float vy, float vz){
    pos.set(px,py,pz);
	vel.set(vx,vy,vz);
}

//------------------------------------------------------------
void particle::update(){	
	vel = vel + frc;
	pos = pos + vel;
	
	color.x *= colorDropOffRate;
	color.y *= colorDropOffRate;
	color.z *= colorDropOffRate;
}
//------------------------------------------------------------
void particle::addRepulsionForce(particle &p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is: 
	ofxVec3f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y, p.pos.z);
	
	// ----------- (2) calculate the difference & length 
	
	ofxVec3f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x + diff.x * scale * pct;
        frc.y = frc.y + diff.y * scale * pct;
		frc.z = frc.z + diff.z * scale * pct;
		p.frc.x = p.frc.x - diff.x * scale * pct;
        p.frc.y = p.frc.y - diff.y * scale * pct;
		p.frc.z = p.frc.z - diff.z * scale * pct;
    }
}

//------------------------------------------------------------
void particle::draw(){
	glPushMatrix();
	glTranslatef(pos.x, pos.y,pos.z);
    ofCircle(0,0, 3);
	glPopMatrix();
}

