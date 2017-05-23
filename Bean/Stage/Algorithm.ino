
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


// - Note
//      Pre-processing of MA
void Preproc()
{
    // Filtering
    filter.separate(ma.x, ma.y, ma.z);
    Unit g = filter.g;   // Gravity Acceleration
    Unit la = filter.la; // Linear Acceleration
}

