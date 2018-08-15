#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/registration/icp.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/surface/mls.h>
#include <pcl/common/centroid.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include<iostream>
#include<fstream>
#include<vector>
typedef pcl::PointXYZRGB PointT;
typedef pcl::PointCloud<PointT> PointCloudT;
void test_filter ()
{


    pcl::StatisticalOutlierRemoval<pcl::PointXYZRGB>sor;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_out_filtered (new pcl::PointCloud<pcl::PointXYZRGB>);
    sor.setInputCloud(cloud);
    sor.setMeanK(100);
    sor.setStddevMulThresh(0.8);
    sor.filter(*cloud_out_filtered);
    pcl::PCDWriter writer;
    writer.write<pcl::PointXYZRGB>("after_filter_1.pcd", *cloud_out_filtered, false);
   
    return ;
}