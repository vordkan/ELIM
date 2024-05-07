#include <opencv2/opencv.hpp>
#include <stack>
#include <iostream>
using namespace std;
using namespace cv;

// Definizione degli spostamenti per i punti nel vicinato 8-connesso
Point PointShift2D[8] = {
    Point(0,0),
    Point(0,1),
    Point(1,0),
    Point(1,1),
    Point(0,-1),
    Point(-1,0),
    Point(-1,-1),
    Point(-1,1)
};

const double min_region_area_factor = 0.01;
const int max_region_num = 100;

void grow(Mat, Mat&, Mat&, Point, int);

int main(int argc, char **argv) {
    Mat src = imread(argv[1], -1);
    if(src.empty()) return -1;
    // Ridimensiona l'immagine se è troppo grande
    if(src.rows > 500 || src.cols > 500)
        resize(src, src, Size(0,0), 0.5, 0.5);
    uchar padding = 1;  // Variabile per il padding
    int min_region_area = int(min_region_area_factor * src.rows * src.cols);  // Calcola l'area minima della regione
    Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC1);  // Crea un'immagine di destinazione in scala di grigi
    Mat mask = Mat::zeros(src.rows, src.cols, CV_8UC1);  // Crea una maschera
    // Ciclo sui pixel dell'immagine di input
    for(int i = 0; i < src.cols; i++) {
        for(int j = 0; j < src.rows; j++) {
            // Se il pixel corrente nella dst è nero
            if(dst.at<uchar>(Point(i,j)) == 0) {
                // Esegui la crescita della regione
                grow(src, dst, mask, Point(i,j), 200);
                // Calcola l'area della maschera
                int mask_area = (int) sum(mask).val[0];
                // Se l'area della maschera supera la soglia minima
                if(mask_area > min_region_area) {
                    // Aggiungi la maschera alla dst con il padding
                    dst = dst + mask * padding;
                    // Visualizza la maschera
                    imshow("Mask", mask * 255);
                    waitKey(0);
                    // Se il padding supera il numero massimo di regioni, esci
                    if(++padding > max_region_num) {
                        cout << "Out of max_region_num" << endl;
                        exit(-2);
                    } else {
                        // Altrimenti, aggiungi la maschera alla dst con padding maggiore
                        dst = dst + mask * 255;
                    }
                }
                mask = mask - mask;  // Reset della maschera
            }
        }
    }
    return 0;
}

// Definizione della funzione "grow"
void grow(Mat src, Mat& dst, Mat& mask, Point seed, int threshold) {
    stack<Point> stack_point;  // Crea una pila di punti
    stack_point.push(seed);     // Aggiungi il punto di partenza alla pila
    // Ciclo finché la pila non è vuota
    while(!stack_point.empty()) {
        Point center = stack_point.top();  // Prendi il punto in cima alla pila
        stack_point.pop();                 // Rimuovi il punto dalla pila
        mask.at<uchar>(center) = 1;        // Imposta il valore della maschera a 1
        // Ciclo sui punti del vicinato 8-connesso
        for(int i = 0; i < 8; i++) {
            Point estimating_point = center + PointShift2D[i];  // Calcola il punto stimato
            // Verifica se il punto stimato è all'interno dei limiti dell'immagine
            if(estimating_point.x < 0 || estimating_point.y < 0 || estimating_point.x > src.cols - 1 || estimating_point.y > src.rows - 1) {
                continue;  // Ignora il punto se è fuori dai limiti
            } else {
                // Calcola la differenza dei valori di colore
                int delta = int(pow(src.at<Vec3b>(center)[0] - src.at<Vec3b>(estimating_point)[0], 2)
                        + pow(src.at<Vec3b>(center)[1] - src.at<Vec3b>(estimating_point)[1], 2)
                        + pow(src.at<Vec3b>(center)[2] - src.at<Vec3b>(estimating_point)[2], 2));
                
                // Verifica se il delta è inferiore alla soglia e il punto non è ancora visitato
                if(delta < threshold && mask.at<uchar>(estimating_point) == 0 && dst.at<uchar>(estimating_point) == 0) {
                    mask.at<uchar>(estimating_point) = 1;  // Imposta il valore della maschera a 1
                    stack_point.push(estimating_point);    // Aggiungi il punto stimato alla pila
                }
            }
        }
    }
}

