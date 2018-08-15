#ifndef DEPTH_IMAGE_WRAPPER
#define DEPTH_IMAGE_WRAPPER
#include "control.hpp"

#include <QLabel>
#include <QThread>
#include <QImage>
#include <QMutex>
//this is necessary, to prevent calling Window.updat() in differ thread
static QMutex *mu=new QMutex();

class image_wrapper:public QObject{
    Q_OBJECT
public :
   image_wrapper(int m,QLabel * lebel);
  
public slots:
    void get_Frame(uchar * data,int width,int height);
    void get_Frame(QImage img);
private:
    QLabel *cur_img;
    QImage *img;
    QPixmap *tpx;
    int mark;


};
#endif
