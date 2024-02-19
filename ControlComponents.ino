// Andrew Chen and Anish Nagariya
// Jan 22nd 2024
// Sends commands to our pan and tilt servo system and causes functionality of electrical components.

#include <Servo.h> 
#include <SoftwareSerial.h>
int panServoPin = 9;  // Connect pan servo to pin 9
int tiltServoPin = 8;  // Connect tilt servo to pin 8
int reloadPin = 10; // Conect reload servo to pin 10

Servo panServo; // servo for camera
Servo tiltServo; // servo for gun base
Servo reloadServo; // servo to reload gun
String str; // read input from python
int posPan = 90; // initial position of pan
int posTilt = 90; // initial position of tilt
const int BUFFER = 7; // buffer to move pan tilt servo for camera and gun
bool shoot = false; // check if bullet has been shot
unsigned long startMillis;  //startMillis var

void setup() {
  Serial.begin(115200);
  // attach servos
  panServo.attach(panServoPin);
  reloadServo.attach(reloadPin);
  tiltServo.attach(tiltServoPin);
  // initalize servos to original position
  panServo.write(posPan);
  tiltServo.write(posTilt);
  reloadServo.write(90);
  // pins for output when face is detected
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  // timeout to load
  Serial.setTimeout(10);
}

// Interpret commands and control servos
void loop() {
  if (Serial.available() > 0) { // check if python is sending text
    str = Serial.readStringUntil('\n'); // parse text


    //control the pan servo (x-plane rotation)
    if (str.startsWith("pan:")) {
      int panXCoord = str.substring(4).toInt(); // read x position of face from python

      //if ouotside of lower buffer point, subtract degrees to servo to follow face
      if(panXCoord > 47 && abs(panXCoord-47) > BUFFER) { 
          panServo.write(posPan--); // tilt x coordinate

      //if outside of upper buffer point, add degrees to servo to follow face
      } else if (panXCoord < 53 && abs(panXCoord-53) > BUFFER) {
        panServo.write(posPan++); // tilt x coordinate
      }

    //control the tilt servo (y-plane rotation);
    } else if (str.startsWith("tilt:")) {
      int tiltYCoord = str.substring(5).toInt(); // read y position of face from python 
      //if outside of lower buffer point, subtract degrees to servo to follow face
      //40 and 50 tell us that the range is from 48 x coord to 42 x coord range 
      int tiltVal = tiltYCoord * (9.0/5.0)-30;
      
      tiltServo.write(max(180 - tiltVal, 0)); // tilt gun y coordinate
    } else if (str.startsWith("high")) { // if face detected
      digitalWrite(2, HIGH); // turn on pin for motor 1
      digitalWrite(4, HIGH); // turn on pin for motor 2
      digitalWrite(7, HIGH); // turn on pin for buzzer
      if (!shoot){ // check if not recently shot bullet 
        startMillis = millis();  // track time of shooting
        delay(500); // let motors rotate
        reloadServo.write(0); // reload
        shoot = true; 
      }
      if (millis() - startMillis > 3000){ // check if it has been more than 2 seconds since bullet shot
        startMillis = millis(); // restart track time and make gun reload back
        shoot = false;
        reloadServo.write(90); // reload back bullet in shooting mechanism
      }
    } else if (str.startsWith("low")){ // face not detecetd
      digitalWrite(2, LOW); // turn off motor 1
      digitalWrite(4, LOW); // turn off motor 2
      digitalWrite(7, LOW); // turn off buzzer
      reloadServo.write(90); // set reload servo to original position
      shoot = false; // indicate it has not shot recently
    }    
  }
}
