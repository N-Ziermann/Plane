#include <SoftwareSerial.h>
#include <Servo.h>

// https://github.com/N-Ziermann/Plane/blob/master/App/app/src/main/java/com/z/plane/MainActivity.kt

SoftwareSerial mySerial(3, 2); // RX, TX
Servo vertical,horizontal,motor;

int inputvalues[] = {90,90,0};
String inputStrings[] = {"","",""};
int inputIndex = 0;                         // index of where inside the inputStrings array to store new data

int noSignalMotorSpeed = 20;                // speed of the motor if signal is lost
unsigned long lastSignalTime = 0;           // time when last bluetooth signal was received
unsigned long signalLostTime = 2000UL;      // time in miliseconds to wait until sure the signal is lost (2 senconds)

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
  horizontal.write(90);
  vertical.write(90);
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


void loop()
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

    else if(d == ';'){          // end of dataString (reset values for next one)
      if(DEBUG){
        Serial.write('\n');
      }
      inputIndex=0;

      // turn values into int
      for (int i = 0; i<3;i++){
        inputvalues[i] = inputStrings[i].toInt();
      }
      // reset Strings
      inputStrings[0] = "";
      inputStrings[1] = "";
      inputStrings[2] = "";

      // configure servos
      horizontal.write(inputvalues[0]);
      vertical.write(inputvalues[1]);
      motor.write((int)map(inputvalues[2],0,100,17,175));
      // for servos to function properly:
      delay(25);
    }
    
    // store values
    else{
      inputStrings[inputIndex] += d;
    }
  }

  // check if signal lost (last signal too long ago)
  if (millis() - lastSignalTime > signalLostTime){
    // if so become slower and turn right
    horizontal.write(180);
    vertical.write(90);
    motor.write(noSignalMotorSpeed);
    delay(25);  // for servos to function properly
  }
}
