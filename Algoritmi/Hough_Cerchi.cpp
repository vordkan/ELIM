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

    // applico l'algoritmo di Canny per rilevare gli edge
    Mat edges;
    Canny(image, edges, 30, 40, 3);
    imshow("Bordi dell'immagine", edges);
    waitKey(0);

    int minR = 50,  
        maxR = 65,  
        size[] = {edges.rows, edges.cols, maxR - minR + 1};
    Mat votes = Mat(3, size, CV_32F); 
    for (int y = 0; y < edges.rows; y++) { 
        for (int x = 0; x < edges.cols; x++) { 
            if (edges.at<uchar>(y, x) == 255) {
                for (int r = minR; r <= maxR; r++) { 
                    for (int theta = 0; theta < 360; theta++) { 
                        int a = x - r * cos(theta * CV_PI / 180); 
                        int b = y - r * sin(theta * CV_PI / 180); 
                        if (a >= 0 && a < edges.cols && b >= 0 && b < edges.rows) { 
                            votes.at<float>(b, a, r - minR)++; 
                        }
                    }
                }
            }
        }
    }
    for (int r = minR; r <= maxR; r++) { 
        for (int y = 0; y < edges.rows; y++) { 
            for (int x = 0; x < edges.cols; x++) { 
                if (votes.at<float>(y, x, r - minR) > atoi(argv[2])) { 
                    circle(image, Point(x, y), 3, Scalar(0), 2, 8, 0); 
                    circle(image, Point(x, y), r, Scalar(0), 2, 8, 0); 
                }
            }
        }
    }
    imshow("cerchi", image);
    waitKey(0);
    return 0;
}