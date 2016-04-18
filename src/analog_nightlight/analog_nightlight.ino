const int sensorPin = 0;
const int ledPin = 9;

int value = 0;

void setup()
{
  analogReference(DEFAULT); // not necessary
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); 
}

void loop()
{
  value = analogRead(sensorPin);
  Serial.println(value); 
  if (value > 900) digitalWrite(ledPin, HIGH);
  else digitalWrite(ledPin, LOW);
}

