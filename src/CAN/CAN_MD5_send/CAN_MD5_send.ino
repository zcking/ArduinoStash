#include <MD5.h>
#include <mcp_can.h>

const int SPI_CS_PIN = 9;
const int HASH_LEN = 20;

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
}

void loop()
{
  // put your main code here, to run repeatedly:
    if (Serial.available())
    {
        String sid = Serial.readStringUntil('#');
        String message = Serial.readStringUntil(' ');
        String key = Serial.readString();
  
        // Parse and convert
        uint32_t id = 0;
        id += (uint32_t)(CharToUInt8(sid.charAt(0))) << 8;
        id += (uint32_t)(CharToUInt8(sid.charAt(1))) << 4;
        id += (uint32_t)CharToUInt8(sid.charAt(2));
        uint8_t dlc = CharToUInt8(message.charAt(1));
        String sdata = message.substring(2, 2+(dlc*2));
        uint8_t data[8] = {0};
        for(int i = 0, j = 0; i < dlc*2; i+=2, j++)
        {
            data[j] = CharToUInt8(sdata.charAt(i)) << 4;
            data[j] += CharToUInt8(sdata.charAt(i+1));
        }
  
        // Create digest
        unsigned char *hash = MD5::make_hash((char *)(message.substring(2, message.length()) + key).c_str());
        char *sdigest = MD5::make_digest(hash, HASH_LEN);
        uint8_t digest[HASH_LEN] = {0};
        for(int i = 0, j = 0; i < HASH_LEN*2; i+=2, j++)
        {
            digest[j] = CharToUInt8(sdigest[i]) << 4;
            digest[j] += CharToUInt8(sdigest[i+1]);
        }

        // Create the 3 auth messages
        uint8_t msg1[8] = {0};
        uint8_t msg2[8] = {0};
        uint8_t msg3[8] = {0};
        for(int i = 0; i < 8; i++)
        {
            msg1[i] = digest[i];
            msg2[i] = digest[i+8];
            if (i < 4)
                msg3[i] = digest[i+16];
        }

        // Send the messages
        CAN.sendMsgBuf(id, 0, dlc, data);
        CAN.sendMsgBuf(id, 0, 8, msg1);
        CAN.sendMsgBuf(id, 0, 8, msg2);
        CAN.sendMsgBuf(id, 0, 8, msg3);
  
        // Print everything to serial interface
        Serial.print("ID: 0x");
        Serial.println(id, HEX);
        Serial.print("DLC: ");
        Serial.println(dlc);
        Serial.print("Data: ");
        for (int i = 0; i < dlc; i++)
        {
            Serial.print("0x");
            if (data[i] < 0x10)
                Serial.print('0');
            Serial.print(data[i], HEX);
            Serial.print(' ');
        }
        Serial.println();
        Serial.print("Key: ");
        Serial.println(key);
        Serial.print("Digest: ");
        Serial.println(sdigest);
        Serial.println("---------------------------------------\n");
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



