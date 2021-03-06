#include "ofxTriangle.h"


void ofxTriangle::triangulate(ofPoint * pts, int nPts)
{
    
	Delaunay::Point tempP;
    vector< Delaunay::Point > v;
	
    int i;
	
    for( i=0; i< nPts; i++)
    {
        
        tempP[0] = pts[i].x;
        tempP[1] = pts[i].y;
		
        v.push_back(tempP);
    }
	
    delobject = new Delaunay(v);
    delobject->Triangulate();
	
    //triangles.clear();
    //nTriangles = 0;
	
    /*Delaunay::fIterator fit;
    for ( fit = delobject->fbegin(); fit != delobject->fend(); ++fit )
    {
        int pta = delobject->Org(fit);
        int ptb = delobject->Dest(fit);
        int ptc = delobject->Apex(fit);
		
		ofxTriangleIdData temp;
		temp.a = pta;
		temp.b = ptb;
		temp.c = ptc;
		
		temp.area = delobject->area(fit);
		
		triangleIds.push_back(temp);
		
    }
	*/
    //delete delobject;
}

void ofxTriangle::triangulate(vector < ofPoint > & pts)
{
    int bSize = pts.size();
    float maxi = bSize;
	
	Delaunay::Point tempP;
    vector< Delaunay::Point > v;
	
    int i;
	
    for( i=0; i< bSize; i++)
    {
        
        tempP[0] = pts[i].x;
        tempP[1] = pts[i].y;
		
        v.push_back(tempP);
    }
	
    delobject = new Delaunay(v);
    delobject->Triangulate();
	
    //triangles.clear();
    //nTriangles = 0;
	
    Delaunay::fIterator fit;
    for ( fit = delobject->fbegin(); fit != delobject->fend(); ++fit )
    {
        int pta = delobject->Org(fit);
        int ptb = delobject->Dest(fit);
        int ptc = delobject->Apex(fit);
		
		ofxTriangleIdData temp;
		temp.a = pta;
		temp.b = ptb;
		temp.c = ptc;
		
		temp.area = delobject->area(fit);
		
		triangleIds.push_back(temp);
	
    }
	
    delete delobject;
}


void ofxTriangle::triangulate(ofxCvBlob &cvblob, int resolution, int rdmPoints)
{
    blob = &cvblob;

    int bSize = blob->pts.size();
    float maxi = min(resolution, bSize);

    Delaunay::Point tempP;
    vector< Delaunay::Point > v;

    int i;

    for( i=0; i< maxi; i++)
    {
        int id = (int)( (float)i/maxi*bSize );

        tempP[0] = blob->pts[id].x;
        tempP[1] = blob->pts[id].y;

        v.push_back(tempP);
    }

    delobject = new Delaunay(v);
    delobject->Triangulate();

    //triangles.clear();
    //nTriangles = 0;

    Delaunay::fIterator fit;
    for ( fit = delobject->fbegin(); fit != delobject->fend(); ++fit )
    {
        int pta = delobject->Org(fit);
        int ptb = delobject->Dest(fit);
        int ptc = delobject->Apex(fit);

        int pta_id = (int)( ((float)pta/resolution)*bSize );
        int ptb_id = (int)( ((float)ptb/resolution)*bSize );
        int ptc_id = (int)( ((float)ptc/resolution)*bSize );

        ofPoint tr[3];
        tr[0] = ofPoint(blob->pts[pta_id].x, blob->pts[pta_id].y);
        tr[1] = ofPoint(blob->pts[ptb_id].x, blob->pts[ptb_id].y);
        tr[2] = ofPoint(blob->pts[ptc_id].x, blob->pts[ptc_id].y);

        if( isPointInsidePolygon(&blob->pts[0], blob->pts.size(), getTriangleCenter(tr) ) )
        {
            ofxTriangleData td;
            td.a = ofPoint(tr[0].x, tr[0].y);
            td.b = ofPoint(tr[1].x, tr[1].y);
            td.c = ofPoint(tr[2].x, tr[2].y);

            td.area = delobject->area(fit);

            triangles.push_back(td);
			
            nTriangles++;
        }
    }
	
    delete delobject;
}

void ofxTriangle::clear()
{
    triangles.clear();
    nTriangles = 0;
}

void ofxTriangle::addRdmPoint(vector<Delaunay::Point> * v)
{
    /*
    Delaunay::Point tempP;

    int px = ofRandom( blob->boundingRect.x, blob->boundingRect.width );
    int py = ofRandom( blob->boundingRect.y, blob->boundingRect.height );

    while ( !isPointInsidePolygon( &blob->pts[0], blob->pts.size(), ofPoint( px, py ) ) )
    {
        px = ofRandom( blob->boundingRect.x, blob->boundingRect.width );
        py = ofRandom( blob->boundingRect.y, blob->boundingRect.height );

        cout << px << " " << py << endl;
    }

    tempP[0] = px;
    tempP[1] = py;
    v->push_back(tempP);
    */
}

ofPoint ofxTriangle::getTriangleCenter(ofPoint *tr)
{
    float c_x = (tr[0].x + tr[1].x + tr[2].x) / 3;
    float c_y = (tr[0].y + tr[1].y + tr[2].y) / 3;

    return ofPoint(c_x, c_y);
}

bool ofxTriangle::isPointInsidePolygon(ofPoint *polygon,int N, ofPoint p)
{
    int counter = 0;
    int i;
    double xinters;
    ofPoint p1,p2;

    p1 = polygon[0];

    for (i=1;i<=N;i++)
    {
        p2 = polygon[i % N];
        if (p.y > MIN(p1.y,p2.y)) {
            if (p.y <= MAX(p1.y,p2.y)) {
                if (p.x <= MAX(p1.x,p2.x)) {
                    if (p1.y != p2.y) {
                        xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
                        if (p1.x == p2.x || p.x <= xinters)
                            counter++;
                    }
                }
            }
        }
        p1 = p2;
    }

  if (counter % 2 == 0)
    return false;
  else
    return true;
}

void ofxTriangle::draw(float x, float y)
{
    ofPushMatrix();
    ofTranslate(x, y, 0);
        draw();
    ofPopMatrix();
}

void ofxTriangle::draw()
{
    ofFill();

    for (int i=0; i<nTriangles; i++)
    {
        ofSetColor(ofRandom(0, 0xffffff));
        ofTriangle( triangles[i].a.x, triangles[i].a.y,
                    triangles[i].b.x, triangles[i].b.y,
                    triangles[i].c.x, triangles[i].c.y);
    }

}
