// demo: CAN-BUS Shield, send data
// Use http://www.seeedstudio.com/wiki/CAN-BUS_Shield if need help
#include <mcp_can.h>
#include <SPI.h>
#include "can_constants.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

// Variables
uint32_t id = 0x100;
uint8_t dlc = 8;
unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

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
    // Generate a random CAN message
    GenerateMessage(id, dlc, &stmp[0]);
    PrintMessage();
   
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(id, 0, dlc, stmp);
    delay(1000);                       // send data per 100ms
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
      if (stmp[i] < 0x10)
          Serial.print("0");
      Serial.print(stmp[i], HEX);
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
}


//---------------------------------------------------------------------------------------
// Sign the message with appropriate key for this node
//---------------------------------------------------------------------------------------
void SignMessage(int &id, int &dlc, unsigned char *data)
{
    // TODO
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
