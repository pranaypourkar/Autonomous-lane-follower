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
Mat image, dst;
image = imread("/home/pranay/Final_Year_Project/code/per_t.png", CV_LOAD_IMAGE_COLOR);

if(! image.data )
// Check for invalid input
{
cout << "Could not open or find the image" << std::endl ;
return -1;
}

//filter
GaussianBlur( image, dst, Size( 3, 3), 0, 0 );//applying Gaussian filter 

namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
imshow( "Display window", image );// Show our image inside it.

namedWindow( "Display window1", WINDOW_AUTOSIZE );// Create a window for display.
imshow( "Display window1", dst );// Show our image inside it.

waitKey(0);
return 0;
}
