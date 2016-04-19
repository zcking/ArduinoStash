const int sensorPin = 0;
const int ledPin = 9;

int value = 0;
int ledLevel = 0;

void setup()
{
  analogReference(DEFAULT); // not necessary
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); 
}

void loop()
{
  value = analogRead(sensorPin);

  value = constrain(value, 750, 900); // if < 750, value = 750; if > 900, value = 900
  ledLevel = map(value, 750, 900, 255, 0); // map 750 -> 255 and 900 -> 0
  
  Serial.println(value); 
  
  analogWrite(ledPin, ledLevel);
}

