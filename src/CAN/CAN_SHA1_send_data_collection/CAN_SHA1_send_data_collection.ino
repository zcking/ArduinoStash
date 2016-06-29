#include <Time.h>
#include "sha1.h"
#include <mcp_can.h>
#include <SPI.h>

// CONSTANTS
const int SPI_CS_PIN = 9;
const uint8_t *MIL_KEY = (const uint8_t *)"mil key";
const uint8_t *DOOR_KEY = (const uint8_t *)"door key";
const uint8_t *ENGINE_KEY = (const uint8_t *)"engine key";
const int MIL_KEY_LEN = 7;
const int DOOR_KEY_LEN = 8;
const int ENGINE_KEY_LEN = 10;
const int MIL_ID = 0x7e0;
const int DOOR_ID = 0x7d0;
const int ENGINE_ID = 0x7a0;


// Variables

// For HMAC
uint8_t *hash;


// For data collection
unsigned long numReceived = 0;
int timer = 0;
int oldTime = 0;
int sec = 0;
int counter = 0;

MCP_CAN CAN(SPI_CS_PIN);

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
    }
}




void loop()
{
    String sid = "7a0";
    String message = "020a0a";
    String skey = "engine key";

    // Parse and convert them 
    uint32_t id = 0x7a0;
    uint8_t dlc = 2;
    String sdata = message.substring(2, 2+(dlc*2));
    uint8_t *data = new uint8_t[8];
    for(int i = 0, j = 0; i < dlc*2; i+=2, j++)
    {
        data[j] = CharToUInt8(sdata.charAt(i)) << 4;
        data[j] += CharToUInt8(sdata.charAt(i+1));
    }

    // Create digest
    const uint8_t *key = (const uint8_t *)skey.c_str();
    unsigned long start = micros();
    Sha1.initHmac(key, skey.length());
    WriteBytes((const uint8_t *)sdata.c_str(), sdata.length());
    hash =  Sha1.resultHmac();
    unsigned long delta = micros() - start;
    counter++;
    Serial.print(counter);
    Serial.print(':');
    Serial.println(delta);

    // Create the 3 auth messages
    uint8_t msg1[8] = {0};
    uint8_t msg2[8] = {0};
    uint8_t msg3[8] = {0};
    for(int i = 0; i < 8; i++)
    {
        msg1[i] = hash[i];
        msg2[i] = hash[i+8];
        if (i < 4)
            msg3[i] = hash[i+16];
    }
    
    // Send the messages
    CAN.sendMsgBuf(id, 0, dlc, data);
    CAN.sendMsgBuf(id, 0, 8, msg1);
    CAN.sendMsgBuf(id, 0, 8, msg2);
    CAN.sendMsgBuf(id, 0, 8, msg3);

    // Increment counter
    numReceived++;
    timer = second(now());
    if (false && timer > oldTime)
    {
        oldTime = timer;
        sec++; // current second
        Serial.print(sec);
        Serial.print(":");
        Serial.println(numReceived);
        numReceived = 0;
    }
    Serial.flush();
    delay(100);
}

void PrintMessage(uint32_t id, uint8_t dlc, uint8_t *stmp)
{
    Serial.print("ID: ");
    Serial.println(id, HEX);
    Serial.print("DLC: ");
    Serial.println(dlc, HEX);
    Serial.print("Data: ");
    for (int i = 0; i < dlc; i++)
    {
      if (stmp[i] < 0x10)
          Serial.print('0');
      Serial.print(stmp[i], HEX);
      Serial.print(' ');
    }
    Serial.print("\n\r-------------------------------------------\n\r");
}

void StampTime(uint8_t *buf)
{
    unsigned long ms = millis(); 

    // Store the timestamp
    buf[0] = (int)((ms >> 24) & 0xFF);
    buf[1] = (int)((ms >> 16) & 0xFF);
    buf[2] = (int)((ms >> 8) & 0xFF);
    buf[3] = (int)((ms & 0xFF));
}


void SendAuthMessages(uint32_t id, uint8_t dlc, uint8_t *buf)
{
    // Create the digest
    const uint8_t *theKey;
    theKey = GetKey(id);
    int keyLen = GetKeyLen(id);
    Sha1.initHmac(theKey, keyLen);
    WriteBytes((const uint8_t *)buf, dlc);
    hash = Sha1.resultHmac();

    // Create the 3 auth messages
    uint8_t msg1[8];
    uint8_t msg2[8];
    uint8_t msg3[8];

    for(int i = 0; i < 8; i++) msg1[i] = hash[i];
    for(int i = 8; i < 16; i++) msg2[i - 8] = hash[i];
    for(int i = 16; i < 20; i++) msg3[i - 16] = hash[i];

    // Fill msg3 last 4 bytes with timestamp
    StampTime(&msg3[4]);

    // Send the auth messages
    CAN.sendMsgBuf(id, 0, 8, msg1);
    CAN.sendMsgBuf(id, 0, 8, msg2);
    CAN.sendMsgBuf(id, 0, 4, msg3);

    Serial.print("Sending Digest: ");
    PrintHash(hash);
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


const uint8_t * GetKey(uint32_t id)
{
    switch(id)
    {
        case MIL_ID:
            return MIL_KEY;
        case DOOR_ID:
            return DOOR_KEY;
        case ENGINE_ID:
            return ENGINE_KEY;
        default:
            return 0;
    }
}

int GetKeyLen(uint32_t id)
{
    switch(id)
    {
        case MIL_ID:
          return MIL_KEY_LEN;
        case DOOR_ID:
          return DOOR_KEY_LEN;
        case ENGINE_ID:
          return ENGINE_KEY_LEN;
        default:
          return -1;
    }
}


void GenerateMessage(uint32_t &id, uint8_t &dlc, unsigned char *data)
{
    int selector = random(3);
    id = SelectMessage(selector);
    dlc = random(9);
    for(int i = 0; i < dlc; i++)
    {
      data[i] = random(0x100); // each byte is random char between 0..0xFF
    }
    for(int i = dlc; i < 8; i++)
    {
      data[i] = 0;
    }
}

int SelectMessage(int selector)
{
    switch(selector)
    {
        case 0: return MIL_ID;
        case 1: return DOOR_ID;
        case 2: return ENGINE_ID;
        default: return -1;
    }
}


void WriteBytes(const uint8_t *data, int len)
{
    for(int i = 0; i < len; i++)
    {
        Sha1.write(data[i]);
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


uint8_t *GetFullMessage(char *key, uint8_t &keyLen, uint8_t *data, uint8_t &dataLen, uint8_t &lenVar)
{
    // Convert to uint8_t
    uint8_t *theKey = CStringToUInt8(key, keyLen);
    uint8_t *theMessage = data;

    // Concatenate into one full message
    uint8_t *fullMessage = new uint8_t[keyLen + dataLen + 1];
    for(int i = 0; i < dataLen; i++)
        fullMessage[i] = theMessage[i];    
    for(int i = 0; i < keyLen; i++)
        fullMessage[i + dataLen] = theKey[i];

    lenVar = keyLen + dataLen;
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

uint8_t CharByteToUInt8(char ch1, char ch2)
{
    uint8_t value = CharToUInt8(ch1);
    value *= 16;
    value += CharToUInt8(ch2);
    return value;
}



