/*********************************************************************************************
INCLUDES
*********************************************************************************************/

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

/*********************************************************************************************
CONSTANTS AND GLOBAL VARIABLES
*********************************************************************************************/

//percentage color distance for a pixel to not be classified as a projected color (Red or Green)
const float colorThreshold = 0.45;

int numImagesHor = 0;
int numImagesVert = 0;
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
	for (unsigned int row = 0; row < imageHeight; row++) {

		for (unsigned int col = 0; col < imageWidth; col++) {
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
			for (unsigned int imageNum = 0; imageNum < numImagesVert; imageNum++) {
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
				
			}
			for (unsigned int imageNum = 0; imageNum < numImagesHor; imageNum++) {
				if (otherColor) {
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

template<typename T> 
void compose_KRt(Mat &K, Mat &R, Mat &t, Mat &P)
{
	Mat P_Temp;
	P_Temp.create(3, 4, DataType<T>::type);

	P.create(3, 4, DataType<T>::type);

	P_Temp.ptr<T>(0)[0] = R.ptr<T>(0)[0]; 
	P_Temp.ptr<T>(0)[1] = R.ptr<T>(0)[1]; 
	P_Temp.ptr<T>(0)[2] = R.ptr<T>(0)[2];

	P_Temp.ptr<T>(1)[0] = R.ptr<T>(1)[0]; 
	P_Temp.ptr<T>(1)[1] = R.ptr<T>(1)[1]; 
	P_Temp.ptr<T>(1)[2] = R.ptr<T>(1)[2];

	P_Temp.ptr<T>(2)[0] = R.ptr<T>(2)[0]; 
	P_Temp.ptr<T>(2)[1] = R.ptr<T>(2)[1]; 
	P_Temp.ptr<T>(2)[2] = R.ptr<T>(2)[2];

	P_Temp.ptr<T>(0)[3] = t.ptr<T>(0)[0];
	P_Temp.ptr<T>(1)[3] = t.ptr<T>(1)[0]; 
	P_Temp.ptr<T>(2)[3] = t.ptr<T>(2)[0];

	P = K * P_Temp;
}

void outputDepth(string filename, const Mat& X) {
	cerr << "Outputting Depth..." << endl;
	cerr << "to " << filename << endl;
	ofstream ofs(filename.c_str(), ofstream::out);
	for (unsigned int i = 0; i < X.cols; i++) {
		ofs << X.ptr<float>(0)[i] / X.ptr<float>(3)[i] << " "
			<< X.ptr<float>(1)[i] / X.ptr<float>(3)[i] << " "
			<< X.ptr<float>(2)[i] / X.ptr<float>(3)[i]<< endl;
	}
	ofs.close();
}

void createDepthMap() {
	//Triangulate points between position in projection image and position in captured images
	cerr << "Calibrating..." << endl;

	// Set reference camera P0 = [I|0]
	Mat P0(3, 4, DataType<float>::type);
	/*
	P0.ptr<float>(0)[0] = 2605.7;
	P0.ptr<float>(0)[1] = 0;
	P0.ptr<float>(0)[2] = 1318.3;
	P0.ptr<float>(0)[3] = 0;
	P0.ptr<float>(1)[0] = 0;
	P0.ptr<float>(1)[1] = 2602.7;
	P0.ptr<float>(1)[2] = 963;
	P0.ptr<float>(1)[3] = 0;
	P0.ptr<float>(2)[0] = 0;
	P0.ptr<float>(2)[1] = 0;
	P0.ptr<float>(2)[2] = 1;
	P0.ptr<float>(2)[3] = 0;
	*/

	Mat K0(3, 3, DataType<float>::type);
	
	K0.ptr<float>(0)[0] = 1296.8;
	K0.ptr<float>(0)[1] = 0;
	K0.ptr<float>(0)[2] = 639.7;
	K0.ptr<float>(1)[0] = 0;
	K0.ptr<float>(1)[1] = 1297.6;
	K0.ptr<float>(1)[2] = 354.1;
	K0.ptr<float>(2)[0] = 0;
	K0.ptr<float>(2)[1] = 0;
	K0.ptr<float>(2)[2] = 1;
	
	/*
	K0.ptr<float>(0)[0] = 1;
	K0.ptr<float>(0)[1] = 0;
	K0.ptr<float>(0)[2] = 0;
	K0.ptr<float>(1)[0] = 0;
	K0.ptr<float>(1)[1] = 1;
	K0.ptr<float>(1)[2] = 0;
	K0.ptr<float>(2)[0] = 0;
	K0.ptr<float>(2)[1] = 0;
	K0.ptr<float>(2)[2] = 1;
	*/

	Mat R0(3, 3, DataType<float>::type);
	R0.ptr<float>(0)[0] = 1;
	R0.ptr<float>(0)[1] = 0;
	R0.ptr<float>(0)[2] = 0;
	R0.ptr<float>(1)[0] = 0;
	R0.ptr<float>(1)[1] = 1;
	R0.ptr<float>(1)[2] = 0;
	R0.ptr<float>(2)[0] = 0;
	R0.ptr<float>(2)[1] = 0;
	R0.ptr<float>(2)[2] = 1;

	Mat t0(3, 1, DataType<float>::type);
	t0.ptr<float>(0)[0] = 0;
	t0.ptr<float>(1)[0] = 0;
	t0.ptr<float>(2)[0] = 0;

	//sets P0 to appropriate pose based on K0, R0, and t0
	compose_KRt<float>(K0, R0, t0, P0);

	// Set relative camera P1 = [R|t]
	Mat P1(3, 4, DataType<float>::type);
	/*
	P1.ptr<float>(0)[0] = 3899.1;
	P1.ptr<float>(0)[1] = -313.8388;
	P1.ptr<float>(0)[2] = 1548.5;
	P1.ptr<float>(0)[3] = 273020;
	P1.ptr<float>(1)[0] = 26.8734;
	P1.ptr<float>(1)[1] = 2971.9;
	P1.ptr<float>(1)[2] = 904.0124;
	P1.ptr<float>(1)[3] = 288210;
	P1.ptr<float>(2)[0] = -0.0390;
	P1.ptr<float>(2)[1] = -0.1646;
	P1.ptr<float>(2)[2] = 0.9856;
	P1.ptr<float>(2)[3] = -212.6413;
	*/

	Mat K1(3, 3, DataType<float>::type);
	K1.ptr<float>(0)[0] = 2105.4;
	K1.ptr<float>(0)[1] = 0;
	K1.ptr<float>(0)[2] = 730.3;
	K1.ptr<float>(1)[0] = 0;
	K1.ptr<float>(1)[1] = 2098.2;
	K1.ptr<float>(1)[2] = 60.4;
	K1.ptr<float>(2)[0] = 0;
	K1.ptr<float>(2)[1] = 0;
	K1.ptr<float>(2)[2] = 1;

	/*
	K1.ptr<float>(0)[0] = 7147.2;
	K1.ptr<float>(0)[1] = 0;
	K1.ptr<float>(0)[2] = 1938.8;
	K1.ptr<float>(1)[0] = 0;
	K1.ptr<float>(1)[1] = 7158.3;
	K1.ptr<float>(1)[2] = 943.9;
	K1.ptr<float>(2)[0] = 0;
	K1.ptr<float>(2)[1] = 0;
	K1.ptr<float>(2)[2] = 1;
	
	
	K1.ptr<float>(0)[0] = 1;
	K1.ptr<float>(0)[1] = 0;
	K1.ptr<float>(0)[2] = 0;
	K1.ptr<float>(1)[0] = 0;
	K1.ptr<float>(1)[1] = 1;
	K1.ptr<float>(1)[2] = 0;
	K1.ptr<float>(2)[0] = 0;
	K1.ptr<float>(2)[1] = 0;
	K1.ptr<float>(2)[2] = 1;
	*/

	Mat R1(3, 3, DataType<float>::type);
	R1.ptr<float>(0)[0] = 0.9956;
	R1.ptr<float>(0)[1] = -0.0361;
	R1.ptr<float>(0)[2] = -0.0864;
	R1.ptr<float>(1)[0] = 0.0333;
	R1.ptr<float>(1)[1] = 0.9988;
	R1.ptr<float>(1)[2] = -0.0346;
	R1.ptr<float>(2)[0] = 0.0876;
	R1.ptr<float>(2)[1] = 0.0315;
	R1.ptr<float>(2)[2] = 0.9957;
	/*
	R1.ptr<float>(0)[0] = 0.9941;
	R1.ptr<float>(0)[1] = -0.0294;
	R1.ptr<float>(0)[2] = 0.1043;
	R1.ptr<float>(1)[0] = 0.0085;
	R1.ptr<float>(1)[1] = 0.9807;
	R1.ptr<float>(1)[2] = 0.1953;
	R1.ptr<float>(2)[0] = -0.1080;
	R1.ptr<float>(2)[1] = -0.1933;
	R1.ptr<float>(2)[2] = 0.9752;
	*/
	Mat t1(3, 1, DataType<float>::type);
	t1.ptr<float>(0)[0] = 104.2324;
	t1.ptr<float>(1)[0] = 38.1896;
	t1.ptr<float>(2)[0] = 27.2477;
	/*
	t1.ptr<float>(0)[0] = 76.7382;
	t1.ptr<float>(1)[0] = -31.3015;
	t1.ptr<float>(2)[0] = 48.0273;
	*/
	compose_KRt<float>(K1, R1, t1, P1);

	// Compute 3D world points
	cerr << "Populating vectors..." << endl;
	vector<Point2f> p0; // corresponding points in P0 view
	vector<Point2f> p1; // corresponding points in P1 view
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {	
			if (pixels[row][col].isValid) {
				p0.push_back(Point2f(row, col));
				p1.push_back(Point2f(pixels[row][col].yEst, pixels[row][col].xEst));	
			}
		}
	}
	//outputting points 
	//ofstream fs(filename.c_str());
	//cerr << "Amount of valid points: " << p0.size() << " " << p1.size() << endl;
	
	/*
	Mat mp0(2, p0.size(), DataType<float>::type);
	for (int i = 0; i < p0.size(); i++) {
		mp0.ptr<float>(0)[i] = p0[i].x;
		mp0.ptr<float>(1)[i] = p0[i].y;
	}
	Mat mp1(2, p1.size(), DataType<float>::type);
	for (int i = 0; i < p0.size(); i++) {
		mp1.ptr<float>(0)[i] = p1[i].x;
		mp1.ptr<float>(1)[i] = p1[i].y;
	}
	*/
	Mat X;
	cerr << "Triangulating..." << endl;
	triangulatePoints(P0, P1, p0, p1, X); // X will be a 4xN matrix (in homogeneous coordinates)
	
	//output to new file
	fstream configfs((string("config.txt")).c_str(), ios_base::in | ios_base::out);
	int currentIndex;
	configfs >> currentIndex;
	cerr << "current index: " << currentIndex << endl;
	configfs.seekg(0, std::ios::beg);
	configfs << ++currentIndex;
	stringstream indexStream;
	indexStream << currentIndex;
	cerr << string("depth_output_") + indexStream.str() + string(".txt") << endl;
	outputDepth(string("saved_data/depth_output_") + indexStream.str() + string(".txt"), X);
}

void outputProcessing(bool isVert, int imageNum) {
	cerr << "Outputting Processing sample..." << endl;
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
}

void outputCorrespondence() {
	cerr << "Outputting  correspondence..." << endl;
	ofstream fs("correspondence.txt");
	for (unsigned int row = 0; row < imageHeight; row++) {
		for (unsigned int col = 0; col < imageWidth; col++) {
			fs << pixels[row][col].isValid << " " << pixels[row][col].xEst << " " << pixels[row][col].yEst << " ";
		}
		fs << endl;
	}
	fs.close();
}

/*********************************************************************************************
MAIN
*********************************************************************************************/

int main(int argc, char** argv) {

	//Set variables from arguments
	if (argc != 5) {
		cerr << "Expected arguments: " << endl;
		cerr << "number of images for horizontal stripes" << endl;
		cerr << "number of images for vertical stripes" << endl;
		cerr << "image width in pixels" << endl;
		cerr << "image height in pixels" << endl;
		return -1;
	}
	stringstream ss;
	ss << argv[1];
	ss >> numImagesHor;
	ss.clear();
	ss << argv[2];
	ss >> numImagesVert;
	ss.clear();
	ss << argv[3];
	ss >> imageWidth;
	ss.clear();
	ss << argv[4];
	ss >> imageHeight;
	ss.clear();
	cerr << "number of horizontal images: " << numImagesHor << endl << "number of vertical images: " 
		<< numImagesVert << endl << "imageWidth: " << imageWidth << endl << "imageHeight: " << imageHeight << endl;

	cerr << "creating vector" << endl;
	//Set up array of pixels and associated data
	pixels = new Pixel*[imageHeight];
	for (unsigned int row = 0; row < imageHeight; row++) {
		pixels[row] = new Pixel[imageWidth];
		for (unsigned int col = 0; col < imageWidth; col++) {
			pixels[row][col].hAssigns.resize(numImagesHor);
			pixels[row][col].vAssigns.resize(numImagesVert);
			pixels[row][col].isValid = true;
			pixels[row][col].xLow = 0;
			pixels[row][col].xHi = imageWidth-1;
			pixels[row][col].yLow = 0;
			pixels[row][col].yHi = imageHeight-1;
		}
	}

	//Process the images in terms of assigning sequences of red/green to pixels
	for (unsigned int i = 0; i < numImagesHor; i++) {
		cerr << "Processing horizontal image" << endl;
		processImage(i, false);
	}
	for (unsigned int i = 0; i < numImagesVert; i++) {
		cerr << "Processing vertical image" << endl;
		processImage(i, true);
	}

	cerr << "Made it to the end of processing" << endl;
	//Calculate where in the projection image each of the recognized image pixels corresponds according to their red/green sequences
	calcCorrespondence();

	//Triangulate the points and output
	createDepthMap();
	
	outputProcessing(true, 0);
	outputCorrespondence();
	return 0;
}
