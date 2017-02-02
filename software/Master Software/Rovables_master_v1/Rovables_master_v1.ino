#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//Define pins
#define  LED_BLUE     A3 
#define  LED_RED      A4
#define  CE           10  
#define  CSN          9
#define BUTTON_X      A9
#define BUTTON_Y      A8
#define  DEBUG_PIN    0
#define MOTOR_OPCODE  0x02


#define PACKET_SIZE     7
#define BUFFER_SIZE     30
#define NUMBER_OF_NODES 6

#define JOYSTICK CONTROL 
//#define COMPUTER CONTROL

RF24 radio(CE,CSN); // Default SPI speed is 4Mbit, but should work up to 10MBbit

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL };


bool SERIAL_DEBUG  = true;
uint8_t receiveData[PACKET_SIZE] = { };
uint8_t sendData[PACKET_SIZE] = { };

int state = 0;
int speed_forward  = 0;  

uint8_t robotUnderControl = 0;

uint8_t serialBuffer[BUFFER_SIZE] = { }; //Initialize to zeros
uint8_t computerReceiveBuffer[PACKET_SIZE][NUMBER_OF_NODES] = { };


void setup(void)
{

  pinMode(LED_BLUE, OUTPUT); 
  pinMode(LED_RED, OUTPUT);
  pinMode(DEBUG_PIN, OUTPUT);

  SerialUSB.begin(0);

  radio.begin();

  radio.setRetries(10,0); //retries, delay
  radio.setPayloadSize(PACKET_SIZE);
  radio.setDataRate(RF24_2MBPS);

  radio.openReadingPipe(0,pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.openReadingPipe(2,pipes[2]);
  radio.openReadingPipe(3,pipes[3]);
  radio.openReadingPipe(4,pipes[4]);
  radio.openReadingPipe(5,pipes[5]);

  radio.startListening();

  digitalWrite(LED_RED, HIGH); 
 
}

void loop(void)
{

//Under joystick control. 
 getJoyStickData();


  //--------------CHECK FOR SERIAL PORT DATA---------------
  byte pipeNum = 0; //variable to hold which reading pipe sent data
  byte gotByte = 0; //used to store payload from transmit module
  int numBytes =0;
  while(Serial.available()) { 
    serialBuffer[numBytes] = SerialUSB.read(); ;
    numBytes++;  
  }
  //SerialUSB.println(numBytes);

  if (serialBuffer[0] == '@') { 
      int whereToSend = serialBuffer[1]*256; 
      whereToSend = whereToSend + serialBuffer[2];
      byte opCode = serialBuffer[3]; 
      byte sizeofData = serialBuffer[4]; 
      for (int i=1; i<sizeofData; i++) {
        computerReceiveBuffer[i][whereToSend] = serialBuffer[i+4]; 
      }//end for 
      computerReceiveBuffer[0][whereToSend] = opCode;

      for(int i=0; i<sizeof(serialBuffer); i++) serialBuffer[i] =0;
  }
  
  //------------CHECK THE RADIO----------------------------
  while(radio.available(&pipeNum)){ //Check if received data
     digitalWrite(DEBUG_PIN,HIGH);
     radio.read( &receiveData, PACKET_SIZE ); //read one byte of data and store it in gotByte variable
     printReceiveData(pipeNum);
     delay(5);
     for(int i=0; i<PACKET_SIZE; i++) { 
      //sendData[i] = computerReceiveBuffer[i][pipeNum];
     }

     if (sendToSlave(pipeNum,  PACKET_SIZE)) {
        //Success sending   
     }
     else {
        //failed sending
     }
     digitalWrite(DEBUG_PIN,LOW);
  }//end while
}//end loop

bool sendToSlave(byte whichPipe, uint8_t packet_size) { 
     bool worked; 
   
     radio.stopListening(); 
     radio.openWritingPipe(pipes[whichPipe]); 
     if(!radio.write(&sendData,packet_size)) worked = false; 
     else worked = true; 
     radio.startListening(); 
     return worked; 
}//end sendToSlave

void printReceiveData(byte whichPipe) { 
   SerialUSB.print("S");
   SerialUSB.print(",");
   SerialUSB.print(whichPipe); 
   SerialUSB.print(",");
  for (int i=0; i<PACKET_SIZE; i++) {
         SerialUSB.print(receiveData[i]); 
         if(i<PACKET_SIZE-1){
          SerialUSB.print(",");
         }
  }
  SerialUSB.println(" ");
}

void getJoyStickData() { 
   
  int forward_back = analogRead(BUTTON_X); 
  int left_right = analogRead(BUTTON_Y); 
  speed_forward  = 0;  
  int intesityTurn = 0; 
  if (forward_back>=556) { 
    speed_forward = ((forward_back-556)/2);
  }
  else if (forward_back<556) {
     speed_forward = ((556-forward_back)/2)-25;
     if(speed_forward <=0)  speed_forward = 0;
  }

  //debugButtons();

  if (left_right >700) { 
    //turn LEFT
      sendData[0] = MOTOR_OPCODE;
      sendData[1] = 120; 
      sendData[2] = 120;
      sendData[3] = 1;
      sendData[4] = 1;
      state = 1; 
  }

  else if (left_right < 300) { 
    //turn RIGHT
      sendData[0] = MOTOR_OPCODE;
      sendData[1] = 130; 
      sendData[2] = 130;
      sendData[3] = 0;
      sendData[4] = 0;
      state =2;
  }

  else if (forward_back > 600) { 
    //go FORWARD 
      sendData[0] = MOTOR_OPCODE;
      sendData[1] = speed_forward; 
      sendData[2] = speed_forward; 
      sendData[3] = 1;
      sendData[4] = 0;
      state = 3; 
  }

  else if (forward_back < 400) {
    //go BACKWARDS
      sendData[0] = MOTOR_OPCODE;
      sendData[1] = speed_forward;  //turning left  60/60 original
      sendData[2] = speed_forward;
      sendData[3] = 0;
      sendData[4] = 1;
      state = 4;
  }

  else  { 
    sendData[0] = MOTOR_OPCODE;
    sendData[1] = 0; 
    sendData[2] = 0;
    state = 0;
  }
}

void debugButtons() { 
  SerialUSB.print(analogRead(BUTTON_X));
  SerialUSB.print(",");
  SerialUSB.print(analogRead(BUTTON_Y));
  SerialUSB.print(","); 
  SerialUSB.print(state);
  SerialUSB.print(",");
  SerialUSB.println(speed_forward); 
}


