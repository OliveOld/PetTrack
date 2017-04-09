#include <Arduino.h>

#include <stdint.h>
#include <stdlib.h>

#include "./led.h"
#include "./stop_watch.h"

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

led led13{ LED_BUILTIN };
stop_watch sw{};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----



void setup() 
{
    Serial.begin(9600);
    led13.init();
    sw.reset();
}

void loop() 
{
    Serial.println(sw.pick());
    led13.change();
    delay(1000);
}

