#ifndef UTIL_H
#define UTIL_H

#include "cv.h"
#include "shapevec.h"
#include "shapemodel.h"
#include <vector>
using cv::Mat;

using std::vector;

//! face detect
cv::Rect findOneFace(const Mat &img);
vector< cv::Rect > findAllFaces(const Mat &img);

// fill a convex poly by a point vector.
void fillConvexFromPoints(Mat &img, vector< cv::Point > &vP, cv::Scalar s);

Mat drawMarkPointsOnImg(Mat &img, const vector< cv::Point > &vP, ShapeInfo &shapeInfo, 
                          bool drawInPlace = false);

#endif
