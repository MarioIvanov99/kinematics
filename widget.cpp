#include <GL/glu.h>
#include <GL/glut.h>
#include <QGLWidget>
#include "widget.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


// constructor
widget::widget(QWidget *parent):
  QGLWidget(parent),
  _angle(0.0), //sets all paramaters to their initial values
  _y_cam(1.0),
  _z_cam(1.0),
  IK(false)
    { // constructor


    } // constructor

// destructor
widget::~widget()
    { // destructor
      bvhValues.channels.clear();

    } // destructor

// called when OpenGL context is set up
void widget::initializeGL()
    { // initializeGL()
    // set the widget background colour
    glClearColor(0.3, 0.3, 0.3, 0.0);
    int a;
    int b;

    std::cout<<"Do inverse kinematics[1] or render an animation[2]"<<std::endl;
    std::cin>>a;
    if(a==1){
      IK=true;
      file = "IKbase.bvh";
      bvhValues.getVertices(file);

      std::cout<<"Should dampening be applied? [0/1]"<<std::endl;
      std::cin>>b;
      if(b==1)
        damp = true;
      if(b==0)
        damp = false;

      if(damp){
        std::cout<<"What is the value of lambda"<<std::endl;
        std::cin>>lambda;
      }

      std::cout<<"How many joints should be moved?"<<std::endl;
      std::cin>>numJoints;

      std::cout<<"Which joints?"<<std::endl;
      for(int i = 0; i < bvhValues.vertices.size(); i++)
        std::cout<<i<<". "<<bvhValues.names[i]<<std::endl;
      while(whichJoints.size()<numJoints){
        int c;
        std::cin>>c;
        whichJoints.push_back(c);
      }

      std::cout<<"What are the endpoints (in order)"<<std::endl;
      while(Ends.size()<numJoints){
        Vector3f End;
        std::cin>>End(0)>>End(1)>>End(2);
        Ends.push_back(End);
      }

      std::cout<<"Which file to output to?"<<std::endl;
      std::cin>>fileO;

    }
    if(a==2){
      IK=false;
      std::cout<<"Which file should be read?"<<std::endl;
      std::cin>>file;
      bvhValues.getVertices(file);
    }

    } // initializeGL()


// called every time the widget is resized
void widget::resizeGL(int w, int h)
    { // resizeGL()
    // set the viewport to the entire widget
    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -100.0, 100.0); // To see the entire figure

    } // resizeGL()


void widget::human(){ //Creates figure

    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(3.0);
	
    int pushh = 1; //Used for testing
    int popp = 0;
	
    std::vector<vertex> toRender = bvhValues.vertices; // Position
    toRender[0].pos.x = bvhValues.channels.front();
    bvhValues.channels.pop_front();
    toRender[0].pos.y = bvhValues.channels.front();
    bvhValues.channels.pop_front();
    toRender[0].pos.z = bvhValues.channels.front();
    bvhValues.channels.pop_front();

    for(int i = 1; i<bvhValues.vertices.size();i++)
      toRender[i].pos = toRender[toRender[i].prev].pos+toRender[i].pos;

    //glPushMatrix();
    glRotatef(bvhValues.channels.front(), 0.0, 0.0, 1.0);
    bvhValues.channels.pop_front();
    glRotatef(bvhValues.channels.front(), 0.0, 1.0, 0.0);
    bvhValues.channels.pop_front();
    glRotatef(bvhValues.channels.front(), 1.0, 0.0, 0.0);
    bvhValues.channels.pop_front();

    for(int i = 1; i<bvhValues.vertices.size(); i++){ // angle changes

      if(toRender[i].isEnd){ // If vertex is end point

        glBegin(GL_LINES);
        glVertex3f(toRender[toRender[i].prev].pos.x, toRender[toRender[i].prev].pos.y, toRender[toRender[i].prev].pos.z);
        glVertex3f(toRender[i].pos.x, toRender[i].pos.y, toRender[i].pos.z);
        glEnd();

        for(int j = 0; j<toRender[i].toPop;j++){ // Call pop matrix the required amount of times
          glPopMatrix();
          popp++;
        }
      }
      else{ // Render a line and push matrix at the end

        glBegin(GL_LINES);
        glVertex3f(toRender[toRender[i].prev].pos.x, toRender[toRender[i].prev].pos.y, toRender[toRender[i].prev].pos.z);
        glVertex3f(toRender[i].pos.x, toRender[i].pos.y, toRender[i].pos.z);
        glEnd();

        //glPushMatrix();
        pushh++;

        glTranslatef(toRender[i].pos.x,toRender[i].pos.y,toRender[i].pos.z);
        glRotatef(bvhValues.channels.front(), 0.0, 0.0, 1.0);
        bvhValues.channels.pop_front();
        glRotatef(bvhValues.channels.front(), 0.0, 1.0, 0.0);
        bvhValues.channels.pop_front();
        glRotatef(bvhValues.channels.front(), 1.0, 0.0, 0.0);
        bvhValues.channels.pop_front();
        glTranslatef(-toRender[i].pos.x,-toRender[i].pos.y,-toRender[i].pos.z);

        glPushMatrix();
      }


    }


}





void widget::updateAngle(){
  _angle += 1.0;
  this->repaint();
}

void widget::updateY(int y){
    _y_cam = y/10;
    this->repaint();
}

void widget::updateX(int x){ //Used only for testing
    _x_cam = x;
    this->repaint();
}

void widget::updateZ(int z){
    _z_cam = z/10;
    this->repaint();
}

// called every time the widget needs painting
void widget::paintGL()
    { // paintGL()
    // clear the widget
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    gluLookAt(1.0,_y_cam, _z_cam, 0.0,0.0,0.0, 0.0,1.0,0.0);
	
    if(bvhValues.channels.size()==0) // Read motion data
      bvhValues.getMotion(file);
    if(IK){ //Do IK and close program with success
      bvhValues.InverseKinematics(numJoints, Ends, damp, fileO, whichJoints, lambda);
      exit(1);
    }
    if(!IK) // If it's not IK, render image
      this->human();
    
    glLoadIdentity();

    // flush to screen
    glFlush();

    } // paintGL()
