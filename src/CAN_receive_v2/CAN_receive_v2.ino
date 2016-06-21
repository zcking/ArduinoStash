// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13
// Use http://www.seeedstudio.com/wiki/CAN-BUS_Shield if need help

#include <SPI.h>
#include <SpritzCipher.h>
#include "mcp_can.h"
#include "can_constants.h"


// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

uint8_t dlc = 0; // dlc
uint8_t data[8]; // data buffer
uint32_t id = 0; // CAN ID


// "Control Units" -- LED Lights to be controlled
const int blueLED = 4;
const int greenLED = 3;
const int RED = 7;
const int GREEN = 5;
const int BLUE = 6;

// For the RGB LED
int redValue = 255;
int blueValue = 0;
int greenValue = 0;

// For authentication
const uint8_t HASH_LEN = 20; // 160-bit hash
uint8_t correct_hash[HASH_LEN]; 
uint8_t msg1[8];
uint8_t msg2[8];
uint8_t msg3[4];

void setup()
{
    InitMessages();
    Serial.begin(115200);             // serial interface baud rate (typical rate for CAN Bus)

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(200);
    }
    Serial.println("CAN BUS Shield init ok!");

    pinMode(blueLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, HIGH);
    digitalWrite(GREEN, HIGH);
}


void loop()
{
    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        // Read the actual message first
        id = CAN.getCanId(); // get CAN ID
        CAN.readMsgBuf(&dlc, data);    // read data,  len: data length, buf: data buf 

        // Store the correct hash according to the received data
        GetCorrectHash();

        // Read in the Authentication messages and check if valid
        if (Authenticate())
        {
            // It's valid, so take action
            PrintMessage();
            TakeAction();
        }
        else
        {
            // Invalid!
            Serial.println("\n**UNAUTHORIZED MESSAGE**\n");
        }
    }
}


// Reads three CAN messages as authenticator messages
bool Authenticate()
{
    // Fill the 3 msg buffers and check them as they are read in for efficiency
    CAN.readMsgBuf(&dlc, msg1);
    //if (!CheckAuthMessage(&msg1[0], 0, 8)) return false;
    CAN.readMsgBuf(&dlc, msg2);
    //if (!CheckAuthMessage(&msg2[0], 8, 8)) return false;
    CAN.readMsgBuf(&dlc, msg3);
    //if (!CheckAuthMessage(&msg3[0], 16, 4)) return false;
    for(int i = 0; i < 8; i++)
    {
      Serial.print(msg1[i]);
      Serial.print(".");
    }
    for(int i = 0; i < 8; i++)
    {
      Serial.print(msg2[i]);
      Serial.print(".");
    }
    for(int i = 0; i < 4; i++)
    {
      Serial.print(msg3[i]);
      Serial.print(".");
    }
    Serial.print('\n');
    for(int i = 0; i < 20; i++)
    {
      Serial.print(correct_hash[i]);
      Serial.print('.');
    }
    Serial.println('---------------------');
    return true;
}


// Stores the correct hash in correcct_hash
void GetCorrectHash()
{
    uint8_t keyLen;
    uint8_t key[100];
    GetKey(id, &key[0], keyLen); // get key and key length for this is
    spritz_mac(&correct_hash[0], HASH_LEN, &data[0], dlc, key, keyLen); // create correct hash
}


// Checks an auth message against correct_hash
bool CheckAuthMessage(uint8_t *buf, int start, int bufLen)
{
    for (int i = 0; i < bufLen; i++)
    {
      if (buf[i] != correct_hash[i + start])
          return false;
    }
    return true;
}


void TakeAction()
{
    switch(id)
    {
      case MIL_ID:
        if (dlc > 0 && data[0] > 0)
            digitalWrite(greenLED, HIGH);
        else
            digitalWrite(greenLED, LOW);
        break;
      case DOOR_ID:
        if (dlc > 0 && data[0] > 0)
            digitalWrite(blueLED, HIGH);
        else 
            digitalWrite(blueLED, LOW);
        break;
      case ENGINE_ID:
        if (dlc >= 3)
        {
          redValue = data[0];
          blueValue = data[2];
          greenValue = data[1];
        }
        else if (dlc == 2)
        {
          redValue = data[0];
          greenValue = data[1];
          blueValue = 0;
        }
        else if (dlc == 1)
        {
          redValue = data[0];
          blueValue = 0;
          greenValue = 0;
        }
        else
        {
          redValue = 0;
          greenValue = 0;
          blueValue = 0;
        }
        redValue = redValue % 255;
        greenValue = greenValue % 255;
        blueValue = blueValue % 255;
        analogWrite(RED, redValue);
        analogWrite(GREEN, greenValue);
        analogWrite(BLUE, blueValue);
    }
}


//---------------------------------------------------------------------------------------
// Prints the CAN message to the Serial interface
//---------------------------------------------------------------------------------------
void PrintMessage()
{
    Serial.println("-----------------------------");
    Serial.println("Received Message:");
    Serial.print("\tID ---> ");
    Serial.println(id, HEX);
    Serial.print("\tDLC --> ");
    Serial.println(dlc);
    Serial.print("\tDATA -> ");

    for(int i = 0; i < dlc; i++)    // print the data
    {
        if (data[i] < 0x10)
            Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
