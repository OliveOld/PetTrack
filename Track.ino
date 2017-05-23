// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  Author
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      File for Release Candidate
//  Caution
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Type / Utility
// ---- ---- ---- ---- ----

typedef void* (*State)(void);

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
class Window;

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

uint32_t   time[Postures]; // uint32_t per Postures

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
    filter.alpha = 759; // 0.759

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
    // ...
}

// - Note
//      Pre-processing of MA
void Preproc()
{
    // ...
}
