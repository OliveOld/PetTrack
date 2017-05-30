// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//  Note
//      File for Release Candidate
//
//  Caution
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Type / Constatnt
// ---- ---- ---- ---- ----

typedef void *PTR;
typedef PTR (*State)(void);

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

// (Overlapped) Window size for SMA
const u16 WindowSize = 4;

// Low Pass Filter smoothing factor
const u16 Alpha = 759;
const u16 MaxAlpha = 1000;
const u16 timeout = 15000; // Wait until 15 seconds

enum Oper
{
    OP_Discon = -1,
    OP_Report = 1,
    OP_Train, // 2
    OP_Sync,  // 3
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
    A_Stdev, // 2. Average Magnitude
    A_Time   // 3. Average Deviation
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Function
// ---- ---- ---- ---- ----

uint32_t square(int x);
uint32_t magnitude(int x, int y, int z);

// - Note
//      Measure current acceleration
void measure();
// - Note
//      Pre-processing of MA
void preproc();

u8 posture(int x, int y, int z);
bool isStatic(uint8_t pos);

void record(uint8_t pos, uint32_t dur);
float orientation(int gx, int gy, int gz);

uint8_t attr(uint8_t param);
uint8_t pos(uint8_t param);
uint8_t param(uint8_t pos, uint8_t attr);

bool wait(int sz);
// - Note
//      Read Packet's Prefix
// - Return
//      `true` if read, `false` if timeout
bool waitPrefix();
void writePrefix();
// - Note
//      Read Packet's Parameter
// - Return
//      `true` if read, `false` if timeout
bool waitParam();
void writeParam();
void writeValue();
// - Note
//      Read Packet's Value
// - Return
//      `true` if read, `false` if timeout
bool waitValue();

uint32_t load(uint8_t pos, uint8_t attr);
void store(uint8_t pos, uint8_t attr, uint8_t value);

void clear();
void failed();

void OnTrain();
void OnSync();
void OnReport();
void OnMonitor();

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Struct
// ---- ---- ---- ---- ----

struct Sample
{
    int x, y, z;
    Sample() { x = y = z = 0; }
};

// - Note
//      3-Axis Low Pass Filter
struct LPF3A
{
    Sample la; // Linear Acceleration
    Sample g;  // Gravity Acceleration
  public:
    int factor(int x);
    // - Note
    //      Receive Measured accceleration,
    //      then update Linear/Gravity acceleration.
    void separate(int mx, int my, int mz);
};

// - Note
//      Group of Sample
class Window
{
  public:
    u16 idx;
    Sample buf[WindowSize];

  private:
    void next();

  public:
    void clear();
    void emplace(int x, int y, int z);
    u32 SMA();
};

struct Packet
{
    u8 prefix;
    u8 param;
    u32 value;
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Variables
// ---- ---- ---- ---- ----

State state;

Sample ma;
Sample Gavg;

LPF3A filter;
Window win;

i32 avg_norm[Postures];
i32 dev_norm[Postures];
i32 avg_sma[Postures];
i32 dev_sma[Postures];

u32 times[Postures];

Packet pack;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Implementations
// ---- ---- ---- ---- ----

uint32_t square(int x)
{
    return x * x;
}

uint32_t magnitude(int x, int y, int z)
{
    return square(x) + square(y) + square(z);
}

int LPF3A::factor(int x)
{
    return (x * Alpha) / MaxAlpha;
}

void LPF3A::separate(int mx, int my, int mz)
{
    // G = a*G + (1-a)* MA
    g.x = mx + factor(g.x - mx);
    g.y = my + factor(g.y - my);
    g.z = mz + factor(g.z - mz);
    // LA = MA - G
    la.x = mx - g.x;
    la.y = my - g.y;
    la.z = mz - g.z;
}

void Window::next()
{
    idx = (idx + 1) % WindowSize;
}

void Window::clear()
{
    idx = 0;
    for (unsigned i = 0; i < WindowSize; ++i)
    {
        buf[i].x = buf[i].y = buf[i].z = 0;
    }
}
void Window::emplace(int x, int y, int z)
{
    buf[idx].x = x;
    buf[idx].y = y;
    buf[idx].z = z;
    next();
}
uint32_t Window::SMA()
{
    u32 sma = 0;
    for (unsigned i = 0; i < WindowSize; ++i)
    {
        u32 inc = abs(buf[i].x) + abs(buf[i].y) + abs(buf[i].z);
        sma += inc;
    }
    return sma / WindowSize;
}

void measure()
{
    AccelerationReading a = Bean.getAcceleration();
    ma.x = a.xAxis;
    ma.y = a.yAxis;
    ma.z = a.zAxis;
}

void preproc()
{
    // MA -> LA, G
    filter.separate(ma.x, ma.y, ma.z);
}

uint8_t posture(int x, int y, int z)
{
    u8 pos = P_Unknown; // It's 0. Unknown for now...

    // Derivation
    u32 norm = magnitude(x, y, z);
    win.emplace(x,y,z);
    u32 sma = win.SMA();

    // Distances: SMA + Norm
    u32 distances[5];
    distances[0] = abs(norm - avg_norm[P_Lie]) + abs(sma - avg_sma[P_Lie]);
    distances[1] = abs(norm - avg_norm[P_Sit]) + abs(sma - avg_sma[P_Sit]);
    distances[2] = abs(norm - avg_norm[P_Stand]) + abs(sma - avg_sma[P_Stand]);
    distances[3] = abs(norm - avg_norm[P_Walk]) + abs(sma - avg_sma[P_Walk]);
    distances[4] = abs(norm - avg_norm[P_Run] + abs(sma - avg_sma[P_Run]));

    // Find minimum
    u32 min = (u32)-1;
    for (int i = 0; i < 5; ++i)
    {
        if (min > distances[i])
        {
            min = distances[i];
            pos = i;
        }
    }

    switch (pos)
    {
    case 0:
        return P_Lie;
    case 1:
        return P_Sit;
    case 2:
        return P_Stand;
    case 3:
        return P_Walk;
    case 4:
        return P_Run;
    default:
        return P_Unknown;
    }
}

bool isStatic(uint8_t pos)
{
    return pos == P_Lie;
}

float orientation(int gx, int gy, int gz)
{
    // Gavg * Gt
    u32 dot = (Gavg.x * gx) + (Gavg.y * gy) + (Gavg.z * gz);
    // |Gavg|
    u32 Mavg = magnitude(Gavg.x, Gavg.y, Gavg.z);
    // |Gt|
    u32 Mt = magnitude(gx, gy, gz);

    // |Gavg||Gt|(cos $theta) = Gavg*Gt
    float cosine = (float)dot;
    cosine = cosine / (Mavg * Mt);

    return (float)acos(cosine);
}

void record(uint8_t pos, uint32_t dur)
{
    times[pos] += dur;
}

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

bool wait(int sz)
{
    u16 lag = 0;
    // polling loop
    while ((Serial.available() >= sz) == false)
    {
        delay(100); // Bean.sleep(100);
        lag += 100;
        if (lag >= timeout)
        {
            return false;
        }
    }
    return true;
}

bool waitPrefix()
{
    // wait 1 byte
    if (wait(1))
    {
        pack.prefix = Serial.read();
        return true;
    }
    // timeout
    else
        return false;
}

void writePrefix()
{
    Serial.write(pack.prefix);
}

bool waitParam()
{
    // wait 1 byte
    if (wait(1))
    {
        pack.param = Serial.read();
        return true;
    }
    // timeout
    else
        return false;
}

void writeParam()
{
    Serial.write(pack.param);
}

bool waitValue()
{
    // wait 4 byte
    if (wait(4))
    {
        u8 *base = (u8 *)&pack.value;
        base[0] = Serial.read();
        base[1] = Serial.read();
        base[2] = Serial.read();
        base[3] = Serial.read();
        return true;
    }
    // timeout
    else
        return false;
}

void writeValue()
{
    u8 *base = (u8 *)&pack.value;
    Serial.write(base[0]);
    Serial.write(base[1]);
    Serial.write(base[2]);
    Serial.write(base[3]);
}
void store(uint8_t pos, uint8_t attr, uint32_t value)
{
    if (attr == A_Mean)
    {
        avg_norm[pos] = (i32)value;
    }
    else if (attr == A_Stdev)
    {
        dev_norm[pos] = (i32)value;
    }
    else if (attr == A_Time)
    {
        times[pos] = value;
    }
}

uint32_t load(uint8_t pos, uint8_t attr)
{
    if (attr == A_Mean)
    {
        return (u32)avg_norm[pos];
    }
    else if (attr == A_Stdev)
    {
        return (u32)dev_norm[pos];
    }
    else if (attr == A_Time)
    {
        return times[pos];
    }
    return -1;
}

void clear()
{
    pack.prefix = 0;
    pack.param = param(P_Unknown, A_Mean);
    pack.value = 0;
}

void failed()
{
    if (Bean.getConnectionState())
    {
        pack.prefix = OP_Discon;
        writePrefix();
        Bean.disconnect();
    }
    clear();
}

void OnTrain()
{
    // wait parameter
    if (waitParam() == false)
    {
        // Failed -> Disconnect!
        return failed();
    }

    // check posture
    u8 p = pos(pack.param);

    // increase the time value of the posture
    times[p] += 100;

    // Process done, send ack.
    writePrefix();
    writeParam();
}

void OnSync()
{
    // wait parameter
    if (waitParam() == false)
    {
        // Failed -> Disconnect!
        return failed();
    }
    // wait for message's value
    if (waitValue() == false)
    {
        // Failed -> Disconnect!
        return failed();
    }

    // check posture and attribute
    u8 p = pos(pack.param);
    u8 a = attr(pack.param);

    // write the value to memory
    store(p, a, pack.value);

    // Process done, send ack.
    writePrefix();
    writeParam();
}

void OnReport()
{
    // wait parameter
    if (waitParam() == false)
    {
        // Failed -> Disconnect!
        return failed();
    }
    // check posture and attribute
    u8 p = pos(pack.param);
    u8 a = attr(pack.param);

    // load value for response
    pack.value = load(p, a);

    // Send the response
    writePrefix();
    writeParam();
    writeValue();
}

void OnMonitor()
{
    u32 elapsed = 200;

    measure(); // Read acceleration
    preproc(); // Filtering

    // LA -> Posture
    Sample *pacc = &filter.la;
    u8 p = posture(pacc->x, pacc->y, pacc->z);

    // if static posture,
    if (isStatic(p) == true)
    {
        // calculate angle
        pacc = &filter.g;
        float angle = orientation(pacc->x, pacc->y, pacc->z);
        angle = abs(angle);

        if (angle > 40)
        {
            p = P_LieSide;
        }
        else if (angle > 90)
        {
            p = P_LieBack;
        }
    }

    // Record (Posture, Millisecond)
    record(p, elapsed);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    // Clear variable
    // Filter's smoothing factor
    // Initial state function

    // Update sensor in a 100ms period
    Bean.setAccelerometerPowerMode(0x5A);
}

void loop()
{
    // ...
}
