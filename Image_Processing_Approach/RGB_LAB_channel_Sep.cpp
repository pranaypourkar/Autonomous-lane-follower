#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat image, image_out;
    image=imread("/home/pranay/Final_Year_Project/code/frame1024.png");
    cvtColor(image, image_out, CV_BGR2Lab);


Mat lab[3];   //destination array
split(image,lab);//split source  


    namedWindow( "lab", WINDOW_NORMAL );// Create a window for display.
    imshow("lab",image_out); 
    namedWindow( "l", WINDOW_NORMAL );// Create a window for display.
    imshow("l",lab[0]); 
    namedWindow( "a", WINDOW_NORMAL );// Create a window for display.
    imshow("a",lab[1]); 
    namedWindow( "b", WINDOW_NORMAL );// Create a window for display.
    imshow("b",lab[2]); 
    waitKey(0);
    return 0;
}