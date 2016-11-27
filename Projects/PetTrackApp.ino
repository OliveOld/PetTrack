#include <Arduino.h>

#include "./Source/MyBlink.h"
//#include <CoreLib.hpp>

static MyBlink blinker = MyBlink{13};

void setup()
{
}

void loop()
{
    Serial.println("Damn");
    delay(1000);
    blinker.on();
    Serial.println("Damn!!!");

    delay(2000);
    blinker.off();

}

