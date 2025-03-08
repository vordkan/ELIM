#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

void createCluster(Mat image, int k, vector<Scalar>& centers, vector<vector<Point>>& clusters) { 
    for (int i = 0; i < k; i++) { 
        Point center; 
        center.y = rand() % image.rows; 
        center.x = rand() % image.cols; 
        Scalar centerPixel = image.at<Vec3b>(center.y, center.x); 
        Scalar centerValue(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
        centers.push_back(centerValue); 
        vector<Point>neighbors; 
        clusters.push_back(neighbors);
    }
}

double colorDistance(Scalar center, Scalar pixel) { 
    Scalar distance = center - pixel;
    return sqrt(pow(distance.val[0], 2) + pow(distance.val[1], 2) + pow(distance.val[2], 2));
}

void assignCluster(Mat image, int k, vector<Scalar> centers, vector<vector<Point>>& clusters) { 
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) { 
            double minDist = INFINITY; 
            int clusterIndex;
            Scalar pixel = image.at<Vec3b>(y, x);
            for (int i = 0; i < k; i++) { 
                double distance = colorDistance(pixel, centers[i]);
                if (distance < minDist) { 
                    minDist = distance; 
                    clusterIndex = i;
                }
            }
            clusters[clusterIndex].push_back(Point(x, y)); 
        }
    }
}

double updateCenters(Mat image, int k, vector<Scalar>& centers, vector<vector<Point>> clusters, double& oldCenter, double newCenter) {
    for (int i = 0; i < k; i++) { 
        double redAvg = 0.0, greenAvg = 0.0, blueAvg = 0.0;
        for (auto p : clusters[i]) { 
            Scalar pixel = image.at<Vec3b>(p.y, p.x); 
            redAvg += pixel.val[2];
            greenAvg += pixel.val[1]; 
            blueAvg += pixel.val[0]; 
        }
        redAvg /= clusters[i].size();
        greenAvg /= clusters[i].size(); 
        blueAvg /= clusters[i].size(); 
        Scalar newPixel(blueAvg, greenAvg, redAvg); 
        newCenter += colorDistance(newPixel, centers[i]); 
        centers[i] = newPixel; 
    }
    newCenter /= k; 
    double delta = abs(oldCenter - newCenter); 
    oldCenter = newCenter; 
    return delta;
}

void segment(Mat& output, int k, vector<Scalar> centers, vector<vector<Point>> clusters) { 
    for (int i = 0; i < k; i++) { 
        for (auto p : clusters[i]) { 
            output.at<Vec3b>(p)[0] = centers[i].val[0]; 
            output.at<Vec3b>(p)[1] = centers[i].val[1];
            output.at<Vec3b>(p)[2] = centers[i].val[2];
        }
    }
}

int main(int argc, char ** argv) {
    srand(time(NULL));

    Mat image = imread(argv[1], IMREAD_COLOR);
    if(image.empty()) {
        cout <<  "Errore lettura immagine" << endl;
        return -1;
    }
    imshow("Immagine di input", image);
    waitKey(0);

    int k = atoi(argv[2]);
    vector<Scalar>centers;
    vector<vector<Point>>clusters; 
    double threshold = 0.1f, 
           oldCenter = INFINITY,
           newCenter = 0.0f,
           delta = oldCenter - newCenter;
    createCluster(image, k, centers, clusters);
    while (delta > threshold) {
        newCenter = 0.0f; 
        for (auto i : clusters) { 
            i.clear();
        }
        assignCluster(image, k, centers, clusters); 
        delta = updateCenters(image, k, centers, clusters, oldCenter, newCenter); 
    }
    Mat output = image.clone(); 
    segment(output, k, centers, clusters); 
    imshow("output", output);
    waitKey(0);
    return 0;
}