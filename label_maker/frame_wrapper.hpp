
#ifndef FRAME_WRAPPER
#define FRAME_WRAPPER

#include "librealsense2/rs.hpp"
#include "librealsense2/hpp/rs_sensor.hpp"
#include "../core/semantic/distance_measure.hpp"
#include <QObject>
class frame_wrapper:public QObject
{
Q_OBJECT
signals: void update_counter(uchar *,int ,int);
private : 
distance_measure dis;
rs2::frame *c_f;
rs2::depth_frame *d_f;
vector<Point> v_p;
void update_frame();//qDebug()<<f.get_timestamp();}

public : 
 frame_wrapper():dis(),c_f(nullptr),d_f(nullptr){}
 inline void set_corners(const vector<Point> v){if(v_p.size())v_p.clear();v_p=v;}
 //rs2::frame get_frame(){return f;}
public slots:
   inline  void get_depth(rs2::depth_frame f){if(d_f)delete d_f;d_f=new rs2::depth_frame(f);}
   inline  void get_color(rs2::frame f){if(c_f)delete c_f;c_f=new rs2::frame(f);update_frame();}
};

#endif
