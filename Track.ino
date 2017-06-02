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

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef uint8_t u8;
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

// - Note
//      Simple timer for Arduino
class Timer
{
    uint32_t start;

  public:
    Timer();
    // - Note
    //      Acquire elapsed time in millisecond
    uint32_t pick() const;
    // - Note
    //      Acquire elapsed time and reset the stop watch
    uint32_t reset();
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Variables
// ---- ---- ---- ---- ----

State state;

Sample ma;
Sample Gavg;

LPF3A filter;
Window win;

u16 weights[Postures];
i32 avg_norm[Postures];
i32 dev_norm[Postures];
i32 avg_sma[Postures];
i32 dev_sma[Postures];

u32 times[Postures];

Timer timer;
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

Timer::Timer()
{
    start = millis();
};

uint32_t Timer::pick() const
{
    const uint32_t curr = millis();
    return curr - start; // elapsed
}

uint32_t Timer::reset()
{
    uint32_t span = pick();
    start = millis();
    return span;
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
    win.emplace(x, y, z);
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
    case 0: return P_Lie;
    case 1: return P_Sit;
    case 2: return P_Stand;
    case 3: return P_Walk;
    case 4: return P_Run;
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

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void *OnMonitor();      // White
void *OnTrain();        // Red
void *OnSync();         // Green
void *OnReport();       // Blue
void *OnConnect();      // -
void *OnDisconnect();   // Yellow

void *OnMonitor()
{
    timer.reset();
    Bean.setLed(150, 150, 150);    // White

    measure(); // Read acceleration
    preproc(); // Filtering

    // LA -> Posture
    Sample *pacc = &filter.la;
    u8 p = posture(pacc->x, pacc->y, pacc->z);

    // if static posture,
    if (p == P_Lie)
    {
        // calculate angle
        pacc = &filter.g;
        float angle = orientation(pacc->x, pacc->y, pacc->z);
        angle = abs(angle);

        if (angle > 40)
            p = P_LieSide;
        else if (angle > 90)
            p = P_LieBack;
    }

    // Record (Posture, Millisecond)
    record(p, timer.pick());

    if (Bean.getConnectionState())
        return (PTR)OnConnect;
    else
        return (PTR)OnMonitor;
}

void *OnTrain()
{
    Bean.setLed(150, 0, 0);    // Red
    // Read parameter synchronously
    pack.param = Serial.read();
    // check posture
    u8 p = pos(pack.param);

    // increase the time value of the posture
    measure();
    preproc();

    Sample la = filter.la;
    u32 norm = magnitude(la.x, la.y, la.z);

    // Current weight for this posture
    const u16 Weight = weights[p];
    // Weight's upper bound == 512
    if(Weight < 511){
        weights[p] += 1;
    }

    // Update the average of Magnitude     
    avg_norm[p] = ((avg_norm[p] * Weight) + norm) / (Weight + 1); 

    // For static postures, train gravity vector
    if (p == P_Lie || p == P_Sit || p == P_Stand)
    {
        Sample g = filter.g;
        Gavg.x = ((g.x * Weight) + Gavg.x) / (Weight + 1);
        Gavg.y = ((g.y * Weight) + Gavg.x) / (Weight + 1);
        Gavg.z = ((g.z * Weight) + Gavg.x) / (Weight + 1);
    }

    win.emplace(la.x, la.y, la.z);
    u32 area = win.SMA();
    // Update the average of SMA 
    avg_sma[p] = ((avg_sma[p] * Weight) + area) / (Weight + 1); 
    
    // Process done, send ack.
    writePrefix();
    writeParam();

    return (PTR)OnConnect;
}

void *OnSync()
{
    Bean.setLed(0, 150, 0);    // Green

    // wait parameter and message's value
    if (waitParam() == false && waitValue() == false)
        // Failed -> Disconnect!
        return (PTR)OnDisconnect;

    // check posture and attribute
    u8 p = pos(pack.param);
    u8 a = attr(pack.param);

    // write the value to memory
    store(p, a, pack.value);

    // Process done, send ack.
    writePrefix();
    writeParam();

    return (PTR)OnConnect;
}

void *OnReport()
{
    Bean.setLed(0, 0, 150);    // Blue

    // wait parameter
    // if failed -> Disconnect
    if (waitParam() == false)
        return (PTR)OnDisconnect;

    // check posture and attribute
    u8 p = pos(pack.param);
    u8 a = attr(pack.param);

    // load value for response
    pack.value = load(p, a);

    // Send the response
    writePrefix();
    writeParam();
    writeValue();

    return (PTR)OnConnect;
}

void *OnConnect()
{
    // If not connected, go back to `OnMonitor`
    if (Bean.getConnectionState() == false)
        return (PTR)OnMonitor;
    // Timeout, disconnect.
    if (waitPrefix() == false)
        return (PTR)OnDisconnect;

    switch (pack.prefix)
    {
    case OP_Sync:       return (PTR)OnSync;
    case OP_Report:     return (PTR)OnSync;
    case OP_Train:      return (PTR)OnSync;
    default:            return (PTR)OnDisconnect;
    }
}

void *OnDisconnect()
{
    Bean.setLed(150, 150, 0);   // Yellow
 
    // Explicit disconnection
    if (Bean.getConnectionState())
    {
        pack.prefix = OP_Discon;
        writePrefix();
        Bean.disconnect();
    }

    // Clear message buffer
    pack.prefix = 0;
    pack.param = param(P_Unknown, A_Mean);
    pack.value = 0;

    // Restart monitoring
    return (PTR)OnMonitor;
}
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    // Clear variables
    timer.reset();
    ma.x = ma.y = ma.z = 0;
    Gavg.x = Gavg.y = Gavg.z = 0;

    for (unsigned i = 0; i < Postures; ++i)
    {
        avg_norm[i] = dev_norm[i] = 0;
        avg_sma[i] = dev_sma[i] = 0;
        times[i] = 0;
    }
    pack.prefix = 0;
    pack.param = param(P_Unknown, A_Mean);
    pack.value = 0;
    
    // Initial state function
    state = (State)OnMonitor;

    // Update sensor in a 100ms period
    Bean.setAccelerometerPowerMode(0x5A);
}

void loop()
{
    if (state)
    {
        Bean.setLed(0,0,0);
        PTR next = (*state)();
        state = (State)next;
    }
    // Warning: Strong purple!
    else
    {
        Bean.setLed(255, 0, 255);
        Bean.sleep(1000);
    }
}
