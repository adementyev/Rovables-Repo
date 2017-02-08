//TODO: Add radio powerdown/up

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include<Wire.h>
#include "definitions.h"


RF24 radio(CE,CSN); // Default SPI speed is 4Mbit, but should work up to 10MBbit

#define WHICH_NODE 4     // must be a number from 0 - 5 identifying the PTX node 

const uint64_t pipes[] =  {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL};
const uint64_t PTXpipe = pipes[ WHICH_NODE ];   // Pulls the address from the above array for this node's pipe

uint8_t dataToWrite[] = {1,1,1,1,1,1};
uint8_t receiveData[PACKET_SIZE];
bool SERIAL_DEBUG  = false;

byte counter = 0;
byte failureToWriteCounter =0;

int testCount=0;
bool ledState = 0;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; 

bool timer4Interrupt = false;
int xPos = 0;
int yPos = 0; 

uint8_t encoder1Counter = 0 ;
uint8_t encoder2Counter = 0 ;

void setup(void)
{
  pinMode(LED_BLUE, OUTPUT); 
  pinMode(LED_RED, OUTPUT);
  pinMode(M1_DIR, OUTPUT); 
  pinMode(M1_PWM, OUTPUT); 
  pinMode(M2_PWM, OUTPUT); 
  pinMode(M2_DIR, OUTPUT); 
  pinMode(TX_PIN, OUTPUT);
  pinMode(INT_IMU, INPUT);
  pinMode(ENC1_INT, INPUT); 
  pinMode(ENC2_INT, INPUT); 
  pinMode(ENC1_LED, OUTPUT); 
  pinMode(ENC2_LED, OUTPUT);

  turnOnEncoders();

  if(SERIAL_DEBUG) SerialUSB.begin(0);

  radio.begin();

  radio.setRetries(10,0); //retries, delay
  radio.setPayloadSize(PACKET_SIZE);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(60);

  radio.openReadingPipe(0,PTXpipe);  //open reading or receive pipe
  radio.stopListening();

  //Flash LEDs to undicate startup
  for(int i=0; i<5; i++) {
    digitalWrite(LED_RED, HIGH); 
    digitalWrite(LED_BLUE, LOW); 
    delay(100);
    digitalWrite(LED_RED, LOW); 
    digitalWrite(LED_BLUE, HIGH); 
    delay(100);
  }
    digitalWrite(LED_BLUE, LOW); 

  //Start the interrupt routines
  attachInterrupt(ENC1_INT, ENC1_INT_Routine, FALLING); 
  attachInterrupt(ENC2_INT, ENC2_INT_Routine, FALLING);
  startTransmitTimer4(10);//freq in Hz
}

//----------------MAIN LOOP-----------------
void loop(void)
{
  
  if(timer4Interrupt==true) { 
    dataToWrite[1] = counter;
    dataToWrite[2] =failureToWriteCounter;
    dataToWrite[3] = encoder1Counter;  
    dataToWrite[4] = encoder2Counter;
    sendAndReceiveResponse();
    timer4Interrupt=false;

  } 
}

//------------HANDLE TIMER 4 INTERRUPT---------------
void TC4_Handler()   
{     
  // Check for overflow (OVF) interrupt
  if (TC4->COUNT16.INTFLAG.bit.OVF && TC4->COUNT16.INTENSET.bit.OVF)             
  {
    timer4Interrupt = true;
    counter++;
    REG_TC4_INTFLAG = TC_INTFLAG_OVF;         // Clear the OVF interrupt flag
  }
}

//------------HANDLE ENCODER INTERRUPTS---------------
void ENC1_INT_Routine() { 
 encoder1Counter++; 
}
void ENC2_INT_Routine() { 
  encoder2Counter++;
}

void sendAndReceiveResponse() { 
  radio.powerUp();
  //Send radio data  
  radio.stopListening();
  radio.openWritingPipe(PTXpipe);        //open writing or transmit pipe
  if (!radio.write( &dataToWrite, PACKET_SIZE )){  //if the write fails let the user know over serial monitor
     if(SERIAL_DEBUG)SerialUSB.println("Failed");   
     failureToWriteCounter++;   
  } 
 else {
    if(SERIAL_DEBUG)SerialUSB.println("SUCCESS");
  }

  //wait for a reply
  radio.startListening();  
  unsigned long started_waiting_at = millis();
  bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 50 )
        timeout = true;
    if ( timeout )
    {
      if(SERIAL_DEBUG)SerialUSB.println("Failed, response timed out.\n\r");
      //timeOutCount++;
    }
    else
    {
      radio.read( &receiveData, PACKET_SIZE );
         toogleLedWhenData();
        switch(receiveData[0]) { 
          case 0x00: 
          break; 
          case IGNORE: 
          break; 
          case MOTOR_CRL: 
             analogWrite(M1_PWM, receiveData[1]);
             analogWrite(M2_PWM, receiveData[2]);
             digitalWrite(M1_DIR, receiveData[3]); 
             digitalWrite(M2_DIR, receiveData[4]);
          break;
          case 0x03:       
          break;
        }
    }
    radio.powerDown();
}

void toogleLedWhenData() {
  testCount++;
  if (testCount>5) { 
    if(ledState == 0) { 
      digitalWrite(LED_BLUE, HIGH);
      ledState=1;
      testCount=0;
  }
  else if (ledState == 1) {
      digitalWrite(LED_BLUE, LOW);
      ledState=0;
      testCount=0;
    }
  }
}

void getDataFromMPU6050() { 
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void serialDebugMPU6050() { 
 SerialUSB.print("$");
  SerialUSB.print(",");
  SerialUSB.print(AcX);
  SerialUSB.print(","); 
  SerialUSB.print(AcY);
  SerialUSB.print(",");
  SerialUSB.print(AcZ);
  SerialUSB.print(","); 
  SerialUSB.print(GyX);
  SerialUSB.print(","); 
  SerialUSB.print(GyY);
  SerialUSB.print(","); 
  SerialUSB.print(GyZ);
  SerialUSB.print(","); 
  SerialUSB.print(millis());
  SerialUSB.print(",");
  SerialUSB.println(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet 
}

void startI2C_400KHz() { 
  Wire.begin();   
  sercom3.disableWIRE();   
  SERCOM3->I2CM.BAUD.bit.BAUD = SystemCoreClock / ( 2 * 400000) - 1 ; 
  sercom3.enableWIRE();    
}

void startTransmitTimer4(int freq) {
  // Set up the generic clock (GCLK4) used to clock timers
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
                    GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK4
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK4 to TC4 and TC5
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TC4 and TC5
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TC4_TC5;     // Feed the GCLK4 to TC4 and TC5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_TC4_COUNT16_CC0 = 48000000/(freq*256) -1;
  //REG_TC4_COUNT16_CC0 = 9374;                   // Set the TC4 CC0 register as the TOP value in match frequency mode
  //REG_TC4_COUNT16_CC0 = 0xB71A;                   // Set the TC4 CC0 register as the TOP value in match frequency mode
  //Freq(HZ) = 48000000/(PRESCALE*(CCO_REG+1))
  //9374  - 20 Hz
  
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);       // Wait for synchronization

  //NVIC_DisableIRQ(TC4_IRQn);
  //NVIC_ClearPendingIRQ(TC4_IRQn);
  NVIC_SetPriority(TC4_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TC4 to 0 (highest)
  NVIC_EnableIRQ(TC4_IRQn);         // Connect TC4 to Nested Vector Interrupt Controller (NVIC)

  REG_TC4_INTFLAG |= TC_INTFLAG_OVF;              // Clear the interrupt flags
  REG_TC4_INTENSET = TC_INTENSET_OVF;             // Enable TC4 interrupts
  // REG_TC4_INTENCLR = TC_INTENCLR_OVF;          // Disable TC4 interrupts
 
  REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV256 |   // Set prescaler to 1024, 48MHz/1024 = 46.875kHz
                   TC_CTRLA_WAVEGEN_MFRQ |        // Put the timer TC4 into match frequency (MFRQ) mode 
                   TC_CTRLA_ENABLE;               // Enable TC4
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);       // Wait for synchronization
} 

void turnOffEncoders() {
  //analogWrite(ENC1_LED, 0);
  //analogWrite(ENC2_LED, 0);
  digitalWrite(ENC1_LED, LOW);
  digitalWrite(ENC2_LED, LOW);
}

void turnOnEncoders() {
  //analogWrite(ENC1_LED, 200);
  //analogWrite(ENC2_LED, 200);
  digitalWrite(ENC1_LED, HIGH);
  digitalWrite(ENC2_LED, HIGH);
}

void turnOffMotors(){
  
}




