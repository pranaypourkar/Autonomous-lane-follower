#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

//core section defined the basic building blocks of the library
//highgui module contains functions for input and output operations

using namespace std;
using namespace cv;

int value;
int threshold_type=3;
Mat image;


void callback(int, void *)
{
	threshold( image, image, value, 255,threshold_type );
	imshow( "Display window", image );// Show our image inside it.
}


int main(int argc, char* argv[])
{

image = imread("/home/pranay/Final_Year_Project/human_detection/test1.jpg", CV_LOAD_IMAGE_GRAYSCALE);  //CV_LOAD_IMAGE_COLOR

if(! image.data )
// Check for invalid input
{
cout << "Could not open or find the image" << std::endl ;
return -1;
}

value=0; //initialize 
namedWindow( "Display window", WINDOW_NORMAL );// Create a window for display.
imshow( "Display window", image );// Show our image inside it.

createTrackbar("Threshold","Display window",&value,255, callback); 



waitKey(0);
return 0;
}



