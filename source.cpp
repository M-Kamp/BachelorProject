//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>LIBRARIES AND NAMESPACE SETUP
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include "Infected.h"

using namespace cv;
using namespace std;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<LIBRARIES AND NAMESPACE SETUP



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>VIDEO INPUT TYPES
int defaultCam = 0; //SETS CAMERA TO DEFAULT CAMERA
int camNum1 = 1;
int camNum2 = 2;

//EMPTY FRAMES
string EmptyFrameVid1 = "C:///-----/P6 - Project Files/EmptyFrameVid1.png";
string EmptyFrameVid2 = "C:///-----/P6 - Project Files/EmptyFrameVid5.png";
string EmptyFrameVid3 = "C:///-----/P6 - Project Files/EmptyFrameVid3.png";
string EvalFrameVid3 = "C:///-----/P6 - Project Files/EvalBack.jpg";
string EvalUnityBack = "C:///-----/EvalBackNew.jpg";

//TESTING VIDEOS
string cvTest = "C:///Desktop/P6 - Project Files/TestVideo1.avi";
string testVideo1 = "C:///-----/P6 - Project Files/Test Videoes/TestVideo4.avi";
string testVideo2 = "C:///-----/P6 - Project Files/Test Videoes/TestVideo5.avi"; 
string testVideo3 = "C:///-----/P6 - Project Files/EvalTestVid.mp4";
string EvalVidShort = "C:///-----/EvalVidShort.mp4";
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<VIDEO INPUT TYPES



//DILATION AND EROSION KERNELS
//Set values is what the system will take as default, when it starts, but the values will change depending on the track bars
int D_KERNEL_SIZE = 4;
int E_KERNEL_SIZE = 1;
int MAX_KERNEL_SIZE = 15;
int timesErode = 3;
int timesDilate = 2;

//NAMES OF THE WINDOWS SHOWING RESULTS
const string windowName1 = "Trackbars: Kernel Sizes";
const string windowName2 = "Thresholded Image";
const string windowName3 = "Contours: First Time";
const string windowName4 = "Contours: Second Time";
const string windowName5 = "Contours: Third Time";
const string windowName6 = "Results";
const string windowName7 = "Original Image";


//THIS FUNCTION GETS CALLED EVERYTIME THE TRACKBARS CHANGES
void event_trackbar(int, void*)
{
	
}

//CALCULATES AND RETURNS DISTANCE BETWEEN TO POINTS
double distanceBetweenObjects(Point point1, Point point2) { 

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

//CREATES TRACKBARS FOR CONTROLLING MORPHOLOGICAL OPERATIONS
void createTrackbars() {
	//CREATE WINDOW FOR TRACKBARS
	namedWindow(windowName1, 0);

	//CREATE MEMORY FOR TRACKBARS
	char TrackbarName[50];
	sprintf_s(TrackbarName, "Erode Kernel Size", E_KERNEL_SIZE);
	sprintf_s(TrackbarName, "Dilate Kernel Size", D_KERNEL_SIZE);
	sprintf_s(TrackbarName, "Times Erode", timesErode);
	sprintf_s(TrackbarName, "Times Dilate", timesDilate);

	//CREATES TRACKBARS FOR ADJUSTING KERNEL SIZES AND AMOUNT OF OPERATIONS
	createTrackbar("Erode Kernel Size", windowName1, &E_KERNEL_SIZE, MAX_KERNEL_SIZE, event_trackbar);
	createTrackbar("Dilate Kernel Size", windowName1, &D_KERNEL_SIZE, MAX_KERNEL_SIZE, event_trackbar);
	createTrackbar("Times Erode", windowName1, &timesErode, MAX_KERNEL_SIZE, event_trackbar);
	createTrackbar("Times Dilate", windowName1, &timesDilate, MAX_KERNEL_SIZE, event_trackbar);
}

//PEFORMS MORHPOLOGICAL OPERATIONS DEPENDING ON VALUES SET IN TRACKBARS
void morphOps(Mat &input, int D_size, int E_size, int X_dilate, int X_erode) {

	//INTANSIATE KERNELS
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(E_size, E_size));
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(D_size, D_size));

	//EROSION
	for (int i = 0; i < X_erode; i++) {
		erode(input, input, erodeElement);
	}

	//DILATION
	for (int i = 0; i < X_dilate; i++) {
		dilate(input, input, dilateElement);
	}
}

//ADDS NEW OBJECT TO LIST OF TRACKED OBJECTS
void addNewObject(Infected &currentFrameObject, vector<Infected> &existingObjects) {

	currentFrameObject.matchOrNew = true;

	existingObjects.push_back(currentFrameObject);
}


void addObjectToObjects(Infected &currentFrameObject, vector<Infected> &existingObjects, int &index) {

	existingObjects[index].currentContour = currentFrameObject.currentContour;

	existingObjects[index].currentBoundingRect = currentFrameObject.currentBoundingRect;

	existingObjects[index].centerPositions.push_back(currentFrameObject.centerPositions.back());

	existingObjects[index].beingTracked = true;
	
	existingObjects[index].matchOrNew = true;
}

void predictObjectMovement(vector<Infected> &existingObjects, vector<Infected> &currentFrameObjects) {

	for (auto &existingObject : existingObjects) {
		existingObject.matchOrNew = false;

		existingObject.predictNextPosition();
	}

	for (auto &currentFrameObject : currentFrameObjects) {

		int indexOfLeastDistance = 0;
		double leastDistance = 1000.0;

		for (unsigned int i = 0; i < existingObjects.size(); i++) {
			if (existingObjects[i].beingTracked == true) {
				double distance = distanceBetweenObjects(currentFrameObject.centerPositions.back(), existingObjects[i].predictedPosition);

				if (distance < leastDistance) {
					leastDistance = distance;
					indexOfLeastDistance = i;
				}
			}
		}

		if (leastDistance < currentFrameObject.currentBoundingRect.width * 1.50) {
			addObjectToObjects(currentFrameObject, existingObjects, indexOfLeastDistance);
		}
		else {
			addNewObject(currentFrameObject, existingObjects);
		}
	}

	for (auto &existingObject : existingObjects) {
		if (existingObject.matchOrNew == false) {
			existingObject.noMatch++;
		}

		if (existingObject.noMatch >= 10) {
			existingObject.beingTracked = false;
		}
	}
}

string to_string(int i) {
	stringstream s;
	s << i;
	return s.str();
}

int main(int argc, char* argv[])
{
	Mat capturedImage;
	Mat cameraFeed;
	Mat thresh;
	Mat empty_frame;
	Mat grey_frame;
	Mat difference;

	vector<Infected> knownNumberOfInfected;

	//CALL FUNCTION TO CREATE TRACKBARS
	createTrackbars();
	
	//INSTANTIATE A VIDEO CAPTURE OBJECT
	VideoCapture capture;

	//OPEN VIDEOCAPTURE FOR IMAGE AQUISITION
	capture.open(EvalVidShort);
	
	//LAST SETUP
	
	empty_frame = imread(EvalUnityBack); //FROM VIDEO

	bool firstFrame = true;
	
	while (1) { //WHILE FOREVER
				
		vector<Infected> currentNumbInfected;

		//IMAGE ACQUISITION
		capture >> cameraFeed;
		capture.read(capturedImage);

		if (cameraFeed.empty()) break; //BREAK IF CAMERAFEED IS EMPTY
		
		//MAKE A COPY OF THE INCOMING FRAMES FROM THE VIDEOCAPTURE
		Mat capturedImageCopy = capturedImage.clone();
		

			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>SEGMENTATION
			//BLUR CAMERA FEED TO REDUCE NOISE
			//GaussianBlur(capturedImageCopy, capturedImageCopy, Size(5, 5), 3.0, 3.0);

			//TAKE THE ABSOLUTE DIFFERENCE BETWEEN THE VERY FIRST FRAME AND THE CURRENT FRAME
			absdiff(capturedImageCopy, empty_frame, difference);			
			
			//CONVERT ABSOLUTE DIFFERENCE FRAME TO GRAYSCALE
			cvtColor(difference, difference, CV_BGR2GRAY);
			
			//THRESHOLD
			inRange(difference, 70, 200, thresh);
			
			//IMPLEMENT MORPHOLOGICAL OPERATIONS DEPENDING ON TRACKBARS
			morphOps(thresh, D_KERNEL_SIZE, E_KERNEL_SIZE, timesDilate, timesErode);
			
			//MAKE COPY OF THRESHOLDED IMAGE
			Mat threshCopy = thresh.clone();
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<SEGMENTATION 
			
			
			imshow("(Absolute difference", difference);
			imshow("grayscale", difference);


			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>FINDING CONTOURS FIRST ITERATION
			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;

			//FINDS CONTOURS IN TRHESHOLDED IMAGE
			//CV_RETR_TREE: Determines how the function should store the found contours in the hierachy vector. 
			//CV_RETR_TREE = retrieves all of the contours and reconstructs a full hierachy of nested contours
			//CV_CHAIN_APPROX_SIMPLE: stores only current contour positions
			findContours(threshCopy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

			Mat firstIteration(threshCopy.size(), CV_8UC3, Scalar(0, 0, 0));

			drawContours(firstIteration, contours, -1, Scalar(0, 255, 0), 1);
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FINDING CONTOURS FIRST ITERATION
			


			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>FILTERING UNWANTED OBJECTS
			vector<vector<Point>> secondContours;
			vector<Point> numPeople;
			Rect bBox;
			
			for (size_t i = 0; i < contours.size(); i++)
			{
				bBox = boundingRect(contours[i]);

				if (bBox.area() >= 700)
				{
					secondContours.push_back(contours[i]);
					numPeople.push_back(Point(bBox.width/2, bBox.height/2));
					currentNumbInfected.push_back(contours[i]);
					Infected possibleObject(numPeople);
				}
			}

			Mat secondIteration(threshCopy.size(), CV_8UC3, Scalar(0, 0, 0));

			drawContours(secondIteration, secondContours, -1, Scalar(20, 150, 20), CV_FILLED);
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<FILTERING UNWANTED OBJECTS
			
			for (auto &Infected : knownNumberOfInfected) {
				if (Infected.beingTracked == true) {
					secondContours.push_back(Infected.currentContour);
				}
			}
			
			if (firstFrame == true) {
				for (auto &currentNumbInfected : currentNumbInfected) {
					knownNumberOfInfected.push_back(currentNumbInfected);
				}
			}
			else {
				predictObjectMovement(knownNumberOfInfected, currentNumbInfected);
			}

			Mat lastContours(threshCopy.size(), CV_8UC3, Scalar(0, 0, 0));

			vector<vector<Point>> finalContours;

			for (auto &numberInfected : knownNumberOfInfected) {
				if (numberInfected.beingTracked == true) {
					finalContours.push_back(numberInfected.currentContour);
				}
			}

			//MAKING SURE ACCEPTED OBJECTS ARE BEING TRACKED
			drawContours(lastContours, finalContours, -1, Scalar(0, 255, 0), CV_FILLED);
				

			capturedImageCopy = capturedImage.clone();

			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>DETECTION RESULTS
			cout << "People Found:" << secondContours.size() << endl;
			cout << "People Accepted:" << numPeople.size() << endl;
			

			for (int i = 0; i < knownNumberOfInfected.size(); i++)
			{
				
				if (knownNumberOfInfected[i].beingTracked == true) {
					
					
					Point center;
					center.x = knownNumberOfInfected[i].currentBoundingRect.x + (knownNumberOfInfected[i].currentBoundingRect.width/2);
					center.y = knownNumberOfInfected[i].currentBoundingRect.y + (knownNumberOfInfected[i].currentBoundingRect.height / 2);
					circle(capturedImageCopy, center, 10, CV_RGB(20, 150, 20), 2);

					stringstream sstr;
					sstr << "(" << center.x << "," << center.y << ")" << "," << i <<")";
					double fontSize = knownNumberOfInfected[i].currentBoundingRect.height / 100.0;
					
					putText(capturedImageCopy, sstr.str(), Point(center.x, center.y), FONT_HERSHEY_SIMPLEX, fontSize, CV_RGB(200, 0, 20), 2);
					rectangle(capturedImageCopy, knownNumberOfInfected[i].currentBoundingRect, Scalar(0, 255, 0), 2);


					//PRINTS INFORMATION OF ALL TRACKED OBJECTS IN SPERATE FILES
					cout << "Object " << i << "is at position:" << " X: " << knownNumberOfInfected[i].currentBoundingRect.x + (knownNumberOfInfected[i].currentBoundingRect.width / 2) << " " << "& " << "Y: " << knownNumberOfInfected[i].currentBoundingRect.y + (knownNumberOfInfected[i].currentBoundingRect.height / 2) << endl;
					/*
					//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>WRITE FILES FOR UNITY
					const int numFiles = 50;
					string xfiles = "C:/Users/store/Desktop/2DMiniGameProject/Assets/txtFiles/Xpos_"; //Directory for x-files
					string yfiles = "C:/Users/store/Desktop/2DMiniGameProject/Assets/txtFiles/Ypos_"; //Directory for y-files
			
					ofstream X_output[numFiles]; //ofstream for x
					ofstream Y_output[numFiles]; //ofstream for y

					for (int j = 0; j < numFiles; ++j) {
						X_output[j].open(xfiles+ char('0' + i) + ".txt");
						Y_output[j].open(yfiles + char('0' + i) + ".txt");
					}

					for (int j = 0; j < numFiles; ++j) {
						X_output[j] << knownNumberOfInfected[i].currentBoundingRect.x + (knownNumberOfInfected[i].currentBoundingRect.width / 2); //Write x pos
						Y_output[j] << -1*(knownNumberOfInfected[i].currentBoundingRect.y + (knownNumberOfInfected[i].currentBoundingRect.height / 2)); //Write y pos
					}
					//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<WRITE FILES FOR UNITY
					*/
				}
				
			}
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<DETECTION RESULTS
					
			
			currentNumbInfected.clear();
			
			firstFrame = false;
	
			
		// SHOW RESULTS
		imshow(windowName2, thresh);
		imshow(windowName3, firstIteration);
		imshow(windowName4, secondIteration);
		imshow(windowName5, lastContours);
		imshow(windowName6, capturedImageCopy);
		imshow(windowName7, cameraFeed);
			
		//WAIT FOR "ESC" PRESS
		if (waitKey(1) == 27) break;
		
	}

	return 0;
}
