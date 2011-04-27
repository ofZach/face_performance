/*!
* ofxTriangle by kikko.fr
* -> C++ Triangle warper by Piyush Kumar
* -> Point inside polygon by Theo
*/

#ifndef OFXTRIANGLE_H_INCLUDED
#define OFXTRIANGLE_H_INCLUDED

#include "ofMain.h"
#include "ofxOpenCv.h"
#include <del_interface.hpp>

using namespace tpp;
using namespace std;

typedef struct
{
    ofPoint a;
    ofPoint b;
    ofPoint c;

    float area;

} ofxTriangleData;

typedef struct {
	int a,b,c;
	float area;
} ofxTriangleIdData;


class ofxTriangle
{
    public :

        ofxTriangle(){}
        ~ofxTriangle(){}

        // Triangulate a openCV blob
		void triangulate(ofPoint * pts, int nPts);
		void triangulate(vector < ofPoint > & pts);
        void triangulate(ofxCvBlob &cvblob, int resolution = 50, int rdmPoints = 0);

        // Triangulate a vector of ofPoint
        void addRdmPoint(vector<Delaunay::Point> * v);

        ofPoint getTriangleCenter(ofPoint *tr);
        bool isPointInsidePolygon(ofPoint *polygon,int N, ofPoint p);

        void draw();
        void draw(float x, float y);
        void clear();


        Delaunay* delobject;

        int nTriangles;
        vector <ofxTriangleData> triangles;
		vector <ofxTriangleIdData> triangleIds;
	
    private :

        ofxCvBlob* blob;

};

#endif // OFXTRIANGLE_H_INCLUDED
