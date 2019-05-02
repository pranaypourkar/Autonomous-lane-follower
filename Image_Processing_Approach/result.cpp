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

// Input Quadilateral or Image plane coordinates
Point2f inputQuadPoints[4]; 
// Output Quadilateral or World plane coordinates
Point2f outputQuadPoints[4];
// Lambda Matrix
Mat lambda( 2, 4, CV_32FC1 );


//**********************************

void fitPoly(const vector <Point> &src,Mat &dst, int order){

try
{
    
    Mat src_x = Mat(src.size(), 1, CV_32FC1);
    Mat src_y = Mat(src.size(), 1, CV_32FC1);
    for (int i = 0; i < src.size(); i++){
        src_x.at<float>(i, 0) = (float)src[i].x;
        src_y.at<float>(i, 0) = (float)src[i].y;
    }

Mat X;
X = Mat::zeros(src_x.rows, order+1,CV_32FC1);
Mat copy;

//change type of copy
copy.convertTo(copy,CV_32FC1);

for(int i = 0; i <=order;i++)
{
copy = src_x.clone();
pow(copy,i,copy);


Mat M1;

//change type
M1.convertTo(M1,CV_32FC1);

M1 = X.col(i);
copy.col(0).copyTo(M1);
}
Mat X_t, X_inv;
X_inv.convertTo(X_inv,CV_32FC1);
X_t.convertTo(X_t,CV_32FC1);

transpose(X,X_t);

Mat temp;
temp.convertTo(temp,CV_32FC1); 
temp = X_t*X;
Mat temp2;
temp2.convertTo(temp2,CV_32FC1);
invert(temp,temp2);

Mat temp3;
temp3.convertTo(temp3,CV_32FC1); 

temp3 = temp2*X_t;

Mat W;
W.convertTo(W,CV_32FC1); 
W = temp3*src_y;

dst = W.clone();

}
catch (cv::Exception const & e) 
{ 
  //std::cerr<<"OpenCV exception: "<<e.what()<<std::endl; 
}

 }

int main(int argc, char* argv[])
{
  
     VideoCapture cap("/home/pranay/Final_Year_Project/video/test4.mp4"); // open the default camera
     //VideoCapture cap(0); // open the default camera
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
     inputQuadPoints[0] = Point2f( 0.25 * input.size().width,0.45 * input.size().height);
     inputQuadPoints[1] = Point2f( 0.75 * input.size().width,0.45 * input.size().height);
     inputQuadPoints[2] = Point2f( 0.1 * input.size().width,1 * input.size().height);
     inputQuadPoints[3] = Point2f( 0.9 * input.size().width,1 * input.size().height);  
     // The 4 points where the mapping is to be done , from top-left in clockwise order
     outputQuadPoints[0] = Point2f( 0,0 );
     outputQuadPoints[1] = Point2f( input.cols,0);
     outputQuadPoints[2] = Point2f( 0,input.rows);
     outputQuadPoints[3] = Point2f( input.cols,input.rows  );

     // Get the Perspective Transform Matrix i.e. lambda 
     lambda = getPerspectiveTransform( inputQuadPoints, outputQuadPoints );

     cout<<"\n"<<lambda<<"\n";

     Mat input_distorted;
   while (true)
     {
          
          cap >> input_distorted; 

          //if frame is empty then break loop
          if (input_distorted.empty())
          {
              break;
          }
          //undistort(input_distorted, input, intrinsic, distCoeffs);
          warpPerspective(input,output,lambda,output.size() );

          
          //put circle on input image
          circle(input, Point2f(0.25 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
          circle(input, Point2f(0.75 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
          circle(input, Point2f( 0.1 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
          circle(input, Point2f(0.9 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);

          //convert to LAB space
          cvtColor(output, image_lab, CV_BGR2Lab);

          inRange(image_lab, cv::Scalar(0, 108, 0), cv::Scalar(255,255,154), threshold_lab);

          
          ///****************test***********
          Mat tst;
          cvtColor( input, tst,CV_BGR2Lab);

          inRange(tst, Scalar(0, 108, 0), Scalar(255,255,154), tst);

          ///****************************

          bitwise_not ( threshold_lab, threshold_lab);

          int height = threshold_lab.size().height, width = threshold_lab.size().width;  
          
          //declare without size
          vector<int> ColumnSum;

          Mat histogram(height,width, CV_8UC1, Scalar(0));
        
          reduce(threshold_lab, ColumnSum, 0, CV_REDUCE_SUM, CV_32S);

          int lefty_max=0, righty_max=0,midpoint,leftx_index,rightx_index,nwindows=9,margin=150,minpix=1;
          midpoint = width/2;
 
          
          for(int i=0;i<ColumnSum.size();i++)
           {
    
              ColumnSum[i] = (ColumnSum[i]/255);
              //cout<<"columnSum[i] "<<columnSum[i]<<" i= "<<i<<"\n";
             if(ColumnSum[i] <1080 && i < 1920)
            {


                histogram.at<uchar>(ColumnSum[i], i, 0) = 255;
                circle(histogram, Point2f(i,ColumnSum[i]), 5, cv::Scalar(255, 255, 255), 5 , 8, 0);

                if(i<midpoint && ColumnSum[i]>lefty_max)
              {
                lefty_max = ColumnSum[i];
                leftx_index = i;
              }
              else if(i>=midpoint && ColumnSum[i]>righty_max)
              {
                righty_max = ColumnSum[i];
                rightx_index = i;
              }

            }

           }


          int window_height = height/nwindows;

          flip(histogram, histogram, 0);

          vector<Point> nonZeroCoordinates(height*width);

          // calculates position values x,y of non zero points in threshold_lab binary image
          findNonZero(threshold_lab, nonZeroCoordinates);
          //access pixel coordinates

          int leftx_current,rightx_current;

          leftx_current = leftx_index;
          rightx_current = rightx_index;
          
          //dont declare sizes of vector
          vector<Point> left_lane_inds;
          vector<Point> right_lane_inds;

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

   /*     if(draw_windows == true )
        {
          cv::rectangle(threshold_lab, Point(win_xleft_low,win_y_low), Point(win_xleft_high,win_y_high), Scalar(255,255,255), 1);
          cv::rectangle(threshold_lab, Point(win_xright_low,win_y_low), Point(win_xright_high,win_y_high), Scalar(255,255,255), 1);
        
        }
    */
        //region_window = input[win_y_low:win_y_high, win_xleft_low:win_xleft_high]

        //window left side non zero pixel    
        roi_left.x = win_xleft_low;
        roi_left.y = win_y_low;
        if(roi_left.x < 0)
        {
          roi_left.x = 0;
        }
      


        roi_left.width = win_xleft_high - win_xleft_low;  //= 2*margin
        roi_left.height = win_y_high - win_y_low;   //window height
  

        window_left = threshold_lab(roi_left); //select left window image in window_left


        //for storing non zero coordinates in left window
        vector<Point> nonZero_leftwindow(roi_left.width*roi_left.height);
        
        //get count of searched non zero coordinates in left window
        int count_left = countNonZero(window_left);

        
        if(count_left > 0)
         {
           //draw window when required and when count_left > 0
           if(draw_windows == true )
            {
            rectangle(threshold_lab, Point(win_xleft_low,win_y_low), Point(win_xleft_high,win_y_high), Scalar(255,255,255), 1);
            } 

           //finds non zero points in left window
           findNonZero(window_left, nonZero_leftwindow);

           //add initial position because window and image dimension are different
           for (int i = 0; i < nonZero_leftwindow.size(); ++i)
           {
            nonZero_leftwindow[i].x = nonZero_leftwindow[i].x + roi_left.x;
            nonZero_leftwindow[i].y = nonZero_leftwindow[i].y + roi_left.y;
           }


           //cout<<"initial left_lane_inds size: "<<left_lane_inds.size()<<"\n";
          // cout<<"left_lane_inds.end()"<<left_lane_inds.end()<<"\n";
          // cout<<"nonZero_leftwindow.begin()"<<nonZero_leftwindow.begin()<<"\n";
           //cout<<"nonZero_leftwindow.end()"<<nonZero_leftwindow.end()<<"\n";
           left_lane_inds.insert(left_lane_inds.end(),nonZero_leftwindow.begin(),nonZero_leftwindow.end());
          // cout<<"final left_lane_inds size: "<<left_lane_inds.size()<<"\n";
           //cout<<"nonZero_leftwindow size: "<<nonZero_leftwindow.size()<<"\n";
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
        //draw window when required and when count_right > 0
        if(draw_windows == true)
          {
          rectangle(threshold_lab, Point(win_xright_low,win_y_low), Point(win_xright_high,win_y_high), Scalar(255,255,255), 1); 
          } 

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




  //generate y values from 0 to height-1
   vector<int> y(input.size().height);
   for (int i = 0; i < input.size().height; i++)
   {
     y[i] = i;
   }


      // order of data matrix
  int fit_order = 2;


  // matrices to store computed coefficients
  Mat fit_weights_left(fit_order+1,1,CV_32FC1);


//  cout<<"left_lane_inds  "<<left_lane_inds.size()<<"\n"; 
  fitPoly(left_lane_inds,fit_weights_left,fit_order);
 
  // computed data printed here
  //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
  //cout<<"Weights of fit of order of X-data are: "<<fit_weights_left<<endl;
  // X = Ax + Bx * t ———–> This one is of first order (so 2 coefficients)
  //Y = Ay + By * t + Cy * t^2 ———–> This one is of second 
 // cout<<"left"<<endl;
 //cout <<"Ax is –> "<<fit_weights_left.at<float>(0,000)<<endl; // rounded coefficients
 //cout <<"Bx is –> "<<fit_weights_left.at<float>(1,000)<<endl;
 //cout <<"C is –> "<<fit_weights_left.at<float>(2,000) <<endl;

  //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;    




  // matrices to store computed coefficients
  Mat fit_weights_right(fit_order+1,1,CV_32FC1);


  //cout<<"right_lane_inds  "<<right_lane_inds.size()<<"\n"; 
  fitPoly(right_lane_inds,fit_weights_right,fit_order);
 
  // computed data printed here
  //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
  //cout<<"Weights of fit of order of Y-data are: "<<fit_weights_right<<endl;
  // X = Ax + Bx * t ———–> This one is of first order (so 2 coefficients)
  //Y = Ay + By * t + Cy * t^2 ———–> This one is of second 
  //cout<<"right"<<endl;
  //cout <<"Ax is –> "<<fit_weights_right.at<float>(0,000) <<endl; // rounded coefficients
  //cout <<"Bx is –> "<<fit_weights_right.at<float>(1,000) <<endl;
  //cout <<"C is –> "<<fit_weights_right.at<float>(2,000) <<endl;
  //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;




  double ym_per_pix =  0.74/286;//30.5/720; // meters per pixel in y dimension
  double xm_per_pix = 0.03/65;//3.7/720; // meters per pixel in x dimension

  ym_per_pix = 74/286;
  xm_per_pix = 03/65;


  //xm_per_pix = 0.74/286;
  double y_eval = height-1;

  vector<Point> left_fitx(height);
  vector<Point> right_fitx(height);


  //generate x values using  coefficients of left region
  // x = (a*y^2 + b*y + c)xm_per_pix


  //right curve
  for (int i = 0; i < height; ++i)
  {
   right_fitx[i].x = ( (fit_weights_right.at<float>(0,000)*pow(y[i],2)) + (fit_weights_right.at<float>(1,000)*y[i]) + fit_weights_right.at<float>(2,000) )*xm_per_pix;
  }

  //left curve
  for (int i = 0; i < height; ++i)
  {
   left_fitx[i].x= ( (fit_weights_left.at<float>(0,000)*pow(y[i],2)) + (fit_weights_left.at<float>(1,000)*y[i]) + fit_weights_left.at<float>(2,000) )*xm_per_pix;
  }



  //already computed
  //***************************
  //generate y values from 0 to height-1
     for (int i = 0; i < height; ++i)
    {
      right_fitx[i].y = y[i]*ym_per_pix;
      left_fitx[i].y = y[i]*ym_per_pix;
    }

  //******************************


   //again apply polyfit with real data

   //************************************************************************

   // matrices to store computed coefficients
   Mat fit_weights_left_real(fit_order+1,1,CV_32FC1);


   //left_fitx size
   //cout<<"left_fitx size "<<left_fitx.size()<<"\n";

   fitPoly(left_fitx,fit_weights_left_real,fit_order);
  // cout<<"**left\n"<<fit_weights_left_real<<"\n***\n";

 
   // computed data printed here
   //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
   //cout<<"Weights of fit of order of X-data real are: "<<fit_weights_left_real<<endl;
   // X = Ax + Bx * t ———–> This one is of first order (so 2 coefficients)
   //Y = Ay + By * t + Cy * t^2 ———–> This one is of second 
   //cout <<"Ax is –> "<<roundf(fit_weights.at<float>(0,000)) / 1000<<endl; // rounded coefficients
   //cout <<"Bx is –> "<<roundf(fit_weights.at<float>(1,000)) / 1000<<endl;
  // cout<<"left"<<endl;
   //cout <<"Ax is –> "<<fit_weights_left_real.at<float>(0,000) <<endl; // rounded coefficients
   //cout <<"Bx is –> "<<fit_weights_left_real.at<float>(1,000) <<endl;
   //cout <<"C is –> "<<fit_weights_left_real.at<float>(2,000) <<endl;
   //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;    




   // matrices to store computed coefficients
   Mat fit_weights_right_real(fit_order+1,1,CV_32FC1);

   //right_fitx size
   //cout<<"right_fitx size "<<right_fitx.size()<<"\n";

   fitPoly(right_fitx,fit_weights_right_real,fit_order);
   //cout<<"**right\n"<<fit_weights_right_real<<"\n***\n";
 
   // computed data printed here
   //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
   //cout<<"Weights of fit of order of Y-data real are: "<<fit_weights_right_real<<endl;
   // X = Ax + Bx * t ———–> This one is of first order (so 2 coefficients)
   //Y = Ay + By * t + Cy * t^2 ———–> This one is of second 
   //cout<<"right"<<endl;
   //cout <<"Ax is –> "<<fit_weights_right_real.at<float>(0,000) <<endl; // rounded coefficients
   //cout <<"Bx is –> "<<fit_weights_right_real.at<float>(1,000) <<endl;
   //cout <<"C is –> "<<fit_weights_right_real.at<float>(2,000) <<endl;
   //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;


   //************************************************************************



    // Calculate the new radii of curvature
   double left_curverad,right_curverad;
   //left_curverad = ((1 + )**1.5) / abs(2*fit_weights_left_real.at<float>(0,000));
   //right_curverad = ((1 + (2*fit_weights_right_real.at<float>(0,000)*y_eval*ym_per_pix + fit_weights_right_real.at<float>(1,000))**2)**1.5) / abs(2*fit_weights_right_real.at<float>(0,000));
   left_curverad = pow((1 + pow((2*fit_weights_left_real.at<float>(0,000)*y_eval*ym_per_pix + fit_weights_left_real.at<float>(1,000)) , 2)) , 1.5)/abs(2*fit_weights_left_real.at<float>(0,000));
   right_curverad = pow((1 + pow((2*fit_weights_right_real.at<float>(0,000)*y_eval*ym_per_pix + fit_weights_right_real.at<float>(1,000)) , 2)) , 1.5)/abs(2*fit_weights_right_real.at<float>(0,000));

   float slope_left,slope_right;
   //slope_left = (2*fit_weights_left_real.at<float>(0,000)*y_eval*ym_per_pix);
   //slope_right = (2*fit_weights_right_real.at<float>(0,000)*y_eval*ym_per_pix);

   //cout<<"slope left at point height-1 : "<<atan(slope_left)<<"\n";
   //cout<<"slope right at point height-1 : "<<atan(slope_right)<<"\n";
 /*  cout<<"***\n";
   cout<<"real radius of cur left : "<<left_curverad <<"\n";
   cout<<"a = "<<fit_weights_left_real.at<float>(0,000)<<" "<<"b = "<<fit_weights_left_real.at<float>(1,000)<<"c = "<<fit_weights_left_real.at<float>(2,000);
   cout<<"***\n";

   cout<<"////\n";
   cout<<"real radius of cur right : "<<right_curverad <<"\n";
   cout<<"a = "<<fit_weights_right_real.at<float>(0,000)<<" "<<"b = "<<fit_weights_right_real.at<float>(1,000)<<"c = "<<fit_weights_right_real.at<float>(2,000);
   cout<<"///\n";
*/
         namedWindow( "Display window1", WINDOW_NORMAL );// Create a window for display.
         imshow( "Display window1", input );// Show our image inside it. //WINDOW_AUTOSIZE

         namedWindow( "Display window2", WINDOW_NORMAL );// Create a window for display.
         imshow( "Display window2", tst );// Show our image inside it.

         namedWindow( "Display window3", WINDOW_NORMAL );// Create a window for display.
         imshow( "Display window3", threshold_lab );// Show our image inside it.

         namedWindow( "Display window4", WINDOW_NORMAL );// Create a window for display.
         imshow( "Display window4", histogram);// Show our image inside it.


       /*  //clear vectors
         columnSum.clear();
         sumCols.clear();
         nonZeroCoordinates.clear();
         left_lane_inds.clear();
         right_lane_inds.clear();

*/
          int iKey = waitKey(50);

          //if user press 'ESC' key
          if (iKey == 27)
          {
             
       /*        columnSum.clear();
               sumCols.clear();
               nonZeroCoordinates.clear();
               left_lane_inds.clear();
               right_lane_inds.clear();
               //nonZero_rightwindow.clear();
               //nonZero_leftwindow.clear();
         */      break;
          }
          
     }


    
    return 0;
}
