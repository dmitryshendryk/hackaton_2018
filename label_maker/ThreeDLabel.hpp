#ifndef THREEDLABEL
#define THREEDLABEL
#include <QLabel>
#include <QMouseEvent>
class ThreeDLabel:public QLabel{
Q_OBJECT
signals:
  void  trans(float ,float );
  void  rotate(float ,float );
  void  zoom(float);
public :
      ThreeDLabel(QWidget *parent=0):QLabel(parent){}
      ~ThreeDLabel(){}
protected :
   	void mouseReleaseEvent(QMouseEvent * ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
private:
    enum shift{TRANS=0,ROTATE,NONE};
    shift nowstate=NONE;
    
    int previous_x=-1,previous_y=-1;
    int previous_rx=-1,previous_ry=-1;
    int previous_zx=-1;
};




#endif