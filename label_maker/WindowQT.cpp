
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
//    this->resize(1920,1280);

//   imagearea=new QLabel(this);
//   imagearea->setGeometry(20,10,840,480);
//   imagearea->setStyleSheet("border: 4px solid  black" );
//   imagearea->setFrameStyle(QFrame::Box);
  
  modelarea=new ThreeDLabel(this);
  modelarea->setGeometry(20,10,340,480);
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


//   object_display_zone=new QTableWidget(this);
//   object_display_zone->setGeometry(870,10,140,480);
//   object_display_zone->setColumnCount(2);
//   QStringList header;  
//   header<<"NAME"<<"NUM";
//   object_display_zone->setHorizontalHeaderLabels(header);
//   object_display_zone->horizontalHeader()->setDefaultSectionSize(70); 
  
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

   qDebug()<<"mainThread ID"<<QThread::currentThreadId();

   color_img=new QImage();
   depth_img=new QImage();
   tpx=new QPixmap();
   dtpx=new QPixmap();
   color_img_states=new QLabel();
   color_img_states->setParent(this);
   color_img_states->setGeometry(20,0,260,20);
//    color_img_states->setText("No color camera signalf recieved");

   color_img_area=new QLabel(this);
   color_img_area->setText("NO SIGNAL");

   color_img_area->setGeometry(380,10,220,240);
   color_img_area->setStyleSheet("border: 5px solid  black" );
   color_img_area->setFrameStyle(QFrame::Box);

   depth_img_states=new QLabel();
   depth_img_states->setParent(this);
   depth_img_states->setGeometry(680,0,260,20);
//    depth_img_states->setText("No depth camera signal recieved");


   depth_img_area=new QLabel(this);

   depth_img_area->setStyleSheet("border: 5px solid  black" );
   depth_img_area->setFrameStyle(QFrame::Box);

   depth_img_area->setGeometry(620,10,220,240);
//    depth_img_area->setText("NO SIGNAL");




   tracking_img=new QLabel(this);
   tracking_img->setGeometry(860,10,220,240);
//    tracking_img->setText("NO SIGNAL");
   tracking_img->setStyleSheet("border: 5px solid  black" );
   tracking_img->setFrameStyle(QFrame::Box);
    
   counter_img=new QLabel(this);
   counter_img->setGeometry(380,260,220,240);
   counter_img->setText("NO SIGNAL");
   counter_img->setStyleSheet("border: 5px solid  black");
   counter_img->setFrameStyle(QFrame::Box);

   color_wrapper=new image_wrapper(0,color_img_area);
   depth_wrapper=new image_wrapper(1,depth_img_area);

   QThread *tr1=new QThread();
   QThread *tr2=new QThread();

   color_wrapper->moveToThread(tr1);
   depth_wrapper->moveToThread(tr2);

   tr1->start();
   tr2->start();

   tracking_wrapper=new image_wrapper(0,tracking_img);
   counter_wrapper=new image_wrapper(0,counter_img);
   
   QThread *tr3=new QThread();
   QThread *tr4=new QThread();
   QThread *tr5=new QThread();
   tracking_wrapper->moveToThread(tr4);
   counter_wrapper->moveToThread(tr5);
   tr4->start();
   tr5->start();

   s_wrapeer=new slam_wrapper(QString("ORBvoc.txt"),QString("Realsense_Tum.yaml"));
   s_wrapeer->moveToThread(tr3);
   tr3->start();
   

    testoglarea=new QLabel(this);
    testoglarea->setGeometry(620,260,220,240);
    testoglarea->setStyleSheet("border: 5px solid  black");
    testoglarea->setFrameStyle(QFrame::Box);
    
    QThread *oglwrapperthread=new QThread();
    opengl_wrapper=new image_wrapper(0,testoglarea);
    opengl_wrapper->moveToThread(oglwrapperthread);
    oglwrapperthread->start();

    QThread * oglthread=new QThread();
    rec =new ogl_receiever(640, 480);
    rec->moveToThread(oglthread);
    oglthread->start();
    
    SaveButton=new QPushButton(this);
    SaveButton->setGeometry(1350,450,80,40);
    SaveButton->setText(QString("Save PointCloud"));

    QFont font ( "Microsoft YaHei", 7, 50); 
    SaveButton->setFont(font);
    
    controler=new control_Thread();
    controler->start_watching(); 
    connect(controler,SIGNAL(finished()),this,SLOT(update_frame()),Qt::QueuedConnection);
  

  
}

void MainWindow::update_frame()
{

    qDebug()<<"slot thread"<<QThread::currentThreadId();
    this->color_img_states->setText("RGB Camera has been found");
    this->depth_img_states->setText("Depth Camera has been found");
    s_wrapeer->ini();


    connect(controler,SIGNAL(New_color_data(uchar *,int ,int )),color_wrapper,SLOT(get_Frame(uchar*,int,int)),Qt::BlockingQueuedConnection);
    connect(controler,SIGNAL(New_depth_data_colormap(uchar * ,int ,int )),depth_wrapper,SLOT(get_Frame(uchar*,int,int)),Qt::BlockingQueuedConnection);



    connect(controler,SIGNAL(New_color_data(uchar *,int ,int )),s_wrapeer,SLOT(get_color_data(uchar*,int,int)),Qt::QueuedConnection);
    connect(controler,SIGNAL(New_depth_data(uchar * ,int ,int )),s_wrapeer,SLOT(get_depth_data(uchar*,int,int)),Qt::QueuedConnection);
    //this must be blocking don't try any other connection way
    connect(s_wrapeer,SIGNAL(update_tracking(uchar *,int ,int )),tracking_wrapper,SLOT(get_Frame(uchar* ,int ,int )),Qt::BlockingQueuedConnection);

    
    test_frame_wrapper=new frame_wrapper();
    QThread *tr7=new QThread();
    test_frame_wrapper->moveToThread(tr7);
    tr7->start();
    connect(controler,SIGNAL(New_color_frame(rs2::frame)),test_frame_wrapper,SLOT(get_color(rs2::frame)),Qt::BlockingQueuedConnection);
    connect(controler,SIGNAL(New_depth_frame(rs2::depth_frame)),test_frame_wrapper,SLOT(get_depth(rs2::depth_frame )),Qt::BlockingQueuedConnection);



    connect(test_frame_wrapper,SIGNAL(update_counter(uchar *,int ,int )),counter_wrapper,SLOT(get_Frame(uchar* ,int ,int )),Qt::BlockingQueuedConnection);     
    connect(s_wrapeer,SIGNAL(tracking_data(std::vector<std::vector<float>>& ,std::vector<cv::Mat>&)),rec,SLOT(get_data(std::vector<std::vector<float>>& ,std::vector<cv::Mat>&)),Qt::BlockingQueuedConnection);
    connect(rec,SIGNAL(new_data(QImage)),opengl_wrapper,SLOT(get_Frame(QImage)),Qt::QueuedConnection);
    // connect(SaveButton,SIGNAL(clicked()),s_wrapeer,SLOT(save_pcl()),Qt::QueuedConnection);

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

