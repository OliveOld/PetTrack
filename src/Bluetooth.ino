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
    olive::bluetooth bt{4,5};
}

void setup() noexcept
{
    using namespace pets;
    auto baud = u32{ 9600 };
    bt.init(baud);
}

void loop() noexcept
{
    using namespace pets;

}