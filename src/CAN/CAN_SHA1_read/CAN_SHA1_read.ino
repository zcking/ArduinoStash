#include <Time.h>
#include "sha1.h"
#include <mcp_can.h>
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>


const int HASH_LEN = 20;

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

// Shared keys - pretend these are stored
// in a "Secure Hardware Module"
// CONSTANTS
const int SPI_CS_PIN = 9;
const uint8_t *MIL_KEY = (const uint8_t *)"mil key";
const uint8_t *DOOR_KEY = (const uint8_t *)"door key";
const uint8_t *ENGINE_KEY = (const uint8_t *)"engine key";
const uint8_t *TOGGLE_AUTH_KEY = (const uint8_t *)"auth key";
const int MIL_KEY_LEN = 7;
const int DOOR_KEY_LEN = 8;
const int ENGINE_KEY_LEN = 10;
const int TOGGLE_AUTH_KEY_LEN = 8;
const int MIL_ID = 0x7e0;
const int DOOR_ID = 0x7d0;
const int ENGINE_ID = 0x7a0;
const int TOGGLE_AUTH_ID = 0x700;

// Incoming data variables
uint32_t id;
uint8_t dlc;
uint8_t data[8] = {0};
uint8_t *hash;
unsigned long ts;

// For timestamp verification 
unsigned long mil_timestamp = 0;
unsigned long door_timestamp = 0;
unsigned long engine_timestamp = 0;
const unsigned long MIN_FREQUENCY = 5000; // how many milliseconds apart a message has to be from its replay

// For data collection - quantity of messages received
unsigned long numReceived = 0;
int timer = 0;
int oldTime = 0; 
int sec = 0;
int counter = 0;
int successful = 0;
int totalReceived = 0, totalSuccessful = 0;

const bool MEASURE_RECEIVED_BYTES = false;
const bool MEASURE_TIME_TO_AUTH = false;
const bool SHOW_NORMAL_OUTPUT = true;
const bool MEASURE_SUCCESSFUL = false;
bool SHOULD_AUTHENTICATE = true;

MCP_CAN CAN(SPI_CS_PIN);

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        if (SHOW_NORMAL_OUTPUT)
        {
            Serial.println("CAN BUS Shield init fail");
            Serial.println(" Init CAN BUS Shield again");
        }
    }
    if (SHOW_NORMAL_OUTPUT)
        Serial.println("CAN BUS Shield init ok!");

    // Initialize all the LEDs
    pinMode(blueLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, LOW);
}



void loop()
{
    if (CAN_MSGAVAIL == CAN.checkReceive())   // check if data coming
    {
        totalReceived++;
        
        // Store message data
        id = CAN.getCanId(); 
        CAN.readMsgBufID(&id, &dlc, data); // read buffer

        // Increment counter
        if (MEASURE_RECEIVED_BYTES)
        {
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
         }

        // Authenticate the message
        unsigned long start = micros();
//        ClearCANBuffer();
        if (SHOULD_AUTHENTICATE)
        {
            if (Authenticate())
            {
                totalSuccessful++;
                
                 // Display the message
                if (SHOW_NORMAL_OUTPUT)
                {
                    PrintMessage();
                    Serial.println("\nAuthentication Successful");
                }
                TakeAction();
            }
            else // failed authentication
            {
                if (SHOW_NORMAL_OUTPUT)
                {
                    Serial.println("\nAuthentication FAILED");
                }
            }
        }
        else // not doing authentication
        {
            if (SHOW_NORMAL_OUTPUT)
            {
                PrintMessage();
            }
            TakeAction();
        }

        Serial.println("------------------------------------------\n");
        
        unsigned long delta = micros() - start;
        counter++;
        if (MEASURE_TIME_TO_AUTH)
        {
            Serial.print(counter);
            Serial.print(':');
            Serial.println(delta);
        }

        if (MEASURE_SUCCESSFUL)
        {
            Serial.print(totalSuccessful);
            Serial.print('/');
            Serial.println(totalReceived);
        }
    }
}


bool ClearCANBuffer()
{
    id = CAN.getCanId(); 
    CAN.readMsgBufID(&id, &dlc, data);
    CAN.readMsgBufID(&id, &dlc, data);
    CAN.readMsgBufID(&id, &dlc, data);
    return true;
}

const uint8_t * GetKey()
{
    switch(id)
    {
        case MIL_ID:
            return MIL_KEY;
        case DOOR_ID:
            return DOOR_KEY;
        case ENGINE_ID:
            return ENGINE_KEY;
        case TOGGLE_AUTH_ID:
            return TOGGLE_AUTH_KEY;
        default:
            return 0;
    }
}

int GetKeyLen()
{
    switch(id)
    {
        case MIL_ID:
          return MIL_KEY_LEN;
        case DOOR_ID:
          return DOOR_KEY_LEN;
        case ENGINE_ID:
          return ENGINE_KEY_LEN;
        case TOGGLE_AUTH_ID:
          return TOGGLE_AUTH_KEY_LEN;
        default:
          return -1;
    }
}

bool CompareAuthMessage(uint8_t recvd[], int msgLen=0, int hashOffset=0, int msgOffset=0)
{
    for(int i = 0; i < msgLen; i++)
    {
        if (recvd[i+msgOffset] != hash[i+hashOffset])
        {
            Serial.print(recvd[i+msgOffset], HEX);
            Serial.print(" != ");
            Serial.println(hash[i+hashOffset], HEX);
            return false;
        }
    }
    return true;
}


bool Authenticate()
{
    // Get the correct key according to the id
    const uint8_t *key = GetKey();
    int keyLen = GetKeyLen();

    // Generate the correct digest according to the data
    String message = "";
    for(int i = 0; i < dlc; i++)
    {
        if (data[i] < 0x10)
            message += '0';
        message += String(data[i], HEX);
    }
    message.toLowerCase();

    // For storing incoming digest
    uint8_t msg1[8] = {0};
    uint8_t msg2[8] = {0};
    uint8_t msg3[8] = {0};
    uint8_t rec_hash[20] = {0};
    uint8_t len;

    // Read in the three sections of the digest and check them
    while (CAN_MSGAVAIL != CAN.checkReceive())
        continue;
    CAN.readMsgBuf(&len, msg1);
    // store timestamp
    ts = (unsigned long)(msg1[0] << 24);
    ts += (unsigned long)(msg1[1] << 16);
    ts += (unsigned long)(msg1[2] << 8);
    ts += (unsigned long)(msg1[3]);

    Sha1.initHmac(key, keyLen);
    WriteBytes((const uint8_t *)message.c_str(), message.length());
    Sha1.write(msg1[0]); // include the timestamp in the hash
    Sha1.write(msg1[1]);
    Sha1.write(msg1[2]);
    Sha1.write(msg1[3]);
//    Serial.print("Timestamp used in hash: ");
//    Serial.println(ts);
    uint8_t *correct_hash;
    hash = Sha1.resultHmac();
//    Serial.print("Expected Digest: ");
//    PrintHash(hash);


//    Serial.println("Running Comparisons now...");
//    PrintMessage(id, len, &msg1[0]);
    if (!CompareAuthMessage(msg1, 4, 0, 4)) return false;
    
    while (CAN_MSGAVAIL != CAN.checkReceive())
        continue;
    CAN.readMsgBuf(&len, msg2);
//    PrintMessage(id, len, &msg2[0]);
    if (!CompareAuthMessage(msg2, 8, 4)) return false;
    
    while (CAN_MSGAVAIL != CAN.checkReceive())
        continue;
    CAN.readMsgBuf(&len, msg3);
//    PrintMessage(id, len, &msg3[0]);
    if (!CompareAuthMessage(msg3, 8, 12)) return false;

    // Verify the timestamp/frequency
    if (!VerifyTimestamp(&msg1[0]))
    {
        if (SHOW_NORMAL_OUTPUT)
            Serial.println("Failed Frequency Check");
        return false;
    }

    if (SHOW_NORMAL_OUTPUT)
    {
        Serial.print("Received Correct Digest: ");
        PrintHash(hash);
    }
    return true;
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


void WriteBytes(const uint8_t *data, int len)
{
    for(int i = 0; i < len; i++)
    {
        Sha1.write(data[i]);
    }
}


void PrintHash(uint8_t *hash)
{
    for (int i = 0; i < 20; i++)
    {
        if (hash[i] < 0x10)
            Serial.print('0');
        Serial.print(hash[i], HEX);
    }
    Serial.println();
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


void PrintMessage(uint32_t ident, uint8_t dataLen, uint8_t *buf)
{
    Serial.println("Received Message:");
    Serial.print("\tID ---> ");
    Serial.println(ident, HEX);
    Serial.print("\tDLC --> ");
    Serial.println(dataLen);
    Serial.print("\tDATA -> ");

    for(int i = 0; i < dataLen; i++)    // print the data
    {
        if (buf[i] < 0x10)
            Serial.print("0");
        Serial.print(buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
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
        break;

      case TOGGLE_AUTH_ID:
        SHOULD_AUTHENTICATE = !SHOULD_AUTHENTICATE;
        if (SHOULD_AUTHENTICATE)
          Serial.println("--- Authentication ENABLED ---");
        else
          Serial.println("--- Authentication DISABLED ---");
        break;
    }
}


void CStringHexToUInt8(char *str, uint8_t *buf)
{
    for(int i = 0, j = 0; i < 40; i+=2, j++)
    {
        buf[j] = CharByteToUInt8(str[i], str[i+1]);
        PrintHexByte(buf[j]);
        Serial.print(' ');
    }
    Serial.println();
}


void PrintHexByte(uint8_t val)
{
    if (val < 0x10)
        Serial.print('0');
    Serial.print(val, HEX);
}


uint8_t *HexToUInt8(char *hexStr, uint8_t &len)
{
    len = strlen(hexStr);
    uint8_t *buf = new uint8_t[len];
    int bufIndex = 0;

    // Convert each pair of chars (a byte)
    for (int i = 0; i < len; i+=2)
    {
        buf[bufIndex] = CharToUInt8(hexStr[i]) * 16;
        buf[bufIndex] += CharToUInt8(hexStr[i+1]);
        bufIndex++;
    }
    len = len / 2;
    
    return buf;
}


uint8_t *CStringToUInt8(char *str, uint8_t &len)
{
    len = strlen(str);
    uint8_t *buf = new uint8_t[len];

    for(int i = 0; i < len; i++)
    {
        // store and convert from ASCII to integer value
        buf[i] = (uint8_t)str[i];
    }

    return buf;
}

uint8_t CharToUInt8(char ch)
{
    uint8_t value = ch;
    if (ch >= '0' && ch <= '9')
        value -= '0';
    else if (ch >= 'a' && ch <= 'f')
        value = (ch - 'a') + 10;
    else
        value = (ch - 'A') + 10;
    return value;
}


char UInt8ToChar(uint8_t x)
{
    char ch = (char)x;
    if (x >= 0 && x <= 9)
        ch += '0';
    else if (x >= 10 && x <= 15)
        ch += 'a';
    return ch;
}

uint8_t CharByteToUInt8(char ch1, char ch2)
{
    uint8_t value = CharToUInt8(ch1);
    value *= 16;
    value += CharToUInt8(ch2);
    return value;
}
