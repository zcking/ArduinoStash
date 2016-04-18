/*
 * Simple Button & LED 
 * This program uses a button to 
 * TOGGLE an LED light on
 * and off.
 */

const int switchPin = 8;
const int ledPin = 13;

bool lastButton = LOW;
bool ledOn = false;
bool currentButton = LOW;

void setup()
{
  pinMode(switchPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

// Fixes the issue with "bouncing" of the switch
bool debounce(bool last)
{
  bool current = digitalRead(switchPin);
  if (last != current)
  {
    delay(5);
    current = digitalRead(switchPin);
  }
  return current;
}

void loop()
{
  currentButton = debounce(lastButton);

  // If the switch/button has been pressed, toggle the ledOn variable
  if (lastButton == LOW && currentButton == HIGH)
  {
    ledOn = !ledOn;
  }

  lastButton = digitalRead(switchPin);
  digitalWrite(ledPin, ledOn);
}

