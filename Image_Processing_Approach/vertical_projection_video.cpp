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


   void fitPoly(const vector <Point> &src,Mat &dst, int order){
    Mat src_x = Mat(src.size(), 1, CV_32F);
    Mat src_y = Mat(src.size(), 1, CV_32F);
    for (int i = 0; i < src.size(); i++){
        src_x.at<float>(i, 0) = (float)src[i].x;
        src_y.at<float>(i, 0) = (float)src[i].y;
    }
     //cvPolyfit(src_x, src_y, order);
//******************

Mat X;
X = Mat::zeros(src_x.rows, order+1,CV_32FC1);
Mat copy;
for(int i = 0; i <=order;i++)
{
copy = src_x.clone();
pow(copy,i,copy);
Mat M1 = X.col(i);
copy.col(0).copyTo(M1);
}
Mat X_t, X_inv;
transpose(X,X_t);
Mat temp = X_t*X;
Mat temp2;
invert (temp,temp2);
Mat temp3 = temp2*X_t;
Mat W = temp3*src_y;


//cout<<"SRC_X: "<<src_x<<endl;
//cout<<"SRC_Y: "<<src_y<<endl;
//cout<<"X: "<<X<<endl;
//cout<<"X_T: "<<X_t<<endl;
//cout<<"W:"<<W<<endl;
dst = W.clone();
    //*******************
 }



 



int main(int argc, char* argv[])
{
   input = imread("/home/pranay/Final_Year_Project/code/test.png", CV_LOAD_IMAGE_COLOR);




   //input.set(CV_CAP_PROP_FRAME_WIDTH,640);//640
   //input.set(CV_CAP_PROP_FRAME_HEIGHT,480);//480

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
    circle(input, Point2f(0.25 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
    circle(input, Point2f(0.75 * input.size().width,0.45 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
    circle(input, Point2f( 0.1 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);
    circle(input, Point2f(0.9 * input.size().width,1 * input.size().height), 5, cv::Scalar(0, 0, 255), 1 , 8, 0);


    //giving error
    //GaussianBlur( output, output, Size(3,3), 0, 0, BORDER_DEFAULT );
    cvtColor( output, output_gray, COLOR_BGR2GRAY );

    Mat gray_threshold;
    std::vector<int> columnSum(800);    

    //threshold(output_gray, gray_threshold, 197, 200, THRESH_BINARY);
    inRange(output, Scalar(0, 0, 0), Scalar(255,255,213), gray_threshold);

    int height = gray_threshold.size().height, width = gray_threshold.size().width;
    std::vector<int> sumCols(width);
    Mat histogram(height,width, CV_8UC1, Scalar(0));


    reduce(gray_threshold, columnSum, 0, CV_REDUCE_SUM, CV_32S);

	int lefty_max=0, righty_max=0,midpoint,leftx_index,rightx_index,nwindows=9,margin=150,minpix=1;
    midpoint = width/2;


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

    std::vector<Point> nonZeroCoordinates;
    findNonZero(gray_threshold, nonZeroCoordinates);
    //access pixel coordinates
    //Point pnt = locations[i];

    int leftx_current,rightx_current;

    leftx_current = leftx_index;
    rightx_current = rightx_index;

    std::vector<Point> left_lane_inds;
    std::vector<Point> right_lane_inds;

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
        	cv::rectangle(output_gray, Point(win_xleft_low,win_y_low), Point(win_xleft_high,win_y_high), Scalar(255,255,255), 1);
        	cv::rectangle(output_gray, Point(win_xright_low,win_y_low), Point(win_xright_high,win_y_high), Scalar(255,255,255), 1);
        
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
  

		window_left = gray_threshold(roi_left);

		std::vector<Point> nonZero_leftwindow;

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
    	if(roi_right.x > gray_threshold.size().width)
    	{
    		roi_right.x = gray_threshold.size().width  ;
    	}
    	
    	roi_right.width = win_xright_high - win_xright_low;
    	roi_right.height = win_y_high - win_y_low;


    	//*******************
    	//change roi_right.x  if required

    	if(roi_right.x + roi_right.width > gray_threshold.size().width)
    	{
    		roi_right.x  = gray_threshold.size().width - roi_right.width;
    	}

    	//cout<<"roi_right.x "<< roi_right.x <<"roi_right.y" <<roi_right.y <<"\n";
    	//cout<<"roi_right.width"<< roi_right.width <<"roi_right.height " <<roi_right.height<<"\n";



    	//******************
    	

    	window_right = gray_threshold(roi_right);

    	std::vector<Point> nonZero_rightwindow;

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

/*

// order of data matrix
int fit_order = 2;


// matrices to store computed coefficients
Mat fit_weights_left(fit_order+1,1,CV_32FC1);


cout<<"left_lane_inds  "<<left_lane_inds.size(); 
fitPoly(left_lane_inds,fit_weights_left,fit_order);
 
// computed data printed here
cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
cout<<"Weights of fit of order of X-data are: "<<fit_weights_left<<endl;
// X = Ax + Bx * t ———–> This one is of first order (so 2 coefficients)
//Y = Ay + By * t + Cy * t^2 ———–> This one is of second 
//cout <<"Ax is –> "<<roundf(fit_weights.at<float>(0,000)) / 1000<<endl; // rounded coefficients
//cout <<"Bx is –> "<<roundf(fit_weights.at<float>(1,000)) / 1000<<endl;
cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;    




// matrices to store computed coefficients
Mat fit_weights_right(fit_order+1,1,CV_32FC1);


cout<<"right_lane_inds  "<<right_lane_inds.size(); 
fitPoly(right_lane_inds,fit_weights_right,fit_order);
 
// computed data printed here
cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
cout<<"Weights of fit of order of X-data are: "<<fit_weights_right<<endl;
// X = Ax + Bx * t ———–> This one is of first order (so 2 coefficients)
//Y = Ay + By * t + Cy * t^2 ———–> This one is of second 
//cout <<"Ax is –> "<<roundf(fit_weights.at<float>(0,000)) / 1000<<endl; // rounded coefficients
//cout <<"Bx is –> "<<roundf(fit_weights.at<float>(1,000)) / 1000<<endl;
cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;

*/



    namedWindow( "Display window1", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window1", input );// Show our image inside it.

    namedWindow( "Display window2", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window2", output_gray );// Show our image inside it.

    namedWindow( "Display window3", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window3", gray_threshold );// Show our image inside it.

    namedWindow( "Display window4", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window4", histogram);// Show our image inside it.

    
      /// Display


    waitKey(0);
    return 0;
}
