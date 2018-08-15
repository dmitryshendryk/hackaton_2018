#include <iostream>
#include "pcl/io/pcd_io.h"
#include "pcl/point_cloud.h"
#include "pcl/visualization/pcl_visualizer.h"
#include "pcl/io/io.h"

#include "pcl/kdtree/kdtree.h"
#include "pcl/features/normal_3d.h"
#include "pcl/features/boundary.h"
#include <QApplication>

#include <cstdlib>
#include <vector>

#include "label_maker/WindowQT.hpp"
#include <opencv2/opencv.hpp>
#include <QMetaType>
using namespace std;
//#define HARRIS



// Mutex: //
// boost::mutex cloud_mutex;

// struct callback_args{
//     // structure used to pass arguments to the callback function
//     PointCloudT::Ptr clicked_points_3d;
//     pcl::visualization::PCLVisualizer::Ptr viewerPtr;
// };

// void pp_callback(const pcl::visualization::PointPickingEvent& event, void* args)
// {
//     struct callback_args* data = (struct callback_args *)args;
//     if (event.getPointIndex() == -1)
//         return;
//     PointT current_point;
//     event.getPoint(current_point.x, current_point.y, current_point.z);
//     data->clicked_points_3d->points.push_back(current_point);
//     // Draw clicked points in red:
//     pcl::visualization::PointCloudColorHandlerCustom<PointT> red(data->clicked_points_3d, 255, 0, 0);
//     data->viewerPtr->removePointCloud("clicked_points");
//     data->viewerPtr->addPointCloud(data->clicked_points_3d, red, "clicked_points");
//     data->viewerPtr->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");
//     std::cout << current_point.x << " " << current_point.y << " " << current_point.z << std::endl;
// }



int main(int argc, char *argv[])
{
    qRegisterMetaType<cv::Mat>("Mat");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}


// int main()
// {

    
//     pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
//     pcl::io::loadPCDFile ("after_filter_1.pcd", *cloud);
//     boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer);
//     #ifdef HARRIS
        
       
//         viewer->addPointCloud(cloud,"all_cloud");
//         //注意Harris的输出点云必须是有强度(I)信息的，因为评估值保存在I分量里
//         pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_out (new pcl::PointCloud<pcl::PointXYZI>);
//         pcl::HarrisKeypoint3D<pcl::PointXYZ,pcl::PointXYZI,pcl::Normal> harris;
//         harris.setInputCloud(cloud);
//         cout<<"input successful"<<endl;
//         harris.setNonMaxSupression(true);
//         harris.setRadius(0.04f);
//         harris.setThreshold(0.02f);
//         cout<<"parameter set successful"<<endl;

//         cloud_out->height=1;
//         cloud_out->width =100;
//         cloud_out->resize(cloud_out->height*cloud->width);    
//             cloud_out->clear();
//         harris.compute(*cloud_out);
//         int size = cloud_out->size();

//         pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_harris (new pcl::PointCloud<pcl::PointXYZ>);
//         cloud_harris->height=1;
//         cloud_harris->width =100;
//         cloud_harris->resize(cloud_out->height*cloud->width);
//         cloud_harris->clear();

//         pcl::PointXYZ point;
//             //可视化结果不支持XYZI格式点云，所有又要导回XYZ格式。。。。
//         for (int i = 0;i<size;i++)
//         {    
//             point.x = cloud_out->at(i).x;
//             point.y = cloud_out->at(i).y;
//             point.z = cloud_out->at(i).z;
//             cloud_harris->push_back(point);
//             std::cout<<i<<std::endl;
//         }



//         pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> harris_color_handler (cloud_harris, 0, 255, 0);
//         viewer->addPointCloud(cloud_harris,harris_color_handler,"harris");
//         viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "harris");


//     std::cout <<"done!!!!!!!!!!!!!!!!1";

       
//     #else
   
//         // if (pcl::io::loadPCDFile<pcl::PointXYZ>("/home/yxg/pcl/pcd/mid.pcd",*cloud) == -1)
      
        
//         std::cout << "points sieze is:"<< cloud->size()<<std::endl;
//         pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
//         pcl::PointCloud<pcl::Boundary> boundaries;
//         pcl::BoundaryEstimation<pcl::PointXYZ,pcl::Normal,pcl::Boundary> est;
//         pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>());
        
        



        
//         pcl::NormalEstimation<pcl::PointXYZ,pcl::Normal> normEst; 
//         normEst.setInputCloud(cloud);
//         normEst.setSearchMethod(tree);
      
//         normEst.setKSearch(9);  
//         normEst.compute(*normals);
//         cout<<"normal size is "<< normals->size()<<endl;
        
//         est.setInputCloud(cloud);
//         est.setInputNormals(normals);
      
//         est.setSearchMethod (tree);
//         est.setKSearch(500);  
//         est.compute (boundaries);

//         //  pcl::PointCloud<pcl::PointXYZ> boundPoints;
//         pcl::PointCloud<pcl::PointXYZ>::Ptr boundPoints (new               pcl::PointCloud<pcl::PointXYZ>);
//         pcl::PointCloud<pcl::PointXYZ> noBoundPoints;
//         int countBoundaries = 0;
//         for (int i=0; i<cloud->size(); i++){
//                 uint8_t x = (boundaries.points[i].boundary_point);
//         int a = static_cast<int>(x); 
//         if ( a == 1)
//                 {
//                         //  boundPoints.push_back(cloud->points[i]);
//                         ( *boundPoints).push_back(cloud->points[i]);
//                         countBoundaries++;
//                 }
//                 else
//                         noBoundPoints.push_back(cloud->points[i]);
                
//     }
//         std::cout<<"boudary size is：" <<countBoundaries <<std::endl;
//         //  pcl::io::savePCDFileASCII("boudary.pcd",boundPoints);

//         pcl::io::savePCDFileASCII("boudary.pcd", *boundPoints);
//         //pcl::io::savePCDFileASCII("NoBoundpoints.pcd",noBoundPoints);
 
      
//         return 0;
        
//     system("pause");
//     #endif
// }    
