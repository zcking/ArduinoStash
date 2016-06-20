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

String message;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(115200); // serial interface baud rate 115200 (typical rate for CAN)

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(200);
    }
    Serial.println("CAN BUS Shield init ok!\n");
    Serial.println("-- MESSAGES --");
    Serial.println("ID\tCONTROL");
    Serial.println("----------------");
    Serial.println("160\tRGB LED");
    Serial.println("208\tBlue LED");
    Serial.println("224\tGreen LED");
    Serial.println("----------------");
}

void loop()
{
    while(Serial.available())
    {
        id = Serial.parseInt();
        dlc = Serial.parseInt();
        for (int i = 0; i < dlc; i++)
            stmp[i] = Serial.parseInt();
        CAN.sendMsgBuf(id, 0, dlc, stmp);
        Serial.print(id, HEX);
        Serial.print("#0");
        Serial.print(dlc);
        for (int i = 0; i < dlc; i++)
        {
          if (stmp[i] < 0x10)
              Serial.print("0");
          Serial.print(stmp[i], HEX);
        }
        Serial.print("\n---------------------------------------\n");
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
