#include <opencv2/opencv.hpp>
using namespace cv;

void polarToCartesian(double, int, Point&, Point&);
void houghLines(Mat, Mat&, int, int, int);

int main(int argc, char** argv) {
	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	Mat dst;
	houghLines(src, dst, 40, 140, 110);
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}

void polarToCartesian(double rho, int theta, Point& p1, Point& p2) {
	int alpha = 1000;
	int x0 = cvRound(rho * cos(theta));
	int y0 = cvRound(rho * sin(theta));
	p1.x = cvRound(x0 + alpha * (-sin(theta)));
	p1.y = cvRound(y0 + alpha * (cos(theta)));
	p2.x = cvRound(x0 - alpha * (-sin(theta)));
	p2.y = cvRound(y0 - alpha * (cos(theta)));
}

void houghLines(Mat src, Mat& dst, int low, int high, int th) {
	int maxDist = hypot(src.rows, src.cols);
	Mat votes = Mat::zeros(maxDist * 2, 180, CV_8U);
	Mat gaus, canny;
	GaussianBlur(src, gaus, Size(3,3), 0);
	Canny(gaus, canny, low, high);
	double rho;
	int theta;
	for(int i = 0; i < canny.rows; i++) {
		for(int j = 0; j < canny.cols; j++) {
			if(canny.at<uchar>(i,j) == 255) {
				for(theta = 0; theta <= 180; theta++) {
					rho = round(j * cos(theta-90) + i * sin(theta-90)) + maxDist;
					votes.at<uchar>(rho,theta)++;
				}
			}
		}
	}
	src.copyTo(dst);
	Point p1, p2;
	for(int i = 0; i < votes.rows; i++) {
		for(int j = 0; j < votes.cols; j++) {
			if(votes.at<uchar>(i,j) >= th) {
				rho = i - maxDist;
				theta = j - 90;
				polarToCartesian(rho, theta, p1, p2);
				line(dst, p1, p2, Scalar(0));
			}
		}
	}
}
