// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  File    : BuildTest.ino
//  Author  : 
//      Team OliveOld
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Updated :
//
//  Note
//      Test code for unit, integration testing
//
//  To Do
//      - Module initialization test
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

#include "./alias.h"
#include "./utils/stop_watch.hpp"
#include "./module/bluetooth.h"
#include "./module/mpu9250.h"
#include "./module/classifier.h"

#include "./module/led.h"

static constexpr u32 sec      = 1000;   // 1 sec == 1000 ms
static constexpr u32 hz       = 1;      // frequency for second
// game loop limitation
static constexpr u32 looptime = (sec) / hz;
static constexpr u32 baud     = 9600;


namespace pets
{
    // sender, receiver pin
    static constexpr u8 rx = 2;
    static constexpr u8 tx = 3;   
    
    olive::bluetooth    bt{ tx, rx };
    olive::mpu9250      imu{};
    classifier          cl{};

    stop_watch  watch{};
}

struct Sample
{
    // ax, ay, az, gx, gy, gz, temp
    f32 measure[7]{};

    Sample() = default;
    Sample(Sample& _copy) noexcept
    {
        memcpy(measure, _copy.measure, sizeof(f32) * 7);
    }

    Sample& operator=(const Sample& _rhs) noexcept
    {
        memcpy(measure, _rhs.measure, sizeof(f32) * 7);
        return *this;
    }
    Sample& operator=(Sample&& _rhs) noexcept
    {
        memcpy(measure, _rhs.measure, sizeof(f32) * 7);
        return *this;
    }

    Sample(const olive::mpu9250& imu) {
        measure[0] = imu.ax();
        measure[1] = imu.ay();
        measure[2] = imu.az();
        measure[3] = imu.gx();
        measure[4] = imu.gy();
        measure[5] = imu.gz();
        measure[6] = imu.temp();
    }
};

static i16      ctr = 0;
static Sample   buffer[30]{};

// Build-in LED
static led      blink{ LED_BUILTIN };


// put your setup code here, to run once:
void setup() noexcept
{
    using namespace pets;
 
    Serial.begin(baud);
    bt.init(baud);
    
    imu.init();
    cl.init();
    //blink.init();

    watch.reset();
}

using namespace pets;

// put your main code here, to run repeatedly:
void loop() noexcept
{
    // 
    u32 elapsed = watch.reset();
    ctr = (ctr + 1) % 30;

    imu.update();
    buffer[ctr] = Sample{ imu };

    // 0 ~ 29
    if (ctr == 29) {
        blink.on();     // Sending data...

        static constexpr auto buflen = sizeof(Sample) * 30;
        // Send data...
        auto slen = bt.write((char*)buffer, buflen);

        blink.off();   // Sending done!
    }
    else {
        // Process data or partial send implementation
    }
}

