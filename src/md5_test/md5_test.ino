#include <MD5.h>
#include <mcp_can.h>

const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);

char *correct_key = "sharedkey";

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
    }
    Serial.println("CAN BUS Shield init ok!");
}

void loop()
{
    while (Serial.available() >= 4)
    {
        // Get user message
        uint32_t id = (uint32_t)Serial.parseInt();
        uint8_t dlc = (uint8_t)Serial.parseInt();
        uint8_t *msg = new uint8_t[dlc+1];
        for(int i = 0; i < dlc; i++)
            msg[i] = (uint8_t)Serial.parseInt();
        char *key = (char *)(Serial.readString().c_str());
        uint8_t msgLen, keyLen, fullMsgLen;
        uint8_t *fullMessage = GetFullMessage(correct_key, keyLen, msg, msgLen, fullMsgLen);
        Serial.flush();

        CAN.sendMsgBuf(id, 0, dlc, msg);
    }
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




