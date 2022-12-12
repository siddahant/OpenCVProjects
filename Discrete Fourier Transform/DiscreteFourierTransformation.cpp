#include "opencv2\opencv.hpp"
#include <stdint.h>
using namespace cv;
using namespace std;


void takeDFT(Mat& Source, Mat& Destination)
{
	Mat Complex[2] = { Source, Mat::zeros(Source.size(), CV_32F) };

	Mat DftReady;
	merge(Complex, 2, DftReady);

	Mat DftOfOrignal;

	dft(DftReady, DftOfOrignal, DFT_COMPLEX_INPUT);

	Destination = DftOfOrignal;
}

void RecenterDFT(Mat& source)
{
	int CenterX = source.cols / 2;
	int CenterY = source.rows / 2;

	Mat q1(source, Rect(0, 0, CenterX, CenterY));
	Mat q2(source, Rect(CenterX, 0, CenterX, CenterY));
	Mat q3(source, Rect(0, CenterY, CenterX, CenterY));
	Mat q4(source, Rect(CenterX, CenterY, CenterX, CenterY));

	Mat TempSwap;
	q1.copyTo(TempSwap);
	q4.copyTo(q1);
	TempSwap.copyTo(q4);

	q2.copyTo(TempSwap);
	q3.copyTo(q2);
	TempSwap.copyTo(q3);

}

void showDFT(Mat& source)
{
	Mat splitArray[2] = { Mat::zeros(source.size(), CV_32F), Mat::zeros(source.size(),CV_32F) };

	split(source, splitArray);
	Mat DftMagnitude;
	magnitude(splitArray[0], splitArray[1], DftMagnitude);
	DftMagnitude += Scalar::all(1);
	log(DftMagnitude, DftMagnitude);
	normalize(DftMagnitude, DftMagnitude, 0, 1, CV_MINMAX);

	RecenterDFT(DftMagnitude);

	imshow("dft", DftMagnitude);
	waitKey();
}

void invertDFT(Mat& source, Mat& Destination)
{
	Mat inverse;
	dft(source, inverse, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);
	Destination = inverse;
}


int main(int argv, char** argc) {
	Mat Orignal = imread("TEST_IMG.jpeg", CV_LOAD_IMAGE_COLOR);

	Mat SplitChannel[3];

	split(Orignal, SplitChannel);

	Mat ChannelOneFloat;

	SplitChannel[0].convertTo(ChannelOneFloat, CV_32FC1, 1.0 / 255.0);

	Mat DftOfChannelOne;

	takeDFT(ChannelOneFloat, DftOfChannelOne);

	showDFT(DftOfChannelOne);

	Mat inverted;

	invertDFT(DftOfChannelOne, inverted);

	imshow("inverted DFT", inverted);
	waitKey();

}