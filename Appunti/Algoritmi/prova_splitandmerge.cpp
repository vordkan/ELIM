#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

#define MIN_DEV 10.7
#define MAX_AREA 64

struct Region {
	vector<Region> adj;
	bool valid;
	Scalar label;
	Rect area;
};

bool predicate(Mat);
Region split(Mat, Rect);
void mergeRegion(Mat, Region&, Region&);
void merge(Mat, Region&);
void display(Mat&, Region);

int main(int argc, char** argv) {
	Mat src = imread(argv[1]);
	if(src.empty()) return -1;
	Region r = split(src, Rect(0, 0, src.rows, src.cols));
	merge(src, r);
	Mat dst = src.clone();
	Mat dst2 = Mat::zeros(src.size(), src.type());
	display(dst, r);
	display(dst2, r);
	imshow("src", src);
	imshow("dst", dst);
	imshow("dst2", dst2);
	waitKey(0);
	return 0;
}

bool predicate(Mat src) {
	Scalar dev;
	meanStdDev(src, Scalar(), dev);
	return(dev[0] < MIN_DEV || src.cols * src.rows < MAX_AREA);
}

Region split(Mat src, Rect area) {
	Region r;
	r.valide = true;
	r.area = area;
	if(predicate(src)) {
		Scalar mean;
		meanStdDev(src, mean, Scalar());
		r.label = mean;
	} else {
		int h = src.rows / 2;
		int w = src.cols / 2;
		Region r1 = split(src(Rect(0, 0, w, h)), Rect(area.x, area.y, w, h));
		Region r2 = split(src(Rect(w, 0, w, h)), Rect(area.x + w, area.y, w, h));
		Region r3 = split(src(Rect(0, h, w, h)), Rect(area.x, area.y + h, w, h));
		Region r4 = split(src(Rect(w, h, w, h)), Rect(area.x + w, area.y + h, w, h));
		r.adj.push_back(r1);
		r.adj.push_back(r2);
		r.adj.push_back(r3);
		r.adj.push_back(r4);
	}
	return r;
}

void mergeRegion(Mat src, Region& r1, Region& r2) {
	if(r1.adj.size() == 0 && r2.adj.size() == 0) {
		Rect r12 = r1.area | r2.area;
		if(predicate(src(r12))) {
			r1.area = r12;
			r1.label = (r1.label + r2.label) / 2;
			r2.valid = false;
		}
	}
}

void merge(Mat src, Region& r) {
	if(r.adj.size() > 0) {
		mergeRegion(src, r.adj.at(0), r.adj.at(1));
		mergeRegion(src, r.adj.at(2), r.adj.at(3));
		mergeRegion(src, r.adj.at(0), r.adj.at(2));
		mergeRegion(src, r.adj.at(1), r.adj.at(3));
		for(int i = 0; i < r.adj.size(); i++)
			merge(src, r.adj.at(i));
	}
}

void display(Mat& dst, Region r) {
	if(r.adj.size() == 0 && r.valid)
		rectangle(dst, r.area, r.label, FILLED);
	for(int i = 0; i < r.adj.size(); i++)
		display(dst, r.adj.at(i));
}
