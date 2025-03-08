#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// sopprime i picchi non massimi
Mat nonMaximaSuppression(Mat& magnitude, Mat&orientation) { 
    Mat nmsImage = magnitude.clone(); 
    float angle, next, prev, curr;
    for (int i = 1; i < magnitude.rows - 1; i++) {
        for (int j = 1; j < magnitude.cols - 1; j++) {
            angle = orientation.at<float>(i, j); 
            if (angle > 180) { 
                angle -= 360;
            }
            if ((angle <= 22.5 && angle > -22.5) || (angle > 157.5 || angle <= -157.5)) { // edge orizzontale
                next = magnitude.at<float>(i, j + 1); 
                prev = magnitude.at<float>(i, j - 1); 
            } else if ((angle <= 67.5 && angle > 22.5) || (angle > -157.5 && angle <= -112.5)) { // edge -45 gradi
                next = magnitude.at<float>(i + 1, j - 1); 
                prev = magnitude.at<float>(i - 1, j + 1); 
            } else if((angle <= 157.5 && angle > 112.5) || (angle > -67.5 && angle <= -22.5)) { // edge 45 gradi
                next = magnitude.at<float>(i - 1, j - 1); 
                prev = magnitude.at<float>(i + 1, j + 1); 
            } else if ((angle <= 112.5 && angle > 67.5) || (angle > -112.5 && angle <= -67.5)) { // edge verticale
                next = magnitude.at<float>(i + 1, j);
                prev = magnitude.at<float>(i - 1, j); 
            }
            curr = magnitude.at<float>(i, j); 
            if (curr < prev || curr < next) { 
                nmsImage.at<float>(i, j) = 0;
            }
        }
    }
    return nmsImage; 
}

// applica il thresholding con due soglie
Mat thresholdingWithHysteresis(Mat& nmsImage, int lowThreshold, int highThreshold) {
    Mat edges = Mat::zeros(nmsImage.size(), nmsImage.type()); 
    for (int i = 1; i < nmsImage.rows - 1; i++) {
        for (int j = 1; j < nmsImage.cols - 1; j++) { 
            if (nmsImage.at<float>(i, j) > highThreshold) { 
                edges.at<float>(i, j) = 255; 
            } else if (nmsImage.at<float>(i, j) > lowThreshold) { 
                for (int u = -1; u <= 1; u++) { 
                    for (int v = -1; v <= 1; v++) {
                        if (nmsImage.at<float>(i + u, j + v) > highThreshold) {
                            edges.at<float>(i, j) = 255; 
                        }
                    }
                }
            }
        }
    }
    return edges; 
}

int main(int argc, char ** argv) {
    Mat image = imread(argv[1], IMREAD_GRAYSCALE);
    if(image.empty()) {
        cout <<  "Errore lettura immagine" << endl;
        return -1;
    }
    imshow("Immagine di input", image);
    waitKey(0);

    GaussianBlur(image, image, Size(9, 9), 0, 0);

    Mat Dx, Dy;
    Sobel(image, Dx, CV_32F, 1, 0, 3);
    Sobel(image, Dy, CV_32F, 0, 1, 3);

    Mat orientation;
    phase(Dx, Dy, orientation, true);

    Mat magnitude;
    pow(Dx, 2, Dx);
    pow(Dy, 2, Dy);
    sqrt(Dx + Dy, magnitude);
    normalize(magnitude, magnitude, 0, 255, NORM_MINMAX, CV_32F);

    // applico la non maxima suppression (sopprimo i picchi con valore non massimo)
    Mat nmsImage = nonMaximaSuppression(magnitude, orientation);

    // applico il thresholding con isteresi
    Mat edges = thresholdingWithHysteresis(nmsImage, atoi(argv[2]), atoi(argv[3]));
    imshow("Bordi dell'immagine", edges);
    waitKey(0);
    return 0;
}