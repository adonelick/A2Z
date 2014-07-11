#include "stdafx.h"
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;

static void help()
{
	printf("\nThis program demonstrates using features2d detector, descriptor extractor and simple matcher\n"
		"Using the SURF desriptor:\n"
		"\n"
		"Usage:\n matcher_simple <image1> <image2>\n");
}

int main2(int argc, char** argv)
{
	//if(argc != 3)
	//{
	//    help();
	//    return -1;
	//}

	Mat img1 = imread("images/img1_3.tiff", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("images/img2_3.tiff", CV_LOAD_IMAGE_GRAYSCALE);
	if (img1.empty() || img2.empty())
	{
		printf("Can't read one of the images\n");
		return -1;
	}

	// detecting keypoints
	SurfFeatureDetector detector(400);
	vector<KeyPoint> keypoints1, keypoints2;
	detector.detect(img1, keypoints1);
	detector.detect(img2, keypoints2);

	// computing descriptors
	SurfDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	extractor.compute(img1, keypoints1, descriptors1);
	extractor.compute(img2, keypoints2, descriptors2);

	//// matching descriptors
	//BFMatcher matcher(NORM_L2);
	//vector<DMatch> matches;
	//matcher.match(descriptors1, descriptors2, matches);

	//// drawing the results
	//namedWindow("matches", 1);
	//Mat img_matches;
	//drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
	//imshow("matches", img_matches);
	//waitKey(0);

	//return 0;

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors1, descriptors2, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors1.rows; i++)
	{
		if (matches[i].distance < 10 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//-- Draw only "good" matches
	Mat img_matches;
	drawMatches(img1, keypoints1, img2, keypoints2,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	imshow("Good Matches", img_matches);

	for (int i = 0; i < good_matches.size(); i++)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}

	waitKey(0);

	return 0;
}
