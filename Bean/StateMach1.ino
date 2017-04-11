// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      간단한 상태머신 예제 코드
//      함수 포인터 기반.
//
//      Train -> Monitor -> Report -> Train
//
//  Caution
//      1. LED로 현재 상태를 보고
//          - Monitor : Green
//          - Report  : Red
//          - Train   : Blue
//      2. Serial로 현재 프로세스를 보고
//          - Input
//          - Update
//          - Output
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// State ID
#define TRAIN 0
#define REPORT 1
#define MONITOR 2

// - Note
//      Blink battery status
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

Machine mach;

void setup()
{
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
    mach.input();
    mach.update();
    mach.output();

    Bean.sleep(500);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// - Note
//      Blink Blue LED
void BlinkBlue()
{
    Bean.setLedBlue(255);
    Bean.sleep(100);
    Bean.setLed(0, 0, 0);
}
// - Note
//      Blink Red LED
void BlinkRed()
{
    Bean.setLedRed(255);
    Bean.sleep(100);
    Bean.setLed(0, 0, 0);
}
// - Note
//      Blink Green LED
void BlinkGreen()
{
    Bean.setLedGreen(255);
    Bean.sleep(100);
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
int ReportInput()
{
    Serial.println("Report Input");
    BlinkRed();
    return REPORT;
}
// - Note
//      Report : Update
int ReportUpdate()
{
    Serial.println("Report Update");
    BlinkRed();
    return REPORT;
}
// - Note
//      Report : Output
int ReportOutput()
{
    Serial.println("Report Output");
    BlinkRed();
    return TRAIN; // Report -> Train
}

// - Note
//      Monitor : Input
int MonitorInput()
{
    Serial.println("Monitor Input");
    BlinkGreen();
    return MONITOR;
}
// - Note
//      Monitor : Update
int MonitorUpdate()
{
    Serial.println("Monitor Update");
    BlinkGreen();
    return MONITOR;
}
// - Note
//      Monitor : Output
int MonitorOutput()
{
    Serial.println("Monitor Output");
    BlinkGreen();
    return REPORT; // Monitor -> Report
}
