#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

void createCluster(Mat image, int k, vector<Scalar>& centers, vector<vector<Point>>& clusters) { // crea i cluster
    for (int i = 0; i < k; i++) { // per ogni cluster
        Point center; // centro casuale
        center.y = rand() % image.rows; // prendo un'ordinata casuale
        center.x = rand() % image.cols; // prendo un'ascissa casuale
        Scalar centerPixel = image.at<Vec3b>(center.y, center.x); // estraggo il pixel del centro
        Scalar centerValue(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]); // estraggo il colore del centro
        centers.push_back(centerValue); // inserisco il centro nel vettore dei centri
        vector<Point>neighbors; // vettore contenente i pixel che si trovano nello stesso cluster del centro
        clusters.push_back(neighbors); // aggiungo i pixel appartenenti al cluster al vettore dei cluster
    }
}

double colorDistance(Scalar center, Scalar pixel) { // calcola la distanza tra il pixel e il centro del cluster
    Scalar distance = center - pixel;
    return sqrt(pow(distance.val[0], 2) + pow(distance.val[1], 2) + pow(distance.val[2], 2));
}

void assignCluster(Mat image, int k, vector<Scalar> centers, vector<vector<Point>>& clusters) { // assegna ogni pixel al cluster il cui centro è più vicino
    for (int y = 0; y < image.rows; y++) { // scorro le righe dell'immagine
        for (int x = 0; x < image.cols; x++) { // scorro le colonne dell'immagine
            double minDist = INFINITY; // inizializzo la distanza minima
            int clusterIndex; // indice del cluster a cui va assegnato il pixel
            Scalar pixel = image.at<Vec3b>(y, x); // estraggo il colore del pixel dall'immagine
            for (int i = 0; i < k; i++) { // per ogni cluster
                double distance = colorDistance(pixel, centers[i]); // calcolo la distanza tra il pixel e il centro
                if (distance < minDist) { // se la distanza è minore di quella minima calcolata finora
                    minDist = distance; // aggiorno la distanza minima
                    clusterIndex = i; // salvo l'indice del cluster
                }
            }
            clusters[clusterIndex].push_back(Point(x, y)); // inserisco il pixel all'interno del cluster corrispondente
        }
    }
}

double updateCenters(Mat image, int k, vector<Scalar>& centers, vector<vector<Point>> clusters, double& oldCenter, double newCenter) { // aggiorna i centri dei cluster
    for (int i = 0; i < k; i++) { // per ogni cluster
        double redAvg = 0.0, greenAvg = 0.0, blueAvg = 0.0; // inizializzo le medie delle componenti colore
        for (auto p : clusters[i]) { // scorro ogni pixel del cluster
            Scalar pixel = image.at<Vec3b>(p.y, p.x); // ottengo il valore del pixel dalle coordinate
            redAvg += pixel.val[2]; // sommo il valore della componente rossa
            greenAvg += pixel.val[1]; // sommo il valore della componente verde
            blueAvg += pixel.val[0]; // sommo il valore della componente blu
        }
        redAvg /= clusters[i].size(); // calcolo la media del rosso
        greenAvg /= clusters[i].size(); // calcolo la media del verde
        blueAvg /= clusters[i].size(); // calcolo la media del blu
        Scalar newPixel(blueAvg, greenAvg, redAvg); // creo un nuovo pixel con i valori medi dei colori
        newCenter += colorDistance(newPixel, centers[i]); // calcolo la distanza tra il nuovo centro e quello precedente
        centers[i] = newPixel; // aggiorno il centro del cluster
    }
    newCenter /= k; // calcolo la media tra le distanze dei centri
    double delta = abs(oldCenter - newCenter); // calcolo la differenza tra il centro del cluster precedente e il nuovo
    oldCenter = newCenter; // aggiorno il valore del vecchio centro sostituendolo con quello nuovo
    return delta;
}

void segment(Mat& output, int k, vector<Scalar> centers, vector<vector<Point>> clusters) { // genera l'immagine di output segmentata
    for (int i = 0; i < k; i++) { // per ogni cluster
        for (auto p : clusters[i]) { // per ogni pixel nel cluster i
            output.at<Vec3b>(p)[0] = centers[i].val[0]; // aggiorno il colore del pixel nell'immagine di output
            output.at<Vec3b>(p)[1] = centers[i].val[1];
            output.at<Vec3b>(p)[2] = centers[i].val[2];
        }
    }
}

int main(int argc, char ** argv) {
    srand(time(NULL));

    // lettura immagine a colori
    Mat image = imread(argv[1], IMREAD_COLOR);
    if(image.empty()) {
        cout <<  "Errore lettura immagine" << endl;
        return -1;
    }
    imshow("Immagine di input", image);
    waitKey(0);

    int k = atoi(argv[2]); // numero di cluster
    vector<Scalar>centers; // vector contenente i centri dei cluster
    vector<vector<Point>>clusters; // vector bidimensionale contenente i pixel di ogni cluster
    double threshold = 0.1f, // soglia per terminare il clustering
           oldCenter = INFINITY, // centro del cluster precedente
           newCenter = 0.0f, // nuovo centro del cluster
           delta = oldCenter - newCenter; // differenza tra vecchio e nuovo centro del cluster
    createCluster(image, k, centers, clusters); // creo i cluster e imposto i centri
    while (delta > threshold) {
        newCenter = 0.0f; // resetto il nuovo centro
        for (auto i : clusters) { // svuoto ogni cluster
            i.clear();
        }
        assignCluster(image, k, centers, clusters); // assegno ogni pixel al cluster con centro più vicino
        delta = updateCenters(image, k, centers, clusters, oldCenter, newCenter); // ricalcolo i centri dei cluster
    }
    Mat output = image.clone(); // immagine segmentata
    segment(output, k, centers, clusters); // segmento l'immagine
    imshow("output", output);
    waitKey(0);
    return 0;
}