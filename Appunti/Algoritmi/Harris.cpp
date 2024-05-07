#include <opencv2/opencv.hpp>
using namespace cv;

/*
	1. Calcolare le derivate parziali rispetto ad x e y
	2. Calcolare Dx^2, Dy^2, e Dxy
	3. Applicare un filtro gaussiano a Dx^2, Dy^2 e Dxy, ottenendo così C00, C11, C01 e C10
	4. Calcolare l'indice R
	5. Normalizzare l'indice R in [0,255]
	6. Sogliare R
*/

void harris(Mat, Mat&, int, float);
void circleHarris(Mat, Mat&, int);

int main(int argc, char** argv) {
	Mat img = imread(argv[1], IMREAD_GRAYSCALE);
	if(img.empty()) return -1;

	Mat ris;
	harris(img, ris, 100, 0.04);

	imshow("Originale", img);
	imshow("Harris", ris);
	waitKey(0);
	return 0;
}

void circleHarris(Mat src, Mat& dst, int threshold) {
	for(int i = 0; i < src.rows; i++) {
		for(int j = 0; j < src.cols; j++) {
			if((int)src.at<float>(i,j) > threshold)
				circle(dst, Point(j,i), 5, Scalar(0));
		}
	}
}

void harris(Mat src, Mat& dst, int threshold, float k) {
	src.copyTo(dst);
	// calcolo delle derivate parziali
	Mat dx, dy, dx2, dy2, dxy;
	Sobel(src, dx, CV_32FC1, 1, 0);
	Sobel(src, dy, CV_32FC1, 0, 1);
	// calcolo di dx2, dy2 e dxy
	pow(dx, 2, dx2);
	pow(dy, 2, dy2);
	multiply(dx, dy, dxy);
	// applico filtro gaussiano a dx2, dy2 e dxy
	Mat C00, C11, C10, C01;
	GaussianBlur(dx2, C00, Size(3,3), 0);
	GaussianBlur(dy2, C11, Size(3,3), 0);
	GaussianBlur(dxy, C01, Size(3,3), 0);
	C10 = C01;
	// calcolo di R = determinante - k * traccia^2
	Mat mainDiag, secondDiag, determ, trace, R;
	multiply(C00, C11, mainDiag);
	multiply(C01, C10, secondDiag);
	determ = mainDiag - secondDiag;
	trace = C00 + C11;
	pow(trace, 2, trace);
	R = determ - (k * trace);
	// normalizzazione di R
	normalize(R, R, 0, 255, NORM_MINMAX);
	circleHarris(R, dst, threshold);
}
