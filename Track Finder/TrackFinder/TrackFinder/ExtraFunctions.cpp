// These are extra functions from the 
// clinic's trackFinder software


// old functions that are no longer used
void displayImagePair(int x, int y, float alpha);
float scoreTranslated(Mat img1, Mat img2, int minX, int minY, int maxX, int maxY);
void SurfDetect(Mat img1, Mat img2);
void SiftDetect(Mat img1, Mat img2);
void MSERDetect(Mat img1, Mat img2);
void MSERCompare(Mat img1, Mat img2);
void moveInterface();
void trackInterface();
Mat translateImage(Mat src, float dx, float dy);
float scoreImages(int scoreType);
float subtractScore(Mat img1, Mat img2);
int acceptInput();
float scorePoint(int x, int y, int contrastWidth, int theta, int avgIntensity, int lContrast, int rContrast);






void displayImagePair(int x, int y, float alpha) {
	float alpha_f = 1.0f*alpha / 100;
	Size img1Size = img1.size();
	Size img2Size = img2.size();
	Mat bgImg(img1Size.height*1.5, img1Size.width*1.5, CV_8UC3);
	Mat under(bgImg, Rect(img1Size.width / 4, img1Size.height / 4, img1Size.width, img1Size.height));
	//Mat over(bgImg, Rect(img1Size.width/4-50+x, img1Size.height/4-50+y, img1Size.width, img1Size.height));
	img1.copyTo(under);

	Mat overlay = bgImg.clone();
	Mat over(overlay, Rect(img1Size.width / 4 - 50 + x, img1Size.height / 4 + 50 - y, img1Size.width, img1Size.height));
	img2.copyTo(over);
	addWeighted(overlay, alpha_f, bgImg, 1 - alpha_f, 0, bgImg);
	namedWindow("Moved Image", CV_WINDOW_AUTOSIZE);
	imshow("Moved Image", bgImg);
}


float scorePoint(int x, int y, int contrastWidth, int theta, int avgIntensity, int lContrast, int rContrast) {
	int step = img1.step;
	int channels = img1.channels();
	int contrastScore = img1.data[y*step + x*channels] - avgIntensity;
	cout << "c " << contrastScore << endl;
	cout << "L " << lContrast << endl;
	cout << "R " << rContrast << endl;
	int score = 0;
	return score;
}



void moveInterface() {
	trackbar_x_val = 0;
	trackbar_y_val = 0;

	namedWindow("Move Image");
	resizeWindow("Move Image", 500, 130);
	createTrackbar("X Position", "Move Image", &trackbar_x_val, trackbar_x_max, on_trackbar_x);
	createTrackbar("Y Position", "Move Image", &trackbar_y_val, trackbar_y_max, on_trackbar_y);
	createTrackbar("Alpha", "Move Image", &trackbar_a_val, trackbar_a_max, on_trackbar_a);
	setTrackbarPos("Alpha", "Move Image", 50);

	displayImagePair(trackbar_x_val, trackbar_y_val, trackbar_a_val);
	waitKey(0);
}




void trackInterface() {
	trackbar_x_val = 0;
	trackbar_y_val = 0;
	trackbar_degree_val = 0;
	trackbar_x_max = img1.size().width;
	trackbar_y_max = img1.size().height;

	namedWindow("Move Track");
	resizeWindow("Move Track", 500, 130);
	createTrackbar("X Position", "Move Track", &trackbar_x_val, trackbar_x_max, on_trackbar_x);
	createTrackbar("Y Position", "Move Track", &trackbar_y_val, trackbar_y_max, on_trackbar_y);
	createTrackbar("Angle", "Move Track", &trackbar_degree_val, trackbar_degree_max, on_trackbar_degree);
	setTrackbarPos("Angle", "Move Track", 90);

	displayTrackImage(trackbar_x_val, trackbar_y_val, trackbar_degree_val, 5, 30);
	waitKey(0);
}





void SurfDetect(Mat img1, Mat img2) {
	//-- Step 1: Detect the keypoints using SURF Detector
	int refHessian = 200;
	int transHessian = 800;

	SurfFeatureDetector refDetector(refHessian);
	SurfFeatureDetector transDetector(transHessian);

	std::vector<KeyPoint> keypoints_1, keypoints_2;

	refDetector.detect(img1, keypoints_1);
	transDetector.detect(img2, keypoints_2);

	//-- Draw keypoints
	Mat img_keypoints_1; Mat img_keypoints_2;

	drawKeypoints(img1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//-- Show detected (drawn) keypoints
	imshow("Keypoints 1", img_keypoints_1);
	imshow("Keypoints 2", img_keypoints_2);

	waitKey(0);
}



void MSERDetect(Mat img, Mat img2) {
	////-- Step 1: Detect the keypoints using MSER Detector
	//Mat ellipses;

	//std::vector<std::vector<Point>> contours;

	//MSER()( img, contours );

	////-- Draw keypoints

	//for( int i = (int)contours.size()-1; i >= 0; i-- )
	//   {
	//       const vector<Point>& r = contours[i];
	//  //     for ( int j = 0; j < (int)r.size(); j++ )
	//  //     {
	//  //         Point pt = r[j];
	//		//cout << pt << endl;
	//		//cout << img.size() << endl;
	//  //         img.at<Vec3b>(pt) = bcolors[i%9];
	//  //     }

	//       // find ellipse
	//       RotatedRect box = fitEllipse( r );
	//       box.angle=(float)CV_PI/2-box.angle;

	//       ellipse( ellipses, box, Scalar(196,255,255), 2 );
	//   }

	////-- Show detected (drawn) keypoints
	//imshow("ellipses", ellipses);

	//waitKey(0);
	MSER ms;
	vector<vector<Point>> regions1, regions2;
	ms(img, regions1, Mat());
	ms(img2, regions2, Mat());
	for (int i = 0; i < regions1.size(); i++)
	{
		ellipse(img, fitEllipse(regions1[i]), Scalar(255));
	}
	for (int i = 0; i < regions2.size(); i++)
	{
		ellipse(img2, fitEllipse(regions2[i]), Scalar(255));
	}
	imshow("mser1", img);
	imshow("mser2", img2);
	waitKey(0);
	return;
}

void MSERCompare(Mat img1, Mat img2) {
	//-- Step 1: Detect the keypoints using MSER Detector

	MserFeatureDetector refDetector;
	MserFeatureDetector transDetector;

	std::vector<KeyPoint> keypoints_1, keypoints_2;

	refDetector.detect(img1, keypoints_1);
	transDetector.detect(img2, keypoints_2);

	//-- Draw keypoints
	Mat img_keypoints_1; Mat img_keypoints_2;

	drawKeypoints(img1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//-- Show detected (drawn) keypoints
	imshow("Keypoints 1", img_keypoints_1);
	imshow("Keypoints 2", img_keypoints_2);

	waitKey(0);
}


void SiftDetect(Mat img1, Mat img2) {
	//-- Step 1: Detect the keypoints using SURF Detector
	int refHessian = 200;
	int transHessian = 800;

	SiftFeatureDetector refDetector;
	SiftFeatureDetector transDetector;

	std::vector<KeyPoint> keypoints_1, keypoints_2;

	refDetector.detect(img1, keypoints_1);
	transDetector.detect(img2, keypoints_2);

	//-- Draw keypoints
	Mat img_keypoints_1; Mat img_keypoints_2;

	drawKeypoints(img1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//-- Show detected (drawn) keypoints
	imshow("Keypoints 1", img_keypoints_1);
	imshow("Keypoints 2", img_keypoints_2);

	waitKey(0);
}



float scoreImages(int scoreType) {
	switch (scoreType)
	{
	case 0: {
				float score = subtractScore(img1, img2);
				cout << score << " at x" << trackbar_x_val - 50 << " y" << trackbar_y_val - 50 << endl;
				return score;
	}
	}

	return -1;
}



float subtractScore(Mat img1, Mat img2) {
	int height, width, step, channels;
	height = img1.rows;
	width = img1.cols;
	step = img1.step;
	channels = img1.channels();
	uchar* data1 = img1.data;
	uchar* data2 = img2.data;

	float imgDiff = 0;

	int minY = max(0, 50 - trackbar_y_val);
	int maxY = min(height - 1, height + 50 - trackbar_y_val);
	int minX = max(0, trackbar_x_val - 50);
	int maxX = min(width - 1, width + trackbar_x_val - 50);

	//cout << "min : x1 " << minX << " y1 " << minY << ", x2 " << minX-trackbar_x_val+50 << " y2 " << minY-50+trackbar_y_val << endl;
	//cout << "max : x1 " << maxX << " y1 " << maxY << ", x2 " << maxX-trackbar_x_val+50 << " y2 " << maxY-50+trackbar_y_val << endl;


	for (int i = minY; i < maxY; i++) {
		for (int j = minX; j < maxX; j++) {
			for (int k = 0; k < channels; k++) {
				imgDiff += abs(data1[i*step + j*channels + k] - data2[(i - 50 + trackbar_y_val)*step + (j - trackbar_x_val + 50)*channels + k]);
			}
		}
	}

	//cout << "image1 X: " << minX << ", " << maxX << endl;
	//cout << "image1 Y: " << minY << ", " << maxY << endl;
	//cout << "image2 X: " << minX-trackbar_x_val+50 << ", " << maxX-trackbar_x_val+50 << endl;
	//cout << "image2 Y: " << minY+trackbar_y_val-50 << ", " << maxY+trackbar_y_val-50 << endl;

	// overlapping area
	//cout << width-abs(50-trackbar_x_val) << ", " << height-abs(50-trackbar_y_val) << endl;

	return imgDiff / ((height - abs(50 - trackbar_y_val))*(width - abs(50 - trackbar_x_val))*channels);
}


Mat translateImage(Mat src, float dx, float dy) {
	Point2f srcTri[3], dstTri[3];
	Mat warp_mat = cvCreateMat(2, 3, CV_32FC1);
	Mat dst = Mat::zeros(src.rows, src.cols, src.type());

	srcTri[0].x = 0;
	srcTri[0].y = 0;
	srcTri[1].x = src.cols - 1.0;
	srcTri[1].y = 0;
	srcTri[2].x = 0;
	srcTri[2].y = src.rows - 1.0;

	dstTri[0].x = -dx;
	dstTri[0].y = -dy;
	dstTri[1].x = src.cols - dx;
	dstTri[1].y = -dy;
	dstTri[2].x = -dx;
	dstTri[2].y = src.rows - dy;

	warp_mat = getAffineTransform(srcTri, dstTri);
	warpAffine(src, dst, warp_mat, dst.size());

	return dst;
}

void on_trackbar_x(int, void*) {
	if (currentMode == OVERLAY) {
		displayImagePair(trackbar_x_val, trackbar_y_val, trackbar_a_val);
		scoreImages(0);
	}
	else
		displayTrackImage(trackbar_x_val, trackbar_y_val, trackbar_degree_val, 5, 30);
	return;
}

void on_trackbar_y(int, void*) {
	if (currentMode == OVERLAY) {
		displayImagePair(trackbar_x_val, trackbar_y_val, trackbar_a_val);
		scoreImages(0);
	}
	else
		displayTrackImage(trackbar_x_val, trackbar_y_val, trackbar_degree_val, 5, 30);
	//scoreImages(0);
	return;
}

void on_trackbar_a(int, void*) {
	displayImagePair(trackbar_x_val, trackbar_y_val, trackbar_a_val);
	return;
}

void on_trackbar_degree(int, void*) {
	displayTrackImage(trackbar_x_val, trackbar_y_val, trackbar_degree_val, 5, 30);
	return;
}



static const Vec3b bcolors[] =
{
	Vec3b(0, 0, 255),
	Vec3b(0, 128, 255),
	Vec3b(0, 255, 255),
	Vec3b(0, 255, 0),
	Vec3b(255, 128, 0),
	Vec3b(255, 255, 0),
	Vec3b(255, 0, 0),
	Vec3b(255, 0, 255),
	Vec3b(255, 255, 255)
};
