#include "frame_wrapper.hpp"
#include <QDebug>
//combine here
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
void frame_wrapper::update_frame()
{
   if(c_f&&d_f){
   Mat tempmat=dis.get_Mat(c_f,d_f);
  // Mat tempmat1=dis.get_corners(c_f);
//    qDebug()<<"size:"<<dis.get_corners_vector().size();

   emit update_counter(tempmat.data,tempmat.size().width ,tempmat.size().height );
   }
   else
    return;
}
