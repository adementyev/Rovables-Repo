/*
Processing 3 sketch for keyboard simple controller for Rovables 
*/

import processing.serial.*;
import java.util.*;

Serial myPort;        // The serial port 



PFont f;
PFont fsmall;

byte MOTOR_OPCODE =  0x02;

void setup()
{
   size(300, 300, P3D); // animation is much smoother in P2D; text looks better with JAVA2D
   background(0);
 
   
   for (int i=0; i<Serial.list().length; i++) { 
     System.out.println(i + " : " + Serial.list()[i]); 
   }//end for
    
    System.out.println("ok");
    String portName = Serial.list()[1];
    myPort = new Serial(this, portName, 1000000);
    frameRate(120);
    
    f = createFont("Arial",90,true);
    fsmall = createFont("Arial",30,true);
    
    textFont(fsmall);  
    text("'a'-left",30,30);
    text("'d'-right",30,70);
    text("'w'-forward",30,110);
    text("'s'-backward",30,150);
    
    text("'q'-stop",30,200);
    
}



void draw() { 
   
}


public void keyPressed()
{  
    println("Key code pressed: " + keyCode);  
     //cycle detector 1 gain 
    if (key == '1')
    {
      myPort.write('1'); //begining
    }
    
    if (key == '2')
    {
      myPort.write('2'); //begining
    }
    
    if (key == '3')
    {
    myPort.write('3');
    }
      
    
   //z axis up
    else if (key =='a') { 
      myPort.write('a');
      //myPort.write(0);
      //myPort.write(4);
      //myPort.write(MOTOR_OPCODE);
      //myPort.write(130); 
      //myPort.write(130);
      //myPort.write(0);
      //myPort.write(0);
    }
    
    //z axis down
    else if (key =='z') { 
      myPort.write('z'); 
    }  
    //x left
   else if (key =='s') { 
      myPort.write('s'); 
    } 
    //x right
   else if (key =='f') { 
      myPort.write('f'); 
   }
   
   //y left
   else if (key =='e') { 
      myPort.write('e'); 
    } 
    //y right
   else if (key =='d') { 
      myPort.write('d'); 
   }
   
   else if (key =='t') { 
      myPort.write('t'); 
   }
   
   else if (key =='g') { 
      myPort.write('g'); 
   }
   
   else if (key =='w') { 
      myPort.write('w'); 
   }
   
   else if (key =='s') { 
      myPort.write('s'); 
   }
   
   else if (key =='q') { 
      myPort.write('q'); 
   }
   
   else if (key =='e') { 
      myPort.write('e'); 
   }
   
   else if (key =='r') { 
      myPort.write('r'); 
   }
    
    
}


public void serialEvent (Serial myPort) {
     try {     //Use try, catch to avoid crashing
       // get the ASCII string:
       String inString = myPort.readStringUntil('\n');        
       //System.out.println(inString.charAt(0));
         //System.out.println(inString);
             
         if (inString !=null ){
                inString = trim(inString);
                System.out.println(inString);
                //System.out.println(inString.length());
         }
      } catch (Exception e){ 
        System.out.println("ErrorSerial: " + e);
      }  
}//end SerialEvent
