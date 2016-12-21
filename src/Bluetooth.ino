// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====
//
//  File    : Bluetoot.ino
//  Author  : 
//      Team OliveOld
//      - Park Dong Ha (luncliff@gmail.com)
//
//  Updated :
//
//  Note
//      Code for Bluetooth module
//
//  To Do
//      - Module initialization test
//      - I/O from remote device
//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
#include "./module/bluetooth.h"

namespace pets
{
    bluetooth bt{};
}

void setup() noexcept
{
    using namespace pets;

    bt.init();
}

void loop() noexcept
{
    using namespace pets;

}