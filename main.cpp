/*
 * Hyperlapse
 */
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    VideoCapture cap("media/hipo.mp4");

    Mat prev_image, next_image, image_color, image_new, mask, prevT;
    vector<Point2f> corners[2], greatCorners[2];

    vector<uchar> status;
    vector<float> err;
    Size winSize(31,31);
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,30,0.01);

    cap>>prev_image;
    cvtColor(prev_image, prev_image, COLOR_BGR2GRAY);

    double dx=0, dy=0, da=0;
    while(1)
    {
        cap>>next_image;
        if(next_image.data==NULL) {
            break;
        }

        cap>>next_image;
        if(next_image.data==NULL) {
            break;
        }

        cap>>next_image;
        if(next_image.data==NULL) {
            break;
        }

        image_color = next_image.clone();

        cvtColor(next_image, next_image, COLOR_BGR2GRAY);

        goodFeaturesToTrack(prev_image, corners[0], 300, 0.01, 20, mask, 3, false, 0.01);
        calcOpticalFlowPyrLK(prev_image, next_image, corners[0], corners[1], status, err,
                             winSize, 3, termcrit, 0, 0.001);

        for(size_t i=0; i < status.size(); i++) {
            if(status[i]) {
                greatCorners[0].push_back(corners[0][i]);
                greatCorners[1].push_back(corners[1][i]);
            }
        }

//        for(int i=0; i<greatCorners[0].size(); i++)
//        {
//            circle(prev_image,greatCorners[0][i],3,Scalar(255));
//            circle(image_color,greatCorners[1][i],3,Scalar(0,255,0));
//        }

        imshow("Sem", image_color);

        Mat T = estimateRigidTransform(greatCorners[1], greatCorners[0], false);

        static double soma_dx=0,soma_dy=0,soma_da=0;

        if(T.data==NULL){
            T= Mat(Size(3,2),CV_32FC1, Scalar(0));
            T(Range(0,2),Range(0,2))= Mat::eye(2,2,CV_32FC1);
            cout << "Perdeu" << endl;
        }
        else
        {
            soma_dx+= T.at<double>(0,2);
            soma_dy+= T.at<double>(1,2);
            soma_da+= atan2(T.at<double>(1,0), T.at<double>(0,0));
        }
        prev_image= next_image.clone();
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

//        Mat T = findHomography(greatCorners[1], greatCorners[0]);

        warpAffine(image_color, image_new, T, image_color.size());
//        warpPerspective(image_color, image_new, T, next_image.size());

        const int HORIZONTAL_BORDER_CROP = 40;
        int vert_border = HORIZONTAL_BORDER_CROP * prev_image.rows/prev_image.cols;
        image_new= image_new(Range(vert_border, image_new.rows-vert_border),
                             Range(HORIZONTAL_BORDER_CROP, image_new.cols-HORIZONTAL_BORDER_CROP));

        resize(image_new,image_new,image_color.size());
        imshow("Com", image_new);

//        waitKey(0);
        if (waitKey(30)!=-1)
            break;

        greatCorners[0].clear();
        greatCorners[1].clear();
        corners[0].clear();
        corners[1].clear();
    }

    return 0;
}
