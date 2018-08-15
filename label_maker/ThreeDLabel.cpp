#include "ThreeDLabel.hpp"
#include <QDebug>
#define TRANS_UNIT 0.005
#define ROTATE_UNIT 0.5
#define SCALAR_UNIT 0.005
void ThreeDLabel::mousePressEvent(QMouseEvent *ev)
{
   if(ev->button()==Qt::LeftButton)
       nowstate=TRANS;
   else if(ev->button()==Qt::RightButton)
      nowstate=ROTATE;
   else 
     return;
}

void ThreeDLabel::mouseReleaseEvent(QMouseEvent *ev)
{
  nowstate=NONE;
  previous_x=previous_y=previous_rx=previous_ry=-1;
  qDebug()<<"mouse realse";
}

void ThreeDLabel::mouseMoveEvent(QMouseEvent *ev)
{
   
   if(nowstate==TRANS)
   {
     float or_x,or_y;
     if(previous_x!=-1&&previous_y!=-1)
         {
            or_x=ev->x()>previous_x?TRANS_UNIT:(-1*TRANS_UNIT);
            or_y=ev->y()<previous_y?TRANS_UNIT:(-1*TRANS_UNIT);
           
            emit  trans(or_x,or_y);    
         }
     previous_x=ev->x();
     previous_y=ev->y();
   }
   else if(nowstate==ROTATE)
   {

     float or_x,or_y;
     if(previous_rx!=-1&&previous_ry!=-1)
         {
            or_x=ev->x()>previous_rx?ROTATE_UNIT:-1*ROTATE_UNIT;
            or_y=ev->y()<previous_ry?ROTATE_UNIT:-1*ROTATE_UNIT;  
            emit rotate(or_x,or_y);   
         }
     previous_rx=ev->x();
     previous_ry=ev->y();

   }
   else
     return;

}
void ThreeDLabel::wheelEvent(QWheelEvent *ev)
{
  float or_s;
  or_s= ev->delta() >0?SCALAR_UNIT:(-1*SCALAR_UNIT);
    // qDebug()<<"wheel"<<ev->delta();
     emit zoom(or_s);
  
 

}