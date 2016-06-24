#include <MD5.h>
#include <mcp_can.h>
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>

const int SPI_CS_PIN = 9;
const int HASH_LEN = 16;

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
String mil_key = "mil key";
String door_key = "door key";
String engine_key = "engine key";

const uint32_t MIL_ID = 0x7e0;
const uint32_t DOOR_ID = 0x7d0;
const uint32_t ENGINE_ID = 0x7a0;

// Incoming data variables
uint32_t id;
uint8_t dlc;
uint8_t data[8] = {0};

MCP_CAN CAN(SPI_CS_PIN);

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))
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
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, LOW);
}

void loop()
{
    if (CAN_MSGAVAIL == CAN.checkReceive())   // check if data coming
    {
        // Store message data
        id = CAN.getCanId(); 
        CAN.readMsgBufID(&id, &dlc, data); // read buffer
        Serial.println("------------------------------------------\n");

        // Authenticate the message
        if (Authenticate())
        {
            // Display the message
            PrintMessage();

            Serial.println("\nAuthentication Successful");
            TakeAction();
        }
    }
}


String GetKey()
{
    switch(id)
    {
        case MIL_ID:
            return mil_key;
        case DOOR_ID:
            return door_key;
        case ENGINE_ID:
            return engine_key;
        default:
            return "";
    }
}

bool CompareAuthMessage(uint8_t recvd[], uint8_t expected[], int start=0, int quit=8)
{
    int err = 0;
    for(int i = start; i < quit; i++)
    {
        if (recvd[i] != expected[i])
            return false;
    }
    return true;
}


bool Authenticate()
{
    // For storing incoming digest
    uint8_t msg1[8] = {0};
    uint8_t msg2[8] = {0};
    uint8_t msg3[8] = {0};
    uint8_t len;

    // Get the correct key according to the id
    String key = GetKey();

    // Generate the correct digest according to the data
    String message = "";
    for(int i = 0; i < dlc; i++)
    {
        if (data[i] < 0x10)
            message += '0';
        message += String(data[i], HEX);
    }
    message.toLowerCase();

    Serial.print("Hashing: ");
    Serial.println(message + key);
    unsigned char *hash = MD5::make_hash((char *)((message + key).c_str()));
    char *sdigest = MD5::make_digest(hash, HASH_LEN);

    // Convert the correct hash to uint8_t[]
    uint8_t digest[HASH_LEN] = {0};
    Serial.print("Converted: ");
    for(int i = 0, j = 0; i < HASH_LEN*2; i+=2, j++)
    {
        digest[j] = CharToUInt8(sdigest[i]) * 16;
        digest[j] += CharToUInt8(sdigest[i+1]);
        Serial.print(digest[i], HEX);
        Serial.print(' ');
    }
    Serial.println();

    // Create the 3 expected auth messages
    uint8_t correct_msg1[8] = {0};
    uint8_t correct_msg2[8] = {0};
    uint8_t correct_msg3[8] = {0};
    for(int i = 0; i < 8; i++)
    {
        correct_msg1[i] = digest[i];
        correct_msg2[i] = digest[i+8];
        if (i < 4)
            correct_msg3[i] = digest[i+16];
    }

    // Read in the three sections of the digest and check them
    CAN.readMsgBuf(&len, msg1);
    if (!CompareAuthMessage(msg1, correct_msg1)) Serial.println("MSG1 fail");
    CAN.readMsgBuf(&len, msg2);
    if (!CompareAuthMessage(msg2, correct_msg2)) Serial.println("MSG2 fail");
    CAN.readMsgBuf(&len, msg3);
    if (!CompareAuthMessage(msg3, correct_msg3)) Serial.println("MSG3 fail");

    Serial.print("Good Digest: ");
    Serial.println(sdigest);
    return true;
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



