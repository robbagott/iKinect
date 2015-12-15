/*********************************************************************************************
INCLUDES
*********************************************************************************************/

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Vec3b getColor(int y, int x) {
	float xpercent = (float) x/1280;
	float ypercent = (float) y/720;
	int r = 255.0*xpercent;
	int g = 255.0 - 255.0*xpercent;
	int b = 255.0*ypercent;
	return Vec3b(b,g,r);
}

int main(int argc, char** argv) {

	int imageWidth = 1280;
	int imageHeight = 720;
	fstream fs("correspondence.txt", std::fstream::in | std::fstream::out);
	Mat testImage1(imageHeight, imageWidth, CV_8UC3, Scalar(0,0,0));
	Mat testImage2(imageHeight, imageWidth, CV_8UC3, Scalar(0,0,0));

	bool isValid;
	int x;
	int y;
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			fs >> isValid;
			fs >> x;
			fs >> y;
			if (isValid) {
				testImage1.at<Vec3b>(y, x) = getColor(row, col);
				testImage2.at<Vec3b>(row, col) = getColor(row, col);
			}
		}
	}
	fs.close();
	
	imwrite("correspondence1.png", testImage1);
	namedWindow("projection window", WINDOW_AUTOSIZE );
   	imshow("projection window", testImage1 );
    	waitKey(0); 
	
	/*
	fs.open("correspondence.txt", std::fstream::in | std::fstream::out);
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			fs >> isValid;
			fs >> x;
			fs >> y;
			if (isValid) {
				testImage2.at<Vec3b>(row, col) = Vec3b(255, 255, 255);
			}
		}
	}
	fs.close();
	*/

	imwrite("correspondence2.png", testImage2);
	namedWindow("camera window", WINDOW_AUTOSIZE );
   	imshow("camera window", testImage2 );
	waitKey(0);
	
	return 0;
}
