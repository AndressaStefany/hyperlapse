/*
 * Hyperlapse
 */
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

Mat image_color, prev_image;
vector<Point2f> points;
int cont=0;

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        points.push_back(Point2f(x, y));
        cout <<"("<<x<<","<<y<<")"<<endl;
//        circle(image_color,points[cont],3,Scalar(0,255,0));
//        cont++;
    }
}

int main() {
    VideoCapture cap("../hipo.mp4");

    Mat next_image, image_new, mask, prevT;

    double dx=0, dy=0, da=0;
    while(1)
    {
        cap>>next_image;
        if(next_image.data==NULL)
            break;

        setMouseCallback("janela2", CallBackFunc, NULL);
        waitKey();

        for(int i=0; i<greatCorners[0].size(); i++)
            circle(prev_image,points[i],3,Scalar(0,255,0), -1, 8);

        imshow("janela2", next_image);

        if (waitKey(30)!=-1)
            break;
    }

    return 0;
}
