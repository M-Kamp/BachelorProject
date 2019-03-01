#include "Infected.h"


Infected::Infected(vector<Point> _contour)
{
	currentContour = _contour;

	currentBoundingRect = boundingRect(currentContour);

	Point currentCenter;
	currentCenter.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2;
	currentCenter.y = (currentBoundingRect.y + currentBoundingRect.y + currentBoundingRect.height) / 2;

	centerPositions.push_back(currentCenter);

	//WHETHER OR NOT THE OBJECT IS BEING TRACKED
	beingTracked = true;

	//IS THE CURRENT OBJECT A MATCH OR A NEW OBJECT
	matchOrNew = true;

	noMatch = 0;
}

void Infected::predictNextPosition(void) {

	int numPositions = (int)centerPositions.size(); //stores the amount of known positions we have of the object

	if (numPositions == 1) { //If it is the first position, there is nothing else to do than to push back the current position

		predictedPosition.x = centerPositions.back().x;
		predictedPosition.y = centerPositions.back().y;
	}
	else if (numPositions == 2) { //if we have 2 positions find delta between the first and second positions. Current positions + delta becomes the predicted

		int deltaX = centerPositions[1].x - centerPositions[0].x;
		int deltaY = centerPositions[1].y - centerPositions[0].y;

		predictedPosition.x = centerPositions.back().x + deltaX;
		predictedPosition.y = centerPositions.back().y + deltaY;
	}
	else if (numPositions == 3) {//if we have 3 positions find delta between 3rd and 2nd position & 2nd and 1st position. Newest points are wheighted the most 

		int sumOfXChanges = ((centerPositions[2].x - centerPositions[1].x) * 2) + ((centerPositions[1].x - centerPositions[0].x) * 1);
		int sumOfYChanges = ((centerPositions[2].y - centerPositions[1].y) * 2) + ((centerPositions[1].y - centerPositions[0].y) * 1);

		int deltaX = (int)round((float)sumOfXChanges / 3.0);
		int deltaY = (int)round((float)sumOfYChanges / 3.0);

		predictedPosition.x = centerPositions.back().x + deltaX;
		predictedPosition.y = centerPositions.back().y + deltaY;
	}
	else if (numPositions == 4) { //same procedure as the previous step, just with 4 points

		int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) + ((centerPositions[2].x - centerPositions[1].x) * 2) + ((centerPositions[1].x - centerPositions[0].x) * 1);
		int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) + ((centerPositions[2].y - centerPositions[1].y) * 2) + ((centerPositions[1].y - centerPositions[0].y) * 1);

		int deltaX = (int)round((float)sumOfXChanges / 6);
		int deltaY = (int)round((float)sumOfYChanges / 6);

		predictedPosition.x = centerPositions.back().x + deltaX;
		predictedPosition.y = centerPositions.back().y + deltaY;
	}
	else if (numPositions >= 5) { //if we have 5 or more points we find the sum of the changes from the last 5 points obtained and wheigh them, 
								  //so that the newest points, are wheighted the most.
		int sumOfXChanges = ((centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4) + ((centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3) +
			((centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2) + ((centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1);

		int sumOfYChanges = ((centerPositions[numPositions - 1].y - centerPositions[numPositions - 2].y) * 4) + ((centerPositions[numPositions - 2].y - centerPositions[numPositions - 3].y) * 3) +
			((centerPositions[numPositions - 3].y - centerPositions[numPositions - 4].y) * 2) + ((centerPositions[numPositions - 4].y - centerPositions[numPositions - 5].y) * 1);

		int deltaX = (int)round((float)sumOfXChanges / 10.0);
		int deltaY = (int)round((float)sumOfYChanges / 10.0);

		predictedPosition.x = centerPositions.back().x + deltaX;
		predictedPosition.y = centerPositions.back().y + deltaY;
	}
	else {
		//we should never get to this point, but in any case we do nothing
	}

}




