
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
void blinkBatteryLevel();
void print(float x, float y, float z);
float SVM(float x, float y, float z);

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Simple timer for Arduino
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
    unsigned int pick() const
    {
        const unsigned int curr = millis();
        //// overflow
        //if (curr < start) {
        //}
        return curr - start; // elapsed
    }
    // - Note
    //      Acquire elapsed time and reset the stop watch
    unsigned int reset()
    {
        unsigned int span = pick();
        start = millis();
        return span;
    }
};

// - Note
//      Accelerometer value in floating point
struct Unit
{
    float x, y, z;
    Unit()
    {
        x = y = z = 0;
    }
};

// - Note
//      Group of Unit
class UnitGroup
{
  public:
    uint8_t idx;
    Unit buf[10];

  private:
    void next()
    {
        idx = (idx + 1) % 10;
    }

  public:
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

// State index definition
#define TRAIN 0
#define REPORT 1
#define MONITOR 2

uint8_t fdummy()
{
    blinkBatteryLevel();
    return MONITOR;
}

// - Note
//      State machine function table
struct State
{
    uint8_t (*pfInput)();
    uint8_t (*pfUpdate)();
    uint8_t (*pfOutput)();
    State()
    {
        // Prevent nullptr : Battery notification
        pfInput = pfUpdate = pfOutput = fdummy;
    }
};

// - Note
//      State machine with Function fable
class Machine
{
    uint8_t state;
    State ftbl[3];
  public:
    Machine()
    {
        state = MONITOR;
    }
    void input()
    {
        state = (*ftbl[state].pfInput)();
    }
    void update()
    {
        state = (*ftbl[state].pfUpdate)();
    }
    void output()
    {
        state = (*ftbl[state].pfOutput)();
    }
};

// - Note
//      Pets main module
class Pets : public Machine
{
  public:
    UnitGroup grp;
    unsigned char Hz;
    unsigned int time[6]; // unsigned int per Postures

  public:
    Pets()
    {
        this->clear();
    }

    void clear()
    {
        Hz = 2;
        for (int i = 0; i < 6; ++i)
        {
            time[i] = 0;
        }
        grp.clear();
    }

    // - Note
    //      Detect posture from Linear acceleration
    int posture(float ax, float ay, float az)
    {
        grp.emplace(ax, ay, az);
        if (grp.SMA() > 250)
        {
            return 2;
        }
        else if (SVM(ax, ay, az) > 100)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    // - Note
    //      Detect posture orientation from Gravity accleration
    int orientation(float gx, float gy, float gz)
    {
        return 0;
    }

    // - Note
    //      Add specific posture's unsigned int
    void record(int pos, unsigned int d)
    {
        int idx = pos;
        time[idx] += d;
    }

    // - Note
    //      Report current state
    // - Return
    //      true  : successfully reported
    //      false : failed to report
    bool report()
    {
        // Serial.print("Hz : ");
        // Serial.println(Hz);
        for (int i = 0; i < 6; ++i)
        {
            Serial.print(i);
            Serial.print(" : ");
            Serial.println(time[i]);
        }
        Serial.flush();
        delay(5);
        return true;
    }
};

stop_watch timer;
Unit ma;
LPF3A filter;
Pets pets;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    Serial.begin(9600);
    Bean.setAccelerationRange(4);
    pets.clear();
    filter.alpha = 0.7;
}

void onConnect();
void onNoConnect();

void loop()
{
    pets.input();
    pets.update();
    pets.output();

    /*
    timer.reset();
    if (Bean.getConnectionState() == true)
        // Connected. Make report
        onConnect();
    else
        // Not connected
        onNoConnect();
    */
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void onConnect()
{
    pets.report();
    pets.clear();
    blinkBatteryLevel();
    Bean.sleep(2000);
}

void onNoConnect()
{
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

    {
        // Sleep to save power
        unsigned int rate = (1000 / pets.Hz);
        unsigned int elapsed = timer.pick();
        if (elapsed < rate)
            Bean.sleep(rate - elapsed);
    }
}

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

float SVM(float x, float y, float z)
{
    return sqrt(x * x + y * y + z * z);
}
