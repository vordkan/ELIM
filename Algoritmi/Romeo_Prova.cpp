#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

void createClusters(Mat image, int k, vector<Scalar>& centers, vector<vector<Point>>& clusters) {
    for (int i = 0; i < k; i++) {
        Point center;
        center.y = rand() % image.rows;
        center.x = rand() % image.cols;
        Scalar centerValue = image.at<Vec3b>(center.y, center.x);
        centers.push_back(centerValue);
        vector<Point>neighbors;
        clusters.push_back(neighbors);
    }
}

double colorDistance(Scalar pixel, Scalar center) {
    Scalar distance = pixel - center;
    return sqrt(pow(distance.val[2], 2) + pow(distance.val[1], 2) + pow(distance.val[0], 2));
}

void assignClusters(Mat image, int k, vector<Scalar> centers, vector<vector<Point>>& clusters) {
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            Scalar pixel = image.at<Vec3b>(y, x);
            double minDist = INFINITY;
            int index;
            for (int i = 0; i < k; i++) {
                double dist = colorDistance(pixel, centers[i]);
                if (dist < minDist) {
                    minDist = dist;
                    index = i;
                }
            }
            clusters[index].push_back(Point(x, y));
        }
    }
}

double updateClusters(Mat image, int k, vector<Scalar> centers, vector<vector<Point>> clusters, double& oldCenter, double newCenter) {
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
    double delta = abs(newCenter - oldCenter);
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

int main(int argc, char** argv) {
    srand(time(NULL));
    Mat image = imread(argv[1], IMREAD_COLOR);
    if (image.empty()) {
        cout << "Immagine vuota";
        return -1;
    }
    imshow("input", image);
    waitKey(0);
    int k = atoi(argv[2]);
    vector<Scalar>centers;
    vector<vector<Point>>clusters;
    double threshold = 0.1,
           oldCenter = INFINITY,
           newCenter = 0.0,
           delta = oldCenter - newCenter;
    createClusters(image, k, centers, clusters);
    while (delta > threshold) {
        newCenter = 0.0;
        for (auto i : clusters) {
            i.clear();
        }
        assignClusters(image, k, centers, clusters);
        delta = updateClusters(image, k, centers, clusters, oldCenter, newCenter);
    }
    Mat output = image.clone();
    segment(output, k, centers, clusters);
    imshow("output", output);
    waitKey(0);
    return 0;
}