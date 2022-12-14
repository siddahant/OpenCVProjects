#include "opencv2\core.hpp",
#include "opencv2\imgcodecs.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\calib3d.hpp"

#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

const float calibrationSquareDimension = 0.01905f; // meters
const Size chessboardDimension = Size(6, 9);


void createKnownBoardPositions(Size boardSize, float squareEdgeLength,vector<Point3f>& cornors) {

	for (int i = 0; i < boardSize.height; i++) {

		for (int j = 0; i < boardSize.width; j++) {

			cornors.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.00f));

		}
	}

}
void getChessboardCornors(vector<Mat> images, vector<vector<Point2f>>& allFoundCornors, bool showResults = false ) {
	
	for (vector<Mat> ::iterator iter = images.begin(); iter != images.end(); iter++) {

		vector<Point2f> pointBuf;
		
		bool found = findChessboardCorners(*iter, Size(9, 6), pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

		if (found)
		{
			allFoundCornors.push_back(pointBuf);

		}
		if (showResults) {

			drawChessboardCorners(*iter, Size(9, 6), pointBuf, found);
			imshow("Looking for image", *iter);
			waitKey(0);
		}
	}

}

void cameracalibration(vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficents) {

	vector<vector<Point2f>> chessboeadImageSpacePoints;
	getChessboardCornors(calibrationImages, chessboeadImageSpacePoints, false);

	vector<vector<Point3f>> worldSpaceCornorPoints(1);

	createKnownBoardPositions(chessboardDimension, squareEdgeLength, worldSpaceCornorPoints[0]);
	worldSpaceCornorPoints.resize(chessboeadImageSpacePoints.size(), worldSpaceCornorPoints[0]);

	vector<Mat> rVector, tVector;
	distanceCoefficents = Mat::zeros(8, 1, CV_64F);

	calibrateCamera(worldSpaceCornorPoints, chessboeadImageSpacePoints, boardSize, cameraMatrix, distanceCoefficents, rVector, tVector);



}

bool savedCameraCalibration(string name, Mat cameraMatrix, Mat distacneCoefficients) {
	ofstream outStream(name);
	if (outStream)
	{
		uint16_t rows = cameraMatrix.rows;
		uint16_t coloums = cameraMatrix.cols;

		for (int r = 0; r < rows; r++) 
		{
			for (int c = 0; c < coloums; c++)
			{
				double value = cameraMatrix.at<double>(r, c);
				outStream << value << endl;
			}
		}

		rows = distacneCoefficients.rows;
		coloums = distacneCoefficients.cols;

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < coloums; c++)
			{
				double value = distacneCoefficients.at<double>(r, c);
				outStream << value << endl;
			}
		}
		outStream.close();
		return true;
	}
	return false;
}


const int fps = 200;

int main(int argv, char** argc) {
	Mat frame;
	Mat drawToFrame;

	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);

	Mat distanceCoefficents;

	vector<Mat> savedImages;

	vector<vector<Point2f>> markerCorners, rejectedCandidated;

	VideoCapture vid(0);

	if (!vid.isOpened()){

		return -1;

	}

	int framePerSecond = 20; 
	namedWindow("webcam", CV_WINDOW_AUTOSIZE);

	while (true) {
		if (!vid.read(frame)) {
			break;
		}

		vector<Vec2f> foundPoinst;

		bool found = false;
		
		found = findChessboardCorners(frame, chessboardDimension, foundPoinst, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		frame.copyTo(drawToFrame);
		drawChessboardCorners(drawToFrame, chessboardDimension, foundPoinst, found);
		if (found)
			imshow("webcam", drawToFrame);
		else
			imshow("webcam", frame);	
		char character = waitKey(1000 / framePerSecond);

		switch (character)
		{
		case ' ':
			//saving image
			if (found) 
			{
				Mat temp;
				frame.copyTo(temp);
				savedImages.push_back(temp);
				cout << "saved" << endl;
			}
			break;
		case 13:
			// start calibration
			if (savedImages.size() > 1)
			{
				cout << "calibration start" << endl;
				cameracalibration(savedImages, chessboardDimension, calibrationSquareDimension, cameraMatrix, distanceCoefficents);
				savedCameraCalibration("calibration", cameraMatrix, distanceCoefficents);
			}
			break;
			cout << "Done Calibratoin" << endl;
		case 27: 
			//exit
			cout << "Exiting...." << endl;
			return 0;
			break;
		}
	}
	return 0;

}