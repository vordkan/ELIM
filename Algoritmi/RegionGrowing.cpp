#include <opencv2/opencv.hpp>
#include <stack>

using namespace cv;
using namespace std;

Point adj[4] = { 
    Point(1, 0), 
    Point(-1, 0), 
    Point(0, 1),
    Point(0, -1) 
};

void grow(Mat& image, Mat& region, Mat& output, Point seed, int threshold) {
    stack<Point> regionStack; 
    regionStack.push(seed); 
    while(!regionStack.empty()) {
        Point pixel = regionStack.top(); 
        regionStack.pop();
        region.at<float>(pixel) = 1; 
        for (auto i : adj) { 
            Point neighbor = pixel + i;
            if (neighbor.x < 0 || neighbor.y < 0 || neighbor.x > image.cols - 1 || neighbor.y > image.rows - 1) { 
                continue; 
            } else { 
                int distance = int(pow(image.at<Vec3b>(pixel)[0] - image.at<Vec3b>(neighbor)[0], 2) 
                                 + pow(image.at<Vec3b>(pixel)[1] - image.at<Vec3b>(neighbor)[1], 2)
                                 + pow(image.at<Vec3b>(pixel)[2] - image.at<Vec3b>(neighbor)[2], 2));
                if (output.at<float>(neighbor) == 0 && 
                    region.at<float>(neighbor) == 0 && 
                    distance < threshold) { 
                        region.at<float>(neighbor) = 1; 
                        regionStack.push(neighbor); 
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    
    Mat image = imread(argv[1], IMREAD_COLOR);
    if (image.empty()) { 
        cout << "Errore lettura immagine" << endl;
        return -1;
    }
    imshow("Immagine di input", image);
    waitKey(0);

    GaussianBlur(image, image, Size(9, 9), 0, 0);

    if (image.cols > 500 || image.rows > 500) {
        resize(image, image, Size(0, 0), 0.5, 0.5);
    }

    int maxRegionNumber = 10; 
    int minRegionSize = int(0.01f * image.rows * image.cols); 
    uchar label = 1;
    Mat output = Mat::zeros(image.size(), CV_32F); 
    Mat region = Mat::zeros(image.size(), CV_32F); 
    for (int i = 0; i < image.rows; i++) { 
        for (int j = 0; j < image.cols; j++) { 
            if (output.at<float>(Point(j, i)) == 0) { 
                grow(image, region, output, Point(j, i), atoi(argv[2])); 
                int regionSize = (int)sum(region).val[0]; 
                if (regionSize > minRegionSize) { 
                    output += region * label; 
                    imshow("Regione", region * 255); 
                    waitKey(0); 
                    if (++label > maxRegionNumber) { 
                        cout << "Oversegmentazione" << endl;
                        return -1;
                    }
                } else {
                    output += region * 255;
                }
                region -= region;
            }
        }
    }
    return 0;
}