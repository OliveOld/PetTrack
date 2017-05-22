// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//
//  Caution
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

typedef void* PTR;
typedef PTR(*)() State;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Utility
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

uint32_t Square(int16_t x)
{
    return x*x;
}

uint32_t Norm(int16_t x, int16_t y, int16_t z)
{
    return Square(x) + Square(y) + Square(z));
}

// - Note
//      Simple timer for Arduino
class Timer
{
    uint32_t start;

  public:
    Timer()
    {
        start = millis();
    };
    // - Note
    //      Acquire elapsed time in millisecond
    uint32_t pick() const
    {
        const uint32_t curr = millis();
        return curr - start; // elapsed
    }
    // - Note
    //      Acquire elapsed time and reset the stop watch
    uint32_t reset()
    {
        uint32_t span = pick();
        start = millis();
        return span;
    }
};

struct Unit
{
    int x, y, z;

    Unit()
    {
        x = y = z = 0;
    }
};

// - Note
//      3-Axis Low Pass Filter
class LPF3A
{
  public:
    int alpha;

    Unit la; // Linear Acceleration
    Unit g;  // Gravity Acceleration
  public:
    LPF3A()
    {
        alpha = 0; // Caution : 0 by default
        la.x = la.y = la.z = 0;
        g.x = g.y = g.z = 0;
    }

    // - Note
    //      Receive Measured accceleration,
    //      then update Linear/Gravity acceleration.
    void separate(int mx, int my, int mz)
    {
        // a == [0,999]
        // G = a*G + (1-a)* MA
        g.x = mx + (alpha * (g.x - mx))/1000;
        g.y = my + (alpha * (g.y - my))/1000;
        g.z = mz + (alpha * (g.z - mz))/1000;
        // LA = MA - G
        la.x = mx - g.x;
        la.y = my - g.y;
        la.z = mz - g.z;
    }
};

const int GSize = 4;

// - Note
//      Group of Unit
class UnitGroup
{
  public:
    uint8_t idx;
    Unit    buf[GSize];

  private:
    void next()
    {
        idx = (idx + 1) % GSize;
    }

  public:
    void clear()
    {
        idx = 0;
        for (int i = 0; i < GSize; ++i)
        {
            buf[i].x = buf[i].y = buf[i].z = 0;
        }
    }
    void emplace(int x, int y, int z)
    {
        buf[idx].x = x;
        buf[idx].y = y;
        buf[idx].z = z;
        next();
    }
    int SMA()
    {
        uint32_t sma = 0;
        for (int i = 0; i < GSize; ++i)
        {
            sma = sma + abs(buf[i].x) + abs(buf[i].y) + abs(buf[i].z);
        }
        return sma / GSize;
    }
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

const int POSTURES = 8;
enum Pos
{
    Unknown  = 0,
    Lie,        // 1
    LieSide,    // 2
    LieBack,    // 3
    Sit,        // 4
    Stand,      // 5
    Walk,       // 6
    Run         // 7
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Function Declarations
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// PTR   Blink();      // Blink LED. Current Battery State

// PTR   Clear();      // Clear the time records
// PTR   Monitor();    // Monitoring

// PTR   Connected();  // On Connect, check serial
// PTR   Report();     // Report data based on request
// PTR   Train();      // Receiving Guide from remote
// PTR   Sync();       // Synchronizing with remote

// void    UpdateMA();
// void    Preproc();

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Core Variables & Functions
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

State state;

LPF3A      filter;
Timer      timer;
Unit       ma;
UnitGroup  grp;
uint32_t   time[POSTURES]; // uint32_t per Postures

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    Clear();
    filter.alpha = 759; // (759 * x) / 1000
    state = Blink;
    // Update in a 100ms period
    Bean.setAccelerometerPowerMode(0x5A);
}

void loop()
{
    timer.reset();

    if(state != NULL){
        state = (*state)();
    }
    else{
        Bean.setLed(0,100,100);
        Bean.sleep(200);
        Bean.setLed(0, 0, 0);        
    }
    Bean.sleep(800);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Function Implementations
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Measure current acceleration
void Measure()
{
    AccelerationReading a = Bean.getAcceleration();
    ma.x = a.x;
    ma.y = a.y;
    ma.z = a.z;
}

// - Note
//      Pre-processing of MA
void Preproc()
{
    // Filtering
    filter.separate(ma.x, ma.y, ma.z);
    Unit g = filter.g;   // Gravity Acceleration
    Unit la = filter.la; // Linear Acceleration
}

// - Note
//      Connection    = Blue
//      Battery Level = Red-Green 
PTR Blink()
{
    uint8_t leftover = Bean.getBatteryLevel();
    
    uint8_t green = leftover;
    uint8_t red   = (100 - leftover);
    uint8_t blue  = 0;

    // If connected, replace to Blue
    if(Bean.getConnectionState() == true){
        blue  = green;
        green = 0;
    }

    Bean.setLed(red, green, blue);
    Bean.sleep(200);
    Bean.setLed(0, 0, 0);

    return reinterpret_cast<PTR>(Blink);
}

// Clear the time records
PTR Clear()
{
    filter.alpha = 0.7592;
    ma.x = ma.y = ma.z = 0;
    for(int i = 0; i < POSTURES; ++i){
        time[i] = 0;
    }
    grp.clear();
    return reinterpret_cast<PTR>(Monitor);
}

// Monitoring
PTR Monitor()
{
    Measure();
    Preproc();




    if(Bean.getConnectionState() == true){
        // -> Connected
        return reinterpret_cast<PTR>(Connected);
    }
    else{
        // -> Keep Monitoring
        return reinterpret_cast<PTR>(Monitor);
    }
}

// On Connect, check serial
PTR Connected()
{
    // Wait for message
    while(Bean.getConnectionState()
          && Serial.available() == 0)
    {
        Blink();
        // Wait until we got message
        Bean.sleep(1000);
    }

    if(Bean.getConnectionState() == false)
    {
        return reinterpret_cast<PTR>(Monitor);
    }

    // Read Message...
    // return reinterpret_cast<PTR>(Report);
    // return reinterpret_cast<PTR>(Train);
    // return reinterpret_cast<PTR>(Sync);
    return reinterpret_cast<PTR>(NULL);
}

// Report data based on request
PTR Report()
{
    Serial.print("Reporting...");
    return reinterpret_cast<PTR>(Clear);
}

// Receiving Guide from remote
PTR Train()
{
    return reinterpret_cast<PTR>(NULL);
}

// Synchronizing with remote
PTR Sync()
{
    return reinterpret_cast<PTR>(NULL);
}
