// Simple CAN Shield Test
#include "mcp_can.h"
#include <SPI.h>

MCP_CAN CAN0; // Set CS pin to 10; if using Seeed CAN Bus Shield, change to 9

unsigned char stmp[8] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

// SYSTEM: Setup routine runs on powerup or reset
void setup()
{
  // Set the Serial interface baud rate 
  Serial.begin(9600);

  // Initialize the CAN Controller
  // Baud rates are defined in mcp_can_dfs.h
  if (CAN0.begin(CAN_250KBPS) == CAN_OK)
    Serial.print("CAN Init OK.\n\r\n\r");
  else
    Serial.print("CAN Init Failed.\n\r");
} // end setup()


// Main Loop - Arduino Entry Point
void loop()
{
  // Send data: id = 0x1FF, extended frame, data len = 8, stmp; data buf
  // ID mode (11/29 bit) defined in mcp_can_dfs.h
  CAN0.sendMsgBuf(0x1FF, CAN_EXTID, 8, stmp);

  // Run in 1 sec interval
  delay(1000);
} // end loop()

