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

const int Postures = 8;
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

const uint16_t timeout = 1000;  // Wait until 1 second
bool wait(int sz)
{
    unsigned short lag = 0;
    // polling loop
    while ((Serial.available() >= sz) == false)
    {
        delay(100); // Bean.sleep(100);
        lag += 100;
        if(lag >= timeout){
            return false;
        }
    }
    return true;
}
// - Note
//      Read Packet's Prefix
// - Return
//      `true` if read, `false` if timeout
bool waitPrefix()
{
    // wait 1 byte
    if( wait(1) ){
        pack.prefix = Serial.read();
        return true;
    }
    // timeout
    else return false;
}

void writePrefix()
{
    Serial.write(pack.prefix);
}

// - Note
//      Read Packet's Parameter
// - Return
//      `true` if read, `false` if timeout
bool waitParam()
{
    // wait 1 byte
    if( wait(1) ){
        pack.param = Serial.read();
        return true;
    }
    // timeout
    else return false;
}

void writeParam()
{
    Serial.write(pack.param);
}

// - Note
//      Read Packet's Value
// - Return
//      `true` if read, `false` if timeout
bool waitValue()
{
    // wait 4 byte
    if( wait(4) ){
        uint8_t* base = (uint8_t*)&pack.value;
        base[0] = Serial.read();
        base[1] = Serial.read();
        base[2] = Serial.read();
        base[3] = Serial.read();
        return true;
    }
    // timeout
    else return false;
}

void writeValue()
{
    uint8_t* base = (uint8_t*)&pack.value;
    Serial.write(base[0]);
    Serial.write(base[1]);
    Serial.write(base[2]);
    Serial.write(base[3]);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

int32_t  averages[Postures];
int32_t  stdevs[Postures];
uint32_t times[Postures];

void     store(uint8_t pos, uint8_t attr, uint32_t value)
{
    if(attr == A_Mean){
        averages[pos] = (int32_t)value;
    }
    else if (attr == A_Stdev){
        stdevs[pos] = (int32_t)value;
    }
    else if (attr == A_Time){
        times[pos] = value;
    }
}

uint32_t load(uint8_t pos, uint8_t attr)
{
    if(attr == A_Mean){
        return (uint32_t)averages[pos];
    }
    else if (attr == A_Stdev){
        return (uint32_t)stdevs[pos];
    }
    else if (attr == A_Time){
        return times[pos];
    }
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void clear()
{
    pack.prefix = 0;
    pack.param = param(P_Unknown, A_Mean);
    pack.value = 0;
}

void failed()
{
    if(Bean.getConnectionState()){
        pack.prefix = OP_Discon;
        writePrefix();
        Bean.disconnect();
    }
    clear();
}

void OnTrain()
{
    // wait parameter
    if(waitParam()==false){
        // Failed -> Disconnect!
        return failed();
    }

    // check posture
    uint8_t p = pos(pack.param);

    // increase the time value of the posture
    times[p] += 100;

    // Process done, send ack.
    writePrefix();
    writeParam();
}

void OnSync()
{
    // wait parameter
    if(waitParam()==false){
        // Failed -> Disconnect!
        return failed();
    }
    // wait for message's value
    if(waitValue()==false){
        // Failed -> Disconnect!
        return failed();
    }

    // check posture and attribute
    uint8_t p = pos(pack.param);
    uint8_t a = attr(pack.param);

    // write the value to memory
    store(p, a, pack.value);

    // Process done, send ack.
    writePrefix();
    writeParam();
}

void OnReport()
{
    // wait parameter
    if(waitParam()==false){
        // Failed -> Disconnect!
        return failed();
    }
    // check posture and attribute
    uint8_t p = pos(pack.param);
    uint8_t a = attr(pack.param);

    // load value for response
    pack.value = load(p,a);

    // Send the response
    writePrefix();
    writeParam();
    writeValue();
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void setup()
{
    for(unsigned i = 0; i < Postures; ++i){
        averages[i] = i*20; // 20, 40, 60...
        stdevs[i] = i*3; // 3, 6, 9...
        times[i] = i;   // 1, 2, 3...
    }
}

void loop()
{
    // If no connection, do nothing
    if(Bean.getConnectionState() == false){
        return Bean.sleep(500);
    }
    // Wait for message
    if(waitPrefix() == false){
        return failed();
    }
    
    if (pack.prefix == OP_Sync)     return OnSync();
    if (pack.prefix == OP_Report)   return OnReport();
    if (pack.prefix == OP_Train)    return OnTrain();
    else 
        return failed();
}
