#include <iostream>
#include <fstream> //Used for file i/o
#include <sstream> //Used for easier string concatenation
#include <math.h>
#include <vector> //Used to store the data
#include "bvhio.h" //Used to store the data
#include <stdlib.h> //Used to store the data
#include <iomanip>

using namespace Eigen;

// constructor
bvhio::bvhio():
  channelCount(0), //sets all paramaters to their initial values
  toRoot(0)
    { // constructor


    } // constructor

void bvhio::getVertices(std::string file){
	
  std::ifstream inFile(file); //Opens input stream

  if(inFile.fail()){ //Checks if the file is not present or unusable
  
      std::cout<<"File cannot be read or does not exist"<<std::endl;
      exit(1);
	  
  }

  std::string check = ""; // String used to check contents of input file

  while(check.compare("MOTION")!=0){ // Gets everything before the motion

      inFile >> check; 
	  
      if(check.compare("OFFSET")==0){ //If offset...
		  
          if(vertices.size()==0){ // If it's the root, then set its prev to -1, since there is no previous
			  
              inFile >> toVertices.pos.x >> toVertices.pos.y >> toVertices.pos.z;
              toVertices.prev = vertices.size()-1;
              vertices.push_back(toVertices);
			  
          }
          else{ //If toRoot is not 0, set the prev to the current branching point
			  
              inFile >> toVertices.pos.x >> toVertices.pos.y >> toVertices.pos.z;
              if(toRoot!=0){
				  
                  toVertices.prev = currentRoot;
                  toRoot=0;
				  
              }
              else{ // Set prev to the previous index
				  
                  toVertices.prev = vertices.size()-1;
				  
              }
              vertices.push_back(toVertices);
			  
          }

      }
      if(check.compare("}")==0){ // Calculates how many pops are required when reading the filre
		  
          inFile >> check;
          toRoot++;
          currentRoot = vertices.back().prev; // Set the current root to the prev of the final vertex

          while(check.compare("}")==0){ // Changes current root while there is still "}"
			  
              inFile >> check;
              toRoot++;
              currentRoot = vertices[currentRoot].prev;
			  
          }
		  
          vertices.back().toPop = toRoot-1; // Set the toPop for all ends

      }
	  
      if(check.compare("CHANNELS")==0){ // Add channels to channelCount. Total is 3*(number of joints)+3
		  
          inFile >> channelAdd;
          channelCount += channelAdd;
		  
      }
	  
      if(check.compare("ROOT")==0||check.compare("JOINT")==0||check.compare("End")==0){ // Have the name of each joint
		  
          inFile >> check;
          names.push_back(check);
		  
      }

  }

  for(int i = 0; i<vertices.size(); i++){ // Set all ends
	  
      if(names[i].compare("Site")==0){
		  
          vertices[i].isEnd = true;
		  
      }
	  
  }

  for(int i = 0; i<vertices.size(); i++){ // Set all branches
	  
      int counter = 0;
	  
      for(int j = 0; j<vertices.size(); j++){
		  
          if(vertices[i].prev == vertices[j].prev)
              counter++;
		  
      }
      if(counter > 1)
          vertices[vertices[i].prev].isBranch = true;
	  
      counter = 0;
	  
  }

  for(int i = 0; i<vertices.size(); i++){ // Set all non-end toPops, since they don't automatically get set to 0
	  
      if(names[i].compare("Site")!=0)
        vertices[i].toPop = 0;
	
  }

}

void bvhio::getMotion(std::string file){ // Get the motion data

  theta1.resize(channelCount); // Put data in theta1
  thetaCount = 0;

  std::ifstream inFile(file); //Opens input stream

  if(inFile.fail()){ //Checks if the file is not present or unusable
      std::cout<<"File cannot be read or does not exist"<<std::endl;
      exit(1);
  }


  std::string check = ""; //String used to check contents of input file

  while(!inFile.eof()){
    inFile >> check;

    if(check.compare("MOTION") == 0){ 
      inFile >> check;      // Getting the non-angle data out of the way
      inFile >> frameCount;	//
      inFile >> check;      //
      inFile >> check;		//
      inFile >> fps;		//

      while(!inFile.eof()){ // Add data to channels
        inFile >> toChannels;
        channels.push_back(toChannels);

        if(thetaCount<channelCount){
          theta1[thetaCount] = toChannels; // Add data to theta1
          thetaCount++;
        }
      }
    }

  }
  channels.pop_back();

}

void bvhio::outputBVH(VectorXf finalAngles, std::string outputFile){ // Output the file

  std::stringstream toFile;
  int counter = 0;

  toFile << "HIERARCHY\n";
  for(int i = 0; i<vertices.size(); i++){ //Follow the format of bvh
    if(i==0){
      toFile << "ROOT " << names[i] << "\n{\n" << "\t" << "OFFSET " << std::setprecision(6) << vertices[i].pos.x << " " << vertices[i].pos.y << " " << vertices[i].pos.z << "\n";
      toFile << "\tCHANNELS 6 Xposition Yposition Zposition Zrotation Yrotation Xrotation\n";
      counter++;
    }
    else if(vertices[i].isEnd){
      for(int j = 0; j < counter; j++){
        toFile << "\t";
      }
      toFile << "End Site\n";
      for(int j = 0; j < counter; j++){
        toFile << "\t";
      }
      toFile << "{\n";
      for(int j = 0; j < counter+1; j++){
        toFile << "\t";
      }
      toFile << "OFFSET " << std::setprecision(6) << vertices[i].pos.x << " " << vertices[i].pos.y << " " << vertices[i].pos.z << "\n";
      for(int j = 0; j < counter; j++){
        toFile << "\t";
      }
      toFile << "}\n";
      for(int j = 0; j < vertices[i].toPop; j++){
        counter--;
        for(int k = 0; k < counter; k++){
          toFile << "\t";
        }
        toFile << "}\n";
      }
    }
    else{
      for(int j = 0; j < counter; j++){
        toFile << "\t";
      }
      toFile<< "JOINT " << names[i] << "\n";
      for(int j = 0; j < counter; j++){
        toFile << "\t";
      }
      toFile<< "{\n";
      counter++;
      for(int j = 0; j < counter; j++){
        toFile << "\t";
      }
      toFile<< "OFFSET " << std::setprecision(6) << vertices[i].pos.x << " " << vertices[i].pos.y << " " << vertices[i].pos.z << "\n";
      for(int j = 0; j < counter; j++){
        toFile << "\t";
      }
      toFile << "CHANNELS 3 Zrotation Yrotation Xrotation\n";

    }


  }
  toFile << "MOTION\n" << "Frames: " << frameCount << "\nFrame Time: " << fps << "\n";

  toFile << "0 0 0 "; // Initial location

  for(int i = 0; i<finalAngles.size(); i++){ // Single frame of data
    toFile<<finalAngles(i)<<" ";
  }

  std::ofstream outFile; //Output file

  outFile.open(outputFile);
  outFile << toFile.str();
  outFile.close();

}

MatrixXf bvhio::translation(float x, float y, float z){ // Translation matrix

    MatrixXf m(4, 4);

    m << 1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1;

    return m;

}

MatrixXf bvhio::rotationZ(float angle){ // Z rotation matrix

    MatrixXf m(4, 4);

    m << cos(angle*(M_PI/180.0)), -sin(angle*(M_PI/180.0)), 0, 0, sin(angle*(M_PI/180.0)), cos(angle*(M_PI/180.0)), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

    return m;

}

MatrixXf bvhio::rotationY(float angle){ // Y rotation matrix

    MatrixXf m(4, 4);

    m << cos(angle*(M_PI/180.0)), 0, sin(angle*(M_PI/180.0)), 0, 0, 1, 0, 0, -sin(angle*(M_PI/180.0)), 0, cos(angle*(M_PI/180.0)), 0, 0, 0, 0, 1;

    return m;

}

MatrixXf bvhio::rotationX(float angle){ // X rotation matrix

    MatrixXf m(4, 4);

    m << 1, 0, 0, 0, 0, cos(angle*(M_PI/180.0)), -sin(angle*(M_PI/180.0)), 0, 0, sin(angle*(M_PI/180.0)), cos(angle*(M_PI/180.0)), 0, 0, 0, 0, 1;

    return m;

}

Vector3f bvhio::ForwardKinematics(std::deque<vertex> limb, int ind, VectorXf angles){ // Gets the location, given angles

  VectorXf position(4); // A position with w is required
  Vector3f position3; // Final return value
  position << limb[ind].pos.x, limb[ind].pos.y, limb[ind].pos.z, 1; // Initial position
 

  for(int j = 0; j<ind; j++){ // Get location, given angles
	  
    position = translation(-limb[j].pos.x, -limb[j].pos.y, -limb[j].pos.z)*position;
    position = rotationZ(angles[limb[j].rotationIndex.x])*position;
    position = rotationY(angles[limb[j].rotationIndex.y])*position;
    position = rotationX(angles[limb[j].rotationIndex.z])*position;
    position = translation(limb[j].pos.x, limb[j].pos.y, limb[j].pos.z)*position;
	
  }

  position3(0) = position(0);
  position3(1) = position(1);
  position3(2) = position(2);

  return position3;
  
}

bool bvhio::checkConvergence(VectorXf End, VectorXf current){ // Checks if every joint is close to its endpoint

  bool isClose = true;

  for(int i = 0; i<End.size(); i+=3){ 
    isClose = isClose&&sqrt(pow(End(i)-current(i),2)+pow(End(i+1)-current(i+1),2)+pow(End(i+2)-current(i+2),2))<0.5;
  }
  
  return isClose;
  
}

void bvhio::InverseKinematics(int numJoints, std::vector<Vector3f> Ends, bool damp, std::string fileO, std::vector<int> joints, float lambda){ // IK

    thetaCount = 3; 
    int currentVertex = 0;
    float deltaTheta = 0.01;

    for(int i = 0; i < vertices.size(); i++){ //Sets more data for vertices
		
      vertices[i].index = i;
      if(names[i].compare("Site")!=0){
		  
        vertices[i].rotationIndex.x = thetaCount-3;
        vertices[i].rotation.x = theta1[thetaCount];
        thetaCount++;
        vertices[i].rotationIndex.y = thetaCount-3;
        vertices[i].rotation.y = theta1[thetaCount];
        thetaCount++;
        vertices[i].rotationIndex.z = thetaCount-3;
        vertices[i].rotation.z = theta1[thetaCount];
        thetaCount++;
		
      }
	  
      if(names[i].compare("Site")==0){ //Sets the collection of "limbs"
		  
        currentVertex = i;
        std::deque<vertex> toBigVector;
        while(currentVertex!=-1){
			
          toBigVector.push_front(vertices[currentVertex]);
          currentVertex = vertices[currentVertex].prev;
		  
        }
        limbs.push_back(toBigVector);
		
      }
    }

    for(int i = 0; i< limbs.size(); i++){ // Sets initial global positions
		
      for(int j = 1; j<limbs[i].size(); j++){
        limbs[i][j].pos = limbs[i][j].pos+limbs[i][j-1].pos;
      }
	  
    }

    VectorXf currentTheta(channelCount-3); //Sets the current theta (which will end up as the final output)
    VectorXf updateTheta; // Additional vector to update it
    
    Vector3f cPosition; // Current position
    Vector3f nPosition; // New position
    VectorXf End(numJoints*3); // End position
	
    int endCount = 0;
	
    for(int i = 0; i<numJoints; i++){ // Set end vector
		
      End(endCount) = Ends[i](0);
      endCount++;
      End(endCount) = Ends[i](1);
      endCount++;
      End(endCount) = Ends[i](2);
      endCount++;
	  
    }

    VectorXf whichLimb(joints.size()); // Set which limb(s) the joint is part of
    VectorXf whichJoint(joints.size()); // Set the joint
	
    VectorXf toUpdate(3*numJoints); // Used for final calculation with the jacobian
	
    bool isSet = false; // Some joints are in multiple limbs

    for(int k = 0; k<joints.size(); k++){
      for(int i = 0; i < limbs.size(); i++){
        for(int j = 0; j < limbs[i].size(); j++){
            if(limbs[i][j].index == joints[k]&&!isSet){
              whichLimb[k] = i;
              whichJoint[k] = j;
              isSet = true; // If they are already set, do not set them again
            }
        }
      }
      isSet = false;
    }

    bool important = false; // Set angles that actually affect the joint
	
    MatrixXf jacobi(3*numJoints, currentTheta.size()); // An N*(channelcount-3) matrix


      int iterations = 0; // Max iterations

      while(!checkConvergence(End, toUpdate)&&iterations<20){ // Continue until converged or max iterations
		  
        for(int i = 0; i < currentTheta.size(); i++){ // For every angle

          endCount = 0;

          for(int q = 0; q < joints.size(); q++){ // Sets angle as importnat
            for(int j = 0; j<10; j++){
              if(limbs[whichLimb[q]][j].rotationIndex.x==i||limbs[whichLimb[q]][j].rotationIndex.y==i||limbs[whichLimb[q]][j].rotationIndex.z==i){
                important = true;
                break;
              }
            }



          if(important){ // If important, calculate change in position

            cPosition = ForwardKinematics(limbs[whichLimb[q]], whichJoint[q], currentTheta);
            //std::cout<<cPosition<<std::endl;
            currentTheta[i]=currentTheta[i]+deltaTheta;

            nPosition = ForwardKinematics(limbs[whichLimb[q]], whichJoint[q], currentTheta);

            currentTheta[i]=currentTheta[i]-deltaTheta;

            cPosition = (nPosition-cPosition)/deltaTheta; //(p2-p1)/dtheta
			
            jacobi(endCount, i) = cPosition[0];
            endCount++;
            jacobi(endCount, i) = cPosition[1];
            endCount++;
            jacobi(endCount, i) = cPosition[2];
            endCount++;

            important = false;
          }
          else{ // If angle is not important, set corresponding value in jacobian to 0
            jacobi(endCount, i) = 0;
            endCount++;
            jacobi(endCount, i) = 0;
            endCount++;
            jacobi(endCount, i) = 0;
            endCount++;
          }
          }

        }

        endCount = 0;

        for(int q = 0; q<numJoints; q++){ // Sets complete position vector
			
          cPosition = ForwardKinematics(limbs[whichLimb[q]], whichJoint[q], currentTheta);
          toUpdate(endCount) = cPosition(0);
          endCount++;
          toUpdate(endCount) = cPosition(1);
          endCount++;
          toUpdate(endCount) = cPosition(2);
          endCount++;
		  
        }

        MatrixXf identity(3*numJoints, 3*numJoints); //Identity for damping

        for(int m = 0; m<3*numJoints; m++) // Set the identity
          for(int n = 0; n<3*numJoints; n++){
            if(m==n)
              identity(m,n) = 1;
            else
              identity(m,n) = 0;
          }

        if(!damp)
          updateTheta = jacobi.transpose()*((jacobi*jacobi.transpose()).inverse()*(End-toUpdate));
        if(damp)
          updateTheta = jacobi.transpose()*((jacobi*jacobi.transpose()-identity*lambda).inverse()*(End-toUpdate));
        currentTheta = currentTheta + updateTheta;
        
        iterations++;


      }

    if(!checkConvergence(End, toUpdate)&&iterations==20){ // If not converged
      std::cout<<"Point out of reach"<<std::endl;
    }

    outputBVH(currentTheta, fileO);

  }
