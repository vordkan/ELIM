#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


void clearScreen() {
    // Pulisce lo schermo
    cout << "\033[2J\033[1;1H";
}

Mat paddedImg (Mat img) {
    Mat padded_image;
    int padSize = 0;
    int padType = 0;
    int choice = -1;

    Scalar color = Scalar(0, 255, 0); // Valore in formato RGB

    cout << "Choose your padding size: " << endl;
    cin >> padSize;

    clearScreen();
    cout << "Choose your padding type: " << endl;
    cout << "1. BORDER_COSTANT" << endl;
    cout << "2. BORDER_REPLICATE" << endl;
    cout << "3. BORDER_REFLECT" << endl;
    cout << "4. BORDER_WRAP" << endl;
    cout << "5. BORDER_REFLECT_101" << endl;
    cout << "0. exit" << endl;
    cout << "your choose: ";
    cin >> choice;

    switch (choice)
    {
        case 1:
            padType = BORDER_CONSTANT;
            break;
        case 2:
            padType = BORDER_REPLICATE;
            break;
        case 3:
            padType = BORDER_REFLECT;
            break;
        case 4:
            padType = BORDER_WRAP;
            break;
        case 5:
            padType = BORDER_REFLECT_101;
            break;
        
        default:
            cout << "tipo di padding non valido" << endl;
            break;
    }

    copyMakeBorder(img, padded_image, padSize, padSize, padSize, padSize, padType, color);
    return padded_image;
}

Mat blurredImg (Mat img){

    // Creare una copia dell'immagine originale per mantenere i valori originali
    //Mat blurred = img.clone();
    Mat blurred = Mat::zeros(img.size(), CV_8UC1);

    // Scorrere l'immagine pixel per pixel
    for (int y = 1; y < img.rows - 1; ++y) {
        for (int x = 1; x < img.cols - 1; ++x) {
            float sum = 0.0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    sum += img.at<uchar>(y + dy, x + dx);
                }
            }
            float avg = sum / 9.0;

            blurred.at<uchar>(y, x) = static_cast<uchar>(avg);
        }
    }
    return blurred;
};

int main( int argc, char** argv ) {
    Mat img = imread("../sus.png", IMREAD_GRAYSCALE);
    if( img.empty() ) 
        return -1;

    // int padding_type = BORDER_CONSTANT; // Costante, riempi con un valore costante
    //int padding_type = BORDER_REFLECT; // Riflessione
    //int padding_type = BORDER_REPLICATE; // Replica i bordi esterni
    
    Mat padded_img = paddedImg(img);

    imshow("Original Image", img);
    imshow("Padded Image", padded_img);
    imshow("Blurred Image", blurredImg(padded_img));

    waitKey( 0 );
    
    // destroyWindow( "Example1");
    return 0;
}