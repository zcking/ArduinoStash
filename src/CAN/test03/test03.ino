#include <mcp_can.h>
#include <mcp_can_dfs.h>

// Simple CAN Shield Test
// Note: this program uses roughly 20% of the 
// alloted 32 KB program memory on the arduino

#include <stdlib.h>
#include "mcp_can.h"
#include <SPI.h>

MCP_CAN CAN0(10);     // Set CS pin to 10

// Constants
#define MAX_CMD_LENGTH 60

#define CR "\n\r"
#define CRCR "\n\r\n\r"


void setup()
{
  // Set the baud rate
  Serial.begin(115200);

  // Initialize the CAN Controller
  // Baud rates defined in mcp_can_dfs.h
  if (CAN0.begin(CAN_250KBPS) == CAN_OK)
    Serial.print("CAN Init Ok.\n\r\n\r");
  else
    Serial.print("CAN Init Failed.\n\r");
} // end setup()



void loop()
{
  // Check for a received CAN message and print it to the Serial Monitor
  SubCheckCANMessage();

  // Check for a command from the Serial Monitor and send message as entered
  SubSerialMonitorCommand();
} // end loop()


//-----------------------------------------------------------------
// Check for CAN message and print it to the Serial Monitor
//-----------------------------------------------------------------
void SubCheckCANMessage()
{
  // Declarations
  byte nMsgLen = 0;
  byte nMsgBuffer[8];
  char sString[4];

  if (CAN0.checkReceive() == CAN_MSGAVAIL)
  {
    // Read the message buffer
    CAN0.readMsgBuf(&nMsgLen, &nMsgBuffer[0]);
    INT32U nMsgID = CAN0.getCanId();

    // Print message ID to serial monitor
    Serial.print("Message ID: 0x");
    if (nMsgID < 16) Serial.print("0");
    Serial.print(itoa(nMsgID, sString, 16));
    Serial.print("\n\r");

    // Print data to serial monitor
    Serial.print("Data: ");
    for (int i = 0; i < nMsgLen; i++)
    {
      Serial.print("0x");
      if (nMsgBuffer[i] < 16) Serial.print("0");
      Serial.print(itoa(nMsgBuffer[i], sString, 16));
      Serial.print(" ");
    } // end for
    Serial.print(CRCR);
  } // end if 
} // end SubCheckCANMessage


//-----------------------------------------------------------------
// Check for command from Serial Monitor
//-----------------------------------------------------------------
void SubSerialMonitorCommand()
{
  // Declarations
  char sString[MAX_CMD_LENGTH+1];
  bool bError = true;

  unsigned long nMsgID = 0xFFFF;
  byte nMsgLen = 0;
  byte nMsgBuffer[8];

  // Check for command from Serial Monitor
  int nLen = nFctReadSerialMonitorString(sString);

  if (nLen > 0)
  {
    // A string was received from serial monitor
    if (strncmp(sString, "#SM ", 4) == 0)
    {
      // The first 4 characters are acceptable
      // We need at least 10 characters to read the ID and data number
      if (strlen(sString) >= 10)
      {
        // Determine message ID and number of data bytes
        nMsgID = lFctCStringLong(&sString[4], 4);
        nMsgLen = (byte)nFctCStringInt(&sString[9], 1);

        if (nMsgLen >= 0 && nMsgLen <= 8)
        {
          // Check if there are enough bytes of data
          int nStrLen = 10 + nMsgLen * 3; // Expected msg length
          if (strlen(sString) >= nStrLen) // Larger length is acceptable
          {
            int nPointer;
            for(int i = 0; i < nMsgLen; i++)
            {
              nPointer = i * 3; // Blank character plus two numbers
              nMsgBuffer[i] = (byte)nFctCStringInt(&sString[nPointer + 11], 2);
            } // end for

            // Reset the error flag
            bError = false;

            // Everything okay; send the message
            CAN0.sendMsgBuf(nMsgID, CAN_STDID, nMsgLen, nMsgBuffer);

            // Repeat the entry on the Serial Monitor
            Serial.print(sString);
            Serial.print(CRCR);
          } // end if
        } // end if
      } // end if
    } // end if

    // Check for entry error
    if (bError)
    {
      Serial.print("???: ");
      Serial.print(sString);
      Serial.print(CR);
    }
  } // end if
} // end SubSerialMonitorCommand()


//-------------------------------------------------------------------
// Read message from Serial Monitor
//-------------------------------------------------------------------
// Returns string length
byte nFctReadSerialMonitorString(char* sString)
{
  // Declarations
  byte nCount = 0;

  if (Serial.available() > 0)
  {
    Serial.setTimeout(100);
    nCount = Serial.readBytes(sString, MAX_CMD_LENGTH);
  } // end if

  // Terminate the string
  sString[nCount] = 0;

  return nCount;
} // end nFctReadSerialMonitorString()


//-------------------------------------------------------------------
// Convert string to int
//-------------------------------------------------------------------
// Note: nLen MUST be between 1 and 4
// Returns integer value (-1 indicates an error in the string
int nFctCStringInt(char *sString, int nLen)
{
  // Declarations
  int nNum;
  int nRetCode = 0;

  // Check the string length
  if (strlen(sString) < nLen)
    nRetCode = -1;
  else
  {
    // String length okay; convert to integer
    int nShift = 0;
    for (int i = nLen - 1; i >= 0; i--)
    {
      if (sString[i] >= '0' && sString[i] <= '9')
        nNum = int(sString[i] - '0');
      else if (sString[i] >= 'A' && sString[i] <= 'F')
        nNum = int(sString[i] - 'A') + 10;
      else
        goto nFctCStringInt_Ret;

      nNum = nNum << (nShift++ * 4);
      nRetCode = nRetCode + nNum;
    } // end for
  } // end else

  // Return the result
nFctCStringInt_Ret:
  return nRetCode;
} // end nFctCStringInt()



//-------------------------------------------------------------------
// Convert string into unsigned long
//-------------------------------------------------------------------
// Note: nLen MUST be between 1 and 4
// Returns integer value (-1 indicates error in string)
unsigned long lFctCStringLong(char *sString, int nLen)
{
  // Declarations
  unsigned long nNum;
  unsigned long nRetCode = 0;

  // Check the string length
  if (strlen(sString) < nLen)
    nRetCode = -1;
  else
  {
    // String length okay; convert to long
    unsigned long nShift = 0;
    for (int i = nLen - 1; i >= 0; i--)
    {
      if (sString[i] >= '0' && sString[i] <= '9')
        nNum = int(sString[i] - '0');
      else if (sString[i] >= 'A' && sString[i] <= 'F')
        nNum = int(sString[i] - 'A') + 10;
      else goto lFctCStringLong_Ret;

      nNum = nNum << (nShift++ * 4);
      nRetCode = nRetCode + nNum;
    } // end for
  } // end else

  // Return the result
lFctCStringLong_Ret:
  return nRetCode;
} // end lFctCStringLong()

