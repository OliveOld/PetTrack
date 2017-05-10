
void print(int x, int y, int z)
{
    Serial.print(x);
    Serial.print(' ');
    Serial.print(y);
    Serial.print(' ');
    Serial.print(z);
    delay(10);
    // Serial.flush();
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

struct iUnit
{
    int x, y, z;

    iUnit()
    {
        x = y = z = 0;
    }
};

struct fUnit
{
    float x, y, z;

    fUnit()
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

    fUnit la; // Linear Acceleration
    fUnit g;  // Gravity Acceleration
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

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

LPF3A filter;
Timer timer;

void setup()
{
    // Attenuation Ratio
    filter.alpha = 0.7585;

    timer.reset();
}
int count = 0;
void loop()
{
    timer.reset();
    // Measure acceleration
    Bean.setLed(0, 125, 0);
    iUnit ma;
    ma.x = Bean.getAccelerationX();
    ma.y = Bean.getAccelerationY();
    ma.z = Bean.getAccelerationZ();
    Bean.setLed(0, 0, 0);

    // Filtering
    filter.separate(ma.x, ma.y, ma.z);
    fUnit g = filter.g;   // Gravity Acceleration
    fUnit la = filter.la; // Linear Acceleration

    count++;
    // if(count == 10){
    print(ma.x,ma.y,ma.z);
    Serial.print('\n');
    delay(10);

    long span = 500 - (long)timer.pick(); 
    if(span > 0){
        Bean.sleep(span);
    }
    // Serial.println(timer.reset());
    // }
}