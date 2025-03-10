#include <opencv2/opencv.hpp>
#include <stack>

void createCluster(Mat img, int k, vector<Scala>& center, vector<vector<Point>>& cluster){
    for(int i = 0; i < k; i++){
        Point center;
        center.x = rand() % img.cols;
        center.y = rand() % img.rows;
        Scalar pixel = img.at<Vec3b>.at(center.y, center.x);
        Scalar value(pixel.val[0], pixel.val[1], pixel.val[2]);
        center.push_back(value);
        vector<Point>neighbor;
        cluster.push_back(neighbor);
    }
}

double colorDistance(Mat img, int k, vector<Scalar> center, vector<vector<Point>> & cluster){
    Scalar distance = center - pixel;
    return sqrt(pow(distance.val[0],2) + pow(distance.val[1],2) + pow(distance.val[2],2));
}

