#ifndef SLAM
#define SLAM
#include <QObject>
#include "../core/slam/include/System.h"
#include "../core/slam/include/Converter.h"

#include "pcl/io/pcd_io.h"
#include "pcl/point_cloud.h"
#include "pcl/visualization/pcl_visualizer.h"
#include "pcl/io/io.h"

#include "pcl/kdtree/kdtree.h"
#include "pcl/features/normal_3d.h"
#include "pcl/features/boundary.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;
class slam_core:public QObject{
Q_OBJECT
signals:
  void new_image(Mat);
private:
 System *sys;
 vector<vector<float *>> v;
 vector<QString> names;
public:
 void ini_slam(QString ,QString);
 void set_data(vector<vector<float *>> & input_v);
 void new_object(QString name){names.push_back(name);}
 void set_obj_name(vector<QString> n){names=n;}
public slots:
    void start_labeling();
    void read_data_out();





};










#endif