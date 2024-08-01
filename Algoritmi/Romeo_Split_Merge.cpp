#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int size; // dimensione minima di una regione
int thr; // soglia

/*----Quadrante------
highLeft | highRight 
lowLeft  | lowRight
0 | 1
3 | 2
-------------------*/

class TreeNode {
    private:
        Rect region; // regione dell'immagine
        TreeNode *lowLeft, // nodo in basso a sinistra nel quadrante
                 *lowRight, // nodo in basso a destra nel quadrante
                 *highLeft, // nodo in alto a sinistra nel quadrante
                 *highRight; // nodo in alto a destra nel quadrante
        vector<TreeNode*> merged; // regioni unite
        vector<bool> isMerged = vector<bool>(4, false); // indica quali regioni sono state unite
        float stdDev, // deviazione standard nella regione
              avg; // media dei pixel nella regione
    public:
        TreeNode(Rect r) { // costruttore, inizializza la regione e i puntatori a nullptr
            region = r;
            lowLeft = lowRight = highLeft = highRight = nullptr;
        }

        // metodi get
        TreeNode* getLowLeft() {return lowLeft;}
        TreeNode* getLowRight() {return lowRight;}
        TreeNode* getHighLeft() {return highLeft;}
        TreeNode* getHighRight() {return highRight;}
        float getStdDev() {return stdDev;}
        float getAvg() {return avg;}
        vector<TreeNode*>& getMerged() {return merged;}
        Rect& getRegion() {return region;}
        bool getIsMerged(int i) {return isMerged[i];}

        // metodi set
        void setLowLeft(TreeNode* n) {lowLeft = n;}
        void setLowRight(TreeNode* n) {lowRight = n;}
        void setHighLeft(TreeNode* n) {highLeft = n;}
        void setHighRight(TreeNode* n) {highRight = n;}
        void setStdDev(float a) {stdDev = a;}
        void setAvg(float a) {avg = a;}
        void setIsMerged(int i) {isMerged[i] = true;}

        void addRegion(TreeNode* root) {merged.push_back(root);}
};

TreeNode* split(Mat& image, Rect r) {
    TreeNode* root = new TreeNode(r); // istanzio un nuovo nodo

    // calcolo media e deviazione standard
    Scalar stdDev, avg;
    meanStdDev(image(r), avg, stdDev);
    root -> setAvg(avg[0]);
    root -> setStdDev(stdDev[0]);
    if (r.width > size && root -> getStdDev() > thr) { // controllo se la larghezza della regione è maggiore della dimensione minima e se la deviazione standard supera la soglia
        Rect highLeft(r.x, r.y, r.width / 2, r.height / 2); // creo il quadrante in alto a sinistra
        root -> setHighLeft(split(image, highLeft));
        Rect highRight(r.x + r.width / 2, r.y, r.width / 2, r.height / 2); // creo il quadrante in alto a destra
        root -> setHighRight(split(image, highRight));
        Rect lowRight(r.x + r.width / 2, r.y + r.height / 2, r.width / 2, r.height / 2); // creo il quadrante in basso a destra
        root -> setLowRight(split(image, lowRight));
        Rect lowLeft(r.x, r.y + r.height / 2, r.width / 2, r.height / 2); // creo il quadrante in basso a sinistra
        root -> setLowLeft(split(image, lowLeft));
    }
    return root;
}

void merge(TreeNode* root) {
    if (root -> getRegion().width > size && root -> getStdDev() > thr) { // controllo se la larghezza della regione è maggiore della dimensione minima e se la deviazione standard supera la soglia
        if (root -> getHighLeft() -> getStdDev() < thr && root -> getHighRight() -> getStdDev() < thr) { // primo caso: fondo highLeft e highRight
            root -> addRegion(root -> getHighLeft());
            root -> addRegion(root -> getHighRight());
            root -> setIsMerged(0);
            root -> setIsMerged(1);
            if (root -> getLowLeft() -> getStdDev() < thr && root -> getLowRight() -> getStdDev() < thr) { // controllo se anche lowLeft e lowRight possono essere fusi
                root -> addRegion(root -> getLowLeft());
                root -> addRegion(root -> getLowRight());
                root -> setIsMerged(3);
                root -> setIsMerged(2);
            } else { // se non posso fondere lowLeft e lowRight
                merge(root -> getLowLeft());
                merge(root -> getLowRight());
            }
        } else if (root -> getHighRight() -> getStdDev() < thr && root -> getLowRight() -> getStdDev() < thr) { // secondo caso: fondo highRight e lowRight
            root -> addRegion(root -> getHighRight());
            root -> addRegion(root -> getLowRight());
            root -> setIsMerged(1);
            root -> setIsMerged(2);   
            if (root -> getHighLeft() -> getStdDev() < thr && root -> getLowLeft() -> getStdDev() < thr) { // controllo se anche highLeft e lowLeft possono essere fusi
                root -> addRegion(root -> getHighLeft());
                root -> addRegion(root -> getLowLeft());
                root -> setIsMerged(0);
                root -> setIsMerged(3);
            } else { // se non posso fondere highLeft e lowLeft
                merge(root -> getHighLeft());
                merge(root -> getLowLeft());
            } 
        } else if (root -> getLowRight() -> getStdDev() < thr && root -> getLowLeft() -> getStdDev() < thr) { // terzo caso: fondo lowRight e lowLeft
            root -> addRegion(root -> getLowRight());
            root -> addRegion(root -> getLowLeft());
            root -> setIsMerged(2);
            root -> setIsMerged(3);   
            if (root -> getHighLeft() -> getStdDev() < thr && root -> getHighRight() -> getStdDev() < thr) { // controllo se anche highLeft e highRight possono essere fusi
                root -> addRegion(root -> getHighLeft());
                root -> addRegion(root -> getHighRight());
                root -> setIsMerged(0);
                root -> setIsMerged(1);
            } else { // se non posso fondere highLeft e highRight
                merge(root -> getHighLeft());
                merge(root -> getHighRight());
            }
        } else if (root -> getLowLeft() -> getStdDev() < thr && root -> getHighLeft() -> getStdDev() < thr) { // quarto caso: fondo lowLeft e highLeft
            root -> addRegion(root -> getLowLeft());
            root -> addRegion(root -> getHighLeft());
            root -> setIsMerged(3);
            root -> setIsMerged(0);   
            if (root -> getHighRight() -> getStdDev() < thr && root -> getLowRight() -> getStdDev() < thr) { // controllo se anche highRight e lowRight possono essere fusi
                root -> addRegion(root -> getHighRight());
                root -> addRegion(root -> getLowRight());
                root -> setIsMerged(1);
                root -> setIsMerged(2);
            } else { // se non posso fondere highRight e lowRight
                merge(root -> getHighRight());
                merge(root -> getLowRight());
            }
        } else { // se non posso eseguire fusioni
            merge(root -> getHighLeft());
            merge(root -> getHighRight());
            merge(root -> getLowRight());
            merge(root -> getLowLeft());
        }
    } else { // se la regione non è ulteriormente divisibile
        root -> addRegion(root);
        for (int i = 0; i < 4; i++) {
            root -> setIsMerged(i);
        }
    }
}

void segment(TreeNode* root, Mat& image) {
    vector<TreeNode*>merged = root -> getMerged(); // lista di regioni unite
    if (merged.empty()) { // se la lista è vuota esegue la segmentazione sulle 4 sottoregioni
        segment(root -> getHighLeft(), image);
        segment(root -> getHighRight(), image);
        segment(root -> getLowLeft(), image);
        segment(root -> getLowRight(), image);
    } else {
        float avg = 0.0f; // media dei valori dei pixel della regione
        for (auto i : merged) { // sommo il valore di ogni pixel della regione
            avg += (int) i -> getAvg();
        }
        avg /= merged.size(); // calcolo la media dei valori pixel della regione
        for (auto i : merged) { // assegno il valore medio ai pixel della regione
            image(i -> getRegion()) = (int)avg;
        }
        if (merged.size() > 1) { // eseguo la segmentazione sulle regioni non unite
            for (int i = 0; i < 4; i++) {
                if (!root -> getIsMerged(i)) {
                    switch (i) {
                        case 0: segment(root->getHighLeft(), image); break;
                        case 1: segment(root->getHighRight(), image); break;
                        case 2: segment(root->getLowRight(), image); break;
                        case 3: segment(root->getLowLeft(), image); break;
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    Mat image = imread(argv[1], IMREAD_GRAYSCALE); // leggo l'immagine di input
    if (image.empty()) {
        cout << "Errore lettura immagine" << endl;
        return -1;
    }
    imshow("Input", image);
    waitKey(0);
    thr = atoi(argv[2]);
    size = atoi(argv[3]);
    GaussianBlur(image, image, Size(9, 9), 0, 0);
    int exponent = log(min(image.cols, image.rows)) / log(2); // dimensione massima del quadrante
    int blockSize = pow(2.0, (double)exponent); // dimensione del quadrante
    Rect square = Rect(0, 0, blockSize, blockSize); // definizione di un quadrante
    image = image(square).clone(); // estraggo il quadrante
    Mat segmentedImage = image.clone(); // immagine segmentata
    TreeNode *root = split(image, Rect(0, 0, image.rows, image.cols)); // divide l'immagine in quadranti e restituisce la radice
    merge(root); // applica l'algoritmo di merging per fondere le regioni adiacenti
    segment(root, segmentedImage); // segmenta l'immagine
    imshow("Segmented", segmentedImage);
    waitKey(0);
    return 0;
}