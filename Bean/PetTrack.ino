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

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Utility
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

float square(float x)
{
    return x*x;
}

float Norm(float x, float y, float z)
{
    return sqrt(square(x) + square(y) + square(z));
}

// - Note
//      Simple timer for Arduino
class Timer
{
    unsigned long start;

  public:
    Timer()
    {
        start = millis();
    };
    // - Note
    //      Acquire elapsed time in millisecond
    unsigned long pick() const
    {
        const unsigned long curr = millis();
        //// overflow
        //if (curr < start) {
        //}
        return curr - start; // elapsed
    }
    // - Note
    //      Acquire elapsed time and reset the stop watch
    unsigned long reset()
    {
        unsigned long span = pick();
        start = millis();
        return span;
    }
};

struct Unit
{
    float x, y, z;

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
    float alpha;

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
    void separate(float mx, float my, float mz)
    {
        // G = a*G + (1-a)* MA
        g.x = mx + alpha * (g.x - mx);
        g.y = my + alpha * (g.y - my);
        g.z = mz + alpha * (g.z - mz);
        // LA = MA - G
        la.x = mx - g.x;
        la.y = my - g.y;
        la.z = mz - g.z;
    }
};

const int GSize = 5;

// - Note
//      Group of Unit
class UnitGroup
{
  public:
    uint8_t idx;
    Unit buf[GSize];

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
    void emplace(float x, float y, float z)
    {
        buf[idx].x = x;
        buf[idx].y = y;
        buf[idx].z = z;
        next();
    }
    float SMA()
    {
        float sma = 0;
        for (int i = 0; i < GSize; ++i)
        {
            sma = sma + abs(buf[i].x) + abs(buf[i].y) + abs(buf[i].z);
        }
        return sma / GSize;
    }
};

const int POSTURES = 8;
enum Pos
{
    Unknown  = 0,
    Lie,
    LieSide,
    LieBack,
    Sit,
    Stand,
    Walk,
    Run
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Function Declarations
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void* Blink();      // Blink LED. Current Battery State

void* Clear();      // Clear the time records
void* Monitor();    // Monitoring

void* Connected();  // On Connect, check serial
void* Report();     // Report data based on request
void* Train();      // Receiving Guide from remote
void* Sync();       // Synchronizing with remote

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Core Variables & Functions
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void* (*state)();

Unit            ma;
UnitGroup       grp;
unsigned long   time[POSTURES]; // unsigned long per Postures

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    Clear();

    state = Blink;
    // Update in a 100ms period
    Bean.setAccelerometerPowerMode(0x5A);
}

void loop()
{
    if(state != NULL){
        void* ptr = (*state)();
        state = reinterpret_cast<void*(*)()>(ptr);
    }
    else{
        Bean.setLed(100,100,100);
        Bean.sleep(200);
        Bean.setLed(0, 0, 0);        
    }
    Bean.sleep(800);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Function Implementations
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Connection    = Blue
//      Battery Level = Red-Green 
void* Blink()
{
    uint8_t leftover = Bean.getBatteryLevel();
    
    uint8_t green = leftover * 2;
    uint8_t red   = (100 - leftover)*2;
    uint8_t blue  = 0;

    // If connected, replace to Blue
    if(Bean.getConnectionState() == true){
        blue  = green;
        green = 0;
    }

    Bean.setLed(red, green, blue);
    Bean.sleep(200);
    Bean.setLed(0, 0, 0);

    return reinterpret_cast<void*>(Blink);
}

// Clear the time records
void* Clear()
{
    ma.x = ma.y = ma.z = 0;
    for(int i = 0; i < POSTURES; ++i){
        time[i] = 0;
    }
    grp.clear();
    return reinterpret_cast<void*>(NULL);
}

// Monitoring
void* Monitor()
{
    return reinterpret_cast<void*>(NULL);
}

// On Connect, check serial
void* Connected()
{
    return reinterpret_cast<void*>(NULL);
}

// Report data based on request
void* Report()
{
    return reinterpret_cast<void*>(NULL);
}

// Receiving Guide from remote
void* Train()
{
    return reinterpret_cast<void*>(NULL);
}

// Synchronizing with remote
void* Sync()
{
    return reinterpret_cast<void*>(NULL);
}