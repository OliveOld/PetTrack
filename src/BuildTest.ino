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

#include "./utils/stop_watch.hpp"

#include "./module/bluetooth.h"
#include "./module/imu_sensor.h"
#include "./module/classifier.h"

namespace pets
{
    bluetooth   bt{};
    imu_sensor  imu{};
    classifier  cl{};
}


// put your setup code here, to run once:
void setup() noexcept
{
    using namespace pets;
    
    bt.init();
    imu.init();
    cl.init();
}

// put your main code here, to run repeatedly:
void loop() noexcept
{
    using namespace pets;

}

