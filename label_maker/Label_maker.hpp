#ifndef LABEL_MAKER
#define LABEL_MAKER

#include <iostream>
#include "pcl/io/pcd_io.h"
#include "pcl/point_cloud.h"
#include "pcl/visualization/pcl_visualizer.h"
#include "pcl/io/io.h"

#include "pcl/kdtree/kdtree.h"
#include "pcl/features/normal_3d.h"
#include "pcl/features/boundary.h"
#include <cstdlib>
#include <vector>
#include <fstream>
#include <QObject>

typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloudT;
using namespace std;

struct callback_args{
    // structure used to pass arguments to the callback function
    PointCloudT::Ptr clicked_points_3d;
    pcl::visualization::PCLVisualizer::Ptr viewerPtr;
    vector<float*> to_vec(){  vector<float*> temp;
                                     for(int i=0;i<clicked_points_3d->points.size();i++)
                                      {
                                        float *a=new float[4];
                                        a[0]=clicked_points_3d->points[i].x;
                                        a[1]=clicked_points_3d->points[i].y;
                                        a[2]=clicked_points_3d->points[i].z;
                                        a[3]=1;
                                    
                                         temp.push_back(a);
                                      }
                                      return temp;
                                  }
};
class label_maker:public QObject
{
  Q_OBJECT
signals :
 void newtotal(int);
private:

  void write_txt(){}
  void earse_point();
  pcl::visualization::PCLVisualizer* viewer;
  boost::shared_ptr<PointCloudT> cloud;
  boost::mutex cloud_mutex;
  bool needstop=false;
public slots:
  void load();
  inline void shut_down_viewer(){}
  
public :
  label_maker();
  void start_new_one(){}
  void pp_callback(const pcl::visualization::PointPickingEvent& event, void* args);
  void reset( );//when add new object
  vector<callback_args> coor; 
  //vector<QString> names;
  //QString get_name(int index){return names[index];}
};

#endif
