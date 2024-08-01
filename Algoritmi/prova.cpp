#include <opencv2/opencv.hpp>
#include <vector>
#include <stack>

using namespace std;
using namespace cv;

void createClusters(Mat image, int k, vector<Scalar>& centers, vector<vector<Point>>& clusters) {
    for (int i = 0; i < k; i++) {
        Point center;
        center.y = rand() % image.rows;
        center.x = rand() % image.cols;
        Scalar centerPixel = image.at<Vec3b>(center.y, center.x);
        Scalar centerValue(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
        centers.push_back(centerValue);
        vector<Point> neighbors;
        clusters.push_back(neighbors);
    }
}

currVar = pow(mediaGlobale * prob - currMediaGlobale, 2) / (prob * (1 - prob));
currVar += prob[w] * pow(currMedGlob[w] / prob[w] - mediaGlobale, 2);

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

double updateClusters(Mat image, int k, vector<Scalar>& centers, vector<vector<Point>> clusters, double& oldCenter, double newCenter) {
    for (int i = 0; i < k; i++) {
        double red = 0.0, green = 0.0, blue = 0.0;
        for (auto p : clusters[i]) {
            Scalar pixel = image.at<Vec3b>(p.y, p.x);
            red += pixel.val[2];
            green += pixel.val[1];
            blue += pixel.val[0];
        }
        red /= clusters[i].size();
        green /= clusters[i].size();
        blue /= clusters[i].size();
        Scalar newPixel(blue, green, red);
        newCenter += colorDistance(newPixel, centers[i]);
        centers[i] = newPixel;
    }
    newCenter /= k;
    double delta = abs(oldCenter - newCenter);
    oldCenter = newCenter;
    return delta;
}

void segment(TreeNode* r, Mat& image) {
    vector<TreeNode*> merged = r -> getMerged();
    if (merged.empty()) {
        segment(r -> getHighLeft(), image);
        segment(r -> getHighRight(), image);
        segment(r -> getLowRight(), image);
        segment(r -> getLowLeft(), image);
    } else {
        double avg = 0.0;
        for (auto i : merged) {
            avg += (int)i->getAvg();
        }
        avg /= merged.size();
        for (auto i : merged) {
            image(i -> getRegion()) = int(avg);
        }
        if (merged.size() > 1) {
            for (int i = 0; i < 4; i++) {
                if (!root -> getIsMerged(i)) {
                    switch(i) {
                        case 0: segment(r -> getHighLeft(), image); break;
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    Mat image = imread(argv[1], IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "Errore lettura immagine";
        return -1;
    }
    imshow("input", image);
    waitKey(0);
    GaussianBlur(image, image, Size(9, 9), 0, 0);
    Mat dx, dy, dxy, dx2, dy2, diagonal1, diagonal2, trace, R;
    Sobel(image, dx, CV_32F, 1, 0, 3);
    Sobel(image, dy, CV_32F, 0, 1, 3);
    multiply(dx, dy, dxy);
    pow(dx, 2, dx2);
    pow(dy, 2, dy2);
    GaussianBlur(dx2, dx2, Size(9, 9), 0, 0);
    GaussianBlur(dy2, dy2, Size(9, 9), 0, 0);
    GaussianBlur(dxy, dxy, Size(9, 9), 0, 0);
    multiply(dx2, dy2, diagonal1);
    multiply(dxy, dxy, diagonal2);
    trace = dx2 + dy2;
    pow(trace, 2, trace);
    R = diagonal1 - diagonal2 - 0.04 * trace;
    normalize(R, R, 0, 255, NORM_MINMAX, CV_32F);
    Mat corners = Mat::zeros(image.size(), image.type());
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            if (R.at<float>(y, x) > atoi(argv[2])) {
                circle(corners, Point(x, y), 3, Scalar(0), 2, 8, 0);
            }
        }
    }
    return 0;
}