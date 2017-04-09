#include <Arduino.h>

#include <stdint.h>
#include <stdlib.h>

#include "Pets.h"
#include "./led.h"
#include "./Pets.h"

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

led led13{ LED_BUILTIN };
stop_watch timer{};
LPF3A filter{ 0.7 };
Pets pets{};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----



void setup() 
{
    Serial.begin(9600);
    led13.init();
    pets.reset();
}

void loop() 
{
    if (Serial) {
        led13.on();
        pets.report(Serial);
        led13.off();
    }
    // Read
    Unit ma;

    filter.separate(ma);
    Unit la = filter.la;
    Unit g = filter.g;

    Post pos = pets.posture(la);
    Ori ori = pets.orientation(g);


    auto elapsed = timer.pick();
    pets.record(pos, elapsed);

    // Faster / Slower
    (is_active(pos)) ? pets.faster(20) : pets.slower(5);

    pets.slower(1);
    timer.reset();

    // Save battery
    pets.sleep(1000);
}

