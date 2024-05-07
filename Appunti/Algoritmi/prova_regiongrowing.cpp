#include <opencv2/opencv.hpp>
#include <iostream>
#include <stack>
using namespace std;
using namespace cv;

Point PointShift[8] = {
	Point(0,0),
	Point(0,1),
	Point(1,0),
	Point(1,1),
	Point(0,-1),
	Point(-1,0),
	Point(-1,-1),
	Point(1,-1)
};

double min_area_factor = 0.01;
int max_region_num = 100;

void grow(Mat, Mat&, Mat&, Point, int);

int main(int argc, char** argv) {
	Mat src = imread(argv[1], -1);
	if(src.empty()) return -1;
	if(src.rows > 500 && src.cols > 500)
		resize(src, src, Size(0,0), 0.5, 0.5);
	uchar padding = 1;
	int min_area = int(min_area_factor * src.rows * src.cols);
	Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC1);
	Mat mask = Mat::zeros(src.rows, src.cols, CV_8UC1);
	for(int i = 0; i < src.cols; i++) {
		for(int j = 0; j < src.rows; j++) {
			if(dst.at<uchar>(Point(i,j)) == 0) {
				grow(src, dst, mask, Point(i,j), 200);
				int mask_area = (int) sum(mask).val[0];
				if(mask_area > min_area) {
					dst = dst + mask * padding;
					imshow("mask", mask * 255);
					waitKey(0);
					if(++padding > max_region_num) {
						cout << "out of range" << endl;
						exit(-2);
					} else {
						dst = dst + mask * 255;
					}
				}
				mask = mask - mask;
			}
		}
	}
}

void grow(Mat src, Mat& dst, Mat& mask, Point seed, int threshold) {
	stack<Point> stackP;
	stackP.push(seed);
	while(!stackP.empty()) {
		Point center = stackP.top();
		stackP.pop();
		mask.at<uchar>(center) = 1;
		for(int i = 0; i < 8; i++) {
			Point estP = center + PointShift[i];
			if(estP.x < 0 || estP.y < 0 || estP.x > src.cols - 1 || estP.y > src.rows - 1) continue;
			else {
				int delta = int(pow(src.at<Vec3b>(center)[0] - src.at<Vec3b>(estP)[0], 2)
					+ pow(src.at<Vec3b>(center)[1] - src.at<Vec3b>(estP)[1], 2)
					+ pow(src.at<Vec3b>(center)[2] - src.at<Vec3b>(estP)[2], 2));
				if(delta < threshold && mask.at<uchar>(estP) == 0 && dst.at<uchar>(estP) == 0) {
					mask.at<uchar>(estP) = 1;
					stackP.push(estP);
				}
			}
		}
	}
}







