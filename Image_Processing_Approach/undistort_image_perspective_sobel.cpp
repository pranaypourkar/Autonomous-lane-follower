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

// Input Quadilateral or Image plane coordinates
Point2f inputQuad[4]; 
// Output Quadilateral or World plane coordinates
Point2f outputQuad[4];
// Lambda Matrix
Mat lambda( 2, 4, CV_32FC1 );
//Input and Output Image;
Mat input, output, output_gray;

//**********************************
//sobel
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  Mat grad;



int main(int argc, char* argv[])
{
   input = imread("/home/pranay/Final_Year_Project/track7.jpg", CV_LOAD_IMAGE_COLOR);

   if(! input.data )
   // Check for invalid input
   {
   cout << "Could not open or find the image" << std::endl ;
   return -1;
   }
   

    // Set the lambda matrix the same type and size as input
    lambda = Mat::zeros( input.rows, input.cols, input.type() );

    // The 4 points that select quadilateral on the input , from top-left in clockwise order
    // These four pts are the sides of the rect box used as input 
    inputQuad[0] = Point2f( 0.4 * input.size().width,0.5 * input.size().height);
    inputQuad[1] = Point2f( 1 * input.size().width,0.5 * input.size().height);
    inputQuad[2] = Point2f( 0.2 * input.size().width,0.8 * input.size().height);
    inputQuad[3] = Point2f( 1 * input.size().width,0.8 * input.size().height);  
    // The 4 points where the mapping is to be done , from top-left in clockwise order
    outputQuad[0] = Point2f( 0,0 );
    outputQuad[1] = Point2f( input.cols,0);
    outputQuad[2] = Point2f( 0,input.rows);
    outputQuad[3] = Point2f( input.cols,input.rows  );

    cout<<input.size().height;
    cout<<input.size().width;

    // Get the Perspective Transform Matrix i.e. lambda 
    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    // Apply the Perspective Transform just found to the src image
    warpPerspective(input,output,lambda,output.size() );


    //put circle on input image
    circle(input, Point2f(0.4 * input.size().width,0.5 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
    circle(input, Point2f(1 * input.size().width,0.5 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
    circle(input, Point2f( 0.2 * input.size().width,0.8 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
    circle(input, Point2f(1 * input.size().width,0.8 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);


    //sobel
    GaussianBlur( output, output, Size(3,3), 0, 0, BORDER_DEFAULT );
    cvtColor( output, output_gray, COLOR_BGR2GRAY );

    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    Sobel( output_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    Sobel( output_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );

    convertScaleAbs( grad_x, abs_grad_x );
    convertScaleAbs( grad_y, abs_grad_y );

    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    namedWindow( "Display window1", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window1", input );// Show our image inside it.

    namedWindow( "Display window2", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window2", output );// Show our image inside it.

    namedWindow( "Display window3", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window3", grad );// Show our image inside it.

    waitKey(0);
    return 0;
}
