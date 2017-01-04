// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  File    : IMU.ino
//  Updated : 17/01/04
//  Author  : 
//      Team OliveOld
//      - Kim Min Ji
//      - Yoon Tae Yeon
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Note
//      Code for IMU sensor module
//
//  To Do
//      - Module initialization test
//      - Acquiring all kinds of values
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


#include "./alias.h"
#include "./module/led.h"
#include "./module/mpu9250.h"
#include "./module/bluetooth.h"

#include "./Sample.h"


// Global Constants
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

static constexpr auto baud = 9600;

static constexpr auto sec       = 1000;     // 1 second is 1000 ms
static constexpr auto hz        = 30;       // 30 samples in 1 sec
static constexpr auto looptime  = sec / hz; // milisec time for 1 loop

static constexpr auto buf_size  = hz;       // Sampling capacity.

static constexpr auto rxPin = 4;    // D4 : Arduino << Bluetooth
static constexpr auto txPin = 3;    // D3 : Arduino >> Bluetooth



namespace pets
{
    // Pets modules
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

    // Bluetooth Module
    olive::bluetooth bth{ txPin, rxPin };
    // MPU9250 Sensor module
    olive::mpu9250   mpu{};

    // Built-in LED
    led led13{ LED_BUILTIN };

}

using namespace pets;


static i8     ctr = 0;            // counter for index and loop
static Sample buffer[buf_size]{}; // Reserved buffer


// Function Signature
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup()    noexcept;
void loop()     noexcept;

Sample snapshot(const olive::mpu9250& _mpu) noexcept;




//  Function Definition
//      - setup
//      - loop
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void setup() noexcept
{

    mpu.init();         // MPU9250
    bth.begin(9600);    // Bluetooth
    Serial.begin(9600); // Serial Monitor
}


// - Pattern
//      Game Loop 
void loop() noexcept
{
    using namespace pets;

    // 1. Process Input
    //while (bth.available()) {
    //    // Read a message byte-by-byte
    //    char ch = static_cast<char>(bth.read());
    //    Serial.print(ch);
    //    //bth.print(ch);
    //    delay(5);
    //}

    // 2. Update Moduels & Variables
    //      Process input from sensor + buffering
    mpu.update();
    buffer[ctr] = snapshot(mpu);

    // 3. Render(Draw) 
    if (ctr++ == buf_size-1)
    {
        led13.on(); // Sending...

        for (auto i = 0; i < buf_size; ++i) {
            // Print via bluetooth module
            bth << buffer[i];
            // 1 record == 1 line
            bth.print("\n");
            // 9600bps 기준으로 delay 를 1ms 을 줘야 한다, 수신 문자열 끊김 방지
            delay(5);     
        }
        ctr = 0;

        led13.off(); // ... done!
    }

}

//  Function Definition
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

Sample snapshot(const olive::mpu9250& _mpu) noexcept
{
    pets::Sample smp{};
    smp.data[0] = _mpu.ax();    // acc-X
    smp.data[1] = _mpu.ay();    // acc-Y
    smp.data[2] = _mpu.az();    // acc-Z
    smp.data[3] = _mpu.gx();    // gyro-X
    smp.data[4] = _mpu.gy();    // gyro-Y
    smp.data[5] = _mpu.gz();    // gyro-Z

    return smp;
}
