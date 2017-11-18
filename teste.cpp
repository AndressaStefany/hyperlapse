#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

int main() {
    VideoCapture cap("media/hipo.mp4");

    Mat image, mask, image_color, second_image, second_image_color, prevT, image_new;
    vector <Point2f> corners, corners_lim[2], great_corners[2];

    // paramentros de calcOpticalFlowPyrLK()
    vector<uchar> status;
    vector<float> err;
    Size winSize(31,31);
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,30,0.01);

    double dx=0, dy=0, da=0;
    while(1) {
        cap >> image;// primeiro frame
        if (image.data == NULL)
            return -1;

        cap >> second_image;// segundo frame
        if (second_image.data == NULL)
            return -1;

        resize(image, image, Size(600, 500));
        image_color = image.clone();

        resize(second_image, second_image, Size(600, 500));
        second_image_color = second_image.clone();
        // deixar as imagens em tons de cinza
        cvtColor(image, image, COLOR_BGR2GRAY);
        cvtColor(second_image, second_image, COLOR_BGR2GRAY);

        // achar melhores pontos e borda
        // metodo Shi-Tomasi
        goodFeaturesToTrack(image, corners, 300, 0.01, 20, mask, 3, false, 0.01);

        // filtra os pontos de borda desejados
        for (int i = 0; i < corners.size(); i++) {
            if (corners[i].x >= 200 && corners[i].y >= 150 && corners[i].x <= 400 && corners[i].y <= 350) {
                corners_lim[0].push_back(corners[i]);
            }
        }

        // rastrear pontos utilizando metodo Lucas Kanade
        calcOpticalFlowPyrLK(image, second_image, corners_lim[0], corners_lim[1], status, err,
                             winSize, 3, termcrit, 0, 0.001);

        // retira os ponto de bordas ruins
        for(size_t i=0; i < status.size(); i++) {
            if(status[i]) {
                great_corners[0].push_back(corners_lim[0][i]);
                great_corners[1].push_back(corners_lim[1][i]);

                // pinta na imagem os pontos
                circle(image_color, corners_lim[0][i], 3, Scalar(0, 255, 255), -1, 8);
                circle(second_image_color, corners_lim[1][i], 3, Scalar(0, 255, 255), -1, 8);
            }
        }

        imshow("image1", image_color);

        // estima a matriz com o deslocamento em x e y, e o angulo
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

        // recalcula a matriz T
        T.at<double>(0,0) = cos(da);
        T.at<double>(0,1) = -sin(da);
        T.at<double>(1,0) = sin(da);
        T.at<double>(1,1) = cos(da);

        T.at<double>(0,2) = dx;
        T.at<double>(1,2) = dy;

        // aplica a rotaçao e translado em image_new
        warpAffine(image_color, image_new, T, image_color.size());

        imshow("Hyper", image_new);

        //apaga as bordas
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
