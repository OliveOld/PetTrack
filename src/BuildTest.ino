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

#include "SoftwareSerial.h"

#include "./alias.h"
#include "./utils/stop_watch.hpp"
#include "./module/bluetooth.h"
#include "./module/imu_sensor.h"
#include "./module/classifier.h"

#include "./module/led.h"

static constexpr u32 sec      = 1000;   // 1 sec == 1000 ms
static constexpr u32 hz       = 1;      // frequency for second
// game loop limitation
static constexpr u32 looptime = (2 * sec) / hz;
static constexpr u32 baud     = 9600;


namespace pets
{
    // sender, receiver pin
    static constexpr u8 rx = 2;
    static constexpr u8 tx = 3;   
    
    bluetooth   bt{ tx, rx };
    imu_sensor  imu{};
    classifier  cl{};

    stop_watch  watch{};
}

// Build-in LED
led blink{ LED_BUILTIN };


// put your setup code here, to run once:
void setup() noexcept
{
    using namespace pets;
 
    Serial.begin(baud);
    bt.init(baud);
    
    imu.init();
    cl.init();
    blink.init();

    watch.reset();
}


// put your main code here, to run repeatedly:
void loop() noexcept
{
    using namespace pets;
    u32 elapsed = watch.reset();



    //Serial.flush();
    blink.change();


    //char buf[20]{};
    //itoa(elapsed, buf, 16);
    delay((looptime - elapsed) / 2);

}

