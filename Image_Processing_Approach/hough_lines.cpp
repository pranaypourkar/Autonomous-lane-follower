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
int threshold_type=3 , threshold_count=179;
Mat src_rgb,src_gray,src_thresh, detected_edges, detected_edges_rgb, src_gray_blur;
int ratio = 3;
int kernel_size = 3;
int canny_lowThreshold=100;
//Mat hsv_channels[3];


int main(int argc, char* argv[])
{

src_rgb = imread("/home/pranay/Final_Year_Project/code/per_t.png", CV_LOAD_IMAGE_COLOR);  //CV_LOAD_IMAGE_COLOR

/// Convert to HSV
//cvtColor( image, image, COLOR_BGR2HSV);

/// Convert to gray
cvtColor( src_rgb, src_gray, COLOR_BGR2GRAY);

if(! src_gray.data )
// Check for invalid input
{
cout << "Could not open or find the image" << std::endl ;
return -1;
}

//split( image, hsv_channels );


//canny
blur( src_gray, src_gray_blur, Size(3,3) );

//threhold
threshold( src_gray_blur, src_thresh, threshold_count, 255,threshold_type);

/// Canny detector
Canny( src_thresh, detected_edges, canny_lowThreshold, canny_lowThreshold*ratio, kernel_size );
/// Using Canny's output as a mask, we display our result

//cout << detected_edges;

detected_edges_rgb = Scalar::all(0);

src_rgb.copyTo( detected_edges_rgb, detected_edges); // here detected_edges is an binary image. It is mapped to pixels in source rgb image


//ROI
//Mat src_ROI (detected_edges, Rect(src_rgb.rows/4, src_rgb.cols/8, (3*src_rgb.cols)/4, src_rgb.rows/2) ); //length=cols/2 breadth=rows/4
//(x,y,width,height)

//hough lines
vector<Vec2f> lines;
HoughLines(detected_edges, lines, 1, CV_PI/180, 120, 0, 0 );
//HoughLines(detected_edges, lines, 1, CV_PI/180, 150, 0, 0 );

/*
• lines: A vector that will store the parameters (r, θ) of the detected lines
• rho : The resolution of the parameter r in pixels. We use 1 pixel.
• theta: The resolution of the parameter θ in radians. We use 1 degree (CV_PI/180)
• threshold: The minimum number of intersections to “detect” a lin
*/
cout<<lines.size() <<" " ;

for( size_t i = 0; i < lines.size(); i++ )
{
float rho = lines[i][0], theta = lines[i][1];
Point pt1, pt2;
double a = cos(theta), b = sin(theta);
double x0 = a*rho, y0 = b*rho;
pt1.x = cvRound(x0 + 1000*(-b));
pt1.y = cvRound(y0 + 1000*(a));
pt2.x = cvRound(x0 - 1000*(-b));
pt2.y = cvRound(y0 - 1000*(a));
line( src_rgb, pt1, pt2, Scalar(255,0,255), 3);
}



int count=0;/*
for(int i=0;i<detected_edges.rows;i++)
{
	for(int j=0;j<detected_edges.cols;j++)
	{
		if(detected_edges.at<uchar>(Point(j,i))==255)
		{
			count++;
		}


	}
}
cout<<count;*/

namedWindow( "window1", WINDOW_AUTOSIZE );// Create a window for display.
namedWindow( "window2", WINDOW_AUTOSIZE );// Create a window for display.
namedWindow( "window3", WINDOW_AUTOSIZE );// Create a window for display.
namedWindow( "window4", WINDOW_AUTOSIZE );// Create a window for display.

imshow( "window1",src_gray_blur);// Show our image inside it.
imshow( "window2",src_thresh);// Show our image inside it.
imshow( "window3",detected_edges);// Show our image inside it.
imshow( "window4",src_rgb);// Show our image inside it.


//createTrackbar("Threshold","Display window",&value,255, callback); 
waitKey(0);
return 0;
}



