#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace cv;

int main( int argc, char** argv ){

    if( argc < 2){
        cout<<"usage: "<<argv[0]<<" image_name"<<endl;
        exit(0);
    }
    String imageName = argv[1];

	//Lettura immagine    
    Mat image;
    image = imread( samples::findFile( imageName ), IMREAD_GRAYSCALE );
    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    Mat output1;
    Laplacian(image,output1,image.type());

    Mat output12;
    normalize(output1, output12, 0, 255, NORM_MINMAX, image.type());
 
    Mat output2;
    Sobel(image,output2,image.type(),1,1);

	Mat output3;
    Sobel(image,output3,image.type(),0,1);

    Mat output4;
    Sobel(image,output4,image.type(),1,0);    

    imshow("Original image",image);
    imshow("Laplacian image",output1);
    imshow("LaplacianN image",output12);
    imshow("Sobel image",output2);
    imshow("SobelX image",output3);
    imshow("SobelY image",output4);
    waitKey(0);
}
