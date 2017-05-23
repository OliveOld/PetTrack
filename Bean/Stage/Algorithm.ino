// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      Algorithm prototype
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// (Overlapped) Window size for SMA
#define WindowSize 4
// Low Pass Filter smoothing factor
#define ALPHA 759

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

uint32_t square(int16_t x)
{
    return x * x;
}

uint32_t magnitude(int16_t x, int16_t y, int16_t z)
{
    return square(x) + square(y) + square(z);
}

struct Sample
{
    int x, y, z;

    Sample()
    {
        x = y = z = 0;
    }
};

// - Note
//      3-Axis Low Pass Filter
struct LPF3A
{
    Sample la; // Linear Acceleration
    Sample g;  // Gravity Acceleration
  public:
    LPF3A()
    {
        la.x = la.y = la.z = 0;
        g.x = g.y = g.z = 0;
    }

    int factor(int x)
    {
        return (x * ALPHA) / 1000;
    }
    // - Note
    //      Receive Measured accceleration,
    //      then update Linear/Gravity acceleration.
    void separate(int mx, int my, int mz)
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
};

// - Note
//      Group of Sample
class Window
{
  public:
    int idx;
    Sample buf[WindowSize];

  private:
    void next()
    {
        idx = (idx + 1) % WindowSize;
    }

  public:
    void clear()
    {
        idx = 0;
        for (int i = 0; i < WindowSize; ++i)
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
    uint32_t SMA()
    {
        uint32_t sma = 0;
        for (int i = 0; i < WindowSize; ++i)
        {
            uint32_t inc = abs(buf[i].x) + abs(buf[i].y) + abs(buf[i].z);
            sma += inc;
        }
        return sma / WindowSize;
    }
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

Window win;

uint32_t area(int x, int y, int z)
{
    win.emplace(x, y, z);
    return win.SMA();
}

Sample ma;

// - Note
//      Measure current acceleration
void measure()
{
    AccelerationReading a = Bean.getAcceleration();
    ma.x = a.xAxis;
    ma.y = a.yAxis;
    ma.z = a.zAxis;
}

LPF3A filter;

// - Note
//      Pre-processing of MA
void preproc()
{
    // MA -> LA, G
    filter.separate(ma.x, ma.y, ma.z);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

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


int32_t  averages[Postures];
int32_t  stdevs[Postures];
uint32_t times[Postures];


// Not Implemented
uint8_t posture(int x, int y, int z)
{
    return P_Unknown;
}

// Not Implemented
bool isStatic(uint8_t pos)
{
    return false;
}

float orientation(int gx, int gy, int gz)
{
    return 0.0f;
}

void record(uint8_t pos, uint32_t dur)
{
    times[pos] += dur;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    // Update sensor in a 100ms period
    Bean.setAccelerometerPowerMode(0x5A);
}

void loop()
{
    measure(); // Read acceleration
    preproc(); // Filtering

    // LA -> Posture
    Sample* pacc = &filter.la;
    uint8_t p = posture(pacc->x, pacc->y, pacc->z);
 
    // if static posture, 
    if( isStatic(p) == true ){
        // calculate angle
        pacc = &filter.g;
        float angle = orientation(pacc->x, pacc->y, pacc->z);
        angle = abs(angle);

        if(angle > 40){
            p = P_LieSide;
        }
        else if (angle > 90){
            p = P_LieBack;
        }
    }

    // Record (Posture, Millisecond)
    record(p, 200);
}