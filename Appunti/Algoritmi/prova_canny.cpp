#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void canny(Mat, Mat&);

int main(int argc, char** argv) {
	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	if(src.empty()) return -1;
	Mat dst;
	canny(src, dst);
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}

void canny(Mat src, Mat& dst, float low, float high) {
	Mat gaus, dx, dy;
	GaussianBlur(src, gaus, Size(3,3), 0);
	Sobel(gaus, dx, CV_32FC1, 1, 0);
	Sobel(gaus, dy, CV_32FC1, 0, 1);
	magnitude(dx, dy, dst);
	dst.convertTo(dst, CV_8U);
	Mat orient;
	phase(dx, dy, orient, true);
	for(int i = 0; i < src.rows; i++) {
		for(int j = 0; j < src.cols; j++) {
			float angle = orient.at<float>(i,j);
			uchar magV = dst.at<uchar>(i,j);
			if((-22.5 < angle && angle <= 22.5) || (-157.5 < angle && angle <= 157.5))
			
			else if(() || ())
			
			else if(() || ())
			
			else
		}
	}
}
