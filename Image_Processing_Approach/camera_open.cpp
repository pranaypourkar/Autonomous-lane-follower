#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{
    VideoCapture cap(1); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);//640
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);//480

    while(1)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        imshow("edges", frame);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}


