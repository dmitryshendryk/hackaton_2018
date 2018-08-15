#include "distance_measure.hpp"

#define THRESH 30
#define MAX_THRESH 175

Mat distance_measure::get_Mat(rs2::frame* c_frame,rs2::depth_frame* d_frame)
{   
    Mat colormat=Mat(Size(1280, 720), CV_8UC3, (void*)c_frame->get_data(), Mat::AUTO_STEP).clone();  
    cvtColor(colormat, gray, COLOR_RGB2GRAY);
        
         
	int thresh_size = (100 / 4) * 2 + 1; //自适应二值化阈值
	adaptiveThreshold(gray, gray, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, thresh_size, thresh_size / 3);
	morphologyEx(gray, gray, MORPH_OPEN, Mat());
        
        
        GaussianBlur(gray, blur, Size(7,7), 0, 0);
        Canny(blur, edges, 50, 150,3);
        erode(edges,dst,element);
       

        findContours(edges, contours,hierarchy, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));



       // Mat drawing = Mat::zeros( edges.size(), CV_8UC3 );
        vector<Moments> mu(contours.size());
        vector<Point2f> mc( contours.size() );
        if (contours.size() != 0) {
        for( int i = 0; i< contours.size(); i++ )
        {
            if (contourArea(contours[i]) <= 500) 
                continue;
            mu[i] = moments(contours[i], false); 
            mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
            if (!(isnan(mc[i].x) || isnan(mc[i].y)))
            {   
                 dist_to_center = d_frame->get_distance(mc[i].x, mc[i].y);
            };
                
            // if (contourArea(contours[i]) <= 180 && contourArea(contours[i]) >= 0)
            //     continue;
            Scalar color = Scalar( 0,255,0 );
            drawContours( colormat, contours, i, color, 2, 8, hierarchy, 0, Point() );
            circle( colormat, mc[i], 4, color, -1, 8, 0 );
            putText(colormat,format("(%.2f)", dist_to_center), mc[i], FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255), 2);

        }
        }
    return colormat;
}


Mat distance_measure::get_corners(rs2::frame *c_frame)
{
    Mat colormat=Mat(Size(1280, 720), CV_8UC3, (void*)c_frame->get_data(), Mat::AUTO_STEP).clone(); 
    //Mat temp;
  
    return on_CornerHarris(colormat);

}

Mat distance_measure::on_CornerHarris(const Mat& input,int factor,void * par)
{
   	//vector<Point> output;
    Mat tempgray;
    cvtColor(input, tempgray, COLOR_RGB2GRAY);
    Mat dstImage;
	Mat normImage;
	Mat scaledImage;
    Mat colormat=input.clone();
	dstImage = Mat::zeros(tempgray.size(), CV_32FC1);
	//input = input.clone();
	cornerHarris(tempgray, dstImage, 2, 3, 0.04, BORDER_DEFAULT);
	normalize(dstImage, normImage, 0, 255, NORM_MINMAX, CV_32FC1);
	convertScaleAbs(normImage, scaledImage);
	int index=0;
    if(corners.size())
         corners.clear();
    for (int j = 0; j < normImage.rows; j++)
	{
		for (int i = 0; i<normImage.cols; i++)
		{
			if ((int)normImage.at<float>(j, i)>THRESH + 80)//only need to take the cordinate
			{
				Point p(j,i);
                corners.push_back(p);
                if(corners.size()>200)
                  return input.clone();
                circle( colormat, p, 4, Scalar(0,255,0), -1, 8, 0 );
			}
		}
			
	}
   if(corners.size()>1000)
   {
      imwrite("error.png",input);
   }
    return colormat;
}
