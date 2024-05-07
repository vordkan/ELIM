#include <opencv2/opencv.hpp>
#include <stdlib.h>
using namespace std;
using namespace cv;

/*
   1. Convolvere l'immagine di input con un filtro Gaussiano
   2. Calcolare la magnitudo e l'orientazione del gradiente
   3. Applicare la non-maxima-suppression
   4. Applicare il thresholding con isteresi
*/

void canny(Mat, Mat&, float, float);

int main(int argc, char * argv[]) {
    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    Mat edgesCanny;
    canny(src, edgesCanny, 30, 30 * 2.4);
    imshow("Input image", src);
    imshow("Canny image", edgesCanny);
    waitKey(0);
    return 0;
}

void canny(Mat src, Mat& dst, float thLow, float thHigh) {
	// applico filtro gaussiano
	Mat gaus;
	GaussianBlur(src, gaus, Size(3,3), 0);
	// calcolo della magnitudo
	Mat dx, dy;
	Sobel(gaus, dx, CV_32FC1, 1, 0);
	Sobel(gaus, dy, CV_32FC1, 0, 1);
	magnitude(dx, dy, dst);
	// calcolo dell'orientazione del gradiente
	dst.convertTo(dst, CV_8U);
	Mat orientation;
	phase(dx, dy, orientation, true);
	// non-maxima-suppression
	for(int i = 1; i < dst.rows-1; i++) {
		for(int j = 1; j < dst.cols-1; j++) {
			float angle = orientation.at<float>(i,j);
			uchar magValue = dst.at<uchar>(i,j);
			if((-22.5 < angle && angle <= 22.5) || (-157.5 < angle && angle <= 157.5)) { // orizzontale
				if(magValue < dst.at<uchar>(i,j-1) && magValue < dst.at<uchar>(i,j+1))
					dst.at<uchar>(i,j) = 0;
			} else if((-112.5 < angle && angle <= -67.5) || (67.5 < angle && angle <= 112.5)) { // verticale
				if(magValue < dst.at<uchar>(i-1,j) && magValue < dst.at<uchar>(i+1,j))
					dst.at<uchar>(i,j) = 0;
			} else if((-67.5 < angle && angle <= -22.5) || (112.5 < angle && angle <= 157.5)) { // + 45
				if(magValue < dst.at<uchar>(i-1,j-1) && magValue < dst.at<uchar>(i+1,j+1))
					dst.at<uchar>(i,j) = 0;
			} else { // -45
				if(magValue < dst.at<uchar>(i-1,j+1) && magValue < dst.at<uchar>(i+1,j-1))
					dst.at<uchar>(i,j) = 0;
			}
		}
	}
	// thresholding con isteresi
	for(int i = 1; i < dst.rows-1; i++) {
		for(int j = 1; j < dst.cols-1; j++) {
			if(dst.at<uchar>(i,j) > thHigh)
				dst.at<uchar>(i,j) = 255;
			else if(dst.at<uchar>(i,j) <= thLow)
				dst.at<uchar>(i,j) = 0;
			else {
				bool strong = false;
				for(int k = -1; k <= 1 && !strong; k++) {
					for(int l = -1; l <= 1 && !strong; l++) {
						if(dst.at<uchar>(i+k,j+l) > thHigh)
							strong = true;
					}
				}
				if(strong)
					dst.at<uchar>(i,j) = 255;
				else
					dst.at<uchar>(i,j) = 0;
			}
		}
	}
}
