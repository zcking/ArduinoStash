#include "fnv.h"
#include "can_constants.h"

int id = 0;
int dlc = 0;
unsigned char data[8] = {0};
unsigned char key[30] = "secret shared key";
const int ID_RANGE = MAX_ID - MIN_ID;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
  GenerateMessage(id, dlc, &data[0]);
  //uint32_t myHash = FNV::fnv1a(&data[0]);
  //uint64_t *auth_msg = SignMessage(&key[0], 17, &data[0]);
  uint64_t hash = SignMessage(&data[0]);
  Serial.print(id, HEX);
  Serial.print("#0");
  Serial.print(dlc, HEX);
  Serial.print(hash);
  Serial.println("\n----------------------");
  delay(500);
  Serial.flush();
}

void GenerateMessage(int &id, int &dlc, unsigned char *data)
{
    do
    {
      id = random(ID_RANGE) + MIN_ID; // id = random id
    } while (!ValidMessage(id));
    dlc = random(9);  // dlc = random int between 0..8
    for(int i = 0; i < dlc; i++)
    {
      data[i] = random(0x100); // each byte is random char between 0..0xFF
    }
}

uint32_t SignMessage(unsigned char* key, unsigned char *data)
{
  uint32_t myHash = FNV::fnv1a_64(data);
  return myHash;
}
//
//uint64_t* SignMessage(unsigned char *key, int key_len, unsigned char *data)
//{
//  unsigned char *concatenated[key_len + dlc];
//  uint64_t auth_msg[20] = {0};
//  for (int i = 0; i < (key_len + dlc); i++)
//  {
//    auth_msg[i] = FNV::fnv1a_64(concatenated[i]);
//  }
//  return &auth_msg[0];
//}

uint64_t SignMessage(unsigned char* data)
{
  return FNV::fnv1a_64(data);
}

