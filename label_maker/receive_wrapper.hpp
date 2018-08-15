#ifndef RECEIEVER_WRAPPER
#define RECEIEVER_WRAPPER
#include <QObject>
#include "openglwrapper.hpp"
#include <vector>
#include <QThread>
#include <QImage>

//this is is only for multithread signal slot mechanics 
class ogl_receiever:public QObject
{
Q_OBJECT
signals:
  void new_data(QImage);
  
public: 
  ogl_receiever(int height,int width){surface=new opengl_core();surface->set_size(height,width);} 
  ~ogl_receiever(){}
public slots:
 void  get_data( std::vector<std::vector<float>> & v,std::vector<cv::Mat>& vf);
 void  test_paint(){surface->set_draw_type(OTHER);surface->render(); emit  new_data(surface->grabFramebuffer());}
 void  load_model();
 void  trans(float,float);
 void  rotate(float,float);
 void  zoom(float);
 void  auto_label();
private:
  void draw_model();
  
  
  
  opengl_core * surface;
  int h,w;

};


#endif
