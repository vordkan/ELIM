#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

/*
	1. Inizializzare i centri di ogni cluster
	2. Assegnare ogni pixel al cluster con il centro più vicino
		Per ogni pixel p_j calcolare la distanza dai k centri ci ed assegnare p_j al
		cluster con il centro c_i più vicino
	3. Aggiornare i centri
		Calcolare la media dei pixel di ogni cluster
	4. Ripetere i punti 2 e 3 finchè il centro (media) di ogni cluster non viene più
	   modificato (ovvero i gruppi non vengono modificati)
*/

void createClusterInfo(const Mat src, int clusters_number, vector<Scalar> &clusters_centers, vector<vector<Point>> &ptInClusters);
double computeColorDistance(Scalar pixel, Scalar clusterPixel);
void findAssociatedCluster(const Mat src, int clusters_number, vector<Scalar> clusters_centers, vector<vector<Point>> &ptInClusters);
double adjustClusterCenter(const Mat src, int clusters_number, vector<Scalar> &clusters_centers, vector<vector<Point>> ptInClusters, double &oldCenter, double newCenter);
void applyFinalClusterToImage(const Mat src, Mat &dst, int clusters_number, vector<Scalar> clusters_centers, vector<vector<Point>> ptInClusters);

int main(int argc, char * argv[]) {
	srand(time(NULL));
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " image_path #k" << endl; // Stampa un messaggio di errore se il numero di argomenti è diverso da 3
        exit(100); // Termina il programma con un codice di errore 100
    }

    Mat src = imread(argv[1]); // Carica un'immagine a colori dal percorso specificato
    int clusters_number = atoi(argv[2]); // Converte l'argomento 2 da stringa a intero
    vector<Scalar> clusters_centers; // Vettore per i centroidi dei cluster
    vector<vector<Point>> ptInClusters; // Vettore per i punti nei cluster
    double threshold = 0.1; // Soglia di convergenza per il calcolo dei centroidi
    double oldCenter = INFINITY; // Variabile per memorizzare la posizione precedente dei centroidi
    double newCenter = 0.0; // Variabile per memorizzare la nuova posizione dei centroidi
    double diff_change = oldCenter - newCenter; // Differenza tra le posizioni precedenti e nuove dei centroidi

    createClusterInfo(src, clusters_number, clusters_centers, ptInClusters); // Crea le informazioni iniziali sui cluster

    while (threshold < diff_change) { // Continua finché la differenza tra i centroidi precedenti e nuovi è maggiore della soglia
        newCenter = 0; // Inizializza la variabile per le nuove posizioni dei centroidi

        for (int k = 0; k < clusters_number; ++k) { // Inizializza i punti nei cluster
            ptInClusters[k].clear();
        }

        findAssociatedCluster(src, clusters_number, clusters_centers, ptInClusters); // Trova il cluster associato a ciascun punto
        diff_change = adjustClusterCenter(src, clusters_number, clusters_centers, ptInClusters, oldCenter, newCenter); // Calcola i nuovi centroidi
    }

    Mat dst; // Matrice per l'immagine di output
    applyFinalClusterToImage(src, dst, clusters_number, clusters_centers, ptInClusters); // Applica il clustering finale all'immagine
    imshow("Input image", src); // Mostra l'immagine di input
    imshow("Output image", dst); // Mostra l'immagine di output
    waitKey(0); // Attendi l'input dell'utente

    return 0;
}

/*
	questa funzione genera casualmente le coordinate dei 
	centroidi iniziali per ciascun cluster nell'immagine 
	src e memorizza anche un colore iniziale per ciascun centroide.
*/
void createClusterInfo(const Mat src, int clusters_number, vector<Scalar> &clusters_centers, vector<vector<Point>> &ptInClusters) {
    for (int k = 0; k < clusters_number; ++k) { // Per ogni cluster
        Point centerKPoint;
        centerKPoint.x = rand() % src.cols; // genera coordinata x del centroide
        centerKPoint.y = rand() % src.rows; // genera coordinata y del centroide
        Scalar centerPixel = src.at<Vec3b>(centerKPoint); // Estrae il colore del pixel
        clusters_centers.push_back(centerPixel); // Aggiunge il centroide al vettore dei centroidi

        vector<Point> ptInClusterK; // Vettore per i punti nel cluster corrente
        ptInClusters.push_back(ptInClusterK); // Aggiunge il vettore dei punti nel cluster ai cluster
    }
}

double computeColorDistance(Scalar pixel, Scalar clusterPixel) {
    double blueDistance = pixel[0] - clusterPixel[0]; // Calcola la differenza nei canali blu
    double greenDistance = pixel[1] - clusterPixel[1]; // Calcola la differenza nei canali verde
    double redDistance = pixel[2] - clusterPixel[2]; // Calcola la differenza nei canali rosso

    double distance = sqrt(pow(blueDistance, 2) + pow(greenDistance, 2) + pow(redDistance, 2)); // Calcola la distanza euclidea
    return distance; // Restituisce la distanza calcolata
}


/*
	questa funzione attraversa ogni pixel dell'immagine 
	e determina il cluster più vicino per ciascun pixel 
	in base alla distanza dei colori. I punti vengono 
	quindi assegnati ai cluster corrispondenti in ptInClusters
*/
void findAssociatedCluster(const Mat src, int clusters_number, vector<Scalar> clusters_centers, vector<vector<Point>> &ptInClusters) {
    for (int r = 0; r < src.rows; ++r) { // Per ogni riga dell'immagine
        for (int c = 0; c < src.cols; ++c) { // Per ogni colonna dell'immagine
            double min_distance = INFINITY; // Inizializza la distanza minima come infinito
            int closestClusterIndex = 0; // Inizializza l'indice del cluster più vicino come zero
            Scalar pixel = src.at<Vec3b>(r, c); // Estrae il colore del pixel

            for (int k = 0; k < clusters_number; ++k) { // Per ogni cluster
                Scalar pixelCluster = clusters_centers[k]; // Estrae il colore del centroide del cluster
                double distance = computeColorDistance(pixel, pixelCluster); // Calcola la distanza tra il pixel e il centroide

                if (distance < min_distance) { // Se la distanza è minore della distanza minima
                    min_distance = distance; // Aggiorna la distanza minima
                    closestClusterIndex = k; // Aggiorna l'indice del cluster più vicino
                }
            }

            ptInClusters[closestClusterIndex].push_back(Point(c, r)); // Aggiunge il punto al cluster più vicino
        }
    }
}

/*
	questa funzione calcola i nuovi centroidi per 
	ciascun cluster basandosi sulla media dei punti 
	nel cluster, quindi calcola la differenza tra i 
	centroidi prima e dopo l'aggiornamento
*/
double adjustClusterCenter(const Mat src, int clusters_number, vector<Scalar> &clusters_centers, vector<vector<Point>> ptInClusters, double &oldCenter, double newCenter) {
    double diff_change;

    for (int k = 0; k < clusters_number; ++k) { // Per ogni cluster
        vector<Point> ptInCluster = ptInClusters[k]; // Ottieni i punti nel cluster corrente
        double newBlue = 0.0; // Variabile per la nuova componente blu
        double newGreen = 0.0; // Variabile per la nuova componente verde
        double newRed = 0.0; // Variabile per la nuova componente rossa

        for (int i = 0; i < ptInCluster.size(); ++i) { // Per ogni punto nel cluster
            Scalar pixel = src.at<Vec3b>(ptInCluster[i]); // Estrae il colore del pixel
            newBlue += pixel[0]; // Aggiorna la somma delle componenti blu
            newGreen += pixel[1]; // Aggiorna la somma delle componenti verde
            newRed += pixel[2]; // Aggiorna la somma delle componenti rosse
        }

        newBlue /= ptInCluster.size(); // Calcola la media delle componenti blu
        newGreen /= ptInCluster.size(); // Calcola la media delle componenti verde
        newRed /= ptInCluster.size(); // Calcola la media delle componenti rosse

        Scalar newPixel(newBlue, newGreen, newRed); // Crea il nuovo centroide
        newCenter += computeColorDistance(newPixel, clusters_centers[k]); // Aggiorna la differenza tra centroidi
        clusters_centers[k] = newPixel; // Aggiorna il centroide del cluster
    }

    newCenter /= clusters_number; // Calcola la media delle differenze tra centroidi
    diff_change = abs(oldCenter - newCenter); // Calcola la differenza tra i centroidi precedenti e nuovi
    cout << "diff_change is " << diff_change << endl; // Stampa la differenza
    oldCenter = newCenter; // Aggiorna il valore precedente del centroide

    return diff_change; // Restituisce la differenza tra centroidi
}

void applyFinalClusterToImage(const Mat src, Mat &dst, int clusters_number, vector<Scalar> clusters_centers, vector<vector<Point>> ptInClusters) {
    src.copyTo(dst); // Copia l'immagine di input nell'immagine di output

    for (int k = 0; k < clusters_number; ++k) { // Per ogni cluster
        vector<Point> ptInCluster = ptInClusters[k]; // Ottieni i punti nel cluster corrente
        Scalar pixelColor = clusters_centers[k]; // Ottieni il colore del cluster

        for (int i = 0; i < ptInCluster.size(); ++i) { // Per ogni punto nel cluster
            dst.at<Vec3b>(ptInCluster[i])[0] = pixelColor[0]; // Imposta il canale blu
            dst.at<Vec3b>(ptInCluster[i])[1] = pixelColor[1]; // Imposta il canale verde
            dst.at<Vec3b>(ptInCluster[i])[2] = pixelColor[2]; // Imposta il canale rosso
        }
    }
}
