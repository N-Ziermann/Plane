#include <SoftwareSerial.h>
#include <Servo.h>


// https://github.com/N-Ziermann/Plane/blob/master/App/app/src/main/java/com/z/plane/MainActivity.kt

SoftwareSerial mySerial(3, 2); // RX, TX
Servo vertical,horizontal,motor;

int inputvalues[] = {90,90,0};
String inputStrings[] = {"","",""};
int inputIndex = 0;             // index of where inside the inputStrings array to store new data

int lastSignalTime = 0;         // time when last bluetooth signal was received
int noSignalMotorSpeed = 40;    // speed of the motor if signal is lost
int signalLostTime = 2000;      // time in seconds to wait until sure the signal is lost



/*
OUTPUT:

13:46:59.591 -> 0,0,63;
13:46:59.790 -> 0,0,63;
13:46:59.990 -> 0,14,63;
13:47:00.223 -> 0,14,63;
13:47:00.456 -> 0,0,63;
13:47:00.656 -> 0,0,60,0,60,0,0,6;
13:47:01.155 -> 00,0,0,060,00,0,0,0,0,060,30,00,0,60,00,0,0,00,0,0,060,0,60,30,0,0,00,0,30,00,0,60,00,0,6;
13:47:03.483 -> 0,0,0,30,0,0,0,0,0,63;

*/
bool DEBUG = true;


void setup()
{
  // attach servos to pins
  vertical.attach(7);
  horizontal.attach(6);
  motor.attach(5,600,2250);
  activateMotor();
  
  // activate bluetooth and debug 
  if(DEBUG){
    Serial.begin(9600);
    while (!Serial) {
      ;
    }
  }
  mySerial.begin(9600);
}


void activateMotor(){   // activates the ESC of the motor
  motor.write(0);
  delay(3000);
  for(int i = 0; i<= 45; i++){
    motor.write(i);
    delay(50);
  }
  for(int i = 45; i>= 17; i--){ // 18 = an 17 = aus
    motor.write(i);
    delay(50);
  }
}


void loop() // run over and over
{
  if (mySerial.available()){
    lastSignalTime = millis();
    char d = mySerial.read();
    
    if(DEBUG){
      Serial.write(d);
    }
    
    if(d==','){                 // value seperator
      inputIndex++;
    }
    
    else if(d == ';'){         // end of dataString (reset values for next one)
      if(DEBUG){
        Serial.write('\n');
      }
      inputIndex=0;            
      
      // turn values into int
      for (int i = 0; i<3;i++){
        inputvalues[i] = inputStrings[i].toInt();
        //Serial.write(inputStrings[i].toInt());
        //if (isNumber(inputStrings[i])){
          
          //Serial.write(inputStrings[i].toInt());
        //}
      }
      // reset Strings
      inputStrings[0] = "";
      inputStrings[1] = "";
      inputStrings[2] = "";
      
      // configure servos
      horizontal.write(inputvalues[0]);
      vertical.write(inputvalues[1]);
      motor.write(inputvalues[2]);
      //motor.write(90);
      
      
      delay(25);  // for servos to function properly
    }
    
    // store values
    else{                               
      //if (isNumber(""+d)){
      inputStrings[inputIndex] += d;
      //}
    }
  }

  
  // check if signal lost (last signal too long ago)
  if (millis() - lastSignalTime > signalLostTime){
    // if so become slower and turn right
    horizontal.write(180);
    vertical.write(90);
    //motor.write(noSignalMotorSpeed);
    delay(25);  // for servos to function properly
  }
}


bool isNumber(String s){
  for (int i=0; i<s.length();i++){
    if(s.charAt(i) == '1'||s.charAt(i)=='2'||s.charAt(i)=='3'||s.charAt(i)=='4'||s.charAt(i)=='5'||s.charAt(i)=='6'||s.charAt(i)=='7'||s.charAt(i)=='8'||s.charAt(i)=='9'||s.charAt(i)=='0'){
    ;
    }
    else{
      return false;
    }
    return true;
  }
}
