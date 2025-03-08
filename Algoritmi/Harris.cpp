#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char ** argv) {

    // lettura immagine in scala di grigio
    Mat image = imread(argv[1], IMREAD_GRAYSCALE);
    if(image.empty()) {
        cout <<  "Errore lettura immagine" << endl;
        return -1;
    }
    imshow("Immagine di input", image);
    waitKey(0);

    // applico un filtro gaussiano per lo smoothing (riduzione del rumore)
    GaussianBlur(image, image, Size(9, 9), 0, 0);

    // calcolo le derivate parziali con Sobel
    Mat Dx, Dy, Dxy;
    Sobel(image, Dx, CV_32F, 1, 0, 3); // derivata parziale rispetto ad x
    Sobel(image, Dy, CV_32F, 0, 1, 3); // derivata parziale rispetto ad y

    // calcolo le derivate seconde e le componenti della diagonale
    multiply(Dx, Dy, Dxy);
    pow(Dx, 2, Dx);
    pow(Dy, 2, Dy);

    // applico Gaussian blur per ridurre il rumore sulle componenti della matrice
    GaussianBlur(Dx, Dx, Size(9, 9), 0, 0);
    GaussianBlur(Dy, Dy, Size(9, 9), 0, 0);
    GaussianBlur(Dxy, Dxy, Size(9, 9), 0, 0);

    // calcolo R usando determinante e traccia
    Mat diagonal1, diagonal2; 
    multiply(Dx, Dy, diagonal1); 
    multiply(Dxy, Dxy, diagonal2); 
    Mat trace = Dx + Dy;
    pow(trace, 2, trace); 
    Mat R = diagonal1 - diagonal2 - 0.04 * trace; 
    normalize(R, R, 0, 255, NORM_MINMAX, CV_32F); 

    // mostro gli angoli nell'immagine di output
    Mat corners = Mat::zeros(image.size(), image.type()); 
    for (int i = 0; i < image.rows; i++) { 
        for (int j = 0; j < image.cols; j++) { 
            if (R.at<float>(i, j) > atoi(argv[2])) { 
                circle(corners, Point(j, i), 5, Scalar(255), 2, 8, 0); 
            }
        }
    }
    imshow("Corner dell'immagine", corners);
    waitKey(0);
    return 0;
}