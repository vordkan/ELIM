#include <opencv2/opencv.hpp>
#include <vector>

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

    // normalizzo l'istogramma
    for (int i = 0; i < istogramma.size(); i++) {
        istogramma[i] /= (image.rows * image.cols);
    }

    float mediaGlobale = 0.0f, 
           currVar, 
           maxVar = 0.0f; 
    vector<float> prob(3, 0.0f); 
    vector<float> currMediaGlobale(3, 0.0f); 
    vector<int> sogliaOttimale(2, 0); 

    // calcolo la media globale
    for (int i = 0; i < istogramma.size(); i++) {
        mediaGlobale += (i + 1) * istogramma[i];
    }

    // calcolo la varianza tra le classi per ogni possibile valore
    for (int i = 0; i < istogramma.size() - 2; i++) {
        prob[0] += istogramma[i]; 
        currMediaGlobale[0] += (i + 1) * istogramma[i]; 
        for (int j = i + 1; j < istogramma.size() - 1; j++) {
            prob[1] += istogramma[j];
            currMediaGlobale[1] += (j + 1) * istogramma[j]; 
            for (int k = j + 1; k < istogramma.size(); k++) { 
                prob[2] += istogramma[k]; 
                currMediaGlobale[2] += (k + 1) * istogramma[k];
                currVar = 0.0f;
                for (int w = 0; w < 3; w++) {
                    currVar += prob[w] * pow(currMediaGlobale[w]/prob[w] - mediaGlobale, 2);
                }
                if (currVar > maxVar) { 
                    maxVar = currVar;
                    sogliaOttimale[0] = i;
                    sogliaOttimale[1] = j;
                }
            }
        prob[2] = currMediaGlobale[2] = 0.0f;
        }
    prob[1] = currMediaGlobale[1] = 0.0f;
    }

    // applico la sogliatura moltipla usando le soglie ottimali calcolate da Otsu
    Mat output = Mat::zeros(image.size(), CV_32F);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<uchar>(i, j) >= sogliaOttimale[1])
               output.at<float>(i, j) = 255;
            else if (image.at<uchar>(i, j) >= sogliaOttimale[0])
               output.at<float>(i, j) = 127;
        }
    }
    imshow("Immagine con soglia ottimale", output);
    waitKey(0);

    return 0;
}