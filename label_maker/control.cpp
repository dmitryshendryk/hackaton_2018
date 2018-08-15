#include "control.hpp"

#define TEST_HEIGHT 320
#define TEST_WIDTH  640
#define TEST_FPS 60
#define DEPTH_PROFILE 208
#define COLOR_PROFILE 23
rs2::stream_profile getStreamProfile(rs2::sensor s, int w, int h, int fps, rs2_format format)
{
 //qDebug()<<"format";

 for (rs2::stream_profile p : s.get_stream_profiles())
 {

    if (p.as<rs2::video_stream_profile>().width() == w && p.as<rs2::video_stream_profile>().height() == h && p.fps() == fps && p.format() == format){


        return p;
    }
 }

 throw "NO MATHCHING FORMAT\n";
}



 control_Thread::control_Thread():QThread(){


    ctx=new rs2::context();
    cfg=new rs2::config();
    nowstate=DEFAULT;

    color_data=nullptr;
    depth_data=nullptr;

    colorframe=nullptr;
    depthframe=nullptr;
}



void control_Thread::run()
{
    rs2::stream_profile p;
    rs2_intrinsics temp;


    switch(nowstate)
    {

    case WATHCH: {
        // check if the camera is connected
        qDebug()<<"watching thread start";
        while(!ctx->query_all_sensors().size()){

           qDebug()<<"NO camera has been found yet!";
            continue;
       }

      std::ifstream t("../../../config/HighAccuracy.json");
      std::string str((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());
   


        cout << str << "\n";
      dev=new rs2::device(ctx->query_devices().front());


      depth_sensor=new rs2::sensor(dev->query_sensors()[0].as<rs2::depth_sensor>());
      color_sensor=new rs2::sensor(dev->query_sensors()[1].as<rs2::sensor>());

    //   depth_sensor->open(getStreamProfile(*depth_sensor, 640, 480, 30,RS2_FORMAT_Z16));
    //   color_sensor->open(getStreamProfile(*color_sensor, 640, 480, 30,RS2_FORMAT_RGB8));
    //   qDebug()<<"dev test"<<QThread::currentThread();

      rs400::advanced_mode temp_advanced_mode_dev = dev->as<rs400::advanced_mode>();
      advanced_mode_dev = &(temp_advanced_mode_dev);
      //advanced_mode_dev->load_json(str);

      depth_sensor->open(getStreamProfile(*depth_sensor, 1280, 720, 30,RS2_FORMAT_Z16));
      color_sensor->open(getStreamProfile(*color_sensor, 1280, 720, 30,RS2_FORMAT_RGB8));
      up_load();

      quit();
      qDebug()<<"back to main thread"<<QThread::currentThread();
    }
      break;

    case GET_IMG:
        qDebug()<<"emit capture color image";


        break;
    case GET_DEPTH:
        qDebug()<<"emit capture depth image";


        break;
    default:
      break;

    }

}

//lambda function make it stable
void control_Thread::up_load()
{

    color_sensor->start([&,this](rs2::frame f){

        int width,height;
        rs2::video_frame tempframe=rs2::video_frame(f);
        width=tempframe.get_width();
        height=tempframe.get_height();
      
        color_data=nullptr;

        color_data=(uchar*)tempframe.get_data();
	      cout<< "color_data:"<< *((uchar *)f.get_data())<<endl;

        if(colorframe) delete colorframe;
        colorframe=new rs2::frame(f);
        emit New_color_data(color_data,width,height);
        emit New_color_frame(f);
       });

     depth_sensor->start([&,this](rs2::frame f){

        int width,height;

        	      cout<< "depth_data:"<< *((uchar *)f.get_data())<<endl;

        if(depthframe) delete depthframe;
        depthframe=new rs2::depth_frame(f);
        width=depthframe->get_width();
        height=depthframe->get_height();

        depth_data=nullptr;

        rs2::colorizer color_map;
        rs2::depth_frame Dpi=color_map(*depthframe);

      
       depth_data=(uchar *)depthframe->get_data();
       emit New_depth_data(depth_data,width,height);
      
       depth_data=(uchar *)Dpi.get_data();
      
       emit New_depth_data_colormap(depth_data,width,height);
       emit New_depth_frame(Dpi);//must block


    });

}



