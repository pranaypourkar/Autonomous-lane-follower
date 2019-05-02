#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{    
    

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);//640
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);//480
    


   


//Mat lab[3];   
Mat threshold, frame,image_out;

namedWindow("My Window", WINDOW_NORMAL);
namedWindow("My Window2", WINDOW_NORMAL);
namedWindow("My Window3", WINDOW_NORMAL);



while (true)
     {
          
          cap >> frame; // get a new frame from camera
          cvtColor(frame, image_out, CV_BGR2Lab);
          //split(image,lab);//split source  


          inRange(image_out, cv::Scalar(0, 108, 0), cv::Scalar(255,255,154), threshold);

          //show the brightness and contrast adjusted image
          imshow("My Window", threshold);
          imshow("My Window2", frame);  
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