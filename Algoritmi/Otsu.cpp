#include <opencv2/opencv.hpp>
#include <vector>

currVar = pow(mediaGlobale / prob - currMedGlob, 2) / (prob * (1 - prob));

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
    
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

    // calcolo l'istogramma dell'immagine analizzando pixel per pixel dell'immagine di input
    vector<float> istogramma(256, 0.0f);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            istogramma[image.at<uchar>(i, j)]++;
        }
    }

    // normalizzo l'istogramma dividendo ogni valore per la grandezza dell'immagine (righe per colonne)
    for (int i = 0; i < istogramma.size(); i++) {
        istogramma[i] /= (image.rows * image.cols);
    }

    float mediaGlobale = 0.0f, 
          prob = 0.0f, 
          currMediaGlobale = 0.0f, 
          currVar = 0.0f, 
          maxVar = 0.0f; 
    int sogliaOttimale; 

    // calcolo la media globale
    for (int i = 0; i < istogramma.size(); i++) {
        mediaGlobale += (i + 1) * istogramma[i];
    }

    // calcolo la varianza tra le classi per ogni possibile valore
    for (int i = 0; i < istogramma.size(); i++) {
        prob += istogramma[i]; 
        currMediaGlobale += (i + 1) * istogramma[i];
        currVar = pow(mediaGlobale * prob - currMediaGlobale, 2) / (prob * (1 - prob)); 
        if (currVar > maxVar) {
            maxVar = currVar;
            sogliaOttimale = i;
        }
    }

    // applico la soglia ottimale calcolata da Otsu e stampo l'output
    Mat output;
    threshold(image, output, sogliaOttimale, 255, THRESH_BINARY);
    imshow("Immagine con soglia ottimale", output);
    waitKey(0);

    return 0;
}