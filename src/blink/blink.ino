/*
 * My First Program
 * This program simply makes the LED 
 * on the Arduino controller blink on
 * and off repeatedly.
 */

const int ledPin = 13; 
 
void setup() {
  // Initialize pin as output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH); // turn light ON
  delay(1000);                // wait 1000ms (1s)
  digitalWrite(ledPin, LOW);  // turn light OFF
  delay(1000);                // wait 1000ms (1s)
}
