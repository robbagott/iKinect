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

void inputFromFile(string filename, int imageWidth, int imageHeight) {
	fstream fs(filename.c_str(), std::fstream::in | std::fstream::out);
	Mat testImage(imageHeight, imageWidth, CV_8UC3, Scalar(255, 255, 255));
	int val = 0;
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			Vec3b color;
			fs >> val;
			if(val==0){
				color.val[0] = 0;
				color.val[1] = 0;
				color.val[2] = 255;
			}
			else if(val==1){
				color.val[0] = 0;
				color.val[1] = 255;
				color.val[2] = 0;
			}
			else if(val==2){
				color.val[0] = 0;
				color.val[1] = 0;
				color.val[2] = 0;
			}
			else{
				cerr << "Error: not valid color" << endl;
				color.val[0] = 0;
				color.val[1] = 0;
				color.val[2] = 0;
			}
			testImage.at<Vec3b>(row, col) = color;
		}
	}
	fs.close();
	size_t pos = filename.find(".");   
	string name = filename.substr(0, pos);


	imwrite(name + ".png",testImage);
	
	//Mat image;
    //image = imread(name + ".png", CV_LOAD_IMAGE_COLOR);   // Read the file
	namedWindow( "Display window", WINDOW_AUTOSIZE );
    imshow( "Display window", testImage );
    waitKey(0);     
	
}

int main(int argc, char** argv){
	if (argc != 4) {
		cerr << "Expected arguments: " << endl;
		cerr << "name of input file" << endl;
		cerr << "image widht in pixels" << endl;
		cerr << "image height in pixels" << endl;
		return -1;
	}
	int imageWidth = 0, imageHeight = 0;
	stringstream ss;
	ss << argv[2];
	ss >> imageWidth;
	ss.clear();
	ss << argv[3];
	ss >> imageHeight;
	ss.clear();
	inputFromFile(argv[1], imageWidth, imageHeight);
	
	return 0;
}
