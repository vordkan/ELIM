#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat paddedImg (Mat img, int padSize, Scalar color, int padType) {
    Mat padded_image;
    copyMakeBorder(img, padded_image, padSize, padSize, padSize, padSize, padType, color);
    return padded_image;
}

void clearScreen() {
    // Pulisce lo schermo
    cout << "\033[2J\033[1;1H";
}


int main( int argc, char** argv ) {
    int paddingSize = 0;
    int paddingType = 0;
    int choice = -1;
    Mat img = imread("../sus.png");
    if( img.empty() ) 
        return -1;

    // int padding_type = BORDER_CONSTANT; // Costante, riempi con un valore costante
    //int padding_type = BORDER_REFLECT; // Riflessione
    //int padding_type = BORDER_REPLICATE; // Replica i bordi esterni

    Scalar color = Scalar(0, 255, 0); // Valore in formato RGB

    cout << "Choose your padding size: " << endl;
    cin >> paddingSize;

    while(choice != 0){
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
                paddingType = BORDER_CONSTANT;
                break;
            case 2:
                paddingType = BORDER_REPLICATE;
                break;
            case 3:
                paddingType = BORDER_REFLECT;
                break;
            case 4:
                paddingType = BORDER_WRAP;
                break;
            case 5:
                paddingType = BORDER_REFLECT_101;
                break;
            
            default:
                cout << "tipo di padding non valido" << endl;
                break;
        }

        imshow("Original Image", img);
        imshow("Padded Image", paddedImg(img, paddingSize, color, paddingType));
        waitKey( 0 );
    }
    
    
    // destroyWindow( "Example1");
    return 0;
}