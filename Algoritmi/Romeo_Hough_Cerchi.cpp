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

    int minR = 50,  // ampiezza minima del raggio delle circonferenze da trovare
        maxR = 65,  // ampiezza massima del raggio delle circonferenze da trovare
        size[] = {edges.rows, edges.cols, maxR - minR + 1}; // dimensione della matrice dei voti
    Mat votes = Mat(3, size, CV_32F); // matrice dei voti tridimensionale
    for (int y = 0; y < edges.rows; y++) { // scorro le righe dell'immagine degli edge
        for (int x = 0; x < edges.cols; x++) { // scorro le colonne dell'immagine degli edge
            if (edges.at<uchar>(y, x) == 255) { // se il pixel analizzato è un edge
                for (int r = minR; r <= maxR; r++) { // scorro tutti i possibili valori dei raggi
                    for (int theta = 0; theta < 360; theta++) { // scorro tutti i possibili valori di theta
                        int a = x - r * cos(theta * CV_PI / 180); // calcolo l'ascissa del centro del cerchio (colonna)
                        int b = y - r * sin(theta * CV_PI / 180); // calcolo l'ordinata del centro del cerchio (riga)
                        if (a >= 0 && a < edges.cols && b >= 0 && b < edges.rows) { // se il centro ha coordinate valide
                            votes.at<float>(b, a, r - minR)++; // incremento il voto nella cella corrispondente
                        }
                    }
                }
            }
        }
    }
    for (int r = minR; r <= maxR; r++) { // scorro tutti i possibili valori dei raggi
        for (int y = 0; y < edges.rows; y++) { // scorro le righe dell'immagine degli edge
            for (int x = 0; x < edges.cols; x++) { // scorro le colonne dell'immagine degli edge
                if (votes.at<float>(y, x, r - minR) > atoi(argv[2])) { // se il valore nella cella dei voti supera la soglia ho un corner
                    circle(image, Point(x, y), 3, Scalar(0), 2, 8, 0); // cerchio il centro della circonferenza
                    circle(image, Point(x, y), r, Scalar(0), 2, 8, 0); // cerchio la circonferenza
                }
            }
        }
    }
    imshow("cerchi", image);
    waitKey(0);
    return 0;
}