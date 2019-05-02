#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat image, image_out;
    image=imread("/home/pranay/Final_Year_Project/human_detection/test1.jpg");
    cvtColor(image, image_out, CV_BGR2Lab);


Mat lab[3];   //destination array
split(image,lab);//split source  


Mat thresold;

namedWindow("My Window", WINDOW_NORMAL);
namedWindow("My Window2", WINDOW_NORMAL);
namedWindow("My Window3", WINDOW_NORMAL);
int m = 0;
createTrackbar("r1", "My Window", &m, 255);

//Create trackbar to change contrast
int n= 0;//108
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
int e= 0;//101
createTrackbar("b2", "My Window2", &e, 255);





while (true)
     {
           
          inRange(image_out, cv::Scalar(m, n, o), cv::Scalar(255 - q,255 - w,255 - e), thresold);

          //show the brightness and contrast adjusted image
          imshow("My Window", thresold);
          imshow("My Window2", image);  
          imshow("My Window3", image_out);

          int iKey = waitKey(50);

          //if user press 'ESC' key
          if (iKey == 27)
          {
               break;
          }
          
     }



    return 0;
}