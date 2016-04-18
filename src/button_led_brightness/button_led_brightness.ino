/*
 * Simple Button & LED 
 * This program uses a button to 
 * change an LED light's brightness
 */

const int switchPin = 8;
const int ledPin = 11;

bool lastPress = LOW;
bool currentButton = LOW;
int ledLevel = 0;

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

  if (lastButton == LOW && currentButton == HIGH)
  {
    ledLevel = (ledLevel + 51) % 255; // Add 51 units of brightness level, limiting to 255
  }

  lastButton = currentButton;
  analogWrite(ledPin, ledLevel);
}

