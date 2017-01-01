// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  File    : IMU.ino
//  Author  : 
//      Team OliveOld
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Updated :
//
//  Note
//      Code for IMU sensor module
//
//  To Do
//      - Module initialization test
//      - Acquiring all kinds of values
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

#include "./module/mpu9250.h"

namespace pets
{
    olive::mpu9250 imu{};
}

void setup() noexcept
{
    using namespace pets;

    imu.init();
}

void loop() noexcept
{
    using namespace pets;
    imu.update();
}