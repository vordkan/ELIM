#include <stdlib.h>
#include <opencv2/opencv.hpp>

float tSize;
int th;

using namespace std;
using namespace cv;

class TNode{
public:
    Rect region;
    TNode *UL,*UR,*LL,*LR;
    vector<TNode*> merged;
    vector<bool> mergedB = vector<bool>(4,false);
    double stddev, mean;

    TNode(Rect R){ region=R; UL=UR=LL=LR=nullptr; }
    void addRegion(TNode *R){ merged.push_back(R); }
    void setMergedB(int i){ mergedB[i] = true; }
};

TNode* split(Mat& src, Rect R){

    TNode* root = new TNode(R);

    Scalar stddev, mean;
    meanStdDev(src(R),mean,stddev);
    root->stddev = stddev[0];
    root->mean = mean[0];

    if(R.width > tSize && root->stddev > th){

        Rect ul(R.x,R.y,R.height/2,R.width/2);
        root->UL=split(src,ul);

        Rect ur(R.x,R.y+R.width/2,R.height/2,R.width/2);
        root->UR=split(src,ur);

        Rect ll(R.x+R.height/2,R.y,R.height/2,R.width/2);
        root->LL=split(src,ll);

        Rect lr(R.x+R.height/2,R.y+R.width/2,R.height/2,R.width/2);
        root->LR=split(src,lr);
    }

    rectangle(src,R,Scalar(0));
    return root;
}

/*
 _____  _____
|UL|UR| |0|1|
------- -----
|LL|LR| |3|2|
------- -----
*/

void merge(TNode *root){

    if(root->region.width > tSize && root->stddev > th){
        if(root->UL->stddev <= th && root->UR->stddev <= th){ //UL-UR
            root->addRegion(root->UL); root->setMergedB(0);
            root->addRegion(root->UR); root->setMergedB(1);
            if(root->LL->stddev <= th && root->LR->stddev <= th){
                root->addRegion(root->LL); root->setMergedB(3);
                root->addRegion(root->LR); root->setMergedB(2);
            } else {
                merge(root->LL);
                merge(root->LR);
            }
        } else if(root->UR->stddev <= th && root->LR->stddev <= th){ //UR-LR
            root->addRegion(root->UR); root->setMergedB(1);
            root->addRegion(root->LR); root->setMergedB(2);
            if(root->UL->stddev <= th && root->LL->stddev <= th){
                root->addRegion(root->UL); root->setMergedB(0);
                root->addRegion(root->LL); root->setMergedB(3);
            } else {
                merge(root->UL);
                merge(root->LL);
            }
        } else if(root->LL->stddev <= th && root->LR->stddev <= th){ //LL-LR
            root->addRegion(root->LL); root->setMergedB(3);
            root->addRegion(root->LR); root->setMergedB(2);
            if(root->UL->stddev <= th && root->UR->stddev <= th){
                root->addRegion(root->UL); root->setMergedB(0);
                root->addRegion(root->UR); root->setMergedB(1);
            } else {
                merge(root->UL);
                merge(root->UR);
            }
        } else if(root->UL->stddev <= th && root->LL->stddev <= th){ //UL-LL
            root->addRegion(root->UL); root->setMergedB(0);
            root->addRegion(root->LL); root->setMergedB(3);
            if(root->UR->stddev <= th && root->LR->stddev <= th){
                root->addRegion(root->UR); root->setMergedB(1);
                root->addRegion(root->LR); root->setMergedB(2);
            } else {
                merge(root->UR);
                merge(root->LR);
            }
        } else {
            merge(root->UL);
            merge(root->UR);
            merge(root->LL);
            merge(root->LR);
        }
    } else {
        root->addRegion(root); for(int i=0; i<4; i++) root->setMergedB(i);
    }

}

void segment(TNode *root, Mat& src){

    vector<TNode*> tmp = root->merged;

    if(tmp.size() == 0){
        segment(root->UL, src);
        segment(root->UR, src);
        segment(root->LR, src);
        segment(root->LL, src);
    } else {
        double val=0;
        for(auto x:tmp)
            val += x->mean;
        val /= tmp.size();

        for(auto x:tmp)
            src(x->region) = (int)val;

        if(tmp.size() > 1){
            if(!root->mergedB[0])
                segment(root->UL,src);
            if(!root->mergedB[1])
                segment(root->UR,src);
            if(!root->mergedB[2])
                segment(root->LR,src);
            if(!root->mergedB[3])
                segment(root->LL,src);
        }
    }

}

void splitAndMerge(Mat& src, Mat& segmented){

    int exponent = log(min(src.rows,src.cols))/log(2);
    int s = pow(2.0, double(exponent));
    Mat croppedSrc = src(Rect(0,0,s,s)).clone();
    GaussianBlur(croppedSrc,croppedSrc,Size(3,3),0,0);

    TNode* root = split(croppedSrc,Rect(0,0,croppedSrc.rows,croppedSrc.cols));
    merge(root);

    segmented = src(Rect(0,0,s,s)).clone();
    segment(root,segmented);

    imshow("croppedSrc",croppedSrc);
    imshow("segmented",segmented);
    waitKey(0);
}

int main(int argc, char** argv){

    Mat src = imread(argv[1],IMREAD_GRAYSCALE);
    if(src.empty()) return -1;

    tSize = 8;
    th = 6;

    Mat segmented;
    splitAndMerge(src,segmented);

    return 0;
}
