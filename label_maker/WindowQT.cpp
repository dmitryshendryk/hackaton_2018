
#include "WindowQT.hpp"
#include <QTableWidgetItem>
#include <QHeaderView> 
#include <QDebug>
#include <QMessageBox>
#include <QDir>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)  
{
  this->resize(1000,640);
  
//   imagearea=new QLabel(this);
//   imagearea->setGeometry(20,10,840,480);
//   imagearea->setStyleSheet("border: 4px solid  black" );
//   imagearea->setFrameStyle(QFrame::Box);
  
  modelarea=new ThreeDLabel(this);
  modelarea->setGeometry(20,10,840,480);
  modelarea->setStyleSheet("border: 4px solid  black" );
  modelarea->setFrameStyle(QFrame::Box);


  start_pt=new QPushButton(this);
  start_lb=new QPushButton(this);
  add_bt=new QPushButton(this);
  save_button=new QPushButton(this);
  read_button=new QPushButton(this);
  bt_load=new QPushButton(this);
  bt_mdlabel=new QPushButton(this);

  bt_load->setGeometry(20,560,90,30);
  bt_load->setText("load model");
  bt_mdlabel->setGeometry(20,600,90,30);
  bt_mdlabel->setText("label model");


  start_pt->setGeometry(140,560,90,30);
  start_pt->setText("start pcd");

  name_line=new QLineEdit(this);
  name_line->setGeometry(570,510,90,30);  
  
  add_bt->setGeometry(570,560,90,30);
  add_bt->setText("add object");
  start_lb->setGeometry(240,560,90,30);
  start_lb->setText("start label");
  save_button->setGeometry(870,560,90,30);
  save_button->setText("save script");
  read_button->setGeometry(870,610,90,30);
  read_button->setText("read script");
  
   QThread * model_thread=new QThread();
   model_rec =new ogl_receiever(modelarea->height(),modelarea->width());
   model_rec->moveToThread(model_thread);
   model_thread->start();


  object_display_zone=new QTableWidget(this);
  object_display_zone->setGeometry(870,10,140,480);
  object_display_zone->setColumnCount(2);
  QStringList header;  
  header<<"NAME"<<"NUM";
  object_display_zone->setHorizontalHeaderLabels(header);
  object_display_zone->horizontalHeader()->setDefaultSectionSize(70); 
  
  connect(read_button,SIGNAL(clicked()),this,SLOT(read_data()));
  connect (start_pt,SIGNAL(clicked()),this,SLOT(ini_pcl()));
  connect(bt_load,SIGNAL(clicked()),model_rec,SLOT(load_model()),Qt::QueuedConnection);
  connect(this,SIGNAL(test_paint()),model_rec,SLOT(test_paint()),Qt::QueuedConnection);
  connect(model_rec,SIGNAL(new_data(QImage)),this,SLOT(updata_model(QImage)),Qt::QueuedConnection);
  connect(modelarea,SIGNAL(trans(float,float)),model_rec,SLOT(trans(float,float)),Qt::QueuedConnection);
  connect(modelarea,SIGNAL(rotate(float,float)),model_rec,SLOT(rotate(float,float)),Qt::QueuedConnection);
  connect(modelarea,SIGNAL(zoom(float)),model_rec,SLOT(zoom(float)),Qt::QueuedConnection);
  connect(bt_mdlabel,SIGNAL(clicked()),model_rec,SLOT(auto_label()),Qt::QueuedConnection);
  emit test_paint();
  
}
void MainWindow::ini_pcl()
{
    QThread *tr=new QThread();
    lb=new label_maker();
    lb->moveToThread(tr);
    tr->start();
    connect(this,SIGNAL(trigger()),lb,SLOT(load()),Qt::QueuedConnection);
    connect(lb,SIGNAL(newtotal(int)),this,SLOT(update_num(int)),Qt::QueuedConnection);
    connect(add_bt,SIGNAL(clicked()),this,SLOT(add_object()));
    connect(start_lb,SIGNAL(clicked()),this,SLOT(start_labeling()));
    
    connect(this,SIGNAL(shut_down()),lb,SLOT(shut_down_viewer()),Qt::DirectConnection);
    emit trigger();
    
}

void MainWindow::add_object()
{
  qDebug()<<"a";
  QString obj_name=name_line->text();
  if(!obj_name.size())
      return;
  name_line->clear();
  object_display_zone->insertRow(object_display_zone->rowCount());
  QTableWidgetItem *t_name,*t_number;
  t_name=new QTableWidgetItem (obj_name);
  t_number=new QTableWidgetItem("0");
  object_display_zone->setItem(object_display_zone->rowCount()-1,0,t_name);
  object_display_zone->setItem(object_display_zone->rowCount()-1,1,t_number);
  //core->new_object(obj_name);
  names.push_back(obj_name);
  lb->reset();
}


void MainWindow::start_labeling()
{
   if(!lb->coor.size()){
         QMessageBox box;
         box.setText("No object has been selected!!!");box.exec();
         return;
   }
   vector<vector<float *>> outputdata;
   for(int i=0;i<lb->coor.size();i++)
   {
     vector<float *> temp=(lb->coor)[i].to_vec();
     qDebug()<<i<<"has"<<temp.size()<<"points";
     outputdata.push_back(temp);
   }
   qDebug()<<"total"<<outputdata.size()<<"objects\nstarting slam";
   emit shut_down();
   QThread * slamthread=new QThread();
   core=new slam_core();
   core->moveToThread(slamthread);
   core->ini_slam("ORBvoc.txt","Realsense_Tum.yaml");
   core->set_data(outputdata);
   core->set_obj_name(names);
   slamthread->start();
   connect(save_button,SIGNAL(clicked()),core,SLOT(read_data_out()),Qt::DirectConnection);
   connect(this,SIGNAL(start_slam()),core,SLOT(start_labeling()),Qt::QueuedConnection);
   connect(core,SIGNAL(new_image(Mat)),this,SLOT(update_pic(Mat)),Qt::BlockingQueuedConnection);
   
   emit start_slam();
}
void MainWindow::update_pic(Mat m)
{
    QImage img (m.data,m.cols,m.rows,QImage::Format_RGB888);

    QPixmap px;
    QPixmap tpx (px.fromImage(img));
    // tpx.scaled( imagearea->size(), Qt::KeepAspectRatio);
    // imagearea->setScaledContents(true);
    // imagearea->setPixmap(tpx); 


}
void MainWindow::updata_model(QImage m)
{
    qDebug()<<"update image";
    m.save(QString("model%1.jpg").arg(index));
    index++;
    QPixmap px;
    QPixmap tpx (px.fromImage(m));
    
    
    
    
    //tpx.scaled( imagearea->size(), Qt::KeepAspectRatio);
    
    //modelarea->setScaledContents(true);
    modelarea->setPixmap(tpx); 
}
void MainWindow::update_num(int n)
{
   object_display_zone->item(object_display_zone->rowCount()-1,1)->setText(QString("%1").arg(n));
}


void MainWindow::read_data()
{
   
   ifstream inf;
   inf.open("data_backup.txt");
   vector<vector<float *>> outputdata;
   
    while (!inf.eof())
	{
		char databuff[1000];
		inf.getline(databuff,1000);
        vector<float *> points;
        QString tempstring(databuff);
        QString name;

        int index=0;
        while(index!=tempstring.size()&&tempstring[index]!=' '){
            name.push_back(tempstring[index]);
            index++;
        }
        names.push_back(name);
        QString p=tempstring.remove(0,index+1);
        QStringList plist=p.split(' ');
        if(plist.size()<3)
           continue;
       
        for(int i=0;i<(plist.size()-1)/3;i++)
        {
            qDebug()<<plist[3*i].toFloat()<<plist[3*i+1].toFloat()<<plist[3*i+2].toFloat();
            float *f=new float[4]{plist[3*i].toFloat(),plist[3*i+1].toFloat(),plist[3*i+2].toFloat(),1.f};  
            points.push_back(f);
        }
        outputdata.push_back(points);
	}
   qDebug()<<outputdata;
   QThread * slamthread=new QThread();
   core=new slam_core();
   core->moveToThread(slamthread);
   core->ini_slam("ORBvoc.txt","Realsense_Tum.yaml");
   core->set_data(outputdata);
   core->set_obj_name(names);
   slamthread->start();
   connect(save_button,SIGNAL(clicked()),core,SLOT(read_data_out()),Qt::DirectConnection);
   connect(this,SIGNAL(start_slam()),core,SLOT(start_labeling()),Qt::QueuedConnection);
   connect(core,SIGNAL(new_image(Mat)),this,SLOT(update_pic(Mat)),Qt::BlockingQueuedConnection);
   
   emit start_slam();


}


MainWindow::~MainWindow()
{
    
}

