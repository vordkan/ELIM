#include <opencv2/opencv.hpp>
using namespace cv;

/*
	1. Inizializzare l'accumulatore H
	2. Applicare l'algoritmo di Canny per individuare i punti di edge
	3. Per ogni punto (𝑥, 𝑦) di edge (risultato di Canny)
	4. Per ogni angolo 𝜃 = 0: 180 calcolare 𝜌 = 𝑥 cos 𝜃 + 𝑦 sin 𝜃
	5. Incrementare H(𝜌, 𝜃)=H(𝜌, 𝜃)+1
	6. Le celle H(𝜌, 𝜃) con un valore maggiore di una soglia th corrispondono alle rette nell'immagine
*/

void polarToCartesian(double, int, Point&, Point&);
void houghLines(Mat, Mat&, int, int, int);

int main(int argc, char** argv) {
	Mat img = imread(argv[1], IMREAD_GRAYSCALE);
	if(img.empty()) return -1;
	Mat dst;
	int cannyLTH = 40, cannyHTH = 120, houghTH = 100;
	houghLines(img, dst, cannyLTH, cannyHTH, houghTH);
	imshow("Original", img);
	imshow("HoughLines", dst);
	waitKey(0);
	return 0;
}

/*
	questa funzione prende una coppia di coordinate polari rappresentate da rho (la distanza dal centro) 
	e theta (l'angolo rispetto all'asse x) e le converte in coordinate cartesiane
*/
void polarToCartesian(double rho, int theta, Point& p1, Point& p2) {
	int alpha = 1000;
	int x0 = cvRound(rho * cos(theta)); // coordinata x del centro del cerchio
	int y0 = cvRound(rho * sin(theta)); // coordinata y del centro del cerchio
	
	p1.x = cvRound(x0 + alpha * (-sin(theta))); // calcola la coordinata x del primo punto (p1) sulla circonferenza del cerchio
	p1.y = cvRound(y0 + alpha * (cos(theta))); // calcola la coordinata y del primo punto (p1) sulla circonferenza del cerchio
	p2.x = cvRound(x0 - alpha * (-sin(theta))); // calcola la coordinata x del secondo punto (p2) sulla circonferenza del cerchio
	p2.y = cvRound(y0 - alpha * (cos(theta))); // calcola la coordinata y del secondo punto (p2) sulla circonferenza del cerchio
}

void houghLines(Mat src, Mat& dst, int cannyLTH, int cannyHTH, int houghTH) {
	// 1. Creare la matrice in cui saranno contenuti i voti
	int maxDist = hypot(src.rows, src.cols);
	Mat votes = Mat::zeros(maxDist * 2, 180, CV_8U);
	// 2. Eseguire algoritmo di Canny sull'immagine per rilevare i bordi
	Mat gaus, cannyimg;
	GaussianBlur(src, gaus, Size(3,3), 0);
	Canny(gaus, cannyimg, cannyLTH, cannyHTH);
	// 3. Essenza dell'algoritmo
	double rho;
	int theta;
	for(int i = 0; i < cannyimg.rows; i++) { // itero le righe di canny
		for(int j = 0; j < cannyimg.cols; j++) { // itero le colonne di canny
			if(cannyimg.at<uchar>(i,j) == 255) { // se sono sul bordo
				// Per ogni angolo 𝜃 = 0: 180
				for(theta = 0; theta <= 180; theta++) {
					// calcolare 𝜌 = 𝑥 cos 𝜃 + 𝑦 sin 𝜃
					rho = round(j * cos(theta-90) + i * sin(theta-90)) + maxDist; // la sottrazione di 90 gradi viene effettuata 
																			      // per ruotare gli angoli in modo che siano 
																			      // allineati rispetto all'asse y dell'immagine invece che all'asse x.
					// Incrementare H(𝜌, 𝜃) = H(𝜌, 𝜃) + 1
					votes.at<uchar>(rho,theta)++;
				}
			}
		}
	}
	// 4. Disegno delle linee
	src.copyTo(dst);
	Point p1, p2;
	for(int i = 0; i < votes.rows; i++) {
		for(int j = 0; j < votes.cols; j++) {
			if(votes.at<uchar>(i,j) >= houghTH) {
				rho = i - maxDist; // la sottrazione di maxDist da i viene effettuata per correggere 
								   // il valore di rho in modo che rappresenti correttamente la 
								   // distanza dall'origine all'asse della retta individuata nell'immagine
				theta = j - 90;
				polarToCartesian(rho, theta, p1, p2);
				line(dst, p1, p2, Scalar(0));
			}
		}
	}
}
