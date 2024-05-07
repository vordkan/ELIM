#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void houghCircles(Mat, Mat&, int, int);

int main(int argc, char** argv) {
	Mat img = imread(argv[1], IMREAD_GRAYSCALE);
	if(img.empty()) return -1;
	Mat ris;
	houghCircles(img, ris, 20, 100);
	imshow("Originale", img);
	imshow("HoughCircles", ris);
	waitKey(0);
	return 0;	
}

void houghCircles(Mat src, Mat& dst, int r_min, int r_max) {
	src.copyTo(dst);
	Mat gaus, cannyimg;
	GaussianBlur(src, gaus, Size(3,3), 0);
	Canny(gaus, cannyimg, 70, 180);
	int sizes[] = {cannyimg.rows, cannyimg.cols, r_max - r_min + 1};
	Mat votes = Mat::zeros(3, sizes, CV_8U);
	for(int i = 0; i < cannyimg.rows; i++) {
		for(int j = 0; j < cannyimg.cols; j++) {
			if(cannyimg.at<uchar>(i,j) == 255) {
				for(int r = r_min; r <= r_max; r++) {
					for(int theta = 0; theta <= 360; theta++) {
						int a = j - r * cos(theta * M_PI / 180);
						int b = i - r * sin(theta * M_PI / 180);
						if(a >= 0 && a < cannyimg.cols && b >= 0 && b < cannyimg.rows)
							votes.at<uchar>(b, a, r - r_min)++;
					}
				}
			}
		}
	}
	for(int r = r_min; r <= r_max; r++)
		for(int b = 0; b < cannyimg.rows; b++)
			for(int a = 0; a < cannyimg.cols; a++)
				if(votes.at<uchar>(b, a, r - r_min) >= 115) {
					circle(dst, Point(a, b), 3, Scalar(0), 2);
					circle(dst, Point(a, b), r, Scalar(0), 2);
				}
}
