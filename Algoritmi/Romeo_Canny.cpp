#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat nonMaximaSuppression(Mat& magnitude, Mat&orientation) { // sopprime i picchi non massimi
    Mat nmsImage = magnitude.clone(); // immagine risultante dalla non maxima suppression
    float angle, // angolo del gradiente nel pixel corrente
          next,  // magnitudo del gradiente del pixel adiacente alla direzione del gradiente
          prev,  // magnitudo del gradiente del pixel adiacente opposta alla direzione del gradiente
          curr;  // magnitudo del gradiente del pixel corrente
    for (int i = 1; i < magnitude.rows - 1; i++) { // analizzo tutti pixel sulle righe eccetto sui bordi
        for (int j = 1; j < magnitude.cols - 1; j++) { // analizzo tutti pixel sulle righe eccetto sui bordi
            angle = orientation.at<float>(i, j); // ottengo l'angolo dell'orientamento della magnitudo del gradiente
            if (angle > 180) { // normalizzo l'angolo per essere sempre compreso tra -180 e 180
                angle -= 360;
            }
            if ((angle <= 22.5 && angle > -22.5) || (angle > 157.5 || angle <= -157.5)) { // edge orizzontale
                next = magnitude.at<float>(i, j + 1); // ottengo il pixel di destra
                prev = magnitude.at<float>(i, j - 1); // e il pixel di sinistra
            } else if ((angle <= 67.5 && angle > 22.5) || (angle > -157.5 && angle <= -112.5)) { // edge -45 gradi
                next = magnitude.at<float>(i + 1, j - 1); // ottengo il pixel in basso a sinistra
                prev = magnitude.at<float>(i - 1, j + 1); // e il pixel in alto a destra
            } else if((angle <= 157.5 && angle > 112.5) || (angle > -67.5 && angle <= -22.5)) { // edge 45 gradi
                next = magnitude.at<float>(i - 1, j - 1); // ottengo il pixel in alto a sinistra
                prev = magnitude.at<float>(i + 1, j + 1); // e il pixel in basso a destra
            } else if ((angle <= 112.5 && angle > 67.5) || (angle > -112.5 && angle <= -67.5)) { // edge verticale
                next = magnitude.at<float>(i + 1, j); // ottengo il pixel di sotto
                prev = magnitude.at<float>(i - 1, j); // e il pixel di sopra
            }
            curr = magnitude.at<float>(i, j); // ottengo il valore della magnitudo del gradiente nel pixel corrente
            if (curr < prev || curr < next) { // se il valore è minore di quello di uno dei pixel adiacenti
                nmsImage.at<float>(i, j) = 0; // sopprimo
            }
        }
    }
    return nmsImage; // restituisco l'immagine con massimi soppressi
}

Mat thresholdingWithHysteresis(Mat& nmsImage, int lowThreshold, int highThreshold) { // applica il thresholding con due soglie
    Mat edges = Mat::zeros(nmsImage.size(), nmsImage.type()); // matrice contenente gli edge
    for (int i = 1; i < nmsImage.rows - 1; i++) { // scorro le righe della nmsImage
        for (int j = 1; j < nmsImage.cols - 1; j++) { // scorro le colonne della nmsImage
            if (nmsImage.at<float>(i, j) > highThreshold) { // se la magnitudo del gradiente del pixel analizzato supera la soglia alta
                edges.at<float>(i, j) = 255; // lo imposto come pixel forte (pixel di edge)
            } else if (nmsImage.at<float>(i, j) > lowThreshold) { // se la magnitudo del gradiente del pixel analizzato si trova tra le due soglie
                for (int u = -1; u <= 1; u++) { // cerco i pixel vicini a quello analizzato (u = -1 = sinistra e u = 1 = destra)
                    for (int v = -1; v <= 1; v++) { // cerco i pixel vicini a quello analizzato (v = -1 = in alto e v = 1 = in basso)
                        if (nmsImage.at<float>(i + u, j + v) > highThreshold) { // se la magnitudo del gradiente del pixel supera la soglia alta
                            edges.at<float>(i, j) = 255; // imposto il pixel come forte (pixel di edge)
                        }
                    }
                }
            }
        }
    }
    return edges; // restituisco l'immagine contenente gli edges
}

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
    Mat Dx, Dy;
    Sobel(image, Dx, CV_32F, 1, 0, 3);
    Sobel(image, Dy, CV_32F, 0, 1, 3);

    // calcolo l'orientazione del gradiente
    Mat orientation;
    phase(Dx, Dy, orientation, true);

    // calcolo la magnitudo del gradiente
    Mat magnitude;
    pow(Dx, 2, Dx);
    pow(Dy, 2, Dy);
    sqrt(Dx + Dy, magnitude);
    normalize(magnitude, magnitude, 0, 255, NORM_MINMAX, CV_32F);

    // applico la non maxima suppression (sopprimo i picchi con valore non massimo)
    Mat nmsImage = nonMaximaSuppression(magnitude, orientation);

    // applico il thresholding con isteresi
    Mat edges = thresholdingWithHysteresis(nmsImage, atoi(argv[2]), atoi(argv[3]));
    imshow("Bordi dell'immagine", edges);
    waitKey(0);

    return 0;
}