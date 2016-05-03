#include "pitches.h"
 
int ledPin = 13;                // choose the pin for the LED
int inputPin = 7;               // choose the input pin (for PIR sensor)
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

void alarm() {
  for (int i = 0; i < 180; i++) {
    // Convert degrees to radians then obtain sin value
    sinVal = (sin(i * (3.1412/180)));

    // Generate a frequency from the sin value 
    toneVal = 2000 + (int(sinVal * 1000));
    tone(alarmPin, toneVal);
  }
}

void run_alarm() {
  if (triggered) {
    alarm();
  } 
  
  val = digitalRead(inputPin);  // read input value
  
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      triggered = true;
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); // turn LED OFF
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
      triggered = false;
    }
  }
}
 
void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
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

  
  if (alarm_enabled) {
    run_alarm();
  } else {
    noTone(alarmPin);
  }
}
