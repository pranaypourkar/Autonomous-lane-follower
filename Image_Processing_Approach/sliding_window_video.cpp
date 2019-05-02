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


//**********************************

    




int main(int argc, char* argv[])
{
  
     VideoCapture cap("/home/pranay/Final_Year_Project/video/test4.mp4"); // open the default camera
     if(!cap.isOpened())  // check if we succeeded
        return -1;

     cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);//640
     cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);//480


     Mat input, output, output_gray , threshold_lab, image_lab;


   while (true)
     {
          
          cap >> input; 
          
          

          //****************************************
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


          //******************************************

          
          //put circle on input image
          circle(input, Point2f(0.25 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
          circle(input, Point2f(0.75 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
          circle(input, Point2f( 0.1 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
          circle(input, Point2f(0.9 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);

          //convert to LAB space
          cvtColor(output, image_lab, CV_BGR2Lab);

          inRange(image_lab, cv::Scalar(0, 108, 0), cv::Scalar(255,255,154), threshold_lab);

          bitwise_not ( threshold_lab, threshold_lab);

          int height = threshold_lab.size().height, width = threshold_lab.size().width;  
          vector<int> columnSum(input.size().height);       
          vector<int> sumCols(width);
          Mat histogram(height,width, CV_8UC1, Scalar(0));
        
          reduce(threshold_lab, columnSum, 0, CV_REDUCE_SUM, CV_32S);

          int lefty_max=0, righty_max=0,midpoint,leftx_index,rightx_index,nwindows=9,margin=150,minpix=1;
          midpoint = width/2;
 

//cout<<columnSum.size()<<"\n";

          
          for(int i=0;i<columnSum.size();i++)
            {
    
              columnSum[i] = (columnSum[i]/255);
                histogram.at<uchar>(columnSum[i], i, 0) = 255;

                if(i<midpoint && columnSum[i]>lefty_max)
              {
                lefty_max = columnSum[i];
                leftx_index = i;
              }
              else if(i>=midpoint && columnSum[i]>righty_max)
              {
                righty_max = columnSum[i];
                rightx_index = i;
              }
              //cout<<columnSum[i]<<" ";

            }


          //lefty_max = height - lefty_max;
          //righty_max = height - righty_max;

          //cout<<"\n"<<leftx_index<<" "<<lefty_max;
          //cout<<"\n"<<rightx_index<<" "<<righty_max;

          int window_height = height/nwindows;

          flip(histogram, histogram, 0);

          vector<Point> nonZeroCoordinates(height*width);
          findNonZero(threshold_lab, nonZeroCoordinates);
          //access pixel coordinates
          //Point pnt = locations[i];

          int leftx_current,rightx_current;

          leftx_current = leftx_index;
          rightx_current = rightx_index;

          vector<Point> left_lane_inds(height*midpoint);
          vector<Point> right_lane_inds(height*midpoint);

          int win_y_low, win_y_high,win_xleft_low,win_xleft_high,win_xright_low,win_xright_high, last_mean_right=rightx_index,last_mean_left=leftx_index,test; 
          bool draw_windows = true;
          Mat window_left,window_right;
          Rect roi_left,roi_right;



    for(int window=0;window<nwindows;window++)
      {
        // Identify window boundaries in x and y (and right and left)
        win_y_low = height - (window+1)*window_height;
        win_y_high = height - window*window_height;
        win_xleft_low = leftx_current - margin;
        win_xleft_high = leftx_current + margin;
        win_xright_low = rightx_current - margin;
        win_xright_high = rightx_current + margin;

        if(draw_windows == true)
        {
          cv::rectangle(threshold_lab, Point(win_xleft_low,win_y_low), Point(win_xleft_high,win_y_high), Scalar(255,255,255), 1);
          cv::rectangle(threshold_lab, Point(win_xright_low,win_y_low), Point(win_xright_high,win_y_high), Scalar(255,255,255), 1);
        
        }

        //region_window = input[win_y_low:win_y_high, win_xleft_low:win_xleft_high]

        //window left side non zero pixel    
        roi_left.x = win_xleft_low;
        roi_left.y = win_y_low;
        if(roi_left.x < 0)
        {
          roi_left.x = 0;
        }
      


        roi_left.width = win_xleft_high - win_xleft_low;
        roi_left.height = win_y_high - win_y_low;
  

        window_left = threshold_lab(roi_left);

        vector<Point> nonZero_leftwindow(roi_left.width*roi_left.height);

        int count_left = countNonZero(window_left);

        
        if(count_left > 0)
         {
           findNonZero(window_left, nonZero_leftwindow);
           //add initial position because window and image dimension are different
           for (int i = 0; i < nonZero_leftwindow.size(); ++i)
           {
            nonZero_leftwindow[i].x = nonZero_leftwindow[i].x + roi_left.x;
            nonZero_leftwindow[i].y = nonZero_leftwindow[i].y + roi_left.y;

           }



           left_lane_inds.insert(left_lane_inds.end(),nonZero_leftwindow.begin(),nonZero_leftwindow.end());
         }


        //window right side non zero
        roi_right.x = win_xright_low;
        roi_right.y = win_y_low;
        if(roi_right.x > width)
        {
          roi_right.x = width ;
        }
      
        roi_right.width = win_xright_high - win_xright_low;
        roi_right.height = win_y_high - win_y_low;


        //*******************
        //change roi_right.x  if required

        if(roi_right.x + roi_right.width > width)
        {
          roi_right.x  = width - roi_right.width;
        }

      //cout<<"roi_right.x "<< roi_right.x <<"roi_right.y" <<roi_right.y <<"\n";
      //cout<<"roi_right.width"<< roi_right.width <<"roi_right.height " <<roi_right.height<<"\n";



      //******************
      

      window_right = threshold_lab(roi_right);

      vector<Point> nonZero_rightwindow(roi_right.width * roi_right.height);

      int count_right = countNonZero(window_right);
      //cout<<"count_left "<<count_right;
      if(count_right > 0)
       {
        findNonZero(window_right, nonZero_rightwindow);
        //*****************

        for (int i = 0; i < nonZero_rightwindow.size(); ++i)
         {
          nonZero_rightwindow[i].x = nonZero_rightwindow[i].x + roi_right.x;
          nonZero_rightwindow[i].y = nonZero_rightwindow[i].y + roi_right.y;

         }


        right_lane_inds.insert(right_lane_inds.end(),nonZero_rightwindow.begin(),nonZero_rightwindow.end());
       }
    

      if(left_lane_inds.size() > minpix)
       {
        leftx_current = int(mean( nonZero_leftwindow)[0]); 
        if (leftx_current==0)
        {
          leftx_current = last_mean_left;
        }
        else
        {
          last_mean_left = leftx_current;
        }
        //cout<<"mean left "<< int(mean( nonZero_leftwindow)[0])<<"\n";

       }
     if(right_lane_inds.size() > minpix)
       {
        rightx_current = int(mean( nonZero_rightwindow)[0]); 
        if (rightx_current==0)
        {
          rightx_current = last_mean_right;
        }
        else
        {
          last_mean_right = rightx_current;
        }
        //cout<<"mean right "<< int(mean( nonZero_rightwindow)[0])<<"\n";
        //cout<<"mean right "<< rightx_current<<"\n";

       }

      }


         namedWindow( "Display window1", WINDOW_NORMAL );// Create a window for display.
         imshow( "Display window1", input );// Show our image inside it. //WINDOW_AUTOSIZE

         //namedWindow( "Display window2", WINDOW_NORMAL );// Create a window for display.
         //imshow( "Display window2", output_gray );// Show our image inside it.

         namedWindow( "Display window3", WINDOW_NORMAL );// Create a window for display.
         imshow( "Display window3", threshold_lab );// Show our image inside it.

         namedWindow( "Display window4", WINDOW_NORMAL );// Create a window for display.
         imshow( "Display window4", histogram);// Show our image inside it.


          int iKey = waitKey(50);

          //if user press 'ESC' key
          if (iKey == 27)
          {
               break;
          }
          
     }

    // Set the lambda matrix the same type and size as input
/*    




  
  


    





    namedWindow( "Display window1", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window1", input );// Show our image inside it.

    namedWindow( "Display window2", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window2", output_gray );// Show our image inside it.

    namedWindow( "Display window3", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window3", gray_threshold );// Show our image inside it.

    namedWindow( "Display window4", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window4", histogram);// Show our image inside it.

    
      /// Display
*/

    waitKey(0);
    return 0;
}
