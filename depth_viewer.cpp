#include <opencv2/opencv.hpp>
#include <climits>
#include <fstream>

using namespace std;
using namespace cv;

Vec3b calcLinearColor(float depth, float minDepth, float maxDepth) {
	float percentage = (depth-minDepth)/(maxDepth-minDepth);
	float blue = 255.0*percentage;
	float red = 255.0 - 255.0*percentage;
	return Vec3b(blue, 0, red);
}

Vec3b calcCyclingColor(float depth) {
	
}

int main() {
	int imageWidth = 1280;
	int imageHeight = 720;
	fstream fs("depth.txt", fstream::in | fstream::out);
	Mat testImage(imageHeight, imageWidth, CV_8UC3, Scalar(0,0,0));

	float maxDepth = 0;
	float minDepth = INT_MAX;
	float depth;
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			fs >> depth;
			if (depth != -1) {
				if (depth > maxDepth) {
					maxDepth = depth;
				}
				else if (depth < minDepth) {
					minDepth = depth;
				}
			}
		}
	}
	
	fs.close();
	fs.open("depth.txt", fstream::in | fstream::out);

	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			fs >> depth;
			if (depth == -1) {
				testImage.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
			}
			else {
				testImage.at<Vec3b>(row, col) = calcLinearColor(depth, minDepth, maxDepth);
				//testImage.at<Vec3b>(row, col) = calcCyclingColor(depth);
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