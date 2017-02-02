#include "ofApp.h"
#include "robotNode.h"
const int PACKET_SIZE =7;
const int INPUT_DATA_SIZE =PACKET_SIZE+2;
int inputData [INPUT_DATA_SIZE];

const int NUM_ROBOTS = 6;
robotNode AllRobots[NUM_ROBOTS];

int testCounter =1;
bool testVar = true;

string ofxGetSerialString(ofSerial &serial, char until) {
    static string str;
    stringstream ss;
    char ch;
    int ttl=1000;
    while ((ch=serial.readByte())>0 && ttl-->0 && ch!=until) {
        ss << ch;
    }
    str+=ss.str();
    if (ch==until) {
        string tmp=str;
        str="";
        return tmp;
    } else {
        return "";
    }
}
//--------------------------------------------------------------
void ofApp::setup(){
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    int baud = 0;
    serial.setup("/dev/tty.usbmodem1948751", baud);
    
    for (int i=0; i<NUM_ROBOTS; i++ ) {
        AllRobots[i].setID(i);
    }
    
    ofSetFrameRate(60);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    string str;
    
    do {
        str = ofxGetSerialString(serial,'\n'); //read until end of line
        if (str=="") continue;
        for(int i = 0; i < str.length(); i++) {
            printf("%c",str[i]);
        }
        vector<string> splitItems = ofSplitString(str, ",");
        vector <int> intVector;

        for(int t=1;t<splitItems.size();++t){
            //intVector.at(t)= ofToInt(splitItems.at(t));
            //cout<<intVector.at(t);
            inputData[t-1] = ofToInt(splitItems.at(t));
            //cout<<inputData[t-1];
            //printf(",");
        }
        printf("\n");
        
        if(splitItems.at(0)=="S" && splitItems.size()==INPUT_DATA_SIZE) {
            int robotID = inputData[0];
            AllRobots[robotID].addData(inputData);
        }//end if
        
    } while (str!="");
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    for (int i=0; i<NUM_ROBOTS; i++) {
        AllRobots[i].visualizePosition();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case 'f':
            testCommand1(4);
            //testCounter++;
            break;
        case 'g':
            testCommand2(4);
            //testCounter++;
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}


void ofApp::sendData(int node, unsigned char opCode, unsigned char sizeofData, unsigned char data []) {
    int bufSize = sizeofData+5;
    unsigned char buf [sizeofData+5];
    buf[0] = '@';
    buf[1] = (unsigned char)(node/256);
    buf[2] = (unsigned char)(node & 0xff);
    buf[3] = opCode;
    buf[4] = sizeofData;
    for(int i=0;i<sizeofData; i++){
        buf[5+i] = data[i];
    }
    serial.writeBytes(&buf[0], bufSize);
}

void ofApp::testCommand1(int node) {
    unsigned char data []= {130,130,1,1,0,0,0};
    sendData(node, 0x02, PACKET_SIZE,data);
}

void ofApp::testCommand2(int node) {
    unsigned char data []= {0,0,1,1,0,0,0};
    sendData(node, 0x02, PACKET_SIZE,data);
}

