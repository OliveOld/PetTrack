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
    Dis = -1,
    Rep = 1,
    Tra,
    Syn,
};

const int POSTURES = 8;
enum Pos
{
    Unknown = 0,
    Lie,     // 1
    LieSide, // 2
    LieBack, // 3
    Sit,     // 4
    Stand,   // 5
    Walk,    // 6
    Run      // 7
};

enum Attr
{
    Mean = 1,
    Stdev,
    Time
};

namespace Comm
{
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
    if (Serial.available() >= sizeof(uint32_t))
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

} // namespace Comm

uint8_t b = 8;
State state;

void setup()
{
    state = NULL;
    b = 9;
}

void loop()
{
    if (state)
    {
        PTR ptr = (*state)();
        state = (State)ptr;
    }
    Serial.write(b);
}
