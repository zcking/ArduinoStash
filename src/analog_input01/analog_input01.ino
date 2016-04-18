const int sensorPin = 0;

void setup()
{
  analogReference(DEFAULT); // not necessary

  Serial.begin(9600); 
}

void loop()
{
  Serial.println(analogRead(sensorPin));
  delay(500);
}

