#include "slamcore.hpp"
#include <QDir>
#include <QDebug>
#include <fstream>
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;
static const float fx=925.257,fy=925.257,cx=640,cy=370;
void slam_core::ini_slam(QString voc,QString tum)
{
     sys=new ORB_SLAM2::System(voc.toStdString().data(),tum.toStdString().data(),ORB_SLAM2::System::RGBD,false);
}
void slam_core::start_labeling()
{
    double index=0.0;
         qDebug()<<"entttttttttttttttttttttttt";
         QString color_folder="/home/cui/dataset/RGB",depth_folder="/home/cui/dataset/depth",after_lab="/home/cui/dataset/BI";
         QDir c_d(color_folder),d_d(depth_folder);       
         cv::Mat color,depth;
         QFileInfoList list=c_d.entryInfoList();
         bool f=true;
         
        QDir c_dir("/home/cui/dataset/Mask_Input_Data/json_train");
        QFileInfoList cList = c_dir.entryInfoList(); 
        foreach (QFileInfo file, cList){ 
          file.dir().remove(file.fileName());
        }
        ofstream of;
        of.open("/home/cui/dataset/Mask_Input_Data/json_train/label_data.txt");
         for(int i=0;i<list.size()-2;i++)
         {
             QString color=QString("colorimage_%1.jpg").arg(i);
             QString depth=QString("depthimage_%1.png").arg(i);
             cv::Mat imRGB=imread((color_folder+"/"+color).toStdString(),CV_LOAD_IMAGE_UNCHANGED);
             cv::Mat imD=imread((depth_folder+"/"+depth).toStdString(),CV_LOAD_IMAGE_UNCHANGED);
             cv::Mat twc=sys->TrackRGBD(imRGB,imD,index);
            //  qDebug()<<twc.rows<<twc.cols;
            //  for(int i=0;i<twc.rows;i++)
            //    {
            //      for(int j=0;j<twc.cols;j++)
            //       std::cout<<twc.ptr<float>(i)[j]<<" ";
            //      std::cout<<endl;
            //    }
            //   std::cout<<endl;
          
             Eigen::Isometry3d T = ORB_SLAM2::Converter::toSE3Quat( twc );
    
             vector<vector<float *>> testv;
            //  vector<float*> ttt;
            //  float a[4]={-0.11572,0.107484,0.765,1};
            //  ttt.push_back(a);
            //  testv.push_back(ttt);
             vector<vector<Point>> opt_hull;
             Mat show;
             show=imRGB.clone();
             for(int i=0;i<v.size();i++)
             {
                 //obj
                 vector<cv::Point> points;//for convex;
                 
                 for(int j=0;j<v[i].size();j++)
                   {
                        qDebug()<<1;
                        PointT p1;
                        //float a[4]={-0.276326,-0.0974852,1.49579,1};
                        p1.x=v[i][j][0];
                        p1.y=v[i][j][1];
                        p1.z=v[i][j][2];
                        p1.r=255;
                        p1.g=255;
                        p1.b=255;
                        PointCloud::Ptr tmp1( new PointCloud() );
                        tmp1->points.push_back(p1); 
                        PointCloud::Ptr cloud1( new PointCloud() );
                        pcl::transformPointCloud( *tmp1, *cloud1, T.matrix());
                        cv::Point cvp;
                        PointT pclp=cloud1->points[0];
                        float d=pclp.z;
                        if(d<=0)
                          continue;
                        qDebug()<<"d................"<<d;
                        cvp.x=pclp.x*fx/d+cx;
                        cvp.y=pclp.y*fy/d+cy;
                        qDebug()<<cvp.x<<cvp.y;
                        points.push_back(cvp);
                        if(cvp.x<0||cvp.x>=imRGB.cols||cvp.y<0||cvp.y>=imRGB.rows)
                                            continue;
                        //circle(imRGB, cvp, 3,Scalar(255,0,0),-1);
                        //find convex
                                
                   }
                  vector<cv::Point> hull;
                  cv::convexHull(cv::Mat(points),hull,false);
                  
                  
                  cv::Point *Hull=new cv::Point[hull.size()];
                  for(int i=0;i<hull.size();i++)
                    Hull[i]=hull[i];
                  if(hull.size())
                    opt_hull.push_back(hull);
                  else
                    opt_hull.push_back(vector<cv::Point>()); 
                 
                 cv::fillConvexPoly( show,Hull, hull.size(), cv::Scalar(255, 0, 0));
                 qDebug()<<hull[i].x<<" "<<hull[i].y;
                   // drawContours(imRGB, hull, i,  cv::Scalar(255, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
             }
            
             if(opt_hull.size()){
             imwrite((QString("/home/cui/dataset/Mask_Input_Data/json_train/colorimage_%1.jpg").arg(i)).toStdString(),imRGB);
             of<<QString("colorimage_%1.jpg %2 ").arg(i).arg(opt_hull.size()).toStdString();
              
             for(int i=0;i<opt_hull.size();i++)
              {
                if(opt_hull[i].size()){
                of<<names[i].toStdString()<<" "<<opt_hull[i].size()<<" ";
                for(int j=0;j<opt_hull[i].size();j++)
                    of<<opt_hull[i][j].x<<" ";
                
                for(int j=0;j<opt_hull[i].size();j++)
                    of<<opt_hull[i][j].y<<" ";
                }
              }
             of<<"\n";
             }
            
            // imshow("rrr",imRGB);
            emit new_image(show);
            index+=1;
          }
           of.close();
}
void slam_core::set_data(vector<vector<float *>>& input_v)
{
   v=input_v;
}

void slam_core::read_data_out()//save the data of 3D points for future use
{
   QString path="data_backup.txt";
   ofstream of;
   of.open(path.toStdString());
   for(int i=0;i<v.size();i++)
   {
     of<<names[i].toStdString()<<" ";
     for(int j=0;j<v[i].size();j++)
     {
           qDebug()<<v[i][j][0];
           of<<v[i][j][0]<<" "<<v[i][j][1]<<" "<<v[i][j][2]<<" ";
     }
     of<<"\n";
   }
   of.close();
}