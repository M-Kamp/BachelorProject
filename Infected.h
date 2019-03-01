#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class Infected
{
public:
	//Infected();
	//~Infected();

	vector<Point> currentContour;

	Rect currentBoundingRect;

	vector<Point> centerPositions;

	bool beingTracked;

	bool matchOrNew;

	int noMatch;

	//VARIABLE FOR NEXT PREDICTED POSITION
	Point predictedPosition;

	Infected(vector<Point> _contour);
	
	//NEXT PREDICTED POSITION
	void predictNextPosition(void);
};
