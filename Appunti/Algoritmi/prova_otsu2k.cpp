#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void otsu(Mat, int&, int&);

int main(int argc, char** argv) {
	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	if(src.empty()) return -1;
	int th1, th2;
	otsu(src, th1, th2);
	Mat dst = src.clone();
	for(int i = 0; i < src.rows; i++) {
		for(int j = 0; j < src.cols; j++) {
			if(dst.at<uchar>(i,j) > th2) dst.at<uchar>(i,j) = 255;
			else if(dst.at<uchar>(i,j) > th1) dst.at<uchar>(i,j) = 127;
			else dst.at<uchar>(i,j) = 0;
		}
	}
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}

void otsu(Mat src, int& th1, int& th2) {
	int dim = src.rows * src.cols;
	int tmp1, tmp2;
	float sigma[256][256] = {0.0};
	float sigma_max = 0.0;
	float mg = 0.0;
	vector<float> hist(256, 0.0);
	for(int i = 0; i < 256; i++)
		for(int j = 0; j < 256; j++)
			hist.at(src.at<uchar>(i,j))++;
	for(int i = 0; i < 256; i++) {
		hist.at(i) /= dim;
		mg += hist.at(i) * i;
	}
	for(int k = 0; k < 256; k++) {
		for(int l = k + 1; l < 256; l++) {
			float p1 = 0.0, p2 = 0.0, p3 = 0.0;
			float ma = 0.0, mb = 0.0, mc = 0.0;
			for(int i = 0; i <= k; i++) p1 += hist.at(i);
			for(int i = k + 1; i <= l; i++) p2 += hist.at(i);
			for(int i = l + 1; i < 256; i++) p3 += hist.at(i);
			
			for(int i = 0; i <= k; i++) ma += hist.at(i) * i / p1;
			for(int i = k + 1; i <= l; i++) mb += hist.at(i) * i / p2;
			for(int i = l + 1; i < 256; i++) mc += hist.at(i) * i / p3;
			
			sigma[k][l] = p1*(ma-mg)*(ma-mg)+p2*(mb-mg)*(mb-mg)+p3*(mc-mg)*(mc-mg);
			if(sigma[k][l] > sigma_max) {
				sigma_max = sigma[k][l];
				tmp1 = k;
				tmp2 = l;
			}
		}
		th1 = tmp1;
		th2 = tmp2;
	}
}
