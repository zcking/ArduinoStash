// demo: CAN-BUS Shield, send data
// Use http://www.seeedstudio.com/wiki/CAN-BUS_Shield if need help
#include <mcp_can.h>
#include <SPI.h>
#include <SpritzCipher.h>
#include <Time.h>

#include "can_constants.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

// Variables
uint32_t id = 0x100;
uint8_t dlc = 8;
uint8_t stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// Hash Buffers
const uint8_t HASH_LEN = 20; // 160-bit. The other 32 bits for timestamp
uint8_t correct_hash[HASH_LEN];
uint8_t hash[HASH_LEN]; // for the incoming hash?

// For timestamping messages
time_t timestamp;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    randomSeed(analogRead(0));
    Serial.begin(115200); // serial interface baud rate 115200 (typical rate for CAN)

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(200);
    }
    Serial.println("CAN BUS Shield init ok!");
}

void loop()
{
    // Check if user has inputted a message
    while (Serial.available())
    {
      // Read the message
      id = Serial.parseInt();
      dlc = Serial.parseInt();
      for(int i = 0; i < dlc; i++)
          stmp[i] = Serial.parseInt();

      // Read the key from serial interface
      uint8_t keyLen = 100;
      uint8_t key[keyLen];
      ReadKey(&key[0], keyLen); // store in key buffer, and sets keyLen

      // Send the normal message
      CAN.sendMsgBuf(id, 0, dlc, stmp);
      
      // Send the Authentication messages
      SendAuthMessagesByKey(key, keyLen);
    }

    // Otherwise, Generate a random CAN message
    GenerateMessage(id, dlc, &stmp[0]);
    CAN.sendMsgBuf(id, 0, dlc, stmp);
    SendAuthMessages(); 
        
    // Display message on Serial interface
    PrintMessage();
    Serial.flush();
    delay(10);
}


void ReadKey(uint8_t *keyBuffer, uint8_t &maxLen)
{
    int index = 0;
    //Serial.print("Entered Key: ");
    while (Serial.available())
    {
        keyBuffer[index] = Serial.parseInt();
        //Serial.print(keyBuffer[index]);
        //Serial.print(" ");
        index++;
    }
//    if (CompareKey(id, &keyBuffer[0], index))
//        Serial.println("- Correct Key");
//    else
//        Serial.println("- Incorrect Key");
    maxLen = index;
}


bool CompareKey(int identifier, uint8_t *key, uint8_t keyLen)
{
    uint8_t correct_key[100];
    uint8_t correct_key_len;
    GetKey(identifier, &correct_key[0], correct_key_len); // get correct key

//    if (keyLen != correct_key_len)
//    {
//        Serial.print(" (length off: ");
//        Serial.print(keyLen);
//        Serial.print(" - ");
//        Serial.print(correct_key_len);
//        Serial.print(") ");
          return false;
//    }
        
    for(int i = 0; i < keyLen; i++)
    {
        if (key[i] != correct_key[i])
            return false;
    }
    return true;
}


// Sends the 3 authentication messages, 
// with a hash according to a given key
void SendAuthMessagesByKey(uint8_t *key, uint8_t keyLen)
{
    // Fill the hash buffer 
    spritz_mac(&hash[0], HASH_LEN, &stmp[0], dlc, key, keyLen); // create correct hash

    // Create and send the 3 auth messages
    uint8_t msg1[8];
    uint8_t msg2[8];
    uint8_t msg3[8];
    for (int i = 0; i < 8; i++) msg1[i] = hash[i];
    for (int i = 8; i < 16; i++) msg2[i - 8] = hash[i];
    for (int i = 16; i < 20; i++) msg3[i - 16] = hash[i];

    // Fill msg3 last 4 bytes with timestamp
    StampTime(&msg3[4]);

    CAN.sendMsgBuf(id, 0, 8, msg1);
    CAN.sendMsgBuf(id, 0, 8, msg2);
    CAN.sendMsgBuf(id, 0, 8, msg3);

//    Serial.print("Sending: ");
//    PrintAuthMessage(&msg1[0], 8);
//    PrintAuthMessage(&msg2[0], 8);
//    PrintAuthMessage(&msg3[0], 4);
//    Serial.print('\n');
}


void StampTime(uint8_t *buf)
{
    unsigned long ms = millis(); 

    // Store the timestamp
    buf[0] = (int)((ms >> 24) & 0xFF);
    buf[1] = (int)((ms >> 16) & 0xFF);
    buf[2] = (int)((ms >> 8) & 0xFF);
    buf[3] = (int)((ms & 0xFF));
}


// Sends the 3 authentication messages
void SendAuthMessages()
{
    GetCorrectHash(); // fill the correct_hash buffer
    uint8_t msg1[8];
    uint8_t msg2[8];
    uint8_t msg3[8];
    for (int i = 0; i < 8; i++) msg1[i] = correct_hash[i];
    for (int i = 8; i < 16; i++) msg2[i - 8] = correct_hash[i];
    for (int i = 16; i < 20; i++) msg3[i - 16] = correct_hash[i];

    // Fill last 4 bytes of msg3 with the timestamp
    StampTime(&msg3[4]);

    CAN.sendMsgBuf(id, 0, 8, msg1);
    CAN.sendMsgBuf(id, 0, 8, msg2);
    CAN.sendMsgBuf(id, 0, 8, msg3);

//    Serial.print("Sending: ");
//    PrintAuthMessage(&msg1[0], 8);
//    PrintAuthMessage(&msg2[0], 8);
//    PrintAuthMessage(&msg3[0], 8);
//    Serial.print('\n');
}


void PrintAuthMessage(uint8_t *msg, uint8_t msgLen)
{
    for(int i = 0; i < msgLen; i++)
    {
        //if (msg[i] < 0x10)
//            Serial.print("0");
        Serial.print(msg[i]);
        Serial.print(" ");
    }
}


// Takes the data and stores the correct hash in correct_hash
void GetCorrectHash()
{
    uint8_t keyLen;
    uint8_t key[100];
    GetKey(id, &key[0], keyLen); // stores length of key in keyLen, and key is filled
    spritz_mac(&correct_hash[0], HASH_LEN, &stmp[0], dlc, key, keyLen); // create correct hash
}


void PrintMessage()
{
    Serial.print("ID: ");
    Serial.println(id, HEX);
    Serial.print("DLC: ");
    Serial.println(dlc, HEX);
    Serial.print("Data: ");
    for (int i = 0; i < dlc; i++)
    {
//      if (stmp[i] < 0x10)
//          Serial.print("0");
      Serial.print(stmp[i]);
      Serial.print(" ");
    }
    Serial.print("\n\r-------------------------------------------\n\r");
}


//---------------------------------------------------------------------------------------
// Generate a random CAN message
//---------------------------------------------------------------------------------------
void GenerateMessage(uint32_t &id, uint8_t &dlc, unsigned char *data)
{
    int selector = random(3);
    id = SelectMessage(selector);
    dlc = random(9);  // dlc = random int between 0..8
    for(int i = 0; i < dlc; i++)
    {
      data[i] = random(0x100); // each byte is random char between 0..0xFF
    }
    for(int i = dlc; i < 8; i++)
    {
      data[i] = 0;
    }
    dlc = 8;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
