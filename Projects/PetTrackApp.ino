#include <Arduino.h>

#include "./Source/MyBlink.h"
//#include "./Library/CoreLib/src/TestLib.h"


static MyBlink blinker = MyBlink{13};

void setup()
{
}

void loop()
{
    delay(1000);
    blinker.on();
    delay(2000);
    blinker.off();

}

