#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

double colorDistance(Scalar pixel, Scalar kpixel) {
	double blue = pixel[0] - kpixel[0];
	double green = pixel[1] - kpixel[1];
	double red = pixel[2] - kpixel[2];
	double dist = sqrt(pow(blue,2)+pow(green,2)+pow(red,2));
	return dist;
}

void create_cluster_info(Mat src, int num_clust, vector<Scalar>& centroids, vector<vector<Point>>& ptInClust) {
	for(int k = 0; k < num_clust; k++) {
		Point pt;
		pt.x = rand() % src.cols;
		pt.y = rand() % src.rows;
		Scalar pixel = src.at<Vec3b>(pt);
		centroids.push_back(pixel);
		vector<Point> points;
		ptInClust.push_back(points);
	}
}

void find_associated_cluster(Mat src, int num_clust, vector<Scalar> centroids, vector<vector<Point>>& ptInClust) {
	for(int r = 0; r < src.rows; r++) {
		for(int c = 0; c < src.cols; c++) {
			double min_dist = INFINITY;
			int index = 0;
			Scalar pixel = src.at<Vec3b>(r,c);
			for(int k = 0; k < num_clust; k++) {
				Scalar kpixel = centroids[k];
				double dist = colorDistance(pixel, kpixel);
				if(dist < min_dist) {
					min_dist = dist;
					index = k;
				}
			}
			ptInClust[index].push_back(Point(c,r));
		}
	}
}

double adjust_cluster(Mat src, int num_clust, vector<Scalar>& centroids, vector<vector<Point>> ptInClust, double& old_center, double new_center) {
	double diff_change;
	for(int k = 0; k < num_clust; k++) {
		vector<Point> kpoints = ptInClust[k];
		double blue = 0.0, green = 0.0, red = 0.0;
		for(int i = 0; i < kpoints.size(); i++) {
			Scalar pixel = src.at<Vec3b>(kpoints[i]);
			blue += pixel[0];
			green += pixel[1];
			red += pixel[2];
		}
		blue /= kpoints.size();
		green /= kpoints.size();
		red /= kpoints.size();
		Scalar new_pixel(blue, green, red);
		new_center += colorDistance(new_pixel, centroids[k]);
		centroids[k] = new_pixel;
	}
	new_center /= num_clust;
	diff_change = abs(old_center - new_center);
	cout << "diff_change: " << diff_change << endl;
	old_center = new_center;
	return diff_change;
}

void apply_final_cluster(Mat src, Mat& dst, int num_clust, vector<Scalar> centroids, vector<vector<Point>> ptInClust) {
	for(int k = 0; k < num_clust; k++) {
		vector<Point> kpoints = ptInClust[k];
		Scalar pixel = centroids[k];
		for(int i = 0; i < kpoints.size(); i ++) {
			dst.at<Vec3b>(kpoints[i])[0] = pixel[0];
			dst.at<Vec3b>(kpoints[i])[1] = pixel[1];
			dst.at<Vec3b>(kpoints[i])[2] = pixel[2];
		}
	}
}

int main(int argc, char** argv) {
	srand(time(NULL));
	Mat src = imread(argv[1]);
	if(src.empty()) return -1;
	int num_clust = atoi(argv[2]);
	vector<Scalar> centroids;
	vector<vector<Point>> ptInClust;
	double threshold = 0.01;
	double old_center = INFINITY;
	double new_center = 0.0;
	double diff_change = old_center - new_center;
	create_cluster_info(src, num_clust, centroids, ptInClust);
	while(threshold < diff_change) {
		new_center = 0.0;
		for(int k = 0; k < num_clust; k++) ptInClust[k].clear();
		find_associated_cluster(src, num_clust, centroids, ptInClust);
		diff_change = adjust_cluster(src, num_clust, centroids, ptInClust, old_center, new_center);
	}
	Mat dst = src.clone();
	apply_final_cluster(src, dst, num_clust, centroids, ptInClust);
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}
