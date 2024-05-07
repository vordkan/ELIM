#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void harrisCircle(Mat, Mat&, int);
void harris(Mat, Mat&, float, int);

int main(int argc, char** argv) {
	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	if(src.empty()) return -1;
	Mat dst;
	harris(src, dst, 0.04, 100);
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}

void harrisCircle(Mat dst, Mat& src, int threshold) {
	for(int i = 0; i < src.rows; i++) {
		for(int j = 0; j < src.cols; j++) {
			if((int)src.at<float>(i,j) > threshold) {
				circle(dst, Point(j,i), 5, Scalar(0));
			}
		}
	}
}

void harris(Mat src, Mat& dst, float k, int threshold) {
	src.copyTo(dst);
	Mat dx, dy, dx2, dy2, dxy;
	Sobel(src, dx, CV_32FC1, 1, 0);
	Sobel(src, dy, CV_32FC1, 0, 1);
	pow(dx, 2, dx2);
	pow(dy, 2, dy2);
	multiply(dx, dy, dxy);
	Mat C00, C01, C10, C11;
	GaussianBlur(dx2, C00, Size(3,3), 0);
	GaussianBlur(dy2, C11, Size(3,3), 0);
	GaussianBlur(dxy, C01, Size(3,3), 0);
	C10 = C01;
	Mat trace, R, det, mainDiag, secondDiag;
	multiply(C00, C11, mainDiag);
	multiply(C01, C10, secondDiag);
	trace = C00 + C11;
	det = mainDiag - secondDiag;
	pow(trace, 2, trace);
	R = det - k * trace;
	normalize(R, R, 0, 255, NORM_MINMAX);
	harrisCircle(src, dst, threshold);
}
