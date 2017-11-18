#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

int main() {
    VideoCapture cap("../hipo.mp4");

    Mat image, mask, image_cut, image_color, second_image, second_image_color, prevT;
    vector <Point2f> corners, corners_lim[2], great_corners[2];

    vector<uchar> status;
    vector<float> err;
    Size winSize(31,31);
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,30,0.01);

    while(1) {
        cap >> image;
        if (image.data == NULL)
            return -1;

        cap >> second_image;
        if (second_image.data == NULL)
            return -1;

        resize(image, image, Size(600, 500));
        image_color = image.clone();

        resize(second_image, second_image, Size(600, 500));
        second_image_color = second_image.clone();

        cvtColor(image, image, COLOR_BGR2GRAY);
        cvtColor(second_image, second_image, COLOR_BGR2GRAY);
//    image_cut = image(Rect(200, 150, 200, 200)).clone();

        goodFeaturesToTrack(image, corners, 300, 0.01, 20, mask, 3, false, 0.01);

        for (int i = 0; i < corners.size(); i++) {
            if (corners[i].x >= 200 && corners[i].y >= 150 && corners[i].x <= 400 && corners[i].y <= 350) {
                corners_lim[0].push_back(corners[i]);
            }
        }

        calcOpticalFlowPyrLK(image, second_image, corners_lim[0], corners_lim[1], status, err,
                             winSize, 3, termcrit, 0, 0.001);

        for(size_t i=0; i < status.size(); i++) {
            if(status[i]) {
                great_corners[0].push_back(corners_lim[0][i]);
                great_corners[1].push_back(corners_lim[1][i]);

                circle(image_color, corners_lim[0][i], 3, Scalar(0, 255, 255), -1, 8);
                circle(second_image_color, corners_lim[1][i], 3, Scalar(0, 255, 255), -1, 8);
            }
        }

        imshow("image1", image_color);
        imshow("image2", second_image_color);

        Mat T = estimateRigidTransform(great_corners[1], great_corners[0], false);

        static double soma_dx=0,soma_dy=0,soma_da=0;

        if(T.data==NULL){
            T = Mat(Size(3,2),CV_32FC1, Scalar(0));
            T(Range(0,2),Range(0,2))= Mat::eye(2,2,CV_32FC1);
            cout << "Perdeu" << endl;
        }
        else {
            soma_dx += T.at<double>(0,2);
            soma_dy += T.at<double>(1,2);
            soma_da += atan2(T.at<double>(1,0), T.at<double>(0,0));
        }

        image= second_image.clone();
        prevT= T.clone();

        dx= soma_dx;
        dy= soma_dy;
        da= soma_da;

        T.at<double>(0,0) = cos(da);
        T.at<double>(0,1) = -sin(da);
        T.at<double>(1,0) = sin(da);
        T.at<double>(1,1) = cos(da);

        T.at<double>(0,2) = dx;
        T.at<double>(1,2) = dy;

        warpAffine(image_color, image_new, T, image_color.size());

        //apagar as bordas
        corners.clear();
        corners_lim[0].clear();
        corners_lim[1].clear();
        great_corners[0].clear();
        great_corners[1].clear();

        if (waitKey(30)!=-1)
            break;
    }
    return 0;
}
