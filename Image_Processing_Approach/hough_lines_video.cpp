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


int value;
int threshold_type=3 , threshold_count=179;
Mat src_rgb,src_gray,src_thresh, detected_edges, detected_edges_rgb, src_gray_blur;
int ratio = 3;
int kernel_size = 3;
int canny_lowThreshold=100;


int main(int argc, char* argv[])
{
      // VideoCapture cap(1);
       VideoCapture cap("/home/pranay/Final_Year_Project/video/test4.mp4"); // open the default camera
     if(!cap.isOpened())  // check if we succeeded
        return -1;

     cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);//640
     cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);//480


     Mat input, output, output_gray , threshold_lab, image_lab;
  

     //for initialization
     cap >> input; 
     int height = input.size().height, width = input.size().width;
     cout<<"height = "<<height<<" width = "<<width;



     //get the perspective transform matrix
     lambda = Mat::zeros( input.rows, input.cols, input.type() );
     // The 4 points that select quadilateral on the input , from top-left in clockwise order
     // These four pts are the sides of the rect box used as input 
     inputQuad[0] = Point2f( 0.2 * input.size().width,0.45 * input.size().height);
     inputQuad[1] = Point2f( 0.80 * input.size().width,0.45 * input.size().height);
     inputQuad[2] = Point2f( 0.1 * input.size().width,0.9 * input.size().height);
     inputQuad[3] = Point2f( 0.9 * input.size().width,0.9 * input.size().height);  
     // The 4 points where the mapping is to be done , from top-left in clockwise order
     outputQuad[0] = Point2f( 0,0 );
     outputQuad[1] = Point2f( input.cols,0);
     outputQuad[2] = Point2f( 0,input.rows);
     outputQuad[3] = Point2f( input.cols,input.rows  );

     //cout<<input.size().height;
     //cout<<input.size().width;

     // Get the Perspective Transform Matrix i.e. lambda 
     lambda = getPerspectiveTransform( inputQuad, outputQuad );


     while (true)
     {
          
          cap >> input; 

          //cout<<"\n"<<loop++<<"\n";

          //if frame is empty then break loop
          if (input.empty())
          {
              break;
          }

          warpPerspective(input,output,lambda,output.size() );


          //******************************************

          
          //put circle on input image
          circle(input, Point2f(0.20 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 10 , 8, 0);
          circle(input, Point2f(0.80 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 10 , 8, 0);
          circle(input, Point2f( 0 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 10 , 8, 0);
          circle(input, Point2f(1 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 10 , 8, 0);


		   //convert to LAB space
          cvtColor(output, image_lab, CV_BGR2Lab);

          inRange(image_lab, cv::Scalar(0, 108, 0), cv::Scalar(255,255,154), threshold_lab);

          /// Canny detector
		  Canny( threshold_lab, detected_edges, canny_lowThreshold, canny_lowThreshold*ratio, kernel_size );
		  /// Using Canny's output as a mask, we display our result

          //cout << detected_edges;

		 detected_edges_rgb = Scalar::all(0);

		 output.copyTo( detected_edges_rgb, detected_edges); // here detected_edges is an binary image. It is mapped to pixels in source rgb image


	//ROI
	//Mat src_ROI (detected_edges, Rect(src_rgb.rows/4, src_rgb.cols/8, (3*src_rgb.cols)/4, src_rgb.rows/2) ); //length=cols/2 breadth=rows/4
	//(x,y,width,height)

	//hough lines
		vector<Vec2f> lines;
		HoughLines(detected_edges, lines, 1, CV_PI/180, 140, 0, 0 );

		//cout<<lines.size() <<" " ;
        
        float angle_degree=0 ,theta_degree;
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
        line( output, pt1, pt2, Scalar(255,0,255), 3);
        theta_degree = theta*(180/CV_PI);

        if (theta_degree>90)
        {
        	theta_degree = theta_degree - 180;
        }

        angle_degree =  angle_degree + theta_degree;
        //cout<<"angle in degree = "<<angle<<endl;
        }
        angle_degree = angle_degree/lines.size();

        cout<<"angle = "<<angle_degree<<endl;



        int count=0;


          
          namedWindow( "Display window1", WINDOW_NORMAL );// Create a window for display.
          imshow( "Display window1", input );// Show our image inside it. //WINDOW_AUTOSIZE

          namedWindow( "Display window2", WINDOW_NORMAL );// Create a window for display.
          imshow( "Display window2", output );// Show our image inside it. //WINDOW_AUTOSIZE

          namedWindow( "Display window3", WINDOW_NORMAL );// Create a window for display.
          imshow( "Display window3", threshold_lab );// Show our image inside it.  

          namedWindow( "window4", WINDOW_NORMAL );// Create a window for display.
          imshow( "window4",detected_edges);// Show our image inside it.
          
          namedWindow("window5", WINDOW_NORMAL);
          imshow( "window5",image_lab);// Show our image inside it.        
 
 		  int iKey = waitKey(50);

          //if user press 'ESC' key
          if (iKey == 27)
          {

          break;
          }
          
        


    
   
	}


return 0;
}



