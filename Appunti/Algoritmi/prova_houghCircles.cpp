#include <opencv2/opencv.hpp>
using namespace cv;

void houghCircles(Mat, Mat&, int, int, int);

int main(int argc, char** argv) {
	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	if(src.empty()) return -1;
	Mat dst;
	houghCircles(src, dst, 30, 60, 115);
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}

void houghCircles(Mat src, Mat& dst, int r_min, int r_max, int th) {
	Mat gaus, canny;
	GaussianBlur(src, gaus, Size(3,3), 0);
	Canny(gaus, canny, 30, 130);
	int size[] = {canny.rows, canny.cols, r_max - r_min + 1};
	Mat votes = Mat::zeros(3, size, CV_8U);
	for(int i = 0; i < canny.rows; i++) {
		for(int j = 0; j < canny.cols; j++) {
			if(canny.at<uchar>(i,j) == 255) {
				for(int r = r_min; r <= r_max; r++) {
					for(int theta = 0; theta < 360; theta++) {
						int a = j - r * cos(theta * M_PI / 180);
						int b = i - r * sin(theta * M_PI / 180);
						if(a >= 0 && b >= 0 && a < src.cols && b < src.rows) {
							votes.at<uchar>(b, a, r - r_min)++;
						}
					}
				}
			}
		}
	}
	src.copyTo(dst);
	for(int r = r_min; r <= r_max; r++) {
		for(int i = 0; i < canny.rows; i++) {
			for(int j = 0; j < canny.cols; j++) {
				if(votes.at<uchar>(i, j, r - r_min) >= th) {
					circle(dst, Point(j,i), 3, Scalar(0), 2);
					circle(dst, Point(j,i), r, Scalar(0), 2);
				}
			}
		}
	}
}
