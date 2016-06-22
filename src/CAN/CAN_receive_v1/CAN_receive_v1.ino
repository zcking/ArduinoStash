// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13
// Use http://www.seeedstudio.com/wiki/CAN-BUS_Shield if need help

#include <SPI.h>
#include <mcp_can.h>
#include <Time.h>
#include "can_constants.h"


// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

unsigned char dlc = 0; // dlc
unsigned char data[8]; // data buffer
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

// For data collection
unsigned long numReceived = 0;
int timer = 0;
int oldTime = 0;
int sec = 0;

void setup()
{
    InitMessages();
    Serial.begin(115200);             // serial interface baud rate (typical rate for CAN Bus)

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
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
        id = CAN.getCanId(); // get CAN ID
        CAN.readMsgBufID(&id, &dlc, data);    // read data,  len: data length, buf: data buf        

        // Increment counter
        numReceived++;
        timer = second(now());
        if (timer > oldTime)
        {
            oldTime = timer;
            sec++; // current second
            Serial.print(sec);
            Serial.print(":");
            Serial.println(numReceived);
            numReceived = 0;
        }
        
        //PrintMessage();
        TakeAction();
    }
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


//---------------------------------------------------------------------------------------
// Check/Authenticate the message received
//---------------------------------------------------------------------------------------
bool Authenticate(int id, int dlc, unsigned char *data)
{
    // TODO
    return true;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
