#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main( int argc, char** argv ) {
    Mat img = imread("../sus.png");
    if( img.empty() ) 
        return -1;
    namedWindow( "Example1", WINDOW_AUTOSIZE );
    imshow( "Example1", img );
    waitKey( 0 );
    destroyWindow( "Example1" );
    return 0;
}
