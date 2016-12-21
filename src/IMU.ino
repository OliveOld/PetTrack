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
#include "./module/imu_sensor.h"

namespace pets
{
    imu_sensor imu9550{};
}

void setup() noexcept
{
    using namespace pets;

    imu9550.init();
}

void loop() noexcept
{
    using namespace pets;

}