#include <opencv2/opencv.hpp>
#include <stack>

using namespace cv;
using namespace std;

Point adj[4] = { // intorno di 4 pixel
    Point(1, 0), // pixel di destra
    Point(-1, 0), // pixel di sinistra
    Point(0, 1), // pixel di sotto
    Point(0, -1) // pixel di sopra
};

void grow(Mat& image, Mat& region, Mat& output, Point seed, int threshold) { // algoritmo di accrescimento
    stack<Point> regionStack; // dichiaro uno stack di appoggio per gestire le regioni
    regionStack.push(seed); // inserisco il seed
    while(!regionStack.empty()) { // finchè lo stack non è vuoto
        Point pixel = regionStack.top(); // estraggo un pixel
        regionStack.pop();
        region.at<float>(pixel) = 1; // setto il pixel come visitato
        for (auto i : adj) { // analizzo un intorno di 4 pixel del pixel estratto
            Point neighbor = pixel + i; // considero un pixel dell'intorno
            if (neighbor.x < 0 || neighbor.y < 0 || neighbor.x > image.cols - 1 || neighbor.y > image.rows - 1) { // se il pixel è esterno all'immagine
                continue; // lo ignoro
            } else { // altrimenti
                // uchar distance = abs(image.at<uchar>(pixel) - image.at<uchar>(neighbor)); (scala di grigio)
                int distance = int(pow(image.at<Vec3b>(pixel)[0] - image.at<Vec3b>(neighbor)[0], 2) // calcolo la distanza dal seed (immagine a colori)
                                 + pow(image.at<Vec3b>(pixel)[1] - image.at<Vec3b>(neighbor)[1], 2)
                                 + pow(image.at<Vec3b>(pixel)[2] - image.at<Vec3b>(neighbor)[2], 2));
                if (output.at<float>(neighbor) == 0 && // se il pixel non appartiene già ad una regione
                    region.at<float>(neighbor) == 0 && // se il pixel non è già stato usato come seed
                    distance < threshold) { // se il pixel rispetta la condizione di accrescimento
                        region.at<float>(neighbor) = 1; // accresco la regione
                        regionStack.push(neighbor); // lo inserisco nello stack
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    
    // lettura dell'immagine di input a colori
    Mat image = imread(argv[1], IMREAD_COLOR);
    if (image.empty()) { // controllo che l'immagine sia correttamente letta
        cout << "Errore lettura immagine" << endl;
        return -1;
    }
    imshow("Immagine di input", image);
    waitKey(0);

    GaussianBlur(image, image, Size(9, 9), 0, 0);

    // ridimensiono l'immagine se troppo grande
    if (image.cols > 500 || image.rows > 500) {
        resize(image, image, Size(0, 0), 0.5, 0.5);
    }

    int maxRegionNumber = 10; // numero massimo di regioni da trovare
    int minRegionSize = int(0.01f * image.rows * image.cols); // numero minimo di pixel che una regione deve possedere
    uchar label = 1; // etichetta delle regioni
    Mat output = Mat::zeros(image.size(), CV_32F); // immagine di output
    Mat region = Mat::zeros(image.size(), CV_32F); // regione dell'immagine
    for (int i = 0; i < image.rows; i++) { // scorro le colonne dell'immagine
        for (int j = 0; j < image.cols; j++) { // scorro le righe dell'immagine
            if (output.at<float>(Point(j, i)) == 0) { // se il pixel analizzato non appartiene ad alcuna regione
                grow(image, region, output, Point(j, i), atoi(argv[2])); // applico l'algoritmo di accrescimento
                int regionSize = (int)sum(region).val[0]; // numero dei pixel che appartengono alla regione corrente
                if (regionSize > minRegionSize) { // se la dimensione è maggiore di quella minima stabilita
                    output += region * label; // assegno l'etichetta alla regione
                    imshow("Regione", region * 255); // mostro la regione
                    waitKey(0); 
                    if (++label > maxRegionNumber) { // se ho trovato troppe regioni
                        cout << "Oversegmentazione" << endl;
                        return -1;
                    }
                } else { // altrimenti ignoro la regione
                    output += region * 255;
                }
                region -= region; // azzero la regione
            }
        }
    }
    return 0;
}