#ifndef __GL_POLYGON_WIDGET_H__
#define __GL_POLYGON_WIDGET_H__ 1

#include <QGLWidget>
#include "bvhio.h"

using namespace Eigen;

class widget: public QGLWidget
    { //

    Q_OBJECT

    public:
    widget(QWidget *parent);
    ~widget();

    public slots:
        // called by the timer in the main window
    void updateAngle();

    void updateX(int); //only used in testing
    void updateY(int); //connects to the Y slider
    void updateZ(int); //connects to the Z slider

    protected:
    // called when OpenGL context is set up
    void initializeGL();
    // called every time the widget is resized
    void resizeGL(int w, int h);
    // called every time the widget needs painting
    void paintGL();

    private:

    void human(); //combines the basic objects into a table with a picture frame and a globe

    double _angle, _x_cam, _y_cam, _z_cam; //respectively alter the rotation of the globe, the opening of the door, the opening of the car doors, the rotation of the car, the speed of the car, the x axis of the camera (not in final product), the y axis of the camera and the z axis of the camera
    bool IK, damp; // Do IK and add damping
    int numJoints; // Number of joints to do IK on
    bvhio bvhValues; //The bvhio object
    std::string file; //Which file to read
    std::string fileO; //Which file to write
    std::vector<int> whichJoints; //Which joint's index
    std::vector<Vector3f> Ends; //End points for each joint
    float lambda;

    }; // class PlaneWidget

#endif
