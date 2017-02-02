//
//  robotNode.h
//  chainFormV_1
//
//  Created by Artem Dementyev  artemd@mit.edu on 5/10/16.
//
//

#ifndef __chainFormV_1__robotNode__
#define __chainFormV_1__robotNode__

#include <stdio.h>
#include "ofApp.h"

#endif /* defined(__chainFormV_1__robotNode__) */


#define BUFFER_SIZE 50
#define NUMBER_SENSORS 7

class robotNode : public ofBaseApp{
    
public: // place public functions or variables declarations here
    
    
    // methods, equivalent to specific functions of your class objects
    void addData(int []);
    int * getLatest();
    void displayLatest();
    void setID(int);
    int  getID();
    void visualizePosition();
    
    float getAverage(int);
    
    
    // variables
    int sensorType;
    int inputData [NUMBER_SENSORS][BUFFER_SIZE];
    int sensorID;
    int latestInput [NUMBER_SENSORS];
    int sum [NUMBER_SENSORS];
    int visualizationType = 1;
    
    
    robotNode(int, int, int ); // constructor - used to initialize an object, if no properties are passed
    //               the program sets them to the default value
    robotNode();
    
private: // place private functions or variables declarations here
    
    
}; // dont't forget the semicolon!!
