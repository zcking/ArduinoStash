
const int ledPin = 8;//the led attach to
const int tiltPin = 3;

void setup()
{ 
  pinMode(ledPin,OUTPUT);//initialize the ledPin as an output
  pinMode(tiltPin,INPUT);
  digitalWrite(tiltPin, HIGH);
} 
/******************************************/
void loop() 
{  
  int digitalVal = digitalRead(tiltPin);
  if(HIGH == digitalVal)
  {
    digitalWrite(ledPin,LOW);//turn the led off
  }
  else
  {
    digitalWrite(ledPin,HIGH);//turn the led on 
  }
}
/**********************************************/

