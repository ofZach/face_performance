/*
 *  GaussianProcess.cpp
 *  eyeWriterCam
 *
 *  Gaussian process regression for appearance-based gaze estimation
 *  ref. Rasmussen and Williams, "Gaussian Processes for Machine Learning"
 *  http://www.gaussianprocess.org/gpml/chapters/
 *
 *  Created by Y. Sugano on 11/01/25.
 *
 */

#pragma once

#include <vector>

#include "cv.h"

class CGaussianProcess
{
public:
	CGaussianProcess(void);
	~CGaussianProcess(void);

	void AddExampler(IplImage *image, double label);
	double GetMean(IplImage *testimage, double scale, double sigma);

	// leave-one-out cross validation
	double GetLOOLPP(double noise, double scale, double sigma);
	void GetDerLOOLPP(double noise, double scale, double sigma, double *d_noise, double *d_scale, double *d_sigma);

	void SetUp();
	void SetUp(double mean, double scale);
	void Clear();
	void UpdateGPMatrices(double noise, double scale, double sigma);

private:
	std::vector<IplImage*> m_ExamplerImages;
	std::vector<double> m_ExamplerLabels;
	
	void GetFourierMagnitudeEye(IplImage *src, IplImage *dst);

	double m_Mean, m_Scale;
	CvMat *m_GP_L, *m_GP_y, *m_GP_Alpha;
	
	// cholesky solver
	static const bool m_CholeskyUpper = false;
	void CholeskyDecomp(CvMat *K, CvMat *L);
	void CholeskySolve(CvMat *L, CvMat *y, CvMat *alpha);

	//void GetCovarianceMatrix(CvMat *K);
	void GetCovarianceMatrix(CvMat *K, double noise, double scale, double sigma);

	//double CalcCovariance(IplImage *img1, IplImage *img2);
	double CalcCovariance(IplImage *img1, IplImage *img2, double scale, double sigma);
};

class CGazeEstimator
{
public:
	CGazeEstimator(void);
	~CGazeEstimator(void);

	void AddExampler(IplImage *image, double labelx, double labely);
	void Reset();
	void Update(int areaWidth, int areaHeight);
	
	bool GetMean(IplImage *testimage, double mean[2]);

	double GetGradient(double noisex, double noisey, double scale, double sigma, 
		double *d_noisex, double *d_noisey, double *d_scale, double *d_sigma);

private:
	CGaussianProcess m_GPx, m_GPy;
	
	bool m_isUpdated;

	// parameters
	double m_Scale, m_Sigma, m_NoiseX, m_NoiseY;

	void UpdateParameters();
};