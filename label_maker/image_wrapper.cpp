#include "image_wrapper.hpp"
image_wrapper::image_wrapper(int m, QLabel *label):QObject(),mark(m),cur_img(label),img(new QImage),tpx(new QPixmap)
{


}
void image_wrapper::get_Frame(uchar *data,int width,int height)
{
    //if(img) delete img;
    mu->lock();
    if(!data)
        qDebug()<<(mark?"depth image empty pointer ":"color image empty pointer");
    img=new QImage(data,width,height,QImage::Format_RGB888);
    if(tpx)delete tpx;
    QPixmap px;
    tpx=new QPixmap(px.fromImage(*img));
    cur_img->setPixmap(*tpx);
    mu->unlock();


}
void image_wrapper::get_Frame(QImage img)
{
  mu->lock();
    qDebug()<<"update img ttttttttttttttttttttttttttttttttttt";
    if(tpx)delete tpx;
    QPixmap px;
    tpx=new QPixmap(px.fromImage(img));
    cur_img->setPixmap(*tpx);
   
  mu->unlock();  
}