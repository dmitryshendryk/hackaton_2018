#include <algorithm>            // std::min, std::max
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include "librealsense2/rs.hpp"
#include "librealsense2/hpp/rs_sensor.hpp"
#include <vector>
using namespace cv;
using namespace std;
//add corner detect using harris method
class distance_measure 
{
    private:
        Mat on_CornerHarris(const Mat& inputmat,int factor=0,void * par=nullptr);

    public:
        Mat get_Mat(rs2::frame *c_frame,rs2::depth_frame *d_frame);
        Mat get_corners(rs2::frame *c_frame); 
        inline vector<Point> get_corners_vector(){return corners;}
    private:
        float dist_to_center = 0.0;
        Mat gray;
        Mat blur;
        Mat edges;
        Mat dst;
        Mat element = getStructuringElement(cv::MORPH_CROSS,cv::Size(2 * 6 + 1, 2 * 6 + 1),cv::Point(6, 6) );
           
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        vector<Point> corners;     

};