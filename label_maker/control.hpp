#ifndef CONTROL_THREAD
#define CONTROL_THREAD
#undef foreach    //Qt doesn't support foreach(xxx) add this to fix the conflict
#include "librealsense2/rs.hpp"
#include "librealsense2/hpp/rs_sensor.hpp"
#include <librealsense2/rs_advanced_mode.hpp>


#include <QMutex>
#include <QThread>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QMutex>

#include <fstream>

using namespace std;


class control_Thread:public QThread
{
   Q_OBJECT
  signals: void Is_Connected();
           void New_color_data(uchar * ,int ,int );
           void New_depth_data(uchar * ,int ,int );
           void New_depth_data_colormap(uchar *,int ,int );
          
           void New_color_frame(rs2::frame );
           void New_depth_frame(rs2::depth_frame );

public:



     enum state{WATHCH=0,GET_IMG=1,GET_DEPTH=2,DEFAULT=3};

     control_Thread();
     inline void set_mark(state newstate){nowstate=newstate;}
     inline void start_watching(){qDebug()<<"start watching";nowstate=WATHCH;start();}
     inline void start_getting_IMG(){qDebug()<<"start getting image";nowstate=GET_IMG;start();}

     void up_load();

     uchar * get_color_data(){return color_data;}
     uchar * get_depth_data(){return depth_data;}

protected:
    virtual void run();

    rs2::sensor *depth_sensor,*color_sensor;
 private:
    state nowstate;
    rs2::context *ctx;
    rs2::config *cfg;
    rs2::device *dev;
    QObject  test;
    rs400::advanced_mode *advanced_mode_dev;
    rs2::frame *colorframe;
    rs2::depth_frame *depthframe;
    uchar * color_data,* depth_data;
};
#endif
