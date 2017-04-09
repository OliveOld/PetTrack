
void print(float x, float y, float z);
float SVM(float x, float y, float z);

void print(float x, float y, float z)
{
    Serial.print("{");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(",");
    Serial.print(z);
    Serial.print("}");
}

float SVM(float x, float y, float z)
{
    return sqrt(x * x + y * y + z * z);
}

struct Unit
{
    float x, y, z;

    Unit()
    {
        x = y = z = 0;
    }
};

Unit ma;

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
        alpha = 0;
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

LPF3A filter;


class UnitGroup
{
  public:
    uint8_t idx;
    Unit buf[10];

  public:
  private:
    void next()
    {
        idx = (idx + 1) % 10;
    }

  public:
    UnitGroup(){
        this->clear();        
    }
    void clear()
    {
        idx = 0;
        for (int i = 0; i < 10; ++i)
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
    double SMA()
    {
        double sma = 0;
        for (int i = 0; i < 10; ++i)
        {
            sma = sma + abs(buf[i].x) + abs(buf[i].y) + abs(buf[i].z);
        }
        return sma;
    }
};

UnitGroup grp;

void setup(){
    grp.clear();
    filter.alpha = 0.8;
}

void loop(){
    ma.x = Bean.getAccelerationX();
    ma.y = Bean.getAccelerationY();
    ma.z = Bean.getAccelerationZ();

    filter.separate(ma.x, ma.y, ma.z);
    Unit la = filter.la;
    
    print(la.x, la.y, la.z);
    grp.emplace(la.x, la.y, la.z);
    Serial.println(grp.SMA());
    delay(5);
}