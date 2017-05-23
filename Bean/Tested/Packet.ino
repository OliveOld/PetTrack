// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      Packet Communication Example
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

typedef void *PTR;
typedef PTR (*State)(void);

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

enum Oper
{
    OP_Discon = -1,
    OP_Report = 1,
    OP_Train,   // 2
    OP_Sync,    // 3
};

const int POSTURES = 8;
enum Pos
{
    P_Unknown = 0,
    P_Lie,     // 1
    P_LieSide, // 2
    P_LieBack, // 3
    P_Sit,     // 4
    P_Stand,   // 5
    P_Walk,    // 6
    P_Run      // 7
};

enum Attr
{
    A_Mean = 1,
    A_Stdev,    // 2
    A_Time      // 3
};

struct Packet
{
    uint8_t prefix;
    uint8_t param;
    uint32_t value;
};
struct Packet pack;

uint8_t param(uint8_t pos, uint8_t attr)
{
    return (pos << 4) + attr;
}

uint8_t pos(uint8_t param)
{
    return param >> 4;
}

uint8_t attr(uint8_t param)
{
    return param & 0x0F;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Read Packet's Prefix
// - Return
//      true if read
bool ReadPrefix()
{
    if (Serial.available())
    {
        pack.prefix = Serial.read();
        return true;
    }
    return false;
}

void WritePrefix()
{
    Serial.write(pack.prefix);
}

// - Note
//      Read Packet's Parameter
// - Return
//      true if read
bool ReadParam()
{
    if (Serial.available())
    {
        pack.param = Serial.read();
        return true;
    }
    return false;
}

void WriteParam()
{
    Serial.write(pack.param);
}

// - Note
//      Read Packet's Value
// - Return
//      true if read
bool ReadValue()
{
    if ((uint32_t)Serial.available() >= sizeof(uint32_t))
    {
        pack.value = Serial.parseInt();
        return true;
    }
    return false;
}

void WriteValue()
{
    Serial.write(pack.value);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

int32_t  average;
int32_t  stdev;
uint32_t time;

void setup()
{
    average = 150;
    stdev   = 75;
    time    = 25;
}

void loop()
{
    pack.param = param(P_Run, A_Mean);

}
