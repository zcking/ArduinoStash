#include <NewTone.h>
#include <NewPing.h>
 
int ledPin = 13;                // choose the pin for the LED
int triggerPin = 7;               
int echoPin = 12;
int maxDistance = 500; // 10cm max distance to ping for
int triggerRange = 25; // range in inches
int alarmPin = 8;
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

// For alarm sound
float sinVal;
int toneVal;
boolean triggered = false;

// system control variables
boolean alarm_enabled = false;
char incomingByte; 

NewPing sonar(triggerPin, echoPin, maxDistance);

void alarm() {
  for (int i = 0; i < 180; i++) {
    // Convert degrees to radians then obtain sin value
    sinVal = (sin(i * (3.1412/180)));

    // Generate a frequency from the sin value 
    toneVal = 2000 + (int(sinVal * 1000));
    NewTone(alarmPin, toneVal);
  }
}

void run_alarm() {
  if (triggered) {
    alarm();
  } 

  int distance = sonar.ping_in();
  Serial.println(distance);

  if (distance < triggerRange && distance != 0) {
    digitalWrite(ledPin, HIGH);
    triggered = true;
  } else {
    digitalWrite(ledPin, LOW);
  }
}
 
void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(alarmPin, OUTPUT);
  Serial.begin(9600);
}
 
void loop(){
  if (Serial.available() > 0) { // If the data came
    incomingByte = Serial.read(); // read byte
    if (incomingByte == '0') {
      alarm_enabled = false;
    }

    if (incomingByte == '1') {
      alarm_enabled = true;
    }
  }

  delay(50);
  int distance = sonar.ping_in();
  Serial.println(distance);
  
  if (alarm_enabled) {
    run_alarm();
  } else {
    noNewTone(alarmPin);
    triggered = false;
  }
}
