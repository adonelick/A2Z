#include "stdafx.h"
#include "canny.h"
#include <iostream>

using namespace cv;
using namespace std;

CannyDetector::CannyDetector(int edgeThresh, int lowThreshold, int max_lowThreshold, int ratio, int kernel_size, char* window_name)
{
	this->window_name = window_name;
	this->edgeThresh = edgeThresh;
	this->lowThreshold = lowThreshold;
	this->max_lowThreshold = max_lowThreshold;
	this->ratio = ratio;
	this->kernel_size = kernel_size;
}

/**
* @function CannyThreshold
* @brief Trackbar callback - Canny thresholds input with a ratio 1:3
*/
void CannyDetector::UpdateThreshold(Mat source, Mat dst)
{
	/// Reduce noise with a kernel 3x3
	blur(source, detected_edges, Size(3, 3));

	/// Canny detector
	Canny(this->detected_edges, this->detected_edges, this->lowThreshold, this->lowThreshold*ratio, this->kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);


	source.copyTo(dst, this->detected_edges);

	imshow(this->window_name, dst);
}

void CannyDetector::ThresholdCtrl()
{
	UpdateThreshold(src, dst);
}

/** @function main */
Mat CannyDetector::FindEdges(Mat source)
{
	/// Load an image
	//src = imread( argv[1] );
	//src = imread( "images/img1_7.tiff" );
	//src2 = imread( "images/img2_7.tiff" );
	src = source;

	if (!src.data)
	{
		return source;
	}

	/// Create a matrix of the same type and size as src (for dst) 
	dst.create(src.size(), src.type());

	/// Convert the image to grayscale

	/// Create a window
	namedWindow(this->window_name, CV_WINDOW_AUTOSIZE);

	/// Create a Trackbar for user to enter threshold
	//createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold);

	/// Show the image
	ThresholdCtrl();

	return dst;
}