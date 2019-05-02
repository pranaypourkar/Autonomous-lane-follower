#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;


//intrinsic and extrinsic parametrs
float camera_matrix[9] = {819.752611, 0.000000, 329.461908, 0.000000, 819.011546, 245.006332, 0.000000, 0.000000, 1.000000};
Mat intrinsic = Mat(3, 3, CV_32FC1, camera_matrix);

float dist_matrix[5] = {0.066489, 0.117170, 0.00383, 0.018518, 0.000000};
Mat distCoeffs = Mat(1, 5, CV_32FC1, dist_matrix);

float projection_matrix[12] = {836.864685, 0.000000, 336.647743, 0.000000, 0.000000, 845.784973, 246.055352, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000};
Mat proj_matrix = Mat(3, 4, CV_32FC1, projection_matrix);

//**********************************


int main(int argc, char* argv[])
{

//cout<<"\n"<<intrinsic<<"\n"<<distCoeffs<<"\n"<<proj_matrix;
    VideoCapture capture = VideoCapture(1);
    Mat image, imageUndistorted;
    while(1)
    {
        capture >> image;
        undistort(image, imageUndistorted, intrinsic, distCoeffs);

        imshow("window1", image);
        imshow("window2", imageUndistorted);
        waitKey(1);
    }

    capture.release();
return 0;
}
