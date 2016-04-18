/*
 * Simple Button & LED 
 * This program uses a button to 
 * turn an LED light on
 * and off.
 */

const int switchPin = 8;
const int ledPin = 13;

void setup()
{
  pinMode(switchPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  // This will turn thE LED on when the button is on
  digitalWrite(ledPin, digitalRead(switchPin));
}

