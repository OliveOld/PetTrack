
class stop_watch
{
    unsigned int start;

  public:
    stop_watch()
    {
        start = millis();
    };

    // - Note
    //      Acquire elapsed time in millisecond
    // - See Also
    //      `millis()`
    unsigned int pick() const
    {
        //static   unsigned int max = -1;
        const unsigned int curr = millis();

        //// overflow
        //if (curr < start) {
        //}
        return curr - start; // elapsed
    }

    // - Note
    //      Acquire elapsed time and reset the stop watch
    //      Millisecond
    unsigned int reset()
    {
        unsigned int span = pick();
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

class Pets
{
  public:
    unsigned char Hz;
    unsigned int time[6]; // unsigned int per Postures

    Pets()
    {
        this->resetAll();
    }
    void resetAll()
    {
        Hz = 10;
        for (int i = 0; i < 6; ++i)
        {
            time[i] = 0;
        }
    }
    // - Note
    //      Report current state
    // - Return
    //      true  : successfully reported
    //      false : failed to report
    bool report()
    {
        Serial.print("Hz : ");
        Serial.println(Hz);
        for (int i = 0; i < 6; ++i)
        {
            Serial.print("Posture ");
            Serial.print(i);
            Serial.print(" : ");
            unsigned int t = time[i];
            Serial.println(t);
        }
        Serial.flush();
        return true;
    }

    // - Note
    //      Detect posture from Linear acceleration
    int posture(float ax, float ay, float az)
    {
        return 0;
    }

    // - Note
    //      Detect posture orientation from Gravity accleration
    int orientation(float gx, float gy, float gz)
    {
        return 0;
    }

    void slower(unsigned char minHz)
    {
        if (Hz > minHz)
            Hz -= 1;
    }
    void faster(unsigned char maxHz)
    {
        if (Hz < maxHz)
            Hz += 1;
    }

    // - Note
    //      Add specific posture's unsigned int
    void record(int pos, unsigned int d)
    {
        int idx = pos;
        time[idx] += d;
    }
};

LPF3A       filter;
stop_watch  timer;
Unit        ma;
Pets        pets;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
void blinkBatteryLevel();
void print(float x, float y, float z);

void setup()
{
    filter.alpha = 0.7;

    pets.resetAll();
    Bean.setAccelerationRange(4);
    Serial.begin(9600);
}

void loop()
{
    timer.reset();
    if (Bean.getConnectionState() == false)
    {
        blinkBatteryLevel();
        Bean.sleep(2000);
        return;
    }

    Bean.setLed(0, 0, 255);
    ma.x = Bean.getAccelerationX();
    ma.y = Bean.getAccelerationY();
    ma.z = Bean.getAccelerationZ();
    Bean.setLed(0, 0, 0);

    filter.separate(ma.x, ma.y, ma.z);
    Unit la = filter.la;
    Unit g = filter.g;

    int pos = pets.posture(la.x, la.y, la.z);
    pets.orientation(g.x, g.y, g.z);
    pets.record(pos, timer.pick());
    
    {
        print(ma.x, ma.y, ma.z);
        print(la.x, la.y, la.z);
        print(g.x, g.y, g.z);
        Serial.println();
        pets.report();
    }

    Bean.sleep(500);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void blinkBatteryLevel()
{
    uint16_t batteryPercentage = Bean.getBatteryLevel();
    if (batteryPercentage > 80)
    {
        Bean.setLed(0, 255, 0);
    }
    else if (batteryPercentage > 60)
    {
        Bean.setLed(255, 255, 0);
    }
    else if (batteryPercentage > 40)
    {
        Bean.setLed(255, 150, 0);
    }
    else if (batteryPercentage > 20)
    {
        Bean.setLed(255, 75, 0);
    }
    else
    {
        Bean.setLed(255, 0, 15);
    }
    Bean.sleep(100);
    Bean.setLed(0, 0, 0);
}

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
