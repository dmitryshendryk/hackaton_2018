#ifndef OPENGL_WRAPPER
#define OPENGL_WRAPPER
#include "OpenGlOffscreenSurface.hpp"
#include <QTimer>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv/cv.h>
#include <iostream>
#include<fstream>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdio>

using namespace std;
//can work
//offscreen opengl render
 enum paint_type{POINT=0,FRAME,CAMERA,MODEL,OTHER};
 typedef struct
{
	float x;
	float y;
	float z;
} VERTEX;
class Object {

public:
	typedef std::vector<VERTEX>::iterator viter;
	typedef std::vector< std::pair<float,float> >::iterator titer;
	typedef std::vector<VERTEX>::iterator niter;
	typedef std::vector<int>::iterator fiter;
	Object(viter vbeg,viter vend,titer tbeg,titer tend,niter nbeg,niter nend,fiter fbeg,fiter fend,int r,int c,std::string mat)
	:vertexs(vbeg,vend),texcoords(tbeg,tend),normals(nbeg,nend),faces(fbeg,fend),row(r),col(c),material(mat){}
	Object(){}

	std::vector<VERTEX> vertexs;
	std::vector< std::pair<float,float> > texcoords;
	std::vector<VERTEX> normals;
	std::vector<int> faces;
	std::string material;
	int row,col;
};
class Material {

public:
	Material(float *a,float *d,float *s,float *e,unsigned m)
	{
		qDebug()<<"set........................"<<a[0]<<d[0]<<s[0]<<e[0];
		ambient[0] = *a;
		ambient[1] = *(a+1);
		ambient[2] = *(a+2);
		ambient[3] = 0;
		diffuse[0] = *d;
		diffuse[1] = *(d+1);
		diffuse[2] = *(d+2);
		diffuse[3] = 0;
		specular[0] = *s;
		specular[1] = *(s+1);
		specular[2] = *(s+2);
		specular[3] = 0;
		emission[0] = *e;
		emission[1] = *(e+1);
		emission[2] = *(e+2);
		emission[3] = 0;
		map = m;
	}
	Material()
	{
		//qDebug()<<"empty!!!!!!!!!!!!!!!!";
		ambient[0] = 0;
		ambient[1] = 0;
		ambient[2] = 0;
		ambient[3] = 0;
		diffuse[0] = 1.0;
		diffuse[1] = 1.0;
		diffuse[2] = 1.0;
		diffuse[3] = 0;
		specular[0] = 1.0;
		specular[1] = 1.0;
		specular[2] = 1.0;
		specular[3] = 0;
		emission[0] = 0;
		emission[1] = 0;
		emission[2] = 0;
		emission[3] = 0;
		map = 0;
	}
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emission[4];
	unsigned map;
};






class opengl_core
    : public OpenGlOffscreenSurface
{
Q_OBJECT

public:
   
    explicit opengl_core(
            QScreen* targetScreen = nullptr,
            const QSize& size = QSize (1, 1));
 
    virtual ~opengl_core(){if(points_data) delete points_data;} 

    std::vector<std::vector<float>>* points_data;//unsafe
    std::vector<cv::Mat>* frames_data;
    
    void set_draw_type(paint_type t){nowtype=t;}//shall not be called in different thread
    void read_model_data();
    void rotate(float x,float y);
    void trans(float x,float y);
    void zoom(float );
	float get_current_rotation_x(){return rx;}
	float get_current_rotation_y(){return ry;}
	//void auto_label();
public slots:
    // void point_draw(std::vector<std::vector<float>> & data);
    // void test_initialize(){ini();this->resize(360,360);qDebug()<<"ini done";}
protected:
    virtual void initializeGL() override;
 
    virtual void resizeGL(
            int width,
            int height) override;
 
    virtual void paintGL() override;
private:
    QOpenGLShaderProgram *m_program;
    vector<vector<GLfloat>>vSets;
    vector<vector<GLfloat>>texturesets;
    vector<vector<GLint>>fSets;


    float x,y,rx,ry,s=0.8;
    float m_t=0;
    float mark=1.0/2500.0;
    bool first=true;
    paint_type nowtype=POINT;
    float w=0.05;
    float h = 0.05*0.75;
    float z = 0.05*0.6;
    map<string,Object> objmap;
	set<string> objname;
	string cd;
	map<string,Material> matname;
	GLfloat dx = 0,dy = 0,dz = 0;
	GLfloat ax = 0,ay = 0,az = 0;
	GLint mx = 0,my = 0;
	GLint MouseDown = 0;
	GLfloat aspect = 1;
};


#endif