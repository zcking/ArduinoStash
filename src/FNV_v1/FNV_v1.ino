#include "fnv.h"

int id = 0;
int dlc = 0;
unsigned char data[8] = {0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  GenerateMessage(id, dlc, &data[0]);
  uint32_t myHash = FNV::fnv1a(&data[0]);
  Serial.println(myHash, HEX);
  Serial.println("----------------------");
  delay(1000);
}

void GenerateMessage(int &id, int &dlc, unsigned char *data)
{
    id = random(0x800); // id = random int between 0..0x7FF
    dlc = random(9);  // dlc = random int between 0..8
    for(int i = 0; i < dlc; i++)
    {
      data[i] = random(0x100); // each byte is random char between 0..0xFF
    }
}
