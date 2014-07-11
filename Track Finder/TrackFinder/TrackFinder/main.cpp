#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "canny.h"
#include <opencv2/nonfree/features2d.hpp>
#include <iostream>
#include <cmath>
#include <list>
#include <fstream>
#include <Windows.h>





using namespace cv;
using namespace std;

struct lineStruct {
	int length;
	Point point1;
	Point point2;
	Scalar color;
	int thickness;
};
//Mat invertImage(Mat img); 
Mat grayscaleLuminosity(Mat src);

void displayTrackImage(int x, int y, int thetaR, int trackWidth, int contrastWidth);
void displayBestTracks(int trackWidth, int contrastWidth, int numTracks, int similarityTolerance, string filename);
lineStruct findTrackOnLine(int x, int y, int thetaR, int trackWidth, int contrastWidth);
lineStruct findTrackOnLine2(int x, int y, int thetaR, int trackWidth, int contrastWidth); //potential new algorithm, discontinued because code freeze
bool tooSimilar(lineStruct line1, lineStruct line2, int tolerance); //checks that lines are not too similar
void outputCSV(string filename, list<lineStruct> trackList);

// trackbar variables for translation
int trackbar_x_val;
int trackbar_x_max = 100;
void on_trackbar_x(int, void*);
int trackbar_y_val;
int trackbar_y_max = 100;
void on_trackbar_y(int, void*);
int trackbar_a_val;
int trackbar_a_max = 100;
void on_trackbar_a(int, void*);
int trackbar_degree_val;
int trackbar_degree_max = 179;
void on_trackbar_degree(int, void*);


Mat img1;
Mat img2;
Mat bgImg;

enum mode {
	OVERLAY,
	TRACK,
	AUTO_TRACK
};

mode currentMode;

int main(int argc, const char** argv)
{
	cout << argv[1] << endl;
	img1 = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	img1 = grayscaleLuminosity(img1);

	if (!img1.data) //check whether the image is loaded or not
	{
		cout << "Error : Failed to load image..!!" << endl;
		system("pause"); //wait for a key press
		return -1;
	}

	int trackWidth, contrastWidth, numTracks, similarityTolerance;
	ifstream fin;
	string txt;
	fin.open("cfg.txt");
	fin >> trackWidth >> contrastWidth >> numTracks >> similarityTolerance;
	fin.close();

	currentMode = AUTO_TRACK;

	switch (currentMode) {
	case 0:
		//moveInterface();
		break;
	case 1:
		//trackInterface();
		break;
	case 2:
		displayBestTracks(trackWidth, contrastWidth, numTracks, similarityTolerance, argv[1]);
		break;
	default:
		cout << "No mode selected, exiting..." << endl;
		break;
	}

	return 0;
}

void outputCSV(string filename, list<lineStruct> trackList) {
	ofstream CSVFile;
	unsigned found = filename.find_last_of(".");
	filename = filename.substr(0, found);
	filename.replace(found, string::npos, ".csv");
	CSVFile.open(filename);
	list<lineStruct>::iterator it = trackList.begin();
	for (int i = 0; i < trackList.size() - 1; i++) {
		CSVFile << (*it).point1.x << ", " << (*it).point1.y << ", " << (*it).point2.x << ", " << (*it).point2.y << ", " << "1";
		CSVFile << endl;
		it++;
	}
	CSVFile.close();
	return;
}


void displayTrackImage(int x, int y, int thetaR, int trackWidth, int contrastWidth) {
	Mat trackImg;
	int width = img1.size().width;
	int height = img1.size().height;
	float theta = thetaR*M_PI / 180;
	float length = sqrt(pow(height, 2) + pow(width, 2));



	img1.copyTo(trackImg);
	cv::line(trackImg, Point(x, y), Point(x, y), Scalar(0, 0, 0), 20);
	cv::line(trackImg, Point(x - length*cos(theta), y + length*sin(theta)), Point(x + length*cos(theta), y - length*sin(theta)), Scalar(0, 0, 0), 5);

	lineStruct line = findTrackOnLine(x, y, thetaR, trackWidth, contrastWidth);
	cv::line(trackImg, line.point1, line.point2, line.color, line.thickness);
	cv::line(trackImg, Point(line.point1.x + contrastWidth / 2 * cos(M_PI / 2 - theta), line.point1.y + contrastWidth / 2 * sin(M_PI / 2 - theta)), Point(line.point2.x + contrastWidth / 2 * cos(M_PI / 2 - theta), line.point2.y + contrastWidth / 2 * sin(M_PI / 2 - theta)), Scalar(255, 0, 0), 2, 0);
	cv::line(trackImg, Point(line.point1.x - contrastWidth / 2 * cos(M_PI / 2 - theta), line.point1.y - contrastWidth / 2 * sin(M_PI / 2 - theta)), Point(line.point2.x - contrastWidth / 2 * cos(M_PI / 2 - theta), line.point2.y - contrastWidth / 2 * sin(M_PI / 2 - theta)), Scalar(255, 0, 0), 2, 0);


	//imshow("Track Image", trackImg);
	return;
}

void displayBestTracks(int trackWidth, int contrastWidth, int numTracks, int similarityTolerance, string filename) {
	Mat trackImg;
	img1.copyTo(trackImg);
	//float length = sqrt(pow(img1.size().width, 2) + pow(img2.size().width, 2));
	int length = img1.size().width;
	lineStruct currentLine;
	vector<lineStruct> trackArray;
	list<lineStruct> completeTrackList;
	list<lineStruct> finalTrackList;
	list<lineStruct>::iterator it;
	list<lineStruct>::iterator compareIt;
	list<lineStruct>::iterator finalIt;

	for (int trackNum = 0; trackNum < numTracks; trackNum++) {
		lineStruct emptyLine = { 0, Point(0, 0), Point(0, 0), Scalar(0, 0, 0), 0 };
		trackArray.push_back(emptyLine);
	}

	lineStruct line1 = { 0, Point(0, 0), Point(0, 0), Scalar(0, 0, 0), 0 };
	lineStruct line2 = { 0, Point(0, 0), Point(0, 0), Scalar(0, 0, 0), 0 };
	lineStruct line3 = { 0, Point(0, 0), Point(0, 0), Scalar(0, 0, 0), 0 };
	for (int theta = 0; theta < 180; theta++) {
		for (int i = 0; i <= length; i++) {
			if (theta <= 90)
				currentLine = findTrackOnLine(i, i, theta, trackWidth, contrastWidth);
			else
				currentLine = findTrackOnLine(img1.size().width - i, i, theta, trackWidth, contrastWidth);

			if (completeTrackList.size() == 0) {
				completeTrackList.push_back(currentLine);
			} else {
				it = completeTrackList.begin();
				for (int trackIndex = 0; it != completeTrackList.end(); trackIndex++) {
					if (currentLine.length > (*it).length) {
						completeTrackList.insert(it, currentLine);
						break;
					}
					++it;
				}
			}


			if (completeTrackList.size() > numTracks * 10) {
				it = completeTrackList.begin();
				for (int trackIndex = 0; trackIndex + 1 < completeTrackList.size(); trackIndex++) {
					compareIt = it;
					compareIt++;
					for (int compareIndex = trackIndex + 1; compareIndex + 1 < completeTrackList.size(); compareIndex++) {
						if (tooSimilar(*it, *compareIt, similarityTolerance)) {
							compareIt = completeTrackList.erase(compareIt);
						}
						else {
							compareIt++;
						}
					}
					it++;
				}
			}

		}
		cout << theta << "/179" << endl;
	}

	if (completeTrackList.size() > numTracks) {
		it = completeTrackList.begin();
		for (int trackIndex = 0; trackIndex + 1 < completeTrackList.size(); trackIndex++) {
			compareIt = it;
			compareIt++;
			for (int compareIndex = trackIndex + 1; compareIndex + 1 < completeTrackList.size(); compareIndex++) {
				if (tooSimilar(*it, *compareIt, similarityTolerance)) {
					compareIt = completeTrackList.erase(compareIt);
				}
				else {
					compareIt++;
				}
			}
			it++;
		}
	}

	it = completeTrackList.begin();
	for (int i = 0; i < completeTrackList.size(); i++) {
		cv::line(trackImg, (*it).point1, (*it).point2, (*it).color, (*it).thickness);
		++it;
	}

	imshow("Track Image", trackImg);

	outputCSV(filename, completeTrackList);
	// not using overlay/final output since it displays in labview ui
	bool run = true;
	bool overlay = true;
	int input = 0;
	input = waitKey(0);
	while (run)
	{
		if (input == 113)
			return;
		else if (input == 111) {
			overlay = !overlay;
		}

		if (overlay == false) {
			imshow("Track Image", img1);
			input = waitKey(0);
		}
		else {
			imshow("Track Image", trackImg);
			input = waitKey(0);
		}

		if (input == 27) {
			cvDestroyWindow("Track Image");
			run = false;
		}

	}
	return;
}


bool tooSimilar(lineStruct line1, lineStruct line2, int tolerance) {
	float score = 0;
	float score1 = 0;
	float score2 = 0;

	int dist1 = 0;
	int dist2 = 0;

	score1 += sqrt(pow((line1.point1.x - line2.point1.x), 2) + pow((line1.point1.y - line2.point1.y), 2));
	score1 += sqrt(pow((line1.point2.x - line2.point2.x), 2) + pow((line1.point2.y - line2.point2.y), 2));

	score2 += sqrt(pow((line1.point1.x - line2.point2.x), 2) + pow((line1.point1.y - line2.point2.y), 2));
	score2 += sqrt(pow((line1.point2.x - line2.point1.x), 2) + pow((line1.point2.y - line2.point1.y), 2));

	score = min(score1, score2);

	if (score1 < score2) {
		dist1 = sqrt(pow((line1.point1.x - line2.point1.x), 2) + pow((line1.point1.y - line2.point1.y), 2));
		dist2 = sqrt(pow((line1.point2.x - line2.point2.x), 2) + pow((line1.point2.y - line2.point2.y), 2));
	}
	else {
		dist1 = sqrt(pow((line1.point1.x - line2.point2.x), 2) + pow((line1.point1.y - line2.point2.y), 2));
		dist2 = sqrt(pow((line1.point2.x - line2.point1.x), 2) + pow((line1.point2.y - line2.point1.y), 2));
	}

	//cout << "Score: " << score << ", Distance 1: " << dist1 << ", Distance 2: " << dist2 << endl;

	if (score > tolerance && dist1 > tolerance && dist2 > tolerance)
		return false;
	else
		return true;
}

lineStruct findTrackOnLine(int x, int y, int thetaR, int trackWidth, int contrastWidth) {
	int width = img1.size().width;
	int height = img1.size().height;
	float theta = thetaR*M_PI / 180;
	float length = sqrt(pow(height, 2) + pow(width, 2));
	int tolerance = 5;
	int cTolerance = 5;
	int badPixels = 0;

	int newX, newY;
	int longestRun = 0;
	int currentRun = 0;
	int longX1 = 0, longX2 = 0, longY1 = 0, longY2 = 0;
	int currentX, currentY;
	int step = img1.step;
	int channels = img1.channels();
	int avgIntensity = mean(img1)[0];

	for (int i = -length; i < length; i++) {
		newX = x - i*cos(theta);
		newY = i*sin(theta) + y;
		int lContrast = 0;
		int rContrast = 0;

		//constrain within rectangular image
		if (0 <= newX && newX < width && 0 <= newY && newY < height) {
			//takes trackWidth into account (currently not weighted)
			int intensity = 0;
			int pixelCount = 0;
			for (int j = 0; j < trackWidth; j++) {
				int widthOffset = -trackWidth / 2 + j;
				int tempX = newX + widthOffset*cos(M_PI / 2 - theta);
				int tempY = newY + widthOffset*sin(M_PI / 2 - theta);
				if (0 <= tempX && tempX < width && 0 <= tempY && tempY < height) {
					pixelCount += 1;
					intensity += img1.data[tempY*step + tempX*channels];
				}
			}


			intensity = intensity / pixelCount;

			//compare intensity at contrastWidth
			for (int j = 0; j <= contrastWidth; j += contrastWidth) {
				int contrastOffset = -contrastWidth / 2 + j;
				int tempX = newX + contrastOffset*cos(M_PI / 2 - theta);
				int tempY = newY + contrastOffset*sin(M_PI / 2 - theta);
				if (0 <= tempX && tempX < width && 0 <= tempY && tempY < height) {
					if (j == 0)
						lContrast = img1.data[tempY*step + tempX*channels] - intensity;
					else
						rContrast = img1.data[tempY*step + tempX*channels] - intensity;
				}
			}


			//####loosen requirements so one bad pixel doesn't ruin the run
			if (intensity < avgIntensity && abs(lContrast) > cTolerance && abs(rContrast) > cTolerance) {
				badPixels = 0;

				if (currentRun == 0) {
					currentX = newX;
					currentY = newY;
				}
				currentRun += 1;
			}
			else {
				badPixels += 1;
				if (badPixels > tolerance) {
					if (currentRun > longestRun) {
						longestRun = currentRun;
						longX1 = currentX;
						longY1 = currentY;
						longX2 = newX;
						longY2 = newY;
					}
					currentRun = 0;
				}
			}
		}
	}

	//cout << "Longest run is: " << longestRun << endl;
	//cout << "Running from " << longX1 << ", " << longY1 << " to " << longX2 << ", " << longY2 << endl;

	lineStruct line = { longestRun, Point(longX1, longY1), Point(longX2, longY2), Scalar(255, 255, 255), 3 };

	//line(trackImg,Point(longX1,longY1),Point(longX2,longY2),Scalar(255,255,255),3,0);

	return line;
}

/*
//potential new algorithm, discontinued because code freeze
lineStruct findTrackOnLine2(int x, int y, int thetaR, int trackWidth, int contrastWidth) {
	int width = img1.size().width;
	int height = img1.size().height;
	float theta = thetaR*M_PI / 180;
	float length = sqrt(pow(height, 2) + pow(width, 2));
	int tolerance = 5;
	int cTolerance = 5;
	int badPixels = 0;

	int newX, newY;
	int longestRun = 0;
	int currentRun = 0;
	int longX1 = 0, longX2 = 0, longY1 = 0, longY2 = 0;
	int currentX, currentY;
	int step = img1.step;
	int channels = img1.channels();
	int avgIntensity = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			avgIntensity += img1.data[i*step + j*channels];
		}
	}
	avgIntensity = avgIntensity / (width*height);

	for (int i = -length; i < length; i++) {
		newX = x - i*cos(theta);
		newY = i*sin(theta) + y;
		int lContrast = 0;
		int rContrast = 0;

		//constrain within rectangular image
		if (0 <= newX && newX < width && 0 <= newY && newY < height) {
			//takes trackWidth into account (currently not weighted)
			int intensity = 0;
			int pixelCount = 0;
			for (int j = 0; j < trackWidth; j++) {
				int widthOffset = -trackWidth / 2 + j;
				int tempX = newX + widthOffset*cos(M_PI / 2 - theta);
				int tempY = newY + widthOffset*sin(M_PI / 2 - theta);
				if (0 <= tempX && tempX < width && 0 <= tempY && tempY < height) {
					pixelCount += 1;
					intensity += img1.data[tempY*step + tempX*channels];
				}
			}
			intensity = intensity / pixelCount;

			//compare intensity at contrastWidth
			for (int j = 0; j <= contrastWidth; j += contrastWidth) {
				int contrastOffset = -contrastWidth / 2 + j;
				int tempX = newX + contrastOffset*cos(M_PI / 2 - theta);
				int tempY = newY + contrastOffset*sin(M_PI / 2 - theta);
				if (0 <= tempX && tempX < width && 0 <= tempY && tempY < height) {
					if (j == 0)
						lContrast = img1.data[tempY*step + tempX*channels] - intensity;
					else
						rContrast = img1.data[tempY*step + tempX*channels] - intensity;
				}
			}

			scorePoint(newX, newY, contrastWidth, theta, avgIntensity, lContrast, rContrast);

			//####loosen requirements so one bad pixel doesn't ruin the run
			if (intensity < avgIntensity && abs(lContrast) > cTolerance && abs(rContrast) > cTolerance) {
				badPixels = 0;

				if (currentRun == 0) {
					currentX = newX;
					currentY = newY;
				}
				currentRun += 1;
			}
			else {
				badPixels += 1;
				if (badPixels > tolerance) {
					if (currentRun > longestRun) {
						longestRun = currentRun;
						longX1 = currentX;
						longY1 = currentY;
						longX2 = newX;
						longY2 = newY;
					}
					currentRun = 0;
				}
			}
		}
	}

	//cout << "Longest run is: " << longestRun << endl;
	//cout << "Running from " << longX1 << ", " << longY1 << " to " << longX2 << ", " << longY2 << endl;

	lineStruct line = { longestRun, Point(longX1, longY1), Point(longX2, longY2), Scalar(255, 255, 255), 3 };

	//line(trackImg,Point(longX1,longY1),Point(longX2,longY2),Scalar(255,255,255),3,0);

	return line;
}

*/






//Mat invertImage(Mat src) {
//	int height,width,step,channels;
//	height = src.rows;
//	width = src.cols;
//	step = src.step;
//	channels = src.channels();
//	uchar* srcData = src.data;
//
//	Mat dst = Mat::zeros(src.rows,src.cols,src.type());
//	uchar* dstData = dst.data;
//
//	for (int i=0; i < height; i++)
//		for (int j=0; j < width; j++)
//			for (int k=0; k < channels; k++) {
//				dstData[i*step+j*channels+k]=255-srcData[i*step+j*channels+k];
//			}
//
//	return dst;
//}

Mat grayscaleLuminosity(Mat src) {
	int height, width, step, channels;
	height = src.rows;
	width = src.cols;
	step = src.step;
	channels = src.channels();
	uchar* srcData = src.data;

	Mat dst = Mat::zeros(src.rows, src.cols, src.type());
	uchar* dstData = dst.data;

	double avgValue = 0;

	for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++) {
		avgValue = 0.07*srcData[i*step + j*channels] + 0.71*srcData[i*step + j*channels + 1] + 0.21*srcData[i*step + j*channels + 2];
		for (int k = 0; k < channels; k++) {
			dstData[i*step + j*channels + k] = avgValue;
		}
	}

	return dst;
}



//float scoreTranslated(Mat img1, Mat img2, int minX, int minY, int maxX, int maxY) {
//	Mat translatedImg = Mat::zeros(img1.rows,img1.cols,img1.type());
//	float bestScore = 100000;
//	float newScore = 0;
//	Point2f bestTranslation = (100000.0,100000.0);
//
//	for (int i = minX; i < maxX; i++) {
//		for (int j = minY; j < maxY; j++) {
//			translatedImg = translateImage(img1,i,j);
//			newScore = scoreImages(translatedImg,img2);
//			if (newScore < bestScore) {
//				bestScore = newScore;
//				bestTranslation = Point2f(i,j);
//			}
//		}
//		cout << "Row " << i-minY << "/" << maxY-minY << endl;
//	}
//
//	cout << "Best Score: " << bestScore << endl;
//	cout << "Best Translation: x = " << bestTranslation.x << ", y = " << bestTranslation.y << endl;
//
//	return bestScore;
//}