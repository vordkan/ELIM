#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

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
    Canny(image, edges, 45, 75, 3);
    imshow("Bordi dell'immagine", edges);
    waitKey(0);

    // inizializzo e riempio la matrice dei voti
    int diagonal = hypot(image.rows, image.cols); 
    Mat votes = Mat::zeros(diagonal * 2, 180, CV_32F); 
    double rho, theta;
    for (int y = 0; y < edges.rows; y++) { 
        for (int x = 0; x < edges.cols; x++) { 
            if (edges.at<uchar>(y, x) == 255) {
                for (theta = 0; theta < 180; theta++) { 
                    rho = diagonal + x * cos((theta - 90) * CV_PI / 180) + y * sin((theta - 90) * CV_PI / 180);
                    votes.at<float>(rho, theta)++; 
                }
            }
        }
    }
    for (int r = 0; r < votes.rows; r++) { 
        for (int t = 0; t < votes.cols; t++) { 
            if (votes.at<float>(r, t) > atoi(argv[2])) { 
                theta = (t - 90) * CV_PI / 180; 
                int x = (r - diagonal) * cos(theta); 
                int y = (r - diagonal) * sin(theta); 
                double sin_t = sin(theta); 
                double cos_t = cos(theta); 
                Point pt1(cvRound(x + diagonal * (-sin_t)), cvRound(y + diagonal * cos_t));
                Point pt2(cvRound(x - diagonal * (-sin_t)), cvRound(y - diagonal * cos_t)); 
                line(image, pt1, pt2, Scalar(0), 2, 0); 
            }
        }
    }
    imshow("rette", image);
    waitKey(0);
    return 0;
}