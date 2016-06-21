// The CAN Message Simulator Constants
struct Message 
{
    int id;
    int dlc;
    unsigned char data[8];
};

struct Message milControl;
struct Message doorContorl;
struct Message engineControl;
struct Message invalidMessage;

const int MIN_ID = 0xa0;
const int MAX_ID = 0xe0;
const int ID_RANGE = MAX_ID - MIN_ID;
const int MIL_ID = 0xe0;
const int DOOR_ID = 0xd0;
const int ENGINE_ID = 0xa0;
const uint8_t MIL_KEY[10] = {'0','1','2','3','4','5','6','7','8','9'};
const uint8_t DOOR_KEY[8] = {8, 7, 6, 5, 4, 3, 2, 1};
const uint8_t ENGINE_KEY[4] = {0, 2, 4, 8};

bool ValidMessage(int id)
{
    switch(id)
    {
        case 0xe0:
        case 0xd0:
        case 0xa0:
            return true;
            break;
        return false;
    }
}

int SelectMessage(int selector)
{
    switch(selector)
    {
        case 0: return 0xe0;
        case 1: return 0xd0;
        case 2: return 0xa0;
        default: return -1;
    }
}

Message GetMessage(int id)
{
    switch(id)
    {
        case 0xe0:
            return milControl;
        case 0xd0:
            return doorContorl;
        case 0xa0:
            return engineControl;
        default:
            return invalidMessage;
    }
}

uint8_t* GetKey(uint32_t id, uint8_t *key, uint8_t &keyLen)
{
    switch(id)
    {
        case MIL_ID:
            keyLen = sizeof(MIL_KEY);
            for(int i = 0; i < keyLen; i++)
                key[i] = MIL_KEY[i];
            break;
        case DOOR_ID:
            keyLen = sizeof(DOOR_KEY);
            for(int i = 0; i < keyLen; i++)
                key[i] = DOOR_KEY[i];
            break;
        case ENGINE_ID:
            keyLen = sizeof(ENGINE_KEY);
            for(int i = 0; i < keyLen; i++)
                key[i] = ENGINE_KEY[i];
            break;
        default:
            return 0; // null - not found
    }
}

void InitMessages()
{
    milControl.id = 0xe0;
    doorContorl.id = 0xd0;
    engineControl.id = 0xa0;
}