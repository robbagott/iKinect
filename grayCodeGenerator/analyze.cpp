/*********************************************************************************************
INCLUDES
*********************************************************************************************/

#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

/*********************************************************************************************
CONSTANTS AND GLOBAL VARIABLES
*********************************************************************************************/

//percentage color distance for a pixel to not be classified as a projected color (Red or Green)
const float colorThreshold = 0.1;

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
};

//This stores the precision to which a pixel in the projected images matches up to a pixel in the taken images. 
//This evolves as progressive images are processed. First, each recognized pixel in the taken image is separated into
//two groups based on their color. On the next image, they are further classified. These classifications are stored here.
vector<vector<Pixel> > pixelAssignments;

/*********************************************************************************************
FUNCTIONS
*********************************************************************************************/

bool isRed(int imageX, int imageY) {
	//d=sqrt((r2-r1)^2+(g2-g1)^2+(b2-b1)^2)
	//p=d/sqrt((255)^2+(255)^2+(255)^2)
	//Get pixel red, green, blue
	float r = 0;
	float g = 0;
	float b = 0;
	float distance = sqrt( pow((255-r),2) + pow((0-g),2) + pow((0-b),2) );
	float percentage = distance/442;

	if (percentage < colorThreshold) {
		return true;
	}
	else {
		return false;
	}
}

bool isGreen(int imageX, int imageY) {
	//d=sqrt((r2-r1)^2+(g2-g1)^2+(b2-b1)^2)
	//p=d/sqrt((255)^2+(255)^2+(255)^2)
	//Get pixel red, green, blue
	float r = 0;
	float g = 0;
	float b = 0;
	float distance = sqrt( pow((0-r),2) + pow((255-g),2) + pow((0-b),2) );
	float percentage = distance/442;

	if (percentage < colorThreshold) {
		return true;
	}
	else {
		return false;
	}
}

void processHImage(int imageNum) {
	//get image

	//For each pixel in the image...
	for (unsigned int i = 0; i < imageWidth; i++) {
		for (unsigned int j = 0; j < imageHeight; j++) {
			if (isRed(i, j)) {
				pixelAssignments[i][j].hAssigns[imageNum] = Red;
			}
			else if (isGreen(i, j)) {
				pixelAssignments[i][j].hAssigns[imageNum] = Green;
			}
			else {
				pixelAssignments[i][j].hAssigns[imageNum] = Other;
			}
		}
	}
}

void processVImage(int imageNum) {
	//get image

	//For each pixel in the image...
	for (unsigned int i = 0; i < imageWidth; i++) {
		for (unsigned int j = 0; j < imageHeight; j++) {
			if (isRed(i, j)) {
				pixelAssignments[i][j].vAssigns[imageNum] = Red;
			}
			else if (isGreen(i, j)) {
				pixelAssignments[i][j].vAssigns[imageNum] = Green;
			}
			else {
				pixelAssignments[i][j].vAssigns[imageNum] = Other;
			}
		}
	}
}

void calcProjLocs() {

}

/*********************************************************************************************
MAIN
*********************************************************************************************/

int main(int argc, char** argv) {

	//Set variables from arguments
	if (argc != 4) {
		cerr << "Expected arguments: " << endl;
		cerr << "number of images per orientation" << endl;
		cerr << "image widht in pixels" << endl;
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
	//cerr << "numImages: " << numImages << endl << "imageWidth: " << imageWidth << endl << "imageHeight: " << imageHeight << endl;

	//Set up array of pixels and associated data
	pixelAssignments.resize(imageWidth);
	for (unsigned int i = 0; i < pixelAssignments.size(); i++) {
		pixelAssignments[i].resize(imageHeight);
		for (unsigned int j = 0; j < pixelAssignments[i].size(); j++) {
			pixelAssignments[i][j].hAssigns.resize(numImages);
			pixelAssignments[i][j].vAssigns.resize(numImages);
		}
	}

	//Process the images in terms of assigning sequences of red/green to pixels
	for (unsigned int i = 0; i < numImages; i++) {
		processHImage(i);
		processVImage(i);
	}

	//Calculate where in the projection image each of the recognized image pixels corresponds according to their red/green sequences
	calcProjLocs();
	return 0;
}