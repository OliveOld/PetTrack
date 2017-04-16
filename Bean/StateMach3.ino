// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      상태머신
//      - [x] Monitor 구현
//      - [x] Report 구현 (Prototype)
//      - [ ] Train 구현
//
//  Caution
//      LED로 현재 상태를 보고
//          - Monitor : Green
//          - Report  : Red
//          - Train   : Blue
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// State ID
#define TRAIN 0
#define REPORT 1
#define MONITOR 2

// - Note
//      Blink battery status
int CheckBattery();

// - Note
//      State machine function table
struct State
{
    int (*pfInput)();
    int (*pfUpdate)();
    int (*pfOutput)();
    State()
    {
        // Prevent nullptr : Battery notification
        pfInput = CheckBattery;
        pfUpdate = CheckBattery;
        pfOutput = CheckBattery;
    }
};

// - Note
//      State machine with Function fable
class Machine
{
  public:
    int     state;
    State   ftbl[3];

  public:
    Machine()
    {
        state = MONITOR; // Start from monitor
    }

    void input()
    {
        // Execute and transit
        state = (*ftbl[state].pfInput)();
    }
    void update()
    {
        // Execute and transit
        state = (*ftbl[state].pfUpdate)();
    }
    void output()
    {
        // Execute and transit
        state = (*ftbl[state].pfOutput)();
    }
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Simple timer for Arduino
class Timer
{
    unsigned int start;

  public:
    Timer()
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

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

struct Unit
{
    float x, y, z;

    Unit()
    {
        x = y = z = 0;
    }
};

float SMV(float x, float y, float z);

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
    float SMA()
    {
        float sma = 0;
        for (int i = 0; i < 10; ++i)
        {
            sma = sma + abs(buf[i].x) + abs(buf[i].y) + abs(buf[i].z);
        }
        return sma;
    }
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Pets main module
class Pets
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
        else if (SMV(ax, ay, az) > 100)
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
    void report()
    {
        // Serial.print("Hz : ");
        // Serial.println(Hz);
        for (int i = 0; i < 6; ++i)
        {
            Serial.print(i);
            Serial.print(" : ");
            Serial.println(time[i]);
            delay(5);
        }
        Serial.flush();
        delay(5);
    }
};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
void print(float x, float y, float z);

void BlinkBlue();
void BlinkRed();
void BlinkGreen();

int TrainInput();
int TrainUpdate();
int TrainOutput();

int ReportInput();
int ReportUpdate();
int ReportOutput();

int MonitorInput();
int MonitorUpdate();
int MonitorOutput();

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// UnitGroup group;    // Group of LA
Pets      pets;      // Pets Main Module
Machine   mach;      // State Machine
LPF3A     filter;    // Low Pass Filter
Unit      ma;       // Measured Acceleration
Timer     timer;     // Timer

// Global variable
float    smv, sma, tilt;    

void setup()
{
    pets.clear();

    // Variable init
    ma.x = ma.y = ma.z = 0;

    // Attenuation Ratio
    filter.alpha = 0.8;

    // Function Table
    mach.ftbl[TRAIN].pfInput = TrainInput;
    mach.ftbl[TRAIN].pfUpdate = TrainUpdate;
    mach.ftbl[TRAIN].pfOutput = TrainOutput;

    mach.ftbl[REPORT].pfInput = ReportInput;
    mach.ftbl[REPORT].pfUpdate = ReportUpdate;
    mach.ftbl[REPORT].pfOutput = ReportOutput;

    mach.ftbl[MONITOR].pfInput = MonitorInput;
    mach.ftbl[MONITOR].pfUpdate = MonitorUpdate;
    mach.ftbl[MONITOR].pfOutput = MonitorOutput;
}

void loop()
{
    timer.reset();
    mach.input();
    mach.update();
    mach.output();

    Serial.println(timer.pick());
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Blink Blue LED
void BlinkBlue()
{
    Bean.setLedBlue(255);
    Bean.sleep(50);
    Bean.setLed(0, 0, 0);
}
// - Note
//      Blink Red LED
void BlinkRed()
{
    Bean.setLedRed(255);
    Bean.sleep(50);
    Bean.setLed(0, 0, 0);
}
// - Note
//      Blink Green LED
void BlinkGreen()
{
    Bean.setLedGreen(255);
    Bean.sleep(50);
    Bean.setLed(0, 0, 0);
}

// - Note
//      Train : Input
int TrainInput()
{
    Serial.println("Train Input");
    BlinkBlue();
    return TRAIN; // Next state
}
// - Note
//      Train : Update
int TrainUpdate()
{
    Serial.println("Train Update");
    BlinkBlue();
    return TRAIN;
}
// - Note
//      Train : Output
int TrainOutput()
{
    Serial.println("Train Output");
    BlinkBlue();
    return MONITOR; // Train -> Monitor
}

// - Note
//      Report : Input
//      Befor report, check bluetooth connection status
int ReportInput()
{
    // Update variables anyway...
    MonitorInput();

    // connected     ->  Go to Report:Update
    // not connected ->  Go to Monitor:Update
    if(Bean.getConnectionState()==true){
        // No message checking. Just report
        return REPORT;
    }
    else{
        // Not connected. change to monitor
        return MONITOR;
    }
}
// - Note
//      Report : Update
//      Blink Green-Red
int ReportUpdate()
{
    MonitorUpdate();
    BlinkRed();
    return REPORT;
}
// - Note
//      Report : Output
int ReportOutput()
{
    Unit la = filter.la;
    Unit ga = filter.g;
    //print(ma.x, ma.y, ma.z);
    print(la.x, la.y, la.z);
    //print(ga.x, ga.y, ga.z);
    print(smv, sma, tilt);
    Serial.println();

    // pets.report();
    Bean.sleep(10);
    return REPORT;
}

// - Note
//      Monitor : Input
int MonitorInput()
{
    // Snapshot accelerometer
    ma.x = Bean.getAccelerationX();
    ma.y = Bean.getAccelerationY();
    ma.z = Bean.getAccelerationZ();

    // connected     ->  Go to Report:Update
    // not connected ->  Go to Monitor:Update
    return (Bean.getConnectionState() == true)? REPORT: MONITOR;
}
// - Note
//      Monitor : Update
//      Blink Green
int MonitorUpdate()
{
    filter.separate(ma.x, ma.y, ma.z);
    Unit la = filter.la;
    Unit ga = filter.g;

    // implicit emplace
    // pets.posture(la.x, la.y, la.z);
    // pets.orientation(ga.x,ga.y,ga.z);

    smv = SMV(la.x, la.y, la.z);
    pets.grp.emplace(la.x, la.y, la.z);
    sma = pets.grp.SMA();
    tilt = 0;

    BlinkGreen();
    return MONITOR;
}
// - Note
//      Monitor : Output
int MonitorOutput()
{    
    Unit la = filter.la;
    Unit ga = filter.g;

    pets.record(0, timer.pick());
    return MONITOR;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

int CheckBattery()
{
    int batteryPercentage = Bean.getBatteryLevel();
    if (batteryPercentage > 80)
        Bean.setLed(0, 255, 0);
    else if (batteryPercentage > 60)
        Bean.setLed(255, 255, 0);
    else if (batteryPercentage > 40)
        Bean.setLed(255, 150, 0);
    else if (batteryPercentage > 20)
        Bean.setLed(255, 75, 0);
    else
        Bean.setLed(255, 0, 15);

    Bean.sleep(100);
    Bean.setLed(0, 0, 0);
    return MONITOR; // Next state
}

void print(float x, float y, float z)
{
    Serial.print('{');    Serial.print(x);
    Serial.print(',');    Serial.print(y);
    Serial.print(',');    Serial.print(z);
    Serial.print('}');
    delay(5);
}

float square(float x)
{
    return x*x;
}
float SMV(float x, float y, float z)
{
    return sqrt(square(x) + square(y) + square(z));
}
