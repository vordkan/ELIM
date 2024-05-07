#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace cv;

int otsu(Mat);

int main(int argc, char **argv) {
	Mat image = imread(argv[1], IMREAD_GRAYSCALE);
	Mat output;
	if(image.empty()) return -1;
	imshow("Originale", image);
	int th = otsu(image);
	threshold(image, output, th, 255, THRESH_BINARY);
	imshow("Otsu", output);
	waitKey(0);
	return 0;
}

int otsu(Mat img) {
	float sigma_max = 0.0;
	int MxN = img.rows * img.cols;
	vector<float> hist(MxN, 0.0);
	int output = 0;
	for(int i = 0; i < img.rows; i++)
		for(int j = 0; j < img.cols; j++)
			hist.at(img.at<uchar>(i,j))++;
	for(int i = 0; i < MxN; i++)
		hist.at(i) /= MxN;
	for(int k = 0; k < 256; k++) {
		float P1 = 0.0, P2 = 0.0;
		for(int i = 0; i <= k; i++)
			P1 += hist.at(i);
		P2 = 1.0 - P1;
		float med_cum_A = 0.0, med_cum_B = 0.0;
		for(int i = 1; i <= k; i++)
			med_cum_A += hist.at(i) * i;
		for(int i = k + 1; i < 256; i++)
			med_cum_B += hist.at(i) * i;
		float sigma = P1 * P2 * ((med_cum_A - med_cum_B)*(med_cum_A - med_cum_B));
		if(sigma > sigma_max) {
			sigma_max = sigma;
			output = k;
		}
	}
	cout << "Output: " << output << endl;
	return output;
}
