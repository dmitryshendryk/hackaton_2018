#include "receive_wrapper.hpp"
#include <QImage>
#include <fstream>
#include <QStringList>

void ogl_receiever::get_data(std::vector<std::vector<float>> & vp,std::vector<cv::Mat>& vf)
{

   surface->points_data=&vp;
   surface->frames_data=&vf;
   surface->set_draw_type(POINT);
   surface->render();
   surface->set_draw_type(FRAME);
   surface->render();

   emit  new_data(surface->grabFramebuffer());
}
void ogl_receiever::draw_model()
{

   surface->set_draw_type(MODEL);
   surface->render();
   emit  new_data(surface->grabFramebuffer());




}
void ogl_receiever::load_model()
{
   surface->read_model_data();
   draw_model();
    
}

void ogl_receiever::trans(float x,float y)
{

   qDebug()<<"trans";
   surface->trans(x,y);
   draw_model();
}
void ogl_receiever::rotate(float x,float y)
{
   qDebug()<<"rotate";
   surface->rotate(x,y);
     draw_model();
}
void ogl_receiever::zoom(float s)
{
    qDebug()<<"zoom";
    surface->zoom(s);
    draw_model();
}
void ogl_receiever::auto_label()
{

    
    int rx=0,ry=0;
    while(rx<360)
     {
         surface->rotate(5,0);
         draw_model();
         rx+=5;
     }
   //  qDebug()<<"rx.....................ry...................."<<rx<<ry;
   
     while(ry<360)
    {
        surface->rotate(0,5);
        draw_model();
        ry+=5;
    }
     
}