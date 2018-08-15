#include "openglwrapper.hpp"
#include <QDebug>


inline int power_of_two(int n)
{
	if(n<=0) return 0;
	return (n&(n-1))==0;
}

inline bool startswith(string &a,string &b)
{
	if(a.size()<b.size())
	{
		return false;
	}
	return a.substr(0,b.size())==b;
}

GLuint load_texture(const char *file_name)
{
	GLint width,height,total_bytes;
	GLuint last_texture_ID,texture_ID = 0;
	GLubyte *pixels = NULL;
	FILE *pFile;

	qDebug() << file_name << "\n";
	if((pFile=fopen(file_name,"rb"))==NULL)
	{
		cout<<"Read texture error"<<endl;
		return 0;
	}

	fseek(pFile,18,SEEK_SET);
	fread(&width,sizeof(width),1,pFile);
	fread(&height,sizeof(height),1,pFile);
	fseek(pFile,54,SEEK_SET);

	total_bytes = (width*3+(4-width*3%4)%4)*height;

	if((pixels=(GLubyte *)malloc(total_bytes))==NULL)
	{
		fclose(pFile);
		return 0;
	}

	if(fread(pixels,total_bytes,1,pFile)<=0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
		if(!power_of_two(width)||!power_of_two(height)||width>max||height>max)
		{
			const GLint new_width = 1024;
			const GLint new_height = 1024;
			GLint new_total_bytes;
			GLubyte *new_pixels = NULL;

			new_total_bytes = (new_width*3+(4-new_width*3%4)%4)*new_height;
			new_pixels = (GLubyte *)malloc(new_total_bytes);
			if(new_pixels==NULL)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			//gluScaleImage(GL_RGB,width,height,GL_UNSIGNED_BYTE,pixels,new_width,new_height,GL_UNSIGNED_BYTE,new_pixels);

			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	glGenTextures(1,&texture_ID);
	if(texture_ID==0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	glGetIntegerv(GL_TEXTURE_BINDING_2D,(GLint *)&last_texture_ID);
	glBindTexture(GL_TEXTURE_2D,texture_ID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pixels);
	glBindTexture(GL_TEXTURE_2D,last_texture_ID);

	free(pixels);
	fclose(pFile);
	return texture_ID;
}
void setLight()
{
	static const GLfloat light_position[] = {50.0f,50.0f,50.0f,0.0f};
	// static const GLfloat light_ambient[] = {0.0f,0.0f,0.0f,0.0f};
	// static const GLfloat light_diffuse[] = {1.0f,0.9f,0.9f,0.0f};
	// static const GLfloat light_specular[] = {1.0f,1.0f,1.0f,0.0f};
	static const GLfloat light_direction[] = {-1.0f,-1.0f,-1.0f};

	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	// glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	// glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	// glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light_direction);
	// glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,10.0f);
	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,120.0f);
	// static const GLfloat AmbientLight[4]={0.7,0.7,0.7,0.7};
	// glLightfv(GL_LIGHT0,GL_AMBIENT,AmbientLight);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void setMaterial(Material &mat)
{
	// for(int i=0;i<3;i++)
	//   std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!"<<mat.ambient[i]<<" ";
	glMaterialfv(GL_FRONT,GL_AMBIENT,mat.ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat.diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat.specular);
	glMaterialfv(GL_FRONT,GL_EMISSION,mat.emission);
	glMaterialf(GL_FRONT,GL_SHININESS,100);
	
	glColorMaterial(GL_FRONT,GL_DIFFUSE|GL_AMBIENT|GL_SPECULAR|GL_EMISSION);
	// // glColorMaterial(GL_FRONT,GL_AMBIENT);
	// // glColorMaterial(GL_FRONT,GL_SPECULAR);
	// glColorMaterial(GL_FRONT,GL_EMISSION);
    glEnable(GL_COLOR_MATERIAL); 
}
void ReadMtl(string cd,string mtlfile,map<string,Material> &mat)
{
	ifstream in;
	string line,word,ptname,ntname,fname;
	unsigned map;
	bool hasmap = false;
	float ambient[3] = {0.0,0.0,0.0};
	float diffuse[3] = {0.0,0.0,0.0};
	float specular[3] = {0.0,0.0,0.0};
	float emission[3] = {0.0,0.0,0.0};

	in.open(mtlfile.c_str());
	if(!in)
	{
		cout<<"Read mtl error !"<<endl;
		return;
	}
	while(getline(in,line))
	{
		if(line.size()==0||line[0]=='#') continue;
		istringstream is(line);
		is>>word;
		if(word=="newmtl")
		{
			is>>ntname;
			if(!ptname.empty())
			{
				if(hasmap)
				{
					mat.insert(make_pair(ptname,Material(ambient,diffuse,specular,emission,map)));
				}
				else
				{
					mat.insert(make_pair(ptname,Material(ambient,diffuse,specular,emission,0)));
				}
			}
			ptname = ntname;
			hasmap = false;
		}
		else if(word=="Ka")
		{
			is>>ambient[0]>>ambient[1]>>ambient[2];
			qDebug()<<"ab"<<ambient[0]<<ambient[1]<<ambient[2];
		}
		else if(word=="Kd")
		{
			is>>diffuse[0]>>diffuse[1]>>diffuse[2];
			qDebug()<<"kd"<<diffuse[0]<<diffuse[1]<<diffuse[2];
		}
		else if(word=="Ks")
		{
			is>>specular[0]>>specular[1]>>specular[2];
			qDebug()<<"Ks"<<specular[0]<<specular[1]<<specular[2];
		}
		else if(word=="Ke")
		{
			is>>emission[0]>>emission[1]>>emission[2];
			qDebug()<<"Ke"<<emission[0]<<emission[1]<<emission[2];
		}
		else if(word=="map_Ka" || word=="map_Kd")
		{
			is>>fname;
			map = load_texture("/home/dmitry/Documents/model/10113_Flat_Screen_Television_v1_Diffuse.jpg");
			hasmap = true;
		}
		else if(word=="map_refl")
		{

           
			is>>fname;
			map=load_texture("/home/dmitry/Documents/model/10113_Flat_Screen_Television_v1_Diffuse.jpg");
			qDebug()<<"gen texture!!!!!!!!!!!!!!!!!!!!!!!!!!"<<map;
			hasmap=true;

		}
	}
	if(!ntname.empty())
	{
		if(hasmap)
		{
			mat.insert(make_pair(ptname,Material(ambient,diffuse,specular,emission,map)));
		}
		else
		{
			mat.insert(make_pair(ptname,Material(ambient,diffuse,specular,emission,0)));
		}
	}

	in.close();
}

void ReadObj(string &cd,string file,map<string,Object> &m,set<string> &n,map<string,Material> &matname)
{
	ifstream in;
	vector<VERTEX> vertexs;
	vector< pair<float,float> > texcoords;
	vector<VERTEX> normals;
	vector<int> faces;
	int row = 0,col = 0;
	string line,word,goname,mtlname;
	//char Buffer[1000];

	// if(file.find(":")!=string::npos)
	// {
	// 	cd = string(file.begin(),file.begin()+file.rfind("\\"));
	// }
	// else if(startswith(file,string(".\\")))
	// {
	// 	GetCurrentDirectoryA(1000,Buffer);
	// 	cd = Buffer+string(file.begin()+1,file.begin()+file.rfind("\\"));
	// }
	// else if(startswith(file,string("..\\")))
	// {
	// 	GetCurrentDirectoryA(1000,Buffer);
	// 	cd = Buffer;
	// 	cd = string(cd.begin(),cd.begin()+cd.rfind("\\"));
	// 	cd = cd + string(file.begin()+2,file.begin()+file.rfind("\\"));
	// 	cout<<cd<<endl;
	// }
	// else
	// {
	// 	GetCurrentDirectoryA(1000,Buffer);
	// 	if(file.rfind("\\")!=string::npos)
	// 	{
	// 		cd = Buffer+string("\\")+string(file.begin(),file.begin()+file.rfind("\\"));
	// 	}
	// 	else
	// 	{
	// 		cd = Buffer;
	// 	}
	// }

	in.open(file.c_str());
	if(!in)
	{
		cout<<"Read obj error !"<<endl;
		exit(0);
	}
	while(getline(in,line))
	{
		if(line.size()==0||line[0]=='#') continue;
		istringstream is(line);
		is>>word;
		if(word=="v")
		{
			VERTEX p;
			
			is>>p.x>>p.y>>p.z;
			qDebug()<<p.x<<p.y<<p.z;
			p.x*=0.05;
			p.y*=0.05;
			p.z*=0.05;
			vertexs.push_back(p);
		}
		else if(word=="vt")
		{
			pair<float,float> p;
			is>>p.first>>p.second;
			texcoords.push_back(p);
		}
		else if(word=="vn")
		{
			VERTEX p;
			is>>p.x>>p.y>>p.z;
			normals.push_back(p);
		}
		else if(word=="o"||word=="g")
		{
			if(!goname.empty()&&!faces.empty())
			{
				Object obj(vertexs.begin(),vertexs.end(),texcoords.begin(),texcoords.end(),normals.begin(),normals.end(),faces.begin(),faces.end(),row,col,mtlname);
				while(n.count(goname)!=0)
				{
					goname.push_back('0');
				}
				m.insert(make_pair(goname,obj));
				n.insert(goname);
				faces.clear();
			}
			is>>goname;
		}
		else if(word=="f")
		{
			int r = 0,c = 0;
			while(is>>word)
			{
				c = count(word.begin(),word.end(),'/');
				if(c==0)
				{
					faces.push_back(atoi(word.c_str()));
				}
				else if(c==1)
				{
					faces.push_back(atoi(string(word.begin(),word.begin()+word.find("/")).c_str()));
					faces.push_back(atoi(string(word.begin()+word.find("/")+1,word.end()).c_str()));
				}
				else if(c==2)
				{
					int a = word.find("/");
					int b = word.find("/",a+1);
					faces.push_back(atoi(string(word.begin(),word.begin()+a).c_str()));
					faces.push_back(atoi(string(word.begin()+a+1,word.begin()+b).c_str()));
					faces.push_back(atoi(string(word.begin()+b+1,word.end()).c_str()));
				}
				++r;
			}
			row = r;
			col = c+1;
		}
		else if(word=="mtllib")
		{
			is>>word;
			ReadMtl(cd,"/home/dmitry/Documents/model/10113_Flat_Screen_Television_v1_L3.mtl",matname);
		}
		else if(word=="usemtl")
		{
			is>>mtlname;
		}
	}
	if(!goname.empty()&&!faces.empty())
	{
		Object obj(vertexs.begin(),vertexs.end(),texcoords.begin(),texcoords.end(),normals.begin(),normals.end(),faces.begin(),faces.end(),row,col,mtlname);
		while(n.count(goname)!=0)
		{
			goname.push_back('0');
		}
		m.insert(make_pair(goname,obj));
		n.insert(goname);
		faces.clear();
	}
	qDebug()<<"m size:"<<m.size();
	qDebug()<<"n size:"<<n.size();
	in.close();
}


opengl_core::opengl_core(
        QScreen*     targetScreen,
        const QSize& size)
    : OpenGlOffscreenSurface(targetScreen, size) {
      //mvp=cv::Mat::eye(cv::Size(4,4),CV_32FC1);
    
     
    }
 
 

 
 
void opengl_core::initializeGL() {
    
    
}
 
 
void opengl_core::resizeGL(int width, int height) {}
 
 
void opengl_core::paintGL()
{
     
    qDebug()<<"paint";
    // if(!m_program){
    // m_program=new  QOpenGLShaderProgram();
    // m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
    //                                                 "attribute highp vec4 vertices;"
    //                                                 "varying highp vec2 coords;"
    //                                                 "void main() {"
    //                                                 "    gl_Position = vertices;"
    //                                                 "    coords = vertices.xy;"
    //                                                 "}");
    // m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
    //                                                 "uniform lowp float t;"
    //                                                 "varying highp vec2 coords;"
    //                                                 "void main() {"
    //                                                 " lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
    //                                                 " i = smoothstep(t - 0.8, t + 0.8, i);"
    //                                                 " i = floor(i * 20.) / 20.;"
    //                                                 "  gl_FragColor = vec4(coords * .5 + .5, i, i);"
    //                                                 "}");

    //     m_program->bindAttributeLocation("vertices", 0);
    //     m_program->link();
      
    // }    
    // m_program->bind();

    // m_program->enableAttributeArray(0);

    // float values[] = {
    //     -1, -1,
    //     1, -1,
    //     -1, 1,
    //     1, 1
    // };
    // m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    // m_program->setUniformValue("t", (float) m_t);

    // functions()->glViewport(0, 0,get_size().width(), get_size().height());
   
    // functions()->glDisable(GL_DEPTH_TEST);

    // functions()->glClearColor(0, 0, 0, 1);
    // functions()->glClear(GL_COLOR_BUFFER_BIT);

    // functions()->glEnable(GL_BLEND);
    // functions()->glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // functions()->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // m_program->disableAttributeArray(0);
    // m_program->release();
   
    //swapBuffers();
    
     functions()->glEnable(GL_DEPTH_TEST);
    // functions()->glEnable(GL_TEXTURE_2D);
     functions()->glViewport(0, 0,get_size().width(), get_size().height());
     functions()->glClearColor(1,1,1, 1);
     functions()->glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
     glShadeModel(GL_SMOOTH);
	 setLight();
	 first =false;
    // qDebug()<<"2222222222222222222222222222";
     switch (nowtype){
     case POINT:
                glBegin(GL_POINTS);
                glColor3f(0.0,255.0,0.0);
                for(size_t i=0;i< points_data->size();i++)
                {
                    std::vector<float> now=(*points_data)[i];
                    glVertex3f(now[0],now[1],now[2]);
                }
                glEnd();
                break;
    case FRAME:
       
                
                 for(int i=0;i<frames_data->size();i++){
                    glPushMatrix();
                    glMultMatrixf((*frames_data)[i].ptr<GLfloat>(0));
                   
                    functions()->glLineWidth(0.01);
   
                    glColor3f(0.0f,0.0f,1.0f);
                     
                    glBegin(GL_LINES);
                   
                    glVertex3f(0,0,0);
                    glVertex3f(w,h,z);
                    glVertex3f(0,0,0);
                    glVertex3f(w,-h,z);
                    glVertex3f(0,0,0);
                    glVertex3f(-w,-h,z);
                    glVertex3f(0,0,0);
                    glVertex3f(-w,h,z);
                    glVertex3f(w,h,z);
                    glVertex3f(w,-h,z);
                    glVertex3f(-w,h,z);
                    glVertex3f(-w,-h,z);
                    glVertex3f(-w,h,z);
                    glVertex3f(w,h,z);
                    glVertex3f(-w,-h,z);
                    glVertex3f(w,-h,z);
                    
                    glEnd();
           
                    glPopMatrix();
                    
                 }
                break;
    case CAMERA:
                break;    
    case MODEL:
        glMatrixMode(GL_MODELVIEW);
        
	    glLoadIdentity();
        glTranslatef(x, y, 0);
        glRotatef(rx,0.0, 1.0, 0.0);
        glRotatef(ry,0.0,0.0,1.f);
        glScalef(s,s,s);
     
       for(set<string>::iterator it=objname.begin();it!=objname.end();++it)
	  {
			
		Object temp = objmap[*it];
		qDebug()<<QString(temp.material.data());
		
		setMaterial(matname[temp.material]);	
		glColor3f(119.0/255.0,136.0/255.0,153.0/255.0 );
		//functions()->glBindTexture(GL_TEXTURE_2D,matname[temp.material].map);	
		//qDebug()<<"id::"<<matname[temp.material].map;
		
		
		if(temp.row==3) glBegin(GL_TRIANGLES);
		else glBegin(GL_QUADS);
		vector<int>::iterator iter = temp.faces.begin();
		
		if(temp.col==1)
		{
			while(iter!=temp.faces.end())
			{
				//qDebug()<<"111111111111111111";
				
				glVertex3f(temp.vertexs[*iter-1].x,temp.vertexs[*iter-1].y,temp.vertexs[*iter-1].z);
				
				++iter;
			}
		}
		else if(temp.col==2)
		{
			
			while(iter!=temp.faces.end())
			{
				glTexCoord2f(temp.texcoords[*(iter+1)-1].first,temp.texcoords[*(iter+1)-1].second);
				
				glVertex3f(temp.vertexs[*iter-1].x,temp.vertexs[*iter-1].y,temp.vertexs[*iter-1].z);
				iter+=2;
			}
		}
		else if(temp.col==3&&!temp.texcoords.empty())
		{
		
			while(iter!=temp.faces.end())
			{
				glNormal3f(temp.normals[*(iter+2)-1].x,temp.normals[*(iter+2)-1].y,temp.normals[*(iter+2)-1].z);
				//glTexCoord2f(temp.texcoords[*(iter+1)-1].first,temp.texcoords[*(iter+1)-1].second);
				//qDebug()<<temp.texcoords[*(iter+1)-1].first<<temp.texcoords[*(iter+1)-1].second;
				
				glVertex3f(temp.vertexs[*iter-1].x,temp.vertexs[*iter-1].y,temp.vertexs[*iter-1].z);
				iter+=3;
			}
		}

		else
		{
			
			while(iter!=temp.faces.end())
			{
				glNormal3f(temp.normals[*(iter+2)-1].x,temp.normals[*(iter+2)-1].y,temp.normals[*(iter+2)-1].z);
				glVertex3f(temp.vertexs[*iter-1].x,temp.vertexs[*iter-1].y,temp.vertexs[*iter-1].z);
				iter+=3;
			}
		}
		//functions()->glBindTexture(GL_TEXTURE_2D,0);
		glEnd();
	
	}
	glPopMatrix();
    break;
    
    default:
         break;
    
   
 }
}
// void opengl_core::point_draw(std::vector<std::vector<float>> & data)//
// {
    
//     points_data=&data;
//     nowstate=POINT;
//     this->render();
//     this->grabFramebuffer();
//     qDebug()<<"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
    
// }
void opengl_core::read_model_data()
{
   ReadObj(cd,"/home/dmitry/Documents/model/10113_Flat_Screen_Television_v1_L3.obj",objmap,objname,matname);
    // string line;
    // fstream f;
    // f.open("/home/lyz/文档/newgirl.obj.txt", ios::in);
    // if (!f.is_open()){
    //     cout << "Something Went Wrong When Opening Objfiles" << endl;
    // }

    // while (!f.eof()){
    //     getline(f, line);//拿到obj文件中一行，作为一个字符串
    //     QString con=QString::fromStdString(line);
    //     con.remove(con.size()-1,1);
    //     QStringList list=con.split(" ");
    //     //qDebug()<<list;
    //     vector<string>parameters;
    //     string tailMark = " ";
    //     string ans = "";
      
    //     line = line.append(tailMark);
    //     for (int i = 0; i < line.length(); i++) {
    //         char ch = line[i];
    //         if (ch != ' ') {
    //             ans+=ch;
    //         }
    //         else {
    //             parameters.push_back(ans); //取出字符串中的元素，以空格切分
    //             ans = "";
    //         }
    //     }
    //     //cout << parameters.size() << endl;
    //     // if (parameters.size() != 4) {
    //     //     cout << "the size is not correct" << endl;
    //     //     cout<<parameters.size();
    //     // }
        
    //         if (list[0] == "v") {   //如果是顶点的话
    //             vector<GLfloat>Point;
    //             for (int i = 1; i < list.size(); i++) {   //从1开始，将顶点的xyz三个坐标放入顶点vector
    //                 if(!list[i].size())
    //                   continue;
    //                 GLfloat xyz = atof(list[i].toStdString().data());
    //                 Point.push_back(xyz/1000.0);
    //             }
    //             vSets.push_back(Point);
    //         }

    //         else if (list[0] == "f") {   //如果是面的话，存放三个顶点的索引
    //             vector<GLint>vIndexSets;
    //             for (int i = 1; i < list.size(); i++){
    //                 if(!list[i].size())
    //                   continue;
    //                 string x = list[i].toStdString();
    //                 string ans = "";
    //                 for (int j = 0; j < x.length(); j++) {   //跳过‘/’
    //                     char ch = x[j];
    //                     if (ch != '/') { 
    //                         ans += ch;
    //                     }
    //                     else {
    //                         break;
    //                     }
    //                 }
    //                 GLint index = atof(ans.c_str());
    //                 index = index--;//因为顶点索引在obj文件中是从1开始的，而我们存放的顶点vector是从0开始的，因此要减1
    //                 vIndexSets.push_back(index);
    //             }
    //             fSets.push_back(vIndexSets);
    //         }
    //         else if(parameters[0]=="vt")
    //         {
    //             vector<GLfloat> texture;
    //             for(int i=1;i<list.size();i++)
    //             {
    //                 if(!list[i].size())
    //                    continue;
    //                 GLfloat rgb=atof(list[i].toStdString().data());
    //                 texture.push_back(rgb);
    //             }
    //         }
    //         else
    //           continue;
        
    // }
    // f.close();
    // qDebug()<<"data loaded!!!!!!! Vertex size"<<vSets.size()<<" f size"<<fSets.size();
    // ReadMtl("/home/lyz/文档/newgirl.mtl.txt",tex_map);
    // qDebug()<<"materil loaded size:"<<tex_map.size();
}

void opengl_core::trans(float t_x,float t_y)
{
  x+=t_x;
  y+=t_y;
}

void opengl_core::rotate(float x,float y)
{
 rx-=x;
 ry-=y;
}
void opengl_core::zoom(float t_s)
{
 s+=t_s;
}

// void opengl_core::auto_label()
// {
//     //start from current angle
//     for(;rx<360;rx+=3)
// 	{

// 	}





// }