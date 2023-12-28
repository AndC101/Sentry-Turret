#include <Servo.h> 
#include <SoftwareSerial.h>
int panServoPin = 9;  // Connect pan servo to pin 9
int tiltServoPin = 10;  // Connect tilt servo to pin 10
int buzzerPin = 8;
int bluePin = 13;

Servo panServo;
Servo tiltServo;
String str;
int posPan = 90;
int posTilt = 90;
const int BUFFER = 7;

void setup() {
  Serial.begin(115200);
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);
  panServo.write(posPan);
  tiltServo.write(posTilt);
  pinMode(buzzerPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

// Interpret commands and control servos
void loop() {
  if (Serial.available() > 0) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(bluePin, HIGH);
    str = Serial.readStringUntil('\n');

    //control the pan servo (x-plane rotation)
    if (str.startsWith("pan:")) {
      int panXCoord = str.substring(4).toInt(); // read x position of face from python

      //if OUTSIDE of lower buffer point, subtract degrees to servo to follow face
      if(panXCoord > 47 && abs(panXCoord-47) > BUFFER) { 
          panServo.write(posPan--);

      //if OUTSIDE of upper buffer point, add degrees to servo to follow face
      } else if (panXCoord < 53 && abs(panXCoord-53) > BUFFER) {
        panServo.write(posPan++);
      }

    //control the tilt servo (y-plane rotation);
    } else if (str.startsWith("tilt:")) {
      int tiltYCoord = str.substring(5).toInt(); // read y position of face from python 
      //if OUTSIDE of lower buffer point, subtract degrees to servo to follow face
      //40 and 50 tell us that the range is from 48 x coord to 42 x coord range 
      if(tiltYCoord > 47 && abs(tiltYCoord-47) > BUFFER) { 
        tiltServo.write(posTilt--);

      //if OUTSIDE of upper buffer point, add degrees to servo to follow face
      } else if (tiltYCoord < 53 && abs(tiltYCoord-53) > BUFFER) {
        tiltServo.write(posTilt++);
      }
    }
  } else {
    digitalWrite(buzzerPin, LOW);
    digitalWrite(bluePin, LOW);

  }
}


