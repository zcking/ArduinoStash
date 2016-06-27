// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13
// Use http://www.seeedstudio.com/wiki/CAN-BUS_Shield if need help

#include <SPI.h>
#include <mcp_can.h>
#include <SpritzCipher.h>
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

// For the RGB LED
const int RED = 7;
const int GREEN = 5;
const int BLUE = 6;

// For the RGB LED
int redValue = 255;
int blueValue = 0;
int greenValue = 0;

// For authentication
const int HASH_LEN = 20; // 160-bit hash
uint8_t correct_hash[HASH_LEN];
uint8_t incoming_hash[HASH_LEN];

// For timestamp verification 
unsigned long mil_timestamp = 0;
unsigned long door_timestamp = 0;
unsigned long engine_timestamp = 0;
const unsigned long MIN_FREQUENCY = 0; // how many milliseconds apart a message has to be from its replay

// For data collection - quantity of messages received
unsigned long numReceived = 0;
int timer = 0;
int oldTime = 0; 
int sec = 0;

void setup()
{
    InitMessages();     // Initialize valid CAN messages in can_constants.h
    Serial.begin(115200);    // serial interface baud rate (typical rate for CAN Bus)

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
    }
    Serial.println("CAN BUS Shield init ok!");

    // Initialize all the LEDs
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
//        Serial.println("\n------------------------------\n");

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

        // Authenticate the message (both HMAC and Frequency check)
        if (Authenticate())
        {
          // Display the message
//          PrintMessage(); 
          
//          Serial.println("\nAUTHENTICATION SUCCESSFUL");
          TakeAction();     // only respond to authorized messages
        }
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
bool Authenticate()
{
    // For storing incoming hash data
    uint8_t msg1[8];
    uint8_t msg2[8];
    uint8_t msg3[8];
    uint8_t len;
    
    // Get the Correct Key according to the current data
    uint8_t key[100] = {0}; // buffer for the key
    uint8_t keyLen;
    GetKey(id, &key[0], keyLen);

//    Serial.print("Expected Key: ");
//    for(int i = 0; i < keyLen; i++)
//    {
//        Serial.print(key[i]);
//        Serial.print(" ");
//    }
//    Serial.println();

    // Generate the correct hash according to the data
    spritz_mac(&correct_hash[0], HASH_LEN, &data[0], dlc, &key[0], keyLen);
    
    // Read in the three hash messages
    CAN.readMsgBuf(&len, msg1); // msg1 (first 8 bytes)
    CAN.readMsgBuf(&len, msg2); // msg2 (last 8 bytes [4 for timestamp])
    CAN.readMsgBuf(&len, msg3); // msg3 (second 8 bytes - order is weird)

    // Verify the timestamp/freqency
    if (!VerifyTimestamp(&msg2[4]))
    {
        //Serial.println("Failed Frequency Check");
        return false;
    }

    // Fill the incoming_hash 
    for (int i = 0; i < 8; i++)
    {
      incoming_hash[i] = msg1[i];
      incoming_hash[i + 8] = msg3[i];
      if (i < 4)
        incoming_hash[i + 16] = msg2[i];
    }

    // Print the incoming_hash
//    Serial.print("Received Hash: ");
//    for (int i = 0; i < HASH_LEN; i++)
//    {
//        if (incoming_hash[i] < 0x10)
//          Serial.print('0');
//        Serial.print(incoming_hash[i], HEX);
//        Serial.print(' ');
//    }
//    Serial.print("\nExpected Hash: ");
//    for(int i = 0; i < HASH_LEN; i++)
//    {
//      if (correct_hash[i] < 0x10)
//        Serial.print('0');
//      Serial.print(correct_hash[i], HEX);
//      Serial.print(' ');
//    }
//    Serial.println();
    return (spritz_compare(&correct_hash[0], &incoming_hash[0], HASH_LEN) == 0);
}


bool VerifyTimestamp(uint8_t *buf)
{
    // Unpack the 4 bytes in buf to store as milliseconds
    unsigned long ms = buf[0] << 24;
    ms += buf[1] << 16;
    ms += buf[2] << 8;
    ms += buf[3];

    // Get the appropriate stored timestamp to compare against
    unsigned long *lastStamp;
    if (id == MIL_ID)
      lastStamp = &mil_timestamp;
    else if (id == DOOR_ID)
      lastStamp = &door_timestamp;
    else if (id == ENGINE_ID)
      lastStamp = &engine_timestamp;

//    Serial.print("Received Timestamp: ");
//    Serial.println(ms);
//    Serial.print("Last Timestamp: ");
//    Serial.println(*lastStamp);

    if (*lastStamp > ms) // ms must have exceded max value and started over 
    {
        *lastStamp = ms;
        return true;
    }
    
    if ((*lastStamp + MIN_FREQUENCY) <= ms)
    {
        *lastStamp = ms;
        return true;
    }
    return false;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
