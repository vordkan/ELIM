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
    Mat diagonal1, // prima diagonale (prodotto tra componente in alto a sinistra e quella in basso a destra della matrice)
        diagonal2; // seconda diagonale (prodotto tra la componente in alto a destra e quella in basso a sinistra della matrice)
    multiply(Dx, Dy, diagonal1); // calcolo la prima diagonale
    multiply(Dxy, Dxy, diagonal2); // calcolo la seconda diagonale
    Mat trace = Dx + Dy; // calcolo la traccia (somma delle diagonali)
    pow(trace, 2, trace); // elevo la traccia al quadrato
    Mat R = diagonal1 - diagonal2 - 0.04 * trace; // calcolo R (determinante - k * traccia al quadrato con determinante uguale alla differenza tra le diagonali)
    normalize(R, R, 0, 255, NORM_MINMAX, CV_32F); // normalizzo R tra 0 e 255

    // mostro gli angoli nell'immagine di output
    Mat corners = Mat::zeros(image.size(), image.type()); // matrice contenente i corner
    for (int i = 0; i < image.rows; i++) { // scorro le righe dell'immagine di input
        for (int j = 0; j < image.cols; j++) { // scorro le colonne dell'immagine di output
            if (R.at<float>(i, j) > atoi(argv[2])) { // se il valore del pixel supera la soglia, è un corner
                circle(corners, Point(j, i), 5, Scalar(255), 2, 8, 0); // mostro un cerchio per evidenziarlo
            }
        }
    }
    imshow("Corner dell'immagine", corners);
    waitKey(0);
    return 0;
}