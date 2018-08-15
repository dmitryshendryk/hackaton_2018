/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "pointcloudmapping.h"
#include <KeyFrame.h>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/visualization/cloud_viewer.h>
#include "Converter.h"
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/registration/icp.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/surface/mls.h>
#include <pcl/common/centroid.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include "pcl/io/io.h"
//#define SEG
PointCloudMapping::PointCloudMapping(double resolution_)
{
    this->resolution = resolution_;
    voxel.setLeafSize( resolution, resolution, resolution);
    globalMap = boost::make_shared< PointCloud >( );
    
    viewerThread = make_shared<thread>( bind(&PointCloudMapping::viewer, this ) );


    // float a[4][4]={{923.23,0,643.571,0},
    //                {0,922.717,355.181,0},
    //                {0,0,1,0},
    //                {0,0,0,1}};
    // float b[4][4]={{946.308,0,644.154,0},
    //                 {0,946.308,341.236,0},
    //                 {0,0,1,0},
    //                 {0,0,0,1}};
    // float c[4][4]={{0.999994,0.00315265,0.00132989,-0.0150798},
    //                 {-0.00314526,0.99998,-0.00552274,2.65396e-05},
    //                 {-0.00134728,0.00551852,0.999984,0.000291534},
    //                 {0,0,0,1}};

    // cv::Mat color_in(4,4,CV_32FC1,a),depth_in(4,4,CV_32FC1,b),ex(4,4,CV_32FC1,c);  
    // cv::Mat w=depth_in*ex.inv()*color_in.inv();
    // W=(double *)(w.data);

}

void PointCloudMapping::shutdown()
{
    {
        unique_lock<mutex> lck(shutDownMutex);
        shutDownFlag = true;
        keyFrameUpdated.notify_one();
    }
    viewerThread->join();
}

void PointCloudMapping::insertKeyFrame(KeyFrame* kf, cv::Mat& color, cv::Mat& depth)
{
    cout<<"receive a keyframe, id = "<<kf->mnId<<endl;
    unique_lock<mutex> lck(keyframeMutex);
    keyframes.push_back( kf );
    colorImgs.push_back( color.clone() );
    depthImgs.push_back( depth.clone() );
    
    keyFrameUpdated.notify_one();
}
//added!!!!!!!!!!!!!!
void PointCloudMapping::insertKeyFrame(KeyFrame* kf, cv::Mat& color, cv::Mat& depth,cv::Mat & bi)
{
    cout<<"receive a keyframe, id = "<<kf->mnId<<endl;
    unique_lock<mutex> lck(keyframeMutex);
    keyframes.push_back( kf );
    colorImgs.push_back( color.clone() );
    depthImgs.push_back( depth.clone() );
    Biimage.push_back(bi.clone());
    keyFrameUpdated.notify_one();
}
//////finish
pcl::PointCloud< PointCloudMapping::PointT >::Ptr PointCloudMapping::generatePointCloud(KeyFrame* kf, cv::Mat& color, cv::Mat& depth)
{
    PointCloud::Ptr tmp( new PointCloud() );
    // point cloud is null ptr
    std::cout<<"rows"<<depth.rows<<"   cols"<<depth.cols<<std::endl;
    //throw "stop";
    for ( int m=0; m<depth.rows; m++ )
    {
        for ( int n=0; n<depth.cols; n++ )
        {

            
            float d = depth.ptr<float>(m)[n];
            if (d < 0.01 || d>3)
                continue;
            PointT p;
            p.z = d;
            p.x = ( n - kf->cx) * p.z / kf->fx;
            p.y = ( m - kf->cy) * p.z / kf->fy;
            //std::cout<<"d"<<d<<" "<<"fx"<<kf->fx<<" "<<"cx"<<kf->cx<<" "<<"fy"<<kf->fy<<" "<<"cy"<<kf->cy<<std::endl;
            
            int r_x,r_y;
             

            p.r = color.ptr<uchar>(m)[n*3];
            p.g = color.ptr<uchar>(m)[n*3+1];
            p.b = color.ptr<uchar>(m)[n*3+2];
            // p.r=255;
            // p.g=255;
            // p.b=255;
            tmp->points.push_back(p);
            if(m==500&&n==500)
              std::cout<<"key pos"<<p.z<<" "<<p.x<<" "<<p.y;
        }
    }
    
    Eigen::Isometry3d T = ORB_SLAM2::Converter::toSE3Quat( kf->GetPose() );
    PointCloud::Ptr cloud(new PointCloud);//everytime create a new cloud 
    pcl::transformPointCloud( *tmp, *cloud, T.inverse().matrix());

    // vector<vector<float *>> testv;
    //          vector<float*> ttt;
    //          float a[4]={0.182977,0.0659795,0.44475,1};
    //          ttt.push_back(a);
    //          testv.push_back(ttt);
    //          //vector<vector<cv::Point>> opt_hull;
    //          for(int i=0;i<testv.size();i++)
    //          {
    //              //obj
    //              vector<cv::Point> points;//for convex;
                 
    //              for(int j=0;j<testv[i].size();j++)
    //                {
                       
    //                     PointT p1;
    //                     //float a[4]={-0.276326,-0.0974852,1.49579,1};
    //                     p1.x=testv[i][j][0];
    //                     p1.y=testv[i][j][1];
    //                     p1.z=testv[i][j][2];
    //                     p1.r=255;
    //                     p1.g=255;
    //                     p1.b=255;
    //                     PointCloud::Ptr tmp1( new PointCloud() );
    //                     tmp1->points.push_back(p1); 
    //                     PointCloud::Ptr cloud1( new PointCloud() );
    //                     pcl::transformPointCloud( *tmp1, *cloud1, T.matrix());
    //                     cv::Point cvp;
    //                     PointT pclp=cloud1->points[0];
    //                     float d=pclp.z;
    //                     if(d<=0)
    //                       continue;
                       
    //                     cvp.x=pclp.x*kf->fx/d+kf->cx;
    //                     cvp.y=pclp.y*kf->fy/d+kf->cy;
                       
    //                     points.push_back(cvp);
    //                     if(cvp.x<0||cvp.x>=color.cols||cvp.y<0||cvp.y>=color.rows)
    //                                         continue;
    //                     //circle(color, cvp, 3,cv::Scalar(255,0,0),-1);
    //                     //imshow("ccc",color);
    //                     //find convex
                                
    //                }
    //               // vector<cv::Point> hull;
    //               // cv::convexHull(cv::Mat(points),hull,false);
                  
                  
    //               // cv::Point *Hull=new cv::Point[hull.size()];
    //               // for(int i=0;i<hull.size();i++)
    //               //   Hull[i]=hull[i];
    //               // if(hull.size())
    //               //   opt_hull.push_back(hull);
    //               // else
    //               //   opt_hull.push_back(vector<cv::Point>()); 
    //              // cv::fillConvexPoly( imRGB,Hull, hull.size(), cv::Scalar(255, 0, 0));
    //                 // qDebug()<<hull[i].x<<" "<<hull[i].y;
    //                 //drawContours(imRGB, hull, i,  cv::Scalar(255, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
    //          }

    std::cout<<"key frame trans matrix"<<endl;
    for(int i=0;i<4;i++)
                {
                  for(int j=0;j<4;j++)
                   std::cout<<T(i,j)<<" ";
                  std::cout<<endl;
                }
    cloud->is_dense = false;
    // PointT p1;
    // float a[4]={-0.276326,-0.0974852,1.49579,1};
    // p1.x=a[0];
    // p1.y=a[1];
    // p1.z=a[2];
    // p1.r=255;
    // p1.g=255;
    // p1.b=255;
    // PointCloud::Ptr tmp1( new PointCloud() );
    // tmp1->points.push_back(p1); 
    // PointCloud::Ptr cloud1( new PointCloud() );
    // pcl::transformPointCloud( *tmp1, *cloud1, T.matrix());
    // cv::Point cvp;
    // PointT pclp=cloud1->points[0];
    // float d=pclp.z;
    
    //  cvp.x=pclp.x*kf->fx/d+kf->cx;
    // cvp.y=pclp.y*kf->fy/d+kf->cy;
    // if(cvp.x<0||cvp.x>=color.cols||cvp.y<0||cvp.y>=color.rows)
    //                     return cloud;
    // cv::circle(color,cvp,10, cv::Scalar(255, 0, 0), 2);
    // cv::imshow("rrr",color);
    cout<<"generate point cloud for kf "<<kf->mnId<<", size="<<cloud->points.size()<<endl;
    return cloud;
}

// overwrite write by myself
pcl::PointCloud< PointCloudMapping::PointT >::Ptr PointCloudMapping::generatePointCloud(KeyFrame* kf, cv::Mat& color, cv::Mat& depth,cv::Mat &bi)
{
    PointCloud::Ptr tmp( new PointCloud() );
    // point cloud is null ptr
    std::cout<<"rows"<<depth.rows<<"   cols"<<depth.cols<<std::endl;
    //throw "stop";
    for ( int m=0; m<depth.rows; m+=3 )
    {
        for ( int n=0; n<depth.cols; n+=3 )
        {
            float d = depth.ptr<float>(m)[n];
            if(bi.ptr<uchar>(m)[n]!=255)
                 continue;
            if (d < 0.01 || d>3)
                continue;
            PointT p;
            p.z = d;
            p.x = ( n - kf->cx) * p.z / kf->fx;
            p.y = ( m - kf->cy) * p.z / kf->fy;

            
            int r_x,r_y;
             
       
            p.r = color.ptr<uchar>(m)[n*3];
            p.g = color.ptr<uchar>(m)[n*3+1];
            p.b = color.ptr<uchar>(m)[n*3+2];
            // p.r=255;
            // p.g=255;
            // p.b=255;
            tmp->points.push_back(p);
        }
    }
    
   
    Eigen::Isometry3d T = ORB_SLAM2::Converter::toSE3Quat( kf->GetPose() );
    PointCloud::Ptr cloud(new PointCloud);//everytime create a new cloud
    pcl::transformPointCloud( *tmp, *cloud, T.inverse().matrix());
  
    cloud->is_dense = false;
    
    cout<<"generate point cloud for kf "<<kf->mnId<<", size="<<cloud->points.size()<<endl;
    return cloud;
}
//ended!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void PointCloudMapping::viewer()
{
    pcl::visualization::CloudViewer viewer("viewer");
    while(1)
    {
        {
            unique_lock<mutex> lck_shutdown( shutDownMutex );
            if (shutDownFlag)
            {
                break;
            }
        }
        {
            unique_lock<mutex> lck_keyframeUpdated( keyFrameUpdateMutex );
            keyFrameUpdated.wait( lck_keyframeUpdated );
        }
        
        // keyframe is updated 
        size_t N=0;
        {
            unique_lock<mutex> lck( keyframeMutex );
            N = keyframes.size();
        }
        
        for ( size_t i=lastKeyframeSize; i<N ; i++ )
        {
            #ifdef SEG
            PointCloud::Ptr p = generatePointCloud( keyframes[i], colorImgs[i], depthImgs[i] ,Biimage[i]);
            #else
            PointCloud::Ptr p = generatePointCloud( keyframes[i], colorImgs[i], depthImgs[i] );
            #endif
            //raw_data=p;
            *globalMap += *p;
        }
        //this filter can't be realtime 
        /*pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud_out_filtered (new pcl::PointCloud<pcl::PointXYZRGBA>);
        pcl::StatisticalOutlierRemoval<pcl::PointXYZRGBA> sor;
        sor.setInputCloud(globalMap);
        sor.setMeanK(100);
        sor.setStddevMulThresh(1.0);
        sor.filter(*cloud_out_filtered);*/
        
        
        
        
        
        PointCloud::Ptr tmp(new PointCloud());
        voxel.setInputCloud( globalMap );
        voxel.filter( *tmp );
        globalMap->swap( *tmp );
        viewer.showCloud( globalMap );
        cout<<"show global map, size="<<globalMap->points.size()<<endl;
        lastKeyframeSize = N;
    }
}

void PointCloudMapping::save()
{

   pcl::io::savePCDFileASCII("point_cloud.pcd", *globalMap);
    
}