// Simple CAN Shield Test (extended)
#include <stdlib.h>
#include "mcp_can.h"
#include <SPI.h>

MCP_CAN CAN0;     // Set CS pin to 10

// Test message
unsigned char stmp[8] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

// SYSTEM: Setup routine runs on power-up or reset
void setup()
{
  // Set the serial interface baud rate
  Serial.begin(9600);

  // Initialize the CAN Controller
  // Baud rates defined in mcp_can_dfs.h
  if (CAN0.begin(CAN_250KBPS) == CAN_OK)
    Serial.print("CAN Init OK.\n\r\n\r");
  else
    Serial.print("CAN Init Failed.\n\r");
} // end setup()


// Main Loop - Arduino Entry Point
void loop()
{
  // Declarations
  byte nMsgLen = 0;
  byte nMsgBuffer[8];
  char sString[4];

  // Send a test message
  // Send data: id = 0x1FF, extended frame, data len = 8, stmp: data buf
  // ID mode (11/29 bit) defined in mcp_can_dfs.h
  CAN0.sendMsgBuf(0x1FF, CAN_EXTID, 8, stmp);

  // Check for a message
  if (CAN0.checkReceive() == CAN_MSGAVAIL)
  {
    // Read the message buffer
    CAN0.readMsgBuf(&nMsgLen, &nMsgBuffer[0]);
    INT32U nMsgID = CAN0.getCanId();

    // Print message ID to serial monitor
    Serial.print("Message ID: 0x");
    if (nMsgID < 16)
      Serial.print("0");
    Serial.print(itoa(nMsgID, sString, 16));
    Serial.print("\n\r");

    // Print data to serial monitor
    Serial.print("Data: ");
    for (int i = 0; i < nMsgLen; i++)
    {
      Serial.print("0x");
      if (nMsgBuffer[i] < 16)
        Serial.print("0");
      Serial.print(itoa(nMsgBuffer[i], sString, 16));
      Serial.print(" ");
    } // end for
    Serial.print("\n\r\n\r");
  } // end if

  // Run in 1 sec intervals
  delay(1000);
} // end loop()

