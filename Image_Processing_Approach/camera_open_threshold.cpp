#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

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
Mat input, output;




int main(int, char**)
{
    VideoCapture cap("/home/pranay/Final_Year_Project/video/test4.mp4"); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cap.set(CV_CAP_PROP_FRAME_WIDTH,640);//640
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);//480

    Mat input, thresold;
    while(1)
    {
       
        cap >> input; // get a new frame from camera


// Set the lambda matrix the same type and size as input
    lambda = Mat::zeros( input.rows, input.cols, input.type() );

    // The 4 points that select quadilateral on the input , from top-left in clockwise order
    // These four pts are the sides of the rect box used as input 
    inputQuad[0] = Point2f( 0.25 * input.size().width,0.45 * input.size().height);
    inputQuad[1] = Point2f( 0.75 * input.size().width,0.45 * input.size().height);
    inputQuad[2] = Point2f( 0.1 * input.size().width,1 * input.size().height);
    inputQuad[3] = Point2f( 0.9 * input.size().width,1 * input.size().height);  
    // The 4 points where the mapping is to be done , from top-left in clockwise order
    outputQuad[0] = Point2f( 0,0 );
    outputQuad[1] = Point2f( input.cols,0);
    outputQuad[2] = Point2f( 0,input.rows);
    outputQuad[3] = Point2f( input.cols,input.rows  );

    //cout<<input.size().height;
    //cout<<input.size().width;

    // Get the Perspective Transform Matrix i.e. lambda 
    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    // Apply the Perspective Transform just found to the src image
    warpPerspective(input,output,lambda,output.size() );


    //put circle on input image
    circle(input, Point2f(0.25 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 3 , 8, 0);
    circle(input, Point2f(0.75 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 3 , 8, 0);
    circle(input, Point2f( 0 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 3 , 8, 0);
    circle(input, Point2f(1 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 3 , 8, 0);





    inRange(output, Scalar(0, 0, 0), Scalar(255 ,255 ,213), thresold);






        namedWindow( "frame", WINDOW_NORMAL );// Create a window for display.
        namedWindow( "thresh", WINDOW_NORMAL );// Create a window for display.
        namedWindow( "output", WINDOW_NORMAL );// Create a window for display.

        imshow("frame", input);
        imshow("thresh", thresold);
        imshow("output", output);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}


