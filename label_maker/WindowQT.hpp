
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
};

#endif // MAINWINDOW_H
