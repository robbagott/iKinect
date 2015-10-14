#include <iostream>
#include "bmp.h"
#include <string>
#include <sstream>
#include <cmath>

int width = 1920;
int height = 1080;

using namespace std;

void createGC(int imageNumber) {

	Bmp image(width, height);

	if (imageNumber <= 0) {
		cout << "Invalid imageNumber input" << endl;
		return;
	}

	Color white = Color(255,255,255);
	Color black = Color(0,0,0);

	bool drawWhite = true;
	int borderCounter = 1;
 	
	int widthUnit = 1920/( pow(2, imageNumber));
	for (unsigned int i = 0; i < 1920; i++) {

		//Handle changing of colors
		if ((i % widthUnit) == 0 && i != 0) {
			if (borderCounter > 0) {
				drawWhite = !drawWhite;
				borderCounter = 0;
			}
			else {
				borderCounter++;
			}
		}

		//Draw in a vertical line
		for (unsigned int j = 0; j < 1080; j++) {
			if (drawWhite) {
				image.setPixel(i, j, white);
			}
			else {
				image.setPixel(i, j, black);
			}
		}
	}

	stringstream ss;
	ss << "graycode_" << imageNumber << ".bmp";
	string filename = ss.str();

	string errMsg = "image writing failed for " + filename;

	if(!image.write(filename, errMsg))
	{
		cout << errMsg << endl;
	}
	else
	{
		cout << "Successfully wrote file: [" << filename << "]" << endl;
	}

}

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "Not enough arguments!" << endl;
		return -1;
	}

	stringstream ss;
	ss << argv[1];
	int imageAmount;
	ss >> imageAmount;
	cout << imageAmount;

	if (imageAmount <= 0 || imageAmount > 10) {
		cout << "Argument not in accepted range!" << endl;
		return -1;
	}

	for (unsigned int i = 1; i <= imageAmount; i++) {
		createGC(i);
	}

	return 0;
}