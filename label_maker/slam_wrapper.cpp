#include "slam_wrapper.hpp"

#include <QDebug>
#include <QThread>
#include <iostream>
#include <vector>

void slam_wrapper::update(uchar *color_data,uchar *depth_data)
{
   //due to there is multithread inside the ORBSLAM      it will determine as this thread elim

     Mat imRGB,imD,t;

   qDebug()<<"update thread"<<QThread::currentThreadId();

   if(color_data&&depth_data)
    {

        index+=1.0;
        imRGB=Mat(Size(width, height), CV_8UC3, (void*)color_data, Mat::AUTO_STEP).clone();
        //CV_16UC1
       // imD=Mat(Size(width, height), CV_8UC3, (void*)depth_data, Mat::AUTO_STEP).clone();
       // cvtColor(imD,imD,CV_RGB2GRAY);
        //imD.convertTo(t,CV_16UC1,257);
	    imD=Mat(Size(width, height), CV_16UC1, (void*)depth_data, Mat::AUTO_STEP).clone();
        emit send_mat(color_data,depth_data,width,height);
        qDebug()<<"slam get new frame";
       
       
    }
    else{
        qDebug()<<"empty";
        return ;
    }

//    #ifdef COMPILEDWITHC11
//     //std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
//   #else
//    //std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
//   #endif

   // Pass the image to the SLAM system
  Mat temp=system->TrackRGBD(imRGB,imD,index);
  vector<MapPoint*> temp_points=system->get_tracking_points_data();
  vector<KeyFrame*> temp_frames=system->get_tracking_frames_data();
  std::vector<std::vector<float>> temp_p,temp_f;
  std::vector<cv::Mat> frame_transfer;
  for(int i=0;i<temp_points.size();i++)
  {
       temp_p.push_back(std::vector<float>());
       cv::Mat pos = temp_points[i]->GetWorldPos();
       temp_p[i].push_back(pos.at<float>(0));
       temp_p[i].push_back(pos.at<float>(1));
       temp_p[i].push_back(pos.at<float>(2));
  }
  for(int i=0;i<temp_frames.size();i++){
       KeyFrame* pKF = temp_frames[i];
       cv::Mat Twc = pKF->GetPoseInverse().t();
       frame_transfer.push_back(Twc);
  }
//   qDebug()<<"tempv size"<<tempv.size();
//   qDebug()<<"slam thread id"<<QThread::currentThreadId();
 
  
  emit update_tracking(temp.data,temp.size().width,temp.size().height);
  emit tracking_data(temp_p,frame_transfer);
  
  //tempv

// #ifdef COMPILEDWITHC11
//    //std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
// #else
//    //std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
// #endif
//     //double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

 index += 1.0;
 


}

void slam_wrapper::update_frame()
{
//    if(c_f&&d_f)
       
       
   
//         Mat imRGB,imD;

//         index+=1.0;
//         imRGB=Mat(Size(width, height), CV_8UC3, (void*)color_data, Mat::AUTO_STEP).clone();
//         //CV_16UC1
//         imD=Mat(Size(width, height), CV_8UC3, (void*)depth_data, Mat::AUTO_STEP).clone();
//         cvtColor(imD,imD,CV_RGB2GRAY);
//         Mat temp;
//         imD.convertTo(temp,CV_32FC1,255.0);
//         emit send_mat(color_data,depth_data,width,height);
//         qDebug()<<"slam get new frame";
    
//     else{
//         qDebug()<<"empty";
//         return ;
//     }

//    #ifdef COMPILEDWITHC11
//     //std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
//   #else
//    //std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
//   #endif

//    // Pass the image to the SLAM system
//   Mat temp=system->TrackRGBD(imRGB,temp,index);


//   cout<<"output mat"<<temp.size().height<<" "<<temp.size().width<<endl;
//   emit update_tracking(temp.data,temp.size().width,temp.size().height);
// #ifdef COMPILEDWITHC11
//    //std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
// #else
//    //std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
// #endif
//     //double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

//  index += 1.0;
//  QThread::usleep(1000);
}
void slam_wrapper::get_color_data(uchar *data,int w,int h)
{

    if(!(width&&height))
    {
        width=w;
        height=h;
         qDebug()<<w<<h;
    }

    color_data=data;
 

}
void slam_wrapper::get_depth_data(uchar *data,int w,int h)//in the same thread
{


    if(!(width&&height))
    {
        width=w;
        height=h;
        qDebug()<<w<<h;
    }

    depth_data=data;
    

    this->update(color_data,depth_data);

}



void slam_wrapper::ini()
{

    system=new ORB_SLAM2::System(voc.toStdString().data(),tum.toStdString().data(),ORB_SLAM2::System::RGBD,false);
  
}
void slam_wrapper::test()
{
    // rs2::pipeline p;
    // p.start();
    // while(true)
    // {
    //     index+=1.0;
    //     rs2::frameset frames=p.wait_for_frames();
    //     rs2::depth_frame dframe=frames.get_depth_frame();
    //     rs2::frame color_frame;
    //     color_frame=frames.get_color_frame();
        
    //     Mat imRGB(Size(640,480),CV_8UC3,(void *)color_frame.get_data(),Mat::AUTO_STEP);
    //     Mat imD(Size(640,480),CV_8UC3,(void *)dframe.get_data(),Mat::AUTO_STEP);
    //      imshow("ttt",imRGB);

    //     system->TrackRGBD(imRGB,imD,index);
    //     QThread::usleep(10000);
    // }
   qDebug()<<QThread::currentThreadId();
  // emit tracking_data();

}
