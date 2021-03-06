/*********************************************************************************************
INCLUDES
*********************************************************************************************/

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*********************************************************************************************
CONSTANTS AND GLOBAL VARIABLES
*********************************************************************************************/

//percentage color distance for a pixel to not be classified as a projected color (Red or Green)
const float colorThreshold = 0.5;

int numImages = 0;
int imageWidth = 0;
int imageHeight = 0;

/*********************************************************************************************
DATA STRUCTURES
*********************************************************************************************/

enum Assignment {
	Red, Green, Other
};

struct Pixel{
	vector<Assignment> hAssigns;
	vector<Assignment> vAssigns;

	//Evolving range of precision over program lifespan
	float xLow;
	float xHi;
	float yLow;
	float yHi;
	int xEst;
	int yEst;

	//final calculated depth
	int depth;

	bool isValid;
};

//This stores the precision to which a pixel in the projected images matches up to a pixel in the taken images. 
//This evolves as progressive images are processed. First, each recognized pixel in the taken image is separated into
//two groups based on their color. On the next image, they are further classified. These classifications are stored here.
Pixel** pixels;

/*********************************************************************************************
FUNCTIONS
*********************************************************************************************/

bool isRed(int row, int col, const Mat& img) {
	//d=sqrt((r2-r1)^2+(g2-g1)^2+(b2-b1)^2)
	//p=d/sqrt((255)^2+(255)^2+(255)^2)
	//Get pixel red, green, blue
	Vec3b px = img.at<Vec3b>(row, col);
	float r = px.val[2];
	float g = px.val[1];
	float b = px.val[0];
	float distance = sqrt( pow((255-r),2) + pow((0-g),2) + pow((0-b),2) );
	float percentage = distance/442;

	if (percentage < colorThreshold) {
		return true;
	}
	else {
		return false;
	}
}

bool isGreen(int row, int col, const Mat& img) {
	Vec3b px = img.at<Vec3b>(row, col);
	float r = px.val[2];
	float g = px.val[1];
	float b = px.val[0];
	float distance = sqrt( pow((0-r),2) + pow((255-g),2) + pow((0-b),2) );
	float percentage = distance/442;

	if (percentage < colorThreshold) {
		return true;
	}
	else {
		return false;
	}
}

//Make pixel red/green assignments for gray-code images
//isVert indicates if the gray-code for the given image is horizontal or vertical gray-code
void processImage(int imageNum, bool isVert) {
	//get image
	string filename = "img_";
	if (isVert) {
		filename +="1_";
	}
	else {
		filename += "0_";
	}
	stringstream ss;
	ss << imageNum;
	filename += ss.str();
	ss.clear();
	filename += ".png";
	cerr << "Processing " << filename << endl;
	Mat img = imread(filename);
	
	if (img.data == NULL) {
		cerr << "Did not find img data" << endl;
		exit(1);
	}

	//For each pixel in the image...
	for (unsigned int row = 0; row < img.rows; row++) {

		for (unsigned int col = 0; col < img.cols; col++) {
			if (isRed(row, col, img)) {
				if (isVert) {
					pixels[row][col].vAssigns[imageNum] = Red;
				}
				else {
					pixels[row][col].hAssigns[imageNum] = Red;
				}
			}
			else if (isGreen(row, col, img)) {
				if (isVert) {
					pixels[row][col].vAssigns[imageNum] = Green;
				}
				else {
					pixels[row][col].hAssigns[imageNum] = Green;
				}
			}
			else {
				if (isVert) {
					pixels[row][col].vAssigns[imageNum] = Other;
				}
				else {
					pixels[row][col].hAssigns[imageNum] = Other;
				}
			}
		}
	}
}

//Calculate where pixels in image correspond to in projection data
void calcCorrespondence() {
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			bool greenIsLeft = true;
			bool greenIsBot = true;
			bool otherColor = false;
			for (unsigned int imageNum = 0; imageNum < numImages; imageNum++) {
				if (otherColor) {
					break;
				}
				switch (pixels[row][col].vAssigns[imageNum]) {
				case Red:
					if (greenIsLeft) {
						pixels[row][col].xLow += imageWidth/pow(2, imageNum+1)-1;
					}
					else {
						pixels[row][col].xHi -= imageWidth/pow(2, imageNum+1)-1;
					}
					greenIsLeft = !greenIsLeft;
					break;
				case Green:
					if (greenIsLeft) {
						pixels[row][col].xHi -= imageWidth/pow(2, imageNum+1)-1;
					}
					else {
						pixels[row][col].xLow += imageWidth/pow(2, imageNum+1)-1;
					}
					break;
				default: //Color is classified as other...
					otherColor = true;
					pixels[row][col].isValid = false;
					break;
				}
				switch (pixels[row][col].hAssigns[imageNum]) {
				case Red:
					if (greenIsBot) {
						pixels[row][col].yHi -= imageHeight/pow(2, imageNum+1)-1;
					}
					else {
						pixels[row][col].yLow += imageHeight/pow(2, imageNum+1)-1;
					}
					greenIsBot = !greenIsBot;
					break;
				case Green:
					if (greenIsBot) {
						pixels[row][col].yLow += imageHeight/pow(2, imageNum+1)-1;
					}
					else {
						pixels[row][col].yHi -= imageHeight/pow(2, imageNum+1)-1;
					}
					break;
				default: //Color is classified as other...
					otherColor = true;
					pixels[row][col].isValid = false;
					break;
				}
			}
		}
	}

	//Calculate estimates
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			pixels[row][col].xEst = (pixels[row][col].xLow + pixels[row][col].xHi)/2;
			pixels[row][col].yEst = (pixels[row][col].yLow + pixels[row][col].yHi)/2;
		}
	}
}

void createDepthMap() {
	//Triangulate points between position in projection image and position in captured images
	//The following was provide by Joseph
	/*
	template<typename T> 
	void compose_KRt(Mat &K, Mat &R, Mat &t, Mat &P)
	{
		P.create(3, 4, DataType<T>::type);

		P.ptr<T>(0)[0] = R.ptr<T>(0)[0]; 
		P.ptr<T>(0)[1] = R.ptr<T>(0)[1]; 
		P.ptr<T>(0)[2] = R.ptr<T>(0)[2];
		 
		P.ptr<T>(1)[0] = R.ptr<T>(1)[0]; 
		P.ptr<T>(1)[1] = R.ptr<T>(1)[1]; 
		P.ptr<T>(1)[2] = R.ptr<T>(1)[2];
		
		P.ptr<T>(2)[0] = R.ptr<T>(2)[0]; 
		P.ptr<T>(2)[1] = R.ptr<T>(2)[1]; 
		P.ptr<T>(2)[2] = R.ptr<T>(2)[2];

		P.ptr<T>(0)[3] = t.ptr<T>(0)[0];
		P.ptr<T>(1)[3] = t.ptr<T>(1)[0]; 
		P.ptr<T>(2)[3] = t.ptr<T>(2)[0];

		P = K * P;
	}

	int main(void)
	{
		// Set reference camera P0 = [I|0]
		Mat P0;
		Mat K0; // intrinsic camera matrix computed from calibration
		Mat R0 = Mat::eye(3, 3, DataType<float>::type);
		Mat t0 = Mat::zeros(3, 1, DataType<float>::type);
		compose_KRt<float>(K0, R0, t0, P0);

		// Set relative camera P1 = [R|t]
		Mat P1;	
		Mat K1; // intrinsic camera matrix computed from calibration 
		Mat R1; // rotation matrix computed from calibration
		Mat t1; // translation matrix computed from calibration
		compose_KRt<float>(K1, R1, t1, P1);

		// Compute 3D world points
		Mat X;
		vector<Point2f> p0; // corresponding points in P0 view
		vector<Point2f> p1; // corresponding points in P1 view
		triangulatePoints(P0, P1, p0, p1, X); // X will be a 4xN matrix (in homogeneous coordinates)
		
		// Print 3D world points (converted from homogeneous coordinates)
		for (int i = 0; i < X.cols; ++i)
			cout 
				<< "x:" << X.ptr<float>(0)[i] / X.ptr<float>(3)[i] 
				<< "y:" << X.ptr<float>(1)[i] / X.ptr<float>(3)[i]
				<< "z:" << X.ptr<float>(2)[i] / X.ptr<float>(3)[i] << endl;
	}
	*/
}

void outputToFile(string filename) {
	//fstream fs(filename.c_str(), std::fstream::in | std::fstream::out);
	//for (unsigned int i = 0; i < imageWidth; i++) {
	//	for (unsigned int j = 0; j < imageHeight; j++) {
	//		fs << pixels[i][j].depth << " ";
	//	}
	//	fs << endl;
	//}
	//fs.close();
}

void outputProcessing(bool isVert, int imageNum) {
	string filename = "procout_";
	if (isVert) {
		filename +="1_";
	}
	else {
		filename += "0_";
	}
	stringstream ss;
	ss << imageNum;
	filename += ss.str();
	ss.clear();
	filename += ".txt";

	ofstream fs(filename.c_str());

	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {	
			if (isVert) {
				switch (pixels[row][col].vAssigns[imageNum]) {
					case Red:
						fs << 0 << " ";
						break;
					case Green:
						fs << 1 << " ";
						break;
					default:
						fs << 2 << " ";
						break;
				}
			}
			else {
				switch (pixels[row][col].hAssigns[imageNum]) {
					case Red:
						fs << 0 << " ";
						break;
					case Green:
						fs << 1 << " ";
						break;
					default:
						fs << 2 << " ";
						break;
				}
			}
		}
		fs << endl;
	}
	fs.close();

	cerr << "output processing" << endl;
}

void outputCorrespondence() {
	ofstream fs("correspondence.txt");
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			fs << pixels[row][col].isValid << " " << pixels[row][col].xEst << " " << pixels[row][col].yEst << " ";
		}
		fs << endl;
	}
	fs.close();

	ofstream dfs("depth.txt");
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			dfs << row<< " ";
		}
		dfs << endl;
	}
	dfs.close();
}

/*********************************************************************************************
MAIN
*********************************************************************************************/

int main(int argc, char** argv) {

	//Set variables from arguments
	if (argc != 4) {
		cerr << "Expected arguments: " << endl;
		cerr << "number of images per orientation" << endl;
		cerr << "image width in pixels" << endl;
		cerr << "image height in pixels" << endl;
		return -1;
	}
	stringstream ss;
	ss << argv[1];
	ss >> numImages;
	ss.clear();
	ss << argv[2];
	ss >> imageWidth;
	ss.clear();
	ss << argv[3];
	ss >> imageHeight;
	ss.clear();
	cerr << "numImages: " << numImages << endl << "imageWidth: " << imageWidth << endl << "imageHeight: " << imageHeight << endl;

	cerr << "creating vector" << endl;
	//Set up array of pixels and associated data
	pixels = new Pixel*;
	for (unsigned int row = 0; row < imageHeight; row++) {
		pixels[row] = new Pixel[imageWidth];
		for (unsigned int col = 0; col < imageWidth; col++) {
			pixels[row][col].hAssigns.resize(numImages);
			pixels[row][col].vAssigns.resize(numImages);
			pixels[row][col].isValid = true;
			pixels[row][col].xLow = 0;
			pixels[row][col].xHi = imageWidth-1;
			pixels[row][col].yLow = 0;
			pixels[row][col].yHi = imageHeight-1;
		}
	}

	//Process the images in terms of assigning sequences of red/green to pixels
	for (unsigned int i = 0; i < numImages; i++) {
		cerr << "Processing set " << i << " of images" << endl;
		processImage(i, true);
		cerr << "Finished processing vertical image" << endl;
		processImage(i, false);
		cerr << "Finished processing horizontal image" << endl;
	}

	cerr << "Made it to the end of processing" << endl;
	//Calculate where in the projection image each of the recognized image pixels corresponds according to their red/green sequences
	calcCorrespondence();

	//Triangulate the points
	createDepthMap();

	//Output depths ot a file;
	outputToFile("depthOutput.txt");

	outputProcessing(true, 0);
	outputCorrespondence();
	return 0;
}