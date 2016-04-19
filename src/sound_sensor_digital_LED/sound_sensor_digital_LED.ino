const int ledPin = 7; // digital output led pin
const int buttonpin = 3; //define switch port for sound sensor
int  val; // define digital variable val

void  setup()
{
  pinMode(ledPin,OUTPUT);//define LED as a output port
  pinMode(buttonpin,INPUT);//define switch as a output port
}

/* NOTE: Must be REALLY quiet to make the LED go off */
void  loop()
{ 
  val=digitalRead(buttonpin);//read the value of the digital interface 3 assigned to val 
  if(val==HIGH)//when the switch sensor have signal, LED blink
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin,LOW);
  }
}

