#include "Label_maker.hpp"
#include <fstream>
#include <QDebug>

void  label_maker::pp_callback(const pcl::visualization::PointPickingEvent& event, void* args)
{
    struct callback_args* data = (struct callback_args *)args;
    if (event.getPointIndex() == -1)
        return;
    PointT current_point;
    event.getPoint(current_point.x, current_point.y, current_point.z);
    PointCloudT::Ptr temp(new PointCloudT);
    bool iscancel=false;
    for(int i=0;i<data->clicked_points_3d->points.size();i++)//cancel the point
    {
        if(data->clicked_points_3d->points[i].x==current_point.x&&data->clicked_points_3d->points[i].y==current_point.y&&data->clicked_points_3d->points[i].z==current_point.z)
            { 
                std::cout<<"is cancel \n";
                iscancel=true;
                continue;
                
            }
        else{
            temp->points.push_back(data->clicked_points_3d->points[i]);
           
        }
                  
    }
    if(!iscancel)
     temp->points.push_back(current_point);
    data->clicked_points_3d=temp;
    // Draw clicked points in red:
    pcl::visualization::PointCloudColorHandlerCustom<PointT> red(data->clicked_points_3d, 255, 0, 0);
    data->viewerPtr->removePointCloud("clicked_points");
    data->viewerPtr->addPointCloud(data->clicked_points_3d, red, "clicked_points");
    data->viewerPtr->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");
    std::cout << current_point.x << " " << current_point.y << " " << current_point.z << std::endl;
}








label_maker::label_maker()
{
  
  
}
void label_maker::load(){

   viewer=new pcl::visualization::PCLVisualizer("viewer");
   cloud= boost::shared_ptr<PointCloudT>(new PointCloudT());
   std::string filename("/home/cui/dataset/point_cloud.pcd");
    //visualizer
 
   
    if (pcl::io::loadPCDFile(filename, *cloud))
    {
        std::cerr << "ERROR: Cannot open file " << filename << "! Aborting..." << std::endl;
        throw "error";
    }
    std::cout << cloud->points.size() << std::endl;

    cloud_mutex.lock();    // for not overwriting the point cloud

    // Display pointcloud:
    viewer->addPointCloud(cloud, "cloud");
    //viewer->setCameraPosition(0, 0, -2, 0, -1, 0, 0);

    // Add point picking callback to viewer:
   auto lambda=[this](const pcl::visualization::PointPickingEvent& event)
{
    // struct callback_args* data = (struct callback_args *)args;
    // if (event.getPointIndex() == -1)
    //     return;
    if(!coor.size())
       return;
    PointT current_point;
    event.getPoint(current_point.x, current_point.y, current_point.z);
    PointCloudT::Ptr temp(new PointCloudT);
    bool iscancel=false;
    int count=0;
    for(int i=0;i<coor[coor.size()-1].clicked_points_3d->points.size();i++)//cancel the point
    {
        if(coor[coor.size()-1].clicked_points_3d->points[i].x==current_point.x&&coor[coor.size()-1].clicked_points_3d->points[i].y==current_point.y&&coor[coor.size()-1].clicked_points_3d->points[i].z==current_point.z)
            { 
                std::cout<<"is cancel \n";
                iscancel=true;
                continue;
                
            }
        else{
            temp->points.push_back(coor[coor.size()-1].clicked_points_3d->points[i]);
            count++;
        }
                  
    }
    if(!iscancel){
     temp->points.push_back(current_point);
     count++;
    }
    coor[coor.size()-1].clicked_points_3d=temp;
    // Draw clicked points in red:
    pcl::visualization::PointCloudColorHandlerCustom<PointT> red(coor[coor.size()-1].clicked_points_3d, 255, 0, 0);
    coor[coor.size()-1].viewerPtr->removePointCloud("clicked_points");
    coor[coor.size()-1].viewerPtr->addPointCloud(coor[coor.size()-1].clicked_points_3d, red, "clicked_points");
    coor[coor.size()-1].viewerPtr->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");
    std::cout << current_point.x << " " << current_point.y << " " << current_point.z << std::endl;
    emit newtotal(count);
    
};
    viewer->registerPointPickingCallback(lambda);
    std::cout << "Shift+click on three floor points, then press 'Q'..." << std::endl;

    // Spin until 'Q' is pressed:
    viewer->spin();
    viewer->removePointCloud();
    delete viewer;
     std::cout << "done." << std::endl;
    cloud_mutex.unlock();

    // while (!viewer->wasStopped())
    // {
    //     qDebug()<<"run";
    //     //viewer->spinOnce(100);
    //     //boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    // }

}

void label_maker::reset()
{
  if(coor.size()){
   coor[coor.size()-1].viewerPtr->removePointCloud("clicked_points");

  }
  PointCloudT::Ptr clicked_points_3d(new PointCloudT);
   struct callback_args cb_args; 
  cb_args.clicked_points_3d = clicked_points_3d;
  cb_args.viewerPtr = pcl::visualization::PCLVisualizer::Ptr(viewer);
  coor.push_back(cb_args);
  
}

