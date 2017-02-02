//
//  robotNode.cpp
//  chainFormV_1
//
//  Created by Artem Dementyev  artemd@mit.edu on 5/10/16.
//
//

#include "robotNode.h"

robotNode::robotNode(int ID, int numSensors, int senType)
{
    sensorType = senType;
    sensorID = ID;
    // numberOfSensors = numSensors;
}

robotNode::robotNode() {
}

void robotNode::addData(int inputArray []) {
    
    int *p;
    p = inputArray;
    // latestInput = *p;
    // std::cout << sizeof(p) << std::endl;
    
    for (int i=0; i<sizeof(p)+1; i++) {
        latestInput[i] = inputArray[i];
    }
    int tempArray[NUMBER_SENSORS][BUFFER_SIZE];
    //sum = 0;
    for (int i=0; i<NUMBER_SENSORS; i++) {
        for(int g =1; g<BUFFER_SIZE; g++) {
            tempArray[i][g] = inputData[i][g-1];
        }
        tempArray[i][0] = latestInput[i];
        
        for (int g =0; g<BUFFER_SIZE; g++) {
            inputData[i][g] = tempArray[i][g];
        }  //end for
    }//end for
    //  displayLatest();
}

int * robotNode::getLatest(){
    return latestInput;
}

void robotNode::setID(int ID){
    sensorID = ID;
}

int robotNode::getID() {
    return sensorID;
}

void robotNode::visualizePosition() {
    
    
    ofSetLineWidth(2);
    ofDrawBitmapStringHighlight("ID:" + ofToString(sensorID), 50+sensorID*50, 300);
    for(int i=1; i<NUMBER_SENSORS; i++) {
        ofDrawBitmapStringHighlight(ofToString(latestInput[i]), 50+sensorID*50, 300+50*i);
        ofSetColor(latestInput[i],0,0);
        ofDrawRectangle(50+sensorID*50, 300+50*i, 30, 30);
    }
}
