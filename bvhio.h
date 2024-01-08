// include guard
#ifndef _BVH_IO
#define _BVH_IO

#include <iostream>
#include <fstream> //Used for file i/o
#include <sstream> //Used for easier string concatenation
#include "Cartesian3.h"
#include <math.h>
#include <vector> //Used to store the data
#include <list>
#include <deque>
#include <Eigen/Dense> // Used for matrix operations

using namespace Eigen;

class vertex{
  public:
  Cartesian3 pos; //Position
  int prev; // Parent
  bool isBranch; // Not used
  bool isEnd; // End point
  Cartesian3 rotation; // The 3 rotation channels
  Cartesian3 rotationIndex; // The indices of the channels
  int toPop; // How many pops are required in the rendering
  int index; // Index of vertex
};

class bvhio{

  public:

    std::vector<vertex> vertices; // The joints
    std::vector<std::string> names; // The names of the joints
    std::list<double> channels;
    vertex toVertices; // Used to input data into vertices
    int frameCount; //Numbre of frames
    float fps; //Frame rate
    double toChannels; //Used to input data into vertices
    int channelCount;
    int channelAdd;
    int currentRoot; //Current branch 
    int toRoot; //How many from end to branch
    int thetaCount;

    std::vector<float> theta1; //List of angles
    std::vector<std::deque<vertex>> limbs; // deque required for easy element and front access



    bvhio();
    ~bvhio(){};

    void getVertices(std::string); //Gets vertices
    void getMotion(std::string); // Gets motion data
    void outputBVH(VectorXf, std::string); // Outputs result of IK to a file
    void InverseKinematics(int, std::vector<Vector3f>, bool, std::string, std::vector<int>, float); // Calculates set of angles
    Vector3f ForwardKinematics(std::deque<vertex>, int, VectorXf); // Calculates position
    MatrixXf translation(float, float, float);
    MatrixXf rotationZ(float);
    MatrixXf rotationY(float);
    MatrixXf rotationX(float);
    bool checkConvergence(VectorXf, VectorXf); // Checks if every point has converged

};

#endif
