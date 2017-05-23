// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//
//  Caution
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Type / Utility
// ---- ---- ---- ---- ----

typedef void *PTR;
typedef PTR (*State)(void);

uint32_t Square(int16_t x);
uint32_t Norm(int16_t x, int16_t y, int16_t z);

// - Note
//      Simple timer for Arduino
class Timer;

// - Note
//      Acceleration Sample
struct Unit;

// - Note
//      3-Axis Low Pass Filter
class LPF3A;

const int GSize = 4;

// - Note
//      Group of Unit
class UnitGroup;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----



// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Function Declarations
// ---- ---- ---- ---- ----

void Measure();
void Preproc();

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Core Variables & Functions
// ---- ---- ---- ---- ----

State state;

LPF3A      filter;
Timer      timer;
Unit       ma;
UnitGroup  grp;
uint32_t   time[POSTURES]; // uint32_t per Postures

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()
{
    // Clear variable
    // Filter's smoothing factor
    // Initial state function

    // Acceleration power mode: 100ms period
    Bean.setAccelerometerPowerMode(0x5A);

}

void loop()
{
    // if(state != NULL){
    //     state = (*state)();
    // }
    Bean.setLed(0,100,100);
    Bean.sleep(200);
    Bean.setLed(0, 0, 0);        
    Bean.sleep(800);
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Function Implementations
// ---- ---- ---- ---- ----

// - Note
//      Measure current acceleration
void Measure()
{
    AccelerationReading a = Bean.getAcceleration();
    ma.x = a.x;
    ma.y = a.y;
    ma.z = a.z;
}

// - Note
//      Pre-processing of MA
void Preproc()
{
    // Filtering
    filter.separate(ma.x, ma.y, ma.z);
    Unit g = filter.g;   // Gravity Acceleration
    Unit la = filter.la; // Linear Acceleration
}
