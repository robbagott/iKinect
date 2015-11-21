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

int main(int argc, char** argv){

	int imageWidth = 1280;
	int imageHeight = 720;
	fstream fs("correspondence.txt", std::fstream::in | std::fstream::out);
	Mat testImage(imageHeight, imageWidth, CV_8UC3, Scalar(0,0,0));

	bool isValid;
	int x;
	int y;
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			fs >> isValid;
			fs >> x;
			fs >> y;
			if (isValid) {
				testImage.at<Vec3b>(y, x) = Vec3b(255, 255, 255);
			}
		}
	}
	fs.close();
	
	imwrite("correspondence.png", testImage);
	namedWindow("Display window", WINDOW_AUTOSIZE );
   	imshow("Display window", testImage );
    	waitKey(0); 
	
	return 0;
}