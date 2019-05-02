#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

//core section defined the basic building blocks of the library
//highgui module contains functions for input and output operations

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
Mat image;
image = imread("/home/pranay/Final_Year_Project/code/frame1024.png", CV_LOAD_IMAGE_COLOR);

if(! image.data )
// Check for invalid input
{
cout << "Could not open or find the image" << std::endl ;
return -1;
}

Mat thresold;
//inRange(image, cv::Scalar(210, 200, 0), cv::Scalar(255, 255, 100), thresold);


namedWindow("My Window", WINDOW_NORMAL);
namedWindow("My Window2", WINDOW_NORMAL);
int m = 0;
createTrackbar("r1", "My Window", &m, 255);

//Create trackbar to change contrast
int n= 0;
createTrackbar("y1", "My Window", &n, 255);

//Create trackbar to change contrast
int o= 0;
createTrackbar("b1", "My Window", &o, 255);


int q = 0;
createTrackbar("r2", "My Window2", &q, 255);

//Create trackbar to change contrast
int w= 0;
createTrackbar("y2", "My Window2", &w, 255);

//Create trackbar to change contrast
int e= 42;
createTrackbar("b2", "My Window2", &e, 255);


  while (true)
     {
          //Change the brightness and contrast of the image (For more infomation http://opencv-srf.blogspot.com/2013/07/change-contrast-of-image-or-video.html)
           
          inRange(image, cv::Scalar(m, n, o), cv::Scalar(255 - q,255 - w,255 - e), thresold);

          //show the brightness and contrast adjusted image
          imshow("My Window", thresold);
          imshow("My Window2", image);

          // Wait until user press some key for 50ms
          //waitKey(0);
          // Wait until user press some key for 50ms
          int iKey = waitKey(50);

          //if user press 'ESC' key
          if (iKey == 27)
          {
               break;
          }
          
     }
return 0;
}
