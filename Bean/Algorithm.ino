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
    A_Stdev, // 2. Average Magnitude
    A_Time   // 3. Average Deviation
};

int32_t avg_norm[Postures];
int32_t dev_norm[Postures];
int32_t avg_sma[Postures];
int32_t dev_sma[Postures];
uint32_t times[Postures];

Sample Gavg;

// Not Implemented
uint8_t posture(int x, int y, int z)
{
    uint8_t pos = P_Unknown; // It's 0. Unknown for now...

    // Derivation
    uint32_t norm = magnitude(x, y, z);
    uint32_t sma = area(x, y, z);

    // Distances: SMA + Norm
    uint32_t distances[5];
    distances[0] = abs(norm - avg_norm[P_Lie]) + abs(sma - avg_sma[P_Lie]);
    distances[1] = abs(norm - avg_norm[P_Sit]) + abs(sma - avg_sma[P_Sit]);
    distances[2] = abs(norm - avg_norm[P_Stand]) + abs(sma - avg_sma[P_Stand]);
    distances[3] = abs(norm - avg_norm[P_Walk]) + abs(sma - avg_sma[P_Walk]);
    distances[4] = abs(norm - avg_norm[P_Run] + abs(sma - avg_sma[P_Run]));

    // Find minimum
    uint32_t min = (uint32_t)-1;
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
    uint32_t dot = (Gavg.x * gx) + (Gavg.y * gy) + (Gavg.z * gz);
    // |Gavg|
    uint32_t Mavg = magnitude(Gavg.x, Gavg.y, Gavg.z);
    // |Gt|
    uint32_t Mt = magnitude(gx, gy, gz);

    // |Gavg||Gt|(cos $theta) = Gavg*Gt
    float cosine = (float)dot;
    cosine = cosine / (Mavg * Mt); 

    return (float)acos(cosine);
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
    uint32_t elapsed = 200;

    measure(); // Read acceleration
    preproc(); // Filtering

    // LA -> Posture
    Sample *pacc = &filter.la;
    uint8_t p = posture(pacc->x, pacc->y, pacc->z);

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