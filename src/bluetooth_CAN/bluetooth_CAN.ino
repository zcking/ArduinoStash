
int  LED = 13;      // LED pin
 
void setup() {
  Serial.begin(9600); // initialization
  pinMode(LED, OUTPUT);
  Serial.println("Press 1 to LED ON or 0 to LED OFF...");
}
 
void loop() {
  if (Serial.available() >= 3) {  // if the data came

      char str_id[4];
      Serial.readBytes(str_id, 4); // read id
      Serial.print("ID: ");
      for(int i = 0; i < 3; i++) Serial.print(str_id[i]);


      char dlc_str[2];
      Serial.readBytes(dlc_str, 2);
      Serial.print("\ndlc: ");
      Serial.print(dlc_str[0]);
      Serial.println(dlc_str[1]);
      int dlc = dlc_str[0] - '0';
      dlc << 4;
      dlc += dlc_str[1] - '0';
      Serial.print("int dlc: ");
      Serial.println(dlc);

      char data[16];
      Serial.readBytes(data, dlc*2); // read data
      Serial.print("Data: ");
      for(int i = 0; i < dlc*2; i+=2)
      {
        Serial.print("0x");
        Serial.print(data[i]);
        Serial.print(data[i+1]);
        Serial.print(", ");
      }

      Serial.println("\n-----------------------\n");

    delay(100);
  }
}
