#ifndef SLAM_WRAPPER
#define SLAM_WRAPPER



/*
 *
 *
 *
 * DOn't change the order to include
 *
 *
 *
 *
 *
*/
#include <librealsense2/hpp/rs_internal.hpp>
#include "librealsense2/rs.hpp"
#include "librealsense2/hpp/rs_processing.hpp"
#include "librealsense2/h/rs_types.h"
#include "librealsense2/hpp/rs_sensor.hpp"
#include "librealsense2/hpp/rs_device.hpp"
#include "librealsense2/hpp/rs_frame.hpp"
#include <librealsense2/rs_advanced_mode.hpp>

#include "../core/slam/include/System.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>

#include <cstring>
#include <thread>
#include <string>
#include <streambuf>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <functional>
#include <QDebug>
#include <QMessageBox>

#include <QObject>
#include <QMutex>

using namespace cv;
class slam_wrapper:public QObject{
Q_OBJECT
signals: void ini_finished();
         void update_tracking(uchar *,int ,int );
         void send_mat(uchar*,uchar*,int ,int );
         void tracking_data(std::vector<std::vector<float>>& ,std::vector<cv::Mat>&);//
         void awake();
private:
   ORB_SLAM2::System *system;
   QString voc,tum;
   double index;
   uchar *color_data,*depth_data;
   int width=0,height=0;
   rs2::frame *c_f;
   rs2::depth_frame *d_f;
public:
    slam_wrapper(QString v,QString t):QObject(),voc(v),tum(t){index=0.f;color_data=nullptr;depth_data=nullptr;}
    void test();
    void trigger(){emit awake();}
public slots:
   inline void save_pcl(){system->save_pcl();QMessageBox box;box.setText("pointcloud saved");box.exec();}
   void update(uchar *color_data,uchar *depth_data);
   void update_frame();
     void ini();

   void get_color_data(uchar *,int ,int );
   void get_depth_data(uchar *,int ,int );

   inline  void get_depth(rs2::depth_frame f){if(d_f)delete d_f;d_f=new rs2::depth_frame(f);}
   inline  void get_color(rs2::frame f){if(c_f)delete c_f;c_f=new rs2::frame(f);update_frame();}







};




#endif
