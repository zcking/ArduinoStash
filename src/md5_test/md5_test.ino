#include <MD5.h>


char *key = "sharedkey";
int keyLen;
int32_t id = 0xa0;
int8_t dlc = 2;
char *data = "a1f2";
int dataLen;

void setup()
{
    Serial.begin(115200);
    uint8_t *newKey = CStringToUInt8(key, keyLen);
    uint8_t *newMessage = HexToUInt8(data, dataLen);

    SendAuthMessages();
}

void loop()
{
    
}


void SendAuthMessages()
{
    // Convert to uint8_t
    uint8_t *theKey = CStringToUInt8(key, keyLen);
    uint8_t *theMessage = HexToUInt8(data, dataLen);

    // Concatenate into one full message
    uint8_t *fullMessage = new uint8_t[keyLen + dataLen + 1];
    for(int i = 0; i < dataLen; i++)
        fullMessage[i] = theMessage[i];    
    for(int i = 0; i < keyLen; i++)
        fullMessage[i + dataLen] = theKey[i];

    // Print the whole, unhashsed message
    for(int i = 0; i < (keyLen + dataLen); i++)
    {
        if (fullMessage[i] < 0x10)
            Serial.print('0');
        Serial.print(fullMessage[i], HEX);
        Serial.print(' ');
    }
    Serial.println();
}


uint8_t *HexToUInt8(char *hexStr, int &len)
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


uint8_t *CStringToUInt8(char *str, int &len)
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




