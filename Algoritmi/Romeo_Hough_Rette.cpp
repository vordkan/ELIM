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
    int diagonal = hypot(image.rows, image.cols); // numero di pixel sulla diagonale dell'immagine
    Mat votes = Mat::zeros(diagonal * 2, 180, CV_32F); // matrice dei voti
    double rho, theta;
    for (int y = 0; y < edges.rows; y++) { // scorro le righe dell'output di Canny
        for (int x = 0; x < edges.cols; x++) { // scorro le colonne dell'output di Canny
            if (edges.at<uchar>(y, x) == 255) { // se il pixel è un pixel di edge
                for (theta = 0; theta < 180; theta++) { // per ogni valore di theta da 0 a 180
                    rho = diagonal + x * cos((theta - 90) * CV_PI / 180) + y * sin((theta - 90) * CV_PI / 180); // calcolo rho (CV_PI è il pigreco)
                    votes.at<float>(rho, theta)++; // incremento il valore corrispondente nella matrice dei voti
                }
            }
        }
    }
    for (int r = 0; r < votes.rows; r++) { // scorro le righe della matrice dei voti
        for (int t = 0; t < votes.cols; t++) { // scorro le colonne della matrice dei voti
            if (votes.at<float>(r, t) > atoi(argv[2])) { // se il valore della cella della matrice dei voti supera la soglia
                theta = (t - 90) * CV_PI / 180; // calcolo theta
                int x = (r - diagonal) * cos(theta); // calcolo l'ascissa della retta
                int y = (r - diagonal) * sin(theta); // calcolo l'ordinata della retta
                double sin_t = sin(theta); // calcolo il seno di theta
                double cos_t = cos(theta); // calcolo il coseno di theta
                Point pt1(cvRound(x + diagonal * (-sin_t)), cvRound(y + diagonal * cos_t)); // traccio un punto sulla retta
                Point pt2(cvRound(x - diagonal * (-sin_t)), cvRound(y - diagonal * cos_t)); // traccio un punto sulla retta
                line(image, pt1, pt2, Scalar(0), 2, 0); // traccio una linea che passa per i 2 punti per evidenziare la retta nell'immagine
            }
        }
    }
    imshow("rette", image);
    waitKey(0);
    return 0;
}