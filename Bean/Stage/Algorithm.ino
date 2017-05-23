
// (Overlapped) Window size for SMA
#define WindowSize 4

uint32_t Square(int16_t x)
{
    return x*x;
}

uint32_t Norm(int16_t x, int16_t y, int16_t z)
{
    return Square(x) + Square(y) + Square(z);
}

struct Unit
{
    int x, y, z;

    Unit(){
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

    int factor(int x)
    {
        return (x * alpha) / 1000;
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
//      Group of Unit
class Window
{
  public:
    int     idx;
    Unit    buf[WindowSize];

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
            sma = sma + abs(buf[i].x) + abs(buf[i].y) + abs(buf[i].z);
        }
        return sma / WindowSize;
    }
};

Window win;

uint32_t SMA(int x, int y, int z)
{
    win.emplace(x,y,z);
    return win.SMA();
}

Unit ma;
LPF3A filter;

// - Note
//      Measure current acceleration
void Measure()
{
    AccelerationReading a = Bean.getAcceleration();
    ma.x = a.xAxis;
    ma.y = a.yAxis;
    ma.z = a.zAxis;
}

// - Note
//      Pre-processing of MA
void Preproc()
{
    // MA -> LA, G
    filter.separate(ma.x, ma.y, ma.z);
}


void setup()
{
    // Update sensor in a 100ms period
    Bean.setAccelerometerPowerMode(0x5A);
    filter.alpha = 759; // 0.759
}

void loop()
{
    Measure();    // Read acceleration
    Preproc();    // Filtering

    // Report
}