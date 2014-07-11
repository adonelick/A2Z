#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

class CannyDetector {
public:
	Mat FindEdges(Mat source);

	CannyDetector(int edgeThresh = 1, int lowThreshold = 0, int max_lowThreshold = 100, int ratio = 4, int kernel_size = 3, char* window_name = "Edge Map");
private:
	Mat src, src_gray;
	Mat dst, detected_edges;

	int edgeThresh;
	int lowThreshold;
	int max_lowThreshold;
	int ratio;
	int kernel_size;
	char* window_name;

	void UpdateThreshold(Mat src_gray, Mat dst);
	void ThresholdCtrl();
};