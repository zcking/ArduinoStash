int ledPin = 13;
int ledPin2 = 4;
char state = 'a';
int flag = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin2, LOW);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0)
  {
    state = Serial.read();
    Serial.print("State: ");
    Serial.println(state);
    flag = 0;
  }

  if (state == '0') 
  {
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, HIGH);
  }

  if (state == '1')
  {
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, LOW);
  }
}
