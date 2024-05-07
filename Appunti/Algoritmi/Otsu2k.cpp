#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace cv;

/*
	1. Calcolo dell'istogramma
	2. Normalizzazione dell'istogramma
	3. Calcolo della media globale
	4. Iterazione attraverso le soglie possibili
	5. Scegliere le soglie ottimali
	6. Binarizzazione
*/

void otsu2k(Mat src, int&, int&);

int main(int argc, char **argv) {
	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	if(src.empty()) return -1;
	int th1, th2;
	otsu2k(src, th1, th2);
	Mat dst = src.clone();
	// 6. Binarizzazione: Utilizza le soglie ottimali trovate per 
	// binarizzare l'immagine. I pixel con intensità sotto la soglia 
	// k diventano neri, quelli tra le soglie k e l diventano grigi
	// e quelli sopra la soglia l diventano bianchi.
	for(int i = 0; i < src.rows; i++) {
		for(int j = 0; j < src.cols; j++) {
			if(dst.at<uchar>(i,j) > th2)
				dst.at<uchar>(i,j) = 255;
			else if(dst.at<uchar>(i,j) > th1)
				dst.at<uchar>(i,j) = 127;
			else
				dst.at<uchar>(i,j) = 0;
		}
	}
	imshow("Originale", src);
	imshow("Otsu2k", dst);
	waitKey(0);
	return 0;
}

void otsu2k(Mat src, int &th1, int &th2) {
	int MxN = src.rows * src.cols; // dimensione dell'immagine
	float sigma[256][256] = {0.0}; // matrice della varianza interclasse
	float sigma_max = 0.0;
	int tmp1, tmp2;
	float mg = 0.0; // media globale
	// 1. Calcolo dell'istogramma: Calcola l'istogramma delle frequenze dei livelli di grigio presenti
	// nell'immagine. Questo rappresenta la distribuzione dei valori di intensità dei pixel.
	vector<float> hist(256, 0.0);
	for(int i = 0; i < 256; i++)
		for(int j = 0; j < 256; j++)
			hist.at(src.at<uchar>(i,j))++;
	for(int i = 0; i < 256; i++) {
		// 2. Normalizzazione dell'istogramma: Normalizza l'istogramma dividendo 
		// ogni valore di frequenza per il numero totale di pixel nell'immagine. 
		// Questo fornisce le probabilità di ogni livello di grigio.
		hist.at(i) /= MxN;
		// 3. Calcolo della media globale: Calcola la media globale dei livelli di grigio, 
		// che rappresenta il valore medio dell'intensità dei pixel nell'immagine.
		mg += hist.at(i) * i;
	}
	// 4. Iterazione attraverso le soglie possibili: Itera attraverso 
	// tutte le possibili combinazioni di due soglie k e l (dove k < l) e calcola le seguenti quantità:

		// a. Le probabilità cumulative p1, p2 e p3 dei valori di intensità dei 
		// pixel rispettivamente sotto k, tra k e l, e sopra l.

		// b. Le medie pesate med_a, med_b e med_c dei valori di intensità dei 
		// pixel rispettivamente sotto k, tra k e l, e sopra l.

		// c. Calcola la varianza interclasse sigma[k][l] utilizzando le 
		// probabilità e le medie calcolate. La varianza interclasse è 
		// una misura della separazione tra le tre classi di pixel.
	for(int k = 0; k < 256; k++) {
		for(int l = k + 1; l < 256; l++) {
			// calcola le probabilità p0, p1, p2
			float p1 = 0.0, p2 = 0.0, p3 = 0.0;
			for(int i = 0; i <= k; i++)
				p1 += hist.at(i);
			for(int i = k + 1; i <= l; i++)
				p2 += hist.at(i);
			for(int i = l + 1; i < 256; i++)
				p3 += hist.at(i);
			// calcola le medie med_a, med_b, med_c
			float med_a = 0.0, med_b = 0.0, med_c = 0.0;
			for(int i = 0; i <= k; i++)
				med_a += hist.at(i) * i / p1;
			for(int i = k + 1; i <= l; i++)
				med_b += hist.at(i) * i / p2;
			for(int i = l + 1; i < 256; i++)
				med_c += hist.at(i) * i / p3;
			// 5. Scegliere le soglie ottimali: Trova la combinazione di 
			// soglie k e l che massimizza la varianza interclasse. 
			// Queste soglie sono considerate ottimali per la binarizzazione.
			sigma[k][l] = p1 * (med_a - mg) * (med_a - mg) + p2 * (med_b - mg) * (med_b - mg) + p3 * (med_c - mg) * (med_c - mg);
			// trova la combinazione k, l con la massima varianza interclasse
			if(sigma[k][l] > sigma_max) {
				sigma_max = sigma[k][l];
				tmp1 = k;
				tmp2 = l;
			}
		}
		// soglie trovate
		th1 = tmp1;
		th2 = tmp2;
	}
}
