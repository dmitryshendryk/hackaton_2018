
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "Label_maker.hpp"
#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "slamcore.hpp"
#include "receive_wrapper.hpp"
#include "ThreeDLabel.hpp"

#include "image_wrapper.hpp"
#include "frame_wrapper.hpp"

#include "openglwrapper.hpp"
#include "receive_wrapper.hpp"

#include "control.hpp"
#include "slam_wrapper.hpp"

using namespace cv;
class MainWindow : public QMainWindow
{
    Q_OBJECT
signals: 
 void trigger();
 void shut_down();
 void start_slam();
 void test_paint();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void ini_pcl();
    void add_object();
    void update_num(int);
    void start_labeling();
    void update_pic(Mat m);
    void read_data();
    void updata_model(QImage m);
    void update_frame();
private:
    label_maker *lb;
    QLabel * imagearea;
    ThreeDLabel *modelarea;
    QPushButton * start_pt,*start_lb,*add_bt,*save_button,*read_button,*bt_load,*bt_mdlabel;
    QLineEdit *name_line;
    QTableWidget * object_display_zone;
    slam_core *core;
    vector<QString> names;
    ogl_receiever * model_rec;
    void shut_down_viewer();
    int index=0;

      QLabel * color_img_states,*depth_img_states;
    QLabel * color_img_area,*depth_img_area,*tracking_img,*counter_img,*testoglarea;
    QPushButton *SaveButton;
    
    image_wrapper *depth_wrapper,*color_wrapper,*tracking_wrapper,*counter_wrapper,*opengl_wrapper;
    frame_wrapper *test_frame_wrapper;

    control_Thread * controler;

    QImage *color_img,*depth_img;
    QPixmap *tpx,*dtpx;
    slam_wrapper *s_wrapeer;
    
    ogl_receiever * rec;
};

#endif // MAINWINDOW_H
